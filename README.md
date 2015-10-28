# my_allocator

A sbrk/brk based memory allocator written in C

# How it works
- The allocator maintains a list of all the free memory blocks as well as all of the used memory blocks.
- If the user asks for a block size not in the free block list, the break is extended.
- If a block that is freed is adjancent to another free block, the two are combined.
