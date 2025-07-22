# include <stdio.h>
# include <stdlib.h>
# include <math.h>

const double G = 10.;
const double M = 15.;

typedef double (*func)(double, double, double, double, double);

void
rk4(double h, double t, double w1, double w2, double w3, double w4, func f1, func f2, func f3, func f4, double ret[])
{
	double k[4][4];

	k[0][0] = f1(t, w1, w2, w3, w4);
	k[1][0] = f2(t, w1, w2, w3, w4);
	k[2][0] = f3(t, w1, w2, w3, w4);
	k[3][0] = f4(t, w1, w2, w3, w4);

	k[0][1] = f1(t + 0.5*h, w1 + 0.5*h*k[0][0], w2 + 0.5*h*k[1][0], w3 + 0.5*h*k[2][0], w4 + 0.5*h*k[3][0]);
	k[1][1] = f2(t + 0.5*h, w1 + 0.5*h*k[0][0], w2 + 0.5*h*k[1][0], w3 + 0.5*h*k[2][0], w4 + 0.5*h*k[3][0]);
	k[2][1] = f3(t + 0.5*h, w1 + 0.5*h*k[0][0], w2 + 0.5*h*k[1][0], w3 + 0.5*h*k[2][0], w4 + 0.5*h*k[3][0]);
	k[3][1] = f4(t + 0.5*h, w1 + 0.5*h*k[0][0], w2 + 0.5*h*k[1][0], w3 + 0.5*h*k[2][0], w4 + 0.5*h*k[3][0]);

	k[0][2] = f1(t + 0.5*h, w1 + 0.5*h*k[0][1], w2 + 0.5*h*k[1][1], w3 + 0.5*h*k[2][1], w4 + 0.5*h*k[3][1]);
	k[1][2] = f2(t + 0.5*h, w1 + 0.5*h*k[0][1], w2 + 0.5*h*k[1][1], w3 + 0.5*h*k[2][1], w4 + 0.5*h*k[3][1]);
	k[2][2] = f3(t + 0.5*h, w1 + 0.5*h*k[0][1], w2 + 0.5*h*k[1][1], w3 + 0.5*h*k[2][1], w4 + 0.5*h*k[3][1]);
	k[3][2] = f4(t + 0.5*h, w1 + 0.5*h*k[0][1], w2 + 0.5*h*k[1][1], w3 + 0.5*h*k[2][1], w4 + 0.5*h*k[3][1]);

	k[0][3] = f1(t + h, w1 + h*k[0][2], w2 + h*k[1][2], w3 + h*k[2][2], w4 + h*k[3][2]);
	k[1][3] = f2(t + h, w1 + h*k[0][2], w2 + h*k[1][2], w3 + h*k[2][2], w4 + h*k[3][2]);
	k[2][3] = f3(t + h, w1 + h*k[0][2], w2 + h*k[1][2], w3 + h*k[2][2], w4 + h*k[3][2]);
	k[3][3] = f4(t + h, w1 + h*k[0][2], w2 + h*k[1][2], w3 + h*k[2][2], w4 + h*k[3][2]);

	ret[0] = h*(k[0][0] + 2*(k[0][1] + k[0][2]) + k[0][3])/6;
	ret[1] = h*(k[1][0] + 2*(k[1][1] + k[1][2]) + k[1][3])/6;
	ret[2] = h*(k[2][0] + 2*(k[2][1] + k[2][2]) + k[2][3])/6;
	ret[3] = h*(k[3][0] + 2*(k[3][1] + k[3][2]) + k[3][3])/6;
}

double
euler(double h, double t, double w1, double w2, double w3, double w4, func f)
{
	return h*(*f)(t, w1, w2, w3, w4);
}

double
energy(double y1, double y2, double y3, double y4)
{
	return 0.5*(y2*y2 + pow(y1*y4, 2)) - G*M/y1;
}

double
f1(double t, double y1, double y2, double y3, double y4)
{
	return y2;
}

double
f2(double t, double y1, double y2, double y3, double y4)
{
	return y1*y4*y4 - G*M/(y1*y1);
}

double
f3(double t, double y1, double y2, double y3, double y4)
{
	return y4;
}

double
f4(double t, double y1, double y2, double y3, double y4)
{
	return -2.*y2/y1*y4;
}

int
main(int argc, char *argv[])
{
	double t, h;
	double r, vr, psi, vpsi;
	double w1e, w2e, w3e, w4e;
	double dw1e, dw2e, dw3e, dw4e;
	double w1rk, w2rk, w3rk, w4rk;
	double dwrk[4];
	int n;

	if(argc < 7){
		fprintf(stderr, "usage: onebody r vr psi vpsi h n\n");
		return 1;
	}
	r = atof(*++argv);
	vr = atof(*++argv);
	psi = atof(*++argv);
	vpsi = atof(*++argv);
	h = atof(*++argv);
	n = atoi(*++argv);

	w1rk = w1e = r;
	w2rk = w2e = vr;
	w3rk = w3e = psi;
	w4rk = w4e = vpsi;
	t = 0.;
	printf("%g %g %g %g %g %g %g %g %g %g %g\n", t, w1e, w2e, w3e, w4e, w1rk, w2rk, w3rk, w4rk,
		energy(w1e, w2e, w3e, w4e), energy(w1rk, w2rk, w3rk, w4rk));
	while(n-- > 0){
		dw1e = euler(h, t, w1e, w2e, w3e, w4e, f1);
		dw2e = euler(h, t, w1e, w2e, w3e, w4e, f2);
		dw3e = euler(h, t, w1e, w2e, w3e, w4e, f3);
		dw4e = euler(h, t, w1e, w2e, w3e, w4e, f4);
		w1e += dw1e;
		w2e += dw2e;
		w3e += dw3e;
		w4e += dw4e;

		rk4(h, t, w1rk, w2rk, w3rk, w4rk, f1, f2, f3, f4, dwrk);
		w1rk += dwrk[0];
		w2rk += dwrk[1];
		w3rk += dwrk[2];
		w4rk += dwrk[3];

		t += h;

		printf("%g %g %g %g %g %g %g %g %g %g %g\n", t, w1e, w2e, w3e, w4e, w1rk, w2rk, w3rk, w4rk,
			energy(w1e, w2e, w3e, w4e), energy(w1rk, w2rk, w3rk, w4rk));
	}

	return 0;
}
