
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

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
#define set(p,v) ((*(unsigned int *)(p))= v)
#define get_ptr(p) (*(unsigned int *)(p))
#define get_size(p) (get_ptr(p)&(~0x7))
#define get_alloc(p) (get_ptr(p) &0x1)
#define findh(p) ((char*)(p) - 4)
#define findf(p) ((char*)(p)+get_size(p) - 8)
#define next(p) ((char*)(p) + get_size(p) -4)
#define pre(p) ((char*)(p) - get_size(p) -8)
/* 
 * mm_init - initialize the malloc package.
 */
void* extend_heap(int siz);
void* coalesce(void* bp);
void* first_fit(int siz);
static char* heap_list;

int mm_init(void)
{
    if ((heap_list = mem_sbrk(16)) == (void*)-1)
        return -1;

    set(heap_list + 4, (8 | 1));
    set(heap_list + 8, (8 | 1));
    set(heap_list + 12, (0 | 1));
    heap_list += 8;

    if (extend_heap((1 << 12) / 4) == NULL)
        return -1;

    return 0;
}
/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int siz,exsize;
    char* bp;
    if(size ==0) return NULL;
    if(size<=8) siz=16;
    else siz=8*((siz+15)/8);
    if((bp = first_fit(siz))!=NULL)
    {
	    place(bp,siz);
	    return bp;
    }
    if(siz>(1<<12)) exsize=siz;
    else exsize=(1<<12);
    if((bp = extend_heap(exsize/4))== NULL) return NULL; 
    place(bp,siz);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr){
	if(ptr==0) return;
	int size = get_size(findh(ptr));
	set(findh(ptr),(size|0));
	set(findf(ptr),(size|0));
	coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}




void* extend_heap(int siz)
{
    char* bp;
    int size;
    if (siz % 2 == 0)
        size = siz * 4;
    else
        size = (siz + 1) * 4;

    if ((bp = mem_sbrk(size)) == (void*)-1)
        return NULL;

    set(findh(bp), (size | 0));
    set(findf(bp), (size | 0));
    set(findh(next(bp)), (0 | 1));

    return coalesce(bp);
}

void place(void* bp,int siz)
{
	int size=get_size(findh(bp));
	if(size-siz>=16)
	{
		set(findh(bp),(siz|1));
		set(findf(bp),(siz|1));
		bp=next(bp);
		set(findh(bp),((size-siz)|0));
		set(findf(bp),((size-siz)|0));
	}
	else
	{
		set(findh(bp),(size|1));
		set(findf(bp),(size|1));
	}
}

void* coalesce(void* bp)
{
	int pree = get_alloc(findf(pre(bp)));
	int nextt = get_alloc(findh(next(bp)));
	int size = get_size(findh(bp));
	if(pree && nextt) return bp;
	else if(pree && !nextt)
	{
		size+=get_size(findh(next(bp)));
		set(findf(bp),(size|0));
		set(findf(bp),(size|0));
	}
	else if(!pree && nextt)
	{
		size+=get_size(findh(pre(bp)));
		set(findf(bp),(size|0));
		set(findh(pre(bp)),(size|0));
		bp=pre(bp);
	}
	else
	{
		size+=get_size(findh(pre(bp)))+get_size(findf(next(bp)));
		set(findf(next(bp)),(size|0));
		set(findh(pre(bp)),(size|0));
		bp=pre(bp);
	}
	return bp;
}

void* first_fit(int siz)
{
    void* bp;
    for (bp = heap_list; get_size(findh(bp)) > 0; bp = next(bp))
    {
        if (get_size(findh(bp)) >= siz && !get_alloc(findh(bp)))
            return bp;
    }
    return NULL;
}












