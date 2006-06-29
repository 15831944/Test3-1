//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __Mill_H
#define  __Mill_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

//===========================================================================

class CMill
{
public:

   // Available Mill Methods
   enum eMethod { eMethod_Ball ,
				  eMethod_Rod  ,
                  eMethod_AGSAG};

    // Constructor
	CMill() { };

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