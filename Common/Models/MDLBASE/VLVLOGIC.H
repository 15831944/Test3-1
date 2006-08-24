//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __VLVLOGIC_H
#define  __VLVLOGIC_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __CTRL_IO_H
  #include "ctrl_io.h"
#endif
#ifndef __ELECTRICS_H
  #include "electrics.h"
#endif

#ifdef __VLVLOGIC_CPP
  #define DllImportExport DllExport
#elif !defined(MdlBase)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//
//
// ===========================================================================

//DEFINE_TAGOBJ(VL_Base)
class VL_Base : public CIO_Logic
  {
  public:
    VL_Base(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~VL_Base();

    virtual void   BuildDataDefn(DataDefnBlk & DDB)=0;
    virtual flag   DataXchg(DataChangeBlk & DCB)=0;
    virtual flag   CopyFrom(CIO_Logic * pOther);
 
    virtual bool   On()=0;
    virtual void   SetOn(bool On)=0;                  
    virtual void   SetManualPosition(double P)     { dManualPosition=P; };    
    virtual void   SetReqdPosition(double P)       { dReqdPosition=P; };      
    virtual void   SetActualPosition(double Posn) { dPosition=Posn; };
    virtual double ManualPosition(FlwNode* pFNode) { return dManualPosition; };
    virtual double ReqdPosition(FlwNode* pFNode)   { return dReqdPosition; };  
    virtual double ActualPosition(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL)=0;   
    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg);

  public:
    bool           m_bOn;
    double         dPosition;
    double         dReqdPosition;
    double         dManualPosition;
    float          dMinOpening;
    float          dMaxOpening;
    float          dLeakOpening;
    double         dPhysPosn;
    bool           bReverseAction;
  };

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(VL_Basic)
class VL_Basic : public VL_Base
  {
  public:
    VL_Basic(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~VL_Basic();

    //virtual void   Initialise(pTaggedObject pAttach);
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(FlwNode* pFNode) {};
    virtual void   EvalCtrlActions(FlwNode* pNd);
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};
    virtual double ActualPosition(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL);
    virtual void   SetActualPosition(double Posn);
    virtual bool   On();
    virtual void   SetOn(bool On);

    virtual void   SetManualPosition(double P) { dManualPosition=P; };    
    virtual void   SetReqdPosition(double P)   { dReqdPosition=P; };      
    virtual double ManualPosition(FlwNode* pFNode) { return dManualPosition; };
    virtual double ReqdPosition(FlwNode* pFNode)   { return dReqdPosition; };  

  public:
    bool           bNormClosed;
    bool           bOpenCmd;
    bool           bCloseCmd;
    bool           bLatchCmds;
    long           iLatchState;
    bool           bOpened;
    bool           bClosed;

  };

// ---------------------------------------------------------------------------

#define IMPLEMENT_VLVPOSLOGIC(Md, Name, Version, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(Md, "VlvPos", Name, Version, "", "", Cat, ShortDesc, Desc);

// ---------------------------------------------------------------------------

_FWDDEF(CIO_VlvPosBlk)
class DllImportExport CIO_VlvPosBlk : public CIO_Blk
  {
  public:
    CIO_VlvPosBlk(pchar Name, pTaggedObject pAttach, CPwrUser * pPwr):
      CIO_Blk("VlvPos", Name, pAttach, pPwr){ };
    virtual ~CIO_VlvPosBlk() {};

    bool           On() { return ((VL_Base*)pLogic)->On();};
    void           SetOn(bool On) { ((VL_Base*)pLogic)->SetOn(On); };
    void           SetManualPosition(double P) { ((VL_Base*)pLogic)->SetManualPosition(P); };
    void           SetReqdPosition(double P)   { ((VL_Base*)pLogic)->SetReqdPosition(P); };
    void           SetActualPosition(double P) { ((VL_Base*)pLogic)->SetActualPosition(P); };
    double         ManualPosition(FlwNode* pFNode) { return ((VL_Base*)pLogic)->ManualPosition(pFNode); };
    double         ReqdPosition(FlwNode* pFNode) { return ((VL_Base*)pLogic)->ReqdPosition(pFNode); };
    double         ActualPosition(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL) { return ((VL_Base*)pLogic)->ActualPosition(pFNode, pFRB); };
    void           Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg) { ((VL_Base*)pLogic)->Ctrl_ConnIDStr(i, ID, Tg); };
  };

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(VL_Stroke)
class VL_Stroke : public VL_Basic
  {
  public:
    double         dStrokeTime;

    VL_Stroke(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~VL_Stroke();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(pFlwNode pFNode) {};
    virtual void   EvalCtrlActions(FlwNode* pNd);
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};
    virtual double ActualPosition(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL);
  };

// ===========================================================================


DEFINE_TAGOBJ(VL_StrokeOC)
class VL_StrokeOC : public VL_Base
  {
  public:
    VL_StrokeOC(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~VL_StrokeOC();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual void   EvalDiscrete(FlwNode* pFNode) {};
    virtual void   EvalCtrlActions(FlwNode* pNd);
    virtual void   EvalCtrlStrategy(FlwNode* pNd) {};

    virtual bool   On();
    virtual void   SetOn(bool On);                  
    virtual double ActualPosition(FlwNode* pFNode, CFlwRegBlk * pFRB=NULL);   

  public:
    bool           m_bActive;
    bool           bActMemory;
    bool           bNormClosed;
    bool           bOpenCmd;
    bool           bCloseCmd;
    bool           bLoLimit;
    bool           bHiLimit;

    byte           iMode;
    bool           bOpenInv;
    bool           bCloseInv;
    double         dOpenTime;
    double         dCloseTime;
    double         dMapLo;
    double         dMapHi;
    double         dMapPosn;
    float          dLoLimit;
    float          dHiLimit;
  };

// ===========================================================================

#undef DllImportExport

#endif
