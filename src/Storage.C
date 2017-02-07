
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
//  $Date: 2005/09/30 18:54:39 $
//  $Author: deeco $
//  $RCSfile: Storage.C,v $

//////////////////////////////////////////////////////////////////
//
// DEECO STORAGE-PROCESS MODULES
//
//////////////////////////////////////////////////////////////////

#include "Storage.h"
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

//// SSenH
// Standard Constructor
//
SSenH::SSenH(void)
{
}

//// SSenH
// Constructor
//
SSenH::SSenH(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) : Proc(cProcApp, procId,
             cProcType, cProcVal)
{
  GeoF = 5.5358104;   // geometric factor for converting
                      // volume in surface area (diameter = height)
  A  = 0;
  Mc = 0;
  x  = 0;

  if (!procInValMap.element("l_0"))
    procInValMap["l_0"]=1;
  if (!procInValMap.element("k_0"))
    procInValMap["k_0"]=1;
  if (!procInValMap.element("T_0"))
    procApp->message(55, vertexId.the_string() + " T_0");
  if (!procInValMap.element("T_Min"))
    procApp->message(55, vertexId.the_string() + " T_Min");
  if (!procInValMap.element("T_Max"))
    procApp->message(55, vertexId.the_string() + " T_Max");
  if (!procInValMap.element("T_Env"))
    procApp->message(55, vertexId.the_string() + " T_Env");
  if (!procInValMap.element("T_I"))
    procApp->message(55, vertexId.the_string() + " T_I");
  if (!procInValMap.element("deltaT"))
    procApp->message(55, vertexId.the_string() + " deltaT");
  if (!procInValMap.element("V"))
    procApp->message(55, vertexId.the_string() + " V");
  if (!procInValMap.element("c_p"))
    procApp->message(55, vertexId.the_string() + " c_p");
  if (!procInValMap.element("Rho"))
    procApp->message(55, vertexId.the_string() + " Rho");
  if (!procInValMap.element("k"))
    procApp->message(55, vertexId.the_string() + " k");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["V"] == 0   ||
      procInValMap["Rho"] == 0 ||
      procInValMap["c_p"] == 0 ||
      procInValMap["Count"] == 0 )
    procApp->message(505, vertexId.the_string());
  if (procInValMap["T_0"] > procInValMap["T_Min"])
    procApp->message(61, vertexId.the_string() + ": T_0,T_Min");
  if (procInValMap["T_I"] < procInValMap["T_Min"])
    procApp->message(61, vertexId.the_string() + ": T_I,T_Min");

  for(int l = 1; l <= procInValMap["l_0"]; l++)
    {
      Symbol symL = int_to_str(l);
      Symbol2 a   = Symbol2("H",symL);
      Ex.insert(a);
    }

  for(int k = 1; k <= procInValMap["k_0"]; k++)
    {
      Symbol symK = int_to_str(k);
      Symbol2 a = Symbol2("H",symK);
      En.insert(a);
    }

  Mc = procInValMap["V"] * procInValMap["Rho"] * procInValMap["c_p"];

  double y;
  y = double(2)/double(3);
  A = pow(procInValMap["V"],y);
  A = A * GeoF;                    // surface area (diameter = height)

  initState();

  if (procApp->testFlag)
    procApp->message(1001, "SSenH");
}

//// ~SSenH
// Destructor
//
SSenH::~SSenH(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "SSenH");
    }
}

////  updateProcOutTsPack
// update procOutTsPack after each optimization
//
void SSenH::updateProcOutTsPack(double actualIntLength,
                                double intLength)
{

  // update state variable mean values, show T_S

  String outputId;
  outputId = "T_S";
  procOutTsPack[outputId]= procOutTsPack[outputId] +
    (((E_s[1]/Mc) + procInValMap["T_0"]) * (actualIntLength/intLength));
}

//// actualExJ
// actualize attributes of exit side
//
void SSenH::actualExJ(const Map<Symbol,double>& pVecU)
{

for(int l = 1; l <= procInValMap["l_0"]; l++)
    {
      Symbol symL = int_to_str(l);
      vecJ[symL]["Ex"]["Out"]["F"]["T"] = (E_s[1]/Mc) + procInValMap["T_0"];
    }
}

//// actualEnJ
// actualize attributes of entrance side
//
void SSenH::actualEnJ(const Map<Symbol,double>& pVecU)
{
  for(int k = 1; k <= procInValMap["k_0"]; k++)
    {
      Symbol symK = int_to_str(k);
      vecJ[symK]["En"]["Out"]["R"]["T"] = (E_s[1]/Mc) + procInValMap["T_0"];
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void SSenH::actualSimplexInput(const Map<Symbol,double>& pVecU,
                               double actualIntLength)
{
  int sign = 1;
  double y = 0;
  x = procInValMap["T_0"] - procInValMap["T_Env"];
  x = x + (E_s[1]/Mc);
  x = x * (procInValMap["k"] * A);
  y = x - (E_s[1]/actualIntLength);
  y = y + ((Mc/actualIntLength)*(procInValMap["T_Min"] - procInValMap["T_0"]));

  if (y < 0)
    {
      sign = -1;
      lessConstraintRhs[2] = sign * y;
    }
  else
    greaterConstraintRhs[1] = y;

  for(int k = 1; k <= procInValMap["k_0"]; k++)
    {
      Symbol symK = int_to_str(k);
      String symKVecJ = " vecJ[" + symK.the_string() + "][En][In][F][T]";

      // control of input

      if (!vecJ[symK]["En"]["In"]["F"].element("T"))
        procApp->message(54,vertexId.the_string()+symKVecJ);

      if (((E_s[1]/Mc) + procInValMap["T_0"]) >= vecJ[symK]["En"]["In"]["F"]["T"])
        {
          equalConstraintRhs[k+2] = 0;
          equalConstraintCoef[k+2]["En"]["H"][symK] = 1;
        }

      if (sign < 0)
        lessConstraintCoef[2]["En"]["H"][symK]= sign * 1.0/procInValMap["Count"];
      else
        greaterConstraintCoef[1]["En"]["H"][symK] = 1.0/procInValMap["Count"];
    }

  for(int l = 1; l <= procInValMap["l_0"]; l++)
    {
      Symbol symL = int_to_str(l);
      String symLVecJ = " vecJ[" + symL.the_string() + "][Ex][In][R][T]";

      // control of input

      if (!vecJ[symL]["Ex"]["In"]["R"].element("T"))
        procApp->message(54,vertexId.the_string() + symLVecJ);

      if (vecJ[symL]["Ex"]["In"]["R"]["T"] >= ((E_s[1]/Mc) + procInValMap["T_0"]))
        {
          equalConstraintRhs[procInValMap["k_0"]+l+2] = 0;
          equalConstraintCoef[procInValMap["k_0"]+l+2]["Ex"]["H"][symL] = 1;
        }

      if (sign < 0)
        lessConstraintCoef[2]["Ex"]["H"][symL] = sign * (-1.0)/procInValMap["Count"];
      else
        greaterConstraintCoef[1]["Ex"]["H"][symL] = -1.0/procInValMap["Count"];
    }
}

//// initState
// set initial state variables
//
void SSenH:: initState(void)
{
  // initialize state variable value and state variable map

  E_s[1] = Mc * (procInValMap["T_I"] - procInValMap["T_0"]);
}

//// actualState
// actualize state variables (1 = change is not too large)
//
int SSenH::actualState(double& actualIntLength,
                       const Map<Symbol,double>& pVecU)
{
  double y = 0;
  for (int l = 1; l <=  procInValMap["l_0"]; l++)
    {
      Symbol symL = int_to_str(l);
      y = y - dotEEx["H"][symL];
    }
  for(int k = 1; k <= procInValMap["k_0"]; k++)
    {
      Symbol symK = int_to_str(k);
      y = y + dotEEn["H"][symK];
    }
  y = y/procInValMap["Count"];  // energy balance of one storage

  y = (y - x) * actualIntLength;

  if (fabs(y/Mc) <= procInValMap["deltaT"])
    {
      y = y + E_s[1];
      E_s[1] = y;

      if (y >= (Mc * (procInValMap["T_Max"] - procInValMap["T_0"])))
        E_s[1]= Mc * (procInValMap["T_Max"] - procInValMap["T_0"]);

      // solar or waste heat dumping

      return 1;
    }
  else
    return 0;        // state variable change is greater than allowed
}

//// showPower
//
Symbol SSenH::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  if (procInValMap["l_0"] > 1)
    procApp->message(64,vertexId.the_string() + " showPower");

  showPowerType   = "H";
  showPowerNumber = "1";
  return "Ex";
}

//// addFixCosts
// add process-dependent fix costs to the fix costs vector
// the specific fix costs of storages are not based on power
// but on storage volume
//
void SSenH::addFixCosts(Map<Symbol, MeanValRec>& scenOutValMap,
                        Map<Symbol, MapSym1d>& aggInValMap,
                        Map<Symbol, MapSym1M>& aggOutValMap)
{
  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     // <String.h>, the first 4 char
          String part2 = name(4);       // <String.h>, the rest
          if (part1 == "sFix")
            {
              String symFix = "Fix" + part2;  // !22.11.95
              if (aggCount == 0)              // process doesn't belong to a process aggregate
                {
                  scenOutValMap[symFix].expand
                    (scenOutValMap[symFix].cutoff()
                      + (procInValMap["V"] * procInValMap["Count"] *
                         procInValMapIt.curr()->value));
                }
              else                // process belongs to a process aggregate
                procApp->message(59,vertexId.the_string());
            }
        }
    }
}

