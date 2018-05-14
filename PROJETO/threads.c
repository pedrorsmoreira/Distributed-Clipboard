#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <semaphore.h>

#include "clipboard.h"
#include "threads.h"
#include "regions.h"

/**
 * @brief      Generates a random number in a valid
 *             range for a computer port connection
 *
 * @return     The random number
 */
int rand_port_gen(){
	srand(time(NULL));
	int ret = 1024 + rand()%63715;
	return ret;
}

/**
 * @brief      Initializes a socket stream server
 *
 * @param[in]  family	Stream type (UNIX or INET)
 *
 * @return     socket file descriptor 
 */
void *server_init(void * family){
	//assures there was no previous socket with the same name
	unlink(SOCK_ADDRESS);

	struct sockaddr *local_addr;
	socklen_t addrlen;

	int *sock_fd = (int *) malloc(sizeof(int));

	//set the communication type parameters 
	if (family == (void *) UNIX){
		struct sockaddr_un local_addr_un;
		addrlen = sizeof(local_addr_un);
		local_addr_un.sun_family = AF_UNIX;
		strcpy(local_addr_un.sun_path, SOCK_ADDRESS);
		local_addr = (struct sockaddr *) &local_addr_un;
		*sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	}
	else if (family == (void *) INET){
		struct sockaddr_in local_addr_in;
		addrlen = sizeof(local_addr_in);
		local_addr_in.sin_family = AF_INET;
		int port = rand_port_gen();
		printf("port number: %d\n", port);
		local_addr_in.sin_port = htons(port);
		local_addr_in.sin_addr.s_addr = INADDR_ANY;
		local_addr = (struct sockaddr *) &local_addr_in;
		*sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	}
	//check endpoint creation faliure
	if (*sock_fd < 0){
		perror("socket: ");
		exit (-1);
	}

	//address the socket (own it)
	if ( bind(*sock_fd, local_addr, addrlen) < 0){
		perror("bind: ");
		exit (-1);
	}

	//get ready to act as a server
	if (listen (*sock_fd, 10) == -1){
		perror("listen: ");
		exit (-1);
	}

 return (void *) sock_fd;
}

/**
 * @brief      Recursive function - accepts a socket stream 
 *             connection every function call and sets the
 *             processing of the new connection
 *
 * @param[in]  CS_   pointer to stream connection parameters
 * 
 * @return     useless
 */
void *accept_clients(void * CS_){
	client_socket *CS = (client_socket *) CS_;

	struct sockaddr *client_addr;
	
	if (CS->param == UNIX){
		struct sockaddr_un client_addr_un;
		client_addr = (struct sockaddr *) &client_addr_un;
	}
	else if (CS->param == INET){
		struct sockaddr_in client_addr_in;
		client_addr = (struct sockaddr *) &client_addr_in;
	}
	
	socklen_t size = sizeof(struct sockaddr);
	
	//stablish connection with the client
	int client_fd = accept( CS->sock_fd, client_addr, &size);
	if (client_fd == -1){
		perror("accept: ");
		exit (-1);
	}

	//create threads to update the clipboard "client" regions
	if (CS->param == INET){
		client_socket RU;
		RU.sock_fd = client_fd;
		pthread_t up_thread[REGIONS_NR];
		for (int i = 0; i < REGIONS_NR; i ++){
			RU.param = i;
			if (pthread_create(&up_thread[i], NULL, region_update, &RU) != 0){
				perror("pthread_create: ");
				exit(-1);
			}
		}
	}
	//create new thread for next client connection
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, accept_clients, CS) != 0){
		perror("pthread_create: ");
		exit(-1);
	}
	//handle clients direct requests
	client_handle(client_fd);

	//terminate threads created for updates
	if (CS->param == INET){
		for (int i = 0; i < REGIONS_NR, i ++)
			pthread_cancel(up_thread[i]);
	}

 return NULL;
}

