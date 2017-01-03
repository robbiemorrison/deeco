
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
// DEECO CONVERSION-PROCESS MODULES
//
//////////////////////////////////////////////////////////////////


#include "Convers.h"
#include <math.h>
#include <float.h>       // for numerical infinity (DBL_MAX, DBL_MIN)


// for comparison of (x-x == 0) a value REL_EPS should be used
// according to the HP-UX Floating Point Guide
// for single precision  REL_EPS= 10e5, double prec. REL_EPS = 10e14
// and quad. prec. REL_EPS = 10e26;
// use then  (x-x =0) --> ( fabs(x-x) < (x/REL_EPS) )
// fabs() -> <math.h>

#define REL_EPS  10e14

#include "N-R_errormsg.c" // for Newton-Raphson-algorithem in CLASS
			  // CFCellConst



///////////////////////////////////////////////////////////////////
//
// CLASS: CBoiConst
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: boiler with constant efficiency


//// CBoiConst
// Standard Constructor
//
CBoiConst::CBoiConst(void)
{
  T_FEx = 0;
  T_REx = 0;
}


//// CBoiConst
// Constructor
//
CBoiConst::CBoiConst(App* cProcApp, Symbol procId, Symbol cProcType,
                     ioValue* cProcVal) : Proc(cProcApp, procId,
                     cProcType, cProcVal)
{
  T_FEx = 0;
  T_REx = 0;

  if (!procInValMap.element("eta"))
    procApp->message(55, vertexId.the_string() + " eta");
  if (!procInValMap.element("lambdaEl_H"))
    procApp->message(55, vertexId.the_string() + " lambdaEl_H");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if (((procInValMap["Flag"] == 1)  || (procInValMap["Flag"] == 2))
      && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if ((procInValMap["Flag"] == 1) && (!procInValMap.element("TR_0")))
    procApp->message(55, vertexId.the_string() + " TR_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["eta"] == 0 ||
      procInValMap["Count"] == 0 )
    procApp->message(505, vertexId.the_string());
  if ((procInValMap["Flag"] != 1) &&
      (procInValMap["Flag"] != 2) &&
      (procInValMap["Flag"] != 4))
    procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("Fuel","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  Ex.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "CBoiConst");
}


//// ~CBoiConst
// Destructor
//
CBoiConst::~CBoiConst(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "CBoiConst");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void CBoiConst::actualExJ(const Map<Symbol,double>& pVecU)
{
  if ((procInValMap["Flag"] == 1) || (procInValMap["Flag"] == 2))
    {
      T_FEx  = procInValMap["TF_0"];
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
    }
  if (procInValMap["Flag"] == 1)
    {
      T_REx = procInValMap["TR_0"];
      vecJ["0"]["Ex"]["Out"]["R"]["T"] = T_REx;
    }

}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void CBoiConst::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                   double actualIntLength)
{
  // control of input

  if (procInValMap["Flag"] == 4)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
    }

  if ((procInValMap["Flag"] == 2) || (procInValMap["Flag"] == 4))
    {
      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
        T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");
    }

  // technical constraints

  if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }

  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = -1.0/procInValMap["eta"];
  equalConstraintCoef[2]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[3] = 0;
  equalConstraintCoef[3]["Ex"]["H"]["0"] = -procInValMap["lambdaEl_H"];
  equalConstraintCoef[3]["En"]["El"]["0"] = 1;

  // abnormal situation

  if ((T_FEx < 0) || (T_REx < 0) || (T_FEx <= T_REx))
    {
      equalConstraintRhs[4] = 0;
      equalConstraintCoef[4]["Ex"]["H"]["0"] = 1;
    }

  // emissions

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
            objectFuncCoef[part2]["En"]["Fuel"]["0"] =
              procInValMapIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol CBoiConst::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CBoiConv
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: conventional oil or gas boiler (no condensing)


//// CBoiConv
// Standard Constructor
//
CBoiConv::CBoiConv(void)
{
  x = 0;
  y = 0;
}


//// CBoiConv
// Constructor
//
CBoiConv::CBoiConv(App* cProcApp, Symbol procId, Symbol cProcType,
    ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  if (!procInValMap.element("T_Env"))
    procApp->message(55, vertexId.the_string() + " T_Env");
  if (!procInValMap.element("A_1"))
    procApp->message(55, vertexId.the_string() + " A_1");
  if (!procInValMap.element("B"))
    procApp->message(55, vertexId.the_string() + " B");
  if (!procInValMap.element("x_CO2"))
    procApp->message(55, vertexId.the_string() + " x_CO2");
  if (!procInValMap.element("T_W"))
    procApp->message(55, vertexId.the_string() + " T_W");
  if (!procInValMap.element("T_B"))
    procApp->message(55, vertexId.the_string() + " T_B");
  if (!procInValMap.element("DotQ_Rad"))
    procApp->message(55, vertexId.the_string() + " DotQ_Rad");
  if (!procInValMap.element("lambdaEl_H"))
    procApp->message(55, vertexId.the_string() + " lambdaEl_H");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if ((procInValMap["Flag"] == 2) && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["x_CO2"] <= 0 ||
      procInValMap["Count"] <= 0 )
    procApp->message(505, vertexId.the_string());
  if ((procInValMap["Flag"] != 2) && (procInValMap["Flag"] != 4))
      procApp->message(61, vertexId.the_string() + " Flag");

  x = 0;
  y = 0;

  x = procInValMap["T_B"] - procInValMap["T_Env"];

  if (x <= 0)
    procApp->message(61, vertexId.the_string() + ": T_B,T_Env");
  x = (1.0/x) * procInValMap["DotQ_Rad"] *  procInValMap["Count"];

  y = procInValMap["A_1"]/ (100*procInValMap["x_CO2"]);
  y = y +  procInValMap["B"];
  y = y/100.00;

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("Fuel","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  Ex.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "CBoiConv");
}


//// ~CBoiConv
// Destructor
//
CBoiConv::~CBoiConv(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "CBoiConv");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void CBoiConv::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 2)
    vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"];
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void CBoiConv::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength)
{
  double z     = 0;
  double rhs   = 0;
  double T_REx = 0;
  double T_FEx = 0;
  double T_Boi = 0;

  // control of input

  if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");

  if (procInValMap["Flag"] != 2)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
    }
  else
    {
      T_FEx = procInValMap["TF_0"];  // Flag == 2
    }

  if ((T_FEx < 0) || (T_REx < 0) || (T_FEx <= T_REx))
    {
      equalConstraintRhs[4] = 0;
      equalConstraintCoef[4]["Ex"]["H"]["0"] = 1;
    }

  // technical constraints

 if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }

  T_Boi = (T_FEx+T_REx)/2.0;

  z = procInValMap["T_W"] + (T_Boi - procInValMap["T_B"]);
  z = z -  procInValMap["T_Env"];
  if (z <= 0)
    procApp->message(65,vertexId.the_string() + " actualSimplexInput, z");
  z = z * y;
  z = 1 - z;
  if (z <= 0)
    procApp->message(65,vertexId.the_string() + " actualSimplexInput, z");

  z = 1.0/z;

  rhs = T_Boi - procInValMap["T_Env"];

  if (rhs <= 0)
    procApp->message(65,vertexId.the_string() + " in actualSimplexInput, rhs");
  rhs = rhs * z * x;

  equalConstraintRhs[2] = rhs;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = -z;
  equalConstraintCoef[2]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[3] = 0;
  equalConstraintCoef[3]["Ex"]["H"]["0"] = -procInValMap["lambdaEl_H"];
  equalConstraintCoef[3]["En"]["El"]["0"] = 1;

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
            objectFuncCoef[part2]["En"]["Fuel"]["0"] =
              procInValMapIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol CBoiConv::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CBoiCond
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: oil or gas condensing boiler


//// CBoiCond
// Standard Constructor
//
CBoiCond::CBoiCond(void)
{
}


//// CBoiCond
// Constructor
//
CBoiCond::CBoiCond(App* cProcApp, Symbol procId, Symbol cProcType,
                   ioValue* cProcVal) : Proc(cProcApp, procId,
                   cProcType, cProcVal)
{
  if (!procInValMap.element("T_0"))
    procApp->message(55, vertexId.the_string() + " T_0");
  if (!procInValMap.element("T_Cond"))
    procApp->message(55, vertexId.the_string() + " T_Cond");
  if (!procInValMap.element("TR_Max"))
    procApp->message(55, vertexId.the_string() + " TR_Max");
  if (!procInValMap.element("TR_Min"))
    procApp->message(55, vertexId.the_string() + " TR_Min");
  if (!procInValMap.element("A_1"))
    procApp->message(55, vertexId.the_string() + " A_1");
  if (!procInValMap.element("A_2"))
    procApp->message(55, vertexId.the_string() + " A_2");
  if (!procInValMap.element("B_1"))
    procApp->message(55, vertexId.the_string() + " B_1");
  if (!procInValMap.element("B_2"))
    procApp->message(55, vertexId.the_string() + " B_2");
  if (!procInValMap.element("C_1"))
    procApp->message(55, vertexId.the_string() + " C_1");
  if (!procInValMap.element("lambdaEl_H"))
    procApp->message(55, vertexId.the_string() + " lambdaEl_H");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if ((procInValMap["Flag"] == 2) && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["B_1"] <= 0 ||
      procInValMap["B_2"] <= 0 ||
      procInValMap["C_1"] <= 0 ||
      procInValMap["Count"] <= 0)
    procApp->message(505, vertexId.the_string());
  if ((procInValMap["Flag"] != 2) && (procInValMap["Flag"] != 4))
      procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("Fuel","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  Ex.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "CBoiCond");
}


//// ~CBoiCond
// Destructor
//
CBoiCond::~CBoiCond(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "CBoiCond");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void CBoiCond::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 2)
    vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"];
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void CBoiCond::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength)
{
  double eta   = 0;
  double T_REx = 0;
  double T_FEx = 0;
  double T_0   = 0;

  // control of input

  if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");

  if (procInValMap["Flag"] != 2)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
    }
  else
    {
      T_FEx = procInValMap["TF_0"];  // Flag == 2
    }

  if ((T_FEx < 0) || (T_REx < 0) || (T_FEx <= T_REx) ||
      (T_REx < procInValMap["TR_Min"]) || ( T_REx > procInValMap["TR_Max"]))
    {
      equalConstraintRhs[4] = 0;
      equalConstraintCoef[4]["Ex"]["H"]["0"] = 1;
    }

  // technical constraints

 if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }

  T_0 = procInValMap["T_0"];

  if (T_REx < procInValMap["T_Cond"])
    eta = procInValMap["A_1"] - procInValMap["B_1"] * (T_REx-T_0) -
      procInValMap["C_1"] * (T_REx-T_0) * (T_REx-T_0);
  else
    eta = procInValMap["A_2"] - procInValMap["B_2"] * (T_REx-T_0);

  if (eta <= 0)
    procApp->message(65,vertexId.the_string() + " actualSimplexInput, eta");

  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = -1.0/eta;
  equalConstraintCoef[2]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[3] = 0;
  equalConstraintCoef[3]["Ex"]["H"]["0"] = -procInValMap["lambdaEl_H"];
  equalConstraintCoef[3]["En"]["El"]["0"] = 1;

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
            objectFuncCoef[part2]["En"]["Fuel"]["0"] =
              procInValMapIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol CBoiCond::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CEHP
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: ambient air, soil or water using electrical heat pump


