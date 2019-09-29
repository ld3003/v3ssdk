#!/bin/sh

# add your app start script here

echo 1 > /sys/class/udc/sunxi_usb_udc/device/otg_role

echo "nameserver 114.114.114.114" > /etc/resolv.conf
cam4g &


echo "****************************************************"
echo "*                                                  *"
echo "*      E-Mail:83114367@qq.com  Wechat:mi158138     *"
echo "*                                                  *"
echo "****************************************************"

exit 0
