//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __MDLSLCT_H
#define __MDLSLCT_H

#include "gpfuncs.h"
#include "grfcmcmd.h"
#include "..\resource.h"
#include "afxcmn.h"
#ifndef __DOCROOT_H
#include "docroot.h"
#endif

#ifndef __GRFWND_H
#include "grfwnd.h"
#endif
#include "afxwin.h"

//---------------------------------------------------------------------------

class CModelListHelper
  {
  public:
    Strng_List    m_ModelClassLst;
    Strng_List    m_ModelDescLst;
    Strng_List    m_ModelBaseTagLst;
    Strng_List    m_ModelDrwGroupLst;
    pStrng        m_pDescStr;
    pStrng        m_pBaseTag;
    pStrng        m_pModelClass;
    pStrng        m_pModelSubClass;
    pStrng        m_pDrwGroup;

    CModelListHelper();
    int BuildList(LPCTSTR SectName, char* pGroup, CComboBox* pCB, CTreeCtrl * pTree, CProfINIFile *pPF);
    void Find(char* pMdlName);
    void Find(DWORD MdlIndex);
  };

//---------------------------------------------------------------------------

class CUnitDlgListBox : public CListBox
  {
  public:
    CUnitDlgListBox(): CListBox() {};
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct );
    virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  };

//---------------------------------------------------------------------------

enum CDlgWhat { CDlgWhat_Null, 
CDlgWhat_InsUnit, 
CDlgWhat_ConUnit, 
CDlgWhat_ConLink, 
CDlgWhat_ConCLnk, 
CDlgWhat_ConELnk, 
CDlgWhat_ConALnk, 
CDlgWhat_ChgUnit, 
CDlgWhat_Last
  };

//---------------------------------------------------------------------------

const int SymMRUMax = 6;

const LPCTSTR DefaultLastUnit     = "Feed_XPG-1";//Feeder/CrossPage(1)";
const LPCTSTR DefaultLastLink     = "Process:Piping:Pipe_GP";
const LPCTSTR DefaultLastCLnk     = "IO - CtrlLink";
const LPCTSTR DefaultLastELnk     = "Electrical:Cable";
const LPCTSTR DefaultLastALnk     = "Air:Link";

const LPCTSTR NullUnitSymbol      = "$$$$$$$$$$";
const LPCTSTR DefaultUnitSymbol   = "*Default*";
const LPCTSTR OriginalUnitSymbol  = "*Original*";

//---------------------------------------------------------------------------
#endif
