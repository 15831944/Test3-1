#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#pragma once

#include "project.h"

// CELnkWiring dialog

//========================================================================
//
//
//
//========================================================================

class CMyEWTreeCtrl;
class CMyEWTreeCtrlData 
  {                
  public:
    BOOL            m_bDragging;
    CMyEWTreeCtrl  *m_pTreeDrag;
    CMyEWTreeCtrl  *m_pSrcTree;
    CMyEWTreeCtrl  *m_pDstTree;
    CRect           m_rctSrcInDstCoOrds;
    CRect           m_rctDstInSrcCoOrds;
    HTREEITEM       m_hItemDrag;
    HTREEITEM       m_hItemDrop;
    CPoint          m_ptHotSpot;
    //CPoint          m_ptOffset;
    //CSize           m_sizeDelta;
    //DWORD		      	m_oldDNDStyle;
    CImageList      *m_pimageTreeDrag;
              
    CMyEWTreeCtrlData()
      {
      m_bDragging=false;
      m_pTreeDrag=NULL;
      m_hItemDrag=NULL;
      m_hItemDrop=NULL;
      m_pimageTreeDrag=NULL;
      }
  };

class CELnkWiring;
class CMyEWTreeCtrl : public CTreeCtrl
  {
  // Construction
  public:
    CMyEWTreeCtrl(CELnkWiring * p);
    CMyEWTreeCtrlData &m_Data;

    // Attributes
  public:
    CELnkWiring &m_Dlg;

    //BOOL            m_bDragging;
    //HTREEITEM       m_hItemDrag;
    //HTREEITEM       m_hItemDrop;
    //CPoint          m_ptHotSpot;
    //CPoint          m_ptOrigin;
    //CSize           m_sizeDelta;
    ////DWORD		      	m_oldDNDStyle;
    //CImageList      *m_pimageTreeDrag;

    // Operations
  public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMyEWTreeCtrl)
  public:
    //}}AFX_VIRTUAL

    // Implementation
  public:
    virtual ~CMyEWTreeCtrl();
    void    OnButtonUp(CPoint point);

    // Generated message map functions
  protected:
    //{{AFX_MSG(CMyEWTreeCtrl)
    afx_msg void OnBeginDrag(LPNMHDR pnmhdr, LRESULT *pResult);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    //}}AFX_MSG
//    afx_msg void OnPaint();

    DECLARE_MESSAGE_MAP()
  };

//========================================================================
//
//
//
//========================================================================

class CTrmData
  {
  public:
    CLinkWiring::CTerm *m_pInfo;
    CString             m_sFullName;
    HTREEITEM           m_hItem;
    long                m_iTS;

    CTrmData()
      {
      m_pInfo     = NULL;
      m_sFullName = "";
      m_hItem     = NULL;
      m_iTS       = 0;
      }
    CTrmData(CLinkWiring::CTerm * pInfo, LPCTSTR sFullName, HTREEITEM hItem)
      {
      m_pInfo     = pInfo;
      m_sFullName = sFullName;
      m_hItem     = hItem;
      m_iTS       = 0;
      }
  };
class CTSData
  {
  public:
    HTREEITEM           m_hItem;
    long                m_nConnected;
    long                m_nTerms;

    CTSData()
      {
      m_hItem       = NULL;
      m_nTerms      = 0;
      m_nConnected  = 0;
      }

    CTSData(HTREEITEM hItem, long nTerms)
      {
      m_hItem       = hItem;
      m_nTerms      = nTerms;
      m_nConnected  = 0;
      };
  };

class CELnkWiring : public CDialog
  {
  DECLARE_DYNAMIC(CELnkWiring)

  public:
    CELnkWiring(LPCTSTR pTag, CWnd* pParent = NULL);   // standard constructor
    virtual ~CELnkWiring();

    static void Start(LPCTSTR pTag);

    // Dialog Data
    enum { IDD = IDD_ELNKWIRING };

  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
  public:
    //CListCtrl           m_SrcTerms;
    //CListCtrl           m_DstTerms;
    CListCtrl           m_Wiring;
    CMyEWTreeCtrl       m_SrcTree;
    CMyEWTreeCtrl       m_DstTree;
    CImageList          m_ImgList;
    CString             m_LnkTag;
    CLinkWiring         m_LW;
    CNodeWiring         m_SNW;
    CNodeWiring         m_DNW;
    CStatic             m_WireBad;
    CStatic             m_SrcNdTag;
    CStatic             m_DstNdTag;
    CMyEWTreeCtrlData   m_TreeData;
    //CArray<CString, CString&> m_SrcTags, m_DstTags;

    CArray <CTSData, CTSData&>    m_SrcTS,  m_DstTS;  
    CArray <CTrmData, CTrmData&>  m_SrcTrm, m_DstTrm;  

    virtual BOOL OnInitDialog(void);
    //afx_msg void OnNMDblclkSrcterms(NMHDR *pNMHDR, LRESULT *pResult);
    //afx_msg void OnNMDblclkDstterms(NMHDR *pNMHDR, LRESULT *pResult);
    // Adds a wire to m_Wiring
    bool AddAWire(int iSrc, int iDst);
    void UpdateImages();
    //bool AddAWire(HTREEITEM hSrc, HTREEITEM hDst);
    afx_msg void OnBnClickedWireadd();
    afx_msg void OnBnClickedWiredel();
    //afx_msg void OnBnClickedWirematch();
    //afx_msg void OnBnClickedWirematchts();
    afx_msg void OnBnClickedWirematchOut2In();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    
    //afx_msg void OnTvnBegindragSrctree(NMHDR *pNMHDR, LRESULT *pResult);
    //afx_msg void OnTvnBegindragDsttree(NMHDR *pNMHDR, LRESULT *pResult);
    //afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    //afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    //afx_msg void OnNMDblclkSrctree(NMHDR *pNMHDR, LRESULT *pResult);
    //afx_msg void OnNMDblclkDsttree(NMHDR *pNMHDR, LRESULT *pResult);
  };

//========================================================================
//
//
//
//========================================================================
