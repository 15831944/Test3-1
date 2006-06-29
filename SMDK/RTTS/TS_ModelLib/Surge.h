//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __Surge_H
#define  __Surge_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//===========================================================================

class CSurge
{
public:

    // Available Surge Methods
	enum eMethod { eMethod_PlugFlow , eMethod_FullyMixed };

    // Constructor
	CSurge() { };

  // Data Entry Field Method
  virtual void BuildDataFields(MDataDefn &DB) = 0;

  // Surge Method Execution
  virtual void EvalProducts( MStream &Feed ,
	                         MStream &Product ,
							 bool    bInit) = 0;


};

//===========================================================================

#endif