//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  GroupParamsDlg.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Dialog to view and modify the group parameters.
//                (mainly the update rate and active flag)
//
//**************************************************************************
#include "stdafx.h"
#include "OPC Client.h"
#include "GroupParamsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GroupParamsDlg dialog


GroupParamsDlg::GroupParamsDlg(IUnknown* pGroup, CWnd* pParent /*=NULL*/)
   : CDialog(GroupParamsDlg::IDD, pParent)
{
   opcGroup.Attach( pGroup ); // get an IOPCGroupStateMgt interface
   //{{AFX_DATA_INIT(GroupParamsDlg)
   m_active = FALSE;
   m_deadband = 0.0f;
   m_LCID = 0;
   m_name = _T("");
   m_rate = 0;
   m_timebias = 0;
   //}}AFX_DATA_INIT
}


void GroupParamsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(GroupParamsDlg)
   DDX_Check(pDX, IDC_ACTIVE, m_active);
   DDX_Text(pDX, IDC_DEADBAND, m_deadband);
   DDV_MinMaxFloat(pDX, m_deadband, 0.f, 100.f);
   DDX_Text(pDX, IDC_LCID, m_LCID);
   DDX_Text(pDX, IDC_NAME, m_name);
   DDX_Text(pDX, IDC_RATE, m_rate);
   DDX_Text(pDX, IDC_TIMEBIAS, m_timebias);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(GroupParamsDlg, CDialog)
   //{{AFX_MSG_MAP(GroupParamsDlg)
   ON_BN_CLICKED(IDAPPLY, OnApply)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// GroupParamsDlg message handlers

BOOL GroupParamsDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   LPWSTR name;
   // Get state from server to initialize
   HRESULT hr = opcGroup.GetState( &m_rate, &m_active, &name, &m_timebias,
                                   &m_deadband, &m_LCID, &client, &server );
   if( SUCCEEDED(hr) )
   {
      m_name = name;
      CoTaskMemFree( name );
   }

   UpdateData( FALSE );
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

// Set state and name in the group
void GroupParamsDlg::OnApply()
{
USES_CONVERSION;
   UpdateData( TRUE );

   HRESULT hr = opcGroup.SetState( &m_rate, &m_rate, &m_active, &m_timebias,
                                   &m_deadband, &m_LCID, &client );
   hr = opcGroup.SetName( T2OLE(m_name.GetBuffer(0)) );
   UpdateData( FALSE );
}

void GroupParamsDlg::OnOK()
{
   OnApply();
   CDialog::OnOK();
}
