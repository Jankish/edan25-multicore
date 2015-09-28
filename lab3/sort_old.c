#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdatomic.h>

#define NUM_THREADS	4

int counter = 1; 

static double sec(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return(t.tv_usec);
	//return (1.0e-6*t.tv_usec + t.tv_sec);
}

struct arguments 
{
	void*		base;
	size_t		n;
	size_t		s;
	int		(*cmp)(const void*, const void*);// Behaves like strcmp
	pthread_t*	threads;

};

void par_sort(
		void*		base,	// Array to sort.
		size_t		n,	// Number of elements in base.
		size_t		s,	// Size of each element.
		int		(*cmp)(const void*, const void*), // Behaves like strcmp
		pthread_t* threads)

{
	int pivot_mid = n / 2;
	int pivot_left = pivot_mid / 2;
	int pivot_right = pivot_mid + pivot_left;

	printf("mid -> %d \t left -> %d \t right -> %d\n", pivot_mid, pivot_left,pivot_right); 
}

static int cmp(const void* ap, const void* bp)
{	
	/* you need to modify this function to compare doubles. */
	const double*	a = ap;
	const double*	b = bp;
	return *a - *b;
}

int main(int ac, char** av)
{
	//int		n = 2000000;
	int		n = 100;
	int		i;
	double*		a;
	double		start, end;
	pthread_t	threads[NUM_THREADS];

	if (ac > 1)
		sscanf(av[1], "%d", &n);

	srand(getpid());

	a = malloc(n * sizeof a[0]);
	for (i = 0; i < n; i++)
		a[i] = rand();


	start = sec();

//#ifdef PARALLEL
	par_sort(a, n, sizeof a[0], cmp, threads);
//#else

	//qsort(a+(n/2), n/2, sizeof a[0], cmp);
//#endif

//	qsort(a, n, sizeof a[0], cmp);
//	for (int i = 0; i < n; i++)
//		printf(" %*lf\n", 30, *a++);

	end = sec();

	printf("%1.2f micro s\n", end - start);

	free(a);

	return 0;
}
