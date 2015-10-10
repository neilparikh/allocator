#include <stdio.h>
#include "malloc.h"

int main(void) {
  int* test = myMalloc(sizeof(int));
  int* test2 = myMalloc(sizeof(int));
  *test = 3;
  printf("%d\n", *test);
}
