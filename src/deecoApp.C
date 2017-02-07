
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
//  $Date: 2005/09/30 18:54:45 $
//  $Author: deeco $
//  $RCSfile: deecoApp.C,v $

/////////////////////////////////////////////////////////////////
//
// DEECO APPLICATION OBJECT
//
/////////////////////////////////////////////////////////////////

#include "deecoApp.h"
#include <stdlib.h>           // for use of exit function
#include <Path.h>             // path name Management
#include <math.h>             // for floating point error management

//// deecoApp
// Standard constructor
//
deecoApp::deecoApp(void)
{
  dApp             = NULL;
  actualScenDef    = NULL;
  actualScenVal    = NULL;
  actualProcDef    = NULL;
  actualProcVal    = NULL;
  actualBalanDef   = NULL;
  actualBalanVal   = NULL;
  actualConnectDef = NULL;
  actualConnectVal = NULL;
  actualAggDef     = NULL;
  actualAggVal     = NULL;
  actualNet        = NULL;
}

//// deecoApp
//
deecoApp::deecoApp(App* cdApp)
{
  dApp=cdApp;
  if (dApp->testFlag)
    {
      dApp->message(1001, "deecoApp");
    }
  actualScenDef = new ioDefValue(dApp, "ScenDef");
  if (actualScenDef == 0)
    {
      dApp->message(11, "new ioDefValue in deecoApp.C");
      exit(11);
    }
  actualScenVal    = NULL;
  actualProcDef    = NULL;
  actualProcVal    = NULL;
  actualBalanDef   = NULL;
  actualBalanVal   = NULL;
  actualConnectDef = NULL;
  actualConnectVal = NULL;
  actualAggDef     = NULL;
  actualAggVal     = NULL;
  actualNet        = NULL;
}

//// ~deecoApp
//
deecoApp::~deecoApp(void)
{
  if (actualScenDef)
    delete actualScenDef;
  if (dApp != 0)
    {
      if (dApp->testFlag)
        dApp->message(1002, "deecoApp");
    }
}

//// run
//
void deecoApp::run(void)
{
  Path p (dApp->projectName + ".sd");           // <Path.h>
  Path scenDefFileName = dApp->projectPath/p;   // <Path.h>
  actualScenDef->read(scenDefFileName);
  if (!actualScenDef->error())
    actualScenDef->writeInput();
  if (!actualScenDef->error())
    runScenarios();                             // help function, see below
}

/////////// help functions /////////////////////////////////////////////

//// runScenarios
// run all scenarios
//
void deecoApp::runScenarios(void)
{
  for (Mapiter<Symbol, int> defMapIt = actualScenDef->defMap.first();
       defMapIt; defMapIt.next())
    {
      if (actualScenDef->selected(defMapIt.curr()->key))
        {
          String scenName = defMapIt.curr()->key.the_string();
          // <Symbol.h>
          prepareDef();
          int defErrorFlag = actualProcDef->error() + actualBalanDef->error()
            + actualConnectDef->error() + actualAggDef->error();
          if (!defErrorFlag)
            {
              prepareVal(scenName);
              int valErrorFlag = actualProcVal->error()
                + actualBalanVal->error()+ actualConnectVal->error()
                + actualAggVal->error();
              if (!valErrorFlag)
                {
                  prepareNet(scenName);
                  if (!actualNet->error())
                    scan(scenName);
                  finishNet();
                }
              finishVal();
            }
          finishDef(scenName);
        }
    }
}

//// scan
// Scanning of one process parameter
//
void deecoApp::scan(String scenId)
{
  if (!actualScenVal->scanFlag)
    {
      if (!actualScenVal->error() && !actualProcVal->error())
        {
          runIntervals(scenId);
          resultVal(scenId, actualScenVal->scanFlag, 0);
        }
    }
  else
    {
      if (!actualScenVal->tsOutFlag)
        {
          Mapiter<Symbol,procPtr> procMapIt (actualNet->procMap);
          procMapIt = actualNet->procMap.element(actualScenVal->
                                                 scanParaProcName);
          // for "procPtr" see typedef in Net.h

          if (procMapIt && procMapIt.curr()->value->
              procInValMap.element(actualScenVal->scanParaName))
            {
              scenId = scenId + "_" + actualScenVal->scanParaName.the_string(); // !1.8.95
              for(double scanParaVal = actualScenVal->scanParaBegin;
                  scanParaVal <= actualScenVal->scanParaEnd;
                  scanParaVal = scanParaVal +  actualScenVal->scanParaStep)
                {

                  // overwrite process parameter value

                  procMapIt.curr()->value->procInValMap
                    [actualScenVal->scanParaName] = scanParaVal;

                  if (!actualScenVal->error() && !actualProcVal->error())
                    {
                      runIntervals(scenId);
                      resultVal(scenId, actualScenVal->scanFlag, scanParaVal);
                    }
                }
            }
          else
            dApp->message(19, actualScenVal->scanParaProcName.the_string()
                          + " " + actualScenVal->scanParaName.the_string());
          // <Symbol.h>
        }
      else
        dApp->message(20,actualScenVal->
                      scanParaProcName.the_string()
                      + " " + actualScenVal->scanParaName.the_string());
    }
}

