# Project 3: Memory Allocator

See: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-3.html 

**About This Program:**

The Memory Allocator Program is a custom memory allocator.To simplify the allocator, mmap alone is being used, and entire **regions** of memory are allocated at a time. Moreover, this program does not only support allocating memory, but also uses the _Free Space Management_ (FSM) algorithms to split up and reuse empty regions, namely: **First fit, Best fit, and Worst fit**. 

**How It Works:**

The programs uses a custom memory allocator instead of the default.

**How It's Built:**

Here is a list of methods from `allocator.c` - the class explores memory management at the C runtime level:

`struct mem_block *split_block(struct mem_block *block, size_t size)`
 => Given a free block, this function will split it into two pieces and update the linked list.
 
`struct mem_block *merge_block(struct mem_block *block)`
=> Given a free block, this function attempts to merge it with neighboring blocks --- both the previous and next neighbors --- and update the linked list accordingly.
 
`void *reuse(size_t size)`
=> Finds a block of memory that we can reuse.
 
`void *first_fit(size_t size)`
=> Given a block size (header + data), locate a suitable location using the first fit free space management algorithm.

`void *worst_fit(size_t size)`
=> Given a block size (header + data), locate a suitable location using the worst fit free space management algorithm. If there are ties (i.e., you find multiple worst fit candidates with the same size), use the first candidate found.

`void *best_fit(size_t size)`
=> Given a block size (header + data), locate a suitable location using the best fit free space management algorithm. If there are ties (i.e., you find multiple best fit candidates with the same size), use the first candidate found.

`void print_memory(void)`
=> Prints out the current memory state, including both the regions and blocks. Entries are printed in order, so there is an implied link from the topmost entry to the next, and so on.

`void *malloc_name(size_t size, char *name)`
=> Allocates memory for name

`void *malloc(size_t size)`
=> Allocates Memory

`void free(void *ptr)`
=> Frees Memory

`void *calloc(size_t nmemb, size_t size)`
=> Creates Memory

`void *realloc(void *ptr, size_t size)`
=> Reallocates Memory

`struct mem_block {char name[32]; size_t size; bool free; unsigned long region_id; struct mem_block *next; struct mem_block *prev; char padding[35]}`
=> Defines metadata structure for both memory 'regions' and 'blocks.' This structure is prefixed before each allocation's data area.

## To compile and use the allocator:

```bash 
make
LD_PRELOAD=$(pwd)/allocator.so ls /
```

(in this example, the command `ls /` is run with the custom memory allocator instead of the default).

## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
# Run all test cases:
make test

# Run a specific test case:
make test run=4

# Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'
```
