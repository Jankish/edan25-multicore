#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>

#define NUM_THREAD	(1)

static int counter = 0;
pthread_t threads[1];

typedef struct 
{
	double* base;
	int  size;
	size_t seg_size;
	int (*cmp)(const void*, const void*);
	int thread_id;
} data_t ;

static double sec(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	//return (t.tv_usec);
	return (1.0e-6*t.tv_usec + t.tv_sec);
}

void* sort_seg(void* segment)
{
	printf("visiting sort_seg\n Counter is %d\n", counter);	
	//data_t 
	//start new thread
	//par_sort();
	data_t* data = (data_t*) segment;
	pthread_create(&threads[data->thread_id],NULL, qsort, (void*)&data);
	//qsort(data->base, data->size, data->seg_size, data->cmp);
	////pthread_join(threads[data->thread_id], NULL);
	/*if (counter < NUM_THREAD) {
		data_t left_seg;
		data_t right_seg;
		left_seg.base = data->base;
		left_seg.size = data->size;
		left_seg.seg_size = data->seg_size;
		left_seg.cmp = data->cmp;
		left_seg.thread_id = data->thread_id;

		right_seg.base = data->base + (data->size/2);
		right_seg.size = data->size - (data->size/2);
		right_seg.seg_size = data->seg_size;
		right_seg.cmp = data->cmp;
		right_seg.thread_id = ++counter;

		printf("Creating thread with id %d\n", right_seg.thread_id);
		pthread_create(&threads[right_seg.thread_id], NULL, sort_seg, (void*)&left_seg);
		sort_seg((void*)&left_seg);
	} else {
		printf("Thread id %d visits qsort\n", data->thread_id);
		printf("Base is %lf Size is %d seg_size is %zu Thread id is %d\n",
			data->base, data->size, data->seg_size, data->thread_id);	
		qsort(data->base, data->size, data->seg_size, data->cmp);
		//pthread_join(threads[data->thread_id],NULL);
	}*/
	return NULL;
}


void par_sort(
	void*		base,	// Array to sort.
	size_t		n,	// Number of elements in base.
	size_t		s,	// Size of each element.
	int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{
	//calc parameters
	
	data_t data;
	data.base = base;
	data.size = n;
	data.seg_size = s;
	data.cmp = cmp;
	data.thread_id = ++counter;
	printf("Creating thread with id %d\n", data.thread_id);

	//pthread_create(&threads[counter], NULL, sort_seg, (void*) &data);
	sort_seg((void*)&data);
/*	for (int i = 1; i <= counter; i++) {
		pthread_join(threads[i], NULL);
	}*/

}


static int cmp(const void* ap, const void* bp)
{	
	/* you need to modify this function to compare doubles. */

	return 0; 
}

int main(int ac, char** av)
{
	int		n = 100;
	int		i;
	double*		a;
	double		start, end;

	if (ac > 1)
		sscanf(av[1], "%d", &n);

	srand(getpid());

	a = malloc(n * sizeof a[0]);
	for (i = 0; i < n; i++)
		a[i] = rand();

	start = sec();

//#ifdef PARALLEL
	par_sort(a, n, sizeof a[0], cmp);
//#else
	//qsort(a, n, sizeof a[0], cmp);
//#endif
	printf("\nPARALLEL SORT\n\n");
	for(int j = 0; j < n; j++)
		printf("%*lf\n",30,*a++);

	end = sec();

	printf("%1.2f s\n", end - start);

	free(a);

	return 0;
}
