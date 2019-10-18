/******************************************************************************
 *
 * File Name: parallel_apps.c
 *	      (c) 2018 PSis
 * Authors: Pedro Moreira e Inês Gaspar
 *
 * DESCRIPTION
 *		app that connects (2^FORK_CYCLE * THREADS_NR) to the clipboard
 *****************************************************************************/


#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

//#define WRONG_FD


#define FORK_CYCLE 10
#define THREADS_NR 10

void *ola(void * fd_){

char palavra[10]; 
#ifndef WRONG_FD
int fd = clipboard_connect("./");
#endif

#ifdef WRONG_FD
int fd = fd_;
#endif
		if(fd== -1){
			return NULL;
		}

	for (int i=0; i<10; i++)
	{
		if ( clipboard_copy(fd , i, palavra, 10) != 10)
			printf("falhou copy\n");
		if ( clipboard_paste(fd , i, palavra, 10) != 10)
			printf("falhou paste\n");
	}
	close(fd);

	return (void *) NULL;
	
}

int main(){
int i;
int fd;
int *a;
a = (int *) malloc(sizeof(int)); 

for (i=0; i<FORK_CYCLE; i++){
	fork();
}

#ifdef WRONG_FD
fd = clipboard_connect("./");
#endif

pthread_t thread_id[THREADS_NR];

for (i=0; i<THREADS_NR; i++)
{
	 if (pthread_create(&(thread_id[i]), NULL, ola, (void *) fd) != 0)
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
