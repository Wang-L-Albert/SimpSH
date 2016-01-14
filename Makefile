CC = gcc
CFLAGS = -g -Wall -Wextra -std=gnu11
EXECUTABLE_NAME = simpsh

default: main.c
	$(CC) $(CFLAGS) $^ -o $(EXECUTABLE_NAME)

    
clean:
	rm -f $(EXECUTABLE_NAME) *.o *.tar.gz
	rm hi.txt
	rm bye.txt
	rm errbert.txt
	rm sortbert.txt
	rm finbert.txt

check: default
	echo "hi albert" >> hi.txt
	echo "bye albert" >> bye.txt
	echo "final albert" >>finbert.txt
	echo "error bert" >> errbert.txt	
	echo "z\noooo\na\nwhatthe" >> sortbert.txt
	./$(EXECUTABLE_NAME) --rdonly hi.txt
	./$(EXECUTABLE_NAME) --wronly bye.txt
	./$(EXECUTABLE_NAME) --verbose --rdonly hi.txt --wronly bye.txt
	#./$(EXECUTABLE_NAME) --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 tr a-z A-Z
	#./$(EXECUTABLE_NAME) --rdonly sortbert.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 sort 
	./$(EXECUTABLE_NAME) --rdonly hi.txt --wronly bye.txt --rdonly sortbert.txt --wronly finbert.txt --wronly errbert.txt --command 0 1 4 tr a-z A-Z --command 2 3 4 sort 

dist: check
	mkrdir lab1-laurenyeung
	tar -C /lab1-laurenyeung/ -cvf lab1-laurenyeung.tar.gz *.c *Makefile README.*


	
