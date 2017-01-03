
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


#include "Demand.h"
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
// CLASS: DConstEl
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant demand for electrical energy


//// DConstEl
// Standard Constructor
//
DConstEl::DConstEl(void)
{
}


//// DConstEl
// Constructor
//
DConstEl::DConstEl(App* cProcApp, Symbol procId, Symbol cProcType,
                   ioValue* cProcVal) : Proc(cProcApp, procId,
                   cProcType, cProcVal)
{

  Symbol2 a = Symbol2("El","0");
  En.insert(a);

  if (!procInValMap.element("DotEEl_0"))
    procApp->message(55, vertexId.the_string() + " DotEEl_0");
  if (procInValMap["DotEEl_0"] < 0)
    procApp->message(67, vertexId.the_string() + " DotEEl_0");
  if (procApp->testFlag)
    procApp->message(1001, "DConstEl");
}


//// ~DConstEl
// Destructor
//
DConstEl::~DConstEl(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DConstEl");
    }
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DConstEl::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength)
{
  equalConstraintRhs[1] = procInValMap["DotEEl_0"];
  equalConstraintCoef[1]["En"]["El"]["0"] = 1;
}


//// showPower
//
Symbol DConstEl::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "El";
  showPowerNumber = "0";
  return "En";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DConstMech
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant demand for mechanical energy


//// DConstMech
// Standard Constructor
//
DConstMech::DConstMech(void)
{
}


//// DConstMech
// Constructor
//
DConstMech::DConstMech(App* cProcApp, Symbol procId, Symbol cProcType,
                       ioValue* cProcVal) : Proc(cProcApp, procId,
                       cProcType, cProcVal)
{

  Symbol2 a = Symbol2("Mech","0");
  En.insert(a);

  if (!procInValMap.element("DotEMech_0"))
    procApp->message(55, vertexId.the_string() + " DotEMech_0");
  if (procInValMap["DotEMech_0"] < 0)
    procApp->message(67, vertexId.the_string() + " DotEMech_0");
  if (procApp->testFlag)
    procApp->message(1001, "DConstMech");
}


//// ~DConstMech
// Destructor
//
DConstMech::~DConstMech(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DConstMech");
    }
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DConstMech::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                    double actualIntLength)
{
  equalConstraintRhs[1] = procInValMap["DotEMech_0"];
  equalConstraintCoef[1]["En"]["Mech"]["0"] = 1;
}


//// showPower
//
Symbol DConstMech::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "Mech";
  showPowerNumber = "0";
  return "En";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DConstH
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant demand for enthalpy


//// DConstH
// Standard Constructor
//
DConstH::DConstH(void)
{
}


//// DConstH
// Constructor
//
DConstH::DConstH(App* cProcApp, Symbol procId, Symbol cProcType,
                 ioValue* cProcVal) : Proc(cProcApp, procId,
                 cProcType, cProcVal)
{
  Symbol2 a = Symbol2("H","0");
  En.insert(a);

  if (!procInValMap.element("DotEH_0"))
    procApp->message(55, vertexId.the_string() + " DotEH_0");
  if (!procInValMap.element("TF_0"))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (!procInValMap.element("TR_0"))
    procApp->message(55, vertexId.the_string() + " TR_0");
  if (procInValMap["TF_0"] <= procInValMap["TR_0"])
    procApp->message(60, vertexId.the_string() + " DotEEn[H][0]");
  if (procInValMap["DotEH_0"] < 0)
    procApp->message(67, vertexId.the_string() + " DotEH_0");
  if (procApp->testFlag)
    procApp->message(1001, "DConstH");
}


//// ~DConstH
// Destructor
//
DConstH::~DConstH(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DConstH");
    }
}


//// actualEnJ
// actualize attributes of entrance side
//
void DConstH::actualEnJ(const Map<Symbol,double>& pVecU)
{
  // calculate output

  vecJ["0"]["En"]["Out"]["F"]["T"] = procInValMap["TF_0"] ;
  vecJ["0"]["En"]["Out"]["R"]["T"] = procInValMap["TR_0"] ;
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DConstH::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                 double actualIntLength)
{
  equalConstraintRhs[1] = procInValMap["DotEH_0"];
  equalConstraintCoef[1]["En"]["H"]["0"] = 1;
}


//// showPower
//
Symbol DConstH::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "En";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DConstQ
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant demand for heat, the return flow
//          temperature is not determined in this case!


//// DConstQ
// Standard Constructor
//
DConstQ::DConstQ(void)
{
}


