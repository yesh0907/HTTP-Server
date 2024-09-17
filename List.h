#ifndef LIST_H_INCLUDE_
#define LIST_H_INCLUDE_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Exported type --------------------------------------------------------------
typedef struct ListObj *List;

// Constructors-Destructors ---------------------------------------------------
List newList(void);
void freeList(List *pL);

// Access functions -----------------------------------------------------------
int ListLength(List L);
int ListIndex(List L);
void *ListFront(List L);
void *ListBack(List L);
void *ListGet(List L);
// Manipulation procedures ----------------------------------------------------
void ListClear(List L);
void ListSet(List L, void *x);
void ListMoveFront(List L);
void ListMoveBack(List L);
void ListMovePrev(List L);
void ListMoveNext(List L);
void ListPrepend(List L, void *x);
void ListAppend(List L, void *x);
void ListInsertBefore(List L, void *x);
void ListInsertAfter(List L, void *x);
void ListDeleteFront(List L);
void ListDeleteBack(List L);
void ListDelete(List L);

// Other Functions ------------------------------------------------------------
void printList(FILE *out, List L, bool isChar);

#endif
