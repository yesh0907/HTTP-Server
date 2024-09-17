#ifndef THREADPOOL_H_INCLUDE_
#define THREADPOOL_H_INCLUDE_

#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct ThreadPoolObj ThreadPool;

typedef void (*thread_func_t)(void *arg);

ThreadPool *ThreadPoolCreate(int num);
void ThreadPoolDestroy(ThreadPool *pool);

bool ThreadPoolAddWork(ThreadPool *pool, thread_func_t func, void *arg);
void ThreadPoolWait(ThreadPool *pool);
bool ThreadPoolStop(ThreadPool *pool);

#endif
