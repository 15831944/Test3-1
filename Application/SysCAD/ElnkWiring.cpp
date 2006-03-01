// ElnkWiring.cpp : implementation file
//

#include "stdafx.h"
#include "SysCAD.h"
#include "ElnkWiring.h"


// ELnkWiring.cpp : implementation file
//

#include "stdafx.h"
#include "SysCAD.h"
#include "ELnkWiring.h"
#include ".\elnkwiring.h"
#include "sfe_clnt.h"

const DWORD TSMark=0x80000000;

//========================================================================
//
//
//
//========================================================================

CMyEWTreeCtrl::CMyEWTreeCtrl(CELnkWiring * p) :  m_Dlg(*p),  m_Data(p->m_TreeData)
  {
  }

CMyEWTreeCtrl::~CMyEWTreeCtrl()
  {
  }


BEGIN_MESSAGE_MAP(CMyEWTreeCtrl, CTreeCtrl)
  //{{AFX_MSG_MAP(CMyEWTreeCtrl)
  ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
  ON_NOTIFY_REFLECT(TVN_BEGINRDRAG, OnBeginDrag)
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONUP()
//  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEWTreeCtrl message handlers
void CMyEWTreeCtrl::OnBeginDrag(LPNMHDR pnmhdr, LRESULT* /*pResult*/)
  {
  CPoint          ptItem, ptAction, ptImage;
  CRect           rctItem, rctClient;//, ptAction, ptImage;
  NM_TREEVIEW     *pnmTreeView = (NM_TREEVIEW *)pnmhdr;

  ASSERT(!m_Data.m_bDragging);
  m_Data.m_bDragging = TRUE;
  m_Data.m_hItemDrag = pnmTreeView->itemNew.hItem;
  m_Data.m_pTreeDrag = this;
  ptAction = pnmTreeView->ptDrag;//action;
  GetItemRect(m_Data.m_hItemDrag, &rctItem, true);  // ptItem is relative to (0,0) and not the view origin

  SelectItem(m_Data.m_hItemDrag);
  SetItemState(m_Data.m_hItemDrag, 0, TVIS_DROPHILITED);
  
  ASSERT(m_Data.m_pimageTreeDrag == NULL);
  m_Data.m_pimageTreeDrag = CreateDragImage(m_Data.m_hItemDrag);//, &ptImage);
  m_Data.m_ptHotSpot = ptAction - rctItem.TopLeft();  // calculate hotspot for the cursor

  m_Data.m_ptHotSpot.x+=12; // allow for image of 12 pixels
  ClientToScreen(&ptAction);

  m_Data.m_pimageTreeDrag->DragShowNolock(TRUE);  // lock updates and show drag image
  m_Data.m_pimageTreeDrag->SetDragCursorImage(0, m_Data.m_ptHotSpot);//CPoint(0, 0));// define the hot spot for the new cursor image
  m_Data.m_pimageTreeDrag->BeginDrag(0, m_Data.m_ptHotSpot);//CPoint(0, 0));
  m_Data.m_pimageTreeDrag->DragEnter(NULL, ptAction);
  m_Data.m_pimageTreeDrag->DragMove(ptAction);  // move image to overlap original icon
  m_Data.m_hItemDrop = NULL;// no drop target

  SetCapture();
  }

void CMyEWTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
  {
  if (m_Data.m_bDragging)
    {

    if (m_Data.m_pSrcTree==this)
      {
      if (m_Data.m_rctDstInSrcCoOrds.PtInRect(point))
        {
        ::ReleaseCapture();
        m_Data.m_pDstTree->SetCapture();
        }
      }
    if (m_Data.m_pDstTree==this)
      {
      if (m_Data.m_rctSrcInDstCoOrds.PtInRect(point))
        {
        ::ReleaseCapture();
        m_Data.m_pSrcTree->SetCapture();
        }
      }

    ASSERT(m_Data.m_pimageTreeDrag != NULL);
    CPoint WPoint=point;
    ClientToScreen(&WPoint);
    if (m_Data.m_pimageTreeDrag)
      m_Data.m_pimageTreeDrag->DragMove(WPoint); // move the image 

    HTREEITEM     hItem;
    if ((hItem = HitTest(point)) != NULL)
      {
      if (m_Data.m_hItemDrop != NULL)  // remove the drophighlighted from previous item
        {
        m_Data.m_pimageTreeDrag->DragLeave(NULL);
        SetItemState(m_Data.m_hItemDrop, 0, TVIS_DROPHILITED);
        }

      m_Data.m_hItemDrop = hItem;
      m_Data.m_pimageTreeDrag->DragLeave(NULL); // unlock the window and hide drag image

      if (m_Data.m_pTreeDrag != this)
        SetItemState(hItem, TVIS_DROPHILITED, TVIS_DROPHILITED);
      
      UpdateWindow();

      m_Data.m_pimageTreeDrag->DragEnter(NULL, WPoint);  // lock updates and show drag image
      }
    }
  CTreeCtrl::OnMouseMove(nFlags, point);
  }


