//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  DeviceDialog.cpp
//  $Author    :  Jim Hansen
//
//  Description:  Device properties dialog
//
//
//**************************************************************************

#include "stdafx.h"
#include "ScdIODB.h"
#include "DeviceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DeviceDialog dialog


DeviceDialog::DeviceDialog(CWnd* pParent /*=NULL*/)
   : CDialog(DeviceDialog::IDD, pParent), pDoc(NULL), pDevice(NULL)
{
   //{{AFX_DATA_INIT(DeviceDialog)
   m_name = _T("");
   //m_address = 0;
   //m_floatFormat = -1;
   //m_port = _T("");
   m_simulate = FALSE;
   //m_timeout = 0;
   //m_wordSwap = FALSE;
   //m_delay = 0;
   //}}AFX_DATA_INIT
}

DeviceDialog::DeviceDialog(ODevice* device, CDoc* doc, CWnd* pParent /*=NULL*/)
   : CDialog(DeviceDialog::IDD, pParent), pDoc(doc), pDevice(device)
{
   ASSERT_VALID( pDoc );
   CSLock wait( &uiCS );
   m_name = pDevice->name;
   m_simulate = pDevice->simulate;

   //m_port         = pDevice->primary.portName;
   //m_address      = pDevice->primary.address;
   //m_floatFormat  = pDevice->primary.format;
   //m_port         = pDevice->primary.portName;
   //m_timeout      = pDevice->primary.timeout;
   //m_wordSwap     = pDevice->primary.wordSwap;
   //m_delay        = pDevice->primary.interPollDelay;
}

BOOL DeviceDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Initialize port combobox
//   CComboBox* cb1 = (CComboBox*)GetDlgItem( IDC_PORT );
//   ASSERT_VALID( cb1 );
//   POSITION pos = pDoc->ports.GetHeadPosition();
//   while( pos )
//      {
//      OPort* pPort = pDoc->ports.GetNext( pos );
//      if( pPort->enable )
//         {
//         cb1->AddString( pPort->name );
//         }
//      }

   UpdateData( FALSE );
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL DeviceDialog::UpdateDevice()
{
   CSLock wait( &uiCS );
   pDevice->name = m_name;
   pDevice->simulate = m_simulate;

   //pDevice->primary.address  = m_address;
   //pDevice->primary.format   = (ODeviceParams::OFORMAT)m_floatFormat;
   //pDevice->primary.portName = m_port;
   //pDevice->primary.timeout  = m_timeout;
   //pDevice->primary.wordSwap = m_wordSwap;
   //pDevice->primary.interPollDelay = m_delay;

   //// match port names up with ports
   //POSITION portPos = pDoc->ports.GetHeadPosition();
   //while( portPos )
   //   {
   //   OPort* pPort = pDoc->ports.GetNext( portPos );
   //   if( pPort->name == pDevice->primary.portName )
   //      pDevice->primary.pPort = pPort;
   //   }

   return TRUE;
}

void DeviceDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(DeviceDialog)
   DDX_Text(pDX, IDC_NAME, m_name);
   //DDX_Text(pDX, IDC_ADDRESS, m_address);
   //DDV_MinMaxByte(pDX, m_address, 1, 247);
   //DDX_CBString(pDX, IDC_PORT, m_port);
   DDX_Check(pDX, IDC_SIMULATE, m_simulate);
   //DDX_Text(pDX, IDC_TIMEOUT, m_timeout);
   //DDX_Check(pDX, IDC_WORDSWAP, m_wordSwap);
   //DDX_Text(pDX, IDC_MSGDELAY, m_delay);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DeviceDialog, CDialog)
   //{{AFX_MSG_MAP(DeviceDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DeviceDialog message handlers


void DeviceDialog::OnOK() 
{
   UpdateData( );
	POSITION pos = pDoc->devices.groups.GetHeadPosition();
    while( pos )
    {
        OTagGroup* pNextGroup = pDoc->devices.groups.GetNext(pos);
        if( pNextGroup->name.CompareNoCase(m_name)==0
         && pNextGroup != pDevice)
        {
            AfxMessageBox( _T("Duplicate name") );
            return;
        }
    }
	
	CDialog::OnOK();
}

void DeviceDialog::OnUpdateall() 
{
	// TODO: Add your command handler code here
	
}
