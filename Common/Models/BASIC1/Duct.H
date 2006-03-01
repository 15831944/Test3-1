//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DUCT_H
#define  __DUCT_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_LINK_H
  #include "m_link.h"
#endif

#ifdef __DUCT_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

DEFINE_TAGOBJ(Duct);
/*#C:
Basic Duct.
*/
class DllImportExport Duct : public MN_Lnk
  {
  public:

    Duct(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Duct(){};
    virtual void   BuildDataDefn(DataDefnBlk &DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);
  private:
  };

// ===========================================================================

DEFINE_TAGOBJ(DuctBF);
class DuctBF : public CFlwEqn
  {
  public:
    double         Diam_Pipe;
    double         KvActual;
    double         KvMax;

    DuctBF(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~DuctBF();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================

#undef DllImportExport

#endif


