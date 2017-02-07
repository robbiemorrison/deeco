
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

//  $Revision: 1.4 $
//  $Date: 2005/11/25 17:55:08 $
//  $Author: morrison $
//  $RCSfile: Scen.C,v $

//////////////////////////////////////////////////////////////////
//
// DEECO SCENARIO MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////

#include  <strstream.h>       // string streams
#include     <stdlib.h>       // exit()
#include    <iomanip.h>       // formatting of ostreams
#include    <sstream>

#include   "TestFlag.h"
#include       "Scen.h"

//////////////////////////////////////////////////////////////////
//
// CLASS: ScenTsPack
//
//////////////////////////////////////////////////////////////////

//// ScenTsPack
// Standard constructor
//
ScenTsPack::ScenTsPack(void)
{
  // no content, call ioTsPack(void)
}

//// ScenTsPack
// Constructor
//
ScenTsPack::ScenTsPack(App* ctsPackApp, String tsDN, int tsIC)
  : ioTsPack(ctsPackApp, tsDN, tsIC)
{
  // no contents
}

//// ~ScenTsPack
// Destructor
//
ScenTsPack::~ScenTsPack(void)
{
  // no content, call ~ioTsPack(void)
}

//// writePack
// write pack to file
//
void ScenTsPack::writePack(String tsFN, ofstream& foutTsData,
                           int tsIntCount)
{
  for (Mapiter<Symbol, double> vecCIt =
       vecC.first(); vecCIt; vecCIt.next())           // <Map.h>
    {
      foutTsData.setf(ios::left, ios::adjustfield);   // <iostream.h>
      foutTsData <<  setw(4)  << tsIntCount           << " , "
                 <<  setw(10) << vecCIt.curr()->key   << " , "
                 <<  setw(8)  << vecCIt.curr()->value
                 << "\n" << flush;  // Robbie: 23.09.05: Added flush.
                             // <Map.h>,  USL C++ Standards Components,
                             // Rel. 3.0, p.9-5
    }
  if (foutTsData.fail())
    {
      errorFlag++;
      tsPackApp->message(17, tsFN);
      foutTsData.close();
    }
}

////////// help functions /////////////

//// readRecord
// read a single data record from file
//
void ScenTsPack::readRecord(ifstream& finTsData)
{
  int i = 0;
  i = tsPackApp->readComma(finTsData);            // read ","
  finTsData >> Id;
  i = i + tsPackApp->readComma(finTsData);        // read ","
  finTsData >>  vecU[Id];
   if (i != 0)
     finTsData.clear(ios::badbit | finTsData.rdstate());
                                            // raise errorflags of finData
                                            // <fstream.h> -> <iostream.h>
}

//////////////////////////////////////////////////////////////////
//
// CLASS: ScenTs
//
//////////////////////////////////////////////////////////////////

//// ScenTs
// Standard Constructor
//
ScenTs::ScenTs(void)
{
  // no content, call ioTsValue(void)
}

//// ScenTs
// Constructor
//
ScenTs::ScenTs(App* cScenTsApp, String scenTsDN, String scenInTsFN,
               String scenOutTsFN, int outFlag) : ioTsValue(cScenTsApp,
               scenTsDN, scenInTsFN, scenOutTsFN, outFlag)
{
  // no content
}

//// ~ScenTs
// Destructor
//
ScenTs::~ScenTs(void)
{
  // no content, call ~ioTsValue(void)
}

//////////////////////////////////////////////////////////////////
//
// CLASS: ScenVal
//
//////////////////////////////////////////////////////////////////

//// ScenVal
// Standard Constructor
//
ScenVal::ScenVal(void)
{
  resPath            = ".";
  scenPath           = ".";
  scenInValFileName  = ".siv";
  scenOutValFileName = ".sov";
  scenInTsFileName   = ".sit";
  scenOutTsFileName  = ".sot";
  procInTsFileName   = ".pit";
  scanParaProcName   = "";
  scanParaName       = "";
  scanParaBegin      = 0;
  scanParaEnd        = 0;
  scanParaStep       = 1;
  scanFlag           = 0;
  dynFlag            = 0;
  tsOutFlag          = 0;
  allResFlag         = 0;
  intLength          = 3600;        // nominal time interval length = 1 hour
  intNumber          = 8760;        // time horizon length = 1 year
}

