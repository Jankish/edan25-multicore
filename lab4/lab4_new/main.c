#define USE_SEQUENTIAL		1
#define USE_REF_DATA		1
#define USE_VERBOSE		1

#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/time.h>
#include "dataflow.h"
#include "list.h"
#include "error.h"
#include "random.h"
#include "timebase.h"

/* Number of samples to use for validation. */
#define NCHECK	(1024)

typedef enum {
	PARALLEL,
	SEQUENTIAL,
	N_CFG_TYPE
} cfg_type_t;

typedef struct {
	cfg_t*		(*new_cfg)(size_t nvertex, size_t nsymbol, size_t max_succ);
	void		(*free_cfg)(cfg_t* cfg);
	void		(*connect)(cfg_t* cfg, size_t pred, size_t succ);
	bool		(*testbit)(cfg_t* cfg, size_t vertex, set_type_t type, size_t symbol);
	void		(*setbit)(cfg_t* cfg, size_t vertex, set_type_t type, size_t symbol);
	void		(*liveness)(cfg_t* cfg);
	void		(*print_sets)(cfg_t* cfg, FILE*);
} cfg_func_t;

struct {
	size_t		v;
	size_t		s;
	int		live;
} check[NCHECK];

typedef struct {
	size_t		pred;
	size_t		succ;
} edge_t;

static edge_t*		more_edges;
static size_t		edge_index;
static size_t		max_more_edges = 10;

char*			progname;

void pr(char* fmt, ...)
{
#ifdef USE_VERBOSE
	va_list	ap;

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
#endif
}

static cfg_func_t	func[] = {

	[ SEQUENTIAL ].new_cfg		= sequential_new_cfg,
	[ SEQUENTIAL ].free_cfg		= sequential_free_cfg,
	[ SEQUENTIAL ].connect		= sequential_connect,
	[ SEQUENTIAL ].testbit		= sequential_testbit,
	[ SEQUENTIAL ].setbit		= sequential_setbit,
	[ SEQUENTIAL ].liveness		= sequential_liveness,
	[ SEQUENTIAL ].print_sets	= sequential_print_sets,
	
	[ PARALLEL ].new_cfg		= new_cfg,
	[ PARALLEL ].free_cfg		= free_cfg,
	[ PARALLEL ].connect		= connect,
	[ PARALLEL ].testbit		= testbit,
	[ PARALLEL ].setbit		= setbit,
	[ PARALLEL ].liveness		= liveness,
	[ PARALLEL ].print_sets		= sequential_print_sets

};

void new_edge(size_t pred, size_t succ)
{
	edge_t*		p;

	if (edge_index == max_more_edges) {

		max_more_edges *= 1.20;

		p = realloc(more_edges, max_more_edges * sizeof(edge_t));

		if (p == NULL) {

			/* free more_edges and everything else
			 * allocated... but we have nothing better
			 * to do than terminating.
			 *
			 */

			error("out of memory");
		} else
			more_edges = p;
	}

	more_edges[edge_index].pred = pred;
	more_edges[edge_index].succ = succ;

	edge_index += 1;
}

static void generate_cfg(
	cfg_t*		cfg,
	size_t		n,
	size_t		max_succ,
	cfg_func_t*	func)
{
	int		i;
	int		j;
	int		k;
	int		s;
	FILE*		fp;
	const char*	name = "cfg.dot";

	if (n <= 2)
		error("too few vertices: %zu\n", n);

	fp = fopen(name, "w");

	if (fp == NULL)
		error("cannot open \"%s\" for writing", name);

	fprintf(fp, "digraph cfg {\n");
	fprintf(fp, "0 -> { 1 2 }\n");

	(*func->connect)(cfg, 0, 1);
	(*func->connect)(cfg, 0, 2);

	for (i = 2; i < n; ++i) {
		
		s = ((unsigned)next()) % max_succ;
		s += 1;

#ifdef PRINT
		printf("succ(%d) = { ", i);
#endif
		
		for (j = 0; j < s; ++j) {
			do
				k = abs(next()) % n;
			while (k == 0); 
			
#ifdef PRINT
			printf("%d ", k);
#endif

			(*func->connect)(cfg, i, k);

			fprintf(fp, "%d -> { %d }\n", i, k);
		}
#ifdef PRINT
		printf("}\n");
#endif
	}

	pr("%zu more edges\n", edge_index);

	for (j = 0; j < edge_index; j += 1) {
		i = more_edges[j].pred;
		k = more_edges[j].succ;
		(*func->connect)(cfg, i, k);
		fprintf(fp, "%d -> { %d }\n", i, k);
	}

	fprintf(fp, "}\n");
	fclose(fp);
}

