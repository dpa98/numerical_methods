# include <stdio.h>
# include <stdlib.h>

int
main(int argc, char *argv[])
{
	int i, n;
	double h, wf, wb, t;

	if(argc != 3){
		fprintf(stderr, "usage: %s h n\n", *argv);
		return 1;
	}

	h = atof(*++argv);
	n = atoi(*++argv);
	wf = wb = 1.;
	t = 0.;
	printf("%g %g %g\n", t, wf, wb);
	for(i = 0; i < n; i++){
		t = (i+1)*h;
		wf = wf - 8*h*wf;
		wb = wb/(1 + 8*h);
		printf("%g %g %g\n", t, wf, wb);
	}
	return 0;
}
