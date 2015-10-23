#ifndef dataflow_h
#define dataflow_h

#include <stdbool.h>
#include <stddef.h>
#include <inttypes.h>

/* Opaque type made complete in sequential.c and dataflow.c 
 *
 * This is forbidden in C++ since it does not work with name mangling (hence the one definition rule).
 *
 */

typedef struct cfg_t cfg_t;

typedef enum {
	IN,
	OUT,
	USE,
	DEF,
	NSETS
} set_type_t;

cfg_t*	new_cfg(size_t nvertex, size_t nsymbol, size_t max_succ);
void	free_cfg(cfg_t* cfg);
void 	connect(cfg_t* cfg, size_t pred, size_t succ);
bool	testbit(cfg_t* cfg, size_t vertex, set_type_t type, size_t index);
void	setbit(cfg_t* cfg, size_t vertex, set_type_t type, size_t index);
void	liveness(cfg_t* cfg);
void	print_sets(cfg_t* cfg, FILE* fp);

cfg_t*	sequential_new_cfg(size_t nvertex, size_t nsymbol, size_t max_succ);
void	sequential_free_cfg(cfg_t* cfg);
void 	sequential_connect(cfg_t* cfg, size_t pred, size_t succ);
bool	sequential_testbit(cfg_t* cfg, size_t vertex, set_type_t type, size_t index);
void	sequential_setbit(cfg_t* cfg, size_t vertex, set_type_t type, size_t index);
void	sequential_liveness(cfg_t* cfg);
void	sequential_print_sets(cfg_t* cfg, FILE* fp);

#endif
