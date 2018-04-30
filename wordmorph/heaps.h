/******************************************************************************
 *
 * File Name: heaps.c
 *	      (c) 2016 AED
 * Authors:    Pedro Moreira & André Godinho
 *
 *******************************************************************************/

/* Prevent multiple inclusions                                      */
#ifndef GenerealHeapFuncs
#define GeneralHeapFuncs

/* Include Header File for the definition of the Item type        */
#include "defs.h"

/* specicifations related with this wordmorph program */
#define WMspecs

typedef struct _heap Heap;


Heap *NewHeap(int size, int (*less) (Item, Item));

#ifdef WMspecs
int *getHeapElementes_pos(Heap *h);
#endif

void FixUp(Heap * h, int k);

Item RemoveMax(Heap * h);

int Direct_Insert(Heap * h, Item element);

void CleanHeap(Heap * h);

void FreeHeap(Heap * h);


/* End if: Protect multiple inclusions                              */
#endif