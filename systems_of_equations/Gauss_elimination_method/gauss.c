#include <stdio.h>

#include <math.h>

void main(){

    float a[20][20], x[20], max, temp, sum;

    int i, j, k, m, n, p, q;

    printf("**Program to find solution of system of linear equation using Gauss Elimination Method**\n\n");

    // Entering the number of equations

    printf("Enter number of equations : ");

    scanf("%d", &n);

    // Entering the coefficients of the equations with value of f(x) equivalent to equation

    printf("\nEnter coefficients equation wise\n");

    for (i = 1; i <= n; i++)

{

        for (j = 1; j <= (n + 1); j++)

{

           scanf("%f", &a[i][j]);

        }

    }

    printf("----------------------------------------------------\n");

    // Calculating the value of the variables

    for (k = 1; k < n; k++)

    {

        max = fabs(a[k][k]);

        p = k;

        for (m = k + 1; m <= n; m++) {

            if (fabs(a[m][k]) > max) {

                max = fabs(a[m][k]);

                p = m; }

        }

        if (p != k)  

{

            for (q = k; q <= (n + 1); q++)

{

                temp = a[k][q];

                a[k][q] = a[p][q];

                a[p][q] = temp;

            }  

}

        for (i = k + 1; i <= n; i++) 

{

            temp = a[i][k] / a[k][k];

            for (j = k; j <= (n + 1); j++)

                a[i][j] = a[i][j] - temp * a[k][j];

        } }

    x[n] = a[n][n + 1] / a[n][n];

    for (i = (n - 1); i >= 1; i--) 

{

        for (sum = 0.0, j = (i + 1); j <= n; j++)

            sum = sum + a[i][j] * x[j];

        x[i] = (a[i][n + 1] - sum) / a[i][i];

    }

    // Printing the solution

    printf("Solution of linear equation using Gauss Elimination Method\n");

    for (i = 1; i <= n; i++)

    {

        printf("----------------------------------------------------\n");

        printf("\t x (%d) = %.3f \n", i, x[i]);

    }

}
