//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  OPCServerDlg.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Dialog to select OPC server to connect.
//                This dialog uses the OPC Enumerator interface
//                described in the OPC Common specification.
//
//**************************************************************************
#include "stdafx.h"
#include "OPC Client.h"
#include "OPC ClientDoc.h"
#include "OPCServerDlg.h"
#include "FindServersDlg.h"
#include <LMCONS.H>
#include <lmserver.h>

const CLSID CLSID_OPCServerList = {0x13486D51,0x4821,0x11D2,{0xA4,0x94,0x3C,0xB3,0x06,0xC1,0x00,0x00}};
#define __GUID_DEFINED__
#include "OPCDa_Cats.c"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern OPCClientDoc* theDoc;
CONST ULONG NEXT_COUNT = 100; // number of elements to request when calling Next

/////////////////////////////////////////////////////////////////////////////
// OPCServerDlg dialog


OPCServerDlg::OPCServerDlg(CWnd* pParent /*=NULL*/)
   : CDialog(OPCServerDlg::IDD, pParent)
{
   m_Enumerated = FALSE;
   memset( &m_clsid, 0, sizeof(m_clsid) );
   m_catid = CATID_OPCDAServer20;
   //{{AFX_DATA_INIT(OPCServerDlg)
   m_Node = _T("");
   m_Server = _T("");
   m_currentServer = _T("");
   //}}AFX_DATA_INIT
}

OPCServerDlg::~OPCServerDlg()
{
   POSITION pos = m_Servers.GetHeadPosition();
   while( pos )
   {
      delete m_Servers.GetNext( pos );
   }
}

void OPCServerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(OPCServerDlg)
   DDX_CBString(pDX, IDC_NODE, m_Node);
   DDX_Text(pDX, IDC_SERVER, m_Server);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OPCServerDlg, CDialog)
   //{{AFX_MSG_MAP(OPCServerDlg)
   ON_LBN_DBLCLK(IDC_SERVER_LIST, OnDblclkServerList)
   ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
   ON_CBN_SELCHANGE(IDC_NODE, OnSelchangeNode)
   ON_LBN_SELCHANGE(IDC_SERVER_LIST, OnSelchangeServerList)
   ON_CBN_DROPDOWN(IDC_NODE, OnDropdownNode)
   ON_BN_CLICKED(IDC_SERVERS10, OnServers10)
   ON_BN_CLICKED(IDC_SERVERS20, OnServers20)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OPCServerDlg message handlers

