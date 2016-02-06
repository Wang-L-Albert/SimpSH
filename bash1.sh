#!/bin/bash
head -c 20000000 </dev/urandom > rand.txt
time sort rand.txt > /dev/null
