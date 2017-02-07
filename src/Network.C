
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

#include "Network.h"
#include <math.h>
#include <float.h>       // for numerical infinity (DBL_MAX, DBL_MIN)

// for comparison of (x-x == 0) a value REL_EPS should be used
// according to the HP-UX Floating Point Guide
// for single precision  REL_EPS= 10e5, double prec. REL_EPS = 10e14
// and quad. prec. REL_EPS = 10e26;
// use then  (x-x =0) --> ( fabs(x-x) < (x/REL_EPS) )
// fabs() -> <math.h>

#define REL_EPS  10e14

///////////////////////////////////////////////////////////////////
//
// CLASS: NBW
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: enthalpy adaptation by mixing valves and
// (waste-) heat dumping

//// NBW
// Standard Constructor
//
NBW::NBW(void)
{
  T_FEx = 0;
  T_REx = 0;
  T_FEn = 0;
  T_REn = 0;
}

//// NBW
// Constructor
//
NBW::NBW(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) : Proc(cProcApp, procId,
             cProcType, cProcVal)
{

  T_FEx = 0;
  T_REx = 0;
  T_FEn = 0;
  T_REn = 0;

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");

  if ((procInValMap["Flag"] != 14) && (procInValMap["Flag"] != 24) &&
      (procInValMap["Flag"] != 12))
    procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("H","0");
  En.insert(a);
  Symbol2 b = Symbol2("H","0");
  Ex.insert(b);

  if (procApp->testFlag)
    procApp->message(1001, "NBW");
}

//// ~NBW
// Destructor
//
NBW::~NBW(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NBW");
    }
}

//// actualExJ
// actualize attributes of exit side
//
void NBW::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (vecJ["0"]["En"]["In"]["F"].element("T"))
    T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
  else
    procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][F][T]");

   if (procInValMap["Flag"] != 24)
    {
      if (vecJ["0"]["En"]["In"]["R"].element("T"))
        T_REn = vecJ["0"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][R][T]");
    }

  if (procInValMap["Flag"] == 12)
    {
      T_FEx = T_FEn;
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
    }
}

//// actualEnJ
// actualize attributes of entrance side
//
void NBW::actualEnJ(const Map<Symbol,double>& pVecU)
{
  if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");

  if (procInValMap["Flag"] != 12)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
    }

  if  (procInValMap["Flag"] == 24)
    {
      T_REn = T_REx;
      vecJ["0"]["En"]["Out"]["R"]["T"] = T_REn;
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void NBW::actualSimplexInput(const Map<Symbol,double>& pVecU,
                             double actualIntLength)
{
if (T_FEn <= T_REn || (T_FEn < 0) || (T_REn < 0))
  {

    // DotEEn[H][0] should be 0 but additional heating is allowed (if it is possible)

    calcAddHeat(T_FEn,T_REx);

    // T_REx,T_FEn are used by descendants

    if ((T_FEx <= T_REx) || (T_FEx < 0) || (T_REx < 0)) // no heat transfer in this case
      {
        noLeavingHeat();
      }
  }
else
  {
    if ((T_FEx <= T_REx) || (T_FEx < 0) || (T_REx < 0))

      // no heat transfer in this case,
      // but total waste heat dumping is allowed
      // (DotEEn[H][0] may be positive!)

      {
        noLeavingHeat();
      }
    else
      {
        if (T_FEn >= T_FEx)               // case 1.
          {
            if (T_FEx >= T_REn)           // case 1.1.
              {
                if (T_REn >= T_REx)       // case 1.1.1.
                  {
                    equalConstraintRhs[1] = 0;
                    equalConstraintCoef[1]["Ex"]["H"]["0"] = 1;
                    equalConstraintCoef[1]["En"]["H"]["0"] = -1;
                  }
                else                      // case 1.1.2.
                  {
                    equalConstraintRhs[1] = 0;
                    equalConstraintCoef[1]["Ex"]["H"]["0"] =
                      (T_FEn-T_REn)/(T_FEn-T_REx);
                    equalConstraintCoef[1]["En"]["H"]["0"] = -1;
                  }
              }
            else                          // case 1.2.
              {
                equalConstraintRhs[1] = 0;
                equalConstraintCoef[1]["Ex"]["H"]["0"] = 1;
                equalConstraintCoef[1]["En"]["H"]["0"] = -1;
              }
          }
        else                              // case 2.
          {
            if (T_FEn >= T_REx)           // case 2.1.
              {
                if (T_REx >= T_REn)       // case 2.1.1.
                  {
                    calcAddHeat(T_REn,T_FEn);
                    // T_REn,T_FEn are used by descendants
                  }
                else                      // case 2.1.2.
                  {
                    calcAddHeat(T_REx,T_FEn);
                    // T_REx,T_FEn are used by descendants
                  }
              }
            else                          // case 2.2.
              {
                calcAddHeat(T_FEn,T_REx);
                // T_FEn,T_REx are used by descendants
              }
          }
      }
  }
}

//// showPower
//
Symbol NBW::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}

//// calcAddHeat
// calculate additional heat (zero in this case)
//
void NBW::calcAddHeat(double T_1, double T_2)
{
  // T_1,T_2 are used by descendants

  equalConstraintRhs[1] = 0;
  equalConstraintCoef[1]["Ex"]["H"]["0"] = 1;
  equalConstraintCoef[1]["En"]["H"]["0"] = -1;

  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = 1;
}

//// noLeavingHeat
// constraints for no heating cases
//
void NBW::noLeavingHeat(void)
{
  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = 1;
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWAConst
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: enthalpy adaptation by mixing valves,
// (waste-) heat dumping and additional heating
// (similar to CBoiConst)

//// NBWAConst
// Standard Constructor
//
NBWAConst::NBWAConst(void)
{
  T_FEx = 0;
  T_REx = 0;
  T_FEn = 0;
  T_REn = 0;
}

//// NBWAConst
// Constructor
//
NBWAConst::NBWAConst(App* cProcApp, Symbol procId, Symbol cProcType,
                     ioValue* cProcVal) : NBW(cProcApp, procId,
                     cProcType, cProcVal)
{
  T_FEx = 0;
  T_REx = 0;
  T_FEn = 0;
  T_REn = 0;

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if (!procInValMap.element("eta"))
    procApp->message(55, vertexId.the_string() + " eta");
  if (!procInValMap.element("lambdaEl_H"))
    procApp->message(55, vertexId.the_string() + " lambdaEl_H");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

  if (procInValMap["eta"] == 0 ||
      procInValMap["Count"] == 0 )
    procApp->message(505, vertexId.the_string());

  if ((procInValMap["Flag"] != 14) && (procInValMap["Flag"] != 24) &&
      (procInValMap["Flag"] != 12))
    procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("Fuel","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  En.insert(c);
  Symbol2 d = Symbol2("H","0");
  Ex.insert(d);

  if (procApp->testFlag)
    procApp->message(1001, "NBWAConst");

}

//// ~NBWAConst
// Destructor
//
NBWAConst::~NBWAConst(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NBWAConst");
    }
}

//// calcAddHeat
// calculate additional heat
//
void NBWAConst::calcAddHeat(double T_1, double T_2)
{
  equalConstraintRhs[1] = 0;
  equalConstraintCoef[1]["Ex"]["H"]["0"] =
    (T_FEn-T_1)/(T_FEx-T_REx);
  equalConstraintCoef[1]["En"]["H"]["0"] = -1;

  if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] < 0)
        procApp->message(67, vertexId.the_string()+" DotEH_0");

      lessConstraintRhs[3] =
        procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[3]["Ex"]["H"]["0"] =
        (T_FEx-T_2)/(T_FEx-T_REx);
    }

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"] = -(1.0/procInValMap["eta"]) *
    ((T_FEx-T_2)/(T_FEx-T_REx));
  equalConstraintCoef[4]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["H"]["0"] = - procInValMap["lambdaEl_H"] *
    ((T_FEx-T_2)/(T_FEx-T_REx));
  equalConstraintCoef[5]["En"]["El"]["0"] = 1;

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

//// noLeavingHeat
// constraints for no heating cases
//
void NBWAConst::noLeavingHeat(void)
{
  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = 1;

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"] = -1;
  equalConstraintCoef[4]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["H"]["0"] = -1;
  equalConstraintCoef[5]["En"]["El"]["0"] = 1;
}

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

//// NBWAConv
// Standard Constructor
//
NBWAConv::NBWAConv(void)
{
  T_FEx = 0;
  T_REx = 0;
  T_FEn = 0;
  T_REn = 0;

  x     = 0;
  y     = 0;
}

//// NBWAConv
// Constructor
//
NBWAConv::NBWAConv(App* cProcApp, Symbol procId, Symbol cProcType,
                   ioValue* cProcVal) : NBW(cProcApp, procId,
                   cProcType, cProcVal)
{
  T_FEx = 0;
  T_REx = 0;
  T_FEn = 0;
  T_REn = 0;

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
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
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;

  if (procInValMap["x_CO2"] <= 0 ||
      procInValMap["Count"] <= 0 )
    procApp->message(505, vertexId.the_string());

  if ((procInValMap["Flag"] != 14) && (procInValMap["Flag"] != 24) &&
      (procInValMap["Flag"] != 12))
    procApp->message(61, vertexId.the_string() + " Flag");

  x = 0;
  y = 0;

  x = procInValMap["T_B"] - procInValMap["T_Env"];

  if (x <= 0)
    procApp->message(61, vertexId.the_string() + ": T_B,T_Env");
  x = (1.0/x) * procInValMap["DotQ_Rad"] *  procInValMap["Count"];

  y = procInValMap["A_1"]/(100*procInValMap["x_CO2"]);
  y = y +  procInValMap["B"];
  y = y/100.00;

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("Fuel","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  En.insert(c);
  Symbol2 d = Symbol2("H","0");
  Ex.insert(d);

  if (procApp->testFlag)
    procApp->message(1001, "NBWAConv");

}

//// ~NBWAConv
// Destructor
//
NBWAConv::~NBWAConv(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NBWAConv");
    }
}

//// calcAddHeat
// calculate additional heat
//
void NBWAConv::calcAddHeat(double T_1, double T_2)
{
  equalConstraintRhs[1] = 0;
  equalConstraintCoef[1]["Ex"]["H"]["0"] =
    (T_FEn-T_1)/(T_FEx-T_REx);
  equalConstraintCoef[1]["En"]["H"]["0"] = -1;

  if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEH_0");

      lessConstraintRhs[3] =
        procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[3]["Ex"]["H"]["0"] =
        (T_FEx-T_2)/(T_FEx-T_REx);
    }

  // calculate time dependent efficiency

  double z     = 0;
  double rhs   = 0;
  double T_Boi = 0;

  T_Boi = (T_FEx+T_2)/2.0; // here T_FEn (or T_REx) plays the role of T_REx in CBoiConv

  z = procInValMap["T_W"] + (T_Boi - procInValMap["T_B"]);
  z = z - procInValMap["T_Env"];
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

  equalConstraintRhs[4] = rhs;
  equalConstraintCoef[4]["Ex"]["H"]["0"] =
    ((T_FEx-T_2)/(T_FEx-T_REx)) * (-z);
  equalConstraintCoef[4]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["H"]["0"] = -procInValMap["lambdaEl_H"] *
    ((T_FEx-T_2)/(T_FEx-T_REx));
  equalConstraintCoef[5]["En"]["El"]["0"] = 1;

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

