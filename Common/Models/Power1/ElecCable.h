//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ELECCABLE_H
#define  __ELECCABLE_H

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

#ifdef __ELECCABLE_CPP
  #define DllImportExport 
#elif !defined(Power1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//--------------------------------------------------------------------------

DEFINE_TAGOBJ(CElecCable);
class CElecCable : public FlwNode
  {
  public:
    double           m_CabLength;
    eCondMaterial    m_eCondMat;
    double           m_kW;
    double           m_kVA;
    double           m_CabV;

    CArray <CEWire, CEWire&> m_Wires;

    CElecCable(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~CElecCable();
    virtual void   ResetData(flag Complete);
  public:

    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg) { if (NoEIOs()<2) ID=Tg=FullObjTag(); else Ctrl_GetConnIDStr(OtherEnd(i), ID, Tg); };

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual int    GetLinkWiring(CLinkWiring &LinkWiring);
    virtual int    SetLinkWiring(CLinkWiring &LinkWiring);
    virtual int    GetWireCount()                         { return m_Wires.GetCount();  };
    virtual CEWire * GetWire(int Index)                   { return &m_Wires[Index]; }
    virtual void   ClearWires();
    virtual bool   SetWire(int iWire, LPCTSTR sTag,
                           LPCTSTR sSrcTermStripName, LPCTSTR sSrcTerminalName,
                           LPCTSTR sDstTermStripName, LPCTSTR sDstTerminalName);

    virtual void   OnElecComponentChange(eElecChanges EC, CECompData *pC); // CTNode Overide

    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);

    virtual void   CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void   ConnectElectrics();

    virtual void   EvalDiscrete();

    virtual flag   GetModelAction(MdlActionArray & Acts);
    virtual flag   SetModelAction(MdlAction & Act);
    virtual dword  ModelStatus();
    
    DEFINE_CI(CElecCable, FlwNode, 4);
  };
  
//===========================================================================

#undef DllImportExport

#endif

