/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email addlist_ends */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email addlist_ends (leave blank if none) */
    ""
};

/* Basic constants and macros */
#define DSIZE 8
#define ALIGNMENT 8
#define CLASS_NUM 15

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

/* Pack a size and allocated bit into a double word */
#define PACK(size, allocated) ((size) | (allocated))

/* Read and write at address p */
#define READ(p) (*(unsigned long *)(p))
#define WRITE(p, val) (*(unsigned long *)(p) = (val))

/* Read the size and allocated field from a header */
#define GET_SIZE(word) (READ(word) & ~0b111)
#define GET_ALLOC(word) (READ(word) & 0b1)

/* Given block ptr bp, compute address of next blocks */
#define NEXT(bp) (char *)(bp) + GET_SIZE(HDRP(bp))

/* Given block ptr bp, compute address of previous or succeeding blocks */
#define SUCC(bp) *((void **)HDRP(bp) + 2)
#define PRED(bp) *((void **)HDRP(bp) + 1)

/* Given block ptr bp, set free or allocated */
#define SET(bp, val) WRITE(HDRP(bp), GET_SIZE(HDRP(bp)) | (val))


/* Some shared global variables and functions' declarations*/
void *heap = NULL;
void *heap_end = NULL;

static void *extend_heap(size_t size);
static void *first_fit(size_t size);
static void cut_block(void *bp, size_t size);
static int coalesce_block(void *bp);

/* !!!!!!!!!!!!
 * Here, choose the implementation
 * 1: use implicit free list implementation
 * 2: use explicit free list implementation
 * 3: use segregated free list implementation
 * !!!!!!!!!!!!
 */
#define IMPLEMENTATION 3

#if IMPLEMENTATION == 1 /* use implicit free list, first fit, deferred coalescing, but no boundary tag */

/* 
 * HDRP(bp): compute the addlist_ends of the header of block whose payload is at addlist_ends bp.
 * Block stucture differs from the explicit free list implementation,
 * so HDRP computation is different.
 */
#define HDRP(bp) (char *)((unsigned long *)(bp) - 1)

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    heap = mem_sbrk(1 * DSIZE);
    if (heap == (void *)-1)
        return -1;
    heap += DSIZE;
    WRITE(HDRP(heap), PACK(1 * DSIZE, 0));
    heap_end = (char *)heap + 1 * DSIZE;

    return 0;
}

/*
 * cut_block - cut a block into two smaller blocks.
 * The first block retains its allocation.
 * The second block is marked as free.
 */
static void cut_block(void *bp, size_t size)
{
    size_t block_size = GET_SIZE(HDRP(bp));

    if (block_size >= DSIZE + size) {
        WRITE(HDRP(bp), PACK(size, GET_ALLOC(HDRP(bp))));
        WRITE(HDRP(bp) + size, PACK(block_size - size, 0));
    }
    return;
}

/*
 * coalesce_block - coalesce two adjacent blocks into one if possible.
 * Keep the allocation of the former one.
 */
static int coalesce_block(void *bp)
{
    void *next_bp = NEXT(bp);
    if (next_bp >= heap_end || GET_ALLOC(HDRP(next_bp)))
        return 0;
    
    WRITE(HDRP(bp), PACK(GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(next_bp)), GET_ALLOC(HDRP(bp))));

    return 1;
}

/* 
 * extend_heap - extend the heap and return the beginning.
 */
static void *extend_heap(size_t size)
{
    void *new_area = mem_sbrk(size);
    if (new_area == (void *)-1)
        return NULL;
    WRITE(new_area, PACK(size, 0));
    heap_end += size;

    return new_area;
}

/*
 * first_fit - find a block which is suitable as early as possible.
 * If reached a free block try to coalesce (if not suitable) or return it (if suitable)
 * If reached the end of heap, fit fails, return NULL.
 */
