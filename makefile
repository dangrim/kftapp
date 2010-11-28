all: kftserver kftclient

kftserver : udpserver.c die.c
	gcc -o kftserver udpserver.c die.c

kftclient : udpclient.c die.c	
	gcc -o kftclient udpclient.c die.c
