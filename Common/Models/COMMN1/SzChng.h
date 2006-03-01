//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SZCHG_H
#define  __SZCHG_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#define MDLBASE
#define SIZEDST1
#include "models.h"

#ifdef __SZCHNG_CPP
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

DEFINE_TAGOBJ(SzChng);
class DllImportExport SzChng : public MdlNode
  {
  protected:
    flag           bOnLine;
    long           iDistRqd;

  public:
    SzChng (pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SzChng();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   InitialiseSolution();
    virtual Z_States Set_Zs(int Pass, int IOIn, double Zi, Z_States Z_State_Src);
    virtual flag   Set_Sizes();
    virtual void   ConfigureJoins();
    virtual void   EvalProducts(long JoinMask);
    
    virtual dword  ModelStatus();
  private:
    //DEFINE_CI(SzChng, MdlNode, 3);

  };

// ===========================================================================

#undef DllImportExport

#endif
