//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __TRANSFORMER_H
#define  __TRANSFORMER_H

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

#ifdef __TRANSFORMER_CPP
  #define DllImportExport 
#elif !defined(Power1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//--------------------------------------------------------------------------


DEFINE_TAGOBJ(CTransformer);
class CTransformer : public FlwNode
  {
  public:
    CETermStrip      m_tsPri;
    CETermStrip      m_tsSec;
    CETermStrip      m_tsLcl;
    double           m_dPriV, m_dSecV, m_dNLI, m_dNLA;
    CDComplex        m_PriZ, m_SecZ, m_NoLoadZ;
    CDComplex        m_V1, m_V2, m_V3;
    CDComplex        m_Z1, m_Z2, m_Z3;
    CECompDataArray  m_PriConnectors;
    CECompDataArray  m_SecConnectors;

    CTransformer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CTransformer();
    virtual void   ResetData(flag Complete);
  public:

    //virtual char * EIO_ConnIDStr(int i) { if (NoElecIOs()<2) return FullObjTag(); return EIO_GetConnIDStr(OtherEnd(i)); };
    //virtual char * AIO_ConnIDStr(int i) { if (NoAirIOs()<2) return FullObjTag(); return AIO_GetConnIDStr(OtherEnd(i)); };

    //virtual void   PostConnect(int IONo);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();

    virtual flag   GetModelAction(CMdlActionArray & Acts);
    virtual flag   SetModelAction(CMdlAction & Act);
    virtual dword  ModelStatus();
    
    DEFINE_CI(CTransformer, FlwNode, 4);
  };
  
//===========================================================================

#undef DllImportExport

#endif

