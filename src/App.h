
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
//  $Date: 2005/11/25 17:09:51 $
//  $Author: morrison $
//  $RCSfile: App.h,v $


//////////////////////////////////////////////////////////////////
//
// GENERAL APPLICATION OBJECTS
//
//////////////////////////////////////////////////////////////////


#ifndef _APP_H_               // header guard
#define _APP_H_


#include     <String.h>      
#include  <Stopwatch.h>       // timing
#include       <Path.h>       // path management
#include    <fstream.h>       // file I/O


// Robbie: 04.02.03: for port to SCO UnixWare 7.1.1 UDK
//
#if NAMESPACED_SC
    using namespace SCO_SC;
#endif


/////////////////////////////////////////////////////////////////
//
// CLASS: App
//
/////////////////////////////////////////////////////////////////

// Summary: program unspecific parts of an application program
// including simple message control, error handling and basic
// functions for data input

class App
{
public:

 // Standard Constructor
    App(void);
 // Constructor
    App(int lF, int eF, int sF, int tF, String aN, String lFN, String mFN,
        String pP, String pN);
 // Destructor
    virtual ~App(void);
 // Simple Message Handling
    void message( int i, String Msg);
 // Data-Input and Output
    void ignoreRestLine(ifstream& fin);   // <fstream.h>
 // Data-Input and Output
    int emptyLine(ifstream& fin);
 // Read only one comma
    int readComma(ifstream& fin);

    int errorFlag;
    int testFlag;
    String projectName;       // used as the first part of all standard
                              // (that is, scenario unspecific) control
                              // and result files

    Path projectPath;         // <Path.h>

protected:

    String appName;
    Path logFileName;
    Path messageFileName;
    int logFlag;
    int silentFlag;
    ifstream finMessage;      // <fstream.h>
    ofstream foutLog;         // <fstream.h>
    Stopwatch clock;          // <Stopwatch.h> making object of type Stopwatch
                              // that is, a clock for time control

 // help function for message handling

 // write message to standard output an log-file
    void writeMessage(String wMsg);
 // read the line with number i from message file
    String readMessageFileLine(int i);

};


#endif  // _APP_H_


//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/App.h,v $
//  end of file



