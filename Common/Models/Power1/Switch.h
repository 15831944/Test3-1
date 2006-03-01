//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SWITCH_H
#define  __SWITCH_H

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

#ifdef __SWITCH_CPP
  #define DllImportExport 
#elif !defined(Power1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

DEFINE_TAGOBJ(CIsolator);
class CIsolator : public FlwNode
  {
  public:
    flag           m_bOn;  
    CETermStrip    m_tsPwrIn;
    CETermStrip    m_tsPwrOut;
    //CArray<CEComponent*,CEComponent*> m_Component;

    CIsolator(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CIsolator();
    virtual void   ResetData(flag Complete);
  public:

    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg) { if (NoEIOs()<2) ID=Tg=FullObjTag(); else Ctrl_GetConnIDStr(OtherEnd(i), ID, Tg); };

    //virtual void   PostConnect(int IONo);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();

    virtual flag   GetModelAction(Strng & Tag, MdlActionArray & Acts);
    virtual flag   SetModelAction(Strng & Tag, MdlAction & Act);
    virtual dword  ModelStatus();
    
    DEFINE_CI(CIsolator, FlwNode, 4);
  };
  
//===========================================================================

DEFINE_TAGOBJ(CCctBreaker);
class CCctBreaker : public FlwNode
  {
  public:
    flag           m_bOn;  
    CETermStrip    m_tsPwrIn;
    CETermStrip    m_tsPwrOut;
    //CArray<CEComponent*,CEComponent*> m_Component;

    CCctBreaker(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CCctBreaker();
    virtual void   ResetData(flag Complete);
  public:

    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg) { if (NoEIOs()<2) ID=Tg=FullObjTag(); else Ctrl_GetConnIDStr(OtherEnd(i), ID, Tg); };

    //virtual void   PostConnect(int IONo);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();

    virtual flag   GetModelAction(Strng & Tag, MdlActionArray & Acts);
    virtual flag   SetModelAction(Strng & Tag, MdlAction & Act);
    virtual dword  ModelStatus();
    
    DEFINE_CI(CCctBreaker, FlwNode, 4);
  };
  
//===========================================================================

#undef DllImportExport

#endif

