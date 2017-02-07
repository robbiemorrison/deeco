
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
//  $Date: 2005/11/25 17:53:35 $
//  $Author: morrison $
//  $RCSfile: Data.C,v $

//////////////////////////////////////////////////////////////////
//
// DEECO DATA MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////

#include    <fstream.h>       // file I/O
#include  <strstream.h>       // string streams
#include    <iomanip.h>       // format of ostreams
#include       <math.h>       // mathematics
#include      <float.h>       // for numerical infinity (DBL_MAX, DBL_MIN)

// Robbie: Remarked out later:
//
// #include  <ieeefp.h>       // not used on HP-UX, common in SVR4

#include   "TestFlag.h"
#include      "Data.h"

// for comparison of (x-x == 0) a value REL_EPS should be used
// according to the HP-UX Floating Point Guide
// for single precision  REL_EPS= 10e5, double prec. REL_EPS = 10e14
// and quad. prec. REL_EPS = 10e26;
// use then  (x-x =0) --> ( fabs(x-x) < (x/REL_EPS) )
// fabs() -> <math.h>

#define REL_EPS  10e14

//////////////////////////////////////////////////////////////////
//
// CLASS: Symbol2
//
//////////////////////////////////////////////////////////////////

// Summary: Symbol2 will be used as a type in templates like
// Map<class X,class Y>; according to USL C++ Standards Components,
// Rel. 3.0, p.9-3, the constructor of the class Y is not allowed
// to require arguments

//// Symbol2
// Standard Constructor
//
Symbol2::Symbol2(void)
{
}

//// Symbol2
// Constructor
//
Symbol2::Symbol2(Symbol cComp1,
                 Symbol cComp2)
{
  comp1 = cComp1;
  comp2 = cComp2;
}

//// ~Symbol2
// Destructor
//
Symbol2::~Symbol2(void)
{
  // no content
}

//// operator==
// equality operator, necessary for use of Symbol2 as a part of Sets
//
int
operator==(const Symbol2& s1,
           const Symbol2& s2)
{
  return (s1.comp1 == s2.comp1 && s1.comp2 == s2.comp2);
}

//// operator<<
// write Symbol2
//
ostream&
operator<<(ostream&        os,
           const Symbol2&  s)
{
  os << "(" << s.comp1 << "," << s.comp2 << ")";
  return os;
}

//////////////////////////////////////////////////////////////////
//
// CLASS: MeanValRec
//
//////////////////////////////////////////////////////////////////

// Summary: MeanValRec will be used as a type in templates like
// Map<class X,class Y>; according to USL C++ Standards Components,
// Rel. 3.0, p.9-3

//// MeanValRec
// Standard Constructor
//
MeanValRec::MeanValRec(void)
{
  mApp         = NULL;
  mean         = 0;
  dev          = 0;
  max          = -DBL_MAX;
  min          = DBL_MAX;
  sumVal       = 0;
  sumSquareVal = 0;
  count        = 0;
  norm         = 0;
}

//// MeanValRec
// Constructor
//
MeanValRec::MeanValRec(App* cmApp)
{
  mApp         = cmApp;
  mean         = 0;
  dev          = 0;
  max          = -DBL_MAX;
  min          = DBL_MAX;
  sumVal       = 0;
  sumSquareVal = 0;
  count        = 0;
  norm         = 0;

#ifdef _TEST_  // inner cycle test only if necessary
  if (cmApp->testFlag)
    cmApp->message(1001, "MeanValRec");
#endif
}

////~MeanValRec
// Destructor
//
MeanValRec::~MeanValRec(void)
{
#ifdef _TEST_               // inner cycle test only if necessary
  if (mApp != 0)            // correctly initialized? e.g. no standard
                            // constructor
    {
      if (mApp->testFlag)
        mApp->message(1002, "MeanValRec");
    }
#endif
}

// Robbie: Added for SDK port.
//
static
double make_Inf()
{
#if 0
  fpsetmask(FP_X_CLEAR);
  double r = 1.0/0.0;
  fpresetdefaults();
#else
  double a = 0.0;
  double r = 1.0/a;
#endif
  return r;
}

// Robbie: Added for SDK port.
//
static
double make_NaN()
{
#if 0
  fpsetmask(FP_X_CLEAR);
  double r = 0.0/0.0;
  fpresetdefaults();
#else
  double a = 0.0;
  double r = 0.0/a;
#endif
  return r;
}

