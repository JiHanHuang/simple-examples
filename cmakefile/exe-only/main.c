/**
 * @file share_test.c
 * @author jihan
 * @brief 
 * @version 0.1
 * @date 2023-09-25
 * 
 * 
 */

#include "print.h"
#include <stdio.h>


int main(int argc, char **argv) {
    printf("run function\n");
    print();
    int b = 1;
    int *a = NULL;
    printf("%p-%p\n", a, b);
    a = &b;
    printf("%p-%p\n", a, &b);
    return 0;
}