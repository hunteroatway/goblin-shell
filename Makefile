CC = cc 

all : 
	gcc shell.c -o shell 
	gcc server.c -o server 
	gcc client.c -o client 
	gcc raid.c -o raid -pthread

check : all 
	./shell
	./server
	./client
	./raid

clean : 
	rm -f *.o shell server client raid core.*
