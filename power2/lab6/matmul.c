#include <stdio.h>
#include <string.h>

#define N (1024)

double a[N][N];
double b[N][N];
double c[N][N];

int main(void){

	size_t	i, j, k;

	memset(a, 0, sizeof a);
	#pragma omp parallel private(i, j, k)
	#pragma omp for schedule(static, N/omp_get_num_procs())
	for (i = 0; i < N; i += 1) {
		for (k = 0; k < N; k += 1) {
			for (j = 0; j < N; j += 1) 
				a[i][j] += b[i][k] * c[k][j];
		}
	}

	return 0;
}
