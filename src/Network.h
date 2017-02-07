
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
// DEECO NETWORK-PROCESS MODULES
//
//////////////////////////////////////////////////////////////////

#ifndef _NETW_                // header guard
#define _NETW_

#include "App.h"
#include "Data.h"
#include "DGraph.h"
#include "Proc.h"
#include <String.h>           // easy string management
#include <Symbol.h>           // use Symbol instead of String for keys
#include <Map.h>              // associative array

///////////////////////////////////////////////////////////////////
//
// CLASS: NBW
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: enthalpy adaptation by mixing valves and
//          (waste-) heat dumping

class NBW : public Proc
{
public:

 // Standard Constructor
    NBW(void);
 // Constructor
    NBW(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~NBW(void);
 // actualize attributes of exit side
    virtual void actualExJ(const Map<Symbol,double>& pVecU);
 // actualize attributes of entrance side
    virtual void actualEnJ(const Map<Symbol,double>& pVecU);
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

 // calculate additional heat (zero in this case)
    virtual void calcAddHeat(double T_1,double T_2);
 // constraints for no heating cases (used by descendants)
    virtual void noLeavingHeat(void);

    double T_FEx, T_REx, T_FEn, T_REn;    // help variables

};

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWAConst
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: enthalpy adaptation by mixing valves,
//          (waste-) heat dumping and additional heating
//          (similar to CBoiConst)

class NBWAConst : public NBW
{
public:

 // Standard Constructor
    NBWAConst(void);
 // Constructor
    NBWAConst(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~NBWAConst(void);

protected:

 // calculate additional heat
    virtual void calcAddHeat(double T_1,double T_2);
 // constraints for no heating cases
    virtual void noLeavingHeat(void);

};

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWAConv
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: enthalpy adaptation by mixing valves and
// (waste-) heat dumping and additional heating
// (similar to CBoiConv)

class NBWAConv : public NBW
{
public:

