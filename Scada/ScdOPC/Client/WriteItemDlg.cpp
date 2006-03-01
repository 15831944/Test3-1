//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  WriteItemDlg.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Dialog for writing values to an item.
//                This just returns a value from the user.
//
//**************************************************************************
#include "stdafx.h"
#include "OPC Client.h"
#include "WriteItemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WriteItemDlg dialog


WriteItemDlg::WriteItemDlg(CWnd* pParent /*=NULL*/)
   : CDialog(WriteItemDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(WriteItemDlg)
   m_async = FALSE;
   m_value = _T("");
   //}}AFX_DATA_INIT
}

BOOL WriteItemDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   CString title;
   GetWindowText(title);
   title += m_item;
   SetWindowText( title );

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void WriteItemDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(WriteItemDlg)
   DDX_Check(pDX, IDC_ASYNC, m_async);
   DDX_Text(pDX, IDC_VALUE, m_value);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(WriteItemDlg, CDialog)
   //{{AFX_MSG_MAP(WriteItemDlg)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WriteItemDlg message handlers
