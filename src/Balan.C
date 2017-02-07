
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

//  $Revision: 1.2 $
//  $Date: 2005/11/25 13:11:40 $
//  $Author: morrison $
//  $RCSfile: Balan.C,v $

//////////////////////////////////////////////////////////////////
//
// DEECO ENERGY BALANCE MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////

#include     <stdlib.h>       // exit()
#include  <strstream.h>       // string streams

#include      "Balan.h"
#include   "TestFlag.h"

//////////////////////////////////////////////////////////////////
//
// CLASS: BalanDefRec
//
//////////////////////////////////////////////////////////////////

// Summary: BalanDefRec will be used as a type in templates like
// Map<class X,class Y>; according to USL C++ Standards Components,
// Rel. 3.0, p9-3 the constructor of the class Y is not allowed
// to require arguments

//// BalanDefRec
// Standard Constructor
//
BalanDefRec::BalanDefRec(void)
{

  mApp           = NULL;
  energyFlowType = "";
  linkType       = "";
  select         = 0;

}

//// BalanDefRec
// Constructor
//
BalanDefRec::BalanDefRec(App* cmApp)
{
  mApp           = cmApp;
  energyFlowType = "";
  linkType       = "";
  select         = 0;

#ifdef _TEST_                 // inner cycle test only if necessary
  if (cmApp->testFlag)
    cmApp->message(1001, "BalanDefRec");
#endif
}

//// ~BalanDefRec
// Destructor
//
BalanDefRec::~BalanDefRec(void)
{
#ifdef _TEST_                 // inner cycle test only if necessary
  if (mApp != 0)              // correctly initialized?
    {
      if (mApp->testFlag)
        mApp->message(1002, "BalanDefRec");
    }
#endif
}

//// operator<<
// write balance definition record
//
ostream& operator<<(ostream& os, const BalanDefRec& dr)
{
  os << dr.energyFlowType << " " << dr.linkType << " " << dr.select;
  return os;
}

//////////////////////////////////////////////////////////////////
//
// Class: BalanDef
//
//////////////////////////////////////////////////////////////////

//// BalanDef
// Standard
//
BalanDef::BalanDef(void)
{
  // no content, call ioDefValue(void)
}

//// BalanDef
//
BalanDef::BalanDef(App * cDefApp, String dTN) : ioDefValue(cDefApp, dTN)
{
  // no contents
}

//// ~BalanDef
//
BalanDef::~BalanDef(void)
{
  // no content, call ~ioDefValue(void)
}

//// writeInput
// write input data (control)
//
void BalanDef::writeInput(void)
{
#ifdef _TEST_XXX  // TOFIX: 25.11.05: disabled with XXX because seg-faults
  if (ioApp->testFlag)
    {
      ostrstream ssBalanDefVal;                // <strstream.h>
      ssBalanDefVal << ioDataName
                    << " : "
                    << defMap
                    << "\n"
                    << ends;
      char* pBalanDefVal = ssBalanDefVal.str();
      ioApp->message(1009, pBalanDefVal);
      delete pBalanDefVal;    // Robbie: 18.11.2005: delete was delete[],
                              // deallocation is correct, see man strstream
                              // and method str().
    }
#endif
}

//// selected
// selection check
//
int BalanDef::selected(Symbol sId)
{
  if (defMap.element(sId))           // <Map.h>
    {
      if (defMap[sId].select == 0)
        return 0;
      else
        return 1;
    }
  else
    return 0;
}

///////////////// help function ////////////////////

//// readRecord
// read data record from definition file
//
void BalanDef::readRecord(void)
{
  if (!ioApp->emptyLine(finData))
    {
      int i = 0;
      finData >> Id;
      i = i + ioApp->readComma(finData);          // read ","
      finData >> defMap[Id].energyFlowType;
      i = i + ioApp->readComma(finData);          // read ","
      finData >> defMap[Id].linkType;
      i = i + ioApp->readComma(finData);          // read ","
      finData >> defMap[Id].select;
      if (i != 0)
        finData.clear(ios::badbit | finData.rdstate());
                                           // raise errorflags of finData
                                           // <fstream.h> -> <iostream.h>
    }
  ioApp->ignoreRestLine(finData); //  "App.h"
}

//////////////////////////////////////////////////////////////////
//
// CLASS: Balan
//
//////////////////////////////////////////////////////////////////

// Summary: object for modelling energy balances

//// Balan
// Standard Constructor
//
// BalanDefRec will be used as a type in templates like Map<class X, class Y>;
// according to USL C++ Standards Components, Rel. 3.0, page 9-3
// the constructor of the class Y is not allowed to require arguments
//
Balan::Balan(void)
{
  balanApp = NULL;
  meanDotE = 0;
}

//// Balan
// Constructor
//
Balan::Balan(App* cBalanApp, Symbol balanId, Symbol eFType,
             Symbol lType) : DVertex(balanId)
{
  energyFlowType = eFType;
  linkType       = lType;
  balanApp       = cBalanApp;
  meanDotE       = 0;

  if (balanApp->testFlag)
      balanApp->message(1001, "Balan");
}

//// ~Balan
// Destructor
//
Balan::~Balan(void)
{
  if (balanApp != 0)
    {
      if (balanApp->testFlag)
        balanApp->message(1002, "Balan");
    }
}

//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/Balan.C,v $
// end of file
