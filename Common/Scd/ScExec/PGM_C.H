//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __PGM_C_H
#define __PGM_C_H

#ifndef __ERRORLOG_H
  #include "errorlog.h"
#endif
#ifndef __CTRLSEQ_H
  #include "ctrlseq.h"
#endif

#ifdef __PGM_C_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// =========================================================================
//
//
//
// =========================================================================

//DEFINE_TAGOBJEDTDERIVED(PIDTearVarBlk, TearVarBlkClassDef);
//
//class DllImportExport PIDTearVarBlk : public TearVarBlk
//  {
//  protected:
//    CnvAttribute Cnv;
//    FmtAttribute Fmt;
//
//  public:
//    PIDTearVarBlk(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
//    PIDTearVarBlk(pchar Tag_, pTaggedObject pAttach);
//    virtual ~PIDTearVarBlk();
//
//    void DoConstruct();
//
//    virtual void   OnEditConstruct();
//    virtual void   OnEditDestroy();
//
//  };
//
// =========================================================================
//
//
//
// =========================================================================

class DllImportExport CPID2Data
  {
  public:
    unsigned char  bOn;
    double         Meas;
    unsigned char  SptTrk;
    unsigned char  TrackMinMaxMeas;
    unsigned char  TrackMinMaxOut;
    unsigned char  AutoMan;
    double         InMn;
    double         InMx;
    double         OutMn;
    double         OutMx;
    unsigned char  RevActing;
    unsigned char  TuneRule;
    double         TuneOut;

    double         Spt;
    
    double         DeTune;
    double         PBand;
    double         Ic;
    double         Dc;
    
    double         Out;
    
    double         KScale;
    double         ETerm;
    double         PTerm;
    double         ITerm;
    double         DTerm;
    double         Bias;

    double         EPSAbs;
    double         EPSRel;
  };

//---------------------------------------------------------------------------

