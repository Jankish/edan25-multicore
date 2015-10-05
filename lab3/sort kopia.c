#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>

static int thread_count = 4;

static double sec(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return(t.tv_usec);
	//return (1.0e-6*t.tv_usec + t.tv_sec);
}

void par_sort(
	void*		base,	// Array to sort.
	size_t		n,	// Number of elements in base.
	size_t		s,	// Size of each element.
	int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{
	if (thread_count > 2) {
		// Find new pivot
		// start two new threads and make the threads call par_sort
		// thread_count - 2
		// (left and right)
	} else if (thread_count > 0) {
		// Find new pivot
		// start 1 new thread 
		// thread_count - 1
	} 
	//qsort(base, n, s, cmp);
	//join
	//thread_count + 1
}

static int cmp(const void* ap, const void* bp)
{	
	/* you need to modify this function to compare doubles. */
	const double*	a = ap;
	const double*	b = bp;
	return *a - *b;
//	return 0;
}

int main(int ac, char** av)
{
	int		n = 20;
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
//	par_sort(a, n, sizeof a[0], cmp);
//#else
	qsort(a, n, sizeof a[0], cmp);
//#endif

	printf(" %*lf\n", 20, *(a+10));
	printf("vector = \n");
	for (int i = 0; i < n; i++)
		printf(" %*lf\n", 30, *a++);
	
	end = sec();

	printf("%1.2f micro s\n", end - start);

	free(a);

	return 0;
}
