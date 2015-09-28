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

int counter = 0;

static double sec(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return (t.tv_usec);
	//return (1.0e-6*t.tv_usec + t.tv_sec);
}

void sort_seg(data_t* data)
{
	data
}


void par_sort(
	void*		base,	// Array to sort.
	size_t		n,	// Number of elements in base.
	size_t		s,	// Size of each element.
	int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{
	counter += 1;
	//calc parameters
	if (counter < NUM_THREAD) {
		//skapa fler trådar
	} else {
		//qsort
	}


}

struct data {
	void* base;
	size_t n;
	size_t s;
	int (*cmp)(const void*, const void*);
	int thread_id;
};

static int cmp(const void* ap, const void* bp)
{	
	/* you need to modify this function to compare doubles. */

	return 0; 
}

int main(int ac, char** av)
{
	int		n = 2000000;
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

#ifdef PARALLEL
	par_sort(a, n, sizeof a[0], cmp);
#else
	//qsort(a, n, sizeof a[0], cmp);
#endif

	end = sec();

	printf("%1.2f s\n", end - start);

	free(a);

	return 0;
}
