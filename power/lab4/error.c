#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"

extern char* progname;

/* error: print error message and terminate. for user error. */
void error(char* fmt, ...)
{
	va_list	ap;

	va_start(ap, fmt);

	fprintf(stderr, "%s: error: ", progname);
	vfprintf(stderr, fmt, ap);
	fputc('\n', stderr);

	va_end(ap);

	exit(EXIT_FAILURE);
}