//// CEHP
// Standard Constructor
//
CEHP::CEHP(void)
{
}


//// CEHP
// Constructor
//
CEHP::CEHP(App* cProcApp, Symbol procId, Symbol cProcType,
           ioValue* cProcVal) : Proc(cProcApp, procId,
           cProcType, cProcVal)
{
  if (!procInValMap.element("eta_G"))
    procApp->message(55, vertexId.the_string() + " eta_G");
  if (!procInValMap.element("f_Ice0"))
    procApp->message(55, vertexId.the_string() + " f_Ice0");
  if (!procInValMap.element("T_Ice"))
    procApp->message(55, vertexId.the_string() + " T_Ice");
  if (!procInValMap.element("deltaT_Con"))
    procApp->message(55, vertexId.the_string() + " deltaT_Con");
  if (!procInValMap.element("deltaT_Vap"))
    procApp->message(55, vertexId.the_string() + " deltaT_Vap");
  if (!procInValMap.element("T_ConMax"))
    procApp->message(55, vertexId.the_string() + " T_ConMax");
  if (!procInValMap.element("T_ConMin"))
    procApp->message(55, vertexId.the_string() + " T_ConMin");
  if (!procInValMap.element("T_VapMax"))
    procApp->message(55, vertexId.the_string() + " T_VapMax");
  if (!procInValMap.element("T_VapMin"))
    procApp->message(55, vertexId.the_string() + " T_VapMin");
  if (!procInValMap.element("c"))
    procApp->message(55, vertexId.the_string() + " c");
  if (!procInValMap.element("d"))
    procApp->message(55, vertexId.the_string() + " d");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if ((procInValMap["Flag"] == 2) && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["f_Ice0"] <= 0 ||
      procInValMap["eta_G"] <= 0 )
    procApp->message(505, vertexId.the_string());
  if (procInValMap["T_ConMax"] <= procInValMap["T_ConMin"])
    procApp->message(61, vertexId.the_string() + " T_ConMax,T_ConMin");
  if (procInValMap["T_VapMax"] <= procInValMap["T_VapMin"])
    procApp->message(61, vertexId.the_string() + " T_VapMax,T_VapMin");
  if ((procInValMap["Flag"] != 2) && (procInValMap["Flag"] != 4))
      procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("H","0");
  Ex.insert(b);

  if (procApp->testFlag)
    procApp->message(1001, "CEHP");
}


//// ~CEHP
// Destructor
//
CEHP::~CEHP(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "CEHP");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void CEHP::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 2)
    vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"];
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void CEHP::actualSimplexInput(const Map<Symbol,double>& pVecU,
                              double actualIntLength)
{
  double T_FCon = 0;
  double T_HS   = 0;
  double T_REx  = 0;
  double T_FEx  = 0;
  double f_Ice  = 1;

  // control of input

  if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");

  if (procInValMap["Flag"] != 2)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
    }
  else
    {
      T_FEx = procInValMap["TF_0"];  // Flag == 2
    }

  // calculate heat source temperature

  if (pVecU.element("T"))
    {
      T_HS = procInValMap["d"] * pVecU["T"] + procInValMap["c"];
      if ( pVecU["T"] <  procInValMap["T_Ice"])
        f_Ice = procInValMap["f_Ice0"];
    }
  else
    procApp->message(57,"T");

  // calculate condenser temperature T_FCon

  T_FCon = T_FEx;

  // calculate inverse efficiency

  double x;

  x = T_FCon + procInValMap["deltaT_Con"];
  x = (x - T_HS + procInValMap["deltaT_Vap"])/x;
  x = x/(procInValMap["eta_G"] * f_Ice);

  if ((T_FEx < 0) || (T_REx < 0) || (T_FEx <= T_REx) ||
      (T_FCon + procInValMap["deltaT_Con"] < T_HS - procInValMap["deltaT_Vap"]) ||
      (x >= 1)    || (x < double(1)/REL_EPS) ||
      (T_FCon + procInValMap["deltaT_Con"] < procInValMap["T_ConMin"]) ||
      (T_FCon + procInValMap["deltaT_Con"] > procInValMap["T_ConMax"]) ||
      (T_HS   - procInValMap["deltaT_Vap"] < procInValMap["T_VapMin"]) ||
      (T_HS   - procInValMap["deltaT_Vap"] > procInValMap["T_VapMax"]))
    {
      equalConstraintRhs[3] = 0;
      equalConstraintCoef[3]["Ex"]["H"]["0"] = 1;
      x = 1;
    }

  // technical constraints

 if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }

  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = -x;
  equalConstraintCoef[2]["En"]["El"]["0"] = 1;

}


//// showPower
//
Symbol CEHP::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CGHP
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: ambient air, soil or water using gas driven heat pump


//// CGHP
// Standard Constructor
//
CGHP::CGHP(void)
{
}


//// CGHP
// Constructor
//
CGHP::CGHP(App* cProcApp, Symbol procId, Symbol cProcType,
           ioValue* cProcVal) : Proc(cProcApp, procId,
           cProcType, cProcVal)
{
  if (!procInValMap.element("eta_G"))
    procApp->message(55, vertexId.the_string() + " eta_G");
  if (!procInValMap.element("f_Ice0"))
    procApp->message(55, vertexId.the_string() + " f_Ice0");
  if (!procInValMap.element("T_Ice"))
    procApp->message(55, vertexId.the_string() + " T_Ice");
  if (!procInValMap.element("deltaT_Con"))
    procApp->message(55, vertexId.the_string() + " deltaT_Con");
  if (!procInValMap.element("deltaT_Vap"))
    procApp->message(55, vertexId.the_string() + " deltaT_Vap");
  if (!procInValMap.element("T_ConMax"))
    procApp->message(55, vertexId.the_string() + " T_ConMax");
  if (!procInValMap.element("T_ConMin"))
    procApp->message(55, vertexId.the_string() + " T_ConMin");
  if (!procInValMap.element("T_VapMax"))
    procApp->message(55, vertexId.the_string() + " T_VapMax");
  if (!procInValMap.element("T_VapMin"))
    procApp->message(55, vertexId.the_string() + " T_VapMin");
  if (!procInValMap.element("c"))
    procApp->message(55, vertexId.the_string() + " c");
  if (!procInValMap.element("d"))
    procApp->message(55, vertexId.the_string() + " d");
  if (!procInValMap.element("eta_Mech"))
    procApp->message(55, vertexId.the_string() + " eta_Mech");
  if (!procInValMap.element("a"))
    procApp->message(55, vertexId.the_string() + " a");
  if (!procInValMap.element("lambdaEl_H"))
    procApp->message(55, vertexId.the_string() + " lambdaEl_H");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if ((procInValMap["Flag"] == 2) && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["f_Ice0"] <= 0 ||
      procInValMap["eta_G"] <= 0)
    procApp->message(505, vertexId.the_string());
  if (procInValMap["T_ConMax"] <= procInValMap["T_ConMin"])
    procApp->message(61, vertexId.the_string() + " T_ConMax,T_ConMin");
  if (procInValMap["T_VapMax"] <= procInValMap["T_VapMin"])
    procApp->message(61, vertexId.the_string() + " T_VapMax,T_VapMin");
  if ((procInValMap["Flag"] != 2) && (procInValMap["Flag"] != 4))
      procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("H","0");
  Ex.insert(b);
  Symbol2 c = Symbol2("Fuel","0");
  En.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "CGHP");
}


//// ~CGHP
// Destructor
//
CGHP::~CGHP(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "CGHP");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void CGHP::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 2)
    vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"];
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void CGHP::actualSimplexInput(const Map<Symbol,double>& pVecU,
                              double actualIntLength)
{
  double T_FCon = 0;
  double T_HS   = 0;
  double T_REx  = 0;
  double T_FEx  = 0;
  double f_Ice  = 1;

  // control of input

  if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");

  if (procInValMap["Flag"] != 2)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
    }
  else
    {
      T_FEx = procInValMap["TF_0"];  // Flag == 2
    }

  // calculate heat source temperature

  if (pVecU.element("T"))
    {
      T_HS = procInValMap["d"] * pVecU["T"] + procInValMap["c"];
      if ( pVecU["T"] <  procInValMap["T_Ice"])
        f_Ice = procInValMap["f_Ice0"];
    }
  else
    procApp->message(57,"T");

  // calculate condenser temperature T_FCon

  double y     = 0;
  double B     = 0;
  double C     = 0;
  double gamma = 0;

  gamma = procInValMap["eta_G"] * f_Ice
    * procInValMap["eta_Mech"]/procInValMap["a"];

  B = procInValMap["deltaT_Con"] - T_HS + procInValMap["deltaT_Vap"];
  B = B - T_REx - gamma * T_FEx + gamma * procInValMap["deltaT_Con"];

  C = -T_REx * (procInValMap["deltaT_Con"] - T_HS + procInValMap["deltaT_Vap"]);
  C = C - gamma * T_FEx * procInValMap["deltaT_Con"];
  y = B * B - 4 * (1 + gamma) * C;

  if (y > 0)
    {
      T_FCon = (-B + sqrt(y))/(2 * (1 + gamma));
      if (T_FCon > T_FEx || T_FCon < T_REx)
        {
          T_FCon = (-B - sqrt(y))/(2 * (1 + gamma));
          if (T_FCon > T_FEx || T_FCon < T_REx)
            procApp->message(70, vertexId.the_string() +
                             " T_FCon in  actualSimplexInput of CGHP-Module");
        }
    }
  else
    procApp->message(65, vertexId.the_string() +
                     " y in actualSimplexInput of CGHP-Module");

  // calculate inverse efficiency

  double x;

  x = T_FCon + procInValMap["deltaT_Con"];
  x = (x - T_HS + procInValMap["deltaT_Vap"])/x;
  x = x/(procInValMap["eta_G"] * f_Ice);

  if ((T_FEx < 0) || (T_REx < 0) ||(T_FEx <= T_REx) ||
      (T_FCon + procInValMap["deltaT_Con"] < T_HS - procInValMap["deltaT_Vap"]) ||
      (x >= 1)    || (x < double(1)/REL_EPS) ||
      (T_FCon + procInValMap["deltaT_Con"] < procInValMap["T_ConMin"]) ||
      (T_FCon + procInValMap["deltaT_Con"] > procInValMap["T_ConMax"]) ||
      (T_HS   - procInValMap["deltaT_Vap"] < procInValMap["T_VapMin"]) ||
      (T_HS   - procInValMap["deltaT_Vap"] > procInValMap["T_VapMax"]))
    {
      equalConstraintRhs[3] = 0;
      equalConstraintCoef[3]["Ex"]["H"]["0"] = 1;
      x = 1;
    }
  else
    {
      x = (procInValMap["eta_Mech"]/x) + procInValMap["a"];
      if (x < double(1)/REL_EPS)
        {
          equalConstraintRhs[3] = 0;
          equalConstraintCoef[3]["Ex"]["H"]["0"] = 1;
          x = 1;
        }
      else
        x = 1/x;
    }

  // technical constraints

 if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }

  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = -x;
  equalConstraintCoef[2]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"] = -procInValMap["lambdaEl_H"];
  equalConstraintCoef[4]["En"]["El"]["0"] = 1;

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
            objectFuncCoef[part2]["En"]["Fuel"]["0"] =
              procInValMapIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol CGHP::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CBPT
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: back-pressure turbine, combined heat and power station


