# include <stdio.h>
# include <stdlib.h>
# include <math.h>

double
newton(double x, double (*f)(double), double tol)
{
	double last;

	while(1){
		last = x;
		x = (*f)(x);
		if(fabs(x-last) <= tol)
			break;
	}
	return x;
}

double
g(double x)
{
	return x - (x*x*x+x-1)/(3*x*x + 1);
}

int
main()
{
	printf("%g\n", newton(1, g, 1e-5));
	return 0;
}