class DllImportExport CPID2 : public ConditionBlk, public CtrlSeqBlk
  {
  friend class GCPID2;
  friend class PIDControl;
  friend class PIDCon;
  friend class PIDInfo;
  friend class PID2Info;
  public:
    CPID2(pGCClassVar pClassVar);
    CPID2(CPID2Data* pData);
    virtual ~CPID2();
  protected:
    CNodeXRefMngr * pXRM;

    Strng          m_sTagSuffix;
    Strng          m_sFullTag;
    
    dword          m_dwExecCount; 
    //NBNB Ensure the required parms are saved/recovered in PID application in GenCon AND PIDCon, etc; also check parms are in ::Copy()
    double         m_Period;
    flag           m_Reset;
    flag           m_PrvAuto;

    double         m_PrvSetPoint;
    double         m_PrvOutput;
    double         m_PrvPBand;
    double         m_PrvDc;
    double         m_PrvScaledError;
    long           m_HoldCvgCount;

    //...
    unsigned char  &m_bOn;
    double         &m_Meas;
    unsigned char  &m_SptTrk;
    unsigned char  &m_TrackMinMaxMeas;
    unsigned char  &m_TrackMinMaxOut;
    unsigned char  &m_AutoMan;
    double         &m_InMn;
    double         &m_InMx;
    double         &m_OutMn;
    double         &m_OutMx;
    unsigned char  &m_RevActing;
    unsigned char  &m_TuneRule;
    //double         &m_GnScale;
    double         &m_TuneOut;
    double         &m_DeTune;
    
    double         &m_Spt;
	double         m_FeedFwd; // Not used
    
    double         &m_PBand;
    double         &m_Ic;
    double         &m_Dc;
    
    double         &m_Out;
    
    
    double         &m_KScale;
    double         &m_ETerm;
    double         &m_PTerm;
    double         &m_ITerm;
    double         &m_DTerm;
	double         &m_Bias; // Not used

    double         &m_EPSAbs;
    double         &m_EPSRel;
    
  public:
    double ExecIns();
    void Copy(CPID2* p);
    void SetPeriod(double dPeriod) { m_Period = dPeriod; };
    flag Interacting() { return true; };
	flag CorrectActionImplemenation() { return false; };


    char*          TagName() { return pXRM->GetOwnerTag(); };

    DEFINE_CI(CPID2, ConditionBlk, 8);
    char*          CBTag() { m_sFullTag=TagName(); m_sFullTag+="."; m_sFullTag+=m_sTagSuffix; return m_sFullTag(); };
    void           CBChanged() { };


//<PID>    // Tear Pos Block overrides
//<PID>    TearVarBlk*    CreateVarBlk(char * Tag, pTaggedObject pAttach);
//<PID>    byte           TearVarType();
//<PID>    //flag           TearAdvanceReqd();
//<PID>    //flag           TearCnvTestReqd();
//<PID>    //flag           TearCnvActionReqd();
//<PID>    int            TearInitTuneInfo(TearVarArray & TV);
//<PID>    int            TearGetTuneInfo(TearVarArray & TV);
//<PID>    int            TearSetTuneInfo(TearVarArray & TV);
//<PID>    void           TearInitialiseOutputs(double EstimatePortion);
//<PID>    void           TearInputs2Outputs();
//<PID>    char *         TearGetTag(Strng & RootTag);
//<PID>    int            TearGetConvergeInfo(TearVarArray & TV);
//<PID>    void           TearGetErrors(TearVarArray & TV);
//<PID>    void           TearGetInputs(TearVarArray & TV);
//<PID>    void           TearGetOutputs(TearVarArray & TV);
//<PID>    void           TearSetOutputs(TearVarArray & TV);
//<PID>    flag           TearAdvance(TearVarArray & TV, EqnSlvCtrlBlk & EqnCB);

    // CtrlSeqBlock overrides
    //void           InitTuneInfo();
    //void           GetTuneInfo();
    //void           SetTuneInfo();
    //
    //void           GetMeasure();
    //void           GetOutput();
    //void           SetOutput();
    virtual char * PIDTag()                { m_sFullTag=TagName(); m_sFullTag+="."; m_sFullTag+=m_sTagSuffix; return m_sFullTag(); };

    virtual char * MeasTag()               { return ""; };
    virtual char * SptTag()                { return ""; };
    virtual char * OutTag()                { return ""; };

    virtual double &SetPoint()             { return m_Spt;};
    virtual double &Measure()              { return m_Meas;};
    virtual double &Output()               { return m_Out; };
    virtual double &InMin()                { return m_InMn;};
    virtual double &InMax()                { return m_InMx;};
    virtual double &OutMin()               { return m_OutMn;};
    virtual double &OutMax()               { return m_OutMx;};
    virtual flag   &TuneRule()             { return m_TuneRule; }
    virtual double &TuneOut()              { return m_TuneOut; }
    virtual flag   &RevActing()            { return m_RevActing; }
    virtual flag   &SptTrk()               { return m_SptTrk; }
    virtual flag   &TrackMinMaxMeas()      { return m_TrackMinMaxMeas; }
    virtual flag   &TrackMinMaxOut()       { return m_TrackMinMaxOut; }
    virtual flag   &AutoMan()              { return m_AutoMan; }
    virtual flag   &On()                   { return m_bOn; }

    virtual double  OpGain()               { return 1/m_PBand; }
    virtual double  OpIc()                 { return m_Ic; }
    virtual double  OpDc()                 { return m_Dc; }
    virtual void    GetOpGain(double &G, double &I, double &D)  { G=1/m_PBand; I=m_Ic; D=m_Dc; }
    virtual void    SetOpGain(double G, double I, double D)  { m_PBand=1/G; m_Ic=I; m_Dc=D; }

    //virtual double &GainScale()            { return m_GnScale; }
    virtual double &DeTune()               { return m_DeTune; }
    virtual int     TestConverged();

    virtual double &EPSAbs()               { return m_EPSAbs; };
    virtual double &EPSRel()               { return m_EPSRel; };


  private:
    void Clear();
  };

//===========================================================================
/* All the classes derived from  GCClassVar are used for encapsulating a C++
   class in a PGM class.  The derived class is instantiated once and is used 
   as a class reference by each PGM.  PGM instances of the class are always 
   of type GCClassVar (BAD, BAD boy this is not what one would expect and 
   hopefully next time I debug, I won't waste 2 hours working this out yet
   again!) which contain a pointer to the reference class.  The PGM class
   instances then call the virtual functions of the reference class to do
   the work. I know, this could have been done properly with an appropriate
   virtual constructor or something! */
//---------------------------------------------------------------------------

