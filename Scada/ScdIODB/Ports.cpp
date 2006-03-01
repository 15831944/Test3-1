//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Ports.cpp
//  $Author    :  Jim Hansen
//
//  Description:  The Port properties dialog
//
//
//**************************************************************************

#include "stdafx.h"
#include "ScdIODB.h"
#include "Ports.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OPortDlg dialog


//OPortDlg::OPortDlg(CDoc* doc, CWnd* pParent /*=NULL*/)
//: CDialog(OPortDlg::IDD, pParent), pDoc( doc ), pCurrent(NULL)
//{
//   //{{AFX_DATA_INIT(OPortDlg)
//   m_baud = -1;
//   m_data = -1;
//   m_flow = -1;
//   m_stop = -1;
//   m_parity = -1;
//   //}}AFX_DATA_INIT
//}
//
//
//void OPortDlg::DoDataExchange(CDataExchange* pDX)
//{
//   CDialog::DoDataExchange(pDX);
//   //{{AFX_DATA_MAP(OPortDlg)
//   DDX_CBIndex(pDX, IDC_BAUD, m_baud);
//   DDX_CBIndex(pDX, IDC_DATA, m_data);
//   DDX_CBIndex(pDX, IDC_FLOW, m_flow);
//   DDX_CBIndex(pDX, IDC_STOP, m_stop);
//   DDX_CBIndex(pDX, IDC_PORT_PARITY, m_parity);
//   //}}AFX_DATA_MAP
//}
//
//
//BEGIN_MESSAGE_MAP(OPortDlg, CDialog)
//   //{{AFX_MSG_MAP(OPortDlg)
//   ON_CBN_SELCHANGE(IDC_PORTS, OnChangePorts)
//   //}}AFX_MSG_MAP
//END_MESSAGE_MAP()
//
///////////////////////////////////////////////////////////////////////////////
//// OPortDlg message handlers
//
//BOOL OPortDlg::OnInitDialog()
//{
//   CDialog::OnInitDialog();
//
//   CComboBox* portBox = (CComboBox*)GetDlgItem( IDC_PORTS );
//   ASSERT_VALID( portBox );
//
//   ASSERT( !pDoc->ports.IsEmpty() );
//   POSITION pos = pDoc->ports.GetHeadPosition();
//   while( pos )
//      {
//      OPort* pPort = pDoc->ports.GetNext( pos );
//      int index = portBox->AddString( pPort->name );
//      portBox->SetItemDataPtr( index, pPort );
//      }
//   portBox->SetCurSel( 0 );
//   OnChangePorts();
//
//   return TRUE;  // return TRUE unless you set the focus to a control
//                 // EXCEPTION: OCX Property Pages should return FALSE
//}
//
//void OPortDlg::OnChangePorts()
//{
//   UpdateData( TRUE );
//   if( pCurrent )
//      {
//      switch( m_baud )
//         {
//         case 0: pCurrent->baud =   300; break;
//         case 1: pCurrent->baud =   600; break;
//         case 2: pCurrent->baud =  1200; break;
//         case 3: pCurrent->baud =  2400; break;
//         case 4: pCurrent->baud =  4800; break;
//         case 5: pCurrent->baud =  9600; break;
//         case 6: pCurrent->baud = 19200; break;
//         case 7: pCurrent->baud = 38400; break;
//         case 8: pCurrent->baud = 56000; break;
//         case 9: pCurrent->baud = 115200; break;
//         default: ASSERT( FALSE );
//         }
//      switch( m_data )
//         {
//         case 0: pCurrent->dataBits = 7; break;
//         case 1: pCurrent->dataBits = 8; break;
//         default: ASSERT( FALSE );
//         }
//      pCurrent->enable = TRUE;
//      pCurrent->flow = (OPort::FLOW)m_flow;
//      pCurrent->parity = m_parity;  // this has to match !!
//      pCurrent->stop = (m_stop==0 ? ONESTOPBIT : TWOSTOPBITS );
//      pCurrent->reInitialize = TRUE;
//      }
//
//   CComboBox* portBox = (CComboBox*)GetDlgItem( IDC_PORTS );
//   ASSERT_VALID( portBox );
//
//   int index = portBox->GetCurSel();
//   pCurrent = (OPort*)portBox->GetItemDataPtr( index );
//
//   // translate into dialog list indices
//   switch( pCurrent->baud )
//      {
//      case   300: m_baud = 0; break;
//      case   600: m_baud = 1; break;
//      case  1200: m_baud = 2; break;
//      case  2400: m_baud = 3; break;
//      case  4800: m_baud = 4; break;
//      case  9600: m_baud = 5; break;
//      case 19200: m_baud = 6; break;
//      case 38400: m_baud = 7; break;
//      case 56000: m_baud = 8; break;
//      case 115200: m_baud = 9; break;
//      default: ASSERT( FALSE );
//      }
//   switch( pCurrent->dataBits )
//      {
//      case 7: m_data = 0; break;
//      case 8: m_data = 1; break;
//      default: ASSERT( FALSE );
//      }
//   m_flow = pCurrent->flow;
//   m_parity = pCurrent->parity;
//   m_stop = (pCurrent->stop==ONESTOPBIT ? 0 : 1);
//   UpdateData( FALSE );
//}
//
//void OPortDlg::OnOK()
//{
//   OnChangePorts();
//
//   CDialog::OnOK();
//}
