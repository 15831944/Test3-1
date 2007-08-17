//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MDLSLCTU_H
#define __MDLSLCTU_H

#include "gpfuncs.h"
#include "grfcmcmd.h"
#include "..\resource.h"
#include "afxcmn.h"
#ifndef __DOCROOT_H
#include "docroot.h"
#endif

#include "mdlslct.h"

#ifndef __GRFWND_H
#include "grfwnd.h"
#endif
#include "afxwin.h"

//---------------------------------------------------------------------------

class CInsertUnitDlg : public CDialog
  {
  public:
    CInsertUnitDlg(CDlgWhat What, flag IsConstructSymbol, pGrfComCmdBlk Mdl, CWnd* pParent = NULL);
    ~CInsertUnitDlg();
    //{{AFX_VIRTUAL(CInsertUnitDlg)
  public:
    virtual BOOL          PreTranslateMessage(MSG* pMsg);
  protected:
    virtual void          DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
  public:
    void                  SetTag(char * pTag); 
    void                  SetTagHidden(bool Hidden); 
    void                  ChooseNewTag();
    void                  SetClass(char * AClass);
    void                  SetBlockName(char * BlkName, char * UnitDrwGroup, char * UnitClass);
    void                  SetOriginalScales(double XScl, double YScl, double Rotation);

    void                  MakeUniqueTag();
    void                  AdjustMRU(bool DoUpdateLock);
    void                  SaveLastSymbol();
    void                  SelectLastSymbol(LPCTSTR RqdSymbol=NULL);
    void                  FillSymbolList();
    void                  ChangeModel(LPTSTR MdlName, DWORD MdlIndex);
    CString               GetSelMdlName();
    long                  GetSelMdlIndex();
    void                  SetCWin(flag On);
    void                  SetIWin(flag On);
    void                  SetEntityCount(int Entities);
    void                  SetPFSection();
    void                  SetMdlPFSection(LPCTSTR Mdl);
    void                  SetSymPFSection(LPCTSTR Sym);
    LPTSTR                GetGroup();
    bool                  IsUnit();
    bool                  IsConstruct();
    void                  SaveMdlData();
    void                  RecoverMdlScale();
    void                  CompleteForUse(bool ForceUpdateSymLists);
    void                  SaveLastModelData();
    static void           Reset();

    void                  ExportDumpAllSymbols();
    int                   GetSelectedSymbol(CString &Sym);
    bool                  GetSelectedSymbolFile(LPCTSTR Ext, Strng &SymName, Strng &BlkName, Strng &SymPath);
    bool                  LoadBitmap();
    bool                  LoadDXF();
    bool                  LoadDefDraw();
    bool                  LoadSelectedSymbol();
    void                  HoldFeedback(bool On) { m_HoldFeedback+=(On?1:-1); };

    void                  PutEditNumber(CEdit & Ed, double X);
    double                GetEditNumber(CEdit & Ed);

    double                Rotation() { return GetEditNumber(m_Rotate); };
    double                Scl_X()    { return GetEditNumber(m_Scl_X)/100; };
    double                Scl_Y()    { return GetEditNumber(m_Scl_Y)/100; };

  public:
    bool                  m_bMdlValid;
    Strng                 m_ModelClass;
    Strng                 m_DrwGroup;
    Strng                 m_SymbolName;
    Strng                 m_BlockName;

    Strng                 m_CUOrigBlkName;
    Strng                 m_CUOrigSymName;
    bool                  m_bCUOrigSymbolFound;

    //Strng                 m_SymbolPath;
    Strng                 m_BaseTag;
    Strng                 m_Model;
    Strng                 m_Tag;
    flag                  m_bTagChgd;
    int                   m_iPrevSelSymbol;

    //{{AFX_DATA(CInsertUnitDlg)
    enum { IDD = IDD_MDLINSERTUNITDLG };
    CEdit                 m_Rotate;
    CEdit                 m_Scl_X;
    CEdit                 m_Scl_Y;
    BOOL                  m_HideTag;
    BOOL                  m_AllSymbols;
    CTreeCtrl             m_MdlTree;
    //}}AFX_DATA

  protected:

    CDlgWhat              m_What;      

    int                   m_iHoldUpd;
    flag                  m_bInited;
    flag                  m_bUsingPrjSymbols;
    flag                  m_bIsConstructSymbol;

    CImageList            m_MdlSlctImgList;

    static int            sm_LastUnitModel;
    static int            sm_LastLinkModel;
    static int            sm_LastCLnkModel;
    static int            sm_LastELnkModel;
    static int            sm_LastALnkModel;
    static WINDOWPLACEMENT  sm_WP;
    static flag           sm_bWPOk;
    static bool           sm_bCreatedSymFolders;

  protected:
    CString               m_sSection;
    CString               m_sSymSection;
    CString               m_sMdlSection;
    CCustomToolTipCtrl*   m_pTTC;
    GrfComCmdBlk*         m_pMdl;
    CModelListHelper      m_MLH;
    //pStrng                m_pBaseTag;
    CRect                 m_SymRectClient;
    CBitmapFile           m_SymbolBmp;
    int                   m_nEntities;

    CBitmap               IWinBitmap;
    CBitmap               CWinBitmap;

    Grf3D_Display       * m_pDsp;
    DXF_Drawing         * m_pDrw;
    Viewport            * m_pVwPort;

    bool                  m_bCreated;
    bool                  m_DXFLoaded;
    bool                  m_BMPLoaded;
    bool                  m_DefLoaded;

    CArray<CPoint, CPoint&> m_PolyPts;
    CArray<DWORD>           m_PolyPtCnt;
    int                     m_PolyCnt;
    CStringList           m_DrwGrpList;
    CStringList           m_DrwSymbols;

  public:
    CStatic               m_SymbolFrame;
    CUnitDlgListBox       m_SymbolList;
    CEdit                 m_MdlTag;

    int                   m_RecoveringData;
    int                   m_HoldFeedback;

    bool                  m_bSymbolChanged;

    struct
      {
      bool  m_bInUse;
      double m_dXScl;
      double m_dYScl;
      double m_dRot;
      }                   m_OriginalScales;

  protected:
    //{{AFX_MSG(CInsertUnitDlg)
    virtual BOOL          OnInitDialog();
    afx_msg void          OnDestroy();
    virtual void          OnOK();
    virtual void          OnCancel();
    afx_msg void          OnChangeMdltag();
    afx_msg void          OnPaint();
    afx_msg void          OnMdldoc();
    afx_msg void          OnAllsymb();
    afx_msg void          OnUpdateOKBtn(CCmdUI* pCmdUi);
    afx_msg void          OnIwin();
    afx_msg void          OnCwin();
    //}}AFX_MSG
    afx_msg BOOL          OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void          OnTvnSelchangedMdlTree(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void          OnEnChangeMdlsclX();
    afx_msg void          OnEnChangeMdlsclY();
    afx_msg void          OnEnChangeMdlsclRot();
    afx_msg void          OnLbnSelchangeMdlsymbol();

    DECLARE_MESSAGE_MAP()
public:
  CStatic m_DrwGrpStatic;
  afx_msg void OnBnClickedHidetag();
  afx_msg void OnStnClickedStaticframe();
  afx_msg void OnBnClickedOk();
  BOOL m_bExistingModel;
};

//---------------------------------------------------------------------------

#endif
