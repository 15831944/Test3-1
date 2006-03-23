//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SQSZPPG_H
#define  __SQSZPPG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "resource.h"
#include "MdlCfg.h"
#include "afxwin.h"
#include "afxcmn.h"

class CSqSzPPg: public CMdlCfgBase 
  {
  public:
    CSqSzPPg(CMdlCfgSheet * Sheet);
    //{{AFX_DATA(CSqSzPPg)
    enum { IDD = IDD_PP_MDLCFG_SIEVE };
    CCustomListCtrl	m_Sizes;
    CComboBox	m_SeriesNames;
    CComboBox	m_Units;
    CString m_SeriesData;
    CString m_SeriesDataMin;
    CString m_SeriesDataMax;
    BOOL m_SeriesQMidPt;
    long m_SeriesDataPrecision;
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CSqSzPPg)
    public:
    virtual void OnOK();
    virtual BOOL OnKillActive();
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    CSD_CfgHelper   H;
    int             CurSeriesIndex;
    CDataCnv*       pSerCnv;

    void PopulateAll();

    // Populate the ManualDisplayList with the sizes
    int PopulateSizes(BOOL DoSort, int SetSelIndex);

    // Populate the SizeDisplayList with the size data
    void PopulateDisplaySizes(void);

    // Create the series data based on the mSerDataType
    void SeriesCreate();

    // Update the number of decimal places from the SpinControl
    void UpdatePrecision(void);

    // Update the Spin Control to match the specified precision
    void SetPrecision(long n);

    //{{AFX_MSG(CSqSzPPg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeSerName();
    afx_msg void OnEndlabeleditSerSizes(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclkSerSizes(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSerAddsize();
    afx_msg void OnSerRemovesize();
    afx_msg void OnSelchangeSerUnits();
    afx_msg void OnEditchangeSerName();
    afx_msg void OnSerAddSeries();
    afx_msg void OnSerRemoveSeries();
    afx_msg void OnChangeSerData();
    afx_msg void OnSerQCreate();
    afx_msg void OnChangeSerDatamin();
    afx_msg void OnChangeSerDatamax();
    afx_msg void OnSerQmidpt();
    //}}AFX_MSG
    afx_msg LRESULT OnDoSomething(WPARAM wParam, LPARAM lParam);
    afx_msg void OnUpdateSerRemoveSeries(CCmdUI* pCmdUi);
    afx_msg void OnUpdateSerQCreate(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  public:
    // Specifies how to create a sieve series
    CComboBox mSerDataType;
    afx_msg void OnCbnSelchangeSerDataType();



    CButton m_CreateBtn;
    CButton m_QMidPtBtn;
    CEdit m_DataBtn;
    CStatic m_DataNameTxt;
    CButton m_AddBtn;
    CButton m_RemoveBtn;
    CListBox m_DisplaySizes;
    afx_msg void OnBnClickedSerToManual();
    CSpinButtonCtrl m_SpinDecimalsBtn;
    CEdit m_SpinTxt;
    afx_msg void OnDeltaposSpinDecimals(NMHDR *pNMHDR, LRESULT *pResult);
    CEdit m_EditNInts;
    CEdit m_EditMin;
    CEdit m_EditMax;
  };


class CSqSzDPg: public CMdlCfgBase 
  {
  public:
    CSqSzDPg(CMdlCfgSheet * Sheet);
    //{{AFX_DATA(CSqSzDPg)
	enum { IDD = IDD_PP_MDLCFG_DIST };
    CComboBox	m_DstNames;
    CComboBox	m_DstDefSp;
    CCustomListCtrl m_SpList;
    CComboBox	m_DstSeries;
    CString	m_DstUnits;
    double	m_DstTSize;
    double	m_DstBSize;
    double	m_DstTSizeDisp;
    double	m_DstBSizeDisp;
	//}}AFX_DATA
    //{{AFX_VIRTUAL(CSqSzDPg)
    public:
    virtual void OnOK();
    virtual BOOL OnKillActive();
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    CSD_CfgHelper   H;
    int             CurDstIndex;
    int             CurDstSeriesIndex;
    CDataCnv*       pDstSerCnv;
    CImageList      ImgList;
    CString         CurDefSpecie;

    void PopulateAll();
    void PopulateDistData();
    void SaveDistData();
    void BuildSpList();
    void BuildDefSpList();
    //{{AFX_MSG(CSqSzDPg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSelChangeDstSeries();
    afx_msg void OnDblclkSpecies(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSelChangeDstNames();
    afx_msg void OnDstAddDist();
    afx_msg void OnDstRemoveDist();
	afx_msg void OnEditchangeDstName();
	//}}AFX_MSG
    afx_msg void OnUpdateDstRemoveDist(CCmdUI* pCmdUi);
    DECLARE_MESSAGE_MAP()
  };

class CSqSzDMeasPg: public CMdlCfgBase 
  {
  public:
    CSqSzDMeasPg(CMdlCfgSheet * Sheet);
    //{{AFX_DATA(CSqSzDMeasPg)
	  enum { IDD = IDD_PP_MDLCFG_MEAS };
    //}}AFX_DATA
    //{{AFX_VIRTUAL(CSqSzDMeasPg)
    public:
    virtual void OnOK();
    virtual BOOL OnKillActive();
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:
    void PopulateAll(void);
    void PopulateDistributionList(void);
    void PopulateMeasurmentList(void);
    void ValidateNames(void);

  protected:
    CSD_CfgHelper   H;
    //{{AFX_MSG(CSqSzDMeasPg)
    virtual BOOL OnInitDialog();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
  public:
    CListBox m_MeasList;
    long CurDistIndex;
    CComboBox m_DistSel;
    afx_msg void OnCbnSelchangeDistributionSel();
    afx_msg void OnBnClickedMeasAdd();
    afx_msg void OnBnClickedMeasRemove();
    CEdit m_EditName;
    afx_msg void OnEnChangeMeasName();
    afx_msg void OnLbnSelchangeMeasList();
    CComboBox m_MeasTypeSel;
    afx_msg void OnCbnSelchangeSelMeasType();
    CComboBox m_ComboValueUnits;
    CComboBox m_ComboResultUnits;
    afx_msg void OnEnChangeEditValue();
    CEdit m_EditBoxValue;
    afx_msg void OnCbnSelchangeComboValUnits();
    afx_msg void OnCbnSelchangeComboResUnits();
    CButton m_BtnMeasUp;
    CButton m_BtnMeasDown;
    afx_msg void OnBnClickedButtonMeasUp();
    afx_msg void OnBnClickedButtonMeasDown();
    CEdit m_CountFactor;
    double dCountFactor;
    afx_msg void OnEnChangeCountFactor();

  };


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
