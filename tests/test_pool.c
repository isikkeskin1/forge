#include "forge_pool.h"

#include <assert.h>
#include <pthread.h>
#include <stddef.h>

typedef struct {
    pthread_mutex_t mutex;
    size_t count;
    long sum;
} accumulator;

typedef struct {
    accumulator *target;
    long value;
} task_context;

static void accumulate(void *opaque) {
    task_context *context = opaque;
    pthread_mutex_lock(&context->target->mutex);
    ++context->target->count;
    context->target->sum += context->value;
    pthread_mutex_unlock(&context->target->mutex);
}

static void test_executes_all_submitted_work(void) {
    forge_worker_pool *pool = forge_worker_pool_create(4);
    assert(pool != NULL);
    assert(forge_worker_pool_size(pool) == 4);

    accumulator target = {PTHREAD_MUTEX_INITIALIZER, 0, 0};
    task_context contexts[1000];
    long expected = 0;
    for (size_t i = 0; i < 1000; ++i) {
        contexts[i].target = &target;
        contexts[i].value = (long)(i % 17);
        expected += contexts[i].value;
        assert(forge_worker_pool_submit(pool, accumulate, &contexts[i]) == 0);
    }
    assert(forge_worker_pool_wait(pool) == 0);
    assert(target.count == 1000);
    assert(target.sum == expected);

    forge_worker_pool_destroy(pool);
    pthread_mutex_destroy(&target.mutex);
}

static void test_invalid_inputs(void) {
    assert(forge_worker_pool_create(0) == NULL);
    assert(forge_worker_pool_submit(NULL, accumulate, NULL) != 0);
    assert(forge_worker_pool_wait(NULL) != 0);
    assert(forge_worker_pool_size(NULL) == 0);
    forge_worker_pool_destroy(NULL);

    forge_worker_pool *pool = forge_worker_pool_create(1);
    assert(pool != NULL);
    assert(forge_worker_pool_submit(pool, NULL, NULL) != 0);
    forge_worker_pool_destroy(pool);
}

int main(void) {
    test_executes_all_submitted_work();
    test_invalid_inputs();
    return 0;
}