 // Standard Constructor
    NBWAConv(void);
 // Constructor
    NBWAConv(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~NBWAConv(void);

protected:

 // calculate additional heat
    virtual void calcAddHeat(double T_1,double T_2);
 // constraints for no heating cases
    virtual void noLeavingHeat(void);

    double x;
    double y;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWACond
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: enthalpy adaptation by mixing valves and
//          (waste-) heat dumping and additional heating
//          (similar to CBoiCond)

class NBWACond : public NBW
{
public:

 // Standard Constructor
    NBWACond(void);
 // Constructor
    NBWACond(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~NBWACond(void);

protected:

 // calculate additional heat
    virtual void calcAddHeat(double T_1,double T_2);
 // constraints for no heating cases
    virtual void noLeavingHeat(void);

};

///////////////////////////////////////////////////////////////////
//
// CLASS: NHEx
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: heat exchanger with time constant
//          temperature difference, no pressure loss,
//          and maximal exergy efficiency

class NHEx : public Proc
{
public:

 // Standard Constructor
    NHEx(void);
 // Constructor
    NHEx(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~NHEx(void);
 // actualize attributes of exit side
    virtual void actualExJ(const Map<Symbol,double>& pVecU);
 // actualize attributes of entrance side
    virtual void actualEnJ(const Map<Symbol,double>& pVecU);
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

    double T_FEx, T_REx, T_FEn, T_REn;    // help variables

};

///////////////////////////////////////////////////////////////////
//
// CLASS: NQTrans
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: transport of heat and comparison of quality

class NQTrans : public Proc
{
public:

 // Standard Constructor
    NQTrans(void);
 // Constructor
    NQTrans(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~NQTrans(void);
 // actualize attributes of exit side
    virtual void actualExJ(const Map<Symbol,double>& pVecU);
 // actualize attributes of entrance side
    virtual void actualEnJ(const Map<Symbol,double>& pVecU);
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

    double T_FEx, T_FEn;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: NHTrans
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: transportation of enthalpy

class NHTrans : public Proc
{
public:

 // Standard Constructor
    NHTrans(void);
 // Constructor
    NHTrans(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~NHTrans(void);
 // actualize attributes of exit side
    virtual void actualExJ(const Map<Symbol,double>& pVecU);
 // actualize attributes of entrance side
    virtual void actualEnJ(const Map<Symbol,double>& pVecU);
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

    double T_FEx, T_FEn, T_REx, T_REn, T_Env;
    double AkOverMc;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: NEHP
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: waste heat or solar energy using electrical heat pump

class NEHP : public Proc
{
public:

 // Standard Constructor
    NEHP(void);
 // Constructor
    NEHP(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~NEHP(void);
 // actualize attributes of exit side
    virtual void actualExJ(const Map<Symbol,double>& pVecU);
 // actualize attributes of entrance side
    virtual void actualEnJ(const Map<Symbol,double>& pVecU);
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
// CLASS: NGHP
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: waste heat or solar energy using gas driven heat pump

class NGHP : public Proc
{
public:

 // Standard Constructor
    NGHP(void);
 // Constructor
    NGHP(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~NGHP(void);
 // actualize attributes of exit side
    virtual void actualExJ(const Map<Symbol,double>& pVecU);
 // actualize attributes of entrance side
    virtual void actualEnJ(const Map<Symbol,double>& pVecU);
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
// CLASS: NAHP
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: absorption heat pump
//
//    using as heat source
//
//       * waste heat or solar energy: first digit of FLAG = 1 (FLAG = 12, 14)
//
//       * ambient air, soil or water: first digit of FLAG = 0 (FLAG = 02, 04)
//
//          (heat flow from ambient air, soil or water needs not to be
//           modelled explicitely in the graph since it is taken from the
//           environment in arbitrary quantity, like in CEHP, CGHP;
//           contrary to those, NAHP is a Network Process also in this case
//           since it is driven by a seperate modul, e.g. a gas boiler
//           CBoiConst, or, possibly, by high temperature waste heat,
//           see underneath)
//
//    serving heat demand
//
//       *  load storage: second digit of FLAG = 2 (FLAG = 12, 02)
//
//       *  else        : second digit of FLAG = 4 (FLAG = 14, 04)
//
//    drive of heat pump
//
//       by seperate modul, e.g. a gas boiler, CBoiConst, or, possibly,
//       by high temperature waste heat.
//       In any case the drive is modelled as TYP I - Nettoenthalpy - Strom
//       (vgl. Thomas' Dis, S. 23),
//       (info about flow and return temperature delivered by drive)

class NAHP : public Proc
{
public:
// Standard Constructor
  NAHP(void);
// Constructor
  NAHP(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
  virtual ~NAHP(void);
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
// CLASS: NBWACog
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: Enthalpy adaptation by mixing valves, (waste-) heat dumping
//          and additional heating (similar to CBoiConst)

class NBWACog : public NBW
{
public:
// Standard Constructor
  NBWACog(void);
// Constructor
  NBWACog(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
  virtual ~NBWACog(void);

protected:

double TF_Min;
double TF_Max;
double TR_Min;
double TR_Max;

// calculate additional heat
  virtual void calcAddHeat(double T_1,double T_2);
// constraints for no heating cases
  virtual void noLeavingHeat(void);
  virtual void replace_calcAddHeat(void);
};

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWAAHP
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: Enthalpy adaptation by mixing valves, (waste-) heat dumping
//          and additional heating (similar to CBoiConst)

class NBWAAHP : public NBW
{
public:
// Standard Constructor
  NBWAAHP(void);
// Constructor
  NBWAAHP(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
  virtual ~NBWAAHP(void);

protected:

// calculate additional heat
  virtual void calcAddHeat(double T_1,double T_2);
// constraints for no heating cases
  virtual void noLeavingHeat(void);
};

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWAEHP
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: Enthalpy adaptation by mixing valves, (waste-) heat dumping
//          and additional heating (similar to CBoiConst)

class NBWAEHP : public NBW
{
public:
// Standard Constructor
  NBWAEHP(void);
// Constructor
  NBWAEHP(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
  virtual ~NBWAEHP(void);

protected:

// calculate additional heat
  virtual void calcAddHeat(double T_1,double T_2);
// constraints for no heating cases
  virtual void noLeavingHeat(void);
};

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWAGHP
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: Enthalpy adaptation by mixing valves, (waste-) heat dumping
//          and additional heating (similar to CBoiConst)

class NBWAGHP : public NBW
{
public:
// Standard Constructor
  NBWAGHP(void);
// Constructor
  NBWAGHP(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
  virtual ~NBWAGHP(void);

protected:

// calculate additional heat
  virtual void calcAddHeat(double T_1,double T_2);
// constraints for no heating cases
  virtual void noLeavingHeat(void);
};

///////////////////////////////////////////////////////////////////
//
// CLASS: NHTransSol
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: Transportation of enthalpy

class NHTransSol : public Proc
{
public:
// Standard Constructor
  NHTransSol(void);
// Constructor
  NHTransSol(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
  virtual ~NHTransSol(void);
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
double T_FEx, T_FEn, T_REx, T_REn, T_Env;
double AkOverMc;

};

///////////////////////////////////////////////////////////////////
//
// CLASS: NHEx24
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: heat exchanger with time constant
//          temperature difference, no pressure loss,
//          and maximal exergy efficiency

class NHEx24 : public Proc
{
public:

 // Standard Constructor
    NHEx24(void);
 // Constructor
    NHEx24(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~NHEx24(void);
 // actualize attributes of exit side
    virtual void actualExJ(const Map<Symbol,double>& pVecU);
 // actualize attributes of entrance side
    virtual void actualEnJ(const Map<Symbol,double>& pVecU);
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

    double T_FEx, T_REx, T_FEn, T_REn;    // help variables

};

///////////////////////////////////////////////////////////////////
//
// CLASS: NStMan
//
///////////////////////////////////////////////////////////////////

// Module author: Kathrin Ramsel
// Development thread: S05
//
// Summary: Waermeschiene

class NStMan : public Proc
{
public:
// Standard Constructor
  NStMan(void);
// Constructor
  NStMan(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
  virtual ~NStMan(void);
// update procOutTsPack after each optimization
//  virtual void updateProcOutTsPack(double actualIntLength, double intLength);
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

#endif  // _NETW_
