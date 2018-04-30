#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>

int check_prime(int nr)
{
	for(int i = 2; i <= nr/2; i++)
		if(nr%i==0)	return 0;
	return 1;
}




void * thread_code(void * fd_){
	int *fd = (int *) fd_;
 	int primes=0;
  	while(1)	{	
   	int aux;	
	
		if ( read(fd[0], &aux, sizeof(int)) == 0) break;
		
		if ( check_prime(aux) ) primes ++;
  	}
return (void *)&primes; 
}




int main(int argc, char **argv){

if  (argc != 3) exit(1);


int slaves_nr = atoi(argv[1]), numbers_nr = atoi(argv[2]);
int fd[2];
int i=0;
int return_value;
int sum=0;

pipe(fd);


thread_ids= (pthread_t*)malloc(sizeof(slaves_nr * pthread_t));

while(i < slaves_nr) {	
	pthread_create(&(thread_ids[i]), NULL, thread_code, (void*) fd);
	i++;
}

for (i = 0; i < numbers_nr; i ++){
	int random_nr = random() % 100000;
	write(fd[1], &random_nr, sizeof(int));
}

close(fd[1]);

while(i < slaves_nr) {	
	pthread_join(thread_ids[i], & (void *)&return_value);
	sum+=return_value;
	i++;
}

printf("Número total de primos %d\n", sum);

}