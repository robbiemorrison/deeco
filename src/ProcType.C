
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


#include "ProcType.h"
#include <stdlib.h>           // for use of exit function
#include "Collect.h"          // collector processes
#include "Port.h"             // import-export processes
#include "Demand.h"           // demand processes
#include "Storage.h"          // storage processes
#include "Convers.h"          // conversion processes
#include "Network.h"          // network processes


///////////////////////////////////////////////////////////////////
//
// CLASS: ProcType
//
///////////////////////////////////////////////////////////////////


//// ProcType
// Standard Constructor
//
ProcType::ProcType(void)
{
  pApp      = NULL;
  errorFlag = 0;
}


//// ProcType
// Constructor
//
ProcType::ProcType(App* cpApp)
{
  pApp=cpApp;
  if (pApp->testFlag)
    {
      pApp->message(1001, "ProcType");
    }
  errorFlag = 0;
}


//// ~ProcType
// Destructor
//
ProcType::~ProcType(void)
{
if (pApp != 0)
  {
    if (pApp->testFlag)
      pApp->message(1002, "ProcType");
  }
}


//// prepareProcess
// search for each process type the corresponding
// module constructor and initialize the process parameters
// j++ if the process type is found
//
Proc* ProcType::prepareProcess(App* pApp, Symbol procId, Symbol procType,
                               ioValue* nProcVal)
{
  int j = 0;
  Proc* p;
  p = NULL;

  //????? constructoraufruf f\"ur type1 Process, Proze\3oberklasse
  //????? im folgenden zuordnen; Proc(     )ersetzen, durch type1(    )
  //????? include files nicht vergessen!!!!!
  //
  // Translation: constructor call for type 1 process, process base class
  // in the following access/allocation; Proc( ) replace, through/by
  // type1( ) include files not forget.


    if (procType == "0")
      {
        p = new Proc(pApp,procId,procType,nProcVal);
        j++;
      }


///////////////////////////////////////////////////////////////////
//                    MODULE REGISTRATION                        //
///////////////////////////////////////////////////////////////////


// Template for each new process type (also called technology module):
//
//  if (procType == "procTypeName")
//    {
//      p = new ProcTypeName(pApp,procId,procType,nProcVal);
//      j++;
//    }


// Demand processes:


   if (procType == "DConstEl")
      {
        p = new DConstEl(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DConstMech")
      {
        p = new DConstMech(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DConstH")
      {
        p = new DConstH(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DConstQ")
      {
        p = new DConstQ(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DConstWH")
      {
        p = new DConstWH(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DConstWQ")
      {
        p = new DConstWQ(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DFlucEl")
      {
        p = new DFlucEl(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DFlucMech")
      {
        p = new DFlucMech(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DFlucH")
      {
        p = new DFlucH(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DFlucQ")
      {
        p = new DFlucQ(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DFlucWH")
      {
        p = new DFlucWH(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DFlucWQ")
      {
        p = new DFlucWQ(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DRoom")
      {
        p = new DRoom(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DNet4Room")
      {
        p = new DNet4Room(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "DHeat")
      {
        p = new DHeat(pApp,procId,procType,nProcVal);
        j++;
      }

// Conversion processes:


   if (procType == "CBoiConst")
      {
        p = new CBoiConst(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "CBoiConv")
      {
        p = new CBoiConv(pApp,procId,procType,nProcVal);
        j++;
      }

    if (procType == "CBoiCond")
      {
        p = new CBoiCond(pApp,procId,procType,nProcVal);
        j++;
      }

    if (procType == "CBoiOfl")
      {
        p = new CBoiOfl(pApp,procId,procType,nProcVal);
        j++;
      }

    if (procType == "CEHP")
      {
        p = new CEHP(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "CGHP")
      {
        p = new CGHP(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "CBPT")
      {
        p = new CBPT(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "CBPTo")
      {
        p = new CBPTo(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "CECT")
      {
        p = new CECT(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "CECTo")
      {
        p = new CECTo(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "CCogConst")
      {
        p = new CCogConst(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "CGasTHR")
      {
        p = new CGasTHR(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "CFCellConst")
      {
        p = new CFCellConst(pApp,procId,procType,nProcVal);
        j++;
      }

// Import-Export processes:


   if (procType == "PConstElImp")
      {
        p = new PConstElImp(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "PConstElExp")
      {
        p = new PConstElExp(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "PConstFuelImp")
      {
        p = new PConstFuelImp(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "PFlucElImp")
      {
        p = new PFlucElImp(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "PFlucElExp")
      {
        p = new PFlucElExp(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "PFlucFuelImp")
      {
        p = new PFlucFuelImp(pApp,procId,procType,nProcVal);
        j++;
      }


// Collector processes:


   if (procType == "OSol")
      {
        p = new OSol(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "OSolHEx" || procType == "OSol_HEx" )
      {
        p = new OSolHEx(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "OPVSol" || procType == "PVSol" )
      {
        p = new OPVSol(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "OWind" || procType == "Wind" )
      {
        p = new OWind(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "OWind2" || procType == "Wind2" )
      {
        p = new OWind2(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "OConPT")
      {
        p = new OConPT(pApp,procId,procType,nProcVal);
        j++;
      }

// Storage processes:


   if (procType == "SSenH")
      {
        p = new SSenH(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "SSupEl")
      {
        p = new SSupEl(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "SSenHn" || procType == "SSenHStratN" )
      {
        p = new SSenHn(pApp,procId,procType,nProcVal);
        j++;
      }

   if (procType == "SSenHnHEx" || procType == "SSenHStratN_HEx" )
      {
        p = new SSenHnHEx(pApp,procId,procType,nProcVal);
        j++;
      }

// Network processes


  if (procType == "NBW")
    {
      p = new NBW(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NBWAConst")
    {
      p = new NBWAConst(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NBWAConv")
    {
      p = new NBWAConv(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NBWACond")
    {
      p = new NBWACond(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NHEx")
    {
      p = new NHEx(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NHTrans")
    {
      p = new NHTrans(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NQTrans")
    {
      p = new NQTrans(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NEHP")
    {
      p = new NEHP(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NAHP" || procType == "NAWP")
    {
      p = new NAHP(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NBWACog" || procType == "NBWA_Cog")
    {
      p = new NBWACog(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NBWAAHP" || procType == "NBWA_AWP")
    {
      p = new NBWAAHP(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NBWAEHP" || procType == "NBWA_EHP")
    {
      p = new NBWAEHP(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NBWAGHP" || procType == "NBWA_GHP")
    {
      p = new NBWAGHP(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NHTransSol" || procType == "NHTrans_Sol")
    {
      p = new NHTransSol(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NHEx24")
    {
      p = new NHEx24(pApp,procId,procType,nProcVal);
      j++;
    }

  if (procType == "NStMan")
    {
      p = new NStMan(pApp,procId,procType,nProcVal);
      j++;
    }


// perform some tests:


  if (j == 0)
        {
          pApp->message(25, procType.the_string() +
                " of " +  procId.the_string());
          errorFlag++;
        }

  if (p == 0)
        {
          pApp->message(11, "new Proc in ProcType.C");
          exit(11);
        }

  return p;

};


// end of file



