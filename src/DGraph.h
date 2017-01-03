
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
// DEECO GRAPH MANAGEMENT OBJECT
//
//////////////////////////////////////////////////////////////////


#ifndef _DGRA_                // header guard
#define _DGRA_


#include "Data.h"
#include <Symbol.h>           // compiler options -l++, -I/usr/include/SC
#include <Map.h>
#include <Path.h>             // ?test
#include <Graph.h>            // Graph management
                              // compiler options -lGraph, -I/usr/include/SC
#include <Graph_alg.h>        // Graph algorithms
                              // compiler options -lGA, -I/usr/include/SC


// Note from Robbie: the compiler options listed above apply to the
// HP-UX settings.  Read the current makefile for updated information.


// Important: the package <Graph.h> uses parameterized macros to simulate
// templates!  This places restrictions on the syntax that can be used.
//
// Important: for a comprehensive tutorial covering the graph containers
// and graph algorithms used here, see: Weitzen, TC. 1992. "The C++ graph
// classes : a tutorial.  In: AT&T and UNIX System Laboratories (eds),
// "USL C++ Standards Components, Release 3.0 : Programmer's Guide".
// pp 7.1-7.30.  AT&T and UNIX System Laboratories.  See also the relevant
// man pages and related systems documentation.
//
// Important: the order of graph object invokation is significant and
// differs from that described in Weitzen (1992).  See Morrison, Robbie.
// 2000. "Optimizing exergy-services supply networks for sustainability
// -- MSc thesis". Otago University, New Zealand, appendix C.2 for
// further details and sample code.


// Attention: insertion of spaces in the argument list is not permitted
// in macro calls, neither is a ";" at the end required.

Graphdeclare1(DGraph,DVertex,DEdge)     // macro invocation <Graph.h>


//////////////////////////////////////////////////////////////////
//
// CLASS: DVertex
//
//////////////////////////////////////////////////////////////////

class DVertex : public Vertex
{
public:

    Symbol vertexId;                // forward declaration

 // Standard Constructor
    DVertex(void)
    {
      vertexId = "";
    }

 // Constructor
    DVertex(Symbol vId) : Vertex()
    {
      vertexId = vId;
    }

 // operator<<
 // write DVertex
   friend ostream& operator<<(ostream& os, const DVertex& v)
   {
     os << v.vertexId;
     return os;
   };

derivedVertex(DGraph,DVertex,DEdge) // macro invocation <Graph.h>

};


//////////////////////////////////////////////////////////////////
//
// CLASS: DEdge
//
//////////////////////////////////////////////////////////////////

class DEdge : public Edge
{
public:

 // Constructor
    DEdge(DVertex* dV1, DVertex* dV2, App* cEdgeApp, Symbol cEdgeId,
          Symbol cEFT,int cEFN, Symbol cLinkType) : Edge(dV1, dV2)
    {
      edgeApp          = cEdgeApp;
      edgeId           = cEdgeId;
      energyFlowType   = cEFT;
      energyFlowNumber = cEFN;
      linkType         = cLinkType;
    }

 // operator<<
 // write DEdge
    friend ostream& operator<<(ostream& os, const DEdge& e)
    {
      os << e.edgeId;
      return os;
    };

    Symbol edgeId;
    Symbol energyFlowType;
    int energyFlowNumber;      // number of the energy leaving or entering a
                               // process, e.g. the l-th leaving waste heat
                               // flow; it is not necessary to specify the
                               // connected procId or balanId, because this
                               // is done automatically by DEdge::DEdge(...)
    Symbol linkType;
    App* edgeApp;

    derivedEdge(DGraph,DVertex,DEdge)     // macro invocation <Graph.h>

protected:

};


//////////////////////////////////////////////////////////////////
//
// CLASS: DGraph
//
//////////////////////////////////////////////////////////////////

// Attention: insertion of spaces in the argument list is not permitted
// in macro calls, neither is a ";" at the end required.

class DGraph : public Graph
{
public:

 // Constructor
    DGraph() : Graph()
    {
    }

    derivedGraph(DGraph,DVertex,DEdge)    // macro invocation <Graph.h>

protected:

};


Graphdeclare2(DGraph,DVertex,DEdge)       // macro invocation <Graph.h>
Graph_algdeclare(DGraph,DVertex,DEdge)    // macro invocation <Graph_alg.h>


#endif  // _DGRA_


