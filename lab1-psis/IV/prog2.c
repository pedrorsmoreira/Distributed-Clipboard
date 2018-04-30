#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>


int main(){
	int a;


	void *load;	
	
	void (*func_1)();	
	void (*func_2)();

	printf("What version of the functions you whant to use?\n");
	printf("\t1 - Normal    (test1)\n");
	printf("\t2 - Optimized (test2)\n");
	scanf("%d", &a);
	

	if (a == 1){
		/* load library test1 */
		load = dlopen ("/home/pedro/Desktop/lab1-psis/IV/teste1.so", RTLD_LAZY);
		
				if (!load) {
               			fprintf(stderr, "%s\n", dlerror());
               			exit(EXIT_FAILURE);
   				        }
				dlerror();



		printf("running the normal versions from \n");
	}else{
		if(a== 2){
			/* load library test2 */
				load = dlopen ("/home/pedro/Desktop/lab1-psis/IV/teste2.so", RTLD_LAZY);
		
					if (!load) {
       			        fprintf(stderr, "%s\n", dlerror());
       				        exit(EXIT_FAILURE);

   				        }
				dlerror();


			printf("running the normal versions\n");
		}else{
			printf("Not running anything\n");
			exit(-1);
		}
	}

	func_1 = (void(*) ()) dlsym(load, "func_1");
	func_2 = (void(*) ()) dlsym(load, "func_2");
		
	/* call func_1 from whichever library was loaded */
	func_1();
	/* call func_2 from whichever library was loaded */
	func_2();	
	exit(0);
	
	
}
