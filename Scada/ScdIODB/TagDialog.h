//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  TagDialog.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :
//
//  Description:  Tag properties dialog
//
//
//**************************************************************************

#ifndef __TAGDIALOG_H__             // Only Include Once
#define __TAGDIALOG_H__

#include "styles.h"

/////////////////////////////////////////////////////////////////////////////
// TagDialog dialog

class TagDialog : public CDialog
{
// Construction
public:
   TagDialog(CWnd* pParent = NULL);   // standard constructor
   TagDialog(OModbusTag*, CDoc*, CWnd* pParent = NULL);
   ~TagDialog();
   BOOL  UpdateTag( OModbusTag* pTag );

// Dialog Data
   //{{AFX_DATA(TagDialog)
	enum { IDD = IDD_TAG };
   CString  m_name;
   CString  m_description;
   CString  m_dataType;
   int      m_simulationType;
	CString	m_Value;
	//}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(TagDialog)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   CDoc*             pDoc;
   OModbusTag*       pTag;
   CPropertySheet*   sheet;
   ScalingPage*      scaling;
   void              CreateDescription();

   // Generated message map functions
   //{{AFX_MSG(TagDialog)
   afx_msg void OnScalingSettings();
   afx_msg void OnEnableScaling();
   virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSet();
	//}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

#endif
