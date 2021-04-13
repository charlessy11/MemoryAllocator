/**
 * @file
 *
 * Function prototypes and globals for our memory allocator implementation.
 */

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <stdbool.h>

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
struct mem_block *split_block(struct mem_block *block, size_t size);

/**
 * Given a free block, this function attempts to merge it with neighboring
 * blocks --- both the previous and next neighbors --- and update the linked
 * list accordingly.
 *
 * @param block the block to merge
 *
 * @return address of the merged block or NULL if the block cannot be merged.
 */
struct mem_block *merge_block(struct mem_block *block);

/**
* Finds a block of memory that we can reuse
*
* @param size the size of the block
*
* @return found if suitable block is found or null if otherwise
*/
void *reuse(size_t size);

/**
 * Given a block size (header + data), locate a suitable location using the
 * first fit free space management algorithm.
 *
 * @param size size of the block (header + data)
 */
void *first_fit(size_t size);

/**
 * Given a block size (header + data), locate a suitable location using the
 * worst fit free space management algorithm. If there are ties (i.e., you find
 * multiple worst fit candidates with the same size), use the first candidate
 * found.
 *
 * @param size size of the block (header + data)
 */
void *worst_fit(size_t size);

/**
 * Given a block size (header + data), locate a suitable location using the
 * best fit free space management algorithm. If there are ties (i.e., you find
 * multiple best fit candidates with the same size), use the first candidate
 * found.
 *
 * @param size size of the block (header + data)
 */
void *best_fit(size_t size);

/**
 * Prints out the current memory state, including both the regions and blocks.
 * Entries are printed in order, so there is an implied link from the topmost
 * entry to the next, and so on.
 */
void print_memory(void);

/**
* Allocates memory for name
*
* @param size the size of the block
* @param name the name to allocate memory for
*
* @return allocated size for the name
*/
void *malloc_name(size_t size, char *name);

/* -- C Memory API functions -- */

/**
* Allocates memory
*
* @param size the size of memory to be allocated
*
* @return allocated size
*/
void *malloc(size_t size);

/**
* Frees memory
*
* @param ptr the pointer to the block
*
*/
void free(void *ptr);

/**
* Creates memory
*
* @param nmemb
* @param size the size of memory to be created
*
*/
void *calloc(size_t nmemb, size_t size);

/**
* Reallocates memory
*
* @param ptr
* @param size the size of memory to be reallocated
*
*/
void *realloc(void *ptr, size_t size);

/* -- Data Structures -- */

/**
 * Defines metadata structure for both memory 'regions' and 'blocks.' This
 * structure is prefixed before each allocation's data area.
 */
struct mem_block {
    /**
     * The name of this memory block. If the user doesn't specify a name for the
     * block, it should be auto-generated based on the allocation ID. The format
     * should be 'Allocation X' where X is the allocation ID.
     */
    char name[32];

    /** Size of the block */
    size_t size;

    /** Whether or not this block is free */
    bool free;

    /**
     * The region this block belongs to.
     */
    unsigned long region_id;

    /** Next block in the chain */
    struct mem_block *next;

    /** Previous block in the chain */
    struct mem_block *prev;

    /**
     * "Padding" to make the total size of this struct 100 bytes. This serves no
     * purpose other than to make memory address calculations easier. If you
     * add members to the struct, you should adjust the padding to compensate
     * and keep the total size at 100 bytes; test cases and tooling will assume
     * a 100-byte header.
     */
    char padding[35];
} __attribute__((packed));

#endif
