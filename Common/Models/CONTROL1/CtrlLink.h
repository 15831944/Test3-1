//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __CTRLLINK_H
#define  __CTRLLINK_H

#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif  
#ifndef __FLWNODE_H
#include "flwnode.h"
#endif
#ifndef __MODELS_H
#include "xrefs.h"
#endif  
#ifndef __MODELS_H
#include "models.h"
#endif  

#ifdef __CTRLLINK_CPP
  #define DllImportExport 
#elif !defined(Control1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//--------------------------------------------------------------------------


DEFINE_TAGOBJ(CCtrlLink);
class CCtrlLink : public FlwNode
  {
  public:
    flag           m_bOn;                  //
    double         m_dValue;

    CCtrlLink(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CCtrlLink();
    virtual void   ResetData(flag Complete);
  public:

    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg) { if (NoCIOs()<2) Tg=ID=FullObjTag(); else Ctrl_GetConnIDStr(OtherEnd(i), ID, Tg); };

    virtual void   PostConnect(int IONo);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalCtrlInitialise();
    virtual void   EvalCtrlStrategy();
    virtual void   EvalDiscrete();
    virtual dword  ModelStatus();
    
    DEFINE_CI(CCtrlLink, FlwNode, 4);
  };
  
//===========================================================================

#undef DllImportExport

#endif

