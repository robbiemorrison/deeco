// this file contains the error massage for the Newton-Raphson-algorithm
// used in the class memberfunction 'CFCellConst::rtsafe' in 'Convers.C'
// the massage is pasted from 'nrutil.c' of Numerical Recipes, Numerical
// Recipes in C, Cambridge University Press, Cambridge (1988)
// changes: nrerror is renamed to nrerror2 to stay independent from 'nrutil.c'
// and 'nrutil.h' used in the Simplex-algorithem

#include <stdio.h>  // used for nrerror2
#include <stddef.h> // used for nrerror2
#include <stdlib.h> // used for nrerror2

void nrerror2(char error_text[]) // pasted from 'nrutil.c' of Numerical Recipes
/* Numerical Recipes standard error handler */
{
        fprintf(stderr,"Numerical Recipes run-time error...\n");
        fprintf(stderr,"%s\n",error_text);
        fprintf(stderr,"...now exiting to system...\n");
        exit(13);
}
