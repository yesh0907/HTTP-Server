#include "List.h"

// structs --------------------------------------------------------------------

// private Node type
typedef struct NodeObj *Node;

// private NodeObj type
typedef struct NodeObj {
    void *data;
    Node next;
    Node prev;
} NodeObj;

// private ListObj type
typedef struct ListObj {
    Node front;
    Node back;
    Node cursor;
    int length;
    int index;
} ListObj;

// Helper Function Declaration ------------------------------------------------
bool isEmpty(List L);

// Constructors-Destructors ---------------------------------------------------

// newNode()
// Returns reference to new Node object. Initializes next and data fields.
Node newNode(void *data) {
    Node N = malloc(sizeof(NodeObj));
    N->data = data;
    N->next = NULL;
    N->prev = NULL;
    return (N);
}

// freeNode()
// Frees heap memory pointed to by *pN, sets *pN to NULL.
void freeNode(Node *pN) {
    if (pN != NULL && *pN != NULL) {
        free(*pN);
        *pN = NULL;
    }
}

// Creates and returns a new empty List.
List newList() {
    List L;
    L = malloc(sizeof(ListObj));
    L->front = L->back = L->cursor = NULL;
    L->length = 0;
    L->index = -1;
    return L;
}

// Frees all heap memory associated with *pL, and sets *pL to NULL.
void freeList(List *pL) {
    if (pL != NULL && *pL != NULL) {
        while (!isEmpty(*pL)) {
            ListDeleteFront(*pL);
        }
        free(*pL);
        *pL = NULL;
    }
}

// Access functions -----------------------------------------------------------

// Returns the number of elements in L.
int ListLength(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListLength() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    return L->length;
}

// Returns index of cursor element if defined, -1 otherwise.
int ListIndex(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListIndex() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    return L->index;
}

// Returns front element of L.
// Pre: ListLength()>0
void *ListFront(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListFront() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling ListFront() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    return L->front->data;
}

// Returns back element of L.
// Pre: ListLength()>0
void *ListBack(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListBack() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling ListBack() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    return L->back->data;
}

// Returns cursor element of L.
// Pre: ListLength()>0, ListIndex()>=0
void *ListGet(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListGet() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling ListGet() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (ListIndex(L) < 0) {
        fprintf(stderr, "List Error: calling ListGet() on List index < 0\n");
        exit(EXIT_FAILURE);
    }
    return L->cursor->data;
}

// Manipulation procedures ----------------------------------------------------

// Resets L to its original empty state.
void ListClear(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListClear() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }

    while (!isEmpty(L)) {
        ListDeleteFront(L);
    }

    L->front = L->back = L->cursor = NULL;
    L->length = 0;
    L->index = -1;
}

// Overwrites the cursor element’s data with x.
// Pre: ListLength()>0, ListIndex()>=0
void ListSet(List L, void *x) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListSet() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling ListSet() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (ListIndex(L) < 0) {
        fprintf(stderr, "List Error: calling ListSet() on List index < 0\n");
        exit(EXIT_FAILURE);
    }
    L->cursor->data = x;
}

// If L is non-empty, sets cursor under the front element, otherwise does nothing.
void ListMoveFront(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListMoveFront() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (!isEmpty(L)) {
        L->cursor = L->front;
        L->index = 0;
    }
}

// If L is non-empty, sets cursor under the back element, otherwise does nothing.
void ListMoveBack(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListMoveBack() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (!isEmpty(L)) {
        L->cursor = L->back;
        L->index = ListLength(L) - 1;
    }
}

// If cursor is defined and not at front, move cursor one step toward the front of L;
// If cursor is defined and at front, cursor becomes undefined;
// If cursor is undefined do nothing
void ListMovePrev(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListMovePrev() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    int idx = ListIndex(L);
    if (idx > 0) {
        L->cursor = L->cursor->prev;
        L->index -= 1;
    } else if (idx == 0) {
        L->cursor = NULL;
        L->index = -1;
    }
}

// If cursor is defined and not at back, move cursor one step toward the back of L;
// If cursor is defined and at back, cursor becomes undefined;
// If cursor is undefined do nothing
void ListMoveNext(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListMoveNext() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    int idx = ListIndex(L);
    int len = ListLength(L);
    if (idx >= 0 && idx < len - 1) {
        L->cursor = L->cursor->next;
        L->index += 1;
    } else if (idx == len - 1) {
        L->cursor = NULL;
        L->index = -1;
    }
}

// Insert new element into L.
// If L is non-empty, insertion takes place before front element.
void ListPrepend(List L, void *x) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListPrepend() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        L->front = L->back = newNode(x);
    } else {
        Node oldFront = L->front;

        Node N = newNode(x);
        N->next = oldFront;

        L->front = N;
        oldFront->prev = N;
        L->index += 1;
    }

    L->length += 1;
}