//// updateMeanValRec
// update Mean
//
void
MeanValRec::updateMeanValRec(double val)
{
  if (val > max)                   // max is initialized with - DBL_MAX <float.h>
    {
      max = val;
    }
  if (val < min)                   // min is initialized with DBL_MAX <float.h>
    {
      min = val;
    }
  sumVal = sumVal + val;
  sumSquareVal = sumSquareVal + (val * val);
  count = count + 1;
  mean = sumVal/double(count);     // to ensure that mean is of type double
  if (count > 1)
    {
      double x;
      x = mean * mean;
      double y = x;
      x = sumSquareVal/double(count) - x;
      //
      // "Varianz", Bronstein, Taschenbuch der Mathematik, 1985, p666

      if (x <= (y/double(REL_EPS)))     // <math.h> // !2.8.95: fabs(x)->x
        {
          x = 0;
        }
      dev = sqrt(x);                    // <math.h>
    }
  else
    {
      dev = make_NaN();
    }
}

//// updateMeanValRec
// update mean (variable weights)
//
void
MeanValRec::updateMeanValRec(double val, double weight)
{
  if (val > max)
    {
      max = val;
    }
  if (val < min)
    {
      min = val;
    }
  sumVal = sumVal + (val * weight);
  sumSquareVal = sumSquareVal + (val * val * weight);
  norm = norm + weight;
  mean = norm  != 0 ?  sumVal/norm
       : sumVal < 0 ? -make_Inf()
       :               make_Inf();

       // This used to be done as plain sumVal/norm with
       // fpsetmask(FP_X_CLEAR)/fpsetdefaults() around it;
       // but that is less portable.
       //
       // NOTE:  ask a statistician about algorithms for *STABLY*
       // updating the mean and standard deviation (there's one
       // published in the Collected Algorithms of the ACM and
       // doubtless there are others).

  if (fabs(norm-weight)> (norm/REL_EPS))
    {
      double x;
      x = mean * mean;
      double y = x;

  // Robbie: rem'd fpsetmask()
  //   fpsetmask(FP_X_CLEAR); // don't trap floating point exception for the moment

      x = (sumSquareVal/norm) - x;
      // "Varianz", Bronstein, Taschenbuch der Mathematik, 1985, p667
      if (x <= (y/double(REL_EPS)))     // !2.8.95: fabs(x)->x
        x = 0;

  // Robbie: rem'd fpsetdefaults()
  //   fpsetdefaults();        // enable floating point exception trap <math.h>

      dev = sqrt(x);
    }
  else
    {
      dev = make_NaN();
    }
}

//// clearMeanValRec
// clear Mean after usage
//
void
MeanValRec::clearMeanValRec(void)
{
  mean         = 0;
  dev          = 0;
  max          = -DBL_MAX;
  min          = DBL_MAX;         // pregnant as a starting value (see below)
  sumVal       = 0;
  sumSquareVal = 0;
  count        = 0;
  norm         = 0;
}

//// expand
// expand a double to a MeanValRec
//
void
MeanValRec::expand(double val)
{
  mean = val;
  double indiff = make_NaN();
  dev  = indiff;
  max  = indiff;
  min  = indiff;
}

//// cutoff
// cutoff a MeanValRec
//
double
MeanValRec::cutoff(void)
{
  return mean;
}

//// maximum
// show maximum
//
double
MeanValRec::maximum(void)
{
  return max;
}

//// minimum
// show minimum
//
double
MeanValRec::minimum(void)
{
  return min;
}

//// operator<<
// write mean record

ostream&
operator<<(ostream&           os,
           const MeanValRec&  mvr)
{
  // simple database format

  os << setw(11) << mvr.mean << " , " << setw(11) << mvr.dev << " , "
     << setw(11) << mvr.min  << " , " << setw(11) << mvr.max;

  return os;
}

//////////////////////////////////////////////////////////////////
//
// CLASS: ioData
//
//////////////////////////////////////////////////////////////////

// Summary: abstract input-output data class

//// ioData
// Standard Constructor
//
ioData::ioData(void)
{
  // defaults

  ioApp      = NULL;
  ioDataName = "";
  ioFileName = "";
  Id         = "";
  errorFlag  = 0;
}

