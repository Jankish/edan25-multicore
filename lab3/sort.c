#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

#define MAX_THREADS (4)
static unsigned int max_depth = 0;
static int count = 0;

typedef struct s_data
{
	double*		part;	// Array to sort.
	int		size;	// Number of elements in base.
	int		tid; // thread number
	int		depth; //
	int		max;
	int		(*cmp)(const void*, const void*);// Behaves like strcmp
	size_t		e_size;	// Size of each element.
} s_data;

static double sec(void)
{
	struct timeval	tv;
	
	gettimeofday(&tv, NULL);
	
	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

static void swap(double* a, int i, int j)
{
	double temp = a[i];
	a[i] = a[j];
	a[j] = temp;
}

static int partition(double a[], int low, int hi)
{
	int b = low;
	int pos = (int)(low + hi) / 2;
	double pivot = a[pos];
	swap(a, pos, hi);
	for (int i = low; i < hi; i ++) {
		if (a[i] < pivot) {
			swap(a, i, b);
			b ++;
		}
	}
	swap(a, hi, b);
	return b;
}

static void* deep_sort(void* d_segment)
{
	s_data* data = (s_data*)d_segment;
//	printf("thead tid = %d\n", data->tid);
	qsort(data->part, data->size, data->e_size, data->cmp);
	return NULL;
}

static void* in_sort(void* data)
{
	int	mid;
	int	status = -1;
	s_data* d = (s_data*) data;
	pthread_t t = NULL;
	
	if (d->depth < max_depth) {
		//choose a very good pivot element
		mid = partition(d->part, 0, d->size - 1);
		//left partition
		s_data rd, ld;
		
		//right partition
		rd.part = d->part + mid;
		rd.size = d->size - mid;
		rd.e_size = d->e_size;
		rd.depth = d->depth + 1;
		rd.tid = count;
		rd.cmp = d->cmp;
		
		count++;
		
		status = pthread_create(&t, NULL, in_sort, &rd);
		if (status != 0) {
			fprintf(stderr, "Error: pthread creation went wrong\n");
			exit(1);
		}
		
		
		ld.part = d->part;
		ld.size = mid;
		ld.tid = d->tid;
		ld.depth = d->depth + 1;
		ld.e_size = d->e_size;
		ld.cmp = d->cmp;
		
		
		in_sort((void*) &ld);
		
		if (status != -1) {
			if ((pthread_join(t, NULL)) != 0) {
				fprintf(stderr, "Could not join with thread\n");
				exit(1);
			}
		}
	} else {
		deep_sort((void*) data);
	}
	
	return NULL;
}

static void par_sort(
	void*		base,	// Array to sort.
	size_t		n,	// Number of elements in base.
	size_t		s,	// Size of each element.
	int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{
	s_data data;
	data.part = base;
	data.size = n;
	data.tid = count;
	data.depth = 0;
	data.cmp = cmp;
	data.e_size = s;
	
	count++;
	in_sort((void*) &data);
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
	int		n = 50000; // equaly fast at 20k entries
	int		i;
	double*		a, *b;
	double		start, end;
	if (ac > 1)
		sscanf(av[1], "%d", &n);

	srand(getpid());

	a = malloc(n * sizeof a[0]);
	b = malloc(n * sizeof b[0]);
	for (i = 0; i < n; i++)
		a[i] = b[i] = rand();
	

//#ifdef PARALLEL
	printf("\nPARALLEL SORT\n\n");
	max_depth = (unsigned int) (ceil(log((double) MAX_THREADS) / log(2.0)));
	start = sec();
	par_sort(a, n, sizeof a[0], cmp);
	end = sec();
//	printf("vector = \n");
//	for (int i = 0; i < n; i++)
//		printf(" %*lf\n", 30, a[i]);
	
	printf("%3.6f s\n", (end - start));
//#else
	printf("\nSEQUENTIAL SORT\n\n");
	start = sec();
	qsort(b, n, sizeof b[0], cmp);
	end = sec();
//	printf("vector = \n");
//	for (int i = 0; i < n; i++)
//		printf(" %*lf\n", 30, b[i]);
	
	printf("%3.6f s\n", (end - start));
//#endif

	free(a);
	free(b);
	return 0;
}
