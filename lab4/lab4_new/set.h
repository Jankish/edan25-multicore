#ifndef set_h
#define set_h

#include <stdbool.h>
#include <stdio.h>
#include <inttypes.h>

typedef struct {
	size_t		n;	/* elements in array. */
	uint64_t	a[];
} set_t;

set_t*	new_set(size_t);
void	free_set(set_t*);
void	set(set_t*, uint64_t);
void	print_set(set_t *set, FILE *fp);
bool	equal(set_t*, set_t*);
bool	test(set_t*, uint64_t);
void	or(set_t*, set_t*, set_t*);
void	propagate(set_t*, set_t*, set_t*, set_t*);
void	reset(set_t*);

#endif
