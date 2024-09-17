#include <string.h>
#include "Headers.h"

// structs --------------------------------------------------------------------
typedef struct HeaderObj {
    char *key;
    char *val;
} HeaderObj;
typedef struct HeaderObj *Header;

typedef struct HeadersObj {
    List L;
} HeadersObj;

// Constructors-Destructors ---------------------------------------------------
Header newHeader(char *k, char *v) {
    Header H;
    H = malloc(sizeof(HeaderObj));
    H->key = k;
    H->val = v;
    return H;
}

void freeHeader(Header *pH) {
    if (pH != NULL && *pH != NULL) {
        free((*pH)->key);
        free((*pH)->val);
        free(*pH);
        *pH = NULL;
    }
}

Headers newHeaders() {
    Headers H;
    H = malloc(sizeof(HeadersObj));
    H->L = newList();
    return H;
}

void freeHeaders(Headers *pH) {
    if (pH != NULL && *pH != NULL) {
        Headers H = *pH;
        Header currHeader;
        ListMoveFront(H->L);
        while (ListIndex(H->L) >= 0) {
            currHeader = ListGet(H->L);
            freeHeader(&currHeader);
            ListMoveNext(H->L);
        }
        freeList(&(H->L));
        free(*pH);
        *pH = NULL;
    }
}

// Access functions -----------------------------------------------------------
char *HeadersGetHeaderValue(Headers H, char *k) {
    if (H == NULL) {
        fprintf(
            stderr, "Headers Error: calling HeadersGetHeaderValue() on NULL Headers reference\n");
        exit(EXIT_FAILURE);
    }
    Header curr;
    ListMoveFront(H->L);
    while (ListIndex(H->L) >= 0) {
        curr = ListGet(H->L);
        if (strcmp(curr->key, k) == 0) {
            return curr->val;
        }
        ListMoveNext(H->L);
    }

    return NULL;
}

// Manipulation procedures ----------------------------------------------------
void HeadersAddHeader(Headers H, char *k, char *v) {
    if (H == NULL) {
        fprintf(stderr, "Headers Error: calling HeadersAddHeader() on NULL Headers reference\n");
        exit(EXIT_FAILURE);
    }

    Header curr;
    ListMoveFront(H->L);
    while (ListIndex(H->L) >= 0) {
        curr = ListGet(H->L);
        if (strcmp(curr->key, k) == 0) {
            return;
        }
        ListMoveNext(H->L);
    }

    Header new_header = newHeader(k, v);
    ListAppend(H->L, new_header);
}

void HeadersUpdateHeader(Headers H, char *k, char *new_v) {
    if (H == NULL) {
        fprintf(stderr, "Headers Error: calling HeadersUpdateHeader() on NULL Headers reference\n");
        exit(EXIT_FAILURE);
    }

    Header curr;
    ListMoveFront(H->L);
    while (ListIndex(H->L) >= 0) {
        curr = ListGet(H->L);
        if (strcmp(curr->key, k) == 0) {
            // free old key and val memory to use new key (same content) and val
            free(curr->key);
            free(curr->val);
            curr->key = k;
            curr->val = new_v;
            return;
        }
        ListMoveNext(H->L);
    }
}

void HeadersRemoveHeader(Headers H, char *k) {
    if (H == NULL) {
        fprintf(stderr, "Headers Error: calling HeadersRemoveHeader() on NULL Headers reference\n");
        exit(EXIT_FAILURE);
    }

    Header curr;
    ListMoveFront(H->L);
    while (ListIndex(H->L) >= 0) {
        curr = ListGet(H->L);
        if (strcmp(curr->key, k) == 0) {
            freeHeader(&curr);
            ListDelete(H->L);
            return;
        }
        ListMoveNext(H->L);
    }
}

// Other functions ------------------------------------------------------------
void printHeaders(FILE *out, Headers H) {
    if (H == NULL) {
        fprintf(stderr, "Headers Error: calling printHeaders() on NULL Headers reference\n");
        exit(EXIT_FAILURE);
    }
    Header curr;

    ListMoveFront(H->L);
    while (ListIndex(H->L) >= 0) {
        curr = ListGet(H->L);
        fprintf(out, "(%s, %s) ", curr->key, curr->val);
        ListMoveNext(H->L);
    }
    fprintf(out, "\n");
}