///////////////////////////////////////////////////////////////////
//
// CLASS: SSupEl
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summmary: superconducting magnetic energy storage

//// SSupEl
// Standard Constructor
//
SSupEl::SSupEl(void)
{
}

//// SSupEl
// Constructor
//
SSupEl::SSupEl(App* cProcApp, Symbol procId, Symbol cProcType,
               ioValue* cProcVal) : Proc(cProcApp, procId,
               cProcType, cProcVal)
{

  ESUnit       = 3.6e9;     // energy unit = 1MWh = 3.6e9J
  dotEEnEl_Add = 0;
  k_1          = 0;
  k_2          = 0;
  dotE_L       = 0;
  dotE_Max     = 0;

  if (!procInValMap.element("E_Max"))
    procApp->message(55, vertexId.the_string() + " E_Max");
  if (!procInValMap.element("E_I"))
    procApp->message(55, vertexId.the_string() + " E_I");
  if (!procInValMap.element("a_1"))
    procApp->message(55, vertexId.the_string() + " a_1");
  if (!procInValMap.element("a_2"))
    procApp->message(55, vertexId.the_string() + " a_2");
  if (!procInValMap.element("b"))
    procApp->message(55, vertexId.the_string() + " b");
  if (!procInValMap.element("c_1"))
    procApp->message(55, vertexId.the_string() + " c_1");
  if (!procInValMap.element("c_2"))
    procApp->message(55, vertexId.the_string() + " c_2");
  if (!procInValMap.element("U_Max"))
    procApp->message(55, vertexId.the_string() + " U_Max");
  if (!procInValMap.element("n"))
    procApp->message(55, vertexId.the_string() + " n");
  if (!procInValMap.element("deltaU"))
    procApp->message(55, vertexId.the_string() + " deltaU");
  if (!procInValMap.element("d"))
    procApp->message(55, vertexId.the_string() + " d");
  if (!procInValMap.element("e"))
    procApp->message(55, vertexId.the_string() + " e");
  if (!procInValMap.element("deltaE"))
    procApp->message(55, vertexId.the_string() + " deltaE");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["E_Max"] <= 0 ||
      procInValMap["d"]     <= 0 ||
      procInValMap["e"]     <= 0 ||
      procInValMap["Count"] <= 0 )
    procApp->message(505, vertexId.the_string());
  if (procInValMap["a_1"] < 0 || procInValMap["a_2"] < 0)
    procApp->message(67, vertexId.the_string() + ": a_1,a_2");

  double x = 0;
  double y = 0;
  y = procInValMap["E_Max"]/ESUnit;
  x = log10(y);
  x = x * procInValMap["c_1"] - procInValMap["c_2"];

  if (x <= 0)
    procApp->message(61, vertexId.the_string() + ": E_Max,c_1,c_2");
  if ( procInValMap["E_I"] < (procInValMap["E_Max"] * procInValMap["b"]))
    procApp->message(61, vertexId.the_string() + ": E_Max,b,E_I");
  if ( procInValMap["E_I"] > procInValMap["E_Max"])
    procApp->message(61, vertexId.the_string() + ": E_Max,E_I");

  double z = 0;
  z = double(2)/double(3);
  dotEEnEl_Add = procInValMap["a_1"] * pow(y,z) + procInValMap["a_2"] * y;
  dotE_Max = procInValMap["E_Max"]/x;

  double L = 0;
  L = dotE_Max/procInValMap["U_Max"];
  L = double(1)/pow(L,2);
  L = 2 * procInValMap["E_Max"] * L;

  k_1 = double(2)/L;
  k_1 = k_1 * pow(procInValMap["U_Max"],2) * x;
  k_1 = k_1/procInValMap["E_Max"];
  k_1 = k_1 * (double(1)/procInValMap["d"] + double(1)/procInValMap["e"]);

  k_2 = procInValMap["n"] * procInValMap["deltaU"];
  k_2 = k_2 * sqrt(double(2)/L);

  Symbol2 a = Symbol2("El","0");
  En.insert(a);
  Symbol2 b = Symbol2("El","0");
  Ex.insert(b);
  Symbol2 c = Symbol2("El","1");          // additional electrical energy
  En.insert(c);                           // used for storage cooling

  // initialize state variable value and state variable map
  E_s[1] = procInValMap["E_I"];

  if (procApp->testFlag)
    procApp->message(1001, "SSupEl");
}

//// ~SSupEl
// Destructor
//
SSupEl::~SSupEl(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "SSupEl");
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void SSupEl::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                double actualIntLength)
{
  int sign = 1;
  dotE_L = k_1 * E_s[1] + k_2 * sqrt(E_s[1]);

  // additional energy

  equalConstraintRhs[1] = dotEEnEl_Add * procInValMap["Count"];
  equalConstraintCoef[1]["En"]["El"]["1"] = 1;

  // minimal energy content

  double x = 0;

  x = procInValMap["b"] * procInValMap["E_Max"] - E_s[1];
  x = x/actualIntLength;
  x = x + dotE_L;

  if (x < 0)
    {
      sign = -1;
      lessConstraintRhs[3] = sign * x;
      lessConstraintCoef[3]["En"]["El"]["0"] = sign * 1.0/procInValMap["Count"];
      lessConstraintCoef[3]["Ex"]["El"]["0"] = sign * (-1.0)/procInValMap["Count"];
    }
  else
    {
      greaterConstraintRhs[2] = x;
      greaterConstraintCoef[2]["En"]["El"]["0"] = 1.0/procInValMap["Count"];
      greaterConstraintCoef[2]["Ex"]["El"]["0"] = (-1.0)/procInValMap["Count"];
    }

  // maximum load

  x = dotE_Max + dotE_L;

  lessConstraintRhs[4] = x;
  lessConstraintCoef[4]["En"]["El"]["0"] = 1.0/procInValMap["Count"];
  lessConstraintCoef[4]["Ex"]["El"]["0"] = (-1.0)/procInValMap["Count"];

  x = dotE_Max - dotE_L;

  if (x < 0)
    {
      sign = -1;
      greaterConstraintRhs[6] = sign * x;
      greaterConstraintCoef[6]["En"]["El"]["0"] = sign * (-1.0)/
        procInValMap["Count"];
      greaterConstraintCoef[6]["Ex"]["El"]["0"] = sign * 1.0/procInValMap["Count"];
    }
  else
    {
      lessConstraintRhs[5] = x;
      lessConstraintCoef[5]["En"]["El"]["0"] = (-1.0)/procInValMap["Count"];
      lessConstraintCoef[5]["Ex"]["El"]["0"] = 1.0/procInValMap["Count"];
    }
}

