//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#if ForSysCAD
#include "sc_defs.h"
#include "scd_wm.h"
#endif
#include "hstmsg.h"
#include "hstqyinf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//=========================================================================

CQueryInfo* gs_pQueryInfo = NULL;

CQueryInfo::CQueryInfo(CWnd* pParent /*=NULL*/)
  : CDialog(CQueryInfo::IDD, pParent)
  {
  //{{AFX_DATA_INIT(CQueryInfo)
  m_LocalCnt = 0;
  m_RemoteCnt = 0;
  m_LocalPer = _T("");
  m_RemotePer = _T("");
  m_LocalPerBest = _T("");
  m_RemotePerBest = _T("");
  //}}AFX_DATA_INIT
  Create(CQueryInfo::IDD, pParent); // NonModal Create
  ShowWindow(SW_HIDE);
  }

//---------------------------------------------------------------------------

void CQueryInfo::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CQueryInfo)
  DDX_Text(pDX, IDC_LOCALCNT, m_LocalCnt);
  DDX_Text(pDX, IDC_REMOTECNT, m_RemoteCnt);
  DDX_Text(pDX, IDC_LOCALPER, m_LocalPer);
  DDX_Text(pDX, IDC_REMOTEPER, m_RemotePer);
  DDX_Text(pDX, IDC_LOCALPERBEST, m_LocalPerBest);
  DDX_Text(pDX, IDC_REMOTEPERBEST, m_RemotePerBest);
  //}}AFX_DATA_MAP
  }

//---------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CQueryInfo, CDialog)
  //{{AFX_MSG_MAP(CQueryInfo)
  ON_MESSAGE(WMU_HSTQRYUPDATE, OnUpdateData)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------

LRESULT CQueryInfo::OnUpdateData(WPARAM wParam, LPARAM lParam)
  {
  CQueryInfoStats* pQI = (CQueryInfoStats*)lParam;
  int i;
  char Buff[256];
  if (pQI->bRemote)
    {
    m_RemoteCnt += pQI->iIncrement;
    i = FindRemote(pQI->lID);
    if (pQI->dPer>100)
      {
      RemoteStats[i].lID = 0;
      RemoteStats[i].dPer = pQI->dPer;
      }
    else
      {
      RemoteStats[i].lID = pQI->lID;
      RemoteStats[i].dPer = pQI->dPer;
      }
    }
  else
    {
    m_LocalCnt += pQI->iIncrement;
    i = FindLocal(pQI->lID);
    if (pQI->dPer>100)
      {
      LocalStats[i].lID = 0;
      LocalStats[i].dPer = pQI->dPer;
      }
    else
      {
      LocalStats[i].lID = pQI->lID;
      LocalStats[i].dPer = pQI->dPer;
      }
    }

  if (m_LocalCnt<1)
    {
    m_LocalPer = "";
    m_LocalPerBest = "";
    }
  else if (m_LocalCnt==1)
    {
    sprintf(Buff, "%4.1f%%", LocalStats[0].dPer);
    m_LocalPer = Buff;
    m_LocalPerBest = "";
    }
  else
    {
    i = FindLocalMin();
    sprintf(Buff, "%4.1f%%", LocalStats[i].dPer);
    m_LocalPer = Buff;
    i = FindLocalMax();
    sprintf(Buff, "%4.1f%%", LocalStats[i].dPer);
    m_LocalPerBest = Buff;
    }
  if (m_RemoteCnt<1)
    {
    m_RemotePer = "";
    m_RemotePerBest = "";
    }
  else if (m_RemoteCnt==1)
    {
    sprintf(Buff, "%4.1f%%", RemoteStats[0].dPer);
    m_RemotePer = Buff;
    m_RemotePerBest = "";
    }
  else
    {
    i = FindRemoteMin();
    sprintf(Buff, "%4.1f%%", RemoteStats[i].dPer);
    m_RemotePer = Buff;
    i = FindRemoteMax();
    sprintf(Buff, "%4.1f%%", RemoteStats[i].dPer);
    m_RemotePerBest = Buff;
    }

  if (m_LocalCnt<1 && m_RemoteCnt<1)
    ShowWindow(SW_HIDE);
  else
    {
    if (!IsWindowVisible())
      {
      WINDOWPLACEMENT WP;
      WP.length = sizeof(WP);
      if (GetWindowPlacement(&WP))
        {
        RECT R;
        GetParent()->GetWindowRect(&R);
        WP.rcNormalPosition.right = WP.rcNormalPosition.right - WP.rcNormalPosition.left + R.left + 2;
        WP.rcNormalPosition.left = R.left + 2;
        int bottom = R.bottom - 2;
        //if ((MainWnd()->StatusBar()->GetStyle() & WS_VISIBLE) != 0)
          bottom -= 18; //approx height of statusbar
        WP.rcNormalPosition.top = bottom - (WP.rcNormalPosition.bottom - WP.rcNormalPosition.top);
        WP.rcNormalPosition.bottom = bottom;
        WP.showCmd = SW_SHOWNOACTIVATE;
        WP.length = sizeof(WP);
        SetWindowPlacement(&WP);
        }
      }
    UpdateData(false);
    UpdateWindow();
    }

  return true;
  }

