
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


#include "Port.h"
#include <math.h>


// for comparison of (x-x == 0) a value REL_EPS should be used
// according to the HP-UX Floating Point Guide
// for single precision  REL_EPS= 10e5, double prec. REL_EPS = 10e14
// and quad. prec. REL_EPS = 10e26;
// use then  (x-x =0) --> ( fabs(x-x) < (x/REL_EPS) )
// fabs() -> <math.h>

#define REL_EPS  10e14


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


//// PConstElImp
// Standard Constructor
//
PConstElImp::PConstElImp(void)
{
}


//// PConstElImp
// Constructor
//
PConstElImp::PConstElImp(App* cProcApp, Symbol procId, Symbol cProcType,
                         ioValue* cProcVal) : Proc(cProcApp, procId,
                         cProcType, cProcVal)
{

  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);

  if (!procInValMap.element("sVarE"))
    procApp->message(55, vertexId.the_string() + " sVarE");

  // the other parameters (sVarM, sVarCO2, etc) are optional

  if (procApp->testFlag)
    procApp->message(1001, "PConstElImp");
}


//// ~PConstElImp
// Destructor
//
PConstElImp::~PConstElImp(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "PConstElImp");
    }
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void PConstElImp::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                     double actualIntLength)
{
  if (procInValMap.element("DotEEl_0"))
    {
      if (procInValMap["DotEEl_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEEl_0");

      lessConstraintRhs[1] = procInValMap["DotEEl_0"];
      lessConstraintCoef[1]["Ex"]["El"]["0"] = 1;
    }

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
            objectFuncCoef[part2]["Ex"]["El"]["0"]=procInValMapIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol PConstElImp::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "El";
  showPowerNumber = "0";
  return "Ex";
}


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


//// PConstElExp
// Standard Constructor

PConstElExp::PConstElExp(void)
{
}


//// PConstElExp
// Constructor
//
PConstElExp::PConstElExp(App* cProcApp, Symbol procId, Symbol cProcType,
                         ioValue* cProcVal) : Proc(cProcApp, procId,
                         cProcType, cProcVal)
{

  Symbol2 a = Symbol2("El","0");
  En.insert(a);

  if (!procInValMap.element("sVarE"))
    procApp->message(55, vertexId.the_string() + " sVarE");

  // the other parameters (sVarM, sVarCO2, etc) are optional

  if (procApp->testFlag)
    procApp->message(1001, "PConstElExp");
}


//// ~PConstElExp
// Destructor
//
PConstElExp::~PConstElExp(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "PConstElExp");
    }
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void PConstElExp::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                     double actualIntLength)
{
  if (procInValMap.element("DotEEl_0"))
    {
      if (procInValMap["DotEEl_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEEl_0");

      lessConstraintRhs[1] = procInValMap["DotEEl_0"];
      lessConstraintCoef[1]["En"]["El"]["0"] = 1;
    }

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
           objectFuncCoef[part2]["En"]["El"]["0"]= -procInValMapIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol PConstElExp::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "El";
  showPowerNumber = "0";
  return "En";
}


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


//// PConstFuelImp
// Standard Constructor
//
PConstFuelImp::PConstFuelImp(void)
{
}


//// PConstFuelImp
// Constructor
//
PConstFuelImp::PConstFuelImp(App* cProcApp, Symbol procId, Symbol cProcType,
                             ioValue* cProcVal) : Proc(cProcApp, procId,
                             cProcType, cProcVal)
{

  Symbol2 a = Symbol2("Fuel","0");
  Ex.insert(a);

  if (!procInValMap.element("sVarE"))
    procApp->message(55, vertexId.the_string() + " sVarE");

  // the other parameters (sVarM, sVarCO2, etc) are optional

  if (procApp->testFlag)
    procApp->message(1001, "PConstFuelImp");
}


//// ~PConstFuelImp
// Destructor
//
PConstFuelImp::~PConstFuelImp(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "PConstFuelImp");
    }
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void PConstFuelImp::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                       double actualIntLength)
{
  if (procInValMap.element("DotEFuel_0"))
    {
      if (procInValMap["DotEFuel_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEFuel_0");

      lessConstraintRhs[1] = procInValMap["DotEFuel_0"];
      lessConstraintCoef[1]["Ex"]["Fuel"]["0"] = 1;
    }

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
            objectFuncCoef[part2]["Ex"]["Fuel"]["0"] =
              procInValMapIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol PConstFuelImp::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "Fuel";
  showPowerNumber = "0";
  return "Ex";
}


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


//// PFlucElImp
// Standard Constructor
//
PFlucElImp::PFlucElImp(void)
{
}


//// PFlucElImp
// Constructor
//
PFlucElImp::PFlucElImp(App* cProcApp, Symbol procId, Symbol cProcType,
                       ioValue* cProcVal) : Proc(cProcApp, procId,
                       cProcType, cProcVal)
{

  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);

  if (procApp->testFlag)
    procApp->message(1001, "PFlucElImp");
}


