
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
//  $Date: 2005/11/25 13:50:30 $
//  $Author: morrison $
//  $RCSfile: Data.h,v $


//////////////////////////////////////////////////////////////////
//
// DEECO DATA MANAGEMENT OBJECTS
//
//////////////////////////////////////////////////////////////////


#ifndef _DATA_H_              // header guard
#define _DATA_H_


#include     <String.h>       // easy string management
#include     <Symbol.h>       // use Symbol not String for keys
#include        <Map.h>       // associative array
#include       <Path.h>       // necessary for right instantiation of <Symbol.h> and <Map.h>
#include    <fstream.h>       // file management


#include        "App.h"


//////////// basic data structures ///////////////////////////////


//////////////////////////////////////////////////////////////////
//
// CLASS: Symbol2
//
//////////////////////////////////////////////////////////////////

// Summary: Symbol2 will be used as a type in templates like
// Map<class X,class Y>; according to USL C++ Standards Components,
// Rel. 3.0, p.9-3, the constructor of the class Y is not allowed
// to require arguments

class Symbol2
{
public:

 // Standard Constructor
    Symbol2(void);
 // Constructor
    Symbol2(Symbol cComp1, Symbol cComp2);
 // Destructor
    virtual ~Symbol2(void);
 // equality operator , necessary for use of Symbol2 as a part of Sets
    friend int operator==(const Symbol2& s1, const Symbol2& s2);
 // write Symbol2
    friend ostream& operator<<(ostream& os, const  Symbol2& s);

    Symbol comp1;
    Symbol comp2;

};


//////////////////////////////////////////////////////////////////
//
// CLASS: MeanValRec
//
//////////////////////////////////////////////////////////////////

// Summary: MeanValRec will be used as a type in templates like
// Map<class X,class Y>; according to USL C++ Standards Components,
// Rel. 3.0, p.9-3

class MeanValRec
{
public:

 // Standard Constructor
    MeanValRec(void);
 // Constructor
    MeanValRec(App* cmApp);
 // Destructor
    virtual ~MeanValRec(void);
 // update Mean
    void updateMeanValRec(double val);
 // update Mean (variable weights)
    void updateMeanValRec(double val, double weight);
 // clear Mean
    void clearMeanValRec(void);
 // expand a double to a MeanValRec
    void expand(double val);
 // cutoff a MeanValRec
    double cutoff(void);
 // show maximum
    double maximum(void);
 // show minimum
    double minimum(void);
 // write mean record
    friend ostream& operator<<(ostream& os, const MeanValRec& mvr);

protected:

    App* mApp;
    double mean;
    double dev;         // standard deviation (square root of variance)
    double max;
    double min;
    double sumVal;
    double sumSquareVal;
    int count;
    double norm;

};



////////////////////////////////////////////////////////////////
//
// typedef
//
///////////////////////////////////////////////////////////////

typedef Map<Symbol, MeanValRec> MapSym1M;
typedef Map<Symbol, MapSym1M> MapSym2M;
typedef Map<Symbol, MapSym2M> MapSym3M;
typedef Map<Symbol, MapSym3M> MapSym4M;
typedef Map<Symbol, MapSym4M> MapSym5M;
typedef Map<Symbol, MapSym5M> MapSym6M;

typedef Map<Symbol, double>   MapSym1d;
typedef Map<Symbol, MapSym1d> MapSym2d;
typedef Map<Symbol, MapSym2d> MapSym3d;
typedef Map<Symbol, MapSym3d> MapSym4d;
typedef Map<Symbol, MapSym4d> MapSym5d;
typedef Map<Symbol, MapSym5d> MapSym6d;


///////////// data structures for input-output management ////////


//////////////////////////////////////////////////////////////////
//
// CLASS: ioData
//
//////////////////////////////////////////////////////////////////

// Summary: abstract input-output data class

class ioData
{
public:

 // Standard Constructor
    ioData(void);
 // Constructor
    ioData(App* cioApp, String ioDN);
 // Destructor
    virtual ~ioData(void);
 // read file
    void read(String ioFN);
 // write file
    void write(String ioFN);
 // test existing of files
    int existFile(String ioFN);
 // error Management
    int error(void);

