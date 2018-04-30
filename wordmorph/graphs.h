/******************************************************************************
 *
 * File Name: graphs.h
 *	      (c) 2016 AED
 * Authors:    Pedro Moreira & André Godinho
 *
 *******************************************************************************/


/* Prevent multiple inclusions                                      */
#ifndef GRAPHS
#define GRAPHS

#include "defs.h"
#include"dictionary.h"
#include"heaps.h"

typedef struct LinkedListStruct LinkedList;

typedef struct _node node;


int difChar(char *w1, char *w2, int maxDif);

LinkedList ***AllocGraphs(Dictionary *Dic, int *problemsInfo);

void FillInGraphs(LinkedList ***Graphs, Dictionary *Dic, int *problemsInfo);

void GRAPHpfs(LinkedList **Graph, Heap *h, int source, int destination, int maxCost, int nvertecis, int st[], int wt[]);

void freeGraphs(LinkedList ***Graphs, int *wordLenght, int maxLenght);


/* End if: Protect multiple inclusions                              */
#endif