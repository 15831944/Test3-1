//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __PlugFlowSurge_CPP
#include "PlugFlowSurge.h"

//====================================================================================
//             Efficiency Curve
//====================================================================================

   CSurge_PlugFlow::CSurge_PlugFlow()
	{


	}

	void CSurge_PlugFlow::BuildDataFields(MDataDefn &DB)
	
	{ 
     DB.ObjectBegin("TS_PlugFlow", "PlugFlow");


	 DB.Text("Plug Flow");

	 DB.ObjectEnd();

	}

//====================================================================================

	void CSurge_PlugFlow::EvalProducts(MStream &Feed ,
	                         MStream &Product,
							 bool    bInit)
	
	{ 
 
	}

//====================================================================================