//// prepareDef
// Prepare the run of all scenarios
//
void deecoApp::prepareDef(void)
{
  String procDefFileName =
      Path(dApp->projectPath, dApp->projectName, ".pd");    // Robbie: 21.07.05: added explicit Path()
  actualProcDef = new ProcDef(dApp, "ProcDef");
  if (actualProcDef == 0)
    {
      dApp->message(11, "new ProcDef in Scen.C");
      exit(11);
    }

  // read standard process definition values

  actualProcDef->read(procDefFileName);

  String balanDefFileName =
      Path(dApp->projectPath, dApp->projectName, ".bd");    // Robbie: 21.07.05: added explicit Path()
  actualBalanDef = new BalanDef(dApp, "BalanDef");
  if (actualBalanDef == 0)
    {
      dApp->message(11, "new BalanDef in Scen.C");
      exit(11);
    }

  // read standard balance definition values

  actualBalanDef->read(balanDefFileName);

  String connectDefFileName =
      Path(dApp->projectPath, dApp->projectName, ".cd");    // Robbie: 21.07.05: added explicit Path()
  actualConnectDef = new ConnectDef(dApp, "ConnectDef");
  if (actualConnectDef == 0)
    {
      dApp->message(11, "new ConnectDef in Scen.C");
      exit(11);
    }

  // read standard connection definition values

  actualConnectDef->read(connectDefFileName);

  String aggDefFileName =
      Path(dApp->projectPath, dApp->projectName, ".ad");    // Robbie: 21.07.05: added explicit Path()
  actualAggDef = new ioDefValue(dApp, "AggDef");
  if (actualAggDef == 0)
    {
      dApp->message(11, "new AggDef in Scen.C");
      exit(11);
    }

  // read standard aggregate definition values

  actualAggDef->read(aggDefFileName);
}

//// finishDef
// finish run
//
void deecoApp::finishDef(String scenId)
{
  if (actualAggDef)     delete actualAggDef;
  if (actualConnectDef) delete actualConnectDef;
  if (actualBalanDef)   delete actualBalanDef;
  if (actualProcDef)    delete actualProcDef;
  dApp->message(1006, scenId);
}

