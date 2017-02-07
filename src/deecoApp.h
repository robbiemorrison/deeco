
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
// DEECO APPLICATION OBJECT
//
//////////////////////////////////////////////////////////////////

#ifndef _DEECOAPP_            // header guard
#define _DEECOAPP_

#define EPSOPT 10e-6          // for (double == 0) comparisons

#include "App.h"
#include "Data.h"
#include "Scen.h"
#include "Net.h"
#include "Proc.h"
#include "Balan.h"
#include "Connect.h"

//////////////////////////////////////////////////////////////////
//
// CLASS: deecoApp
//
//////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
//
// Summary: 'deeco' application object

class deecoApp
{
public:

  // Standard constructor
  deecoApp(void);

  // Constructor
  deecoApp(App* cdApp);

  // Destructor
  virtual ~deecoApp(void);

  // run deeco
  void run(void);

protected:

  App*        dApp;                   // "App.h"

  ioDefValue* actualScenDef;          // "Data.h"
  ProcDef*    actualProcDef;          // "Proc.h"
  BalanDef*   actualBalanDef;         // "Balan.h"
  ConnectDef* actualConnectDef;       // "Connect.h"
  ioDefValue* actualAggDef;           // "Data.h"

  ScenVal*    actualScenVal;          // "Scen.h"
  ioValue*    actualProcVal;          // "Data.h"
  ioValue*    actualBalanVal;         // "Data.h"
  ioValue*    actualConnectVal;       // "Data.h"
  ioValue*    actualAggVal;           // "Data.h"

  Net*        actualNet;              // "Net.h"

  ////// help functions //////

  // run all scenarios
  void runScenarios(void);

  // run all intervals of a single scenario
  void runIntervals(String scenId);

  // run all intervals of a single scenario; dynamic optimization
  void runIntervalsDyn(String scenId);

  // run all intervals of a single scenario; quasidynamic optimization
  void runIntervalsQuasi(String scenId);

  // scan one process parameter value
  void scan(String scenId);

  // prepare ioDefValue objects
  void prepareDef(void);

  // finish ioDefValue objects
  void finishDef(String scenId);

  // prepare ioValue objects
  void prepareVal(String scenId);

  // show result-data of ioValue objects
  void resultVal(String scenId, int scenScanF, double scenScanV);

  // calculate percentage of supply or demand
  void calcPercentage(void);

  // transfer mean values from processes to outValMap
  void transferOutValMap(void);

  // finish ioValue objects
  void finishVal(void);

  // prepare the energy supply net object
  void prepareNet(String scenId);

  // finish the net object
  void finishNet(void);

  // optimizing in one time interval (including several subintervals of variable length)
  void optimize(ScenTsPack* oScenTsPack, ioTsPack* oProcTsPack, int i);

};

#endif  // _DEECOAPP_
