
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

//  $Revision: 1.1 $
//  $Date: 2005/11/25 13:21:32 $
//  $Author: morrison $
//  $RCSfile: Connect.h,v $

//////////////////////////////////////////////////////////////////
//
// DEECO PROCESS CONNECTION MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////

#ifndef _CONNECT_H_           // header guard
#define _CONNECT_H_

#include     <String.h>
#include     <Symbol.h>       // use Symbol not String for keys
#include        <Map.h>       // associative array
#include       <Path.h>       // necessary for right instantiation of <Symbol.h> and <Map.h>
#include    <fstream.h>       // file I/O

#include        "App.h"
#include       "Data.h"

//////////////////////////////////////////////////////////////////
//
// CLASS: ConnectDefRec
//
//////////////////////////////////////////////////////////////////

// Summary: for reading connection definition records

class ConnectDefRec
{
public:

 // Standard Constructor
    ConnectDefRec(void);
 // Constructor
    ConnectDefRec(App* cmApp);
 // Destructor
    virtual ~ConnectDefRec(void);
 // write connection definition  record
    friend ostream& operator<<(ostream& os, const ConnectDefRec& dr);

    Symbol procId;
    int energyFlowNumber;
    Symbol balanId;
    Symbol energyFlowType;
    Symbol linkType;
    int direct;

protected:

    App* mApp;

};

//////////////////////////////////////////////////////////////////
//
// CLASS: ConnectDef
//
//////////////////////////////////////////////////////////////////

class ConnectDef : public ioDefValue
{
public:

 // Standard Constructor
    ConnectDef(void);
 // Constructor
    ConnectDef(App* cDefApp, String dTN);
 // Destructor
    virtual ~ConnectDef(void);
 // write Input Data (Control)
    void writeInput(void);
 // selection Check
    int selected(Symbol sId);

    Map <Symbol, ConnectDefRec> defMap;     // overwrite defMap of ioDefValue

protected:

    virtual void readRecord(void);

};

#endif  // _CONNECT_H_

//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/Connect.h,v $
//  end of file
