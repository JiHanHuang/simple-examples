#!/bin/bash

#回显一行
set -x
echo "ls -al"
set +x

function cmder(){
	(set -x; $@)
}

cmder ls

echo "end"
