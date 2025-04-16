#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SAMPLE_SIZE 10000

// 比较函数，用于 qsort
static inline int compare(const void *a, const void *b) {
    if (*(long *)a > *(long *)b) {
        return 1;
    }
    return -1;
}

void verify_percentile_accuracy(double *percentiles, int percentile_count) {
    // 初始化随机数种子
    srand((unsigned int)time(NULL));

    // 初始化延迟跟踪器
    tracker_init(percentiles, percentile_count);

    // 生成随机延迟数据
    long *samples = (long *)malloc(SAMPLE_SIZE * sizeof(long));
    for (int i = 0; i < SAMPLE_SIZE; ++i) {
        samples[i] =
            rand() % 100000; // 随机生成 0 到 999,999 的延迟值（单位：微秒）
        tracker_insert(samples[i]);
    }

    // 对数据排序
    qsort(samples, SAMPLE_SIZE, sizeof(long),
          (int (*)(const void *, const void *))compare);

    // 验证每个分位数
    for (int i = 0; i < percentile_count; ++i) {
        double quantile = percentiles[i];
        int target_index = (int)(quantile * SAMPLE_SIZE) - 1;
        long actual_value = samples[target_index];
        long estimated_value = tracker_query(quantile);

        // 输出结果
        printf("Quantile: %.2f%%\n", quantile * 100);
        printf("  Actual value: %.3f ms\n", actual_value / 1000.0);
        printf("  Estimated value: %.3f ms\n", estimated_value / 1000.0);
        printf("  Error: %.3f ms\n\n",
               fabs(actual_value - estimated_value) / 1000.0);
    }

    // 释放内存
    free(samples);
}

int main() {
    // 配置需要验证的分位数
    double percentiles[] = {0.1, 0.5, 0.9, 0.99, 0.999};
    // double percentiles[] = {0.5};
    int percentile_count = sizeof(percentiles) / sizeof(percentiles[0]);

    // 验证分位数的准确性
    verify_percentile_accuracy(percentiles, percentile_count);

    tracker_print_results();

    tracker_free();
    
    return 0;
}