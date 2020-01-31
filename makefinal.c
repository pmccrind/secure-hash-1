final: final.o
	gcc final.o -lm -o final

final.o: final.c
	gcc -c final.c -lm

