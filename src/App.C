
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
//  $Date: 2005/11/25 12:22:43 $
//  $Author: morrison $
//  $RCSfile: App.C,v $

//////////////////////////////////////////////////////////////////
//
// GENERAL APPLICATION OBJECTS
//
//////////////////////////////////////////////////////////////////

#include   <iostream.h>       // input-output management
#include       <time.h>       // time and date
// #include  <assert.h>       // for exception handling
#include      <ctype.h>       // control of characters
#include     <stdlib.h>       // exit()
#include    <fstream.h>       // file I/O
#include  <Stopwatch.h>       // timing functions
#include     <unistd.h>       // sleep()
#include <sys/systeminfo.h>   // SI_ macros
#include     <string>         // std::string
#include    <sstream>         // string streams of class std::ostringstream

#include   "TestFlag.h"       // for _TEST_, but can also be included via CC call
#include        "App.h"

// Robbie: 10.02.03: forward declarations for new non-member helper functions
//
extern String binary_name;
static void beepa();                              // new beeper routine
static String GetCompileTimeInfo(String dvs);     // assemble compile-time info
static String GetRunTimeInfo();                   // assemble run-time info
static String GetAdditionalInfo();                // assemble additional info
static String GetLegalInfo();                     // assemble legal info

/////////////////////////////////////////////////////////////////
//
//  CLASS: App
//
////////////////////////////////////////////////////////////////

// Summary: program unspecific parts of an application program
// including simple message control, error handling and basic
// functions for data input

//// App
// Standard Constructor
//
App::App(void)
{
  errorFlag       = 0;
  testFlag        = 0;
  projectName     = "";
  projectPath     = ".";                          // <Path.h>, the current directory
  appName         = "";
  logFileName     = ".log";
  messageFileName = ".msg";
  logFlag         = 1;
  silentFlag      = 0;

  if (testFlag)
    message(1001, "App (Standard)");
};

//// App
//   Constructor
//
App::App(int lF, int eF, int sF, int tF, String aN, String lFN, String mFN,
         String pP, String pN)          // "App.h" - > <String.h>
{

  clock.start();              // "App.h" - > <Stopwatch.h> start stop-watch
  logFlag    = lF;
  errorFlag  = eF;
  silentFlag = sF;
  testFlag   = tF;
  appName    = aN;

  // Robbie: 10.02.03: not needed here
  //
  // appName.shrink(11);      // shrink to 11 characters for formatting reasons
                              // "App.h" - > <String.h>

  projectName     = pN;
  projectPath     = pP;       // <Path.h>, conversion from String to Path
  logFileName     = pP/lFN;   // <Path.h>, concatenation
  messageFileName = mFN;

  if (testFlag)
    message(1001, "App");     // help function, see below

  cout << "\n" << flush;  // Robbie: 23.09.05: Added flush.

  // Robbie: 10.02.03: splash screen now accommodates variable length appNames
  //
  const int LOA = 41;         // line length overall (LOA) in chars
  appName.shrink(LOA - 6);    // truncate as required for formatting reasons
  int len = appName.length(); // length of application string
  int seg = (LOA-1-len)/2;    // integer arithmetic
  String line;
  line.pad(LOA, '-');
  String segment = line;
  line.shrink(seg);
  String buffer = line + " " + appName + " " + line;
  buffer.shrink(LOA);
  buffer += "\n";

  // Robbie: 11.02.03: modified syntax to make more robust
  // Robbie: 24.02.03: changed text
  //
  String sHeader;
  sHeader  = " " + buffer;
  sHeader += " |                                       |\n";
  sHeader += " |      dynamic energy, emissions,       |\n";
  sHeader += " |        and cost optimization          |\n";
  sHeader += " |                                       |\n";
  sHeader += " |   Institute for Energy Engineering    |\n";
  sHeader += " |    Technical University of Berlin     |\n";
  sHeader += " -----------------------------------------";
  sHeader += "\n";               // "App.h" - > <String.h>

  writeMessage(sHeader);               // help function, see below

  // Robbie: 10.02.03: display compile-time information as required
  //
  String ctInfo = GetCompileTimeInfo(appName);
  writeMessage(ctInfo);

  // Robbie: 10.02.03: display run-time information as required
  //
  String rtInfo = GetRunTimeInfo();
  writeMessage(rtInfo);

  // Robbie: 24.02.03: display legal information as required
  //
  String lgInfo = GetLegalInfo();
  writeMessage(lgInfo);

  // Robbie: 10.02.03: display additional information as required
  //
  String adInfo = GetAdditionalInfo();
  if (adInfo.is_empty() == 0)           // returns zero if string is not empty
    {
      writeMessage(adInfo);
    }

  // Robbie: 10.02.03: the following is no longer required
  //
  // time_t nseconds;                   // <time.h>, manual ctime, time;
  // nseconds = time(0);                // <time.h>, get time in seconds
  // char* pTime  = ctime(&nseconds);   // <time.h>, conversion to time of day and date
  // writeMessage(pTime);               // write time and date
}

