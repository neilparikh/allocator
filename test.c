//test.c
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	printf("%p\n",sbrk(7));
	printf("%p\n",sbrk(0));
}

