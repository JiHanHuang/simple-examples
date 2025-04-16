/**
 * @file tracker.c 用于计算自定义百分位数的延迟，想对候选值进行排序，但是后面测试发现会在数据突变的时候，会导致数据不收敛，或者新数据收敛非常慢。
 * @author huangjiahao (huangjh110@chinatelecom.cn)
 * @brief 
 * @version 0.1
 * @date 2025-04-14
 * 
 * @copyright Copyright (c) 2025 中电信智能网络科技有限公司
 * 
 */

#include <limits.h>
#include <time.h>

#define MAX_TRACKED 16
#define OBSERVE_SIZE 2

typedef struct {
    double quantile; // 目标分位数（例如 0.5 表示中位数）
    long estimate;   // 当前分位数的估计值
    // // 左侧候选值数组，用于计算分位数，计算平均值，多1个值，用于存储临时值，用于排序。临时值为0号位
    // long left_candidates[OBSERVE_SIZE + 1];
    // // 右侧候选值数组，用于计算分位数，计算平均值，多1个值，用于存储临时值，用于排序。临时值为OBSERVE_SIZE号位
    // long right_candidates[OBSERVE_SIZE + 1];
    long candidates[2 * OBSERVE_SIZE + 1];
    int count;       // 候选值数量
    int actual_rank; // 实际排名
    int count_left;  // 左侧候选值的计数
    int count_right; // 右侧候选值的计数
} percentile_track_t;

typedef struct {
    percentile_track_t tracks[MAX_TRACKED]; // 跟踪多个分位数的数组
    int track_count; // 当前正在跟踪的分位数数量
    int total_count; // 总的样本数量
    long global_min; // 全局最小值
    long global_max; // 全局最大值
} latency_tracker_t;
static latency_tracker_t g_tracker;

long current_time_us() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long)ts.tv_sec * 1000000L + ts.tv_nsec / 1000;
}

void tracker_init(double *percentiles, int count) {
    latency_tracker_t *tracker = &g_tracker;
    tracker->track_count = count;
    tracker->total_count = 0;
    tracker->global_min = LONG_MAX;
    tracker->global_max = 0;

    for (int i = 0; i < count; ++i) {
        tracker->tracks[i].quantile = percentiles[i];
        tracker->tracks[i].estimate = 0;
        tracker->tracks[i].count_left = 0;
        tracker->tracks[i].count_right = 0;
        memset(tracker->tracks[i].candidates, 0,
               sizeof(tracker->tracks[i].candidates));
    }
}

void tracker_print_percentiles(long value) {
    latency_tracker_t *tracker = &g_tracker;

    printf("%ld ", value);
    for (int i = 0; i < tracker->track_count; ++i) {
        percentile_track_t *pt = &tracker->tracks[i];
       printf("[%.2f%%, E:%ld, C:%d, L:%d, R:%d,  ", 
               pt->quantile, pt->estimate,pt->count, pt->count_left, pt->count_right);
        for (int j = 0; j < 2 * OBSERVE_SIZE + 1; ++j) {
            printf("%ld ", pt->candidates[j]);
        }
        printf("] ");
    }
    printf("\n");
}

static inline int compare(const void *a, const void *b) {
    if (*(long *)a > *(long *)b) {
        return 1;
    }
    return -1;
}

/**
 * 更新全局延迟跟踪器的统计数据（如最小值、最大值、总计数）。
 * 根据插入的值更新每个分位点（percentile）的估计值。估值区的数据需要排序，避免少量小数据扰乱
 */
void tracker_insert(long value) {
    latency_tracker_t *tracker = &g_tracker;
    tracker->total_count++;
    int actual_rank, target_rank;
    if (value < tracker->global_min)
        tracker->global_min = value;
    if (value > tracker->global_max)
        tracker->global_max = value;

    for (int i = 0; i < tracker->track_count; ++i) {
        percentile_track_t *pt = &tracker->tracks[i];

        if (pt->count <= 2 * OBSERVE_SIZE) {
            pt->candidates[pt->count] = value;
            pt->count++;
            continue;
        }
        if (pt->count == 2 * OBSERVE_SIZE + 1) {
            pt->count_left = OBSERVE_SIZE;
            pt->count_right = OBSERVE_SIZE;
            pt->actual_rank = OBSERVE_SIZE;
            qsort(pt->candidates, 2 * OBSERVE_SIZE + 1, sizeof(long), compare);
            pt->estimate = pt->candidates[OBSERVE_SIZE];
        }

        pt->count++;

        if (value < pt->estimate) {
            pt->count_left++;
        } else {
            pt->count_right++;
        }

        target_rank = pt->quantile * tracker->total_count;
        actual_rank = pt->count_left;
        if (actual_rank > target_rank) {
            for (int j = 2 * OBSERVE_SIZE; j > 0; --j) {
                pt->candidates[j] = pt->candidates[j - 1];
            }
            pt->candidates[0] = value;
        } else {
            for (int i = 0; i < 2 * OBSERVE_SIZE; ++i) {
                pt->candidates[i] = pt->candidates[i + 1];
            }
            pt->candidates[2 * OBSERVE_SIZE] = value;
        }
        qsort(pt->candidates, 2 * OBSERVE_SIZE + 1, sizeof(long), compare);
        pt->estimate = pt->candidates[OBSERVE_SIZE];
    }

    // tracker_print_percentiles(value);
}

long tracker_query(double quantile) {
    latency_tracker_t *tracker = &g_tracker;
    for (int i = 0; i < tracker->track_count; ++i) {
        if (fabs(tracker->tracks[i].quantile - quantile) < 1e-6) {
            return tracker->tracks[i].estimate;
        }
    }
    return -1;
}

void tracker_print_results() {
    latency_tracker_t *tracker = &g_tracker;

    for (int i = 0; i < tracker->track_count; ++i) {
        long estimate = tracker_query(tracker->tracks[i].quantile);
        printf("%.2f%% latency (approx): %.3f ms\n",
               tracker->tracks[i].quantile * 100, estimate / 1000.0);
    }
    printf("Max latency: %.3f ms, Min latency: %.3f ms\n",
           tracker->global_max / 1000.0, tracker->global_min / 1000.0);
}

void tracker_free() {
    return;
}