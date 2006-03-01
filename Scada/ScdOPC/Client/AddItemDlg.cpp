//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  AddItemDlg.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Dialog to add an item. The OPC Browse interface is used
//                to display the server's items.
//                To support browsing on demand, the tree keeps a flag
//                in each item's data which is true if the branch has
//                been expanded. A dummy is inserted below each item
//                so the expand button will be displayed.
//
//**************************************************************************
#include "stdafx.h"
#include "OPC Client.h"
#include "OPC ClientDoc.h"
#include "AddItemDlg.h"
#include "ItemPropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

LPCWSTR empty = {L""};
CONST ULONG NEXT_COUNT = 100; // number of elements to request when calling
                              // enumerator::Next()

/////////////////////////////////////////////////////////////////////////////
// AddItemDlg dialog

AddItemDlg::AddItemDlg(OPCClientDoc* parent, IOPCServer* pServer, CWnd* pParent /*=NULL*/)
   : CDialog(AddItemDlg::IDD, pParent), pDoc( parent )
{
   browse.Attach( pServer );  // Get a Browse interface
   m_vt = VT_EMPTY;
   nameSpaceType = OPC_NS_HIERARCHIAL;
   //{{AFX_DATA_INIT(AddItemDlg)
   m_filter = _T("*");
   m_accessPath = _T("");
   m_itemID = _T("");
   //}}AFX_DATA_INIT
}


void AddItemDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AddItemDlg)
   DDX_Control(pDX, IDC_FLAT, m_flat);
   DDX_Control(pDX, IDC_TAGS, m_tags);
   DDX_Control(pDX, IDC_TREE, m_tree);
   DDX_Text(pDX, IDC_FILTER, m_filter);
   DDX_Text(pDX, IDC_ACCESS_PATH, m_accessPath);
   DDX_Text(pDX, IDC_ITEMID, m_itemID);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddItemDlg, CDialog)
   //{{AFX_MSG_MAP(AddItemDlg)
   ON_BN_CLICKED(IDC_NATIVE, OnNative)
   ON_BN_CLICKED(IDC_BOOL, OnBool)
   ON_BN_CLICKED(IDC_SHORT, OnShort)
   ON_BN_CLICKED(IDC_LONG, OnLong)
   ON_BN_CLICKED(IDC_DOUBLE, OnDouble)
   ON_BN_CLICKED(IDC_STRING, OnString)
   ON_LBN_SELCHANGE(IDC_TAGS, OnSelchangeTags)
   ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
   ON_LBN_SELCHANGE(IDC_FLAT, OnSelchangeFlat)
   ON_BN_CLICKED(ID_ADDITEM, OnAdditem)
   ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE, OnItemexpandingTree)
   ON_EN_KILLFOCUS(IDC_FILTER, OnNewFilter)
   ON_LBN_DBLCLK(IDC_TAGS, OnDblclkTags)
   ON_LBN_DBLCLK(IDC_FLAT, OnDblclkFlat)
   ON_BN_CLICKED(ID_OPC_ITEMPARAMETERS, OnOpcItemparameters)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//**************************************************************************
