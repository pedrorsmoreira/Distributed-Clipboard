/******************************************************************************
 *
 * File Name: dictionary.c
 *	      (c) 2016 AED
 * Authors:    Pedro Moreira & André Godinho
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"dictionary.h"


/*******************************************************************
 *  Data Type: _Dictionary
 *
 *  Description: Structure with:
 *                 1) pointer holding a matrix of strings
 *                 2) pointer holding the number of strings
					  of each line of the matrix
 *				   3) size of the biggest strings
 *				   4) maximum number of strings of the same size
 *******************************************************************/
struct _Dictionary{
  char ***head;
  int *wordLenght;
  int maxLenght;
  int BiggerLine;
};

/******************************************************************************
 * DictionaryInit()
 *
 * Arguments: none
 * Returns: Pointer to Dictionary struct 
 * Side-Effects: none
 *
 * Description: allocates memory for one struct of the type Dictionary
 *				Initializes all variables of the struct
 *
 *****************************************************************************/
Dictionary* DictionaryInit(){
  Dictionary *Dic = (Dictionary*) malloc(sizeof(Dictionary));
	  if (Dic == NULL)		exit(0);
	Dic->head = NULL;
	Dic->wordLenght = NULL;
	Dic->maxLenght = 0;
	Dic->BiggerLine = 0;
return Dic;
}

/******************************************************************************
 * getDic_head()
 *
 * Arguments: Pointer to a Dictionary struct
 * Returns: Triple pointer for a string
 * Side-Effects: none
 *
 * Description: Acess to the matrix which stores the dicciconary words
 *
 *****************************************************************************/
char ***getDic_head(Dictionary *Dic){
	return Dic->head;
}

/******************************************************************************
 * getDic_wordLenght()
 *
 * Arguments: Pointer to a Dictionary struct
 * Returns: Pointer to the vector wordlenght.
 * Side-Effects: none
 *
 * Description: Acess to the vector wordlenght
 *
 *****************************************************************************/
int *getDic_wordLenght(Dictionary *Dic){
	return Dic->wordLenght;
}

/******************************************************************************
 * getDic_maxLenght()
 *
 * Arguments: Pointer to a Dictionary struct
 * Returns: Integer maxlenght
 * Side-Effects: none
 *
 * Description: Acess to the integer maxLenght
 *
 *****************************************************************************/
int getDic_maxLenght(Dictionary *Dic){
	return Dic->maxLenght;
}

/******************************************************************************
 * getDic_BiggerLinet()
 *
 * Arguments: Pointer to a Dictionary struct
 * Returns: Integer BiggerLine
 * Side-Effects: none
 *
 * Description: Acess to the integer BiggerLine
 *
 *****************************************************************************/
int getDic_BiggerLine(Dictionary *Dic){
	return Dic->BiggerLine;
}

/******************************************************************************
 * getDic_BiggerLinet()
 *
 * Arguments: Pointer to a Dictionary struct
 			  integer maxLenght
 * Returns: void
 * Side-Effects: none
 *
 * Description: Insert a new maxLenght
 *
 *****************************************************************************/
void InsertDic_maxLenght(Dictionary *Dic, int maxLenght){
	 Dic->maxLenght = maxLenght;
}

/******************************************************************************
 * allocDic()
 *
 * Arguments: Pointer to a Dictionary struct
 * Returns: void
 * Side-Effects: none
 *
 * Description: Allocates memory to the tridimensional matrix, it has in mind the 
 				adequate space to have all the words needed for the all the program
 *
 *****************************************************************************/
void allocDic(Dictionary *Dic)
{
	int i, j;
	
	Dic->head = (char***) calloc(Dic->maxLenght + 1,sizeof(char**));
	if (Dic->head == NULL)		exit(0);
	
		for(i = 1; i <= Dic->maxLenght; i++){
		if (Dic->wordLenght[i] == 0) continue;
		Dic->head[i] = (char**) malloc(Dic->wordLenght[i]*sizeof(char*));
		if (Dic->head[i] == NULL)		exit(0);
		
			for (j = 0; j < Dic->wordLenght[i]; j++){
				Dic->head[i][j] = (char*) malloc((i + 1)*sizeof(char));
				if(Dic->head[i][j] == NULL)		exit(0);
			}
		}
}

