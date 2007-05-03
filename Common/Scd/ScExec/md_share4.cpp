//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __MD_SHARE4_CPP
#include "sc_defs.h"
#include "md_share4.h"

//===========================================================================
//
//
//
//===========================================================================

CMdlAction::CMdlAction()
  {
  iIndex=0;
  iType=MAT_NULL;
  fEnabled=0;
  strcpy(cName, "");
  iValue=0;
  dValue=0;
  dValueMin=0;
  dValueMax=0;
  dBumpless=0;
  };
         
CMdlAction::CMdlAction(int Index, eMdlActionTypes Type, bool Enabled, LPCSTR Name, int IntValue)
  {
  iIndex=Index;
  iType=Type;
  fEnabled=Enabled;
  strcpy(cName, Name);
  iValue=IntValue;
  dValue=0;
  dValueMin=0;
  dValueMax=0;
  dBumpless=0;
  };

CMdlAction::CMdlAction(int Index, eMdlActionTypes Type, bool Enabled, LPCSTR Name, bool InvalidOK,
                       double DblValue, double ValueMin, double ValueMax, double Bumpless)
  {
  iIndex=Index;
  iType=Type;
  fEnabled=Enabled;
  strcpy(cName, Name);
  fInvalidOK=InvalidOK;
  dValue=DblValue;
  dValueMin=ValueMin;
  dValueMax=ValueMax;
  dBumpless=Bumpless;
  };


//class DllImportExport CMdlActionArray : public CArray<CMdlAction, CMdlAction&> {};
//typedef CArray<CMdlAction, CMdlAction&> CMdlActionArray;

//===========================================================================
//
//
//
//===========================================================================
CMdlGraphicWnd::CMdlGraphicWnd(eMdlGraphicTasks Task, CWnd * Wnd, CRect ClientRect, CDC * DC) 
  {
  m_eTask       = Task;
  m_pWnd        = Wnd;
  m_pDC         = DC;
  m_pPaintDC    = NULL;
  m_ClientRect  = ClientRect;
  m_TextSize    = CPoint(-1,-1);
  m_MousePt     = CPoint(-1,-1);
  m_MouseFlags  = 0;
  m_bReturn     = false;
  };

CMdlGraphicWnd::CMdlGraphicWnd(eMdlGraphicTasks Task, CWnd * Wnd, CRect ClientRect, CPaintDC * DC, CRect TextSize) 
  {
  m_eTask       = Task;
  m_pWnd        = Wnd;
  m_pDC         = NULL;
  m_pPaintDC    = DC;
  m_ClientRect  = ClientRect;
  m_TextSize    = TextSize.BottomRight();
  m_MousePt     = CPoint(-1,-1);
  m_MouseFlags  = 0;
  m_bReturn     = false;
  };

CMdlGraphicWnd::CMdlGraphicWnd(eMdlGraphicTasks Task, CWnd * Wnd, CRect ClientRect, CPoint MousePt, UINT MouseFlags) 
  {
  m_eTask       = Task;
  m_pWnd        = Wnd;
  m_pDC         = NULL;
  m_pPaintDC    = NULL;
  m_ClientRect  = ClientRect;
  m_TextSize    = CPoint(-1,-1);
  m_MousePt     = MousePt;
  m_MouseFlags  = MouseFlags;
  m_bReturn     = false;
  };

//---------------------------------------------------------------------------

CMdlGraphic::CMdlGraphic()
  {
  m_iIndex    = 0;
  m_iType     = MGT_Simple;
  m_fEnabled  = 0;
  strcpy(m_cName, "");
  };

CMdlGraphic::CMdlGraphic(int Index, eMdlGraphicTypes Type, bool Enabled, LPCSTR Name, int IntValue)
  {
  m_iIndex    = Index;
  m_iType     = Type;
  m_fEnabled  = Enabled;
  strcpy(m_cName, Name);
  };

CMdlGraphic::CMdlGraphic(int Index, eMdlGraphicTypes Type, bool Enabled, LPCSTR Name, bool InvalidOK,
                         double DblValue, double ValueMin, double ValueMax, double Bumpless)
  {
  m_iIndex    = Index;
  m_iType     = Type;
  m_fEnabled  = Enabled;
  strcpy(m_cName, Name);
  };

//===========================================================================
//
//
//
//===========================================================================
