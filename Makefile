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
	echo "hai albert" >> hai.txt
	touch hoi.txt
	echo "hoi albert" >> hoi.txt	
	./$(EXECUTABLE_NAME) --rdonly hi.txt
	./$(EXECUTABLE_NAME) --wronly hi.txt
	./$(EXECUTABLE_NAME) --verbose --rdonly hoi.txt --wronly hai.txt

dist:

	
