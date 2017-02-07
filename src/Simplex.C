
//    -----------------
//     LEGAL NOTICE
//    -----------------
//
//    'deeco' : dynamic energy, emissions, and cost optimization.
//    An energy-services supply systems modeling environment.
//    Copyright (C) 1995-2005.  Thomas Bruckner, Robbie Morrison,
//    Dietmar Lindenberger, Johannes Bruhn, Kathrin Ramsel, Jan
//    Heise, Susanna Medel.
//
//    This file is part of 'deeco'.
//
//    'deeco' is free software; you can redistribute it and/or
//    modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation; either version 2 of
//    the License, or (at your option) any later version.
//
//    'deeco' is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public
//    License along with 'deeco'; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
//    MA 02110-1301, USA.
//
//    Address for notices: Thomas Bruckner, Institute for Energy
//    Engineering, Technical University of Berlin, Marchstrasse 18,
//    D-10587 Berlin, GERMANY.
//
//    More information can be found in README.legal and README.gpl.
//
//    -----------------

//////////////////////////////////////////////////////////////////
//
// SIMPLEX ALGORITHM
//
//////////////////////////////////////////////////////////////////

// The Simplex algorithm used in this file is based
// on the file "xsimplx.c" delivered as an example by
// Press et al., "Numerical Recipes in C", Cambridge
// University Press, Cambridge (1988)
// It uses: simplx.c, simp1.c, simp2.c, simp3.c, nrutil.c
// Attention! simplx() searchs for a maximum

// /* Driver for routine simplx */

#define NRANSI

#include "Simplex.h"
#include "TestFlag.h"
#include <iomanip.h>

// Robbie: 21.02.05: Compiler upgrade modifications.
//
// The compiler on helgoland.fb10.tu-berlin.de was upgraded in
// February 2005 to provide support for the new C++ Standard
// Library, including the Standard Template Library (STL).
// The reported version are:
//                                 new    old
//
//    SCO version                : 3.02   3.01
//    EDG C++ front end version  : 2.43   2.39
//
// Overview of changes:
//
//    Simplex.C     add #include <math.h> as given below
//    simp1.c       disable #include <math.h>
//
// The #include <math.h> preprocessor instruction in simp1.c was
// disabled and placed outside and before the #define float double
// ... #undefine float block in Simplex.C.
//
// No changes to the makefile were required.

#include <math.h>         // see above

#define float double      // necessary because the simplex routines
                          // of the Numerical Recipes work with float
                          // (see p.22, Numerical Recipes)
#include "nrutil.c"
#include "simplx.c"
#include "simp1.c"
#include "simp2.c"
#include "simp3.c"

#undef float              // necessary because the simplex routines
                          // of the Numerical Recipes work with float
                          // (see p.22, Numerical Recipes)

///////////////////////////////////////////////////////////////////
//
// CLASS: Simplex
//
///////////////////////////////////////////////////////////////////

//// Simplex
// Standard Constructor
//
Simplex::Simplex(void)
{
  N       = 0;
  M       = 0;
  M1      = 0;
  M2      = 0;
  M3      = 0;
  i       = 0;
  icase   = 0;
  j       = 0 ;
  izrov   = NULL;
  iposv   = NULL;
  aMatrix = NULL;
  sApp    = NULL;
  errorFlag = 0;
}

//// Simplex
// Constructor
//
Simplex::Simplex(App* csApp, int cN, int cM, int cM1, int cM2, int cM3)
{
  sApp  = csApp;
  N     = cN;                    // number of optimization variables
  M     = cM;                    // number of constraints
  M1    = cM1;                   // number of "<=" constraints, b >= 0
  M2    = cM2;                   // number of ">=" constraints, b >= 0
  M3    = cM3;                   // number of "="  constraints, b >= 0
  i     = 0;
  icase = 0;
  j     = 0;

  aMatrix = matrix(1, M+2, 1, N+1);     // "nrutil.c", allocates a float
                                        // matrix with range [nrl..nrh][ncl..nch]
                                        // nrl = numbers of rows, low
                                        // nrh = numbers of rows, high
                                        // ncl = number of columns, low
                                        // nch = number of columns, high
                                        // necessary as a input for simplx()

  izrov = ivector(1, N);                // "nrutil.c", allocates an int
                                        // vector with range [1..N]
  iposv = ivector(1, M);                // "nrutil.c", allocates an int
                                        // vector with range [1..M]

  if (sApp->testFlag)
    {
      sApp->message(1001, "Simplex");
    }
  errorFlag = 0;

}

//// ~Simplex
// Destructor
//
Simplex::~Simplex(void)
{

  free_matrix(aMatrix, 1, M+2, 1, N+1); // "nrutil.c", frees a matrix allocated
                                        // with matrix(), see above
  free_ivector(iposv,  1, M);           // "nrutil.c", frees an int vector
                                        // allocated by ivector(), see above
  free_ivector(izrov,  1, N);

if (sApp != 0)
  {
    if (sApp->testFlag)
      sApp->message(1002, "Simplex");
  }
}

// optimization with Simplex
//
void Simplex::runSimplex(int intCount)
{
  // test

#ifdef _TEST_
  cerr << "Initial Matrix\n\n" << flush;  // Robbie: 23.09.05: Added flush.
  showMatrix(N,M);
#endif

  // run the simplex routine of numerical recipes

  simplx(aMatrix, M, N, M1, M2, M3, &icase, izrov, iposv);  // "simplx.c"

  // ATTENTION: aMatrix is altered by simplx!!!!!!!!!!

  // analyze the results

  if (icase == 1)
    {
      sApp->message(45, int_to_str(intCount));
      errorFlag++;
    }

  if (icase == -1)
    {
      sApp->message(46, int_to_str(intCount));
      errorFlag++;
    }

  // test

#ifdef _TEST_
  cerr << "Final Matrix\n\n" << flush;  // Robbie: 23.09.05: Added flush.
  showMatrix(N,M);
#endif

}

//// showMatrix
// show aMatrix
//
void Simplex::showMatrix(int energyFlowCount, int constraintCount)
{
  cerr << "N = " << N  << "\n" << flush;  // Robbie: 23.09.05: Added flush.
  cerr << "M = " << M  << "\n" << flush;  // Robbie: 23.09.05: Added flush.
  cerr << "M1= " << M1 << "\n" << flush;  // Robbie: 23.09.05: Added flush.
  cerr << "M2= " << M2 << "\n" << flush;  // Robbie: 23.09.05: Added flush.
  cerr << "M3= " << M3 << "\n" << flush;  // Robbie: 23.09.05: Added flush.

  for (int l = 1; l <= constraintCount + 1; l++)
    {
      for (int k = 1; k <= energyFlowCount + 1; k++)
        {
          cerr << setw(8) << aMatrix[l][k] << "  " << flush;  // Robbie: 23.09.05: Added flush.
        }
      cerr << "\n" << flush;  // Robbie: 23.09.05: Added flush.
    }
}

//// error
// error management
//
int Simplex::error(void)
{
  if (errorFlag)
    return errorFlag;
  else
    return 0;
}

#undef NRANSI

// /* (C) Copr. 1986-92 Numerical Recipes Software 5.){2ptN75L:"52'. */

// end of file
