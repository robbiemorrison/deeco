
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
// DYNAMIC ENERGY EMISSION AND COST OPTIMIZATION : deeco
//
//////////////////////////////////////////////////////////////////


#define MYHAND 1              // 1 = dedicated exception handling, otherwise 0


#include     <stdlib.h>       // for use of exit function


#ifdef MYHAND
#  include      <new.h>       // for use of a new handler
#  include     <math.h>       // for catching math errors, see matherr
#  include   "except.h"       // not needed in HP-UX
#  include   <signal.h>       // for catching the interrupt signal
#  include  <sstream>         // contemporary string stream without client deallocation
#  include   <string>         // to interface with std::ostringstream
#endif


#include        "App.h"
#include   "deecoApp.h"
#include    "version.h"       // contains user-defined DEECO_VERSION_STRING


// Robbie: 10.02.03: Binary name placed in global space -- not good style
//
String binary_name;           // 'name' of application binary


#ifdef MYHAND
App* mApp;                    // forward declaration in global scope,
#endif                        //   constructed later in main() -- this
                              //   is to support the error handlers
                              //   which are implemented as non-member
                              //   functions


//////////////////////////////////////////////////////////////////
//
// FUNCTION: main
//
//////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])   // main including command line arguments
{

  // Robbie: 10.02.03: Name of binary file, used for run-time reporting.
  //
  binary_name = argv[0];           // global variable, used in "App.C"

  // read command line arguments

  String sComandArg;               // "App.h" - > "deecoApp.h" - ><String.h>
  char c;
  int comSilentFlag = 0;
  int comTestFlag   = 0;
  int comLogFlag    = 0;
  String comProjectName = "deeco";
  String comProjectPath = "";
  int i;
  for (i = 1; i < argc; i++)       // read command line arguments
    {
      sComandArg = argv[i];
                                   // char* to String conversion
      sComandArg.getX(c);          // "App.h" - > "deecoApp.h" - > <String.h>
                                   // get first char of sCommandArg
      if (c == '-')                // if  sCommandArg is a flag
        {
          sComandArg.getX(c);      // get the next char of sCommandArg
          switch(c)
            {
            case 's': comSilentFlag = 1;
                      break;
            case 't': comTestFlag = 1;
                      break;
            case 'l': comLogFlag = 1;
                      break;
            case 'n': comProjectName = sComandArg;   // get the rest
                      break;
            case 'd': comProjectPath = sComandArg;   // get the rest
                      break;
            }
        }
    }

  // catching the interrupt signal

#ifdef MYHAND
  void myInterruptHandler(int);              // see below
  if (signal(SIGINT, SIG_IGN) != SIG_IGN)    // do if an interrupt signal is
    signal(SIGINT, myInterruptHandler);      // detected, <signal.h>
#endif

  // catching floating point errors

  // Robbie: rem'd fpsetdefaults()
  //
  //  fpsetdefaults(); // floating point error handling <math.h>: Round to nearest;
                       // clear all exception flags; enable exception traps
                       // except underflow and inexact result; set underflowing
                       // values equal to 0, see HP-UX Floating Point Guide
                       // (lrom); Chapter 5; if an exception occurs which is trapped
                       // the SIGFPE Signal <signal.h> is generated which is
                       // handled by "myFloatHandler"
                       // fpsetdefaults is "better" than the default values of
                       // the system!

#ifdef MYHAND
  void myFloatHandler(int);                  // see below
  if (signal(SIGFPE, SIG_IGN) != SIG_IGN)    // do if an float error signal is
    signal(SIGFPE, myFloatHandler);          // detected, <signal.h>
#endif

  // new-handling

#ifdef MYHAND
  void myNewHandler(void);             // see below
  set_new_handler (myNewHandler);      // <new.h>
#endif

  // "main" program

  // Robbie: 07.02.03: Added a little more flexibility to the version sequence
  //
  String deeco_ver_str;
  deeco_ver_str  = "deeco";                // identifier must not contain spaces
  deeco_ver_str += " ";
  deeco_ver_str += DEECO_VERSION_STRING;   // defined in "version.h"

  // Robbie: 09.08.05: 'deeco' exit status.
  //
  // The 'deeco' exit status no longer hard-coded to 0.
  //
  // The new method checks the errorFlag and modifies 'deeco' exit
  // status variable deeco_exit_status accordingly.
  //
  // Note too that the only modifications for this functionality were
  // three places in this file. These are all below this point. No
  // other code required changing.
  //
  int deeco_exit_status = 0;

  App* mApp = new App(comLogFlag,
                      0,
                      comSilentFlag,
                      comTestFlag,
                      deeco_ver_str,
                      "deeco.log",
                      "deeco.msg",
                      comProjectPath,
                      comProjectName);

  if (mApp == 0)
    {
      cerr << "Freestore exceeded" << flush;  // Robbie: 23.09.05: Added flush.
      exit(11);
    }

  // Thomas: don't kill the following brackets; they are used to
  // stimulate the usage of mDeecoApp.~deecoApp which uses the
  // message method of mApp (so it should be used before mApp is
  // deleted); if the brackets are missing the destructor of
  // mDeecoApp is called after "return"; this is also true if
  // mDeeco.~deecoApp is used explicitly before "delete mApp"
  // because it is called implicitly again after "delete mApp"
  {
    deecoApp mDeecoApp(mApp); // Robbie: 24.11.05: was by assignment
    mDeecoApp.run();

    // Robbie: 09.08.05: 'deeco' exit status.
    //
    // Set deeco_exit_status based errorFlag.
    //
    if (mApp->errorFlag)
      {
        deeco_exit_status = 1;
      }
  }

  if (mApp != 0) delete mApp;

  // Robbie: 09.08.05: 'deeco' exit status.
  //
  // 'deeco' exit status no longer hard coded as 0.
  //
  return deeco_exit_status;

} // main()