void *region_update(void *RU_){
	client_socket RU = *( (client_socket *)RU );
	
	while(1){

	/*
		A fazer: (na funcao que cria estas variaveis, provavelmente o main)
		Inicializar o semaforo com 0 tokens, so quando o utilizador manda fazer uma acao e
		que colocamos la tokens. usamos sem_post( ENDERECO DO SEMAFORO).
		Inicializar o count a 0.
		Inicializa o mutex e a variavel de condicao.
	*/
	sem_t semaphores[REGIONS_NR];
	pthread_mutex_t mutexes[REGIONS_NR];
	pthread_cond_t conditions[REGIONS_NR];
	for (int i=0; i < REGIONS_NR; i++){
		/* inicialmente o semaforo nao tem tokens! */
		sem_init(&(semaphores[i]), 0, 0);
		/* NULL => inicializacao defaut */
		pthread_mutex_init(&(mutexes[i]), NULL);
		pthread_cond_init(&(conditions[i]), NULL);
		/* se nao quiseres usar estas duas funcoes, basta fazeres estas 2 atribuicoes, que sao
		exactamente equivalentes: 
			mutexes[i] = PTHREAD_MUTEX_INITIALIZER;
			conditions[i] = PTHREAD_COND_INITIALIZER;
		/*
	}s

	/*

	Funcoes utilizadas:

	 sem_wait( ender. semaforo) -> tenta retirar um token do semaforo. Se nao houver la tokens
		fica em espera ate que haja e so depois prossegue a partir daqui 

	 sem_post( ender. semaforo) -> coloca no semaforo um token.

	 mutex_lock( ender, mutex) -> tenta adquirir o mutex. Se ja alguem tiver feito lock 
		primeiro, ele nao consegeu adquiri-lo ficando a espera que ele esteja disponivel.
		Conseguindo adquiri-lo, prossegue o seu codigo.

	 mutex_unlock( ender. mutex) -> liberta o mutex anteriormente adquirido.

	 cond_wait( var. cond. , ender. mutex) -> liberta o mutex anteriormente adquirido e fica 
		em espera ate que a var. cond. esteja no estado desejado (por exemplo um boleano estar 
		a TRUE - ver a funcao abaixo). Verificando-se o estado ela tenta readquirir o mutex.
		Caso o mutex esteja ocupado, ela fica num subwait para o adquirir. Quando adquirir,
		prossegue o seu codigo.
		NOTA: esta funcao deve estar dentro de um ciclo de proteccao porque mesmo que por vezes
		a condicao nao esteja no estado desejado, o proprio sistema Operativo retira-a do wait
		sem avisar.

	 cond_broadcast( var. cond.) -> este funcao avisa todas as threads que estejam em espera, 
		que a var. cond. em questao atingiu o estado pretendido, podendo por isso sair do wait. 
		Note que de seguida ela ainda tem que conseguir dar lock ao mutex. Caso contrario, 
		fica numa subespera!
	*/ 
		sem_wait( &(semaphores[index]));

		/* (*NOTA) em vez deste sem_post, o PROG quando vai colocar o primeiro token no semafor, pode
			em ciclo, colocar la logo todos os tokens necessarios a todas as threads que
			vao executar */

		sem_post( &(semaphores[index]));

		-------------------------
		  -- EXECUTAR CODIGO --
		-------------------------

		pthread_mutex_lock(&(mutexes[index]));
		count++;
		if(count == REGIONS_NR){
		/* (*NOTA) este sem_wait so e usado se os sem_post estiverem nesta funcao!
		Caso contrario, nao e necessario.
		*/
			sem_wait( &(semaphores[index]));
			count = 0;
			pthread_cond_broadcast( &(conditions[index]));
		}		
		while( count != 0)
			pthread_cond_wait( &(conditions[index]), &(mutexes[index]));
		p_thread_mutex_unlock( &(mutexes[index]));
	}
 return NULL
}

/**
 * @brief      handles client requests (and remote 
 			   updates) until it closes the connection	
 *
 * @param[in]  client_fd  client file descriptor
 */
void client_handle(int client_fd){
	Smessage data;
	int data_size = sizeof(Smessage);

	//listens until the connection is closed
	while ( read(client_fd, &data, data_size) > 0){
		//check for valid region
		if ( (data.region < 0) || (data.region > REGIONS_NR))	
			exit(-2);
		//copy or paste
		if (data.order == COPY){
			update_region(client_fd, data, data_size);
			TURN ON THE UPDATE THREADS OF REGION X
			CLOSE THE GATE AGAIN...ppppppppppppppppppppppppppppppppppp
		}
		else if (data.order == PASTE)
			send_region(client_fd, data, data_size);
		else if (data.order == WAIT){
		//	WAIT FOR OPEN GATE ON REGION X ppppppppppppppppppppppppppppppp
		//	send_region(client_fd, data, data_size);
		}
		else exit(-2);
	}
}