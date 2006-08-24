//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __INCOMER_H
#define  __INCOMER_H

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

#ifdef __INCOMER_CPP
  #define DllImportExport 
#elif !defined(Power1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//--------------------------------------------------------------------------


DEFINE_TAGOBJ(CIncomer);
class CIncomer : public FlwNode
  {
  public:
    CIncomer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CIncomer();
    virtual void   ResetData(flag Complete);
  public:

    //virtual char * EIO_ConnIDStr(int i) { if (NoElecIOs()<2) return FullObjTag(); return EIO_GetConnIDStr(OtherEnd(i)); };
    //virtual char * AIO_ConnIDStr(int i) { if (NoAirIOs()<2) return FullObjTag(); return AIO_GetConnIDStr(OtherEnd(i)); };

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual long   NodeFlwTask(NodeFlwTasks Task);

    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual void   GlobalConnect();
    virtual void   GlobalDisConnect();
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);

    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();
    virtual void   EvalDiscrete();
    
    virtual flag   GetModelAction(CMdlActionArray & Acts);
    virtual flag   SetModelAction(CMdlAction & Act);
    virtual dword  ModelStatus();
    
    DEFINE_CI(CIncomer, FlwNode, 4);

  protected:
    Strng          sConnectTag;
    Strng          sConnectedTag;
    bool           fConnectionOn;
    bool           fConnectInitiator;
    bool           fCrossConnected;
    bool           fGlblConnectsInPlace;
    bool           m_bIAmOut;

    CIncomer      *pConnectedTo;
    int            iConnectedIO;

  public:
    bool           m_bOn;
    bool           m_bPresent;
    //eTSConfigurations m_nType;
    double         m_dVolts;
    double         m_dXfrPower;
    double         m_dXfrPwrFct;

    CETermStrip    m_tsPwrIn;
    CETermStrip    m_tsPwrOut;

  };
  
//===========================================================================

#undef DllImportExport

#endif

