/******************************************************************************
 *
 * File Name: problems.c
 *	      (c) 2016 AED
 * Authors:    Pedro Moreira & André Godinho
 *
 *****************************************************************************/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"problems.h"

/* vector needs to be static to be used in function LessNum for the heap*/
int static *wt;

/******************************************************************************
 * readProblemsInit()
 *
 * Arguments: Pointer to a file
 *			  pointer to the vector problemsInfo
 * Returns: Integer maxLenght
 * Side-Effects: none
 *
 * Description: First read of the problems file. According to the file it will
 *				generate the vector problemsInfo, the integer maxLenght.
 *
 *****************************************************************************/

int readProblemsInit(FILE *fp, int *problemsInfo)
{
    int maxLenght = 0, freedom, size, dif;
	char w1[MAXWORD], w2[MAXWORD];
	
		while (fscanf(fp, "%s %s %d", w1, w2, &freedom) == 3){
			size = strlen(w1);
			dif = difChar(w1, w2, freedom);
			if (dif < 2)	continue;
			if (dif < freedom)	  freedom = dif;
			if(problemsInfo[size] < freedom)	problemsInfo[size] = freedom;
			if (maxLenght < size)	 maxLenght = size;
		}
 return maxLenght;
} 

/******************************************************************************
 * LessNum()
 *
 * Arguments: a, b - 2 Items to be compared
 * Returns: Integer with the results of the comparison
 * Side-Effects: none
 *
 * Description: Comparison function with abstract type to be passed as argument;
 *              Function is specific for "int" variables
 *
 *****************************************************************************/

int LessNum(Item a, Item b)
{
  int aa, bb;

  aa = *((int *)a);
  bb = *((int *)b);

  return (wt[aa] > wt[bb]);
}
/******************************************************************************
 * readproblems()
 *
 * Arguments: Pointer to the problems file
 *			  pointer to the exit file
 *			  triple pointer to an element of the type LinkedList
 * Returns: void
 * Side-Effects: none
 *
 * Description: The function allocates memory for vectors st, wt and to a 
 *			    heap with a size equals to BiggerLine. Furthermore, it reads 
 *				the problems file for the second time. Each time it reads a 
 *				problem it will find the path with the lowest cost by using 
 *				the Dijkstra's algorithm and it will print it out by calling 
 *				the function WriteOutPutFile. Plus, it frees the vectors st, 
 *				wt and the heap.
 *
 *****************************************************************************/

void readproblems(FILE *fp, FILE *fpOut, LinkedList ***Graphs, Dictionary *Dic)
{
 char w1[MAXWORD], w2[MAXWORD], ***head_dic = NULL;
 int freedom, size, dif, *st = NULL, *wordLenght = NULL, BiggerLine, source, destination;
 Heap *h = NULL;
	
	if (getDic_maxLenght(Dic) != 0){
	head_dic = getDic_head(Dic);
	wordLenght = getDic_wordLenght(Dic);
	BiggerLine = getDic_BiggerLine(Dic);
 
	st = (int*) malloc (BiggerLine*sizeof(int));
	if (st == NULL)		exit(0);
	wt = (int*) malloc (BiggerLine*sizeof(int));
	if (wt == NULL)		exit(0);
	h = NewHeap(BiggerLine, LessNum);
	}
	
	while (fscanf(fp, "%s %s %d", w1, w2, &freedom) == 3){
		dif = difChar(w1, w2, 1);
		if (dif < 2){
			fprintf(fpOut, "%s %d\n%s\n\n", w1, dif, w2);
			continue;
		}
		CleanHeap(h);
		size = strlen(w1);
		source = binarySearch(head_dic[size], w1, 0, wordLenght[size] - 1);
		destination = binarySearch(head_dic[size], w2, 0, wordLenght[size] - 1);
		 
		GRAPHpfs(Graphs[size], h, source, destination, (freedom*freedom), wordLenght[size], st, wt);

		 if (st[destination] == -1){
			fprintf(fpOut, "%s %d\n%s\n\n", w1, -1, w2);
			continue; 
		 }
		 fprintf(fpOut, "%s %d\n", w1, wt[destination]);
		 WriteOutPutFile(&fpOut, head_dic[size], st, destination);
		 fprintf(fpOut, "\n");
	}
  if (getDic_maxLenght(Dic) != 0){
  free(st);
  free(wt);
  FreeHeap(h);
  }
}