
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
// DEECO ENERGY BALANCE MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////


#ifndef _BAL_H_               // header guard
#define _BAL_H_


#include     <String.h>
#include     <Symbol.h>       // use Symbol not String for keys
#include        <Map.h>       // associative array
#include       <Path.h>       // for right instantiation of <Symbol.h> and <Map.h>
#include    <fstream.h>       // file I/O


#include        "App.h"
#include       "Data.h"
#include     "DGraph.h"


//////////////////////////////////////////////////////////////////
//
// CLASS: BalanDefRec
//
//////////////////////////////////////////////////////////////////

// Summary: for reading balance definition records

class BalanDefRec
{
public:

 // Standard Constructor
    BalanDefRec(void);
 // Constructor
    BalanDefRec(App* cmApp);
 // Destructor
    virtual ~BalanDefRec(void);
 // write balance definition record
    friend ostream& operator<<(ostream& os, const BalanDefRec& dr);
    
    Symbol energyFlowType;
    Symbol linkType;
    int select;

protected:

    App* mApp;

};


//////////////////////////////////////////////////////////////////
//
// CLASS: BalanDef
//
//////////////////////////////////////////////////////////////////

class BalanDef : public ioDefValue
{
public:

 // Standard Constructor
    BalanDef(void);
 // Constructor
    BalanDef(App* cDefApp, String dTN);
 // Destructor
    virtual ~BalanDef(void);
 // write input data (control)
    void writeInput(void);
 // selection check
    int selected(Symbol sId);

    Map <Symbol, BalanDefRec> defMap;  // overwrite defMap of ioDefValue

protected:

    virtual void readRecord(void);

};


//////////////////////////////////////////////////////////////////
//
// CLASS: Balan
//
//////////////////////////////////////////////////////////////////

// Summary: object for modelling energy balances

class Balan : public DVertex
{
public:

 // Standard Constructor
    Balan(void);
 // Constructor
    Balan(App* cBalanApp, Symbol balanId, Symbol eFType, Symbol lType);
 // Destructor
    virtual ~Balan(void);

    Symbol energyFlowType;
    Symbol linkType;
    double meanDotE;       // to calculate % of supply

protected:

    App* balanApp;

};


#endif  // _BAL_

