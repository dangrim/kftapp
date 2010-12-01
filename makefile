all: kftserver kftclient

kftserver : kftserver.c die.c dropper.c
	gcc -o kftserver kftserver.c die.c dropper.c

kftclient : kftclient.c die.c dropper.c
	gcc -o kftclient kftclient.c die.c dropper.c
