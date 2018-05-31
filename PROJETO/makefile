CFLAGS= -O2 -Ofast -g -Wall -std=gnu11

default: allin

allin: app clipboard

app: app_teste.c library.c
	gcc app_teste.c library.c -o app $(CFLAGS)

clipboard: clipboard.c library.c threads.c regions.c utils.c
	gcc clipboard.c library.c threads.c regions.c utils.c -pthread -g -o clipboard $(CFLAGS)

clean:
	rm -f *.o app clipboard