//// actualState
// actualize state variables (1 = change is not too large)
//
int SSupEl::actualState(double& actualIntLength,
                        const Map<Symbol,double>& pVecU)
{
  double x = 0;

  x = dotEEn["El"]["0"];
  x = x - dotEEx["El"]["0"];
  x = x/procInValMap["Count"];       // energy balance of one storage
  x = (x - dotE_L) * actualIntLength;

  if (fabs(x) <= procInValMap["deltaE"])
    {
      x = x + E_s[1];
      E_s[1] = x;

      if (x >= procInValMap["E_Max"])
        E_s[1] = procInValMap["E_Max"];

      // electrical energy dumping, for example, from wind energy

      return 1;
    }
  else
    return 0;        // state variable change is greater than allowed
}

//// showPower
//
Symbol SSupEl::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "El";
  showPowerNumber = "1";
  return "Ex";
}

//// addFixCosts
// add process-dependent fix costs to the fix costs vector
// the specific fix costs of storages are not based on power
// but on storage volume
//
void SSupEl::addFixCosts(Map<Symbol, MeanValRec>& scenOutValMap,
                         Map<Symbol, MapSym1d>& aggInValMap,
                         Map<Symbol, MapSym1M>& aggOutValMap)
{
  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt; procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);               // <String.h>, the first 4 char
          String part2 = name(4);                 // <String.h>, the rest
          if (part1 == "sFix")
            {
              String symFix = "Fix" + part2;      // !22.11.95
              if (aggCount == 0)  // process doesn't belong to a process aggregate
                {
                  scenOutValMap[symFix].expand
                    (scenOutValMap[symFix].cutoff()
                    + (procInValMap["E_Max"] * procInValMap["Count"] *
                    procInValMapIt.curr()->value));
                }
              else                // process belongs to a process aggregate
                procApp->message(59,vertexId.the_string());
            }
        }
    }
}

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

//// SSenHn
// Standard Constructor

SSenHn::SSenHn(void)
{
}

//// SSenHn
// Constructor

SSenHn::SSenHn(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  GeoF=0;         // geometric factor for converting
                  // volume in surface area (diameter = height)
                  // GeoF depends on number of layers, see underneath

  A1=0; A2=0; AN_Deckel=0; AN_Mitte=0;  // Oberfl"achen f"ur
                                        // stratN = 1, 2, >=3
  Mc=0;

  if (!procInValMap.element("l_0"))
    procInValMap["l_0"]=1;
  if (!procInValMap.element("k_0"))
    procInValMap["k_0"]=1;
  if (!procInValMap.element("T_0"))
    procApp->message(55, vertexId.the_string()+ " T_0");
  if (!procInValMap.element("T_Min"))
    procApp->message(55, vertexId.the_string()+ " T_Min");
  if (!procInValMap.element("T_Max"))
    procApp->message(55, vertexId.the_string()+ " T_Max");
  if (!procInValMap.element("T_Env_min"))
    procApp->message(55, vertexId.the_string()+ " T_Env_min");
  if (!procInValMap.element("T_offset"))
    procApp->message(55, vertexId.the_string()+ " T_offset");
  if (!procInValMap.element("T_Quer"))
    procApp->message(55, vertexId.the_string()+ " T_Quer");

  // Temperaturdifferenz zwischen Schichten der Anfangsverteilung (f. initState)
  if (!procInValMap.element("NDeltaT"))
    procApp->message(55, vertexId.the_string()+ " NDeltaT");

  // Maximale Temp.aenderung pro Schicht und Zeitschritt
  if (!procInValMap.element("deltaT"))
    procApp->message(55, vertexId.the_string()+ " deltaT");

  if (!procInValMap.element("V"))
    procApp->message(55, vertexId.the_string()+ " V");
  if (!procInValMap.element("c_p"))
    procApp->message(55, vertexId.the_string()+ " c_p");
  if (!procInValMap.element("Rho"))
    procApp->message(55, vertexId.the_string()+ " Rho");
  if (!procInValMap.element("k"))
    procApp->message(55, vertexId.the_string()+ " k");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

  if (procInValMap.element("stratN"))
    stratN = int(procInValMap["stratN"]);
  else procApp->message(55, vertexId.the_string()+ " stratN");

  // Flag=1: Edot_Ex_(l=1) ist Typ 1 (Quelle fuer NAWP), sonst: Flag=2
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");

  if ((procInValMap["Flag"] != 1) && (procInValMap["Flag"] != 2))
    procApp->message(61, vertexId.the_string() + " Flag");

  if ((procInValMap["Flag"] == 1) && (!procInValMap.element("deltaT_Spreiz")) )
    procApp->message(55, vertexId.the_string() + " deltaT_Spreiz");

  if (procInValMap["V"] == 0 ||
      procInValMap["Rho"] == 0 ||
      procInValMap["c_p"] == 0 ||
      procInValMap["Count"] == 0 )
    procApp->message(505, vertexId.the_string());

  if (stratN == 0)
    procApp->message(505, vertexId.the_string());

  if (procInValMap["T_0"] > procInValMap["T_Min"])
    procApp->message(61, vertexId.the_string() + ": T_0,T_Min");

  if ( (procInValMap["T_Quer"]-(stratN-1)/2.0 * procInValMap["NDeltaT"]
                       < procInValMap["T_Min"]) ||
       (procInValMap["T_Quer"]+(stratN-1)/2.0 * procInValMap["NDeltaT"]
                       > procInValMap["T_Max"]) )
  procApp->message(61, vertexId.the_string() + ": T_Min, T_Quer, NDeltaT, T_Max");

  for(int l=1; l <=  procInValMap["l_0"]; l++){
      Symbol symL = int_to_str(l);
      Symbol2 a = Symbol2("H",symL);
      Ex.insert(a);
  }

  for(int k=1; k <=  procInValMap["k_0"]; k++){
      Symbol symK = int_to_str(k);
      Symbol2 a = Symbol2("H",symK);
      En.insert(a);
  }

  Mc = procInValMap["V"] * procInValMap["Rho"] * procInValMap["c_p"];

  if (stratN == 1 ){
    GeoF=5.5358104;
    double y;
    y  = double(2)/double(3);

    A[1] = pow(procInValMap["V"],y) * GeoF;
  }

  if (stratN == 2 ){
    GeoF=2.7679052;
    double y;

    y  = double(2)/double(3);
    A2 = pow(procInValMap["V"],y) * GeoF;

    A[1] = A2;
    A[2] = A2;
  }

  if (stratN >= 3 ){
    double y;
    y  = double(2)/double(3);

    GeoF=0.9226351;
    AN_Deckel = pow(procInValMap["V"],y) * GeoF * (stratN+4)/double(stratN);

    GeoF=3.6905403;
    AN_Mitte = pow(procInValMap["V"],y) * GeoF / double(stratN);

    A[1] = AN_Deckel;
    A[stratN] = AN_Deckel;
    for(int n=2; n < stratN; n++) A[n] = AN_Mitte;
  }

  initState();

  if (procApp->testFlag)
    procApp->message(1001, "SSenHn");

 }

//// ~SSenHn
// Destructor

SSenHn::~SSenHn(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "SSenHn");
    }
}

////  updateProcOutTsPack
// update procOutTsPack after each optimization

void SSenHn::updateProcOutTsPack(double  actualIntLength,
                                   double intLength)
{
  // update state variable mean values, show T_S
  String outputId;
  String n_string;

  procOutTsPack["T_quer"] = 0;

  for(int n=1; n <=  stratN; n++)
    {
      n_string = int_to_str(n);
      outputId = "T_S["+n_string+"]";
      procOutTsPack[outputId] += (stratN * E_s[n]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]) + procInValMap["T_0"])
                                 * actualIntLength/intLength;

      procOutTsPack["T_quer"] += procOutTsPack[outputId] / double(stratN);
    }
}

//// actualExJ
// actualize attributes of exit side

void SSenHn::actualExJ(const Map<Symbol,double> & pVecU)
{
  for(int l=1; l <=  procInValMap["l_0"]; l++){
    Symbol symL = int_to_str(l);

    vecJ[symL]["Ex"]["Out"]["F"]["T"] =
             (stratN * E_s[1]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]))+ procInValMap["T_0"];
  }

  if (procInValMap["Flag"] == 1){ // Nachheizen mit Waermepumpe
      int l=1;
      Symbol symL = int_to_str(l);
      vecJ[symL]["Ex"]["Out"]["R"]["T"] =
             (stratN * E_s[1]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]))+ procInValMap["T_0"]
                                 - procInValMap["deltaT_Spreiz"];
  }
}

