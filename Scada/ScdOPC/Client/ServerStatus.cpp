//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ServerStatus.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Dialog to display OPC server status.
//                A timer updates the status while the dialog is alive.
//
//**************************************************************************
#include "stdafx.h"
#include "Resource.h"
#include "ServerStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServerStatus dialog


CServerStatus::CServerStatus(OPCServer& newServer, CWnd* pParent /*=NULL*/)
   : CDialog(CServerStatus::IDD, pParent)
{
   //{{AFX_DATA_INIT(CServerStatus)
   m_current = _T("");
   m_groups = 0;
   m_bandwidth = 0;
   m_last = _T("");
   m_start = _T("");
   m_status = _T("");
   //}}AFX_DATA_INIT
   opcServer.Attach( (IOPCServer*)newServer );  // Get an IOPCServer interface
}


void CServerStatus::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CServerStatus)
   DDX_Text(pDX, IDC_CURRENT_TIME, m_current);
   DDX_Text(pDX, IDC_GROUPS, m_groups);
   DDX_Text(pDX, IDC_BANDWIDTH, m_bandwidth);
   DDX_Text(pDX, IDC_LAST_TIME, m_last);
   DDX_Text(pDX, IDC_START_TIME, m_start);
   DDX_Text(pDX, IDC_STATUS, m_status);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServerStatus, CDialog)
   //{{AFX_MSG_MAP(CServerStatus)
   ON_WM_TIMER()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerStatus message handlers

BOOL CServerStatus::OnInitDialog()
{
   CDialog::OnInitDialog();

   Update();
   VERIFY( SetTimer( 4, 800, NULL ) == 4 );

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CServerStatus::OnOK()
{
   VERIFY( KillTimer( 4 ) );

   CDialog::OnOK();
}

// Get server status and display time, number of groups, bandwidth
void CServerStatus::Update()
   {
   OPCSERVERSTATUS* ss;

   HRESULT hr = opcServer.GetStatus( &ss );
   if( FAILED(hr) )
   {
      VERIFY( KillTimer( 4 ) );
      return;
   }

   m_bandwidth = ss->dwBandWidth;
   if( m_bandwidth == -1 )
      m_bandwidth = 0;
   m_groups = ss->dwGroupCount;

   if( ss->ftStartTime.dwHighDateTime != 0 )
   {
      CTime start( ss->ftStartTime );
      m_start = start.Format(_T("%c"));
   }
   else
      m_start = _T("Unknown");

   if( ss->ftLastUpdateTime.dwHighDateTime != 0 )
   {
      CTime last( ss->ftLastUpdateTime );
      m_last = last.Format(_T("%c"));
   }
   else
      m_last = _T("No updates");

   if( ss->ftCurrentTime.dwHighDateTime != 0 )
   {
      CTime current( ss->ftCurrentTime );
      m_current = current.Format(_T("%c"));
   }
   else
      m_current = _T("Unknown");

   m_status = _T("The server is");
   switch( ss->dwServerState )
   {
      case OPC_STATUS_FAILED:
         m_status += _T(" failed");
         break;
      case OPC_STATUS_RUNNING:
         m_status += _T(" running");
         break;
      case OPC_STATUS_NOCONFIG:
         m_status += _T(" unconfigured");
         break;
      case OPC_STATUS_SUSPENDED:
         m_status += _T(" suspended");
         break;
      case OPC_STATUS_TEST:
         m_status += _T(" in test mode");
         break;
      default:
         m_status += _T(" Unknown state!");
   }
   CString vendorInfo( ss->szVendorInfo );   // convert Unicode to ansi if needed
   SetWindowText( vendorInfo );
   CoTaskMemFree( ss->szVendorInfo );
   CoTaskMemFree( ss );

   UpdateData( FALSE );
   }

void CServerStatus::OnTimer(UINT nIDEvent)
{
   Update();
}
