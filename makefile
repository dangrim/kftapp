all: kftserver kftclient

kftserver : udpserver.c die.c kft.c file.c
	gcc -o kftserver udpserver.c die.c kft.c file.c

kftclient : udpclient.c die.c	kft.c file.c
	gcc -o kftclient udpclient.c die.c kft.c file.c

kft :	kft.c
	gcc -o kft kft.c
