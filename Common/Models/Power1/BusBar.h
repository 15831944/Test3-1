//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __BUSBAR_H
#define  __BUSBAR_H

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

#ifdef __BUSBAR_CPP
  #define DllImportExport 
#elif !defined(Power1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

class CBusBarSeg 
  {
  public:
    //CETermStrip* m_pTS;
    double m_dLen;
    CECompDataArray m_Connectors;
//    CArray<CECompData, CECompData&> m_Conns;

    CBusBarSeg()
      {
      m_dLen=1;
      }
    ~CBusBarSeg()
      {
      }
  };

DEFINE_TAGOBJ(CBusBar);
class CBusBar : public FlwNode
  {
  public:
    //flag           m_bPresent;                 
    CString        m_sNStr;
    double         m_dArea;
    eCondMaterial  m_eCondMat;
    CArray <CETermStrip*, CETermStrip*> m_Drop;
    CArray <CBusBarSeg, CBusBarSeg> m_Seg;

    CBusBar(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CBusBar();
    virtual void   ResetData(flag Complete);
  public:

    LPTSTR         GetPrimaryCfg(); 
    flag           PutPrimaryCfg(LPCTSTR Cfg); 
    void           SetSegCount(long N);
    long           GetSegCount();
    long           GetDropCount();

    //virtual void   PostConnect(int IONo);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   OnElecComponentChange(eElecChanges EC, CECompData *pC);
    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();

    virtual flag   GetModelAction(CMdlActionArray & Acts);
    virtual flag   SetModelAction(CMdlAction & Act);
    virtual dword  ModelStatus();
    
    DEFINE_CI(CBusBar, FlwNode, 4);
  };
  
//===========================================================================

#undef DllImportExport

#endif

