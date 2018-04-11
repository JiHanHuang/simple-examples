#!/bin/bash

# JiHan 2018.02.06
# A shell script with parameters
# Using in SW platform

OUT='./a.out'
RUNNUM=2
QUEUE='q_x86_expr'
LOG=log

runopt (){
    while getopts "ahf:n:o:q:l:" opts
    do
        case $opts in
            "a")
                echo "This is a shell to bsub task in sw."
                ;;
            "h")
                echo "
Usage: >_< [-ahfnoql] 
                [-f filename] [-n node_number]
                [-o out_file] [-q sw_queue]  
                [-l log_file]   
  
Emergency help:  
 -a                   Show message  
 -h                   Help
 -f filename          A file to make 
 -n node_number       SW -n number,  
 -o out_file          Out put file
 -q sw_queue          SW -q select queue  
 -l log_file          Out put log file
"
                return 0
                ;;
            "f")
                make SRC=$OPTARG
                ;;
            "n")
                RUNNUM=$OPTARG
                ;;
            "o")
                OUT=$OPTARG
                ;;
            "q")
                QUEUE=$OPTARG
                ;;
            "l")
                LOG=$OPTARG
                ;;
            "?")
                echo "Invalid option -$OPTARG"
                ;;
            ":")
                echo "No argument value for option -$OPTARG"
                ;;
        esac
    done
    return $OPTIND
}


if [ $# -lt 1 ];then
    echo "no opts"
    make clean
    make
    bsub -I -q ${QUEUE} -n ${RUNNUM} ${OUT} | tee ${LOG}
else
#    ???????
#    optinfo=$(runopt "$@")
#    echo "$optinfo"
    runopt "$@"
#   $? is the last command execution status
    if [ $? -gt 1 ];then
        bsub -I -q ${QUEUE} -n ${RUNNUM} ${OUT} | tee ${LOG}
    fi
fi


