#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

int check_prime(int nr)
{
	for(int i = 2; i <= nr/2; i++)
		if(nr%i==0)	return 0;
	return 1;
}

int main(int argc, char **argv){

if  (argc != 3) exit(1);


int slaves_nr = atoi(argv[1]), numbers_nr = atoi(argv[2]);
int fd1[2], fd2[2];
int i=0;

pipe(fd1);
pipe(fd2);

while(i < slaves_nr) {	
	//se for filho sai
	if (fork() == 0) break;	
i++;
}

//pai
if (i == slaves_nr) {
	close(fd1[0]);
	close(fd2[1]);
	for (i = 0; i < numbers_nr; i ++){
	int random_nr = random() % 100000;
	write(fd1[1], &random_nr, sizeof(int));
	}
	
	close(fd1[1]);

	while(1) {
	int aux;
	if ( read(fd2[0], &aux, sizeof(int)) == 0) break;
		
		printf("I'm a prime %d\n", aux);
	}

close(fd2[0]);
}

//filho	
else	{
 close(fd1[1]);
 close(fd2[0]);
 //int primes=0;
  while(1)	{	
   int aux;	
	
	if ( read(fd1[0], &aux, sizeof(int)) == 0) break;
	
	if ( check_prime(aux) ) write(fd2[1], &aux, sizeof(int));
  }	
close(fd2[1]);
}

return 0;
}