/******************************************************************************
 *
 * File Name: heaps.c
 *	      (c) 2016 AED
 * Authors:    Pedro Moreira & André Gondinho
 *
 * DESCRIPTION
 *		implements tree auxiliary functions for any type of trees, plus some
 *		specifications for a program (wordmorph) where the heap data is
 *		filled with with integers. The specifications that can be put
 *		unfunctional just by not defining WMspecs.
 *
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "heaps.h"

/* A heap is represented by a structure: */
struct _heap {
  int (*less) (Item, Item);     /* this field is a function pointer to elements in the heap. */
  int n_elements;               /* # elements in heap. */
  int size;                     /* max size of the heap. */
  Item *heapdata;               /* An array of Items. */
#ifdef WMspecs
  int *elements_pos;			/* An array holding the elements position in the heapdata*/
#endif
};

/******************************************************************************
 * NewHeap()
 *
 * Arguments: size - heap size
 *            less - item comparison function
 * Returns: Heap
 * Side-Effects: none
 *
 * Description: allocates space for a new heap of specified size
 *
 *****************************************************************************/

Heap *NewHeap(int size, int (*less) (Item, Item))
{
  Heap *h;

  h = (Heap *) malloc(sizeof(Heap));
  if (h == ((Heap *) NULL))		exit(0);

  h->n_elements = 0;
  h->less = less;
  h->size = size;
  h->heapdata = (Item *) malloc(size * sizeof(Item));
  if (h->heapdata == ((Item *) NULL))	exit(0);
#ifdef WMspecs
  h->elements_pos = (int *) malloc(size * sizeof(int));
  if (h->heapdata == NULL)	exit(0);
#endif
  return (h);
}

#ifdef WMspecs
int *getHeapElementes_pos(Heap *h){
	return h->elements_pos;
}
#endif

/******************************************************************************
 * FixUp()
 *
 * Arguments: h - pointer to heap structure
 *            k - index of element to fixup
 * Returns:
 * Side-Effects: none
 *
 * Description: Performs fixup from index
 *
 *****************************************************************************/

void FixUp(Heap * h, int k)
{
  int aux;
  Item t;
  while ((k > 0) && (h->less) ((h->heapdata)[(k - 1) / 2], (h->heapdata)[k])) {
    t = (h->heapdata)[k];
    (h->heapdata)[k] = (h->heapdata)[(k - 1) / 2];
    (h->heapdata)[(k - 1) / 2] = t;
#ifdef WMspecs
	aux = (h->elements_pos)[*((int*)((h->heapdata)[k]))];
	(h->elements_pos)[*((int*)((h->heapdata)[k]))] = (h->elements_pos)[*((int*)((h->heapdata)[(k - 1) / 2]))];
	(h->elements_pos)[*((int*)((h->heapdata)[(k - 1) / 2]))] = aux;
#endif
    k = (k - 1) / 2;
  }
  return;
}

/******************************************************************************
 * FixDown()
 *
 * Arguments: h - pointer to heap structure
 *            k - index of element to fixdown
 * Returns:
 * Side-Effects: none
 *
 * Description: performs fixdown from index
 *
 *****************************************************************************/

void FixDown(Heap * h, int k)
{
  int j, aux;
  Item t;

  while ((2 * k + 1) < h->n_elements) {
    j = 2 * k + 1;
    if (((j + 1) < h->n_elements) &&
        (h->less) (h->heapdata[j], h->heapdata[j + 1])) {
      /* second offspring is greater */
      j++;
    }
    if (!(h->less) (h->heapdata[k], h->heapdata[j])) {
      /* Elements are in correct order. */
      break;
    }

    /* the 2 elements are not correctly sorted, it is
       necessary to exchange them */
    t = (h->heapdata)[k];
    (h->heapdata)[k] = (h->heapdata)[j];
    (h->heapdata)[j] = t;
#ifdef WMspecs
	aux = (h->elements_pos)[*((int*)((h->heapdata)[k]))];
	(h->elements_pos)[*((int*)((h->heapdata)[k]))] = (h->elements_pos)[*((int*)((h->heapdata)[j]))];
	(h->elements_pos)[*((int*)((h->heapdata)[j]))] = aux;
#endif	
    k = j;
  }

  return;
}

/******************************************************************************
 * RemoveMax()
 *
 * Arguments: h - pointer to heap
 * Returns: Item
 * Side-Effects: none
 *
 * Description: exchange first and last element of heap, remove last element
 *              (max element) from heap and maintain heap order by performing
 *              a fixdown of the first element
 *
 *****************************************************************************/

Item RemoveMax(Heap * h)
{
  int aux;
  Item t;

    t = (h->heapdata)[0];
    (h->heapdata)[0] = (h->heapdata)[-- (h->n_elements)];
    (h->heapdata)[h->n_elements] = t;
#ifdef WMspecs	
	aux = (h->elements_pos)[*((int*)((h->heapdata)[0]))];
	(h->elements_pos)[*((int*)((h->heapdata)[0]))] = (h->elements_pos)[*((int*)((h->heapdata)[h->n_elements]))];
	(h->elements_pos)[*((int*)((h->heapdata)[h->n_elements]))] = aux;
#endif	
    FixDown(h, 0);
    return t;
}

/******************************************************************************
 * Direct_Insert()
 *
 * Arguments: h - pointer to heap
 *            element - pointer to new element
 * Returns: vopid
 * Side-Effects: none
 *
 * Description: add element at the end of heap but does not perform fixup
 *
 *****************************************************************************/

int Direct_Insert(Heap * h, Item element)
{
  if (h->n_elements == h->size)		return 0;

  h->heapdata[h->n_elements ++] = element;

  return 1;
}

/******************************************************************************
 * CleanHeap()
 *
 * Arguments: Heap
 * Returns: none
 * Side-Effects: None
 *
 * Description: cleans an heap
 *
 *****************************************************************************/

void CleanHeap(Heap * h){
	for (; h->n_elements > 0; free(h->heapdata[-- h->n_elements]));
}

/******************************************************************************
 * FreeHeap()
 *
 * Arguments: h - pointer to heap
 *
 * Returns: void
 * Side-Effects: none
 *
 * Description: frees all the space allocated, held by the heap structure
 * elements and frees the heap itself
 *
 *****************************************************************************/

void FreeHeap(Heap * h)
{
   for (; h->n_elements > 0; free(h->heapdata[-- h->n_elements])); /*Clean Heap*/
   free(h->heapdata);
#ifdef WMspecs
   free(h->elements_pos);
#endif
   free(h);
}