void CMyEWTreeCtrl::OnButtonUp(CPoint point)
  {
  if (m_Data.m_bDragging)  // end of the drag operation
    {
    //long        lStyle;
    CString     cstr;


    m_Data.m_bDragging = FALSE;
    ASSERT(m_Data.m_pimageTreeDrag != NULL);
    m_Data.m_pimageTreeDrag->DragLeave(NULL);

    SetItemState(m_Data.m_hItemDrop, 0, TVIS_DROPHILITED);
    SelectItem(m_Data.m_hItemDrop);

    m_Data.m_pimageTreeDrag->EndDrag();
    delete m_Data.m_pimageTreeDrag;
    m_Data.m_pimageTreeDrag = NULL;

    if (m_Data.m_pTreeDrag && (m_Data.m_pTreeDrag!=this))
      {
      DWORD iSrc;
      DWORD iDst;
      if (m_Data.m_pTreeDrag==m_Data.m_pSrcTree)
        {
        iSrc=m_Data.m_pSrcTree->GetItemData(m_Data.m_hItemDrag);
        iDst=m_Data.m_pDstTree->GetItemData(m_Data.m_hItemDrop);
        }
      else
        {
        iSrc=m_Data.m_pSrcTree->GetItemData(m_Data.m_hItemDrop);
        iDst=m_Data.m_pDstTree->GetItemData(m_Data.m_hItemDrag);
        }

      if ((iSrc&TSMark)!=0 && (iDst&TSMark)!=0)
        {
        DWORD iSrc0=(iSrc&0x7fff0000)>>16;
        DWORD iSrcN=iSrc&0x7fff;
        DWORD iDst0=(iDst&0x7fff0000)>>16; 
        DWORD iDstN=iDst&0x7fff;
        if ((iSrcN-iSrc0)==(iDstN-iDst0))
          {
          DWORD id=iDst0;
          DWORD is=iSrc0; 
          while(is<=iSrcN)
            m_Dlg.AddAWire(is++, id++);
          }
        else
          m_Dlg.m_WireBad.SetWindowText("Mismatched Terminal count");
        }
      else if ((iSrc&TSMark)==0 && (iDst&TSMark)==0)
        {
        m_Dlg.AddAWire(iSrc, iDst);
        }
      else
        m_Dlg.m_WireBad.SetWindowText("Must select either Terminals or TermStrips");
      m_Dlg.UpdateImages();
      }

    ::ReleaseCapture();
    }
  }

void CMyEWTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
  {
  OnButtonUp(point);
  CTreeCtrl::OnLButtonUp(nFlags, point);
  }

void CMyEWTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
  {
  OnButtonUp(point);
  CTreeCtrl::OnRButtonUp(nFlags, point);
  }

//void CMyEWTreeCtrl::OnPaint() 
//  {
//  CTreeCtrl::OnPaint();
//
//  CPaintDC dc(this); // device context for painting
//  CDCResChk ResChk(dc);
//	
//  CRect ClientRct;
//  GetClientRect(&ClientRct);
//
//  CPen Pen;
//  Pen.CreatePen(PS_SOLID, 0, RGB(0,0,255));
//  CPen* OldPen = dc.SelectObject(&Pen);
//  //RECT Rect;
//  //GetClientRect(&Rect);
//  //dc.Rectangle(&Rect);
//
//  dc.MoveTo(ClientRct.left, ClientRct.bottom);
//  dc.LineTo(ClientRct.right, ClientRct.top);
//  }

//========================================================================
//
//
//
//========================================================================
// CELnkWiring dialog

IMPLEMENT_DYNAMIC(CELnkWiring, CDialog)
CELnkWiring::CELnkWiring(LPCTSTR pTag, CWnd* pParent /*=NULL*/)
	: CDialog(CELnkWiring::IDD, pParent)
  , m_LnkTag(pTag)
  , m_LW(pTag)
  , m_SrcTree(this)
  , m_DstTree(this)
  {

  Create(CELnkWiring::IDD, pParent);
  }

CELnkWiring::~CELnkWiring()
  {
  }

void CELnkWiring::Start(LPCTSTR pTag)
  {
  CELnkWiring * pELnk=new CELnkWiring(pTag, AfxGetMainWnd());
  pELnk->SetWindowPos(&CWnd::wndTop, 0,0,0,0,SWP_NOSIZE|SWP_SHOWWINDOW|SWP_NOACTIVATE);
  pELnk->CenterWindow(AfxGetMainWnd());
  };

void CELnkWiring::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //DDX_Control(pDX, IDC_SRCTERMS, m_SrcTerms);
  //DDX_Control(pDX, IDC_DSTTERMS, m_DstTerms);
  DDX_Control(pDX, IDC_WIRED, m_Wiring);
  DDX_Control(pDX, IDC_WIREBADSTATIC, m_WireBad);
  DDX_Control(pDX, IDC_SRCNDTAG, m_SrcNdTag);
  DDX_Control(pDX, IDC_DSTNDTAG, m_DstNdTag);
  DDX_Control(pDX, IDC_SRCTREE, m_SrcTree);
  DDX_Control(pDX, IDC_DSTTREE, m_DstTree);
  }

                    
