#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
 
void dump(int sig)
{
	void* array[128];
	size_t arr_size, i;
	char **strings = NULL;

	signal(SIGSEGV, SIG_IGN);
	
	arr_size = backtrace (array, sizeof(array)/sizeof(array[0]));
	strings = backtrace_symbols (array, arr_size);

	printf("Signal:[%s], PID:(%d), Stack trace:\n", strsignal(sig), getpid());	

	if (strings) {
	    for (i = 0; i < arr_size; i++) {
	    	printf( "%ld: %s\n", i + 1, strings[i]);
	    }
	    free (strings);
	}

	raise(sig);
}

void func_c()
{
    char *segement = NULL;
    printf("%s\n",segement);
}
 
void func_b()
{
	func_c();
}
 
void func_a()
{
	func_b();
}
 
int main(int argc, const char *argv[])
{
	if (signal(SIGSEGV, dump) == SIG_ERR)
		perror("can't catch SIGSEGV");
	func_a();
	return 0;
}