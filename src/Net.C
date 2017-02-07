
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

//  $Revision: 1.3 $
//  $Date: 2005/11/25 17:55:40 $
//  $Author: morrison $
//  $RCSfile: Net.C,v $

//////////////////////////////////////////////////////////////////
//
// DEECO ENERGY SUPPLY NETWORK MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////

#include        <Map.h>       // associative array
#include     <stdlib.h>       // exit()
#include  <strstream.h>       // string streams
#include      <float.h>       // for use of "infinity" = DBL_MAX

#include   "ProcType.h"       // management of the different process modules
#include        "Net.h"

// Important: for a comprehensive tutorial covering the graph containers
// and graph algorithms used here, see: Weitzen, TC. 1992. "The C++ graph
// classes : a tutorial.  In: AT&T and UNIX System Laboratories (eds),
// "USL C++ Standards Components, Release 3.0 : Programmer's Guide".
// pp 7.1-7.30.  AT&T and UNIX System Laboratories.  See also the relevant
// man pages and related systems documentation.

///////////////////////////////////////////////////////////////////
//
// CLASS: Net
//
///////////////////////////////////////////////////////////////////

//// Net
// Standard Constructor
//
Net::Net(void)
{
  nApp                          = NULL;
  nSimplex                      = NULL;
  energyFlowCount               = 0;
  constraintCount               = 0;
  lessConstraintCount           = 0;
  greaterConstraintCount        = 0;
  equalConstraintCount          = 0;
  nonProcConstraintCount        = 0;
  nonProcLessConstraintCount    = 0;
  nonProcGreaterConstraintCount = 0;
  nonProcEqualConstraintCount   = 0;

  actualProcType                = NULL;
  errorFlag                     = 0;
}

//// Net
// Constructor
//
Net::Net(App* cnApp)
{
  nApp=cnApp;
  if (nApp->testFlag)
    {
      nApp->message(1001, "Net");
    }
  errorFlag = 0;

  nSimplex                      = NULL;
  energyFlowCount               = 0;
  constraintCount               = 0;
  lessConstraintCount           = 0;
  greaterConstraintCount        = 0;
  equalConstraintCount          = 0;
  nonProcConstraintCount        = 0;
  nonProcLessConstraintCount    = 0;
  nonProcGreaterConstraintCount = 0;
  nonProcEqualConstraintCount   = 0;

  actualProcType = new ProcType(nApp);
  if (actualProcType == 0)
    {
      nApp->message(11, "new ProcType in Net.C");
      exit(11);
    }
}

//// ~Net
// Destructor
//
Net::~Net(void)
{
  if (nApp != 0)
    {
      if (nApp->testFlag)
        {
          nApp->message(1002, "Net");
        }
    }
  if (actualProcType)
    {
      delete actualProcType;
    }
}

//// prepareNet
// prepare the energy supply network structure
//
void
Net::prepareNet(ProcDef*     nProcDef,
                ioValue*     nProcVal,
                BalanDef*    nBalanDef,
                ConnectDef*  nConnectDef,
                ioDefValue*  nAggDef,
                ioValue*     nAggVal)
{
  establishNet(nProcDef, nProcVal, nBalanDef, nConnectDef, nAggDef, nAggVal);
  if (!errorFlag)
    {
      testNet();
    }
  if (!errorFlag)
    {
      orderNet();
    }
}

//// finishNet
// finish  the energy supply network structure object
// for example, delete pointers declared in prepareNet
//
void
Net::finishNet(void)
{
  // delete edges pointers

  edgePtr e;
  for (Mapiter<Symbol,edgePtr> edgeMapIt = edgeMap.first();
       edgeMapIt;
       edgeMapIt.next())
    {
      e = edgeMapIt.curr()->value;
      systGraph.remove(e);
      edgeMapIt.remove();
      if (e)
        {
          delete e;
        }
    }

  // delete process pointers

  procPtr p;
  for (Mapiter<Symbol,procPtr> procMapIt = procMap.first();
       procMapIt;
       procMapIt.next())
    {
      p = procMapIt.curr()->value;
      systGraph.remove(p);
      procMapIt.remove();
      if (p)
        {
          delete p;
        }
    }

  // delete balance pointers

  balanPtr b;
  for (Mapiter<Symbol,balanPtr> balanMapIt = balanMap.first();
       balanMapIt;
       balanMapIt.next())
    {
      b = balanMapIt.curr()->value;
      systGraph.remove(b);
      balanMapIt.remove();
      if (b)
        {
          delete b;
        }
    }
}

//// transferInTsPack
// transfer inTsPacks from nProcTsPack->inTsPackMap
// (used for reading the ... .pit file, for all processes)
// to procInTsPack (for a single process)
//
void
Net::transferInTsPack(ioTsPack* nProcTsPack)
{
  for (Mapiter<Symbol,MapSym1d> inTsPackIt1 = nProcTsPack->inTsPackMap.first();
       inTsPackIt1;
       inTsPackIt1.next())
    {
      for (Mapiter<Symbol,double> inTsPackIt2 = inTsPackIt1.curr()->value.first();
           inTsPackIt2;
           inTsPackIt2.next())
        {
          if (procMap.element(inTsPackIt1.curr()->key))
            {
              procMap[inTsPackIt1.curr()->key]->procInTsPack
                [inTsPackIt2.curr()->key] = inTsPackIt2.curr()->value;
            }
        }
    }
}

//// transferOutTsPack
// transfer outTsPacks from procOutTsPack (of each process) to
// actualProcTsPack->outTsPackMap (for writing the file ... .pot)
//
void
Net::transferOutTsPack(ioTsPack* nProcTsPack)
{
  for (Mapiter<Symbol,procPtr> procMapIt = procMap.first();
       procMapIt;
       procMapIt.next())
    {
      for (Mapiter<Symbol,double> outTsPackIt
             = procMapIt.curr()->value->procOutTsPack.first();
           outTsPackIt;
           outTsPackIt.next())
        {
          nProcTsPack->outTsPackMap[procMapIt.curr()->key]
            [outTsPackIt.curr()->key] = outTsPackIt.curr()->value;
          outTsPackIt.curr()->value = 0;   // ! 1.8.95 clear values
        }
    }
}

//// prepareOpt
// calculate the attributes and establish the matrix "aMatrix"
// (input for simplex)
//
void
Net::prepareOpt(ScenVal*     nScenVal,
                ScenTsPack*  nScenTsPack,
                ioDefValue*  nAggDef,
                ioValue*     nAggVal,
                double       actualIntLength)
{
  int actualM1 = 0;
  int actualM2 = 0;
  int actualM3 = 0;

  if (!error())
    {
      calcAttributes(nScenTsPack->vecU, actualIntLength);
    }

  //  cerr << flush;         // Robbie: 23.09.05: Added to prevent problem below!
                             //
                             // Actually, after adding '<< flush' to all ofstream,
                             // cout, and cerr statements, this doesn't seem to
                             // be a problem.
  if (!error())              //
    {                        //
      prepareSimplTab();     // Thomas: don't use if (!error()) because of
    }                        // dynamic memory allocation
                             //
                             // Robbie: if added, sometimes cores, sometimes gives:
                             // 'ERROR 12 : Can't read file : testfile/Mode.sd'

  if (!error())
    {
      calcBalanceConstCoef(actualM3);
    }
  if (!error())
    {
      calcAggConstCoef(actualM1, nAggDef, nAggVal);
    }
  if (!error())
    {
      calcProcConstCoef(nScenVal, actualM1, actualM2, actualM3);
    }
}

//// finishOpt
// get the results and calculate the state variable transformation;
// return value is the length of the time interval
//
double
Net::finishOpt(ScenVal*     nScenVal,
               ScenTsPack*  nScenTsPack,
               ioDefValue*  nAggDef,
               ioValue*     nAggVal,
               double&      actualIntLength)
{
  if (nSimplex->error())
    {
      errorFlag++;
    }
  if (!error())
    {
      getProcessResults1(actualIntLength, nScenTsPack->vecU);
    }
  if (!error())
    {
      getProcessResults2(nScenVal, nScenTsPack, actualIntLength);
    }
  if (!error())
    {
      getAggResults(nAggDef, nAggVal, actualIntLength);
    }
  if (!error())
    {
      clearConstraints();    // !2.8.95
    }
  if (nSimplex)
    {
      delete nSimplex;
    }
  return actualIntLength;
}

//// error
// error management
//
int
Net::error(void)
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

