#!/usr/local/cs/execline-2.1.4.5/bin/execlineb -P
redirfd -w 1 finbert.txt exec
redirfd -w 2 errbert.txt exec
sort rand.txt

