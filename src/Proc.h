
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
//  $Date: 2005/11/25 16:45:08 $
//  $Author: morrison $
//  $RCSfile: Proc.h,v $

//////////////////////////////////////////////////////////////////
//
// DEECO PROCESS MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////

#ifndef _PROC_H_              // header guard
#define _PROC_H_

#include     <String.h>
#include     <Symbol.h>       // use Symbol not String for keys
#include        <Map.h>       // associative array
#include       <Path.h>       // necessary for right instantiation of <Symbol.h> and <Map.h>
#include    <fstream.h>       // file I/O

// Robbie: 24.12.03: Commented out because not required under UDK.
//
// #include <List.h>          // list management

#include        "App.h"
#include       "Data.h"
#include     "DGraph.h"

//////////////////////////////////////////////////////////////////
//
// CLASS: ProcDefRec
//
//////////////////////////////////////////////////////////////////

// Summary: for reading process definition records for processes

class ProcDefRec
{
public:

 // Standard Constructor
    ProcDefRec(void);

 // Constructor
    ProcDefRec(App* cmApp);

 // Destructor
    virtual ~ProcDefRec(void);

 // write process definition  record
    friend ostream& operator<<(ostream& os, const  ProcDefRec& dr);

 // selection check
    int selected(Symbol sId);

    Symbol  procType;
    int     select;

protected:

    App* mApp;

};

//////////////////////////////////////////////////////////////////
//
// CLASS: ProcDef
//
//////////////////////////////////////////////////////////////////

class ProcDef : public ioDefValue
{
public:

 // Standard Constructor
    ProcDef(void);

 // Constructor
    ProcDef(App* cDefApp, String dTN);

 // Destructor
    virtual ~ProcDef(void);

 // write input data (control)
    void writeInput(void);

 // selection check
    int selected(Symbol sId);

    Map<Symbol,ProcDefRec> defMap;           // overwrite defMap of ioDefValue

protected:

    virtual void readRecord(void);

};

//////////////////////////////////////////////////////////////////
//
// CLASS: Proc
//
//////////////////////////////////////////////////////////////////

// Summary: object for modeling processes

class Proc : public DVertex
{

  friend class deecoApp;           // in order to update intervalCount

public:

 // Standard Constructor
    Proc(void);

 // Constructor
    Proc(App*     cProcApp,        // housekeeping object, constructed before main()
         Symbol   procId,          // process identifier given by the user, eg: BHKW
         Symbol   cProcType,       // process type given by the user, eg: CCogConst
         ioValue* cProcVal);       // process input values given by the user

 // Destructor
    virtual ~Proc(void);

 // update mean values after each optimization
    void updateMean(double actualIntLength);

 // update procOutTsPack after each optimization
    virtual void updateProcOutTsPack(double actualIntLength,
                                     double intLength);
 // actualize attributes of exit side
    virtual void actualExJ(const Map<Symbol,double>& pVecU);

 // actualize attributes of entrance side
    virtual void actualEnJ(const Map<Symbol,double>& pVecU);