////////// help functions for prepareNet //////////////////////////////

//// establishNet
// establish the energy supply network
//
void
Net::establishNet(ProcDef*     nProcDef,
                  ioValue*     nProcVal,
                  BalanDef*    nBalanDef,
                  ConnectDef*  nConnectDef,
                  ioDefValue*  nAggDef,
                  ioValue*     nAggVal)
{
  insertProcesses(nProcDef, nProcVal, nAggDef, nAggVal);
  insertBalances(nBalanDef);
  insertConnections(nConnectDef);
}

//// insertProcesses
// insert processes in the net as vertices
//
void
Net::insertProcesses(ProcDef*     nProcDef,
                     ioValue*     nProcVal,
                     ioDefValue*  nAggDef,
                     ioValue*     nAggVal)
{
  for (Mapiter<Symbol,ProcDefRec> procDefMapIt = nProcDef->defMap.first();
       procDefMapIt;
       procDefMapIt.next())
    {
      Proc* p = NULL;
      if (procDefMapIt.curr()->value.select)
        {
          p = actualProcType->prepareProcess(nApp,procDefMapIt.curr()->key,
            procDefMapIt.curr()->value.procType,nProcVal);
          procMap[procDefMapIt.curr()->key] = p;
          if (!systGraph.insert(p))
            {
              nApp->message(24, procDefMapIt.curr()->key.the_string());
              errorFlag++;
            }

          // numbering of the energy flows for usage in simplex algorithm

          const Symbol2* pSym;                    // const is necessary
                                                  // (see manual for Sets!)
          Setiter<Symbol2> ExIt(p->Ex);           // <Set.h>
          while (pSym = ExIt.next())              // "=" is correct, not "=="!
            {
              p->exFlowAssign[++energyFlowCount] = *pSym;
            }                                     // take into account the side
                                                  // effect of ++ !
          Setiter<Symbol2> EnIt(p->En);
          while (pSym = EnIt.next())              // "=" is correct, not "=="!
            {
              p->enFlowAssign[++energyFlowCount] = *pSym;
            }                                     // take into account the side
                                                  // effect of ++ !
        }
    }

  // add number of aggregation constraints imposed by process aggregates
  // (one lessContraint for each aggregate) to nonProcLessConstraintCount

  for (Mapiter<Symbol,int> aggDefMapIt= nAggDef->defMap.first();
       aggDefMapIt;
       aggDefMapIt.next())
    {
      if (aggDefMapIt.curr()->value == 1)
        {
          nonProcLessConstraintCount++;
          nonProcConstraintCount++;
        }
    }

  // check whether all processes of an aggregate are allowed
  // (and set aggCount for those processes)

  for (Mapiter<Symbol,MapSym1d> aggValIt1 = nAggVal->inValMap.first();
       aggValIt1;
       aggValIt1.next())
    {
      Mapiter<Symbol,int> aggDefMapIt2 (nAggDef->defMap);
      if ((aggDefMapIt2 = nAggDef->defMap.element(aggValIt1.curr()->key))
          && aggDefMapIt2.curr()->value != 0)
        {
          for (Mapiter<Symbol,double> aggValIt2
                 = aggValIt1.curr()->value.first();
               aggValIt2;
               aggValIt2.next())
            {
              Mapiter <Symbol,procPtr> procMapIt (procMap);
              if (procMapIt = procMap.element(aggValIt2.curr()->key))  // "=" is okay
                {
                  procMapIt.curr()->value->aggCount
                    = aggValIt1.curr()->value.size();
                }
              else
                {
                  nApp->message(47, aggValIt2.curr()->key.the_string());
                }
            }
        }
    }
}

//// insertBalances
// insert balances in the net as vertices
//
void
Net::insertBalances(BalanDef* nBalanDef)
{
  for (Mapiter<Symbol,BalanDefRec> balanDefMapIt = nBalanDef->defMap.first();
       balanDefMapIt;
       balanDefMapIt.next())
    {
      Balan* b = NULL;
      if (balanDefMapIt.curr()->value.select)
        {
          b = new Balan(nApp,balanDefMapIt.curr()->key,
                        balanDefMapIt.curr()->value.energyFlowType,
                        balanDefMapIt.curr()->value.linkType);
          if (b->linkType != "0" &&
              b->linkType != "1" &&
              b->linkType != "2" &&
              b->linkType != "3" &&
              b->linkType != "4")
            {
              nApp->message(27, balanDefMapIt.curr()->
                value.linkType.the_string() +  " of "
                + balanDefMapIt.curr()->key.the_string());
              errorFlag++;
            }
          if (b == 0)
            {
              nApp->message(11, "new Balan in Net.C");
              exit(11);
            }
          balanMap[balanDefMapIt.curr()->key] = b;
          if (!systGraph.insert(b))
            {
              nApp->message(26, balanDefMapIt.curr()->key.the_string());
              errorFlag++;
            }

          // each balance represents an additional restriction

          nonProcEqualConstraintCount
            = ++nonProcEqualConstraintCount;
          nonProcConstraintCount
            = nonProcLessConstraintCount
            + nonProcGreaterConstraintCount
            + nonProcEqualConstraintCount;
        }
    }

  // the electrical energy balance is not modeled explicitly
  // but the corresponding restriction have to be accounted for:

  nonProcEqualConstraintCount
    = ++nonProcEqualConstraintCount;
  nonProcConstraintCount
    = nonProcLessConstraintCount
    + nonProcGreaterConstraintCount
    + nonProcEqualConstraintCount;
}

//// insertConnections
// insert energy flows in the net as edges and make first tests
// of compatibility
//
void
Net::insertConnections(ConnectDef* nConnectDef)
{
  Mapiter<Symbol,procPtr>  procMapIt (procMap);
  Mapiter<Symbol,balanPtr> balanMapIt (balanMap);

  for (Mapiter<Symbol,ConnectDefRec> conDefMapIt
         = nConnectDef->defMap.first();
       conDefMapIt;
       conDefMapIt.next())
    {
      if (conDefMapIt.curr()->value.direct)
        {
          procMapIt  =  procMap.element(conDefMapIt.curr()->value.procId);
          balanMapIt = balanMap.element(conDefMapIt.curr()->value.balanId);
          if (procMapIt)
            {
              if (balanMapIt)
                {
                  edgePtr e;
                  if (conDefMapIt.curr()->value.direct == 1)
                    {
                      e = new DEdge(procMapIt.curr()->value,
                                    balanMapIt.curr()->value,
                                    nApp,conDefMapIt.curr()->key,
                                    conDefMapIt.curr()->value.energyFlowType,
                                    conDefMapIt.curr()->value.energyFlowNumber,
                                    conDefMapIt.curr()->value.linkType);
                      if (e == 0)
                        {
                          nApp->message(11, "new DEdge in Net.C");
                          exit(11);
                        }
                      edgeMap[conDefMapIt.curr()->key] = e;
                      if (!systGraph.insert(e))
                        {
                          nApp->message(28, conDefMapIt.curr()->key.the_string());
                          errorFlag++;
                        }
                    }
                  if (conDefMapIt.curr()->value.direct == -1)
                    {
                      e = new DEdge(balanMapIt.curr()->value,
                                    procMapIt.curr()->value,
                                    nApp,conDefMapIt.curr()->key,
                                    conDefMapIt.curr()->value.energyFlowType,
                                    conDefMapIt.curr()->value.energyFlowNumber,
                                    conDefMapIt.curr()->value.linkType);
                      if (e == 0)
                        {
                          nApp->message(11, "new DEdge in Net.C");
                          exit(11);
                        }
                      edgeMap[conDefMapIt.curr()->key] = e;
                      if (!systGraph.insert(e))
                        {
                          nApp->message(28, conDefMapIt.curr()->key.the_string());
                          errorFlag++;
                        }
                    }

                  // energy flow compatible to balance type?

                  if (e->energyFlowType !=
                        balanMapIt.curr()->value->energyFlowType ||
                      e->linkType !=
                        balanMapIt.curr()->value->linkType)
                    {
                      nApp->message(39, conDefMapIt.curr()->key.the_string());
                      errorFlag++;
                    }
                }
              else
                {
                  nApp->message(29, conDefMapIt.curr()->key.the_string());
                  errorFlag++;
                }
            }
          else
            {
              nApp->message(30, conDefMapIt.curr()->key.the_string());
              errorFlag++;
            }
        }
    }
}

