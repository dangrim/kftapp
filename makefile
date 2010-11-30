all: server client

server : kftserver.c die.c dropper.c
	gcc -o server kftserver.c die.c dropper.c

client : kftclient.c die.c dropper.c
	gcc -o client kftclient.c die.c dropper.c