BEGIN_MESSAGE_MAP(CELnkWiring, CDialog)
  //ON_NOTIFY(NM_DBLCLK, IDC_SRCTERMS, OnNMDblclkSrcterms)
  //ON_NOTIFY(NM_DBLCLK, IDC_DSTTERMS, OnNMDblclkDstterms)
  ON_BN_CLICKED(IDC_WIREADD, OnBnClickedWireadd)
  ON_BN_CLICKED(IDC_WIREDEL, OnBnClickedWiredel)
  //ON_BN_CLICKED(IDC_WIREMATCH, OnBnClickedWirematch)
  //ON_BN_CLICKED(IDC_WIREMATCHTS, OnBnClickedWirematchts)
  ON_BN_CLICKED(IDC_WIREMATCHIO, OnBnClickedWirematchOut2In)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
//  ON_NOTIFY(TVN_BEGINDRAG, IDC_SRCTREE, OnTvnBegindragSrctree)
//  ON_NOTIFY(TVN_BEGINDRAG, IDC_DSTTREE, OnTvnBegindragDsttree)
//  ON_WM_LBUTTONUP()
//  ON_WM_MOUSEMOVE()
//ON_NOTIFY(NM_DBLCLK, IDC_SRCTREE, OnNMDblclkSrctree)
//ON_NOTIFY(NM_DBLCLK, IDC_DSTTREE, OnNMDblclkDsttree)
END_MESSAGE_MAP()

// CELnkWiring message handlers