//// testNet
// test the graph, looking for cycles
//
// Important: for a comprehensive tutorial covering the graph containers
// and graph algorithms used here, see: Weitzen, TC. 1992. "The C++ graph
// classes : a tutorial.  In: AT&T and UNIX System Laboratories (eds),
// "USL C++ Standards Components, Release 3.0 : Programmer's Guide".
// pp 7.1-7.30.  AT&T and UNIX System Laboratories.  See also the relevant
// man pages and related systems documentation.
//
void
Net::testNet(void)
{
  // are there any cycles ?

  vertexPtr p = cycle(systGraph);                 // "Net.h" -> "DGraph.h" ->
                                                  // <Graph_alg.h>
  if (p != 0)
    {
      nApp->message(31, p->vertexId.the_string());
      ostrstream ssError;                         // <strstream.h>
      List_of_p<DEdge> cycleList;                 // <List.h>
      cycleList = cycle_list(systGraph, p);       // <Graph_alg.h>
      List_of_piter<DEdge> cycleListIt (cycleList);
      edgePtr cycleEdge;
      ssError << "{";

      // show edge list

      while (cycleListIt.next(cycleEdge))
        {
          ssError << *cycleEdge;
          if(!cycleListIt.at_end())
            {
              ssError << ",";
            }
          else
            {
              ssError << "}" << ends;
            }
        }
      char* pError = ssError.str();               // <strstream.h>, "freeze" to char*
      nApp->message(38, pError);
      delete pError;          // Robbie: 18.11.2005: delete was delete[],
                              // deallocation is correct, see man strstream
                              // and method str().
      errorFlag++;
    }

  if (nApp->testFlag)
    {
      ostrstream ssVInfo;                         // <strstream.h>
      ssVInfo << systGraph.vertices() << ends;    // show vertices
      char* pVInfo = ssVInfo.str();               // <strstream.h>, "freeze" to char*
      nApp->message(1014, pVInfo);
      delete pVInfo;          // Robbie: 18.11.2005: delete was delete[],
                              // deallocation is correct, see man strstream
                              // and method str().

      ostrstream ssEInfo;                         // <strstream.h>
      ssEInfo << systGraph.edges() << ends;       // show edges
      char* pEInfo = ssEInfo.str();               // <strstream.h>, "freeze" to char*
      nApp->message(1015, pEInfo);
      delete pEInfo;          // Robbie: 18.11.2005: delete was delete[],
                              // deallocation is correct, see man strstream
                              // and method str().
    }

  // is each energy input (output) of a process connected with exact one
  // entering (leaving) energy flow?
  //
  // is the linkType of a connection similar to that of the process entry or exit?

  for (Mapiter<Symbol,procPtr> procMapIt = procMap.first();
       procMapIt;
       procMapIt.next())
    {
      // see: AT&T and UNIX System Laboratories (eds). 1992. "USL C++
      // Standard Components, Release 3.0, Programmer's Guide". p14.3.

      Setiter<Symbol2> ExIt(procMapIt.curr()->value->Ex);       // <Set.h>
      Setiter<Symbol2> EnIt(procMapIt.curr()->value->En);
      const Symbol2* pSym;       // const is necessary, see manual for Sets
      edgePtr e;
      while (pSym = ExIt.next())        // "=" is correct
        {
          int i = 0;

          // Robbie: 02.09.2005: Split original statement into two for ISO C++.
          //
          Set_of_p <DEdge> robbie01 = procMapIt.curr()->value->out_edges();
          Set_of_piter <DEdge> outEdgeIt( robbie01 );

          while(e = outEdgeIt.next())  // "=" is correct
            {
              if (pSym->comp1 == e->energyFlowType &&
                  pSym->comp2 == int_to_str(e->energyFlowNumber))
                {
                  e->set_val(e->val() + 1);
                  i++;
                }
            }
          if (i == 0 && (pSym->comp1 != "El"))   // no explicit connections
                                                 // for electrical energy flows
            {
              nApp->message(32, procMapIt.curr()->key.the_string()
                + " , " + pSym->comp1.the_string() + " , "
                + pSym->comp2.the_string());
              errorFlag++;
            }
          if (i == 1 && (pSym->comp1 == "El"))
            {
              nApp->message(36, procMapIt.curr()->key.the_string()
                + " , " + pSym->comp1.the_string() + " , "
                + pSym->comp2.the_string());
              errorFlag++;
            }
          if (i > 1)
            {
              nApp->message(33, procMapIt.curr()->key.the_string()
                + " , " + pSym->comp1.the_string() + " , "
                + pSym->comp2.the_string());
              errorFlag++;
            }
        }
      while (pSym = EnIt.next())       // "=" is correct
        {
          int i = 0;

          // Robbie: 02.09.2005: Split original statement into two for ISO C++.
          //
          Set_of_p <DEdge> robbie02 = procMapIt.curr()->value->in_edges();
          Set_of_piter <DEdge> inEdgeIt( robbie02 );

          while(e = inEdgeIt.next())
            {
              if (pSym->comp1 == e->energyFlowType &&
                  pSym->comp2 == int_to_str(e->energyFlowNumber))
                {
                  e->set_val(e->val() + 1);
                  i++;
                }
            }
          if (i == 0 && (pSym->comp1 != "El"))   // no explicit connections
                                                 // for electrical energy flows
            {
              nApp->message(32, procMapIt.curr()->key.the_string()
                + " , " + pSym->comp1.the_string() + " , "
                + pSym->comp2.the_string());
              errorFlag++;
            }
          if (i == 1 && (pSym->comp1 == "El"))
            {
              nApp->message(36, procMapIt.curr()->key.the_string()
                + " , " + pSym->comp1.the_string() + " , "
                + pSym->comp2.the_string());
              errorFlag++;
            }
          if (i > 1)
            {
              nApp->message(33, procMapIt.curr()->key.the_string()
                + " , " + pSym->comp1.the_string() + " , "
                + pSym->comp2.the_string());
              errorFlag++;
            }
        }
    }

  // are there any connections which don't belong to a specified
  // energy process entrance or exit?

  // Robbie: 02.09.2005: Split original statement into two for ISO C++.
  //
  Set_of_p <DEdge> robbie03 = systGraph.edges();
  Set_of_piter <DEdge> EdgeIt( robbie03 );

  edgePtr e;
  while(e = EdgeIt.next())             // "=" is correct
    {
      if (e->val() > 1)
        {
          nApp->message(34, e->edgeId.the_string());
          errorFlag++;
        }
      if (e->val() == 0)
        {
          nApp->message(35, e->edgeId.the_string());
          errorFlag++;
        }
    }

  // clear the marked edges

  Set_of_p<DEdge> gEdges;
  gEdges = systGraph.edges();
  reset_val(gEdges);

  // are the energy flows connected to the balances in a right manner
  // according to the linkType characterization of the balances?

  for (Mapiter<Symbol,balanPtr> balanMapIt = balanMap.first();
       balanMapIt;
       balanMapIt.next())
    {
      Balan* testBalan;
      testBalan = balanMapIt.curr()->value;
      Set_of_p<DEdge> inEdge;
      inEdge = testBalan->in_edges();
      Set_of_p<DEdge> outEdge;
      outEdge = testBalan->out_edges();

      // linkType have to be an element of {0,1,2,3,4}

      if (testBalan->linkType != "0" &&
          testBalan->linkType != "1" &&
          testBalan->linkType != "2" &&
          testBalan->linkType != "3" &&
          testBalan->linkType != "4")
        {
          nApp->message(44, testBalan->linkType.the_string() + " of "
                        + testBalan->vertexId.the_string());
          errorFlag++;
        }

      if (testBalan->linkType == "1")    // only one entering energy flow
                                         // is allowed
        {
          if (inEdge.size() > 1)
            {
              nApp->message(40, testBalan->vertexId.the_string());
              errorFlag++;
            }
        }

      if (testBalan->linkType == "2" ||           // only one entering and one
          testBalan->linkType == "3")             // leaving flow is allowed
        {

          if (inEdge.size()  > 1 ||
              outEdge.size() > 1)
            {
              nApp->message(42, testBalan->vertexId.the_string());
              errorFlag++;
            }
        }

      if (testBalan->linkType == "4")   // only one leaving energy
                                        // flow is allowed
        {
          if (outEdge.size() > 1)
            {
              nApp->message(41, testBalan->vertexId.the_string());
              errorFlag++;
            }
        }

      // at least one entering and one leaving energy flow

      if (inEdge.size()  == 0 ||
          outEdge.size() == 0)
        {
          nApp->message(43, testBalan->vertexId.the_string());
          errorFlag++;
        }
    }
}

