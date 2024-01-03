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

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc)) //block의 size와 allocation 여부를 packing

#define GET(p) (*(size_t *)(p)) //address p에 있는 word를 read
#define PUT(p, val) (*(size_t *)(p) = (val)) //address p에 있는 word를 write

#define GET_SIZE(p) (GET(p) & ~0x7) //block의 size는 바이트 단위
#define GET_ALLOC(p) (GET(p) & 0x1) //header나 footer의 마지막 1비트는 0(free), 1(allocated)를 나타내는 비트

#define HDRP(bp) ((char *)(bp) - WSIZE) //block address를 통해 header의 address 계산
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) //block address를 통해 footer의 address 계산

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE)) //현재 block address + 현재 블록 size -> next block의 address 계산
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE)) //현재 block address - prev 블록 size -> prev block의 address 계산

#define NUM_SEGLIST 20
#define NEXT_SEG(bp) ((char *)(bp) + WSIZE) 
#define PREV_SEG(bp) ((char *)(bp))
#define SET_PTR(p, ptr) (*(unsigned int *)(p) = (unsigned int)(ptr))
#define GET_PREV_PTR(ptr) (*(char **)(ptr))
#define GET_NEXT_PTR(ptr) (*(char **)(NEXT_SEG(ptr)))

void *start_of_heap;
void *seg_free_lists[NUM_SEGLIST];
static void *extend_heap(size_t num_words);
static void *coalesce(void *bp);
static void *find_fit(size_t size);
static void *place(void *bp, size_t asize);
static void delete(void *bp);
static void insert(void *bp);

int get_MSB_pos(size_t size, int idx){
    while(size > 1 && idx < NUM_SEGLIST - 1){
        size >>= 1;
        idx++;
    }
    return idx;
}

static void *extend_heap(size_t num_words)
{
    size_t size = (num_words + num_words % 2) * WSIZE;
    char* extend_bp = mem_sbrk(size);
    if(extend_bp == (void *)-1) return NULL;
    PUT(HDRP(extend_bp), PACK(size, 0));
    PUT(FTRP(extend_bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(extend_bp)),PACK(0, 1));
    insert(extend_bp);
    return coalesce(extend_bp);
}

static void *coalesce(void *bp)
{
    size_t prev_is_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
    size_t next_is_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t block_size = GET_SIZE(HDRP(bp));
    if(!prev_is_alloc && next_is_alloc){
        delete(PREV_BLKP(bp));
        delete(bp);
        block_size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(block_size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(block_size, 0));
        insert(PREV_BLKP(bp));
        return PREV_BLKP(bp);
    }
    if(prev_is_alloc && !next_is_alloc){
        delete(bp);
        delete(NEXT_BLKP(bp));
        block_size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(block_size, 0));
        PUT(FTRP(bp), PACK(block_size, 0));
        insert(bp);
        return bp;
    }
    if(!prev_is_alloc && !next_is_alloc){
        delete(PREV_BLKP(bp));
        delete(bp);
        delete(NEXT_BLKP(bp));
        block_size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(block_size, 0));
        PUT(FTRP(PREV_BLKP(bp)), PACK(block_size, 0));
        insert(PREV_BLKP(bp));
        return PREV_BLKP(bp);
    }
    return bp;
}

static void *find_fit(size_t size)
{
    /*find first fit in the segregated free list*/
    /*
    int index = get_MSB_pos(size, 0);
    while(index < NUM_SEGLIST){
        char* ptr = seg_free_lists[index];
        while(ptr != NULL){
            if(GET_SIZE(HDRP(ptr)) >= size)   
                return ptr;
            ptr = GET_PREV_PTR(ptr);
        }
        index++;
    }
    return NULL;
    */
    /*find best fit in the segregated free list*/
    
    int index = get_MSB_pos(size, 0);
    while(index < NUM_SEGLIST){
        char* ptr = seg_free_lists[index];
        char* bestptr = NULL;
        size_t min = (1 << 31);
        while(ptr != NULL){
            if(GET_SIZE(HDRP(ptr)) >= size){
                if(min > GET_SIZE(HDRP(ptr)) - size){
                    min = GET_SIZE(HDRP(ptr)) - size;
                    bestptr = ptr;
                }
            }
            ptr = GET_PREV_PTR(ptr);
        }
        if(bestptr != NULL)
            return bestptr;
        index++;
    }
    return NULL;
    
}

static void *place(void *bp, size_t asize)
{
    size_t block_size = GET_SIZE(HDRP(bp));
    delete(bp);
    if(block_size - asize <= 2 * DSIZE){
        PUT(HDRP(bp), PACK(block_size, 1));
        PUT(FTRP(bp), PACK(block_size, 1));
    }
    else if(asize > 100){ 
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(block_size - asize, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(block_size - asize, 0));
        insert(NEXT_BLKP(bp));
    }
    else{
        PUT(HDRP(bp), PACK(block_size - asize, 0));
        PUT(FTRP(bp), PACK(block_size - asize, 0));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(asize, 1));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(asize, 1));
        insert(bp);
        bp = NEXT_BLKP(bp);
    }
    return bp;
}

