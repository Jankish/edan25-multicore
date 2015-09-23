#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>

static double sec(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return (1.0e-6*t.tv_usec + t.tv_sec);
}

void par_sort(
	void*		base,	// Array to sort.
	size_t		n,	// Number of elements in base.
	size_t		s,	// Size of each element.
	int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{
	
	
	
	
	
	
	
	
}

static int cmp(const void* ap, const void* bp)
{	
	/* you need to modify this function to compare doubles. */
//	assert(*ap != NULL);
//	assert(*bp != NULL);
	const double*	a = ap;
	const double*	b = bp;
//	if (*a > *b)
//		return 1;
//	else if (*a < *b)
//		return -1;
//	else
//		return 0;
	return *a - *b;
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

#ifdef PARALLEL
	par_sort(a, n, sizeof a[0], cmp);
#else
	qsort(a, n, sizeof a[0], cmp);
#endif

	printf("vector = \n");
	for (int i = 0; i < n; i++)
		printf(" %*lf\n", 30, *a++);
	
	end = sec();

	printf("%1.2f s\n", end - start);

	free(a);

	return 0;
}