//// prepareVal
// prepare ioValue objects
//
void deecoApp::prepareVal(String scenId)
{
  actualScenVal = new ScenVal(dApp, "ScenVal " + scenId, 1);

  if (actualScenVal == 0)
    {
      dApp->message(11, "new of ScenVal " + scenId);
      exit(11);
    }
  actualScenVal->read(
      Path(dApp->projectPath, scenId, ".siv"));             // Robbie: 21.07.05: added explicit Path()

  if (!actualScenVal->error())
    actualScenVal->writeInput();
  actualScenVal->clearFile(
      Path(actualScenVal->resPath, scenId, ".sov"));        // Robbie: 21.07.05: added explicit Path()

  // overwrite process definition values:

  if (actualProcDef->existFile(
        Path(actualScenVal->scenPath, scenId, ".pd")))      // Robbie: 21.07.05: added explicit Path()
    {
      actualProcDef->read(
          Path(actualScenVal->scenPath, scenId, ".pd"));    // Robbie: 21.07.05: added explicit Path()
      if (dApp->testFlag)
        dApp->message(1013,
            Path(actualScenVal->scenPath, scenId, ".pd"));  // Robbie: 21.07.05: added explicit Path()
    }

  if (!actualProcDef->error())
    actualProcDef->writeInput();

  // overwrite balance definition values:

  if (actualBalanDef->existFile(
        Path(actualScenVal->scenPath, scenId, ".bd")))      // Robbie: 21.07.05: added explicit Path()
    {
      actualBalanDef->read(
          Path(actualScenVal->scenPath, scenId, ".bd"));    // Robbie: 21.07.05: added explicit Path()
      if (dApp->testFlag)
        dApp->message(1013,
            Path(actualScenVal->scenPath, scenId, ".bd"));  // Robbie: 21.07.05: added explicit Path()
    }

  if (!actualBalanDef->error())
    actualBalanDef->writeInput();

  // overwrite connection definition values:

  if (actualConnectDef->existFile(
        Path(actualScenVal->scenPath, scenId, ".cd")))      // Robbie: 21.07.05: added explicit Path()
    {
      actualConnectDef->read(
          Path(actualScenVal->scenPath, scenId, ".cd"));    // Robbie: 21.07.05: added explicit Path()
      if (dApp->testFlag)
        dApp->message(1013,
            Path(actualScenVal->scenPath, scenId, ".cd"));  // Robbie: 21.07.05: added explicit Path()
    }

  if (!actualConnectDef->error())
    actualConnectDef->writeInput();

  // overwrite (process) aggregation definition values:

  if (actualAggDef->existFile(
        Path(actualScenVal->scenPath, scenId, ".ad")))      // Robbie: 21.07.05: added explicit Path()
    {
      actualAggDef->read(
          Path(actualScenVal->scenPath, scenId, ".ad"));    // Robbie: 21.07.05: added explicit Path()
      if (dApp->testFlag)
        dApp->message(1013,
            Path(actualScenVal->scenPath, scenId, ".ad"));  // Robbie: 21.07.05: added explicit Path()
    }

  if (!actualAggDef->error())
    actualAggDef->writeInput();

  // read input values ////////////////////////////////

  actualProcVal = new ioValue(dApp, "ProcVal " + scenId);
  if (actualProcVal == 0)
    {
      dApp->message(11, "new of ProcVal " + scenId);
      exit(11);
    }

  // read standard values

  actualProcVal->read(
      Path(dApp->projectPath, dApp->projectName, ".piv"));  // Robbie: 21.07.05: added explicit Path()

  // overwrite with scenario specific values if there are some

  if (actualProcVal->existFile(
        Path(actualScenVal->scenPath, scenId, ".piv")))     // Robbie: 21.07.05: added explicit Path()
    {
      actualProcVal->read(
          Path(actualScenVal->scenPath, scenId, ".piv"));   // Robbie: 21.07.05: added explicit Path()
      if (dApp->testFlag)
        dApp->message(1013,
            Path(actualScenVal->scenPath, scenId, ".piv")); // Robbie: 21.07.05: added explicit Path()
    }
  if (!actualProcVal->error())
    actualProcVal->writeInput();

  actualProcVal->clearFile(
      Path(actualScenVal->resPath, scenId, ".pov"));        // Robbie: 21.07.05: added explicit Path()

  actualBalanVal = new ioValue(dApp, "BalanVal " + scenId);
  if (actualBalanVal == 0)
    {
      dApp->message(11, "new of BalanVal " + scenId);
      exit(11);
    }

  // no input balance values

  // no longer necessary, but don't delete
  //
  // actualBalanVal->clearFile(actualScenVal->resPath/scenId + ".bov");

  actualConnectVal = new ioValue(dApp, "ConnectVal " + scenId);
  if (actualConnectVal == 0)
    {
      dApp->message(11, "new of ConnectVal " + scenId);
      exit(11);
    }

  // no input connection values

  // no longer necessary, but don't delete
  //
  // actualConnectVal->clearFile(actualScenVal->resPath/scenId + ".cov");

  actualAggVal = new ioValue(dApp, "AggVal " + scenId);
  if (actualAggVal == 0)
    {
      dApp->message(11, "new of AggVal " + scenId);
      exit(11);
    }

  // read standard values

  actualAggVal->read(
      Path(dApp->projectPath, dApp->projectName, ".aiv"));  // Robbie: 21.07.05: added explicit Path()

  // overwrite with scenario specific values if there are some

  if (actualAggVal->existFile(
        Path(actualScenVal->scenPath,scenId, ".aiv")))      // Robbie: 21.07.05: added explicit Path()
    {
      actualAggVal->read(
          Path(actualScenVal->scenPath,scenId, ".aiv"));    // Robbie: 21.07.05: added explicit Path()
      if (dApp->testFlag)
        dApp->message(1013,
            Path(actualScenVal->scenPath, scenId, ".aiv")); // Robbie: 21.07.05: added explicit Path()
    }
  if (!actualAggVal->error())
    actualAggVal->writeInput();

  actualAggVal->clearFile(
      Path(actualScenVal->resPath, scenId, ".aov"));        // Robbie: 21.07.05: added explicit Path()

}

//// resultVal
// show result-data of  ioValue objects
//
void deecoApp::resultVal(String scenId, int scenScanF, double scenScanV)
{
  if (!actualNet->error())
    {
      calcPercentage();
    }

  // no longer necessary
  //
  //   if (!actualNet->error() && !actualConnectVal->error())
  //   actualConnectVal->write(actualScenVal->resPath/scenId + ".cov", scenScanF,
  //                           scenScanV);
  //   actualConnectVal->clearResults();
  //
  // if this part is re-established, don't forget to restore clearFile
  // in prepareVal (search for .cov)

  // no longer necessary
  //
  //   if (!actualNet->error() && !actualBalanVal->error())
  //       actualBalanVal->write(actualScenVal->resPath/scenId + ".bov", scenScanF,
  //                             scenScanV);
  //   actualBalanVal->clearResults();
  //
  // if this part is re-established, don't forget to restore clearFile
  // in prepareVal (search for .bov)

  if (!actualNet->error())
    transferOutValMap();
  if (!actualNet->error() && !actualProcVal->error())
    actualProcVal->write(
        Path(actualScenVal->resPath, scenId, ".pov"),       // Robbie: 21.07.05: added explicit Path()
        scenScanF, scenScanV);
  actualProcVal->clearResults();

  if (!actualNet->error() && !actualAggVal->error())
    actualAggVal->write(
        Path(actualScenVal->resPath, scenId, ".aov"),       // Robbie: 21.07.05: added explicit Path()
        scenScanF, scenScanV);
  actualAggVal->clearResults();

  if (!actualNet->error() && !actualScenVal->error())
    actualScenVal->write(actualScenVal->resPath/Path(scenId + ".sov"), scenScanF,
                           scenScanV);
  actualScenVal->clearResults();
}