//// noLeavingHeat
// constraints for no heating cases
//
void NBWAConv::noLeavingHeat(void)
{
  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = 1;

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"] = -1;
  equalConstraintCoef[4]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["H"]["0"] = -1;
  equalConstraintCoef[5]["En"]["El"]["0"] = 1;
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWACond
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: enthalpy adaptation by mixing valves and
// (waste-) heat dumping and additional heating
// (similar to CBoiCond)

//// NBWACond
// Standard Constructor
//
NBWACond::NBWACond(void)
{
  T_FEx = 0;
  T_REx = 0;
  T_FEn = 0;
  T_REn = 0;
}

//// NBWACond
// Constructor
//
NBWACond::NBWACond(App* cProcApp, Symbol procId, Symbol cProcType,
                   ioValue* cProcVal) : NBW(cProcApp, procId,
                   cProcType, cProcVal)
{
  T_FEx = 0;
  T_REx = 0;
  T_FEn = 0;
  T_REn = 0;

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
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;

  if (procInValMap["B_1"] <= 0 ||
      procInValMap["B_2"] <= 0 ||
      procInValMap["C_1"] <= 0 ||
      procInValMap["Count"] <= 0 )
    procApp->message(505, vertexId.the_string());

  if ((procInValMap["Flag"] != 14) && (procInValMap["Flag"] != 24) &&
      (procInValMap["Flag"] != 12))
    procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("Fuel","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  En.insert(c);
  Symbol2 d = Symbol2("H","0");
  Ex.insert(d);

  if (procApp->testFlag)
    procApp->message(1001, "NBWACond");
}

//// ~NBWACond
// Destructor
//
NBWACond::~NBWACond(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NBWACond");
    }
}

//// calcAddHeat
// calculate additional heat
//
void NBWACond::calcAddHeat(double T_1, double T_2)
{
  equalConstraintRhs[1] = 0;
  equalConstraintCoef[1]["Ex"]["H"]["0"] =
    (T_FEn-T_1)/(T_FEx-T_REx);
  equalConstraintCoef[1]["En"]["H"]["0"] = -1;

  if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] <0)
        procApp->message(67, vertexId.the_string() + " DotEH_0");

      lessConstraintRhs[3] =
        procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[3]["Ex"]["H"]["0"] =
        (T_FEx-T_2)/(T_FEx-T_REx);
    }

  // calculate time dependent efficiency

  // technical constraints
  // here T_2 = (T_FEn or T_REx) plays the role of T_REx in CBoiCond

  double eta = 0;
  double T_0 = 0;

  T_0 = procInValMap["T_0"];

  if (T_2 <  procInValMap["T_Cond"])
    eta =  procInValMap["A_1"] - procInValMap["B_1"]*(T_2-T_0) -
      procInValMap["C_1"] * (T_2-T_0) * (T_2-T_0);
  else
    eta = procInValMap["A_2"] - procInValMap["B_2"] * (T_2-T_0);

  if (eta <= 0)
    procApp->message(65,vertexId.the_string() + " actualSimplexInput, eta");

  if ((T_2 >  procInValMap["TR_Max"]) || (T_2 <  procInValMap["TR_Min"]))
    {
      equalConstraintRhs[2] = 0;
      equalConstraintCoef[2]["Ex"]["H"]["0"] = 1;
      eta = 1;
    } // no additional heating possible

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"] =
    ((T_FEx-T_2)/(T_FEx-T_REx)) * (-1.0/eta);
  equalConstraintCoef[4]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["H"]["0"] = -procInValMap["lambdaEl_H"] *
    ((T_FEx-T_2)/(T_FEx-T_REx));
  equalConstraintCoef[5]["En"]["El"]["0"] = 1;

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

//// noLeavingHeat
// constraints for no heating cases
//
void NBWACond::noLeavingHeat(void)
{
  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = 1;

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"] = -1;
  equalConstraintCoef[4]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["H"]["0"] = -1;
  equalConstraintCoef[5]["En"]["El"]["0"] = 1;
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NHEx
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: heat exchanger with time constant
// temperature difference, no pressure loss, and
// maximal exergy efficiency

//// NHEx
// Standard Constructor
//
NHEx::NHEx(void)
{
  T_FEx = 0;
  T_REx = 0;
  T_FEn = 0;
  T_REn = 0;
}

//// NHEx
// Constructor
//
NHEx::NHEx(App* cProcApp, Symbol procId, Symbol cProcType,
           ioValue* cProcVal) : Proc(cProcApp, procId,
           cProcType, cProcVal)
{
  T_FEx = 0;
  T_REx = 0;
  T_FEn = 0;
  T_REn = 0;

  if (!procInValMap.element("deltaT"))
    procApp->message(55, vertexId.the_string() + " deltaT");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if ((procInValMap["Flag"] != 11) && (procInValMap["Flag"] != 22) &&
      (procInValMap["Flag"] != 33) && ( procInValMap["Flag"] != 44))
    procApp->message(61, vertexId.the_string() + " Flag");
  if (procInValMap["deltaT"] <= 0)
    procApp->message(67, vertexId.the_string() + " deltaT");

  Symbol2 a = Symbol2("H","0");
  En.insert(a);
  Symbol2 b = Symbol2("H","0");
  Ex.insert(b);

  if (procApp->testFlag)
    procApp->message(1001, "NHEx");
}

//// ~NHEx
// Destructor
//
NHEx::~NHEx(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NHEx");
    }
}

//// actualExJ
// actualize attributes of exit side
//
void NHEx::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 11)  // control of input
    {
      if (vecJ["0"]["En"]["In"]["F"].element("T"))
        T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][F][T]");

      T_FEx = T_FEn - procInValMap["deltaT"];
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;

      if (vecJ["0"]["En"]["In"]["R"].element("T"))
        T_REn = vecJ["0"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][R][T]");

      T_REx = T_REn - procInValMap["deltaT"];
      vecJ["0"]["Ex"]["Out"]["R"]["T"] = T_REx;
    }

  if (procInValMap["Flag"] == 22)  // control of input
    {
      if (vecJ["0"]["En"]["In"]["F"].element("T"))
        T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][F][T]");

      T_FEx = T_FEn - procInValMap["deltaT"];
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
    }

  if (procInValMap["Flag"] == 33)  // control of input
    {
      if (vecJ["0"]["En"]["In"]["R"].element("T"))
        T_REn = vecJ["0"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][R][T]");

      T_REx = T_REn - procInValMap["deltaT"];
      vecJ["0"]["Ex"]["Out"]["R"]["T"] = T_REx;
    }
}

//// actualEnJ
// actualize attributes of entrance side
//
void NHEx::actualEnJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 44)  // control of input
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");

      T_FEn = T_FEx + procInValMap["deltaT"];
      vecJ["0"]["En"]["Out"]["F"]["T"] = T_FEn;

      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
        T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");

      T_REn = T_REx + procInValMap["deltaT"];
      vecJ["0"]["En"]["Out"]["R"]["T"] = T_REn;
    }

 if (procInValMap["Flag"] == 33)  // control of input
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");

      T_FEn = T_FEx + procInValMap["deltaT"];
      vecJ["0"]["En"]["Out"]["F"]["T"] = T_FEn;
    }

 if (procInValMap["Flag"] == 22)  // control of input
    {
      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
        T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");

      T_REn = T_REx + procInValMap["deltaT"];
      vecJ["0"]["En"]["Out"]["R"]["T"] = T_REn;
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef and
// rhs before optimization
//
void NHEx::actualSimplexInput(const Map<Symbol,double>& pVecU,
                              double actualIntLength)
{
 if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }

 equalConstraintRhs[2] = 0;
 equalConstraintCoef[2]["Ex"]["H"]["0"] = 1;
 equalConstraintCoef[2]["En"]["H"]["0"] = -1;

 if ((T_FEn < (T_REx +  procInValMap["deltaT"])) ||
     (T_REn > (T_FEx +  procInValMap["deltaT"])) ||
     (T_FEn < 0 ) || (T_REn < 0 ) || (T_FEx < 0 ) || (T_REx < 0 ) ||
     (T_FEn <= T_REn) || (T_FEx <= T_REx))

   {
     equalConstraintRhs[3] = 0;
     equalConstraintCoef[3]["Ex"]["H"]["0"] = 1;
   }
}

//// showPower
//
Symbol NHEx::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NQTrans
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: transport of heat and comparison of quality

//// NQTrans
// Standard Constructor

NQTrans::NQTrans(void)
{
  T_FEn = 0;
  T_FEx = 0;
}

//// NQTrans
// Constructor
//
NQTrans::NQTrans(App* cProcApp, Symbol procId, Symbol cProcType,
                 ioValue* cProcVal) : Proc(cProcApp, procId,
                 cProcType, cProcVal)
{
  T_FEn = 0;
  T_FEx = 0;

  if (!procInValMap.element("eta"))
    procApp->message(55, vertexId.the_string() + " eta");
  if (!procInValMap.element("lambdaEl_H"))
    procApp->message(55, vertexId.the_string() + " lambdaEl_H");
  if (!procInValMap.element("deltaT"))
    procApp->message(55, vertexId.the_string() + " deltaT");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["eta"] == 0 ||
      procInValMap["Count"] == 0 )
    procApp->message(505, vertexId.the_string());
  if ((procInValMap["Flag"] != 14) && (procInValMap["Flag"] != 11) &&
      (procInValMap["Flag"] != 44))
    procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("H","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  Ex.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "NQTrans");
}

//// ~NQTrans
// Destructor
//
NQTrans::~NQTrans(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NQTrans");
    }
}

//// actualExJ
// actualize attributes of exit side
//
void NQTrans::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 11)
    {
      if (vecJ["0"]["En"]["In"]["F"].element("T"))
        T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][F][T]");

      T_FEx = T_FEn - procInValMap["deltaT"];
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
      vecJ["0"]["Ex"]["Out"]["R"]["T"] = 0;  // is not determined in this case!
    }
}

//// actualEnJ
// actualize attributes of entrance side
//
void NQTrans::actualEnJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 44)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");

      T_FEn = T_FEx + procInValMap["deltaT"];
      vecJ["0"]["En"]["Out"]["F"]["T"] = T_FEn;
      vecJ["0"]["En"]["Out"]["R"]["T"] = 0;  // is not determined in this case!
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void NQTrans::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                 double actualIntLength)
{
  if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }

  // control of input

  if (procInValMap["Flag"] == 14)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
      if (vecJ["0"]["En"]["In"]["F"].element("T"))
        T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][F][T]");
    }

  if ((T_FEx < 0 ) || (T_FEn < 0 ) || (T_FEn < (T_FEx + procInValMap["deltaT"])))
    {
      equalConstraintRhs[4] = 0;
      equalConstraintCoef[4]["Ex"]["H"]["0"] = 1;
      equalConstraintRhs[2] = 0;
      equalConstraintCoef[2]["Ex"]["H"]["0"] = -1;  // no heat dumping in this case
      equalConstraintCoef[2]["En"]["H"]["0"] = 1;
      equalConstraintRhs[3] = 0;
      equalConstraintCoef[3]["Ex"]["H"]["0"] = -1;
      equalConstraintCoef[3]["En"]["El"]["0"] = 1;
    }
  else
    {

      // technical constraints

      equalConstraintRhs[2] = 0;
      equalConstraintCoef[2]["Ex"]["H"]["0"] = -1.0/procInValMap["eta"];
      equalConstraintCoef[2]["En"]["H"]["0"] = 1;

      equalConstraintRhs[3] = 0;
      equalConstraintCoef[3]["Ex"]["H"]["0"] = -procInValMap["lambdaEl_H"];
      equalConstraintCoef[3]["En"]["El"]["0"] = 1;

    }
}

//// showPower
//
Symbol NQTrans::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NHTrans
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: transportation of enthalpy

//// NHTrans
// Standard Constructor
//
NHTrans::NHTrans(void)
{
  T_FEn    = 0;
  T_REn    = 0;
  T_FEx    = 0;
  T_REx    = 0;
  T_Env    = 0;
  AkOverMc = 0;
}

