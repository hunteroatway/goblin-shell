CC = cc 

all : 
	gcc shell.c -o shell 
	gcc server.c -o server 
	gcc client.c -o client 

check : all 
	./shell
	./server
	./client

clean : 
	rm -f *.o shell server client core.*