//// orderNet
// ordering the graph
//
// This algorithm is based on "H.Walther, G. Naegler, Graphen-Algorithmen-
// Programme, Springer, Wien, New York (1987), p.53 ("Verbalalgorithmus
// zur Numerierung eines kreisfreien Graphen")", may be that this
// is not the fastest algorithm, but it works ...
//
// Note by Robbie: this general procedure is known as a "topological sort".
//
// An improved version of the algorithm used here is given in:
//
//     Bruckner, Thomas, Robbie Morrison, Chris Handley, Murray
//     Patterson.  2003.  High-resolution modeling of
//     energy-services supply systems using 'deeco' : overview
//     and application to policy development.  Annals of
//     Operations Research, 121(1-4):151-180.
//
void
Net::orderNet(void)
{
  Set_of_p<DVertex> vSet;
  Set_of_p<DEdge> eSet;
  vSet = systGraph.vertices();
  eSet = systGraph.edges();

  while(vSet)                     // vSet returns non-zero while it is not empty
    {

      // Robbie: 02.09.2005: Split original statement into two for ISO C++.
      //
      Set_of_p <DVertex> robbie04 = vSet;
      Set_of_piter <DVertex> vSetIt( robbie04 );

      // Set_of_piter<DVertex> vSetIt (vSet);

      Set_of_p <DEdge> robbie05 = eSet;
      Set_of_piter <DEdge> eSetIt( robbie05 );

      // Set_of_piter<DEdge> eSetIt (eSet);

      DEdge* e;
      DVertex* v;

      while (e = eSetIt.next())                   // "=" is okay
        {
          v = e->dst();
          v->set_val(v->val() + 1);
        }
      eSetIt.reset();                             // <Set.h>
      while (v = vSetIt.next())                   // "=" is okay
        {
          if (v->val() == 0)
            {
              if (procMap.element(v->vertexId))   // only processes
                {
                  sortList.put(v);
                }
              Set_of_p<DEdge> eOutSet = v->out_edges();

              // Robbie: 02.09.2005: Split original statement into two for ISO C++.
              //
              Set_of_p <DEdge> robbie06 = eOutSet;
              Set_of_piter <DEdge> eOutSetIt( robbie06 );

              DEdge* outE;
              while (outE = eOutSetIt.next())     // "=" is okay
                {
                  eSet.remove(outE);
                }
              eOutSetIt.reset();                  // <Set.h>
              vSet.remove(v);
            }
        }
      vSetIt.reset();                             // <Set.h>
      reset_val(vSet);                            // clear the vals
    }

  if (nApp->testFlag)
    {
      ostrstream ssInfo;                          // <strstream.h>
      List_of_piter<DVertex> sortListIt (sortList);
      vertexPtr sortVertex;
      ssInfo << "{";
      while (sortListIt.next(sortVertex))         // show sorted vertex list
        {
          ssInfo << *sortVertex;
          if (!sortListIt.at_end())
            {
              ssInfo << ",";
            }
          else
            {
              ssInfo << "}" << ends;
            }
        }
      char* pInfo = ssInfo.str();               // <strstream.h>, "freeze" to char*
      nApp->message(1016, pInfo);
      delete pInfo;           // Robbie: 18.11.2005: delete was delete[],
                              // deallocation is correct, see man strstream
                              // and method str().
    }
}

////////////// help functions for prepareOpt /////////////////

//// calcAttributes
// calculate attributes
//
void
Net::calcAttributes(const Map<Symbol,double>&  nVecU,
                    double                     actualIntLength)
{
  // fix the number of constraints which are time independent

  constraintCount        = nonProcConstraintCount;
  lessConstraintCount    = nonProcLessConstraintCount;
  greaterConstraintCount = nonProcGreaterConstraintCount;
  equalConstraintCount   = nonProcEqualConstraintCount;

  // calculate attributes

  List_of_piter<DVertex> sortListIt(sortList);
  sortListIt.end_reset();        // go to the end of the list (= sinks)
  vertexPtr sortVertex;

  // first from sinks to sources

  while (sortListIt.prev(sortVertex))
    {
      procPtr(sortVertex)->actualEnJ(nVecU);

      // Robbie: 02.09.2005: Split original statement into two for ISO C++.
      //
      Set_of_p <DEdge> robbie07 = sortVertex->in_edges();
      Set_of_piter <DEdge> inEdgeIt( robbie07 );

      edgePtr e;

      while (e = inEdgeIt.next())     // "=" is okay
        {
          if (e->energyFlowType == "H" &&
              e->linkType != "0")
            {
              // "find and get"

              if (e->linkType != "1")
                {
                  Mapiter <Symbol,MapSym2d> vecJIt (procPtr(sortVertex)->
                    vecJ[int_to_str(e->energyFlowNumber)]["En"]);

                  vecJIt = procPtr(sortVertex)->
                    vecJ[int_to_str(e->energyFlowNumber)]
                    ["En"].element("Out");

                  // do the "out" values which are imposed to exist
                  // by the link type really exist?

                  if (vecJIt &&
                      (
                       (e->linkType == "2" && vecJIt.curr()->value.element("R")) ||
                       (e->linkType == "3" && vecJIt.curr()->value.element("F")) ||
                       (e->linkType == "4" && vecJIt.curr()->value.element("F")  &&
                                              vecJIt.curr()->value.element("R"))
                       ))
                    {
                      vertexPtr predBalan;
                      predBalan = e->src();

                      // Robbie: 02.09.2005: Split original statement into two for ISO C++.
                      //
                      Set_of_p <DEdge> robbie08 = predBalan->in_edges();
                      Set_of_piter <DEdge> predBalanInEdgeIt( robbie08 );

                      edgePtr predBalanE;
                      while (predBalanE = predBalanInEdgeIt.next())  // "=" is okay
                        {
                          vertexPtr predProc;
                          predProc = predBalanE->src();

                          // transfer attributes
                          // procPtr(..) is used to convert vertexPtr -> procPtr

                          procPtr(predProc)
                            ->vecJ[int_to_str(predBalanE->energyFlowNumber)]
                            ["Ex"]["In"] = vecJIt.curr()->value;
                        }
                      vecJIt.remove();       // clear!
                    }
                  else
                    {
                      String mess = "(" + e->energyFlowType.the_string() + ","
                        + int_to_str(e->energyFlowNumber) + ") of "
                        + sortVertex->vertexId.the_string();
                      nApp->message(37,mess);
                      errorFlag++;
                    }
                }
            }
        }
    }

  // from sources to sinks

  sortListIt.reset();                        // to be sure that ...
  while (sortListIt.next(sortVertex))
    {
      procPtr(sortVertex)->actualExJ(nVecU);

      // Robbie: 02.09.2005: Split original statement into two for ISO C++.
      //
      Set_of_p <DEdge> robbie09 = sortVertex->out_edges();
      Set_of_piter <DEdge> outEdgeIt( robbie09 );

      edgePtr e;
      while (e = outEdgeIt.next())           // "=" is okay
        {
          if (e->energyFlowType == "H" && e->linkType != "0" )
            {
              if (e->linkType != "4")
                {
                  Mapiter <Symbol,MapSym2d> vecJIt (procPtr(sortVertex)->
                    vecJ[int_to_str(e->energyFlowNumber)]["Ex"]);

                  vecJIt = procPtr(sortVertex)
                    ->vecJ[int_to_str(e->energyFlowNumber)]
                    ["Ex"].element("Out");

                  // do the "out" values which are imposed to exist
                  // by the linktype really exist?

                  if (vecJIt &&
                      (
                       (e->linkType == "3" && vecJIt.curr()->value.element("R")) ||
                       (e->linkType == "2" && vecJIt.curr()->value.element("F")) ||
                       (e->linkType == "1" && vecJIt.curr()->value.element("F")  &&
                                              vecJIt.curr()->value.element("R"))
                      ))
                    {
                      vertexPtr succBalan;
                      succBalan = e->dst();

                      // Robbie: 02.09.2005: Split original statement into two for ISO C++.
                      //
                      Set_of_p <DEdge> robbie10 = succBalan->out_edges();
                      Set_of_piter <DEdge> succBalanEdgeIt( robbie10 );

                      edgePtr succBalanE;
                      while (succBalanE = succBalanEdgeIt.next())  // "=" is okay
                        {
                          vertexPtr succProc;
                          succProc = succBalanE->dst();
                          procPtr(succProc)
                            ->vecJ[int_to_str(succBalanE->energyFlowNumber)]
                            ["En"]["In"] = vecJIt.curr()->value;
                        }
                      vecJIt.remove();
                    }
                  else
                    {
                      String mess = "(" + e->energyFlowType.the_string() + ","
                        + int_to_str(e->energyFlowNumber) + ") of "
                        + sortVertex->vertexId.the_string();
                      nApp->message(37,mess);
                      errorFlag++;
                    }
                }
            }
        }

      // actualize objective function coefficients, constraint coefficients, and rhs

      procPtr(sortVertex)->actualSimplexInput(nVecU,actualIntLength);

      // calculate number of constraints for simplex algorithm

      lessConstraintCount
        = lessConstraintCount
        + procPtr(sortVertex)->lessConstraintCoef.size();
      greaterConstraintCount
        = greaterConstraintCount
        + procPtr(sortVertex)->greaterConstraintCoef.size();
      equalConstraintCount
        = equalConstraintCount
        + procPtr(sortVertex)->equalConstraintCoef.size();
      constraintCount
        = lessConstraintCount
        + greaterConstraintCount
        + equalConstraintCount;
    }
}

