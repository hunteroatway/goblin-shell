CC = cc 

all : 
	gcc shell.c -o build/shell 
	gcc server.c -o build/server 
	gcc client.c -o build/client 
	gcc raid.c -o build/raid -pthread

check : all 
	./shell
	./server
	./client
	./raid

clean : 
	rm -f *.o shell server client raid core.*