//// ioData
// Constructor
//
ioData::ioData(App*    cioApp,
               String  ioDN)
{
  // defaults

  ioApp      = cioApp;
  ioDataName = ioDN;
  ioFileName = "";
  Id         = "";
  errorFlag  = 0;

  if (ioApp->testFlag)
    {
      ioApp->message(1001, ioDataName);
    }
}

//// ~ioData
// Destructor
//
ioData::~ioData(void)
{
  if (ioApp != 0)      // correctly initialized? for example, no standard constructor
    {
      if (ioApp->testFlag)
        ioApp->message(1002,ioDataName);
    }
}

//// read
// read table from file

void
ioData::read(String ioFN)
{
  finData.open(ioFN, ios::nocreate);               // <fstream.h>
  if (!finData.fail())                             // <fstream.h>
    {
      while(!(finData.eof() || finData.fail()))
        {
          readRecord();                            // pure virtual help function,
        }                                          // see below
      if (finData.fail())
        {
          errorFlag++;
          ioApp->message(16, ioFN);
        }
      else
        {
          if (ioApp->testFlag)
            {
              ioApp->message(1011, ioFN);
            }
        }
      finData.close();
    }
  else
    {
      ioApp->message(12, ioFN);
      errorFlag++;
    }
}

//// write
// write file
//
void
ioData::write(String ioFN)
{
  foutData.open(ioFN, ios::trunc);
  if (!foutData.fail())
    {
      writeData();         // pure virtual help function, see below
      if (foutData.fail())
        {
          errorFlag++;
          ioApp->message(17, ioFN);
        }
      else
        {
          if (ioApp->testFlag)
            {
              ioApp->message(1012, ioFN);
            }
        }
      foutData.close();
    }
  else
    {
      ioApp->message(13, ioFN);
      errorFlag++;
    }
}

//// existFile
// test existing of files
//
int
ioData::existFile(String ioFN)
{
  finData.open(ioFN, ios::nocreate);
  if (!finData.fail())
    {
      finData.close();
      return 1;
    }
  else
    {
      return 0;
    }
}

//// error
// error Management
//
int
ioData::error(void)
{
  if (errorFlag)
    {
      return errorFlag;
    }
  else
    {    return 0;
    }
}

//////////////////////////////////////////////////////////////////
//
// CLASS: ioDefValue
//
//////////////////////////////////////////////////////////////////

// Summary: for reading definition table files

//// ioDefValue
// Standard Constructor
//
ioDefValue::ioDefValue(void)
{
  // no content, call ioData(void)
}

//// ioDefValue
// Constructor
//
ioDefValue::ioDefValue(App*    cDefApp,
                       String  dTN)
  : ioData(cDefApp, dTN)
{
  // no content
}

//// ~ioDefValue
// Destructor
//
ioDefValue::~ioDefValue(void)
{
  // no content, call ~ioData(void)
}

//// writeInput
// write input data (control)
//
void ioDefValue::writeInput(void)
{
#ifdef _TEST_
  if (ioApp->testFlag)
    {
      ostrstream ssDefVal;              // <strstream.h>
      ssDefVal << ioDataName
               <<" : "
               << defMap
               << "\n"
               << ends;
      char* pDefVal = ssDefVal.str();   // <strstream.h>, "freeze" to char*
      ioApp->message(1009, pDefVal);
      delete pDefVal;         // Robbie: 18.11.2005: delete was delete[],
                              // deallocation is correct, see man strstream
                              // and method str().
    }
#endif
}

//// selected
// selection check
//
int
ioDefValue::selected(Symbol sId)
{
  if (defMap.element(sId))              // <Map.h>
    {
      if (defMap[sId] == 0)
        {
          return 0;
        }
      else
        {
          return 1;
        }
    }
  else
    {
      return 0;
    }
}

///////// help routines //////////////////////////////////////////

//// readRecord
// read record from file
//
void
ioDefValue::readRecord(void)
{
  if (!ioApp->emptyLine(finData))
    {
      int i = 0;
      finData >> Id;
      i = ioApp->readComma(finData);       // read ","
      finData >> defMap[Id];
      if (i != 0)
        {
          finData.clear(ios::badbit | finData.rdstate());
        }                                  // raise errorflags of finData
                                           // <fstream.h> -> <iostream.h>
    }
  ioApp->ignoreRestLine(finData);          // "App.h"
}

