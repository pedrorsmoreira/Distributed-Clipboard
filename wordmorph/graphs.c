/******************************************************************************
 *
 * File Name: graphs.c
 *	      (c) 2016 AED
 * Authors:    Pedro Moreira & André Godinho
 *
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include"graphs.h"



/*******************************************************************
 *  Data Type: LinkedListStruct
 *
 *  Description: Structure with:
 *                 1) Pointer to the item of the linked list node
 *                 2) Pointer to next node of the linked list.
 *******************************************************************/
struct LinkedListStruct{
  Item this;
  LinkedList * next;
};

/********************************************************************
 *  Data Type: _node
 *	 	(suposed to be used in a graph adjacencies linked list)
 *	
 *	Description: Structure with:
 *				   1) Integer holding the vertex
 *				   2) Integer holding the cost.
 *
 ********************************************************************/
struct _node{
  int vertex;
  int cost;
};

/******************************************************************************
 * difChar()
 *
 * Arguments: w1 - pointer to 1st string
 *            w1 - pointer to 2nd string
 *			  maxDif - integer holding the maximum number of different
 *					   characters between w1 and w2 allowed
 *
 * Returns: number of different characters between w1 and w2
 * Side-Effects: if the number excedes
 * 				 maxDif always returns (maxDif + 1)
 *
 *****************************************************************************/
int difChar(char *w1, char *w2, int maxDif)
{
  int i, dif = 0;
	for(i = 0; w1[i]!= '\0'; i++){
		if(w1[i] != w2[i]){
			dif++;
			if (dif > maxDif)	return dif;
		}
	}
  return dif;
}

/******************************************************************************
 * AllocGraphs()
 *
 * Arguments: Dic - pointer to Dictionary struct
 *            problemsInfo - pointer to a vector of integers
 *
 * Returns: triple pointer for an elemente of the type LinkedList 
 *										(vector of graphs)
 *
 * Side-Effects: if any allocation fails exits with exit(0)
 *
 * Description: alocates memory for a vector with adjacencies lists
 *
 *****************************************************************************/
LinkedList ***AllocGraphs(Dictionary *Dic, int *problemsInfo)
{
 int i, maxLenght = getDic_maxLenght(Dic);

 LinkedList ***Graphs = (LinkedList ***) calloc(maxLenght + 1, sizeof(LinkedList**));
	if (Graphs == NULL)		exit(0);

 for (i = 1; i <= maxLenght; i++){
	 if (problemsInfo[i] == 0)	continue;
	 Graphs[i] = (LinkedList**) calloc(getDic_wordLenght(Dic)[i], sizeof(LinkedList*));
		if (Graphs[i] == NULL)	exit(0);
	}
 return Graphs;
}

/******************************************************************************
 * new_node()
 *
 * Arguments: v - integer holding the number of vertex to insert in the node
 *            c - integer holding the number of the cost to insert in the node
 *
 * Returns: pointer to the new node generated
 * Side-Effects:if alocation fails exits with exit(0)
 *
 * Description: alocates memory for a node, fills it and returns it.
 *
 *****************************************************************************/
node *new_node(int v, int c)
{
 node *x = (node *) malloc(sizeof(node));
	if (x == NULL)	exit(0);
 
 x->vertex = v;
 x->cost = c;
 return x;
}

/*****************************************************
 *  Function:
 *    insertUnsortedLinkedList
 *
 *  Description:
 *    Creates a new linked list node.
 *
 *  Arguments:
 *    Item to associate to the new node:
 *      Item this
 *    Pointer to the next node:
 *      (LinkedList *) next
 *
 *  Return value:
 *    Returns the pointer to the node.
 *******************************************************/
LinkedList * insertUnsortedLinkedList(LinkedList * next, Item this)
{
  LinkedList * new;

  /* Memory allocation                                            */
  new = (LinkedList *) malloc(sizeof(LinkedList));

  /* Check memory allocation errors                               */
  if(new == NULL)
    return NULL;

  /* Initialize new node                                          */
  new->this = this;
  new->next = next;

  return new;
}

/******************************************************************************
 * GRAPHinsertE()
 *
 * Arguments: graph - vector of pointers to linked lists
 *            v1 - first vertex
 *			  v2 - second vertex linked to the first
 *			  dif - different characters between the words that v1 and v2
 *					simbolize
 *
 *
 * Returns: void
 * Side-Effects: none
 *
 * Description: inserts a new edge with vertecis v1 and v2 and the
 *				cost of the link is the square number of argument dif.
 *
 *****************************************************************************/
void GRAPHinsertE(LinkedList **graph, int v1, int v2, int dif){
   int cost = dif*dif;
 graph[v1] = insertUnsortedLinkedList(graph[v1], new_node(v2, cost));
 graph[v2] = insertUnsortedLinkedList(graph[v2], new_node(v1, cost));
}

/******************************************************************************
 * createGraph()
 *
 * Arguments: Graph - vector of pointers to linked lists
 *            DicLine - vector of strings
 *			  LineSize - number of strings of DicLine
 *			  maxDif - maximum number of different characters
 *					   to test the alowence of graph link
 *
 * Returns: void
 * Side-Effects: none
 *
 * Description: tests the alowence of graph link between every single
 *				combination of the strings held in DicLine and when the
 *				test is passed inserts in the graph a new edge where the
 *				vertecis are integers that index the position of the strings
 *				in DicLine.
 *
 *****************************************************************************/
