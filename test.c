#include <stdio.h>
#include <stdlib.h>
#include "malloc.h"

void test1()
{
	printf("////////////////////////////////\n");
	char *p1 = MyMalloc(24);
	ft_strcpy(p1, "STR_P1");
	printspace(&MallocSpace);
	print_chain(&MallocSpace);
	printf("\n");
	printf("////////////////////////////////\n");
	char *p2 = MyMalloc(0);
	printf("%p\n", p2);
	ft_strcpy(p2, "STR_P2");
	printspace(&MallocSpace);
	print_chain(&MallocSpace);
	printf("\n");
	printf("////////////////////////////////\n");
	ft_strcpy(p1, "STR_COPY_TO_P1");
	printspace(&MallocSpace);
	print_chain(&MallocSpace);
	printf("\n");
	printf("////////////////////////////////\n");
	char *p3 = MyMalloc(4096);
	printf("%p\n", p3);
	ft_strcpy(p3, "STR_P3");
	printspace(&MallocSpace);
	print_chain(&MallocSpace);
	printf("\n");
	printf("////////////////////////////////\n");
	MyFree(p2);
	printspace(&MallocSpace);
	print_chain(&MallocSpace);
	printf("\n");
	printf("////////////////////////////////\n");
	MyFree(p1);
	printspace(&MallocSpace);
	print_chain(&MallocSpace);
	printf("\n");
	printf("////////////////////////////////\n");
	MyFree(p3);
	printspace(&MallocSpace);
	print_chain(&MallocSpace);
	printf("\n");
	printf("////////////////////////////////\n");
}
/////////////////////////////////////////////////////////////

pthread_mutex_t test_lock = PTHREAD_MUTEX_INITIALIZER;

void print_message_function(void *ptr)
{
	printf("%s\n", (char *)ptr);
	char *p = MyMalloc(24);
	ft_strcpy(p, ptr);
	if (pthread_mutex_lock(&test_lock) == 0)
	{
		printf("//////%s\n", (char *)ptr);
		printspace(&MallocSpace);
		print_chain(&MallocSpace);
		pthread_mutex_unlock(&test_lock);
	}
	MyFree(p);
	if (pthread_mutex_lock(&test_lock) == 0)
	{
		printf("//////%s\n", (char *)ptr);
		printspace(&MallocSpace);
		print_chain(&MallocSpace);
		pthread_mutex_unlock(&test_lock);
	}
}
void test2()
{

	pthread_t thread1, thread2;
	char *message1 = "thread1";
	char *message2 = "thread2";
	int ret_thrd1, ret_thrd2;
	ret_thrd1 = pthread_create(&thread1, NULL, (void *)&print_message_function, (void *)message1);
	ret_thrd2 = pthread_create(&thread2, NULL, (void *)&print_message_function, (void *)message2);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
}
int main()
{
	test1();
}

