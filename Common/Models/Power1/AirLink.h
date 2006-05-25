//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __AIRLINK_H
#define  __AIRLINK_H

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

#ifdef __AIRLINK_CPP
  #define DllImportExport 
#elif !defined(Power1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//--------------------------------------------------------------------------


DEFINE_TAGOBJ(CAirLink);
class CAirLink : public FlwNode
  {
  public:
    flag           m_bOn;                  //
    //double         m_dValue;

    CAirLink(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CAirLink();
    virtual void   ResetData(flag Complete);
  public:

    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg) { if (NoAIOs()<2) ID=Tg=FullObjTag(); else Ctrl_GetConnIDStr(OtherEnd(i), ID, Tg); };

    //virtual void   PostConnect(int IONo);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    virtual void   EvalDiscrete();

    virtual flag   GetModelAction(Strng & Tag, MdlActionArray & Acts);
    virtual flag   SetModelAction(Strng & Tag, MdlAction & Act);
    virtual dword  ModelStatus();
    
    DEFINE_CI(CAirLink, FlwNode, 4);
  };
  
//===========================================================================

#undef DllImportExport

#endif