BOOL CELnkWiring::OnInitDialog(void)
  {
  CDialog::OnInitDialog();


  m_TreeData.m_pSrcTree=&m_SrcTree;
  m_TreeData.m_pDstTree=&m_DstTree;
  m_SrcTree.GetClientRect(&m_TreeData.m_rctSrcInDstCoOrds);
  m_SrcTree.ClientToScreen(&m_TreeData.m_rctSrcInDstCoOrds);
  m_DstTree.ScreenToClient(&m_TreeData.m_rctSrcInDstCoOrds);

  m_DstTree.GetClientRect(&m_TreeData.m_rctDstInSrcCoOrds);
  m_DstTree.ClientToScreen(&m_TreeData.m_rctDstInSrcCoOrds);
  m_SrcTree.ScreenToClient(&m_TreeData.m_rctDstInSrcCoOrds);

  CBitmap BM;                                           
  VERIFY(BM.LoadBitmap(IDB_TSTRIPCONN));
  VERIFY(m_ImgList.Create(12, 12, FALSE, 0, 3));
  VERIFY(m_ImgList.Add(&BM, (CBitmap*)NULL)>=0);
  m_SrcTree.SetImageList(&m_ImgList, TVSIL_NORMAL);
  m_DstTree.SetImageList(&m_ImgList, TVSIL_NORMAL);

  //m_SrcTerms.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  //m_SrcTerms.InsertColumn(0, "No", LVCFMT_LEFT, 20);
  //m_SrcTerms.InsertColumn(1, "Terminal",  LVCFMT_LEFT, 170);
  //
  //m_DstTerms.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  //m_DstTerms.InsertColumn(0, "No", LVCFMT_LEFT, 20);
  //m_DstTerms.InsertColumn(1, "Terminal",  LVCFMT_LEFT, 170);

  m_Wiring.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
  m_Wiring.InsertColumn(0, "No", LVCFMT_LEFT,  20);
  m_Wiring.InsertColumn(1, "Source", LVCFMT_RIGHT, 170);
  m_Wiring.InsertColumn(2, "Destination", LVCFMT_LEFT, 170);

  gs_pPrj->GetLinkWiring(m_LW);

  //CString N;
  //m_SrcTags.SetSize(m_LW.m_SrcTerminals.GetSize());
  //m_DstTags.SetSize(m_LW.m_DstTerminals.GetSize());
  //for (int i=0; i<m_LW.m_SrcTerminals.GetSize(); i++)
  //  {
  //  N.Format("%i", i+1);
  //  m_SrcTags[i]=m_LW.m_SrcTerminals[i].m_sTermStripName+":"+m_LW.m_SrcTerminals[i].m_sTerminalName;
  //  m_SrcTerms.InsertItem(i, N);
  //  m_SrcTerms.SetItemText(i,1,m_SrcTags[i]);
  //  m_SrcTerms.SetItemData(i, i);
  //  }
  //
  //for (int i=0; i<m_LW.m_DstTerminals.GetSize(); i++)
  //  {
  //  N.Format("%i", i+1);
  //  //m_DstTags[i]=m_LW.m_DstTerminals[i].m_sTermStripName+":"+m_LW.m_DstTerminals[i].m_sTerminalName;
  //  m_DstTerms.InsertItem(i, N);
  //  m_DstTerms.SetItemText(i,1,m_DstTags[i]);
  //  m_DstTerms.SetItemData(i, i);
  //  m_DstTrm[i].m_sFullName=m_LW.m_DstTerminals[i].FullName();
  //  }

  m_SrcTS.SetSize(0,16);
  m_DstTS.SetSize(0,16);
  m_SrcTrm.SetSize(m_LW.m_SrcTerminals.GetSize());
  m_DstTrm.SetSize(m_LW.m_DstTerminals.GetSize());

  HTREEITEM hTS=NULL;
  CString PrvTS;
  long iStart=-1;
  for (int i=0; i<m_LW.m_SrcTerminals.GetSize(); i++)
    {
    CLinkWiring::CTerm &I=m_LW.m_SrcTerminals[i];
    
    if (0)
      {
      CString S;
      I.FullName(S);
      dbgpln("Src %3i) %3i %s", i, I.m_iWire, (LPCTSTR)S);
      }

    if (!hTS || PrvTS!=I.m_sTermStripName)
      {
      hTS=m_SrcTree.InsertItem(I.m_sTermStripName, TVI_ROOT);
      m_SrcTree.Expand(hTS, TVE_EXPAND);
      m_SrcTree.SetItemImage(hTS, 0,0);
      PrvTS=I.m_sTermStripName;
      iStart=i;
      m_SrcTS.Add(CTSData(hTS, 0));
      };
    HTREEITEM hItm=m_SrcTree.InsertItem(I.m_sTerminalName, hTS);
    m_SrcTree.SetItemData(hTS, TSMark|i|(iStart<<16));
    m_SrcTree.SetItemData(hItm, i);

    m_SrcTrm[i].m_pInfo=&m_LW.m_SrcTerminals[i];
    m_SrcTrm[i].m_sFullName=I.FullName();
    m_SrcTrm[i].m_hItem=hItm;
    m_SrcTrm[i].m_iTS=m_SrcTS.GetUpperBound();
    m_SrcTS[m_SrcTS.GetUpperBound()].m_nTerms++;
    }

  hTS=NULL;
  PrvTS="";
  iStart=-1;
  for (i=0; i<m_LW.m_DstTerminals.GetSize(); i++)
    {
    CLinkWiring::CTerm &I=m_LW.m_DstTerminals[i];
    if (0)
      {
      CString S;
      I.FullName(S);
      //dbgpln("Dst %3i) %3i %s", i, I.m_iWire, (LPCTSTR)S);
      }

    if (!hTS || PrvTS!=I.m_sTermStripName)
      {
      hTS=m_DstTree.InsertItem(I.m_sTermStripName, TVI_ROOT);
      m_DstTree.SetItemImage(hTS, 0,0);
      m_DstTree.Expand(hTS, TVE_EXPAND);
      PrvTS=I.m_sTermStripName;
      iStart=i;
      m_DstTS.Add(CTSData(hTS, 0));
      };
    HTREEITEM hItm=m_DstTree.InsertItem(I.m_sTerminalName, hTS);
    m_DstTree.SetItemData(hTS, TSMark|i|(iStart<<16));
    m_DstTree.SetItemData(hItm, i);

    m_DstTrm[i].m_pInfo=&m_LW.m_DstTerminals[i];
    m_DstTrm[i].m_sFullName=I.FullName();
    m_DstTrm[i].m_hItem=hItm;
    m_DstTrm[i].m_iTS=m_DstTS.GetUpperBound();
    m_DstTS[m_DstTS.GetUpperBound()].m_nTerms++;
    }

  for (int w=0; ; w++)
    {
    for (int iSrc=m_SrcTrm.GetSize()-1; iSrc>=0; iSrc--)
      if (m_SrcTrm[iSrc].m_pInfo->m_iWire==w)
        break;
    for (int iDst=m_DstTrm.GetSize()-1; iDst>=0; iDst--)
      if (m_DstTrm[iDst].m_pInfo->m_iWire==w)
        break;
    if (iSrc>=0 && iDst>=0)
      {
      CString Src=m_SrcTrm[iSrc].m_sFullName;
      CString Dst=m_DstTrm[iDst].m_sFullName;
      //long SrcN=m_SrcTerms.GetItemData(iSrc);
      //long DstN=m_DstTerms.GetItemData(iDst);

      long n=m_Wiring.GetItemCount();
      CString No;
      No.Format("%i", n+1);

      m_Wiring.InsertItem(n,No);
      m_Wiring.SetItemText(n,1,Src);
      m_Wiring.SetItemText(n,2,Dst);
      m_Wiring.SetItemData(n,(iDst<<16)|iSrc);
      }
    else if (iSrc>=0 || iDst>=0)
      {
      LogError("Wiring", 0, "Missing wire");
      }
    else
      break;
    }

  UpdateImages();

  m_SrcNdTag.SetWindowText(m_LW.m_SrcNdTag+" Terminals");
  m_DstNdTag.SetWindowText(m_LW.m_DstNdTag+" Terminals");

  CenterWindow(AfxGetMainWnd());
  return 0;
  }

