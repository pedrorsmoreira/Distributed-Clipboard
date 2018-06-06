#include "clipboard.h"
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
	int fd = clipboard_connect("./");
	if(fd < 0){
		printf("failed to connect\n");
		exit(-1);
	}

	char c[2];
	//to be read as a string
	c[1] = '\0';

	while(1) {
		//ascii valid careceter
		c[0] = rand()%(122-65)+65;

		if (clipboard_copy(fd, 0, c, 2) < 1)
			printf("failed copy\n");
	}

 clipboard_close(fd);
 exit(0);
}
