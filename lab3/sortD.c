#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>

static size_t thread_count = 4;
static void in_sort(
		    void*		base,	// Array to sort.
		    size_t		n,	// Number of elements in base.
		    size_t		s,	// Size of each element.
		    int		(*cmp)(const void*, const void*), // Behaves like strcmp
		    pthread_t*	t, // thread array
		    size_t		t_size);



static double sec(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return(t.tv_usec);
	//return (1.0e-6*t.tv_usec + t.tv_sec);
}

static void deep_sort(
	       void*		base,	// Array to sort.
	       size_t		n,	// Number of elements in base.
	       size_t		s,	// Size of each element.
	       int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{
	qsort(base, n, s, cmp);
}


static void start_new_thread(
		      pthread_t*	t_holder,
		      size_t		holder_size,
		      void		(*deep_sort)(const void*, const size_t*, const size_t*,
						     const void*),
		      void *		arg)
{
	pthread_t* pt = NULL;
	int pos = holder_size - thread_count;
	if (pos < holder_size) {
		if(pthread_create(pt, NULL, (void*) deep_sort, &arg) != 0) {
			fprintf(stderr, "Error: pthread creation went wrong\n");
			exit(1);
		}
		t_holder[pos] = *pt;
		thread_count--;
	}
}

typedef struct s_data
{
	void*		base;	// Array to sort.
	size_t		n;	// Number of elements in base.
	size_t		s;	// Size of each element.
	int		(*cmp)(const void*, const void*); // Behaves like strcmp
} s_data;


static void par_sort(
	void*		base,	// Array to sort.
	size_t		n,	// Number of elements in base.
	size_t		s,	// Size of each element.
	int		(*cmp)(const void*, const void*)) // Behaves like strcmp
{
	pthread_t*	t[4];
	in_sort(base, n, s, cmp, t, (size_t)4); // exchange 4 to some kind of variable
}

static void in_sort(
		void*		base,	// Array to sort.
		size_t		n,	// Number of elements in base.
		size_t		s,	// Size of each element.
		int		(*cmp)(const void*, const void*), // Behaves like strcmp
		pthread_t*	t, // thread array
		size_t		t_size) //size of thread array
{
	size_t low	= 0;
	size_t high	= s;
	size_t mid	= 0;
	
	if (thread_count > 2) {
		// Find new pivot
		mid = (low + high) / 2; // 10 + 0 = 5 first time
		void* left = (double*)base + (sizeof(double) + low);
		size_t l_size = mid - low;
		void* right = (double*)base + (sizeof(double) + mid);
		size_t r_size = s - mid;
		s_data* ld = {left, l_size, s, cmp};
		s_data* rd = {right, r_size, s, cmp};
		
		// start two new threads and make the threads call par_sort
		start_new_thread(t, 4, deep_sort, ld); //left
		start_new_thread(t, 4, deep_sort, rd); //right
		// (left and right)
	} else if (thread_count > 0) {
		// Find new pivot
	
		// start 1 new thread
		// thread_count - 1
	}
//	qsort(base, n, s, cmp);
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
//	pthread*	t[4];

	if (ac > 1)
		sscanf(av[1], "%d", &n);

	srand(getpid());

	a = malloc(n * sizeof a[0]);
	for (i = 0; i < n; i++)
		a[i] = rand();
	
//	t = malloc(4 * sizeof(pthread));
	
	

	start = sec();

//#ifdef PARALLEL
	par_sort(a, n, sizeof a[0], cmp);
//#else
//	qsort(a, n, sizeof a[0], cmp);
//#endif

	printf(" %*lf\n", 20, *(a+10));
	printf("vector = \n");
	for (int i = 0; i < n; i++)
		printf(" %*lf\n", 30, *a++);
	
	end = sec();

	printf("%1.2f micro s\n", end - start);

	free(a);
//	free(t);
	return 0;
}