//void CELnkWiring::OnNMDblclkSrcterms(NMHDR *pNMHDR, LRESULT *pResult)
//  {
//  m_WireBad.SetWindowText("");
//  long iSrc=m_SrcTerms.GetSelectedCount()>0 ? m_SrcTerms.GetSelectionMark() : -1;
//  long iDst=m_DstTerms.GetSelectedCount()>0 ? m_DstTerms.GetSelectionMark() : -1;
//  if (iSrc>=0 && iDst>=0)
//    AddAWire(iSrc, iDst);
//
//  *pResult = 0;
//  }
//
//void CELnkWiring::OnNMDblclkDstterms(NMHDR *pNMHDR, LRESULT *pResult)
//  {
//  m_WireBad.SetWindowText("");
//  long iSrc=m_SrcTerms.GetSelectedCount()>0 ? m_SrcTerms.GetSelectionMark() : -1;
//  long iDst=m_DstTerms.GetSelectedCount()>0 ? m_DstTerms.GetSelectionMark() : -1;
//  if (iSrc>=0 && iDst>=0)
//    AddAWire(iSrc, iDst);
//
//  *pResult = 0;
//  }

// Adds a wire to m_Wiring
bool CELnkWiring::AddAWire(int iSrc, int iDst)
  {
  if (iSrc>=0 && iDst>=0)
    {
    if (m_SrcTrm[iSrc].m_pInfo->m_iWire<0 && m_DstTrm[iDst].m_pInfo->m_iWire<0 )
      {
      CString Src=m_SrcTrm[iSrc].m_sFullName;
      CString Dst=m_DstTrm[iDst].m_sFullName;

      long n=m_Wiring.GetItemCount();
      CString No;
      No.Format("%i", n+1);

      m_Wiring.InsertItem(n,No);
      m_Wiring.SetItemText(n,1,Src);
      m_Wiring.SetItemText(n,2,Dst);
      m_Wiring.SetItemData(n,(iDst<<16)|iSrc);

      m_LW.m_SrcTerminals[iSrc].m_iWire=n;
      m_LW.m_DstTerminals[iDst].m_iWire=n;
      }
    else
      {
      //if (m_SrcTrm[iSrc].m_pInfo->m_iWire!=m_DstTrm[iDst].m_pInfo->m_iWire)
      //  m_WireBad.SetWindowText("One wire added");
      }
    return true;
    }

  return false;
  }
// Adds a wire to m_Wiring

void CELnkWiring::UpdateImages()
  {
  for (int i=0; i<m_SrcTS.GetSize(); i++)
    m_SrcTS[i].m_nConnected=0;
  for (i=0; i<m_DstTS.GetSize(); i++)
    m_DstTS[i].m_nConnected=0;

  //m_SrcTS
  for (i=0; i<m_SrcTrm.GetSize(); i++)
    {
    CTrmData &I=m_SrcTrm[i];
    long iImg=3;
    if (I.m_pInfo->m_iWire>=0)
      {
      iImg=4+(I.m_pInfo->m_iWire%7);
      m_SrcTS[I.m_iTS].m_nConnected++;
      }
    m_SrcTree.SetItemImage(I.m_hItem, iImg, iImg);
    }

  for (i=0; i<m_DstTrm.GetSize(); i++)
    {
    CTrmData &I=m_DstTrm[i];
    long iImg=3;
    if (I.m_pInfo->m_iWire>=0)
      {
      iImg=4+(I.m_pInfo->m_iWire%7);
      m_DstTS[I.m_iTS].m_nConnected++;
      }
    m_DstTree.SetItemImage(I.m_hItem, iImg, iImg);
    }
  for (i=0; i<m_SrcTS.GetSize(); i++)
    {
    CTSData &I=m_SrcTS[i];
    int iImg=I.m_nConnected==0?0:I.m_nConnected==I.m_nTerms?2:1;
    m_SrcTree.SetItemImage(I.m_hItem, iImg, iImg);
    }
  for (i=0; i<m_DstTS.GetSize(); i++)
    {
    CTSData &I=m_DstTS[i];
    int iImg=I.m_nConnected==0?0:I.m_nConnected==I.m_nTerms?2:1;
    m_DstTree.SetItemImage(I.m_hItem, iImg, iImg);
    }
  }
//bool CELnkWiring::AddAWire(HTREEITEM hSrc, HTREEITEM hDst)
//  {
//  if (hSrc>=0 && hDst>=0)
//    {
//
//    CString Src=m_SrcTree.GetItemText(hSrc);
//    CString Dst=m_DstTree.GetItemText(hDst);
//    long SrcN=m_SrcTree.GetItemData(hSrc);
//    long DstN=m_DstTree.GetItemData(hDst);
//
//    long n=m_Wiring.GetItemCount();
//    CString No;
//    No.Format("%i", n+1);
//
//    m_Wiring.InsertItem(n,No);
//    m_Wiring.SetItemText(n,1,Src);
//    m_Wiring.SetItemText(n,2,Dst);
//    m_Wiring.SetItemData(n,(DstN<<16)|SrcN);
//
//    //xm_SrcTree.DeleteItem(iSrc);
//    //xm_DstTree.DeleteItem(iDst);
//
//    return true;
//    }
//
//  return false;
//  }

