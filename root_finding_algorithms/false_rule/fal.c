# include <stdio.h>
# include <stdlib.h>
# include <math.h>

double
regulafalsi(double a, double b, double (*f)(double), double tol)
{
	double x, p;

	while(fabs((*f)(x)) > tol){
//	while(fabs(x-a) > tol){
		x = (a*(*f)(b) - b*(*f)(a)) / ((*f)(b) - (*f)(a));
		p = (*f)(a) * (*f)(x);
		if(p < 0)
			b = x;
		else if(p > 0)
			a = x;
		else
			break;
	}

	return x;
}

double
func(double x)
{
	return exp(x) - sin(x);
}

int
main(void)
{
	printf("%.10g\n", regulafalsi(-4., -2., func, 1e-5));

	return 0;
}
