#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(){

 int i = 0, rand_seconds = 0;

	while(i < 10){	
		if (fork() == 0) break;
	i++;
	}

		if (i < 10){
		srandom(i);
		rand_seconds = random()%11; // dá sempre o mesmo valor....
		sleep(rand_seconds);
		printf("I'm the child #%d, PID - %d and slept for %d seconds \n", i+1, getpid(), rand_seconds);
		}

return 0;
}