//// NHTrans
// Constructor
//
NHTrans::NHTrans(App* cProcApp, Symbol procId, Symbol cProcType,
                 ioValue* cProcVal) : Proc(cProcApp, procId,
                 cProcType, cProcVal)
{
  T_FEn    = 0;
  T_REn    = 0;
  T_FEx    = 0;
  T_REx    = 0;
  T_Env    = 0;
  AkOverMc = 0;

  if (!procInValMap.element("eta_0"))
    procApp->message(55, vertexId.the_string() + " eta_0");
  if (!procInValMap.element("TF_0"))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (!procInValMap.element("TR_0"))
    procApp->message(55, vertexId.the_string() + " TR_0");
  if (!procInValMap.element("T_Env"))
    procApp->message(55, vertexId.the_string() + " T_Env");
  if (!procInValMap.element("p_El"))
    procApp->message(55, vertexId.the_string() + " p_El");
  if (!procInValMap.element("c_p"))
    procApp->message(55, vertexId.the_string() + " c_p");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if (procInValMap["eta_0"] == 0 ||
      procInValMap["c_p"] == 0 )
    procApp->message(56, vertexId.the_string());
  if ((procInValMap["Flag"] != 11) && (procInValMap["Flag"] != 22) &&
      (procInValMap["Flag"] != 33) && (procInValMap["Flag"] != 44))
    procApp->message(61, vertexId.the_string() + " Flag");

  T_Env = procInValMap["T_Env"];

  double x;
  x = procInValMap["TF_0"] + procInValMap["TR_0"] - (2 * procInValMap["T_Env"]);
  if (x <= 0)
    procApp->message(61, vertexId.the_string() + " TF_0,TR_0,T_Env");

  x = (procInValMap["TF_0"] - procInValMap["TR_0"])/x;
  AkOverMc = x * (1 - procInValMap["eta_0"]);
  if (AkOverMc > 0.1)
    procApp->message(61, vertexId.the_string() + " AkOverMc is too large ");
    // assumption: AkOverMc << 1

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("H","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  Ex.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "NHTrans");
}

//// ~NHTrans
// Destructor
//
NHTrans::~NHTrans(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NHTrans");
    }
}

//// actualExJ
// actualize attributes of exit side
//
void NHTrans::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 11)
    {
      if (vecJ["0"]["En"]["In"]["F"].element("T"))
        T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][F][T]");
      if (vecJ["0"]["En"]["In"]["R"].element("T"))
        T_REn = vecJ["0"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][R][T]");
      T_FEx = T_Env + (T_FEn - T_Env) * (1 - AkOverMc);
      T_REx = T_Env + (T_REn - T_Env) * (1 + AkOverMc);
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
      vecJ["0"]["Ex"]["Out"]["R"]["T"] = T_REx;
    }

 if (procInValMap["Flag"] == 22)
    {
      if (vecJ["0"]["En"]["In"]["F"].element("T"))
        T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][F][T]");

      T_FEx = T_Env + (T_FEn - T_Env) * (1 - AkOverMc);
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
    }

  if (procInValMap["Flag"] == 33)
    {
      if (vecJ["0"]["En"]["In"]["R"].element("T"))
        T_REn = vecJ["0"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][R][T]");
      T_REx = T_Env + (T_REn - T_Env) * (1 + AkOverMc);
      vecJ["0"]["Ex"]["Out"]["R"]["T"] = T_REx;
    }
}

//// actualEnJ
// actualize attributes of entrance side
//
void NHTrans::actualEnJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 44)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
        T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");
      T_FEn = T_Env + (T_FEx - T_Env) * (1 + AkOverMc);
      T_REn = T_Env + (T_REx - T_Env) * (1 - AkOverMc);
      vecJ["0"]["En"]["Out"]["F"]["T"] = T_FEn;
      vecJ["0"]["En"]["Out"]["R"]["T"] = T_REn;
    }

 if (procInValMap["Flag"] == 22)
    {
      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
        T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");
      T_REn = T_Env + (T_REx - T_Env) * (1 - AkOverMc);
      vecJ["0"]["En"]["Out"]["R"]["T"] = T_REn;
    }

 if (procInValMap["Flag"] == 33)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][F][T]");
      T_FEn = T_Env + (T_FEx - T_Env) * (1 + AkOverMc);
      vecJ["0"]["En"]["Out"]["F"]["T"] = T_FEn;
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void NHTrans::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                 double actualIntLength)
{
  if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] < 0)
        procApp->message(67, vertexId.the_string() + " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"];
      lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }

  // control of input

  if ((T_FEn <= T_REn) || (T_FEx <= T_REx) ||
      (T_FEx < 0)      || (T_FEn < 0)      ||
      (T_REx < 0)      || (T_REn < 0))
    {
      equalConstraintRhs[4] = 0;
      equalConstraintCoef[4]["Ex"]["H"]["0"] = 1;
      equalConstraintRhs[2] = 0;
      equalConstraintCoef[2]["Ex"]["H"]["0"] = -1;  // no heat dumping in this case
      equalConstraintCoef[2]["En"]["H"]["0"] = 1;
      equalConstraintRhs[3] = 0;
      equalConstraintCoef[3]["Ex"]["H"]["0"] = -1;
      equalConstraintCoef[3]["En"]["El"]["0"] = 1;
    }
  else
    {

      // technical constraints

      double x;
      x = (T_FEn - T_REn)/(T_FEx - T_REx);

      equalConstraintRhs[2] = 0;
      equalConstraintCoef[2]["Ex"]["H"]["0"]= -x;
      equalConstraintCoef[2]["En"]["H"]["0"]= 1;

      x = procInValMap["p_El"]/procInValMap["c_p"];
      x = x /( T_FEx - T_REx);

      equalConstraintRhs[3] = 0;
      equalConstraintCoef[3]["Ex"]["H"]["0"] = -x;
      equalConstraintCoef[3]["En"]["El"]["0"] = 1;

    }
}

//// showPower
//
Symbol NHTrans::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NEHP
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: waste heat or solar energy using electrical heat pump

//// NEHP
// Standard Constructor
//
NEHP::NEHP(void)
{
}

//// NEHP
// Constructor
//
NEHP::NEHP(App* cProcApp, Symbol procId, Symbol cProcType,
           ioValue* cProcVal) : Proc(cProcApp, procId,
           cProcType, cProcVal)
{
  if (!procInValMap.element("eta_G"))
    procApp->message(55, vertexId.the_string() + " eta_G");
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
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if ((procInValMap["Flag"] == 12 || procInValMap["Flag"] == 22)
      && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if ((procInValMap["Flag"] == 22 || procInValMap["Flag"] == 24) &&
      (!procInValMap.element("TR_0")))
    procApp->message(55, vertexId.the_string() + " TR_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["eta_G"] <= 0)
    procApp->message(505, vertexId.the_string());
  if (procInValMap["T_ConMax"] <= procInValMap["T_ConMin"])
    procApp->message(61, vertexId.the_string() + " T_ConMax,T_ConMin");
  if (procInValMap["T_VapMax"] <= procInValMap["T_VapMin"])
    procApp->message(61, vertexId.the_string() + " T_VapMax,T_VapMin");
  if ((procInValMap["Flag"] != 12) && (procInValMap["Flag"] != 14) &&
      (procInValMap["Flag"] != 22) && (procInValMap["Flag"] != 24))
    procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("H","0");
  Ex.insert(b);
  Symbol2 c = Symbol2("H","0");
  En.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "NEHP");
}

//// ~NEHP
// Destructor
//
NEHP::~NEHP(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NEHP");
    }
}

//// actualExJ
// actualize attributes of exit side
//
void NEHP::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 12 || procInValMap["Flag"] == 22)
    vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"];
}

//// actualEnJ
// actualize attributes of entrance side
//
void NEHP::actualEnJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 22 || procInValMap["Flag"] == 24 )
    vecJ["0"]["En"]["Out"]["R"]["T"] = procInValMap["TR_0"];
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void NEHP::actualSimplexInput(const Map<Symbol,double>& pVecU,
                              double actualIntLength)
{
  double T_FCon = 0;
  double T_HS   = 0;
  double T_REx  = 0;
  double T_FEx  = 0;
  double T_REn  = 0;
  double T_FEn  = 0;

  // control of input

  if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");

  if (procInValMap["Flag"] != 12 && procInValMap["Flag"] != 22)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
    }
  else
    {
      T_FEx = procInValMap["TF_0"];  // Flag == 12 or == 22
    }

  if (vecJ["0"]["En"]["In"]["F"].element("T"))
    T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
  else
    procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][F][T]");

  if (procInValMap["Flag"] != 22 && procInValMap["Flag"] != 24)
    {
      if (vecJ["0"]["En"]["In"]["R"].element("T"))
        T_REn = vecJ["0"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][R][T]");
    }
  else
    {
      T_REn = procInValMap["TR_0"];  // Flag == 22 or == 24
    }

  // calculate heat source temperature

  if (procInValMap["Flag"] == 12 || procInValMap["Flag"] == 14)
    T_HS = T_REn;  // the coldest temperature of the heat source
  if (procInValMap["Flag"] == 24 || procInValMap["Flag"] == 22)
    T_HS = T_FEn;  // to simulate an evaporator in a heat storage

  // calculate condenser temperature T_FCon

  T_FCon = T_FEx;

  // calculate inverse efficiency

  double x;
  x = T_FCon + procInValMap["deltaT_Con"];
  x = (x - T_HS + procInValMap["deltaT_Vap"])/x;
  x = x/procInValMap["eta_G"];

  if ((T_FEx < 0) || (T_REx < 0) || (T_FEx <= T_REx) ||
      (T_FEn < 0) || (T_REn < 0) || (T_FEn <= T_REn) ||
      (T_FCon + procInValMap["deltaT_Con"] < T_HS-procInValMap["deltaT_Vap"]) ||
      (x >= 1)    || (x < double(1)/REL_EPS) ||
      (T_FCon + procInValMap["deltaT_Con"] < procInValMap["T_ConMin"]) ||
      (T_FCon + procInValMap["deltaT_Con"] > procInValMap["T_ConMax"]) ||
      (T_HS   - procInValMap["deltaT_Vap"] < procInValMap["T_VapMin"]) ||
      (T_HS   - procInValMap["deltaT_Vap"] > procInValMap["T_VapMax"]))
    {
      equalConstraintRhs[3] = 0;
      equalConstraintCoef[3]["Ex"]["H"]["0"] = 1;
      x = 2;  // arbitrary value != 1, see constraint number 4
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

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"] = x - 1;
  equalConstraintCoef[4]["En"]["H"]["0"] = 1;

}

//// showPower
//
Symbol NEHP::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NGHP
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: waste heat or solar energy using gas driven heat pump

//// NGHP
// Standard Constructor
//
NGHP::NGHP(void)
{
}

//// NGHP
// Constructor
//
NGHP::NGHP(App* cProcApp, Symbol procId, Symbol cProcType,
           ioValue* cProcVal) : Proc(cProcApp,procId,cProcType,cProcVal)
{
  if (!procInValMap.element("eta_G"))
    procApp->message(55, vertexId.the_string() + " eta_G");
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
  if (!procInValMap.element("eta_Mech"))
    procApp->message(55, vertexId.the_string() + " eta_Mech");
  if (!procInValMap.element("a"))
    procApp->message(55, vertexId.the_string() + " a");
  if (!procInValMap.element("lambdaEl_H"))
    procApp->message(55, vertexId.the_string() + " lambdaEl_H");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if ((procInValMap["Flag"] == 12 || procInValMap["Flag"] == 22)
      && (!procInValMap.element("TF_0")))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if ((procInValMap["Flag"] == 22 || procInValMap["Flag"] == 24)
      && (!procInValMap.element("TR_0")))
    procApp->message(55, vertexId.the_string() + " TR_0");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["eta_G"] <= 0)
    procApp->message(505, vertexId.the_string());
  if (procInValMap["T_ConMax"] <= procInValMap["T_ConMin"])
    procApp->message(61, vertexId.the_string() + " T_ConMax,T_ConMin");
  if (procInValMap["T_VapMax"] <= procInValMap["T_VapMin"])
    procApp->message(61, vertexId.the_string() + " T_VapMax,T_VapMin");
  if ((procInValMap["Flag"] != 12) && (procInValMap["Flag"] != 14) &&
      (procInValMap["Flag"] != 22) && (procInValMap["Flag"] != 24))
    procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("H","0");
  Ex.insert(b);
  Symbol2 c = Symbol2("H","0");
  En.insert(c);
  Symbol2 d = Symbol2("Fuel","0");
  En.insert(d);

  if (procApp->testFlag)
    procApp->message(1001, "NGHP");
}

