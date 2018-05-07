CFLAGS= -g -Wall -std=gnu11

default: all

all: app clipboard

app: app_teste.c library.c
	gcc app_teste.c library.c -o app $(CFLAGS)

clipboard: clipboard.c library.c
	gcc clipboard.c library.c -o clipboard $(CFLAGS)

clean:
	rm -f *.o app clipboard
