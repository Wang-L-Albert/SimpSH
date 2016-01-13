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

check:
	echo "hi albert" >> hi.txt
	echo "bye albert" >> bye.txt
	echo "error bert" >> errbert.txt	
	echo -e "z\noooo\na\n" >> sortbert.txt
	./$(EXECUTABLE_NAME) --rdonly hi.txt
	./$(EXECUTABLE_NAME) --wronly bye.txt
	./$(EXECUTABLE_NAME) --verbose --rdonly hi.txt --wronly bye.txt
	./$(EXECUTABLE_NAME) --rdonly hi.txt --wronly bye.txt --wronly errbert.txt --command 0 1 2 tr a-z A-Z

dist:

	