//// ~App
//   Destructor
//
App::~App(void)
{
  if (testFlag)
    {
      message(1002, "App");
    }

  // Robbie: 25.11.05: std::ostringstream was ostrstream

  std::string sTemp;                         // intermediary variable
  clock.stop();                              // "App.h" - > <Stopwatch.h>

  std::ostringstream ssUserClock;            // <sstream> not <sstrstream.h>
  ssUserClock << clock.user()                // show user (cpu) time
              <<" s"
              << std::ends;
  sTemp             = ssUserClock.str();     // create intermediary
  String sUserClock = sTemp.c_str();         // client code need not delocate memory
  message(1003, sUserClock);

  std::ostringstream ssRealClock;            // <sstream> not <sstrstream.h>
  ssRealClock << clock.real()                // show real (elapsed) time
              <<" s"
              << std::ends;
  sTemp             = ssRealClock.str();     // create intermediary
  String sRealClock = sTemp.c_str();         // client code need not delocate memory
  message(1004, sRealClock);

  // modified code ends

  if (errorFlag)
    {
      message(10, "");
    }
  message(1005,appName);

  // Robbie: revised beeping routine
  //
  if(!silentFlag)
    {
      write(1, "\n", 1);   // Robbie: 21.07.05: added third argument
      beepa();             // one beep if no error; three if error
      if (errorFlag)
        {
          sleep(1);        // wait one second  <unistd.h>
          beepa();
          sleep(1);
          beepa();
        }
    }

  // Robbie: 24.11.05: HP-UX beeper code removed, see release 006 otherwise.
}

//// message
// simple message handling
//
void App::message(int i, String Msg)
{
  if (i >= 1000)
    {
      String sInfo = "INFO   "
                   + int_to_str(i)
                   + " : "
                   + readMessageFileLine(i)  // help function, see below
                   + " "
                   + Msg;
      writeMessage(sInfo);                   // help function, see below
    }
  else if  (i >= 500)
    {
      String sWarning = "WARNING "
                      + int_to_str(i)
                      + " : "
                      + readMessageFileLine(i)
                      + " "
                      + Msg;
      writeMessage(sWarning);
    }
  else
    {
      String sError = "ERROR    "
                    + int_to_str(i)
                    + " : "
                    + readMessageFileLine(i)
                    + " "
                    + Msg;
      writeMessage(sError);
      errorFlag++;
    }
}

//// ignoreRestLine
// data-Input
// read a line of a file until line end or file end is reached
// ignore(....) (see <iostream.h>) only read line end
//
void App::ignoreRestLine(ifstream& fin)           // <fstream.h>
{
  if (!fin.eof())
    {
      char c;
      c = fin.get();                              // <fstream.h>, see manual istream
                                                  // get the next character
      while (! (c == EOF || c=='\n'))
        {
          c = fin.get();
        }
    }
}

