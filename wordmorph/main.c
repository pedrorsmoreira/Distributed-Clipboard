/******************************************************************************
 *
 * File Name: main.c
 *	      (c) 2016 AED
 * Authors:    Pedro Moreira & André Godinho
 *
 * Description: main program
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"problems.h"
#include"AsortingFuncs.h"
#include"check_args.h"


int main (int argc, char *argv[])
{
	Dictionary *Dic;
	LinkedList ***Graphs = NULL;
	char *fileNameOut;
	int problemsInfo[MAXWORD] = {0};
	FILE *fpDic, *fpProb, *fpOut;
	
	if (argc < 3)		exit(0);
	fileNameOut = check_args(argv[1], argv[2]);
	
	Dic = DictionaryInit();
	
	fpProb = fopen(argv[2], "r");
		if (fpProb == NULL)		exit(0);
	InsertDic_maxLenght(Dic, readProblemsInit(fpProb, problemsInfo));

	if (getDic_maxLenght(Dic) != 0){
	fpDic = fopen(argv[1], "r");
		if (fpDic == NULL)	exit(0);
	readDic1(fpDic, problemsInfo, Dic);
	allocDic(Dic);
	rewind(fpDic);
	readDic2(fpDic, Dic, problemsInfo);
	fclose(fpDic);
	AsortDic(Dic);

	Graphs = AllocGraphs(Dic, problemsInfo);
	FillInGraphs(Graphs, Dic, problemsInfo);
	}

	fpOut = fopen(fileNameOut, "w");
	  if (fpOut == NULL)		exit(0);
	rewind(fpProb);
	readproblems(fpProb, fpOut, Graphs, Dic);
	fclose(fpOut);
	fclose(fpProb);
	
	free(fileNameOut);
	if (getDic_maxLenght(Dic) != 0){
	freeGraphs(Graphs, getDic_wordLenght(Dic), getDic_maxLenght(Dic));
	freeDictionary(Dic);
	}
	return 0;
}