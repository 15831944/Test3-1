#ifndef __RCTEDITDLG_H
#define __RCTEDITDLG_H

#pragma once

#include "gpwfuncs.h"
#include "scdver.h"
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "project.h"


// CRctEditDlg dialog

class CRctEditDlg : public CDialog
  {
  DECLARE_DYNAMIC(CRctEditDlg)

  public:
    CRctEditDlg(LPCTSTR pNdTag, LPCTSTR RctFileName, CWnd* pParent = NULL);   // standard constructor
    virtual ~CRctEditDlg();

    // Dialog Data
    enum { IDD = IDD_RCTEDITDLG };

  protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
  public:
    afx_msg void OnBnClickedOk();

  protected:
    CString             m_NdTag;
    CString             m_RctFileName;

    CRctBlkInformation  m_RctInfo;

  public:
    afx_msg void OnBnClickedCancel();
  };

#endif