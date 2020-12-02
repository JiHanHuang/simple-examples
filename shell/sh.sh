#!/bin/bash
###
 # @Author: JiHan
 # @Date: 2018-02-06 14:53:12
 # @LastEditTime: 2020-12-01 16:50:32
 # @Description:  A shell script with parameters
### 

OUT=''

runopt (){
    while getopts "aho:" opts
    do
        case $opts in
            "a")
                echo "This is a shell script with parameters."
                ;;
            "h")
                echo "
Usage: >_< [-a|h] [-o out_file]
  
Emergency help:  
 -a                   Show message  
 -o out_file          Out put file
"
                exit 0
                ;;
            "o")
                OUT=$OPTARG
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

echo "OUT:$OUT"