//// emptyLine
// data-input
// the usage of >> makes problems if there are empty lines in a file
// for example, at the end of that file.  emptyLine returns 0 if there
// are other than white-space characters in the line.  If a line is not
// empty the usage of >> should give correct results.
//
int App::emptyLine(ifstream& fin)                 // <fstream.h>
{
  char c;
  c = fin.peek();                                 // look at the next character
                                                  // but don't get it
  while (isspace(c) && c != '\n' && c != EOF)     // <ctype.h>, isspace ==1 if c
    {                                             // is a whitespace character like
                                                  // space, tab, carriage return,
                                                  // newline; EOF is not a whitespace
                                                  // character
      c = fin.get();
      c = fin.peek();
    }
  if (!isspace(c) && c != EOF && c != '%')        // comment lines start with %
    return 0;
  else
    return 1;
}

//// readComma
// read only one comma
//
int App::readComma(ifstream& fin)
{
  char c;
  c = fin.peek();
  while (isspace(c) && c != '\n' && c != EOF)
    {
      c = fin.get();
      c = fin.peek();
    }
  if (c == ',')
    {
      c = fin.get();
      return 0;
    }
  else
    return 1;
}

/////////////////// help routines (protected) /////////////////////

//// writeMessage
//
void App::writeMessage(String wMsg)
{
  if (logFlag)
    {
      foutLog.open(logFileName, ios::app);          // <fstream.h>
      if (foutLog.fail())                           // <fstream.h>
        {
          if (!silentFlag)
            cerr << "Unable to write in log file\n"
                 << flush;    // Robbie: 23.09.05: Added flush.
                              // message should not be buffered
        }
      else
        {
          foutLog << wMsg
                  << "\n"
                  << flush;   // Robbie: 23.09.05: Added flush.
          foutLog.close();
        }
    }
  if (!silentFlag)
    cerr << wMsg
         << "\n"
         << flush;            // Robbie: 23.09.05: Added flush.
                              // message should not be buffered
}

//// readMessageFileLine
// read a message specified by the number i from message file
//
String App::readMessageFileLine(int i)
{
  // Robbie: 21.11.05: pMsg now on stack, not heap, also SIZE added

  //  char* pMsg = new char[254];      // char* instead of String necessary
  //                                   // because of use of getline (see below)
  //  assert(pMsg != 0);               // <assert.h>, enough memory ?

  const int SIZE = 256;             // maximum string length
  char pMsg[SIZE];                  // Robbie: 21.11.05: pMsg now on stack, not heap

  String sMsg;                      // message string
  int j = 0;
  finMessage.open(messageFileName, ios::nocreate);  // <fstream.h>
  if (!finMessage.fail())                           // <fstream.h>
    {
      do
        {
          if (!emptyLine(finMessage))
            {
              finMessage >> j;
            }
          if (j == i)
            {
              finMessage.getline(pMsg, SIZE);     // read message, <fstream.h>
              sMsg = pMsg;                        // conversion from char* -> String
            }
          else
            {
              ignoreRestLine(finMessage);         // ignore the rest of line
            }
        }
      while (!(finMessage.eof() || (j == i) || finMessage.fail()));
      if (finMessage.fail())
        {
          sMsg = " (Error reading error/warning/info file)";
        }
      if (j != i && !finMessage.fail())
        {
          sMsg = "(No detailed information available)";
        }
      finMessage.close();
      // delete [] pMsg;                          // Robbie: 21.11.05: not required now
      return sMsg;
    }
  else
    {
      // delete [] pMsg;                          // Robbie: 21.11.05: not required now
      sMsg = "(No error/warning/info file)";
      return sMsg;
    }
}

///////////// additional help routines (non-member) ////////////////

// Robbie: 1998: revised beeper function using 'bel' char (ASCII 7)
//
// write() is <iostream.h>
//
static void
beepa()
{
  // Robbie: 21.07.05: added third argument to write statements
  for(int i = 0; i < 4; i++ )
    {
//    write(1, "\007", 1);       // noisy beepa
      write(1, "BBEEP! ", 7);    // noiseless beepa
    }
    write(1, "\n", 1);
}

