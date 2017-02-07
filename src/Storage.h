
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
// DEECO STORAGE-PROCESS MODULES
//
//////////////////////////////////////////////////////////////////

#ifndef _STOR_                // header guard
#define _STOR_

#include "App.h"
#include "Data.h"
#include "DGraph.h"
#include "Proc.h"
#include <String.h>           // easy string management
#include <Symbol.h>           // use Symbol instead of String for keys
#include <Map.h>              // associative array

///////////////////////////////////////////////////////////////////
//
// CLASS: SSenH
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: storage of sensible heat, no heat exchangers,
//          temperature-independent heat capacity, time-independent
//          storage-environment temperature, cylindrical form with
//          diameter = height

class SSenH : public Proc
{
public:

 // Standard Constructor
    SSenH(void);
 // Constructor
    SSenH(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~SSenH(void);
 // update procOutTsPack after each optimization
    virtual void updateProcOutTsPack(double  actualIntLength,
                                     double intLength);
 // actualize attributes of exit side
    virtual void actualExJ(const Map<Symbol,double>& pVecU);
 // actualize attributes of entrance side
    virtual void actualEnJ(const Map<Symbol,double>& pVecU);
 // actualize objective function coefficients, constraint coefficients, and
 // rhs before optimization
    virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                    double actualIntLength);
 // set initial state variables
    virtual void initState(void);
 // actualize state variables (1 = change is not too large)
    virtual int actualState(double& actualIntLength,
                            const Map<Symbol,double>& pVecU);
 // show (meaning return value) the type ("Ex" or "En")
 // of the energy flow which is used to calculate
 // the fixed costs (necessary for process aggregate constraints);
 // the energyFlowType and energyFlowNumber can be shown with the help
 // of showPowerType and showPowerNumber
    virtual Symbol showPower(Symbol& showPowerType, Symbol& showPowerNumber);
 // add process-dependent fix costs to the fix costs vector
    virtual void addFixCosts(Map<Symbol, MeanValRec>& scenOutValMap,
                             Map<Symbol, MapSym1d>& aggInValMap,
                             Map<Symbol, MapSym1M>& aggOutValMap);

protected:

    double GeoF;  // geometric factor for converting
                  // volume in surface area (diameter = height)
    double A;
    double Mc;
    double x;     // help variable

};

///////////////////////////////////////////////////////////////////
//
// CLASS: SSupEl
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summmary: superconducting magnetic energy storage

class SSupEl : public Proc
{
public:

 // Standard Constructor
    SSupEl(void);
 // Constructor
    SSupEl(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~SSupEl(void);
 // actualize objective function coefficients, constraint coefficients, and
 // rhs before optimization
    virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                    double actualIntLength);
 // actualize state variables (1 = change is not too large)
    virtual int actualState(double& actualIntLength,
                            const Map<Symbol,double>& pVecU);
 // show (meaning return value) the type ("Ex" or "En")
 // of the energy flow which is used to calculate
 // the fixed costs (necessary for process aggregate constraints);
 // the energyFlowType and energyFlowNumber can be shown with the help
 // of showPowerType and showPowerNumber
    virtual Symbol showPower(Symbol& showPowerType, Symbol& showPowerNumber);
 // add process-dependent fix costs to the fix costs vector
    virtual void addFixCosts(Map<Symbol, MeanValRec>& scenOutValMap,
                             Map<Symbol, MapSym1d>& aggInValMap,
                             Map<Symbol, MapSym1M>& aggOutValMap);

protected:

    double ESUnit;                      // energy unit

    // help variables

