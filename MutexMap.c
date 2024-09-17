#include "MutexMap.h"

#include <string.h>

struct EntryObj {
    char *uri;
    pthread_rwlock_t mutex;
};
typedef struct EntryObj *Entry;

// Entry: Constructor and Destructor
Entry newEntry(char *URI) {
    Entry e = malloc(sizeof(struct EntryObj));
    e->uri = strdup(URI);
    pthread_rwlock_init(&(e->mutex), NULL);
    return e;
}

void freeEntry(Entry *pE) {
    if (pE != NULL && *pE != NULL) {
        Entry e = *pE;
        free(e->uri);
        pthread_rwlock_destroy(&(e->mutex));
        free(*pE);
        *pE = NULL;
    }
}

// Private Helper Functions
Entry findEntry(List map, char *URI) {
    ListMoveFront(map);

    Entry curr;
    while (ListIndex(map) >= 0) {
        curr = ListGet(map);
        if (strcmp(curr->uri, URI) == 0) {
            return curr;
        }

        ListMoveNext(map);
    }

    return NULL;
}

void freeAllEntries(List map) {
    ListMoveFront(map);

    Entry curr;
    while (ListIndex(map) >= 0) {
        curr = ListGet(map);
        if (curr != NULL) {
            freeEntry(&curr);
        }
        ListMoveNext(map);
    }
}

struct MutexMapObj {
    List mapping;
};

// Constructor and Destructor
MutexMap newMuxtexMap() {
    MutexMap map = malloc(sizeof(struct MutexMapObj));
    map->mapping = newList();
    return map;
}

void freeMutexMap(MutexMap *pMM) {
    if (pMM != NULL && *pMM != NULL) {
        List map = (*pMM)->mapping;
        freeAllEntries(map);
        freeList(&map);
        free(*pMM);
        *pMM = NULL;
    }
}

// Access Functions
pthread_rwlock_t *MutexMapGetMutex(MutexMap mutex_map, char *URI) {
    Entry e = findEntry(mutex_map->mapping, URI);
    return &(e->mutex);
}

// Manipulation Functions
pthread_rwlock_t *MutexMapAddEntry(MutexMap mutex_map, char *URI) {
    List map = mutex_map->mapping;
    Entry e = findEntry(map, URI);
    if (e == NULL) {
        e = newEntry(URI);
        ListAppend(map, e);
    }
    return &(e->mutex);
}

void MutexMapRemoveEntry(MutexMap mutex_map, char *URI) {
    List map = mutex_map->mapping;
    Entry e = findEntry(map, URI);
    if (e != NULL) {
        freeEntry(&e);
        ListDelete(map);
    }
}