//// ScenVal
// Constructor
//
ScenVal::ScenVal(App * cValApp, String vN, double init)
                 : ioValue(cValApp, vN), goalWeight(init)
{
  // defaults

  resPath            = cValApp->projectPath;
  scenPath           = cValApp->projectPath;
  scenInValFileName  = cValApp->projectPath/Path(cValApp->projectName+".siv");
  scenOutValFileName = cValApp->projectPath/Path(cValApp->projectName+".sov");
  scenInTsFileName   = cValApp->projectPath/Path(cValApp->projectName+".sit");
  scenOutTsFileName  = cValApp->projectPath/Path(cValApp->projectName+".sot");
  procInTsFileName   = cValApp->projectPath/Path(cValApp->projectName+".pit");
  scanParaProcName   = "";
  scanParaName       = "";
  scanParaBegin      = 0;
  scanParaEnd        = 0;
  scanParaStep       = 1;
  scanFlag           = 0;
  dynFlag            = 0;
  tsOutFlag          = 0;
  allResFlag         = 0;
  intLength          = 3600;         // nominal time interval length = 1 hour
  intNumber          = 8760;         // time horizon length = 1 year
}

//// ~ScenVal
// Destructor
//
ScenVal::~ScenVal(void)
{
  // no content, call ~ioValue(void)
}

//// readRecord
// read scenario input value file
//
void ScenVal::readRecord(void)
{
  if (!ioApp->emptyLine(finData))
    {
      char c;
      int i = 0;
      finData >> c;
      switch(c)
        {
        case 'P':
          finData >> c;
          switch(c)
            {
            case 'R':
              i = ioApp->readComma(finData);      // read ","
              finData >> resPath;
              break;
            case 'S':
              i = i + ioApp->readComma(finData);  // read ","
              finData >> scenPath;
              break;
            }
          break;
        case 'T':
          finData >> c;
          switch(c)
            {
            case 'S':
              i = i + ioApp->readComma(finData);  // read ","
              finData >>  scenInTsFileName;
              break;
            case 'P':
              i = i + ioApp->readComma(finData);  // read ","
              finData >>  procInTsFileName;
              break;
            }
          break;
        case 'S':
          i = i + ioApp->readComma(finData);      // read ","
          finData >>  scanParaProcName;
          i = i + ioApp->readComma(finData);      // read ","
          finData >>  scanParaName ;
          i = i + ioApp->readComma(finData);      // read ","
          finData >>  scanParaBegin;
          i = i + ioApp->readComma(finData);      // read ","
          finData >>  scanParaEnd;
          i = i + ioApp->readComma(finData);      // read ","
          finData >>  scanParaStep;
          scanFlag = 1;
          break;
        case 'D':
          i = i + ioApp->readComma(finData);      // read ","
          finData >> dynFlag;
          break;
        case 'E':
          i = i + ioApp->readComma(finData);      // read ","
          finData >> tsOutFlag;
          break;
        case 'A':
          i = i + ioApp->readComma(finData);      // read ","
          finData >> allResFlag;
          break;
        case 'I':
          i = i + ioApp->readComma(finData);      // read ","
          finData >> intLength;
          i = i + ioApp->readComma(finData);      // read ","
          finData >> intNumber;
          break;
        case 'G':
          i = i + ioApp->readComma(finData);      // read ","
          finData >>  CComp;
          i = i + ioApp->readComma(finData);      // read ","
          finData >>  goalWeight[CComp];
          break;
        }
      if (i != 0)
        finData.clear(ios::badbit | finData.rdstate());
                                           // raise errorflags of finData
                                           // <fstream.h> -> <iostream.h>
    }
  ioApp->ignoreRestLine(finData); // ignore the rest of the line
}

