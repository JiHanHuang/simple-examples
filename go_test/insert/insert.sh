#!bin/bash
/usr/sbin/iptables -A IPTABLES_TEST_INPUT -p tcp -s 10.92.86.172 --dport 60000 -j DROP