void createGraph(LinkedList **Graph, char **DicLine, int LineSize, int maxDif)
{
	int j, k, dif;
	for(j = 0; j < LineSize; j++){
		 for(k = j+1; k < LineSize; k++){
			 dif = difChar(DicLine[j], DicLine[k], maxDif);
			 if(dif > maxDif)	continue;
		 GRAPHinsertE(Graph, j, k, dif);
		}
	}
}

/******************************************************************************
 * FillInGraphs()
 *
 * Arguments: Graphs - vector with all the graphs
 *				(type of graph is **LinkedList)
 *            Dic - pointer to a struct of type Dictionary
 *			  problemsInfo - vector holding the condition to each graph link
 *			     if it has zero, theres no graph to create in that position
 *
 * Returns: void
 * Side-Effects: none
 *
 * Description: According to the problems file, more specifically, to the 
				information stored in the problemsInfo, the function
				will create only the graphs ne to resolve the 
				problems of the file.
 *
 *****************************************************************************/
void FillInGraphs(LinkedList ***Graphs, Dictionary *Dic, int *problemsInfo)
{
	int i, maxLenght = getDic_maxLenght(Dic);
	for(i = 1; i <= maxLenght; i++){
		if(problemsInfo[i] == 0)	continue;
	   createGraph(Graphs[i], getDic_head(Dic)[i], getDic_wordLenght(Dic)[i], problemsInfo[i]);
	}
}

/******************************************************************************
 * GRAPHpfs()
 *
 * Arguments: Graph - vector of pointers to linked lists
 *            h - pointer to a heap.
 *			  destination - destination vertex of the graph
 *		  	  maxCost - max cost of edges alowed for the path
 *		  	  nvertices - number of total vertices of the graph
 *		  	  st - vector that will dictate the path
 *		  	  wt - vector that holds in each position the full cost
 *				from the source to each vertex.
 *
 *
 * Returns: void
 * Side-Effects: none
 *
 * Description: calculates the cheapest path between the source and the
 *				destination vertecis of the graph and stores it in st
 *
 *****************************************************************************/
#define maxWT 10000
#define P (wt[*v] + ((node*)(t->this))->cost)

void GRAPHpfs(LinkedList **Graph, Heap *h, int source, int destination, int maxCost, int nvertecis, int st[], int wt[])
{
int i, *v, w, *vertecisPos = getHeapElementes_pos(h);
LinkedList *t;

	for (i = 0; i < nvertecis; i++) {
		v = (int *)malloc(sizeof(int));
		if (v == NULL)		exit(0);
		*v = i;
	 st[i] = -1;
	 wt[i] = maxWT;
	 Direct_Insert(h, (Item) v);
	 vertecisPos[i] = i;
	}

 wt[source] = 0;
 FixUp(h, source);

	for(v = RemoveMax(h); *v != destination && wt[*v] != maxWT; v = RemoveMax(h)){
		for (t = Graph[*v]; t != NULL; t = t->next)
			if (((node*)(t->this))->cost <= maxCost && wt[w = ((node*)(t->this))->vertex] > P){
			wt[w] = P;
			FixUp(h, vertecisPos[w]);
			st[w] = *v;
			}
	 free (v);
	}
 free(v);
}

/******************************************************************
 *  Function:
 *    freeLinkedList
 *
 *  Description:
 *    Frees the memory allocated to a linked list.
 *
 *  Arguments:
 *    Pointer to the first element of a linked list:
 *      (LinkedList *) first
 *    Function to free the memory allocated to the items:
 *      void freeItem(Item)
 *
 *  Return value:
 *    None
 *******************************************************************/
void freeLinkedList(LinkedList * first, void (* freeItemFnt)(Item))
{
  LinkedList * next;
  LinkedList * aux;

  /* Cycle from the first to the last element                     */
  for(aux = first; aux != NULL; aux = next)
    {
      /* Keep trace of the next node                                */
      next = aux->next;

      /* Free current item                                          */
      freeItemFnt(aux->this);

      /* Free current node                                          */
      free(aux);
    }

  return;
}

/******************************************************************
 *  Function:
 *    free_node
 *
 *  Description:
 *    Frees the memory allocated to a node
 *
 *  Arguments:
 *    a struct of type Item
 *
 *  Return value:
 *    None
 *******************************************************************/
void free_node(Item node){
	free(node);
}

/******************************************************************
 *  Function:
 *    freeGraph
 *
 *  Description:
 *    Frees the memory allocated to a graph with the words of one size
 *
 *  Arguments:
 *    A triple pointer to an element of the type LinkedList 
 	  Integer which is equal to the amount of words that exist with that size
 	  according to the integer that is stored in wordlenght
 *
 *  Return value:
 *    None
 *******************************************************************/
void freeGraph(LinkedList **Graph, int N)
{
 int a;
	for (a = 0; a < N; a++)
		freeLinkedList(Graph[a], free_node);
	free(Graph);
}

/******************************************************************
 *  Function:
 *    freeGraphs
 *
 *  Description:
 *    Frees the memory allocated to the all graphs, plus the memory allocated
 	  to the struct itself.
 *
 *  Arguments:
 *    A triple pointer to an element of the type LinkedList 
 	  Pointer to the vector of integers wordlenght
 	  Integer max lenght
 *
 *  Return value:
 *    None
 *******************************************************************/
void freeGraphs(LinkedList ***Graphs, int *wordLenght, int maxLenght)
{
	int i;
	 for (i = 1; i <= maxLenght; i++)
		freeGraph(Graphs[i], wordLenght[i]);
	free(Graphs);
}