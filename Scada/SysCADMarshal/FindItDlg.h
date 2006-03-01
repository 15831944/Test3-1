#pragma once
#include "afxwin.h"

// CFindItDlg dialog

enum
  {
  eFindStr_SlotTag,     // this order is reflected in the ComboBox Addstring order
  eFindStr_OPCTag,
  eFindStr_LinkTag,
  eFindStr_SimulatorTag,
  };

class CSysCADMarshalDoc;
class CFindItDlg : public CDialog
{
	DECLARE_DYNAMIC(CFindItDlg)

public:
	CFindItDlg(CSysCADMarshalDoc * pDoc, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFindItDlg();

// Dialog Data
	enum { IDD = IDD_FINDIT };

  static bool OpenIt(CWnd * pParent, CSysCADMarshalDoc * pDoc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


  DECLARE_MESSAGE_MAP()
public:
  virtual BOOL    OnInitDialog( );
  virtual void    OnOK();
  virtual void    OnCancel();
  void            SaveState();
  void            SetSelection(bool On);

  static CFindItDlg * sm_pTheOne;

  CSysCADMarshalDoc * m_pDoc;
  CComboBox       m_FilterTag;
  CComboBox       m_FilterSimTag;
  CComboBox       m_FilterOPCTag;
  CComboBox       m_FilterType;
  CComboBox       m_How;
  bool            m_Off;

  afx_msg void OnBnClickedFilteron();
  afx_msg void OnBnClickedFilteroff();
  CButton m_OffBtn;
  CButton m_Ignore;
  CButton m_AndClauses;
  CButton m_ErrorsOnly;
  CButton m_HoldsOnly;
};
