#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int counter;

void reset_counter(int wtv){
counter = 0;
srandom(time(NULL));
int reset_time = random()%11;
alarm(reset_time);
}



int main(){

//criar a estrutura para receber o SIGALRM
struct sigaction action;

// (all signals are allowed to interrupt execution of this handler) ????
sigemptyset(&action.sa_mask);

//pôr a funcão reset_counter como handler da estrutura action
action.sa_handler = reset_counter;

//associar action aos sianis SIGALRM ( que são os sinais gerados pela funcao alarm(x) )
if ( sigaction(SIGALRM, &action, NULL) == -1 )
{
printf("erro na func siagaction\n");
exit(1);
}

// não ponho alarm(0) porque o "0" como argumento tem um significado específico (any pending alarm is canceled)
alarm(1);

while(1){
sleep(1);
printf("current counter value - %d \n", counter++);
}

exit(0);
}