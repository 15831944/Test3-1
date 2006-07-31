//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CRUSH1_H
#define  __CRUSH1_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#define MDLBASE
#define SIZEDST1
#include "models.h"

#ifdef __CRUSH1_CPP
  #define DllImportExport DllExport
#elif !defined(Commn1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//
//
// ===========================================================================

const byte CM_PartCrv_Individ  = 0;
const byte CM_PartCrv_Overall  = 1;
const byte CM_SelBreak         = 2;
const byte CM_TestData_Individ = 3;
const byte CM_TestData_Overall = 4;
const byte CM_JK               = 5;

//=========================================================================== 

DEFINE_TAGOBJ(CrushBlk);
class DllImportExport CrushBlk : public TaggedObject
  {
  protected:
    SzPartCrv1     DischParts;
    SzPartCrv1     DataParts;
    SzPartCrv1     Extents;
    SzSelBrk1      SelBrk;

    byte           iMethod;
    flag           bForcePartCrv;
    flag           bEnterExtents;
    flag           bTrackStatus;
    double         dK1, dK2, dK3, dt10; //used by JK model

  public:
    CrushBlk(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CrushBlk();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual pchar  FullObjTag(flag What=TOA_All)
      {
      m_sFullObjTag=BaseTag();
      return FullObjTag();
      }

    void Add_StandardDataDefn(DataDefnBlk & DDB, flag Vis);
    void Add_ObjectDataDefn(DataDefnBlk & DDB, flag Vis);

    flag IsValidData(SpConduit & QFd);
    void CrushProduct(SpConduit & QFd, SpConduit & Disch);

  protected:
    void CalcExtents();

  private:
    DEFINE_CI(CrushBlk, TaggedObject, 3);

  };

//=========================================================================== 
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(Crush1);
class DllImportExport Crush1 : public MdlNode
  {
  protected:
    SpConduit      Disch;
    CIO_MtrSpdBlk  MSB;
    CrushBlk       CB;
    CPwrUser       m_Pwr;
    flag           bOnLine;
    double         DischOnSpeed;
    double         Power;
    double         BWI;

  public:
    Crush1 (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Crush1();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   InitialiseSolution();
    virtual void   SetDatums(int Pass, CFlwNodeIndexList & List, int IOIn);
    virtual flag   Set_Sizes();
    virtual void   ConfigureJoins();

    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg);
    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    virtual void   EvalPowerRequired();
    
    virtual dword  ModelStatus();
  private:
    //DEFINE_CI(Crush1, MdlNode, 3);

  };

// ===========================================================================

#undef DllImportExport

#endif
