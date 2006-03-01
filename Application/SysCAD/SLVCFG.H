//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __SLVCFG_H
#define __SLVCFG_H

#ifndef __GPWFUNCS_H
#include "gpwfuncs.h"
#endif
#ifndef __EXECUTIV_H
#include "executiv.h"
#endif

//===========================================================================
//
//
//
//===========================================================================

const int CfgFixedPages = 6;

class CSolverCfgSheet : public CPropertySheet
  {
  DECLARE_DYNAMIC(CSolverCfgSheet)
  public:
    static int iCurrentPage;

    CSolverCfgSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    CSolverCfgSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
    virtual ~CSolverCfgSheet();

    CCustomPropertyPage* Pages[CfgFixedPages];

    //{{AFX_VIRTUAL(CSolverCfgSheet)
    public:
    virtual BOOL OnInitDialog();
    protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    //}}AFX_VIRTUAL

  protected:
    //{{AFX_MSG(CSolverCfgSheet)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

  void UpdateOptMeth(byte iReqdOptMeth);
  };

bool SetGrfPageActive(int index, bool Active);

//===========================================================================

#endif