//// ~PFlucElImp
// Destructor
//
PFlucElImp::~PFlucElImp(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "PFlucElImp");
    }
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void PFlucElImp::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                    double actualIntLength)
{
  if (procInValMap.element("DotEEl_0"))
    {
      if (procInValMap["DotEEl_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEEl_0");

      lessConstraintRhs[1] = procInValMap["DotEEl_0"];
      lessConstraintCoef[1]["Ex"]["El"]["0"] = 1;
    }

  if (!procInTsPack.element("sVarE"))
    procApp->message(58, vertexId.the_string() + " sVarE");

  // the other parameters (sVarM, sVarCO2, etc) are optional

  for (Mapiter<Symbol,double> procInTsPackIt = procInTsPack.first();
       procInTsPackIt;  procInTsPackIt.next())
    {
      String name = procInTsPackIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
           objectFuncCoef[part2]["Ex"]["El"]["0"] = procInTsPackIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol PFlucElImp::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "El";
  showPowerNumber = "0";
  return "Ex";
}


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


//// PFlucElExp
// Standard Constructor
//
PFlucElExp::PFlucElExp(void)
{
}


//// PFlucElExp
// Constructor
//
PFlucElExp::PFlucElExp(App* cProcApp, Symbol procId, Symbol cProcType,
                       ioValue* cProcVal) : Proc(cProcApp, procId,
                       cProcType, cProcVal)
{

  Symbol2 a = Symbol2("El","0");
  En.insert(a);

  if (procApp->testFlag)
    procApp->message(1001, "PFlucElExp");
}


//// ~PFlucElExp
// Destructor
//
PFlucElExp::~PFlucElExp(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "PFlucElExp");
    }
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void PFlucElExp::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                    double actualIntLength)
{
  if (procInValMap.element("DotEEl_0"))
    {
      if (procInValMap["DotEEl_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEEl_0");

      lessConstraintRhs[1] = procInValMap["DotEEl_0"];
      lessConstraintCoef[1]["En"]["El"]["0"] = 1;
    }

  if (!procInTsPack.element("sVarE"))
    procApp->message(58, vertexId.the_string() + " sVarE");

  // the other parameters (sVarM, sVarCO2, etc) are optional

  for (Mapiter<Symbol,double> procInTsPackIt = procInTsPack.first();
       procInTsPackIt;  procInTsPackIt.next())
    {
      String name = procInTsPackIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
           objectFuncCoef[part2]["En"]["El"]["0"] = -procInTsPackIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol PFlucElExp::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "El";
  showPowerNumber = "0";
  return "En";
}


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


//// PFlucFuelImp
// Standard Constructor
//
PFlucFuelImp::PFlucFuelImp(void)
{
}


//// PFlucFuelImp
// Constructor
//
PFlucFuelImp::PFlucFuelImp(App* cProcApp, Symbol procId, Symbol cProcType,
                           ioValue* cProcVal) : Proc(cProcApp, procId,
                           cProcType, cProcVal)
{

  Symbol2 a = Symbol2("Fuel","0");
  Ex.insert(a);

  if (procApp->testFlag)
    procApp->message(1001, "PFlucFuelImp");
}


//// ~PFlucFuelImp
// Destructor
//
PFlucFuelImp::~PFlucFuelImp(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "PFlucFuelImp");
    }
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void PFlucFuelImp::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                      double actualIntLength)
{
  if (procInValMap.element("DotEFuel_0"))
    {
      if (procInValMap["DotEFuel_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEFuel_0");

      lessConstraintRhs[1] = procInValMap["DotEFuel_0"];
      lessConstraintCoef[1]["Ex"]["Fuel"]["0"] = 1;
    }

  if (!procInTsPack.element("sVarE"))
    procApp->message(58, vertexId.the_string() + " sVarE");

  // the other parameters (sVarM, sVarCO2, etc) are optional

  for (Mapiter<Symbol,double> procInTsPackIt = procInTsPack.first();
       procInTsPackIt; procInTsPackIt.next())
    {
      String name = procInTsPackIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
            objectFuncCoef[part2]["Ex"]["Fuel"]["0"] =
              procInTsPackIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol PFlucFuelImp::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "Fuel";
  showPowerNumber = "0";
  return "Ex";
}


// end of file


