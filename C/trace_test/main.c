/** 跟踪函数级别耗时
 *
 * 使用perf进行跟踪：
 * perf record -p `pidof a.out` -g -e probe_a:bar
 * perf report
 * 可以在同级目录下执行perf report进行查看结果(存在perf.data的情况下)
 *
 * 使用valgrind，ltrace更倾向于系统调用查阅
 * apt install valgrind
 * ltrace -p `pidof a.out`
 *
 * 使用gprof
 * gprof a.out gmon.out > analysis.txt
 */




#include <stdio.h>
#include <unistd.h>

void foo() {
    for (int i = 0; i < 10; i++){
        usleep(100);
    }
}

void bar() {
    for (int i = 0; i < 500000; i++){
        foo();
    }
}

int main() {
    foo();
    bar();
    return 0;
}
