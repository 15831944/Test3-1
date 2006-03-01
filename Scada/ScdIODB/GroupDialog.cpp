//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  GroupDialog.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Group name dialog.
//
//
//**************************************************************************

#include "stdafx.h"
#include "ScdIODB.h"
#include "GroupDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GroupDialog dialog


GroupDialog::GroupDialog(CWnd* pParent /*=NULL*/)
   : CDialog(GroupDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(GroupDialog)
   m_name = _T("");
   //}}AFX_DATA_INIT
}

GroupDialog::GroupDialog(OTagGroup* pGroup, CWnd* pParent /*=NULL*/)
   : CDialog(GroupDialog::IDD, pParent)
{
   m_name = pGroup->name;
   m_pGroup = pGroup;
}


void GroupDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(GroupDialog)
   DDX_Text(pDX, IDC_NAME, m_name);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GroupDialog, CDialog)
   //{{AFX_MSG_MAP(GroupDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GroupDialog message handlers

void GroupDialog::OnOK() 
{
   UpdateData( );
	POSITION pos = m_pGroup->parent->groups.GetHeadPosition();
    while( pos )
    {
        OTagGroup* pNextGroup = m_pGroup->parent->groups.GetNext(pos);
        if( pNextGroup->name.CompareNoCase(m_name)==0
         && pNextGroup != m_pGroup)
        {
            AfxMessageBox( _T("Duplicate name") );
            return;
        }
    }
	
	CDialog::OnOK();
}
