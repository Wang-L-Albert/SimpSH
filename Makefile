CC = gcc
CFLAGS = -g -Wall -Wextra -std=gnu11
EXECUTABLE_NAME = simpsh

default: main.c
	$(CC) $(CFLAGS) $^ -o $(EXECUTABLE_NAME)
    
clean:
	rm -f $(EXECUTABLE_NAME) *.o *.tar.gz
	rm hi.txt
	rm hai.txt
	rm hoi.txt

check:
	touch hi.txt
	echo "hi albert" >> hi.txt
	touch hai.txt
	echo "bye albert" >> bye.txt
	touch hoi.txt
	echo "error bert" >> errbert.txt	
	./$(EXECUTABLE_NAME) --rdonly hi.txt
	./$(EXECUTABLE_NAME) --wronly bye.txt
	./$(EXECUTABLE_NAME) --verbose --rdonly hi.txt --wronly bye.txt
	./$(EXECUTABLE_NAME) --rdonly hi.txt --wronly hai.txt --wronly hoi.txt --command 0 1 2 tr a-z A-Z

dist:

	