BOOL AddItemDlg::OnInitDialog()
{
USES_CONVERSION;
   CWaitCursor wait;
   CDialog::OnInitDialog();

   GetDlgItem(ID_OPC_ITEMPARAMETERS)->EnableWindow(FALSE);
   CheckRadioButton( IDC_NATIVE, IDC_STRING, IDC_NATIVE );

   // Fill the tree with the server's hierarchy
   if( !browse.IsOk() )
   {
      m_tree.ShowWindow( FALSE );
      m_tags.ShowWindow( FALSE );
      m_flat.ShowWindow( TRUE );
      m_flat.AddString( _T("Browsing not supported") );
      return TRUE;
   }

   HRESULT hr = browse.QueryOrganization(&nameSpaceType);
   if( FAILED(hr) )
   {
      browse.Detach();
      return TRUE;
   }

   if( nameSpaceType == OPC_NS_HIERARCHIAL ) // show a tree
   {
      HTREEITEM Root = m_tree.InsertItem( _T("Root") );
      m_tree.SetItemData( Root, 1 );
      IEnumString* pEnumString = NULL;
      hr = browse.BrowseOPCItemIDs(OPC_BRANCH,
                                  L"*",
                                  m_vt,
                                  0,  // no filtering (or OPC_READABLE|OPC_WRITEABLE)
                                  &pEnumString);
      if( SUCCEEDED(hr) )
      {
         LPWSTR pName[NEXT_COUNT];
         ULONG count = 0;
         do
         {
            hr = pEnumString->Next(NEXT_COUNT, &pName[0], &count);
            // CString does translation from UNICODE to native type
            // (depends on the way the application is built)
            for( ULONG index=0; index<count; index++ )
            {
                CString name( pName[index] );
                HTREEITEM item = m_tree.InsertItem( name, Root );
                m_tree.SetItemData( item, 0 );
                m_tree.InsertItem( _T("Dummy"), item );
                CoTaskMemFree( pName[index] );
            }
         }
         while( hr == S_OK );
         pEnumString->Release();
      }
      m_tree.SelectItem( Root );
      m_tree.Expand( Root, TVE_EXPAND );
   }
   else  // namespace must be flat, only display in one listbox
   {
      m_tree.ShowWindow( FALSE );
      m_tags.ShowWindow( FALSE );
      m_flat.ShowWindow( TRUE );
      UpdateFlat();
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// AddItemDlg message handlers

void AddItemDlg::OnNative()
{
   m_vt = VT_EMPTY;
   if( nameSpaceType == OPC_NS_HIERARCHIAL )
      OnSelchangedTree( NULL, NULL );
   else
      UpdateFlat();
}

void AddItemDlg::OnBool()
{
   m_vt = VT_BOOL;
   if( nameSpaceType == OPC_NS_HIERARCHIAL )
      OnSelchangedTree( NULL, NULL );
   else
      UpdateFlat();
}

void AddItemDlg::OnShort()
{
   m_vt = VT_I2;
   if( nameSpaceType == OPC_NS_HIERARCHIAL )
      OnSelchangedTree( NULL, NULL );
   else
      UpdateFlat();
}

void AddItemDlg::OnLong()
{
   m_vt = VT_I4;
   if( nameSpaceType == OPC_NS_HIERARCHIAL )
      OnSelchangedTree( NULL, NULL );
   else
      UpdateFlat();
}

void AddItemDlg::OnDouble()
{
   m_vt = VT_R8;
   if( nameSpaceType == OPC_NS_HIERARCHIAL )
      OnSelchangedTree( NULL, NULL );
   else
      UpdateFlat();
}

void AddItemDlg::OnString()
{
   m_vt = VT_BSTR;
   if( nameSpaceType == OPC_NS_HIERARCHIAL )
      OnSelchangedTree( NULL, NULL );
   else
      UpdateFlat();
}

//**************************************************************************
// When the user selects a tag, put its name in the ItemID field
void AddItemDlg::OnSelchangeTags()
{
USES_CONVERSION;
   if( !browse.IsOk() )
      return;

   CWaitCursor wait;
   HTREEITEM item = m_tree.GetSelectedItem( );
   HRESULT hr = BrowseHere( item );    // move browse position here

   m_itemID.Empty();
   GetDlgItem(ID_OPC_ITEMPARAMETERS)->EnableWindow(FALSE);
   int nSel;
   if ((nSel = m_tags.GetCurSel()) != LB_ERR)
   {
      m_tags.GetText(nSel,m_itemID);
   }
   else
      return;

   // Ask the server for the itemID
   LPWSTR pName = NULL;
   hr = browse.GetItemID( T2OLE(m_itemID.GetBuffer(0)), &pName );
   if( SUCCEEDED(hr) )
   {
      m_itemID = pName;
      CoTaskMemFree( pName );
      GetDlgItem(ID_OPC_ITEMPARAMETERS)->EnableWindow(TRUE);
   }

   UpdateData( FALSE );

   // return browse position to root
   hr = S_OK;
   for( int i=0; i<10 && SUCCEEDED(hr); i++ )
      hr = browse.ChangeBrowsePosition( OPC_BROWSE_UP, empty );
}

//**************************************************************************
// When the user selects a branch, fill the list of tags
void AddItemDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
USES_CONVERSION;
   if( !browse.IsOk() )
      return;

   CWaitCursor wait;
   HTREEITEM item = m_tree.GetSelectedItem( );
   HRESULT hr = BrowseHere( item );    // move browse position here
   if( SUCCEEDED(hr) )
   {
      // get the tags
      m_tags.ResetContent();
      IEnumString* pEnumString = NULL;
      hr = browse.BrowseOPCItemIDs(OPC_LEAF,
                                   T2OLE(m_filter.GetBuffer(0)),
                                   m_vt,
                                   0,  // no filtering (or OPC_READABLE|OPC_WRITEABLE)
                                   &pEnumString);
      if( hr == S_OK )
      {
         LPWSTR pName[NEXT_COUNT];
         ULONG count = 0;
         do
         {
            hr = pEnumString->Next(NEXT_COUNT, &pName[0], &count);
            // CString does translation from UNICODE to native type
            // (depends on the way the application is built)
            for( ULONG index=0; index<count; index++ )
            {
               CString name( pName[index] );
               m_tags.AddString( name );
               CoTaskMemFree( pName[index] );
            }
         }
         while( hr == S_OK );
         pEnumString->Release();
      }
   }
   hr = S_OK;
   for( int i=0; i<10 && SUCCEEDED(hr); i++ )
      hr = browse.ChangeBrowsePosition( OPC_BROWSE_UP, empty );

   if( pResult )
      *pResult = 0;
}

//**************************************************************************
// Recursive function that moves the browse position to this tree item
HRESULT AddItemDlg::BrowseHere( HTREEITEM item )
{
USES_CONVERSION;
   if( !browse.IsOk() )
      return E_FAIL;

   HRESULT hr = S_OK;
   if( item != NULL )
   {
      HTREEITEM parent = m_tree.GetParentItem( item );
      hr = BrowseHere( parent );
      if( SUCCEEDED( hr ) )
      {
         CString node( m_tree.GetItemText( item ) );
         if( node != _T("Root") )
         {
            hr = browse.ChangeBrowsePosition( OPC_BROWSE_DOWN, T2OLE(node.GetBuffer(0)) );
         }
      }
   }
   return hr;
}

//**************************************************************************
void AddItemDlg::OnSelchangeFlat()
{
USES_CONVERSION;
   if( !browse.IsOk() )
      return;

   CWaitCursor wait;
   m_itemID.Empty();
   GetDlgItem(ID_OPC_ITEMPARAMETERS)->EnableWindow(FALSE);
   int nSel = m_flat.GetCurSel();
   if ( nSel != LB_ERR )
   {
      m_flat.GetText(nSel,m_itemID);
   }
   else
      return;

   // Ask the server for the itemID
   LPWSTR pName = NULL;
   HRESULT hr = browse.GetItemID( T2OLE(m_itemID.GetBuffer(0)), &pName );
   if( SUCCEEDED(hr) )
   {
      m_itemID = pName;
      CoTaskMemFree( pName );
   GetDlgItem(ID_OPC_ITEMPARAMETERS)->EnableWindow(TRUE);
   }

   UpdateData( FALSE );
}

//**************************************************************************
void AddItemDlg::UpdateFlat()
{
USES_CONVERSION;
   if( !browse.IsOk() )
      return;

   CWaitCursor wait;
   // get the tags
   m_flat.ResetContent();
   IEnumString* pEnumString = NULL;
   HRESULT hr = browse.BrowseOPCItemIDs(OPC_FLAT,
                                      T2OLE(m_filter.GetBuffer(0)),
                                      m_vt,
                                      0,  // no filtering (or OPC_READABLE|OPC_WRITEABLE)
                                      &pEnumString);
   if( SUCCEEDED(hr) )
   {
      LPWSTR pName[NEXT_COUNT];
      ULONG count = 0;
      do
      {
         hr = pEnumString->Next(NEXT_COUNT, &pName[0], &count);
         // CString does translation from UNICODE to native type
         // (depends on the way the application is built)
         for( ULONG index=0; index<count; index++ )
         {
            CString name( pName[index] );
            m_flat.AddString( name );
            CoTaskMemFree( pName[index] );
         }
      }
      while( hr == S_OK );
      pEnumString->Release();
   }
   m_flat.SetCurSel( 0 );
}

//**************************************************************************
void AddItemDlg::OnAdditem()
{
   CWaitCursor wait;
   UpdateData();
   if( pDoc )
   {
      pDoc->AddItem( m_itemID, m_accessPath, m_vt );
   }
}

//**************************************************************************
void AddItemDlg::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult)
{
USES_CONVERSION;
   if( !browse.IsOk() )
      return;

   CWaitCursor wait;
   NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
   HTREEITEM start = pNMTreeView->itemNew.hItem;
   DWORD expanded = m_tree.GetItemData( start );
   if( !expanded )
   {
      m_tree.SetItemData( start, 1 );  // mark this item expanded
      // remove existing children
      HTREEITEM hItem = m_tree.GetNextItem (start, TVGN_CHILD);
      while (hItem)
      {
         m_tree.DeleteItem( hItem );
         hItem = m_tree.GetNextItem(start, TVGN_CHILD);
      }
      // browse new children
      HRESULT hr = BrowseHere( start );    // move browse position here
      if( SUCCEEDED(hr) )
      {
         IEnumString* pEnumString = NULL;
         hr = browse.BrowseOPCItemIDs( OPC_BRANCH,
                                       L"*",
                                       VT_EMPTY,
                                       0,  // no filtering (or OPC_READABLE|OPC_WRITEABLE)
                                       &pEnumString);
         if( SUCCEEDED(hr) )
         {
            LPWSTR pName[NEXT_COUNT];
            ULONG count = 0;
            do
            {
               hr = pEnumString->Next(NEXT_COUNT, &pName[0], &count);
               // CString does translation from UNICODE to native type
               // (depends on the way the application is built)
               for( ULONG index=0; index<count; index++ )
               {
                   CString name( pName[index] );
                   HTREEITEM item = m_tree.InsertItem( name, start );
                   m_tree.SetItemData( item, 0 );
                   m_tree.InsertItem( _T("Dummy"), item );
                   CoTaskMemFree( pName[index] );
               }
            }
            while( hr == S_OK );
            pEnumString->Release();
         }
      }
      // return browse position to root
      hr = S_OK;
      for( int i=0; i<10 && SUCCEEDED(hr); i++ )
         hr = browse.ChangeBrowsePosition( OPC_BROWSE_UP, empty );
   }
   *pResult = 0;
}

void AddItemDlg::OnNewFilter()
{
   UpdateData( );
   if( nameSpaceType == OPC_NS_HIERARCHIAL )
      OnSelchangedTree( NULL, NULL );
   else
      UpdateFlat();
}

void AddItemDlg::OnDblclkTags()
{
    OnAdditem();
}

void AddItemDlg::OnDblclkFlat()
{
    OnAdditem();
}

void AddItemDlg::OnOpcItemparameters()
{
   UpdateData();
   if( m_itemID.IsEmpty() )
      return;
   CItemPropertiesDlg dlg(m_itemID);
   dlg.SetOPCPointer( (IUnknown*)pDoc->opcServer );
   dlg.DoModal();
}
