/******************************************************************************
 *
 * File Name: AsortingFuncs.c
 *	      (c) 2016 AED
 * Authors:    Pedro Moreira & André Godinho
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"AsortingFuncs.h"

/******************************************************************************
 * apartition()
 *
 * Arguments: Pointer to a vector of strings
 *			  Integer with the first position of the vector
 *			  Integer with the last position of the vector
 *
 * Returns: Integer 
 * Side-Effects: none
 *
 * Description: 
 *
 *****************************************************************************/
int apartition (char **vec, int l, int r)
{
	char *straux, *word = vec[r];
	int i = l-1 ; 
	int j = r;

	for (;;)
	{		
		while (strcmp(vec[++i],word) <0);
		while (strcmp(word, vec[--j])<0)	if (j == l) break;

		if (i >= j) break;
		straux = vec[i];
		vec[i] = vec[j];
		vec[j] = straux;
	}
	straux = vec[i];
	vec[i] = vec[r];
	vec[r] = straux;
	return i;
}
/******************************************************************************
 * quicksort()
 *
 * Arguments: pointer to a vector of strings 
 *			  integer which represents the first position of the vector 
 *			  integer that represents the last position of the vector
 * Returns: void
 * Side-Effects: none
 *
 * Description: Recursive function that has a divide to conquer idea, more 
 *				specifically, it divides the vector in two and 
 *				organizes alphabetically each one. Once organized, the function
 *				returns to the previous call until it returns to the first call.
 *
 *****************************************************************************/
void quicksort (char** vec, int l, int r)
{
	int i=0;
	if (r <= l) return; 

	i = apartition (vec, l, r);
	quicksort (vec, l, i-1);
	quicksort(vec, i+1, r);
}
/******************************************************************************
 * AsortDic()
 *
 * Arguments: Pointer to a struct of the type Dictionary.
 * Returns: void
 * Side-Effects: none
 *
 * Description: Calls the function quicksort which will organize alphabetically one vector of 
 *				characters with the same lenght. By doing a loop, it will organize
 *				the whole dictionary.
 *
 *****************************************************************************/
void AsortDic(Dictionary *Dic)
{
    int i, maxLenght = getDic_maxLenght(Dic), *wordLenght = getDic_wordLenght(Dic);

	for(i = 1; i <= maxLenght; i++){
			if(wordLenght[i] > 1)
			quicksort(getDic_head(Dic)[i], 0, wordLenght[i] - 1);
	}
}