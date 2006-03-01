//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  FindServersDlg.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Dialog allows user to cancel long remote browse operation.
//
//
//**************************************************************************
#include "stdafx.h"
#include "OPC Client.h"
#include "FindServersDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT FIND_ID = 45000;
const UINT TIMER_PERIOD = 100;

/////////////////////////////////////////////////////////////////////////////
// CFindServersDlg dialog


CFindServersDlg::CFindServersDlg(CStringList* pList, CWnd* pParent /*=NULL*/)
    : CDialog(CFindServersDlg::IDD, pParent), m_pOPCServers( pList )
{
   hk = 0;
   index = 0;
    //{{AFX_DATA_INIT(CFindServersDlg)
    m_node = _T("");
    m_server = _T("");
    //}}AFX_DATA_INIT
}


void CFindServersDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CFindServersDlg)
    DDX_Control(pDX, IDC_ANIMATE, m_animation);
    DDX_Text(pDX, IDC_NODENAME, m_node);
    DDX_Text(pDX, IDC_SERVERNAME, m_server);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindServersDlg, CDialog)
    //{{AFX_MSG_MAP(CFindServersDlg)
    ON_WM_TIMER()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindServersDlg message handlers

BOOL CFindServersDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   m_animation.Open(IDR_SEARCH);

   SetTimer( FIND_ID, TIMER_PERIOD, NULL );
   UpdateData( FALSE );
   return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindServersDlg::OnTimer(UINT nIDEvent)
{
   if( hk == 0 )
   {
      hk = HKEY_CLASSES_ROOT;
      if( !m_node.IsEmpty() )
      {
         DWORD dwR = RegConnectRegistry ((LPTSTR)(LPCTSTR)m_node, HKEY_CLASSES_ROOT, &hk);
         if( dwR != ERROR_SUCCESS )
         {
            hk = 0;
            OnCancel();
            return;
         }
      }
   }

   // search the registry for OPC entries
   TCHAR key[MAX_KEYLEN];
   DWORD size = MAX_KEYLEN;
   FILETIME ft;
   DWORD tick = GetTickCount();
   while( RegEnumKeyEx(hk, index, key, &size, 0, NULL, NULL, &ft)==ERROR_SUCCESS )
   {
      HKEY hProgID=0;
      if(RegOpenKeyEx(hk, key, 0, KEY_QUERY_VALUE, &hProgID )==ERROR_SUCCESS )
      {
         HKEY hOPC=0;
         if(RegOpenKeyEx(hProgID, _T("OPC"), 0, KEY_QUERY_VALUE, &hOPC)==ERROR_SUCCESS )
         {
            TCHAR desc[MAX_KEYLEN];
            size=MAX_KEYLEN;
            DWORD type=REG_SZ;
            RegQueryValueEx(hOPC, key, 0, &type, (BYTE*)desc, &size);
            m_pOPCServers->AddTail( key );
            m_server = key;
            UpdateData( FALSE );
            RegCloseKey( hOPC );
         }
         RegCloseKey( hProgID );
      }
      index++;
      size = MAX_KEYLEN;
      if( GetTickCount() - tick > TIMER_PERIOD-10 )
         return;
   }

    CDialog::OnTimer(nIDEvent);
   OnOK();
}