 // actualize objective function coefficients, constraint coefficients,
 // and rhs before optimization
    virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                    double                    actualIntLength);
 // set initial state variables  // !2.8.95
    virtual void initState(void);

 // actualize state variables (return of 1 means change is not too large)
    virtual int actualState(double&                   actualIntLength,
                            const Map<Symbol,double>& pVecU);

 // show (meaning return value) the type ("Ex" or "En") of
 // the energy flow which is used to calculate the fixed
 // costs (necessary for process aggregate constraints); the
 // energyFlowType and energyFlowNumber can be shown with
 // the help of the variables showPowerType and
 // showPowerNumber
    virtual Symbol showPower(Symbol& showPowerType, Symbol& showPowerNumber);

 // add process-dependent fixed costs to the fixed costs vector
    virtual void addFixCosts(Map<Symbol,MeanValRec>& scenOutValMap,
                             Map<Symbol,MapSym1d>&   aggInValMap,
                             Map<Symbol,MapSym1M>&   aggOutValMap);

  // procId is a member of DVertex as vertexId

  // General notes:
  //
  // See "Data.h" for the typedefs for:
  //
  //   MapSymxd, where x = 1 ... 5 and d is double
  //   MapSymxM, where x = 1 ... 5 and M is class MeanValRec
  //
  // These are nested associative arrays, also called
  // multi-maps, where "x" indicates the degree of nesting.
  //
  // The term "Pack" indicates a slice across all
  // time-series for that interval.
  //
  // Process type is the synonymous with technology module.
  // In other words, the name of the class.  Note that some
  // modules were renamed and their old names are now
  // "redirected" to the correct constructor.  Check the
  // code and not old datasets for accurate information.

  Symbol       procType;                   // caution: same object used in class
                                           // ProcDefRec, which also has public access

  Set<Symbol2> Ex;                         // set of leaving energy flow indices
  Set<Symbol2> En;                         // set of entering energy flow indices

#ifdef SOLVER_GLPK

  // Robbie: 05.09.05: Extra variables added to support MILP optimization.
  //

  Set<Symbol2> InVar;                      // set of integer-valued variables
  Set<Symbol2> BinVar;                     // set of 0-1 valued variables

#endif

  Map<int,Symbol2>  exFlowAssign;          // assign the number of energy flow for use
  Map<int,Symbol2>  enFlowAssign;          //  in simplex algorithm to the energy flow

  // parameters

  Map<Symbol,double>  procInValMap;        // initialized on construction using
                                           //   ioValue::inValMap[procId], where the
                                           //   objects "ioValue*" and "procId" are
                                           //   given as constructor arguments

  Map<Symbol,double> vecCConstant;         // constant part of vecC

  // time varying quantities

  Map<Symbol,double>  procInTsPack;        // time series input
  Map<Symbol,double>  procOutTsPack;       // time series output
  Map<int,double>     E_s;                 // state variables, indexed from 1 usually
  MapSym5d            vecJ;                // attributes
  MapSym2d            dotEEx;              // exiting energy flows
  MapSym2d            dotEEn;              // entering energy flows

  // input for simplex algorithm

  MapSym4d           objectFuncCoef;
  Map<int,MapSym3d>  greaterConstraintCoef;
  Map<int,double>    greaterConstraintRhs;
  Map<int,MapSym3d>  lessConstraintCoef;
  Map<int,double>    lessConstraintRhs;
  Map<int,MapSym3d>  equalConstraintCoef;
  Map<int,double>    equalConstraintRhs;

  Map<int,int>       constraintAssign;     // process constraint number
                                           //   as a function of the simplex
                                           //   constraint number
  // mean values

  MapSym2M             meanDotEEx;         // exiting energy flow mean values
  MapSym2M             meanDotEEn;         // entering energy flow mean values
  MapSym5M             meanVecJ;
  Map<int,MeanValRec>  meanE_s;
  MapSym4M             meanObjectFuncCoef;
  Map<int,MeanValRec>  meanDotE_0;         // rhs of constraints
  Map<int,MapSym3M>    meanCoef;           // coefficient of constraints
  Map<int,MeanValRec>  meanSlack;          // constraint slack variable
  Map<int,MeanValRec>  meanShadowPrice;    // constraint shadow price

  // aggregate constraints

  int aggCount;                            // the number of processes belonging to
                                           //   the same aggregate as this process
protected:

  // Robbie: 13.09.05: Added to give processes access to the current interval.
  //
  // get interval count, inlined for the moment
  //
  int getInterval(void)
    {
      return count;
    }

  App* procApp;

private:

  // Robbie: 13.09.05: Added to give processes access to the current interval.
  //
  static int count;                        // true interval count

};

#endif  // _PROC_H_

//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/Proc.h,v $
//  end of file
