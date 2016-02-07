CC = gcc
CFLAGS = -g -Wall -Wextra -Wno-unused -Werror -std=c99
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
#	rm hellbert.txt
	rm rand.txt

check: default
	echo "hi albert" >> hi.txt
	echo "bye albert" >> bye.txt
	echo "final albert" >>finbert.txt
	echo "error bert" >> errbert.txt	
	echo "z\noooo\na\nwhatthe" >> sortbert.txt
	chmod +x test.sh
	./test.sh
	
dist: check
	mkdir lab1-laurenyeung
	mv *.sh *.c *.pdf *simpsh *Makefile *README lab1-laurenyeung
	tar -cvf lab1-laurenyeung.tar.gz lab1-laurenyeung


	