//// CBPT
// Standard Constructor
//
CBPT::CBPT(void)
{
}


//// CBPT
// Constructor
//
CBPT::CBPT(App* cProcApp, Symbol procId, Symbol cProcType,
           ioValue* cProcVal) : Proc(cProcApp, procId,
           cProcType, cProcVal)
{
  if (!procInValMap.element("eta_Boi"))
    procApp->message(55, vertexId.the_string() + " eta_Boi");
  if (!procInValMap.element("eta_Con"))
    procApp->message(55, vertexId.the_string() + " eta_Con");
  if (!procInValMap.element("eta_G"))
    procApp->message(55, vertexId.the_string() + " eta_G");
  if (!procInValMap.element("T_Con"))
    procApp->message(55, vertexId.the_string() + " T_Con");
  if (!procInValMap.element("deltaT"))
    procApp->message(55, vertexId.the_string() + " deltaT");
  if (!procInValMap.element("n"))
    procApp->message(55, vertexId.the_string() + " n");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if ((procInValMap["Flag"] == 2) && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["eta_Boi"] <= 0 ||
      procInValMap["eta_Con"] <= 0)
    procApp->message(505, vertexId.the_string());
  if ((procInValMap["Flag"] != 2) && (procInValMap["Flag"] != 4))
      procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);
  Symbol2 b = Symbol2("H","1");
  Ex.insert(b);
  Symbol2 c = Symbol2("Fuel","0");
  En.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "CBPT");
}


