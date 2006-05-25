//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __ENGSTATE_H
#define  __ENGSTATE_H

//===========================================================================

#define WITHSFEWND 0

#if WITHSFEWND 

//===========================================================================

#ifndef __EXECUTIV_H
  #include "executiv.h"
#endif
#ifndef __FE_CONST_H
  #include "..\flwlib\fe_const.h"
#endif

#define NWorstLines  (BadListMax)
typedef char WorstLine[128];

_FWDDEF(EngStateWnd)
class EngStateWnd : public FxdEdtView, CExecObj
  {
    Strng   sFlwLibTag;
    DECLARE_DYNCREATE(EngStateWnd)
  protected:
    EngStateWnd();     // protected constructor used by dynamic creation

  public:
    virtual void Initialise();
    void    SetFlwLibName(char *FlwLibTag) { sFlwLibTag=FlwLibTag; };

  protected:
    virtual ~EngStateWnd();
    virtual void OnActivateView( BOOL bActivate, CView* pActivateView, CView* pDeactiveView );
    virtual void StartBuild();
    virtual void Build(int NRows=0, int NCols=0, int ColIndent=0, pvoid BuildData=NULL);
    virtual void Load(FxdEdtInfo &EI, Strng & Str);
    virtual long Parse(FxdEdtInfo &EI, Strng & Str);
//    virtual flag Changed(FxdEdtInfo &EI);
//    virtual void ClearChanged(FxdEdtInfo &EI);
    virtual void OnDrawBack(rGDIBlk GB,int PgNo, CRgn &ClipRgn);
    virtual void OnDrawLine(rGDIBlk GB,int PgNo, int RowNo);

    // Generated message map functions
  protected:
    //{{AFX_MSG(EngStateWnd)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

  public:
    flag Chnged;

    Strng      CnvTxt[10+NWorstLines]; 
    CCnvIndex  iCnv[10+NWorstLines]; 
    pFxdEdtFld TimeFld;
    pFxdEdtFld TargFld;
    pFxdEdtFld StopFld;
    pFxdEdtFld dTimeFld;
    pFxdEdtFld MultOFld;
    pFxdEdtFld MultIFld;
    pFxdEdtFld LoadOFld;
    pFxdEdtFld LoadIFld;
    pFxdEdtFld IterGFld;
    pFxdEdtFld IterBFld;
    pFxdEdtFld WorstFld[NWorstLines];
    pFxdEdtFld pFld[NWorstLines+50];

    //rSFELibrary SFE() { return Doc->SFE; };
    //void LoadEngStateData(Strng &Data);

    // CExecObj Overrides
    virtual flag   EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd);
    virtual flag   EO_Start(CXM_TimeControl &CB);
    virtual void   EO_QuerySubsReqd(CXMsgLst &XM);
    virtual void   EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet);
    virtual flag   EO_ReadSubsData(CXMsgLst &XM);  
    virtual flag   EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock);  
    virtual flag   EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet);
    virtual flag   EO_Stop(CXM_TimeControl &CB);
  };

//===========================================================================

class CEngStateFrm : public CMDIChildWnd
{
  DECLARE_DYNCREATE(CEngStateFrm)
// cnm protected:
  CEngStateFrm();           // protected constructor used by dynamic creation

public:
  //{{AFX_VIRTUAL(CEngStateFrm)
  //}}AFX_VIRTUAL
protected:
  virtual ~CEngStateFrm();
  //{{AFX_MSG(CEngStateFrm)
  afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
  afx_msg void OnClose();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

//===========================================================================

#endif

//===========================================================================

#endif


