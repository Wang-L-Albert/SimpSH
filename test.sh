#!/bin/bash

####################
####  Lab 1A   #####
####################

#./simpsh --rdonly hi.txt
#./simpsh --wronly hi.txt
#./simpsh --verbose --rdonly hi.txt --wronly bye.txt
#./simpsh --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 tr a-z A-Z --wait
#./simpsh --verbose --rdonly sortbert.txt --wronly finbert.txt --wronly errbert.txt --command 0 1 2 sort --verbose 
#./simpsh --rdonly hi.txt --wronly bye.txt --rdonly sortbert.txt --wronly finbert.txt --wronly errbert.txt --command 0 1 4 tr a-z A-Z --command 2 3 4 sort 
#./simpsh --verbose --rdonly sortbert.txt --wronly finbert.txt --wronly errbert.txt --command 0 1 2 sort --verbose --rdonly hi.txt
#./simpsh --rdonly hi.txt --wronly NOEXIST.txt --rdonly sortbert.txt --wronly finbert.txt --wronly errbert.txt  --command 0 1 4 tr a-z A-Z --command 2 3 4 sort

#SYNTAX ERROR CASES
#./simpsh --rdonly
#./simpsh --rdonly hi.txt bye.txt #wrong?
#./simpsh --rdonly fake.txt  #cannot be opened
#./simpsh --rdonly hi.txt bye.txt --wronly finbert.txt #skip over?
#./simpsh --rdonly hi.txt --wronly bye.txt --command 0 1 2 sort #no stderr
#./simpsh --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 3 sort #too many args
#./simpsh --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 4 0 0 sort
#./simpsh  --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 bogus #not a command
#./simpsh  --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 tr #missing command args

################
#### Lab 1B ####
################

#append
#	./simpsh --rdonly hi.txt --creat --wronly hellbert.txt --append --wronly errbert.txt --command 0 1 2 tr a-z A-Z
#	./simpsh --rdonly hi.txt --creat --wronly hellbert.txt --creat --append --wronly errbert.txt --command 0 1 2 bogus
#cloexec
#creat
#	./simpsh --rdonly hi.txt --creat --wronly hellbert.txt
#directory
#dsync
#excl
#nofollow
#nonblock
#rsync
#sync
#trunc

#Done testing oflags. Implementation of all flags done similarly (bitwise orring). 2 of the flags work, therefore the rest should work. 

#rdwr
#	./simpsh --verbose --rdwr hi.txt --rdwr hi.txt --wronly errbert.txt --command 0 1 2 tr a-z A-Z --wait
#pipe
	# ./simpsh --rdonly sortbert.txt --pipe --wronly finbert.txt --wronly errbert.txt --command 0 2 4 tr a-z A-Z --command 1 3 4 sort
	# ./simpsh --verbose --rdonly sortbert.txt --pipe --pipe --wronly finbert.txt --wronly errbert.txt --command 0 2 6 tr a-z A-Z --command 1 4 6 tr A a --command 3 5 6 tr Z z




# #wait
# 	./simpsh --verbose --rdonly sortbert.txt --wronly finbert.txt --wronly errbert.txt --command 0 1 2 sort --verbose --wait
# 	./simpsh --rdonly sortbert.txt --pipe --wronly finbert.txt --wronly errbert.txt --command 0 2 4 tr a-z A-Z --command 1 3 4 sort --wait
# #close N
# 	./simpsh --rdonly sortbert.txt --pipe --close 2 --wronly finbert.txt --wronly errbert.txt --command 0 2 4 tr a-z A-Z --command 1 3 4 sort --wait
# 	./simpsh --rdonly sortbert.txt --pipe --close 1 --wronly finbert.txt --wronly errbert.txt --command 0 2 4 tr a-z A-Z --command 1 3 4 sort --wait
# 	./simpsh --rdonly sortbert.txt --pipe --wronly finbert.txt --wronly errbert.txt --command 0 2 4 tr a-z A-Z --command 1 3 4 sort --close 1 --wait
# 	./simpsh --rdonly sortbert.txt --pipe --wronly finbert.txt --wronly errbert.txt --command 0 2 4 tr a-z A-Z --command 1 3 4 sort --close 2 --wait
# #abort
# 	./simpsh --rdonly hi.txt --wronly bye.txt --abort
# #catch N
# 	./simpsh --catch 11 --abort
# #ignore N
# 	#./simpsh --ignore 2 --pause #run by hand to see that ^C does nothing. must close terminal.
# 	./simpsh --ignore 11 --abort #seg faults
# #default N
# 	./simpsh --ignore 2 --default 2 --pause
# #pause
# 	./simpsh --catch 2 --pause