//**************************************************************************
BOOL OPCServerDlg::OnInitDialog()
{
   CWaitCursor wait;
   CDialog::OnInitDialog();

   CheckRadioButton(IDC_SERVERS10, IDC_SERVERS20, IDC_SERVERS20);
   // Fill in the list of OPC servers
   GetOPCServers();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

//**************************************************************************
BOOL OPCServerDlg::EnumerateNodes(LPNETRESOURCE lpnr)
{
   CComboBox* pNodes = (CComboBox*)GetDlgItem(IDC_NODE);

   HANDLE hEnum = 0;
   DWORD dwResult = WNetOpenEnum(RESOURCE_GLOBALNET,
                     RESOURCETYPE_ANY,
                     RESOURCEUSAGE_CONTAINER,
                     lpnr,              // NULL first time this function is called
                     &hEnum);           // handle to resource

   if (dwResult != NO_ERROR)
   {
      return FALSE;
   }

   DWORD cbBuffer = 16384;      // 16K buffer
   LPNETRESOURCE lpnrLocal = (LPNETRESOURCE) GlobalAlloc(GPTR, cbBuffer);
   do
   {
      DWORD cEntries = 0xFFFFFFFF; // enumerate all possible entries
      dwResult = WNetEnumResource(hEnum,
                                  &cEntries,
                                  lpnrLocal,
                                  &cbBuffer);

      if (dwResult == NO_ERROR)
      {
         for( DWORD i = 0; i < cEntries; i++)
         {
            // If this NETRESOURCE is a container, call the function
            // recursively.
            if(RESOURCEUSAGE_CONTAINER ==
               (lpnrLocal[i].dwUsage & RESOURCEUSAGE_CONTAINER))
            {
               if(RESOURCEDISPLAYTYPE_SERVER == lpnrLocal[i].dwDisplayType )
               {
                  CString node(lpnrLocal[i].lpRemoteName);
                  int index = node.Find( _T("\\") );
                  if( index > -1 )
                     node = node.Mid(index+2);
                  if( pNodes )
                     pNodes->AddString(node);
               }
               else
               {
                  EnumerateNodes(&lpnrLocal[i]);
                  break;   // ONLY enumerate the first "Container"
               }
            }
         }
      }
      else if (dwResult != ERROR_NO_MORE_ITEMS)
      {
         break;
      }
   }
   while(dwResult != ERROR_NO_MORE_ITEMS);

   GlobalFree((HGLOBAL) lpnrLocal);

   dwResult = WNetCloseEnum(hEnum);

   pNodes->SetWindowText(m_Node);

   if (dwResult != NO_ERROR)
   {
      return FALSE;
   }

   return TRUE;
}

//**************************************************************************

BOOL OPCServerDlg::GetOPCServers(LPTSTR node)
{
   CListBox* pList = (CListBox*)GetDlgItem( IDC_SERVER_LIST );
   USES_CONVERSION;

   CWaitCursor wait;
   ASSERT_VALID(pList);
   pList->ResetContent();

   // New 2.0 method using component categories
   IOPCServerList* pServers=NULL;

   COSERVERINFO si;
   MULTI_QI  qi;

   si.dwReserved1 = 0;
   si.pwszName = NULL;
   if( node && node[0] != 0 )
      si.pwszName = T2OLE(node);
   si.pAuthInfo = NULL;
   si.dwReserved2 = 0;

   qi.pIID = &IID_IOPCServerList;
   qi.pItf = NULL;
   qi.hr = 0;

   HRESULT hr = CoCreateInstanceEx(CLSID_OPCServerList, NULL, CLSCTX_ALL, &si, 1, &qi);
   if (FAILED(hr) || FAILED(qi.hr))
   {
      CString msg(_T("Error connecting to OPC 2.0 Server Browser."));
      if( !m_Node.IsEmpty() )
         msg.Format(_T("Error connecting to OPC 2.0 Server Browser on %s."), (LPCTSTR)m_Node);

      if( hr == REGDB_E_CLASSNOTREG )
      {
          CString msg(_T("Please install the OPC 2.0 Components."));
          if( !m_Node.IsEmpty() )
             msg.Format(_T("Please install the OPC 2.0 Components on %s."), (LPCTSTR)m_Node);
      }

      if( FAILED(hr) )
        theDoc->ReportError(msg, hr);
      else
        theDoc->ReportError(msg, qi.hr);
   }
   else
   {
       pServers = (IOPCServerList*)qi.pItf;

       IEnumGUID *pEnumGUID=NULL;
       hr = pServers->EnumClassesOfCategories(1, &m_catid, 1, &m_catid, &pEnumGUID);
       if( SUCCEEDED(hr) )
       {
          unsigned long count;
          CLSID clsid[NEXT_COUNT];

          do
          {
             hr = pEnumGUID->Next(NEXT_COUNT, clsid, &count);
             for( ULONG index=0; index<count; index++ )
             {
                LPOLESTR pszProgID;
                LPOLESTR pszUserType;
                HRESULT hr2 = pServers->GetClassDetails(clsid[index], &pszProgID, &pszUserType);
                if (SUCCEEDED(hr2))
                {
                   OPCServerInfo* pServerInfo = new OPCServerInfo(pszProgID, pszUserType, clsid[index] );
                   m_Servers.AddTail( pServerInfo );
                   CString name;
                   name.Format( _T("%s  (%s)"),
                               (LPCTSTR)pServerInfo->m_ProgID,
                               (LPCTSTR)pServerInfo->m_Description );
                   int pos = pList->AddString( name );
                   pList->SetItemData( pos, (DWORD)pServerInfo );
                   if( m_Server == pServerInfo->m_ProgID )
                      pList->SetCurSel(pos);

                   CoTaskMemFree(pszProgID);
                   CoTaskMemFree(pszUserType);
                }
             }
          }
          while ( hr==S_OK );
          pEnumGUID->Release();
          pServers->Release();
          return TRUE;
       }
       else // hr failed
       {
          CString msg(_T("EnumClassesOfCategories failed:"));
          theDoc->ReportError(msg, hr);
       }
       pServers->Release();
   }

   // Old 1.0 method: search the registry for OPC entries
   if( m_catid != CATID_OPCDAServer10 )
       return FALSE;

   HKEY hk = HKEY_CLASSES_ROOT;
   if( node )
   {
      DWORD dwR = RegConnectRegistry ((LPTSTR)node, HKEY_CLASSES_ROOT, &hk);
      if( dwR != ERROR_SUCCESS )
         return FALSE;
   }

   TCHAR key[MAX_KEYLEN];
   DWORD size = MAX_KEYLEN;
   FILETIME ft;
   for( int index=0; RegEnumKeyEx(hk, index, key, &size, 0, NULL, NULL, &ft)==ERROR_SUCCESS; index++)
   {
      HKEY hProgID=0;
      if(RegOpenKeyEx(hk, key, 0, KEY_QUERY_VALUE, &hProgID )==ERROR_SUCCESS )
      {
         // Find OPC key
         HKEY hOPC=0;
         if(RegOpenKeyEx(hProgID, _T("OPC"), 0, KEY_QUERY_VALUE, &hOPC)==ERROR_SUCCESS )
         {
            // Found OPC key, save this information
            CLSID clsid;
            DWORD type=REG_SZ;
            HKEY hCLSID=0;
            if(RegOpenKeyEx(hProgID, _T("CLSID"), 0, KEY_QUERY_VALUE, &hCLSID)==ERROR_SUCCESS )
            {
                TCHAR clsidString[MAX_KEYLEN];
                size=MAX_KEYLEN;
                if(RegQueryValueEx(hCLSID, key, 0, &type, (BYTE*)clsidString, &size )==ERROR_SUCCESS )
                {
                    hr = CLSIDFromString( T2OLE(clsidString), &clsid );
                    if( FAILED(hr))
                    {
                    theDoc->ReportError( _T("CLSIDFromString: "), hr );
                    continue;
                    }
                }
            }
            
            TCHAR desc[MAX_KEYLEN];
            size=MAX_KEYLEN;
            RegQueryValueEx(hOPC, key, 0, &type, (BYTE*)desc, &size);
            OPCServerInfo* pServerInfo = new OPCServerInfo(T2OLE(key), L"", clsid );
            m_Servers.AddTail( pServerInfo );
            int pos = pList->AddString( pServerInfo->m_ProgID );
            pList->SetItemData( pos, (DWORD)pServerInfo );
            if( m_Server == key )
               pList->SetCurSel(pos);
            RegCloseKey( hOPC );
         }
         RegCloseKey( hProgID );
      }
      size = MAX_KEYLEN;
   }

   OnSelchangeServerList();

   return TRUE;
}

//**************************************************************************
// allow double-click selection on a server
void OPCServerDlg::OnDblclkServerList()
{
   OnSelchangeServerList();
   OnOK();
}

//**************************************************************************
// Refresh the server list using the selected Node name
void OPCServerDlg::OnRefresh()
{
   UpdateData();
   GetOPCServers(m_Node.GetBuffer(0));
}

//**************************************************************************
// Automatically search for servers when the node name changes ?
void OPCServerDlg::OnSelchangeNode()
{
}

//**************************************************************************
void OPCServerDlg::OnSelchangeServerList()
{
   CWaitCursor wait;
   UpdateData();

   CListBox* pList = (CListBox*)GetDlgItem( IDC_SERVER_LIST );
   int index = pList->GetCurSel();
   OPCServerInfo* pServerInfo = (OPCServerInfo*)pList->GetItemData(index);
   if( pServerInfo && (DWORD)pServerInfo != LB_ERR )
   {
      m_clsid = pServerInfo->m_clsid;
      m_Server = pServerInfo->m_ProgID;
   }

   UpdateData( FALSE );
}

//**************************************************************************
void OPCServerDlg::OnDropdownNode()
{
   if( !m_Enumerated )
   {
      CWaitCursor wait;
      // Fill in the list of network nodes
      EnumerateNodes(NULL);
      m_Enumerated = TRUE;
   }
}

//**************************************************************************
void OPCServerDlg::OnOK()
{
   CWaitCursor wait;
   UpdateData();

   // Look for a matching server name
   POSITION pos = m_Servers.GetHeadPosition();
   while( pos )
   {
      OPCServerInfo* pServerInfo = m_Servers.GetNext( pos );
      if(m_Server == pServerInfo->m_ProgID)
      {
         m_clsid = pServerInfo->m_clsid;
         CDialog::OnOK();
      }
   }

   // If not found, Get CLSID from the ProgID
   USES_CONVERSION;
   IOPCServerList* pServers=NULL;

   COSERVERINFO si;
   MULTI_QI  qi;

   si.dwReserved1 = 0;
   si.pwszName = NULL;
   if( !m_Node.IsEmpty() )
      si.pwszName = T2OLE(m_Node.GetBuffer(0));
   si.pAuthInfo = NULL;
   si.dwReserved2 = 0;

   qi.pIID = &IID_IOPCServerList;
   qi.pItf = NULL;
   qi.hr = 0;

   HRESULT hr = CoCreateInstanceEx(CLSID_OPCServerList, NULL, CLSCTX_ALL, &si, 1, &qi);
   if (FAILED(hr) || FAILED(qi.hr))
   {
      CString msg(_T("Please install the OPC 2.0 Components."));
      if( !m_Node.IsEmpty() )
         msg.Format(_T("Please install the OPC 2.0 Components on %s."), (LPCTSTR)m_Node);
      if( FAILED(hr) )
        theDoc->ReportError(msg, hr);
      else
        theDoc->ReportError(msg, qi.hr);
      return;
   }
   pServers = (IOPCServerList*)qi.pItf;
   hr = pServers->CLSIDFromProgID(T2OLE(m_Server.GetBuffer(0)), &m_clsid);
   pServers->Release();
   if (FAILED(hr))
   {
      CString msg;
      msg.Format(_T("Error locating '%s'."), (LPCTSTR)m_Server);
      if( !m_Node.IsEmpty() )
         msg.Format(_T("Error locating '%s' on %s."), (LPCTSTR)m_Server, (LPCTSTR)m_Node);
      AfxMessageBox(msg);
      return;
   }

   CDialog::OnOK();
}

void OPCServerDlg::OnServers10()
{
   UpdateData();
   m_catid = CATID_OPCDAServer10;
   GetOPCServers(m_Node.GetBuffer(0));
}

void OPCServerDlg::OnServers20()
{
   UpdateData();
   m_catid = CATID_OPCDAServer20;
   GetOPCServers(m_Node.GetBuffer(0));
}
