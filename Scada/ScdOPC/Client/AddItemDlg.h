//**************************************************************************
//
//  Copyright (c) FactorySoft, INC. 1996-1998, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  AddItemDlg.h
//  $Author    :  Jim Hansen
//
//  Description:  Dialog to add an item. The OPC Browse interface is used
//                to display the server's items.
//
//**************************************************************************

class AddItemDlg : public CDialog
{
// Construction
public:
   AddItemDlg(OPCClientDoc* parent, IOPCServer* pServer, CWnd* pParent = NULL);

// Dialog Data
   VARTYPE  m_vt;
   OPCNAMESPACETYPE  nameSpaceType;
   //{{AFX_DATA(AddItemDlg)
   enum { IDD = IDD_ADD_ITEM };
   CListBox m_flat;
   CListBox m_tags;
   CTreeCtrl   m_tree;
   CString  m_filter;
   CString  m_accessPath;
   CString  m_itemID;
   //}}AFX_DATA


// Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(AddItemDlg)
   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   //}}AFX_VIRTUAL

// Implementation
protected:
   OPCClientDoc* pDoc;
   OPCBrowseServerAddressSpace browse;
   void     UpdateFlat();
   HRESULT  BrowseHere( HTREEITEM item );

   // Generated message map functions
   //{{AFX_MSG(AddItemDlg)
   afx_msg void OnNative();
   afx_msg void OnBool();
   afx_msg void OnShort();
   afx_msg void OnLong();
   afx_msg void OnDouble();
   afx_msg void OnString();
   afx_msg void OnSelchangeTags();
   virtual BOOL OnInitDialog();
   afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnSelchangeFlat();
   afx_msg void OnAdditem();
   afx_msg void OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult);
   afx_msg void OnNewFilter();
   afx_msg void OnDblclkTags();
   afx_msg void OnDblclkFlat();
   afx_msg void OnOpcItemparameters();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};
