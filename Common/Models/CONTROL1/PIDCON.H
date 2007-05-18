//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __PIDCON_H
#define  __PIDCON_H

#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif  
#ifndef __FLWNODE_H
#include "flwnode.h"
#endif
#ifndef __MODELS_H
#include "models.h"
#endif  
#ifndef __MDLXREF_H
#include "xrefs.h"
#endif  
#ifndef __PGM_C_H
#include "pgm_c.h"
#endif  

#ifdef __PIDCON_CPP
  #define DllImportExport 
#elif !defined(Control1)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//============================================================================

_FWDDEF(PIDInfo)
class PIDInfo : public CXRefStatus
  {
  public:
    Strng      m_sID;
    Strng      m_sIDMeas;
    Strng      m_sIDSpt;
    Strng      m_sIDFeedFwd;
    Strng      m_sIDOut;
    Strng      m_sTgMeas;
    Strng      m_sTgSpt;
    Strng      m_sTgFeedFwd;
    Strng      m_sTgOut;
    CTgFnIoVar m_MeasVar;
    CTgFnIoVar m_OutputVar;
    CTgFnIoVar m_SetPointVar;
    CTgFnIoVar m_FeedFwdVar;
    //Strng      m_sName;
    flag       bOn;
    flag       bValid:1;
    long       iPriority;
//    CPID2      ThePID;
    virtual void Init(CNodeXRefMngr * pXRM, int iNo)=0;
    virtual unsigned char &SptTrk()=0;
    virtual unsigned char &TrackMinMaxMeas()=0;
    virtual unsigned char &TrackMinMaxOut()=0;
    virtual unsigned char &AutoMan()=0;
    virtual unsigned char &RevActing()=0;
    virtual unsigned char &On()=0;

    virtual double &Period()=0;
    virtual double &Spt()=0;
	virtual double &FeedFwd()=0;
    virtual double &Meas()=0;
    virtual double &Out()=0;
    //virtual double &Meas()=0;
    virtual double PBand()=0;
    virtual double Gain()=0;
    virtual void   SetPBand(double v)=0;
    virtual void   SetGain(double v)=0;

    virtual double &Ic()=0;
    virtual double &Dc()=0;
    virtual double &InMn()=0;
    virtual double &InMx()=0;
    virtual double &OutMn()=0;
    virtual double &OutMx()=0;

    virtual double &ETerm()=0;
    virtual double &PTerm()=0;
    virtual double &ITerm()=0;
    virtual double &DTerm()=0;
    virtual double &Bias()=0;

    virtual byte   &TuneRule()=0;
    virtual double &TuneOut()=0;

    virtual double &EPSAbs()=0;
    virtual double &EPSRel()=0;

    virtual double ExecIns()=0;
    virtual flag   Interacting()=0;
    virtual flag   CorrectActionImplemenation()=0; // PID2 and PID3 Reverse Action definition is
	                                               // Opposite to PID4. This Method lets copy
	                                               // cope with changing between types.

    virtual Strng & TagSuffixS()=0;
    virtual char * TagSuffix()=0;
    virtual void   SetTagSuffix(char * s)=0;

    virtual void   BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj) = 0;


    PIDInfo(int Index);
    virtual ~PIDInfo();

    void Copy(PIDInfo *p)
      {
      m_MeasVar=p->m_MeasVar;
      m_OutputVar=p->m_OutputVar;
      m_SetPointVar=p->m_SetPointVar;
      m_FeedFwdVar=p->m_FeedFwdVar;

      //m_sName=p->m_sName;
      iPriority=p->iPriority;
      bOn=p->bOn;
      //bValid:1;

      SptTrk()=p->SptTrk();
      TrackMinMaxMeas()=p->TrackMinMaxMeas();
      TrackMinMaxOut()=p->TrackMinMaxOut();
      AutoMan()=p->AutoMan();

      if (CorrectActionImplemenation() == p->CorrectActionImplemenation())
        RevActing()=p->RevActing();
      else
        {
        if (p->RevActing())
          RevActing()=false;
        else
        RevActing()=true;
        }

      On()=p->On();

      Period()=p->Period();
      Spt()=p->Spt();
      Meas()=p->Meas();
      Out()=p->Out();

      SetGain(p->Gain());
      double G=GTZ(fabs(Gain()));
      if (Interacting() && !p->Interacting())
        {
        Ic()=p->Ic()*G;
        Dc()=p->Dc()/G;
        }
      else if (!Interacting() && p->Interacting())
        {
        Ic()=p->Ic()/G;
        Dc()=p->Dc()*G;
        }
      else
        {
        Ic()=p->Ic();
        Dc()=p->Dc();
        }

      InMn()=p->InMn();
      InMx()=p->InMx();
      OutMn()=p->OutMn();
      OutMx()=p->OutMx();
	  Bias()=p->Bias();

      TuneRule()=p->TuneRule();
      TuneOut()=p->TuneOut();

      EPSAbs()=p->EPSAbs();
      EPSRel()=p->EPSRel();
      //ExecIns()=p->ExecIns();

      SetTagSuffix(p->TagSuffix());
      };
  
    //CXRefStatus Override
    bool IsXRefActive() const    { return bValid && bOn;  };
  };

