// RctEditDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SysCAD.h"
#include "RctEditDlg.h"
#include "sp_db.h"


// CRctEditDlg dialog

IMPLEMENT_DYNAMIC(CRctEditDlg, CDialog)
CRctEditDlg::CRctEditDlg(LPCTSTR pNdTag, LPCTSTR RctFileName, CWnd* pParent /*=NULL*/)
: CDialog(CRctEditDlg::IDD, pParent),
m_NdTag(pNdTag),
m_RctFileName(RctFileName)
  {
  int NRBs = gs_pPrj->GetRctInfo(m_RctInfo);

  dbgpln("****************************************************************************************");
  for (int iRB=0; iRB<m_RctInfo.m_RBs.GetCount(); iRB++)
    {
    CRctBlkInformation::CRctBlk &RB=m_RctInfo.m_RBs[iRB];
    dbgpln("%s %.*s", RB.m_sNdTag, 88-(RB.m_sNdTag.GetLength()+1),
      "----------------------------------------------------------------------------------------");
    dbgpln("  File : %s", RB.m_sRctFile);
    dbgpln("  Tag  : %s", RB.m_sBlkTag);
    dbgpln("  ; %s", RB.m_sComment);

    for (int iRct=0; iRct< RB.m_Rcts.GetCount(); iRct++)
      {                             
      CRctBlkInformation::CRct &Rct=RB.m_Rcts[iRct];

      bool DoneReactions=false;
      LPCTSTR Header=NULL;
      switch(Rct.m_iBasis)
        {
        case RctNull:
          Header="NULL";
          break;
        case RctEquilibrium:
        case RctRate:
        case RctExtent:
          Header=NULL;
          break;
        case RctSource:
          Header="Source";
          break;
        case RctSink:
          Header="Sink";
          break;
        case RctHX1:
        case RctHX2:
        case RctPwr:
          Header="HeatExchange";
          break;
        };
      if (Header)
        dbgpln("  %s:", Header);
      else if (!DoneReactions)
        {
        dbgpln("  Reactions:");
        DoneReactions=true;
        }
      if (Rct.m_sComment.GetLength()>0)
        dbgpln("    ; %s", Rct.m_sComment);
      dbgpln("    \"%s\":%s Sequence: %i", Rct.m_sTag, Rct.m_sDefn, Rct.m_iSeqNo);
      }
    dbgpln("  End");
    }


  dbgpln("****************************************************************************************");
  dbgpln("%3s %-25s %-25s %10s %14s", "No", "Tag", "Sym", "MoleWt", "mlHf");
  for (int is=0; is<SDB.Count(); is++)
    {
    CSpecie & D=SDB[is];

    dbgpln("%3i %-25.25s %-25.25s %10.3f %14.3f", is, D.Tag(), D.Sym(), D.MoleWt(), D.mlHf(1, StdT, StdP, NULL, NULL));

    }
  dbgpln("****************************************************************************************");
  }

CRctEditDlg::~CRctEditDlg()
  {
  }

void CRctEditDlg::DoDataExchange(CDataExchange* pDX)
  {
  CDialog::DoDataExchange(pDX);
  }


BEGIN_MESSAGE_MAP(CRctEditDlg, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CRctEditDlg message handlers

void CRctEditDlg::OnBnClickedOk()
  {
  // ReWrite the files


  gs_pPrj->SetRctInfo("XXXX");

  OnOK();
  }

void CRctEditDlg::OnBnClickedCancel()
  {

  OnCancel();
  }
