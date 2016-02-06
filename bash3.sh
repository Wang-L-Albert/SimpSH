#!/bin/bash
head -c 20000000 </dev/urandom > rand2.txt
time cat < rand.txt | sort - | tr '\n' '\t' | tr '[0-9]' '[a-j]' > /dev/null