static void insert(void *bp)
{
    /*LIFO*/
    size_t size = GET_SIZE(HDRP(bp));
    int idx = get_MSB_pos(size, 0);
    char *prev = seg_free_lists[idx];
    SET_PTR(NEXT_SEG(bp), NULL);
    SET_PTR(PREV_SEG(bp), NULL);
    if(prev != NULL){
        SET_PTR(NEXT_SEG(prev), bp); 
        SET_PTR(PREV_SEG(bp), prev);
    }
    seg_free_lists[idx] = bp;
}

static void delete(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));
    int idx = get_MSB_pos(size, 0);
    char* prev = GET_PREV_PTR(bp);
    char* next = GET_NEXT_PTR(bp);
    if(prev == NULL && next == NULL)
        seg_free_lists[idx] = NULL;
    else if(prev != NULL && next == NULL){
        seg_free_lists[idx] = prev;
        SET_PTR(NEXT_SEG(prev), NULL);
    }
    else if(prev == NULL && next != NULL)
        SET_PTR(PREV_SEG(next), NULL);
    else{
        SET_PTR(NEXT_SEG(prev), next);
        SET_PTR(PREV_SEG(next), prev);
    }
}
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /*Initalize seglist*/
    for(int i = 0; i < NUM_SEGLIST; i++)
        seg_free_lists[i] = NULL;
    /*memory heap에 memory 할당*/
    if((start_of_heap = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    PUT(start_of_heap, 0); //Alignment padding
    PUT(start_of_heap + WSIZE, PACK(DSIZE, 1)); //Prologue header
    PUT(start_of_heap + 2*WSIZE, PACK(DSIZE, 1)); //Prologue footer
    PUT(start_of_heap + 3*WSIZE, PACK(0, 1)); //Epilogue header
    start_of_heap += 2*WSIZE;
    
    return 0;
}


/* 
 * mm_malloc - Align the block size(including overhead) and find a fit in the free list.
 *             if the fit doesn't exist, extend the heap and place the block.
 */
void *mm_malloc(size_t size)
{
    size_t align_size;
    size_t extend_size;
    char* bp;
    if(size == 0) return NULL;
    align_size = ALIGN(size + DSIZE);
    bp = find_fit(align_size);
    if(bp != NULL){
        bp = place(bp, align_size);
        return bp;
    }
    
    extend_size = MAX(CHUNKSIZE, align_size);
    bp = extend_heap(extend_size/WSIZE);
    if(bp == NULL) return NULL;
    bp = place(bp, align_size);
    return bp;
}

/*
 * mm_free - Free the block and coalesce the free blocks.
 */
void mm_free(void *ptr)
{
    PUT(HDRP(ptr), PACK(GET_SIZE(HDRP(ptr)), 0));
    PUT(FTRP(ptr), PACK(GET_SIZE(FTRP(ptr)), 0));
    insert(ptr);
    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    
    if(ptr == NULL) return mm_malloc(size);
    else if(size == 0) {mm_free(ptr); return NULL;}
    size_t cur_size = GET_SIZE(HDRP(ptr));
    char* old_ptr = ptr;
    char* new_ptr;
    size_t asize = ALIGN(size + DSIZE);
    
    if(cur_size >= asize){
        return old_ptr;
    }

    else{
        size_t add_b_size = cur_size;
        if(!GET_ALLOC(HDRP(NEXT_BLKP(old_ptr)))) {
            add_b_size += GET_SIZE(HDRP(NEXT_BLKP(old_ptr)));
            if(add_b_size >= asize){
                delete(NEXT_BLKP(old_ptr));
                PUT(HDRP(old_ptr), PACK(add_b_size, 1));
                PUT(FTRP(old_ptr), PACK(add_b_size, 1));
                return old_ptr;
            }
        }
        if(!GET_ALLOC(HDRP(PREV_BLKP(old_ptr)))){
            add_b_size += GET_SIZE(HDRP(PREV_BLKP(old_ptr)));
            if(add_b_size >= asize){
                delete(PREV_BLKP(old_ptr));
                if(!GET_ALLOC(HDRP(NEXT_BLKP(old_ptr)))){
                    if(add_b_size - GET_SIZE(HDRP(NEXT_BLKP(old_ptr))) < asize) delete(NEXT_BLKP(old_ptr));   
                    else add_b_size -= GET_SIZE(HDRP(NEXT_BLKP(old_ptr)));
                }
                new_ptr = PREV_BLKP(old_ptr);
                memmove(new_ptr, old_ptr, cur_size);
                PUT(HDRP(new_ptr), PACK(add_b_size, 1));
                PUT(FTRP(new_ptr), PACK(add_b_size, 1));
                return new_ptr;
            }
        }
    }
    
    new_ptr = mm_malloc(asize);
    if(new_ptr == NULL) return NULL;
    size_t copy_size = MIN(asize, cur_size);
    memmove(new_ptr, old_ptr, copy_size);
    mm_free(old_ptr);
    return new_ptr;
}