// Robbie: 10.02.03: Create compile-time information.
//
// This function returns a string containing compile-time information.
// Developers may _add_ to this subroutine as they see fit.
//
// All the macros are built-in except for NAMESPACED_SC. NAMESPACED_SC
// is defined in the deeco makefile if the UDK compiler is being used.
// Otherwise the SDK compiler is assumed.  No special system headers
// are required.
//
// Typical output is shown below:
//
//   Build date                 : Feb 17 2003
//   Build time                 : 16:30:14
//   Development environment    : UDK
//   SCO  version               : 3.01
//   EDG C++ front end version  : 2.39
//   Language specification     : cfront 3.0
//   Test flag status           : inactive
//   Version string (arbitrary) : S11.test
//
// Please note that this information is generated exclusively at
// compile-time and does _not_ rely on developer-specified input.
//
static String
GetCompileTimeInfo(String dvs)                    // dvs is deeco version string
{
    String output;                                // return buffer
    output += "COMPILE-TIME INFORMATION\n\n";

    // process deeco_ver_str

    String version_string;
    int pos = dvs.index("\040");                  // space character ASCII 32 in octal
    version_string = dvs.chunk(pos + 1);

    // pre-prepare required information
    //
    // NAMESPACED_SC defined in deeco makefile, required for UDK
    // __SCO_VERSION__ defined in SCO UnixWare 7 UDK but not in UnixWare 2 SDK

    String compiler_type = "";
#   if defined (NAMESPACED_SC) && defined (__SCO_VERSION__)
        compiler_type = "UDK";
#   else
        compiler_type = "SDK (by inference);";    // by inference
#   endif

    String sco_ver = "(not defined)";
#   if defined (__SCO_VERSION__)
        char temp1[9];
        sprintf(temp1, "%u", __SCO_VERSION__);    // type long int, %u is format as unsigned decimal
        sco_ver  = temp1[0];                      // main version, single digit
        sco_ver += ".";
        sco_ver += temp1[1];                      // minor version, two digits
        sco_ver += temp1[2];
#   endif

    String edg_ver = "(not defined)";
#   if defined (__EDG_VERSION__)                  // defined in SCO UnixWare 7 only
        char temp2[3];
        sprintf(temp2, "%u", __EDG_VERSION__);    // type long int, %u is format as unsigned decimal
        edg_ver  = temp2[0];                      // main version, single digit
        edg_ver += ".";
        edg_ver += temp2[1];                      // minor version, two digits
        edg_ver += temp2[2];
#   endif

// Robbie: 02.08.05: added information about _CFRONT_3_0

    String language_spec = "ISO C++ (1998)";
#   if defined (_CFRONT_3_0)                      // defined only for cfront
        language_spec = "cfront 3.0";
#   endif

    // build buffer

    output += " Build date                 : ";   // MMM DD YYYY
    output += __DATE__;                           // Standard C predefined macro
    output += "\n";

    output += " Build time                 : ";   // hh:mm:ss
    output += __TIME__;                           // Standard C predefined macro
    output += "\n";

    output += " Development environment    : ";   // based on NAMESPACED_SC makefile directive
    output += compiler_type;                      // Standard C predefined macro
    output += "\n";

    output += " SCO version                : ";   // V.vv, where V is major version, v is minor
    output += sco_ver;                            // determined earlier
    output += "\n";

    output += " EDG C++ front end version  : ";   // V.vv, where V is major version, v is minor
    output += edg_ver;                            // determined earlier
    output += "\n";

// Robbie: 02.08.05: added information about _CFRONT_3_0

    output += " Language specification     : ";
    output += language_spec;
    output += "\n";

    String test_status = "inactive";
#   if defined (_TEST_)
        test_status = "active";
#   endif

    output += " Test flag status           : ";
    output += test_status;
    output += "\n";

    output += " Version string (arbitrary) : ";
    output += version_string;
    output += "\n";

    return output;
}

