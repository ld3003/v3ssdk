#/bin/bash 

physicalNumber=0
coreNumber=0
logicalNumber=0
HTNumber=0

logicalNumber=$(grep "processor" /proc/cpuinfo|sort -u|wc -l)
physicalNumber=$(grep "physical id" /proc/cpuinfo|sort -u|wc -l)
coreNumber=$(grep "cpu cores" /proc/cpuinfo|uniq|awk -F':' '{print $2}'|xargs)
HTNumber=$((logicalNumber / (physicalNumber * coreNumber)))

echo "****** CPU Information ******"
echo "Logical CPU Number : ${logicalNumber}"
echo "Physical CPU Number : ${physicalNumber}"
echo "CPU Core Number : ${coreNumber}"
echo "HT Number : ${HTNumber}"

echo "*****************************"

cd /v3ssdk

LD_LIBRARY_PATH=
TOP_DIR=`pwd`

PATH=$TOP_DIR/buildroot/out/host/bin/:$PATH


CROSS_COMPILE=$TOP_DIR/tools/external-toolchain/bin/arm-linux-gnueabi-    
UBOOT_DIR=$TOP_DIR/u-boot-2011.09
KERNEL_DIR=$TOP_DIR/linux-3.4
BUILDROOT_DIR=$TOP_DIR/buildroot
BUILDROOT_OUT_DIR=$TOP_DIR/buildroot/out
ROOTFS_DIR=$TOP_DIR/rootfs
HOSTTOOLS_DIR=$TOP_DIR/tools/bin
PACKTOOLS_DIR=$TOP_DIR/tools/pack
ROOTFS_OVERRIDE_DIR=$TOP_DIR/prebuilt/rootfs-override
APP_DIR=$TOP_DIR/app
BR_CROSS_COMPILE=$BUILDROOT_OUT_DIR/host/usr/bin/arm-buildroot-linux-gnueabihf-



