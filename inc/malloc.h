#ifndef FANRUI_MALLOC_H
#define FANRUI_MALLOC_H
#endif

#include <sys/mman.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

// 可分配的空间上限
#define PAGE_SIZE ((size_t)getpagesize())
#define MALLOC_SPACE_SIZE 4096 * 256
#define BLOCK_METADATA_SIZE sizeof(CLMetaData)

// 最小分配MIN_SIZE+BLOCK_METADATA_SIZE大小
#define MIN_SIZE (2 * sizeof(size_t))
#define minsize(x) \
	((x < MIN_SIZE) ? MIN_SIZE : x)
#define req2size(x) \
	(minsize(x) + BLOCK_METADATA_SIZE)


// CLMetaData结构体
typedef struct CLMetaData
{
	struct CLMetaData *next;
	size_t size;
} CLMetaData;

// CLMetaData常用函数
#define block2mem(block) \
	((block) ? ((void *)((char *)(block) + BLOCK_METADATA_SIZE)) : NULL)
#define mem2block(mem) \
	((mem) ? ((void *)((char *)(mem)-BLOCK_METADATA_SIZE)) : NULL)
#define datasize(block) \
	((block) ? ((block)->size - BLOCK_METADATA_SIZE) : 0

// 管理结构体
typedef struct CLMallocSpace
{
	CLMetaData *inUseBlocksChain; //活动链表
	CLMetaData *freeBlocksChain;  //空闲链表
	void *spaceHead;
	size_t totalSize;
	size_t freeSpaceSize;
	size_t uninitialized;
} CLMallocSpace;

extern CLMallocSpace MallocSpace;
extern pthread_mutex_t my_malloc_lock;

void MyFree(void *ptr);
void *MyMalloc(size_t size);

void *alloc_block(size_t size);
void free_block(void *ptr);

void *mmap_alloc_mem(size_t size);
void show_mmap_alloc_mem(void);

void print_chain(CLMallocSpace *space);
void printspace(CLMallocSpace *space);


void ft_putnbr(size_t n);
void ft_putnbr16(unsigned long long n);
void ft_putstr(char const *s);
int ft_strlen(char const *s);
void ft_strcpy(char *dest, char const *src);
void *ft_memcpy(void *dest, const void *src, size_t n);
void ft_bzero(void *p, size_t size);