////  prepareSimplTab
// prepare simplex tableau
//
void
Net::prepareSimplTab(void)
{
  nSimplex = new Simplex(nApp,
                         energyFlowCount,
                         constraintCount,
                         lessConstraintCount,
                         greaterConstraintCount,
                         equalConstraintCount);
  if (nSimplex == 0)
    {
      nApp->message(11, "new Simplex in Net.C");
      exit(11);
    }
  if (constraintCount == 0 ||
      energyFlowCount <= 1)
    {
      nApp->message(49,"");
      errorFlag++;
    }
  if (!error())
    {
      // default values for aMatrix

      for (int k = 1; k <= energyFlowCount + 1; k++)
        {
          for (int l = 1; l <=constraintCount + 2; l++)
            {
              nSimplex->aMatrix[l][k] = 0;
            }
        }
    }
}

//// calcBalanceConstCoef
// calculate energy balance constraints simplex coefficients
//
void
Net::calcBalanceConstCoef(int& actualM3)
{
  int M1M2 = nSimplex->M1 + nSimplex->M2;

  for (Mapiter<Symbol,balanPtr> balanMapIt = balanMap.first();
       balanMapIt;
       balanMapIt.next())
    {
      actualM3++;

      // Robbie: 02.09.2005: Split original statement into two for ISO C++.
      //
      Set_of_p <DEdge> robbie11 = balanMapIt.curr()->value->in_edges();
      Set_of_piter <DEdge> inBalanEdgeIt( robbie11 );

      edgePtr inBalanE;
      while (inBalanE=inBalanEdgeIt.next())       // "=" is okay
        {
          for (Mapiter<int,Symbol2> exFlowIt
                 = procPtr(inBalanE->src())->exFlowAssign.first();
               exFlowIt;
               exFlowIt.next())
            {
              Symbol2 Sym = Symbol2(inBalanE->energyFlowType,
                                    int_to_str(inBalanE->energyFlowNumber));
              if (exFlowIt.curr()->value == Sym)
                {
                  nSimplex->aMatrix[M1M2 + actualM3 + 1][exFlowIt.curr()->key + 1] = -1;
                }
            }
        }

      // Robbie: 02.09.2005: Split original statement into two for ISO C++.
      //
      Set_of_p <DEdge> robbie12 = balanMapIt.curr()->value->out_edges();
      Set_of_piter <DEdge> outBalanEdgeIt( robbie12 );

      edgePtr outBalanE;
      while (outBalanE = outBalanEdgeIt.next())   // "=" is okay
        {
          for (Mapiter<int,Symbol2> enFlowIt
                 = procPtr(outBalanE->dst())->enFlowAssign.first();
               enFlowIt;
               enFlowIt.next())
            {
              Symbol2 Sym = Symbol2(outBalanE->energyFlowType,
                                    int_to_str(outBalanE->energyFlowNumber));
              if (enFlowIt.curr()->value == Sym)
                {
                  nSimplex->aMatrix[M1M2 + actualM3 + 1][enFlowIt.curr()->key + 1] = 1;
                }
            }
        }
    }
}

//// calcAggConstCoef
// calculate process aggregation constraint simplex coefficients
//
void
Net::calcAggConstCoef(int&         actualM1,
                      ioDefValue*  nAggDef,
                      ioValue*     nAggVal)
{
  for (Mapiter<Symbol,int> aggDefMapIt = nAggDef->defMap.first();
       aggDefMapIt;
       aggDefMapIt.next())
    {
      if (aggDefMapIt.curr()->value == 1)
        {
          actualM1++;
          aggConstAssign[actualM1] = aggDefMapIt.curr()->key;
          for (Mapiter<Symbol,double> aggValMapIt
                 = nAggVal->inValMap[aggDefMapIt.curr()->key].first();
               aggValMapIt;
               aggValMapIt.next())
            {
              double rhs;
              rhs = aggValMapIt.curr()->value;
              if (!(rhs < 0))      // to be certain that a constraint is wanted
                {
                  nSimplex->aMatrix[actualM1 + 1][1] = rhs;
                  procPtr p;
                  p = procMap[aggValMapIt.curr()->key];
                  Symbol powerType;
                  Symbol powerNumber;
                  Symbol power;
                  power = p->showPower(powerType, powerNumber);

                  if (power == "En")
                    {
                      for (Mapiter<int,Symbol2> enFlowIt
                             = p->enFlowAssign.first();
                           enFlowIt;
                           enFlowIt.next())
                        {
                          Symbol2 Sym = Symbol2(powerType, powerNumber);
                          if (enFlowIt.curr()->value == Sym)
                            {
                              nSimplex->aMatrix[actualM1 + 1]
                                [enFlowIt.curr()->key + 1] = -1;
                            }
                        }
                    }

                  if (power == "Ex")
                    {
                      for (Mapiter<int,Symbol2> exFlowIt
                             = p->exFlowAssign.first();
                           exFlowIt;
                           exFlowIt.next())
                        {
                          Symbol2 Sym = Symbol2(powerType, powerNumber);
                          if (exFlowIt.curr()->value == Sym)
                            {
                              nSimplex->aMatrix[actualM1 + 1]
                                [exFlowIt.curr()->key + 1] = -1;
                            }
                        }
                    }
                }
              else
                {
                  nApp->message(53, aggDefMapIt.curr()->key.the_string());
                  errorFlag++;
                }
            }
        }
    }
}

//// calcProcConstCoef
// calculate process constraint simplex coefficients
//
void
Net::calcProcConstCoef(ScenVal*  nScenVal,
                       int&      actualM1,
                       int&      actualM2,
                       int&      actualM3)
{
  for (Mapiter<Symbol,procPtr> procMapIt = procMap.first();
       procMapIt;
       procMapIt.next())
    {
      establishAMatrixProc(procMapIt.curr()->value,nScenVal,
                           actualM1,actualM2,actualM3);
    }
}

