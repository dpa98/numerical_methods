# include <stdio.h>
# include <stdlib.h>

double isize;	// interval size

int
deriv(FILE *f, char *name)
{
	double func[3];
	int i, r, n;

	i = 0;
	for(n = 1; (r = fscanf(f, "%lg", &func[i])) == 1; n++){
		i = (i+1)%3;
		if(n < 3)
			continue;
		printf("%g\n", 0.5*(func[(i+2)%3] - func[i])/isize);
	}
	if(r != EOF){
		fprintf(stderr, "deriv: %s: %d: error while reading number\n", name, n);
		return 1;
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	FILE *f;

	if(argc < 2){
		fprintf(stderr, "usage: deriv h [file ...]\n");
		return 1;
	}

	isize = atof(*++argv);
	if(isize <= 0){
		fprintf(stderr, "deriv: %s: invalid interval size\n", *argv);
		return 1;
	}
	if(argc == 2)
		return deriv(stdin, "<stdin>");
	else while(*++argv != NULL){
		f = fopen(*argv, "r");
		if(f == NULL){
			fprintf(stderr, "deriv: %s: unable to open file\n", *argv);
			return 1;
		}
		if(deriv(f, *argv))
			return 1;
		fclose(f);
	}

	return 0;
}