class PID2Info : public PIDInfo, public CPID2Data, public CPID2
  {
  public:
    //CPID2      ThePID;

    PID2Info(int Index);
    virtual ~PID2Info();

    virtual void Init(CNodeXRefMngr * pXRM, int iNo);
    virtual unsigned char &SptTrk()         { return m_SptTrk; };
    virtual unsigned char &TrackMinMaxMeas(){ return m_TrackMinMaxMeas; };
    virtual unsigned char &TrackMinMaxOut() { return m_TrackMinMaxOut; };
    virtual unsigned char &AutoMan()        { return m_AutoMan; };
    virtual unsigned char &RevActing()      { return m_RevActing; };
    virtual unsigned char &On()             { return m_bOn; };

    virtual char * PIDTag()                 { m_sFullTag=TagName(); m_sFullTag+="."; m_sFullTag+=TagSuffix(); return m_sFullTag(); };

    virtual char * MeasTag()                { return m_MeasVar.sVar();     };//? m_MeasVar.sVar()     : "Measure"   ; };
    virtual char * SptTag()                 { return m_SetPointVar.sVar(); };//? m_SetPointVar.sVar() : "SetPoint"  ; };
    virtual char * OutTag()                 { return m_OutputVar.sVar();   };//? m_OutputVar.sVar()   : "Control"   ; };

    virtual double &Period()                { return m_Period; };
    virtual double &Spt()                   { return m_Spt; };
    virtual double &FeedFwd()               { return m_FeedFwd; };
    virtual double &Meas()                  { return m_Meas; };
    virtual double &Out()                   { return m_Out; };
    //virtual double &Meas();
    virtual double PBand()                  { return m_PBand; };
    virtual double Gain()                   { return 1.0/GTZ(m_PBand); };
    virtual void   SetPBand(double v)       { m_PBand=v; };
    virtual void   SetGain(double v)        { m_PBand=1.0/GTZ(v); };

    virtual double &Ic()                    { return m_Ic; };
    virtual double &Dc()                    { return m_Dc; };
    virtual double &InMn()                  { return m_InMn; };
    virtual double &InMx()                  { return m_InMx; };
    virtual double &OutMn()                 { return m_OutMn; };
    virtual double &OutMx()                 { return m_OutMx; };

    virtual double &ETerm()                 { return m_ETerm; };
    virtual double &PTerm()                 { return m_PTerm; };
    virtual double &ITerm()                 { return m_ITerm; };
    virtual double &DTerm()                 { return m_DTerm; };
	  virtual double &Bias()                  { return m_Bias; };

    virtual byte   &TuneRule()              { return m_TuneRule; };
    virtual double &TuneOut()               { return m_TuneOut; };

    virtual double &EPSAbs()                { return m_EPSAbs; };
    virtual double &EPSRel()                { return m_EPSRel; };

    virtual double ExecIns()                { return CPID2::ExecIns(); };
    virtual flag   Interacting()            { return CPID2::Interacting(); };
    virtual flag   CorrectActionImplemenation() 
	                                        { return false;/*CPID2::CorrectActionImplemenation();*/ };

    //virtual Strng &TagSuffix()              { return m_sTagSuffix; };
    virtual Strng & TagSuffixS()            { return m_sTagSuffix; };
    virtual char * TagSuffix()              { return m_sTagSuffix(); };
    virtual void   SetTagSuffix(char * s)   { if (strlen(s)) m_sTagSuffix=s; };

    virtual void   BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj);
  };