static void *first_fit(size_t size) {
    void *currentbp = heap;
    
    while(currentbp < heap_end) {
        if (GET_ALLOC(HDRP(currentbp))) { 
            currentbp = NEXT(currentbp);
            continue;
        }
        if(GET_SIZE(HDRP(currentbp)) >= size) /* If fit */
            return currentbp;
        if (!coalesce_block(currentbp)) 
            currentbp = NEXT(currentbp);
    }

    return NULL;
}

/* 
 * mm_malloc - allocate a valid free block of given bytes
 */
void *mm_malloc(size_t size)
{
    size = ALIGN(size + DSIZE);
    void *currentbp = first_fit(size);
    if(currentbp == NULL)
        currentbp = extend_heap(size) + DSIZE;
    
    cut_block(currentbp, size);

    return WRITE(HDRP(currentbp), PACK(size, 1)), currentbp;
}

/*
 * mm_free - Just mark it block as free.
 */
void mm_free(void *ptr)
{
    if (ptr == NULL)
        return;
    SET(ptr, 0);

    return;
}

/*
 * mm_realloc - If realloc a smaller or equal size, just cut it.
 * else first try to coalesce, if can not satisfy, malloc a new block and copy the data.
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *newptr = NULL;
    if (ptr == NULL)
        return mm_malloc(size);

    if (size == 0)
        return mm_free(ptr), NULL;
    
    size = ALIGN(size + DSIZE);

    /* If the next block is free, try to coalesce them instead of allocating a new block */
    while(coalesce_block(ptr)) ;
    if (GET_SIZE(HDRP(ptr)) >= size)
        return cut_block(ptr, size), ptr;

    /* If reach here, it means the current heap can not satisfy the request */
    /* So, we need to allocate a new block and copy the data from the old block */
    newptr = mm_malloc(size - DSIZE);
    if (newptr == NULL)
        return NULL;
    memmove(newptr, ptr, GET_SIZE(HDRP(ptr)) - DSIZE);
    WRITE(HDRP(newptr), PACK(GET_SIZE(HDRP(newptr)), 1));

    return mm_free(ptr), newptr;
    
}

#elif IMPLEMENTATION == 2 /* use explicit free list, first fit, deferred coalescing, but no boundary tag */

/* Different from what in the implicit free list implementation */
#define HDRP(bp) (char *)((unsigned long *)(bp) - 3)

/* functions for explicit list */
/* Insert a block to the end of the list and erase a block from the list */
static void erase(void *bp);
static void insert(void *bp);

static void *first_fit(size_t size) {
    void *currentbp = SUCC(heap);
    while(currentbp != heap) {
        if (coalesce_block(currentbp))
            continue;
        if(GET_SIZE(HDRP(currentbp)) >= size)
            return currentbp;
        currentbp = SUCC(currentbp);
    }

    return NULL;
}

/*
 * insert - insert a free block to the end of the free list 
 */
static void insert(void *bp) {
    PRED(bp) = PRED(heap);
    SUCC(PRED(bp)) = bp;
    SUCC(bp) = heap;
    PRED(SUCC(bp)) = bp;

    return;
}

/*
 * erase - erase a block from the free list
 */
static void erase(void *bp) {
    PRED(SUCC(bp)) = PRED(bp);
    SUCC(PRED(bp)) = SUCC(bp);

    return;
}

int mm_init(void)
{
    heap = mem_sbrk(3 * DSIZE);
    if (heap == (void *)-1)
        return -1;
    heap = heap + 3 * DSIZE;
    WRITE(HDRP(heap), PACK(3 * DSIZE, 1));
    PRED(heap) = heap;
    SUCC(heap) = heap;

    heap_end = heap + 3 * DSIZE;

    return 0;
}

static void cut_block(void *bp, size_t size)
{
    size_t block_size = GET_SIZE(HDRP(bp));
    if (block_size >= 3 * DSIZE + size) {
        WRITE(HDRP(bp), PACK(size, GET_ALLOC(HDRP(bp))));
        WRITE(HDRP(NEXT(bp)), PACK(block_size - size, 0));
        insert(NEXT(bp));
    }

    return;
}

