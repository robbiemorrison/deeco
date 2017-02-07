
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
//  $Date: 2005/09/30 18:54:15 $
//  $Author: deeco $
//  $RCSfile: Convers.h,v $

//////////////////////////////////////////////////////////////////
//
// DEECO CONVERSION-PROCESS MODULES
//
//////////////////////////////////////////////////////////////////

#ifndef _CONV_                // header guard
#define _CONV_

#include "App.h"
#include "Data.h"
#include "DGraph.h"
#include "Proc.h"
#include <String.h>           // easy string management
#include <Symbol.h>           // use Symbol instead of String for keys
#include <Map.h>              // associative array

///////////////////////////////////////////////////////////////////
//
// CLASS: CBoiConst
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: boiler with constant efficiency

class CBoiConst : public Proc
{
public:

  // Standard Constructor
  CBoiConst(void);
  // Constructor
  CBoiConst(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CBoiConst(void);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double>& pVecU);
  // actualize objective function coefficients, constraint coefficients, and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength);
  // show (meaning return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol& showPowerType, Symbol& showPowerNumber);

protected:

  double T_FEx,T_REx;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CBoiConv
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: conventional oil or gas boiler (no condensing)

class CBoiConv : public Proc
{
public:

  // Standard Constructor
  CBoiConv(void);
  // Constructor
  CBoiConv(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CBoiConv(void);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double>& pVecU);
  // actualize objective function coefficients, constraint coefficients, and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength);
  // show (meaning return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol& showPowerType, Symbol& showPowerNumber);

protected:

  double x;
  double y;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CBoiCond
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: oil or gas condensing boiler

class CBoiCond : public Proc
{
public:

  // Standard Constructor
  CBoiCond(void);
  // Constructor
  CBoiCond(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~CBoiCond(void);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double>& pVecU);
  // actualize objective function coefficients, constraint coefficients, and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength);
  // show (meaning return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol& showPowerType, Symbol& showPowerNumber);

protected:

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CEHP
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: ambient air, soil or water using electrical heat pump

class CEHP : public Proc
{
public:

  // Standard Constructor
  CEHP(void);
  // Constructor
  CEHP(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CEHP(void);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double>& pVecU);
  // actualize objective function coefficients, constraint coefficients, and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength);
  // show (meaning return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol& showPowerType, Symbol& showPowerNumber);

protected:

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CGHP
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: ambient air, soil or water using gas driven heat pump

class CGHP : public Proc
{
public:

  // Standard Constructor
  CGHP(void);
  // Constructor
  CGHP(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CGHP(void);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double>& pVecU);
  // actualize objective function coefficients, constraint coefficients, and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength);
  // show (meaning return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol& showPowerType, Symbol& showPowerNumber);

protected:

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CBPT
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: back-pressure turbine, combined heat and power station

class CBPT : public Proc
{
public:

  // Standard Constructor
  CBPT(void);
  // Constructor
  CBPT(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CBPT(void);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double>& pVecU);
  // actualize objective function coefficients, constraint coefficients, and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength);
  // show (meaning return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol& showPowerType, Symbol& showPowerNumber);

protected:

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CECT
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: extraction-condensing turbine, combined heat and power station

class CECT : public Proc
{
public:

  // Standard Constructor
  CECT(void);
  // Constructor
  CECT(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CECT(void);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double>& pVecU);
  // actualize objective function coefficients, constraint coefficients, and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength);
  // show (meaning return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol& showPowerType, Symbol& showPowerNumber);

protected:

    double TF_Min;
    double TF_Max;
    double TR_Min;
    double TR_Max;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CCogConst
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: cogeneration unit with constant efficiencies

class CCogConst : public Proc
{
public:

  // Standard Constructor
  CCogConst(void);
  // Constructor
  CCogConst(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CCogConst(void);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double>& pVecU);
  // actualize objective function coefficients, constraint coefficients, and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength);
  // show (meaning return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol& showPowerType, Symbol& showPowerNumber);

protected:

    double T_FEx;
    double T_REx;
    double TF_Min;
    double TF_Max;
    double TR_Min;
    double TR_Max;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CBoiOfl
//
///////////////////////////////////////////////////////////////////

// Module author: Kathrin Ramsel
// Development thread: S05
//
// Summary: boiler with constant efficiency

class CBoiOfl : public Proc
{
public:

  // Standard Constructor
  CBoiOfl(void);
  // Constructor
  CBoiOfl(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CBoiOfl(void);
  // update procOutTsPack after each optimization
  virtual void updateProcOutTsPack(double actualIntLength, double intLength);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double> & pVecU);
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

protected:

