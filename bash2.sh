#!/bin/bash

time cat < rand.txt | cat - rand.txt | cat - rand.txt rand.txt > /dev/null
