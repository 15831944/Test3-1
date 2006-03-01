//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __GRFDLGS_H
#define  __GRFDLGS_H

#ifndef __GRFCMDS_H
#include "grfcmds.h"
#endif

//===========================================================================

class CFillColourButton : public CButton
  {
  friend class CCreateFillDlg;
  friend class CUpdateBarDlg;
  friend class CCreateBarDlg;
  public:
    CFillColourButton();
    void          SetColour(COLORREF Colour);
    COLORREF      Colour() { return m_ColourRGB; };

  protected:
    virtual void  DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );

  protected:
    COLORREF      m_ColourRGB;
  };

class CCreateFillDlg : public CDialog
  {
  public:
    CCreateFillDlg(CWnd* pParent, char* pTag, char* pVar, double Min, double Max, int PointsCnt, BOOL ApplyFillToAll, COLORREF FillColour);
    virtual ~CCreateFillDlg();
    //{{AFX_DATA(CCreateFillDlg)
    enum { IDD = IDD_CREATEFILLDLG };
    CString m_Vars;
    BOOL m_ApplyFillToAll;
    double m_Min;
    double m_Max;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CCreateFillDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  public:
    void          SetTag(char* pTag);
    void          SetVar(char* pVar);
    void          SetMin(double d);
    void          SetMax(double d);
    void          SetApplyToAll(BOOL b);
    void          SetPointsCount(int Cnt);
    void          SetFillColour(COLORREF Colour);
    COLORREF      FillColour();
  protected:
    int iPointsCnt;
    CString sTag;
    //{{AFX_MSG(CCreateFillDlg)
    virtual void OnCancel();
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnChangeVars();
    afx_msg void OnDelete();
    //}}AFX_MSG
    afx_msg void OnUpdateOKBtn(CCmdUI* pCmdUi);
    afx_msg void OnBnClickedFillcolourbtn();
    DECLARE_MESSAGE_MAP()

    CFillColourButton m_FillColour;
  };

//===========================================================================