class PID3Info : public PIDInfo, public CPID3Data, public CPID3
  {
  public:
    //CPID3      ThePID;

    PID3Info(int Index);
    virtual ~PID3Info();

    virtual void Init(CNodeXRefMngr * pXRM, int iNo);
    virtual unsigned char &SptTrk()         { return m_SptTrk; };
    virtual unsigned char &TrackMinMaxMeas(){ return m_TrackMinMaxMeas; };
    virtual unsigned char &TrackMinMaxOut() { return m_TrackMinMaxOut; };
    virtual unsigned char &AutoMan()        { return m_AutoMan; };
    virtual unsigned char &RevActing()      { return m_RevActing; };
    virtual unsigned char &On()             { return m_bOn; };

    virtual char * PIDTag()                 { m_sFullTag=TagName(); m_sFullTag+="."; m_sFullTag+=TagSuffix(); return m_sFullTag(); };

    virtual char * MeasTag()                { return m_MeasVar.sVar();     };//? m_MeasVar.sVar()     : "Measure"   ; };
    virtual char * SptTag()                 { return m_SetPointVar.sVar(); };//? m_SetPointVar.sVar() : "SetPoint"  ; };
    virtual char * OutTag()                 { return m_OutputVar.sVar();   };//? m_OutputVar.sVar()   : "Control"   ; };

    virtual double &Period()                { return m_Period; };
    virtual double &Spt()                   { return m_Spt; };
    virtual double &FeedFwd()               { return m_FeedFwd; };
    virtual double &Meas()                  { return m_Meas; };
    virtual double &Out()                   { return m_Out; };
    //virtual double &Meas();
    virtual double PBand()                  { return 1.0/GTZ(m_Gn); };
    virtual double Gain()                   { return m_Gn; };
    virtual void   SetPBand(double v)       { m_Gn=1.0/GTZ(v); };
    virtual void   SetGain(double v)        { m_Gn=v; };
    
    virtual double &Ic()                    { return m_Ic; };
    virtual double &Dc()                    { return m_Dc; };
    virtual double &InMn()                  { return m_InMn; };
    virtual double &InMx()                  { return m_InMx; };
    virtual double &OutMn()                 { return m_OutMn; };
    virtual double &OutMx()                 { return m_OutMx; };

    virtual double &ETerm()                 { return m_ETerm; };
    virtual double &PTerm()                 { return m_PTerm; };
    virtual double &ITerm()                 { return m_ITerm; };
    virtual double &DTerm()                 { return m_DTerm; };
	  virtual double &Bias()                  { return m_Bias; };

    virtual byte   &TuneRule()              { return m_TuneRule; };
    virtual double &TuneOut()               { return m_TuneOut; };

    virtual double &EPSAbs()                { return m_EPSAbs; };
    virtual double &EPSRel()                { return m_EPSRel; };

    virtual double ExecIns()                { return CPID3::ExecIns(); };
    virtual flag   Interacting()            { return CPID3::Interacting(); };
    virtual flag   CorrectActionImplemenation() 
	                                        { return false;/*CPID3::CorrectActionImplemenation();*/ };
    virtual Strng & TagSuffixS()            { return m_sTagSuffix; };
    virtual char * TagSuffix()              { return m_sTagSuffix(); };
    virtual void   SetTagSuffix(char * s)   { if (strlen(s)) m_sTagSuffix=s; };
//    virtual Strng &TagSuffix()              { return m_sTagSuffix; };

    virtual void   BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj);
  };


