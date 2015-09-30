#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

static size_t thread_count = 16;
static int t_count = 0;
pthread_t threads[16];
int c = 0;

typedef struct s_data
{
	double*		part;	// Array to sort.
	int		size;	// Number of elements in base.
	size_t		e_size;	// Size of each element.
	int		(*cmp)(const void*, const void*);// Behaves like strcmp
} s_data;

static double sec(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return(t.tv_usec);
}

static void* deep_sort(void* d_segment)
{
	s_data* data = (s_data*)d_segment;
	qsort(data->part, data->size, data->e_size, data->cmp);
	return NULL;
}

static void* in_sort(void* data)
{
	s_data* d = (s_data*) data;
	if (t_count < thread_count && d->size > 10) {
		s_data ld, rd;
		ld.part = d->part;
		ld.size = (d->size / 2);
		ld.e_size = d->e_size;
		ld.cmp = d->cmp;
	
		rd.part = d->part + (d->size / 2);
		rd.size = d->size - (d->size / 2);
		rd.e_size = d->e_size;
		rd.cmp = d->cmp;

		if(pthread_create(&threads[t_count++], NULL, in_sort, (void*) &ld) != 0) {
			fprintf(stderr, "Error: pthread creation went wrong\n");
			exit(1);
		}
		in_sort((void*) &rd);
	} else {
		deep_sort(data);
	}
	return NULL;
}


static void par_sort(
	void*		base,	// Array to sort.
	size_t		n,	// Number of elements in base.
	size_t		s,	// Size of each element.
	int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{
	printf("PAR SORT\n\n");
	int status;
	s_data data;
	data.part = base;
	data.size = n;
	data.e_size = s;
	data.cmp = cmp;
	in_sort((void*)&data);
	
	for (int i = 0; i < t_count; i++) {
		status = pthread_join(threads[i], NULL);
		if (status != 0)
			printf("could not join with thread %d: %s\n", i, strerror(status));
	}
	deep_sort((void*)&data);
//	qsort(data.part, data.size, data.e_size, data.cmp);
	
}



static int cmp(const void* ap, const void* bp)
{	
	/* you need to modify this function to compare doubles. */
	const double*	a = ap;
	const double*	b = bp;
//	return *a - *b;
	return (*a > *b) - (*a < *b);
}

int main(int ac, char** av)
{
	int		n = 100;
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
	start = sec();
	printf("\nPARALLEL SORT\n\n");
	par_sort(a, n, sizeof a[0], cmp);
	end = sec();
	printf("vector = \n");
	for (int i = 0; i < n; i++)
		printf(" %*lf\n", 30, *a++);
	
	printf("%1.2f micro s\n", end - start);
//#else
	start = sec();
	printf("\nSEQUENTIAL SORT\n\n");
	qsort(b, n, sizeof b[0], cmp);
	end = sec();
	printf("vector = \n");
	for (int i = 0; i < n; i++)
		printf(" %*lf\n", 30, *b++);
	
	printf("%1.2f micro s\n", end - start);
//#endif

	

//	free(a);
	return 0;
}
