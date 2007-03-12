//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "scdver.h"
#include "..\resource.h"
#include "helpinfo.h"
#include "mdlslctl.h"
#include "project.h"
#include "licbase.h"
#include "msgwnd.h"
#include "sfe_base.h"
//#include "optoff.h"
                        
#ifdef _DEBUG
#undef THIS_FILE    
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

void CUnitDlgListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
  {
  ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);
  LPCTSTR lpszText = (LPCTSTR) lpDrawItemStruct->itemData;
  ASSERT(lpszText != NULL);
  CDC dc;

  dc.Attach(lpDrawItemStruct->hDC);

  // Save these values to restore them when done drawing.
  COLORREF crOldTextColor = dc.GetTextColor();
  COLORREF crOldBkColor = dc.GetBkColor();

  // If this item is selected, set the background color 
  // and the text color to appropriate values. Also, erase
  // rect by filling it with the background color.
  if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
    (lpDrawItemStruct->itemState & ODS_SELECTED))
  {
    dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
    dc.FillSolidRect(&lpDrawItemStruct->rcItem, 
      ::GetSysColor(COLOR_HIGHLIGHT));
  }
  else
    dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);

  // If this item has the focus, draw a red frame around the
  // item's rect.
  if ((lpDrawItemStruct->itemAction | ODA_FOCUS) &&
    (lpDrawItemStruct->itemState & ODS_FOCUS))
  {
    CBrush br(RGB(255, 0, 0));
    dc.FrameRect(&lpDrawItemStruct->rcItem, &br);
  }

  // Draw the text.
  dc.DrawText(
    lpszText,
    strlen(lpszText),
    &lpDrawItemStruct->rcItem,
    /*DT_CENTER|*/DT_SINGLELINE|DT_VCENTER);

  // Reset the background color and the text color back to their
  // original values.
  dc.SetTextColor(crOldTextColor);
  dc.SetBkColor(crOldBkColor);

  dc.Detach();
  };

void CUnitDlgListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
  {
  ASSERT(lpMeasureItemStruct->CtlType == ODT_LISTBOX);
  LPCTSTR lpszText = (LPCTSTR) lpMeasureItemStruct->itemData;
  ASSERT(lpszText != NULL);
  CSize   sz;
  CDC*    pDC = GetDC();

  sz = pDC->GetTextExtent(lpszText);

  ReleaseDC(pDC);

  lpMeasureItemStruct->itemHeight = 2*sz.cy;

  //CListBox::MeasureItem(lpMeasureItemStruct);
  };

//===========================================================================


//===========================================================================


//===========================================================================

CModelListHelper::CModelListHelper()
  {
  m_pDescStr = NULL;
  m_pBaseTag = NULL;
  m_pModelClass = NULL;
  m_pModelSubClass = NULL;
  }

//---------------------------------------------------------------------------

