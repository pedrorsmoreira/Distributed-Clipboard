#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <signal.h>

#define MESSAGE_SIZE 10000
#define PASTE 1
#define WAIT 2

int read_region(){
	int region;
	char aux[50];

	while(1){
		printf("Introduce the region where to act(from 0 to %d): ", REGIONS_NR -1);
		if (fgets(aux, 50, stdin) == NULL){
			perror("fgets: ");
			exit(-1);
		}
		sscanf(aux, "%d", &region);
		if ((region >= 0) && (region < REGIONS_NR))	break;
		printf("Invalid region\n---\n");
	}
	return region;
}

void paste(int order, int fd){
	int region;
	size_t nbytes;
	printf("write output file name:\n");
	char out[50];
	char aux[50];
	if (fgets(out, 50, stdin) == NULL){
		perror("fgets: ");
		exit(-1);
	}
	out[strlen(out) - 1] = '\0';
	FILE *file = NULL; 

	file = fopen(out, "w");

	printf("Introduce the number of bytes to alloc:");
	if (fgets(aux, 50, stdin) == NULL){
		perror("fgets: ");
		exit(-1);
	}
	sscanf(aux, "%d", &nbytes);

	char * buffer = (char *) malloc(nbytes);
	if (buffer == NULL){
		printf("allocation failed\n");
		exit(-1);
	}

	//read the region of the action from stdin
	region = read_region();

	if (order == PASTE)
		nbytes = clipboard_paste(fd, region, buffer, nbytes);
	else
		nbytes = clipboard_wait(fd, region, buffer, nbytes);
	
	if (nbytes == 0)	printf("paste failed\n---\n");
	else{
		fwrite(buffer, 1, nbytes, file);
		printf("\n\n (%d bytes pasted)\n---\n", nbytes);
	}
	free(buffer);
	fclose(file);

}

size_t get_file(char *filename, char **buffer){
	/* declare a file pointer */
	FILE    *infile;
	size_t    numbytes;

	/* open an existing file for reading */
	infile = fopen(filename, "r");

	/* quit if the file does not exist */
	if(infile == NULL){
	    printf("falhou a abrir o ficheiro\n");
	    exit(-1);
	}
	 
	/* Get the number of bytes */
	fseek(infile, 0, SEEK_END);
	numbytes = ftell(infile);
	printf("numbytes %d\n", numbytes);

	/* reset the file position indicator to 
	the beginning of the file */
	fseek(infile, 0, SEEK_SET);	

	/* grab sufficient memory for the 
	buffer to hold the text */
	*buffer = (char*)malloc(numbytes);
	if (*buffer == NULL){
	 printf("falhou a alocacao do buffer\n");
	 exit(-1);
	}

	/* copy all the text into the buffer */
	size_t result = fread(*buffer, sizeof(char), numbytes, infile);
	printf("fread deu %d\n", result);

	fclose(infile);

return numbytes;
}




int main(){
//variables to hold the info from stdin
char aux[50], message[MESSAGE_SIZE];
char in[50];
//get the FIFOs file descriptor
int region;
size_t size;
int fd = clipboard_connect("./");
if(fd== -1){
	printf("Clipboard is off\n");
	exit(-1);
}

	//read the action from stdin and perform it
	while (1){
	char order;
		printf("(Enter [Q] to quit) \nIntroduce the action, copy [C], paste [P] or wait [W]:");
		if (fgets(aux, sizeof(aux), stdin) == NULL){
			perror("fgets: ");
			exit(-1);
		}
		sscanf(aux, "%c", &order);
		//perform
		if ((order == 'q') || (order == 'Q')){
			clipboard_close(fd);
			return 0;
		}
		else if ((order == 'c') || (order == 'C')){
			//read the region of the action from stdin
			region = read_region();

			printf("write file name to copy (enter [N] to write from stdin):\n");
		char *buffer; 
			if (fgets(in, 50, stdin) == NULL){
				perror("fgets: ");
				exit(-1);
			}	
			if ( (in[0] == 'n' || in[0] == 'N') && in[1] == '\n' ){
				printf("Write the message to be copied:");	
				if (fgets(message, MESSAGE_SIZE, stdin) == NULL){
					perror("fgets: ");
					exit(-1);
				}
				size = strlen(message)+1;
				buffer = message;
			}
			else{
				in[strlen(in) - 1] = '\0';
				size = get_file(in, &buffer);
			}

			size = clipboard_copy(fd, region, buffer, size);
			if (size == 0)	printf("copy failed\n---\n");
			else	printf("%d bytes copied\n---\n", size);
		}
		else if ((order == 'P') || (order == 'p')){
			paste(PASTE, fd);			
		}
		else if ((order == 'W') || (order == 'w')){
			paste(WAIT, fd);
		}
		else
			printf("Option not available\n---\n");
	}
exit(-5);
}
