
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
// DEECO DEMAND-PROCESS MODULES
//
//////////////////////////////////////////////////////////////////


#ifndef _DEMA_                // header guard
#define _DEMA_


#include "App.h"
#include "Data.h"
#include "DGraph.h"
#include "Proc.h"
#include <String.h>           // easy string management
#include <Symbol.h>           // use Symbol instead of String for keys
#include <Map.h>              // associative array


///////////////////////////////////////////////////////////////////
//
// CLASS: DConstEl
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant demand for electrical energy

class DConstEl : public Proc
{
public:

 // Standard Constructor
    DConstEl(void);
 // Constructor
    DConstEl(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DConstEl(void);
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
// CLASS: DConstMech
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant demand for mechanical energy

class DConstMech : public Proc
{
public:

 // Standard Constructor
    DConstMech(void);
 // Constructor
    DConstMech(App* cProcApp, Symbol procId,Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DConstMech(void);
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
// CLASS: DConstH
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant demand for enthalpy

class DConstH : public Proc
{
public:

 // Standard Constructor
    DConstH(void);
 // Constructor
    DConstH(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DConstH(void);
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
// CLASS: DConstQ
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant demand for heat, the return
// flow temperature is not determined in this case!

class DConstQ : public Proc
{
public:

 // Standard Constructor
    DConstQ(void);
 // Constructor
    DConstQ(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DConstQ(void);
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


//////////////////////////////////////////////////////////////////
//
// CLASS: DConstWH
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant waste heat production (enthalpy flow)

class DConstWH : public Proc
{
public:

 // Standard Constructor
    DConstWH(void);
 // Constructor
    DConstWH(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DConstWH(void);
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
// CLASS: DConstWQ
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant waste heat production (heat flow)

class DConstWQ : public Proc
{
public:

 // Standard Constructor
    DConstWQ(void);
 // Constructor
    DConstWQ(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DConstWQ(void);
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
// CLASS: DFlucEl
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating demand for electrical energy

class DFlucEl : public Proc
{
public:

 // Standard Constructor
    DFlucEl(void);
 // Constructor
    DFlucEl(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DFlucEl(void);
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
// CLASS: DFlucMech
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating demand for mechanical energy

class DFlucMech : public Proc
{
public:

 // Standard Constructor
    DFlucMech(void);
 // Constructor
    DFlucMech(App* cProcApp, Symbol procId,Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DFlucMech(void);
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
// CLASS: DFlucH
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating demand for enthalpy

class DFlucH : public Proc
{
public:

 // Standard Constructor
    DFlucH(void);
 // Constructor
    DFlucH(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DFlucH(void);
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
// CLASS: DFlucQ
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating demand for heat, the return
// temperature is not determined in this case!

class DFlucQ : public Proc
{
public:

 // Standard Constructor
    DFlucQ(void);
 // Constructor
    DFlucQ(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DFlucQ(void);
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
// CLASS: DFlucWH
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating waste heat production (enthalpy flow)

class DFlucWH : public Proc
{
public:

 // Standard Constructor
    DFlucWH(void);
 // Constructor
    DFlucWH(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DFlucWH(void);
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
// CLASS: DFlucWQ
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating waste heat production (heat flow)

class DFlucWQ : public Proc
{
public:

 // Standard Constructor
    DFlucWQ(void);
 // Constructor
    DFlucWQ(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DFlucWQ(void);
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


//////////////////////////////////////////////////////////////////
//
// CLASS: DRoom
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: room heating demand for enthalpy

class DRoom : public Proc
{
public:

 // Standard Constructor
    DRoom(void);
 // Constructor
    DRoom(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~DRoom(void);
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
// CLASS: DNet4Room
//
///////////////////////////////////////////////////////////////////

// Module author: Jan Heise
// Development thread: S07
//
// Summary: Fluctuating demand for heat (roomheating),
//          the return and flow temperatures are determined
//          by the destrict heating grid

class DNet4Room : public Proc
{
public:
// Standard Constructor
  DNet4Room(void);
// Constructor
  DNet4Room(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
  virtual ~DNet4Room(void);
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
};


///////////////////////////////////////////////////////////////////
//
// CLASS: DHeat
//
///////////////////////////////////////////////////////////////////

// Module author: Jan Heise
// Development thread: S07
//
// Summary: room heating demand for enthalpy with minimal
//          flow temperature for drinkwater heating

class DHeat : public Proc
{
public:
// Standard Constructor
   DHeat(void);
// Constructor
   DHeat(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
   virtual ~DHeat(void);
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



#endif // _DEMA_