//// calcPercentage
// calculate percentage of supply or demand
//
void deecoApp::calcPercentage(void)
{
  double meanDotEEl = 0;

  // calculate total energy flow through the balances

  for (Mapiter <Symbol, procPtr> procMapIt = actualNet->procMap.first();
       procMapIt; procMapIt.next())
    {

      // Robbie: 02.09.2005: split original statement into two for ISO C++
      //
      Set_of_p <DEdge> temp01 = procMapIt.curr()->value->in_edges();
      Set_of_piter <DEdge> inEdgeIt( temp01 );

      edgePtr eEn;

      while (eEn = inEdgeIt.next())  // "=" is okay
        {
          balanPtr(eEn->src())->meanDotE += procMapIt.curr()->value->
            meanDotEEn[eEn->energyFlowType]
            [int_to_str(eEn->energyFlowNumber)].cutoff();
        }

      // Set_of_p<DEdge> outEdgeIt..... should lead to the same ...

      // electrical energy

      Mapiter <Symbol,MapSym1M> meanDotIt1 (procMapIt.curr()->value->meanDotEEn);
      meanDotIt1 = procMapIt.curr()->value->meanDotEEn.element("El");
      if (meanDotIt1)
        {
          for (Mapiter <Symbol,MeanValRec> meanDotIt2 = meanDotIt1.curr()->value.first();
               meanDotIt2; meanDotIt2.next())
            meanDotEEl += meanDotIt2.curr()->value.cutoff(); // !31.5.95
        }
    }

  // calculate shares of energy flows

  for (Mapiter <Symbol, procPtr> procMap2It = actualNet->procMap.first();
       procMap2It; procMap2It.next())
    {
      String sep = "][";
      String end = "]";
      MeanValRec procMeanValRec;

      String sym = "RelDotE[En][";

      // Robbie: 02.09.2005: split original statement into two for ISO C++
      //
      Set_of_p <DEdge> temp02 = procMap2It.curr()->value->in_edges();
      Set_of_piter <DEdge> inEdgeIt( temp02 );

      edgePtr eEn;
      inEdgeIt.reset();               // to be sure that ... // Robbie: 26.08.05: Not needed?
      while (eEn = inEdgeIt.next())   // "=" is okay
        {
          String sym1;
          sym1 = sym + eEn->energyFlowType.the_string() + sep +
            int_to_str(eEn->energyFlowNumber) + end;

      // The following code uses fpsetmask(FP_X_CLEAR) to switch off ALL
      // floating-point exceptions.  The program thinks that the underflow
      // and inexact exceptions are already off.  We could if necessary hack
      // this by using some of the <ieeefp.h> code to switch off the remaining
      // exceptions, but under SCO UnixWare all the exceptions are off anyway.
      // Question:  perhaps we DO want floating point overflow exceptions
      // generated?  Because the program thinks they will be, the programmer(s)
      // probably didn't put in any checks for infinities.
      //
      // Anyway, these calls here are protecting a statement that contains several
      // function calls.  What possible exceptions exactly are we guarding against?
      // curr() and src() are concerned with the graph, and don't do any floating
      // point calculations.  expand() and cutoff() are defined in Data.C; the
      // latter simply stores its argument, and the former stores its argument and
      // then sets three attributes to NaN, which is none of our business HERE, and
      // in fact the code was (and is) arranged to take care of this itself.

      // Robbie: rem'd fpsetmask
      //
      //      fpsetmask(FP_X_CLEAR);  // don't trap floating point exception for
                                      // the moment <math.h>

          procMeanValRec.expand(procMap2It.curr()->value->meanDotEEn
              [eEn->energyFlowType][int_to_str(eEn->energyFlowNumber)].cutoff()/
              balanPtr(eEn->src())->meanDotE);

      // Robbie: rem'd fpsetdefaults
      //
      //      fpsetdefaults();        // enable floating point exception trap <math.h>

          actualProcVal->outValMap[procMap2It.curr()->key][sym1]=procMeanValRec;
        }

      sym = "RelDotE[Ex][";

      // Robbie: 02.09.2005: split original statement into two for ISO C++
      //
      Set_of_p <DEdge> temp03 = procMap2It.curr()->value->out_edges();
      Set_of_piter <DEdge> outEdgeIt( temp03 );

      edgePtr eEx;
      outEdgeIt.reset();              // to be sure that ...
      while (eEx = outEdgeIt.next())  // "=" is okay
        {
          String sym1;
          sym1 = sym + eEx->energyFlowType.the_string() + sep
            + int_to_str(eEx->energyFlowNumber) + end;

      // Robbie: rem'd fpsetmask
      //
      //      fpsetmask(FP_X_CLEAR);  // don't trap floating point exception for
                                      // the moment <math.h>

          procMeanValRec.expand(procMap2It.curr()->value->meanDotEEx
              [eEx->energyFlowType][int_to_str(eEx->energyFlowNumber)].cutoff()/
              balanPtr(eEx->dst())->meanDotE);

      // Robbie: rem'd fpsetdefaults
      //
      //      fpsetdefaults();        // enable floating point exception trap <math.h>

          actualProcVal->outValMap[procMap2It.curr()->key][sym1] = procMeanValRec;
        }

      // !31.5.95

      if (procMap2It.curr()->value->meanDotEEn.element("El"))
        {
          sym = "RelDotE[En][El][";
          for (Mapiter <Symbol,MeanValRec> meanDotEEnElIt =
                 procMap2It.curr()->value->meanDotEEn["El"].first();
               meanDotEEnElIt; meanDotEEnElIt.next())
            {
              String sym1;
              sym1 = sym + meanDotEEnElIt.curr()->key.the_string() + end;

      // Robbie: rem'd fpsetmask
      //
      //      fpsetmask(FP_X_CLEAR);  // don't trap floating point exception for
                                      // the moment <math.h>

              procMeanValRec.expand(meanDotEEnElIt.curr()->value.cutoff()/meanDotEEl);

      // Robbie: rem'd fpsetdefaults
      //
      //      fpsetdefaults();        // enable floating point exception trap <math.h>

              actualProcVal->outValMap[procMap2It.curr()->key][sym1] = procMeanValRec;
            }
        }

      // !31.5.95

      if (procMap2It.curr()->value->meanDotEEx.element("El"))
        {
          sym = "RelDotE[Ex][El][";
          for (Mapiter <Symbol,MeanValRec> meanDotEExElIt =
                 procMap2It.curr()->value->meanDotEEx["El"].first();
               meanDotEExElIt; meanDotEExElIt.next())
            {
              String sym1;
              sym1 = sym + meanDotEExElIt.curr()->key.the_string() + end;

      // Robbie: rem'd fpsetmask
      //
      //      fpsetmask(FP_X_CLEAR);  // don't trap floating point exception for
                                      // the moment <math.h>

              procMeanValRec.expand(meanDotEExElIt.curr()->value.cutoff()/meanDotEEl);

      // Robbie: rem'd fpsetdefaults
      //
      //      fpsetdefaults();        // enable floating point exception trap <math.h>

              actualProcVal->outValMap[procMap2It.curr()->key][sym1] = procMeanValRec;
            }
        }
    }
}

