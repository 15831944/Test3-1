//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __CTank_H
#define  __CTank_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __M_SURGE_H
  #include "m_surge.h"
#endif

#ifdef __CTank_CPP
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

const long MaxLvlLookupCnt = 200;
XID xidLvl_Lookup         = MdlBsXID(22001);
XID xidLvl_Count          = MdlBsXID(22002);
XID xidLvl_Sort           = MdlBsXID(22003);
XID xidLvl_FirstHeight    = MdlBsXID(22010);
XID xidLvl_FirstArea      = xidLvl_FirstHeight+MaxLvlLookupCnt;
XID xidLvl_FirstVol       = xidLvl_FirstArea+MaxLvlLookupCnt;

class DllImportExport CVolumeShapeLookup
  {
  public:
    CVolumeShapeLookup();
    void BuildDataDefn(TaggedObject* pTagObj, DataDefnBlk & DDB);
    flag DataXchg(TaggedObject* pTagObj, DataChangeBlk & DCB);

    void SetCount(long NewCount);
    void SortAndValidate();
    long Count() { return iCount; };
    bool Active() { return bActive; };
    void Calculate(double Volume);

  public:
    bool bActive;
    bool bCheckRqd;
    flag bWithSurfAreaLookup;
    long iCount;
    double ALvl;
    double AHeight;
    double AArea;
    double Vol;
    CArray<double, double> m_Heights;
    CArray<double, double> m_Areas;
    CArray<double, double> m_Volumes;
  };

//=========================================================================== 
DEFINE_TAGOBJ(CTank);

//=========================================================================== 

/*Basic Mixed CTank - Multiple Inputs and Outputs*/
class DllImportExport CTank : public MN_Surge
  {
  public:
    CTank(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CTank (){};
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual void   EvalProducts(CNodeEvalIndex & NEI);
    virtual void   EvalDerivs(CNodeEvalIndex & NEI);

  private:
    CVolumeShapeLookup LvlLookup;
  };

//=========================================================================== 

#undef DllImportExport

#endif