//---------------------------------------------------------------------------

int CQueryInfo::FindLocal(long ID)
  {
  int i = 0;
  int BlankPos = -1;
  while (i<MaxQueryInfoStats)
    {
    if (LocalStats[i].lID==ID)
      break;
    if (BlankPos<0 && LocalStats[i].lID==0)
      BlankPos = i;
    i++;
    }
  if (i<MaxQueryInfoStats)
    return i;
  if (BlankPos>=0)
    return BlankPos;
  return FindLocalMax();
  }

//---------------------------------------------------------------------------

int CQueryInfo::FindRemote(long ID)
  {
  int i = 0;
  int BlankPos = -1;
  while (i<MaxQueryInfoStats)
    {
    if (RemoteStats[i].lID==ID)
      break;
    if (BlankPos<0 && RemoteStats[i].lID==0)
      BlankPos = i;
    i++;
    }
  if (i<MaxQueryInfoStats)
    return i;
  if (BlankPos>=0)
    return BlankPos;
  return FindRemoteMax();
  }

//---------------------------------------------------------------------------

int CQueryInfo::FindLocalMin()
  {
  double Min = 999.9;
  int Pos = 0;
  for (int i=0; i<MaxQueryInfoStats; i++)
    {
    if (LocalStats[i].lID != 0)
      if (LocalStats[i].dPer < Min)
        {
        Pos = i;
        Min = LocalStats[i].dPer;
        }
    }
  return Pos;
  }

//---------------------------------------------------------------------------

int CQueryInfo::FindRemoteMin()
  {
  double Min = 999.9;
  int Pos = 0;
  for (int i=0; i<MaxQueryInfoStats; i++)
    {
    if (RemoteStats[i].lID != 0)
      if (RemoteStats[i].dPer < Min)
        {
        Pos = i;
        Min = RemoteStats[i].dPer;
        }
    }
  return Pos;
  }

//---------------------------------------------------------------------------

int CQueryInfo::FindLocalMax()
  {
  double Max = -999.9;
  int Pos = 0;
  for (int i=0; i<MaxQueryInfoStats; i++)
    {
    if (LocalStats[i].lID != 0)
      if (LocalStats[i].dPer > Max)
        {
        Pos = i;
        Max = LocalStats[i].dPer;
        }
    }
  return Pos;
  }

//---------------------------------------------------------------------------

int CQueryInfo::FindRemoteMax()
  {
  double Max = -999.9;
  int Pos = 0;
  for (int i=0; i<MaxQueryInfoStats; i++)
    {
    if (RemoteStats[i].lID != 0)
      if (RemoteStats[i].dPer > Max)
        {
        Pos = i;
        Max = RemoteStats[i].dPer;
        }
    }
  return Pos;
  }

//---------------------------------------------------------------------------
//=========================================================================