//// actualEnJ
// actualize attributes of entrance side

void SSenHn::actualEnJ(const Map<Symbol,double> & pVecU)
{
  for(int k=1; k <=  procInValMap["k_0"]; k++){
    Symbol symK = int_to_str(k);

    vecJ[symK]["En"]["Out"]["R"]["T"] =
             (stratN * E_s[stratN]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]))+ procInValMap["T_0"];
  }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void SSenHn::actualSimplexInput(const Map<Symbol,double> & pVecU,
                               double actualIntLength)
{
     //control of input

  for(int k=1; k <=  procInValMap["k_0"]; k++){
      Symbol symK = int_to_str(k);
      String symKVecJ = " vecJ[" +symK.the_string() +"][En][In][F][T]";

      if (vecJ[symK]["En"]["In"]["F"].element("T"))
        T_FEn[symK] = vecJ[symK]["En"]["In"]["F"]["T"];
      else procApp->message(54,vertexId.the_string()+symKVecJ);
  }

  for(int l=2; l <=  procInValMap["l_0"]; l++){
      Symbol symL = int_to_str(l);
      String symLVecJ = " vecJ["+ symL.the_string() +"][Ex][In][R][T]";

      if (vecJ[symL]["Ex"]["In"]["R"].element("T"))
        T_REx[symL] = vecJ[symL]["Ex"]["In"]["R"]["T"];
      else procApp->message(54,vertexId.the_string()+symLVecJ);
  }

  if (procInValMap["Flag"] == 2){
      int l=1;
      Symbol symL = int_to_str(l);
      String symLVecJ = " vecJ["+ symL.the_string() +"][Ex][In][R][T]";

      if (vecJ[symL]["Ex"]["In"]["R"].element("T"))
        T_REx[symL] = vecJ[symL]["Ex"]["In"]["R"]["T"];
      else procApp->message(54,vertexId.the_string()+symLVecJ);
  }
  else{             //Flag=1: Um Zyklenfreiheit des Graphen zu
                    //  gewaehrleisten, muss bei Anschluss einer
                    //  Absoprionswaermepumpe (deren Leistungszahl
                    //  von T_REx abhaengt) die Information ueber
                    //  T_REx vom Speicher zur Pumpe fliessen.
                    //  Dazu wird die Infomation ueber die Temperatur
                    //  spreizung der Pumpe behelfsmaessig als Parameter
                    //  des Speichers behandelt.

      int l=1;
      Symbol symL = int_to_str(l);
      T_REx[symL] = (stratN * E_s[1]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]))+ procInValMap["T_0"]
                                 - procInValMap["deltaT_Spreiz"];
  }

  // calculate effective environment temperature

  //   jaehrl. Temp.schwankungen erreichen Eindringtiefen von
  //   nur etwa 3m (Kuemmel Skript), sind also fuer saisonalen Speicher
  //   in guter Naeherung vernachlaessigbar

  // Berechne Verlustterme (loss) der Schichten (in SSenH "x")
  // (T_s, loss, A als protected Map<int,double> in Storage.h deklariert)
  // (E_s als Map<int,double> in Proc.h deklariert)

  for(int n=1; n <=  stratN; n++){
    T_s[n]  = (stratN * E_s[n]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"])) + procInValMap["T_0"];
        double T_diff = procInValMap["T_offset"];
        if (T_s[n]-procInValMap["T_Env_min"] < procInValMap["T_offset"])
          T_diff = T_s[n]-procInValMap["T_Env_min"];
    loss[n] = (procInValMap["k"] * A[n]) * T_diff;
  }

 // Berechne control functions: f_F["k"]["n"], f_R["l"]["n"]
 // (werden erst in actualState benoetigt; sind in Storage.h `protected' dekl.

  T_s[0] = 600; // large number

 for(int n=1; n <= stratN; n++){ Symbol symN = int_to_str(n);

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);

          if( (T_s[n-1] >= T_FEn[symK])  &&  (T_FEn[symK] > T_s[n]) )
               f_F[symK][symN] = 1;
          else f_F[symK][symN] = 0;

          if ( (n==1) && (T_FEn[symK] > T_s[1]) )
               f_F[symK][symN] = 1;
          //      cout << "actualIntLength=" << actualIntLength
          //           << "; f_F["<<symK << "][" << symN << "] = "
          //           << f_F[symK][symN] << endl;
    }

    for(int l=1; l <=  procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);

          if( (T_s[n-1] >= T_REx[symL])  &&   (T_REx[symL] > T_s[n]) )
               f_R[symL][symN] = 1;
          else f_R[symL][symN] = 0;

          if ( (n==stratN) && (T_REx[symL] <= T_s[n]) )
               f_R[symL][symN] = 1;
          //      cout << "actualIntLength=" << actualIntLength
          //           << "; f_R[" << symL << "][" << symN << "] = "
          //           << f_R[symL][symN] << endl;
     }
          // cout << endl;
  }
          // cout << endl;

  Symbol symN0      = int_to_str(0);
  Symbol symN1      = int_to_str(1);
  Symbol symNN      = int_to_str(stratN);
  Symbol symNNplus1 = int_to_str(stratN+1);

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);

      f_F[symK][symN0] = 0;
      f_F[symK][symNNplus1] = 0;
    }

    for(int l=1; l <=  procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);

      f_R[symL][symN0] = 0;
      f_R[symL][symNNplus1] = 0;
    }

 // Berechne Temperaturquotienten   : T_quot_F["k"]["n"], T_quot_R["l"]["n"]
 // (werden erst in actualState benoetigt; sind in Storage.h `protected' dekl.

 for(int n=1; n <=  stratN; n++){ Symbol symN = int_to_str(n);

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);

       if (T_FEn[symK] != T_s[stratN])
             T_quot_F[symK][symN] = (T_FEn[symK]-T_s[n])/(T_FEn[symK]-T_s[stratN]);
       else  T_quot_F[symK][symN] = 0;

     // cout << "T_quot_F[" << symK << "][" << symN << "] = " << T_quot_F[symK][symN] << endl;
     }

    for(int l=1; l <=  procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);

       if (T_s[1] !=  T_REx[symL])
            T_quot_R[symL][symN] = (T_REx[symL]-T_s[n])/(T_s[1]-T_REx[symL]);
       else T_quot_R[symL][symN] = 0;

      // cout << "T_quot_R[" << symL << "][" << symN << "] = " << T_quot_R[symL][symN] << endl;
    }
 }

 // Berechne control functions: f_F_sum["k"]["n"], f_R_sum["l"]["n"]
 // (werden erst in actualState benoetigt; sind in Storage.h `protected' dekl.

 for(int n=1; n<=stratN; n++){ Symbol symN = int_to_str(n);

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);

      f_F_sum[symK][symN] = 0;

      for(int j=1; j<=(n-1); j++ ){ Symbol symJ = int_to_str(j);
            f_F_sum[symK][symN] += f_F[symK][symJ];
      }
      //  cout << "actualIntLength=" << actualIntLength
      //       << "; f_F_sum[" << symK << "][" << symN << "] = "
      //       << f_F_sum[symK][symN] << endl;
    }

    for(int l=1; l<=procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);

      f_R_sum[symL][symN] = 0;

      for(int j=n; j<=stratN; j++ ){ Symbol symJ = int_to_str(j);
            f_R_sum[symL][symN] += f_R[symL][symJ];
      }
      //  cout << "actualIntLength=" << actualIntLength
      //       << "; f_R_sum[" << symL << "][" << symN << "] = "
      //       << f_R_sum[symL][symN] << endl;
    }

 }

    //    Eine NB zur Gewaehrleistung einer Mindesttemperatur der
    //    Schicht N ist nicht implementiert.
    //    (Fuer die Analysen i.R. von Soleg ist dies auch nicht erforderlich,
    //    da keine Backup-Technik aum BEladen des saisonalen Speichers
    //    betrachtet wird.)

  // Energiefluss-Richtung

  for(int k=1; k <=  procInValMap["k_0"]; k++) { Symbol symK = int_to_str(k);

      if ( (stratN * E_s[stratN]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]) + procInValMap["T_0"]) >=T_FEn[symK]){
          equalConstraintRhs[k]=0;
          equalConstraintCoef[k]["En"]["H"][symK]=1;
      }
    }

  for(int l=1; l <=  procInValMap["l_0"]; l++) { Symbol symL = int_to_str(l);

      if (T_REx[symL] >= (stratN * E_s[1]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]) + procInValMap["T_0"]) ){
          equalConstraintRhs[procInValMap["k_0"]+l]=0;
          equalConstraintCoef[procInValMap["k_0"]+l]["Ex"]["H"][symL]=1;
      }
    }

 }

