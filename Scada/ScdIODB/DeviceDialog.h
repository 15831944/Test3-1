//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  DeviceDialog.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  Device properties dialog
//
//
//**************************************************************************

#ifndef __DEVICEDIALOG_H__             // Only Include Once
#define __DEVICEDIALOG_H__

class DeviceDialog : public CDialog
{
// Construction
public:
   DeviceDialog(CWnd* pParent = NULL);   // standard constructor
   DeviceDialog(ODevice* device, CDoc* doc, CWnd* pParent = NULL);
   BOOL UpdateDevice();

// Dialog Data
   //{{AFX_DATA(DeviceDialog)
	enum { IDD = IDD_DEVICE };
   CString  m_name;
   //BYTE  m_address;
   //int      m_floatFormat;
   //CString  m_port;
   BOOL  m_simulate;
   //DWORD m_timeout;
   //BOOL  m_wordSwap;
	 //DWORD	m_delay;
	//}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(DeviceDialog)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   CDoc*    pDoc;
   ODevice* pDevice;
   // Generated message map functions
   //{{AFX_MSG(DeviceDialog)
   virtual BOOL OnInitDialog();
   afx_msg void OnId();
	virtual void OnOK();
	afx_msg void OnUpdateall();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif