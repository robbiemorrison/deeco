
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
// DEECO COLLECTOR-PROCESS MODULES
//
//////////////////////////////////////////////////////////////////

#include "Collect.h"
#include <math.h>
#include <float.h>              // for use of DBL_MAX

///////////////////////////////////////////////////////////////////
//
// CLASS: OSol
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: solar thermal energy, flate plate collector,
// no heat exchanger, linear efficiency dependency

//// OSol
// Standard Constructor
//
OSol::OSol(void)
{
  x1 = 0;
  x2 = 0;
}

//// OSol
// Constructor
//
OSol::OSol(App* cProcApp, Symbol procId, Symbol cProcType,
           ioValue* cProcVal) : Proc(cProcApp, procId,
           cProcType, cProcVal)
{
  x1 = 0;
  x2 = 0;

  Symbol2 a = Symbol2("H","0");
  Symbol2 b = Symbol2("El","0");
  Ex.insert(a);
  En.insert(b);

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if (!procInValMap.element("A_c"))
    procApp->message(55, vertexId.the_string() + " A_c");
  if (!procInValMap.element("FDashEta_0"))
    procApp->message(55, vertexId.the_string() + " FDashEta_0");
  if (!procInValMap.element("FDashK_c"))
    procApp->message(55, vertexId.the_string() + " FDashK_c");
  if (!procInValMap.element("c_p"))
    procApp->message(55, vertexId.the_string() + " c_p");
  if (!procInValMap.element("DotMOverA_c"))
    procApp->message(55, vertexId.the_string() + " DotMOverA_c");
  if (!procInValMap.element("p_El"))
    procApp->message(55, vertexId.the_string() + " p_El");
  if (!procInValMap.element("Gamma"))
    procApp->message(55, vertexId.the_string() + " Gamma");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["A_c"] == 0         ||
      procInValMap["FDashEta_0"] == 0  ||
      procInValMap["FDashK_c"] == 0    ||
      procInValMap["c_p"] == 0         ||
      procInValMap["DotMOverA_c"] == 0 ||
      procInValMap["p_El"] == 0        ||
      procInValMap["Gamma"] == 0       ||
      procInValMap["Count"] == 0 )
     procApp->message(505, vertexId.the_string());
  if (procApp->testFlag)
    procApp->message(1001, "OSol");
}

//// ~OSol
// Destructor
//
OSol::~OSol(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "OSol");
    }
}

//// actualExJ
// actualize attributes of exit side
//
void OSol::actualExJ(const Map<Symbol,double>& pVecU)
{
  double y = 0;
  double z = 0;
  double T_REx;

  // control of input

  if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
    {
      T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
      if (!(T_REx > 0))
        procApp->message(56,vertexId.the_string() + " DotEEx[H][0]");
    }
  else
    procApp->message(54,vertexId.the_string() + " vecJ[0][Ex][In][R][T]");

  // calculate output

  if (pVecU.element("I"))
    {
      if (pVecU.element("T"))
        y = procInValMap["FDashEta_0"]/procInValMap["FDashK_c"] * pVecU["I"] -
          (T_REx-pVecU["T"]);
      else
        procApp->message(57,"T");
    }
  else
    procApp->message(57,"I");

  y  = y * procInValMap["A_c"] * procInValMap["c_p"] * procInValMap["DotMOverA_c"];
  z  = procInValMap["FDashK_c"]/(procInValMap["c_p"] * procInValMap["DotMOverA_c"]);
  x1 = y * (1 - exp(-z));
  y  = procInValMap["p_El"] * procInValMap["A_c"];

  // x1 and x2 are used in actualSimplexInput also

  if (x1 < (y/DBL_MAX))   // protect devide by zero
    x2 = 0;
  else
    x2 = y/x1;

  if (x1 < (procInValMap["Gamma"] * y))
    x1 = 0;

  y = x1/(procInValMap["DotMOverA_c"] * procInValMap["A_c"] * procInValMap["c_p"]);
  vecJ["0"]["Ex"]["Out"]["F"]["T"] = y + T_REx;

}

//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void OSol::actualSimplexInput(const Map<Symbol,double>& pVecU,
                              double actualIntLength)
{
  if (procInValMap["Flag"] == 1)
    {
      lessConstraintRhs[1] = procInValMap["Count"] * x1;
      lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }
  if (procInValMap["Flag"] == 0)
    {
      equalConstraintRhs[1] = procInValMap["Count"] * x1;
      equalConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }

  equalConstraintRhs[2] = 0;
  equalConstraintCoef[2]["En"]["El"]["0"] = 1;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = -x2;
}

//// addFixCosts
// add process-dependent fix costs to the fix costs vector
// the specific fix costs of collectors are not based on power
// but on collector area
//
void OSol::addFixCosts(Map<Symbol, MeanValRec>& scenOutValMap,
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
              String symFix = "Fix" + part2; // !22.11.95
              if (aggCount == 0)        // process doesn't belong to a process aggregate
                {
                  scenOutValMap[symFix].expand
                    (scenOutValMap[symFix].cutoff()
                     + (procInValMap["A_c"] * procInValMap["Count"] *
                        procInValMapIt.curr()->value));
                }
              else                      // process belongs to a process aggregate
                procApp->message(59,vertexId.the_string());
            }
        }
    }
}

///////////////////////////////////////////////////////////////////
//
// CLASS: OSolHEx
//
///////////////////////////////////////////////////////////////////

// Module author: Dietmar Lindenberger
// Development thread: S03
//
// Summary: Solar Thermal Energy Flate Plate Collector
// linear efficiency dependency WITH Heat Exchanger

//// OSolHEx
// Standard Constructor

OSolHEx::OSolHEx(void)
{
x1=0;
x2=0;
}

//// OSolHEx
// Constructor

OSolHEx::OSolHEx(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  x1=0;
  x2=0;

  Symbol2 a = Symbol2("H","0");
  Symbol2 b =  Symbol2("El","0");
  Ex.insert(a);
  En.insert(b);

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");
  if (!procInValMap.element("A_c"))
    procApp->message(55, vertexId.the_string()+ " A_c");
  if (!procInValMap.element("FDashEta_0"))
    procApp->message(55, vertexId.the_string()+ " FDashEta_0");
  if (!procInValMap.element("FDashK_c"))
    procApp->message(55, vertexId.the_string()+ " FDashK_c");
  if (!procInValMap.element("c_p"))
    procApp->message(55, vertexId.the_string()+ " c_p");
  if (!procInValMap.element("DotMOverA_c"))
    procApp->message(55, vertexId.the_string()+ " DotMOverA_c");
  if (!procInValMap.element("p_El"))
    procApp->message(55, vertexId.the_string()+ " p_El");
  if (!procInValMap.element("Gamma"))
    procApp->message(55, vertexId.the_string()+ " Gamma");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

  if (!procInValMap.element("etaHEx"))
    procApp->message(55, vertexId.the_string()+ " etaHEx");

  if (procInValMap["A_c"] == 0 ||
      procInValMap["FDashEta_0"] == 0 ||
      procInValMap["FDashK_c"] == 0 ||
      procInValMap["c_p"] == 0 ||
      procInValMap["DotMOverA_c"] == 0||
      procInValMap["p_El"] == 0 ||
      procInValMap["Gamma"] == 0 ||
      procInValMap["Count"] == 0 ||
      procInValMap["etaHEx"] == 0 )
     procApp->message(505, vertexId.the_string());

  if (procApp->testFlag)
    procApp->message(1001, "OSolHEx");
}