//// ~CBPT
// Destructor
//
CBPT::~CBPT(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "CBPT");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void CBPT::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 2)
    vecJ["1"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"];
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void CBPT::actualSimplexInput(const Map<Symbol,double>& pVecU,
                              double actualIntLength)
{
  double T_M   = 0;
  double T_REx = 0;
  double T_FEx = 0;
  double tau   = 0;

  // control of input

  if (vecJ["1"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["1"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string() + " vecJ[1][Ex][In][R][T]");

  if (procInValMap["Flag"] != 2)
    {
      if (vecJ["1"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["1"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[1][Ex][In][F][T]");
    }
  else
    {
      T_FEx = procInValMap["TF_0"];  // Flag == 2
    }

  // calculate heat extracting (back pressure) temperature

  T_M = T_FEx + procInValMap["deltaT"];
  T_M = T_M - (T_FEx-T_REx) * double(procInValMap["n"] - 1)/(2 * procInValMap["n"]);


  // calculate electricity loss

  double x = 0;
  double y = 0;

  tau = (1 - procInValMap["T_Con"]/T_M) * procInValMap["eta_G"];

  if ((T_FEx < 0) || (T_REx < 0) ||(T_FEx <= T_REx) ||
      (tau >= procInValMap["eta_Con"]))
    {
      equalConstraintRhs[5] = 0;
      equalConstraintCoef[5]["Ex"]["H"]["1"] = 1;
      x = 1;
      y = 1;
    }
  else
    {
      x = (1 - tau)/(procInValMap["eta_Con"] - tau);
      x = x/procInValMap["eta_Boi"];
      y = (1 - procInValMap["eta_Con"])/(procInValMap["eta_Con"] - tau);
    }

  // technical constraints

  if (procInValMap.element("DotEEl_0"))
    {
      if (procInValMap["DotEEl_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEEl_0");

      lessConstraintRhs[1] = procInValMap["DotEEl_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["El"]["0"] = 1;
    }


  if (procInValMap.element("DotEFuel_0"))
    {
      if (procInValMap["DotEFuel_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEFuel_0");

      lessConstraintRhs[2] = procInValMap["DotEFuel_0"] * procInValMap["Count"];
      lessConstraintCoef[2]["En"]["Fuel"]["0"] = 1;
    }

  equalConstraintRhs[3] = 0;
  equalConstraintCoef[3]["Ex"]["El"]["0"] = -x;
  equalConstraintCoef[3]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["El"]["0"] = -y;
  equalConstraintCoef[4]["Ex"]["H"]["1"] = 1;

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
            objectFuncCoef[part2]["En"]["Fuel"]["0"] =
              procInValMapIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol CBPT::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "El";
  showPowerNumber = "0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CECT
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: extraction-condensing turbine, combined heat and power station


//// CECT
// Standard Constructor
//
CECT::CECT(void)
{
  TF_Min = 0;
  TF_Max = 0;
  TR_Min = 0;
  TR_Max = 0;
}


//// CECT
// Constructor
//
CECT::CECT(App* cProcApp, Symbol procId, Symbol cProcType,
           ioValue* cProcVal) : Proc(cProcApp, procId,
           cProcType, cProcVal)
{
  if (!procInValMap.element("a"))
    procApp->message(55, vertexId.the_string() + " a");
  if (!procInValMap.element("b"))
    procApp->message(55, vertexId.the_string() + " b");
  if (!procInValMap.element("c"))
    procApp->message(55, vertexId.the_string() + " c");
  if (!procInValMap.element("g_H"))
    procApp->message(55, vertexId.the_string() + " g_H");
  if (!procInValMap.element("c_BP"))
    procApp->message(55, vertexId.the_string() + " c_BP");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if ((procInValMap["Flag"] == 2) && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (procInValMap.element("d_FB") && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (procInValMap.element("d_FG") && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (procInValMap.element("d_FH") && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (procInValMap.element("d_FH") && (!procInValMap.element("DotEH_0")))
    procApp->message(55, vertexId.the_string() + " DotEH_0");
  if (procInValMap.element("d_RB") && (!procInValMap.element("TR_0")))
    procApp->message(55, vertexId.the_string() + " TR_0");
  if (procInValMap.element("d_RG") && (!procInValMap.element("TR_0")))
    procApp->message(55, vertexId.the_string() + " TR_0");
  if (procInValMap.element("d_RH") && (!procInValMap.element("TR_0")))
    procApp->message(55, vertexId.the_string() + " TR_0");
  if (procInValMap.element("d_RH") && (!procInValMap.element("DotEH_0")))
    procApp->message(55, vertexId.the_string() + " DotEH_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if ((procInValMap["Flag"] != 2) && (procInValMap["Flag"] != 4))
      procApp->message(61, vertexId.the_string() + " Flag");
  if (procInValMap["c_BP"] < 0)
    procApp->message(67, vertexId.the_string() + " c_BP");
  if (procInValMap["c"] < 0)
    procApp->message(67, vertexId.the_string() + " c");

  TF_Min = DBL_MIN;
  if (procInValMap.element("TF_Min"))
    TF_Min = procInValMap["TF_Min"];
  TF_Max = DBL_MAX;
  if (procInValMap.element("TF_Max"))
    TF_Max = procInValMap["TF_Max"];
  TR_Min = DBL_MIN;
  if (procInValMap.element("TR_Min"))
    TR_Min = procInValMap["TR_Min"];
  TR_Max = DBL_MAX;
  if (procInValMap.element("TR_Max"))
    TR_Max = procInValMap["TR_Max"];

  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);
  Symbol2 b = Symbol2("H","0");
  Ex.insert(b);
  Symbol2 c = Symbol2("Fuel","0");
  En.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "CECT");
}


//// ~CECT
// Destructor
//
CECT::~CECT(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "CECT");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void CECT::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 2)
    vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"];
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void CECT::actualSimplexInput(const Map<Symbol,double>& pVecU,
                              double actualIntLength)
{
  double T_REx   = 0;
  double T_FEx   = 0;
  double g_H     = 0;
  double b       = 0;
  double DotEH_0 = 0;
  double TF_0    = 0;
  double TR_0    = 0;


  // control of input

  if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");

  if (procInValMap["Flag"] != 2)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
    }
  else
    {
      T_FEx = procInValMap["TF_0"];  // Flag == 2
    }

  TF_0 = procInValMap["TF_0"];  // remember: For non-existing keys,
                                // elements of a <Symbol,double> - map are
                                // initialized with 0
  TR_0 = procInValMap["TR_0"];


  // calculate temperature-dependent slope of back-pressure line

  g_H = procInValMap["g_H"];
  g_H = g_H + procInValMap["d_FG"] * (T_FEx-TF_0);
  g_H = g_H + procInValMap["d_RG"] * (T_REx-TR_0);

  // calculate temperature-dependent b

  b = procInValMap["b"];
  b = b + procInValMap["d_FB"] * (T_FEx-TF_0);
  b = b + procInValMap["d_RB"] * (T_REx-TR_0);

  // technical constraints

  if (procInValMap.element("DotEFuel_0"))
    {
      if (procInValMap["DotEFuel_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEFuel_0");

      lessConstraintRhs[1] = procInValMap["DotEFuel_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["En"]["Fuel"]["0"] = 1;
    }

  if (procInValMap.element("DotEEl_0"))
    {
      if (procInValMap["DotEEl_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEEl_0");

      lessConstraintRhs[2] = procInValMap["DotEEl_0"] * procInValMap["Count"];
      lessConstraintCoef[2]["Ex"]["El"]["0"] = 1;
    }

  if (procInValMap.element("c_LP"))
    {
      if (procInValMap["c_LP"] < 0)
        procApp->message(67, vertexId.the_string() + " c_LP");

      lessConstraintRhs[3] = procInValMap["c_LP"] * procInValMap["Count"];
      lessConstraintCoef[3]["Ex"]["El"]["0"] = 1;
      lessConstraintCoef[3]["Ex"]["H"]["0"]  = -g_H;
    }

  lessConstraintRhs[4] = procInValMap["c_BP"] * procInValMap["Count"];
  lessConstraintCoef[4]["Ex"]["El"]["0"] = -1;
  lessConstraintCoef[4]["Ex"]["H"]["0"] = g_H;

  if (procInValMap.element("DotEH_0"))
    {
      DotEH_0 =  procInValMap["DotEH_0"];
      DotEH_0 =  DotEH_0 + procInValMap["d_FH"] * (T_FEx-TF_0);
      DotEH_0 =  DotEH_0 + procInValMap["d_RH"] * (T_REx-TR_0);

      if (DotEH_0 < 0)
        DotEH_0 = 0;

      lessConstraintRhs[5] = DotEH_0 * procInValMap["Count"];
      lessConstraintCoef[5]["Ex"]["H"]["0"] = 1;
    }

  // fuel consumption

  equalConstraintRhs[6] = procInValMap["c"] * procInValMap["Count"];
  equalConstraintCoef[6]["Ex"]["El"]["0"] = -procInValMap["a"];
  equalConstraintCoef[6]["Ex"]["H"]["0"] = -b;
  equalConstraintCoef[6]["En"]["Fuel"]["0"] = 1;

  // abnormal situations

  if ((T_FEx < 0)      || (T_REx < 0)      ||
      (T_FEx <= T_REx) ||
      (T_FEx < TF_Min) || (T_FEx > TF_Max) ||
      (T_REx < TR_Min) || (T_REx > TR_Max) ||
      (b <= 0)         || (g_H <= 0))
    {
      equalConstraintRhs[7] = 0;
      equalConstraintCoef[7]["Ex"]["H"]["0"] = 1;
    }

  // emissions

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
            objectFuncCoef[part2]["En"]["Fuel"]["0"] =
              procInValMapIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol CECT::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "El";
  showPowerNumber = "0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CCogConst
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: cogeneration unit with constant efficiencies


//// CCogConst
// Standard Constructor
//
CCogConst::CCogConst(void)
{
  T_FEx  = 0;
  T_REx  = 0;
  TF_Min = 0;
  TF_Max = 0;
  TR_Min = 0;
  TR_Max = 0;
}


//// CCogConst
// Constructor
//
CCogConst::CCogConst(App*     cProcApp,
                     Symbol   procId,
                     Symbol   cProcType,
                     ioValue* cProcVal)
  : Proc(cProcApp,
         procId,
         cProcType,
         cProcVal)
{
  T_FEx = 0;
  T_REx = 0;

  if (!procInValMap.element("eta_El"))
    procApp->message(55, vertexId.the_string() + " eta_El");
  if (!procInValMap.element("eta_H"))
    procApp->message(55, vertexId.the_string() + " eta_H");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if (((procInValMap["Flag"] == 1)  || (procInValMap["Flag"] == 2))
      && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if ((procInValMap["Flag"] == 1) && (!procInValMap.element("TR_0")))
    procApp->message(55, vertexId.the_string() + " TR_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["eta_El"] == 0)
    procApp->message(505, vertexId.the_string());
  if ((procInValMap["Flag"] != 1) &&
      (procInValMap["Flag"] != 2) &&
      (procInValMap["Flag"] != 4))
    procApp->message(61, vertexId.the_string() + " Flag");

  TF_Min = DBL_MIN;
  if (procInValMap.element("TF_Min"))
    TF_Min = procInValMap["TF_Min"];
  TF_Max = DBL_MAX;
  if (procInValMap.element("TF_Max"))
    TF_Max = procInValMap["TF_Max"];
  TR_Min = DBL_MIN;
  if (procInValMap.element("TR_Min"))
    TR_Min = procInValMap["TR_Min"];
  TR_Max = DBL_MAX;
  if (procInValMap.element("TR_Max"))
    TR_Max = procInValMap["TR_Max"];

  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);
  Symbol2 b = Symbol2("Fuel","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  Ex.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "CCogConst");
}


//// ~CCogConst
// Destructor
//
CCogConst::~CCogConst(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "CCogConst");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void CCogConst::actualExJ(const Map<Symbol,double>& pVecU)
{
  if ((procInValMap["Flag"] == 1) || (procInValMap["Flag"] == 2))
    {
      T_FEx = procInValMap["TF_0"];
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
    }
  if (procInValMap["Flag"] == 1)
    {
      T_REx = procInValMap["TR_0"];
      vecJ["0"]["Ex"]["Out"]["R"]["T"] = T_REx;
    }

}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void CCogConst::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                   double                    actualIntLength)
{
  // control of input

  if (procInValMap["Flag"] == 4)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
    }

  if ((procInValMap["Flag"] == 2) || (procInValMap["Flag"] == 4))
    {
      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
        T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");
    }

  // technical constraints

  if (procInValMap.element("DotEEl_0"))
    {
      if (procInValMap["DotEEl_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEEl_0");

      lessConstraintRhs[1] = procInValMap["DotEEl_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["El"]["0"] = 1;
    }

  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["El"]["0"] = -1.0/procInValMap["eta_El"];
  equalConstraintCoef[2]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[3] = 0;
  equalConstraintCoef[3]["Ex"]["H"]["0"] = 1;
  equalConstraintCoef[3]["En"]["Fuel"]["0"] = -procInValMap["eta_H"];

  // abnormal situations

  if ((T_FEx < 0 )     || (T_REx < 0)      ||
      (T_FEx <= T_REx) ||
      (T_FEx < TF_Min) || (T_FEx > TF_Max) ||
      (T_REx < TR_Min) || (T_REx > TR_Max))
    {
      equalConstraintRhs[4] = 0;
      equalConstraintCoef[4]["Ex"]["H"]["0"] = 1;
    }

  // emissions

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sVar")
            objectFuncCoef[part2]["En"]["Fuel"]["0"] =
              procInValMapIt.curr()->value;
        }
    }
}


//// showPower
//
Symbol CCogConst::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "El";
  showPowerNumber = "0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CBoiOfl
//
///////////////////////////////////////////////////////////////////

// Module author: Kathrin Ramsel
// Development thread: S05
//
// Summary: boiler with constant efficiency


//// CBoiOfl
// Standard Constructor

CBoiOfl::CBoiOfl(void)
{
  T_FEx =0;
  T_REx =0;
}

//// CBoiOfl
// Constructor

CBoiOfl::CBoiOfl(App* cProcApp, Symbol procId, Symbol cProcType,
	     ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  T_FEx =0;
  T_REx =0;

  if (!procInValMap.element("a"))
    procApp->message(55, vertexId.the_string()+ " a");
  if (!procInValMap.element("b"))
    procApp->message(55, vertexId.the_string()+ " b");
  if ((!procInValMap.element("lambdaEl_H")) && (!procInValMap.element("DotEEl_const")))
    procApp->message(55, vertexId.the_string()+ " lambdaEl_H/DotEEl_const");
  if ((procInValMap.element("lambdaEl_H")) && (procInValMap.element("DotEEl_const")))
    procApp->message(55, vertexId.the_string()+ " lambdaEl_H/DotEEl_const");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");
  if (((procInValMap["Flag"] == 1)  || (procInValMap["Flag"] == 2))
      && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string()+ " TF_0");
  if ((procInValMap["Flag"] == 1) && (!procInValMap.element("TR_0")))
    procApp->message(55, vertexId.the_string()+ " TR_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;
  if (procInValMap["a"] == 0 || procInValMap["Count"] == 0 )
    procApp->message(505, vertexId.the_string());
  if ((procInValMap["Flag"] != 1) &&
      (procInValMap["Flag"] != 2) &&
      (procInValMap["Flag"] != 4))
    procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("Fuel","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  Ex.insert(c);

 initState();

  if (procApp->testFlag)
    procApp->message(1001, "CBoiOfl");
}

//// ~CBoiOfl
// Destructor

CBoiOfl::~CBoiOfl(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
	procApp->message(1002, "CBoiOfl");
    }
}


////  updateProcOutTsPack
// update procOutTsPack after each optimization

void CBoiOfl::updateProcOutTsPack(double  actualIntLength, double intLength)
{
  // update state variable mean values, show DotEH_Ex

  String outputIdH;
  outputIdH = "H1_Ex";
  procOutTsPack[outputIdH]= procOutTsPack[outputIdH]+
    ((dotEEx["H"]["0"]) * (actualIntLength/intLength));
}


//// actualExJ
// actualize attributes of exit side

void CBoiOfl::actualExJ(const Map<Symbol,double> & pVecU)
{
  if ((procInValMap["Flag"] == 1) || (procInValMap["Flag"] == 2))
    {
      T_FEx  = procInValMap["TF_0"];
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
    }

  if (procInValMap["Flag"] == 1)
    {
      T_REx = procInValMap["TR_0"];
      vecJ["0"]["Ex"]["Out"]["R"]["T"] = T_REx;
    }
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void CBoiOfl::actualSimplexInput(const Map<Symbol,double> & pVecU,
			       double actualIntLength)
{
if (procInTsPack.element("on_off"))
  {
   if (procInTsPack["on_off"] != 0)
     {
//control of input

  if (procInValMap["Flag"] == 4)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
	T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
	procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][F][T]");
    }

  if ((procInValMap["Flag"] == 2) || (procInValMap["Flag"] == 4))
    {
      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
	T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
	procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");
    }


// technical constraints

  if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] <0)
	procApp->message(67, vertexId.the_string()+ " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["H"]["0"]= 1;
    }


  if (procInValMap.element("deltaDotEH"))
    {
      if (procInValMap["deltaDotEH"] <0)
	procApp->message(67, vertexId.the_string()+ " deltaDotEH");

      lessConstraintRhs[2] = procInValMap["deltaDotEH"] * procInValMap["Count"] + E_s[0];
      lessConstraintCoef[2]["Ex"]["H"]["0"]= 1;
    }


  if (procInValMap["b"] <0)
	procApp->message(67, vertexId.the_string()+ " b");

  equalConstraintRhs[3] = procInValMap["b"] * procInValMap["Count"];
  equalConstraintCoef[3]["Ex"]["H"]["0"]= - 1.0 * procInValMap["a"];
  equalConstraintCoef[3]["En"]["Fuel"]["0"]= 1;


  if (procInValMap.element("lambdaEl_H"))
  {
   equalConstraintRhs[4] = 0;
   equalConstraintCoef[4]["Ex"]["H"]["0"]= - procInValMap["lambdaEl_H"];
   equalConstraintCoef[4]["En"]["El"]["0"]= 1;
  }

  if(procInValMap.element("DotEEl_const"))
  {
   equalConstraintRhs[4] = procInValMap["DotEEl_const"] * procInValMap["Count"] ;
   equalConstraintCoef[4]["En"]["El"]["0"]= 1;
  }

  // abnormal situation
  if ((T_FEx <0 ) || (T_REx <0 ) ||(T_FEx <= T_REx))
    {
      equalConstraintRhs[5] = 0;
      equalConstraintCoef[5]["Ex"]["H"]["0"]= 1;
    }


  // emissions

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt;  procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
	{
	  String part1 = name(0,4);     //<String.h>, the first 4 char
	  String part2 = name(4);       //<String.h>, the rest
	  if (part1=="sVar")
	    objectFuncCoef[part2]["En"]["Fuel"]["0"]=
	      procInValMapIt.curr()->value;
	}
    }
}
else
  {
   equalConstraintRhs[1] = 0;
   equalConstraintCoef[1]["Ex"]["H"]["0"]= 1;

   equalConstraintRhs[3] = 0;
   equalConstraintCoef[3]["En"]["Fuel"]["0"]= 1;

   equalConstraintRhs[4] = 0;
   equalConstraintCoef[4]["En"]["El"]["0"]= 1;
  }
}
else
   procApp->message(58,vertexId.the_string()+" on_off");
}


//// initState
// set initial state variables
void CBoiOfl:: initState(void)
{
  // initialize state variable value and state variable map
  E_s[0] = procInValMap["DotEH_0"];
}


//// actualState
// actualize state variables (1 = change is not too large)

int CBoiOfl::actualState(double & actualIntLength,
	          const Map<Symbol,double> & pVecU)
{
      E_s[0] = dotEEx["H"]["0"];

 return 1;
}



//// showPower

Symbol CBoiOfl::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
  showPowerType="H";
  showPowerNumber="0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CBPTo
//
///////////////////////////////////////////////////////////////////

// Module author: Kathrin Ramsel
// Development thread: S05
//
// Summary: back-pressure turbine, combined heat and power station


//// CBPTo
// Standard Constructor

CBPTo::CBPTo(void)
{
}

//// CBPTo
// Constructor

CBPTo::CBPTo(App* cProcApp, Symbol procId, Symbol cProcType,
	       ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  if (!procInValMap.element("eta_Con"))
    procApp->message(55, vertexId.the_string()+ " eta_Con");
  if (!procInValMap.element("eta_G"))
    procApp->message(55, vertexId.the_string()+ " eta_G");
  if (!procInValMap.element("T_Con"))
    procApp->message(55, vertexId.the_string()+ " T_Con");
  if (!procInValMap.element("deltaT"))
    procApp->message(55, vertexId.the_string()+ " deltaT");
  if (!procInValMap.element("n"))
    procApp->message(55, vertexId.the_string()+ " n");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");
  if ((procInValMap["Flag"] == 2) && (!procInValMap.element("TF_Ex")))
    procApp->message(55, vertexId.the_string()+ " TF_Ex");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;
 if ((procInValMap["Flag"] == 4) && (!procInValMap.element("TF_En")))
      procApp->message(55, vertexId.the_string()+ " TF_En");

  if (procInValMap["eta_Con"] <= 0)
    procApp->message(505, vertexId.the_string());

  if ((procInValMap["Flag"] != 2) && (procInValMap["Flag"] != 4))
      procApp->message(61, vertexId.the_string() + " Flag");


  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);
  Symbol2 b = Symbol2("H","1");
  Ex.insert(b);
  Symbol2 c = Symbol2("H","0");
  En.insert(c);



  if (procApp->testFlag)
    procApp->message(1001, "CBPTo");
}

//// ~CBPTo
// Destructor

CBPTo::~CBPTo(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
	procApp->message(1002, "CBPTo");
    }
}


//// actualExJ
// actualize attributes of exit side

void CBPTo::actualExJ(const Map<Symbol,double> & pVecU)
{
  if (procInValMap["Flag"] == 2)
    vecJ["1"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"];
}


//// actualEnJ
// actualize attributes of entrance side

void CBPTo::actualEnJ(const Map<Symbol,double> & pVecU)
{
  {
   if (vecJ["1"]["Ex"]["In"]["R"].element("T"))
       vecJ["0"]["En"]["Out"]["R"]["T"] = vecJ["1"]["Ex"]["In"]["R"]["T"] ;
   else
       procApp->message(54,vertexId.the_string()+" vecJ[1][Ex][In][R][T]");
  }

   if (procInValMap["Flag"] == 4)
       vecJ["0"]["En"]["Out"]["F"]["T"] = procInValMap["TF_En"];
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void CBPTo::actualSimplexInput(const Map<Symbol,double> & pVecU,
			       double actualIntLength)
{
  double T_M   =0;
  double T_REx =0;
  double T_FEx =0;
  double tau   =0;

  //control of input

  if (vecJ["1"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["1"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string()+" vecJ[1][Ex][In][R][T]");

  if (procInValMap["Flag"] !=2)
    {
      if (vecJ["1"]["Ex"]["In"]["F"].element("T"))
	T_FEx = vecJ["1"]["Ex"]["In"]["F"]["T"];
      else
	procApp->message(54,vertexId.the_string()+" vecJ[1][Ex][In][F][T]");
    }
  else
    {
      T_FEx = procInValMap["TF_0"];  //Flag == 2
    }


  // calculate heat extracting (back pressure) temperature

  T_M = T_FEx +  procInValMap["deltaT"];
  T_M = T_M  - (T_FEx-T_REx)*double(procInValMap["n"]-1)/(2*procInValMap["n"]);


  // calculate electricity loss

  double x =0;
  double y =0;

  tau = (1- procInValMap["T_Con"]/T_M) * procInValMap["eta_G"];

  if (     (T_FEx <0 )       || (T_REx <0 ) ||(T_FEx <= T_REx)
      || (tau >= procInValMap["eta_Con"]))
    {
      equalConstraintRhs[5] = 0;
      equalConstraintCoef[5]["Ex"]["H"]["1"]= 1;
      x =1;
      y =1;
    }
  else
    {
      x = (1-tau)/(procInValMap["eta_Con"]- tau);
      y = (1- procInValMap["eta_Con"])/(procInValMap["eta_Con"]- tau);
    }

  // technical constraints

  if (procInValMap.element("DotEEl_0"))
    {
      if (procInValMap["DotEEl_0"] <0)
	procApp->message(67, vertexId.the_string()+ " DotEEl_0");

      lessConstraintRhs[1] = procInValMap["DotEEl_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["El"]["0"]= 1;
    }


  if (procInValMap.element("DotEH_En_0"))
    {
      if (procInValMap["DotEH_En_0"] <0)
	procApp->message(67, vertexId.the_string()+ " DotEH_En_0");

      lessConstraintRhs[2] = procInValMap["DotEH_En_0"]*procInValMap["Count"];
      lessConstraintCoef[2]["En"]["H"]["0"]= 1;
    }


  equalConstraintRhs[3] = 0;
  equalConstraintCoef[3]["Ex"]["El"]["0"]= - x;
  equalConstraintCoef[3]["En"]["H"]["0"]= 1;


  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["El"]["0"]= - y;
  equalConstraintCoef[4]["Ex"]["H"]["1"]= 1;

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt;  procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
	{
	  String part1 = name(0,4);     //<String.h>, the first 4 char
	  String part2 = name(4);       //<String.h>, the rest
	  if (part1=="sVar")
	    objectFuncCoef[part2]["En"]["H"]["0"]=
	      procInValMapIt.curr()->value;
	}
    }
}


//// showPower

Symbol CBPTo::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
  showPowerType="El";
  showPowerNumber="0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CECTo
//
///////////////////////////////////////////////////////////////////

// Module author: Kathrin Ramsel
// Development thread: S05
//
// Summary: Extraction-condensing turbine, combined heat and power station
// without boiler (energy flow on entrance side due to steam input)


//// CECTo
// Standard Constructor

CECTo::CECTo(void)
{
TF_Min=0;
TF_Max=0;
TR_Min=0;
TR_Max=0;
}

//// CECTo
// Constructor

CECTo::CECTo(App* cProcApp, Symbol procId, Symbol cProcType,
	       ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  if (!procInValMap.element("a"))
    procApp->message(55, vertexId.the_string()+ " a");
  if (!procInValMap.element("b"))
    procApp->message(55, vertexId.the_string()+ " b");
  if (!procInValMap.element("c"))
    procApp->message(55, vertexId.the_string()+ " c");
  if (!procInValMap.element("g_H"))
    procApp->message(55, vertexId.the_string()+ " g_H");
  if (!procInValMap.element("c_BP"))
    procApp->message(55, vertexId.the_string()+ " c_BP");
  if (!procInValMap.element("f"))
    procApp->message(55, vertexId.the_string()+ " f");
  if (procInValMap.element("sVarM_FI") && (!procInValMap.element("h_1")))
    procApp->message(55, vertexId.the_string()+ " h_1");
  if (procInValMap.element("sVarM_FI") && (!procInValMap.element("h_2")))
    procApp->message(55, vertexId.the_string()+ " h_2");
  if (procInValMap.element("sVarM_FI") && (!procInValMap.element("Delta_h_HD")))
    procApp->message(55, vertexId.the_string()+ " Delta_h_HD");
  if (procInValMap.element("sVarM_FI") && (!procInValMap.element("eta_m")))
    procApp->message(55, vertexId.the_string()+ " eta_m");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");
  if (((procInValMap["Flag"] == 22) || (procInValMap["Flag"] == 42)) &&
                                      (!procInValMap.element("TF_Ex")))
    procApp->message(55, vertexId.the_string()+ " TF_Ex");
  if (((procInValMap["Flag"] == 44) || (procInValMap["Flag"] == 42)) &&
                                      (!procInValMap.element("TF_En")))
    procApp->message(55, vertexId.the_string()+ " TF_En");
  if (procInValMap.element("d_FB")  && (!procInValMap.element("TF_Ex")))
    procApp->message(55, vertexId.the_string()+ " TF_Ex");
  if (procInValMap.element("d_FG")  && (!procInValMap.element("TF_Ex")))
    procApp->message(55, vertexId.the_string()+ " TF_Ex");
  if (procInValMap.element("d_FH")  && (!procInValMap.element("TF_Ex")))
    procApp->message(55, vertexId.the_string()+ " TF_Ex");
  if (procInValMap.element("d_FH")  && (!procInValMap.element("DotEH_0")))
    procApp->message(55, vertexId.the_string()+ " DotEH_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;
  if ((procInValMap["Flag"] != 22) &&
      (procInValMap["Flag"] != 42) &&
      (procInValMap["Flag"] != 44))
      procApp->message(61, vertexId.the_string() + " Flag");

  TF_Min = DBL_MIN;
  if (procInValMap.element("TF_Min"))
    TF_Min =  procInValMap["TF_Min"];
  TF_Max = DBL_MAX;
  if (procInValMap.element("TF_Max"))
    TF_Max =  procInValMap["TF_Max"];
  TR_Min = DBL_MIN;
  if (procInValMap.element("TR_Min"))
    TR_Min =  procInValMap["TR_Min"];
  TR_Max = DBL_MAX;
  if (procInValMap.element("TR_Max"))
    TR_Max =  procInValMap["TR_Max"];

  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);
  Symbol2 b = Symbol2("H","0");
  Ex.insert(b);
  Symbol2 c = Symbol2("H","0");
  En.insert(c);
  Symbol2 d = Symbol2("El","0");
  En.insert(d);

  if (procApp->testFlag)
    procApp->message(1001, "CECTo");
}

//// ~CECTo
// Destructor

CECTo::~CECTo(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
	procApp->message(1002, "CECTo");
    }
}

////  updateProcOutTsPack
// update procOutTsPack after each optimization

void CECTo::updateProcOutTsPack(double  actualIntLength, double intLength)
{
  // update variable mean values

  String outputIdEl;
  outputIdEl = "El_Ex";
  procOutTsPack[outputIdEl]= procOutTsPack[outputIdEl]+
    ((dotEEx["El"]["0"]) * (actualIntLength/intLength));

  String outputIdH;
  outputIdH = "H1_Ex";
  procOutTsPack[outputIdH]= procOutTsPack[outputIdH]+
    ((dotEEx["H"]["0"]) * (actualIntLength/intLength));

}

//// actualExJ
// actualize attributes of exit side

void CECTo::actualExJ(const Map<Symbol,double> & pVecU)
{
  if ((procInValMap["Flag"] == 22) || (procInValMap["Flag"] == 42))
    vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_Ex"];
}

//// actualEnJ
// actualize attributes of entrance side

void CECTo::actualEnJ(const Map<Symbol,double> & pVecU)
{
  {
   if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
       vecJ["0"]["En"]["Out"]["R"]["T"] = vecJ["0"]["Ex"]["In"]["R"]["T"] ;
   else
       procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");
  }

   if ((procInValMap["Flag"] == 44) || (procInValMap["Flag"] == 42))
       vecJ["0"]["En"]["Out"]["F"]["T"] = procInValMap["TF_En"];
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void CECTo::actualSimplexInput(const Map<Symbol,double> & pVecU,
			       double actualIntLength)
{
if (procInTsPack.element("on_off"))
  {
   if (procInTsPack["on_off"] != 0)
     {
  double T_REx   = 0;
  double T_FEx   = 0;
  double g_H     = 0;
  double b       = 0;
  double DotEH_0 = 0;
  double TF_0    = 0;

  //control of input

  if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");

  if (procInValMap["Flag"] == 44)
      {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
         T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
         procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][F][T]");
      }
  else
    {
      T_FEx = procInValMap["TF_Ex"];  //Flag == 22 or 42
    }

  TF_0 = procInValMap["TF_Ex"];


  // calculate temperature-dependent slope of back-pressure line

  g_H = procInValMap["g_H"];
  g_H = g_H + procInValMap["d_FG"]*(T_FEx-TF_0);

  // calculate temperature-dependent b

  b = procInValMap["b"];
  b = b + procInValMap["d_FB"]*(T_FEx-TF_0);


  // technical constraints

  if (procInValMap.element("DotEH_En_0"))
    {
      if (procInValMap["DotEH_En_0"] <0)
	procApp->message(67, vertexId.the_string()+ " DotEH_En_0");

      lessConstraintRhs[1] = procInValMap["DotEH_En_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["En"]["H"]["0"]= 1;
    }

  if (procInValMap.element("DotEEl_0"))
    {
      if (procInValMap["DotEEl_0"] <0)
	procApp->message(67, vertexId.the_string()+ " DotEEl_0");

      lessConstraintRhs[2] = procInValMap["DotEEl_0"] * procInValMap["Count"];
      lessConstraintCoef[2]["Ex"]["El"]["0"]= 1;
    }

  if (procInValMap.element("DotEEl_Min"))
    {
      if (procInValMap["DotEEl_Min"] <0)
	procApp->message(67, vertexId.the_string()+ " DotEEl_Min");

      greaterConstraintRhs[3] = procInValMap["DotEEl_Min"] * procInValMap["Count"];
      greaterConstraintCoef[3]["Ex"]["El"]["0"]= 1;
    }

  if (procInValMap.element("c_LP"))
    {
      if (procInValMap["c_LP"] <0)
	procApp->message(67, vertexId.the_string()+ " c_LP");

      lessConstraintRhs[4] = procInValMap["c_LP"]*procInValMap["Count"];
      lessConstraintCoef[4]["Ex"]["El"]["0"]= 1;
      lessConstraintCoef[4]["Ex"]["H"]["0"]= - g_H;
    }

  if (procInValMap.element("n") && procInValMap.element("m"))
    {
      if (procInValMap["n"] <0)
	procApp->message(67, vertexId.the_string()+ " n");

      lessConstraintRhs[5] = procInValMap["n"]*procInValMap["Count"];
      lessConstraintCoef[5]["Ex"]["El"]["0"]= 1;
      lessConstraintCoef[5]["Ex"]["H"]["0"]= - procInValMap["m"];
    }

  if (procInValMap["c_BP"] <0)
    procApp->message(67, vertexId.the_string()+ " c_BP");

      lessConstraintRhs[6] = procInValMap["c_BP"]*procInValMap["Count"];
      lessConstraintCoef[6]["Ex"]["El"]["0"]= - 1;
      lessConstraintCoef[6]["Ex"]["H"]["0"]= g_H;

  if (procInValMap["f"] <0)
    procApp->message(67, vertexId.the_string()+ " f");

      lessConstraintRhs[7] = 0;
      lessConstraintCoef[7]["Ex"]["El"]["0"]= - 1;
      lessConstraintCoef[7]["Ex"]["H"]["0"]= procInValMap["f"];

  if (procInValMap.element("DotEH_0"))
    {
      DotEH_0 =  procInValMap["DotEH_0"];
      DotEH_0 =  DotEH_0 + procInValMap["d_FH"]*(T_FEx-TF_0);

      if (DotEH_0 <0)
	DotEH_0 =0;

      lessConstraintRhs[8] = DotEH_0 * procInValMap["Count"];
      lessConstraintCoef[8]["Ex"]["H"]["0"]= 1;
    }

  if(procInValMap.element("DotEEl_const"))
    {
     if (procInValMap["DotEEl_const"] < 0)
         procApp->message(67, vertexId.the_string()+ " DotEEl_const");

     equalConstraintRhs[9] = procInValMap["DotEEl_const"] * procInValMap["Count"];
     equalConstraintCoef[9]["En"]["El"]["0"]= 1;
    }
  else
     equalConstraintRhs[9] = 0;
     equalConstraintCoef[9]["En"]["El"]["0"]= 1;

  // enthalpy consumption

  equalConstraintRhs[10] = procInValMap["c"] * procInValMap["Count"];
  equalConstraintCoef[10]["Ex"]["El"]["0"]= -  procInValMap["a"];
  equalConstraintCoef[10]["Ex"]["H"]["0"]= - b;
  equalConstraintCoef[10]["En"]["H"]["0"]= 1;

  // abnormal situations

  if (   (T_FEx < 0 )       || (T_REx < 0 )      ||(T_FEx <= T_REx)
      || (T_FEx < TF_Min)   || (T_FEx > TF_Max)
      || (T_REx < TR_Min)   || (T_REx > TR_Max)
      || (b     <= 0)       || (g_H <= 0) )
    {
      equalConstraintRhs[11] = 0;
      equalConstraintCoef[11]["Ex"]["H"]["0"]= 1;
    }


  // cogeneration feed-in law refund

  if(procInValMap.element("sVarM_FI"))
            {
             objectFuncCoef["M"]["Ex"]["H"]["0"]= -((procInValMap["h_1"]-procInValMap["h_2"])/
			     procInValMap["Delta_h_HD"])*procInValMap["eta_m"]*
                                                     procInValMap["sVarM_FI"];
            }
  if(procInValMap.element("sVarM"))
            {
             objectFuncCoef["M"]["En"]["H"]["0"]= procInValMap["sVarM"];
            }

}
else
  {
   equalConstraintRhs[1] = 0;
   equalConstraintCoef[1]["En"]["H"]["0"]= 1;

   equalConstraintRhs[2] = 0;
   equalConstraintCoef[2]["Ex"]["El"]["0"]= 1;

   equalConstraintRhs[8] = 0;
   equalConstraintCoef[8]["Ex"]["H"]["0"]= 1;

   equalConstraintRhs[9] = 0;
   equalConstraintCoef[9]["En"]["El"]["0"]= 1;
  }
}
else
   procApp->message(58,vertexId.the_string()+" on_off");
}


//// showPower

Symbol CECTo::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
  showPowerType="El";
  showPowerNumber="0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CGasTHR
//
///////////////////////////////////////////////////////////////////

// Module author: Kathrin Ramsel
// Development thread: S05
//
// Summary: gasturbine, with HRSG


//// CGasTHR
// Standard Constructor

CGasTHR::CGasTHR(void)
{
TF_Min=0;
TF_Max=0;
TR_Min=0;
TR_Max=0;
}

//// CGasTHR
// Constructor

CGasTHR::CGasTHR(App* cProcApp, Symbol procId, Symbol cProcType,
	       ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  if (!procInValMap.element("DotEEl_N"))
    procApp->message(55, vertexId.the_string()+ " DotEEl_N");
  if (!procInValMap.element("e_0"))
    procApp->message(55, vertexId.the_string()+ " e_0");
  if (!procInValMap.element("e_1"))
    procApp->message(55, vertexId.the_string()+ " e_1");
  if (!procInValMap.element("e_2"))
    procApp->message(55, vertexId.the_string()+ " e_2");
  if (!procInValMap.element("e_3"))
    procApp->message(55, vertexId.the_string()+ " e_3");

  if (!procInValMap.element("a_N"))
    procApp->message(55, vertexId.the_string()+ " a_N");
  if (!procInValMap.element("b_N"))
    procApp->message(55, vertexId.the_string()+ " b_N");
  if (!procInValMap.element("a_0"))
    procApp->message(55, vertexId.the_string()+ " a_0");
  if (!procInValMap.element("a_1"))
    procApp->message(55, vertexId.the_string()+ " a_1");
  if (!procInValMap.element("a_2"))
    procApp->message(55, vertexId.the_string()+ " a_2");
  if (!procInValMap.element("a_3"))
    procApp->message(55, vertexId.the_string()+ " a_3");
  if (!procInValMap.element("b_0"))
    procApp->message(55, vertexId.the_string()+ " b_0");
  if (!procInValMap.element("b_1"))
    procApp->message(55, vertexId.the_string()+ " b_1");
  if (!procInValMap.element("b_2"))
    procApp->message(55, vertexId.the_string()+ " b_2");
  if (!procInValMap.element("b_3"))
    procApp->message(55, vertexId.the_string()+ " b_3");

  if (!procInValMap.element("T_RG_N"))
    procApp->message(55, vertexId.the_string()+ " T_RG_N");
  if (!procInValMap.element("g_0"))
    procApp->message(55, vertexId.the_string()+ " g_0");
  if (!procInValMap.element("g_1"))
    procApp->message(55, vertexId.the_string()+ " g_1");
  if (!procInValMap.element("g_2"))
    procApp->message(55, vertexId.the_string()+ " g_2");
  if (!procInValMap.element("g_3"))
    procApp->message(55, vertexId.the_string()+ " g_3");

  if (!procInValMap.element("eta_ZF"))
    procApp->message(55, vertexId.the_string()+ " eta_ZF");
  if (!procInValMap.element("c_ZF"))
    procApp->message(55, vertexId.the_string()+ " c_ZF");
  if (!procInValMap.element("DotEZF_Max"))
    procApp->message(55, vertexId.the_string()+ " DotEZF_Max");

  if (!procInValMap.element("Dotm_RG_N"))
    procApp->message(55, vertexId.the_string()+ " Dotm_RG_N");
  if (!procInValMap.element("m_0"))
    procApp->message(55, vertexId.the_string()+ " m_0");
  if (!procInValMap.element("m_1"))
    procApp->message(55, vertexId.the_string()+ " m_1");
  if (!procInValMap.element("m_2"))
    procApp->message(55, vertexId.the_string()+ " m_2");
  if (!procInValMap.element("m_3"))
    procApp->message(55, vertexId.the_string()+ " m_3");

  if (!procInValMap.element("h_W1"))
    procApp->message(55, vertexId.the_string()+ " h_W1");
  if (!procInValMap.element("h_W2"))
    procApp->message(55, vertexId.the_string()+ " h_W2");
  if (!procInValMap.element("h_W4"))
    procApp->message(55, vertexId.the_string()+ " h_W4");
  if (!procInValMap.element("T_LV"))
    procApp->message(55, vertexId.the_string()+ " T_LV");
  if (!procInValMap.element("cp_RG"))
    procApp->message(55, vertexId.the_string()+ " cp_RG");

  if (!procInValMap.element("deltaT_min"))
    procApp->message(55, vertexId.the_string()+ " deltaT_min");
  if (!procInValMap.element("deltaT_pinch"))
    procApp->message(55, vertexId.the_string()+ " deltaT_pinch");

  if (procInValMap.element("sVarM_FI") && (!procInValMap.element("R_FI")))
    procApp->message(55, vertexId.the_string()+ " R_FI");

  if (!procInValMap.element("T_N"))
    procApp->message(55, vertexId.the_string()+ " T_N");

  TF_Min = DBL_MIN;
  if (procInValMap.element("TF_Min"))
    TF_Min =  procInValMap["TF_Min"];
  TF_Max = DBL_MAX;
  if (procInValMap.element("TF_Max"))
    TF_Max =  procInValMap["TF_Max"];
  TR_Min = DBL_MIN;
  if (procInValMap.element("TR_Min"))
    TR_Min =  procInValMap["TR_Min"];
  TR_Max = DBL_MAX;
  if (procInValMap.element("TR_Max"))
    TR_Max =  procInValMap["TR_Max"];

  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);
  Symbol2 b = Symbol2("H","1");
  Ex.insert(b);
  Symbol2 c = Symbol2("H","2");
  Ex.insert(c);
  Symbol2 d = Symbol2("Fuel","0");
  En.insert(d);

  if (procApp->testFlag)
    procApp->message(1001, "CGasTHR");
}

//// ~CGasTHR
// Destructor

CGasTHR::~CGasTHR(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
	procApp->message(1002, "CGasTHR");
    }
}

////  updateProcOutTsPack
// update procOutTsPack after each optimization

void CGasTHR::updateProcOutTsPack(double  actualIntLength, double intLength)
{
  // update state variable mean values

  String outputIdEl;
  outputIdEl = "El_Ex";
  procOutTsPack[outputIdEl]= procOutTsPack[outputIdEl]+
    ((dotEEx["El"]["0"]) * (actualIntLength/intLength));

  String outputIdH1;
  outputIdH1 = "H1_Ex";
  procOutTsPack[outputIdH1]= procOutTsPack[outputIdH1]+
    ((dotEEx["H"]["1"]) * (actualIntLength/intLength));

  String outputIdH2;
  outputIdH2 = "H2_Ex";
  procOutTsPack[outputIdH2]= procOutTsPack[outputIdH2]+
    ((dotEEx["H"]["2"]) * (actualIntLength/intLength));
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void CGasTHR::actualSimplexInput(const Map<Symbol,double> & pVecU,
			       double actualIntLength)
{
if (procInTsPack.element("on_off"))
  {
   if (procInTsPack["on_off"] != 0)
     {
  double T_FEx_1    = 0;
  double T_FEx_2    = 0;
  double Dotm_RG    = 0;
  double T_RG       = 0;
  double T_RG_NEU   = 0;
  double deltaT_ZF  = 0;
  double DotEH_Max  = 0;
  double DotEEl_Max = 0;
  double DeltaT     = 0;
  double Fuel_El    = 0;

//control of input
  if (vecJ["1"]["Ex"]["In"]["F"].element("T"))
     T_FEx_1 = vecJ["1"]["Ex"]["In"]["F"]["T"];
  else
    procApp->message(54,vertexId.the_string()+" vecJ[1][Ex][In][F][T]");

  if (vecJ["2"]["Ex"]["In"]["F"].element("T"))
     T_FEx_2 = vecJ["2"]["Ex"]["In"]["F"]["T"];
  else
    procApp->message(54,vertexId.the_string()+" vecJ[2][Ex][In][F][T]");

  if (T_FEx_1 != T_FEx_2)
    procApp->message(73,vertexId.the_string()+" dotE_H_Ex_1, dotE_H_Ex_2");

// Nebenrechnungen
     DeltaT = pVecU["T"] - procInValMap["T_N"];

 // flue gas temperature
    T_RG = procInValMap["T_RG_N"] * (procInValMap["g_0"] +
                                    (procInValMap["g_1"]*DeltaT) +
                                    (procInValMap["g_2"]*DeltaT*DeltaT) +
                                    (procInValMap["g_3"]*DeltaT*DeltaT*DeltaT));

 // flue gas mass stream
    Dotm_RG = procInValMap["Dotm_RG_N"] * (procInValMap["m_0"] +
                                          (procInValMap["m_1"]*DeltaT) +
	                          (procInValMap["m_2"]*DeltaT*DeltaT) +
                                          (procInValMap["m_3"]*DeltaT*DeltaT*DeltaT));

 // gas stream temperature after upgrading
    if (T_RG <= (T_FEx_1 + procInValMap["deltaT_min"]))
      {
       T_RG_NEU = T_FEx_1 + procInValMap["deltaT_min"];
       deltaT_ZF = T_RG_NEU - T_RG;
       T_RG = T_RG_NEU;
      }

// technical constraints
 // NB 1
  if (procInValMap.element("DotEFuel_0"))
    {
     if (procInValMap["DotEFuel_0"] <0)
 	procApp->message(67, vertexId.the_string()+ " DotEFuel_0");

     lessConstraintRhs[1] = procInValMap["DotEFuel_0"];
     lessConstraintCoef[1]["En"]["Fuel"]["0"]= 1;
    }

 // NB 2
  if (procInValMap.element("DotEEl_0"))
    {
     if(procInValMap["DotEEl_0"]<0)
                procApp->message(67, vertexId.the_string()+ " DotEEl_0");

     lessConstraintRhs[2] = procInValMap["DotEEl_0"];
     lessConstraintCoef[2]["Ex"]["El"]["0"]= 1;
    }

 // NB 3
  if (procInValMap["DotEEl_N"] <0)
      procApp->message(67, vertexId.the_string()+ " DotEEl_N");

  DotEEl_Max = procInValMap["DotEEl_N"] * (procInValMap["e_0"] +
                                          (procInValMap["e_1"] *DeltaT) +
                                          (procInValMap["e_2"] *DeltaT*DeltaT) +
                                          (procInValMap["e_3"] *DeltaT*DeltaT*DeltaT));

  if (DotEEl_Max < 0)
     procApp->message(67, vertexId.the_string()+ " DotEEl_Max");

  lessConstraintRhs[3] = DotEEl_Max;
  lessConstraintCoef[3]["Ex"]["El"]["0"]= 1;


// Maximum Output of Heat Recovery Steam Generator

   DotEH_Max = Dotm_RG * procInValMap["cp_RG"] *
                (T_RG-procInValMap["T_LV"]-procInValMap["deltaT_pinch"])/
                (procInValMap["h_W4"] - procInValMap["h_W2"]) *
                (procInValMap["h_W4"] - procInValMap["h_W1"]);

   if (DotEH_Max < 0)
 	procApp->message(67, vertexId.the_string()+ " DotEH_Max");

 // NB4
   lessConstraintRhs[4] = 0;
   lessConstraintCoef[4]["Ex"]["El"]["0"]= -(DotEH_Max / DotEEl_Max);
   lessConstraintCoef[4]["Ex"]["H"]["1"]= 1;


//second additional heating
 //NB 5
   lessConstraintRhs[5] = procInValMap["DotEZF_Max"];
   lessConstraintCoef[5]["Ex"]["H"]["2"]= 1;

 //NB 6 *MIP*
   lessConstraintRhs[6] = 0;
   lessConstraintCoef[6]["Ex"]["H"]["2"]= 1;
   lessConstraintCoef[6]["Ex"]["El"]["0"]= -((1.0 / DotEEl_Max)*
                                            (procInValMap["DotEZF_Max"]/procInValMap["c_ZF"]));

 //NB 7
   if (procInValMap["b_N"] <0)
	procApp->message(67, vertexId.the_string()+ " b_N");

   Fuel_El = procInValMap["b_N"]*(procInValMap["b_0"]+
                                 (procInValMap["b_1"] *DeltaT)+
	                 (procInValMap["b_2"] *DeltaT*DeltaT)+
                	 (procInValMap["b_3"] *DeltaT*DeltaT*DeltaT));
   if(Fuel_El < 0)
     procApp->message(67, vertexId.the_string()+ " Fuel_El");

   equalConstraintRhs[7] = procInValMap["b_N"]*(procInValMap["b_0"] +
                                               (procInValMap["b_1"] *DeltaT) +
	                               (procInValMap["b_2"] *DeltaT*DeltaT) +
                	               (procInValMap["b_3"] *DeltaT*DeltaT*DeltaT))+
                           (Dotm_RG * procInValMap["cp_RG"] * deltaT_ZF / procInValMap["eta_ZF"]);
   equalConstraintCoef[7]["Ex"]["H"]["2"] = - 1.0 / procInValMap["eta_ZF"];
   equalConstraintCoef[7]["Ex"]["El"]["0"] = - 1.0 * procInValMap["a_N"] *
			    (procInValMap["a_0"] +
                                                    (procInValMap["a_1"] *DeltaT) +
	                                    (procInValMap["a_2"] *DeltaT*DeltaT) +
                	                    (procInValMap["a_3"] *DeltaT*DeltaT*DeltaT));
   equalConstraintCoef[7]["En"]["Fuel"]["0"] = 1;


  // abnormal situations

  if (   (T_FEx_1 < 0 )       || (T_FEx_1 <= vecJ["0"]["Ex"]["In"]["R"]["T"]) ||
         (T_FEx_1 < TF_Min)   || (T_FEx_1 > TF_Max))
    {
      equalConstraintRhs[8] = 0;
      equalConstraintCoef[8]["Ex"]["H"]["1"]= 1;
      equalConstraintRhs[9] = 0;
      equalConstraintCoef[9]["Ex"]["H"]["2"]= 1;
    }

 // emissions and cogeneration feed-in law refund

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt;  procInValMapIt.next())
    {
     String name = procInValMapIt.curr()->key.the_string();
     if (name.length() > 4)
       {
        String part1 = name(0,4);     //<String.h>, the first 4 char
        String part2 = name(4);       //<String.h>, the rest
        if (part1=="sVar")
          {
           if (part2=="M_FI")
           objectFuncCoef["M"]["Ex"]["El"]["0"]= - procInValMap["R_FI"] * procInValMapIt.curr()->value;
           else
           objectFuncCoef[part2]["En"]["Fuel"]["0"]= procInValMapIt.curr()->value;
          }
       }
    }
}
else
  {
   equalConstraintRhs[1] = 0;
   equalConstraintCoef[1]["En"]["Fuel"]["0"]= 1;

   equalConstraintRhs[2] = 0;
   equalConstraintCoef[2]["Ex"]["El"]["0"]= 1;

   equalConstraintRhs[4] = 0;
   equalConstraintCoef[4]["Ex"]["H"]["1"]= 1;

   equalConstraintRhs[5] = 0;
   equalConstraintCoef[5]["Ex"]["H"]["2"]= 1;
  }
}
else
   procApp->message(58,vertexId.the_string()+" on_off");
}

//// showPower
Symbol CGasTHR::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
  showPowerType="El";
  showPowerNumber="0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: CFCellConst
//
///////////////////////////////////////////////////////////////////

// Module author: Jan Heise
// Development thread: S07
//
// Summary: fuelcell unit with constant efficiencies


//// CFCellConst
// Standard Constructor

CFCellConst::CFCellConst(void)
{
  T_FEx =0;
  T_REx =0;
  TF_Min=0;
  TF_Max=0;
  TR_Min=0;
  TR_Max=0;
  epsilon_hc=0;
}

//// CFCellConst
// Constructor

CFCellConst::CFCellConst(App* cProcApp, Symbol procId, Symbol cProcType,
	         ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  T_FEx =0.0;
  T_REx =0.0;

  if (!procInValMap.element("eta_ElN"))
    procApp->message(55, vertexId.the_string()+ " eta_ElN");
  if (!procInValMap.element("sigma_id"))
    procApp->message(55, vertexId.the_string()+ " sigma_id");
  if (!procInValMap.element("DotEEl_0"))
    procApp->message(55, vertexId.the_string()+ " DotEEl_0");
  if (!procInValMap.element("DotEEl_min"))
    procApp->message(55, vertexId.the_string()+ " DotEEl_min");
  if (!procInValMap.element("a"))
    procApp->message(55, vertexId.the_string()+ " a");
  if (!procInValMap.element("b"))
    procApp->message(55, vertexId.the_string()+ " b");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");
  if ((procInValMap["Flag"] != 1) &&
      (procInValMap["Flag"] != 2) &&
      (procInValMap["Flag"] != 4))
    procApp->message(61, vertexId.the_string() + " Flag");
  if (((procInValMap["Flag"] == 1)  || (procInValMap["Flag"] == 2))
      && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string()+ " TF_0");
  if ((procInValMap["Flag"] == 1) && (!procInValMap.element("TR_0")))
    procApp->message(55, vertexId.the_string()+ " TR_0");
  if (((procInValMap["Flag"] == 2) || (procInValMap["Flag"] == 4)) &&
      (!procInValMap.element("DeltaT_m")))
    procApp->message(55, vertexId.the_string()+ " DeltaT_m");
  if (((procInValMap["Flag"] == 2) || (procInValMap["Flag"] == 4)) &&
      (!procInValMap.element("TFsec_0")))
    procApp->message(55, vertexId.the_string()+ " TFsec_0");
  if (((procInValMap["Flag"] == 2) || (procInValMap["Flag"] == 4)) &&
      (!procInValMap.element("TRsec_0")))
    procApp->message(55, vertexId.the_string()+ " TRsec_0");
  if ((procInValMap["Flag"] == 4) && (!procInValMap.element("TF_Max")))
    procApp->message(55, vertexId.the_string()+ " TF_Max");
  if (!procInValMap.element("powerleadFlag"))
	procApp->message(55, vertexId.the_string()+ " powerleadFlag");
  if ((procInValMap["powerleadFlag"] != 0) &&
	  (procInValMap["powerleadFlag"] != 1))
  	procApp->message(61, vertexId.the_string() + " powerleadFlag");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;
  if (procInValMap["DotEEl_0"] <0)
    procApp->message(67, vertexId.the_string()+ " DotEEl_0");
  if (procInValMap["DotEEl_min"] <0)
    procApp->message(67, vertexId.the_string()+ " DotEEl_min");
  if (procInValMap["b"] <0)
    procApp->message(67, vertexId.the_string()+ " b");

  if (procInValMap["eta_ElN"] == 0)
    procApp->message(505, vertexId.the_string());


  TF_Min = DBL_MIN;
  if (procInValMap.element("TF_Min"))
    TF_Min =  procInValMap["TF_Min"];
  TF_Max = DBL_MAX;
  if (procInValMap.element("TF_Max"))
    TF_Max =  procInValMap["TF_Max"];
  TR_Min = DBL_MIN;
  if (procInValMap.element("TR_Min"))
    TR_Min =  procInValMap["TR_Min"];
  TR_Max = DBL_MAX;
  if (procInValMap.element("TR_Max"))
    TR_Max =  procInValMap["TR_Max"];

  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);
  Symbol2 b = Symbol2("Fuel","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  Ex.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "CFCellConst");
}

//// ~CFCellConst
// Destructor

CFCellConst::~CFCellConst(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
	procApp->message(1002, "CFCellConst");
    }
}


//// actualExJ
// actualize attributes of exit side

void CFCellConst::actualExJ(const Map<Symbol,double> & pVecU)
{
  if (procInValMap["Flag"] == 1)
    {
      T_FEx  = procInValMap["TF_0"];
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;

      T_REx = procInValMap["TR_0"];
      vecJ["0"]["Ex"]["Out"]["R"]["T"] = T_REx;
    }


  if (procInValMap["Flag"] == 2)
    {
      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
	T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
	procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");

      T_FEx  = procInValMap["TF_0"];
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
    }

}


//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void CFCellConst::actualSimplexInput(const Map<Symbol,double> & pVecU,
			       double actualIntLength)
{
  // variables

  double TR_sec = 0.0;
  double coef   = 0.0;         // coefficient from dotEEl in fourth constraint
  double help   = 0.0;         // helps calculating coef
  double epsilon_sec=0.0;      // needed for calculating TR_sec
  double lowerBound=0;         // lower boundary of 'epsilon_sec' in rtsafe
  double upperBound=1;         // upper boundary of 'epsilon_sec' in rtsafe
  double Acc=0.0001;           // accuracy for Newton-Raphson-algorithem

  //control of input

  if (procInValMap["Flag"] == 4)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
	T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
	procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][F][T]");
    }

  if ((procInValMap["Flag"] == 2) || (procInValMap["Flag"] == 4))
    {
      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
	T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
	procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");
    }


  // technical constraints


  lessConstraintRhs[1] = procInValMap["DotEEl_0"] * procInValMap["Count"];
  lessConstraintCoef[1]["Ex"]["El"]["0"]= 1;


  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["El"]["0"]= - 1.0/procInValMap["eta_ElN"];
  equalConstraintCoef[2]["En"]["Fuel"]["0"]= 1;


  greaterConstraintRhs[3] = procInValMap["DotEEl_min"] * procInValMap["Count"];
  greaterConstraintCoef[3]["Ex"]["El"]["0"]= 1;


  // abnormal situations

  if (   (T_FEx < 0 )       || (T_REx < 0 )      || (T_FEx <= T_REx)
      || (T_FEx < TF_Min)   || (T_FEx > TF_Max)
      || (T_REx < TR_Min)   || (T_REx > TR_Max))
    {
      equalConstraintRhs[4] = 0;
      equalConstraintCoef[4]["Ex"]["H"]["0"]= 1;
    }
  else // normal situation
    {
	 // calculat temprature TR_sec needed in fourth constraint

    if (procInValMap["Flag"] == 1)
      TR_sec = procInValMap["TRsec_0"];

    if ((procInValMap["Flag"] == 2) || (procInValMap["Flag"] == 4))
      {
        epsilon_hc = (T_FEx - T_REx)/(procInValMap["TFsec_0"] - T_REx);
		  if ((epsilon_hc < 0) || (epsilon_hc>=1))
			procApp->message(70, vertexId.the_string()+ " epsilon_hc");

	    epsilon_sec = rtsafe(hxfunc, lowerBound, upperBound, Acc);
        TR_sec  = procInValMap["TFsec_0"] - epsilon_sec *
							(procInValMap["TFsec_0"] - T_REx);
    if (TR_sec < 303.15)
	  TR_sec = 303.15; // Begrenzung des Negativen WHD im Kondensators
      }

    if ((TR_sec <= T_REx) || (TR_sec <= 0))
    	procApp->message(70, vertexId.the_string()+ " TR_sec");

  // calculation of coefficient from dotEEl in fourth constraint

  	coef = (1 - procInValMap["a"])/procInValMap["eta_ElN"];
  	if (coef <= 0)
    	procApp->message(65,vertexId.the_string()+" actualSimplexInput, coef");
  	help = procInValMap["TFsec_0"] - procInValMap["TRsec_0"];
  	help = (TR_sec - procInValMap["TRsec_0"])/help;
  	help = help/procInValMap["sigma_id"] + 1;
  	if (help <= 0)
    	procApp->message(65,vertexId.the_string()+" actualSimplexInput, help");
  	coef = coef - help;
  	if (coef <= 0)
    	procApp->message(65,vertexId.the_string()+" actualSimplexInput, coef or help");

  // fourth constraint normal situation

	if (procInValMap["powerleadFlag"] == 0)
      	{
		equalConstraintRhs[4] = procInValMap["b"] * procInValMap["Count"];
      	equalConstraintCoef[4]["Ex"]["El"]["0"]= coef;
      	equalConstraintCoef[4]["Ex"]["H"]["0"]= -1;
		}
	else // heat dumping when only power is needed
		{
		greaterConstraintRhs[4] = procInValMap["b"] * procInValMap["Count"];
      	greaterConstraintCoef[4]["Ex"]["El"]["0"]= coef;
      	greaterConstraintCoef[4]["Ex"]["H"]["0"]= -1;
		}
    }

  // emissions

  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt;  procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
	{
	  String part1 = name(0,4);     //<String.h>, the first 4 char
	  String part2 = name(4);       //<String.h>, the rest
	  if (part1=="sVar")
	    objectFuncCoef[part2]["En"]["Fuel"]["0"]=
	      procInValMapIt.curr()->value;
	}
    }
}

//// showPower

Symbol CFCellConst::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
  showPowerType="El";
  showPowerNumber="0";
  return "Ex";
}


  // function describes the heatexchanging unit, gives back function value
  // and the first derivative,
  // needed for call 'rtsafe'(Newton-Raphson-algorithem) to calculate TR_sec

void CFCellConst::hxfunc(double x, double *f, double *df)
{
  *f  = (x-epsilon_hc)/(log((1-epsilon_hc)/(1-x))) - (procInValMap["DeltaT_m"]/
								(procInValMap["TFsec_0"]-CFCellConst::T_REx));
  *df = (log((1-epsilon_hc)/(1-x)) - (x-epsilon_hc)/(1-x))/
											pow(log((1-epsilon_hc)/(1-x)),2.0);
}
  // the Newton-Raphson-algorithm is used to calculate epsilon_sec in
  // 'CFCellConst::actualSimplexInput'
  // It is based on Press et al., Numerical Recipes in C,
  // Cambridge University Press, Cambridge (1988)
  // changes: float -> double; funcd -> CFCellConst::*hxfunc (declaration);
  // 		 *funcd -> CFCellConst::hxfunc (call)

double CFCellConst::rtsafe(void (CFCellConst::*hxfunc)(double, double *,
								double *), double x1, double x2, double xacc)
// Using a combination of Newton-Raphson and bisection, find
// the root of a function bracketed between x1 and x2. The
// root, returned as the function value rtsafe, will be
// refined until its accuracy is known within xacc.  funcd
// is a user-supplied routine that returns both the function
// value and the first derivative of the function.
{
	#define MAXIT 100 //Maximum allowed number of iterations.
	int j;
	double df,dx,dxold,f,fh,fl;
	double temp,xh,xl,rts;
	CFCellConst::hxfunc(x1,&fl,&df);
	CFCellConst::hxfunc(x2,&fh,&df);
	if ((fl > 0.0 && fh > 0.0) || (fl < 0.0 && fh < 0.0))
		nrerror2("Root must be bracketed in rtsafe"); //see N-R_errormsg.c
	if (fl == 0.0) return x1;
	if (fh == 0.0) return x2;
	if (fl < 0.0) //					Orient the search so that f(xl) < 0.
		{
		xl=x1;
		xh=x2;
		}
	else
		{
		xh=x1;
		xl=x2;
		}
	rts=0.5*(x1+x2);//				Initialize the guess for root,
	dxold=fabs(x2-x1);//			the stepsize before last,
	dx=dxold; //							and the last step.
	CFCellConst::hxfunc(rts,&f,&df);
	for (j=1;j<=MAXIT;j++) // Loop over allowed iterations.
		{
		if ((((rts-xh)*df-f)*((rts-xl)*df-f) > 0.0)//Bisect if Newton
												   //out of range,
				|| (fabs(2.0*f) > fabs(dxold*df))) //or not decreasing
												   //fast enough.
			{
			dxold=dx;
			dx=0.5*(xh-xl);
			rts=xl+dx;
			if (xl == rts) return rts; // Change in root is negligible.
			}
		else // Newton step acceptable. Take it.
			{
			dxold=dx;
			dx=f/df;
			temp=rts;
			rts -= dx;
			if (temp == rts) return rts;
			}
		if (fabs(dx) < xacc) return rts;//  Convergence criterion.
		CFCellConst::hxfunc(rts,&f,&df);// The one new function
									    // valuation per iteration.
		if (f < 0.0) // Maintain the bracket on the root.
			xl=rts;
		else
			xh=rts;
		}
	nrerror2("Maximum number of iterations exceeded in rtsafe");
	return 0.0; // Never get here.
	#undef MAXIT
}


// end of file

