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
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* Basic constants and macros */
#define WSIZE 4
#define DSIZE 8

#define IMPLEMENTATION 1

#if IMPLEMENTATION == 1 /* use implicit free list, first fit, deferred coalescing, but no boundary tag */
/* !!! BEGINNING OF IMPLICIT FREE LIST IMPLEMENTATION !!! */

/* Pack a size and allocated bit into a header word */
#define PACK(size, allocated) ((size) | (allocated))

/* Read and write a word at address p */
#define READ(p) (*(unsigned int *)(p))
#define WRITE(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated field from a header word */
#define GET_SIZE(word) (READ(word) & ~0b111)
#define GET_ALLOC(word) (READ(word) & 0b1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT(bp) ((char *)(bp) + GET_SIZE((char *)(bp) - DSIZE))

/* Given block ptr bp, set free if para is 0 or allocated if 1 */
#define SET(bp, val) WRITE(HDRP(bp), GET_SIZE(HDRP(bp)) | (val))

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
void *heap = NULL;
void *heap_end = NULL;

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    heap = mem_sbrk(2 * DSIZE);
    if (heap == (void *)-1)
        return -1;
    heap += DSIZE;
    WRITE(HDRP(heap), PACK(2 * DSIZE, 0));
    heap_end = (char *)heap + 1 * DSIZE;
    return 0;
}

void cut_block(void *bp, size_t size)
{
    size_t block_size = GET_SIZE(HDRP(bp));
    if (block_size > size) {
        WRITE(HDRP(bp), PACK(size, 0));
        WRITE(HDRP(bp) + size, PACK(block_size - size, 0));
    }
    return;
}

int coalesce_block(void *bp)
{
    void *next_bp = NEXT(bp);
    size_t block_size = GET_SIZE(HDRP(bp));
    size_t next_blk_size = GET_SIZE(HDRP(next_bp));
    if (next_bp >= heap_end)
        return 0;
    if (GET_ALLOC(HDRP(next_bp)))
        return -1;
    
    WRITE(HDRP(bp), PACK(block_size + next_blk_size, GET_ALLOC(HDRP(bp))));
    return 1;
}

/* If cannot find a suitable fit, extend the heap */
/* and return the beginning of the new area (the same as previous brk)*/
void *extend_heap(size_t size)
{
    //printf("\nbefore, heap end is %p\n", mem_heap_hi());
    void *new_area = mem_sbrk(size);
    //printf("after, heap end is %p\n", mem_heap_hi());
    if (new_area == (void *)-1)
        return NULL;
    WRITE(new_area, PACK(size, 0));
    heap_end += size;
    return new_area;
}

void *first_fit(size_t size) {
    void *currentbp = heap;
    while(1) {
        /* While be able to coalesce && block_size can not satisfy the request */
        if (currentbp >= heap_end) {
            return NULL;
        }
        if (GET_ALLOC(HDRP(currentbp))) {
            currentbp = NEXT(currentbp);
            continue;
        }
        /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
        if(GET_SIZE(HDRP(currentbp)) >= size) {
            return currentbp;
        /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
        }
        while(GET_SIZE(HDRP(currentbp)) < size) {
            /* If cannot coalesce anymore, jump to next block; else continue coalescing */
            if(GET_ALLOC(HDRP(currentbp))) {
                currentbp = NEXT(currentbp);
                break;
            }
            switch (coalesce_block(currentbp)) {
                case -1:
                    /* If this or next block is allocated, jump to next block */
                    currentbp = NEXT(currentbp);
                    break;
                case 0:
                    /* If remaining memory is not enough, extend the heap */
                    return currentbp;
                    break;
                case 1:
                    /* If successfully coalesced, continue coalescing */
                    
            }
        }
    }
}
/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size = ALIGN(size + DSIZE);
    void *currentbp = first_fit(size);
    if(currentbp == NULL) {
        currentbp = extend_heap(size) + DSIZE;
        //WRITE(HDRP(currentbp), PACK(size, 1));
    }
    else if(GET_SIZE(HDRP(currentbp)) >= size) {
        cut_block(currentbp, size);
        //SET(currentbp, 1);
    }
    else {
        extend_heap(size - GET_SIZE(HDRP(currentbp)));
        coalesce_block(currentbp);
    }
    WRITE(HDRP(currentbp), PACK(size, 1));
    return currentbp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    if (ptr == NULL)
        return;
    SET(ptr, 0);
    return;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *newptr = NULL;
    if (ptr == NULL)
        return mm_malloc(size);

    if (size == 0)
        return mm_free(ptr), NULL;
    
    size = ALIGN(size + DSIZE);
    if (size <= GET_SIZE(HDRP(ptr))) {
        WRITE(HDRP(ptr), PACK(size, 1));
        return ptr;
    }

    /* If the next block is free, try to coalesce them instead of allocating a new block */
    /* coalesce block until it can not coalesce anymore or the block size can satisfy the request */
    while(GET_SIZE(HDRP(ptr)) < size && coalesce_block(ptr) == 1) {
        ;
    }
    if (GET_SIZE(HDRP(ptr)) >= size) {
        cut_block(ptr, size);
        SET(ptr, 1);
        return ptr;
    }

    /* If reach here, it means the current block can not satisfy the request */
    /* So, we need to allocate a new block and copy the data from the old block */
    newptr = mm_malloc(size - DSIZE);
    if (newptr == NULL) {
        //SET(ptr, 1);
        return NULL;
    }
    memmove(newptr, ptr, GET_SIZE(HDRP(ptr)) - DSIZE);
    WRITE(HDRP(newptr), PACK(size, 1));
    mm_free(ptr);
    return newptr;
    
}
/* !!! END OF IMPLICIT FREE LIST IMPLEMENTATION !!! */


