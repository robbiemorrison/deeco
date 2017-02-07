
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
//  $Date: 2005/11/25 13:53:20 $
//  $Author: morrison $
//  $RCSfile: Net.h,v $

//////////////////////////////////////////////////////////////////
//
// DEECO ENERGY SUPPLY NETWORK MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////

#ifndef _NET_H_               // header guard
#define _NET_H_

#include       <List.h>       // list management

#include        "App.h"
#include     "DGraph.h"
#include       "Scen.h"
#include       "Proc.h"
#include      "Balan.h"
#include    "Connect.h"
#include   "ProcType.h"
#include    "Simplex.h"

typedef DVertex* vertexPtr;   // "DGraph.h"
typedef Proc*    procPtr;     // "Proc.h"
typedef Balan*   balanPtr;    // "Balan.h"
typedef DEdge*   edgePtr;     // "DGraph.h"

///////////////////////////////////////////////////////////////////
//
// CLASS: Net
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
//
// Summary: 'deeco' energy supply network management object

class Net
{
public:

  // Standard Constructor
  Net(void);

  // Constructor
  Net(App* cnApp);

  // Destructor
  virtual ~Net(void);

  // prepare the energy supply network object
  void prepareNet(ProcDef*     nProcDef,
                  ioValue*     nProcVal,
                  BalanDef*    nBalanDef,
                  ConnectDef*  nConnectDef,
                  ioDefValue*  nAggDef,
                  ioValue*     nAggVal);

  // finish the energy supply network object
  void finishNet(void);

  // transfer inTsPacks
  void transferInTsPack(ioTsPack* nProcTsPack);

  // transfer outTsPacks
  void transferOutTsPack(ioTsPack* nProcTsPack);

  // calculate the attributes and establish the matrix **a (input for simplex)
  void prepareOpt(ScenVal*     nScenVal,
                  ScenTsPack*  nScenTsPack,
                  ioDefValue*  nAggDef,
                  ioValue*     nAggVal,
                  double       actualIntLength);

  // get the results and calculate the state variable transformation
  double finishOpt(ScenVal*     nScenVal,
                   ScenTsPack*  nScenTsPack,
                   ioDefValue*  nAggDef,
                   ioValue*     nAggVal,
                   double&      actualIntLength);

  // error Management
  int error(void);

  Map<Symbol,procPtr>  procMap;
  Map<Symbol,balanPtr> balanMap;
  Map<Symbol,edgePtr>  edgeMap;

  DGraph systGraph;              // Graph of the energy supply, "DGraph.h" system
  List_of_p<DVertex> sortList;   // ordered list of vertices,"DGraph.h", <List.h>
  ProcType* actualProcType;      // "ProcType.h"
  Simplex* nSimplex;

protected:

  int energyFlowCount;                // (=N)
  int constraintCount;                // (=M)
  int lessConstraintCount;            // (=M1)
  int greaterConstraintCount;         // (=M2)
  int equalConstraintCount;           // (=M3)
  int nonProcConstraintCount;         // without the process internal
                                      //   constraints; time independent!
  int nonProcLessConstraintCount;
  int nonProcGreaterConstraintCount;
  int nonProcEqualConstraintCount;

  Map<int, Symbol> aggConstAssign;    // connect the aggregate id with
                                      //   the number of the corresponding
                                      //   lessContraint
  App* nApp;
  int errorFlag;

////////// help functions //////////////////////////////

  // establish the energy supply network
  void establishNet(ProcDef*     nProcDef,
                    ioValue*     nProcVal,
                    BalanDef*    nBalanDef,
                    ConnectDef*  nConnectDef,
                    ioDefValue*  nAggDef,
                    ioValue*     nAggVal);

  // insert processes in the net as vertices
  void  insertProcesses(ProcDef*     nProcDef,
                        ioValue*     nProcVal,
                        ioDefValue*  nAggDef,
                        ioValue*     nAggVal);

  // insert balances in the net as vertices
  void insertBalances(BalanDef* nBalanDef);

  // insert energy flows in the net as edges and make first tests of compatibility
  void insertConnections(ConnectDef* nConnectDef);

  // test the graph for cycles
  void testNet(void);

  // order the graph
  void orderNet(void);

  // calculate attributes
  void calcAttributes(const Map<Symbol,double>&  nVecU,
                      double                     actualIntLength);

  // prepare simplex tableau
  void prepareSimplTab(void);

  // calculate energy balance constraints simplex coefficients
  void calcBalanceConstCoef(int& actualM3);

  // calculate process aggregation constraint simplex coefficients
  void calcAggConstCoef(int&         actualM1,
                        ioDefValue*  nAggDef,
                        ioValue*     nAggVal);

  // calculate process constraint simplex coefficients
  void calcProcConstCoef(ScenVal*  nScenVal,
                         int&      actualM1,
                         int&      actualM2,
                         int&      actualM3);

  // establish the parts of the simplex tableau matrix "aMatrix"
  // which are process dependent
  void establishAMatrixProc(procPtr   nProc,
                            ScenVal*  nScenVal,
                            int&      actualM1,
                            int&      actualM2,
                            int&      actualM3);

  // set the constraint coefficients for the different restriction types
  void setConstCoef(procPtr              nProc,
                    Map<int, MapSym3d>&  constraintCoef,
                    Map<int, double>&    constraintRhs,
                    int&                 actualM,
                    int                  mOffset);

  // getting results for each process (part 1)
  void getProcessResults1(double&                    actualIntLength,
                          const Map<Symbol,double>&  nVecU);

  // getting results for each process (part 2)
  void getProcessResults2(ScenVal*     nScenVal,
                          ScenTsPack*  nScenTsPack,
                          double       actualIntLength);

  // getting results for process aggregates
  void getAggResults(ioDefValue*  nAggDef,
                     ioValue*     nAggVal,
                     double       actualIntLength);

  // clear constraints of processes and aggregates
  void clearConstraints(void);

  // update constraint coefficient mean values
  void updateCoef(double              dotE,
                  double              intLength,
                  Symbol              ExEn,
                  Symbol              energyFlowType,
                  Symbol              energyFlowNumber,
                  Map<int,MapSym3d>&  coefMap,
                  Map<int,MapSym3M>&  meanCoefMap);

};

#endif  // _NET_H_

//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/Net.h,v $
//  end of file