//// writeData;
//
void
ioDefValue::writeData(void)
{
  // no content, no result data to write
}

//////////////////////////////////////////////////////////////////
//
// CLASS: ioValue
//
//////////////////////////////////////////////////////////////////

// Summary: for reading parameter table files and writing
// result data files (mean values)

//// ioValue
// standard Constructor
//
ioValue::ioValue(void)
{
  // no content, call ioData(void)
}

//// ioValue
// constructor
//
ioValue::ioValue(App*    cValApp,
                 String  vN)
  : ioData(cValApp, vN)
{
  // no content
}

//// ~ioValue
// Destructor
//
ioValue::~ioValue(void)
{
  // no content, call ~ioData(void)
}

//// writeInput
// write input data (control)
//
void
ioValue::writeInput(void)
{
#ifdef _TEST_
  if (ioApp->testFlag)
    {
      ostrstream ssInVal;               // <strstream.h>
      ssInVal << ioDataName
              << " : "
              << inValMap
              << "\n"
              << ends;
      char* pInVal = ssInVal.str();     // <strstream.h>,"freeze" to char*
      ioApp->message(1010, pInVal);
      delete pInVal;          // Robbie: 18.11.2005: delete was delete[],
                              // deallocation is correct, see man strstream
                              // and method str().
    }
#endif
}

//// clearFile
// normally, output files are opened in deeco with ...ios::trunc, for
// example, they are deleted before "write" is used; to avoid problems,
// when, scanFlag is set, ioValue output files are deleted explicitly
//
void
ioValue::clearFile(String ioFN)
{
  foutData.open(ioFN, ios::trunc);
  if (!foutData.fail())
    {
      foutData.close();
    }
  else
    {
      ioApp->message(23, ioFN);
      errorFlag++;
    }
}

//// write
// append to result file, overloading of ioData::write(String)
//
void
ioValue::write(String  ioFN,
               int     scanF,
               double  scanV)
{
  if (scanF)
    {
      foutData.open(ioFN, ios::app);    // append to files containing
    }                                   // results from scenarios which differ
                                        // only because of another scanning
                                        // parameter value
  else
    {
      foutData.open(ioFN, ios::trunc);    // delete old files
    }
  if (!foutData.fail())
    {
      writeData(scanF, scanV);
      if (foutData.fail())
        {
          errorFlag++;
          ioApp->message(17, ioFN);
        }
      else
        {
          if (ioApp->testFlag)
            {
              ioApp->message(1012, ioFN);
            }
        }
      foutData.close();
    }
  else
    {
      ioApp->message(13, ioFN);
      errorFlag++;
    }
}

//// clear
// clear result mean values after finishing one scenario scanning parameter
//
void
ioValue::clearResults(void)
{
  for (Mapiter<Symbol, Map<Symbol, MeanValRec> > outValMapIt = outValMap.first();
       outValMapIt; outValMapIt.next())   // <Map.h>, see USL C++ Standard
                                          // Components Release 3.0,p 9-5
    {
      Map<Symbol, MeanValRec> idOutValMap = outValMapIt.curr()->value;
      for (Mapiter<Symbol, MeanValRec>  idOutValMapIt = idOutValMap.first();
          idOutValMapIt; idOutValMapIt.next())
        idOutValMapIt.curr()->value.clearMeanValRec();
    }
}

///////// help routines //////////////////////////////////////////

//// readRecord
// read record from file
//
void
ioValue::readRecord(void)
{
  if (!ioApp->emptyLine(finData))
    {
      int i = 0;
      finData >> Id;
      i = ioApp->readComma(finData);       // read ","
      finData >> paramId;
      i = i + ioApp->readComma(finData);   // read ","
      finData >> inValMap[Id][paramId];    // <Map.h>
      if (i != 0)
        {
          finData.clear(ios::badbit | finData.rdstate());
        }                                  // raise errorflags of finData
                                           // <fstream.h> -> <iostream.h>
    }
  ioApp->ignoreRestLine(finData);          // "App.h"
}

//// write
// overwrite pure virtual function of ioData
//
void
ioValue::writeData(void)
{
  ioApp->message(1, "Usage of pure virtual function ioValue::writeData(void)");
}

