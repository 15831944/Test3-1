//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  CopyTagDlg.cpp
//  $Author    :  Jim Hansen
//
//  Description:  The dialog that prompts to rename/overwrite/skip a
//                copy operation that runs into a duplicate tag.
//
//**************************************************************************

#include "stdafx.h"
#include "ScdIODB.h"
#include "CopyTagDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCopyTagDlg dialog


CCopyTagDlg::CCopyTagDlg(OModbusTag* pNewTag,
                         OModbusTag* pCurrentTag,
                         CWnd* pParent /*=NULL*/)
   : CDialog(CCopyTagDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CCopyTagDlg)
   //}}AFX_DATA_INIT
   m_name = pNewTag->name;
   m_title = pNewTag->name + _T(" already exists.");
   m_current = _T("Current tag is a ");
   m_current += pCurrentTag->TypeString();
   //m_current += _T(" at ");
   //m_current += pCurrentTag->LocationString();
   if( pCurrentTag->enableProcessing )
      {
      m_current += _T(" with custom scaling");
      }
   else
      m_current += _T(" with no scaling");

   m_new = _T("  New  tag  is a ");
   m_new += pNewTag->TypeString();
   //m_new += _T(" at ");
   //m_new += pNewTag->LocationString();
   if( pNewTag->enableProcessing )
      {
      m_new += _T(" with custom scaling");
      }
   else
      m_new += _T(" with no scaling");
}


void CCopyTagDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CCopyTagDlg)
   DDX_Text(pDX, IDC_NAME, m_name);
   DDX_Text(pDX, IDC_TITLE, m_title);
   DDX_Text(pDX, IDC_CURRENT, m_current);
   DDX_Text(pDX, IDC_NEW, m_new);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCopyTagDlg, CDialog)
   //{{AFX_MSG_MAP(CCopyTagDlg)
   ON_BN_CLICKED(IDC_RENAME, OnRename)
   ON_BN_CLICKED(IDC_OVERWRITE, OnOverwrite)
   ON_BN_CLICKED(IDC_SKIP, OnSkip)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCopyTagDlg message handlers

void CCopyTagDlg::OnRename()
{
   UpdateData( TRUE );
   EndDialog( IDC_RENAME );
}

void CCopyTagDlg::OnOverwrite()
{
   EndDialog( IDC_OVERWRITE );
}

void CCopyTagDlg::OnSkip()
{
   EndDialog( IDC_SKIP );
}