//// initState
// set initial state variables
void SSenHn::initState(void)
{
  // initialize state variables
  // Erzeuge geschichtete Temperaturverteilung bei vorgegebener
  // Mitteltemperatur "T_Quer" mit stratN volumenmaessig
  // gleichgrossen und bzgl. T aequidistanten Schichten
  // mit Temperaturdistanz NDeltaT

  double T_quer  = double(procInValMap["T_Quer"]);
  double NDeltaT = double(procInValMap["NDeltaT"]);

      cout << "T_quer = "  << T_quer  << endl;
      cout << "stratN = "  << stratN  << endl;
      cout << "NDeltaT = " << NDeltaT << endl;

  T_s[1] = T_quer + (stratN-1)/2.0 * NDeltaT;

  for(int n=1; n<=stratN; n++){
    T_s[n] = T_s[1] - (n-1) * NDeltaT;
    E_s[n] = (procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]) / stratN * (T_s[n] - procInValMap["T_0"]);
    cout << " T_s[" << n << "]  = " << T_s[n] << endl;
  }
}

//// actualState
// actualize state variables (1 = change is not too large)

int SSenHn::actualState(double & actualIntLength,
                          const Map<Symbol,double> & pVecU)
{
 // Die benoetigten Variablen
 //     f_F_sum["k"]["n"],  f_R_sum["l"]["n"],
 //     T_quot_F["k"]["n"], T_quot_R["l"]["n"],
 //     dot_mc_mix["n"]
 //     loss["n"]
 // sind in Storage.h `protected' deklariert
 // und damit auch in den anderen virtual functions bekannt.
 // Da actualSimplexInput immer VOR actualState aufgerufen wird,
 // brauchen sie innerhalb actualstate() nicht neuberechnet werden.
 // (So ist Thomas in SSenH auch mit dem Verlustterm `x' verfahren.)

  Symbol symN0      = int_to_str(0);
  Symbol symN1      = int_to_str(1);
  Symbol symNN      = int_to_str(stratN);
  Symbol symNNplus1 = int_to_str(stratN+1);

 // Berechne "mixed flow rates"  dot_mc_mix["n"]

  dot_mc_mix[symN1]      = 0;
  dot_mc_mix[symNNplus1] = 0;

  for(int n=2; n <=  stratN; n++){ Symbol symN = int_to_str(n);

   dot_mc_mix[symN] = 0;

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);

      if (T_FEn[symK] != T_s[stratN])
            dot_mc_En[symK] = dotEEn["H"][symK] / (T_FEn[symK] - T_s[stratN]);
      else  dot_mc_En[symK] = 0;

      dot_mc_mix[symN] += dot_mc_En[symK] * f_F_sum[symK][symN];
    }

    for(int l=1; l <=  procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);

      if (T_s[1] != T_REx[symL])
            dot_mc_Ex[symL] = dotEEx["H"][symL] / (T_s[1] - T_REx[symL]);
      else  dot_mc_Ex[symL] = 0;

      dot_mc_mix[symN] -= dot_mc_Ex[symL] * f_R_sum[symL][symN];
    }

    // cout << "dot_mc_mix[" << symN << "] = " << dot_mc_mix[symN] << endl;
  }

// actualize state variables (1 = change is not too large)

  int toolong = 0;

  for(int n=1; (n<=stratN) && (!toolong); n++){  Symbol symN = int_to_str(n);

    Symbol symNplus1 = int_to_str(n+1);
    double y=0;

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);
      y += dotEEn["H"][symK] * f_F[symK][symN] * T_quot_F[symK][symN];
    }

    for(int l=1; l <=  procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);
      y += dotEEx["H"][symL] * f_R[symL][symN] * T_quot_R[symL][symN];
    }

    if (dot_mc_mix[symN]       > 0)
          y += dot_mc_mix[symN]      * (T_s[n-1]-T_s[n]);  // (>0)

    if (dot_mc_mix[symNplus1]  < 0)
          y += dot_mc_mix[symNplus1] * (T_s[n]-T_s[n+1]);  // (<0)

    y = y/double(procInValMap["Count"]);  // energy balance of one storage

    y = (y - loss[n]) * actualIntLength;

    if (fabs(stratN*y/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"])) <= procInValMap["deltaT"]){

      y = y + E_s[n];  // (wie in SSenH)
      E_s[n] = y;      //

      if ( (n==1) && ( E_s[n] >= (procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"])/double(stratN) * ( procInValMap["T_Max"]
                                          -procInValMap["T_0"]  ) ) )
        E_s[1]= (procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"])/double(stratN) * (procInValMap["T_Max"] - procInValMap["T_0"]);
        // solar or waste heat dumping
    }
    else toolong = 1;
  }

  if (toolong) return 0; // state variable change of at least one layer
                         // is greater than allowed
  else {

    // Sortierung der Schichten nach Temperatur,
    // um ggf. aufgetretene Temperaturinversion zu
    // beheben

    int tauschflag=1;
 //   int getauscht=0; //Nur zur Ueberpruefung, ob ueberhaupt getauscht

    for(int n=1; (n<stratN) && (tauschflag==1); n++){
      tauschflag=0;

      for(int j=1; j<stratN; j++){
        if (E_s[j+1] > E_s[j]){

          // Tausche

          double dummy=E_s[j+1];
          E_s[j+1] = E_s[j];
          E_s[j]= dummy;

          tauschflag=1;
//        getauscht =1;
        }
      }
    }
  //      if (getauscht) cout << "getauscht!" << "\n" << endl;

    return 1;
  }
}

//// showPower (same as SSenH)

Symbol SSenHn::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
  if (procInValMap["l_0"] > 1)
    procApp->message(64,vertexId.the_string()+" showPower");

  showPowerType="H";
  showPowerNumber="1";
  return "Ex";
}

//// addFixCosts (same as SSenH)
// add process-dependent fix costs to the fix costs vector
// the specific fix costs of storages are not based on power
// but on storage volume

void SSenHn::addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
                       Map<Symbol, MapSym1d> & aggInValMap,
                       Map<Symbol, MapSym1M> & aggOutValMap)
{
  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt;  procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     //<String.h>, the first 4 char
          String part2 = name(4);       //<String.h>, the rest
          if (part1=="sFix")
            {
              String symFix = "Fix"+part2; //!22.11.95
              if (aggCount==0) //process doesn't belong to a process aggregate
                {
                  scenOutValMap[symFix].expand
                    (scenOutValMap[symFix].cutoff()
                     + (procInValMap["V"] * procInValMap["Count"] *
                        procInValMapIt.curr()->value));
                }
              else                //process belongs to a process aggregate
                procApp->message(59,vertexId.the_string());
            }
        }
    }
}

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

//// SSenHnHEx
// Standard Constructor

SSenHnHEx::SSenHnHEx(void)
{
}

//// SSenHnHEx
// Constructor