//// establishAMatrixProc
// establish the parts of the simplex tableau matrix "aMatrix"
// which are process dependent
//
void
Net::establishAMatrixProc(procPtr   nProc,
                          ScenVal*  nScenVal,
                          int&      actualM1,
                          int&      actualM2,
                          int&      actualM3)
{
  // objective function and electrical energy balance

  for (Mapiter<int,Symbol2> exFlowIt =nProc->exFlowAssign.first();
       exFlowIt; exFlowIt.next())
    {
      double d = 0;
      int i    = 0;

      for (Mapiter<Symbol,double> goalWeightIt
             = nScenVal->goalWeight.first();
           goalWeightIt;
           goalWeightIt.next())
        {
          Mapiter <Symbol,MapSym1d> objectFuncIt1
            (nProc->objectFuncCoef[goalWeightIt.curr()->key]["Ex"]);
          objectFuncIt1
            = nProc->objectFuncCoef[goalWeightIt.curr()->key]
            ["Ex"].element(exFlowIt.curr()->value.comp1);
          if (objectFuncIt1)
            {
              Mapiter<Symbol,double> objectFuncIt2
                (objectFuncIt1.curr()->value);
              objectFuncIt2
                = objectFuncIt1.curr()->value.element(exFlowIt.curr()->value.comp2);
              if (objectFuncIt2)
                {
                  d = d + (goalWeightIt.curr()->value
                           * objectFuncIt2.curr()->value);
                  i++;
                }
            }
        }

      if (i == nScenVal->goalWeight.size())
        {
          nSimplex->aMatrix[1][exFlowIt.curr()->key + 1] = -d;
        }

      // the first row of aMatrix;
      // "-" because Numerical Recipes simplex maximizes; we need the minimum
      // "+1" because column 1 is reserved for the rhs-values

      else
        {
          if (i != 0)  // if there is one object function coefficient,
                       // there should be all
            {
              nApp->message(48, nProc->vertexId.the_string());
              errorFlag++;
            }
        }

      // electrical energy balance

      if (exFlowIt.curr()->value.comp1 == "El")
        {
          nSimplex->aMatrix[1 + constraintCount][exFlowIt.curr()->key + 1]= -1;
        }
    }

  for (Mapiter<int,Symbol2> enFlowIt = nProc->enFlowAssign.first();
       enFlowIt;
       enFlowIt.next())
    {
      double d = 0;
      int i    = 0;
      for (Mapiter<Symbol,double> goalWeightIt
             = nScenVal->goalWeight.first();
           goalWeightIt;
           goalWeightIt.next())
        {
          Mapiter <Symbol,MapSym1d> objectFuncIt1
            (nProc->objectFuncCoef[goalWeightIt.curr()->key]["En"]);
          objectFuncIt1
            = nProc->objectFuncCoef[goalWeightIt.curr()->key]
              ["En"].element(enFlowIt.curr()->value.comp1);
          if (objectFuncIt1)
            {
              Mapiter<Symbol,double> objectFuncIt2(objectFuncIt1.curr()
                                                    ->value);
              objectFuncIt2 = objectFuncIt1.curr()->value.element
                (enFlowIt.curr()->value.comp2);
              if (objectFuncIt2)
                {
                  d = d + (goalWeightIt.curr()->value
                           * objectFuncIt2.curr()->value);
                  i++;
                }
            }
        }

      if (i == nScenVal->goalWeight.size())
        {
          nSimplex->aMatrix[1][enFlowIt.curr()->key + 1] = -d;
        }
      else
        {
          if (i != 0)     // if there is one object function coefficient,
                          // there should be all
            {
              nApp->message(48, nProc->vertexId.the_string());
              errorFlag++;
            }
        }

      // electrical energy balance

      if (enFlowIt.curr()->value.comp1 == "El")
        {
          nSimplex->aMatrix[1 + constraintCount][enFlowIt.curr()->key + 1] = 1;
        }
    }

  // "<=" constraints

  setConstCoef(nProc, nProc->lessConstraintCoef,
               nProc->lessConstraintRhs, actualM1, 0);

  // ">=" constraints

  setConstCoef(nProc, nProc->greaterConstraintCoef,
               nProc->greaterConstraintRhs, actualM2, nSimplex->M1);

  // "=" constraints

  setConstCoef(nProc, nProc->equalConstraintCoef,
               nProc->equalConstraintRhs, actualM3, nSimplex->M1 + nSimplex->M2);

}

//// setConstCoef
// set the constraint coefficients for the different restriction types
//
void
Net::setConstCoef(procPtr             nProc,
                  Map<int,MapSym3d>&  constraintCoef,
                  Map<int,double>&    constraintRhs,
                  int&                actualM,
                  int                 mOffset)
{
  for (Mapiter<int,MapSym3d> coefIt = constraintCoef.first();
       coefIt;
       coefIt.next())
    {
      actualM++;
      nProc->constraintAssign[mOffset + actualM]
        = coefIt.curr()->key;
      nSimplex->aMatrix[mOffset + actualM + 1][1]
        = constraintRhs[coefIt.curr()->key];

      for (Mapiter<int,Symbol2> exFlowIt = nProc->exFlowAssign.first();
           exFlowIt;
           exFlowIt.next())
        {
          Mapiter <Symbol,MapSym1d> coefIt2 (coefIt.curr()->value["Ex"]);
          coefIt2
            = coefIt.curr()->value["Ex"].element(exFlowIt.curr()->value.comp1);
          if (coefIt2)
            {
              Mapiter <Symbol,double> coefIt3 (coefIt2.curr()->value);
              coefIt3
                = coefIt2.curr()->value.element(exFlowIt.curr()->value.comp2);
              if (coefIt3)
                {
                  nSimplex->aMatrix[mOffset + actualM + 1][exFlowIt.curr()->key + 1]
                    = -coefIt3.curr()->value;
                }
            }
        }

      for (Mapiter<int,Symbol2> enFlowIt = nProc->enFlowAssign.first();
           enFlowIt;
           enFlowIt.next())
        {
          Mapiter <Symbol,MapSym1d> coefIt2 (coefIt.curr()->value["En"]);
          coefIt2
            = coefIt.curr()->value["En"].element(enFlowIt.curr()->value.comp1);
          if (coefIt2)
            {
              Mapiter <Symbol,double> coefIt3 (coefIt2.curr()->value);
              coefIt3
                = coefIt2.curr()->value.element(enFlowIt.curr()->value.comp2);
              if (coefIt3)
                {
                  nSimplex->aMatrix[mOffset + actualM + 1][enFlowIt.curr()->key + 1]
                    = -coefIt3.curr()->value;
                }
            }
        }
    }
}

//////////// help functions for finishOpt ////////////////////

//// getProcessResults1
// getting results for each process (part1)
//
void
Net::getProcessResults1(double&                    actualIntLength,
                        const Map<Symbol,double>&  nVecU)
{
  for (Mapiter<Symbol,procPtr> procMapIt = procMap.first();
       procMapIt;
       procMapIt.next())
    {
      // getting results for leaving energy flows

      for (Mapiter <int,Symbol2> exFlowIt = procMapIt.curr()->value->
             exFlowAssign.first();
           exFlowIt;
           exFlowIt.next())
        {
          int j = 1;
          while (j < nSimplex->M && nSimplex->iposv[j] != exFlowIt.curr()->key)
            {
              j++;
            }
          if (nSimplex->iposv[j] == exFlowIt.curr()->key)
            {
              procMapIt.curr()->value->dotEEx[exFlowIt.curr()->value.comp1]
                  [exFlowIt.curr()->value.comp2]
                = nSimplex->aMatrix[j + 1][1];
            }
          else
            {
              procMapIt.curr()->value->dotEEx[exFlowIt.curr()->value.comp1]
                  [exFlowIt.curr()->value.comp2]
                = 0;
            }
        }

      // getting results for entering energy flows

      for (Mapiter <int,Symbol2> enFlowIt
             = procMapIt.curr()->value->enFlowAssign.first();
           enFlowIt;
           enFlowIt.next())
        {
          int j = 1;
          while (j < nSimplex->M &&
                 nSimplex->iposv[j] != enFlowIt.curr()->key)
            {
              j++;
            }
          if (nSimplex->iposv[j] == enFlowIt.curr()->key)
            {
              procMapIt.curr()->value->dotEEn[enFlowIt.curr()->value.comp1]
                  [enFlowIt.curr()->value.comp2]
                = nSimplex->aMatrix[j + 1][1];
            }
          else
            {
              procMapIt.curr()->value->dotEEn[enFlowIt.curr()->value.comp1]
                  [enFlowIt.curr()->value.comp2]
                = 0;
            }
        }

      // actualize state variable (if the change of them is not too large)

      while (!procMapIt.curr()->value->actualState(actualIntLength, nVecU))
        {
          actualIntLength = actualIntLength/double(2);
        }
    }
}

