//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SOCKET_H
#define  __SOCKET_H

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

#ifdef __SOCKET_CPP
  #define DllImportExport 
#elif !defined(Power1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//--------------------------------------------------------------------------


DEFINE_TAGOBJ(CSocket);
class CSocket : public FlwNode
  {
  public:
    CSocket(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CSocket();
    virtual void   ResetData(flag Complete);

  public:
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);

    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();
    virtual void   EvalPowerRequired();
    
    virtual flag   GetModelAction(CMdlActionArray & Acts);
    virtual flag   SetModelAction(CMdlAction & Act);
    virtual dword  ModelStatus();
    
    DEFINE_CI(CSocket, FlwNode, 4);

  public:
    bool           m_bPresent;
    CElecSupply    m_Supply;
    Strng          m_sTargetTag;

    CPwrUserSocket m_Socket;

    CETermStrip    m_tsPwrIn;
    CETermStrip    m_tsPwrOut;

    double         m_dRqdPower;
    double         m_dRqdPwrFct;

  };
  
//===========================================================================

#undef DllImportExport

#endif