//// writeData
// write in file
//
void
ioValue::writeData(int     scanF,
                   double  scanV)
{
  for (Mapiter<Symbol, Map<Symbol, MeanValRec> > outValMapIt = outValMap.first();
       outValMapIt; outValMapIt.next())   // <Map.h>, see USL C++ Standard
                                          // Components Release 3.0,p 9-5
    {
      Map<Symbol, MeanValRec> idOutValMap = outValMapIt.curr()->value;
      for (Mapiter<Symbol, MeanValRec>  idOutValMapIt = idOutValMap.first();
           idOutValMapIt; idOutValMapIt.next())
        {
          foutData.setf(ios::left, ios::adjustfield);   // <iostream.h>

          // write scanValue at the beginning of each line if scanFlag =1

          if (scanF)
            {
              foutData << scanV                                 << " , "
                       << setw(5)  << outValMapIt.curr()->key   << " , "
                       << setw(21) << idOutValMapIt.curr()->key << " , "
                       << idOutValMapIt.curr()->value
                       << "\n" << flush;  // Robbie: 23.09.05: Added flush.
            }
          else
            {
              foutData << setw(5)  << outValMapIt.curr()->key   << " , "
                       << setw(21) << idOutValMapIt.curr()->key << " , "
                       << idOutValMapIt.curr()->value
                       << "\n" << flush;  // Robbie: 23.09.05: Added flush.
            }
        }
    }
}

//////////////////////////////////////////////////////////////////
//
// CLASS: ioTsPack
//
//////////////////////////////////////////////////////////////////

// Summary: here "Pack" stands for all time series data of a
// certain interval; in general there is more than one row in the
// ts-file including such data!
// help class of ioTsValue, no descendant of ioData !

//// ioTsPack
// Standard Constructor
//
ioTsPack::ioTsPack(void)
{
  // defaults

  tsPackApp = NULL;
  Id        = "";
  paramId   = "";
  intName   = "";
  errorFlag = 0;
}

//// ioTsPack
// Constructor
//
ioTsPack::ioTsPack(App*    ctsPackApp,
                   String  tsDN,
                   int     tsIntCount)
{
  // defaults

  tsPackApp = ctsPackApp;
  Id        = "";
  paramId   = "";
  errorFlag = 0;
  intName   = int_to_str(tsIntCount) + ". " + tsDN;

#ifdef _TEST_                 // inner circle control only in test phase
  if (tsPackApp->testFlag)
    {
      tsPackApp->message(1001, intName);
    }
#endif
}

//// ~ioTsPack
// Destructor
//
ioTsPack::~ioTsPack(void)
{
#ifdef _TEST_                 // inner circle control only in test phase
  if (tsPackApp != 0)         // correctly initialized?
    {
      if (tsPackApp->testFlag)
        tsPackApp->message(1002, intName);
    }
#endif
}

//// readPack
// read pack from file
//
void
ioTsPack::readPack(String     tsFN,
                   ifstream&  finTsData,
                   int        tsIntCount)
{
  int i = 0;

// Robbie: CC warning: variable "j" was set but never used.
//  int j = 0;

  streampos actualPos = 0;                 // <fstream.h>, position in file

  if (!finTsData.eof())                    // !1.8.95
    {
      do
        {
          actualPos = finTsData.tellg();   // <fstream.h>, get current position
          if (!tsPackApp->emptyLine(finTsData))
            {
              finTsData >> i;
              if (i == tsIntCount)
                {
                  readRecord(finTsData);     // help function, see below
                }
            }
          if(i <= tsIntCount)
            {
              tsPackApp->ignoreRestLine(finTsData);  // ignore rest of the line

// Robbie: see 19 lines above
//              j = i;                     // save i

            }
        }
      while (!finTsData.eof() && i <= tsIntCount && !finTsData.fail());
    }                                      // !1.8.95

  if (finTsData.fail())
    {
      errorFlag++;
      tsPackApp->message(16, tsFN);
      finTsData.close();
    }

//     if(j != tsIntCount)
//     {
//       tsPackApp->message(15, int_to_str(tsIntCount));
//       errorFlag++;
//     }   // make this for each input data separately

  if (!finTsData.eof() && !finTsData.fail())
    {
      // the number of the next interval is already read;
      // go before that number, this may make problems under
      // DOS (text != binary) Unix: text==binary

      finTsData.seekg(actualPos); // <fstream.h>, go to the beginning of
                                  // the line; finTsData.peek() is not
                                  // usable because i is an integer and not
                                  // a character
    }
}