#elif IMPLEMENTATION == 2/* use explicit free list, first fit, deferred coalescing, have boundary tag */
/* !!! BEGINNING OF EXPLICIT FREE LIST IMPLEMENTATION !!! */

/* Pack a size and allocated bit into a header word */
#define PACK(size, allocated) ((size) | (allocated))

/* Read and write a word at address p */
#define READ(p) (*(unsigned long *)(p))
#define WRITE(p, val) (*(unsigned long *)(p) = (val))

/* Read the size and allocated field from a header word */
#define GET_SIZE(word) (READ(word) & ~0b111)
#define GET_ALLOC(word) (READ(word) & 0b1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) (char *)((unsigned long *)(bp) - 3)
//#define FTRP(bp) (char *)((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)
#define PREDP(bp) ((void **)HDRP(bp) + 1)
#define SUCCP(bp) ((void **)HDRP(bp) + 2)
/* Given block ptr bp, compute address of next and previous blocks */
#define SUCC(bp) *SUCCP(bp)
#define PRED(bp) *PREDP(bp)
#define NEXT(bp) (char *)(bp) + GET_SIZE(HDRP(bp))

/* Given block ptr bp, set free if para is 0 or allocated if 1 */
#define SET(bp, val) WRITE(HDRP(bp), GET_SIZE(HDRP(bp)) | (val))

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

void *heap = NULL;
void *heap_end = NULL;
//void *list_beg = NULL;
void *list_end = NULL;
void *list_beg = NULL;
/* 
 * mm_init - initialize the malloc package.
 */

void insert(void *bp, void *succ) {
    PRED(bp) = PRED(succ);
    SUCC(PRED(bp)) = bp;
    SUCC(bp) = succ;
    PRED(SUCC(bp)) = bp;
    if(list_beg == list_end) {
        list_beg = bp;
    }
}

void erase(void *bp) {
    if (bp == list_beg) {
        list_beg = SUCC(bp);
    }
    PRED(SUCC(bp)) = PRED(bp);
    SUCC(PRED(bp)) = SUCC(bp);
}

void insert_end(void *bp) {
    insert(bp, list_end);
}

int mm_init(void)
{
    heap = mem_sbrk(3 * DSIZE);
    if (heap == (void *)-1)
        return -1;
    list_end = heap + 3 * DSIZE;
    heap += 3 * DSIZE;
    WRITE(HDRP(heap), PACK(3 * DSIZE, 1));
    PRED(heap) = heap;
    SUCC(heap) = heap;

    heap = mem_sbrk(3 * DSIZE);
    if (heap == (void *)-1)
        return -1;
    heap_end = heap + 3 * DSIZE;
    heap += 3 * DSIZE;
    WRITE(HDRP(heap), PACK(3 * DSIZE, 0));
    PRED(heap) = heap;
    SUCC(heap) = heap;
    list_beg = heap;
    insert_end(heap);

    return 0;
}

void cut_block(void *bp, size_t size)
{
    size_t block_size = GET_SIZE(HDRP(bp));
    if (block_size >= 3 * DSIZE + size) {
        WRITE(HDRP(bp), PACK(size, 0));
        WRITE(HDRP(NEXT(bp)), PACK(block_size - size, 0));
        insert_end(NEXT(bp));
        //link(bp, NEXT(bp));
    }
    return;
}