//// writeScenInVal
// write actual scenario input values
//
void  ScenVal::writeInput(void)
{
#ifdef _TEST_XXX  // TOFIX: 25.11.05: disabled with XXX because
                  // seg-faults, see below too
  if (ioApp->testFlag)
    {
      ostrstream ssScenInVal;                // <strstream.h>
      ssScenInVal         << ioDataName       << "\n"
               << "PR , " << resPath          << "\n"
               << "PS , " << scenPath         << "\n"
               << "TS , " << scenInTsFileName << "\n"
               << "TP , " << procInTsFileName << "\n"
               << "S  , " << scanParaProcName << " , "
                          << scanParaName     << " , "
                          << scanParaBegin    << " , "
                          << scanParaEnd      << " , "
                          << scanParaStep     << "\n"
               << "D  , " << dynFlag          << "\n"
               << "E  , " << tsOutFlag        << "\n"
               << "A  , " << allResFlag       << "\n"
               << "I  , " << intLength        << " , "
                          << intNumber        << "\n"
      //       << "G  , " << goalWeight       << "\n" // TOFIX: disabling prevents seg-fault
                          << ends;
      char* pScenInVal = ssScenInVal.str();
      ioApp->message(1010, pScenInVal);
      delete pScenInVal;      // Robbie: 18.11.2005: delete was delete[],
                              // deallocation is correct, see man strstream
                              // and method str().
    }
#endif
#ifdef _TEST_  // alternative using <sstream> but which cannot report on 'G'
  if (ioApp->testFlag)
    {
      std::string sTemp;                          // intermediary variable

      std::ostringstream ssScenInVal;             // <sstream>
      ssScenInVal         << ioDataName                    << "\n"
               << "PR , " << resPath                       << "\n"
               << "PS , " << scenPath                      << "\n"
               << "TS , " << scenInTsFileName              << "\n"
               << "TP , " << procInTsFileName              << "\n"
               << "S  , " << scanParaProcName.the_string() << " , " // Symbol to String
                          << scanParaName.the_string()     << " , " // Symbol to String
                          << scanParaBegin                 << " , "
                          << scanParaEnd                   << " , "
                          << scanParaStep                  << "\n"
               << "D  , " << dynFlag                       << "\n"
               << "E  , " << tsOutFlag                     << "\n"
               << "A  , " << allResFlag                    << "\n"
               << "I  , " << intLength                     << " , "
                          << intNumber                     << "\n"
      //       << "G  , " << goalWeight                    << "\n"  // see note 1
               << "G  , " << "** goalWeight not supported in this code: "
                          << __FILE__
                          << " L"
                          << __LINE__
                          << " **\n"
                          << std::ends;

      // note 1: disabled as no support for: std::basic_ostream << Map<Symbol, double>

      sTemp             = ssScenInVal.str();     // create intermediary
      String sScenInVal = sTemp.c_str();         // client code need not delocate memory
      ioApp->message(1003, sScenInVal);
    }
#endif
}

//// update
// update result mean values after each time interval
//
void ScenVal::update(const Map<Symbol, double>& vecU)
{
  for (Mapiter<Symbol, double> vecUIt = vecU.first();
       vecUIt; vecUIt.next())
    scenOutValMap[vecUIt.curr()->key].updateMeanValRec(vecUIt.curr()->value);
}

//// clearResults
// clear result mean values after finishing one sceniario scanning parameter
//
void ScenVal::clearResults(void)
{
  for (Mapiter<Symbol, MeanValRec> scenOutValMapIt = scenOutValMap.first();
       scenOutValMapIt; scenOutValMapIt.next())
    scenOutValMapIt.curr()->value.clearMeanValRec();
}

////// help function ////////////////////////////

//// writeData
// should not be used
//
void ScenVal::writeData(void)
{
  ioApp->message(1, "Usage of ScenVal::writeData(void)");
}

//// writeData
//
void ScenVal::writeData(int scanF, double scanV)
{
  for (Mapiter<Symbol,MeanValRec> scenOutValMapIt = scenOutValMap.first();
       scenOutValMapIt; scenOutValMapIt.next())     // <Map.h>
    {
      foutData.setf(ios::left, ios::adjustfield);   // <iostream.h>

      // write scanValue at the beginning of each line if scanFlag = 1
      // <Map.h>,  USL C++ Standards Components, Rel. 3.0, p9-5

      // Robbie: 25.11.05: Following declaration not needed: ostrstream ssScanVal;

      if (scanF)
        {
          foutData << scanV                                     << " , "
                   << setw(20) << scenOutValMapIt.curr()->key   << " , "
                   << setw(8)  << scenOutValMapIt.curr()->value
                   << "\n"
                   << flush;  // Robbie: 23.09.05: Added flush.
        }
      else
        {
          foutData << setw(20) << scenOutValMapIt.curr()->key   << " , "
                   << setw(8)  << scenOutValMapIt.curr()->value
                   << "\n"
                   << flush;  // Robbie: 23.09.05: Added flush.
        }
    }
}

//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/Scen.C,v $
//  end of file
