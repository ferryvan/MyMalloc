#include "malloc.h"

//////////////////////////////////////////////////////////////
// 将内存空间申请后初始化为block_size*heads_num的全零空间
static void init_space(CLMallocSpace *space)
{
	if (space == NULL || space->uninitialized == 0)
		return;
	space->spaceHead = mmap_alloc_mem(space->totalSize);
	space->freeBlocksChain = space->spaceHead;
	CLMetaData *head = space->freeBlocksChain;
	ft_bzero(head, space->totalSize);
	head->next = NULL;
	head->size = space->totalSize;

	space->uninitialized = 0;
}

// 找到第一个满足大小的块（first fit），断链返回，找不到则返回NULL
static CLMetaData *search_freeBlocksChain_by_size(CLMallocSpace *space, size_t size)
{
	if (!space->freeBlocksChain)
		return NULL;
	CLMetaData *curr = space->freeBlocksChain;
	CLMetaData *perv = space->freeBlocksChain;
	while (curr)
	{
		if (curr->size >= size)
		{
			//断链
			if (perv == curr)
				space->freeBlocksChain = curr->next;
			else
				perv->next = curr->next;
			return curr;
		}
		perv = curr;
		curr = curr->next;
	}
	return NULL;
}

// 切分一个块为先后newsize和block_size-newsize大小的两个块，返回后面那个块(newblock)的指针
static void *split_block(CLMetaData *block, size_t newSize)
{
	CLMetaData *newBlock = (void *)((char *)block + newSize);

	newBlock->next = NULL;
	newBlock->size = block->size - newSize;

	block->next = NULL;
	block->size = newSize;
	return newBlock;
}

// 按升序插入空闲链表，以便first fit
static void insert_into_freeBlocksChain(CLMallocSpace *space, CLMetaData *block)
{
	if (!space->freeBlocksChain)
	{
		block->next = NULL;
		space->freeBlocksChain = block;
		return;
	}
	CLMetaData *perv = space->freeBlocksChain;
	CLMetaData *curr = space->freeBlocksChain;
	while (curr)
	{
		//找到第一个地址比它大的，插入在前面，形成升序链表
		if (((void *)((char *)(block))) <= ((void *)((char *)(curr))))
		{
			if (perv == curr) //链首情况
				space->freeBlocksChain = block;
			else
				perv->next = block;
			block->next = curr;
			return;
		}
		perv = curr;
		curr = curr->next;
	}
	if (!curr) //链尾情况
	{
		perv->next = block;
		block->next = curr;
		return;
	}
	return;
}

// 插入使用中的链表，直接头插法
static void insert_into_inUseBlocksChain(CLMallocSpace *space, CLMetaData *block)
{
	block->next = space->inUseBlocksChain;
	space->inUseBlocksChain = block;
}

// 尝试在空间分配一个大小为size的块
static void *try_alloc(CLMallocSpace *space, size_t size)
{
	if (space == NULL || space->uninitialized == 1)
		return NULL;
	if (space->freeSpaceSize < size)
		return NULL;

	CLMetaData *curr = NULL;
	curr = search_freeBlocksChain_by_size(space, size);
	if (curr)
	{
		//如果当前空闲块比所需块大小加最小大小还要大，就分裂
		if (curr->size >= size + req2size(0))
		{
			CLMetaData *newBlock = split_block(curr, size);
			insert_into_freeBlocksChain(space, newBlock);
			insert_into_inUseBlocksChain(space, curr);
		}
		else
			insert_into_inUseBlocksChain(space, curr);
		space->freeSpaceSize -= curr->size;
		return block2mem(curr);
	}
	return NULL;
}

// 分配入口
void *alloc_block(size_t size)
{
	void *mem = NULL;
	init_space(&MallocSpace);
	mem = try_alloc(&MallocSpace, req2size(size));
	return mem;
}
//////////////////////////////////////////////////////////////

// 找到活动链表满足当前指针的块，断链返回，找不到则返回NULL
static CLMetaData *search_inUseBlocksChain_by_ptr(CLMallocSpace *space, void *ptr)
{
	if (!space || !ptr)
		return NULL;
	CLMetaData *curr = space->inUseBlocksChain;
	CLMetaData *perv = space->inUseBlocksChain;
	while (curr)
	{
		if (block2mem(curr) == ptr)
		{
			//断链
			if (perv == curr)
				space->inUseBlocksChain = curr->next;
			else
				perv->next = curr->next;
			return curr;
		}
		perv = curr;
		curr = curr->next;
	}
	return NULL;
}

