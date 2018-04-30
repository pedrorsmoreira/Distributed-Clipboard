/******************************************************************************
 *
 * File Name: dictionary.h
 *	      (c) 2016 AED
 * Authors:    Pedro Moreira & André Godinho
 *******************************************************************************/

/* Prevent multiple inclusions                                      */
#ifndef DICTIONARY
#define DICTIONARY

#define MAXWORD 100

typedef struct _Dictionary Dictionary;


Dictionary* DictionaryInit();

char ***getDic_head(Dictionary *Dic);

int *getDic_wordLenght(Dictionary *Dic);

int getDic_maxLenght(Dictionary *Dic);

int getDic_BiggerLine(Dictionary *Dic);

void InsertDic_maxLenght(Dictionary *Dic, int maxLenght);

void allocDic(Dictionary *Dic);

void readDic1(FILE *fp, int *problemsInfo, Dictionary *Dic);

void readDic2 (FILE * fp, Dictionary *Dic, int *problemsInfo);

int binarySearch(char **vec, char *word, int beg, int end);

void WriteOutPutFile(FILE **fp, char ** DictionaryLine, int st[], int destination);

void freeDictionary(Dictionary *Dic);


/* End if: Protect multiple inclusions                              */
#endif