//// ~NGHP
// Destructor
//
NGHP::~NGHP(void)
{
  if (procApp !=0 )
    {
      if (procApp->testFlag)
        procApp->message(1002, "NGHP");
    }
}

//// actualExJ
// actualize attributes of exit side
//
void NGHP::actualExJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 12 || procInValMap["Flag"] == 22)
    vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"];
}

//// actualEnJ
// actualize attributes of entrance side
//
void NGHP::actualEnJ(const Map<Symbol,double>& pVecU)
{
  if (procInValMap["Flag"] == 22 || procInValMap["Flag"] == 24 )
    vecJ["0"]["En"]["Out"]["R"]["T"] = procInValMap["TR_0"];
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void NGHP::actualSimplexInput(const Map<Symbol,double>& pVecU,
                              double actualIntLength)
{
  double T_FCon = 0;
  double T_HS   = 0;
  double T_REx  = 0;
  double T_FEx  = 0;
  double T_REn  = 0;
  double T_FEn  = 0;

  // control of input

  if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");

  if (procInValMap["Flag"] != 12 && procInValMap["Flag"] != 22)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][F][T]");
    }
  else
    {
      T_FEx = procInValMap["TF_0"];  // Flag == 12 or == 22
    }

  if (vecJ["0"]["En"]["In"]["F"].element("T"))
    T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
  else
    procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][F][T]");

  if (procInValMap["Flag"] != 22 && procInValMap["Flag"] != 24)
    {
      if (vecJ["0"]["En"]["In"]["R"].element("T"))
        T_REn = vecJ["0"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string() + " vecJ[0][En][In][R][T]");
    }
  else
    {
      T_REn = procInValMap["TR_0"];  // Flag == 22 or == 24
    }

  // calculate heat source temperature

  if (procInValMap["Flag"] == 12 || procInValMap["Flag"] == 14)
    T_HS = T_REn;  // the coldest temperature of the heat source
  if (procInValMap["Flag"] == 24 || procInValMap["Flag"] == 22)
    T_HS = T_FEn;  // to simulate an evaporator in a heat storage

  // calculate condenser temperature T_FCon

  double y     = 0;
  double B     = 0;
  double C     = 0;
  double gamma = 0;

  gamma = procInValMap["eta_G"] * procInValMap["eta_Mech"]/procInValMap["a"];

  B = procInValMap["deltaT_Con"] - T_HS + procInValMap["deltaT_Vap"];
  B = B - T_REx - gamma * T_FEx + gamma * procInValMap["deltaT_Con"];
  C = -T_REx * (procInValMap["deltaT_Con"] - T_HS +  procInValMap["deltaT_Vap"]);
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
                             " T_FCon in  actualSimplexInput of NGHP-Module");
        }
    }
  else
    procApp->message(65, vertexId.the_string() +
                     " y in  actualSimplexInput of NGHP-Module");

  // calculate inverse efficiency

  double x;
  double z;

  x = T_FCon +  procInValMap["deltaT_Con"];
  x = (x - T_HS + procInValMap["deltaT_Vap"])/x;
  x = x /procInValMap["eta_G"];

  if ((T_FEx < 0) || (T_REx < 0) || (T_FEx <= T_REx) ||
      (T_FEn < 0) || (T_REn < 0) || (T_FEn <= T_REn) ||
      (T_FCon + procInValMap["deltaT_Con"] < T_HS - procInValMap["deltaT_Vap"]) ||
      (x >= 1)    || (x < double(1)/REL_EPS) ||
      (T_FCon + procInValMap["deltaT_Con"] < procInValMap["T_ConMin"]) ||
      (T_FCon + procInValMap["deltaT_Con"] > procInValMap["T_ConMax"]) ||
      (T_HS   - procInValMap["deltaT_Vap"] < procInValMap["T_VapMin"]) ||
      (T_HS   - procInValMap["deltaT_Vap"] > procInValMap["T_VapMax"]))
    {
      equalConstraintRhs[3] = 0;
      equalConstraintCoef[3]["Ex"]["H"]["0"] = 1;
      x = 2;  // arbitrary value != 1
    }
  else
    {
      z = (procInValMap["eta_Mech"]/x) + procInValMap["a"];
      if (z < double(1)/REL_EPS)
        {
          equalConstraintRhs[3] = 0;
          equalConstraintCoef[3]["Ex"]["H"]["0"] = 1;
          z = 2;  // arbitrary value != 1
        }
      else
        z = 1/z;
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
  equalConstraintCoef[2]["Ex"]["H"]["0"] = -z;
  equalConstraintCoef[2]["En"]["Fuel"]["0"] = 1;

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"] = -z * (1/x - 1) * procInValMap["eta_Mech"];
  equalConstraintCoef[4]["En"]["H"]["0"] = 1;

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["H"]["0"] = -procInValMap["lambdaEl_H"];
  equalConstraintCoef[5]["En"]["El"]["0"] = 1;

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
Symbol NGHP::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}

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

//// NAHP
// Standard Constructor

NAHP::NAHP(void)
{
}

//// NAHP
// Constructor

NAHP::NAHP(App* cProcApp, Symbol procId, Symbol cProcType,
               ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  if (!procInValMap.element("alpha"))
    procApp->message(55, vertexId.the_string()+ " alpha");
  if (!procInValMap.element("G"))
    procApp->message(55, vertexId.the_string()+ " G");
  if (!procInValMap.element("B"))
    procApp->message(55, vertexId.the_string()+ " B");
  if (!procInValMap.element("ddT_min"))
    procApp->message(55, vertexId.the_string()+ " ddT_min");
  if (!procInValMap.element("ddT_0"))
    procApp->message(55, vertexId.the_string()+ " ddT_0");

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");

  if ((procInValMap["Flag"] != 02) && (procInValMap["Flag"] != 04) &&
      (procInValMap["Flag"] != 12) && (procInValMap["Flag"] != 14))
    procApp->message(61, vertexId.the_string() + " Flag");

  if ((  procInValMap["Flag"] == 02 || procInValMap["Flag"] == 12 )  &&
      (!procInValMap.element("TFEx_0")))
    procApp->message(55, vertexId.the_string()+ " TFEx_0");

  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

  Symbol2 a = Symbol2("H","0");
  En.insert(a);                 //drive
  Symbol2 b = Symbol2("H","0");
  Ex.insert(b);                 //heat supply

  if ((procInValMap["Flag"] == 12) || (procInValMap["Flag"] == 14)) {
    Symbol2 c = Symbol2("H","1");
    En.insert(c);               //heat source
  }
  else{       // Flag = 02  ||  Flag = 04 (heat from environment)
    if (!procInValMap.element("c"))
      procApp->message(55, vertexId.the_string()+ " c");
    if (!procInValMap.element("d"))
      procApp->message(55, vertexId.the_string()+ " d");
    if (!procInValMap.element("deltaT_Spreiz"))
      procApp->message(55, vertexId.the_string()+ " deltaT_Spreiz");
  }

  if (procApp->testFlag)
    procApp->message(1001, "NAHP");
}

//// ~NAHP
// Destructor

NAHP::~NAHP(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NAHP");
    }
}

//// actualExJ
// actualize attributes of exit side

void NAHP::actualExJ(const Map<Symbol,double> & pVecU)
{
  if ( (procInValMap["Flag"] == 02) || (procInValMap["Flag"] == 12) )
    vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TFEx_0"];
}

//// actualEnJ
// actualize attributes of entrance side

void NAHP::actualEnJ(const Map<Symbol,double> & pVecU)
{
  // Entrance side(s) is (are both, in case of Flag = 12 || Flag = 14) Typ 1
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void NAHP::actualSimplexInput(const Map<Symbol,double> & pVecU,
                               double actualIntLength)
{
  double T_FEn0=0; //drive
  double T_REn0=0;

  double T_FEn1=0; //Heat source
  double T_REn1=0;

  double T_FEx=0;  //Heat to be supplied
  double T_REx=0;

  double T_HS=0;   // Heat source temperature in case Flag=02 || Flag=04

  //control of input

    // drive

  if (vecJ["0"]["En"]["In"]["F"].element("T"))
    T_FEn0 = vecJ["0"]["En"]["In"]["F"]["T"];
  else
    procApp->message(54,vertexId.the_string()+" vecJ[0][En][In][F][T]");

  if (vecJ["0"]["En"]["In"]["R"].element("T"))
    T_REn0 = vecJ["0"]["En"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string()+" vecJ[0][En][In][R][T]");

    // heat to be supplied

  if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
    T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");

  if (procInValMap["Flag"] ==04 || procInValMap["Flag"] ==14)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][F][T]");
    }
  else
    {
      T_FEx = procInValMap["TFEx_0"];  //Flag == 12 or == 22
    }

    // heat source

  if (procInValMap["Flag"] ==12 || procInValMap["Flag"] ==14)
    {
      if (vecJ["1"]["En"]["In"]["F"].element("T"))
        T_FEn1 = vecJ["1"]["En"]["In"]["F"]["T"];
      else
         procApp->message(54,vertexId.the_string()+" vecJ[1][En][In][F][T]");

      if (vecJ["1"]["En"]["In"]["R"].element("T"))
        T_REn1 = vecJ["1"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[1][En][In][R][T]");
    }
  else      //Flag == 02 or == 04
    {
       // calculate heat source temperature

       if (pVecU.element("T"))
         {
           T_HS   = procInValMap["d"] * pVecU["T"] + procInValMap["c"];
           T_FEn1 = T_HS;
           T_REn1 = T_HS - procInValMap["deltaT_Spreiz"];
         }
       else
         procApp->message(57,"T");
    }

  // calculate inverse efficiency `x'

  double dT_Hub   =0;
  double dT_Schub =0;
  double ddT      =0;
  double ddT_min  =0;

  double ddT_0    = procInValMap["ddT_0"];
  double B        = procInValMap["B"];
  double G        = procInValMap["G"];
  double alpha    = procInValMap["alpha"];

  double COP      =0;
  double x        =0;

  dT_Schub  = ( T_FEn0 + T_REn0 )/2.0  - ( T_FEx  + T_REx  )/2.0;
  dT_Hub    =   T_FEx                - ( T_FEn1 + T_REn1 )/2.0;

  ddT       = dT_Schub - B * dT_Hub;

  ddT_min   = procInValMap["ddT_min"];

   COP      = 1 +  (ddT-ddT_min) / (G*ddT-(G - 1/alpha) *ddT_min);

    x       = 1/COP;

 if  (     (T_FEx  <0 )       || (T_REx  <0 ) || (T_FEx  <=  T_REx )
      ||   (T_FEn0 <0 )       || (T_REn0 <0 ) || (T_FEn0 <=  T_REn0)
      ||   (T_FEn1 <0 )       || (T_REn1 <0 ) || (T_FEn1 <=  T_REn1)
      ||   (dT_Hub   <= 0 )
      ||   (dT_Schub <= 0 )
      ||   (ddT      <= 0 )
      ||   (x >= 1)           || (x < double(1)/REL_EPS) )

   {
      equalConstraintRhs[3] = 0;
      equalConstraintCoef[3]["Ex"]["H"]["0"]= 1;
      x = 2; // arbitrary value != 1, see constraint number 4
   }

  // technical constraints

 if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] <0)
        procApp->message(67, vertexId.the_string()+ " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"] * procInValMap["Count"]
          *(ddT  -(1-1/(1+G)/alpha * ddT_min))
          /(ddT_0-(1-1/(1+G)/alpha * ddT_min));
      lessConstraintCoef[1]["Ex"]["H"]["0"]= 1;
    }

  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"]= - x;
  equalConstraintCoef[2]["En"]["H"]["0"]= 1;

  if (procInValMap["Flag"] ==12 || procInValMap["Flag"] ==14)
    {
          // upgrading of waste heat or solar energy

          // (in case of upgrading environmental heat with flag=02 || 04
          //  there is no heat flow to be explicitely set zero)

     equalConstraintRhs[4] = 0;
     equalConstraintCoef[4]["Ex"]["H"]["0"]= x-1 ;
     equalConstraintCoef[4]["En"]["H"]["0"]= 1;
    }
}

