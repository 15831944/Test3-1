//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __PlugFlowSurge_H
#define  __PlugFlowSurge_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include "Surge.h"

//---------------------------------------------------------------------------

class CSurge_PlugFlow : public CSurge
{
public:
    // Constructor
	CSurge_PlugFlow();

    // Data Entry Field Method
	virtual void BuildDataFields(MDataDefn &DB);

    // Surge Method Execution
	virtual void EvalProducts(MStream &Feed ,
	                         MStream &Product,
							 bool    bInit=false);

protected:

    long mHeight;
	long mArea;
	long mSections;
};

#endif