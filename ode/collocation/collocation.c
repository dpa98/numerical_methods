# include <stdio.h>
# include <stdlib.h>
# include <math.h>

double *tvals;
int tpos;
int tmax;

void
add(double t)
{
	if(tvals == NULL){
		tmax = 1;
		tvals = malloc(tmax*sizeof(double));
		if(tvals == NULL){
			fprintf(stderr, "collocation: out of memory\n");
			exit(1);
		}
	}else if(tpos >= tmax){
		tmax *= 2;
		tvals = realloc(tvals, tmax*sizeof(double));
		if(tvals == NULL){
			fprintf(stderr, "collocation: out of memory\n");
			exit(1);
		}
	}
	tvals[tpos++] = t;
}

double
fk(int k, double t)
{
	if(k == 1)
		return 1.;
	if(k == 2)
		return t-2.;
	return ((k-1)*(k-2)-2*(k-1)*t + t*t)*pow(t, k-3);
}

int
main(void)
{
	double t;
	int i, j;

	while(scanf("%lg", &t) == 1){
		if(tpos == 0 && t != 0.){
			fprintf(stderr, "collocation: %g: first entry should be 0\n", t);
			return 1;
		}
		add(t);
	}
	if(tvals[tpos-1] != 1.){
		fprintf(stderr, "collocation: %g: last entry should be 1\n", tvals[tpos-1]);
		return 1;
	}

	for(i = 0; i < tpos; i++){
		for(j = 0; j < tpos; j++)
			if(i == 0)
				printf(j == 0 ? "1 " : "0 ");
			else if(i == tpos-1)
				printf("1 ");
			else
				printf("%.16g ", fk(j+1, tvals[i]));
		printf(i == 0 ? "1\n" : "0\n");
	}

	return 0;
}
