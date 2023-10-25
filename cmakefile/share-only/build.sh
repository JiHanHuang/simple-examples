#!/bin/bash
echo "-------clean old dir-------"
echo "rm -rf build"
rm -rf build
mkdir -p build
cd build
echo "-------cmake ../-------"
cmake ../
echo "-------make-------"
make -j4
if [ $? -ne 0 ];then
    exit 1
fi
echo "------- make install -------"
make install
echo "------- build over -------"