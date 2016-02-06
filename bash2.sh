#!/bin/bash
time od -tf -i -N 1000000 < /dev/urandom | tr a-z A-Z > /dev/null 
