CFLAGS= -O2 -Ofast -Wall -std=gnu11

default: clipboard

all: app clipboard buggy_app parallel_apps
apps: app buggy_app parallel_apps

parallel_apps: parallel_apps.c library.c
	gcc parallel_apps.c library.c -o parallel_apps $(CFLAGS)

buggy_app: buggy_app.c library.c
	gcc buggy_app.c library.c -o buggy_app $(CFLAGS)

app: app_teste.c library.c
	gcc app_teste.c library.c -o app $(CFLAGS)

clipboard: clipboard.c library.c threads.c regions.c utils.c
	gcc clipboard.c library.c threads.c regions.c utils.c -pthread -o clipboard $(CFLAGS)

clean:
	rm -f *.o app clipboard buggy_app parallel_apps