# #SYNTAX ERROR CASES
# 	./simpsh --rdonly hi.txt --wronly NOEXIST.txt --rdonly sortbert.txt --wronly finbert.txt --wronly errbert.txt  --command 0 1 4 tr a-z A-Z --command 2 3 4 sort --wait

# 	./simpsh --rdonly sortbert.txt --pipe --wronly finbert.txt --wronly errbert.txt --command 0 2 4 tr a-z A-Z --command 1 3 4 sort --close NOEXIST.txt --wait

################
#### Lab 1C ####
################

	head -c 20000000 </dev/urandom > rand.txt
	head -c 20000000 </dev/urandom > rand2.txt
	echo
	#./simpsh --profile --rdonly rand.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 sort --wait
	#./simpsh --profile --rdonly rand.txt --pipe --pipe --creat --trunc --wronly finbert.txt --creat --append --wronly errbert.txt --command 0 2 6 cat rand.txt --command 3 5 6 cat - rand.txt --command 1 4 6 cat - rand.txt rand.txt --wait
	#./simpsh --profile --rdonly rand.txt --pipe --pipe --pipe --rdonly finbert.txt --rdonly errbert.txt --command 0 2 8 cat rand.txt --command 1 4 8 sort --command 3 6 8 tr ' ' 'a' --command 4 7 8 tr '[0-9]' '[a-j]' --wait


#######################
#### Problem Design ###
#######################

	
#	./simpsh  --parallel 2 --rdonly hi.txt --pipe --pipe --creat --wronly finbert.txt --creat --append --wronly errbert.txt --command 0 2 6 cat hi.txt --command 3 5 6 cat - hi.txt --command 1 4 6 cat - hi.txt hi.txt
	./simpsh --rdonly hi.txt --pipe --pipe --pipe --wronly finbert.txt --wronly errbert.txt --command 0 2 8 cat hi.txt --command 1 4 8 sort --command 3 6 8 tr ' ' 'a' --command 5 7 8 tr '[0-9]' '[a-j]' --wait
#	./simpsh --rdonly hi.txt --pipe --pipe --wronly finbert.txt --wronly errbert.txt --command 0 2 6 cat hi.txt --command 1 4 6 sort --command 3 5 6 tr ' ' 'a' --wait

#	./simpsh --parallel a --rdonly rand.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 sort --wait
#	./simpsh --parallel 10 --rdonly rand.txt --pipe --pipe --pipe --rdonly finbert.txt --rdonly errbert.txt --command 0 2 8 cat rand.txt --command 1 4 8 sort --command 3 6 8 tr ' ' 'a' --command 4 7 8 tr '[0-9]' '[a-j]' --wait
#	./simpsh --parallel 0 --rdonly rand.txt --pipe --pipe --pipe --rdonly finbert.txt --rdonly errbert.txt --command 0 2 8 cat rand.txt --command 1 4 8 sort --command 3 6 8 tr ' ' 'a' --wait
#	./simpsh --parallel -3 --rdonly rand.txt --pipe --pipe --pipe --rdonly finbert.txt --rdonly errbert.txt --command 0 2 8 cat rand.txt --command 1 4 8 sort --command 3 6 8 tr ' ' 'a' --command 4 7 8 tr '[0-9]' '[a-j]' --wait