class CCreateBarDlg : public CDialog
  {
  public:
    CCreateBarDlg(CWnd* pParent, Strng_List* TagReq, Strng_List* VarReq, bool TagsOn, double Min, double Max, COLORREF Col, double Scale, byte UseType);
    virtual ~CCreateBarDlg();
    //{{AFX_DATA(CCreateBarDlg)
    enum { IDD = IDD_CREATEBARDLG };
    CString m_TagList;
    CString m_VarList;
    double m_Min;
    double m_Max;
    double m_Scale;
    //}}AFX_DATA
    COLORREF m_Col;
    //{{AFX_VIRTUAL(CCreateBarDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  public:
    void SetMin(double d);
    void SetMax(double d);
    void SetScale(double d);
    void UpdateTagList(BOOL bRetrieve);
    void UpdateVarList(BOOL bRetrieve);
    void SetTagsOn(BOOL TagsOn);
    void SetDoneCreate();
    void          SetFillColour(COLORREF Colour);
    COLORREF      FillColour();
  protected:
    Strng_List* pTagReq;
    Strng_List* pVarReq;
    bool bTagsOn;
    bool bDoneCreate;
    byte iUseType;
    CFillColourButton m_FillColour;

    void UpdateList(BOOL bRetrieve, Strng_List& Lst, CString& s);
    //{{AFX_MSG(CCreateBarDlg)
    afx_msg void OnCreateBtn();
    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    afx_msg void OnChangeTaglist();
    afx_msg void OnChangeVarlist();
    //}}AFX_MSG
    afx_msg void OnUpdateOKBtn(CCmdUI* pCmdUi);
    afx_msg void OnUpdateCreateBtn(CCmdUI* pCmdUi);
    afx_msg void OnBnClickedFillcolourbtn();
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CUpdateBarDlg : public CDialog
  {
  public:
    CUpdateBarDlg(CWnd* pParent, Strng_List* TagReq, Strng_List* VarReq, Strng_List* MinReq, Strng_List* MaxReq, Strng_List* ColReq, DWORD BarsStyle, double Scale, long DispOpts, bool ForBars);
    virtual ~CUpdateBarDlg();
    //{{AFX_DATA(CUpdateBarDlg)
    enum { IDD = IDD_UPDATEBARDLG };
    CHeaderListBox m_BarList;
    CString m_Tag;
    CString m_Var;
    double m_Min;
    double m_Max;
    double m_Scale;
    CComboBox m_DispOpts;
    //}}AFX_DATA
    COLORREF m_Col;
    //{{AFX_VIRTUAL(CUpdateBarDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  public:
    void UpdateLists();
    void          SetFillColour(COLORREF Colour);
    COLORREF      FillColour();
    long          DispOpts() { return m_iDispOpts; };
    BOOL Changed() { return bChanged || m_BarList.Changed(); };
  protected:
    Strng_List* pTagReq;
    Strng_List* pVarReq;
    Strng_List* pMinReq;
    Strng_List* pMaxReq;
    Strng_List* pColReq;
    DWORD dwBarsStyle;
    long m_iDispOpts;
    bool bChanged;
    bool bForBars;
    CFillColourButton m_FillColour;

    void ParseLine(int index);
    //{{AFX_MSG(CUpdateBarDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnAdd();
    afx_msg void OnSelchangeBarlist();
    afx_msg void OnChangeScale();
    afx_msg void OnSelchangeDispOpts();
    //}}AFX_MSG
    afx_msg void OnUpdateEdit(CCmdUI* pCmdUi);
    afx_msg void OnKillfocus(UINT nID);
    afx_msg void OnBnClickedFillcolourbtn();
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CUpdateAnnotDlg : public CDialog
  {
  public:
    CUpdateAnnotDlg(CWnd* pParent, Strng_List* TagReq, Strng_List* VarReq, Strng_List* FmtReq, double Scale);
    virtual ~CUpdateAnnotDlg();
    //{{AFX_DATA(CUpdateAnnotDlg)
    enum { IDD = IDD_UPDATEANNOTDLG };
    CHeaderListBox m_VarList;
    CCustomListBox m_TagList;
    CString m_Tag;
    CString m_Var;
    CString m_Fmt;
    double m_Scale;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CUpdateAnnotDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  public:
    void UpdateLists();
    BOOL Changed() { return (bChanged || m_TagList.Changed()  || m_VarList.Changed()); };
  protected:
    Strng_List* pTagReq;
    Strng_List* pVarReq;
    Strng_List* pFmtReq;
    UCHAR bChanged:1;
    //{{AFX_MSG(CUpdateAnnotDlg)
    afx_msg void OnAdd();
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeTaglist();
    afx_msg void OnSelchangeVarlist();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnAdd2();
    afx_msg void OnChangeScale();
    //}}AFX_MSG
    afx_msg void OnUpdateEdit(CCmdUI* pCmdUi);
    afx_msg void OnKillfocus(UINT nID);
    afx_msg void OnUpdateEdit2(CCmdUI* pCmdUi);
    afx_msg void OnKillfocus2(UINT nID);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CUpdateAnnotDlg1 : public CDialog
  {
  public:
    CUpdateAnnotDlg1(CWnd* pParent, Strng_List* TagReq, Strng_List* TLbReq, Strng_List* VarReq, Strng_List* VLbReq, Strng_List* FmtReq, Strng_List* CvsReq, double Scale);
    virtual ~CUpdateAnnotDlg1();
    //{{AFX_DATA(CUpdateAnnotDlg1)
    enum { IDD = IDD_UPDATEANNOTDLG1 };
    CHeaderListBox m_TagList;
    CHeaderListBox m_VarList;
    CString m_Tag;
    CString m_TagLabel;
    CString m_Var;
    double m_Scale;
    CString m_Cnv;
    CString m_VarLabel;
    CString m_Fmt;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CUpdateAnnotDlg1)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  public:
    void UpdateLists();
    BOOL Changed() { return (bChanged || m_TagList.Changed()  || m_VarList.Changed()); };
  protected:
    Strng_List* pTagReq;
    Strng_List* pTLbReq;
    Strng_List* pVarReq;
    Strng_List* pVLbReq;
    Strng_List* pFmtReq;
    Strng_List* pCvsReq;
    UCHAR bChanged:1;
    //{{AFX_MSG(CUpdateAnnotDlg1)
    afx_msg void OnAdd();
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeTaglist();
    afx_msg void OnSelchangeVarlist();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnAdd2();
    afx_msg void OnChangeScale();
    afx_msg void OnSelchangeFmt();
    //}}AFX_MSG
    afx_msg void OnUpdateEdit(CCmdUI* pCmdUi);
    afx_msg void OnKillfocus(UINT nID);
    afx_msg void OnUpdateEdit2(CCmdUI* pCmdUi);
    afx_msg void OnKillfocus2(UINT nID);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CLayersDlg : public CDialog
  {
  public:
    CLayersDlg(GrfCmdBlk* GrfCmdBlk, CWnd* pParent = NULL);
    virtual ~CLayersDlg();
    //{{AFX_DATA(CLayersDlg)
    enum { IDD = IDD_LAYERS };
    CListCtrl m_LayList;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CLayersDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  public:
    void Refresh();
  protected:
    GrfCmdBlk* pGrfCmdBlk;
    void SetLayer(BOOL Show);
    //{{AFX_MSG(CLayersDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnLayoff();
    afx_msg void OnLayon();
    afx_msg void OnLcolor();
    afx_msg void OnClickLayerlist(NMHDR* pNMHDR, LRESULT* pResult);
    virtual void OnCancel();
    virtual void OnOK();
    //}}AFX_MSG
    afx_msg void OnUpdateBtns(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CUpdateTagsDlg : public CDialog
  {
  public:
    CUpdateTagsDlg(CWnd* pParent = NULL);
    ~CUpdateTagsDlg();
    //{{AFX_DATA(CUpdateTagsDlg)
    enum { IDD = IDD_UPDATEGRFTAGSDLG };
    double m_Size;
    double m_Rotation;
    BOOL m_DoSize;
    BOOL m_DoRotation;
    BOOL m_DoPosition;
    int m_ShowHide;
    //}}AFX_DATA
    void SetDoPosition(BOOL DoPosition);
    void SetDescription(char* pTxt);
    //{{AFX_VIRTUAL(CUpdateTagsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    void DoShowMore();
    CBitmap IWinBitmap;
    CBitmap CWinBitmap;
    static double PrevSize;
    static double PrevRotation;
    static BOOL PrevDoSize;
    static BOOL PrevDoRotation;
    static BOOL PrevDoPosition;
    static int PrevShowHide;
    static BOOL PrevPosOK;
    static int PrevXPos;
    static int PrevYPos;
    //{{AFX_MSG(CUpdateTagsDlg)
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnShowall();
    afx_msg void OnHideall();
    afx_msg void OnApply();
    virtual BOOL OnInitDialog();
    afx_msg void OnDoposition();
    afx_msg void OnDosize();
    afx_msg void OnDorotation();
    afx_msg void OnIwin();
    afx_msg void OnCwin();
    afx_msg void OnNext();
    afx_msg void OnDestroy();
    //}}AFX_MSG
    afx_msg void OnUpdatePosition(CCmdUI* pCmdUi);
    afx_msg void OnUpdateNext(CCmdUI* pCmdUi);
    afx_msg void OnUpdateSize(CCmdUI* pCmdUi);
    afx_msg void OnUpdateRotation(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================
// CScaleRotateDlg dialog

class CScalRotPosHelper
  {
  public:
    double	XScale;
    double	YScale;
    double	Rotation;
    double	XPos;
    double	YPos;

    CScalRotPosHelper()
      { XScale=0.0; YScale=0.0; Rotation=0.0; XPos=0.0; YPos=0.0; };
    BOOL operator !=(CScalRotPosHelper & D)
      { return (XScale!=D.XScale || YScale!=D.YScale || Rotation!=D.Rotation || XPos!=D.XPos || YPos!=D.YPos); };
  };

class CScaleRotateDlg : public CDialog
  {
  public:
    CScaleRotateDlg(CWnd* pParent = NULL);   // standard constructor
    //{{AFX_DATA(CScaleRotateDlg)
    enum { IDD = IDD_GRFSCALEROTATEDLG };
    //}}AFX_DATA
    CScalRotPosHelper Data;
    CScalRotPosHelper PrevData;
    CScalRotPosHelper LastData;
    BOOL m_Changed;
    BOOL m_AppChanged;
    BOOL m_ValidGrf;
    void SetParms(double XPos, double YPos, double XScale, double YScale, double Rotation);
    void UpdateParms(double XPos, double YPos, double XScale, double YScale, double Rotation);
    void SetGroup(double XScale, double YScale, double Rotation);
    void TestChanged();
    //{{AFX_VIRTUAL(CScaleRotateDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  protected:
    void My_DDX_Text(CDataExchange* pDX, int nIDC, double& value, double Default);
    static BOOL PrevPosOK;
    static int PrevXPos;
    static int PrevYPos;
    //{{AFX_MSG(CScaleRotateDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnApply();
    afx_msg void OnRestore();
    afx_msg void OnAccept();
    //}}AFX_MSG
    afx_msg void OnUpdateEdit(CCmdUI* pCmdUi);
    afx_msg void OnChangeEdit(UINT nID);
    afx_msg void OnUpdateApply(CCmdUI* pCmdUi);
    afx_msg void OnUpdateRestore(CCmdUI* pCmdUi);
    afx_msg void OnUpdateAccept(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

//===========================================================================

class CGrfTextDlg : public CDialog
  {
  DECLARE_DYNAMIC(CGrfTextDlg)

  public:
    CGrfTextDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CGrfTextDlg();

    // Dialog Data
    enum { IDD = IDD_GRFTEXTDLG };

  protected:
    bool bChanged;
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
  public:
    CString m_sText;
    double m_TextScale;
    double m_Rotation;
    double m_TextWidth;

    void SetCreateMode(bool CreateMode);
    void SetTextSelected(bool ValidText);
    void SetData(double Size, double Width, double Rotation);
    void SetScale(double Size);
    void SetRotation(double Rotation);
    void SetText(char* Txt);
    void CompleteForUse();

    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedSelectNext();
    afx_msg void OnBnClickedCreateNext();
    afx_msg void OnBnClickedApply();
  };

//===========================================================================

#endif