static int coalesce_block(void *bp)
{
    void *next_bp = NEXT(bp);
    
    if(GET_ALLOC(HDRP(next_bp)) || next_bp >= heap_end) 
        return 0;

    WRITE(HDRP(bp), PACK(GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(next_bp)), GET_ALLOC(HDRP(bp))));

    return erase(next_bp), 1;
}

static void *extend_heap(size_t size)
{
    if (size < 3 * DSIZE)
        size = 3 * DSIZE;
    void *new_area = mem_sbrk(size);
    if (new_area == (void *)-1)
        return NULL;
    WRITE(new_area, PACK(size, 0));
    insert(new_area + 3 * DSIZE);
    heap_end += size;

    return new_area;
}

void *mm_malloc(size_t size)
{
    size = ALIGN(size + DSIZE);
    void *currentbp = first_fit(size);
    if(currentbp == NULL) {
        extend_heap(size);
        currentbp = first_fit(size);
    }
    cut_block(currentbp , size);
    
    erase(currentbp);
    SET(currentbp, 1);

    return currentbp - 2 * DSIZE;
}

void mm_free(void *ptr)
{
    if (ptr == NULL)
        return;
    SET(ptr + 2 * DSIZE, 0);
    insert(ptr + 2 * DSIZE);

    return;
}

void *mm_realloc(void *ptr, size_t size)
{
    void *newptr = NULL;
    void *bp = ptr + 2 * DSIZE;
    if (ptr == NULL)
        return mm_malloc(size);

    if (size == 0)
        return mm_free(ptr), NULL;
    
    size = ALIGN(size + DSIZE);

    while(coalesce_block(bp));
    if (GET_SIZE(HDRP(bp)) >= size) 
        return cut_block(bp, size), ptr;

    newptr = mm_malloc(size - DSIZE);
    if (newptr == NULL)
        return NULL;
    memmove(newptr, ptr, GET_SIZE(HDRP(bp)) - DSIZE);
    WRITE(HDRP(newptr + 2 * DSIZE), PACK(GET_SIZE(HDRP(newptr + 2 * DSIZE)), 1));
    return mm_free(ptr), newptr;
}

#elif IMPLEMENTATION == 3 /* similar with implementation 2, but use segregated free list, segregated fit */

/* The same as the explicit free list implementation */
#define HDRP(bp) (char *)((unsigned long *)(bp) - 3)

/* functions for explicit list */
/* Insert a block to the end of the list and erase a block from the list */
static void erase(void *bp);
static void insert(void *bp);

/* functions for segregated list */
/* give a size, find the list */
static void *find_list(size_t size);

static void *find_list(size_t size) {
    int i = 0;
    for (i = 0; i < CLASS_NUM - 1; i++)
        if (size <= (1 << (i + 5)))
            return (heap + i * 3 * DSIZE);
    
    return (heap + (CLASS_NUM - 1) * 3 * DSIZE);
}

static void *first_fit(size_t size) {
    void *link_end = NULL;
    for(link_end = find_list(size); link_end < heap + CLASS_NUM * 3 * DSIZE; link_end = NEXT(link_end)) {
        void *currentbp = SUCC(link_end);
        while(currentbp != link_end) {
            if (coalesce_block(currentbp)) {
                currentbp = SUCC(link_end);
                continue;
            }
            if(GET_SIZE(HDRP(currentbp)) >= size)
                return currentbp;
            currentbp = SUCC(currentbp);
        }
    }

    return NULL;
}

/*
 * insert - insert a free block to the end of the free list.
 * But at first, have to find the list where the block should belongs to.
 */
static void insert(void *bp) {
    void *res = find_list(GET_SIZE(HDRP(bp)));
    
    PRED(bp) = PRED(res);
    SUCC(PRED(bp)) = bp;
    SUCC(bp) = res;
    PRED(SUCC(bp)) = bp;

    return;
}

/*
 * erase - erase a block from the free list
 */
static void erase(void *bp) {
    PRED(SUCC(bp)) = PRED(bp);
    SUCC(PRED(bp)) = SUCC(bp);

    return;
}