//// DConstQ
// Constructor
//
DConstQ::DConstQ(App* cProcApp, Symbol procId, Symbol cProcType,
                 ioValue* cProcVal) : Proc(cProcApp, procId,
                 cProcType, cProcVal)
{

  Symbol2 a = Symbol2("H","0");
  En.insert(a);

  if (!procInValMap.element("DotEQ_0"))
    procApp->message(55, vertexId.the_string() + " DotEQ_0");
  if (!procInValMap.element("TF_0"))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (procInValMap["DotEQ_0"] < 0)
    procApp->message(67, vertexId.the_string() + " DotEQ_0");
  if (procApp->testFlag)
    procApp->message(1001, "DConstQ");
}


//// ~DConstQ
// Destructor
//
DConstQ::~DConstQ(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DConstQ");
    }
}


//// actualEnJ
// actualize attributes of entrance side
//
void DConstQ::actualEnJ(const Map<Symbol,double>& pVecU)
{
  // calculate output

  vecJ["0"]["En"]["Out"]["F"]["T"] = procInValMap["TF_0"] ;
  vecJ["0"]["En"]["Out"]["R"]["T"] = 0;  // is not determined in this case!
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DConstQ::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                 double actualIntLengths)
{
  equalConstraintRhs[1] = procInValMap["DotEQ_0"];
  equalConstraintCoef[1]["En"]["H"]["0"]= 1;
}


//// showPower
//
Symbol DConstQ::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "En";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DConstWH
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant waste heat production (enthalpy flow)


//// DConstWH
// Standard Constructor
//
DConstWH::DConstWH(void)
{
}


//// DConstWH
// Constructor
//
DConstWH::DConstWH(App* cProcApp, Symbol procId, Symbol cProcType,
                   ioValue* cProcVal) : Proc(cProcApp, procId,
                   cProcType, cProcVal)
{

  Symbol2 a = Symbol2("H","0");
  Ex.insert(a);

  if (!procInValMap.element("DotEWH_0"))
    procApp->message(55, vertexId.the_string() + " DotEWH_0");
  if (!procInValMap.element("TF_0"))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (!procInValMap.element("TR_0"))
    procApp->message(55, vertexId.the_string() + " TR_0");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if (procInValMap["DotEWH_0"] < 0)
    procApp->message(67, vertexId.the_string() + " DotEWH_0");
  if (procInValMap["TF_0"] <=   procInValMap["TR_0"])
    procApp->message(60, vertexId.the_string() + " DotEEx[H][0]");
  if (procApp->testFlag)
    procApp->message(1001, "DConstWH");
}


//// ~DConstWH
// Destructor
//
DConstWH::~DConstWH(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DConstWH");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void DConstWH::actualExJ(const Map<Symbol,double>& pVecU)
{
  // calculate output

  vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"] ;
  vecJ["0"]["Ex"]["Out"]["R"]["T"] = procInValMap["TR_0"] ;
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DConstWH::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength)
{
  if (procInValMap["Flag"] == 1)
    {
      lessConstraintRhs[1] = procInValMap["DotEWH_0"];
      lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }
  if (procInValMap["Flag"] == 0)
    {
      equalConstraintRhs[1] = procInValMap["DotEWH_0"];
      equalConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }
}


//// showPower
//
Symbol DConstWH::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
showPowerType   = "H";
showPowerNumber = "0";
return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DConstWQ
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: constant waste heat production (heat flow)


//// DConstWQ
// Standard Constructor
//
DConstWQ::DConstWQ(void)
{
}


//// DConstWQ
// Constructor
//
DConstWQ::DConstWQ(App* cProcApp, Symbol procId, Symbol cProcType,
                   ioValue* cProcVal) : Proc(cProcApp, procId,
                   cProcType, cProcVal)
{

  Symbol2 a = Symbol2("H","0");
  Ex.insert(a);

  if (!procInValMap.element("DotEWQ_0"))
    procApp->message(55, vertexId.the_string() + " DotEWQ_0");
  if (!procInValMap.element("TF_0"))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if (procInValMap["DotEWQ_0"] < 0)
    procApp->message(67, vertexId.the_string() + " DotEWQ_0");
  if (procApp->testFlag)
    procApp->message(1001, "DConstWQ");
}


//// ~DConstWQ
// Destructor
//
DConstWQ::~DConstWQ(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DConstWQ");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void DConstWQ::actualExJ(const Map<Symbol,double>& pVecU)
{
  // calculate output

  vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInValMap["TF_0"] ;
  vecJ["0"]["Ex"]["Out"]["R"]["T"] = 0;  // is not determined in this case!
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DConstWQ::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                  double actualIntLength)
{
  if (procInValMap["Flag"] == 1)
    {
      lessConstraintRhs[1] = procInValMap["DotEWQ_0"];
      lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }
  if (procInValMap["Flag"] == 0)
    {
      equalConstraintRhs[1] = procInValMap["DotEWQ_0"];
      equalConstraintCoef[1]["Ex"]["H"]["0"] = 1;
    }

}


//// showPower
//
Symbol DConstWQ::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DFlucEl
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating demand for electrical energy


//// DFlucEl
// Standard Constructor
//
DFlucEl::DFlucEl(void)
{
}


//// DFlucEl
// Constructor
//
DFlucEl::DFlucEl(App* cProcApp, Symbol procId, Symbol cProcType,
                 ioValue* cProcVal) : Proc(cProcApp, procId,
                 cProcType, cProcVal)
{

  Symbol2 a = Symbol2("El","0");
  En.insert(a);

  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procApp->testFlag)
    procApp->message(1001, "DFlucEl");
}


//// ~DFlucEl
// Destructor
//
DFlucEl::~DFlucEl(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DFlucEl");
    }
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficient, and
// rhs before optimization
//
void DFlucEl::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                 double actualIntLength)
{
  if (procInTsPack.element("DotEEl_0"))
    equalConstraintRhs[1] = procInTsPack["DotEEl_0"] * procInValMap["Count"];
  else
    procApp->message(58,vertexId.the_string() + " DotEEl_0");

   if (procInTsPack["DotEEl_0"] < 0)
    procApp->message(68, vertexId.the_string() + " DotEEl_0");

  equalConstraintCoef[1]["En"]["El"]["0"] = 1;
}


