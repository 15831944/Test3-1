//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MDLSLCTL_H
#define __MDLSLCTL_H

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

class CInsertLinkDlg : public CDialog
  {
  public:
    CInsertLinkDlg(LPCTSTR Group, GrfComCmdBlk* Mdl, LPCTSTR SrcTS, LPCTSTR DstTS, CWnd* pParent = NULL);
    CInsertLinkDlg(GrfComCmdBlk* Mdl, LPCTSTR LnkTag, CWnd* pParent = NULL);
    ~CInsertLinkDlg();
    static void Reset();
  public:
    void                  SetData(double ArrowScale, bool Hide);
    void                  SetTag(char* pTag); 
    void                  SetPoints(int Points); 
    void                  CompleteForUse();
    int                   NoOfModels() { return nModels; };

    static bool           DrawOrth() { return sm_bDrawOrth; };
    static void           SetDrawOrth(bool b) { sm_bDrawOrth=b; };
    static bool           DrawSingleSeg() { return sm_bDrawSingleSeg; };
    static int            DrawMethod() { return sm_iLinkDrawMethod; };
    static int            ArrowRuleMethod() { return sm_iArrowRuleMethod; };
    static double         LineWidth() { return sm_LineWidthMap[sm_iLineWidth]; };

  protected:
    //{{AFX_VIRTUAL(CInsertLinkDlg)
  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL
    //{{AFX_MSG(CInsertLinkDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnBack();
    afx_msg void OnOrtho();
    afx_msg void OnFree();
    afx_msg void OnBreak();
    afx_msg void OnSelchangeModelList();
    afx_msg void OnSelchangeDrawMethodList();
    afx_msg void OnSelchangeArrowRuleList();
    afx_msg void OnMdldoc();
    afx_msg void OnChangeMdltag();
    //}}AFX_MSG
    afx_msg void OnUpdateOKBtn(CCmdUI* pCmdUi);
    afx_msg void OnUpdateBackBtn(CCmdUI* pCmdUi);
    afx_msg void OnUpdateBreakBtn(CCmdUI* pCmdUi);
    afx_msg void OnBnClickedOk();
    DECLARE_MESSAGE_MAP()

  public:
    //{{AFX_DATA(CInsertLinkDlg)
    enum { IDD = IDD_MDLINSERTLINKDLG };
    BOOL	  m_HideTag;
    double	m_ArrowScale;
    //}}AFX_DATA

    Strng                 m_ModelName;
    Strng                 m_Model;
    Strng                 m_Tag;
    Strng                 m_BaseTag;
    Strng                 m_Group;
    bool                  m_bIsCable;
    Strng                 m_SrcTS;
    Strng                 m_DstTS; 
    BOOL                  m_AutoWire;

  protected:
    int                   nPoints;
    bool                  m_bRedraw;
    flag                  m_bTagChgd;
    int                   m_iPrevSel;
    int                   nModels;
    GrfComCmdBlk*         m_pMdl;
    CModelListHelper      m_MLH;

    CBitmap               OrthoBitmap;
    CBitmap               FreeBitmap;
    CBitmap               BreakBitmap;
    CBitmap               OrthoOffBitmap;
    CBitmap               FreeOffBitmap;
    CBitmap               BreakOffBitmap;

    static int            sm_LastConn;
    static WINDOWPLACEMENT sm_WP;
    static bool           sm_bWPOk;
    static bool           sm_bDrawSingleSeg;
    static int            sm_iLinkDrawMethod;
    static int            sm_iArrowRuleMethod;
    static int            sm_iLineWidth;
    static double         sm_LineWidthMap[3];

  public:
    static bool           sm_bDrawOrth;
    afx_msg void OnCbnSelchangeLinkwidth();
  };

//---------------------------------------------------------------------------
#endif