class GCPID2 : public GCClassVar
  {
  public:
    GCPID2(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB/*FILE* f*/, void* pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB/*rCTokenFile f*/, void* pSubClass);
    virtual void OnRestore(void* pOldSubClass, void* pNewSubClass);
//    virtual flag EvalPBConvergence(EqnSlvCtrlBlk & EQCtrl, pGCClassVar pClassVar);
//    virtual BOOL InitPBConvergence(pGCClassVar pClassVar);
//    virtual void InitIteration(pGCClassVar pClassVar); //kkk
    // CNM
    virtual void   OnOptimize(CNodeXRefMngr * pXRM, void* pClssVar, void* pSubClass);

  };

// =========================================================================
//
//
//
// =========================================================================

class DllImportExport CPID3Data
  {
  public:
    unsigned char  bOn;
    double         Meas;
    unsigned char  SptTrk;
    unsigned char  TrackMinMaxMeas;
    unsigned char  TrackMinMaxOut;
    unsigned char  AutoMan;
    double         InMn;
    double         InMx;
    double         OutMn;
    double         OutMx;
    unsigned char  RevActing;
    unsigned char  TuneRule;
    double         TuneOut;
    double         Spt;
    
    double         DeTune;
    double         Gn;
    double         Ic;
    double         Dc;
    
    double         Out;
    
    double         ETerm;
    double         PTerm;
    double         ITerm;
    double         DTerm;
	double         Bias;

    double         EPSAbs;
    double         EPSRel;

  };

//---------------------------------------------------------------------------