//// ~OSolHEx
// Destructor

OSolHEx::~OSolHEx(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "OSolHEx");
    }
}

//// actualExJ
// actualize attributes of exit side

void OSolHEx::actualExJ(const Map<Symbol,double> & pVecU)
{
double y,z=0;
double T_REx;

//control of input

if (vecJ["0"]["Ex"]["In"]["R"].element("T"))
  {
    T_REx = vecJ["0"]["Ex"]["In"]["R"]["T"];
    if (! (T_REx>0))
      procApp->message(56,vertexId.the_string()+" DotEEx[H][0]");
  }
else
  procApp->message(54,vertexId.the_string()+" vecJ[0][Ex][In][R][T]");

//calculate output

if (pVecU.element("I"))
  {
    if (pVecU.element("T"))
      y =procInValMap["FDashEta_0"]/procInValMap["FDashK_c"]*pVecU["I"]-
        (T_REx-pVecU["T"]);
    else
      procApp->message(57,"T");
  }
else
  procApp->message(57,"I");

y =y *procInValMap["A_c"] * procInValMap["c_p"] * procInValMap["DotMOverA_c"];
z =procInValMap["FDashK_c"]/(procInValMap["c_p"]*procInValMap["DotMOverA_c"]);
x1 = y * (1-exp(-z));

// take into account heat exchanger with efficiency etaHEx
// (based on modified heat removal factor)

x1 = x1 / (1 + x1/(procInValMap["A_c"]* procInValMap["DotMOverA_c"]
                                      * procInValMap["c_p"] )
                 /(procInValMap["FDashEta_0"]/procInValMap["FDashK_c"]*pVecU["I"]
                   -(T_REx-pVecU["T"]))
                 *(1-procInValMap["etaHEx"])/procInValMap["etaHEx"]
          );

y  = procInValMap["p_El"]*procInValMap["A_c"];

//x1 and x2 are used in actualSimplexInput also

if (x1 < (y/DBL_MAX))   //protect devide by zero
  x2 =0;
else
  x2 = y/x1;

if (x1 < (procInValMap["Gamma"]*y))
  x1 = 0;

y = x1/(procInValMap["DotMOverA_c"]*procInValMap["A_c"]*procInValMap["c_p"]);

vecJ["0"]["Ex"]["Out"]["F"]["T"] = y + T_REx;
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void OSolHEx::actualSimplexInput(const Map<Symbol,double> & pVecU,
                              double actualIntLength)
{
  if (procInValMap["Flag"]==1)
    {
      lessConstraintRhs[1]                   = procInValMap["Count"] * x1;
      lessConstraintCoef[1]["Ex"]["H"]["0"]  = 1;
    }
  if (procInValMap["Flag"]==0)
    {
      equalConstraintRhs[1]                   = procInValMap["Count"] * x1;
      equalConstraintCoef[1]["Ex"]["H"]["0"]  = 1;
    }

  equalConstraintRhs[2]                  = 0;
  equalConstraintCoef[2]["En"]["El"]["0"]= 1;
  equalConstraintCoef[2]["Ex"]["H"]["0"] = -x2;
}

//// addFixCosts
// add process-dependent fix costs to the fix costs vector
// the specific fix costs of collectors are not based on power
// but on collector area

void OSolHEx::addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
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
                     + (procInValMap["A_c"] * procInValMap["Count"] *
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
// CLASS: OPVSol
//
///////////////////////////////////////////////////////////////////

// Module author: Johannes Bruhn
// Development thread: S04
//
// Summary: Photovoltaic Electric Energy

//// OPVSol
// Standard Constructor

OPVSol::OPVSol(void)
{
}

//// OPVSol
// Constructor

OPVSol::OPVSol(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");
  if (!procInValMap.element("A_m"))
    procApp->message(55, vertexId.the_string()+ " A_m");
  if (!procInValMap.element("eta_opt"))
    procApp->message(55, vertexId.the_string()+ " eta_opt");
  if (!procInValMap.element("eta_m"))
    procApp->message(55, vertexId.the_string()+ " eta_m");
  if (!procInValMap.element("alpha"))
    procApp->message(55, vertexId.the_string()+ " alpha");
  if (!procInValMap.element("epsilon"))
    procApp->message(55, vertexId.the_string()+ " epsilon");
  if (!procInValMap.element("TDash_eff"))
    procApp->message(55, vertexId.the_string()+ " TDash_eff");
  if (!procInValMap.element("eta_5%"))
    procApp->message(55, vertexId.the_string()+ " eta_5%");
  if (!procInValMap.element("eta_5%"))
    procApp->message(55, vertexId.the_string()+ " eta_5%");
  if (!procInValMap.element("eta_10%"))
    procApp->message(55, vertexId.the_string()+ " eta_10%");
  if (!procInValMap.element("eta_20%"))
    procApp->message(55, vertexId.the_string()+ " eta_20%");
  if (!procInValMap.element("eta_30%"))
    procApp->message(55, vertexId.the_string()+ " eta_30%");
  if (!procInValMap.element("eta_50%"))
    procApp->message(55, vertexId.the_string()+ " eta_50%");
  if (!procInValMap.element("eta_100%"))
    procApp->message(55, vertexId.the_string()+ " eta_100%");
  if (!procInValMap.element("P_WR"))
    procApp->message(55, vertexId.the_string()+ " P_WR");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

      if (procInValMap["Flag"]          <  0    ||
          procInValMap["Flag"]          >  1    ||
          procInValMap["A_m"]           == 0    ||
          procInValMap["eta_opt"]       <  0    ||
          procInValMap["eta_opt"]       >  1    ||
          procInValMap["eta_m"]         <  0    ||
          procInValMap["eta_m"]         >  1    ||
          procInValMap["epsilon"]       >  1    ||
          procInValMap["epsilon"]       <  0    ||
          procInValMap["TDash_eff"]     == 0    ||
          procInValMap["eta_5%"]        >  1    ||
          procInValMap["eta_5%"]        <  0    ||
          procInValMap["eta_10%"]       >  1    ||
          procInValMap["eta_10%"]       <  0    ||
          procInValMap["eta_20%"]       >  1    ||
          procInValMap["eta_20%"]       <  0    ||
          procInValMap["eta_30%"]       >  1    ||
          procInValMap["eta_30%"]       <  0    ||
          procInValMap["eta_50%"]       >  1    ||
          procInValMap["eta_50%"]       <  0    ||
          procInValMap["eta_100%"]      >  1    ||
          procInValMap["eta_100%"]      <  0    ||
          procInValMap["P_WR"]          <  0    ||
          procInValMap["Count"]         == 0    )
     procApp->message(505, vertexId.the_string());

  if (procApp->testFlag)
    procApp->message(1001, "OPVSol");
}

//// ~OPVSol
// Destructor

OPVSol::~OPVSol(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "OPVSol");
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void OPVSol::actualSimplexInput(const Map<Symbol,double> & pVecU,
                              double actualIntLength)
{
        double T_m,k;
        if (pVecU.element("W"))              // Windgeschwindigkeit
                k=5.7+3.8*pVecU["W"];        // Waermeuebergangskoeff.
                                             // nach Duffie und Beckmann [Duf74]
        else
                procApp->message(62, " W");

// Berechnung der Modultemperatur

        const  double sigma=0.0000000567;       // Sefan-Bolzmann-Konstante
        if (pVecU.element("I"))         // Strahlung
                if (pVecU.element("T")) // Aussentemperatur
                    {
                        if (pVecU["T"] < 0)
                                procApp->message(67, " T");
                        if (pVecU["I"] > 0)
                                T_m=(pVecU["I"]*procInValMap["eta_opt"]+k*pVecU["T"]
                                +sigma*procInValMap["epsilon"]*procInValMap["TDash_eff"]*pVecU["T"]
                                -pVecU["I"]*procInValMap["eta_m"]+pVecU["I"]*procInValMap["eta_m"]
                                *procInValMap["alpha"]*298.15)/(k+sigma*procInValMap["epsilon"]*
                                procInValMap["TDash_eff"]+pVecU["I"]*
                                procInValMap["eta_m"]*procInValMap["alpha"]);
                        else
                                if (pVecU["I"] == 0)
                                     T_m=pVecU["T"];
                                else
                                     procApp->message(67, " I");
                     }
                else
                        procApp->message(62, " T");

        else
                procApp->message(62, " I");

// Berechnung der Modulleistung

        double P_el=pVecU["I"]*procInValMap["A_m"]*
                (1+procInValMap["alpha"]*(T_m-298.15))*procInValMap["eta_m"];

// Berechnen des Wechselrichterwirkungsgrades //

        double A=P_el/procInValMap["P_WR"];

        double eta_WR;

        if (A <= 0.05)
                eta_WR=procInValMap["eta_5%"]/0.05*A;
        if (A > 0.05 && A <= 0.1)
                eta_WR=(procInValMap["eta_10%"]-procInValMap["eta_5%"])/0.05*(A-0.05)
                +procInValMap["eta_5%"];
        if (A > 0.1 && A <= 0.2)
                eta_WR=(procInValMap["eta_20%"]-procInValMap["eta_10%"])/0.1*(A-0.1)
                +procInValMap["eta_10%"];
        if (A > 0.2 && A <= 0.3)
                eta_WR=(procInValMap["eta_30%"]-procInValMap["eta_20%"])/0.1*(A-0.2)
                +procInValMap["eta_20%"];
        if (A > 0.3 && A <= 0.5)
                eta_WR=(procInValMap["eta_50%"]-procInValMap["eta_30%"])/0.2*(A-0.3)
                +procInValMap["eta_30%"];
        if (A > 0.5 && A <= 1)
                eta_WR=(procInValMap["eta_100%"]-procInValMap["eta_50%"])/0.5*(A-0.5)
                +procInValMap["eta_50%"];
        if (A > 1)
                procApp->message(61, vertexId.the_string()+ " P_WR");

        double EDot_Ex_El=P_el*eta_WR;

        if (procInValMap["Flag"]==0)
        {
                equalConstraintRhs [1]                   = procInValMap["Count"] * EDot_Ex_El;
                equalConstraintCoef[1]["Ex"]["El"]["0"]  = 1;
        }
        else
        {
                lessConstraintRhs[1]                    = procInValMap["Count"] * EDot_Ex_El;
                lessConstraintCoef[1]["Ex"]["El"]["0"]  = 1;
        }

}

//// addFixCosts
// add process-dependent fix costs to the fix costs vector
// the specific fix costs of collectors are not based on power
// but on collector area

void OPVSol::addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
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
                     + (procInValMap["A_m"] * procInValMap["Count"] *
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
// CLASS: OWind
//
//////////////////////////////////////////////////////////////////

// Module author: Johannes Bruhn
// Development thread: S04
//
// Summary: Windpower System, simple model

//// OWind
// Standard Constructor

OWind::OWind(void)
{
}

//// OWind
// Constructor

OWind::OWind(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);

  if (!procInValMap.element("d_r"))
    procApp->message(55, vertexId.the_string()+ " d_r");
  if (!procInValMap.element("h_NN"))
    procApp->message(55, vertexId.the_string()+ " h_NN");
  if (!procInValMap.element("h_r"))
    procApp->message(55, vertexId.the_string()+ " h_r");
  if (!procInValMap.element("h_mW"))
    procApp->message(55, vertexId.the_string()+ " h_mW");
  if (!procInValMap.element("z_0"))
    procApp->message(55, vertexId.the_string()+ " z_0");
  if (!procInValMap.element("D"))
    procApp->message(55, vertexId.the_string()+ " D");
  if (!procInValMap.element("h_mT"))
    procApp->message(55, vertexId.the_string()+ " h_mT");
  if (!procInValMap.element("eta_Tr"))
    procApp->message(55, vertexId.the_string()+ " eta_Tr");
  if (!procInValMap.element("eta_Gen"))
    procApp->message(55, vertexId.the_string()+ " eta_Gen");
  if (!procInValMap.element("v_ab"))
    procApp->message(55, vertexId.the_string()+ " v_ab");
  if (!procInValMap.element("v_start"))
    procApp->message(55, vertexId.the_string()+ " v_start");
  if (!procInValMap.element("P_nenn"))
    procApp->message(55, vertexId.the_string()+ " P_nenn");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");
  if (!procInValMap.element("cp_max"))
    procApp->message(55, vertexId.the_string()+ " cp_max");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

   if (
          procInValMap["d_r"]           <= 0    ||
          procInValMap["h_r"]           <= 0    ||
          procInValMap["z_0"]           <= 0    ||
          procInValMap["D"]             <  0    ||
          procInValMap["eta_Tr"]        <= 0    ||
          procInValMap["eta_Gen"]       <= 0    ||
          procInValMap["v_ab"]          <= 0    ||
          procInValMap["v_start"]       <  0    ||
          procInValMap["P_nenn"]        <= 0    ||
          procInValMap["Flag"]          <  0    ||
          procInValMap["Flag"]          >  1    ||
          procInValMap["Count"]         <= 0    ||
          procInValMap["cp_max"]        <  0    ||
          procInValMap["cp_max"]        >  0.592        //cp Betz
      )
     procApp->message(505, vertexId.the_string());

  if (procApp->testFlag)
    procApp->message(1001, " OWind");
}

//// ~OWind
// Destructor

OWind::~OWind(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
                procApp->message(1002, " OWind");
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void OWind::actualSimplexInput(const Map<Symbol,double> & pVecU,
                              double actualIntLength)
{

////Windgeschwindigkeit in Rotorhoehe berechnen////                     (1)

        double ws;

        if (pVecU.element("W"))              // Windgeschwindigkeit
                if (pVecU["W"] < 0)
                        procApp->message(67, " W");
                else
                        ws=pVecU["W"]*log((procInValMap["h_r"]-procInValMap["D"])/
                        procInValMap["z_0"])/log((procInValMap["h_mW"]-procInValMap["D"])
                        /procInValMap["z_0"]);
        else
                procApp->message(62, " W");

//Berechnung des Luftdrucks auf Nabenhoehe//                    (2)

        if (!pVecU.element("T"))                     // Temperatur
                procApp->message(62, " T");
        if (pVecU["T"] < 0)
                procApp->message(67, " T");
        const double p_0=1013.25;        //Normaldruck in hPa//
        double p=p_0*pow(1-(0.0065*(procInValMap["h_NN"]+procInValMap["h_r"])/288),5.256);

//Berechnung der Luftdichte und Temp in Rotorhoehe//                    (3,4)

        double rho=0.1293*p/(101.325*(1+0.00367*(pVecU["T"]-0.0065*
                                (procInValMap["h_r"]-procInValMap["h_mT"])-273.15)));

//berechnen von k_e nach [Qua 00]                               (5)

        double k_e=1+0.2794*pow(ws,-0.8674);

/////Berechnung der Leistung///////                                                     (6)

        const double pi = 3.1415926535;
        double EDot_Ex_El;

        if (ws >= procInValMap["v_start"] && ws < procInValMap["v_ab"])
        {
                EDot_Ex_El=0.5*rho*pow((procInValMap["d_r"]/2),2)*pi*pow(ws,3)
                        *procInValMap["cp_max"]*procInValMap["eta_Tr"]*procInValMap["eta_Gen"]
                        *k_e;
                if (EDot_Ex_El > procInValMap["P_nenn"])
                        EDot_Ex_El = procInValMap["P_nenn"];
                if (EDot_Ex_El < 0 )
                        EDot_Ex_El = 0 ;
        }
        else
             EDot_Ex_El=0;
        if  (procInValMap["Flag"]==0)
        {
                equalConstraintRhs [1]                   = procInValMap["Count"] * EDot_Ex_El;
                equalConstraintCoef[1]["Ex"]["El"]["0"]  = 1;
        }
        else
        {
                lessConstraintRhs [1]                   = procInValMap["Count"] * EDot_Ex_El;
                lessConstraintCoef[1]["Ex"]["El"]["0"]  = 1;
        }
}

//// addFixCosts
// add process-dependent fix costs to the fix costs vector
// the specific fix costs of engine are  based on power

void OWind::addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
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
                     + (procInValMap["P_nenn"] * procInValMap["Count"] *
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
// CLASS: OWind2
//
//////////////////////////////////////////////////////////////////

// Module author: Johannes Bruhn
// Development thread: S04
//
// Summary: Windpower System, extended model

//// OWind2
// Standard Constructor

OWind2::OWind2(void)
{
}

//// OWind2
// Constructor

OWind2::OWind2(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  Symbol2 a = Symbol2("El","0");
  Ex.insert(a);

  if (!procInValMap.element("d_r"))
    procApp->message(55, vertexId.the_string()+ " d_r");
  if (!procInValMap.element("h_NN"))
    procApp->message(55, vertexId.the_string()+ " h_NN");
  if (!procInValMap.element("h_r"))
    procApp->message(55, vertexId.the_string()+ " h_r");
  if (!procInValMap.element("h_mW"))
    procApp->message(55, vertexId.the_string()+ " h_mW");
  if (!procInValMap.element("z_0"))
    procApp->message(55, vertexId.the_string()+ " z_0");
  if (!procInValMap.element("D"))
    procApp->message(55, vertexId.the_string()+ " D");
  if (!procInValMap.element("h_mT"))
    procApp->message(55, vertexId.the_string()+ " h_mT");
  if (!procInValMap.element("eta_Tr"))
    procApp->message(55, vertexId.the_string()+ " eta_Tr");
  if (!procInValMap.element("eta_Gen"))
    procApp->message(55, vertexId.the_string()+ " eta_Gen");
  if (!procInValMap.element("v_ab"))
    procApp->message(55, vertexId.the_string()+ " v_ab");
  if (!procInValMap.element("v_start"))
    procApp->message(55, vertexId.the_string()+ " v_start");
  if (!procInValMap.element("P_nenn"))
    procApp->message(55, vertexId.the_string()+ " P_nenn");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string()+ " Flag");
  if (!procInValMap.element("c0"))
    procApp->message(55, vertexId.the_string()+ " c0");
  if (!procInValMap.element("c1"))
    procApp->message(55, vertexId.the_string()+ " c1");
  if (!procInValMap.element("c2"))
    procApp->message(55, vertexId.the_string()+ " c2");
  if (!procInValMap.element("c3"))
    procApp->message(55, vertexId.the_string()+ " c3");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;

   if (
          procInValMap["d_r"]           <= 0    ||
          procInValMap["h_r"]           <= 0    ||
          procInValMap["z_0"]           <= 0    ||
          procInValMap["D"]             <  0    ||
          procInValMap["eta_Tr"]        <= 0    ||
          procInValMap["eta_Gen"]       <= 0    ||
          procInValMap["v_ab"]          <= 0    ||
          procInValMap["v_start"]       <  0    ||
          procInValMap["P_nenn"]        <= 0    ||
          procInValMap["Flag"]          <  0    ||
          procInValMap["Flag"]          >  1    ||
          procInValMap["Count"]         <= 0
      )
     procApp->message(505, vertexId.the_string());

  if (procApp->testFlag)
    procApp->message(1001, " OWind2");
}

//// ~OWind2
// Destructor

OWind2::~OWind2(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
                procApp->message(1002, " OWind2");
    }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void OWind2::actualSimplexInput(const Map<Symbol,double> & pVecU,
                              double actualIntLength)
{

////Windgeschwindigkeit in Rotorhoehe berechnen////                     (1)

        double ws;

        if (pVecU.element("W"))              // Windgeschwindigkeit
                if (pVecU["W"] < 0)
                        procApp->message(67, " W");
                else
                        ws=pVecU["W"]*log((procInValMap["h_r"]-procInValMap["D"])/
                        procInValMap["z_0"])/log((procInValMap["h_mW"]-procInValMap["D"])
                        /procInValMap["z_0"]);
        else
                procApp->message(62, " W");

/////Berechnung des Luftdrucks auf Nabenhoehe//////////                 (2)

        if (!pVecU.element("T"))                     // Temperatur
                procApp->message(62, " T");
        if (pVecU["T"] < 0)
                procApp->message(67, " T");

        const double p_0=1013.25;        //Normaldruck in hPa//
        double p=p_0*pow(1-(0.0065*(procInValMap["h_NN"]+procInValMap["h_r"])/288),5.256);

//Berechnung der Luftdichte und Temp. in Rotorhoehe//                   (3,4)

        double rho=0.1293*p/(101.325*(1+0.00367*(pVecU["T"]-0.0065*
                                (procInValMap["h_r"]-procInValMap["h_mT"])-273.15)));

//berechnen von k_e nach [Qua 00]

        double k_e=1+0.2794*pow(ws,-0.8674);

///////////////////////////////////
/////Berechnung der Leistung///////                                                     (5)
///////////////////////////////////

        const double pi = 3.1415926535;
        double EDot_Ex_El;

        if (ws >= procInValMap["v_start"] && ws < procInValMap["v_ab"])
        {
                EDot_Ex_El=(procInValMap["c0"]*pow(ws,3)+procInValMap["c1"]
                        *pow(ws,2)+procInValMap["c2"]*ws+procInValMap["c3"])*
                        0.5*rho*pow((procInValMap["d_r"]/2),2)*pi
                        *procInValMap["eta_Tr"]*procInValMap["eta_Gen"]*k_e;

                if (EDot_Ex_El > procInValMap["P_nenn"])
                        EDot_Ex_El = procInValMap["P_nenn"];
                if (EDot_Ex_El < 0 )
                        EDot_Ex_El = 0 ;
        }
        else
             EDot_Ex_El=0;

        if  (procInValMap["Flag"]==0)
        {
                equalConstraintRhs [1]                   = procInValMap["Count"] * EDot_Ex_El;
                equalConstraintCoef[1]["Ex"]["El"]["0"]  = 1;
        }
        else
        {
                lessConstraintRhs [1]                   = procInValMap["Count"] * EDot_Ex_El;
                lessConstraintCoef[1]["Ex"]["El"]["0"]  = 1;
        }
}

//// addFixCosts
// add process-dependent fix costs to the fix costs vector
// the specific fix costs of engine are  based on power

void OWind2::addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
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
                     + (procInValMap["P_nenn"] * procInValMap["Count"] *
                        procInValMapIt.curr()->value));
                }
              else                //process belongs to a process aggregate
                procApp->message(59,vertexId.the_string());
            }
        }
    }
}