int coalesce_block(void *bp)
{
    void *next_bp = NEXT(bp);
    
    size_t block_size = GET_SIZE(HDRP(bp));
    size_t next_blk_size = GET_SIZE(HDRP(next_bp));
    if(GET_ALLOC(HDRP(next_bp)) || next_bp >= heap_end) 
        return -1;

    WRITE(HDRP(bp), PACK(block_size + next_blk_size, GET_ALLOC(HDRP(bp))));
    erase(next_bp);
    return 1;
}

/* If cannot find a suitable fit, extend the heap */
/* and return the beginning of the new area (the same as previous brk)*/
void *extend_heap(size_t size)
{
    //printf("\nbefore, heap end is %p\n", mem_heap_hi());
    if (size < 3 * DSIZE)
        size = 3 * DSIZE;
    //printf("extend %ld bytes; total %ld bytes\n", size, mem_heap_hi()- mem_heap_lo() + 1);
    void *new_area = mem_sbrk(size);
    //printf("after, heap end is %p\n", mem_heap_hi());
    if (new_area == (void *)-1)
        return NULL;
    WRITE(new_area, PACK(size, 0));
    insert_end(new_area + 3 * DSIZE);
    heap_end += size;
    //printf("now max heap size is: %ld\n", heap_end - heap);
    //printf("extend: %p -- %p, \t%ld bytes\n", new_area, new_area + size, size);
    return new_area;
}
/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size = ALIGN(size + DSIZE);
    void *currentbp = list_beg;
    int res = 0;
    while(1) {
        if ((currentbp) == list_end) {
            //currentbp = heap_back_hdr + 3 * DSIZE;
            extend_heap(size);
            currentbp = list_beg;
            continue;
        }
        while((res = coalesce_block(currentbp)) == 1) ;
        /* vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
        if(GET_SIZE(HDRP(currentbp)) >= size) {
            cut_block(currentbp, size);         
            //printf("malloc call erase\n");
            erase(currentbp);
            SET(currentbp, 1);
            
            //printf("malloc: %p -- %p, \t%ld bytes\n", currentbp - 3 * DSIZE, currentbp - 3 * DSIZE + size, size);
            
            return currentbp - 2 * DSIZE;
            /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ */
        }
        else if (res == -1) {
            currentbp = SUCC(currentbp);
        }
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    if (ptr == NULL)
        return;
    SET(ptr + 2 * DSIZE, 0);
    insert_end(ptr + 2 * DSIZE);
   // printf("free call coal\n");
    //coalesce_block(ptr + 2 * DSIZE);

    //printf("free: %p -- %p, \t%ld bytes\n", ptr - 1 * DSIZE, ptr - 1 * DSIZE + GET_SIZE(HDRP(ptr + 2 * DSIZE)), GET_SIZE(HDRP(ptr + 2 * DSIZE)));

    return;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *newptr = NULL;
    void *bp = ptr + 2 * DSIZE;
    if (ptr == NULL)
        return mm_malloc(size);

    if (size == 0)
        return mm_free(ptr), NULL;
    
    size = ALIGN(size + DSIZE);

    /* If the next block is free, try to coalesce them instead of allocating a new block */
    /* coalesce block until it can not coalesce anymore or the block size can satisfy the request */
    //printf("rea call coal\n");
    while(GET_SIZE(HDRP(bp)) < size && coalesce_block(bp) == 1) {
        ;
    }
    if (GET_SIZE(HDRP(bp)) >= size) {
        cut_block(bp, size);
        SET(bp, 1);
        return ptr;
    }

    /* If reach here, it means the current block can not satisfy the request */
    /* So, we need to allocate a new block and copy the data from the old block */

    newptr = mm_malloc(size - DSIZE);
    if (newptr == NULL) {
        //SET(ptr, 1);
        return NULL;
    }
    memmove(newptr, ptr, GET_SIZE(HDRP(bp)) - DSIZE);
    WRITE(HDRP(newptr + 2 * DSIZE), PACK(size, 1));
    mm_free(ptr);
    return newptr;
}
/* !!! END OF EXPLICIT FREE LIST IMPLEMENTATION !!! */

#elif IMPLEMENTATION == 3 /* use segregated free list, segregated fit, which is the same as c-standard malloc implementation */
/* !!! BEGINNING OF EXPLICIT FREE LIST IMPLEMENTATION !!! */

/* Abandoned */

/* END OF EXPLICIT FREE LIST IMPLEMENTATION */
#endif













