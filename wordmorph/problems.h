/******************************************************************************
 *
 * File Name: problems.h
 *	      (c) 2016 AED
 * Authors:    Pedro Moreira & André Godinho
 *
 *****************************************************************************/

/* Prevent multiple inclusions                                      */
#ifndef ReadProblemsFile
#define ReadProblemsFile

#include"graphs.h"


int readProblemsInit(FILE *fp, int *problemsInfo);

void readproblems(FILE *fp, FILE *fpOut, LinkedList ***Graphs, Dictionary *Dic);


/* End if: Protect multiple inclusions                              */
#endif