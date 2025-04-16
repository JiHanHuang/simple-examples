#include <malloc.h>
#include <stdio.h>
#include <stdlib.h> // 包含 malloc 和 free 的头文件
#include <unistd.h>
#include <string.h>

// #define CMD "cat /proc/`pidof a.out`/smaps | grep -A 21 heap"
#define CMD "cat /proc/`pidof a.out`/smaps | grep -A 21 heap"
#define MAX 10
#define MMAP_DATA_SIZE 128 * 1024 // 128k，用mmap进行分配
#define HEAP_DATA_SIZE 64 * 1024  // 127k，用heap进行分配管理

void malloc2heapOrMmap() {
    // printf 如果后面跟参数，也会触发malloc的
    // printf("开始分配内存\n");
    // getchar();

    char *arr = (char *)malloc(12 * 1024 * sizeof(char));
    // 由于环境上堆的大小初始化为132k，而内存分配是先看堆够不够
    //char *arr = (char *)malloc((132 * 1024) * sizeof(char));

    // 检查是否分配成功
    if (arr == NULL) {
        printf("内存分配失败\n");
    }

    printf("内存分配成功:%p\n", arr);
    getchar();

    // 释放动态分配的内存
    free(arr);
    printf("内存释放成功\n");
}

void heapGrow() {
    printf("开始分配内存\n");
    //堆132k，会放到堆
    char *aa = (char *)malloc((127 * 1024) * sizeof(char));
    printf("内存分配成功:%p\n", aa);
    getchar();
    printf("开始扩展堆\n");
    // 堆放不下，但是又小于了128k，会扩展堆
    char *bb = (char *)malloc((127 * 1024) * sizeof(char));
    printf("内存分配成功:%p\n", bb);
    getchar();
    printf("释放第二次内存分配\n");
    free(bb);
    getchar();
    printf("释放第一次内存分配\n");
    free(aa);
}

void malloc1() {
    // printf("开始分配内存\n");
    // getchar();

    char *arr = (char *)malloc(1 * sizeof(char));
    // 检查是否分配成功
    if (arr == NULL) {
        printf("内存分配失败\n");
    }

    printf("内存分配成功:%p\n", arr);
    getchar();

    // 释放动态分配的内存
    free(arr);
    printf("内存释放成功\n");
}

void malloc_free() {
    int size = HEAP_DATA_SIZE;
    // 由于环境上堆的大小初始化为132k，先填满堆
    char *h = (char *)malloc(size * sizeof(char));
    if (h == NULL) {
        printf("内存分配失败\n");
    }

    printf("堆内存分配成功:%p\n", h);
    fflush(stdout);
    system(CMD);
    fflush(stdout);
    getchar();

    char *arr[MAX];

    int i = 0;
    for (i = 0; i < MAX; i++) {
        arr[i] = (char *)malloc(size * sizeof(char));
        // 检查是否分配成功
        if (arr[i] == NULL) {
            printf("内存分配失败\n");
        }
        printf("%d 内存分配成功:%p\n", i, arr[i]);
        fflush(stdout);
        system(CMD);
        fflush(stdout);
    }

    printf("---------------\n");

    malloc_info(0, stdout);
    getchar();
    printf("---------------\n");
    fflush(stdout);

    for (i = MAX - 1; i >= 0; i--) {
        // 释放动态分配的内存
        free(arr[i]);
        printf("%d 内存释放成功:%p\n", i, arr[i]);
        fflush(stdout);
        system(CMD);
        fflush(stdout);
    }
    free(h);
    system(CMD);
    printf("内存释放成功\n");
}

void mtrim() {
    int size = HEAP_DATA_SIZE;
    // 由于环境上堆的大小初始化为132k，先填满堆
    char *h = (char *)malloc(size * sizeof(char));
    if (h == NULL) {
        printf("内存分配失败\n");
    }

    printf("堆内存分配成功:%p\n", h);
    fflush(stdout);
    system(CMD);
    fflush(stdout);
    getchar();

    char *arr[MAX];

    int i = 0, ret = -1;
    for (i = 0; i < MAX; i++) {
        arr[i] = (char *)malloc(size * sizeof(char));
        // 检查是否分配成功
        if (arr[i] == NULL) {
            printf("内存分配失败\n");
        }
        printf("%d 内存分配成功:%p\n", i, arr[i]);
        fflush(stdout);
        system(CMD);
        fflush(stdout);
    }

    printf("---------------\n");

    malloc_info(0, stdout);
    getchar();
    printf("---------------\n");
    fflush(stdout);

    // for (i = MAX - 1; i >= 0; i--) {
    for (i = 0; i < MAX; i++) {
        // 释放动态分配的内存
        free(arr[i]);
        ret = malloc_trim(0);
        printf("%d 内存释放成功:%p, trim:%d\n", i, arr[i], ret);
        fflush(stdout);
        system(CMD);
        fflush(stdout);
    }
    free(h);
    system(CMD);
    printf("内存释放成功\n");
}

