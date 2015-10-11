#include <stdio.h>
#include "malloc.h"

int main(void) {
  int* test = my_malloc(sizeof(int));
  int* test2 = my_malloc(sizeof(int));
  my_free(test2);
  my_free(test);
  cleanup();
}
