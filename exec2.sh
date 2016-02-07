#!/usr/local/cs/execline-2.1.4.5/bin/execlineb -P
redirfd -w 1 finbert.txt exec
redirfd -w 2 errbert.txt exec
pipeline {
	cat rand.txt
} pipeline {
	cat rand.txt -
} cat rand.txt rand.txt -