SSenHnHEx::SSenHnHEx(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  GeoF=0;         // geometric factor for converting
                  // volume in surface area (diameter = height)
                  // GeoF depends on number of layers, see underneath

  A1=0; A2=0; AN_Deckel=0; AN_Mitte=0;  // Oberfl"achen f"ur
                                        // stratN = 1, 2, >=3
  Mc=0;

  if (!procInValMap.element("l_0"))
    procInValMap["l_0"]=1;
  if (!procInValMap.element("k_0"))
    procInValMap["k_0"]=1;
  if (!procInValMap.element("T_0"))
    procApp->message(55, vertexId.the_string()+ " T_0");
  if (!procInValMap.element("T_Min"))
    procApp->message(55, vertexId.the_string()+ " T_Min");
  if (!procInValMap.element("T_Max"))
    procApp->message(55, vertexId.the_string()+ " T_Max");
  if (!procInValMap.element("T_Env_min"))
    procApp->message(55, vertexId.the_string()+ " T_Env_min");
  if (!procInValMap.element("T_offset"))
    procApp->message(55, vertexId.the_string()+ " T_offset");
  if (!procInValMap.element("T_Quer"))
    procApp->message(55, vertexId.the_string()+ " T_Quer");

  // Temperaturdifferenz zwischen Schichten der Anfangsverteilung (f. initState)
  if (!procInValMap.element("NDeltaT"))
    procApp->message(55, vertexId.the_string()+ " NDeltaT");

  // Maximale Temp.aenderung pro Schicht und Zeitschritt
  if (!procInValMap.element("deltaT"))
    procApp->message(55, vertexId.the_string()+ " deltaT");

  if (!procInValMap.element("V"))
    procApp->message(55, vertexId.the_string()+ " V");
  if (!procInValMap.element("c_p"))
    procApp->message(55, vertexId.the_string()+ " c_p");
  if (!procInValMap.element("Rho"))
    procApp->message(55, vertexId.the_string()+ " Rho");
  if (!procInValMap.element("k"))
    procApp->message(55, vertexId.the_string()+ " k");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

  if (procInValMap.element("stratN"))
    stratN = int(procInValMap["stratN"]);
  else procApp->message(55, vertexId.the_string()+ " stratN");

  if (!procInValMap.element("etaHEx"))
    procApp->message(55, vertexId.the_string()+ " etaHEx");

  // Flag=1: Edot_Ex_(l=1) ist Typ 1 (Quelle fuer NAWP)
  // UND kein Waermetauscher zur Entladung.
  // Sonst: Flag=2
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");

  if ((procInValMap["Flag"] != 1) && (procInValMap["Flag"] != 2))
    procApp->message(61, vertexId.the_string() + " Flag");

  if ((procInValMap["Flag"] == 1) && (!procInValMap.element("deltaT_Spreiz")) )
    procApp->message(55, vertexId.the_string() + " deltaT_Spreiz");

  if (procInValMap["V"] == 0 ||
      procInValMap["Rho"] == 0 ||
      procInValMap["c_p"] == 0 ||
      procInValMap["Count"] == 0 ||
      procInValMap["etaHEx"] == 0)
    procApp->message(505, vertexId.the_string());

  if (stratN == 0)
    procApp->message(505, vertexId.the_string());

  if (procInValMap["T_0"] > procInValMap["T_Min"])
    procApp->message(61, vertexId.the_string() + ": T_0,T_Min");

  if ( (procInValMap["T_Quer"]-(stratN-1)/2.0 * procInValMap["NDeltaT"]
                       < procInValMap["T_Min"]) ||
       (procInValMap["T_Quer"]+(stratN-1)/2.0 * procInValMap["NDeltaT"]
                       > procInValMap["T_Max"]) )
  procApp->message(61, vertexId.the_string() + ": T_Min, T_Quer, NDeltaT, T_Max");

  for(int l=1; l <=  procInValMap["l_0"]; l++){
      Symbol symL = int_to_str(l);
      Symbol2 a = Symbol2("H",symL);
      Ex.insert(a);
  }

  for(int k=1; k <=  procInValMap["k_0"]; k++){
      Symbol symK = int_to_str(k);
      Symbol2 a = Symbol2("H",symK);
      En.insert(a);
  }

  Mc = procInValMap["V"] * procInValMap["Rho"] * procInValMap["c_p"];

  initState();

  if (procApp->testFlag)
    procApp->message(1001, "SSenHnHEx");

 }

//// ~SSenHnHEx
// Destructor

SSenHnHEx::~SSenHnHEx(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "SSenHnHEx");
    }
}

////  updateProcOutTsPack
// update procOutTsPack after each optimization

void SSenHnHEx::updateProcOutTsPack(double  actualIntLength,
                                   double intLength)
{
  // update state variable mean values, show T_S
  String outputId;
  String n_string;

  procOutTsPack["T_quer"] = 0;

  for(int n=1; n <=  stratN; n++)
    {
      n_string = int_to_str(n);
      outputId = "T_S["+n_string+"]";
      procOutTsPack[outputId] += (stratN * E_s[n]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]) + procInValMap["T_0"])
                                 * actualIntLength/intLength;

      procOutTsPack["T_quer"] += procOutTsPack[outputId] / double(stratN);
    }
}

//// actualExJ
// actualize attributes of exit side

void SSenHnHEx::actualExJ(const Map<Symbol,double> & pVecU)
{
  // control of input
  for(int l=2; l <=  procInValMap["l_0"]; l++){
      Symbol symL = int_to_str(l);
      String symLVecJ = " vecJ["+ symL.the_string() +"][Ex][In][R][T]";

      if (vecJ[symL]["Ex"]["In"]["R"].element("T"))
        T_REx[symL] = vecJ[symL]["Ex"]["In"]["R"]["T"];
      else procApp->message(54,vertexId.the_string()+symLVecJ);
  }

  if (procInValMap["Flag"] == 2){
  Symbol symL = int_to_str(1);
  String symLVecJ = " vecJ["+ symL.the_string() +"][Ex][In][R][T]";

  if (vecJ[symL]["Ex"]["In"]["R"].element("T"))
    T_REx[symL] = vecJ[symL]["Ex"]["In"]["R"]["T"];
  else procApp->message(54,vertexId.the_string()+symLVecJ);
  }

  // normal case: heat exchanger apply (Flag=2 || (Flag=1 && l != 1))
  for(int l=1; l <=  procInValMap["l_0"]; l++){
    Symbol symL = int_to_str(l);
    vecJ[symL]["Ex"]["Out"]["F"]["T"] =
         procInValMap["etaHEx"]  * ((stratN * E_s[1]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]))+ procInValMap["T_0"])
     +(1-procInValMap["etaHEx"]) * T_REx[symL];
  }

  if (procInValMap["Flag"] == 1){ // (channel l=1: additional heating,
                                  // no heat exchanger, OVERWRITE normal case)
      Symbol symL = int_to_str(1);
      vecJ[symL]["Ex"]["Out"]["F"]["T"] =
             (stratN * E_s[1]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]))+ procInValMap["T_0"];
      vecJ[symL]["Ex"]["Out"]["R"]["T"] =
             (stratN * E_s[1]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]))+ procInValMap["T_0"]
                                 - procInValMap["deltaT_Spreiz"];
  }
}

//// actualEnJ
// actualize attributes of entrance side

