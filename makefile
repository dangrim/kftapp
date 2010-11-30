all: server client

server : kftserver.c die.c
	gcc -o server kftserver.c die.c

client : kftclient.c die.c
	gcc -o client kftclient.c die.c
