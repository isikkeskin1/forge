#include "forge_scan.h"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

int forge_i64_scan_ge(const int64_t *data, size_t length, int64_t threshold,
                      forge_i64_scan_result *result) {
    if (result == NULL || (data == NULL && length != 0)) {
        return -1;
    }

    forge_i64_scan_result next = {0, 0};
    for (size_t i = 0; i < length; ++i) {
        if (data[i] < threshold) {
            continue;
        }
        if (next.count == SIZE_MAX) {
            return -1;
        }
        const int64_t value = data[i];
        if ((value > 0 && next.sum > INT64_MAX - value) ||
            (value < 0 && next.sum < INT64_MIN - value)) {
            return -1;
        }
        ++next.count;
        next.sum += value;
    }

    *result = next;
    return 0;
}

typedef struct {
    const int64_t *data;
    size_t length;
    int64_t threshold;
    forge_i64_scan_result result;
    int status;
} forge_scan_task;

static void forge_i64_scan_task_run(void *context) {
    forge_scan_task *task = context;
    task->status = forge_i64_scan_ge(task->data, task->length, task->threshold,
                                     &task->result);
}

static int forge_scan_merge(forge_i64_scan_result *total,
                            const forge_i64_scan_result *part) {
    if (SIZE_MAX - total->count < part->count) {
        return -1;
    }
    if ((part->sum > 0 && total->sum > INT64_MAX - part->sum) ||
        (part->sum < 0 && total->sum < INT64_MIN - part->sum)) {
        return -1;
    }
    total->count += part->count;
    total->sum += part->sum;
    return 0;
}

int forge_i64_scan_ge_parallel(forge_worker_pool *pool, const int64_t *data,
                               size_t length, int64_t threshold,
                               size_t grain_size,
                               forge_i64_scan_result *result) {
    if (pool == NULL || result == NULL || grain_size == 0 ||
        (data == NULL && length != 0)) {
        return -1;
    }
    if (length == 0) {
        result->count = 0;
        result->sum = 0;
        return 0;
    }

    const size_t task_count = length / grain_size + (length % grain_size != 0);
    if (task_count > SIZE_MAX / sizeof(forge_scan_task)) {
        return -1;
    }
    forge_scan_task *tasks = calloc(task_count, sizeof(*tasks));
    if (tasks == NULL) {
        return -1;
    }

    size_t submitted = 0;
    for (size_t i = 0; i < task_count; ++i) {
        const size_t offset = i * grain_size;
        size_t chunk = length - offset;
        if (chunk > grain_size) {
            chunk = grain_size;
        }
        tasks[i].data = data + offset;
        tasks[i].length = chunk;
        tasks[i].threshold = threshold;
        tasks[i].status = -1;
        if (forge_worker_pool_submit(pool, forge_i64_scan_task_run, &tasks[i]) != 0) {
            break;
        }
        ++submitted;
    }

    int status = 0;
    if (forge_worker_pool_wait(pool) != 0 || submitted != task_count) {
        status = -1;
    }

    forge_i64_scan_result total = {0, 0};
    if (status == 0) {
        for (size_t i = 0; i < task_count; ++i) {
            if (tasks[i].status != 0 || forge_scan_merge(&total, &tasks[i].result) != 0) {
                status = -1;
                break;
            }
        }
    }

    free(tasks);
    if (status == 0) {
        *result = total;
    }
    return status;
}
