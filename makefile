all: kftserver kftclient

kftserver : udpserver.c die.c kft.c
	gcc -o kftserver udpserver.c die.c kft.c

kftclient : udpclient.c die.c	kft.c
	gcc -o kftclient udpclient.c die.c kft.c

kft :	kft.c
	gcc -o kft kft.c
