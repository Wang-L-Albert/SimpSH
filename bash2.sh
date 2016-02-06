#!/bin/bash
time od -tf -i -N 2000000 < /dev/urandom | tr a-z A-Z > /dev/null 
