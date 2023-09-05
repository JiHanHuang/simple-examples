#!/bin/bash

IPT='iptables'

IP_LIST=""
PORT_LIST=""

IPT_FLAGE='-A'

runopt (){
    while getopts "ahi:p:c" opts
    do
        case $opts in
            "a")
                echo "Iptables defense"
                ;;
            "h")
                echo "
Usage: >_< [-a|h] [-i ip]|[-p port]|[-c]
  
Emergency help:  
 -a                   Show message  
 -h                   Show help
 -i ip		      Set ip white list in iptables.[1.1.1.1|1.1.1.1,1.1.1.2|1.1.1.1/24]
 -p port	      Set port white list in iptables.[80|80,90|2000:3000]
 -c				  Clean iptables rules those set by this script.
"
                exit 0
                ;;
            "i")
                IP_LIST=$OPTARG
                ;;
            "p")
                PORT_LIST=$OPTARG
                ;;
			"c")
                IPT_FLAGE='-D'
                ;;
            "?")
                echo "Invalid option -$OPTARG"
                exit 1
                ;;
            ":")
                echo "No argument value for option -$OPTARG"
                exit 1
                ;;
        esac
    done
    return $OPTIND
}

runopt "$@"

if [ "${IPT_FLAGE}" == "-A" ]; then
	read -p "Make sure your sshd listen port is 22? [y/n]" check
	if [ "X${check}" != "Xy" ];then
		echo "Please run [iptables -A INPUT -p tcp --dport <your_ssh_port> -j ACCEPT] before run this script."
		exit 0
	fi
fi

if [ "X${IP_LIST}" != "X" ];then
	echo ${IP_LIST} | grep ',' > /dev/null
	if [ $? -eq 0 ];then
		ips=`echo ${IP_LIST} | sed 's/,/ /g'`
		echo $ips
		for ip in ${ips};do
			$IPT $IPT_FLAGE INPUT -s ${ip} -j ACCEPT
			if [ $? -ne 0 ];then
				exit 1
			fi
		done
	else
		$IPT $IPT_FLAGE INPUT -s ${IP_LIST} -j ACCEPT
		if [ $? -ne 0 ];then
			exit 1
		fi
	fi
fi
if [ "X${PORT_LIST}" != "X" ];then
	echo ${PORT_LIST} | grep ',' > /dev/null
	if [ $? -eq 0 ];then
		$IPT $IPT_FLAGE INPUT -p tcp -m multiport --dports ${PORT_LIST} -j ACCEPT
		if [ $? -ne 0 ];then
			exit 1
		fi
	else
		$IPT $IPT_FLAGE INPUT -p tcp --dport ${PORT_LIST} -j ACCEPT
		if [ $? -ne 0 ];then
			exit 1
		fi
	fi
fi

if [ "${IPT_FLAGE}" == "-D" ]; then
	$IPT $IPT_FLAGE INPUT -j DROP
	$IPT $IPT_FLAGE INPUT -p tcp -j REJECT --reject-with tcp-reset
	$IPT $IPT_FLAGE INPUT -p udp -j REJECT --reject-with icmp-port-unreachable
fi

$IPT $IPT_FLAGE INPUT -p tcp --dport 22 -j ACCEPT
$IPT $IPT_FLAGE INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
$IPT $IPT_FLAGE INPUT -i lo -j ACCEPT

if [ "${IPT_FLAGE}" == "-A" ]; then
	$IPT $IPT_FLAGE INPUT -p tcp -j REJECT --reject-with tcp-reset
	$IPT $IPT_FLAGE INPUT -p udp -j REJECT --reject-with icmp-port-unreachable
	$IPT $IPT_FLAGE INPUT -j DROP
fi

if [ "${IPT_FLAGE}" == "-A" ]; then
	echo "Set iptables finish"
else
	echo "Unset iptables finish"
fi
