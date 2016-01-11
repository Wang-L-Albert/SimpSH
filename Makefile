CC = gcc
CFLAGS = -g -Wall -Wextra -Werror -std=gnu11
EXECUTABLE_NAME = simpsh

default: main.c
	$(CC) $(CFLAGS) $^ -o $(EXECUTABLE_NAME)
    
clean:
	rm -f $(EXECUTABLE_NAME) *.o *.tar.gz
	
