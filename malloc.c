#include "malloc.h"

#include <unistd.h>

struct Block {
  void *pointer;
  size_t size;
};

typedef struct Block Block;

static long int s_page_size;
static void *s_start_break;
static void *s_end_break;
static void *s_start_available;
static int s_init = 0;
static Block *s_used_blocks;
static Block *s_free_blocks;

static void prv_setup(void) {
  s_page_size = getpagesize();
  s_used_blocks = sbrk(20 * sizeof(Block));
  s_free_blocks = sbrk(20 * sizeof(Block));
  s_start_available = s_start_break = s_end_break = sbrk(0);
}

static void prv_add_page(void) {
  sbrk(s_page_size);
  s_end_break = sbrk(0);
}

void *my_malloc(size_t size) {
  if (!s_init) {
    prv_setup();
    s_init = 1;
  }

  while (s_end_break - s_start_available < size) {
    prv_add_page();
  }

  void *pointer_to_return = s_start_available;
  s_start_available += size;

  return pointer_to_return;
}

void cleanup(void) {
  brk(s_start_break);
}
