# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <errno.h>
# include <string.h>
# include <math.h>

enum{
	M_INIT = 1,
	M_GROW
};

enum{
	LINESZ = 8192,
	MAXCOLS = 512
};

typedef struct mat {
	int nrows;
	int ncols;
	int nelem;
	int max;
	double *elem;
} matrix;

char *argv0;
char *filename;
int lineno;

void
fatal(int syserror, int line, char *fmt, ...)
{
	char buf[256];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	fprintf(stderr, "%s: ", argv0);
	if(line)
		fprintf(stderr, "%s:%d: ", filename, lineno);
	fprintf(stderr, "%s", buf);
	if(syserror)
		fprintf(stderr, ": %s", strerror(errno));
	fprintf(stderr, "\n");

	exit(1);
}

char *
emalloc(unsigned int n)
{
	char *p;

	p = malloc(n);
	if(p == NULL)
		fatal(0, 0, "out of memory");
	return p;
}

char *
erealloc(void *p, unsigned int n)
{
	p = realloc(p, n);
	if(p == NULL)
		fatal(0, 0, "out of memory");
	return p;
}

matrix *
matalloc(void)
{
	matrix *m;

	m = (matrix *)emalloc(sizeof(matrix));
	m->nrows = m->ncols = m->nelem = 0;
	m->elem = NULL;

	return m;
}

matrix *
matalloc0(int nrows, int ncols)
{
	matrix *m;

	m = (matrix *)emalloc(sizeof(matrix));
	m->nrows = nrows;
	m->ncols = ncols;
	m->max = m->nelem = nrows*ncols;
	m->elem = (double *)emalloc(m->max * sizeof(double));

	return m;
}

void
matfree(matrix *m)
{
	free(m->elem);
	free(m);
}

void
matadd(matrix *m, double e)
{
	if(m->elem == NULL){
		m->elem = (double *)emalloc(M_INIT*sizeof(double));
		m->nelem = 0;
		m->max = M_INIT;
	}else if(m->nelem >= m->max){
		m->max *= M_GROW;
		m->elem = (double *)erealloc(m->elem, m->max*sizeof(double));
	}
	m->elem[ m->nelem++ ] = e;
}

double *
matelem(matrix *m, int i, int j)
{
	return &m->elem[i*m->ncols + j];
}

void
matprint(matrix *m)
{
	int i, j;

	for(i = 0; i < m->nrows; i++){
		for(j = 0; j < m->ncols; j++)
			printf(j == 0 ? "%g" : " %g", *matelem(m, i, j));
		printf("\n");
	}
}

int
iswhite(char c)
{
	return c == ' ' || c == '\t';
}

int
readline(FILE *f, char *line, int n)
{
	int i;
	char c;

	for(i = 0; i < n-1 && (c = getc(f)) > 0 && c != '\n'; i++)
		line[i] = c;
	line[i] = '\0';

	return i;
}

int
tokens(char *buf, char *elems[], int n)
{
	char *p;
	int i, sep, inword;

	inword = i = 0;
	for(p = buf; *p != '\0'; p++){
		sep = iswhite(*p);
		if(sep && inword){
			*p = '\0';
			inword = 0;
		}else if(!sep && !inword){
			if(i >= n)
				fatal(0, 1, "too many columns");
			elems[i++] = p;
			inword = 1;
		}
	}

	return i;
}

matrix *
matread(FILE *f)
{
	char line[LINESZ];
	char *elems[MAXCOLS];
	int i, n;
	matrix *m;
	double d;

	m = NULL;
	for(lineno = 1; (n = readline(f, line, LINESZ)) > 0; lineno++){
		n = tokens(line, elems, MAXCOLS);
		if(n == 0)
			break;
		if(m == NULL){
			m = matalloc();
			m->ncols = n;
		}
		if(n != m->ncols)
			fatal(0, 1, "wrong number of columns");
		for(i = 0; i < n; i++){
			if(sscanf(elems[i], "%lg", &d) != 1)
				fatal(0, 1, "%s: invalid number", elems[i]);
			matadd(m, d);
		}
		m->nrows++;
	}

	return m;
}

void
swap(double *a, double *b)
{
	double tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

matrix *
lupdecomp(matrix *m, matrix *p)
{
	int i, j, k, l, n;
	double r, a;

	n = m->nrows;
	for(i = 0; i < n; i++)
		*matelem(p, i, 0) = i;
	for(k = 0; k < n; k++){
		r = 0.;
		for(i = k; i < n; i++)
			if((a = fabs(*matelem(m, i, k))) > r){
				r = a;
				l = i;
			}
		if(r == 0.)
			return NULL;
		swap(matelem(p, k, 0), matelem(p, l, 0));
		for(i = 0; i < n; i++)
			swap(matelem(m, k, i), matelem(m, l, i));
		for(i = k+1; i < n; i++){
			*matelem(m, i, k) /= *matelem(m, k, k);
			for(j = k+1; j < n; j++)
				*matelem(m, i, j) -= *matelem(m, i, k) * *matelem(m, k, j);
		}
	}
	return m;
}

void
lupsolve(matrix *m, matrix *p, matrix *x, int k)
{
	# define getlow(m, i, j) ( ((i) == (j)) ? 1 : *matelem(m, i, j) )
	# define getupp(m, i, j) *matelem(m, i, j)
	int i, j, n;
	matrix *y;
	double sum;

	y = matalloc0(x->nrows, 1);
	n = m->nrows;
	for(i = 0; i < n; i++){
		sum = 0.;
		for(j = 0; j < i; j++)
			sum += getlow(m, i, j) * *matelem(y, j, 0);
		*matelem(y, i, 0) = *matelem(m, *matelem(p, i, 0), n+k) - sum;
	}
	for(i = n-1; i >= 0; i--){
		sum = 0.;
		for(j = i+1; j < n; j++)
			sum += getupp(m, i, j) * *matelem(x, j, k);
		*matelem(x, i, k) = (*matelem(y, i, 0) - sum) / getupp(m, i, i);
	}
	matfree(y);
}

int
main(int argc, char *argv[])
{
	FILE *f;
	matrix *m, *m0, *p, *x;
	int j;

	argv0 = *argv;
	if(argc < 2){
		filename = "<stdin>";
		while((m = matread(stdin)) != NULL){
			p = matalloc0(m->nrows, 1);
			x = matalloc0(m->nrows, m->ncols - m->nrows);
			m0 = lupdecomp(m, p);
			if(m0 == NULL)
				fatal(0, 1, "singular matrix");
			for(j = 0; j < x->ncols; j++)
				lupsolve(m, p, x, j);
			matprint(x);
			printf("\n");
			matfree(p);
			matfree(x);
			matfree(m);
		}
	}else while(*++argv){
		filename = *argv;
		f = fopen(*argv, "r");
		if(f == NULL)
			fatal(1, 0, "%s: unable to read file", *argv);
		while((m = matread(f)) != NULL){
			p = matalloc0(m->nrows, 1);
			x = matalloc0(m->nrows, m->ncols - m->nrows);
			m0 = lupdecomp(m, p);
			if(m0 == NULL)
				fatal(0, 1, "singular matrix");
			for(j = 0; j < x->ncols; j++)
				lupsolve(m, p, x, j);
			matprint(x);
			printf("\n");
			matfree(p);
			matfree(x);
			matfree(m);
		}
		fclose(f);
	}
	return 0;
}