// 合并按地址升序排序的空闲链表中的小内存碎片
void merge_freeBlocksChain_fragment(CLMallocSpace *space)
{
	if (!space || !space->freeBlocksChain)
		return;
	CLMetaData *curr = space->freeBlocksChain;
	while (curr)
	{
		//当前的块的地址空间顺序下一块和空闲链表中下一块相同
		while ((curr->next != NULL) &&
			   (curr->size != 0) &&
			   (((void *)((char *)(curr) + (curr->size))) == ((void *)((char *)(curr->next)))))
		{
			// 合并curr块和它的下一个块
			// 循环合并至最后
			curr->size += curr->next->size;
			curr->next = curr->next->next;
		}
		curr = curr->next;
	}
	return;
}

size_t try_free(CLMallocSpace *space, void *ptr)
{
	if (!space || !ptr)
		return 0;
	CLMetaData *curr = space->inUseBlocksChain;
	curr = search_inUseBlocksChain_by_ptr(space, ptr); //
	if (curr)
	{
		space->freeSpaceSize += curr->size;
		insert_into_freeBlocksChain(space, curr);
		merge_freeBlocksChain_fragment(space);
		return 1;
	}
	return 0;
}

// 释放入口
void free_block(void *ptr)
{
	CLMallocSpace *space = &MallocSpace;
	int success = try_free(space, ptr);

	if (success)
	{
		// 无内存块时，释放整片空间
		if (space->freeSpaceSize == space->totalSize)
		{
			munmap(space->spaceHead, space->totalSize);
			space->inUseBlocksChain = NULL;
			space->freeBlocksChain = NULL;
			space->spaceHead = NULL;
		}
	}
}

//////////////////////////////////////////////////////////////

CLMallocSpace MallocSpace = {NULL, NULL, NULL, MALLOC_SPACE_SIZE, MALLOC_SPACE_SIZE, 1};
pthread_mutex_t my_malloc_lock = PTHREAD_MUTEX_INITIALIZER;

//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////

void *mmap_alloc_mem(size_t size)
{
	void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	return mem == MAP_FAILED ? NULL : mem;
}

void *MyMalloc(size_t size)
{
	void *mem = NULL;
	if (pthread_mutex_lock(&my_malloc_lock) == 0)
	{
		//ft_putstr("here\n");
		mem = alloc_block(size);
		pthread_mutex_unlock(&my_malloc_lock);
	}
	return mem;
}

void MyFree(void *ptr)
{
	if (!ptr)
		return;
	if (pthread_mutex_lock(&my_malloc_lock) == 0)
	{
		free_block(ptr);
		pthread_mutex_unlock(&my_malloc_lock);
	}
}
//////////////////////////////////////////////////////////////
/*
typedef struct CLMallocSpace
{
	CLMetaData *inUseBlocksChain; //活动链表
	CLMetaData *freeBlocksChain;  //空闲链表
	void *spaceHead;
	size_t totalSize;
	size_t freeSpaceSize;
	bool uninitialized; //未初始化的
} CLMallocSpace;
*/

void print_chain(CLMallocSpace *space)
{
	printf("print_chain:\n");

	printf("print_freeBlocksChain:\n");
	CLMetaData *curr = space->freeBlocksChain;
	int cnt = 1;
	while (curr)
	{

		printf("%d:\t%p\n", cnt, (void *)((char *)(curr)));
		printf("size:%ld\n", curr->size);
		ft_putstr(block2mem(curr));
		printf("\n");
		curr = curr->next;
		cnt += 1;
	}

	printf("print_inUseBlocksChain:\n");
	curr = space->inUseBlocksChain;
	cnt = 1;
	while (curr)
	{
		printf("%d:\t%p\n", cnt, (void *)((char *)(curr)));
		printf("size:%ld\n", curr->size);
		ft_putstr(block2mem(curr));
		printf("\n");
		curr = curr->next;
		cnt += 1;
	}
}

void printspace(CLMallocSpace *space)
{
	printf("inUseBlocksChain:\t%p\n", (void *)((char *)(space->inUseBlocksChain)));
	printf("freeBlocksChain:\t%p\n", (void *)((char *)(space->freeBlocksChain)));
	printf("spaceHead:\t%p\n", (void *)((char *)(space->spaceHead)));
	printf("totalSize:\t%ld\n", space->totalSize);
	printf("freeSpaceSize:\t%ld\n", space->freeSpaceSize);
}