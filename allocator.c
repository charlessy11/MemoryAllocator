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
#include <limits.h>

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
    LOGP("--Splitting blocks--\n");

    size_t min_sz = sizeof(struct mem_block) + ALIGN_SZ;
    //check if splitting is necessary
    if (size < min_sz || size < 104 || !block->free) {
        return NULL;
    } else {
        if (size > block->size) {
            return NULL;
        }
        ssize_t new_sz = block->size - size;
        LOG("New Size = %lu\n", new_sz);
        if (new_sz < min_sz) {
            return NULL;
        }
        block->size = size;
        struct mem_block *new_block = (void *) block + size;
        new_block->size = new_sz;
        new_block->region_id = block->region_id;
        new_block->free = true;
        
        LOG("Block/Header Size = %lu\n", min_sz);
        LOG("Block Size Being Split = %lu\n", size);
        LOG("New Size of First Block AFTER Split = %lu\n", block->size);
        LOG("New Size of Second Block AFTER Split = %lu\n", new_block->size);

        if (block->next != NULL) {
            block->next->prev = new_block;
            new_block->next = block->next;
        } else {
            new_block->next = NULL;
        }

        new_block->prev = block;
        new_block->next = block->next;
        block->next = new_block;

        //update pointers in linked list
        if (block == g_tail) {
            //update tail to be newly split block
            g_tail = new_block;
        } 
        LOG("Block/Header Size = %lu\n", min_sz);
        LOG("New Size of Block = %lu\n", block->size);
        LOG("New Size of Second New Block = %lu\n", new_block->size);
        LOGP("SUCCESS! Able to split blocks!\n");
        return new_block;
    }
    LOGP("FAIL! NOT able to split blocks!\n");
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
    if (block->free == false) {
        return NULL;
    }
    if (block->prev != NULL) { 
        if (block->prev->region_id == block->region_id && block->prev->free) {
            if (g_tail == block) {
                g_tail = block->prev;
            }

            block->prev->size += block->size;

            if (block->next == NULL) {
                block->prev->next = NULL;
            } else {
                block->prev->next = block->next;
                block->next->prev = block->prev;
            }
            
            block = block->prev;
        }
    }

    if (block->next != NULL) {
        if (block->next->region_id == block->region_id && block->next->free) {
            if (g_tail == block->next) {
                g_tail = block;
            }

            block->size += block->next->size;

            if (block->next->next == NULL) {
                block->next = NULL;
            } else {
                block->next = block->next->next;
                block->next->prev = block;
            }
        }
    }

    return block;
}

/**
 * Given a block size (header + data), locate a suitable location using the
 * first fit free space management algorithm.
 *
 * @param size size of the block (header + data)
 */
void *first_fit(size_t size)
{
    LOGP("\t--FIRST FIT--\n");
    struct mem_block *curr = g_head;
    //keep searching until a free block is found
    while (curr != NULL) {
        if (size <= curr->size && curr->free == true) {
            LOGP("Found free block!");
            return curr;
        }

        if (curr->next == NULL) {
            LOGP("NO found free block!");
            return NULL;
        }
        curr = curr->next;
    }
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
    LOGP("\t---- WORST_FIT() ----\n");

    struct mem_block *curr = g_head;
    struct mem_block *worst = NULL;
    size_t worst_size = 0;

    while (curr != NULL) {
        if (curr->free == true && curr->size > worst_size && curr->size > size) {
            worst = curr;
            worst_size = curr->size;
        }     
        curr = curr->next;
    }

    if (worst == NULL) {
        return NULL;
    }

    return worst;
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
    LOGP("--BEST FIT--");
    struct mem_block *curr = g_head;
    struct mem_block *best = NULL;
    size_t best_size = INT_MAX;

    while (curr != NULL) {
        //check if free == true
        if (curr->free == true) {
            ssize_t diff = (ssize_t)curr->size - size;
            if (curr->size == size) {
                return curr;
            }
            else if (diff < best_size) {
                    best = curr;
                    best_size = diff;
            }
        }
        curr = curr->next;
    }

    return best;
}

void *reuse(size_t size)
{
    char *algo = getenv("ALLOCATOR_ALGORITHM");
    if (algo == NULL) {
        algo = "first_fit";
    }

    void *found = NULL;

    if (strcmp(algo, "first_fit") == 0) {
        found = first_fit(size);
    } else if (strcmp(algo, "best_fit") == 0) {
        found = best_fit(size);
    } else if (strcmp(algo, "worst_fit") == 0) {
        found = worst_fit(size);
    } else {
        return NULL;
    }

    if (found != NULL) {
        split_block(found, size);
    }
    return found;
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
    pthread_mutex_lock(&alloc_mutex);

    static const int prot_flags = PROT_READ | PROT_WRITE;
    static const int map_flags = MAP_PRIVATE | MAP_ANONYMOUS;

    size_t total_sz = size + sizeof(struct mem_block);
    size_t aligned_sz = total_sz;
    if (aligned_sz % ALIGN_SZ != 0) {
        aligned_sz = aligned_sz + ALIGN_SZ - (total_sz % ALIGN_SZ);
    }

    struct mem_block *reused_block = reuse(aligned_sz);
    if (reused_block != NULL) {
        reused_block->free = false;
        char *scribbling = getenv("ALLOCATOR_SCRIBBLE");
        if (scribbling != NULL && atoi(scribbling) == 1) {
            memset(reused_block + 1, 0xAA, size);
        }
        pthread_mutex_unlock(&alloc_mutex);
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
        pthread_mutex_unlock(&alloc_mutex);
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

    char *scribbling = getenv("ALLOCATOR_SCRIBBLE");
    if (scribbling != NULL && atoi(scribbling) == 1) {
        memset(block + 1, 0xAA, size);
    }    
    block->free = false;
    pthread_mutex_unlock(&alloc_mutex);
    return block + 1;
}

void free(void *ptr)
{
    pthread_mutex_lock(&alloc_mutex);

    if (ptr == NULL) {
        /* Freeing a NULL pointer does nothing */
        pthread_mutex_unlock(&alloc_mutex);
        return;
    }

    struct mem_block *block = (struct mem_block *)ptr - 1;
    block->free = true;

    block = merge_block(block);

    if ((block->prev == NULL || block->prev->region_id != block->region_id) && (block->next == NULL || block->next->region_id != block->region_id)) {
        if (block->prev != NULL) {
            block->prev->next = block->next;
        }
        if (block->next != NULL) {
            block->next->prev = block->prev;
        }
        if (g_tail == block) {
            g_tail = block->prev;
        }
        if (g_head == block) {
            g_head = block->next;
        }
        if (munmap(block, block->size) == -1) {
            perror("munmap");
            pthread_mutex_unlock(&alloc_mutex);
            return;
        }
    }
    pthread_mutex_unlock(&alloc_mutex);
}

void *calloc(size_t nmemb, size_t size)
{
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

    void *new_ptr = malloc(size);
    if (new_ptr == NULL) {
        perror("malloc");
        return NULL;
    }
    memcpy(new_ptr, ptr, size); 
    free(ptr); 

    return new_ptr;
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
    unsigned long current_region = 0;
    
    while (current_block != NULL) {
        if (current_block->region_id != current_region || current_block == g_head) {
            printf("[REGION %lu] %p\n", current_block->region_id, current_block);
            current_region = current_block->region_id;
        }

        printf("  [BLOCK] %p-%p '%s' %zu [%s]\n", 
            current_block, 
            (char *)current_block + current_block->size, 
            current_block->name,
            current_block->size,
            current_block->free ? "FREE" : "USED");
        current_block = current_block->next;
    }
}