// Insert new element into L.
// If L is non-empty, insertion takes place after back element.
void ListAppend(List L, void *x) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListAppend() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        L->front = L->back = newNode(x);
    } else {
        Node oldBack = L->back;

        Node N = newNode(x);
        N->prev = oldBack;

        L->back = N;
        oldBack->next = N;
    }

    L->length += 1;
}

// Insert new element before cursor.
// Pre: ListLength()>0, ListIndex()>=0
void ListInsertBefore(List L, void *x) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling insertBeforeListI) on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling insertBeforeListI) on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (ListIndex(L) < 0) {
        fprintf(stderr, "List Error: calling insertBeforeListI) on List index < 0\n");
        exit(EXIT_FAILURE);
    }
    Node cursorPrev = L->cursor->prev;

    if (cursorPrev == NULL) {
        ListPrepend(L, x);
    } else {
        Node N = newNode(x);
        N->next = L->cursor;
        N->prev = cursorPrev;
        cursorPrev->next = N;
        L->cursor->prev = N;

        L->index += 1;
        L->length += 1;
    }
}

// Insert new element after cursor.
// Pre: ListLength()>0, ListIndex()>=0
void ListInsertAfter(List L, void *x) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling insertAfterListI) on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling insertAfterListI) on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (ListIndex(L) < 0) {
        fprintf(stderr, "List Error: calling insertAfterListI) on List index < 0\n");
        exit(EXIT_FAILURE);
    }
    Node cursorNext = L->cursor->next;

    if (cursorNext == NULL) {
        ListAppend(L, x);
    } else {
        Node N = newNode(x);
        N->prev = L->cursor;
        N->next = cursorNext;

        L->cursor->next = N;
        cursorNext->prev = N;

        L->length += 1;
    }
}

// Delete the front element.
// Pre: ListLength()>0
void ListDeleteFront(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListDeleteFront() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling ListDeleteFront() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    Node oldFront = L->front;
    L->front = L->front->next;
    if (L->front != NULL) {
        L->front->prev = NULL;
    }

    L->length -= 1;
    L->index -= 1;
    if (ListIndex(L) == -1) {
        L->cursor = NULL;
    }
    freeNode(&oldFront);
}

// Delete the back element.
// Pre: ListLength()>0
void ListDeleteBack(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListDeleteBack() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling ListDeleteBack() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    Node oldBack = L->back;
    L->back = L->back->prev;
    if (L->back != NULL) {
        L->back->next = NULL;
    }
    L->length -= 1;
    if (ListIndex(L) >= ListLength(L)) {
        L->index = -1;
        L->cursor = NULL;
    }
    freeNode(&oldBack);
}

// Delete cursor element, making cursor undefined.
// Pre: ListLength()>0, ListIndex()>=0
void ListDelete(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListDelete() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }
    if (isEmpty(L)) {
        fprintf(stderr, "List Error: calling ListDelete() on an empty List\n");
        exit(EXIT_FAILURE);
    }
    if (ListIndex(L) < 0) {
        fprintf(stderr, "List Error: calling ListDelete() on List index < 0\n");
        exit(EXIT_FAILURE);
    }
    Node cursor = L->cursor;
    if (cursor->prev == NULL && cursor->next != NULL) {
        ListDeleteFront(L);
    } else if (cursor->prev != NULL && cursor->next == NULL) {
        ListDeleteBack(L);
    } else if (cursor->prev == NULL && cursor->next == NULL) {
        freeNode(&cursor);
        L->cursor = NULL;
        L->index = -1;
        L->length -= 1;
    } else {
        cursor->prev->next = cursor->next;
        cursor->next->prev = cursor->prev;

        L->index = -1;
        L->length -= 1;
        freeNode(&cursor);
    }
    L->cursor = NULL;
}

// Other Functions ------------------------------------------------------------

// Prints to the file pointed to by out, a string representation of L consisting
// of a space separated sequence of integers, with front on left.
void printList(FILE *out, List L, bool isChar) {
    Node N = NULL;

    if (L == NULL) {
        fprintf(stderr, "List Error: calling printList() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }

    for (N = L->front; N != NULL; N = N->next) {
        if (isChar) {
            fprintf(out, "%s\n", (char *) N->data);
        } else {
            fprintf(out, "%p\n", N->data);
        }
    }
}

// Helper Functions -----------------------------------------------------------

// Returns true if L is empty, otherwise returns false.
bool isEmpty(List L) {
    if (L == NULL) {
        fprintf(stderr, "List Error: calling ListGet() on NULL List reference\n");
        exit(EXIT_FAILURE);
    }

    return (L->length == 0);
}
