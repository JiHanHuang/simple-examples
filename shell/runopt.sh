#!/bin/bash

#一个用于shell的命令行脚本示例
#可根据需求增加和删除函数以及命令

default_action(){
    echo "run default"
}

action_do(){
	echo "action do"
}

action_do_arg(){
    echo "action do:$@"
}

multi_args_do(){
    echo "-c:$C_ARG  -b:$D_ARG"
}

show_help(){
                echo "
Usage: >_< [-h|a] [-d|c|b arg]
  
Emergency help:  
 -h              Show this message  
 -a              del lcov data
 -d arg          run lcov
 -c arg          add arg for multi_args_do
 -b arg          add arg for multi_args_do
"
}

runopt (){
    while getopts "had:c:b:" opts
    do
        case $opts in
            "h")
                show_help
                exit 0
                ;;
            "a")
                action_do
                exit 1
                ;;
            "d")
                action_do_arg $OPTARG
                exit 1
                ;;
            "c")
                C_ARG=$OPTARG
                ;;
            "b")
                D_ARG=$OPTARG
                ;;
            "?")
                show_help
                exit 1
                ;;
            ":")
                show_help
                exit 1
                ;;
        esac
    done
    return $OPTIND
}

if [ $# -gt 0 ];then
    runopt "$@"
    if [ $? -le 1 ];then
        echo "Cannot parse args: $@"
        show_help
        exit 1
    fi
    multi_args_do
    exit 0
fi
default_action


