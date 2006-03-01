//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CRUSHER_H
#define  __CRUSHER_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#define MDLBASE
//#define BASIC1
#define SIZEDST1
#include "models.h"

#ifdef __CRUSHER_CPP
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

const byte CM_PartCrv  = 0;
const byte CM_SelBreak = 1;

DEFINE_TAGOBJ(Crusher);
class Crusher : public MdlNode
  {
  public:
    double         Power, BWI;//, BWI_F, BndSmlFrac, BndSmlSize, HoldUpMass;
    C2DFn          BWDist, EDist, DischPart;//, DischSize;
    SpConduit      Disch;

    byte           iMethod;

    SzSelBrk1      SelBrk;

    CIO_MtrSpdBlk  MSB;
    CPwrUser       m_Pwr;


    Crusher (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Crusher();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   InitialiseSolution();
    virtual Z_States Set_Zs(int Pass, int IOIn, double Zi, Z_States Z_State_Src);
    virtual flag   Set_Sizes();
    virtual void   ConfigureJoins();
    virtual void   EvalProducts(long JoinMask);

    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg);
    virtual void   EvalCtrlActions();
    virtual void   EvalPowerRequired();
    
  private:

  };

// ===========================================================================

#undef DllImportExport

#endif