//// getProcessResults2
// getting results for each process (part 2)
//
void
Net::getProcessResults2(ScenVal*     nScenVal,
                        ScenTsPack*  nScenTsPack,
                        double       actualIntLength)
{
  Map<Symbol,double> actualVecC;

  // mean values of the objective function

  nScenVal->scenOutValMap["Goal"].updateMeanValRec(-nSimplex->aMatrix[1][1]);
  // "-" because Numerical Recipes maximizes and we need the minimum

  for (Mapiter<Symbol,procPtr>procMapIt = procMap.first();
       procMapIt;
       procMapIt.next())
    {
      if (nScenVal->allResFlag)
        {

          // update state variable mean values

          for (Mapiter<int,double> E_sIt
                 = procMapIt.curr()->value->E_s.first();
               E_sIt;
               E_sIt.next())
            {
              procMapIt.curr()->value->meanE_s
                [E_sIt.curr()->key].updateMeanValRec(E_sIt.curr()->
                value, actualIntLength);
            }

          // getting results for slack variables and shadow prices

          for (Mapiter <int,int> constIt
                 = procMapIt.curr()->value->constraintAssign.first();
               constIt;
               constIt.next())
            {
              // getting slack value

              int j = 1;
              while(j < nSimplex->M &&
                    nSimplex->iposv[j] !=constIt.curr()->key + nSimplex->N)
                {
                  j++;
                }

              if (!procMapIt.curr()->value->equalConstraintCoef.element
                  (constIt.curr()->value))
                {
                  if (nSimplex->iposv[j] == constIt.curr()->key + nSimplex->N)
                    {
                      procMapIt.curr()->value->meanSlack
                        [constIt.curr()->value].updateMeanValRec
                        (nSimplex->aMatrix[j + 1][1], actualIntLength);
                    }
                  else
                    {
                      procMapIt.curr()->value->meanSlack
                        [constIt.curr()->value].updateMeanValRec(0,
                        actualIntLength);
                    }
                }

              // getting shadow prices

              int i = 1;
              while(i < nSimplex->N &&
                    nSimplex->izrov[i] != constIt.curr()->key + nSimplex->N)
                {
                  i++;
                }

              if (!procMapIt.curr()->value->equalConstraintCoef.element
                  (constIt.curr()->value))      // !16.5.95
                {                               // don't use izrov[i]
                                                // for "=" constraints

                  if (nSimplex->izrov[i] == constIt.curr()->key + nSimplex->N)
                    {
                      procMapIt.curr()->value->meanShadowPrice
                       [constIt.curr()->value].updateMeanValRec(-nSimplex->
                       aMatrix[1][1 + i], actualIntLength);
                     }

                  // "-nSimplex..." because the objective function has been
                  // chosen to be (-1) * general costs for minimization

                  else
                    {
                      procMapIt.curr()->value->meanShadowPrice
                        [constIt.curr()->value].updateMeanValRec(0,
                        actualIntLength);
                    }
                }

              // update mean values of meanDotE_0

              double rhs = 0;
              int k      = 0;

              Mapiter<int,double> lessConstRhsIt (procMapIt.curr()->value->
                                                  lessConstraintRhs);
              if (lessConstRhsIt
                  = procMapIt.curr()->value->
                    lessConstraintRhs.element(constIt.curr()->value)) // "=" is okay
                {
                  rhs = lessConstRhsIt.curr()->value;
                  k++;
                }
              if (!k)
                {
                  Mapiter<int,double> greaterConstRhsIt
                    (procMapIt.curr()->value->greaterConstraintRhs);
                  if (greaterConstRhsIt
                      = procMapIt.curr()->value->greaterConstraintRhs.
                      element(constIt.curr()->value))  // "=" is okay
                    {
                      rhs= greaterConstRhsIt.curr()->value;
                      k++;
                    }
                }

              if (!k)
                {
                  Mapiter<int,double> equalConstRhsIt
                    (procMapIt.curr()->value->equalConstraintRhs);
                  if (equalConstRhsIt
                      = procMapIt.curr()->value->equalConstraintRhs.
                        element(constIt.curr()->value))  // "=" is okay
                    {
                      rhs = equalConstRhsIt.curr()->value;
                      k++;
                    }
                }

              procMapIt.curr()->value->meanDotE_0[constIt.curr()->
                value].updateMeanValRec(rhs, actualIntLength);
            }
        }

      // update meanDotEEx, meanCoef (partially) and the objective function
      // components (partially)

      for (Mapiter<Symbol,MapSym1d> dotEExIt1
             = procMapIt.curr()->value->dotEEx.first();
           dotEExIt1;
           dotEExIt1.next())
        {
          for (Mapiter<Symbol,double> dotEExIt2
                 = dotEExIt1.curr()->value.first();
               dotEExIt2;
               dotEExIt2.next())
            {
              double dotE;
              dotE = dotEExIt2.curr()->value;
              procMapIt.curr()->value->meanDotEEx[dotEExIt1.curr()->key]
                [dotEExIt2.curr()->key].updateMeanValRec(dotE, actualIntLength);

              if (nScenVal->allResFlag)
                {
                  updateCoef(dotE,actualIntLength,
                             "Ex",
                             dotEExIt1.curr()->key,
                             dotEExIt2.curr()->key,
                             procMapIt.curr()->value->lessConstraintCoef,
                             procMapIt.curr()->value->meanCoef);
                  updateCoef(dotE,actualIntLength,
                             "Ex",
                             dotEExIt1.curr()->key,
                             dotEExIt2.curr()->key,
                             procMapIt.curr()->value->greaterConstraintCoef,
                             procMapIt.curr()->value->meanCoef);
                  updateCoef(dotE,actualIntLength,
                             "Ex",
                             dotEExIt1.curr()->key,
                             dotEExIt2.curr()->key,
                             procMapIt.curr()->value->equalConstraintCoef,
                             procMapIt.curr()->value->meanCoef);
                }

              for (Mapiter<Symbol,MapSym3d> objFuncCoefIt1
                     = procMapIt.curr()->value->objectFuncCoef.first();
                   objFuncCoefIt1;
                   objFuncCoefIt1.next())
                {
                  double coef = 0;
                  Mapiter<Symbol,MapSym1d> objFuncCoefIt3
                    (objFuncCoefIt1.curr()->value["Ex"]);

                  if (objFuncCoefIt3 = objFuncCoefIt1.curr()->value
                      ["Ex"].element(dotEExIt1.curr()->key))  // "=" is okay
                    {
                      Mapiter<Symbol,double> objFuncCoefIt4
                        (objFuncCoefIt3.curr()->value);
                      if (objFuncCoefIt4
                          = objFuncCoefIt3.curr()->value.
                            element(dotEExIt2.curr()->key))
                        {
                          coef
                            = objFuncCoefIt4.curr()->value;
                          actualVecC[objFuncCoefIt1.curr()->key]
                            += (coef * dotE);
                          if (nScenVal->allResFlag)
                            {
                              procMapIt.curr()->value->meanObjectFuncCoef
                                [objFuncCoefIt1.curr()->key]
                                ["Ex"][dotEExIt1.curr()->key]
                                [dotEExIt2.curr()->key].updateMeanValRec(coef,
                                (dotE * actualIntLength));
                            }
                        }
                    }
                }
            }
        }

      // update meanDotEEn, meanCoef (partially) and the objective function
      // components (partially)

      for (Mapiter<Symbol,MapSym1d> dotEEnIt1
             = procMapIt.curr()->value->dotEEn.first();
           dotEEnIt1;
           dotEEnIt1.next())
        {
          for (Mapiter<Symbol,double> dotEEnIt2
                 = dotEEnIt1.curr()->value.first();
               dotEEnIt2;
               dotEEnIt2.next())
            {
              double dotE;
              dotE = dotEEnIt2.curr()->value;
              procMapIt.curr()->value->meanDotEEn[dotEEnIt1.curr()->key]
                [dotEEnIt2.curr()->key].updateMeanValRec(dotE, actualIntLength);
              if (nScenVal->allResFlag)
                {
                  updateCoef(dotE,actualIntLength,
                             "En",
                             dotEEnIt1.curr()->key,
                             dotEEnIt2.curr()->key,
                             procMapIt.curr()->value->lessConstraintCoef,
                             procMapIt.curr()->value->meanCoef);
                  updateCoef(dotE,actualIntLength,
                             "En",
                             dotEEnIt1.curr()->key,
                             dotEEnIt2.curr()->key,
                             procMapIt.curr()->value->greaterConstraintCoef,
                             procMapIt.curr()->value->meanCoef);
                  updateCoef(dotE,actualIntLength,
                             "En",
                             dotEEnIt1.curr()->key,
                             dotEEnIt2.curr()->key,
                             procMapIt.curr()->value->equalConstraintCoef,
                             procMapIt.curr()->value->meanCoef);
                }

              for (Mapiter<Symbol,MapSym3d> objFuncCoefIt1
                     = procMapIt.curr()->value->objectFuncCoef.first();
                   objFuncCoefIt1;
                   objFuncCoefIt1.next())
                {
                  double coef = 0;
                  Mapiter<Symbol,MapSym1d> objFuncCoefIt3
                    (objFuncCoefIt1.curr()->value["En"]);
                  if (objFuncCoefIt3
                      = objFuncCoefIt1.curr()->value
                        ["En"].element(dotEEnIt1.curr()->key))  // "=" is okay
                    {
                      Mapiter<Symbol,double> objFuncCoefIt4
                        (objFuncCoefIt3.curr()->value);
                      if (objFuncCoefIt4
                          = objFuncCoefIt3.curr()->value.
                            element(dotEEnIt2.curr()->key))
                        {
                          coef = objFuncCoefIt4.curr()->value;
                          actualVecC[objFuncCoefIt1.curr()->key]
                            += (coef * dotE);
                          if (nScenVal->allResFlag)
                            {
                              procMapIt.curr()->value->meanObjectFuncCoef
                                [objFuncCoefIt1.curr()->key]
                                ["En"][dotEEnIt1.curr()->key]
                                [dotEEnIt2.curr()->key].updateMeanValRec(coef,
                                (dotE * actualIntLength));
                            }
                        }
                    }
                }
            }
        }

      // add the optimization independent part of the general cost components

      for (Mapiter<Symbol,double> actualVecCIt = actualVecC.first();
           actualVecCIt;
           actualVecCIt.next())
        {
          actualVecCIt.curr()->value
            += procMapIt.curr()->value->vecCConstant[actualVecCIt.curr()->key];
        }

      // further update

      if (nScenVal->allResFlag)
        {
          procMapIt.curr()->value->updateMean(actualIntLength);
        }
      if (nScenVal->tsOutFlag)
        {
          procMapIt.curr()->value->updateProcOutTsPack
            (actualIntLength,nScenVal->intLength);
        }
    }

  for (Mapiter<Symbol,double> actualVecCIt = actualVecC.first();
       actualVecCIt;
       actualVecCIt.next())
    {
      String symVar = "Var" + actualVecCIt.curr()->key.the_string();
      nScenVal->scenOutValMap[symVar].updateMeanValRec(actualVecCIt.curr()->
                                                       value, actualIntLength);
      if (nScenVal->tsOutFlag)
        {
          nScenTsPack->vecC[symVar]
            += (actualVecCIt.curr()->value
                * (actualIntLength/nScenVal->intLength));
        }
    }
}