//// showPower
//
Symbol DFlucEl::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "El";
  showPowerNumber = "0";
  return "En";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DFlucMech
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating demand for mechanical energy


//// DFlucMech
// Standard Constructor
//
DFlucMech::DFlucMech(void)
{
}


//// DFlucMech
// Constructor
//
DFlucMech::DFlucMech(App* cProcApp, Symbol procId, Symbol cProcType,
                     ioValue* cProcVal) : Proc(cProcApp, procId,
                     cProcType, cProcVal)
{

  Symbol2 a = Symbol2("Mech","0");
  En.insert(a);

  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procApp->testFlag)
    procApp->message(1001, "DFlucMech");
}


//// ~DFlucMech
// Destructor
//
DFlucMech::~DFlucMech(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DFlucMech");
    }
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DFlucMech::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                   double actualIntLength)
{
  if (procInTsPack.element("DotEMech_0"))
    equalConstraintRhs[1] = procInTsPack["DotEMech_0"] * procInValMap["Count"];
  else
    procApp->message(58,vertexId.the_string() + " DotEMech_0");

  if (procInTsPack["DotEMech_0"] < 0)
    procApp->message(68, vertexId.the_string() + " DotEMech_0");

  equalConstraintCoef[1]["En"]["Mech"]["0"] = 1;
}


//// showPower
//
Symbol DFlucMech::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "Mech";
  showPowerNumber = "0";
  return "En";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DFlucH
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating demand for enthalpy


//// DFlucH
// Standard Constructor
//
DFlucH::DFlucH(void)
{
}


//// DFlucH
// Constructor
//
DFlucH::DFlucH(App* cProcApp, Symbol procId, Symbol cProcType,
               ioValue* cProcVal) : Proc(cProcApp, procId,
               cProcType, cProcVal)
{

  Symbol2 a = Symbol2("H","0");
  En.insert(a);

  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;

  if (procApp->testFlag)
    procApp->message(1001, "DFlucH");
}


//// ~DFlucH
// Destructor
//
DFlucH::~DFlucH(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DFlucH");
    }
}


//// actualEnJ
// actualize attributes of entrance side
//
void DFlucH::actualEnJ(const Map<Symbol,double>& pVecU)
{
  // calculate output

  if (procInTsPack.element("TF_0"))
    vecJ["0"]["En"]["Out"]["F"]["T"] = procInTsPack["TF_0"];
  else
    procApp->message(58,vertexId.the_string() + " TF_0");

  if (procInTsPack.element("TR_0"))
    vecJ["0"]["En"]["Out"]["R"]["T"] = procInTsPack["TR_0"];
  else
    procApp->message(58,vertexId.the_string() + " TR_0");

  if (procInTsPack["TF_0"] <= procInTsPack["TR_0"])
    procApp->message(60, vertexId.the_string() + " DotEEn[H][0]");

}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DFlucH::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                double actualIntLength)
{
  if (procInTsPack.element("DotEH_0"))
    equalConstraintRhs[1] = procInTsPack["DotEH_0"] * procInValMap["Count"];
  else
    procApp->message(58,vertexId.the_string() + " DotEH_0");

  if (procInTsPack["DotEH_0"] < 0)
    procApp->message(68, vertexId.the_string() + " DotEH_0");

   equalConstraintCoef[1]["En"]["H"]["0"] = 1;
}