//// writePack
// write pack to file
//
void
ioTsPack::writePack(String     tsFN,
                    ofstream&  foutTsData,
                    int        tsIntCount)
{
  for (Mapiter<Symbol, Map<Symbol, double> > outTsPackMapIt =
       outTsPackMap.first(); outTsPackMapIt; outTsPackMapIt.next())  // <Map.h>
    {
      Map<Symbol, double> idOutTsPackMap = outTsPackMapIt.curr()->value;
      for (Mapiter<Symbol, double> idOutTsPackMapIt = idOutTsPackMap.first();
          idOutTsPackMapIt; idOutTsPackMapIt.next())
        {
          foutTsData.setf(ios::left, ios::adjustfield);              // <iostream.h>
          foutTsData << setw(4)  << tsIntCount                     << " , "
                     << setw(5)  << outTsPackMapIt.curr()->key     << " , "
                     << setw(21) << idOutTsPackMapIt.curr()->key   << " , "
                     << setw(11) << idOutTsPackMapIt.curr()->value
                     << "\n" << flush;  // Robbie: 23.09.05: Added flush.
                         // <Map.h>,  USL C++ Standards Components, Rel. 3.0, p9-5
        }
    }
    if (foutTsData.fail())
      {
        errorFlag++;
        tsPackApp->message(17, tsFN);
        foutTsData.close();
      }
}

//// error
// error Management
//
int
ioTsPack::error(void)
{
  if (errorFlag)
    {
      return errorFlag;
    }
  else
    {
      return 0;
    }
}

//// help functions ////////////////////////////

//// readRecord
// read a single data record from file
//
void
ioTsPack::readRecord(ifstream& finTsData)
{
  int i = 0;
  i = tsPackApp->readComma(finTsData);           // read ","
  finTsData >> Id;
  i = i + tsPackApp->readComma(finTsData);       // read ","
  finTsData >> paramId;
  i = i + tsPackApp->readComma(finTsData);       // read ","
  finTsData >> inTsPackMap[Id][paramId];
  if (i != 0)
    {
      finTsData.clear(ios::badbit | finTsData.rdstate());
    }                                   // raise errorflags of finData
}                                       // <fstream.h> -> <iostream.h>

//////////////////////////////////////////////////////////////////
//
// CLASS: ioTsValue
//
//////////////////////////////////////////////////////////////////

//// ioTsValue
// Standard Constructor
//
ioTsValue::ioTsValue(void)
{
  // no content, call ioData(void)
}

//// ioTsValue
// Constructor
//
ioTsValue::ioTsValue(App*    cTsApp,
                     String  tsDN,
                     Path    inTsFileName,
                     Path    outTsFileName,
                     int     outFlag)
  : ioData(cTsApp, tsDN)
{
  finData.open(inTsFileName, ios::nocreate);
  if (finData.fail())
    {
      errorFlag++;
      ioApp->message(12, inTsFileName);
    }
  if (outFlag)
    {
      foutData.open(outTsFileName, ios::trunc);
      if (foutData.fail())
        {
          errorFlag++;
          ioApp->message(13, outTsFileName);
        }
    }
  else
    {
      foutData.clear(ios::badbit | foutData.rdstate());
    }

    // set badbit of foutData; don't use foutData in the
    // case that outFlag = 0, for example, don't close it
    // in ioTsValue::~ioTsValue(void)
}

//// ~ioTsValue
// Destructor
//
ioTsValue::~ioTsValue(void)
{
  if (!finData.fail())
    {
      finData.close();
    }
  if (!foutData.fail())
    {
      foutData.close();
    }
}

///////////// help functions ////////////////

//// readRecord
// overwrite pure virtual function of ioData; should not be used;
// use readPack instead
//
void
ioTsValue::readRecord(void)
{
  ioApp->message(1, "Usage of pure virtual function ioTsValue::readRecord(void)");
}

//// writeData;
// overwrite pure virtual function of ioData; should not be used;
// use writePack instead
//
void
ioTsValue::writeData(void)
{
  ioApp->message(1, "Usage of pure virtual function ioTsValue::writeData(void)");
}

//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/Data.C,v $
//  end of file