//// getAggResults
// getting results for process aggregates
//
void
Net::getAggResults(ioDefValue*  nAggDef,
                   ioValue*     nAggVal,
                   double       actualIntLength)
{
  for (Mapiter <int,Symbol> aggConstIt = aggConstAssign.first();
       aggConstIt;
       aggConstIt.next())
    {
      // getting slack value

      int j = 1;
      while (j < nSimplex->M &&
             nSimplex->iposv[j] != aggConstIt.curr()->key + nSimplex->N)
        {
          j++;
        }

      double slack = 0;
      if (nSimplex->iposv[j] == aggConstIt.curr()->key + nSimplex->N)
        {
          slack = nSimplex->aMatrix[j + 1][1];
        }
      else
        {
          slack = 0;
        }
      nAggVal->outValMap[aggConstIt.curr()->value]
        ["Slack"].updateMeanValRec(slack, actualIntLength);

      // getting the total energy flow (equal to power of aggregate)

      double power = 0;
      Mapiter <Symbol,double> aggInValMapIt
        (nAggVal->inValMap[aggConstIt.curr()->value]);
      aggInValMapIt = nAggVal->inValMap[aggConstIt.curr()->value].first();

      // the constraint rhs is equal for each process, so the first one
      // can be used

      double rhs;
      rhs   = aggInValMapIt.curr()->value;
      power = rhs - slack;
      nAggVal->outValMap[aggConstIt.curr()->value]
        ["Power"].updateMeanValRec(power, actualIntLength);

      // getting shadow prices

      int i = 1;
      while (i < nSimplex->N &&
             nSimplex->izrov[i] != aggConstIt.curr()->key
             + nSimplex->N)
        {
          i++;
        }

      double shadow = 0;
      if (nSimplex->izrov[i] == aggConstIt.curr()->key + nSimplex->N)
        {
          shadow = -nSimplex->aMatrix[1][1 + i];
        }

      // "-nSimplex..." because the objective function has been chosen
      // to be (-1) * general costs for minimization

      else
        {
          shadow = 0;
          nAggVal->outValMap[aggConstIt.curr()->value]
            ["SPrice"].updateMeanValRec(shadow,actualIntLength);
        }
    }

  // take into account the aggregates for which no constraint is
  // given

  for (Mapiter<Symbol,int> aggDefMapIt = nAggDef->defMap.first();
       aggDefMapIt;
       aggDefMapIt.next())
    {
      if (!(aggDefMapIt.curr()->value == 0) &&
          !(aggDefMapIt.curr()->value == 1))
        {
          double power = 0;
          for (Mapiter<Symbol,double> aggValMapIt
                 = nAggVal->inValMap[aggDefMapIt.curr()->key].first();
               aggValMapIt;
               aggValMapIt.next())
            {
              procPtr p;
              p = procMap[aggValMapIt.curr()->key];
              Symbol powerType;
              Symbol powerNumber;
              Symbol powerKind;
              powerKind = p->showPower(powerType,powerNumber);
              if (powerKind == "Ex")
                {
                  power += p->dotEEx[powerType][powerNumber];
                }
              if (powerKind == "En")
                {
                  power += p->dotEEn[powerType][powerNumber];
                }
            }
          nAggVal->outValMap[aggDefMapIt.curr()->key]
            ["Power"].updateMeanValRec(power,actualIntLength);
        }
    }
}

//// clearConstraints       // !2.8.95->
// clear constraints of processes and aggregates
//
void
Net::clearConstraints(void)
{
  // processes

  for (Mapiter<Symbol,procPtr>procMapIt = procMap.first();
       procMapIt; procMapIt.next())
    {
      procMapIt.curr()->value->objectFuncCoef.make_empty();
      procMapIt.curr()->value->greaterConstraintCoef.make_empty();
      procMapIt.curr()->value->greaterConstraintRhs.make_empty();
      procMapIt.curr()->value->lessConstraintCoef.make_empty();
      procMapIt.curr()->value->lessConstraintRhs.make_empty();
      procMapIt.curr()->value->equalConstraintCoef.make_empty();
      procMapIt.curr()->value->equalConstraintRhs.make_empty();
      procMapIt.curr()->value->constraintAssign.make_empty();
    }

  // aggregates

  aggConstAssign.make_empty();

}  // !2.8.95<-

//// updateCoef
//
//
void
Net::updateCoef(double              dotE,
                double              intLength,
                Symbol              ExEn,
                Symbol              energyFlowType,
                Symbol              energyFlowNumber,
                Map<int,MapSym3d>&  coefMap,
                Map<int,MapSym3M>&  meanCoefMap)
{
  for (Mapiter<int,MapSym3d> coefMapIt1 = coefMap.first();
       coefMapIt1;
       coefMapIt1.next())
    {
      double coef = 0;
      if (coefMapIt1.curr()->value.element(ExEn))
        {
          Mapiter<Symbol,MapSym1d> coefMapIt3
            (coefMapIt1.curr()->value[ExEn]);

          if (coefMapIt3 = coefMapIt1.curr()->value
              [ExEn].element(energyFlowType))  // "=" is okay
            {
              Mapiter<Symbol,double> coefMapIt4
                (coefMapIt3.curr()->value);
              if (coefMapIt4 = coefMapIt3.curr()->
                  value.element(energyFlowNumber))
                {
                  coef = coefMapIt4.curr()->value;
                  meanCoefMap[coefMapIt1.curr()->key]
                    [ExEn][ energyFlowType]
                    [energyFlowNumber].updateMeanValRec(coef,
                    (dotE * intLength));
                }
            }
        }
    }
}

//  Robbie: 02.09.05: Notes about ISO C++ modifications.
//
//  Set (the variant for holding pointers) iterator:
//
//    For some reason, the iterator would not attach to the SCO_SC::Set_of_p in one
//    step as it had before. The fix was to split this process into two statements,
//    as shown:
//
//       Set_of_p <DEdge> temp01 = procMapIt.curr()->value->in_edges();
//       Set_of_piter <DEdge> inEdgeIt( temp01 );

//  $Source: /home/morrison/milp-mid-2005/deeco.006.2/RCS/Net.C,v $
//  end of file
