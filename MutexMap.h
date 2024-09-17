#ifndef MUTEXMAP_H_INCLUDE_
#define MUTEXMAP_H_INCLUDE_

#include <stdlib.h>
#include <pthread.h>
#include "List.h"

typedef struct MutexMapObj *MutexMap;

// Constructor and Destructor
MutexMap newMuxtexMap();
void freeMutexMap(MutexMap *pMM);

// Access Functions
pthread_rwlock_t *MutexMapGetMutex(MutexMap mutex_map, char *URI);

// Manipulation Function
pthread_rwlock_t *MutexMapAddEntry(MutexMap mutex_map, char *URI);
void MutexMapRemoveEntry(MutexMap mutex_map, char *URI);

#endif
