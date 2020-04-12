CC = cc 

all : 
	gcc shell.c -o goblin-shell 
	gcc server.c -o server 
	gcc client.c -o client 
	gcc raid.c -o raid -pthread

check : all 
	./goblin-shell
	./server
	./client
	./raid

clean : 
	rm -f *.o goblin-shell server client raid core.*