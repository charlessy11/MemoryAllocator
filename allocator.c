/**
 * @file
 *
 * Explores memory management at the C runtime level.
 *
 * To use (one specific command):
 * LD_PRELOAD=$(pwd)/allocator.so command
 * ('command' will run with your allocator)
 *
 * To use (all following commands):
 * export LD_PRELOAD=$(pwd)/allocator.so
 * (Everything after this point will use your custom allocator -- be careful!)
 */

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

#include "allocator.h"
#include "logger.h"

#define ALIGN_SZ 8

static struct mem_block *g_head = NULL; /*!< Start (head) of our linked list */
static struct mem_block *g_tail = NULL;

static unsigned long g_allocations = 0; /*!< Allocation counter */
static unsigned long g_regions = 0; /*!< Allocation counter */

pthread_mutex_t alloc_mutex = PTHREAD_MUTEX_INITIALIZER; /*< Mutex for protecting the linked list */

/**
 * Given a free block, this function will split it into two pieces and update
 * the linked list.
 *
 * @param block the block to split
 * @param size new size of the first block after the split is complete,
 * including header sizes. The size of the second block will be the original
 * block's size minus this parameter.
 *
 * @return address of the resulting second block (the original address will be
 * unchanged) or NULL if the block cannot be split.
 */
struct mem_block *split_block(struct mem_block *block, size_t size)
{
    // TODO block splitting algorithm
    return NULL;
}

/**
 * Given a free block, this function attempts to merge it with neighboring
 * blocks --- both the previous and next neighbors --- and update the linked
 * list accordingly.
 *
 * @param block the block to merge
 *
 * @return address of the merged block or NULL if the block cannot be merged.
 */
struct mem_block *merge_block(struct mem_block *block)
{
    // TODO block merging algorithm
    return NULL;
}

/**
 * Given a block size (header + data), locate a suitable location using the
 * first fit free space management algorithm.
 *
 * @param size size of the block (header + data)
 */
void *first_fit(size_t size)
{
    return NULL;
}

/**
 * Given a block size (header + data), locate a suitable location using the
 * worst fit free space management algorithm. If there are ties (i.e., you find
 * multiple worst fit candidates with the same size), use the first candidate
 * found.
 *
 * @param size size of the block (header + data)
 */
void *worst_fit(size_t size)
{
    // TODO: worst fit FSM implementation
    return NULL;
}

/**
 * Given a block size (header + data), locate a suitable location using the
 * best fit free space management algorithm. If there are ties (i.e., you find
 * multiple best fit candidates with the same size), use the first candidate
 * found.
 *
 * @param size size of the block (header + data)
 */
void *best_fit(size_t size)
{
    // TODO: best fit FSM implementation
    return NULL;
}

void *reuse(size_t size)
{
    // TODO: using free space management (FSM) algorithms, find a block of
    // memory that we can reuse. Return NULL if no suitable block is found.
    char *algo = getenv("ALLOCATOR_ALGORITHM");
    if (algo == NULL) {
        algo = "first_fit";
    }

    void *reused_block = NULL;

    if (strcmp(algo, "first_fit") == 0) {
        reused_block = first_fit(size);
    } else if (strcmp(algo, "best_fit") == 0) {
        reused_block = best_fit(size);
    } else if (strcmp(algo, "worst_fit") == 0) {
        reused_block = worst_fit(size);
    }

    if (reused_block != NULL) {

    }
    return reused_block;
}

void *malloc_name(size_t size, char *name) {
    void *alloc = malloc(size);
    if (alloc == NULL) {
        return NULL;
    }
    struct mem_block *new_block = ((struct mem_block *) alloc) - 1;
    strcpy(new_block->name, name);
    return alloc;
}

void *malloc(size_t size)
{
    // TODO: allocate memory. You'll first check if you can reuse an existing
    // block. If not, map a new memory region.
    static const int prot_flags = PROT_READ | PROT_WRITE;
    static const int map_flags = MAP_PRIVATE | MAP_ANONYMOUS;

    size_t total_sz = size + sizeof(struct mem_block);
    size_t aligned_sz = total_sz;
    if (aligned_sz % ALIGN_SZ != 0) {
        aligned_sz = aligned_sz + ALIGN_SZ - (total_sz % ALIGN_SZ);
    }

    struct mem_block *reused_block = reuse(aligned_sz);
    if (reused_block != NULL) {
        return reused_block + 1;
    }

    int page_sz = getpagesize(); //4096
    size_t num_pages = aligned_sz / page_sz;
    if (aligned_sz % page_sz != 0) {
        num_pages++;
    }

    size_t region_sz = num_pages * page_sz;
    struct mem_block *block = mmap(NULL, region_sz, prot_flags, map_flags, -1, 0);
    if (block == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    snprintf(block->name, 32, "Allocation %lu", g_allocations++);
    block->region_id = g_regions++;

    if (g_head == NULL && g_tail == NULL) {
        block->prev = NULL;
        g_head = block;
        g_tail = block;
    } else {
        g_tail->next = block;
        block->prev = g_tail;
        g_tail = block;
    }

    block->next = NULL;
    block->free = true;  
    block->size = region_sz;
    split_block(block, aligned_sz);
    block->free = false;

    return block + 1;
}

void free(void *ptr)
{
    if (ptr == NULL) {
        /* Freeing a NULL pointer does nothing */
        return;
    }

    struct mem_block *block = (struct mem_block *)ptr - 1;
    block->free = true;
    // if (munmap(block, block->size) == -1) {
    //     perror("munmap");
    //     return;
    // }
    // TODO: free memory. If the containing region is empty (i.e., there are no
    // more blocks in use), then it should be unmapped.
}

void *calloc(size_t nmemb, size_t size)
{
    // TODO: hmm, what does calloc do?
    void *ptr = malloc(nmemb * size);
    if (ptr == NULL) {
        return NULL;
    }
    memset(ptr, 0, nmemb * size);
    return ptr;
}

void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        /* If the pointer is NULL, then we simply malloc a new block */
        return malloc(size);
    }

    if (size == 0) {
        free(ptr);
        return NULL;
    }

    // TODO: reallocation logic

    return NULL;
}

/**
 * print_memory
 *
 * Prints out the current memory state, including both the regions and blocks.
 * Entries are printed in order, so there is an implied link from the topmost
 * entry to the next, and so on.
 */
void print_memory(void)
{
    puts("-- Current Memory State --");
    struct mem_block *current_block = g_head;
    struct mem_block *current_region = NULL;
    // TODO implement memory printout
}