void SSenHnHEx::actualEnJ(const Map<Symbol,double> & pVecU)
{
  for(int k=1; k <=  procInValMap["k_0"]; k++){
    Symbol symK = int_to_str(k);

    vecJ[symK]["En"]["Out"]["R"]["T"] =
             (stratN * E_s[stratN]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]))+ procInValMap["T_0"];
  }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void SSenHnHEx::actualSimplexInput(const Map<Symbol,double> & pVecU,
                               double actualIntLength)
{
  //control of input and effect of heat exchanger
  for(int k=1; k <=  procInValMap["k_0"]; k++){
      Symbol symK = int_to_str(k);
      String symKVecJ = " vecJ[" +symK.the_string() +"][En][In][F][T]";

      if (vecJ[symK]["En"]["In"]["F"].element("T"))
        T_FEn[symK] = vecJ[symK]["En"]["In"]["F"]["T"];
      else procApp->message(54,vertexId.the_string()+symKVecJ);
  }

  for(int l=2; l <=  procInValMap["l_0"]; l++){
      Symbol symL = int_to_str(l);
      String symLVecJ = " vecJ["+ symL.the_string() +"][Ex][In][R][T]";

      if (vecJ[symL]["Ex"]["In"]["R"].element("T")){
        T_REx[symL] = vecJ[symL]["Ex"]["In"]["R"]["T"];

        // effect of heat exchanger ON storage (actualSimplexInput, actualState)
        T_RExIn[symL] = procInValMap["etaHEx"] * T_REx[symL]
                      + (1-procInValMap["etaHEx"]) *
                        ((stratN * E_s[1]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]))+ procInValMap["T_0"]);
      }
      else procApp->message(54,vertexId.the_string()+symLVecJ);
  }

  if (procInValMap["Flag"] == 2){
      Symbol symL = int_to_str(1);
      String symLVecJ = " vecJ["+ symL.the_string() +"][Ex][In][R][T]";

      if (vecJ[symL]["Ex"]["In"]["R"].element("T")){
        T_REx[symL] = vecJ[symL]["Ex"]["In"]["R"]["T"];
        // effect of heat exchanger ON storage (actualSimplexInput, actualState)
        T_RExIn[symL] = procInValMap["etaHEx"] * T_REx[symL]
                      + (1-procInValMap["etaHEx"]) *
                        ((stratN * E_s[1]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]))+ procInValMap["T_0"]);
      }
      else procApp->message(54,vertexId.the_string()+symLVecJ);
  }
  else{             //Flag=1: Um Zyklenfreiheit des Graphen zu
                    //  gewaehrleisten, muss bei Anschluss einer
                    //  Waermepumpe (deren Leistungszahl
                    //  von T_REx abhaengen kann) die Information ueber
                    //  T_REx vom Speicher zur Pumpe fliessen.
                    //  Dazu wird die Infomation ueber die Temperatur
                    //  spreizung der Pumpe behelfsmaessig als Parameter
                    //  des Speichers behandelt.
      Symbol symL = int_to_str(1);
      T_RExIn[symL] = (stratN * E_s[1]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]))+ procInValMap["T_0"]
                                 - procInValMap["deltaT_Spreiz"];
  }

  // calculate effective environment temperature

  //   jaehrl. Temp.schwankungen erreichen Eindringtiefen von
  //   nur etwa 3m (Kuemmel Skript), sind also fuer saisonalen Speicher
  //   in guter Naeherung vernachlaessigbar

  // Berechne Oberflaechen und Verlustterme (loss) der Schichten (in SSenH "x")
  // (T_s, loss, A als protected Map<int,double> in Storage.h deklariert)
  // (E_s als Map<int,double> in Proc.h deklariert)

  if (stratN == 1 ){
    GeoF=5.5358104;
    double y;
    y  = double(2)/double(3);

    A[1] = pow(procInValMap["V"],y) * GeoF;
  }

  if (stratN == 2 ){
    GeoF=2.7679052;
    double y;

    y  = double(2)/double(3);
    A2 = pow(procInValMap["V"],y) * GeoF;

    A[1] = A2;
    A[2] = A2;
  }

  if (stratN >= 3 ){
    double y;
    y  = double(2)/double(3);

    GeoF=0.9226351;
    AN_Deckel = pow(procInValMap["V"],y) * GeoF * (stratN+4)/double(stratN);

    GeoF=3.6905403;
    AN_Mitte = pow(procInValMap["V"],y) * GeoF / double(stratN);

    A[1] = AN_Deckel;
    A[stratN] = AN_Deckel;
    for(int n=2; n < stratN; n++) A[n] = AN_Mitte;
  }

  for(int n=1; n <=  stratN; n++){
    T_s[n]  = (stratN * E_s[n]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"])) + procInValMap["T_0"];
        double T_diff = procInValMap["T_offset"];
        if (T_s[n]-procInValMap["T_Env_min"] < procInValMap["T_offset"])
          T_diff = T_s[n]-procInValMap["T_Env_min"];
    loss[n] = (procInValMap["k"] * A[n]) * T_diff;
  }

 // Berechne control functions: f_F["k"]["n"], f_R["l"]["n"]
 // (werden erst in actualState benoetigt; sind in Storage.h `protected' dekl.
  T_s[0] = 600; // large number

 for(int n=1; n <= stratN; n++){ Symbol symN = int_to_str(n);

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);

          if( (T_s[n-1] >= T_FEn[symK])  &&  (T_FEn[symK] > T_s[n]) )
               f_F[symK][symN] = 1;
          else f_F[symK][symN] = 0;

          if ( (n==1) && (T_FEn[symK] > T_s[1]) )
               f_F[symK][symN] = 1;
          //      cout << "actualIntLength=" << actualIntLength
          //           << "; f_F[" << symK << "][" << symN << "] = "
          //           << f_F[symK][symN] << endl;
    }

    for(int l=1; l <=  procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);

          if( (T_s[n-1] >= T_RExIn[symL])  &&   (T_RExIn[symL] > T_s[n]) )
               f_R[symL][symN] = 1;
          else f_R[symL][symN] = 0;

          if ( (n==stratN) && (T_RExIn[symL] <= T_s[n]) )
               f_R[symL][symN] = 1;
          //      cout << "actualIntLength=" << actualIntLength
          //           << "; f_R[" << symL << "][" << symN << "] = "
          //           << f_R[symL][symN] << endl;
     }
          // cout << endl;
  }
          // cout << endl;

  Symbol symN0      = int_to_str(0);
  Symbol symN1      = int_to_str(1);
  Symbol symNN      = int_to_str(stratN);
  Symbol symNNplus1 = int_to_str(stratN+1);

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);

      f_F[symK][symN0] = 0;
      f_F[symK][symNNplus1] = 0;
    }

    for(int l=1; l <=  procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);

      f_R[symL][symN0] = 0;
      f_R[symL][symNNplus1] = 0;
    }

 // Berechne Temperaturquotienten   : T_quot_F["k"]["n"], T_quot_R["l"]["n"]
 // (werden erst in actualState benoetigt; sind in Storage.h `protected' dekl.

 for(int n=1; n <=  stratN; n++){ Symbol symN = int_to_str(n);

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);

       if (T_FEn[symK] != T_s[stratN])
         T_quot_F[symK][symN] = (T_FEn[symK]-T_s[n])/(T_FEn[symK]-T_s[stratN]);
       else  T_quot_F[symK][symN] = 0;

   // cout << "T_quot_F[" << symK << "][" << symN << "] = " << T_quot_F[symK][symN] << endl;
     }

    for(int l=1; l <=  procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);

       if (T_s[1] !=  T_RExIn[symL])
            T_quot_R[symL][symN] = (T_RExIn[symL]-T_s[n])/(T_s[1]-T_RExIn[symL]);
       else T_quot_R[symL][symN] = 0;
   // cout << "T_quot_R[" << symL << "][" << symN << "] = " << T_quot_R[symL][symN] << endl;
    }
 }

 // Berechne control functions: f_F_sum["k"]["n"], f_R_sum["l"]["n"]
 // (werden erst in actualState benoetigt; sind in Storage.h `protected' dekl.

 for(int n=1; n<=stratN; n++){ Symbol symN = int_to_str(n);

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);

      f_F_sum[symK][symN] = 0;

      for(int j=1; j<=(n-1); j++ ){ Symbol symJ = int_to_str(j);
            f_F_sum[symK][symN] += f_F[symK][symJ];
      }
      //  cout << "actualIntLength=" << actualIntLength
      //       << "; f_F_sum[" << symK << "][" << symN << "] = "
      //       << f_F_sum[symK][symN] << endl;
    }

    for(int l=1; l<=procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);

      f_R_sum[symL][symN] = 0;

      for(int j=n; j<=stratN; j++ ){ Symbol symJ = int_to_str(j);
            f_R_sum[symL][symN] += f_R[symL][symJ];
      }
      //  cout << "actualIntLength=" << actualIntLength
      //       << "; f_R_sum[" << symL << "][" << symN << "] = "
      //       << f_R_sum[symL][symN] << endl;
    }

 }

    //    Eine NB zur Gewaehrleistung einer Mindesttemperatur der
    //    Schicht N ist nicht implementiert.
    //    (Fuer die Analysen i.R. von Soleg ist dies auch nicht erforderlich,
    //    da keine Backup-Technik zum BEladen des saisonalen Speichers
    //    betrachtet wird.)

  // Energiefluss-Richtung

  for(int k=1; k <=  procInValMap["k_0"]; k++) { Symbol symK = int_to_str(k);

      if ( (stratN * E_s[stratN]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]) + procInValMap["T_0"]) >=T_FEn[symK]){
          equalConstraintRhs[k]=0;
          equalConstraintCoef[k]["En"]["H"][symK]=1;
      }
    }

  for(int l=1; l <=  procInValMap["l_0"]; l++) { Symbol symL = int_to_str(l);

      if (T_RExIn[symL] >= (stratN * E_s[1]/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]) + procInValMap["T_0"]) ){
          equalConstraintRhs[procInValMap["k_0"]+l]=0;
          equalConstraintCoef[procInValMap["k_0"]+l]["Ex"]["H"][symL]=1;
      }
    }

 }