void mmap_heap() {
    // 由于环境上堆的大小初始化为132k，先填满堆
    char *h = (char *)malloc(HEAP_DATA_SIZE * sizeof(char));
    if (h == NULL) {
        printf("内存分配失败\n");
    }

    printf("堆内存分配成功:%p\n", h);
    fflush(stdout);
    system(CMD);
    fflush(stdout);
    getchar();

    char *arr[MAX];

    int i = 0, ret = -1, size;
    for (i = 0; i < MAX; i++) {
        //前一半是heap，后一半是MMAP
        if (i < MAX / 2) {
            size = HEAP_DATA_SIZE;
        } else {
            size = MMAP_DATA_SIZE;
        }
        arr[i] = (char *)malloc(size * sizeof(char));
        // 检查是否分配成功
        if (arr[i] == NULL) {
            printf("内存分配失败\n");
        }
        printf("%d 内存分配成功:%p\n", i, arr[i]);
        fflush(stdout);
        system(CMD);
        fflush(stdout);
    }

    printf("---------------\n");

    malloc_info(0, stdout);
    getchar();
    printf("---------------\n");
    fflush(stdout);

    //先释放堆，让trim_threshold变大
    for (i = MAX - 1; i >= 0; i--) {
        // for (i = 0; i < MAX; i++) {
        // 释放动态分配的内存
        free(arr[i]);
        // ret = malloc_trim(0);
        printf("%d 内存释放成功:%p, trim:%d\n", i, arr[i], ret);
        fflush(stdout);
        system(CMD);
        fflush(stdout);
    }
    free(h);
    system(CMD);
    printf("内存释放成功\n");
}

void mmap_range_mf() {
    int i = 0, ret = -1;
    printf("开始分配内存\n");
    getchar();
    for (i = 0; i < 1000; i++) {
        char *arr = (char *)malloc((132 * 1024) * sizeof(char));
        // 检查是否分配成功
        if (arr == NULL) {
            printf("内存分配失败\n");
        }

        // 释放动态分配的内存
        free(arr);
    }

    printf("内存释放结束\n");
    getchar();

    ret = malloc_trim(0);
    printf("内存 trim:%d\n", ret);
}

void mallocHeapRandom() {
    int size = 0;

    char *arr[MAX];

    int i = 0;
    for (i = 0; i < MAX; i++) {
        size = (i + 1) * 1024;
        arr[i] = (char *)malloc(size * sizeof(char));
        // 检查是否分配成功
        if (arr[i] == NULL) {
            printf("内存分配失败\n");
        }
    }

    // printf("内存分配成功\n");
    // malloc_info(0, stdout);
    // getchar();

    for (i = MAX - 1; i >= 0; i--) {
        // 释放动态分配的内存
        free(arr[i]);
    }
    // printf("内存释放成功\n");
}

void mallocHeapBins() {
    int size = 0;

    char *arr[MAX] = {0};
    arr[0] = (char *)malloc(10 * 1024 * sizeof(char));
    arr[1] = (char *)malloc(1 * 1024 * sizeof(char));
    arr[2] = (char *)malloc(124 * sizeof(char));
    arr[3] = (char *)malloc(17 * sizeof(char)); // 这个相邻top chunk
    free(arr[2]); // 这个chunk会被放到bins中，测试是tcachebins中
    free(arr[1]); // 这个chunk会被放到bins中，测试是tcachebins中
    free(arr[0]); // 这个chunk会被放到bins中，这个比较大，放到了unsortedbins中

    //会使用bins中的chunk，但是上面的tcachebins中没有合适的，后续就去找对应的small_bins，
    //没有再找unsortedbins中的chunk，而这个chunk就被分为两块了。
    //空闲的依旧在unsortedbins中,分配的一块不在bins中了。
    arr[2] = (char *)malloc(100 * sizeof(char));

    free(arr[3]); // 和top chunk合并, 看大小，如果太小也不会合并，而是放到bins中
    free(arr[2]);
}

void mallocTirmHeap() {
       mallopt(M_TRIM_THRESHOLD, 1024);
    printf("finished M_TRIM_THRESHOLD\\n");
    mallopt(M_TOP_PAD, 0);

    char *p[11];
    int i;

    /* 分配 11 片内存 */
    for(i = 0; i < 11; i++) {
        p[i] = (char*)malloc(1024 * 2);
        strcpy(p[i], "123");
    }

    /* 只释放 10 片内存 */
    for(i = 0; i < 10; i++) {
        free(p[i]);
    }

    pid_t pid = getpid();
    printf("pid = %d\\n", pid);
    getchar();

    /* 释放堆顶下面的空闲 */
    malloc_trim(0);

    pause();

}

int main() {
    // printf("开始\n");
    // malloc2heapOrMmap();
    //  heapGrow();
    // malloc1();
    // malloc_free();
    // mtrim();
    // mmap_heap();
    // mmap_range_mf();
    // mallocHeapRandom();
    mallocTirmHeap();
    // mallocHeapBins();
    getchar();
    return 0;
}
