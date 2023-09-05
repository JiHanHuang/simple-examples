#!/bin/bash

#抓取输出数据的包

gcc main.c -g

echo ">>>>>>>>>>>>"

PWD='Hj747777'


echo "$PWD" | sudo -S tcpdump -i lo udp port 2123 -w gtp-ut.pcap &
sleep 1
echo "start tcpdump"

./a.out

sleep 1
echo "$PWD" | sudo -S killall tcpdump
sleep 1
rm /mnt/hgfs/vm_share/gtp-ut.pcap -f
if [ $? -eq 0 ];then
    mv ./gtp-ut.pcap /mnt/hgfs/vm_share/
    echo "copy file success"
fi

echo "<<<<<<<<<<<<"
