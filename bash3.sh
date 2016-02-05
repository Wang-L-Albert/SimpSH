#!/bin/bash
head -c 20000000 </dev/urandom > rand.txt
head -c 20000000 </dev/urandom > rand2.txt
/usr/bin/time -p grep '[0-9]a-Z$' < rand.txt | cat - | sed -e 's/a/A/'| diff - rand2.txt | uniq `> /dev/null