    double dotEEnEl_Add;
    double k_1;
    double k_2;
    double dotE_L;
    double dotE_Max;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: SSenHn
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summmary: stratified storage of sensible heat with N layers
//           (modelled according to "variable-inlet" approach)
//           no heat exchangers, temperature-independent heat capacity,
//           time-DEpendent storage-environment temperature,
//           cylindrical form with diameter = height

class SSenHn : public Proc
{
public:
// Standard Constructor
  SSenHn(void);
// Constructor
  SSenHn(App* cProcApp, Symbol procId,
              Symbol cProcType, ioValue* cProcVal);
// Destructor
  virtual ~SSenHn(void);
// update procOutTsPack after each optimization
  virtual void updateProcOutTsPack(double  actualIntLength,
                                   double intLength);
// actualize attributes of exit side
   virtual void actualExJ(const Map<Symbol,double> & pVecU);
// actualize attributes of entrance side
   virtual void actualEnJ(const Map<Symbol,double> & pVecU);
// actualize objective function coefficients, constraint coef. and
// rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double> & pVecU,
                                  double actualIntLength);
// set initial state variables
  virtual void initState(void);
// actualize state variables (1 = change is not too large)
  virtual int actualState(double & actualIntLength,
                          const Map<Symbol,double> & pVecU);
// show (=return value) the type ("Ex" or "En")
// of the energy flow which is used to calculate
// the fixed costs (necessary for process aggregate constraints);
// the energyFlowType and energyFlowNumber can be shown with the help
// of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol & showPowerType, Symbol & showPowerNumber);
// add process-dependent fix costs to the fix costs vector
  virtual void addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
                           Map<Symbol, MapSym1d> & aggInValMap,
                           Map<Symbol, MapSym1M> & aggOutValMap);

 protected:

double GeoF; // geometric factor for converting
             // volume in surface area (diameter = height)
             // GeoF depends on number of layers

int  stratN;
double A1, A2, AN_Deckel, AN_Mitte;    // Oberfl\"achen f"ur
                                       // stratN = 1, 2, >=3

Map<int,double> A;    // Maps automatically initialized
Map<int,double> loss; // with value '0'

double Mc;
Map<int,double> T_s;

MapSym1d T_FEn;
MapSym1d T_REx;
MapSym1d dot_mc_En;
MapSym1d dot_mc_Ex;
MapSym1d dot_mc_mix;
MapSym2d f_F, f_R;         // control functions
MapSym2d f_F_sum, f_R_sum;
MapSym2d T_quot_F, T_quot_R;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: SSenHnHEx
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summmary: stratified storage with heat exchanger for unload
//           N layers (modelled according to "variable-inlet" approach)
//           temperature-independent heat capacity,
//           time-independent storage-environment temperature,
//           cylindrical form with diameter = height

class SSenHnHEx : public Proc
{
public:
// Standard Constructor
  SSenHnHEx(void);
// Constructor
  SSenHnHEx(App* cProcApp, Symbol procId,
              Symbol cProcType, ioValue* cProcVal);
// Destructor
  virtual ~SSenHnHEx(void);
// update procOutTsPack after each optimization
  virtual void updateProcOutTsPack(double  actualIntLength,
                                   double intLength);
// actualize attributes of exit side
   virtual void actualExJ(const Map<Symbol,double> & pVecU);
// actualize attributes of entrance side
   virtual void actualEnJ(const Map<Symbol,double> & pVecU);
// actualize objective function coefficients, constraint coef. and
// rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double> & pVecU,
                                  double actualIntLength);
// set initial state variables
  virtual void initState(void);
// actualize state variables (1 = change is not too large)
  virtual int actualState(double & actualIntLength,
                          const Map<Symbol,double> & pVecU);
// show (=return value) the type ("Ex" or "En")
// of the energy flow which is used to calculate
// the fixed costs (necessary for process aggregate constraints);
// the energyFlowType and energyFlowNumber can be shown with the help
// of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol & showPowerType, Symbol & showPowerNumber);
// add process-dependent fix costs to the fix costs vector
  virtual void addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
                           Map<Symbol, MapSym1d> & aggInValMap,
                           Map<Symbol, MapSym1M> & aggOutValMap);

 protected:

double GeoF; // geometric factor for converting
             // volume in surface area (diameter = height)
             // GeoF depends on number of layers

int  stratN;
double A1, A2, AN_Deckel, AN_Mitte;    // Oberfl\"achen f"ur
                                       // stratN = 1, 2, >=3

Map<int,double> A;    // Maps automatically initialized
Map<int,double> loss; // with value '0'

double Mc;
Map<int,double> T_s;

MapSym1d T_FEn;
MapSym1d T_REx;
MapSym1d T_RExIn;
MapSym1d dot_mc_En;
MapSym1d dot_mc_Ex;
MapSym1d dot_mc_mix;
MapSym2d f_F, f_R;         // control functions
MapSym2d f_F_sum, f_R_sum;
MapSym2d T_quot_F, T_quot_R;

};

#endif  // _STOR_
