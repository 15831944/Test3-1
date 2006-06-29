//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __Comminution_H
#define  __Comminution_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//===========================================================================

class CComminution
{
public:

   // Available Comminution Methods
   enum eMethod { eMethod_Individual_Fixed ,
                  eMethod_Whiten ,
				  eMethod_HPRC ,
                  eMethod_Model400 };

    // Constructor
	CComminution() { };

    // Data Entry Field Method
    virtual void BuildDataFields(MDataDefn &DB) = 0;

    // Seperator Method Execution
    virtual void EvalProducts( MBaseMethod &M,
							   MStream &Feed ,
	                           MStream &Product , 
							   bool bInit) = 0;


};

//===========================================================================

#endif