#! /bin/sh
nohup ./gateway-linux ../etc/sms_platform.conf &
if [ `ps -fC gateway-linux | wc -l` -gt 1 ]
then echo "gateway-linux: 模块已启动"
else echo "gateway-linux: 模块启动失败"
fi