//// transferOutValMap
// transfer mean values from processes to outValMap
//
void deecoApp::transferOutValMap(void)
{
  String sep = "][";
  String end = "]";
  String sym;

  for (Mapiter <Symbol,procPtr> procMapIt = actualNet->procMap.first();
       procMapIt; procMapIt.next())
    {
      // meanDotEEx

      sym = "DotE[Ex][";
      for (Mapiter<Symbol, MapSym1M> meanDotEExIt1 = procMapIt.curr()->
             value->meanDotEEx.first(); meanDotEExIt1; meanDotEExIt1.next())
        {
          for (Mapiter<Symbol,MeanValRec> meanDotEExIt2 = meanDotEExIt1.curr()->
                 value.first(); meanDotEExIt2; meanDotEExIt2.next())
            {
              String sym1;
              String sym2;
              sym1 = sym + meanDotEExIt1.curr()->key.the_string() + sep
                  + meanDotEExIt2.curr()->key.the_string()+end;
              actualProcVal->outValMap[procMapIt.curr()->key][sym1] =
                meanDotEExIt2.curr()->value;
              sym2 = "Sum" + sym + meanDotEExIt1.curr()->key.the_string() + end;
              actualScenVal->scenOutValMap[sym2].expand
                (actualScenVal->scenOutValMap[sym2].cutoff()
                 + meanDotEExIt2.curr()->value.cutoff());
            }
        }

      // meanDotEEn

      sym = "DotE[En][";
      for (Mapiter<Symbol, MapSym1M> meanDotEEnIt1 = procMapIt.curr()->
             value->meanDotEEn.first(); meanDotEEnIt1; meanDotEEnIt1.next())
        {
          for (Mapiter<Symbol,MeanValRec> meanDotEEnIt2 = meanDotEEnIt1.curr()->
                 value.first(); meanDotEEnIt2; meanDotEEnIt2.next())
            {
              String sym1;
              String sym2;
              sym1 = sym + meanDotEEnIt1.curr()->key.the_string() + sep
                + meanDotEEnIt2.curr()->key.the_string() + end;
              actualProcVal->outValMap[procMapIt.curr()->key][sym1] =
                meanDotEEnIt2.curr()->value;
              sym2 = "Sum" + sym + meanDotEEnIt1.curr()->key.the_string() + end;
              actualScenVal->scenOutValMap[sym2].expand
                (actualScenVal->scenOutValMap[sym2].cutoff()
                 + meanDotEEnIt2.curr()->value.cutoff());
            }
        }

      // meanE_s

      sym = "E_s[";
      for (Mapiter<int, MeanValRec> meanE_sIt = procMapIt.curr()->
             value->meanE_s.first(); meanE_sIt; meanE_sIt.next())
        {
          String sym1;
          sym1 = sym + int_to_str(meanE_sIt.curr()->key) + end;
          actualProcVal->outValMap[procMapIt.curr()->key][sym1] =
            meanE_sIt.curr()->value;
        }

      // E_s (final) // !2.8.95->

      sym = "E_sF[";
      for (Mapiter<int, double> E_sIt = procMapIt.curr()->
             value->E_s.first(); E_sIt; E_sIt.next())
        {
          String sym1;
          sym1 = sym + int_to_str(E_sIt.curr()->key) + end;
          actualProcVal->outValMap[procMapIt.curr()->key][sym1].expand
            (E_sIt.curr()->value);
        }            // !2.8.95<-

      // meanDotE_0

      sym = "DotE0[";
      for (Mapiter<int, MeanValRec> meanDotE_0It = procMapIt.curr()->
             value->meanDotE_0.first(); meanDotE_0It; meanDotE_0It.next())
        {
          String sym1;
          sym1 = sym + int_to_str(meanDotE_0It.curr()->key) + end;
          actualProcVal->outValMap[procMapIt.curr()->key][sym1] =
            meanDotE_0It.curr()->value;
        }

      // meanSlack

      sym = "Slack[";
      for (Mapiter<int, MeanValRec> meanSlackIt = procMapIt.curr()->
             value->meanSlack.first(); meanSlackIt; meanSlackIt.next())
        {
          String sym1;
          sym1 = sym + int_to_str(meanSlackIt.curr()->key) + end;
          actualProcVal->outValMap[procMapIt.curr()->key][sym1] =
            meanSlackIt.curr()->value;
        }

      // meanShadowPrice

      sym = "SPrice[";
      for (Mapiter<int, MeanValRec> meanShadowPriceIt = procMapIt.curr()->
             value->meanShadowPrice.first(); meanShadowPriceIt;
           meanShadowPriceIt.next())
        {
          String sym1;
          sym1 = sym + int_to_str(meanShadowPriceIt.curr()->key) + end;
          actualProcVal->outValMap[procMapIt.curr()->key][sym1] =
            meanShadowPriceIt.curr()->value;
        }

      // meanCoef

      sym = "CCoef[";
      for (Mapiter<int,MapSym3M> meanCoefIt1 = procMapIt.curr()->
             value->meanCoef.first(); meanCoefIt1; meanCoefIt1.next())
        {
          for (Mapiter<Symbol, MapSym2M> meanCoefIt2 = meanCoefIt1.curr()->
                 value.first(); meanCoefIt2; meanCoefIt2.next())
            {
              for (Mapiter<Symbol, MapSym1M> meanCoefIt3 = meanCoefIt2.curr()->
                     value.first(); meanCoefIt3; meanCoefIt3.next())
                {
                  for (Mapiter<Symbol, MeanValRec> meanCoefIt4 =
                         meanCoefIt3.curr()->value.first(); meanCoefIt4;
                       meanCoefIt4.next())
                    {
                      String sym1;
                      sym1 = sym + int_to_str(meanCoefIt1.curr()->key)
                           + sep + meanCoefIt2.curr()->key.the_string()
                           + sep + meanCoefIt3.curr()->key.the_string()
                           + sep + meanCoefIt4.curr()->key.the_string()
                           + end;

                      actualProcVal->outValMap[procMapIt.curr()->key][sym1] =
                        meanCoefIt4.curr()->value;
                    }
                }
            }
        }

      // meanObjectFuncCoef

      sym = "OCoef[";
      for (Mapiter<Symbol,MapSym3M> meanObjectFuncCoefIt1 = procMapIt.curr()->
             value->meanObjectFuncCoef.first(); meanObjectFuncCoefIt1;
           meanObjectFuncCoefIt1.next())
        {
          for (Mapiter<Symbol, MapSym2M> meanObjectFuncCoefIt2 =
                 meanObjectFuncCoefIt1.curr()->value.first();
               meanObjectFuncCoefIt2; meanObjectFuncCoefIt2.next())
            {
              for (Mapiter<Symbol, MapSym1M> meanObjectFuncCoefIt3 =
                     meanObjectFuncCoefIt2.curr()->value.first();
                   meanObjectFuncCoefIt3; meanObjectFuncCoefIt3.next())
                {
                  for (Mapiter<Symbol, MeanValRec> meanObjectFuncCoefIt4 =
                         meanObjectFuncCoefIt3.curr()->value.first();
                       meanObjectFuncCoefIt4;meanObjectFuncCoefIt4.next())
                    {
                      String sym1;
                      sym1 = sym + meanObjectFuncCoefIt1.curr()->key.the_string()
                           + sep + meanObjectFuncCoefIt2.curr()->key.the_string()
                           + sep + meanObjectFuncCoefIt3.curr()->key.the_string()
                           + sep + meanObjectFuncCoefIt4.curr()->key.the_string()
                           + end;
                      actualProcVal->outValMap[procMapIt.curr()->key][sym1] =
                        meanObjectFuncCoefIt4.curr()->value;
                    }
                }
            }
        }

      // meanVecJ

      sym = "VecJ[";
      for (Mapiter<Symbol,MapSym4M> meanVecJIt1 = procMapIt.curr()
             ->value->meanVecJ.first(); meanVecJIt1;meanVecJIt1.next())
        {
          for (Mapiter<Symbol,MapSym3M> meanVecJIt2 = meanVecJIt1.curr()
                 ->value.first(); meanVecJIt2; meanVecJIt2.next())
            {
              for (Mapiter<Symbol,MapSym2M> meanVecJIt3 = meanVecJIt2.curr()
                     ->value.first(); meanVecJIt3; meanVecJIt3.next())
                {
                  for (Mapiter<Symbol,MapSym1M> meanVecJIt4=meanVecJIt3.curr()
                         ->value.first();meanVecJIt4;meanVecJIt4.next())
                    {
                      for (Mapiter<Symbol,MeanValRec> meanVecJIt5 =
                             meanVecJIt4.curr()->value.first();meanVecJIt5;
                           meanVecJIt5.next())
                        {
                          String sym1;
                          sym1 = sym + meanVecJIt1.curr()->key.the_string()
                               + sep + meanVecJIt2.curr()->key.the_string()
                               + sep + meanVecJIt3.curr()->key.the_string()
                               + sep + meanVecJIt4.curr()->key.the_string()
                               + sep + meanVecJIt5.curr()->key.the_string()
                               + end;
                          actualProcVal->outValMap
                            [procMapIt.curr()->key][sym1] =
                            meanVecJIt5.curr() ->value;
                        }
                    }
                }
            }
        }
      procMapIt.curr()->value->addFixCosts(actualScenVal->scenOutValMap,
          actualAggVal->inValMap,actualAggVal->outValMap);

      // clear maps after transfer (they should be empty when the are used with the
      // next scan parameter)     // !2.8.95->

      procMapIt.curr()->value->meanDotEEx.make_empty();
      procMapIt.curr()->value->meanDotEEn.make_empty();
      procMapIt.curr()->value->meanVecJ.make_empty();
      procMapIt.curr()->value->meanE_s.make_empty();
      procMapIt.curr()->value->meanObjectFuncCoef.make_empty();
      procMapIt.curr()->value->meanDotE_0.make_empty();
      procMapIt.curr()->value->meanCoef.make_empty();
      procMapIt.curr()->value->meanSlack.make_empty();
      procMapIt.curr()->value->meanShadowPrice.make_empty();

      procMapIt.curr()->value->initState(); // the initial state
    }

  // clear balance points  (they should be empty when the are used with the
  // next scan parameter)

  for (Mapiter<Symbol,balanPtr> balanMapIt = actualNet->balanMap.first();
       balanMapIt; balanMapIt.next())
    {
      balanMapIt.curr()->value->meanDotE = 0;
    }

  // !2.8.95<-

  // add fix and variable costs

  Map <Symbol, MeanValRec> copyOfScenOutValMap = actualScenVal->
    scenOutValMap;  // don't insert in a Map which is iterated
  for (Mapiter<Symbol, MeanValRec> scenOutValMapIt = copyOfScenOutValMap.
         first(); scenOutValMapIt; scenOutValMapIt.next())
    {
      String name = scenOutValMapIt.curr()->key.the_string();
      if (name.length() > 3)
        {
          String part1 = name(0,3);     // <String.h>, the first 3 char
          String part2 = name(3);       // <String.h>, the rest
          if (part1 == "Fix")
            actualScenVal->scenOutValMap[part2].expand
              (actualScenVal->scenOutValMap[part2].cutoff()
               + scenOutValMapIt.curr()->value.cutoff());
          if (part1 == "Var")
            actualScenVal->scenOutValMap[part2].expand
              (actualScenVal->scenOutValMap[part2].cutoff()
               + scenOutValMapIt.curr()->value.cutoff());
        }
    }
}

