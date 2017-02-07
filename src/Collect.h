
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

#ifndef _COLL_                // header guard
#define _COLL_

#include "App.h"
#include "Data.h"
#include "DGraph.h"
#include "Proc.h"
#include <String.h>           // easy string management
#include <Symbol.h>           // use Symbol instead of String for keys
#include <Map.h>              // associative array

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

class OSol : public Proc
{
public:

 // Standard Constructor
    OSol(void);
 // Constructor
    OSol(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~OSol(void);
 // actualize attributes of exit side
    virtual void actualExJ(const Map<Symbol,double>& pVecU);
 // actualize objective function coefficients, constraint coefficients, and
 // rhs before optimization
    virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                    double actualIntLength);
 // add process-dependent fix costs to the fix costs vector
    virtual void addFixCosts(Map<Symbol, MeanValRec>& scenOutValMap,
                             Map<Symbol, MapSym1d>& aggInValMap,
                             Map<Symbol, MapSym1M>& aggOutValMap);
protected:

    double x1;    // help variables
    double x2;    // help variables

};

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
// (no heat exchanger, linear efficiency dependency)

class OSolHEx : public Proc
{
public:
// Standard Constructor
   OSolHEx(void);
// Constructor
   OSolHEx(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
   virtual ~OSolHEx(void);
// actualize attributes of exit side
   virtual void actualExJ(const Map<Symbol,double> & pVecU);
// actualize objective function coefficients, constraint coef. and
// rhs before optimization
   virtual void actualSimplexInput(const Map<Symbol,double> & pVecU,
                                   double actualIntLength);
// add process-dependent fix costs to the fix costs vector
  virtual void addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
                           Map<Symbol, MapSym1d> & aggInValMap,
                           Map<Symbol, MapSym1M> & aggOutValMap);
protected:
double x1,x2;    //help variables
};

///////////////////////////////////////////////////////////////////
//
// CLASS: OPVSol
//
///////////////////////////////////////////////////////////////////

// Module author: Johannes Bruhn
// Development thread: S04
//
// Summary: Photovoltaic Electric Energy

class OPVSol : public Proc
{
public:
// Standard Constructor
   OPVSol(void);
// Constructor
   OPVSol(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
   virtual ~OPVSol(void);
// actualize objective function coefficients, constraint coef. and
// rhs before optimization
   virtual void actualSimplexInput(const Map<Symbol,double> & pVecU,
                                   double actualIntLength);
// add process-dependent fix costs to the fix costs vector
   virtual void addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
                           Map<Symbol, MapSym1d> & aggInValMap,
                           Map<Symbol, MapSym1M> & aggOutValMap);
};

///////////////////////////////////////////////////////////////////
//
// CLASS: OWind
//
///////////////////////////////////////////////////////////////////

// Module author: Johannes Bruhn
// Development thread: S04
//
// Summary: Windpower System, simple model

class OWind : public Proc
{
public:
// Standard Constructor
   OWind(void);
// Constructor
   OWind(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
   virtual ~OWind(void);
// actualize objective function coefficients, constraint coef. and
// rhs before optimization
   virtual void actualSimplexInput(const Map<Symbol,double> & pVecU,
                                   double actualIntLength);
// add process-dependent fix costs to the fix costs vector
   virtual void addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
                           Map<Symbol, MapSym1d> & aggInValMap,
                           Map<Symbol, MapSym1M> & aggOutValMap);
};

///////////////////////////////////////////////////////////////////
//
// CLASS: OWind2
//
///////////////////////////////////////////////////////////////////

// Module author: Johannes Bruhn
// Development thread: S04
//
// Summary: Windpower System, extended model

class OWind2 : public Proc
{
public:
// Standard Constructor
   OWind2(void);
// Constructor
   OWind2(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
// Destructor
   virtual ~OWind2(void);
// actualize objective function coefficients, constraint coef. and
// rhs before optimization
   virtual void actualSimplexInput(const Map<Symbol,double> & pVecU,
                                   double actualIntLength);
// add process-dependent fix costs to the fix costs vector
   virtual void addFixCosts(Map<Symbol, MeanValRec> & scenOutValMap,
                           Map<Symbol, MapSym1d> & aggInValMap,
                           Map<Symbol, MapSym1M> & aggOutValMap);
};

///////////////////////////////////////////////////////////////////
//
// CLASS: OConPT
//
///////////////////////////////////////////////////////////////////

// Module author: Susanna Medel
// Development thread: S11
//
// Summary: solar-thermal power station with concentrating
//          parabolic trough collectors

class OConPT : public Proc
{
public:

 // Standard Constructor
    OConPT(void);
 // Constructor
    OConPT(App* cProcApp, Symbol procId, Symbol cProcType, ioValue* cProcVal);
 // Destructor
    virtual ~OConPT(void);
 // actualize objective function coefficients, constraint coefficients, and
 // rhs before optimization
    virtual void actualSimplexInput(const Map<Symbol,double>& pVecU,
                                    double actualIntLength);

protected:

 // declarations
    double HeatTransferCoeff_abs_g(double S, double Tabs, double Tg, double Eabs, double Eg, double Aabs, double Ag, double &Hsabs_g);
    double HeatTransferCoeff_g_amb(double Tamb, double Tg, double S, double Eg, double &Hsg_amb);
    double HeatTransferCoeff_c(double &Tg, double Tamb, double l, double nserie, double &hc);
    double HeatTransferCoeff_i(double &Tabs, double &Tm, double din, double vel, double &hi);
    double Iabsorved(double idc,double ro, double tao, double alfa, double gamma, double &iabs);
    double I_D_collect(double id, double n, double w, double latitude, double &idc);
    double solar_hour(double h, double &w);

 // help variable in Collect.C
    double y;
    double Q_c, m, T_abs, T_g, Hg_amb, Habs_g, H_c, H_i, C_p, T_m, Sigma, Kc, Fr, F, I_abs, I_Dc, Q_cv, Q_cf, absolut, v;
    double Ta, Id, w, h, doy;
    int i, x;

};

#endif  // _COLL_
