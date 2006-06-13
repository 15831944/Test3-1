//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __LICCOUNT_CPP
#include "sc_defs.h"
#include "project.h"
#include "licbase.h"
#include "tagobj.h"
#include "scdver.h"
#include "liccount.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------

void CLicenseCount::CalcLicUnits()
  {
  #if CK_LICENSINGON
  iNoOfIllegalUnits = 0;
  if (gs_pPrj)
    {
    RequestTagInfoRec Rqst(RQ_TagCountCategory);
    Rqst.dwReqData = gs_License.LicCatagories();
    Rqst.sExcludeExecObj = pExecName_DrvMngr; //do NOT include driver tags
    ReplyTagInfoRec Info;
    while (gs_Exec.RequestTagInfo(Rqst, Info))
      iNoOfIllegalUnits += Info.dwData;
    CalcUnits(true);
    iNoOfIllegalUnits = iNoOfUnits - iNoOfIllegalUnits;
    if (iNoOfIllegalUnits>0)
      {
      if (!bLicBlock)
        LogError("SysCAD", LF_Exclamation, "Project contains %d units not allowed by %s license", iNoOfIllegalUnits, (gs_Exec.GlblProbalMode() ? "ProBal" : "Dynamic"));
      bLicBlock = 1;
      }
    }
  #endif
  }

//---------------------------------------------------------------------------

void CLicenseCount::CountUnits()
  {
  #if CK_LICENSINGON
  iNoOfUnits = 0;
  if (gs_pPrj)
    {
    RequestTagInfoRec Rqst(RQ_TagCount);
    Rqst.sExcludeExecObj = pExecName_DrvMngr; //do NOT include driver tags
    ReplyTagInfoRec Info;
    while (gs_Exec.RequestTagInfo(Rqst, Info))
      iNoOfUnits += Info.dwData;
    iNoOfUnits--;
    }
  #endif
  }

//---------------------------------------------------------------------------

void CLicenseCount::CalcUnits(flag ForceCnt/*=false*/)
  {
  #if CK_LICENSINGON
  flag PrevLicBlock = bLicBlock;
  const int iUnitsAllowed = (gs_Exec.GlblProbalMode() ? gs_License.ProbalUnitsAllowed() : gs_License.DynUnitsAllowed());
  if ((ForceCnt || iUnitsAllowed!=CK_InfiniteUnits) && gs_pPrj)
    {
    CountUnits();
    if (iNoOfUnits>iUnitsAllowed)
      {
      if (!bLicBlock)
        LogError("SysCAD", LF_Exclamation, "Maximum number of units allowed exceeds limit of %d allowed by %s license", iUnitsAllowed, (gs_Exec.GlblProbalMode() ? "ProBal" : "Dynamic"));
      bLicBlock = 1;
      }
    else
      bLicBlock = 0;
    if (!ForceCnt && !bLicBlock && PrevLicBlock!=bLicBlock)
      CalcLicUnits();
    }
  else
    bLicBlock = 0;
  #endif
  }

//---------------------------------------------------------------------------

flag CLicenseCount::DynamicCntExceeded()
  {
  #if CK_LICENSINGON
  if (iNoOfUnits==0)
    CountUnits();
  return (iNoOfUnits>gs_License.DynUnitsAllowed());
  #else
  return false;
  #endif
  }

//---------------------------------------------------------------------------

flag CLicenseCount::ProBalCntExceeded()
  {
  #if CK_LICENSINGON
  if (iNoOfUnits==0)
    CountUnits();
  return (iNoOfUnits>gs_License.ProbalUnitsAllowed());
  #else
  return false;
  #endif
  }

//---------------------------------------------------------------------------

CLicenseCount gs_LicenseCnt;