void CELnkWiring::OnBnClickedWireadd()
  {
  m_WireBad.SetWindowText("");

  HTREEITEM hSrc=m_SrcTree.GetSelectedItem();
  HTREEITEM hDst=m_DstTree.GetSelectedItem();

  if (hSrc && hDst)
    {
    long iSrc=m_SrcTree.GetItemData(hSrc);
    long iDst=m_DstTree.GetItemData(hDst);
    if (iSrc>=0 && iDst>=0)
      {
      AddAWire(iSrc, iDst);
      m_WireBad.SetWindowText("One wire added");
      }
    }
  else
    m_WireBad.SetWindowText("Bad Selection - Select one of each");
  
  UpdateImages();

//  long nSrc=m_SrcTerms.GetSelectedCount();
//  long nDst=m_DstTerms.GetSelectedCount();
//  if (nSrc==1 && nDst==1)
//    {
//    long iSrc=m_SrcTerms.GetSelectionMark();
//    long iDst=m_DstTerms.GetSelectionMark();
//    if (iSrc>=0 && iDst>=0)
//      {
//      AddAWire(iSrc, iDst);
//      m_WireBad.SetWindowText("One wire added");
//      }
//    }
//  else
//    m_WireBad.SetWindowText("Bad Selection - Select one of each");

  }

void CELnkWiring::OnBnClickedWiredel()
  {
  CString No;
  m_WireBad.SetWindowText("");
  bool DoingSelected=(m_Wiring.GetSelectedCount()>0);
  long N=0;
  for (;;)
    {
    int i;
    if (DoingSelected)
      {
      if (m_Wiring.GetSelectedCount()==0)
        goto Done;
      i=m_Wiring.GetSelectionMark();
      }
    else
      {
      if (m_Wiring.GetItemCount()==0)
        goto Done;
      i=0;
      }

    CString Src=m_Wiring.GetItemText(i,1);
    CString Dst=m_Wiring.GetItemText(i,2);
    DWORD iSrc=m_Wiring.GetItemData(i)&0xffff;
    DWORD iDst=(m_Wiring.GetItemData(i)&0xffff0000)>>16;
    m_Wiring.DeleteItem(i);
    m_SrcTrm[iSrc].m_pInfo->m_iWire=-1;
    m_DstTrm[iDst].m_pInfo->m_iWire=-1;

    N++;
    }
 
Done:;
  UpdateImages();
  No.Format("%i Wires removed", N);
  m_WireBad.SetWindowText(No);
  }

//void CELnkWiring::OnBnClickedWirematch()
//  {
//  m_WireBad.SetWindowText("");
//  //long nSrc=m_SrcTerms.GetSelectedCount();
//  //long nDst=m_DstTerms.GetSelectedCount();
//  //CString Src, Dst;
//  //if (nSrc!=nDst)
//  //  {
//  //  m_WireBad.SetWindowText("Bad Selection - Same number of wires required (or None)");
//  //  return;
//  //  }
//  long N=0;
//  //for (int iSrc=0; iSrc<m_SrcTerms.GetItemCount(); iSrc++)
//  //  {
//  //  Src=m_SrcTerms.GetItemText(iSrc,1);
//  //  for (int iDst=0; iDst<m_DstTerms.GetItemCount(); iDst++)
//  //    {
//  //    Dst=m_DstTerms.GetItemText(iDst,1);
//  //    if (Src.CompareNoCase(Dst)==0)
//  //      {
//  //      AddAWire(iSrc,iDst);
//  //      iSrc--; // in anticipation of increment
//  //      N++;
//  //      break;  //out of Dst Loop
//  //      }
//  //    }
//  //  }
//  CString Msg;
//  Msg.Format("%i Wires added", N);
//  m_WireBad.SetWindowText(Msg);
//  UpdateImages();
//  }
//
//void CELnkWiring::OnBnClickedWirematchts()
//  {
//  m_WireBad.SetWindowText("");
//  //long nSrc=m_SrcTerms.GetSelectedCount();
//  //long nDst=m_DstTerms.GetSelectedCount();
//  //if (nSrc!=1 || nDst!=1)
//  //  {
//  //  m_WireBad.SetWindowText("Bad Selection - One in each required");
//  //  return;
//  //  }
//  //
//  //CString SrcTS=m_SrcTerms.GetItemText(m_SrcTerms.GetSelectionMark(), 1);
//  //long SrcLen=SrcTS.Find(':');
//  //if (SrcLen<0)
//  //  return;
//  //
//  //CString DstTS=m_DstTerms.GetItemText(m_DstTerms.GetSelectionMark(), 1);
//  //long DstLen=DstTS.Find(':');
//  //if (DstLen<0)
//  //  return;
//  //
//  long N=0;
//  //SrcLen++;
//  //DstLen++;
//  //CString Src,Dst;
//  //for (int iSrc=0; iSrc<m_SrcTerms.GetItemCount(); iSrc++)
//  //  {
//  //  Src=m_SrcTerms.GetItemText(iSrc,1);
//  //  if (strnicmp(SrcTS, Src, SrcLen)!=0)
//  //    continue;
//  //  for (int iDst=0; iDst<m_DstTerms.GetItemCount(); iDst++)
//  //    {
//  //    Dst=m_DstTerms.GetItemText(iDst,1);
//  //    if (strnicmp(DstTS, Dst, DstLen)!=0)
//  //      continue;
//  //    if (stricmp(((LPCTSTR)Src)+SrcLen, ((LPCTSTR)Dst)+DstLen)==0)
//  //      {
//  //      AddAWire(iSrc,iDst);
//  //      iSrc--; // in anticipation of increment
//  //      N++;
//  //      break;  //out of Dst Loop
//  //      }
//  //    }
//  //  }
//  CString Msg;
//  Msg.Format("%i Wires added", N);
//  m_WireBad.SetWindowText(Msg);
//  UpdateImages();
//  }