/////////////////////////////////////////////////////////////////
//
// CLASS: OConPT
//
///////////////////////////////////////////////////////////////////

// Module author: Susanna Medel
// Development thread: S11
//
// Summary: solar-thermal power station with concentrating
//          parabolic trough collectors

///OConPT
//Standard Constructor
OConPT::OConPT(void)
{
        y = 0.06;
}

///OConPT
//Constructor

OConPT::OConPT(App* cProcApp, Symbol procId, Symbol cProcType,
                 ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
        y = 0.06;

        Symbol2 a = Symbol2("El","0");
        Symbol2 b = Symbol2("Fuel","0");
        Ex.insert(a);
        En.insert(b);

        if (!procInValMap.element("Q_c"))
                procApp->message(55, vertexId.the_string()+ "Q_c");
        if (!procInValMap.element("Hg_amb"))
                procApp->message(55, vertexId.the_string()+ "Hg_amb");
        if (!procInValMap.element("Habs_g"))
                procApp->message(55, vertexId.the_string()+ "Habs_g");
        if (!procInValMap.element("H_c"))
                procApp->message(55, vertexId.the_string()+ "H_c");
        if (!procInValMap.element("H_i"))
                procApp->message(55, vertexId.the_string()+ "H_i");
        if (!procInValMap.element("A_abs"))
                procApp->message(55, vertexId.the_string()+ "A_abs");
        if (!procInValMap.element("A_a"))
                procApp->message(55, vertexId.the_string()+ "A_a");
        if (!procInValMap.element("A_g"))
                procApp->message(55, vertexId.the_string()+ "A_g");
        if (!procInValMap.element("A_i"))
                procApp->message(55, vertexId.the_string()+ "A_i");
        if (!procInValMap.element("l"))                         //characteristic length of the system [m]
                procApp->message(55, vertexId.the_string()+ "l");
        if (!procInValMap.element("d_o"))
                procApp->message(55, vertexId.the_string()+ "d_o");
        if (!procInValMap.element("d_i"))
                procApp->message(55, vertexId.the_string()+ "d_i");
        if (!procInValMap.element("d_g"))
                procApp->message(55, vertexId.the_string()+ "d_g");
        if (!procInValMap.element("E_abs"))
                procApp->message(55, vertexId.the_string()+ "E_abs");
        if (!procInValMap.element("E_g"))
                procApp->message(55, vertexId.the_string()+ "E_g");
        if (!procInValMap.element("T_in"))
                procApp->message(55, vertexId.the_string()+ "T_in");
        if (!procInValMap.element("T_out"))
                procApp->message(55, vertexId.the_string()+ "T_out");
        if (!procInValMap.element("n_row"))
                procApp->message(55, vertexId.the_string()+ "n_row");
        if (!procInValMap.element("n_serie"))
                procApp->message(55, vertexId.the_string()+ "n_serie");
        if (!procInValMap.element("ro"))                                // reflectivity of the mirror
                procApp->message(55, vertexId.the_string()+ "ro");
        if (!procInValMap.element("tao"))                               // transmissivity of the mirror
                procApp->message(55, vertexId.the_string()+ "tao");
        if (!procInValMap.element("gamma"))                             // glass tube emissivity
                procApp->message(55, vertexId.the_string()+ "gamma");
        if (!procInValMap.element("alfa"))                              // absortivity of the absorber
                procApp->message(55, vertexId.the_string()+ "alfa");
        if (!procInValMap.element("Lambda"))                            // thermal conductivity of the absorber tube
                procApp->message(55, vertexId.the_string()+ "Lambda");
        if (!procInValMap.element("Latitude"))                          // thermal conductivity of the absorber tube
                procApp->message(55, vertexId.the_string()+ "Latitude");
        if (!procInValMap.element("eff_vr"))
                procApp->message(55, vertexId.the_string()+ "eff_vr");
        if (!procInValMap.element("f_vel"))
                procApp->message(55, vertexId.the_string()+ "f_vel");
        if (!procInValMap.element("a_0.04"))
                procApp->message(55, vertexId.the_string()+ "a_0.04");
        if (!procInValMap.element("a_0.05"))
                procApp->message(55, vertexId.the_string()+ "a_0.05");
        if (!procInValMap.element("a_0.06"))
                procApp->message(55, vertexId.the_string()+ "a_0.06");
        if (!procInValMap.element("a_0.07"))
                procApp->message(55, vertexId.the_string()+ "a_0.07");
        if (!procInValMap.element("a_0.08"))
                procApp->message(55, vertexId.the_string()+ "a_0.08");
        if (!procInValMap.element("a_0.09"))
                procApp->message(55, vertexId.the_string()+ "a_0.09");
        if (!procInValMap.element("a_0.1"))
                procApp->message(55, vertexId.the_string()+ "a_0.1");
        if (!procInValMap.element("b_0.04"))
                procApp->message(55, vertexId.the_string()+ "b_0.04");
        if (!procInValMap.element("b_0.05"))
                procApp->message(55, vertexId.the_string()+ "b_0.05");
        if (!procInValMap.element("b_0.06"))
                procApp->message(55, vertexId.the_string()+ "b_0.06");
        if (!procInValMap.element("b_0.07"))
                procApp->message(55, vertexId.the_string()+ "b_0.07");
        if (!procInValMap.element("b_0.08"))
                procApp->message(55, vertexId.the_string()+ "b_0.08");
        if (!procInValMap.element("b_0.09"))
                procApp->message(55, vertexId.the_string()+ "b_0.09");
        if (!procInValMap.element("b_0.1"))
                procApp->message(55, vertexId.the_string()+ "b_0.1");
        if (!procInValMap.element("c_0.04"))
                procApp->message(55, vertexId.the_string()+ "c_0.04");
        if (!procInValMap.element("c_0.05"))
                procApp->message(55, vertexId.the_string()+ "c_0.05");
        if (!procInValMap.element("c_0.06"))
                procApp->message(55, vertexId.the_string()+ "c_0.06");
        if (!procInValMap.element("c_0.07"))
                procApp->message(55, vertexId.the_string()+ "c_0.07");
        if (!procInValMap.element("c_0.08"))
                procApp->message(55, vertexId.the_string()+ "c_0.08");
        if (!procInValMap.element("c_0.09"))
                procApp->message(55, vertexId.the_string()+ "c_0.09");
        if (!procInValMap.element("c_0.1"))
                procApp->message(55, vertexId.the_string()+ "c_0.1");

        if (procInValMap["Q_c"] == 0 ||
        procInValMap["Hg_amb"] == 0 ||
        procInValMap["Habs_g"] == 0 ||
        procInValMap["H_c"] == 0 ||
        procInValMap["H_i"] == 0 ||
        procInValMap["A_abs"] == 0 ||
        procInValMap["A_a"] == 0 ||
        procInValMap["A_g"] == 0 ||
        procInValMap["A_i"] == 0 ||
        procInValMap["l"] == 0 ||
        procInValMap["d_o"] == 0 ||
        procInValMap["d_i"] == 0 ||
        procInValMap["d_g"] == 0 ||
        procInValMap["E_abs"] == 0 ||
        procInValMap["E_g"] == 0 ||
        procInValMap["T_in"] == 0 ||
        procInValMap["T_out"] == 0 ||
        procInValMap["n_row"] == 0 ||
        procInValMap["n_serie"] == 0 ||
        procInValMap["ro"] == 0 ||
        procInValMap["tao"] == 0 ||
        procInValMap["gamma"] == 0 ||
        procInValMap["alfa"] == 0 ||
        procInValMap["Lambda"] == 0 ||
        procInValMap["Latitude"] == 0 ||
        procInValMap["eff_vr"] == 0 ||
        procInValMap["f_vel"] == 0 ||
        procInValMap["a_0.04"] == 0 ||
        procInValMap["a_0.05"] == 0 ||
        procInValMap["a_0.06"] == 0 ||
        procInValMap["a_0.07"] == 0 ||
        procInValMap["a_0.08"] == 0 ||
        procInValMap["a_0.09"] == 0 ||
        procInValMap["a_0.1"] == 0  ||
        procInValMap["b_0.04"] == 0 ||
        procInValMap["b_0.05"] == 0 ||
        procInValMap["b_0.06"] == 0 ||
        procInValMap["b_0.07"] == 0 ||
        procInValMap["b_0.08"] == 0 ||
        procInValMap["b_0.09"] == 0 ||
        procInValMap["b_0.1"] == 0  ||
        procInValMap["c_0.04"] == 0 ||
        procInValMap["c_0.05"] == 0 ||
        procInValMap["c_0.06"] == 0 ||
        procInValMap["c_0.07"] == 0 ||
        procInValMap["c_0.08"] == 0 ||
        procInValMap["c_0.09"] == 0 ||
        procInValMap["c_0.1"] == 0)
                procApp->message(505, vertexId.the_string());

        if (procApp->testFlag)
                procApp->message(1001, "OConPT");
}

