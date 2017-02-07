
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
//  $Date: 2005/11/25 17:55:24 $
//  $Author: morrison $
//  $RCSfile: Proc.C,v $

//////////////////////////////////////////////////////////////////
//
// DEECO PROCESS MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////

// Robbie: 23.09.05: The following code can suppress 503 errors.
//
// 0 = enable  ERROR 503 reporting with 'deeco' -t option
// 1 = silence ERROR 503 reporting with 'deeco' -t option
//
// Typical example of reporting:
//
//   WARNING 503 : Standard version used of (virtual
//   function not overwritten?): actualState in Proc.C BHKW
//
#define SILENCE 1

#include  <strstream.h>       // string streams
#include     <stdlib.h>       // exit()

#include   "TestFlag.h"
#include       "Proc.h"

//////////////////////////////////////////////////////////////////
//
// CLASS: ProcDefRec
//
//////////////////////////////////////////////////////////////////

// Summary: ProcDefRec will be used as a type in templates like
// Map<class X,class Y>; according to USL C++ Standards Components,
// Rel. 3.0, p9-3 the constructor of the class Y is not allowed
// to require arguments

//// ProcDefRec
// Standard Constructor
//
ProcDefRec::ProcDefRec(void)
{
  mApp     = NULL;
  procType = "";
  select   = 0;
}

//// ProcDefRec
// Constructor
//
ProcDefRec::ProcDefRec(App* cmApp)
{
  mApp     = cmApp;
  procType = "";
  select   = 0;

#ifdef _TEST_                 // inner cycle test only if necessary
  if (cmApp->testFlag)
    cmApp->message(1001, "ProcDefRec");
#endif
}

//// ~ProcDefRec
// Destructor
//
ProcDefRec::~ProcDefRec(void)
{
#ifdef _TEST_                 // inner cycle test only if necessary
  if (mApp != 0)              // correctly initialized?
    {
      if (mApp->testFlag)
        mApp->message(1002, "ProcDefRec");
    }
#endif
}

//// operator<<
// write process definition record
//
ostream&
operator<<(ostream&           os,
           const ProcDefRec&  dr)
{
  os << dr.procType << " " <<  dr.select;
  return os;
}

//////////////////////////////////////////////////////////////////
//
// Class: ProcDef
//
//////////////////////////////////////////////////////////////////

//// ProcDef
// Standard
//
ProcDef::ProcDef(void)
{
  // no content, call ioDefValue(void)
}

//// ProcDef
//
ProcDef::ProcDef(App * cDefApp, String dTN)
  : ioDefValue(cDefApp, dTN)
{
  // no contents
}

//// ~ProcDef
//
ProcDef::~ProcDef(void)
{
  // no content, call ~ioDefValue(void)
}

//// writeInput
// write input data (control)
//
void
ProcDef::writeInput(void)
{
#ifdef _TEST_
  if (ioApp->testFlag)
    {
      ostrstream ssProcDefVal;                 // <strstream.h>
      ssProcDefVal << ioDataName
                   << " : "
                   << defMap
                   << "\n"
                   <<  ends;
      char* pProcDefVal = ssProcDefVal.str();  // <strstream.h>, "freeze" to char*
      ioApp->message(1009, pProcDefVal);
      delete pProcDefVal;     // Robbie: 18.11.2005: delete was delete[],
                              // deallocation is correct, see man strstream
                              // and method str().
    }
#endif
}

//// selected
// selection check
//
int
ProcDef::selected(Symbol sId)
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
void
ProcDef::readRecord(void)
{
  if (!ioApp->emptyLine(finData))
    {
      int i = 0;
      finData >> Id;
      i = ioApp->readComma(finData);         // read ","
      finData >> defMap[Id].procType;
      i = i + ioApp->readComma(finData);     // read ","
      finData >> defMap[Id].select;
      if (i != 0)
        finData.clear(ios::badbit | finData.rdstate());
                                             // raise errorflags of finData
                                             // <fstream.h> -> <iostream.h>
    }
  ioApp->ignoreRestLine(finData);            // "App.h"
}

//////////////////////////////////////////////////////////////////
//
// CLASS: Proc
//
//////////////////////////////////////////////////////////////////

// Summary: object for modeling processes
//
// Note: Proc will be used as a type in templates like Map<class X,class Y>;
// according to USL C++ Standards Components, Rel. 3.0, p.9-3
// the constructor of the class Y is not allowed to require arguments

//// Proc
// Standard Constructor
//
Proc::Proc(void)
{
  procApp  = NULL;
  aggCount = 0;
}

//// Proc
// Constructor
//
Proc::Proc(App*     cProcApp,
           Symbol   procId,
           Symbol   cProcType,
           ioValue* cProcVal)
  : DVertex(procId)
{
  aggCount     = 0;
  procApp      = cProcApp;
  procType     = cProcType;
  procInValMap = cProcVal->inValMap[procId];

  if (procApp->testFlag)
    {
      procApp->message(1001, "Proc");
    }
}

//// ~Proc
// Destructor
//
Proc::~Proc(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        {
          procApp->message(1002, "Proc");
        }
    }
}

