#! /bin/sh

killall -9 gateway-linux

if [ `ps -fC gateway-linux | wc -l` -gt 1 ]
then echo "gateway-linux: 模块停止失败"
else echo "gateway-linux: 模块已停止"
fi
