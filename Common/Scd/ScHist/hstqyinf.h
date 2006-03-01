//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __HSTQYINF_H
#define  __HSTQYINF_H

#include "resource.h"

//-------------------------------------------------------------------------

const short MaxQueryInfoStats = 10;

class CQueryInfo : public CDialog
  {
  public:
 	  CQueryInfo(CWnd* pParent = NULL);   // standard constructor

  	//{{AFX_DATA(CQueryInfo)
  	enum { IDD = IDD_QUERYINFO };
	  int		m_LocalCnt;
	  int		m_RemoteCnt;
	  CString	m_LocalPer;
	  CString	m_RemotePer;
	  CString	m_LocalPerBest;
	  CString	m_RemotePerBest;
	  //}}AFX_DATA

  protected:
    CQueryInfoStats LocalStats[MaxQueryInfoStats];
    CQueryInfoStats RemoteStats[MaxQueryInfoStats];
    int FindLocal(long ID);
    int FindRemote(long ID);
    int FindLocalMin();
    int FindLocalMax();
    int FindRemoteMin();
    int FindRemoteMax();

  	//{{AFX_VIRTUAL(CQueryInfo)
	  protected:
	  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	  //}}AFX_VIRTUAL

  protected:
  	//{{AFX_MSG(CQueryInfo)
    afx_msg LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
	  //}}AFX_MSG
	  DECLARE_MESSAGE_MAP()
  };

extern CQueryInfo* gs_pQueryInfo;

#endif

//-------------------------------------------------------------------------
