#include "malloc.h"

#include <unistd.h>
#include <stdbool.h>
#include <string.h>

struct Block {
  void *pointer;
  size_t size;
};

typedef struct Block Block;

static long int s_page_size;
static void *s_start_break;
static void *s_end_break;
static void *s_start_available;
static bool s_init = false;

static Block *s_used_blocks;
static int s_num_used_blocks = 0;
static Block *s_free_blocks;
static int s_num_free_blocks = 0;

static void prv_setup(void) {
  s_page_size = getpagesize();
  s_used_blocks = sbrk(20 * sizeof(Block));
  s_free_blocks = sbrk(20 * sizeof(Block));
  s_start_available = s_start_break = s_end_break = sbrk(0);
}

static void prv_add_page(void) {
  sbrk(s_page_size);
  void *old_pointer = s_end_break;
  s_end_break = sbrk(0);
  s_free_blocks[s_num_free_blocks++] = (Block) {.pointer = old_pointer,
                                                .size = (s_end_break - old_pointer)};
}

void *my_malloc(size_t size) {
  if (!s_init) {
    prv_setup();
    s_init = true;
  }

  int block_to_use = -1;
  for (int i = 0; i < s_num_free_blocks; i++) {
    if (s_free_blocks[i].size >= size) {
      block_to_use = i;
      break;
    }
  }

  if (block_to_use < 0) {
    prv_add_page();
    block_to_use = s_num_free_blocks - 1;
  }

  s_used_blocks[s_num_used_blocks++] = (Block) {.pointer = s_free_blocks[block_to_use].pointer,
                                                .size = size};
  s_free_blocks[block_to_use].pointer += size;
  s_free_blocks[block_to_use].size -= size;

  return s_used_blocks[s_num_used_blocks-1].pointer;
}

void my_free(void *ptr) {
  int block_to_free = -1;
  for (int i = 0; i < s_num_used_blocks; i++) {
    if (s_used_blocks[i].pointer == ptr) {
      block_to_free = i;
      break;
    }
  }

  if (block_to_free < 0) {
    return;
  }

  int block_to_insert_at = -1;
  for (int i = 0; i < s_num_free_blocks; i++) {
    if (s_free_blocks[i].pointer > ptr) {
      block_to_insert_at = i;
      break;
    }
  }

  if (block_to_insert_at < 0) {
    s_free_blocks[s_num_free_blocks++] = s_used_blocks[block_to_free];
  } else if ((s_used_blocks[block_to_free].pointer + s_used_blocks[block_to_free].size) ==
             s_free_blocks[block_to_insert_at].pointer) {
    // just expand this block
    s_free_blocks[block_to_insert_at].pointer = s_used_blocks[block_to_free].pointer;
    s_free_blocks[block_to_insert_at].size += s_used_blocks[block_to_free].size;

    if (block_to_insert_at > 0 && (s_free_blocks[block_to_insert_at - 1].pointer +
                                   (s_free_blocks[block_to_insert_at - 1].size) ==
                                   s_free_blocks[block_to_insert_at].pointer)) {
      // the previous and current blocks are contigous
      s_free_blocks[block_to_insert_at].pointer = s_free_blocks[block_to_insert_at - 1].pointer;
      s_free_blocks[block_to_insert_at].size += s_free_blocks[block_to_insert_at - 1].size;
      memmove(sizeof(Block) * (block_to_insert_at - 1) + s_free_blocks,
              sizeof(Block) * (block_to_insert_at) + s_free_blocks,
              sizeof(Block) * (s_num_free_blocks - block_to_insert_at - 1));
      s_num_free_blocks--;
    }
  } else {
      memmove(sizeof(Block) * (block_to_insert_at + 1) + s_free_blocks,
              sizeof(Block) * (block_to_insert_at) + s_free_blocks,
              sizeof(Block) * (s_num_free_blocks - block_to_insert_at - 1));
      s_num_free_blocks--;
  }

  if (block_to_free < (s_num_used_blocks - 1)) {
    memmove(sizeof(Block) * block_to_free + s_used_blocks,
            sizeof(Block) * (block_to_free + 1) + s_used_blocks,
            sizeof(Block) * (s_num_used_blocks - block_to_free + 2));
    s_num_used_blocks--;
  } else {
    s_num_used_blocks--;
  }
}

void cleanup(void) {
  brk(s_start_break);
}
