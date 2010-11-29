all: server client

kftserver : udpserver.c die.c kft.c file.c
	gcc -o kftserver udpserver.c die.c kft.c file.c

kftclient : udpclient.c die.c	kft.c file.c
	gcc -o kftclient udpclient.c die.c kft.c file.c

kft :	kft.c
	gcc -o kft kft.c

server : kftserver.c die.c
	gcc -o server kftserver.c die.c

client : kftclient.c die.c
	gcc -o client kftclient.c die.c
