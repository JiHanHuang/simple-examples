#!/bin/bash
PWD='Hj747777'

./build.sh

echo "$PWD" | sudo -S tcpdump -i lo tcp port 4433 -w tls_test.pcap &
sleep 1
echo "start tcpdump"

# ./server > s.log  2>&1 &
# sleep 1
# ./client > c.log 2>&1

# echo "server>>>"
# cat s.log
# echo "client>>>"
# cat c.log

./server &
sleep 1
./client 

sleep 2
echo "$PWD" | sudo -S killall tcpdump
killall server

exit

sleep 1
rm /mnt/hgfs/vm_share/tls_test.pcap -f
if [ $? -eq 0 ];then
    mv ./tls_test.pcap /mnt/hgfs/vm_share/
    echo "copy file success"
fi
