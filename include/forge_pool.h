#ifndef FORGE_POOL_H
#define FORGE_POOL_H

#include <stddef.h>

typedef void (*forge_task_fn)(void *context);

typedef struct forge_worker_pool forge_worker_pool;

/* Create a fixed-size worker pool. Returns NULL for zero workers or allocation failure. */
forge_worker_pool *forge_worker_pool_create(size_t worker_count);

/* Submit one task. Returns 0 on success, -1 if the pool is unavailable or stopping. */
int forge_worker_pool_submit(forge_worker_pool *pool, forge_task_fn function, void *context);

/* Wait until all submitted work has completed. */
int forge_worker_pool_wait(forge_worker_pool *pool);

/* Stop workers after queued work completes and release the pool. */
void forge_worker_pool_destroy(forge_worker_pool *pool);

size_t forge_worker_pool_size(const forge_worker_pool *pool);

#endif
