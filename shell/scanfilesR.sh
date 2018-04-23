#!/bin/sh

#unable using path like: ~/dir or /dir

files=
OLD_IFS="$IFS"
IFS=" "
tmp=($@)
IFS="$OLD_IFS"
lpath=$(pwd)
num=0

function scandir() {
    local cur_dir parent_dir workdir
    workdir=$1
    cd ${workdir}
    if [ ${workdir} = "/" ]
    then
        cur_dir=""
    else
        cur_dir=$(pwd)
    fi  

    for dirlist in $(ls ${cur_dir})
    do  
        if test -d ${dirlist};then
            cd ${dirlist}
            scandir ${cur_dir}/${dirlist}
            cd ..
        else
             files=${files}" "${cur_dir}/${dirlist}
#             echo ${cur_dir}/${dirlist}
        fi  
    done
}

if [ $# -lt 1 ];then
   scandir ${lpath}
#   echo $files
else 
  while [ $num -lt $# ]
  do
#           echo ">>"${lpath}/${tmp[$num]}
        if test -d "${lpath}/${tmp[$num]}"
        then
           scandir ${lpath}/${tmp[$num]}
        elif test -f "${lpath}/${tmp[$num]}"
        then
           files=${files}" "${lpath}/${tmp[$num]}
        else
            echo "the Directory isn't exist which you input,pls input a new one!!"
        fi  
        num=$(($num+1))
  done
#  echo $files

fi

dos2unix $files
