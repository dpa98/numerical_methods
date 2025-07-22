/*
# gamma
interpol db/gammasamp.txt > db/i1.txt
echo '
plot "db/gammasamp.txt" u 1:2 w p t "samples",\
"db/i1.txt" u 1:2 w l t "interpolation",\
"db/gamma.txt" u 1:2 w l t "gamma"
pause mouse close
' | gnuplot
interpol db/gammachebsamp.txt > db/i2.txt
echo '
plot "db/gammachebsamp.txt" u 1:2 w p t "samples",\
"db/i2.txt" u 1:2 w l t "interpolation",\
"db/gamma.txt" u 1:2 w l t "gamma"
pause mouse close
' | gnuplot
# erf
interpol db/erfsamp.txt > db/i3.txt
echo '
set key top left
plot "db/erfsamp.txt" u 1:2 w p t "samples",\
"db/i3.txt" u 1:2 w l t "interpolation",\
"db/erf.txt" u 1:2 w l t "erf"
pause mouse close
' | gnuplot
interpol db/erfchebsamp.txt > db/i4.txt
echo '
set key top left
plot "db/erfchebsamp.txt" u 1:2 w p t "samples",\
"db/i4.txt" u 1:2 w l t "interpolation",\
"db/erf.txt" u 1:2 w l t "erf"
pause mouse close
' | gnuplot
*/
# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <errno.h>
# include <string.h>
# include <math.h>

enum{
	LINESZ = 256,
	INIT = 2,
	GROW = 2,
	NSAMP = 1024
};

char *argv0;
char *filename;
int lineno;

double *xvals;
double *yvals;
int nalloc;
int maxalloc;

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

void
add(double x, double y)
{
	if(maxalloc == 0){
		nalloc = 0;
		maxalloc = INIT;
		xvals = (double *)emalloc(maxalloc*sizeof(double));
		yvals = (double *)emalloc(maxalloc*sizeof(double));
	}else if(nalloc >= maxalloc){
		maxalloc *= GROW;
		xvals = (double *)erealloc(xvals, maxalloc*sizeof(double));
		yvals = (double *)erealloc(yvals, maxalloc*sizeof(double));
	}
	if(nalloc > 0 && x <= xvals[nalloc-1])
		fatal(0, 1, "repeated or unsorted x values");
	xvals[nalloc] = x;
	yvals[nalloc++] = y;
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
iswhite(char c)
{
	return c == ' ' || c == '\t';
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

void
load(FILE *f)
{
	char line[LINESZ];
	char *elems[2];
	int n;
	double x, y;

	for(lineno = 1; (n = readline(f, line, LINESZ)) > 0; lineno++){
		n = tokens(line, elems, 2);
		if(n == 0)
			break;
		if(sscanf(elems[0], "%lg", &x) != 1)
			fatal(0, 1, "%s: invalid number", elems[0]);
		if(sscanf(elems[1], "%lg", &y) != 1)
			fatal(0, 1, "%s: invalid number", elems[1]);
		add(x, y);
	}
}

double
brackets(int l, int r)
{
	if(l+1 >= r)
		return yvals[l];
	return (brackets(l+1, r) - brackets(l, r-1)) / (xvals[r-1] - xvals[l]);
}

double
hornerscheme(double x, double *c, double *r, int n)
{
	double f;

	f = c[--n];
	while(--n >= 0)
		f = f*(x - r[n]) + c[n];
	return f;
}

int
main(int argc, char *argv[])
{
	FILE *f;
	int i;
	double x, dx, *c;

	argv0 = *argv;
	maxalloc = 0;
	if(argc == 1){
		filename = "<stdin>";
		load(stdin);
	}else while(*++argv){
		filename = *argv;
		f = fopen(*argv, "r");
		if(f == NULL)
			fatal(1, 0, "%s: unable to read file", *argv);
		load(f);
		fclose(f);
	}

	c = (double *)emalloc(nalloc*sizeof(double));
	for(i = 0; i < nalloc; i++)
		c[i] = brackets(0, i+1);

	dx = (xvals[nalloc-1] - xvals[0])/NSAMP;
	x = xvals[0];
	for(i = 0; i <= NSAMP; i++){
		printf("%g %g\n", x, hornerscheme(x, c, xvals, nalloc));
		x += dx;
	}

	return 0;
}
