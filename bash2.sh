#!/bin/bash
#./simpsh --profile --rdonly hi.txt --creat --wronly errbert.txt --pipe --wronly /dev/null --command 0 3 1 od -t -f -i -N 1000000 /dev/urandom --command 2 4 1 tr a-z A-Z  --wait
/usr/bin/time -p od -tf -i -N 1000000 < /dev/urandom | tr a-z A-Z > /dev/null 
