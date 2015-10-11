#include "malloc.h"

#include <stdlib.h>

struct Block {
  void *pointer;
  size_t size;
};

static long int prv_page_size;
static void *prv_start_break;
static void *prv_end_break;
static void *prv_start_available;
static int prv_init = 0;

static void prv_setup(void) {
  prv_page_size = getpagesize();
  sbrk(0);
  prv_start_break = prv_end_break = sbrk(0);
  prv_start_available = prv_start_break;
}

static void prv_add_page(void) {
  sbrk(prv_page_size);
  prv_end_break = sbrk(0);
}

void *my_malloc(size_t size) {
  if (!prv_init) {
    prv_setup();
    prv_init = 1;
  }
  while (prv_end_break - prv_start_available < size) {
    prv_add_page();
  }

  void *pointer_to_return = prv_start_available;
  prv_start_available += size;

  return pointer_to_return;
}

void cleanup(void) {
  brk(prv_start_break);
}