//// finishVal
// finish ioValue objects
//
void deecoApp::finishVal(void)
{
  if (actualAggVal)     delete actualAggVal;
  if (actualConnectVal) delete actualConnectVal;
  if (actualBalanVal)   delete actualBalanVal;
  if (actualProcVal)    delete actualProcVal;
  if (actualScenVal)    delete actualScenVal;
}

//// prepareNet
// Prepare the energy supply net object
//
void deecoApp::prepareNet(String scenId)
{
  actualNet = new Net(dApp);
  if (actualNet == 0)
    {
      dApp->message(11, "new of Net " + scenId);
      exit(11);
    }
  actualNet->prepareNet(actualProcDef, actualProcVal, actualBalanDef,
                        actualConnectDef, actualAggDef, actualAggVal);
  actualProcVal->inValMap.make_empty();                // save storage
}

//// finishNet
// finish the net object
//
void deecoApp::finishNet(void)
{
  actualNet->finishNet();
  if (actualNet) delete actualNet;
}

//////// run a single scenario //////////////////////////////

//// runIntervals
// run scenario with actual input values
//
void deecoApp::runIntervals(String scenId)
{
  if (actualScenVal->dynFlag != 0)
    runIntervalsDyn(scenId);       // dynamic optimization
  else
    runIntervalsQuasi(scenId);     // quasi-dynamic optimization
}

