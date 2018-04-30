/******************************************************************************
 *
 * File Name: cheack_args.c
 *	      (c) 2016 AED
 * Authors:    Pedro Moreira & André Godinho
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"check_args.h"


/******************************************************************************
 * split_pal()
 *
 * Arguments: pal - string with extension ".pal"
 *			  name - string passed by reference
 *			  extension - string passed by reference
 *
 * Returns: void
 * Side-Effects: if an allocation fails exits with exit(0)
 *
 * Description: splits the string pal in "<beforepal>." and "pal"
 *				and stores it respectively in the string name and
 *				the string extension.
 *
 *****************************************************************************/
void split_pal(char *pal, char** name, char **extension)
{
	*name = (char*) malloc((strlen(pal)+2)*sizeof(char));
       if (*name == NULL)	exit(0);
    *extension = (char*) malloc(4*sizeof(char));
	   if (extension == NULL)	exit(0);
	sscanf(pal, "%[^.].%s", *name, *extension);
}

/******************************************************************************
 * split_pal()
 *
 * Arguments: dic - string with extension ".dic"
 *			  pal - string with extension ".pal"
 *
 * Returns: string pal but with the extension ".path"
 * Side-Effects: none
 *
 * Description: if any of the arguments comes with a wrong extension,
 *				exits the program with exit (0)
 *
 *****************************************************************************/
char *check_args (char *dic, char *pal)
{
	char *name, *extension;

	dic += strlen(dic) - 3;
	if ( strcmp(dic, "dic") != 0)	exit(0);

	split_pal(pal, &name, &extension);

	if(strcmp(extension, "pal") != 0){
		free(name);
		free(extension);
	}
	free(extension);
	strcat(name, ".path");

  return name;
}