//// updateMean
// update a part of the mean values after each optimization
//
void
Proc::updateMean(double actualIntLength)
{
  // update meanVecJ

  for (Mapiter<Symbol,MapSym4d> vecJIt1 = vecJ.first();
       vecJIt1; vecJIt1.next())
    {
      for (Mapiter<Symbol,MapSym3d> vecJIt2 = vecJIt1.curr()
           ->value.first();vecJIt2; vecJIt2.next())
        {
          for (Mapiter<Symbol,MapSym2d> vecJIt3 = vecJIt2.curr()
               ->value.first(); vecJIt3; vecJIt3.next())
            {
              for (Mapiter<Symbol,MapSym1d> vecJIt4 = vecJIt3.curr()
                   ->value.first(); vecJIt4; vecJIt4.next())
                {
                  for (Mapiter<Symbol,double> vecJIt5 = vecJIt4.curr()
                       ->value.first(); vecJIt5; vecJIt5.next())
                    {
                      meanVecJ[vecJIt1.curr()->key][vecJIt2.curr()->key]
                        [vecJIt3.curr()->key][vecJIt4.curr()->key]
                          [vecJIt5.curr()->key].updateMeanValRec
                            (vecJIt5.curr()->value, actualIntLength);
                    }
                }
            }
        }
    }
}

//// updateProcOutTsPack
// update procOutTsPack after each optimization
// because  optimization intervals can be shorter than
// the initial time intervals (variable time step size!)
// may be overwritten
//
void
Proc::updateProcOutTsPack(double actualIntLength, double intLength)
{
  // update state variable mean values

  for (Mapiter<int,double> E_sIt = E_s.first(); E_sIt; E_sIt.next())
    {
      String outputId;
      outputId = "E_S[" + int_to_str(E_sIt.curr()->key) + "]";
      procOutTsPack[outputId] = procOutTsPack[outputId] +
        (E_sIt.curr()->value * (actualIntLength/intLength));
    }
}

//// actualExJ
// actualize attributes of exit side
//
void
Proc::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procApp->testFlag)
    {
#if (SILENCE == 0)       // defined in this file
      procApp->message(503, "actualExJ in Proc.C "
        + vertexId.the_string());
#endif
    }
}

//// actualEnJ
// actualize attributes of entrance side
//
void
Proc::actualEnJ(const Map<Symbol,double>& pVecU)
{
  if (procApp->testFlag)
    {
#if (SILENCE == 0)       // defined in this file
      procApp->message(503, "actualEnJ in Proc.C "
        + vertexId.the_string());
#endif
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void
Proc::actualSimplexInput(const Map<Symbol,double>& pVecU,
                         double                    actualIntLength)
{
  if (procApp->testFlag)
    {
#if (SILENCE == 0)       // defined in this file
      procApp->message(503, "actualSimplexInput in Proc.C "
        + vertexId.the_string());
#endif
    }
}

//// initState    // !2.8.95->
// set initial state variables
//
void
Proc::initState(void)
{
  if (procApp->testFlag)
    {
#ifndef SILENCE               // defined in this file
      procApp->message(503, "initState in Proc.C "
        + vertexId.the_string());
#endif
    }
} // !2.8.95<-

//// actualState
// actualize state variables
//
int
Proc::actualState(double&                    actualIntLength,
                  const Map<Symbol,double>&  pVecU)
{
  (void)actualIntLength; // Robbie: Say we're deliberately not using it.

  if (procApp->testFlag)
    {
#if (SILENCE == 0)       // defined in this file
      procApp->message(503, "actualState in Proc.C "
        + vertexId.the_string());
#endif
    }
  return 1;
}

//// showPower
// show (=return value) the type ("Ex" or "En")
// of the energy flow which is used to calculate
// the fixed costs (necessary for process aggregate constraints);
// the energyFlowType and energyFlowNumber can be shown with the help
// of showPowerType and showPowerNumber;
// should be overwritten
//
Symbol
Proc::showPower(Symbol& showPowerType,
                Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}

//// addFixCosts
// add process-dependent fix costs to the fix costs vector
//
void
Proc::addFixCosts(Map<Symbol, MeanValRec>& scenOutValMap,
                  Map<Symbol, MapSym1d>& aggInValMap,
                  Map<Symbol, MapSym1M>& aggOutValMap)
{
  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt;  procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);           // <String.h>, the first 4 char
          String part2 = name(4);             // <String.h>, the rest
          double power = 0;
          if (part1 == "sFix")
            {
              String symFix = "Fix" + part2;  // !22.11.95
              if (aggCount == 0)  // process doesn't belong to a process aggregate
                {
                  Symbol powerType;
                  Symbol powerNumber;
                  Symbol powerKind = showPower(powerType,powerNumber);
                  if (powerKind == "Ex")
                    power = meanDotEEx[powerType][powerNumber].maximum();
                  if (powerKind == "En")
                    power = meanDotEEn[powerType][powerNumber].maximum();
                  scenOutValMap[symFix].expand
                    (scenOutValMap[symFix].cutoff() +
                      (power * procInValMapIt.curr()->value));
                }
              else                // process belongs to a process aggregate
                {
                  Symbol agg;
                  for (Mapiter <Symbol, MapSym1d> aggInValMapIt =
                       aggInValMap.first(); aggInValMapIt; aggInValMapIt.next())
                    {
                      Mapiter <Symbol,double> aggInValMapIt2
                        (aggInValMapIt.curr()->value);
                      if (aggInValMapIt2 = aggInValMapIt.curr()->
                          value.element(vertexId))
                        agg = aggInValMapIt.curr()->key;
                    }
                  power = aggOutValMap[agg]["Power"].maximum();
                  scenOutValMap[symFix].expand
                    (scenOutValMap[symFix].cutoff() +
                      power/double(aggCount) * procInValMapIt.curr()->value);
                }
            }
        }
    }
}

int
Proc::count = 0;

//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/Proc.C,v $
// end of file
