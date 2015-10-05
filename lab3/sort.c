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

static size_t thread_count = 4;
static int t_count = 0;
pthread_t threads[4];
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
	return(t.tv_sec * 1000000 + t.tv_usec);
}

void swap(double a[], int i, int j)
{
	double temp = a[i];
	a[i] = a[j];
	a[j] = temp;
}


int partition(double a[], int low, int hi)
{
	int b = low;
	int pos = (int) (low + (hi-low + 1)*(1.0*rand()/RAND_MAX));
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
	qsort(data->part, data->size, data->e_size, data->cmp);
	return NULL;
}

static void* in_sort(void* data)
{
	size_t mid;
	printf("in_sort t_count = %d\n", t_count);
	s_data* d = (s_data*) data;
	
	if (t_count >= thread_count || (d->size <= d->size / thread_count)) {
		printf("t_count = %d\n", t_count);
		deep_sort((void*) d);
		pthread_exit(NULL);
	}
	mid = partition(d->part, 0, d->size);
	s_data ld, rd;
	ld.part = d->part;
	ld.size = mid;
	ld.e_size = d->e_size;
	ld.cmp = d->cmp;
	
	rd.part = d->part + mid;
	rd.size = d->size - mid;
	rd.e_size = d->e_size;
	rd.cmp = d->cmp;

	s_data data_array[2];
	data_array[0] = ld;
	data_array[1] = rd;
	
	pthread_t th[2];
	
	for ( int t = 0; t < 2; t++) {
		t_count++;
		if(pthread_create(&th[t], NULL, in_sort, (void*) &data_array[t]) != 0) {
			fprintf(stderr, "Error: pthread creation went wrong\n");
			exit(1);
		}
	}
	int status;
	for (int t = 0; t < 2; t++) {
		t_count--;
		status = pthread_join(th[t], NULL);
		if (status != 0)
			printf("could not join with thread %d:\n", status);
		
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
	s_data data;
	data.part = base;
	data.size = n;
	data.e_size = s;
	data.cmp = cmp;
	if(pthread_create(&threads[t_count++], NULL, in_sort, (void*) &data) != 0) {
		fprintf(stderr, "Error: pthread creation went wrong\n");
		exit(1);
	}
	int status;
	status = pthread_join(threads[--t_count], NULL);
	if (status != 0)
		printf("could not join with thread %d:\n", status);
}



static int cmp(const void* ap, const void* bp)
{	
	/* you need to modify this function to compare doubles. */
	const double*	a = ap;
	const double*	b = bp;
//	return *a - *b;
	
	/*/
	 www.gnu.org/software/libc/manual/html_node/
	 Comparison-Functions.html#Comparison-Functions
	 */
	return (*a > *b) - (*a < *b);
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
	start = sec();
	par_sort(a, n, sizeof a[0], cmp);
	end = sec();
	printf("vector = \n");
	for (int i = 0; i < n; i++)
		printf(" %*lf\n", 30, *a++);
	
	printf("%1.2f millisec s\n", (end - start)/1000.0);
//#else
	printf("\nSEQUENTIAL SORT\n\n");
	start = sec();
	qsort(b, n, sizeof b[0], cmp);
	end = sec();
	printf("vector = \n");
	for (int i = 0; i < n; i++)
		printf(" %*lf\n", 30, *b++);
	
	printf("%1.2f millisec s\n", (end - start)/1000.0);
//#endif

	

//	free(a);
	return 0;
}
