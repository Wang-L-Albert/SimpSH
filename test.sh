#!/bin/bash

./simpsh --rdonly hi.txt
./simpsh --wronly hi
./simpsh --rdonly hi.txt
./simpsh --wronly bye.txt
./simpsh --verbose --rdonly hi.txt --wronly bye.txt
./simpsh --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 tr a-z A-Z
./simpsh --verbose --rdonly sortbert.txt --wronly finbert.txt --wronly errbert.txt --command 0 1 2 sort --verbose
./simpsh --rdonly hi.txt --wronly bye.txt --rdonly sortbert.txt --wronly finbert.txt --wronly errbert.txt --command 0 1 4 tr a-z A-Z --command 2 3 4 sort 
./simpsh --verbose --rdonly sortbert.txt --wronly finbert.txt --wronly errbert.txt --command 0 1 2 sort --verbose --rdonly hi.txt


./simpsh --rdonly

./simpsh --rdonly hi.txt bye.txt #wrong?

./simpsh --rdonly fake.txt  #cannot be opened

./simpsh --rdonly hi.txt bye.txt --wronly finbert.txt #skip over?

./simpsh --rdonly hi.txt --wronly bye.txt --command 0 1 2 sort #no stderr

./simpsh --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 3 sort #too many args

./simpsh --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 4 0 0 sort

./simpsh  --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 bogus #not a command

./simpsh  --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 tr #missing command args