///~OConPT
//Destructor

OConPT::~OConPT(void)
{
        if(procApp !=0)
        {
                if (procApp->testFlag)
                        procApp->message(1002, "OConPT");
        }
}

///actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimation

void OConPT::actualSimplexInput(const Map<Symbol,double> & pVecU,
                                double actualIntLength)
{

        Q_c = procInValMap["Q_c"];
        Hg_amb = procInValMap["Hg_amb"];
        Habs_g = procInValMap["Habs_g"];
        H_c = procInValMap["H_c"];
        H_i = procInValMap["H_i"];
        m = 0.0;
        T_abs = 0.0;
        T_g = 0.0;
        T_m = (procInValMap["T_out"]+procInValMap["T_in"])/2.0;  // bulk temperature
        C_p = 715.77+2.8229*T_m; // specific heat capacity of the heat transfer fluid [J/kg K]
        Sigma = 5.67e-8;       // Stefan-Boltzmann constant 5,67*10-8 [W/m2K4]
        Kc = 0.0;
        Fr = 0.0;
        F = 0.0;
        I_abs = 0.0;
        I_Dc = 0.0;
        Q_cf = 0.0;
        Q_cv = 0.0;
        x = 0;
        Id = pVecU["Id"];
        Ta = pVecU["Ta"];
        doy = pVecU["doy"]; // day of the year
        h = pVecU["h"]; // solar hour
        absolut = 0.0;

// calculation of the solar hour.

        w = solar_hour(h ,w);

// calculation of beam radiadion on a collector surface [W/m2]

        I_Dc = I_D_collect(Id, doy, w, procInValMap["Latitude"], I_Dc);

                for (i=0;x!=1 && i<=1000;i++){

                        // calculation of mass flow (m)

                        m = Q_c /(C_p*(procInValMap["T_out"]-procInValMap["T_in"]));

                        // calculation of the absorber temperature (T_abs)

                        T_abs = (Q_c /(H_i*procInValMap["A_i"]*procInValMap["n_serie"])) + T_m;

                        // calculation of the glass-cover temperature (T_g)

                        T_g = ((T_abs*Habs_g)+(Ta*(procInValMap["A_g"]/procInValMap["A_abs"])*(Hg_amb+H_c)))/(Habs_g+(procInValMap["A_g"]/procInValMap["A_abs"])*(Hg_amb + H_c));

                        // calculation of the heat transfer coeficients [W/m2K]

                        Habs_g = HeatTransferCoeff_abs_g(Sigma, T_abs, T_g, procInValMap["E_abs"], procInValMap["E_g"], procInValMap["A_abs"], procInValMap["A_g"], Habs_g);
                        Hg_amb = HeatTransferCoeff_g_amb(Ta, T_g, Sigma, procInValMap["E_g"], Hg_amb);
                        H_c = HeatTransferCoeff_c(T_g, Ta, procInValMap["l"],procInValMap["n_serie"], H_c);
                        H_i = HeatTransferCoeff_i(T_abs, T_m, procInValMap["d_i"], procInValMap["f_vel"], H_i);

                        // calculation of global heat-losses coefficient [W/m2K]

                        Kc = (1)/(((procInValMap["A_abs"]/procInValMap["A_g"])*(1/(H_c+Hg_amb)))+(1/Habs_g));

                        // Calculation of the efficiency factor of the absorber

                        F = (1 / Kc)/((1 / Kc)+(procInValMap["d_o"]/H_i*procInValMap["d_i"])+(procInValMap["d_o"]/(2*procInValMap["Lambda"]))*log(procInValMap["d_o"]/procInValMap["d_i"]));

                        // Calculation of waermeabfuhrfaktor

                        Fr = ((m*C_p)/(procInValMap["A_abs"]*procInValMap["n_serie"]*Kc))*(1-exp(-((procInValMap["A_abs"]*procInValMap["n_serie"]*F*Kc)/(m*C_p))));

                        // Calculation of the absorbed radiation

                        I_abs = Iabsorved(I_Dc, procInValMap["ro"], procInValMap["tao"], procInValMap["alfa"], procInValMap["gamma"], I_abs);

                        // Calulation of the heat power

                        Q_cv =  Fr*procInValMap["A_a"]*procInValMap["n_serie"]*(I_abs-(Kc*(procInValMap["A_abs"]/procInValMap["A_a"])*(procInValMap["T_in"]-Ta)));

                        if (m < 0.0){ // case that the obtained heat power is negative

                                Q_cf = 0.0;
                                x = 1;
                        }

                        else {

                        absolut = fabs((Q_c - Q_cv)/Q_c);
                        if (absolut >= 10e-8) {

                                Q_c = Q_cv;
                                x = 0;
                        }
                        else {
                                Q_cf = Q_c*procInValMap["n_row"]*procInValMap["eff_vr"];
                                x = 1;

                                }
                        }

                }

        // Optimization of Auxiliary heat and Efuel

        if(0.035 < y && y <= 0.1){

                if(0.035 < y && y <= 0.045){  // condeser pressure between these two values

                        v = procInValMap["b_0.04"]*Q_cf + procInValMap["c_0.04"];
                        if (v > 0.0){
                        greaterConstraintRhs[1] = v;
                        greaterConstraintCoef[1]["Ex"]["El"]["0"] = -procInValMap["a_0.04"];
                        greaterConstraintCoef[1]["En"]["Fuel"]["0"] = 1;
                        }
                        if (v <= 0.0){
                        lessConstraintRhs[1] = -v;
                        lessConstraintCoef[1]["Ex"]["El"]["0"] = procInValMap["a_0.04"];
                        lessConstraintCoef[1]["En"]["Fuel"]["0"] = -1;
                        }
                }

                if(0.045 < y && y <= 0.055){  // condenser pressure between these two values

                        v = procInValMap["b_0.05"]*Q_cf + procInValMap["c_0.05"];
                        if (v > 0.0){
                        greaterConstraintRhs[1] = v;
                        greaterConstraintCoef[1]["Ex"]["El"]["0"] = -procInValMap["a_0.05"];
                        greaterConstraintCoef[1]["En"]["Fuel"]["0"] = 1;
                        }
                        if (v <= 0.0){
                        lessConstraintRhs[1] = -v;
                        lessConstraintCoef[1]["Ex"]["El"]["0"] = procInValMap["a_0.05"];
                        lessConstraintCoef[1]["En"]["Fuel"]["0"] = -1;
                        }
                }

                if(0.055 < y && y <= 0.065){ // condenser pressure between these two values

                        v = procInValMap["b_0.06"]*Q_cf + procInValMap["c_0.06"];
                        if (v > 0.0){
                        greaterConstraintRhs[1] = v;
                        greaterConstraintCoef[1]["Ex"]["El"]["0"] = -procInValMap["a_0.06"];
                        greaterConstraintCoef[1]["En"]["Fuel"]["0"] = 1;
                        }
                        if (v <= 0.0){
                        lessConstraintRhs[1] = -v;
                        lessConstraintCoef[1]["Ex"]["El"]["0"] = procInValMap["a_0.06"];
                        lessConstraintCoef[1]["En"]["Fuel"]["0"] = -1;
                        }

                }

                if(0.065 < y && y <= 0.075){ // condenser pressure between these two values

                        v = procInValMap["b_0.07"]*Q_cf + procInValMap["c_0.07"];
                        if (v > 0.0){
                        greaterConstraintRhs[1] = v;
                        greaterConstraintCoef[1]["Ex"]["El"]["0"] = -procInValMap["a_0.07"];
                        greaterConstraintCoef[1]["En"]["Fuel"]["0"] = 1;
                        }
                        if (v <= 0.0){
                        lessConstraintRhs[1] = -v;
                        lessConstraintCoef[1]["Ex"]["El"]["0"] = procInValMap["a_0.07"];
                        lessConstraintCoef[1]["En"]["Fuel"]["0"] = -1;
                        }
                }

                if(0.075 < y && y <= 0.085){ // condenser pressure between these two values

                        v = procInValMap["b_0.08"]*Q_cf + procInValMap["c_0.08"];
                        if (v > 0.0){
                        greaterConstraintRhs[1] = v;
                        greaterConstraintCoef[1]["Ex"]["El"]["0"] = -procInValMap["a_0.08"];
                        greaterConstraintCoef[1]["En"]["Fuel"]["0"] = 1;
                        }
                        if (v <= 0.0){
                        lessConstraintRhs[1] = -v;
                        lessConstraintCoef[1]["Ex"]["El"]["0"] = procInValMap["a_0.08"];
                        lessConstraintCoef[1]["En"]["Fuel"]["0"] = -1;
                        }
                }

                if(0.085 < y && y <= 0.095){ // condenser pressure between these two values
                        v = procInValMap["b_0.09"]*Q_cf + procInValMap["c_0.09"];
                        if (v > 0.0){
                        greaterConstraintRhs[1] = v;
                        greaterConstraintCoef[1]["Ex"]["El"]["0"] = -procInValMap["a_0.09"];
                        greaterConstraintCoef[1]["En"]["Fuel"]["0"] = 1;
                        }
                        if (v <= 0.0){
                        lessConstraintRhs[1] = -v;
                        lessConstraintCoef[1]["Ex"]["El"]["0"] = procInValMap["a_0.09"];
                        lessConstraintCoef[1]["En"]["Fuel"]["0"] = -1;
                        }
                }

                if(0.095 < y && y <= 0.105){ // condenser pressure between these two values
                        v = procInValMap["b_0.1"]*Q_cf + procInValMap["c_0.1"];
                        if (v > 0.0){
                        greaterConstraintRhs[1] = v;
                        greaterConstraintCoef[1]["Ex"]["El"]["0"] = -procInValMap["a_0.1"];
                        greaterConstraintCoef[1]["En"]["Fuel"]["0"] = 1;
                        }
                        if (v <= 0.0){
                        lessConstraintRhs[1] = -v;
                        lessConstraintCoef[1]["Ex"]["El"]["0"] = procInValMap["a_0.1"];
                        lessConstraintCoef[1]["En"]["Fuel"]["0"] = -1;
                        }
                }

        }
                else{
                        procApp->message(62, "Condensor pressure outside of implemented range");
                }

        // Constraint2
        lessConstraintRhs[2] = 30000000;
        lessConstraintCoef[2]["Ex"]["El"]["0"] = 1;

        // Constraint3
        lessConstraintRhs[3] = 100000000;
        lessConstraintCoef[3]["En"]["Fuel"]["0"] = 1;

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

// functions

double OConPT::HeatTransferCoeff_abs_g(double S, double Tabs, double Tg, double Eabs, double Eg, double Aabs,double Ag, double &Hsabs_g)
{

        Hsabs_g = (S*(pow(Tabs, 2)+pow(Tg, 2))*(Tabs + Tg))/((1/Eabs)+(Aabs/Ag)*((1/Eg) - 1));
        return Hsabs_g;
}

double OConPT::HeatTransferCoeff_g_amb(double Tamb, double Tg, double S, double Eg, double &Hsg_amb)

{

        double Tm;
        Tm = 0.0;

        Tm = (Tg + Tamb)/2;
        Hsg_amb = 4*S*Eg*pow(Tm,3);

        return Hsg_amb;

}

double OConPT::HeatTransferCoeff_i(double &Tabs, double &Tm, double din, double vel, double &hi){

        double Reynolds(double &n, double &d,double &x, double &v, double &m);
        double Prand(double &n, double &x, double &cp, double &m);
        double Nussel(double &mu, double &muw, double &c, double &x, double &d);

        double Re, Pr, Nu, k, mu, Cp, den, muw;

        Re = 0.0;// Re is Reynolds number
        Pr = 0.0;// Pr is Prand number
        hi = 0.0;// hi internal coefficient of heat transfer (W/m2 K)
        Nu = 0.0;// Nu is the Nusselt number
        k = 0.1492+7e-6*Tm - 2e-7*pow(Tm, 2);// k fluid thermal conductivity (W/m K)
        mu = 0.0006-7e-7*Tm; // mu fluid viscosity (kg/ms)
        Cp = 715.77 + 2.82*Tm;// specific heat of fluid [J/kg K]
        den = 1190.8-0.2214*Tm-0.0008*pow(Tm, 2);// density of the fluid [kg/m3]
        muw = 0.0006-7e-7*Tabs;// fluid viscosity at the absorber wall temperature

        Re = Reynolds(din, den, mu, vel, Re);
        Pr = Prand(k, mu, Cp, Pr);
        Nu = Nussel(mu, muw, Re, Pr, Nu);

        hi= ((Nu*k) / din);
        return hi;
}

double Reynolds(double &n, double &d, double &x, double &v, double &m) {

        m = (d*v*n)/x;
        return m;
}

double Prand(double &n,double &x, double &cp, double &m) {

        m = (x*cp) / n;
        return m;
}

double Nussel(double &mu, double &muw, double &c, double &x, double &d)
{

        double n, m, C, K;
        n = 0.0; m = 0.0; C = 0.0; K = 0.0; // constants

        if(c < 2000.0){  // laminar flow long tube

                C = 3.66;
                m = 0.0;
                n = 0.0;
                K = 1;

                d = C*pow(c, m)*pow(x, n)*K;
        }
        if((c > 2000.0) && (0.6 < x  && x < 100.0))
        {                       //turbulent flow of highly viscous liquids

                C = 0.027;
                m = 0.8;
                n = 0.33;
                K = mu/muw;
                d = C*pow(c, m)*pow(x, n)*K;

        }
        else
        {
                C=0.023;
                m=0.8;
                n=0.4;
                K=1;
                d = C*pow(c, m)*pow(x, n)*K;

        }

        return d;

}

double OConPT::HeatTransferCoeff_c(double &Tg, double Tamb, double l, double nserie, double &hc){

        double Grashofc(double &tg, double &tamb, double &dn, double &n, double lt, double &gr);
        double Prandc(double &n, double &k, double &cp, double &p);
        double Nusseltc(double &gr, double &pr, double &nu);

        double Pr, Nu, Gr, k, mu, Cp, den, Tm2, lt;

        Tm2 = (Tamb+Tg)/2;
        lt = l*nserie;

                mu = 7.0e-6+4.0e-8*Tm2; //air viscosity[J/kg K]
                k = 24.1e-3+0.00006*Tm2; // air thermal conductivity [W/m K]

  // Robbie: 01.09.05: Corrected the line given below.
  //
  // The comma is clearly wrong in the value 0,1587
  //
  // Trials with a trivial program show Cp will ALWAYS be set to 1023.3
  //
  // Note that this modification HAS NOT BEEN tested. The next person to use this
  // module should confirm this change.
  //
  //   Cp  = 1023.3 - 0,1587*Tm2 + 0.0003*pow(Tm2, 2);
  //
  Cp = 1023.3-0.1587*Tm2+0.0003*pow(Tm2, 2);// specific heat of air [J/kg K]

                den = 2.699-0.0066*Tm2+5e-6*Tm2*Tm2; // air density (Kg/m3)

                hc = 0.0;// heat transfer coeficient[W/m2 K]
                Pr = 0.0;// Prand number
                Nu = 0.0;// Nusselt number
                Gr = 0.0;// Grashof number

                Pr = Prandc(mu, k, Cp, Pr);
                Gr = Grashofc(Tg, Tamb, den, mu, lt, Gr);
                Nu = Nusseltc(Gr, Pr, Nu);

                hc = Nu*k/lt;

        return hc;
}

double Prandc(double &n, double &k, double &cp, double &p){

        p = n*cp / k;
        return p;
}

double Grashofc(double &tg, double &tamb, double &den, double &n, double lt, double &gr){

        double g, b;
        gr = 0.0;
        g = 9.81;// gravity [m/s2]
        b = 1 / tamb;// coefficient of volumetric thermal expansion for gases[K-1]

        gr = g*b*pow(den, 2)*(tg-tamb)*pow(lt, 3)/ pow(n, 2);
        return gr;

}

double Nusseltc(double &gr, double &pr, double &nu){

        double C,K,n;

        if (10e4 < gr*pr && 10e9 > gr*pr)

        {
                C = 0.47;
                n = 0.25;
                K =1;
        }

        if (gr*pr > 10e9)
        {
                C = 0.1;
                n = 0.33;
                K = 1;
        }

        nu = C*K*pow(gr*pr, n);
        return nu;
}

double OConPT::Iabsorved(double idc, double ro, double tao, double alfa, double gamma, double &iabs)
{

double Kopt;

Kopt = 0.0; //  correction factor

if( idc <= 0.0) {
        Kopt = 0.0;
        iabs = 0.0;
}

else {
        Kopt = 1.0;
        iabs = idc*ro*gamma*tao*alfa*Kopt;
}

return iabs;

}

double OConPT::I_D_collect(double id, double n, double w, double latitude, double &idc)
{

        double declin, tita;

        declin = 0.0;
        declin = 23.45*sin(360*(284+n)/365*3.1416/180);
        // tita = pow(1 - pow(cos(declin*3.1416/180), 2)*pow(sin(w*3.1416/180), 2), 0.5);

        tita = pow(pow(sin(latitude*3.1416/180)*sin(declin*3.1416/180) + cos(latitude*3.1416/180)*cos(declin*3.1416/180)*cos(w*3.1416/180), 2) + pow(cos(declin*3.1416/180), 2)*pow(sin(w*3.1416/180), 2), 0.5);
        idc = id*tita;

        return idc;

}

double OConPT::solar_hour(double h, double &w)

{
        int t;
        if (h <= 12.0){
                t = 12.0-h;
                w = t*(-15.0);
        }
        if (h > 12.0){

                t = h-12.0;
                w = t*15.0;
        }
        return w;

}

// end of file
