
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


//  $Revision: 1.2 $
//  $Date: 2005/11/25 16:55:45 $
//  $Author: morrison $
//  $RCSfile: Scen.h,v $


//////////////////////////////////////////////////////////////////
//
// DEECO SCENARIO MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////


#ifndef _SCEN_H_              // header guard
#define _SCEN_H_

#include     <String.h>       // easy string management
#include     <Symbol.h>       // use Symbol not String for keys
#include        <Map.h>       // associative array
#include       <Path.h>       // necessary for right instantiation of <Symbol.h> and <Map.h>
#include    <fstream.h>       // file I/O


#include        "App.h"
#include       "Data.h"
#include       "Proc.h"
#include      "Balan.h"
#include    "Connect.h"


//////////////////////////////////////////////////////////////////
//
// CLASS: ScenTsPack
//
//////////////////////////////////////////////////////////////////

class ScenTsPack : public ioTsPack
{
public:

 // Standard Constructor
    ScenTsPack(void);
 // Constructor
    ScenTsPack(App* ctsPackApp, String tsN, int tsIC);
 // Destructor
    virtual ~ScenTsPack(void);
 // read pack from file : usage of the virtual function readRecord is sufficient
 // write pack to file
    void writePack(String tsFN, ofstream& foutTsData, int tsIC);

    Map<Symbol, double> vecU;     // environment data and load factors
    Map<Symbol, double> vecC;     // general cost data

protected:

 // read single data record
    virtual void readRecord(ifstream& finTsData);

};


//////////////////////////////////////////////////////////////////
//
// CLASS: ScenTs
//
//////////////////////////////////////////////////////////////////

class ScenTs : public ioTsValue
{
public:

 // Standard Constructor
    ScenTs(void);
 // Constructor
    ScenTs(App* cScenTsApp, String scenTsDN, String scenInTsFN,
           String scenOutTsFN, int outFlag);
 // Destructor
    virtual ~ScenTs(void);

protected:

};


//////////////////////////////////////////////////////////////////
//
// CLASS: ScenVal
//
//////////////////////////////////////////////////////////////////

// Note: scenario data input files don't have a database format

class ScenVal : public ioValue
{
public:

 // Standard Constructor
    ScenVal(void);
 // Constructor
    ScenVal(App* cValApp,  String vN, double init); // init is used to
                                                    // initialize goalWeight,
                                                    // see <Map.h>
 // Destructor
    virtual ~ScenVal(void);
 // write Scenario Input Values in log-file
    void writeInput(void);
 // run scenario with actual input values
    void run(String scenId);
 // update result mean values after each time interval
    void update(const Map<Symbol, double>& vecU);
 // clear result mean values after finishing one sceniario scanning parameter
    void clearResults(void);

    Map<Symbol, double> goalWeight;         // vector of goal function weigths
    Map<Symbol, MeanValRec> scenOutValMap;
    Symbol  CComp;
    Path resPath;             // path for result files
    Path scenPath;            // path for control files
                              //   reserved for that scenario, which
                              //   overwrites the control files in
                              //   projectPath
    Symbol scanParaProcName;  // <Symbol.h> scan one parameter
    Symbol scanParaName;      //   of one process for each scenario
    double scanParaBegin;
    double scanParaEnd;
    double scanParaStep;
    int scanFlag;
    int tsOutFlag;            // write time series result file
    int allResFlag;           // show all results

    Path scenInValFileName;
    Path scenOutValFileName;
    int dynFlag;              // 0 = quasidynamic, 1 = dynamic optimization

    Path scenInTsFileName;    // name of time-series file for environment data
    Path scenOutTsFileName;   // name of time-series file for output data
    Path procInTsFileName;    // name of time-series file for process input data
    double intLength;         // length of time intervals
    int intNumber;            // total number of time intervals

protected:

 //// help functions /////////

 // read scenario input value file
    virtual void readRecord(void);
 // write result data
    virtual void writeData(void);
    virtual void writeData(int scanF, double scanV);

};


#endif  // _SCEN_H_


//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/Scen.h,v $
//  end of file

