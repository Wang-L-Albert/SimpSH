#!/usr/local/cs/execline-2.1.4.5/bin/execlineb -P
redirfd -w 1 finbert.txt exec
redirfd -w 2 errbert.txt exec
pipeline {
	cat rand.txt
} pipeline {
	sort
} pipeline {
	tr ' ' '\n'
} 
	tr '[0-9]' '[a-j]'