//// showPower

Symbol NAHP::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
  showPowerType="H";
  showPowerNumber="0";
  return "Ex";
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWACog
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: Enthalpy adaptation by mixing valves, (waste-) heat dumping
//          and additional heating (similar to CCogConst)

//// NBWACog
// Standard Constructor

NBWACog::NBWACog(void)
{
  T_FEx=0;
  T_REx=0;
  T_FEn=0;
  T_REn=0;
  TF_Min=0;
  TF_Max=0;
  TR_Min=0;
  TR_Max=0;
}

//// NBWACog
// Constructor

NBWACog::NBWACog(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :NBW(cProcApp,procId,cProcType,cProcVal)
{
  T_FEx=0;
  T_REx=0;
  T_FEn=0;
  T_REn=0;

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");
  if (!procInValMap.element("eta_El"))
    procApp->message(55, vertexId.the_string()+ " eta_El");
  if (!procInValMap.element("eta_H"))
    procApp->message(55, vertexId.the_string()+ " eta_H");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

  if (procInValMap["eta_H"] == 0 ||
      procInValMap["Count"] == 0 ||
      procInValMap["eta_El"] == 0 )
    procApp->message(505, vertexId.the_string());

  if ((procInValMap["Flag"] != 14) && (procInValMap["Flag"] != 24)
      && ( procInValMap["Flag"] != 12))
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

  Symbol2 a = Symbol2("Fuel","0");
  En.insert(a);
  Symbol2 b = Symbol2("H","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  Ex.insert(c);
  Symbol2 d = Symbol2("El","0");
  Ex.insert(d);

  if (procApp->testFlag)
    procApp->message(1001, "NBWACog");
}

//// ~NBWACog
// Destructor

NBWACog::~NBWACog(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NBWACog");
    }
}

// calculate additional heat
void NBWACog::calcAddHeat(double T_1, double T_2)
{
  equalConstraintRhs[1] = 0;
  equalConstraintCoef[1]["Ex"]["H"]["0"]= (T_FEn-T_1)/(T_FEx-T_REx);
  equalConstraintCoef[1]["En"]["H"]["0"]= - 1;

  if (procInValMap.element("DotEEl_0"))
    {
      if (procInValMap["DotEEl_0"] <0)
        procApp->message(67, vertexId.the_string()+" DotEEl_0");

      lessConstraintRhs[3] = procInValMap["DotEEl_0"] * procInValMap["Count"];
      lessConstraintCoef[3]["Ex"]["El"]["0"]= 1;
    }

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"]= (T_FEx-T_2)/(T_FEx-T_REx);
  equalConstraintCoef[4]["En"]["Fuel"]["0"]= - procInValMap["eta_H"];

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["El"]["0"]= 1;
  equalConstraintCoef[5]["En"]["Fuel"]["0"]= - procInValMap["eta_El"];

  // abnormal situations

  if (   (T_FEx < 0 )       || (T_2 < 0 )      ||(T_FEx <= T_2)
      || (T_FEx <= T_2) // here T_2 (T_FEn or T_REx)
                        // plays the role of T_REx in CCogConst
      || (T_FEx < TF_Min)   || (T_FEx > TF_Max)
      || (T_2   < TR_Min)   || (T_2   > TR_Max))
    {
      cout << "Alarm NBWACog!" << endl;
      cout << "T_2 = " << T_2 << endl;
      cout << "T_FEx = " << T_FEx << endl;
      equalConstraintRhs[6] = 0;
      equalConstraintCoef[6]["Ex"]["H"]["0"]= 1;
    }

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

// constraints for no heating cases
void NBWACog::noLeavingHeat(void)
{
  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"]= 1;

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"]= -1;
  equalConstraintCoef[4]["En"]["Fuel"]["0"]= 1;

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["H"]["0"]=- 1;
  equalConstraintCoef[5]["Ex"]["El"]["0"]= 1;

}

void NBWACog::replace_calcAddHeat(void)
{
  equalConstraintRhs[7] = 0;
  equalConstraintCoef[7]["Ex"]["El"]["0"]= 1;
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWAAHP
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: Enthalpy adaptation by mixing valves, (waste-) heat dumping
//          and additional heating (similar to CCogConst)

//// NBWAAHP
// Standard Constructor

NBWAAHP::NBWAAHP(void)
{
  T_FEx=0;
  T_REx=0;
  T_FEn=0;
  T_REn=0;
}

//// NBWAAHP
// Constructor

NBWAAHP::NBWAAHP(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :NBW(cProcApp,procId,cProcType,cProcVal)
{
  T_FEx=0;
  T_REx=0;
  T_FEn=0;
  T_REn=0;

  if (!procInValMap.element("alpha"))
    procApp->message(55, vertexId.the_string()+ " alpha");
  if (!procInValMap.element("G"))
    procApp->message(55, vertexId.the_string()+ " G");
  if (!procInValMap.element("B"))
    procApp->message(55, vertexId.the_string()+ " B");
  if (!procInValMap.element("ddT_min"))
    procApp->message(55, vertexId.the_string()+ " ddT_min");
  if (!procInValMap.element("ddT_0"))
    procApp->message(55, vertexId.the_string()+ " ddT_0");

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");

  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

  if ((procInValMap["Flag"] != 14) && (procInValMap["Flag"] != 24)
      && ( procInValMap["Flag"] != 12))
    procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("H","1");
  En.insert(a);                  //heat source (from storage) (type 1 - flow)
  Symbol2 b = Symbol2("H","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","2");
  En.insert(c);                  //drive (type 1 - flow)
  Symbol2 d = Symbol2("H","0");
  Ex.insert(d);                  //supplied heat

  if (procApp->testFlag)
    procApp->message(1001, "NBWAAHP");
}

//// ~NBWAAHP
// Destructor

NBWAAHP::~NBWAAHP(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NBWAAHP");
    }
}

// calculate additional heat
void NBWAAHP::calcAddHeat(double T_1, double T_2)
{
  equalConstraintRhs[1] = 0;
  equalConstraintCoef[1]["Ex"]["H"]["0"]=
    (T_FEn-T_1)/(T_FEx-T_REx);
  equalConstraintCoef[1]["En"]["H"]["0"]= - 1;

  double T_FEn1=0; //Heat source
  double T_REn1=0;
// double T_HS  =0;

  double T_FEn2=0; //drive
  double T_REn2=0;

  //control of input

    // heat source

      if (vecJ["1"]["En"]["In"]["F"].element("T"))
        T_FEn1 = vecJ["1"]["En"]["In"]["F"]["T"];
      else
         procApp->message(54,vertexId.the_string()+" vecJ[1][En][In][F][T]");

      if (vecJ["1"]["En"]["In"]["R"].element("T"))
        T_REn1 = vecJ["1"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[1][En][In][R][T]");

    // drive (type 1 energy flow)

  if (vecJ["2"]["En"]["In"]["F"].element("T"))
    T_FEn2 = vecJ["2"]["En"]["In"]["F"]["T"];
  else
    procApp->message(54,vertexId.the_string()+" vecJ[2][En][In][F][T]");

  if (vecJ["2"]["En"]["In"]["R"].element("T"))
    T_REn2 = vecJ["2"]["En"]["In"]["R"]["T"];
  else
    procApp->message(54,vertexId.the_string()+" vecJ[2][En][In][R][T]");

  // calculate inverse efficiency `x'

  double dT_Hub   =0;
  double dT_Schub =0;
  double ddT      =0;
  double ddT_min  =0;

  double ddT_0    = procInValMap["ddT_0"];
  double B        = procInValMap["B"];
  double G        = procInValMap["G"];
  double alpha    = procInValMap["alpha"];

  double COP      =0;
  double x        =0;

  dT_Schub  = ( T_FEn2 + T_REn2 )/2.0  - ( T_FEx  + T_2  )/2.0;
            //  here T_2 (T_FEn or T_REx) plays the role of T_REx in NAWP
  dT_Hub    =   T_FEx                - ( T_FEn1 + T_REn1 )/2.0;

  ddT       = dT_Schub - B * dT_Hub;

  ddT_min   = procInValMap["ddT_min"];

   COP      = 1 +  (ddT-ddT_min) / (G*ddT- (G - 1/alpha) *ddT_min);

    x       = 1/COP;

  if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] <0)
        procApp->message(67, vertexId.the_string()+" DotEH_0");

      lessConstraintRhs[3] = procInValMap["DotEH_0"] * procInValMap["Count"]
          *(ddT  -(1-1/(1+G)/alpha * ddT_min))
          /(ddT_0-(1-1/(1+G)/alpha * ddT_min));
      lessConstraintCoef[3]["Ex"]["H"]["0"]=
        (T_FEx-T_2)/(T_FEx-T_REx);
    }

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"]= x * (T_FEx-T_2)/(T_FEx-T_REx);
  equalConstraintCoef[4]["En"]["H"]["2"]= -1;

 if  (     (T_FEx  <0 )       || (T_2  <0 ) || (T_FEx  <=  T_2 )
      ||   (T_FEx  <  T_2 ) // within NBWAAHP T_2 plays the role of T_REx in NAWP
      ||   (T_FEn2 <0 )       || (T_REn2 <0 ) || (T_FEn2 <=  T_REn2)
      ||   (T_FEn1 <0 )       || (T_REn1 <0 ) || (T_FEn1 <=  T_REn1)
      ||   (dT_Hub   <= 0 )
      ||   (dT_Schub <= 0 )
      ||   (ddT      <= 0 )
      ||   (x >= 1)           || (x < double(1)/REL_EPS) )

   {
      equalConstraintRhs[5] = 0;
      equalConstraintCoef[5]["Ex"]["H"]["0"]= 1;
      x = 2; // arbitrary value != 1, see constraint number 6
   }

  equalConstraintRhs[6] = 0;
  equalConstraintCoef[6]["Ex"]["H"]["0"]= (1-x) * (T_FEx-T_2)/(T_FEx-T_REx);
  equalConstraintCoef[6]["En"]["H"]["1"]= -1;
}

// constraints for no heating cases
void NBWAAHP::noLeavingHeat(void)
{
  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"]= 1;

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"]= -1;
  equalConstraintCoef[4]["En"]["H"]["1"]= 1;

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["H"]["0"]= -1;
  equalConstraintCoef[5]["En"]["H"]["2"]= 1;

}

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWAEHP
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: Enthalpy adaptation by mixing valves, (waste-) heat dumping
//          and additional heating (similar to NEHP)

//// NBWAEHP
// Standard Constructor

NBWAEHP::NBWAEHP(void)
{
  T_FEx=0;
  T_REx=0;
  T_FEn=0;
  T_REn=0;
}

//// NBWAEHP
// Constructor

NBWAEHP::NBWAEHP(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :NBW(cProcApp,procId,cProcType,cProcVal)
{
  T_FEx=0;
  T_REx=0;
  T_FEn=0;
  T_REn=0;

  if (!procInValMap.element("eta_G"))
    procApp->message(55, vertexId.the_string()+ " eta_G");
  if (!procInValMap.element("deltaT_Con"))
    procApp->message(55, vertexId.the_string()+ " deltaT_Con");
  if (!procInValMap.element("deltaT_Vap"))
    procApp->message(55, vertexId.the_string()+ " deltaT_Vap");
  if (!procInValMap.element("T_ConMax"))
    procApp->message(55, vertexId.the_string()+ " T_ConMax");
  if (!procInValMap.element("T_ConMin"))
    procApp->message(55, vertexId.the_string()+ " T_ConMin");
  if (!procInValMap.element("T_VapMax"))
    procApp->message(55, vertexId.the_string()+ " T_VapMax");
  if (!procInValMap.element("T_VapMin"))
    procApp->message(55, vertexId.the_string()+ " T_VapMin");

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");

  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

  if (procInValMap["eta_G"] <= 0 )
    procApp->message(505, vertexId.the_string());

  if (procInValMap["T_ConMax"] <= procInValMap["T_ConMin"])
    procApp->message(61, vertexId.the_string() + " T_ConMax,T_ConMin");

  if (procInValMap["T_VapMax"] <= procInValMap["T_VapMin"])
    procApp->message(61, vertexId.the_string() + " T_VapMax,T_VapMin");

  if ((procInValMap["Flag"] != 14) && (procInValMap["Flag"] != 24)
      && ( procInValMap["Flag"] != 12))
    procApp->message(61, vertexId.the_string() + " Flag");

  Symbol2 a = Symbol2("H","1");
  En.insert(a);                  //heat source (from storage) (type 1 - flow)
  Symbol2 b = Symbol2("El","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  Ex.insert(c);
  Symbol2 d = Symbol2("H","0");
  En.insert(d);

  if (procApp->testFlag)
    procApp->message(1001, "NBWAEHP");
}

//// ~NBWAEHP
// Destructor

NBWAEHP::~NBWAEHP(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NBWAEHP");
    }
}

// calculate additional heat
void NBWAEHP::calcAddHeat(double T_1, double T_2)
{
  equalConstraintRhs[1] = 0;
  equalConstraintCoef[1]["Ex"]["H"]["0"]=
    (T_FEn-T_1)/(T_FEx-T_REx);
  equalConstraintCoef[1]["En"]["H"]["0"]= - 1;

  if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] <0)
        procApp->message(67, vertexId.the_string()+" DotEH_0");

      lessConstraintRhs[3] =
        procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[3]["Ex"]["H"]["0"]=
        (T_FEx-T_2)/(T_FEx-T_REx);
    }

  double T_FCon=0;
  double T_HS=0;
  double T_FEn1=0;