class PID4Info : public PIDInfo, public CPID4Data, public CPID4
  {
  public:
    //CPID4      ThePID;

    PID4Info(int Index);
    virtual ~PID4Info();

    virtual void Init(CNodeXRefMngr * pXRM, int iNo);
    virtual unsigned char &SptTrk()         { return m_SptTrk; };
    virtual unsigned char &TrackMinMaxMeas(){ return m_TrackMinMaxMeas; };
    virtual unsigned char &TrackMinMaxOut() { return m_TrackMinMaxOut; };
    virtual unsigned char &AutoMan()        { return m_AutoMan; };
    virtual unsigned char &RevActing()      { return m_RevActing; };
    virtual unsigned char &On()             { return m_bOn; };

    virtual char * PIDTag()                 { m_sFullTag=TagName(); m_sFullTag+="."; m_sFullTag+=TagSuffix(); return m_sFullTag(); };

    virtual char * MeasTag()                { return m_MeasVar.sVar();     };//? m_MeasVar.sVar()     : "Measure"   ; };
    virtual char * SptTag()                 { return m_SetPointVar.sVar(); };//? m_SetPointVar.sVar() : "SetPoint"  ; };
    virtual char * OutTag()                 { return m_OutputVar.sVar();   };//? m_OutputVar.sVar()   : "Control"   ; };

    virtual double &Period()                { return m_Period; };
    virtual double &Spt()                   { return m_Spt; };
    virtual double &FeedFwd()               { return m_FeedFwd; };
    virtual double &Meas()                  { return m_Meas; };
    virtual double &Out()                   { return m_Out; };
    //virtual double &Meas();
    virtual double PBand()                  { return 1.0/GTZ(m_Gn); };
    virtual double Gain()                   { return m_Gn; };
    virtual void   SetPBand(double v)       { m_Gn=1.0/GTZ(v); };
    virtual void   SetGain(double v)        { m_Gn=v; };
    
    virtual double &Ic()                    { return m_Ic; };
    virtual double &Dc()                    { return m_Dc; };
    virtual double &InMn()                  { return m_InMn; };
    virtual double &InMx()                  { return m_InMx; };
    virtual double &OutMn()                 { return m_OutMn; };
    virtual double &OutMx()                 { return m_OutMx; };

    virtual double &ETerm()                 { return m_ETerm; };
    virtual double &PTerm()                 { return m_PTerm; };
    virtual double &ITerm()                 { return m_ITerm; };
    virtual double &DTerm()                 { return m_DTerm; };
	  virtual double &Bias()                  { return m_Bias; };

    virtual byte   &TuneRule()              { return m_TuneRule; };
    virtual double &TuneOut()               { return m_TuneOut; };

    virtual double &EPSAbs()                { return m_EPSAbs; };
    virtual double &EPSRel()                { return m_EPSRel; };

    virtual double ExecIns()                { return CPID4::ExecIns(); };
    virtual flag   Interacting()            { return CPID4::Interacting(); };
    virtual flag   CorrectActionImplemenation() 
	                                        { return true;/*CPID4::CorrectActionImplemenation();*/ };
  
    virtual Strng & TagSuffixS()            { return m_sTagSuffix; };
    virtual char * TagSuffix()              { return m_sTagSuffix(); };
    virtual void   SetTagSuffix(char * s)   { if (strlen(s)) m_sTagSuffix=s; };
//    virtual Strng &TagSuffix()              { return m_sTagSuffix; };

    virtual void   BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj);
  };


typedef CSmartPtr<PIDInfo> CSPPIDInfo;
typedef CArray <CSPPIDInfo, CSPPIDInfo> CPIDInfoArray;     //array of PID controllers

DEFINE_TAGOBJ(PIDControl);
class PIDControl : public FlwNode
  {
  friend class CTgFnIoVar;
  public:
    static flag    bWithCnvComment;

    CArray <byte, byte&> PIDType;        //array of PID Types 2/3
    CPIDInfoArray      PID;     //array of PID controllers
    CArray <IOAreaRec, IOAreaRec&> m_IOAreas;
    CArray <bool, bool> m_IOConnd;
    
    flag           bOn;                  //must the pgm be executed etc
    Strng          m_StateLine[3];       //status messages
    flag           //bDoneExtRefs:1,       //
                   bAboutToStart:1;      //flag set True for first iteration when run is pressed
    Strng          m_sTmpID, m_sTmpTg;
    bool           m_bFixIOReqd;

    PIDControl(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~PIDControl();
    virtual void   ResetData(flag Complete);
    virtual void   Ctrl_ConnIDStr(int i, Strng & ID, Strng & Tg);
  private:
    void           SetCount(long NewSize);
    flag           SortRqd();
    void           Sort();
    void           FixIOTags();
  public:
    virtual LPSTR  Tag();
    virtual LPSTR  SetTag(LPSTR ReqdTag, bool AllowEmptyTag=false);
    
    void           PostConnect(int IONo);
    void           PreDisConnect(int IONo);
    
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   PreStartCheck();
    virtual void   EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All);
    virtual int    ChangeTag(pchar pOldTag, pchar pNewTag);
    virtual int    DeleteTag(pchar pDelTag);

    // CNodeXRefMngr Overides
    virtual bool   TestXRefListActive();
    virtual int    UpdateXRefLists(CXRefBuildResults & Results);
    virtual void   UnlinkAllXRefs();

    virtual dword  ModelStatus();

    DEFINE_CI(PIDControl, FlwNode, 4);

  };
  
//===========================================================================

#undef DllImportExport

#endif