int mm_init(void)
{
    int i = 0;
    heap = mem_sbrk(CLASS_NUM * 3 * DSIZE);
    if (heap == (void *)-1)
        return -1;
    heap += 3 * DSIZE;
    heap_end = heap + CLASS_NUM * 3 * DSIZE;
    for(i = 0; i < CLASS_NUM; i++) {
        WRITE(HDRP(heap + i * 3 * DSIZE), PACK(3 * DSIZE, 1));
        SUCC(heap + i * 3 * DSIZE) = heap + i * 3 * DSIZE;
        PRED(heap + i * 3 * DSIZE) = heap + i * 3 * DSIZE;
    }

    return 0;
}

static void cut_block(void *bp, size_t size)
{
    size_t block_size = GET_SIZE(HDRP(bp));
    if (block_size >= 3 * DSIZE + size) {
        WRITE(HDRP(bp), PACK(size, GET_ALLOC(HDRP(bp))));
        WRITE(HDRP(NEXT(bp)), PACK(block_size - size, 0));
        erase(bp);
        insert(bp);
        insert(NEXT(bp));
    }

    return;
}

static void shrink_block(void *bp, size_t size)
{
    size_t block_size = GET_SIZE(HDRP(bp));
    if (block_size >= 3 * DSIZE + size) {
        WRITE(HDRP(bp), PACK(size, GET_ALLOC(HDRP(bp))));
        WRITE(HDRP(NEXT(bp)), PACK(block_size - size, 0));
        insert(NEXT(bp));
    }

    return;
}

static int coalesce_block(void *bp)
{
    void *next_bp = NEXT(bp);
    
    if(GET_ALLOC(HDRP(next_bp)) || next_bp >= heap_end) 
        return 0;
    
    WRITE(HDRP(bp), PACK(GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(next_bp)), GET_ALLOC(HDRP(bp))));
    erase(bp);
    erase(next_bp);
    insert(bp);

    return 1;
}

static int expand_block(void *bp) {
    void *next_bp = NEXT(bp);

    if (GET_ALLOC(HDRP(next_bp)) || next_bp >= heap_end)
        return 0;

    erase(next_bp);
    WRITE(HDRP(bp), PACK(GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(next_bp)), GET_ALLOC(HDRP(bp))));

    return 1;

}

static void *extend_heap(size_t size)
{
    if (size < 3 * DSIZE)
        size = 3 * DSIZE;
    void *new_area = mem_sbrk(size);
    if (new_area == (void *)-1)
        return NULL;

    WRITE(new_area, PACK(size, 0));
    insert(new_area + 3 * DSIZE);
    heap_end += size;

    return new_area;
}

void *mm_malloc(size_t size)
{
    size = ALIGN(size + DSIZE);
    void *currentbp = first_fit(size);
    if(currentbp == NULL) {
        extend_heap(size);
        currentbp = first_fit(size);
    }
    cut_block(currentbp, size);
    erase(currentbp);

    return SET(currentbp, 1), currentbp - 2 * DSIZE;
}

void mm_free(void *ptr)
{
    if (ptr == NULL)
        return;
        
    SET(ptr + 2 * DSIZE, 0);
    insert(ptr + 2 * DSIZE);
    
    return;
}

void *mm_realloc(void *ptr, size_t size)
{
    void *newptr = NULL;
    void *bp = ptr + 2 * DSIZE;
    if (ptr == NULL)
        return mm_malloc(size);

    if (size == 0)
        return mm_free(ptr), NULL;
    
    size = ALIGN(size + DSIZE);

    while(expand_block(bp));
    if (GET_SIZE(HDRP(bp)) >= size)
        return shrink_block(bp, size), ptr;

    newptr = mm_malloc(size - DSIZE);
    if (newptr == NULL)
        return NULL;
    memmove(newptr, ptr, GET_SIZE(HDRP(bp)) - DSIZE);
    SET(HDRP(newptr + 2 * DSIZE), 1);
    return mm_free(ptr), newptr;
}

#endif