class DllImportExport CPID3 : public ConditionBlk, public CtrlSeqBlk
  {
  friend class GCPID3;
  friend class PIDControl;
  friend class PIDCon;
  friend class PIDInfo;
  friend class PID3Info;
  public:
    CPID3(pGCClassVar pClassVar);
    CPID3(CPID3Data* pData);
    virtual ~CPID3();
  protected:
    CNodeXRefMngr * pXRM;

    Strng          m_sTagSuffix;
    Strng          m_sFullTag;
    
    dword          m_dwExecCount; 
    //NBNB Ensure the required parms are saved/recovered in PID application in GenCon AND PIDCon, etc; also check parms are in ::Copy()
    double         m_Period;
    flag           m_Reset;
    flag           m_PrvAuto;

    double         m_PrvSetPoint;
    double         m_PrvOutput;
    double         m_PrvGn;
    double         m_PrvDc;
    double         m_PrvMeas;
    double         m_PrvRange;
    long           m_HoldCvgCount;

    //...
    unsigned char  &m_bOn;
    double         &m_Meas;
    unsigned char  &m_SptTrk;
    unsigned char  &m_TrackMinMaxMeas;
    unsigned char  &m_TrackMinMaxOut;
    unsigned char  &m_AutoMan;
    double         &m_InMn;
    double         &m_InMx;
    double         &m_OutMn;
    double         &m_OutMx;
    unsigned char  &m_RevActing;
    unsigned char  &m_TuneRule;
    double         &m_TuneOut;
    double         &m_Spt;
	double         m_FeedFwd; // Not used

    
    double         &m_DeTune;
    double         &m_Gn;
    double         &m_Ic;
    double         &m_Dc;
    
    double         &m_Out;
    
    double         &m_ETerm;
    double         &m_PTerm;
    double         &m_ITerm;
    double         &m_DTerm;
	double         &m_Bias; // Not used

    double         &m_EPSAbs;
    double         &m_EPSRel;
    
  public:
    double ExecIns();
    void Copy(CPID3* p);
    void SetPeriod(double dPeriod) { m_Period = dPeriod; };
    flag Interacting() { return true; };
	flag CorrectActionImplemenation() { return false; };


    char*          TagName() { return pXRM->GetOwnerTag(); };

    DEFINE_CI(CPID3, ConditionBlk, 8);
    char*          CBTag() { m_sFullTag=TagName(); m_sFullTag+="."; m_sFullTag+=m_sTagSuffix; return m_sFullTag(); };
    void           CBChanged() { };

//<PID>    // Tear Pos Block overrides
//<PID>    TearVarBlk*    CreateVarBlk(char * Tag, pTaggedObject pAttach);
//<PID>    byte           TearVarType();
//<PID>    //flag           TearAdvanceReqd();
//<PID>    //flag           TearCnvTestReqd();
//<PID>    //flag           TearCnvActionReqd();
//<PID>    int            TearInitTuneInfo(TearVarArray & TV);
//<PID>    int            TearGetTuneInfo(TearVarArray & TV);
//<PID>    int            TearSetTuneInfo(TearVarArray & TV);
//<PID>    void           TearInitialiseOutputs(double EstimatePortion);
//<PID>    void           TearInputs2Outputs();
//<PID>    char *         TearGetTag(Strng & RootTag);
//<PID>    int            TearGetConvergeInfo(TearVarArray & TV);
//<PID>    void           TearGetErrors(TearVarArray & TV);
//<PID>    void           TearGetInputs(TearVarArray & TV);
//<PID>    void           TearGetOutputs(TearVarArray & TV);
//<PID>    void           TearSetOutputs(TearVarArray & TV);
//<PID>    flag           TearAdvance(TearVarArray & TV, EqnSlvCtrlBlk & EqnCB);
    
    // CtrlSeqBlock overrides
    //void           InitTuneInfo();
    //void           GetTuneInfo();
    //void           SetTuneInfo();
    //
    //void           GetMeasure();
    //void           GetOutput();
    //void           SetOutput();
    
    virtual double &SetPoint()             { return m_Spt;};
    virtual double &Measure()              { return m_Meas;};
    virtual double &Output()               { return m_Out; };
    virtual double &InMin()                { return m_InMn;};
    virtual double &InMax()                { return m_InMx;};
    virtual double &OutMin()               { return m_OutMn;};
    virtual double &OutMax()               { return m_OutMx;};
    virtual flag   &TuneRule()             { return m_TuneRule; }
    virtual double &TuneOut()              { return m_TuneOut; }
    virtual flag   &RevActing()            { return m_RevActing; }
    virtual flag   &SptTrk()               { return m_SptTrk; }
    virtual flag   &TrackMinMaxMeas()      { return m_TrackMinMaxMeas; }
    virtual flag   &TrackMinMaxOut()       { return m_TrackMinMaxOut; }
    virtual flag   &AutoMan()              { return m_AutoMan; }
    virtual flag   &On()                   { return m_bOn; }

    virtual double  OpGain()               { return m_Gn; }
    virtual double  OpIc()                 { return m_Ic; }
    virtual double  OpDc()                 { return m_Dc; }
    virtual void    GetOpGain(double &G, double &I, double &D)  { G=m_Gn; I=m_Ic; D=m_Dc; }
    virtual void    SetOpGain(double G, double I, double D)  { m_Gn=G; m_Ic=I; m_Dc=D; }

    //virtual double &GainScale()            { return m_GnScale; }
    virtual double &DeTune()               { return m_DeTune; }
    virtual int     TestConverged();

    virtual double &EPSAbs()               { return m_EPSAbs; };
    virtual double &EPSRel()               { return m_EPSRel; };

  private:
    void Clear();
  };

//===========================================================================
/* All the classes derived from  GCClassVar are used for encapsulating a C++
   class in a PGM class.  The derived class is instantiated once and is used 
   as a class reference by each PGM.  PGM instances of the class are always 
   of type GCClassVar (BAD, BAD boy this is not what one would expect and 
   hopefully next time I debug, I won't waste 2 hours working this out yet
   again!) which contain a pointer to the reference class.  The PGM class
   instances then call the virtual functions of the reference class to do
   the work. I know, this could have been done properly with an appropriate
   virtual constructor or something! */
//---------------------------------------------------------------------------

class GCPID3 : public GCClassVar
  {
  public:
    GCPID3(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB/*FILE* f*/, void* pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB/*rCTokenFile f*/, void* pSubClass);
    virtual void OnRestore(void* pOldSubClass, void* pNewSubClass);
//    virtual flag EvalPBConvergence(EqnSlvCtrlBlk & EQCtrl, pGCClassVar pClassVar);
//    virtual BOOL InitPBConvergence(pGCClassVar pClassVar);
//    virtual void InitIteration(pGCClassVar pClassVar); //kkk
    // CNM
    virtual void   OnOptimize(CNodeXRefMngr * pXRM, void* pClssVar, void* pSubClass);

  };

// =========================================================================
//
//
//
// =========================================================================

