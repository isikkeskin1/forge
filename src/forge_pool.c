#include "forge_pool.h"

#include <pthread.h>
#include <stdlib.h>

#define FORGE_POOL_QUEUE_CAPACITY 256u

typedef struct {
    forge_task_fn function;
    void *context;
} forge_task;

struct forge_worker_pool {
    pthread_t *threads;
    size_t worker_count;
    forge_task queue[FORGE_POOL_QUEUE_CAPACITY];
    size_t head;
    size_t tail;
    size_t queued;
    size_t active;
    int stopping;
    pthread_mutex_t mutex;
    pthread_cond_t work_ready;
    pthread_cond_t space_ready;
    pthread_cond_t idle;
};

static void *forge_worker_main(void *argument) {
    forge_worker_pool *pool = argument;
    for (;;) {
        pthread_mutex_lock(&pool->mutex);
        while (pool->queued == 0 && !pool->stopping) {
            pthread_cond_wait(&pool->work_ready, &pool->mutex);
        }
        if (pool->queued == 0 && pool->stopping) {
            pthread_mutex_unlock(&pool->mutex);
            return NULL;
        }

        forge_task task = pool->queue[pool->head];
        pool->head = (pool->head + 1) % FORGE_POOL_QUEUE_CAPACITY;
        --pool->queued;
        ++pool->active;
        pthread_cond_signal(&pool->space_ready);
        pthread_mutex_unlock(&pool->mutex);

        task.function(task.context);

        pthread_mutex_lock(&pool->mutex);
        --pool->active;
        if (pool->queued == 0 && pool->active == 0) {
            pthread_cond_broadcast(&pool->idle);
        }
        pthread_mutex_unlock(&pool->mutex);
    }
}

forge_worker_pool *forge_worker_pool_create(size_t worker_count) {
    if (worker_count == 0 || worker_count > SIZE_MAX / sizeof(pthread_t)) {
        return NULL;
    }
    forge_worker_pool *pool = calloc(1, sizeof(*pool));
    if (pool == NULL) {
        return NULL;
    }
    pool->threads = calloc(worker_count, sizeof(*pool->threads));
    if (pool->threads == NULL) {
        free(pool);
        return NULL;
    }
    pool->worker_count = worker_count;
    if (pthread_mutex_init(&pool->mutex, NULL) != 0 ||
        pthread_cond_init(&pool->work_ready, NULL) != 0 ||
        pthread_cond_init(&pool->space_ready, NULL) != 0 ||
        pthread_cond_init(&pool->idle, NULL) != 0) {
        free(pool->threads);
        free(pool);
        return NULL;
    }

    size_t created = 0;
    for (; created < worker_count; ++created) {
        if (pthread_create(&pool->threads[created], NULL, forge_worker_main, pool) != 0) {
            pthread_mutex_lock(&pool->mutex);
            pool->stopping = 1;
            pthread_cond_broadcast(&pool->work_ready);
            pthread_mutex_unlock(&pool->mutex);
            for (size_t i = 0; i < created; ++i) pthread_join(pool->threads[i], NULL);
            pthread_cond_destroy(&pool->idle);
            pthread_cond_destroy(&pool->space_ready);
            pthread_cond_destroy(&pool->work_ready);
            pthread_mutex_destroy(&pool->mutex);
            free(pool->threads);
            free(pool);
            return NULL;
        }
    }
    return pool;
}

int forge_worker_pool_submit(forge_worker_pool *pool, forge_task_fn function, void *context) {
    if (pool == NULL || function == NULL) return -1;
    pthread_mutex_lock(&pool->mutex);
    while (pool->queued == FORGE_POOL_QUEUE_CAPACITY && !pool->stopping) {
        pthread_cond_wait(&pool->space_ready, &pool->mutex);
    }
    if (pool->stopping) {
        pthread_mutex_unlock(&pool->mutex);
        return -1;
    }
    pool->queue[pool->tail].function = function;
    pool->queue[pool->tail].context = context;
    pool->tail = (pool->tail + 1) % FORGE_POOL_QUEUE_CAPACITY;
    ++pool->queued;
    pthread_cond_signal(&pool->work_ready);
    pthread_mutex_unlock(&pool->mutex);
    return 0;
}

int forge_worker_pool_wait(forge_worker_pool *pool) {
    if (pool == NULL) return -1;
    pthread_mutex_lock(&pool->mutex);
    while (pool->queued != 0 || pool->active != 0) {
        pthread_cond_wait(&pool->idle, &pool->mutex);
    }
    pthread_mutex_unlock(&pool->mutex);
    return 0;
}

void forge_worker_pool_destroy(forge_worker_pool *pool) {
    if (pool == NULL) return;
    (void)forge_worker_pool_wait(pool);
    pthread_mutex_lock(&pool->mutex);
    pool->stopping = 1;
    pthread_cond_broadcast(&pool->work_ready);
    pthread_mutex_unlock(&pool->mutex);
    for (size_t i = 0; i < pool->worker_count; ++i) pthread_join(pool->threads[i], NULL);
    pthread_cond_destroy(&pool->idle);
    pthread_cond_destroy(&pool->space_ready);
    pthread_cond_destroy(&pool->work_ready);
    pthread_mutex_destroy(&pool->mutex);
    free(pool->threads);
    free(pool);
}

size_t forge_worker_pool_size(const forge_worker_pool *pool) {
    return pool == NULL ? 0 : pool->worker_count;
}
