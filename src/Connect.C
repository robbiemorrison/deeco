
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
//  $Date: 2005/11/25 13:31:16 $
//  $Author: morrison $
//  $RCSfile: Connect.C,v $


//////////////////////////////////////////////////////////////////
//
// DEECO PROCESS CONNECTION MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////


#include     <stdlib.h>       // exit()
#include  <strstream.h>       // string streams


#include   "TestFlag.h"
#include    "Connect.h"


//////////////////////////////////////////////////////////////////
//
// CLASS: ConnectDefRec
//
//////////////////////////////////////////////////////////////////

// Summary: for reading connection definition records
//
// Note: ConnectDefRec will be used as a type in templates like
// Map<class X,class Y>; according to USL C++ Standards Components,
// Rel. 3.0, p9-3 the constructor of the class Y is not allowed
// to require arguments

//// ConnectDefRec
// Standard Constructor
//
ConnectDefRec::ConnectDefRec(void)
{
  mApp             = NULL;
  procId           = "";
  energyFlowNumber = 0;
  balanId          = "";
  energyFlowType   = "";
  linkType         = "";
  direct           = 0;
}

//// ConnectDefRec
// Constructor
//
ConnectDefRec::ConnectDefRec(App* cmApp)
{
  mApp             = cmApp;
  procId           = "";
  energyFlowNumber = 0;
  balanId          = "";
  energyFlowType   = "";
  linkType         = "";
  direct           = 0;

#ifdef _TEST_     // inner cycle test only if necessary
  if (cmApp->testFlag)
    cmApp->message(1001, "ConnectDefRec");
#endif
}


////~ConnectDefRec
// Destructor
//
ConnectDefRec::~ConnectDefRec(void)
{
#ifdef _TEST_                // inner cycle test only if necessary
  if (mApp != 0)             // correctly initialized?
    {
      if (mApp->testFlag)
        mApp->message(1002, "ConnectDefRec");
    }
#endif
}


//// operator<<
// write connection definition record
//
ostream& operator<<(ostream& os, const  ConnectDefRec& dr)
{
  os << dr.procId         << " " << dr.energyFlowNumber << " " << dr.balanId << " "
     << dr.energyFlowType << " " << dr.linkType         << " " << dr.direct;
  return os;
}


//////////////////////////////////////////////////////////////////
//
// Class: ConnectDef
//
//////////////////////////////////////////////////////////////////


//// ConnectDef
// Standard Constructor
//
ConnectDef::ConnectDef(void)
{
  // no content, call ioDefValue(void)
}


//// ConnectDef
// Constructor
//
ConnectDef::ConnectDef(App * cDefApp, String dTN) : ioDefValue(cDefApp, dTN)
{
  // no contents
}


//// ~ConnectDef
// Constructor
//
ConnectDef::~ConnectDef(void)
{
  // no content, call ioDefValue(void)
}


//// writeInput
// write input data (control)
//
void ConnectDef::writeInput(void)
{
#ifdef _TEST_
  if (ioApp->testFlag)
    {
      ostrstream ssConnectDefVal;            // <strstream.h>
      ssConnectDefVal << ioDataName
                      << " : "
                      << defMap
                      << "\n"
                      << ends;
      char* pConnectDefVal = ssConnectDefVal.str();
      ioApp->message(1009, pConnectDefVal);
      delete pConnectDefVal;  // Robbie: 18.11.2005: delete was delete[],
                              // deallocation is correct, see man strstream
                              // and method str().
    }
#endif
}


//// selected
// selection check
//
int ConnectDef::selected(Symbol sId)
{
  if (defMap.element(sId))                   // <Map.h>
    {
      if (defMap[sId].direct == 0)
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
void ConnectDef::readRecord(void)
{
  if (!ioApp->emptyLine(finData))
    {
      int i = 0;
      finData >> Id;
      i = ioApp->readComma(finData);              // read ","
      finData >> defMap[Id].procId;
      i = i + ioApp->readComma(finData);          // read ","
      finData >> defMap[Id].energyFlowNumber;
      i = i + ioApp->readComma(finData);          // read ","
      finData >> defMap[Id].balanId;
      i = i + ioApp->readComma(finData);          // read ","
      finData >> defMap[Id].energyFlowType;
      i = i + ioApp->readComma(finData);          // read ","
      finData >> defMap[Id].linkType;
      i = i + ioApp->readComma(finData);          // read ","
      finData >> defMap[Id].direct;
      if (i != 0)
        finData.clear(ios::badbit | finData.rdstate());
                                      // raise errorflags of finData
                                      // <fstream.h> -> <iostream.h>
    }
  ioApp->ignoreRestLine(finData);     //"App.h"
}


//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/Connect.C,v $
//  end of file