class DllImportExport CPID4Data
  {
  public:
    unsigned char  bOn;
    double         Meas;
    unsigned char  SptTrk;
    unsigned char  TrackMinMaxMeas;
    unsigned char  TrackMinMaxOut;
    unsigned char  AutoMan;
    double         InMn;
    double         InMx;
    double         OutMn;
    double         OutMx;
    unsigned char  RevActing;
    unsigned char  TuneRule;
    double         TuneOut;
    double         Spt;
	double         FeedFwd;
    
    double         DeTune;
    double         Gn;
    double         Ic;
    double         Dc;
    
    double         Out;
    
    double         ETerm;
    double         PTerm;
    double         ITerm;
    double         DTerm;
	double         Bias;

    double         EPSAbs;
    double         EPSRel;

  };

//---------------------------------------------------------------------------

class DllImportExport CPID4 : public ConditionBlk, public CtrlSeqBlk
  {
  friend class GCPID4;
  friend class PIDControl;
  friend class PIDCon;
  friend class PIDInfo;
  friend class PID4Info;
  public:
    CPID4(pGCClassVar pClassVar);
    CPID4(CPID4Data* pData);
    virtual ~CPID4();
  protected:
    CNodeXRefMngr * pXRM;

    Strng          m_sTagSuffix;
    Strng          m_sFullTag;
    
    dword          m_dwExecCount; 
    //NBNB Ensure the required parms are saved/recovered in PID application in GenCon AND PIDCon, etc; also check parms are in ::Copy()
    double         m_Period;
    flag           m_Reset;
    flag           m_PrvAuto;

    double         m_PrvSetPoint;
    double         m_PrvOutput;
    double         m_PrvGn;
    double         m_PrvDc;
    double         m_PrvMeas;
    double         m_PrvRange;
    long           m_HoldCvgCount;

    //...
    unsigned char  &m_bOn;
    double         &m_Meas;
    unsigned char  &m_SptTrk;
    unsigned char  &m_TrackMinMaxMeas;
    unsigned char  &m_TrackMinMaxOut;
    unsigned char  &m_AutoMan;
    double         &m_InMn;
    double         &m_InMx;
    double         &m_OutMn;
    double         &m_OutMx;
    unsigned char  &m_RevActing;
    unsigned char  &m_TuneRule;
    double         &m_TuneOut;
    double         &m_Spt;
	double         &m_FeedFwd;
    
    double         &m_DeTune;
    double         &m_Gn;
    double         &m_Ic;
    double         &m_Dc;
    
    double         &m_Out;
    
    double         &m_ETerm;
    double         &m_PTerm;
    double         &m_ITerm;
    double         &m_DTerm;
	double         &m_Bias; // 0-1 i.e. 0-100%

    double         &m_EPSAbs;
    double         &m_EPSRel;
    
  public:
    double ExecIns();
    void Copy(CPID4* p);
    void SetPeriod(double dPeriod) { m_Period = dPeriod; };
    flag Interacting() { return true; };
	flag CorrectActionImplemenation() { return true; };

    char*          TagName() { return pXRM->GetOwnerTag(); };

    DEFINE_CI(CPID4, ConditionBlk, 12);
    char*          CBTag() { m_sFullTag=TagName(); m_sFullTag+="."; m_sFullTag+=m_sTagSuffix; return m_sFullTag(); };
    void           CBChanged() { };

//<PID>    // Tear Pos Block overrides
//<PID>    TearVarBlk*    CreateVarBlk(char * Tag, pTaggedObject pAttach);
//<PID>    byte           TearVarType();
//<PID>    //flag           TearAdvanceReqd();
//<PID>    //flag           TearCnvTestReqd();
//<PID>    //flag           TearCnvActionReqd();
//<PID>    int            TearInitTuneInfo(TearVarArray & TV);
//<PID>    int            TearGetTuneInfo(TearVarArray & TV);
//<PID>    int            TearSetTuneInfo(TearVarArray & TV);
//<PID>    void           TearInitialiseOutputs(double EstimatePortion);
//<PID>    void           TearInputs2Outputs();
//<PID>    char *         TearGetTag(Strng & RootTag);
//<PID>    int            TearGetConvergeInfo(TearVarArray & TV);
//<PID>    void           TearGetErrors(TearVarArray & TV);
//<PID>    void           TearGetInputs(TearVarArray & TV);
//<PID>    void           TearGetOutputs(TearVarArray & TV);
//<PID>    void           TearSetOutputs(TearVarArray & TV);
//<PID>    flag           TearAdvance(TearVarArray & TV, EqnSlvCtrlBlk & EqnCB);
    
    // CtrlSeqBlock overrides
    //void           InitTuneInfo();
    //void           GetTuneInfo();
    //void           SetTuneInfo();
    //
    //void           GetMeasure();
    //void           GetOutput();
    //void           SetOutput();
    
    virtual double &SetPoint()             { return m_Spt;};
    virtual double &Measure()              { return m_Meas;};
    virtual double &Output()               { return m_Out; };
    virtual double &InMin()                { return m_InMn;};
    virtual double &InMax()                { return m_InMx;};
    virtual double &OutMin()               { return m_OutMn;};
    virtual double &OutMax()               { return m_OutMx;};
    virtual flag   &TuneRule()             { return m_TuneRule; }
    virtual double &TuneOut()              { return m_TuneOut; }
    virtual flag   &RevActing()            { return m_RevActing; }
    virtual flag   &SptTrk()               { return m_SptTrk; }
    virtual flag   &TrackMinMaxMeas()      { return m_TrackMinMaxMeas; }
    virtual flag   &TrackMinMaxOut()       { return m_TrackMinMaxOut; }
    virtual flag   &AutoMan()              { return m_AutoMan; }
    virtual flag   &On()                   { return m_bOn; }

    virtual double  OpGain()               { return m_Gn; }
    virtual double  OpIc()                 { return m_Ic; }
    virtual double  OpDc()                 { return m_Dc; }
    virtual void    GetOpGain(double &G, double &I, double &D)  { G=m_Gn; I=m_Ic; D=m_Dc; }
    virtual void    SetOpGain(double G, double I, double D)  { m_Gn=G; m_Ic=I; m_Dc=D; }

    //virtual double &GainScale()            { return m_GnScale; }
    virtual double &DeTune()               { return m_DeTune; }
    virtual int     TestConverged();

    virtual double &EPSAbs()               { return m_EPSAbs; };
    virtual double &EPSRel()               { return m_EPSRel; };

  private:
    void Clear();
  };

