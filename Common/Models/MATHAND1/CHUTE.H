//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CHUTE_H
#define  __CHUTE_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  
#ifndef __M_XFER_H
  #include "m_xfer.h"
#endif  
#ifndef __VLVLOGIC_H
  #include "vlvlogic.h"
#endif  
#ifndef __ELECTRICS_H
  #include "electrics.h"
#endif  

#ifdef __CHUTE_CPP
  #define DllImportExport DllExport
#elif !defined(Mathand1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//=========================================================================== 

DEFINE_TAGOBJ(Chute);
class Chute : public MdlNode
  {
  public:
    SpConduit      QFd;

    CArray <CIO_VlvPosBlk*, CIO_VlvPosBlk*> VPB;
    CSVector       VPBTag;
    CDVector       Split;
    CPwrUser       m_Pwr;

    Chute(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~Chute();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   PostConnect(int IONo);
    virtual void   PreDisConnect(int IONo);

    virtual void   ConfigureJoins();
    virtual void   EvalJoinPressures(long JoinMask);

    virtual void   EvalProducts(long JoinMask);
    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    virtual dword  ModelStatus();
  private:
  };

//=========================================================================== 

#undef DllImportExport

#endif
