#!/bin/bash
head -c 20000000 </dev/urandom > rand.txt
/usr/bin/time -p sort rand.txt > /dev/null