//  double T_REn1=0; // does not affect efficiency of heat pump

  // calculate condenser temperature T_FCon
  T_FCon = T_FEx;

  //control of input for calculation of heat source temperature

      if (vecJ["1"]["En"]["In"]["F"].element("T"))
        T_FEn1 = vecJ["1"]["En"]["In"]["F"]["T"];
      else
         procApp->message(54,vertexId.the_string()+" vecJ[1][En][In][F][T]");

//      if (vecJ["1"]["En"]["In"]["R"].element("T")) // not really necessary
//      T_REn1 = vecJ["1"]["En"]["In"]["R"]["T"];  // since not used
//      else
//      procApp->message(54,vertexId.the_string()+" vecJ[1][En][In][R][T]");

  T_HS = T_FEn1;

 // calculate inverse efficiency
  double x;

  x = T_FCon +  procInValMap["deltaT_Con"];
  x = (x - T_HS + procInValMap["deltaT_Vap"])/x;
  x = x /procInValMap["eta_G"];

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"]=  x * (T_FEx-T_2)/(T_FEx-T_REx);
  equalConstraintCoef[4]["En"]["El"]["0"]= -1;

  if (     (T_FEx <0 )       || (T_2 <0 ) ||(T_FEx <= T_2)
      || (T_FEx <= T_2)
       // T_2 (=T_FEn or T_REx) here plays the role of T_REx in NEHP
       //      ||   (T_FEn <0 )       || (T_REn <0 ) ||(T_FEn <= T_REn)
      || (T_FCon+procInValMap["deltaT_Con"] < T_HS-procInValMap["deltaT_Vap"])
      || (x >= 1)    || (x < double(1)/REL_EPS)
      || (T_FCon+procInValMap["deltaT_Con"] < procInValMap["T_ConMin"])
      || (T_FCon+procInValMap["deltaT_Con"] > procInValMap["T_ConMax"])
      || (T_HS-procInValMap["deltaT_Vap"]   < procInValMap["T_VapMin"])
      || (T_HS-procInValMap["deltaT_Vap"]   > procInValMap["T_VapMax"]))
    {
      cout << "Alarm NBWAEHP!!!!" << endl;
      cout << "T_2 = " << T_2 << endl;
      cout << "T_FEx = " << T_FEx << endl;
      cout << "T_FCon+deltaT_Con = " << T_FCon+procInValMap["deltaT_Con"] << endl;
      cout << "T_HS-deltaT_Vap = " << T_HS-procInValMap["deltaT_Vap"] << endl;
      equalConstraintRhs[5] = 0;
      equalConstraintCoef[5]["Ex"]["H"]["0"]= 1;
      x = 2; // arbitrary value != 1, see constraint number 6
    }

  equalConstraintRhs[6] = 0;
  equalConstraintCoef[6]["Ex"]["H"]["0"]= (1-x) * (T_FEx-T_2)/(T_FEx-T_REx);
  equalConstraintCoef[6]["En"]["H"]["1"]= -1;
}

// constraints for no heating cases
void NBWAEHP::noLeavingHeat(void)
{
  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"]= 1;

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"]= -1;
  equalConstraintCoef[4]["En"]["H"]["0"]= 1;

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["H"]["0"]=- 1;
  equalConstraintCoef[5]["En"]["El"]["0"]= 1;

  equalConstraintRhs[6] = 0;
  equalConstraintCoef[6]["Ex"]["H"]["0"]= -1;
  equalConstraintCoef[6]["En"]["H"]["1"]= 1;
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NBWAGHP
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: Enthalpy adaptation by mixing valves, (waste-) heat dumping
//          and additional heating (similar to NGHP)

//// NBWAGHP
// Standard Constructor

NBWAGHP::NBWAGHP(void)
{
//  double T_REx=0;
//  double T_FEx=0;
//  double T_REn=0;
//  double T_FEn=0;
}

//// NBWAGHP
// Constructor

NBWAGHP::NBWAGHP(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :NBW(cProcApp,procId,cProcType,cProcVal)
{
//  double T_REx=0;
//  double T_FEx=0;
//  double T_REn=0;
//  double T_FEn=0;

  if (!procInValMap.element("eta_G"))
    procApp->message(55, vertexId.the_string()+ " eta_G");
  if (!procInValMap.element("deltaT_Con"))
    procApp->message(55, vertexId.the_string()+ " deltaT_Con");
  if (!procInValMap.element("deltaT_Vap"))
    procApp->message(55, vertexId.the_string()+ " deltaT_Vap");
  if (!procInValMap.element("T_ConMax"))
    procApp->message(55, vertexId.the_string()+ " T_ConMax");
  if (!procInValMap.element("T_ConMin"))
    procApp->message(55, vertexId.the_string()+ " T_ConMin");
  if (!procInValMap.element("T_VapMax"))
    procApp->message(55, vertexId.the_string()+ " T_VapMax");
  if (!procInValMap.element("T_VapMin"))
    procApp->message(55, vertexId.the_string()+ " T_VapMin");
  if (!procInValMap.element("eta_Mech"))
    procApp->message(55, vertexId.the_string()+ " eta_Mech");
  if (!procInValMap.element("a"))
    procApp->message(55, vertexId.the_string()+ " a");
  if (!procInValMap.element("lambdaEl_H"))
    procApp->message(55, vertexId.the_string()+ " lambdaEl_H");

  if (procInValMap["eta_G"] <= 0)
    procApp->message(505, vertexId.the_string());

  if (procInValMap["T_ConMax"] <= procInValMap["T_ConMin"])
    procApp->message(61, vertexId.the_string() + " T_ConMax,T_ConMin");

  if (procInValMap["T_VapMax"] <= procInValMap["T_VapMin"])
    procApp->message(61, vertexId.the_string() + " T_VapMax,T_VapMin");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");

  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("Fuel","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  En.insert(c);
  Symbol2 d = Symbol2("H","0");
  Ex.insert(d);
  Symbol2 e = Symbol2("H","1"); //Quelle
  En.insert(e);

  if (procApp->testFlag)
    procApp->message(1001, "NBWAGHP");

}

//// ~NBWAGHP
// Destructor

NBWAGHP::~NBWAGHP(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NBWAGHP");
    }
}

// calculate additional heat
void NBWAGHP::calcAddHeat(double T_1, double T_2)
{
  equalConstraintRhs[1] = 0;
  equalConstraintCoef[1]["Ex"]["H"]["0"]=
    (T_FEn-T_1)/(T_FEx-T_REx);
  equalConstraintCoef[1]["En"]["H"]["0"]= - 1;

  double T_FEn1=0; //Heat source
  double T_REn1=0;
  double T_FCon=0;
  double T_HS=0;

  //control of input

    // heat source

      if (vecJ["1"]["En"]["In"]["F"].element("T"))
        T_FEn1 = vecJ["1"]["En"]["In"]["F"]["T"];
      else
         procApp->message(54,vertexId.the_string()+" vecJ[1][En][In][F][T]");

      if (vecJ["1"]["En"]["In"]["R"].element("T"))
        T_REn1 = vecJ["1"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[1][En][In][R][T]");

  // calculate heat source temperature
  if (procInValMap["Flag"] ==12 || procInValMap["Flag"] ==14)
    T_HS= T_REn1; //the coldest temperature of the heat source
  if (procInValMap["Flag"] ==24 || procInValMap["Flag"] ==22)
    T_HS = T_FEn1; //to simulate an evaporator in a heat storage

  // calculate condenser temperature T_FCon
  double y=0;
  double B=0;
  double C=0;
  double gamma=0;

  gamma = procInValMap["eta_G"]*procInValMap["eta_Mech"]/procInValMap["a"];

  B =  procInValMap["deltaT_Con"] - T_HS +   procInValMap["deltaT_Vap"];
  B = B - T_REx - gamma * T_FEx + gamma *  procInValMap["deltaT_Con"];

  C = - T_REx*(procInValMap["deltaT_Con"]-T_HS +  procInValMap["deltaT_Vap"]);
  C = C - gamma * T_FEx *procInValMap["deltaT_Con"];
  y = B*B - 4 * (1+gamma) * C;
  if (y>0)
    {
      T_FCon = (-B + sqrt(y))/(2*(1+gamma));
      if (T_FCon > T_FEx || T_FCon < T_REx)
        {
          T_FCon =  (-B - sqrt(y))/(2*(1+gamma));
          if (T_FCon > T_FEx || T_FCon < T_REx)
            procApp->message(70, vertexId.the_string()+
                             " T_FCon in  actualSimplexInput of NGHP-Module");
        }
    }
  else
    procApp->message(65, vertexId.the_string()+
                     " y in  actualSimplexInput of NBWAGHP-Module");

  if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] <0)
        procApp->message(67, vertexId.the_string()+" DotEH_0");

      lessConstraintRhs[3] =
        procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[3]["Ex"]["H"]["0"]=
        (T_FEx-T_2)/(T_FEx-T_REx);
    }

  // calculate inverse efficiency
  double x;
  double z;

  x = T_FCon +  procInValMap["deltaT_Con"];
  x = (x - T_HS + procInValMap["deltaT_Vap"])/x;
  x = x /procInValMap["eta_G"];

  if (     (T_FEx <0 )       || (T_REx <0 ) ||(T_FEx <= T_REx)
      ||   (T_FEn <0 )       || (T_REn <0 ) ||(T_FEn <= T_REn)
      || (T_FCon+procInValMap["deltaT_Con"] < T_HS-procInValMap["deltaT_Vap"])
      || (x >= 1)    || (x < double(1)/REL_EPS)
      || (T_FCon+procInValMap["deltaT_Con"] < procInValMap["T_ConMin"])
      || (T_FCon+procInValMap["deltaT_Con"] > procInValMap["T_ConMax"])
      || (T_HS-procInValMap["deltaT_Vap"]   < procInValMap["T_VapMin"])
      || (T_HS-procInValMap["deltaT_Vap"]   > procInValMap["T_VapMax"]))
    {
      equalConstraintRhs[2] = 0;
      equalConstraintCoef[2]["Ex"]["H"]["0"]= 1;
      x = 2; // arbitrary value != 1
    }
  else
    {
      z = (procInValMap["eta_Mech"]/x) + procInValMap["a"];
      if (z < double(1)/REL_EPS)
        {
          equalConstraintRhs[2] = 0;
          equalConstraintCoef[2]["Ex"]["H"]["0"]= 1;
          z = 2; // arbitrary value != 1
        }
      else
        z = 1/z;
    }

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"]= - z * ((T_FEx-T_2)/(T_FEx-T_REx));
  equalConstraintCoef[4]["En"]["Fuel"]["0"]= 1;

  equalConstraintRhs[6] = 0;
  equalConstraintCoef[6]["Ex"]["H"]["0"]= -z*(1/x -1)*procInValMap["eta_Mech"]
      * ((T_FEx-T_2)/(T_FEx-T_REx));
  equalConstraintCoef[6]["En"]["H"]["1"]= 1;

  equalConstraintRhs[7] = 0;
  equalConstraintCoef[7]["Ex"]["H"]["0"]= - procInValMap["lambdaEl_H"]
                    * ((T_FEx-T_2)/(T_FEx-T_REx));
  equalConstraintCoef[7]["En"]["El"]["0"]= 1;

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

// constraints for no heating cases
void NBWAGHP::noLeavingHeat(void)
{
  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"]= 1;

  equalConstraintRhs[4] = 0;
  equalConstraintCoef[4]["Ex"]["H"]["0"]= -1;
  equalConstraintCoef[4]["En"]["Fuel"]["0"]= 1;

  equalConstraintRhs[5] = 0;
  equalConstraintCoef[5]["Ex"]["H"]["0"]=- 1;
  equalConstraintCoef[5]["En"]["El"]["0"]= 1;

  equalConstraintRhs[6] = 0;
  equalConstraintCoef[6]["Ex"]["H"]["0"]=- 1;
  equalConstraintCoef[6]["En"]["H"]["0"]= 1;

  equalConstraintRhs[7] = 0;
  equalConstraintCoef[7]["Ex"]["H"]["0"]=- 1;
  equalConstraintCoef[7]["En"]["H"]["1"]= 1;
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NHTransSol
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: Transportation of enthalpy from collector to storage
//
// Like NHTrans, but without equalconstraint[2] in actualSimplexInput.
// This constraint may be inconsistent with equalconstraint[1] of OSol.
// (Due to heat losses (T_FEx <= T_REx) may occur; as a consequence
//  E_HEx is set to zero; in this case E_HEn, coming from OSol, should
//  NOT be set to zero, because in OSol a positive energy flow
//  may have been generated.)

//// NHTransSol
// Standard Constructor

NHTransSol::NHTransSol(void)
{
  T_FEn=0;
  T_REn=0;
  T_FEx=0;
  T_REx=0;
  T_Env=0;
  AkOverMc=0;
}

//// NHTransSol
// Constructor

NHTransSol::NHTransSol(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  T_FEn=0;
  T_REn=0;
  T_FEx=0;
  T_REx=0;
  T_Env=0;
  AkOverMc=0;

  if (!procInValMap.element("eta_0"))
    procApp->message(55, vertexId.the_string()+ " eta_0");
  if (!procInValMap.element("TF_0"))
    procApp->message(55, vertexId.the_string()+ " TF_0");
  if (!procInValMap.element("TR_0"))
    procApp->message(55, vertexId.the_string()+ " TR_0");
  if (!procInValMap.element("T_Env"))
    procApp->message(55, vertexId.the_string()+ " T_Env");
  if (!procInValMap.element("p_El"))
    procApp->message(55, vertexId.the_string()+ " p_El");
  if (!procInValMap.element("c_p"))
    procApp->message(55, vertexId.the_string()+ " c_p");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");

  if (procInValMap["eta_0"] == 0 ||
      procInValMap["c_p"] == 0 )
    procApp->message(56, vertexId.the_string());

  if ((procInValMap["Flag"] != 11) && (procInValMap["Flag"] != 22)
      && (procInValMap["Flag"] != 33) && (procInValMap["Flag"] != 44))
    procApp->message(61, vertexId.the_string() + " Flag");

  T_Env = procInValMap["T_Env"];

  double x;
  x = procInValMap["TF_0"]+procInValMap["TR_0"]-(2 *procInValMap["T_Env"]);
  if (x <= 0)
    procApp->message(61, vertexId.the_string() + " TF_0,TR_0,T_Env");

  x = (procInValMap["TF_0"] -  procInValMap["TR_0"])/x;
  AkOverMc= x * (1- procInValMap["eta_0"]);
  if (AkOverMc > 0.1)
    procApp->message(61, vertexId.the_string() + " AkOverMc is too large ");
  // assumption: AkOverMc << 1

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("H","0");
  En.insert(b);
  Symbol2 c = Symbol2("H","0");
  Ex.insert(c);

  if (procApp->testFlag)
    procApp->message(1001, "NHTransSol");
}

//// ~NHTransSol
// Destructor

NHTransSol::~NHTransSol(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NHTransSol");
    }
}

//// actualExJ
// actualize attributes of exit side

void NHTransSol::actualExJ(const Map<Symbol,double> & pVecU)
{
  if (procInValMap["Flag"] == 11)
    {
      if (vecJ["0"]["En"]["In"]["F"].element("T"))
        T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][En][In][F][T]");
      if (vecJ["0"]["En"]["In"]["R"].element("T"))
        T_REn = vecJ["0"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][En][In][R][T]");
      T_FEx = T_Env + (T_FEn - T_Env) * (1-  AkOverMc);
      T_REx = T_Env + (T_REn - T_Env) * (1+  AkOverMc);
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
      vecJ["0"]["Ex"]["Out"]["R"]["T"] = T_REx;
    }

 if (procInValMap["Flag"] == 22)
    {
      if (vecJ["0"]["En"]["In"]["F"].element("T"))
        T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][En][In][F][T]");

      T_FEx = T_Env + (T_FEn - T_Env) * (1-  AkOverMc);
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
    }

  if (procInValMap["Flag"] == 33)
    {
      if (vecJ["0"]["En"]["In"]["R"].element("T"))
        T_REn = vecJ["0"]["En"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][En][In][R][T]");
      T_REx = T_Env + (T_REn - T_Env) * (1+  AkOverMc);
      vecJ["0"]["Ex"]["Out"]["R"]["T"] = T_REx;
    }
}