///////// helper functions /////////////////////////////////////////////////////////////////

// The use of mApp->message() in these helper functions is
// predicated on the fact that these handlers will be called
// AFTER mApp is constructed.

#ifdef MYHAND

//// myInterruptHandler
// do after an interrupt signal is generated
//
void myInterruptHandler(int)
{
  mApp->message(1017, "");
}


//// myFloatHandler
// do after a floating point exception signal is generated
//
void myFloatHandler(int)
{
  mApp->message(51, "");
}


//// matherr
// my matherr (see man matherr) function; for detecting errors of
// mathematical functions -> <math.h>
//
int matherr(struct exception* x)
{
  std::ostringstream ssError;                // <sstream> not <sstrstream.h>

  switch(x->type)
    {
    case DOMAIN:    ssError << "DOMAIN"; break;
    case SING:      ssError << "SINGULARITY"; break;
    case OVERFLOW:  ssError << "OVERFLOW"; break;
    case UNDERFLOW: ssError << "UNDERFLOW"; break;
    case TLOSS:     ssError << "TOTAL LOSS OF SIGNIFICANCE"; break;
    case PLOSS:     ssError << "PARTIAL LOSS OF SIGNIFICANCE"; break;
    }

  ssError << " "
          << x->name
          << "  arg:"
          << x->arg1
          << "\n"
          << std::ends;

  std::string sTemp = ssError.str();    // create intermediary
  String sError     = sTemp.c_str();    // client code need not delocate memory

  if (x->type != UNDERFLOW)
    {
      mApp->message(50, sError);
      mApp->errorFlag++;
    }
  else                             // in general, UNDERFLOW is not a serious
    {                              // error, because the value is set to zero
      mApp->message(504, sError);
    }
  return 1;                        // don't print the default error message;
}                                  // don't set "errno"



void myNewHandler(void)
{
  mApp->message(52, "");
  exit(11);
}
#endif

// end of file

