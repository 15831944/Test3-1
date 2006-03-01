//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ItemProperties.cpp
//  $Author    :  Jim Hansen
//
//  Description:  This dialog displays the OPC 2.0 Item properties.
//
//**************************************************************************
#include "stdafx.h"
#include "OPC Client.h"
#include "ItemPropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemPropertiesDlg dialog


CItemPropertiesDlg::CItemPropertiesDlg(LPCTSTR itemID, CWnd* pParent /*=NULL*/)
   : CDialog(CItemPropertiesDlg::IDD, pParent), m_ItemID(itemID)
{
   //{{AFX_DATA_INIT(CItemPropertiesDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}


void CItemPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CItemPropertiesDlg)
   DDX_Control(pDX, IDC_LIST, m_list);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CItemPropertiesDlg, CDialog)
   //{{AFX_MSG_MAP(CItemPropertiesDlg)
   ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemPropertiesDlg message handlers

BOOL CItemPropertiesDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   if( !m_OPCItemProps.IsOk() )
   {
      CString appName((LPCSTR)AFX_IDS_APP_TITLE);
      MessageBox(_T("Item Properties not supported by this Server."), appName, MB_OK);
      OnCancel();
   }

   CString title;
   GetWindowText( title );
   title += m_ItemID;
   SetWindowText( title );

   m_list.InsertColumn(0, _T("Property"), LVCFMT_LEFT, 170, 0);
   m_list.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 130, 0);

   OnRefresh();
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CItemPropertiesDlg::OnRefresh()
{
USES_CONVERSION;
   CWaitCursor wait;

   if( m_OPCItemProps.IsOk() )
   {
      m_list.DeleteAllItems();

      DWORD numItems = 0;
      DWORD* pParameterIDs = NULL;
      LPWSTR* pDescriptions = NULL;
      VARTYPE* pDataTypes = NULL;
      HRESULT hr = m_OPCItemProps.QueryAvailableProperties(T2OLE(m_ItemID.GetBuffer(0)),
                                        &numItems,
                                        &pParameterIDs,
                                        &pDescriptions,
                                        &pDataTypes);
      if( SUCCEEDED(hr) )
      {
         VARIANT* pValues = NULL;
         HRESULT* pErrors = NULL;
         hr = m_OPCItemProps.GetItemProperties(T2OLE(m_ItemID.GetBuffer(0)),
                                     numItems,
                                     pParameterIDs,
                                     &pValues,
                                     &pErrors);
         if( SUCCEEDED(hr) )
         {
            LV_ITEM     lvitem;
            for( DWORD iIndex=0; iIndex<numItems; iIndex++)
            {
               memset(&lvitem, 0, sizeof(LV_ITEM));
               lvitem.mask     = LVIF_TEXT; //|LVIF_IMAGE;
               lvitem.iItem    = iIndex;
               lvitem.iSubItem = 0;
               lvitem.pszText  = OLE2T(pDescriptions[iIndex]);
               lvitem.iImage   = 2;
               m_list.InsertItem(&lvitem);

               lvitem.mask     = LVIF_TEXT;
               lvitem.iSubItem = 1;
               CString text;
               VariantToString(pValues[iIndex], text);
               lvitem.pszText = (LPTSTR)(LPCTSTR)text;
               m_list.SetItem(&lvitem);

               if(pDescriptions[iIndex]) CoTaskMemFree(pDescriptions[iIndex]);
               if(pValues) VariantClear( &pValues[iIndex] );
            }
         }
         if( pParameterIDs ) CoTaskMemFree(pParameterIDs);
         if( pDescriptions ) CoTaskMemFree(pDescriptions);
         if( pDataTypes ) CoTaskMemFree(pDataTypes);
         if( pValues ) CoTaskMemFree(pValues);
         if( pErrors ) CoTaskMemFree(pErrors);
      }
   }
}
