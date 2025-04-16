/**
 * @file tracker.c 用于计算自定义百分位数的延迟，这种情况会出现50百分位的数据小于10百分位的数据
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
#define OBSERVE_SIZE 8

typedef struct {
    double quantile; // 目标分位数（例如 0.5 表示中位数）
    long estimate;   // 当前分位数的估计值
    long left_candidates
        [OBSERVE_SIZE]; // 左侧候选值数组，用于计算分位数，计算平均值
    long right_candidates
        [OBSERVE_SIZE]; // 右侧候选值数组，用于计算分位数，计算平均值
    int left_index;  // 左侧候选值数组的当前索引
    int right_index; // 右侧候选值数组的当前索引
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
        tracker->tracks[i].left_index = 0;
        tracker->tracks[i].right_index = 0;
        tracker->tracks[i].count_left = 0;
        tracker->tracks[i].count_right = 0;
        memset(tracker->tracks[i].left_candidates, 0,
               sizeof(tracker->tracks[i].left_candidates));
        memset(tracker->tracks[i].right_candidates, 0,
               sizeof(tracker->tracks[i].right_candidates));
    }
}

/**
 * 更新全局延迟跟踪器的统计数据（如最小值、最大值、总计数）。
 * 根据插入的值更新每个分位点（percentile）的估计值。估值区的数据不保序
 */
void tracker_insert(long value) {
    latency_tracker_t *tracker = &g_tracker;
    tracker->total_count++;

    if (value < tracker->global_min)
        tracker->global_min = value;
    if (value > tracker->global_max)
        tracker->global_max = value;

    for (int i = 0; i < tracker->track_count; ++i) {
        percentile_track_t *pt = &tracker->tracks[i];

        if (value < pt->estimate) {
            pt->left_candidates[pt->left_index++ % OBSERVE_SIZE] = value;
            pt->count_left++;
        } else if (value > pt->estimate) {
            pt->right_candidates[pt->right_index++ % OBSERVE_SIZE] = value;
            pt->count_right++;
        }

        int target_rank = pt->quantile * tracker->total_count;
        int actual_rank = pt->count_left;

        if (actual_rank > target_rank && pt->count_left > 0) {
            pt->estimate = pt->left_candidates[pt->left_index % OBSERVE_SIZE];
        } else if (actual_rank < target_rank && pt->count_right > 0) {
            pt->estimate = pt->right_candidates[pt->right_index % OBSERVE_SIZE];
        }
    }
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