//// runIntervalsDyn
// run scenario with actual input values; dynamic optimization
//
void deecoApp::runIntervalsDyn(String scenId)
{
  cout << scenId
       << ": Dynamic Optimization not implemented!\n"
       << flush;  // Robbie: 23.09.05: Added flush.
}

//// runIntervalsQuasi
// run scenario with actual input values; quasi-dynamic optimization
//
void deecoApp::runIntervalsQuasi(String scenId)
{
  // actualScenVal->scenInTsFileName already definied in ScenVal::read!
  actualScenVal->scenOutTsFileName = actualScenVal->resPath/Path(scenId + ".sot");
  // actualScenVal->procInTsFileName already definied in ScenVal::read!
  Path procOutTsFileName = actualScenVal->resPath/Path(scenId + ".pot");

  ScenTs* actualScenTs = new ScenTs(dApp, "ScenTs " + scenId,
    actualScenVal->scenInTsFileName, actualScenVal->scenOutTsFileName,
    actualScenVal->tsOutFlag);
  if (actualScenTs == 0)
    {
      dApp->message(11, "new ScenTs in deecoApp.C");
      exit(11);
    }

  ioTsValue* actualProcTs = new ioTsValue(dApp, "ProcTs " + scenId,
    actualScenVal->procInTsFileName, procOutTsFileName,
    actualScenVal->tsOutFlag);
  if (actualProcTs == 0)
    {
      dApp->message(11, "new ProcTs in deecoApp.C");
      exit(11);
    }

  if (!actualScenTs->error() && !actualProcTs->error())
    {
      for (int i = 1; i <= actualScenVal->intNumber; i++)
        {

          if (dApp->testFlag)
            {
              dApp->message(1018, int_to_str(i));  // report interval number
            }

          // Robbie: 13.09.05: Capture the interval number 'i' here.
          //
          // Proc::count is a static member variable, accessed from within
          // modules using getInterval().  It can be set here because class
          // deecoApp is a friend of class Proc.
          //
          Proc::count = i;

          ScenTsPack* actualScenTsPack = new ScenTsPack(dApp,
            "ScenInterval " + scenId + " (ScenTsPack)", i);
          if (actualScenTsPack == 0)
            {
              dApp->message(11, "new ScenTsPack in deecoApp.C");
              exit(11);
            }
          actualScenTsPack->readPack(actualScenVal->scenInTsFileName,
            actualScenTs->finData, i);

          ioTsPack* actualProcTsPack = new ioTsPack(dApp,
            "ScenInterval " + scenId + " (ProcTsPack)", i);
          if (actualProcTsPack == 0)
            {
              dApp->message(11, "new ProcTsPack in deecoApp.C");
              exit(11);
            }
          actualProcTsPack->readPack(actualScenVal->procInTsFileName,
            actualProcTs->finData, i);

          optimize(actualScenTsPack,actualProcTsPack,i);

          if (!actualScenTsPack->error() && !actualProcTsPack->error())
            {
              if (actualScenVal->tsOutFlag)
                {
                  actualScenTsPack->writePack(actualScenVal->scenOutTsFileName,
                    actualScenTs->foutData, i);
                  actualProcTsPack->writePack(procOutTsFileName,
                    actualProcTs->foutData, i);
                }

              actualScenVal->update(actualScenTsPack->vecU);

            }
          else
            dApp->message(18, int_to_str(i));

          if (actualProcTsPack) delete actualProcTsPack;
          if (actualScenTsPack) delete actualScenTsPack;

        }
    }
  if (actualProcTs) delete actualProcTs;
  if (actualScenTs) delete actualScenTs;
}