void CELnkWiring::OnBnClickedWirematchOut2In()
  {
  m_WireBad.SetWindowText("");
  if (1)
    {
    //long nSrc=m_SrcTerms.GetSelectedCount();
    //long nDst=m_DstTerms.GetSelectedCount();
    //if (nSrc!=1 || nDst!=1)
    //  {
    //  m_WireBad.SetWindowText("Bad Selection - One in each required");
    //  return;
    //  }
    //
    //CString SrcTS=m_SrcTerms.GetItemText(m_SrcTerms.GetSelectionMark(), 1);
    //long SrcLen=SrcTS.Find(':');
    //if (SrcLen<0)
    //  return;
    //
    //CString DstTS=m_DstTerms.GetItemText(m_DstTerms.GetSelectionMark(), 1);
    //long DstLen=DstTS.Find(':');
    //if (DstLen<0)
    //  return;
    //SrcLen++;
    //DstLen++;
  
    CString SrcKey("OUT:");
    CString DstKey("IN:");
    long SrcKeyLen=SrcKey.GetLength();
    long DstKeyLen=DstKey.GetLength();
    long N=0;
    CString Src,Dst;
    for (int iSrc=0; iSrc<m_SrcTrm.GetSize(); iSrc++)
      {
      Src=m_SrcTrm[iSrc].m_sFullName;
      //CString SrcTS=Src;
      long SrcLen=Src.Find(':');
      if (SrcLen<=SrcKeyLen)
        continue;
      SrcLen++;
      if (strnicmp(LPCTSTR(Src)+SrcLen-SrcKeyLen, SrcKey, SrcKeyLen)!=0)
        continue;
    //
    //CString DstTS=m_DstTerms.GetItemText(m_DstTerms.GetSelectionMark(), 1);
    //long DstLen=DstTS.Find(':');
    //if (DstLen<0)
    //  return;
    //SrcLen++;
    //DstLen++;
      CString DstTS=Src.Left(SrcLen-SrcKeyLen)+DstKey;
      long DstLen=DstTS.GetLength();
  
      //if (strnicmp(SrcTS, Src, SrcLen)!=0)
      //  continue;
      for (int iDst=0; iDst<m_DstTrm.GetSize(); iDst++)
        {
        Dst=m_DstTrm[iDst].m_sFullName;
        if (strnicmp(DstTS, Dst, DstLen)!=0)
          continue;
        if (stricmp(((LPCTSTR)Src)+SrcLen, ((LPCTSTR)Dst)+DstLen)==0)
          {
          AddAWire(iSrc,iDst);
          //iSrc--; // in anticipation of increment
          N++;
          break;  //out of Dst Loop
          }
        }
      }
    CString Msg;
    Msg.Format("%i Wires added", N);
    m_WireBad.SetWindowText(Msg);
    UpdateImages();
    }
  else
    m_WireBad.SetWindowText("Not Done Yet!");
  }

void CELnkWiring::OnBnClickedOk()
  {
  for (int i=0; i<m_LW.m_SrcTerminals.GetSize(); i++)
    m_LW.m_SrcTerminals[i].m_iWire=-1;
  for (i=0; i<m_LW.m_DstTerminals.GetSize(); i++)
    m_LW.m_DstTerminals[i].m_iWire=-1;
  for (i=0; i<m_Wiring.GetItemCount(); i++)
    {
    long iSrc=m_Wiring.GetItemData(i)&0xffff;
    long iDst=(m_Wiring.GetItemData(i)&0xffff0000)>>16;
    //dbgpln("%*s %2i) %3i %3i", 5, "", i, iSrc, iDst);//, m_LW.m_SrcTerminals[i].);
    m_LW.m_SrcTerminals[iSrc].m_iWire=i;
    m_LW.m_DstTerminals[iDst].m_iWire=i;
    }

  gs_pPrj->SetLinkWiring(m_LW);

  OnOK();
  delete this;
  }

void CELnkWiring::OnBnClickedCancel()
  {
  OnCancel();
  delete this;
  }