static void generate_usedefs(
	cfg_t*		cfg,
	size_t		n,
	size_t		nsym,
	size_t		nactive,
	cfg_func_t*	func)
{
	size_t		i;
	size_t		j;
	size_t		sym;

	for (i = 0; i < n; ++i) {
		for (j = 0; j < nactive; ++j) {
			sym = abs(next()) % nsym;

			if (j % 4 != 0) {
				if (!(*func->testbit)(cfg, i, DEF, sym))
					(*func->setbit)(cfg, i, USE, sym);
			} else if (!(*func->testbit)(cfg, i, USE, sym))
				(*func->setbit)(cfg, i, DEF, sym);
		}
	}
}

void check_cfg(cfg_t* cfg);

#if defined PPC32 || defined PPC64

static double (*sec)(void) = timebase_sec;

#else

static double syscall_sec(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);

	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

static double (*sec)(void) = syscall_sec;

#endif

int main(int argc, char** argv) 
{
	int		j;
	int		live;
	int		status;
	FILE*		fp;
	double		begin;
	double		end;
	size_t		nsym;
	size_t		nactive;
	size_t		n;
	size_t		max_succ;
	cfg_t*		cfg;
	bool		print;
	int		seed;
	uint64_t	score;
	int		i;
	char*		s;

	progname = argv[0];

	s = getenv("S");

	if (s == NULL || sscanf(s, "%d", &seed) != 1) {
		seed = getpid();
		pr("random seed = %d\n", seed);
	} else
		pr("getenv seed = %d\n", seed);


	init_timebase();

	if (argc == 6) {
		nsym	 	= atoi(argv[1]);
		n		= atoi(argv[2]);
		max_succ	= atoi(argv[3]);
		nactive	 	= atoi(argv[4]);
		print	 	= atoi(argv[5]);
	} else {
		print		= 0;
#if USE_REF_DATA
		nsym	 	= 100000;
		n		= 100000;
		nactive	 	= 1000;
#else
		nsym	 	= 1000;
		n		= 1000;
		nactive	 	= 100;
#endif
		max_succ	= 4;
	}

	more_edges = malloc(max_more_edges * sizeof(edge_t));
	if (more_edges == NULL)
		error("out of memory");

	pr("nsymbol     = %zu\n", nsym);
	pr("nvertex     = %zu\n", n);
	pr("max-succ    = %zu\n", max_succ);
	pr("nactive     = %zu\n", nactive); 

	status = 0;

	init_random(seed);

	/* Make a cfg without symbols to ensure
	 * all vertices are reachable.
 	 *
	 */
	cfg = sequential_new_cfg(n, nsym, max_succ);
	generate_cfg(cfg, n, max_succ, &func[SEQUENTIAL]);
	check_cfg(cfg);
	sequential_free_cfg(cfg);

	for (i = 0; i < N_CFG_TYPE; i += 1) {
		init_random(seed);

		pr("generating cfg...\n");

		cfg = (*func[i].new_cfg)(n, nsym, max_succ);
		generate_cfg(cfg, n, max_succ, &func[i]);

		pr("generating usedefs...\n");

		generate_usedefs(cfg, n, nsym, nactive, &func[i]);

		pr("liveness...\n\n");

		begin = sec();
		(*func[i].liveness)(cfg);
		end = sec();

		printf("T = %8.4lf s\n\n", end-begin);

		if (print)
			print_sets(cfg, stdout);

		switch (i) {
		case PARALLEL:
			score = round((end-begin) * 1000);

			fp = fopen("score", "w");
			if (fp == NULL)
				error("cannot open file \"score\" for writing");
			fprintf(fp, "%" PRIu64 "\n", score);
			fclose(fp);

			for (j = 0; j < NCHECK; j += 1) {
				check[j].v = rand() % n;	
				check[j].s = rand() % nsym;	
				check[j].live = (*func[i].testbit)(cfg, check[j].v, IN, check[j].s);
			}
			break;

		case SEQUENTIAL:

			printf("validating IN sets...");
			fflush(stdout);

			status = 0;

			for (j = 0; j < NCHECK; j += 1) {
				live = (*func[i].testbit)(cfg, check[j].v, IN, check[j].s);
				if (live != check[j].live) {
					printf("FAIL: wrong output at bit comparison %d:\n\n", j+1);
					printf("vertex:  %8zu\n", check[j].v);
					printf("symbol:  %8zu\n", check[j].s);
					printf("live:    %8d\n", check[j].live);
					printf("ref:     %8d\n", live);
					status = 1;
					break;
				}
			}

			if (status == 0)
				printf("PASS\n");

			break;
			
		default:
			error("unknown type: %s line %d\n", __FILE__, __LINE__);
		}

		if (print)
			(*func[i].print_sets)(cfg, stdout);

		(*func[i].free_cfg)(cfg);

#if !USE_SEQUENTIAL
		break;
#endif
	}

	free(more_edges);

	return status;
}
