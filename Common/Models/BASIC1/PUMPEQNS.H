//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __PUMPEQNS_H
#define  __PUMPEQNS_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif

#ifndef __FIXEDIT_H
  #include "fixedit.h"
#endif
#ifndef __GPWNDS_H
  #include "gpwnds.h"
#endif
#ifndef __DATAATTR_H
  #include "dataattr.h"
#endif
#ifndef __SCDARRAY_H
  #include "scdarray.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif
#ifndef __FNBASIC1_H
  #include "fnbasic1.h"
#endif
#ifndef __DBHELPER_H
  #include "dbhelper.h"
#endif

#if defined(__PUMPEQNS_CPP)
  #define DllImportExport DllExport
#elif !defined(Basic1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif
// ===========================================================================

DEFINE_TAGOBJ(PC_FixQm);
class DllImportExport PC_FixQm : public CFlwEqn
  {
  public:
    PC_FixQm(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PC_FixQm();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual void   StepFlowInfo(CFlwBlkBase & FE);
    virtual double ShaftPower() { return m_dShaftPower; };

  protected:
    double         dReqd_Qm;
    double         m_dShaftPower;
    double         m_dEfficiency;
    double         dMaxPress;
    double         dMaxPSlew;
    double         dMaxNdPress;
  };

// ===========================================================================

DEFINE_TAGOBJ(PC_FixQv);
class DllImportExport PC_FixQv : public CFlwEqn
  {
  public:
    PC_FixQv(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PC_FixQv();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual void   StepFlowInfo(CFlwBlkBase & FE);
    virtual double ShaftPower() { return m_dShaftPower; };

  protected:
    flag           m_bAtInlet;
    double         dReqd_Qv;
    double         m_dShaftPower;
    double         m_dEfficiency;
    double         dMaxPress;
    double         dMaxPSlew;
    double         dMaxNdPress;
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(PC_FixBst);
class DllImportExport PC_FixBst : public CFlwEqn
  {
  public:
    PC_FixBst(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PC_FixBst();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual double ShaftPower() { return m_dShaftPower; };

    DEFINE_CI(PC_FixBst, CFlwEqn, 4);  

  protected:
    double          m_P_Boost;
    double          m_dShaftPower;
    double          m_dEfficiency;
    byte            m_bLoSpeedAction;
    double          m_DP;
    CFrictionFactor m_FF;
  };

// ===========================================================================

DEFINE_TAGOBJ(PC_FixHd);
class DllImportExport PC_FixHd : public CFlwEqn
  {
  public:
    PC_FixHd(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PC_FixHd();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual double ShaftPower() { return m_dShaftPower; };

    DEFINE_CI(PC_FixHd, CFlwEqn, 4);  

  protected:
    double          m_P_Head;
    double          m_dShaftPower;
    double          m_dEfficiency;
    byte            m_bLoSpeedAction;
    double          m_dPrimeRho;
    double          m_DP;
    CFrictionFactor m_FF;
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(PC_SmplCrvBst);
class DllImportExport PC_SmplCrvBst : public CFlwEqn
  {
  public:
    PC_SmplCrvBst(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PC_SmplCrvBst();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual double ShaftPower() { return m_dShaftPower; };

    DEFINE_CI(PC_SmplCrvBst, CFlwEqn, 4);  

  protected:
    double         m_dBstMx;
    double         m_dBstOp;
    double         m_dQvOp;
    double         m_dQvMx;
    double         m_dQmOp;
    double         m_dQmMx;
    double         m_dPrimeRho;
    double         m_dShaftPower, m_dEfficiency;
    byte           bLoSpeedAction;
    flag           m_bQmOp;
  };

// ===========================================================================
// ===========================================================================

DEFINE_TAGOBJ(PC_SmplCrvHead);
class DllImportExport PC_SmplCrvHead : public CFlwEqn
  {
  public:
    PC_SmplCrvHead(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PC_SmplCrvHead();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual double ShaftPower() { return m_dShaftPower; };

    DEFINE_CI(PC_SmplCrvHead, CFlwEqn, 4);  

  protected:
    double         m_dHeadMx;
    double         m_dHeadOp;
    double         m_dQvOp;
    double         m_dQvMx;
    double         m_dQmOp;
    double         m_dQmMx;
    double         m_dPrimeRho;
    double         m_dShaftPower, m_dEfficiency;
    byte           bLoSpeedAction;
    flag           m_bQmOp;
  };

// ===========================================================================
// NOT YET
//DEFINE_TAGOBJ(PC_SmplCrvHd);
class DllImportExport PC_SmplCrvHd : public CFlwEqn
  {
  public:
    PC_SmplCrvHd(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PC_SmplCrvHd();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);
    virtual double ShaftPower() { return m_dShaftPower; };

  protected:
    double         HdMx;
    double         HdOp;
    double         QvOp;
    double         QvMx;
    double         m_dShaftPower, m_dEfficiency;
    byte           bLoSpeedAction;
    double         dPrimeRho;
  };

//===========================================================================
//
//
//
//===========================================================================

#define WITHPWRCRV  01

enum PumpHeadTypes {PHT_Null, PHT_Head, PHT_Press, PHT_Isentropic, PHT_Adiabatic };

class DllImportExport CPumpDataSet
  {
  public:

    CFn2_Base*     m_pHdEqn;
    CFn2_Base*     m_pEffEqn;
#if WITHPWRCRV
    CFn2_Base*     m_pPwrEqn;
#endif

    double         m_dSurgePt;

    Strng          m_RegName;
    double         m_RegMin, m_RegMax;
    CCnvIndex      m_iRegCnv;
    Strng          m_sRegCnvTxt;

    PumpHeadTypes  iHdType;
    CCnvIndex      iHdCnv;
    Strng          sHdCnvTxt;

    CPumpDataSet();
    ~CPumpDataSet();
    bool SetUp(CStringArray & Values/*char **C*/, char * PumpDBSpec);
    PumpHeadTypes  HeadType() { return iHdType; };
  };

//typedef CSmartPtrAllocate<PumpDataSet> CSPAPumpDataSet;
class DllImportExport CPumpDataSets : public CArray < CPumpDataSet*, CPumpDataSet*> {};

// ===========================================================================

class DllImportExport PC_DBase
  {
  protected:
    int            LnNo;
    //FILE           *PumpDBFile;
    CDBHelper      m_TheDB;


    flag           bCrvOK;

    CPumpDataSets   m_DS;
    int            iCurDS;
    PumpHeadTypes  iHdType;

  public:

    Strng          m_DBName;
    Strng          m_DBSheet;
    Strng          PumpDBSpec;
    Strng          m_RegName;
    double         m_RegMin, m_RegMax;
    CCnvIndex      m_iRegCnv;
    Strng          m_sRegCnvTxt;
    bool           m_EffDefined;
    bool           m_PwrDefined;

    PC_DBase(char * pDBName, char * pDBSheet);

    virtual ~PC_DBase();

    static const int MaxPumpDBCols;

    Strng_List     PumpNameList;

    flag           OK() { return bCrvOK; };
    void           InitPumpNameList();
    flag           OpenPumpDataBase(int DBNo);
    void           ClosePumpDataBase();
    //flag           VerifyTable(char ** Cols);
    flag           ParseTokens(CStringArray & Values);//char *Buff, int BuffSize, char ** Cols);
    flag           SetPumpTo(char *RqdSpec);
    flag           AddDataSet(CStringArray & Values);//char **C);
    void           FindCurrentCurve(double OpReg);
    int            NDataSets() { return m_DS.GetSize(); };
    CPumpDataSet &  DataSet(int s) { return *m_DS[s]; };
    double         LoVolFlw(double OpReg);
    double         HiVolFlw(double OpReg);
    double         HeadAt(double Qv, double OpReg);
    PumpHeadTypes  HeadType();
    flag           EffAtHead(double Qv, double OpReg, double &Eff);
#if WITHPWRCRV
    flag           PwrAtHead(double Qv, double OpReg, double &Pwr);
#endif
    double         SG(double Rho) { return (Rho > 0.1) ? Rho/1000.0 : 1.0; };
    bool           EffDefined() { return m_EffDefined; }
    bool           PwrDefined() { return m_PwrDefined; }

  };

// ===========================================================================

XID xidPumpDBSpec  = MdlBsXID(15000);
XID xidPumpDBMnSpd = MdlBsXID(15001);
XID xidPumpDBMxSpd = MdlBsXID(15002);

#define WithPCEdt 0
#if WithPCEdt
  DEFINE_TAGOBJEDT(PC_DataBase);
#else
  DEFINE_TAGOBJ(PC_DataBase);
#endif
class DllImportExport PC_DataBase : public CFlwEqn
  {
  public:
    PC_DBase       m_PDB;
    double         m_dShaftPower, m_dEfficiency;

    double         m_Head, m_HdDerating, m_FlwDerating;
    double         m_OpReg, m_PrvOpReg, m_dMinRho;
    flag           fWasStepStart;

    byte           LoSpeedAction;

    double         dDamp;

    DDBValueLstMem DDBPumpList;

    PC_DataBase(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~PC_DataBase();
//    virtual flag   BuildDataDefnAsObject() { return False; };
//    virtual char * BuildDataDefnAsObjTag() { return "PDB"; };
//    virtual char * BuildDataDefnAsObjClass() { return "PC_DBase"; };
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
    virtual void   EvalProducts(CFlwBlkBase & FE, double Pi, double Po, CFlwThermalBlk & FTB);

    virtual double ShaftPower() { return m_dShaftPower; };

  };

// ===========================================================================
//
//
//
// ===========================================================================
#if WithPCEdt

//class DllImportExport PC_Column
//  {
//  public:
//    Strng  sName;
//    flag   fOn;
//    //int    iSpId, iDataId;
//    double dDispMin, dDispMax;
//
//    PC_Column()
//      {
//      fOn=True;
//      //iSpId=0;
//      //iDataId=0;
//      dDispMin=0.0;
//      dCDispMax=1.0;
//      dFDispMax=1.0;
//      };
//  };
//class DllImportExport PC_ColumnArray : public CArray <PC_Column, PC_Column&> {};
//

/**/
_FWDDEF(PC_DataBaseEdt);
class DllImportExport PC_DataBaseEdt : public FxdEdtBookRef
  {
  public :
    //rPC_DataBase   rPC;
    PC_DBase *     pPDB;
    CRect          GraphR;
    double         XMin;       //normalised x min (lhs of graph)
    double         XMax;       //normalised x max (rhs of graph)
    double         XFactor;    // = XPix/(XMax-XMin)
    double         YMin[3];    //normalised y min (bottom of graph)
    double         YMax[3];    //normalised y max (top of graph)
    double         YFactor[3]; // = YPix/(YMax-YMin)
    int            iDragPtNo;  //the index of the point being moved (-1 if none)
    int            iNameWidth; //
    //int            iWorkDist;
    int            iPg1;
    int            fAutoScale;
    CPoint         CursPoint;

    CnvAttribute   XCnv, YCnv[3];//, *pWrkCnv;
    FmtAttribute   XFmt, YFmt[3];//, *pWrkFmt;
    Strng          XName, YName[3];

    CLabelWnd*     pLabels;    //
    static const int XPix;     //number of x pixels
    static const int YPix;     //number of x pixels
    static const int CrvPts;
    static int iGraphWidth;
    static int iGraphHeight;

    double dXDispMin, dYDispMin[3];//, dXLoLimit;
    double dXDispMax, dYDispMax[3];//, dXUpLimit;

//    PC_ColumnArray     Columns;



    //flag           fXtraPg;

    //void SetWorkDist(int i) { iWorkDist=i; };
    int GraphWidth()  { return iGraphWidth; };
    int GraphHeight() { return iGraphHeight; };
    //PC_DataBase & PC() { return rPC; };
    //PC_DBase &    PDB() { return rPC.PDB; };

    //CSD_Distribution & D() { return *rSD.Distributions[iWorkDist]; };
    //CSD_DistDefn & DD() { return D().rDefn; };
    //int Length() { return D().NIntervals(); };
    int DistFromPgNo(int Pg);

    flag   AutoScale() { return fAutoScale; };
    void   SetAutoScale(flag On) { fAutoScale=On; };

    //flag   CumulativeOn() { return DD().fCumulativeOn; };
    //void   SetCumulativeOn(flag On) { DD().fCumulativeOn=On; };

    //flag   FractionalOn() { return DD().fFractionalOn; };
    //void   SetFractionalOn(flag On) { DD().fFractionalOn=On; };

    //flag   DispRetained() { return DD().fDispRetained; };
    //void   SetDispRetained(flag On) { DD().fDispRetained=On; };

    //flag   XLog() { return DD().fXLog; };
    //flag   YLog() { return DD().fYLog; };
    //void   SetXLog(flag On) { DD().fXLog=On; };
    //void   SetYLog(flag On) { DD().fYLog=On; };

    //void   AllocDist(int d) { rSD.AllocDist(d); };
    //void   FreeDist(int d) { rSD.FreeDist(d); };
    //int    CurView() { return rSD.iCurView; };
    //void   SetCurView(int iV) { rSD.iCurView=iV; };

    double XDispMin() { return dXDispMin; };
    double XDispMax() { return dXDispMax; };
    double YDispMin(int c) { return dYDispMin[c]; };
    double YDispMax(int c) { return dYDispMax[c]; };
    //double YFDispMax(int c) { return DD().Columns[c].dFDispMax; };
    void SetXDispMin(double m) { dXDispMin=m; };
    void SetXDispMax(double m) { dXDispMax=m; };
    void SetYDispMin(int c, double m) { dYDispMin[c]=m; };
    void SetYDispMax(int c, double m) { dYDispMax[c]=m; };
    //void SetYFDispMax(int c, double m) { ;}; //DD().Columns[c].dFDispMax=m; };

    //double GetXData(int iInt) { return D().rIntervals[iInt]; };
    //CDArray & GetXData() { return D().rIntervals; };
    //double GetYData(PC_Column &C, int iInt);
    //CDArray & GetYData(PC_Column &C);
    //double GetYData(int c, int iInt);
    //CDArray & GetYData(int c);

    int CalcXPix(double X) { return (int)Range((double)-XPix, ((X)-XMin)*XFactor, (double)XPix*2);};
    //int CalcYPix(double Y) { return (int)Range((double)-YPix, ((Y)-YMin)*(YCFactor*950)/1000+(YPix*25)/1000, (double)YPix*2);};
    int CalcYPix(int c, double Y) { return (int)Range((double)-YPix, ((Y)-YMin[c])*(YFactor[c]*950)/1000+(YPix*25)/1000, (double)YPix*2);};
//
//    int CalcXLogPix(double X) { return (int)Range((double)-XPix, (Log10(GTZ(X))-XMin)*XFactor, (double)XPix*2); };
//    int CalcYCLogPix(int c, double Y) { return (int)Range((double)-YPix, (Log10(GTZ(Y))-YMin[c])*(YCFactor[c]*950)/1000+(YPix*25)/1000, (double)YPix*2); };
//    int CalcYFLogPix(int c, double Y) { return (int)Range((double)-YPix, (Log10(GTZ(Y))-YMin[c])*(YFFactor[c]*950)/1000+(YPix*25)/1000, (double)YPix*2); };

    double CalcXVal(int X) { return (float)X/NZ(XFactor)+XMin;};
    double CalcYVal(int c, int Y) { return (float)Y/NZ(YFactor[c])+YMin[c];};
    double CalcXLogVal(int X) { return pow(10.0, (float)X/NZ(XFactor)+XMin); };

    PC_DataBaseEdt(pFxdEdtView pView_, pPC_DataBase pPC_);//, rStrng Tag, rStrng Name);

    virtual ~PC_DataBaseEdt();

    virtual void PutDataStart();
    virtual void PutDataDone();
    virtual void GetDataStart();
    virtual void GetDataDone();

    virtual void StartBuild();
    virtual void Build();
    virtual void ChangeLayout(rFxdEdtPage Pg, int TotalRows, int TotalCols);
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long ButtonPushed(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);

    virtual void OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn);
    virtual flag DoLButtonDown(UINT nFlags, CPoint point);
    virtual flag DoLButtonUp(UINT nFlags, CPoint point);
    virtual flag DoLButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoRButtonDown(UINT nFlags, CPoint point);
    virtual flag DoRButtonUp(UINT nFlags, CPoint point);
    virtual flag DoRButtonDblClk(UINT nFlags, CPoint point);
    virtual flag DoMouseMove(UINT nFlags, CPoint point);

    virtual flag DoAccCnv(UINT Id);
    virtual flag DoAccFmt(UINT Id);
    virtual flag DoAccRptTagLists();

    virtual void SetPosition(rGDIBlk GB);//, CRect *CRReqd);
  private:
    void SetRanges();//CDArray & YPts, CDArray & XPts);
    void PointtoLP(POINT &Pt, int c);
    void PointtoWP(POINT Pt, double& x, int c, double& y);
    int ClosestPt(CPoint point, int c);
    void ToggleDigCursor(POINT &Pt);

  };
#endif
// ===========================================================================
//
//
//
// ===========================================================================

#undef DllImportExport

#endif
