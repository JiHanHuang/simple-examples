#!/bin/bash

rm server -f
rm client -f

g++ -g client.cpp -o client -lssl -lcrypto
g++ -g server.cpp -o server -lssl -lcrypto

echo "build finish"