//void CELnkWiring::OnTvnBegindragSrctree(NMHDR *pNMHDR, LRESULT *pResult)
//  {
//  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//  // TODO: Add your control notification handler code here
//  //mageList* pImageList = MyTreeCtrl.CreateDragImage(hItem);
//  *pResult = 0;
//  }

//void CELnkWiring::OnTvnBegindragDsttree(NMHDR *pNMHDR, LRESULT *pResult)
//  {
//  LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//  // TODO: Add your control notification handler code here
//  *pResult = 0;
//  }
//
//void CELnkWiring::OnLButtonUp(UINT nFlags, CPoint point)
//  {
//  // TODO: Add your message handler code here and/or call default
//
//  CDialog::OnLButtonUp(nFlags, point);
//  }
//
//void CELnkWiring::OnMouseMove(UINT nFlags, CPoint point)
//  {
//  // TODO: Add your message handler code here and/or call default
//  UINT uFlags;
//  CRect RC, RD; 
//  CPoint p;
//
//  p=point;
//  ClientToScreen(&p);
//  m_SrcTree.ScreenToClient(&p);
//  HTREEITEM hHitS=m_SrcTree.HitTest(p, &uFlags);
//  CString S;
//  //  S.Format("S:%08x %08x\n",hHitS, uFlags); 
//  //  ::OutputDebugString(S);
//  //  if (hHitS && (uFlags&TVHT_ONITEM))
//  //    {
//  //    m_SrcTree.SetItemState(hHitS, TVIS_DROPHILITED, TVIS_DROPHILITED);
//  //    }
//
//  p=point;
//  ClientToScreen(&p);
//  m_DstTree.ScreenToClient(&p);
//  HTREEITEM hHitD=m_DstTree.HitTest(p, &uFlags);
//  S.Format("D:%08x %08x\n",hHitD, uFlags); 
//  ::OutputDebugString(S);
//  if (hHitD)// && (uFlags&TVHT_ONITEM))
//    {
//    m_DstTree.SetItemState(hHitD, TVIS_DROPHILITED, TVIS_DROPHILITED);
//    }
//
//
//  CDialog::OnMouseMove(nFlags, point);
//  }

//========================================================================
//
//
//
//========================================================================

/*
void CELnkWiring::OnNMDblclkSrctree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  m_WireBad.SetWindowText("");
  HTREEITEM hSrc=m_SrcTree.GetSelectedItem();
  HTREEITEM hDst=m_DstTree.GetSelectedItem();
  if (hSrc && hDst)
    {
    DWORD iSrc=m_SrcTree.GetItemData(hSrc);
    DWORD iDst=m_DstTree.GetItemData(hDst);
    if ((iSrc&TSMark)!=0 && (iDst&TSMark)!=0)
      {
      DWORD iSrc0=(iSrc&0x7fff0000)>>16;
      DWORD iSrcN=iSrc&0x7fff;
      DWORD iDst0=(iDst&0x7fff0000)>>16; 
      DWORD iDstN=iDst&0x7fff;
      if ((iSrcN-iSrc0)==(iDstN-iDst0))
        {
        DWORD id=iDst0;
        DWORD is=iSrc0; 
        while(is<=iSrcN)
          AddAWire(is++, id++);
        }
      else
        m_WireBad.SetWindowText("Mismatched Terminal count");
      }
    else if ((iSrc&TSMark)==0 && (iDst&TSMark)==0)
      {
      AddAWire(iSrc, iDst);
      }
    else
      m_WireBad.SetWindowText("Musrt select either Terminals or TermStrips");
    UpdateImages();
    }
  *pResult = 0;
  }

void CELnkWiring::OnNMDblclkDsttree(NMHDR *pNMHDR, LRESULT *pResult)
  {
  m_WireBad.SetWindowText("");
  HTREEITEM hSrc=m_SrcTree.GetSelectedItem();
  HTREEITEM hDst=m_DstTree.GetSelectedItem();
  if (hSrc && hDst)
    {
    DWORD iSrc=m_SrcTree.GetItemData(hSrc);
    DWORD iDst=m_DstTree.GetItemData(hDst);
    if ((iSrc&TSMark)!=0 && (iDst&TSMark)!=0)
      {
      DWORD iSrc0=(iSrc&0x7fff0000)>>16;
      DWORD iSrcN=iSrc&0x7fff;
      DWORD iDst0=(iDst&0x7fff0000)>>16; 
      DWORD iDstN=iDst&0x7fff;
      if ((iSrcN-iSrc0)==(iDstN-iDst0))
        {
        DWORD id=iDst0;
        DWORD is=iSrc0; 
        while(is<=iSrcN)
          AddAWire(is++, id++);
        }
      else
        m_WireBad.SetWindowText("Mismatched Terminal count");
      }
    else if ((iSrc&TSMark)==0 && (iDst&TSMark)==0)
      {
      AddAWire(iSrc, iDst);
      }
    else
      m_WireBad.SetWindowText("Musrt select either Terminals or TermStrips");
    UpdateImages();
    }
  *pResult = 0;
  }

*/