/******************************************************************************
 * readDic1c()
 *
 * Arguments: Pointer to the dictionary file
 			  vector of integers problemsInfo
 			  Pointer to a Dictionary struct
 * Returns: void
 * Side-Effects: none
 *
 * Description: First read of the Dictionary file, the function generates de wordlenght vector
 				and the BiggerLine integer
 *
 *****************************************************************************/
void readDic1(FILE *fp, int *problemsInfo, Dictionary *Dic)
{	
  char straux[MAXWORD];
  int  size;
	Dic->wordLenght = (int *) calloc(Dic->maxLenght + 1, sizeof(int));
	  if (Dic->wordLenght == NULL)	exit(0);
	
		while (fscanf (fp, "%s", straux) == 1){
			size = strlen(straux);
			if (problemsInfo[size] == 0)		continue;
			if (++ Dic->wordLenght[size] > Dic->BiggerLine)
			Dic->BiggerLine = Dic->wordLenght[size];
		}
}

/******************************************************************************
 * readDic2()
 *
 * Arguments: Pointer to the dictionary file 
 			  Vector of integers problemsInfo 
 			  Pointer to a Dictionary struct
 * Returns: void
 * Side-Effects: none
 *
 * Description: It fills in the matrix with the words of the Dictionary file
 *
 *****************************************************************************/
void readDic2 (FILE * fp, Dictionary *Dic, int *problemsInfo)
{
    int lenght, aux_lenght[MAXWORD] = {0};
    char straux[MAXWORD];
	
	while (fscanf(fp, "%s", straux) == 1){
		lenght = strlen(straux);
		if (problemsInfo[lenght] == 0)	continue;
		strcpy(Dic->head[lenght][aux_lenght[lenght] ++], straux);
	}
}

/******************************************************************************
 * binarySearch()
 *
 * Arguments: Pointer to a vector of pointers, which each one points to a string 
 		      string, which is the word that is suposed to be found
 		      an integer which has the first position of the vector
 		      an integer which has the last position
 * Returns: Integer that is the position of the word in the vector vec
 * Side-Effects: If it does not find the word on the vector, it returs -1.
 *
 * Description: Finds the position of a word on the matrix, more specifically 
 				the position in the wordlenght vector and returns it. 
 *
 *****************************************************************************/
int binarySearch(char **vec, char *word, int beg, int end){
	while (end >= beg){
      int m = (beg+end)/2;
      if (strcmp(word, vec[m]) == 0) return m;
      if (strcmp(word, vec[m]) < 0) end = m-1;
      else beg = m+1;
	}
   return (-1);
}

/******************************************************************************
 * WriteOutPutFile()
 *
 * Arguments: double pointer to a string (word)
 			  vector of integers st, 
 			  and an integer which is the position of the destination vertex
 * Returns: void
 * Side-Effects: none
 *
 * Description: The pointer to a file enters by reference and the path 
 				 is printed on the file. It's a recursive function, the function
 				 prints in a backwards motion
 *
 *****************************************************************************/
void WriteOutPutFile(FILE **fp, char ** DictionaryLine, int st[], int destination)
{
	int prev = st[destination];
	if(prev != -1){
		WriteOutPutFile(fp, DictionaryLine, st, prev);
	fprintf(*fp, "%s\n", DictionaryLine[destination]);
	}
}

/******************************************************************************
 * freeDictionary()
 *
 * Arguments: Pointer to a Dictionary struct
 * Returns: void
 * Side-Effects: none
 *
 * Description: It frees the memory allocated to the matrix which stores all the
 				words (on the function allocDic, DictionaryInit and readDic1)
 *
 *****************************************************************************/
void freeDictionary(Dictionary *Dic)
{
	int i, j;
	 for (i = 1; i <= Dic->maxLenght; i ++){
		  for (j = 0; j < Dic->wordLenght[i]; j++)
			    free(Dic->head[i][j]);
		free(Dic->head[i]);
	 }
	free(Dic->head);
	free (Dic->wordLenght);
	free(Dic);
}