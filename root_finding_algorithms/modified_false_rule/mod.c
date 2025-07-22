# include <stdio.h>
# include <stdlib.h>
# include <math.h>

double
regulafalsi(double a, double b, double (*f)(double), double tol)
{
	double x, p, fa, fb;

	fa = (*f)(a);
	fb = (*f)(b);
	while(fabs((*f)(x)) > tol){
//	while(fabs(x-a) > tol){
		x = (a*fb - b*fa) / (fb - fa);
		p = fa * (*f)(x);
		if(p < 0){
			b = x;
			fb = (*f)(b);
			fa /= 2;
		}else if(p > 0){
			a = x;
			fa = (*f)(a);
			fb /= 2;
		}else
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
