# include <stdio.h>
# include <stdlib.h>
# include <math.h>

double
bisection(double a, double b, double (*f)(double), int n)
{
	double m, p;

	while(n-- > 0){
		m = (a + b)/2;
		p = (*f)(a) * (*f)(m);
		if(p < 0)
			b = m;
		else if(p > 0)
			a = m;
		else
			break;
	}

	return m;
}

double
func(double x)
{
	return x*x*x - 2;
}

int
main(void)
{
	printf("%.10g\n", bisection(-2., 2., func, 20));

	return 0;
}
