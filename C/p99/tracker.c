/**
 * @file tracker.c 采用最精确的方式进行延迟跟踪，缺点是占用内存大。只在tracker_query排序，因此性能没问题。
 * @author huangjiahao (huangjh110@chinatelecom.cn)
 * @brief 
 * @version 0.1
 * @date 2025-04-16
 * 
 * @copyright Copyright (c) 2025 中电信智能网络科技有限公司
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

#define DEFAULT_VALUES_NUM 1024

static double *percentiles = NULL; // 存储百分比值
static int percentile_count = 0;  // 百分比值的数量
static long *values = NULL;      // 存储插入的值
static int value_count = 0;      // 当前值的数量
static int value_capacity = DEFAULT_VALUES_NUM;   // 当前数组容量
static bool is_sorted = true;              // 标记数组是否已排序
static long max_value = LONG_MIN;          // 最大值
static long min_value = LONG_MAX;          // 最小值

long current_time_us() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long)ts.tv_sec * 1000000L + ts.tv_nsec / 1000;
}

// 初始化函数
void tracker_init(double *input_percentiles, int count) {
    percentiles = (double *)malloc(count * sizeof(double));
    if (percentiles == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for percentiles.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; i++) {
        percentiles[i] = input_percentiles[i];
    }
    percentile_count = count;

    values = (long *)malloc(value_capacity * sizeof(long));
    if (values == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for values.\n");
        free(percentiles); // 确保释放已分配的内存
        exit(EXIT_FAILURE);
    }
    value_count = 0;
    max_value = LONG_MIN;
    min_value = LONG_MAX;
}

// 插入值（不保持有序）
void tracker_insert(long value) {
    if (value_count == value_capacity) {
        // 改为 1.5 倍增长策略
        value_capacity = value_capacity + value_capacity / 2; 
        values = (long *)realloc(values, value_capacity * sizeof(long));
        if (values == NULL) {
            fprintf(stderr, "Error: Failed to reallocate memory for values.\n");
            exit(EXIT_FAILURE);
        }
    }

    values[value_count++] = value;
    is_sorted = false; // 标记为未排序

    // 更新最大值和最小值
    if (value > max_value) {
        max_value = value;
    }
    if (value < min_value) {
        min_value = value;
    }
}

// 比较函数用于qsort
static inline int compare_long(const void *a, const void *b) {
    long arg1 = *(const long *)a;
    long arg2 = *(const long *)b;
    return (arg1 > arg2) - (arg1 < arg2);
}

// 查询百分位数
long tracker_query(double quantile) {
    if (value_count == 0) {
        fprintf(stderr, "No values inserted yet.\n");
        return -1;
    }

    // 如果未排序，则进行排序
    if (!is_sorted) {
        qsort(values, value_count, sizeof(long), (int (*)(const void *, const void *))compare_long);
        is_sorted = true;
    }

    int index = (int)(quantile * (value_count - 1) + 0.5); // 四舍五入
    return values[index];
}

// 打印所有百分比对应的百分位数
void tracker_print_results() {
    if (value_count == 0) {
        printf("No values to display.\n");
        return;
    }

    printf("Percentile results:\n");
    for (int i = 0; i < percentile_count; i++) {
        double quantile = percentiles[i];
        long result = tracker_query(quantile);
                printf("%.2f%% latency: %.3f ms\n",
               quantile * 100, result / 1000.0);
    }

    // 打印最大值和最小值
        printf("Max latency: %.3f ms, Min latency: %.3f ms\n",
           max_value / 1000.0, min_value / 1000.0);
}

// 释放内存
void tracker_free() {
    if (percentiles != NULL) {
        free(percentiles);
        percentiles = NULL;
    }
    if (values != NULL) {
        free(values);
        values = NULL;
    }
}