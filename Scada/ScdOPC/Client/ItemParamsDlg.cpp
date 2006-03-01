// ItemParamsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OPC Client.h"
#include "ItemParamsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ItemParamsDlg dialog


ItemParamsDlg::ItemParamsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ItemParamsDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(ItemParamsDlg)
   m_active = FALSE;
   m_accessPath = _T("");
   m_accessRights = _T("");
   m_clientHandle = _T("");
   m_euType = _T("");
   m_euValue = _T("");
   m_itemID = _T("");
   m_nativeData = _T("");
   m_requestedData = _T("");
   m_serverHandle = _T("");
   //}}AFX_DATA_INIT
   pItemAttr = NULL;
}


void ItemParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ItemParamsDlg)
	DDX_Check(pDX, IDC_ACTIVE, m_active);
	DDX_Text(pDX, IDC_ACCESS_PATH, m_accessPath);
	DDX_Text(pDX, IDC_ACCESS_RIGHTS, m_accessRights);
	DDX_Text(pDX, IDC_CLIENT_HANDLE, m_clientHandle);
	DDX_Text(pDX, IDC_EU_TYPE, m_euType);
	DDX_Text(pDX, IDC_EU_VALUE, m_euValue);
	DDX_Text(pDX, IDC_ITEMID, m_itemID);
	DDX_Text(pDX, IDC_NATIVE_DATA, m_nativeData);
	DDX_Text(pDX, IDC_REQUESTED_DATA, m_requestedData);
	DDX_Text(pDX, IDC_SERVER_HANDLE, m_serverHandle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ItemParamsDlg, CDialog)
	//{{AFX_MSG_MAP(ItemParamsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ItemParamsDlg message handlers

BOOL ItemParamsDlg::OnInitDialog() 
{
   ASSERT( pItemAttr );
   UpdateData(TRUE);
   m_itemID = pItemAttr->szItemID;
   m_accessPath = pItemAttr->szAccessPath;
   m_active = pItemAttr->bActive;

   CString temp;
   temp.Format( m_clientHandle, pItemAttr->hClient );
   m_clientHandle = temp;

   temp.Format( m_serverHandle, pItemAttr->hServer );
   m_serverHandle = temp;

   if( pItemAttr->dwAccessRights == OPC_READABLE )
      m_accessRights = _T("Item is Readable");
   else if( pItemAttr->dwAccessRights == OPC_WRITEABLE )
      m_accessRights = _T("Item is Writeable");
   else if( pItemAttr->dwAccessRights == (OPC_READABLE|OPC_WRITEABLE) )
      m_accessRights = _T("Item is Readable and Writeable");
   else
      m_accessRights = _T("Unknown Access Rights");

   CString datatype;
   if( (pItemAttr->vtRequestedDataType & VT_ARRAY) == VT_ARRAY )
      datatype = _T("Array of ");
   switch( pItemAttr->vtRequestedDataType & ~VT_ARRAY )
   {
      case VT_BOOL:  datatype += _T("Boolean");   break;
      case VT_UI1:  datatype += _T("Byte");   break;
      case VT_I2:  datatype += _T("Short");   break;
      case VT_I4:  datatype += _T("Long");   break;
      case VT_R4:  datatype += _T("Float");   break;
      case VT_R8:  datatype += _T("Double");   break;
      case VT_BSTR:  datatype += _T("String");   break;
      default:  datatype += _T("Unknown");   break;
   }
   if( (pItemAttr->vtRequestedDataType & VT_ARRAY) == VT_ARRAY )
      datatype = _T("s");
   temp.Format( m_requestedData, (LPCTSTR)datatype );
   m_requestedData = temp;
   
   datatype.Empty();
   if( (pItemAttr->vtCanonicalDataType & VT_ARRAY) == VT_ARRAY )
      datatype = _T("Array of ");
   switch( pItemAttr->vtCanonicalDataType & ~VT_ARRAY )
   {
      case VT_BOOL:  datatype += _T("Boolean");   break;
      case VT_UI1:  datatype += _T("Byte");   break;
      case VT_I2:  datatype += _T("Short");   break;
      case VT_I4:  datatype += _T("Long");   break;
      case VT_R4:  datatype += _T("Float");   break;
      case VT_R8:  datatype += _T("Double");   break;
      case VT_BSTR:  datatype += _T("String");   break;
      default:  datatype += _T("Unknown");   break;
   }
   if( (pItemAttr->vtCanonicalDataType & VT_ARRAY) == VT_ARRAY )
      datatype = _T("s");
   temp.Format( m_nativeData, (LPCTSTR)datatype );
   m_nativeData = temp;
   
   if( pItemAttr->dwEUType == OPC_NOENUM )
      datatype = _T("None");
   else if( pItemAttr->dwEUType == OPC_ANALOG )
      datatype = _T("Analog");
   else if( pItemAttr->dwEUType == OPC_ENUMERATED )
      datatype = _T("Enumerated");
   else
      datatype = _T("Unknown");
   temp.Format( m_euType, (LPCTSTR)datatype );
   m_euType = temp;
   
   // Also pItemAttr->vEUInfo
   CDialog::OnInitDialog();


   return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
