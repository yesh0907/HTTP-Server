#include "ThreadPool.h"
#include <stdio.h>

struct ThreadPoolWorkObj {
    thread_func_t func;
    void *arg;
    struct ThreadPoolWorkObj *next;
};
typedef struct ThreadPoolWorkObj ThreadPoolWork;

struct ThreadPoolObj {
    ThreadPoolWork *head;
    ThreadPoolWork *tail;
    pthread_mutex_t mutex;
    pthread_cond_t has_work;
    pthread_cond_t no_work;
    int workers_count;
    int curr_thread_count;
    int total_threads_counts;
    pthread_t *thread_ids;
    bool stop;
};

static ThreadPoolWork *ThreadPoolWorkCreate(thread_func_t func, void *arg) {
    ThreadPoolWork *work;

    if (func == NULL) {
        return NULL;
    }

    work = malloc(sizeof(ThreadPoolWork));
    work->func = func;
    work->arg = arg;
    work->next = NULL;
    return work;
}

static void ThreadPoolWorkDestroy(ThreadPoolWork *work) {
    if (work == NULL) {
        return;
    }

    free(work);
}

static ThreadPoolWork *ThreadPoolWorkGet(ThreadPool *pool) {
    ThreadPoolWork *work;

    if (pool == NULL) {
        return NULL;
    }

    work = pool->head;
    if (work == NULL) {
        return NULL;
    }

    if (work->next == NULL) {
        pool->head = NULL;
        pool->tail = NULL;
    } else {
        pool->head = work->next;
    }

    return work;
}

static void *ThreadPoolWorker(void *arg) {
    ThreadPool *pool = arg;
    ThreadPoolWork *work;

    if (pool == NULL) {
        return NULL;
    }

    while (true) {
        pthread_mutex_lock(&(pool->mutex));

        while (pool->head == NULL && !pool->stop) {
            pthread_cond_wait(&(pool->has_work), &(pool->mutex));
        }

        if (pool->stop) {
            break;
        }

        work = ThreadPoolWorkGet(pool);
        pool->workers_count++;
        pthread_mutex_unlock(&(pool->mutex));

        if (work != NULL) {
            work->func(work->arg);
            ThreadPoolWorkDestroy(work);
        }

        pthread_mutex_lock(&(pool->mutex));
        pool->workers_count--;
        if (!pool->stop && pool->workers_count == 0 && pool->head == NULL) {
            pthread_cond_signal(&(pool->no_work));
        }
        pthread_mutex_unlock(&(pool->mutex));
    }

    pool->curr_thread_count--;
    pthread_cond_signal(&(pool->no_work));
    pthread_mutex_unlock(&(pool->mutex));
    return NULL;
}

ThreadPool *ThreadPoolCreate(int num) {
    ThreadPool *pool;

    pool = malloc(sizeof(ThreadPool));
    pool->curr_thread_count = num;
    pool->total_threads_counts = num;
    pool->workers_count = 0;
    pool->stop = false;

    pthread_mutex_init(&(pool->mutex), NULL);
    pthread_cond_init(&(pool->has_work), NULL);
    pthread_cond_init(&(pool->no_work), NULL);

    pool->head = NULL;
    pool->tail = NULL;

    pool->thread_ids = malloc(sizeof(pthread_t) * num);

    for (int i = 0; i < num; i++) {
        if (pthread_create(&(pool->thread_ids[i]), NULL, ThreadPoolWorker, pool) != 0) {
            printf("failed to create thread %d\n", i);
            return NULL;
        }
    }

    return pool;
}

void ThreadPoolDestroy(ThreadPool *pool) {
    ThreadPoolWork *work;
    ThreadPoolWork *temp;

    if (pool == NULL) {
        return;
    }

    pthread_mutex_lock(&(pool->mutex));
    work = pool->head;
    while (work != NULL) {
        temp = work->next;
        ThreadPoolWorkDestroy(work);
        work = temp;
    }
    pool->stop = true;
    pthread_cond_broadcast(&(pool->has_work));
    pthread_mutex_unlock(&(pool->mutex));

    ThreadPoolWait(pool);

    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->has_work));
    pthread_cond_destroy(&(pool->no_work));

    for (int i = 0; i < pool->total_threads_counts; i++) {
        if (pthread_join(pool->thread_ids[i], NULL) != 0) {
            printf("thread did not join\n");
        }
    }

    free(pool->thread_ids);
    free(pool);
}

bool ThreadPoolAddWork(ThreadPool *pool, thread_func_t func, void *arg) {
    ThreadPoolWork *work;

    if (pool == NULL) {
        return false;
    }

    work = ThreadPoolWorkCreate(func, arg);
    if (work == NULL) {
        return false;
    }

    pthread_mutex_lock(&(pool->mutex));
    if (pool->head == NULL) {
        pool->head = work;
        pool->tail = pool->head;
    } else {
        pool->tail->next = work;
        pool->tail = work;
    }

    pthread_cond_signal(&(pool->has_work));
    pthread_mutex_unlock(&(pool->mutex));

    return true;
}

void ThreadPoolWait(ThreadPool *pool) {
    if (pool == NULL) {
        return;
    }

    pthread_mutex_lock(&(pool->mutex));
    while (true) {
        if ((!pool->stop && pool->workers_count != 0)
            || (pool->stop && pool->curr_thread_count != 0)) {
            pthread_cond_wait(&(pool->no_work), &(pool->mutex));
        } else {
            break;
        }
    }
    pthread_mutex_unlock(&(pool->mutex));
}

bool ThreadPoolStop(ThreadPool *pool) {
    if (pool == NULL) {
        return true;
    }

    return pool->stop;
}
