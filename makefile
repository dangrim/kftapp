all: kftserver kftclient kft

kftserver : udpserver.c die.c
	gcc -o kftserver udpserver.c die.c

kftclient : udpclient.c die.c	
	gcc -o kftclient udpclient.c die.c

kft :	kft.c
	gcc -o kft kft.c