int CModelListHelper::BuildList(LPCTSTR SectName, char* pGroup, CComboBox* pCB, CTreeCtrl *pTree, CProfINIFile *pPF)
  {
  LPCTSTR InitialSelect = pPF?pPF->RdStr(SectName, "$LastInsert", (char*)DefaultLastUnit):"?";
  if (!InitialSelect)
    InitialSelect ="";

  DWORD LicCat = gs_License.LicCatagories();
  Strng Desc;
  RequestModelInfoRec MInfo;
  HTREEITEM hSelected=NULL, hFirst=NULL;

  CString Sect(SectName);
  Sect+=".Tree";

  int nValidModels = 0;
  int nModels = 0;
  while (gs_pPrj->RequestModelInfoByGroupIndex(pGroup, nModels, MInfo))
    {
    const bool ModelGroupOK = ((LicCat & TOC_MDL_MASK & MInfo.Category)!=0);
    const bool SolveModeOK = ((LicCat & TOC_SOLVE_MASK & MInfo.Category)!=0);
    if (MInfo.IsSelectable && ModelGroupOK && SolveModeOK)
      {
      char* pSlctText = MInfo.ShortDesc() ? MInfo.ShortDesc() : MInfo.Class();
      if (pCB)
        pCB->AddString(pSlctText);
      Desc.Set("%s:%s", pSlctText, (MInfo.Desc() ? MInfo.Desc() : "No Description Available"));
      m_ModelDescLst.Append(Desc());
      m_ModelBaseTagLst.Append(MInfo.TagInitialID());
      m_ModelClassLst.Append(MInfo.Class());
      m_ModelDrwGroupLst.Append(MInfo.DrwGroup());

      if (0)
        {
        // temporary code to Restructure FlwSymbols 
        for (int Pass=0; Pass<4; Pass++)
          {
          Strng Path;
          Path.Set("%s%s.*.%s", Pass<2?BaseGrfSymbolFiles():GrfSymbolFiles(), MInfo.TagInitialID(), Pass%2==0?"DXF":"BMP");
          WIN32_FIND_DATA fd;
          HANDLE H = FindFirstFile(Path(), &fd);
          bool AllDone = (H==INVALID_HANDLE_VALUE);
          while (!AllDone)
            {
            Strng Folder, Src, Dst;
            Folder.Set("%s%s", BaseGrfSymbolFiles(), MInfo.DrwGroup());
            Src.Set("%s%s", BaseGrfSymbolFiles(), fd.cFileName);
            Dst.Set("%s%s\\%s", BaseGrfSymbolFiles(), MInfo.DrwGroup(), &fd.cFileName[MInfo.TagInitialID.Length()+1]);
            dbgpln("MOVE %-60s >> %s", Src(), Dst());

            Strng E;
            if (FnCreatePath(Folder(), E))
              {
              Move_FileEx(Src(), Dst(), MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
              }

            AllDone = !FindNextFile(H, &fd);
            }
          FindClose(H);
          }
        }

      if (pTree)
        {
        HTREEITEM hParent = TVI_ROOT, hItem;
        char Buff[2048];
        char *pS=Buff, *pE;
        strcpy(Buff, pSlctText);
        while ((pE=strchr(pS, ':'))!=NULL)
          {
          *pE=0;
          XStrTrim(pS, " ");
          for (hItem=pTree->GetNextItem(hParent, TVGN_CHILD);
               hItem!=NULL; 
               hItem=pTree->GetNextItem(hItem, TVGN_NEXT))
            {
            CString S=pTree->GetItemText(hItem);
            if (S.CompareNoCase(pS)==0)
              break;
            }
          if (hItem==NULL)
            {
            XStrTrim(pS, " ");
            hItem=pTree->InsertItem(pS, 0, 0, hParent);
            pTree->SortChildren(hParent);
            pTree->SetItemData(hItem, 0xFFFF);
            pTree->SetItemImage(hItem, 0, 1);
            }
          hParent=hItem;
          pS=pE+1;
          }

        XStrTrim(pS, " ");
        hItem=pTree->InsertItem(pS, 0, 0, hParent);
        if (_stricmp(MInfo.Class(), InitialSelect)==0)
          hSelected=hItem;
        if (hFirst==NULL)
          hFirst=hItem;
        pTree->SetItemData(hItem, nValidModels);
        pTree->SortChildren(hParent);
        pTree->SetItemImage(hItem, 2, 3);

        if (pPF)
          {
          // Restore State of Tree
          HTREEITEM hParent = TVI_ROOT, hItem;
          CString Txt[10];
          long iDepth=0;
          hItem=pTree->GetNextItem(TVI_ROOT, TVGN_CHILD);
          while (hItem && iDepth>=0)
            {
            UINT State=pTree->GetItemState(hItem, TVIS_EXPANDED );
            if (1)
              {
              CString S;
              for (int i=0; i<iDepth; i++)
                S+=Txt[i];
              S+=pTree->GetItemText(hItem);        
              UINT State=(UINT)pPF->RdInt(Sect, S, 0);
              if (State&TVIS_EXPANDED)
                {
                pTree->Expand(hItem, TVE_EXPAND);        
                //??????????
                }
              }
            
            HTREEITEM h=pTree->GetNextItem(hItem, TVGN_CHILD);
            if (h)
              {
              Txt[iDepth]=pTree->GetItemText(hItem);        
              Txt[iDepth]+=":";        
              iDepth++;
              hItem=h;
              }
            else
              {
              h=pTree->GetNextItem(hItem, TVGN_NEXT);
              if (h)
                {
                hItem=h;
                }
              else
                {
                // Go One up & One along 
                hItem=pTree->GetNextItem(hItem, TVGN_PARENT);
                if (hItem)
                  hItem=pTree->GetNextItem(hItem, TVGN_NEXT);
                iDepth--;
                }
              }
            }
          }
        }
      nValidModels++;
      }
    nModels++;
    }
  if (pTree)
    {
    if (!hSelected)
      hSelected=hFirst;
    pTree->Select(hSelected, TVGN_CARET);
    //pTree->SelectSetFirstVisible(hSelected);
    }
  return nValidModels;
  }

//---------------------------------------------------------------------------

void CModelListHelper::Find(char* pMdlName)
  {
  const int len = strlen(pMdlName);
  m_pDescStr = m_ModelDescLst.First();
  m_pBaseTag = m_ModelBaseTagLst.First();
  m_pModelClass = m_ModelClassLst.First();
  m_pDrwGroup = m_ModelDrwGroupLst.First();
  while (m_pDescStr)
    {
    if (m_pModelClass->XStrNICmp(pMdlName, len)==0)// && pDescStr->Str()[len]==':')
      break;

    m_pDescStr = m_ModelDescLst.Next();
    m_pBaseTag = m_ModelBaseTagLst.Next();
    m_pModelClass = m_ModelClassLst.Next();
    m_pDrwGroup = m_ModelDrwGroupLst.Next();
    }
  }

//---------------------------------------------------------------------------

void CModelListHelper::Find(DWORD MdlIndex)
  {
  m_pDescStr = m_ModelDescLst.First();
  m_pBaseTag = m_ModelBaseTagLst.First();
  m_pModelClass = m_ModelClassLst.First();
  m_pDrwGroup = m_ModelDrwGroupLst.First();
  while (MdlIndex>0)
    {
    m_pDescStr = m_ModelDescLst.Next();
    m_pBaseTag = m_ModelBaseTagLst.Next();
    m_pModelClass = m_ModelClassLst.Next();
    m_pDrwGroup = m_ModelDrwGroupLst.Next();
    MdlIndex--;
    }
  }

