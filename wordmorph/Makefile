CC = gcc
CFLAGS = -Wall -ansi -O3
LFLAGS = -g
OBJS = main.o problems.o dictionary.o check_args.o AsortingFuncs.o graphs.o heaps.o
EXEC = wordmorph

$(EXEC): $(OBJS)
	$(CC) $(CFFLAGS) -o $(EXEC) $(OBJS)

main.o: main.c problems.h AsortingFuncs.h check_args.h
	$(CC) $(CFLAGS) -c main.c

problems.o: problems.c problems.h
	$(CC) $(CFLAGS) -c problems.c

dictionary.o: dictionary.c dictionary.h
	$(CC) $(CFLAGS) -c dictionary.c

check_args.o: check_args.c check_args.h
	$(CC) $(CFLAGS) -c check_args.c

AsortingFuncs.o: AsortingFuncs.c AsortingFuncs.h
	$(CC) $(CFLAGS) -c AsortingFuncs.c
	
graphs.o: graphs.c graphs.h
	$(CC) $(CFLAGS) -c graphs.c

heaps.o: heaps.c heaps.h
	$(CC) $(CFLAGS) -c heaps.c

clean:
		echo "Apagar todos os ficheiros objeto e executavel"
		rm -f *.o $(EXEC)