//// actualEnJ
// actualize attributes of entrance side

void NHTransSol::actualEnJ(const Map<Symbol,double> & pVecU)
{
  if (procInValMap["Flag"] == 44)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][F][T]");
      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
        T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");
      T_FEn = T_Env + (T_FEx - T_Env) * (1+  AkOverMc);
      T_REn = T_Env + (T_REx - T_Env) * (1-  AkOverMc);
      vecJ["0"]["En"]["Out"]["F"]["T"] = T_FEn;
      vecJ["0"]["En"]["Out"]["R"]["T"] = T_REn;
    }

 if (procInValMap["Flag"] == 22)
    {
      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
        T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");
      T_REn = T_Env + (T_REx - T_Env) * (1-  AkOverMc);
      vecJ["0"]["En"]["Out"]["R"]["T"] = T_REn;
    }

 if (procInValMap["Flag"] == 33)
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][F][T]");
      T_FEn = T_Env + (T_FEx - T_Env) * (1+  AkOverMc);
      vecJ["0"]["En"]["Out"]["F"]["T"] = T_FEn;
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void NHTransSol::actualSimplexInput(const Map<Symbol,double> & pVecU,
                               double actualIntLength)
{
  if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] <0)
        procApp->message(67, vertexId.the_string()+ " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"];
      lessConstraintCoef[1]["Ex"]["H"]["0"]= 1;
    }

  //control of input

  if ( (T_FEn <= T_REn) ||  (T_FEx <= T_REx) ||
      (T_FEx <0 ) || (T_FEn <0 ) ||(T_REx <0 ) || (T_REn <0 ))
    {
      equalConstraintRhs[4] = 0;
      equalConstraintCoef[4]["Ex"]["H"]["0"]= 1;
      //   equalConstraintRhs[2] = 0;
      //   equalConstraintCoef[2]["Ex"]["H"]["0"]= - 1; // no heat dumping in this case
      //   equalConstraintCoef[2]["En"]["H"]["0"]= 1;
      equalConstraintRhs[3] = 0;
      equalConstraintCoef[3]["Ex"]["H"]["0"]= - 1;
      equalConstraintCoef[3]["En"]["El"]["0"]= 1;
    }
  else
    {

      // technical constraints

      double x;
      x = (T_FEn - T_REn) / (T_FEx - T_REx);

      equalConstraintRhs[2] = 0;
      equalConstraintCoef[2]["Ex"]["H"]["0"]= - x;
      equalConstraintCoef[2]["En"]["H"]["0"]= 1;

      x = procInValMap["p_El"] / procInValMap["c_p"];
      x = x / ( T_FEx - T_REx);

      equalConstraintRhs[3] = 0;
      equalConstraintCoef[3]["Ex"]["H"]["0"]= - x;
      equalConstraintCoef[3]["En"]["El"]["0"]= 1;

    }
}
//// showPower

Symbol NHTransSol::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
  showPowerType="H";
  showPowerNumber="0";
  return "Ex";
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NHEx24
//
///////////////////////////////////////////////////////////////////

// Module author: Kathrin Ramsel
// Development thread: S05
//
// Summary: heat exchanger with time constant temperature difference,
//          no pressure loss and maximal exergy efficiency

//// NHEx24
// Standard Constructor

NHEx24::NHEx24(void)
{
  T_FEx=0;
  T_REx=0;
  T_FEn=0;
  T_REn=0;
}

//// NHEx24
// Constructor

NHEx24::NHEx24(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  T_FEx=0;
  T_REx=0;
  T_FEn=0;
  T_REn=0;

  if (!procInValMap.element("deltaT"))
    procApp->message(55, vertexId.the_string()+ " deltaT");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

  if ((procInValMap["Flag"] != 11) && (procInValMap["Flag"] != 22)
      && ( procInValMap["Flag"] != 33) && ( procInValMap["Flag"] != 44)
      && ( procInValMap["Flag"] != 24))
    procApp->message(61, vertexId.the_string() + " Flag");

  if (procInValMap["deltaT"] <= 0)
    procApp->message(67, vertexId.the_string() + " deltaT");

  Symbol2 a = Symbol2("H","0");
  En.insert(a);
  Symbol2 b = Symbol2("H","0");
  Ex.insert(b);

  if (procApp->testFlag)
    procApp->message(1001, "NHEx24");
}

//// ~NHEx24
// Destructor

NHEx24::~NHEx24(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NHEx24");
    }
}

//// actualExJ
// actualize attributes of exit side

void NHEx24::actualExJ(const Map<Symbol,double> & pVecU)
{
  if ((procInValMap["Flag"] == 11) || (procInValMap["Flag"] == 22)) //control of input
    {
      if (vecJ["0"]["En"]["In"]["F"].element("T"))
        T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][En][In][F][T]");

      T_FEx = T_FEn - procInValMap["deltaT"] ;
      vecJ["0"]["Ex"]["Out"]["F"]["T"] = T_FEx;
    }

  if ((procInValMap["Flag"] == 11) || (procInValMap["Flag"] == 33))
    {
       if (vecJ["0"]["En"]["In"]["R"].element("T"))
         T_REn = vecJ["0"]["En"]["In"]["R"]["T"];
       else
         procApp->message(54,vertexId.the_string()+" vecJ[0][En][In][R][T]");

       T_REx = T_REn - procInValMap["deltaT"] ;
       vecJ["0"]["Ex"]["Out"]["R"]["T"] = T_REx;
    }
}