//// showPower
//
Symbol DFlucH::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "En";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DFlucQ
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating demand for heat, the return
//          temperature is not determined in this case!


//// DFlucQ
// Standard Constructor
//
DFlucQ::DFlucQ(void)
{
}


//// DFlucQ
// Constructor
//
DFlucQ::DFlucQ(App* cProcApp, Symbol procId, Symbol cProcType,
               ioValue* cProcVal) : Proc(cProcApp, procId,
               cProcType, cProcVal)
{

  Symbol2 a = Symbol2("H","0");
  En.insert(a);

  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;

  if (procApp->testFlag)
    procApp->message(1001, "DFlucQ");
}


//// ~DFlucQ
// Destructor
//
DFlucQ::~DFlucQ(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DFlucQ");
    }
}


//// actualEnJ
// actualize attributes of entrance side
//
void DFlucQ::actualEnJ(const Map<Symbol,double>& pVecU)
{
  // calculate output

  if (procInTsPack.element("TF_0"))
    vecJ["0"]["En"]["Out"]["F"]["T"] = procInTsPack["TF_0"];
  else
    procApp->message(58,vertexId.the_string() + " TF_0");

  vecJ["0"]["En"]["Out"]["R"]["T"] = 0;  // is not determined in this case!
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DFlucQ::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                double actualIntLength)
{
  if (procInTsPack.element("DotEQ_0"))
    equalConstraintRhs[1] = procInTsPack["DotEQ_0"] * procInValMap["Count"];
  else
    procApp->message(58,vertexId.the_string() + " DotEQ_0");

  if (procInTsPack["DotEQ_0"] < 0)
    procApp->message(68, vertexId.the_string() + " DotEQ_0");

  equalConstraintCoef[1]["En"]["H"]["0"] = 1;
}


//// showPower
//
Symbol DFlucQ::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "En";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DFlucWH
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating waste heat production (enthalpy flow)


//// DFlucWH
// Standard Constructor
//
DFlucWH::DFlucWH(void)
{
}


//// DFlucWH
// Constructor
//
DFlucWH::DFlucWH(App* cProcApp, Symbol procId, Symbol cProcType,
                 ioValue* cProcVal) : Proc(cProcApp, procId,
                 cProcType, cProcVal)
{

  Symbol2 a = Symbol2("H","0");
  Ex.insert(a);

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procApp->testFlag)
    procApp->message(1001, "DFlucWH");
}


//// ~DFlucWH
// Destructor
//
DFlucWH::~DFlucWH(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DFlucWH");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void DFlucWH::actualExJ(const Map<Symbol,double>& pVecU)
{
  // calculate output

  if (procInTsPack.element("TF_0"))
    vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInTsPack["TF_0"] ;
  else
    procApp->message(58,vertexId.the_string() + " TF_0");

  if (procInTsPack.element("TR_0"))
    vecJ["0"]["Ex"]["Out"]["R"]["T"] = procInTsPack["TR_0"] ;
  else
    procApp->message(58,vertexId.the_string() + " TR_0");

  if (procInTsPack["TF_0"] <= procInTsPack["TR_0"])
    procApp->message(60, vertexId.the_string() + " DotEEx[H][0]");
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DFlucWH::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                 double actualIntLength)
{
  if (procInTsPack.element("DotEWH_0"))
    {
      if (procInValMap["Flag"] == 1)
        {
          lessConstraintRhs[1] = procInTsPack["DotEWH_0"] * procInValMap["Count"];
          lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
        }
      if (procInValMap["Flag"] == 0)
        {
          equalConstraintRhs[1] = procInTsPack["DotEWH_0"] * procInValMap["Count"];
          equalConstraintCoef[1]["Ex"]["H"]["0"] = 1;
        }
    }
  else
    procApp->message(58,vertexId.the_string() + " DotEWH_0");

  if (procInTsPack["DotEWH_0"] < 0)
    procApp->message(68, vertexId.the_string() + " DotEWH_0");
}


//// showPower
//
Symbol DFlucWH::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DFlucWQ
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: fluctuating waste heat production (heat flow)


//// DFlucWQ
// Standard Constructor
//
DFlucWQ::DFlucWQ(void)
{
}


//// DFlucWQ
// Constructor
//
DFlucWQ::DFlucWQ(App* cProcApp, Symbol procId, Symbol cProcType,
                 ioValue* cProcVal) : Proc(cProcApp, procId,
                 cProcType, cProcVal)
{
  Symbol2 a = Symbol2("H","0");
  Ex.insert(a);

  if (!procInValMap.element("Flag"))
    procApp->message(55, vertexId.the_string() + " Flag");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;

  if (procApp->testFlag)
    procApp->message(1001, "DFlucWQ");
}


//// ~DFlucWQ
// Destructor
//
DFlucWQ::~DFlucWQ(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DFlucWQ");
    }
}


