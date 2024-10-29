#include <malloc.h>
#include <stdio.h>
#include <stdlib.h> // 包含 malloc 和 free 的头文件
#include <unistd.h>

void malloc2heapOrMmap() {
    // printf 如果后面跟参数，也会触发malloc的
    printf("开始分配内存\n");
    getchar();

    // char *arr = (char *)malloc(127*1024 * sizeof(char));
    // 由于环境上堆的大小初始化为132k，而内存分配是先看堆够不够
    char *arr = (char *)malloc((132 * 1024) * sizeof(char));

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

// #define CMD "cat /proc/`pidof a.out`/smaps | grep -A 21 heap"
#define CMD "cat /proc/`pidof a.out`/smaps | grep -A 21 heap"
#define MAX 10
#define MMAP_DATA_SIZE 128 * 1024 // 128k，用mmap进行分配
#define HEAP_DATA_SIZE 64 * 1024 // 127k，用heap进行分配管理

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

    int i = 0,ret=-1;
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

    int i = 0,ret=-1, size;
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

int main() {
    // printf("开始\n");
    //  malloc2heapOrMmap();
    //  heapGrow();
    // malloc1();
    // malloc_free();
    mtrim();
    // mmap_heap();
    getchar();
    return 0;
}
