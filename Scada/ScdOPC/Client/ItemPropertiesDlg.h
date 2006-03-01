//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  ItemProperties.h
//  $Author    :  Jim Hansen
//
//  Description:  This dialog displays the OPC 2.0 Item properties.
//
//**************************************************************************
#if !defined(AFX_ITEMPARAMETERSDLG_H__E23B9331_EE16_11D1_A27E_00E098013D8D__INCLUDED_)
#define AFX_ITEMPARAMETERSDLG_H__E23B9331_EE16_11D1_A27E_00E098013D8D__INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CItemPropertiesDlg dialog

class CItemPropertiesDlg : public CDialog
{
// Construction
public:
   CItemPropertiesDlg(LPCTSTR itemID, CWnd* pParent = NULL);   // standard constructor

    void    SetOPCPointer( IUnknown* pUnk ) {m_OPCItemProps.Attach(pUnk);}
// Dialog Data
   //{{AFX_DATA(CItemPropertiesDlg)
   enum { IDD = IDD_ITEM_QUERY_PROPERTIES };
   CListCtrl   m_list;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CItemPropertiesDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
    OPCItemProperties    m_OPCItemProps;
    CString      m_ItemID;

   // Generated message map functions
   //{{AFX_MSG(CItemPropertiesDlg)
   virtual BOOL OnInitDialog();
   afx_msg void OnRefresh();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMPARAMETERSDLG_H__E23B9331_EE16_11D1_A27E_00E098013D8D__INCLUDED_)