//===========================================================================
/* All the classes derived from  GCClassVar are used for encapsulating a C++
   class in a PGM class.  The derived class is instantiated once and is used 
   as a class reference by each PGM.  PGM instances of the class are always 
   of type GCClassVar (BAD, BAD boy this is not what one would expect and 
   hopefully next time I debug, I won't waste 2 hours working this out yet
   again!) which contain a pointer to the reference class.  The PGM class
   instances then call the virtual functions of the reference class to do
   the work. I know, this could have been done properly with an appropriate
   virtual constructor or something! */
//---------------------------------------------------------------------------

class GCPID4 : public GCClassVar
  {
  public:
    GCPID4(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB/*FILE* f*/, void* pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB/*rCTokenFile f*/, void* pSubClass);
    virtual void OnRestore(void* pOldSubClass, void* pNewSubClass);
//    virtual flag EvalPBConvergence(EqnSlvCtrlBlk & EQCtrl, pGCClassVar pClassVar);
//    virtual BOOL InitPBConvergence(pGCClassVar pClassVar);
//    virtual void InitIteration(pGCClassVar pClassVar); //kkk
    // CNM
    virtual void   OnOptimize(CNodeXRefMngr * pXRM, void* pClssVar, void* pSubClass);

  };

//---------------------------------------------------------------------------

/*class GCXYFn : public GCClassVar
  {
  public:
    GCXYFn(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar);
  };*/

//---------------------------------------------------------------------------

class GCTime : public GCClassVar
  {
  public:
    GCTime(rGCInsMngr IB);
    virtual void Init(pGCClassVar pClassVar);
    virtual void Done(pGCClassVar pClassVar);
    virtual double CallFunct(rGCInsMngr IB, void* pSubClass, short FunctId, pGCClassVar pClassVar);
    virtual void OnSave(FilingControlBlock &FCB/*FILE* f*/, void* pSubClass);
    virtual flag OnLoad(FilingControlBlock &FCB/*rCTokenFile f*/, void* pSubClass);
  };

//---------------------------------------------------------------------------
#undef DllImportExport

#endif
