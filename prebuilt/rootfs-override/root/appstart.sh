#!/bin/sh

# add your app start script here

echo "nameserver 114.114.114.114" > /etc/resolv.conf
mkdir -p /root/sdcard/
mount /dev/mmcblk0p2 /root/sdcard/
sh /root/sdcard/run.sh

echo "****************************************************"
echo "*                                                  *"
echo "*      E-Mail:83114367@qq.com  Wechat:mi158138     *"
echo "*                                                  *"
echo "****************************************************"

exit 0
