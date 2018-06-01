#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define FORK_CYCLE 10
#define THREADS_NR 10

void *ola(void * xx){

int *a =(int *) malloc(sizeof(int));
if(a == NULL)
	printf("falhou malloc\n");

char palavra[10]; 

int fd = clipboard_connect("./");
		if(fd== -1){
			//printf("Clipboard is off\n");
			return (void *) a;
		}

	for (int i=0; i<10; i++)
	{
		clipboard_copy(fd , i, palavra, 10);
		clipboard_paste(fd, i, palavra, 10); 
	}
	close(fd);

	return (void *) a;
	
}

int main(){
int i;
int *a;
a = (int *) malloc(sizeof(int)); 

for (i=0; i<FORK_CYCLE; i++){
	fork();
}

pthread_t thread_id[THREADS_NR];

for (i=0; i<THREADS_NR; i++)
{
	 if (pthread_create(&(thread_id[i]), NULL, ola, (void *) &i) != 0)
	 	printf("falhou pthread_create\n");
}

for (int j=0; j<THREADS_NR; j++)
{//printf("de %d here1\n", getpid());
	if (pthread_join((thread_id[j]), (void *) a) != 0)
		printf("falhou pthread_join\n");
	//printf("de %d here2\n", getpid());
}
printf("acabou processo\n");

return 0;
}