//// actualEnJ
// actualize attributes of entrance side

void NHEx24::actualEnJ(const Map<Symbol,double> & pVecU)
{
  if ((procInValMap["Flag"] == 44) || (procInValMap["Flag"] == 22)
                          || (procInValMap["Flag"] == 24))
    {
      if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
          T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      else
          procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");

      T_REn = T_REx + procInValMap["deltaT"] ;
      vecJ["0"]["En"]["Out"]["R"]["T"] = T_REn;
    }

  if ((procInValMap["Flag"] == 33) || (procInValMap["Flag"] == 44))
    {
      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
          T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
          procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][F][T]");

      T_FEn = T_FEx + procInValMap["deltaT"] ;
      vecJ["0"]["En"]["Out"]["F"]["T"] = T_FEn;
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void NHEx24::actualSimplexInput(const Map<Symbol,double> & pVecU,
                                    double actualIntLength)
{
// control of input

 if (procInValMap["Flag"] == 24)
    {
      if (vecJ["0"]["En"]["In"]["F"].element("T"))
        T_FEn = vecJ["0"]["En"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][En][In][F][T]");

      if (vecJ["0"]["Ex"]["In"]["F"].element("T"))
        T_FEx = vecJ["0"]["Ex"]["In"]["F"]["T"];
      else
        procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][F][T]");
    }

// technical constraints

 if (procInValMap.element("DotEH_0"))
    {
      if (procInValMap["DotEH_0"] <0)
          procApp->message(67, vertexId.the_string()+ " DotEH_0");

      lessConstraintRhs[1] = procInValMap["DotEH_0"] * procInValMap["Count"];
      lessConstraintCoef[1]["Ex"]["H"]["0"]= 1;
    }

  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["Ex"]["H"]["0"]= 1;
  equalConstraintCoef[2]["En"]["H"]["0"]= -1;

 if (procInValMap["Flag"] == 24)
   {
    if(vecJ["0"]["En"]["In"]["F"]["T"] > vecJ["0"]["Ex"]["In"]["F"]["T"])
      {
       equalConstraintRhs[3] = 0;
       equalConstraintCoef[3]["Ex"]["H"]["0"]= 1;
       equalConstraintCoef[3]["En"]["H"]["0"]= -1;
      }
    else
      {
       equalConstraintRhs[3] = 0;
       equalConstraintCoef[3]["Ex"]["H"]["0"]= 1;
      }
   }

 if ((T_FEn < (T_REx +  procInValMap["deltaT"])) ||
     (T_REn > (T_FEx +  procInValMap["deltaT"])) ||
     (T_FEn <0 ) || (T_REn <0 ) || (T_FEx <0 ) || (T_REx <0 ) ||
     (T_FEn <= T_REn) || (T_FEx <= T_REx) || (T_FEn < (T_FEx + procInValMap["deltaT"])))
    {
     equalConstraintRhs[4] = 0;
     equalConstraintCoef[4]["Ex"]["H"]["0"]= 1;
    }
}

//// showPower

Symbol NHEx24::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
  showPowerType="H";
  showPowerNumber="0";
  return "Ex";
}

///////////////////////////////////////////////////////////////////
//
// CLASS: NStMan
//
///////////////////////////////////////////////////////////////////

// Module author: Kathrin Ramsel
// Development thread: S05
//
// Summary: Waermeschiene

//// NStMan
// Standard Constructor

NStMan::NStMan(void)
{
}

//// NStMan
// Constructor

NStMan::NStMan(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  if (!procInValMap.element("TF_0"))
    procApp->message(55, vertexId.the_string()+ " TF_0");
  if ((procInValMap["Flag"] == 44) && (!procInValMap.element("eps")))
    procApp->message(55, vertexId.the_string()+ " eps");
  if (!procInValMap.element("DotEH_loss"))
    procInValMap["DotEH_loss"]=0;
  if (!procInValMap.element("eta"))
    procInValMap["eta"]=1;
  if (!procInValMap.element("l_0"))
    procInValMap["l_0"]=1;
  if (!procInValMap.element("k_0"))
    procInValMap["k_0"]=1;
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");
  if ((procInValMap["Flag"] != 42) && (procInValMap["Flag"] != 44))
    procApp->message(61, vertexId.the_string() + " Flag");
  if ((procInValMap["Flag"] == 42) && (procInValMap["l_0"] != 1))
    procApp->message(41, vertexId.the_string()+ " l_0");
  if ((procInValMap.element("sVarM_FI")) && ((!procInValMap.element("Delta_h_HD"))
        || (!procInValMap.element("h_1")) || (!procInValMap.element("h_2"))))
    procApp->message(55, vertexId.the_string()+ " Delta_h, h");
  for(int k=1; k <=  procInValMap["k_0"]; k++)
    {
      Symbol symK = int_to_str(k);
      Symbol2 a = Symbol2("H",symK);
      En.insert(a);
    }

  if (procInValMap["Flag"] == 44)
   {
     for(int l=1; l <=  procInValMap["l_0"]; l++)
       {
         Symbol symL = int_to_str(l);
         Symbol2 b = Symbol2("H",symL);
         Ex.insert(b);
       }
   }

  if (procInValMap["Flag"] == 42)
   {
     Symbol2 b = Symbol2("H","0");
     Ex.insert(b);
   }

  if (procApp->testFlag)
    procApp->message(1001, "NStMan");
}

//// ~NStMan
// Destructor

NStMan::~NStMan(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "NStMan");
    }
}

//void NStMan::updateProcOutTsPack(double  actualIntLength, double intLength)
//{
//if (procInValMap["Flag"] == 44)
//{
//  // update state variable mean values, show DotEH_Ex
//  String outputId;
//  outputId = "DotEH_En";
// for(int k=1 ; k <=  procInValMap["k_0"] ; k++ )
//   {
//    Symbol symK = int_to_str(k);
//    procOutTsPack[outputId]= procOutTsPack[outputId]+
//                   ((dotEEn["H"][symK]) * (actualIntLength/intLength));
//   }
//}

//if (procInValMap["Flag"] == 42)
// {
//  // update state variable mean values, show DotEH_Ex
//   String outputId;
//   outputId = "DotEH_Ex";
//   procOutTsPack[outputId]= procOutTsPack[outputId]+
//                  ((dotEEx["H"]["0"]) * (actualIntLength/intLength));
// }
//}

//// actualExJ
// actualize attributes of exit side

void NStMan::actualExJ(const Map<Symbol,double> & pVecU)
{
 if (procInValMap["Flag"] == 42)
   vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"];
}

//// actualEnJ
// actualize attributes of entrance side

void NStMan::actualEnJ(const Map<Symbol,double> & pVecU)
{
int delta1=0;
int delta2=0;
int SumTR=0;
int TR_0=0;

  if (procInValMap["Flag"] == 44)
   {
  // Ruecklauftemperatur bestimmen (Mittelwert)
   {
   for(int l=1 ; l <=  procInValMap["l_0"] ; l++ )
     {
       Symbol symL = int_to_str(l);
       SumTR = SumTR + vecJ[symL]["Ex"]["In"]["R"]["T"];
     }
   }
       TR_0 = SumTR / procInValMap["l_0"];

   for(int l=1 ; l <=  procInValMap["l_0"] ; l++ )
     {
      Symbol symL = int_to_str(l);
        delta1 = vecJ[symL]["Ex"]["In"]["R"]["T"] - TR_0;
        delta2 = TR_0 - vecJ[symL]["Ex"]["In"]["R"]["T"];
      if ((delta1 > procInValMap["eps"]) || (delta2 > procInValMap["eps"]))
        procApp->message(71,vertexId.the_string()+" TR_0");
     }

   for(int k=1 ; k <=  procInValMap["k_0"] ; k++ )
     {
       Symbol symK = int_to_str(k);
       vecJ[symK]["En"]["Out"]["F"]["T"] = procInValMap["TF_0"] ;
       vecJ[symK]["En"]["Out"]["R"]["T"] = TR_0;
     }
   }

  if (procInValMap["Flag"] == 42)
    {
    for(int k=1 ; k <=  procInValMap["k_0"] ; k++ )
      {
        Symbol symK = int_to_str(k);
        vecJ[symK]["En"]["Out"]["F"]["T"] = procInValMap["TF_0"] ;

        if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
           vecJ[symK]["En"]["Out"]["R"]["T"] = vecJ["0"]["Ex"]["In"]["R"]["T"];
        else
          procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");
      }
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void NStMan::actualSimplexInput(const Map<Symbol,double> & pVecU,
                               double actualIntLength)
{
int delta1=0;
int delta2=0;

 if (procInValMap["Flag"] == 44)
 {
     for(int l=1; l <=  procInValMap["l_0"]; l++)
       {
         Symbol symL = int_to_str(l);
          delta1 = vecJ[symL]["Ex"]["In"]["F"]["T"] - procInValMap["TF_0"];
          delta2 = procInValMap["TF_0"] - vecJ[symL]["Ex"]["In"]["F"]["T"];
         if ((delta1 > procInValMap["eps"]) || (delta2 > procInValMap["eps"]))
          procApp->message(71,vertexId.the_string()+" TF_0");
       }

     {
      equalConstraintRhs[1] = procInValMap["DotEH_loss"];
      for(int k=1; k <=  procInValMap["k_0"]; k++)
       {
        Symbol symK = int_to_str(k);
        equalConstraintCoef[1]["En"]["H"][symK] = 1;
       }
      for(int l=1; l <=  procInValMap["l_0"]; l++)
       {
        Symbol symL = int_to_str(l);
        equalConstraintCoef[1]["Ex"]["H"][symL] = -1.0/procInValMap["eta"];
       }
     }
 }

 if (procInValMap["Flag"] == 42)
 {
     equalConstraintRhs[1] = procInValMap["DotEH_loss"];
     equalConstraintCoef[1]["Ex"]["H"]["0"] = -1.0/procInValMap["eta"];
     for(int k=1; k <=  procInValMap["k_0"]; k++)
      {
       Symbol symK = int_to_str(k);
       equalConstraintCoef[1]["En"]["H"][symK] = 1;
      }
 }

 if (procInValMap.element("DotEH_0"))
 {
    if (procInValMap["DotEH_0"] <0)
        procApp->message(67, vertexId.the_string()+ " DotEH_0");

    lessConstraintRhs[2] = procInValMap["DotEH_0"];
    for(int l=1; l <=  procInValMap["l_0"]; l++)
      {
       Symbol symL = int_to_str(l);
       lessConstraintCoef[2]["Ex"]["H"][symL] = 1;
      }
 }

//cogeneration feed-in law refund
 if(procInValMap.element("sVarM_FI"))
   {
    for(int k=1; k <=  procInValMap["k_0"]; k++)
       {
        Symbol symK = int_to_str(k);
        objectFuncCoef["M"]["En"]["H"][symK]= ((procInValMap["h_1"]-procInValMap["h_2"])/
                                              procInValMap["Delta_h_HD"])*procInValMap["sVarM_FI"];
       }

       if(procInValMap["Flag"] == 42)
       objectFuncCoef["M"]["Ex"]["H"]["0"]= -((procInValMap["h_1"]-procInValMap["h_2"])/
                                              procInValMap["Delta_h_HD"])*procInValMap["sVarM_FI"];

       if(procInValMap["Flag"] == 44)
          {
          for(int l=1; l <=  procInValMap["l_0"]; l++)
             {
              Symbol symL = int_to_str(l);
              objectFuncCoef["M"]["Ex"]["H"][symL]= -((procInValMap["h_1"]-procInValMap["h_2"])/
                                                      procInValMap["Delta_h_HD"])*procInValMap["sVarM_FI"];
             }
          }
   }
}

//// showPower

Symbol NStMan::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
  if (procInValMap["l_0"] > 1)
    procApp->message(64,vertexId.the_string()+" showPower");

  showPowerType="H";
  showPowerNumber="1";
  return "Ex";
}

// end of file
