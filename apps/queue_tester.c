#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <queue.h>

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

static void inc_data(queue_t q, void* data)
{
	(void) q;
	int* a = (int*)data;

	(*a)++;
}

static void dequeue_all(queue_t q, void* data)
{
	queue_dequeue(q, &data);
}


/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

void test_queue_multiple(void)
{
	int data1 = 3, data2 = 5, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_multiple ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data1);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data2);
	TEST_ASSERT(queue_length(q) == 0);

}

/* Dequeue empty */
void test_queue_empty(void)
{
    int *ptr;
    queue_t q;

    fprintf(stderr, "*** TEST queue_empty ***\n");

    q = queue_create();
    TEST_ASSERT(queue_dequeue(q, (void**)&ptr) == -1);
}



void test_queue_delete_single(void)
{
	int data = 3;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_single ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	TEST_ASSERT(queue_length(q) == 1);
	TEST_ASSERT(queue_delete(q, &data) == 0);
	TEST_ASSERT(queue_length(q) == 0);
}


void test_queue_delete_multiple(void)
{
	int data1 = 3, data2 = 5, data3 = 7;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_multiple ***\n");

	q = queue_create();
	queue_enqueue(q, &data1);
	queue_enqueue(q, &data2);
	queue_enqueue(q, &data3);
	TEST_ASSERT(queue_delete(q, &data2) == 0);
	TEST_ASSERT(queue_length(q) == 2);
}

void test_queue_destroy(void)
{
    queue_t q;
    fprintf(stderr, "*** TEST queue_destroy_empty ***\n");

    q = queue_create();
    TEST_ASSERT(queue_destroy(q) == 0);
}

void test_queue_iterator()
{
	queue_t q;
	int arr[] = { 1, 2, 3, 4, 5};
	unsigned int i;

	fprintf(stderr, "*** TEST queue_iterator ***\n");

	q = queue_create();
	for (i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
		queue_enqueue(q, &arr[i]);

	queue_iterate(q, inc_data);

	TEST_ASSERT(arr[0] == 2);			
	TEST_ASSERT(arr[2] == 4);			
	queue_iterate(q, dequeue_all);
	TEST_ASSERT(queue_length(q) == 0);
	TEST_ASSERT(queue_destroy(q) == 0);		
}

int main(void)
{
	test_create();
	test_queue_simple();
	test_queue_multiple();
	test_queue_empty();
	test_queue_delete_single();
	test_queue_delete_multiple();
	test_queue_destroy();
	test_queue_iterator();
	return 0;
}