
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
// SIMPLEX ALGORITHM MANAGEMENT OPJECT
//
//////////////////////////////////////////////////////////////////

// see: Press et al., "Numerical Recipes in C", Cambridge
// University Press, Cambridge (1988)


#ifndef _DSIM_                // header guard
#define _DSIM_


#include "App.h"

#define float double          // necessary because the simplex routines
                              // of the Numerical Recipes work with float
                              // (see p.22, Numerical Recipes)

#define NRANSI                // (Numerical Recipes, see xsimplx.c)
#include "nr.h"               // headers of Numerical Recipes functions
#include "nrutil.h"           // headers of Numerical Recipes help functions
#undef float                  // necessary because the simplex routines
                              // of the Numerical Recipes work with float
                              // (see p.22, Numerical Recipes)

#include <stdio.h>


///////////////////////////////////////////////////////////////////
//
// CLASS: Simplex
//
///////////////////////////////////////////////////////////////////

class Simplex
{
public:

 // Standard Constructor
    Simplex(void);
 // Constructor
    Simplex(App* csApp, int cN, int cM, int cM1, int cM2, int cM3);
 // Destructor
    virtual ~Simplex(void);
 // optimization with Simplex
    void runSimplex(int intCount);
 // show aMatrix
    void showMatrix(int energyFlowCount, int constraintCount);
 // error Management
    int error(void);

    int N, M, M1, M2, M3;
    int i,icase,j,*izrov,*iposv;
    double **aMatrix;        // if #define float double is not used in Simplex.h
                             // and Simplex.C float ** aMatrix has to be used

protected:

    App* sApp;
    int errorFlag;

};


#endif  // _DSIM_

