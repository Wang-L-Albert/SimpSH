#!/bin/bash
head -c 20000000 </dev/urandom > rand.txt
head -c 20000000 </dev/urandom > rand2.txt
time cat < rand.txt | sed -e 's/a/A/'| tr a-z A-Z | tr A-Z a-z | diff - rand2.txt | grep 'a' - | uniq - > /dev/null
