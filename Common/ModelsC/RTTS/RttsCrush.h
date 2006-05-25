//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __RTDCRUSH_H
#define  __RTDCRUSH_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  

#ifdef __RTDCRUSH_CPP
  #define DllImportExport DllExport
#elif !defined(RTTS)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================
//
//
//
// ===========================================================================

class Model400Helper
  {
  friend class RTCrusher;
  public:
    Model400Helper();
    ~Model400Helper();
  protected:
    int SetNumComps(int NewSize);
    void BuildDataDefn(DataDefnBlk & DDB, pTaggedObject This, flag Vis);
    flag DataXchg(DataChangeBlk & DCB);
    void RecalcT10();
    BOOL CrushProduct(SpConduit & QFd, SpConduit & Disch);
    BOOL CrushProductOne(SpConduit & QFd, SpConduit & Disch);
    void DemoTest();
  private:
    double* feedData;
    double* sizeSeries;
    int     PrevNumSizes;
    long    iNumComps;
    double* pT10;

    //see Model400 Cone crusher model (osm_crush.h)...
    double K1;
    double K2;
    double K3;
    //double T10;
    double M0;
    double M1;
    double M2;
    double M3;
    double ECS;
  };

// ===========================================================================

DEFINE_TAGOBJ(RTCrusher);
class RTCrusher : public MdlNode
  {
  //friend class Model400Helper;
  public:
    flag           bOnLine;
    long           iNumComps;
    byte           iCrushModel;
    Model400Helper M400;
    SpConduit      Disch;
    CrushBlk       CB;

    RTCrusher(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~RTCrusher();
    int            SetNumComps(int NewSize);

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   PreStartCheck();

    virtual void   EvalProducts(long JoinMask);
    virtual void   EvalCtrlActions(eScdCtrlTasks Tasks=CO_All);
    virtual void   ConfigureJoins();

    virtual dword  ModelStatus();

  private:
    DEFINE_CI(RTCrusher, MdlNode, 4);

  };

// ===========================================================================

#undef DllImportExport

#endif