copy_file_list=(
    $ROOTFS_OVERRIDE_DIR/etc/inittab:$ROOTFS_DIR/etc/inittab
    $ROOTFS_OVERRIDE_DIR/etc/profile:$ROOTFS_DIR/etc/profile
    $ROOTFS_OVERRIDE_DIR/etc/wpa_supplicant.conf:$ROOTFS_DIR/etc/wpa_supplicant.conf
    $ROOTFS_OVERRIDE_DIR/etc/hostapd.conf:$ROOTFS_DIR/etc/hostapd.conf
    $ROOTFS_OVERRIDE_DIR/etc/udhcpd.conf:$ROOTFS_DIR/etc/udhcpd.conf
    $ROOTFS_OVERRIDE_DIR/etc/mdev.conf:$ROOTFS_DIR/etc/mdev.conf
    $ROOTFS_OVERRIDE_DIR/etc/init.d/S50sshd:$ROOTFS_DIR/etc/init.d/
    $ROOTFS_OVERRIDE_DIR/etc/init.d/S90camera:$ROOTFS_DIR/etc/init.d/
    $ROOTFS_OVERRIDE_DIR/etc/init.d/S90wifiap:$ROOTFS_DIR/etc/init.d/_S90wifiap
    $ROOTFS_OVERRIDE_DIR/etc/init.d/S90wifista:$ROOTFS_DIR/etc/init.d/_S90wifista
    $ROOTFS_OVERRIDE_DIR/etc/init.d/S99appstart:$ROOTFS_DIR/etc/init.d/
    $ROOTFS_OVERRIDE_DIR/etc/ssh/sshd_config:$ROOTFS_DIR/etc/ssh/
    $ROOTFS_OVERRIDE_DIR/root/*.sh:$ROOTFS_DIR/root/
    $ROOTFS_OVERRIDE_DIR/root/demo-h264enc:$ROOTFS_DIR/usr/bin/
    $APP_DIR/demo-camera/demo-camera:$ROOTFS_DIR/usr/bin/
    $APP_DIR/demo-qt/digitalclock:$ROOTFS_DIR/root/
    $PREBUILT_DIR/libs/*:$ROOTFS_DIR/lib/
)

function copy_file_to_rootfs()
{
  for line in ${copy_file_list[@]} ; do
	  srcfile=`echo $line | awk -F: '{print $1}'`
	  dstfile=`echo $line | awk -F: '{print $2}'`
	  cp -drf $srcfile $dstfile 2>/dev/null
  done
}

function build_uboot()
{
	cd $UBOOT_DIR
	
	make -j${logicalNumber} CROSS_COMPILE=${CROSS_COMPILE} sun8iw8p1_spinand_emmc && \
	make -j${logicalNumber} CROSS_COMPILE=${CROSS_COMPILE} fes && \
  	make -j${logicalNumber} CROSS_COMPILE=${CROSS_COMPILE} boot0
  [ $? -ne 0 ] && echo "build u-boot Failed"
}

function clean_uboot()
{
	cd $UBOOT_DIR
	make CROSS_COMPILE=${CROSS_COMPILE} distclean
}

function build_kernel()
{
	cd $KERNEL_DIR
	
	#make ARCH=arm -j4 CROSS_COMPILE=${CROSS_COMPILE} mangopi_defconfig && \
	make ARCH=arm -j${logicalNumber} CROSS_COMPILE=${CROSS_COMPILE} && \
	make ARCH=arm -j${logicalNumber} CROSS_COMPILE=${CROSS_COMPILE} uImage modules && \
	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} INSTALL_MOD_PATH=${ROOTFS_DIR} modules_install
  [ $? -ne 0 ] && echo "build kernel Failed"


  	cp -v ./arch/arm/boot/uImage ../tools/pack/chips/sun8iw8p1/boot-resource/boot-resource/
}

function clean_kernel()
{
	cd $KERNEL_DIR
	make ARCH=arm -j4 CROSS_COMPILE=${CROSS_COMPILE} distclean
}

function build_buildroot()
{
	echo "Build buildroot"

	cd $BUILDROOT_DIR
	mkdir -p $BUILDROOT_OUT_DIR


	#cp $BUILDROOT_OUT_DIR/.config configs/mangopi_defconfig
	#cp configs/mangopi_defconfig $BUILDROOT_OUT_DIR/.config

	make O=$BUILDROOT_OUT_DIR oldconfig
	
	make O=$BUILDROOT_OUT_DIR 

	rm -r $ROOTFS_DIR
	mkdir -p $ROOTFS_DIR
	
	tar xvf $BUILDROOT_OUT_DIR/images/rootfs.tar.bz2 -C $ROOTFS_DIR
}

function clean_buildroot()
{
	cd $BUILDROOT_DIR
	
	make O=$BUILDROOT_OUT_DIR clean
 
	rm -r $TOP_DIR/rootfs
}

function pack()
{

	copy_file_to_rootfs
	
	$HOSTTOOLS_DIR/make_ext4fs -s -l 200M /tmp/rootfs-ext4.img $ROOTFS_DIR
	
	cd $PACKTOOLS_DIR
	
	./pack -c sun8iw8p1 -p camdroid -b tiger-spinand-standard -e spinand
	
}



function build_library()
{
	
	cd $APP_DIR/ncnn/
	mkdir build
	cd build
	cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/arm-buildroot-gnueabihf.toolchain.cmake ..
	make -j${logicalNumber}

	cd $APP_DIR/libfacedetection/
	mkdir build
	cd build
	cmake ../
	make -j${logicalNumber}

}

function build_demos()
{

	cp -v $APP_DIR/sdcard/*  ./tools/pack/chips/sun8iw8p1/boot-resource/boot-resource/
	
	cd $APP_DIR/demo-camera/
	echo "*****************************************"
	pwd
	make CROSS_COMPILE=$BR_CROSS_COMPILE -j ${logicalNumber} demo-camera
	

	cd $APP_DIR/gpio/
	echo "*****************************************"
	pwd
	make CROSS_COMPILE=$BR_CROSS_COMPILE -j ${logicalNumber}

	#cd $APP_DIR/demo-codec/
	#echo "*****************************************"
	#pwd
	#make CROSS_COMPILE=$BR_CROSS_COMPILE -j ${logicalNumber}

	cd $APP_DIR/demo-qt
	echo "*****************************************"
	pwd
  	$TOP_DIR/buildroot/out/host/bin/qmake
	make -j${logicalNumber}
	cp -v demo-qt  ../../tools/pack/chips/sun8iw8p1/boot-resource/boot-resource/



	cd $APP_DIR/opencv
	echo "*****************************************"
	pwd
	make CROSS_COMPILE=$BR_CROSS_COMPILE -j ${logicalNumber} test

	cd $APP_DIR/curl
	echo "*****************************************"
	pwd
	make CROSS_COMPILE=$BR_CROSS_COMPILE -j ${logicalNumber} test


	cd $APP_DIR/cam4g
	echo "*****************************************"
	pwd
	make CROSS_COMPILE=$BR_CROSS_COMPILE -j ${logicalNumber}

	cd $APP_DIR/opencv2fb
	echo "*****************************************"
	pwd
	make CROSS_COMPILE=$BR_CROSS_COMPILE -j ${logicalNumber}


	cd $APP_DIR/facedetectapp
	echo "*****************************************"
	pwd
	make CROSS_COMPILE=$BR_CROSS_COMPILE -j ${logicalNumber}



	cd $APP_DIR/serialport_linux/src
	echo "*****************************************"
	pwd
	make CROSS_COMPILE=$BR_CROSS_COMPILE -j ${logicalNumber} 

	cd $APP_DIR/facedetapp
	echo "*****************************************"
	pwd
  	$TOP_DIR/buildroot/out/host/bin/qmake
	make -j${logicalNumber}

	cd $APP_DIR/qrcode
	echo "*****************************************"
	pwd
  	$TOP_DIR/buildroot/out/host/bin/qmake
	make -j${logicalNumber}

	cd $APP_DIR/facenet/
	echo "*****************************************"
	pwd
	mkdir build
	cd build
	cmake ../
	make -j${logicalNumber}
}

function clean_demos()
{
	cd $TOP_DIR
	source env-qt
	
	cd $APP_DIR/demo-camera/
	make clean
	
	cd $APP_DIR/demo-qt
	./run_qmake.sh
	make clean

}

function dockerbuild()
{
	cd /v3ssdk
	build_buildroot
	#build_uboot
	build_kernel
	build_library
	build_demos
	pack

}

if [ $# -eq 0 ] ; then
	build_buildroot
	#build_uboot
	build_kernel
	build_library
	build_demos
	pack
else
	case $1 in
	clean)
		clean_uboot
		clean_kernel
		clean_buildroot
		clean_demos
		;;
	uboot)
		build_uboot
		;;
	kernel)
		build_kernel
		;;
	buildroot)
		build_buildroot
		;;
	lib)
		build_library
		;;
	app)
		build_demos
		;;
	pack)
		pack
		;;
	docker)
		dockerbuild
		;;
	esac
fi