  double T_FEx,T_REx;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CBPTo
//
///////////////////////////////////////////////////////////////////

// Module author: Kathrin Ramsel
// Development thread: S05
//
// Summary: back-pressure turbine, combined heat and power station

class CBPTo : public Proc
{
public:

  // Standard Constructor
  CBPTo(void);
  // Constructor
  CBPTo(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CBPTo(void);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double> & pVecU);
  // actualize attributes of entrance side
  virtual void actualEnJ(const Map<Symbol,double> & pVecU);
  // actualize objective function coefficients, constraint coef. and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double> & pVecU,
                                  double actualIntLength);
  // show (=return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol & showPowerType, Symbol & showPowerNumber);

protected:

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CECTo
//
///////////////////////////////////////////////////////////////////

// Module author: Kathrin Ramsel
// Development thread: S05
//
// Summary: extraction-condensing turbine, combined heat and power station
// without boiler (energy flow on entrance side due to steam input)

class CECTo : public Proc
{
public:

  // Standard Constructor
  CECTo(void);
  // Constructor
  CECTo(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CECTo(void);
  // update procOutTsPack after each optimization
  virtual void updateProcOutTsPack(double actualIntLength, double intLength);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double> & pVecU);
  // actualize attributes of entrance side
  virtual void actualEnJ(const Map<Symbol,double> & pVecU);
  // actualize objective function coefficients, constraint coef. and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double> & pVecU,
                                  double actualIntLength);
  // show (=return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol & showPowerType, Symbol & showPowerNumber);

protected:

  double TF_Min;
  double TF_Max;
  double TR_Min;
  double TR_Max;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CGasTHR
//
///////////////////////////////////////////////////////////////////

// Module author: Kathrin Ramsel
// Development thread: S05
//
// Summary: gas turbine, with HRSG

class CGasTHR : public Proc
{
public:

  // Standard Constructor
  CGasTHR(void);
  // Constructor
  CGasTHR(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CGasTHR(void);
  // update procOutTsPack after each optimization
  virtual void updateProcOutTsPack(double actualIntLength, double intLength);
  // actualize attributes of exit side
  //  virtual void actualExJ(const Map<Symbol,double> & pVecU);
  // actualize attributes of entrance side
  //  virtual void actualEnJ(const Map<Symbol,double> & pVecU);
  // actualize objective function coefficients, constraint coef. and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double> & pVecU,
                                  double actualIntLength);
  // show (=return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol & showPowerType, Symbol & showPowerNumber);

protected:

  double TF_Min;
  double TF_Max;
  double TR_Min;
  double TR_Max;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: CFCellConst
//
///////////////////////////////////////////////////////////////////

// Module author: Jan Heise
// Development thread: S07
//
// Summary: fuel cell unit with constant efficiencies

class CFCellConst : public Proc
{
public:

  // Standard Constructor
  CFCellConst(void);
  // Constructor
  CFCellConst(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
  // Destructor
  virtual ~CFCellConst(void);
  // actualize attributes of exit side
  virtual void actualExJ(const Map<Symbol,double> & pVecU);
  // actualize objective function coefficients, constraint coef. and
  // rhs before optimization
  virtual void actualSimplexInput(const Map<Symbol,double> & pVecU,
                  double actualIntLength);
  // show (=return value) the type ("Ex" or "En")
  // of the energy flow which is used to calculate
  // the fixed costs (necessary for process aggregate constraints);
  // the energyFlowType and energyFlowNumber can be shown with the help
  // of showPowerType and showPowerNumber
  virtual Symbol showPower(Symbol & showPowerType, Symbol & showPowerNumber);

  // function describes the heat exchanging unit,
  // gives back function value and the first derivation
  // needed for call 'rtsafe'(Newton-Raphson algorithm) to calculate TR_sec
  void hxfunc(double , double *, double *);
  // Newton-Raphson-algorithem for hxfunc

  // Robbie: 21.07.05: ISO C++ language change.
  //
  // Removed the scope resolution 'CFCellConst::' from the following function. This
  // is a documented language change.
  //
  double rtsafe(void (CFCellConst::*hxfunc)(double, double *,
                             double *), double x1, double x2, double xacc);

protected:

  double T_FEx;
  double T_REx;
  double TF_Min;
  double TF_Max;
  double TR_Min;
  double TR_Max;
  double epsilon_hc;

};

#endif  // _CONV_

//  $Source: /home/deeco/source/deeco006/RCS/Convers.h,v $
//  end of file
