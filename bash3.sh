#!/bin/bash
time cat < rand.txt | sort - | tr ' ' 'a' | tr '[0-9]' '[a-j]' > /dev/null