    ifstream finData;                   // <fstream.h>
    ofstream foutData;

protected:

    App* ioApp;                         // "App.h"
    String ioDataName;                  // <String.h>
    Path ioFileName;                    // <Path.h>
    Symbol Id;                          // <Symbol.h>
    int errorFlag;

    // help routines (pure virtual funcions, should be overwritten)

    virtual void readRecord(void) = 0;
    virtual void writeData(void)  = 0;

};


//////////////////////////////////////////////////////////////////
//
// CLASS: ioDefValue
//
//////////////////////////////////////////////////////////////////

// Summary: for reading definition table files

class ioDefValue : public ioData
{
public:

 // Standard Constructor
    ioDefValue(void);
 // Constructor
    ioDefValue(App* cDefApp, String dTN);
 // Destructor
    virtual ~ioDefValue(void);
 // write input data (control)
    virtual void writeInput(void);
 // selection check
    virtual int selected(Symbol sId);

    Map<Symbol, int> defMap;            // <Map.h>, <Symbol.h>

protected:

    virtual void readRecord(void);
    virtual void writeData(void);

};


//////////////////////////////////////////////////////////////////
//
// CLASS: ioValue
//
//////////////////////////////////////////////////////////////////

// Summary: for reading parameter table files and writing
// result data files (mean values)

class ioValue : public ioData
{
public:

 // Standard Constructor
    ioValue(void);
 // Constructor
    ioValue(App* cValApp, String vN);
 // Destructor
    virtual ~ioValue(void);
 // clear output file
    void clearFile(String ioFN);
 // write input data (control)
    void writeInput(void);
 // append to result file
    void write(String ioFN, int scanF, double scanV);
 // clear result mean values after finishing one sceniario scanning parameter
    void  clearResults(void);

    Map<Symbol, Map<Symbol, double> > inValMap;        // <Map.h>, <Symbol.h>
    Map<Symbol, Map<Symbol, MeanValRec> > outValMap;   // see above for definition
                                                       // of  MeanValRec
protected:

    Symbol paramId;

    virtual void readRecord(void);
    virtual void writeData(void);                      // only to overwrite the pure
                                                       // virtual function
                                                       // ioData::writeData(void)
    virtual void writeData(int scanF, double scanV);

};


//////////////////////////////////////////////////////////////////
//
// CLASS: ioTsPack
//
//////////////////////////////////////////////////////////////////

// Summary: here "Pack" stands for all time series data of a
// certain interval; in general there is more than one row in the
// ts-file including such data!
// help class of ioTsValue, no descendant of ioData !

class  ioTsPack
{
public:

 // Standard Constructor
    ioTsPack(void);
 // Constructor
    ioTsPack(App* ctsPackApp, String tsDN, int tsIC);
 // Destructor
    virtual ~ioTsPack(void);
 // read pack form file, tsFN is the name of the corresponding ioTsFile
    void readPack(String tsFN, ifstream& finTsData, int tsIntCount);
 // write pack to file
    void writePack(String tsFN, ofstream& foutTsData, int tsIntCount);
 // error Management
    int error(void);

    Map<Symbol, Map<Symbol, double> > inTsPackMap;  // the map of data for a
                                                    // given interval
    Map<Symbol, Map<Symbol, double> > outTsPackMap;

protected:

    App* tsPackApp;                     // "App.h"
    Symbol Id;
    Symbol paramId;
    String intName;
    int errorFlag;

 //// help functions ////////////////////////////

 // read a single data record from file
    virtual void readRecord(ifstream& finTsData);

};


//////////////////////////////////////////////////////////////////
//
// CLASS: ioTsValue
//
//////////////////////////////////////////////////////////////////

class ioTsValue: public ioData
{
public:

 // Standard Constructor
    ioTsValue(void);
 // Constructor
    ioTsValue(App* cTsApp, String tsDN, Path inTsFileName,
              Path outTsFileName, int outFlag);
 // Destructor
    virtual ~ioTsValue(void);

protected:

 //// help functions ////////////////////////////

    // overwrite pure virtual functions of ioData

    virtual void readRecord(void);
    virtual void writeData(void);

};


#endif  // _DATA_H_


//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/Data.h,v $
//  end of file

