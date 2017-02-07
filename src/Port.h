
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
// DEECO IMPORT-EXPORT-PROCESS MODULES
//
//////////////////////////////////////////////////////////////////

#ifndef _PORT_                // header guard
#define _PORT_

#include "App.h"
#include "Data.h"
#include "DGraph.h"
#include "Proc.h"
#include <String.h>           // easy string management
#include <Symbol.h>           // use Symbol instead of String for keys
#include <Map.h>              // associative array

///////////////////////////////////////////////////////////////////
//
// CLASS: PConstElImp
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: import of electrical energy from a surrounding
// energy-supply system, which is time independent

class PConstElImp : public Proc
{
public:

 // Standard Constructor
    PConstElImp(void);
 // Constructor
    PConstElImp(App* cProcApp, Symbol procId, Symbol cProcType,
                ioValue* cProcVal);
 // Destructor
    virtual ~PConstElImp(void);
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
// CLASS: PConstElExp
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: export of electrical energy to a surrounding
// energy-supply system, which is time independent

class PConstElExp : public Proc
{
public:

 // Standard Constructor
    PConstElExp(void);
 // Constructor
    PConstElExp(App* cProcApp, Symbol procId, Symbol cProcType,
                ioValue* cProcVal);
 // Destructor
    virtual ~PConstElExp(void);
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
// CLASS: PConstFuelImp
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: import of chemical energy from a surrounding
// energy-supply system, which is time independent

class PConstFuelImp : public Proc
{
public:

 // Standard Constructor
    PConstFuelImp(void);
 // Constructor
    PConstFuelImp(App* cProcApp, Symbol procId, Symbol cProcType,
                  ioValue* cProcVal);
 // Destructor
    virtual ~PConstFuelImp(void);
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
// CLASS: PFlucElImp
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: import of electrical energy from a surrounding
// energy-supply system, which is time dependent

class PFlucElImp : public Proc
{
public:

 // Standard Constructor
    PFlucElImp(void);
 // Constructor
    PFlucElImp(App* cProcApp, Symbol procId, Symbol cProcType,
               ioValue* cProcVal);
 // Destructor
    virtual ~PFlucElImp(void);
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
// CLASS: PFlucElExp
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: export of electrical energy to a surrounding
// energy-supply system, which is time dependent

class PFlucElExp : public Proc
{
public:

 // Standard Constructor
    PFlucElExp(void);
 // Constructor
    PFlucElExp(App* cProcApp, Symbol procId, Symbol cProcType,
               ioValue* cProcVal);
 // Destructor
    virtual ~PFlucElExp(void);
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
// CLASS: PFlucFuelImp
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: import of chemical energy from a surrounding
// energy-supply system, which is time dependent

class PFlucFuelImp : public Proc
{
public:

 // Standard Constructor
    PFlucFuelImp(void);
 // Constructor
    PFlucFuelImp(App* cProcApp, Symbol procId, Symbol cProcType,
                 ioValue* cProcVal);
 // Destructor
    virtual ~PFlucFuelImp(void);
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

#endif  // _PORT_