//// actualExJ
// actualize attributes of exit side
//
void DFlucWQ::actualExJ(const Map<Symbol,double>& pVecU)
{
  // calculate output

  if (procInTsPack.element("TF_0"))
    vecJ["0"]["Ex"]["Out"]["F"]["T"] = procInTsPack["TF_0"] ;
  else
    procApp->message(58,vertexId.the_string() + " TF_0");

    vecJ["0"]["Ex"]["Out"]["R"]["T"] = 0;   // is not determined in this case!
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DFlucWQ::actualSimplexInput(const Map<Symbol,double>& pVecU,
                                 double actualIntLength)
{
  if (procInTsPack.element("DotEWQ_0"))
    {
      if (procInValMap["Flag"] == 1)
        {
          lessConstraintRhs[1] = procInTsPack["DotEWQ_0"] * procInValMap["Count"];
          lessConstraintCoef[1]["Ex"]["H"]["0"] = 1;
        }
      if (procInValMap["Flag"] == 0)
        {
          equalConstraintRhs[1] = procInTsPack["DotEWQ_0"] * procInValMap["Count"];
          equalConstraintCoef[1]["Ex"]["H"]["0"] = 1;
        }
    }
  else
    procApp->message(58,vertexId.the_string() + " DotEWQ_0");

  if (procInTsPack["DotEWQ_0"] < 0)
    procApp->message(68, vertexId.the_string() + " DotEWQ_0");

}


//// showPower
//
Symbol DFlucWQ::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "Ex";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DRoom
//
///////////////////////////////////////////////////////////////////

// Module author: Thomas Bruckner
// Development thread: S01
//
// Summary: room heating demand for enthalpy


//// DRoom
// Standard Constructor

DRoom::DRoom(void)
{
}


//// DRoom
// Constructor

DRoom::DRoom(App* cProcApp, Symbol procId, Symbol cProcType,
             ioValue* cProcVal) : Proc(cProcApp, procId,
             cProcType, cProcVal)
{
  Symbol2 a = Symbol2("H","0");
  En.insert(a);

  if (!procInValMap.element("DotQRHMax"))
    procApp->message(55, vertexId.the_string() + " DotQRHMax");
  if (!procInValMap.element("T_ID"))
    procApp->message(55, vertexId.the_string() + " T_ID");
  if (!procInValMap.element("T_IN"))
    procApp->message(55, vertexId.the_string() + " T_IN");
   if (!procInValMap.element("h_D"))
    procApp->message(55, vertexId.the_string() + " h_D");
  if (!procInValMap.element("h_N"))
    procApp->message(55, vertexId.the_string() + " h_N");
  if (!procInValMap.element("TF_0"))
    procApp->message(55, vertexId.the_string() + " TF_0");
  if (!procInValMap.element("TR_0"))
    procApp->message(55, vertexId.the_string() + " TR_0");
  if (!procInValMap.element("f_RH"))
    procApp->message(55, vertexId.the_string() + " f_RH");
  if (!procInValMap.element("n"))
    procApp->message(55, vertexId.the_string() + " n");
  if (!procInValMap.element("Count"))
    procInValMap["Count"] = 1;
  if (procInValMap["n"] <= 1.0/REL_EPS)
      procApp->message(505, vertexId.the_string());
  if (procInValMap["TF_0"] <= procInValMap["TR_0"])
    procApp->message(60, vertexId.the_string() + " DotEEn[H][0]");
  if (procInValMap["DotQRHMax"] < 0)
    procApp->message(67, vertexId.the_string() + " DotQRHMax");
  if (procApp->testFlag)
    procApp->message(1001, "DRoom");
}


//// ~DRoom
// Destructor
//
DRoom::~DRoom(void)
{
  if (procApp != 0)
    {
      if (procApp->testFlag)
        procApp->message(1002, "DRoom");
    }
}


//// actualEnJ
// actualize attributes of entrance side
//
void DRoom::actualEnJ(const Map<Symbol,double>& pVecU)
{
// help variables

  double T_I = 0;
  double TF  = 0;
  double B   = 0;
  double x   = 0;
  double y   = 0;
  double z   = 0;

  if (pVecU.element("h"))       // hour of the day
    {
      if (procInTsPack.element("DotQRH"))
        {
          if ((pVecU["h"] < (procInValMap["h_N"])) &&
              (pVecU["h"] >= procInValMap["h_D"]))
            T_I = procInValMap["T_ID"];
          else
            {
              if  (((pVecU["h"] < (procInValMap["h_D"])) &&
                    (pVecU["h"] >= 0)) ||
                   ((pVecU["h"] >= procInValMap["h_N"]) &&
                    (pVecU["h"] < 24)))
                T_I = procInValMap["T_IN"];
              else
                procApp->message(62, " h");
            }

          if ((procInValMap["TR_0"] - T_I) < (T_I/REL_EPS))
            procApp->message(61,vertexId.the_string() + " TR_0,T_I");

          if ((procInValMap["TF_0"] - T_I) < (T_I/REL_EPS))
            procApp->message(61,vertexId.the_string() + " TF_0,T_I");

          x = (procInValMap["TF_0"] - T_I)/(procInValMap["TR_0"] - T_I);
          x = log(x);
          y = (procInTsPack["DotQRH"] * procInValMap["f_RH"]);
          y = y/procInValMap["DotQRHMax"];
          z = (procInValMap["n"] - 1)/procInValMap["n"];
          y = pow(y,z);
          B = exp(x * y);
          x = procInValMap["TF_0"] - procInValMap["TR_0"];
          x = x * procInTsPack["DotQRH"] * procInValMap["f_RH"];
          x = x/procInValMap["DotQRHMax"];
          y = B * x;
          y = ((1 - B) * T_I) - y;
          if (fabs(1 - B) < (1.0/REL_EPS))
            procApp->message(506,vertexId.the_string() + " B");

          TF = y/(1 - B);
          vecJ["0"]["En"]["Out"]["F"]["T"] = TF;
          y = TF - x;
          vecJ["0"]["En"]["Out"]["R"]["T"] = y;
        }
      else
        procApp->message(58,vertexId.the_string() + " DotQRH");

      if (procInTsPack["DotQRH"] < 0)
        procApp->message(68, vertexId.the_string() + " DotQRH");

    }
  else
    procApp->message(57,"h");
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coefficients, and
// rhs before optimization
//
void DRoom::actualSimplexInput(const Map<Symbol,double>& pVecU,
                               double actualIntLength)
{
  if (procInTsPack.element("DotQRH"))
    equalConstraintRhs[1] = procInTsPack["DotQRH"] * procInValMap["Count"];
  else
    procApp->message(58,vertexId.the_string() + " DotQRH");

  equalConstraintCoef[1]["En"]["H"]["0"] = 1;
}


//// showPower
//
Symbol DRoom::showPower(Symbol& showPowerType, Symbol& showPowerNumber)
{
  showPowerType   = "H";
  showPowerNumber = "0";
  return "En";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DNet4Room
//
///////////////////////////////////////////////////////////////////

// Module author: Jan Heise
// Development thread: S07
//
// Summary: fluctuating demand for heat (roomheating),
//          the return and flow temperatures are determined
//          by the destrict heating grid


//// DNet4Room
// Standard Constructor

DNet4Room::DNet4Room(void)
{
}

//// DNet4Room
// Constructor

DNet4Room::DNet4Room(App* cProcApp, Symbol procId, Symbol cProcType,
	     ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{

  Symbol2 a = Symbol2("H","0");
  En.insert(a);

  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;
  if (!procInValMap.element("T_ID"))
    procApp->message(55, vertexId.the_string()+ " T_ID");
  if (!procInValMap.element("T_FP"))
    procApp->message(55, vertexId.the_string()+ " T_FP");
  if (!procInValMap.element("TFnet_min"))
    procApp->message(55, vertexId.the_string()+ " TFnet_min");
  if (!procInValMap.element("TFnet_0"))
    procApp->message(55, vertexId.the_string()+ " TFnet_0");
  if (!procInValMap.element("TEnvNet_min"))
    procApp->message(55, vertexId.the_string()+ " TEnvNet_min");
  if (!procInValMap.element("TEnvNet_0"))
    procApp->message(55, vertexId.the_string()+ " TEnvNet_0");
  if (!procInValMap.element("A"))
    procApp->message(55, vertexId.the_string()+ " A");
  if (!procInValMap.element("B"))
    procApp->message(55, vertexId.the_string()+ " B");
  if (!procInValMap.element("C"))
    procApp->message(55, vertexId.the_string()+ " C");
  if (!procInValMap.element("D"))
    procApp->message(55, vertexId.the_string()+ " D");

  if(procInValMap["TFnet_0"] < procInValMap["TFnet_min"])
	procApp->message(61, vertexId.the_string()+ " TFnet_0, TFnet_min");
  if(procInValMap["TEnvNet_0"] > procInValMap["TEnvNet_min"])
	procApp->message(61, vertexId.the_string()+ " TEnvNet_0, TEnvNet_min");

  if (procApp->testFlag)
    procApp->message(1001, "DNet4Room");
}

//// ~DNet4Room
// Destructor

DNet4Room::~DNet4Room(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
	procApp->message(1002, "DNet4Room");
    }
}


//// actualEnJ
// actualize attributes of entrance side

void DNet4Room::actualEnJ(const Map<Symbol,double> & pVecU)
{
//calculate output
// variables

  double TEnv = 0;
  double a = 0; // help
  double x = 0; // help

  if (pVecU.element("T"))
      TEnv =pVecU["T"];
    else
      procApp->message(57,"T");
// calculate TF,en
  if (TEnv > procInValMap["TEnvNet_min"])
	vecJ["0"]["En"]["Out"]["F"]["T"] =procInValMap["TFnet_min"];
  else
  {
	if (TEnv > procInValMap["TEnvNet_0"])
	{
	a=procInValMap["TFnet_0"]-procInValMap["TFnet_min"];
	a=a/(procInValMap["TEnvNet_0"]-procInValMap["TEnvNet_min"]);
  	x=a*(TEnv-procInValMap["TEnvNet_0"])+procInValMap["TFnet_0"];
  	vecJ["0"]["En"]["Out"]["F"]["T"] =x;
	}
  	else
	vecJ["0"]["En"]["Out"]["F"]["T"] =procInValMap["TFnet_0"];
  }
// calculate TR,en
  if (TEnv > procInValMap["T_ID"])
	vecJ["0"]["En"]["Out"]["R"]["T"] =procInValMap["T_FP"];
  else
  {
	if (TEnv > procInValMap["TEnvNet_0"])
	{
	x=procInValMap["A"]+procInValMap["B"]*TEnv;
	x=x+procInValMap["C"]*TEnv*TEnv+procInValMap["D"]*TEnv*TEnv*TEnv;
  	vecJ["0"]["En"]["Out"]["R"]["T"] =x;
	}
  	else
	{
	a=procInValMap["TEnvNet_0"];
	x=procInValMap["A"]+procInValMap["B"]*a;
	x=x+procInValMap["C"]*a*a+procInValMap["D"]*a*a*a;
	vecJ["0"]["En"]["Out"]["R"]["T"] =x;
	}
  }
}

//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void DNet4Room::actualSimplexInput(const Map<Symbol,double> & pVecU,
				double actualIntLength)
{
  if (procInTsPack.element("DotQRH"))
    equalConstraintRhs[1]  = procInTsPack["DotQRH"] * procInValMap["Count"];
  else
    procApp->message(58,vertexId.the_string()+" DotQRH");

  if (procInTsPack["DotQRH"] <0)
    procApp->message(68, vertexId.the_string()+ " DotQRH");

  equalConstraintCoef[1]["En"]["H"]["0"]= 1;
}

//// showPower

Symbol DNet4Room::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
showPowerType="H";
showPowerNumber="0";
return "En";
}


///////////////////////////////////////////////////////////////////
//
// CLASS: DHeat
//
///////////////////////////////////////////////////////////////////

// Module author: Jan Heise
// Development thread: S07
//
// Summary: room heating demand for enthalpy with minimal flow temperature
//          for drinkwater heating


//// DHeat
// Standard Constructor

DHeat::DHeat(void)
{
}

//// DHeat
// Constructor

DHeat::DHeat(App* cProcApp, Symbol procId, Symbol cProcType,
	     ioValue* cProcVal) :Proc(cProcApp,procId,cProcType,cProcVal)
{
  Symbol2 a = Symbol2("H","0");
  En.insert(a);

  if (!procInValMap.element("DotQRHMax"))
    procApp->message(55, vertexId.the_string()+ " DotQRHMax");
  if (!procInValMap.element("T_ID"))
    procApp->message(55, vertexId.the_string()+ " T_ID");
  if (!procInValMap.element("T_IN"))
    procApp->message(55, vertexId.the_string()+ " T_IN");
   if (!procInValMap.element("h_D"))
    procApp->message(55, vertexId.the_string()+ " h_D");
  if (!procInValMap.element("h_N"))
    procApp->message(55, vertexId.the_string()+ " h_N");
  if (!procInValMap.element("TF_0"))
    procApp->message(55, vertexId.the_string()+ " TF_0");
  if (!procInValMap.element("TR_0"))
    procApp->message(55, vertexId.the_string()+ " TR_0");
  if (!procInValMap.element("f_RH"))
    procApp->message(55, vertexId.the_string()+ " f_RH");
  if (!procInValMap.element("n"))
    procApp->message(55, vertexId.the_string()+ " n");
  if (!procInValMap.element("Count"))
    procInValMap["Count"]=1;
  if (!procInValMap.element("TF_min"))
    procApp->message(55, vertexId.the_string()+ " TF_min");
  if (!procInValMap.element("f_MFR"))
    procApp->message(55, vertexId.the_string()+ " f_MFR");




  if (procInValMap["n"] <= 1.0/REL_EPS)
      procApp->message(505, vertexId.the_string());

  if (procInValMap["TF_0"] <=  procInValMap["TR_0"])
    procApp->message(60, vertexId.the_string()+ " DotEEn[H][0]");

  if (procInValMap["DotQRHMax"] <0)
    procApp->message(67, vertexId.the_string()+ " DotQRHMax");

  if (procApp->testFlag)
    procApp->message(1001, "DRoom");
}

//// ~DHeat
// Destructor

DHeat::~DHeat(void)
{
  if (procApp !=0)
    {
      if (procApp->testFlag)
	procApp->message(1002, "DRoom");
    }
}

//// actualEnJ
// actualize attributes of entrance side

void DHeat::actualEnJ(const Map<Symbol,double> & pVecU)
{
// help variables

double T_I =0;
double TF =0;
double B =0;
double x=0;
double y=0;
double z=0;

if (pVecU.element("h"))       // hour of the day
  {
    if (procInTsPack.element("DotQRH"))
      {
	if ((pVecU["h"] < (procInValMap["h_N"])) &&
	    (pVecU["h"] >= procInValMap["h_D"]))
	  T_I = procInValMap["T_ID"];
	else
	  {
	    if  (((pVecU["h"] < (procInValMap["h_D"])) &&
		  (pVecU["h"] >= 0)) ||
		 ((pVecU["h"] >= procInValMap["h_N"]) &&
		  (pVecU["h"] < 24)))
	      T_I = procInValMap["T_IN"];
	    else
	      procApp->message(62, " h");
	  }

	if ((procInValMap["TR_0"]- T_I) < (T_I/REL_EPS))
	  procApp->message(61,vertexId.the_string()+" TR_0,T_I");

	if ((procInValMap["TF_0"]- T_I) < (T_I/REL_EPS))
	  procApp->message(61,vertexId.the_string()+" TF_0,T_I");

	x = (procInValMap["TF_0"]- T_I)/(procInValMap["TR_0"]- T_I);
	x = log(x);     // <math.h>
	y = (procInTsPack["DotQRH"]*procInValMap["f_RH"]);
	y = y/procInValMap["DotQRHMax"];
	z = (procInValMap["n"]-1)/procInValMap["n"];
	y = pow(y,z);   // <math.h>
	B = exp(x*y);
	x = procInValMap["TF_0"]-procInValMap["TR_0"];
	x = x * procInTsPack["DotQRH"]*procInValMap["f_RH"];
        x = x /procInValMap["DotQRHMax"];
	y = B *x;
	y = ((1-B)*T_I)-y;
	if (fabs(1-B) < (1.0/REL_EPS))
	  procApp->message(506,vertexId.the_string()+" B");

	TF = y/(1-B);
	  if (TF < procInValMap["TF_min"]) // indication for summertime
		{
		TF = procInValMap["TF_min"];
		vecJ["0"]["En"]["Out"]["F"]["T"] =TF;
		y = TF - x/procInValMap["f_MFR"];
		vecJ["0"]["En"]["Out"]["R"]["T"] =y;
		}
	  else
		{
	  vecJ["0"]["En"]["Out"]["F"]["T"] =TF;
	  y = TF - x;
	  vecJ["0"]["En"]["Out"]["R"]["T"] =y;
		}
      }
    else
      procApp->message(58,vertexId.the_string()+" DotQRH");

    if (procInTsPack["DotQRH"] <0)
      procApp->message(68, vertexId.the_string()+ " DotQRH");

  }
else
  procApp->message(57,"h");
}


//// actualSimplexInput
// actualize objective function coefficients, constraint coef. and
// rhs before optimization

void DHeat::actualSimplexInput(const Map<Symbol,double> & pVecU,
			       double actualIntLength)
{
  if (procInTsPack.element("DotQRH"))
    equalConstraintRhs[1]  = procInTsPack["DotQRH"] * procInValMap["Count"];
  else
    procApp->message(58,vertexId.the_string()+" DotQRH");

  equalConstraintCoef[1]["En"]["H"]["0"]= 1;
}

//// showPower

Symbol DHeat::showPower(Symbol & showPowerType, Symbol & showPowerNumber)
{
showPowerType="H";
showPowerNumber="0";
return "En";
}



// end of file






