
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
// DEECO PROCESS TYPE MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////

#ifndef _DPROCTYP_            // header guard
#define _DPROCTYP_

#include "App.h"
#include "Data.h"
#include "Proc.h"
#include <Symbol.h>           // string key management

///////////////////////////////////////////////////////////////////
//
// CLASS: ProcType
//
///////////////////////////////////////////////////////////////////

class ProcType
{
public:

 // Standard Constructor
    ProcType(void);
 // Constructor
    ProcType(App* cpApp);
 // Destructor
    virtual ~ProcType(void);
 // Prepare single processes
    Proc* prepareProcess(App* nApp, Symbol procId, Symbol procType,
                         ioValue* nProcVal);
protected:

    App* pApp;
    int errorFlag;

};

#endif  // _DPROCTYP_