//// optimize
// optimizing in one time interval (including several subintervals of variable
// length)
//
void
deecoApp::optimize(ScenTsPack* ScenTsPack,
                   ioTsPack*   oProcTsPack,
                   int i)
{
  actualNet->transferInTsPack(oProcTsPack);
  double actualIntLength = actualScenVal->intLength;
  double restIntLength = actualIntLength;
  while (restIntLength > EPSOPT)
    {
      actualIntLength = restIntLength;
      actualNet->prepareOpt(actualScenVal, ScenTsPack, actualAggDef,
                            actualAggVal, actualIntLength);
      if (!actualNet->error())
        actualNet->nSimplex->runSimplex(i);
      actualNet->finishOpt(actualScenVal, ScenTsPack, actualAggDef,
                           actualAggVal, actualIntLength);
      restIntLength =  restIntLength - actualIntLength;
    }
  actualNet->transferOutTsPack(oProcTsPack);
}

//  Robbie: 02.09.05: Notes about ISO C++ modifications.
//
//  Path calls:
//
//    29 explicit calls to Path(dir, basename, ext) added, perhaps required due to
//    some SCO UDK C++ Standard Components interface change, but I didn't find any
//    documentation to support this
//
//  Set (the variant for holding pointers) iterator:
//
//    For some reason, the iterator would not attach to the SCO_SC::Set_of_p in one
//    step as it had before. The fix was to split this process into two statements,
//    as shown:
//
//       Set_of_p <DEdge> temp01 = procMapIt.curr()->value->in_edges();
//       Set_of_piter <DEdge> inEdgeIt( temp01 );

//  $Source: /home/deeco/source/deeco006/RCS/deecoApp.C,v $
//  end of file