//// initState
// set initial state variables
void SSenHnHEx::initState(void)
{
  // initialize state variables
  // Erzeuge geschichtete Temperaturverteilung bei vorgegebener
  // Mitteltemperatur "T_Quer" mit stratN volumenmaessig
  // gleichgrossen und temperatur-aequidistanten Schichten
  // mit Temperaturdistanz NDeltaT

  double T_quer  = double(procInValMap["T_Quer"]);
  double NDeltaT = double(procInValMap["NDeltaT"]);

        cout << "T_quer = "  << T_quer  << endl;
        cout << "stratN = "  << stratN  << endl;
        cout << "NDeltaT = " << NDeltaT << endl;

  T_s[1] = T_quer + (stratN-1)/2.0 * NDeltaT;

  for(int n=1; n<=stratN; n++){
    T_s[n] = T_s[1] - (n-1) * NDeltaT;
    E_s[n] = (procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"]) / stratN * (T_s[n] - procInValMap["T_0"]);
    cout << " T_s[" <<n<< "]  = " <<  T_s[n] << endl;
  }
}

//// actualState
// actualize state variables (1 = change is not too large)

int SSenHnHEx::actualState(double & actualIntLength,
                          const Map<Symbol,double> & pVecU)
{
 // Die benoetigten Variablen
 //     f_F_sum["k"]["n"],  f_R_sum["l"]["n"],
 //     T_quot_F["k"]["n"], T_quot_R["l"]["n"],
 //     dot_mc_mix["n"]
 //     loss["n"]
 // sind in Storage.h `protected' deklariert
 // und damit auch in den anderen virtual functions bekannt.
 // Da actualSimplexInput immer VOR actualState aufgerufen wird,
 // brauchen sie innerhalb actualstate() nicht neuberechnet werden.
 // (So ist Thomas in SSenH auch mit dem Verlustterm `x' verfahren.)

  Symbol symN0      = int_to_str(0);
  Symbol symN1      = int_to_str(1);
  Symbol symNN      = int_to_str(stratN);
  Symbol symNNplus1 = int_to_str(stratN+1);

 // Berechne "mixed flow rates"  dot_mc_mix["n"]

  dot_mc_mix[symN1]      = 0;
  dot_mc_mix[symNNplus1] = 0;

  for(int n=2; n <=  stratN; n++){ Symbol symN = int_to_str(n);

   dot_mc_mix[symN] = 0;

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);

      if (T_FEn[symK] != T_s[stratN])
            dot_mc_En[symK] = dotEEn["H"][symK] / (T_FEn[symK] - T_s[stratN]);
      else  dot_mc_En[symK] = 0;

      dot_mc_mix[symN] += dot_mc_En[symK] * f_F_sum[symK][symN];
    }

    for(int l=1; l <=  procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);

      if (T_s[1] != T_RExIn[symL])
            dot_mc_Ex[symL] = dotEEx["H"][symL] / (T_s[1] - T_RExIn[symL]);
      else  dot_mc_Ex[symL] = 0;

      dot_mc_mix[symN] -= dot_mc_Ex[symL] * f_R_sum[symL][symN];
    }

    // cout << "dot_mc_mix[" << symN << "] = " << dot_mc_mix[symN] << endl;
  }

// actualize state variables (return 1 indicates change is not too large)

  int toolong = 0;

  for(int n=1; (n<=stratN) && (!toolong); n++){  Symbol symN = int_to_str(n);

    Symbol symNplus1 = int_to_str(n+1);
    double y=0;

    for(int k=1; k <=  procInValMap["k_0"]; k++){ Symbol symK = int_to_str(k);
      y += dotEEn["H"][symK] * f_F[symK][symN] * T_quot_F[symK][symN];
    }

    for(int l=1; l <=  procInValMap["l_0"]; l++){ Symbol symL = int_to_str(l);
      y += dotEEx["H"][symL] * f_R[symL][symN] * T_quot_R[symL][symN];
    }

    if (dot_mc_mix[symN]       > 0)
          y += dot_mc_mix[symN]      * (T_s[n-1]-T_s[n]);  // (>0)

    if (dot_mc_mix[symNplus1]  < 0)
          y += dot_mc_mix[symNplus1] * (T_s[n]-T_s[n+1]);  // (<0)

    y = y/double(procInValMap["Count"]);  // energy balance of one storage

    y = (y - loss[n]) * actualIntLength;

    if (fabs(stratN*y/(procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"])) <= procInValMap["deltaT"]){

      y = y + E_s[n];  // (wie in SSenH)
      E_s[n] = y;      //

      if ( (n==1) && ( E_s[n] >= (procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"])/double(stratN) * ( procInValMap["T_Max"]
                                          -procInValMap["T_0"]  ) ) )
        E_s[1]= (procInValMap["V"]*procInValMap["Rho"]* procInValMap["c_p"])/double(stratN) * (procInValMap["T_Max"] - procInValMap["T_0"]);
        // solar or waste heat dumping
    }
    else toolong = 1;
  }

  if (toolong) return 0; // state variable change of at least one layer
                         // is greater than allowed
  else {

    // Sortierung der Schichten nach Temperatur,
    // um ggf. aufgetretene Temperaturinversion zu
    // beheben

    int tauschflag=1;
    int getauscht=0; //Nur zur Ueberpruefung, ob ueberhaupt getauscht

    for(int n=1; (n<stratN) && (tauschflag==1); n++){
      tauschflag=0;

      for(int j=1; j<stratN; j++){
        if (E_s[j+1] > E_s[j]){

          // Tausche

          double dummy=E_s[j+1];
          E_s[j+1] = E_s[j];
          E_s[j]= dummy;

          tauschflag=1;
          getauscht =1;
        }
      }
    }
        if (getauscht) cout << "getauscht!" << "\n" << endl;

    return 1;
  }
}

//// showPower (same as SSenH)

Symbol SSenHnHEx::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
  if (procInValMap["l_0"] > 1)
    procApp->message(64,vertexId.the_string()+" showPower");

  showPowerType="H";
  showPowerNumber="1";
  return "Ex";
}

//// addFixCosts (same as SSenH)
// add process-dependent fix costs to the fix costs vector
// the specific fix costs of storages are not based on power
// but on storage volume

void SSenHnHEx::addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
                       Map<Symbol, MapSym1d> & aggInValMap,
                       Map<Symbol, MapSym1M> & aggOutValMap)
{
  for (Mapiter<Symbol,double> procInValMapIt = procInValMap.first();
       procInValMapIt;  procInValMapIt.next())
    {
      String name = procInValMapIt.curr()->key.the_string();
      if (name.length() > 4)
        {
          String part1 = name(0,4);     //<String.h>, the first 4 char
          String part2 = name(4);       //<String.h>, the rest
          if (part1=="sFix")
            {
              String symFix = "Fix"+part2; //!22.11.95
              if (aggCount==0) //process doesn't belong to a process aggregate
                {
                  scenOutValMap[symFix].expand
                    (scenOutValMap[symFix].cutoff()
                     + (procInValMap["V"] * procInValMap["Count"] *
                        procInValMapIt.curr()->value));
                }
              else                //process belongs to a process aggregate
                procApp->message(59,vertexId.the_string());
            }
        }
    }
}

//  Robbie: 02.09.05: Notes about ISO C++ modifications.
//
//  For loops:
//
//    added 'int' to about 12 for (int i = x, ... ) calls
//
//    This required two passes, because the compiler only found 9 the first time
//    through.
//
//    I am not very happy about this because some for loops are nested and the scope
//    implications really do need to be checked -- and any naming ambiguities
//    resolved.

//  $Source: /home/deeco/source/deeco006/RCS/Storage.C,v $
// end of file
