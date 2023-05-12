#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "heap.h"

typedef struct nodo
{
  void *data;
  int priority;
} heapElem;

typedef struct Heap
{
  heapElem *heapArray;
  int size;
  int capac;
} Heap;

Heap *createHeap()
{
  Heap *h = malloc(sizeof(Heap));

  h -> heapArray = malloc(3 * sizeof(Heap));
  h -> size = 0;
  h -> capac = 3;

  return h;
}

void *heap_top(Heap* pq)
{
  if(!pq -> size) return NULL;

  return pq -> heapArray[0].data;
}

void *heap_top_priority(Heap* pq)
{
  if(!pq -> size) return NULL;

  return pq -> heapArray[0].priority;
}

void heap_push(Heap *pq, void *data, int priority)
{
  int i = pq -> size;
  
  if(pq -> size == pq -> capac)
  {
    pq -> capac = pq -> capac * 2 + 1;
    pq -> heapArray = realloc(pq -> heapArray, pq -> capac * sizeof(heapElem));
  }

  while (priority > (pq -> heapArray[(i - 1) / 2].priority) && i)
  {
    pq -> heapArray[i] = pq -> heapArray[(i - 1) / 2];
    i = (i - 1) / 2;
  }

  pq -> heapArray[i].data = data;
  pq -> heapArray[i].priority = priority;
  pq -> size++;
}

void heap_pop(Heap *pq)
{
  int hijoMenor = 1, hijoMayor = 2, pos = 0;
  heapElem auxiliar;
  
  pq -> size--;
  pq -> heapArray[0] = pq -> heapArray[pq -> size];
  
  while (hijoMenor < pq -> size)
  {
    int nodoHijo = hijoMenor;
    
    if (hijoMenor < pq -> size && pq -> heapArray[hijoMayor].priority > pq -> heapArray[hijoMenor].priority) nodoHijo = hijoMayor;
    
    if(pq -> heapArray[pos].priority >= pq -> heapArray[nodoHijo].priority) break;
    
    auxiliar = pq -> heapArray[pos];
    pq -> heapArray[pos] = pq -> heapArray[nodoHijo];
    pq -> heapArray[nodoHijo] = auxiliar;
    pos = nodoHijo;
    hijoMenor = hijoMenor * 2 + 1;
    hijoMayor = hijoMayor * 2 + 2;
  }
}