// Robbie: 10.02.03: Create run-time information.
//
// This function returns a string containing run-time information.
// Developers may _add_ to this subroutine as they see fit.
//
// Typical output is shown below:
//
//   Program start date         : Feb 10 2003
//   Program start time         : 18:51:55
//   Hostname                   : helgoland.fb10.tu-berlin.de
//   Login name                 : morrison
//   Operating system version   : SCO UnixWare 7.1.1
//   Operating system type      : UNIX System V Release 5 (SVR5)
//   Processor                  : Pentium III
//
// Please note that this information is generated exclusively at
// run-time and does _not_ rely on developer-specified input.
//
static String
GetRunTimeInfo()
{
    String output;                                // output buffer
    output += "RUN-TIME INFORMATION\n\n";

// Robbie: 02.08.05: localtime now gmtime because Linux execution domain is probably buggy

    const time_t now = time(0);                   // get time_t time, seconds since 1970
//    const tm* current = localtime(&now);        // convert to tm struct in local time, return pointer
    const tm* current = gmtime(&now);             // convert to tm struct in UTC time, return pointer
    const size_t MAX = 1024;                      // define buffer length for non-dynamic char arrays
    char buffer[MAX] = "\0";

    output += " Program start date         : ";
    strftime(buffer, MAX, "%b %d %Y", current);   // format date as string
    output += buffer;
    output += "\n";

    output += " Program start time         : ";
    strftime(buffer, MAX, "%H:%M:%S", current);   // format (normal) time as string
    output += buffer;
    output += " (UTC)";
    output += "\n";

    output += " Command-line name          : ";
    if (binary_name(0,2) == "./") {               // set in main()
      binary_name = binary_name.chunk(2);         // remove leading "./" as required
    }
    output += binary_name;
    output += "\n";

    output += " Working directory          : ";
    getcwd(buffer, MAX);                          // <unistd.h>
    output += buffer;
    output += "/";                                // add trailing slash
    output += "\n";

    output += " Hostname                   : ";
    sysinfo(SI_HOSTNAME, buffer, MAX);
    output += buffer;
    output += "\n";

    output += " Login name                 : ";
    output +=  getlogin();                        // <stdlib.h>
    output += "\n";

    output += " Operating system version   : ";
    sysinfo(SI_OS_PROVIDER, buffer, MAX);         // <sys/systeminfo.h> (several places)
    output += buffer;
    output += " ";
    sysinfo(SI_SYSNAME, buffer, MAX);
    output += buffer;
    output += " ";
    sysinfo(SI_VERSION, buffer, MAX);
    output += buffer;
    output += " ";
    output += "\n";

    output += " Operating system type      : ";
    sysinfo(SI_OS_BASE, buffer, MAX);
    if (strcmp(buffer, "UNIX_SVR5") == 0) {       // make the output a little more readable
        strcpy(buffer, "UNIX System V Release 5 (SVR5)");
    }
    output += buffer;
    output += "\n";

    output += " Processor                  : ";
    sysinfo(SI_MACHINE, buffer, MAX);
    output += buffer;
    output += "\n";

    return output;
}

/////////////////// users should modify these messages ////////////

// Robbie: 10.02.03: Create additional information.
//
// This function returns a string containing additional information.
// Developers may _modify_ the contents of this subroutine as they see fit.
//
static String
GetAdditionalInfo()
{
    String buffer;                                // message buffer

    buffer += " This binary built using the UDK compiler update\n";
    buffer += " that now supports the C++ Standard Library.\n";

    String output;                                // output buffer
    output += "ADDITIONAL INFORMATION\n\n";

    output += buffer;
    output += "\n";

    return output;
}

// Robbie: 24.02.03: Create legal information.
//
// This function returns a string containing legal information.
// Developers may _modify_ the contents of this subroutine with due caution.
//
static String
GetLegalInfo()
{
    String buffer;                                // message buffer

    buffer += " Copyright (C) 1995-2005.  Thomas Bruckner, Robbie\n";
    buffer += " Morrison, Dietmar Lindenberger, Johannes Bruhn,\n";
    buffer += " Kathrin Ramsel, Jan Heise, Susanna Medel.\n";
    buffer += "\n";
    buffer += " 'deeco' comes with ABSOLUTELY NO WARRANTY.  See\n";
    buffer += " source code notices for further legal information.\n";

    String output;                                // output buffer
    output += "LEGAL INFORMATION\n\n";
    output += buffer;

    return output;
}

//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/App.C,v $
//  end of file
