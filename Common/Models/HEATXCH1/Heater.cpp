//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __HEATER_CPP
#include "heater.h"
#include "datacnvs.h"
//#include "optoff.h"

#if !SKIPIT 

//==========================================================================
//
//
//
//===========================================================================

XID xidH_SzCalcMethod = MdlBsXID(4950);
XID xidH_DesDuty      = MdlBsXID(4951);
XID xidH_Duty         = MdlBsXID(4952);
XID xidH_HTC          = MdlBsXID(4953);
XID xidH_Area         = MdlBsXID(4954);
XID xidH_EffUA        = MdlBsXID(4955);
XID xidH_LMTD         = MdlBsXID(4956);

//--------------------------------------------------------------------------

HeaterSizeCalcs::HeaterSizeCalcs()
  {
  bOn = 0;
  iSizCalcMethod = HSCM_Area_Mdl;
  dHTC = 0.98;
  dArea = 1.0;
  dLMTD = 5.0;
  dDesignDuty = 150.0;
  dModelDuty = dDesignDuty;
  dActualDuty = dDesignDuty;
  }

//--------------------------------------------------------------------------

void HeaterSizeCalcs::BuildDataDefn(DataDefnBlk & DDB, pTaggedObject This)
  {
  static DDBValueLst DDB2[]={
    {HSCM_None, "None"},
    {HSCM_Duty, "Duty"},
    {HSCM_HTC_Des,  "HTC - Design Duty"},
    {HSCM_Area_Des, "Area - Design Duty"},
    {HSCM_LMTD_Des, "LMTD - Design Duty"},
    {HSCM_HTC_Mdl,  "HTC - Model Duty"},
    {HSCM_Area_Mdl, "Area - Model Duty"},
    {HSCM_LMTD_Mdl, "LMTD - Model Duty"},
    {0}};
  DDB.BeginObject(This, "HSC", "SizeData", NULL, DDB_RqdPage);
  DDB.Text    ("Heater Sizing Calculation...");
  DDB.Double  ("ModelDuty",       "",     DC_Pwr,   "kW",    &dModelDuty,     This, isResult);
  DDB.Byte    ("SizeCalcMethod",  "",     DC_,      "",      xidH_SzCalcMethod, This, isParm|SetOnChange, DDB2);
  DDB.Visibility(NSHM_All, DesDutyMethod());
  DDB.Double  ("DesignDuty",      "",     DC_Pwr,   "kW",    xidH_DesDuty,    This, isParm);
  DDB.Visibility(NSHM_All, iSizCalcMethod!=HSCM_None);
  DDB.Text    ("");
  DDB.Double  ("Duty",            "",     DC_Pwr,   "kW",    xidH_Duty,       This, isResult);
  DDB.Double  ("HTC",             "",     DC_HTC,   "kW/m^2.K",xidH_HTC,      This, (HTCMethod() ? isResult : isParm));
  DDB.Double  ("Area",            "",     DC_Area,  "m^2",   xidH_Area,       This, (AreaMethod() ? isResult : isParm));
  DDB.Double  ("U*A",             "",     DC_UA,    "kW/K",  xidH_EffUA,      This, isResult|noFile|noSnap);
  DDB.Double  ("LMTD",            "",     DC_dT,    "C",     xidH_LMTD,       This, (LMTDMethod() ? isResult : isParm)|noFile|noSnap);
  DDB.Visibility();
  DDB.EndObject();
  }

//--------------------------------------------------------------------------

flag HeaterSizeCalcs::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidH_SzCalcMethod:
      if (DCB.rB)
        {
        iSizCalcMethod=*DCB.rB;
        if (DesDutyMethod())
          dActualDuty=dDesignDuty;
        else if (MdlDutyMethod())
          dActualDuty=dModelDuty;
        if (DesDutyMethod() || MdlDutyMethod())
          {
          double PrevLMTD = dLMTD;
          if (AreaMethod())
            dArea = dActualDuty/GTZ(dHTC*PrevLMTD);
          else if (HTCMethod())
            dHTC = dActualDuty/GTZ(PrevLMTD*dArea);
          else if (LMTDMethod())
            {
            }
          }
        dLMTD=dActualDuty/GTZ(dHTC*dArea);
        }
      DCB.B=iSizCalcMethod;
      return 1;
    case xidH_Duty:
      if (DutyMethod())
        dActualDuty=dHTC*dArea*dLMTD;
      DCB.D=dActualDuty;
      return 1;
    case xidH_DesDuty:
      if (DCB.rB)
        {
        dDesignDuty=Max(1.0e-9, *DCB.rD);
        if (DesDutyMethod())
          {
          dActualDuty=dDesignDuty;
          double PrevLMTD = dLMTD;
          if (AreaMethod())
            dArea = dActualDuty/GTZ(dHTC*PrevLMTD);
          else if (HTCMethod())
            dHTC = dActualDuty/GTZ(PrevLMTD*dArea);
          else if (LMTDMethod())
            {
            }
          }
        dLMTD=dActualDuty/GTZ(dHTC*dArea);
        }
      DCB.D=dDesignDuty;
      return 1;
    case xidH_HTC:
      if (DCB.rD)
        {
        double NewHTC = Max(1.0e-9, *DCB.rD);
        if (AreaMethod())
          {
          double PrevLMTD=dActualDuty/GTZ(dHTC*dArea);
          dArea = dActualDuty/GTZ(NewHTC*PrevLMTD);
          }
        dHTC=NewHTC;
        }
      DCB.D=dHTC;
      return 1;
    case xidH_Area:
      if (DCB.rD)
        {
        double NewArea = Max(1.0e-9, *DCB.rD);
        if (HTCMethod())
          {
          double PrevLMTD=dActualDuty/GTZ(dHTC*dArea);
          dHTC = dActualDuty/GTZ(PrevLMTD*NewArea);
          }
        dArea=NewArea;
        }
      DCB.D=dArea;
      return 1;
    case xidH_EffUA:
      DCB.D=dHTC*dArea;
      return 1;
    case xidH_LMTD:
      if (DutyMethod())
        {
        if (DCB.rD)
          dLMTD=*DCB.rD;
        }
      else
        {
        if (DCB.rD)
          {
          double NewLMTD = Max(1.0e-9, *DCB.rD);
          if (AreaMethod())
            {
            dArea = dActualDuty/GTZ(dHTC*NewLMTD);
            }
          else if (HTCMethod())
            {
            dHTC = dActualDuty/GTZ(NewLMTD*dArea);
            }
          }
        dLMTD=dActualDuty/GTZ(dHTC*dArea);
        }
      DCB.D=dLMTD;
      return 1;
    }
  return 0;
  }

//--------------------------------------------------------------------------

void HeaterSizeCalcs::SetMdlDuty(double MdlDuty)
  {
  dModelDuty=MdlDuty;
  if (DesDutyMethod())
    dActualDuty=dDesignDuty;
  else if (MdlDutyMethod())
    dActualDuty=dModelDuty;
  }

//--------------------------------------------------------------------------
//==========================================================================

static double Drw_Heater[] = { DD_Arc, 0, 0, 5,
                      DD_Poly, -5,0, -3,0, -3,-2, 3,2, 3,0, 5,0,
                      DD_End };

enum HDM_HeaterDutyMethod { HDM_Duty, HDM_FixedT, HDM_DutyTag };

XID xidH_DutyMethod   = MdlBsXID(4805);
XID xidH_DutyTag      = MdlBsXID(4806);

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(SimpleHeater, "SimpleHeater", "1", Drw_Heater, "HeatExchange", "HT", TOC_PROBAL|TOC_GRP_TRANSFER|TOC_HEATBAL,
                    "Heat Transfer:Simple Heater",
                    "Simple General Purpose Heater")

SimpleHeater::SimpleHeater(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MN_BstRes(pClass_, TagIn, pAttach, eAttach),
  DutyVar(BXOptions(BXO_Funct|BXO_Blank))
  {
  m_FEP.AssignFlwEqnGroup(HeatXDPGroup, HeatXDPGroup.Default(), this);
  AttachIOAreas(TwoIOAreaList);

  dDuty_Rqd = 50.0;
  dDuty = dDuty_Rqd;
  dTout_Rqd = dNAN;
  iDutyMethod = HDM_Duty;
  //DutyVar.Init(true, true);
  dDutyFactor = 1.0;
  dFarDutyVal = 0.0;
  bDoneExtRefs = 0;
  bAboutToStart = 0;
  bDutyVarValid = 0;
  for (int i=1; i<3; i++)
    m_StateLine[i] = "";
  m_StateLine[0] = "OK";
  
  FinaliseConstruction();
  }
   
//--------------------------------------------------------------------------

void SimpleHeater::BuildDataDefn(DataDefnBlk &DDB)
  {
  static DDBValueLst DDB1[]={
    {HDM_Duty, "FixedDuty"},
    {HDM_FixedT, "ProductTemp"},
    {HDM_DutyTag, "DutyTag"},
    {0}};
  DDB.BeginStruct(this);
  DDB.Visibility();
  DDB.Text    ("");
  DDB.Text    ("Requirements");
  DDB.CheckBox("On",              "",     DC_,      "",      &bOnLine,        this, isParm|SetOnChange);
  DDB.Byte    ("DutyMethod",      "",     DC_,      "",      xidH_DutyMethod, this, isParm|SetOnChange, DDB1);
  DDB.Visibility(NSHM_All, iDutyMethod==HDM_Duty);
  DDB.Double  ("",             "DutyRqd", DC_Pwr,   "kW",    xidDutyRqd,      this, isParm);
  DDB.Visibility(NSHM_All, iDutyMethod==HDM_FixedT);
  DDB.Double  ("OutletTempRqd",   "TRqd", DC_T,     "C",     xidToutRqd,      this, isParm|NAN_OK);
  DDB.Visibility(NSHM_All, iDutyMethod==HDM_DutyTag);
  DDB.String  ("DutyTag",         "",     DC_,      "",      xidH_DutyTag,    this, isParmStopped|isTag); //must be in kW !
  DDB.TagComment("kW");
  DDB.Double  ("DutyFactor",      "",     DC_,      "",      &dDutyFactor,    this, isParm);
  DDB.Visibility();
  //VLE.Add_OnOff(DDB);
  DDB.Text    ("");
  DDB.Text    ("Results");
  DDB.Double  ("",                "Duty", DC_Pwr,   "kW",    xidDuty,         this, isResult);
  DDB.Double  ("Mass_Flow",       "Qm",   DC_Qm,    "kg/s",  xidQm,           this, isResult|noFile|noSnap);
  DDB.Double  ("TemperatureIn",   "Ti",   DC_T,     "C",     xidTempIn,       this, isResult|noFile|noSnap);
  DDB.Double  ("TemperatureOut",  "To",   DC_T,     "C",     xidTempOut,      this, isResult|noFile|noSnap);
  DDB.Double  ("PressIn",         "Pi",   DC_P,     "kPag",  xidPIn,          this, isResult|isParmConstruct);
  DDB.Double  ("Press_Change",    "dP",   DC_DP,    "kPa",   xiddP,           this, isResult|noFile|noSnap|InitHidden);
  DDB.Double  ("PressOut",        "Po",   DC_P,     "kPag",  xidPOut,         this, isResult|isParmConstruct);
  DDB.Double  ("VapourFracIn",    "Vfi",  DC_Frac,  "%",     xidVapFracIn,    this, isResult|noFile|noSnap);
  DDB.Double  ("VapourFracOut",   "Vfo",  DC_Frac,  "%",     xidVapFracOut,   this, isResult|noFile|noSnap);
  //DDB.Double  ("SatT",            "",     DC_T,     "C",     &dSatTOut,       this, isResult|noFile|noSnap);
  //DDB.Double  ("SatP",            "",     DC_P,     "kPag",  &dSatPOut,       this, isResult|noFile|noSnap|InitHidden);
  //DDB.Double  ("H2O_Frac",        "",     DC_Frac,  "%",     &dH20Frac,       this, isResult|noFile|noSnap|InitHidden);*/
  if (0)
    {
    DDB.Text    ("");
    DDB.Double  ("Vol_Flow",        "Qv",   DC_Qv,    "L/s",   xidQv,           this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("NVol_Flow",       "NQv",  DC_NQv,   "NL/s",  xidNQv,          this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("Temperature",     "T",    DC_T,     "C",     xidTemp,         this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("Pressure",        "P",    DC_P,     "kPag",  xidPMean,        this, isResult|noFile|noSnap|InitHidden);
    DDB.Double  ("Density",         "Rho",  DC_Rho,   "kg/m^3",xidRho,          this, isResult|noFile|noSnap|InitHidden);
    }
  DDB.Double  ("Vap_Frac",        "Vf",   DC_Frac,  "%",     xidVapFrac,      this, isResult|noFile|noSnap|InitHidden);
  DDB.Double  ("PrevPOut",        "",     DC_P,     "kPag",  &dPout,          this, isParm|noView);
  //DDB.Double  ("PrevDuty",        "",     DC_Pwr,   "kW",    &dDuty,          this, isParm|noView);

  DDB.Text    ("");
  BuildDataDefnShowIOs(DDB);

  //VLE.BuildDataDefn(DDB, NULL, NULL, DDB_RqdPage, 0);

  HSC.BuildDataDefn(DDB, this);

  DDB.EndStruct();
  }

// --------------------------------------------------------------------------

flag SimpleHeater::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidH_DutyMethod:
      if (DCB.rB)
        {
        iDutyMethod=*DCB.rB;
        if (iDutyMethod==HDM_Duty || iDutyMethod==HDM_DutyTag)
          dTout_Rqd=dNAN;
        else if (!Valid(dTout_Rqd))
          dTout_Rqd=C2K(120.0);//dTout;
        }
      DCB.B=iDutyMethod;
      return 1;
    case xidH_DutyTag:
      if (DCB.rpC)
        {
        if (DutyVar.SetVar(DCB.rpC))
          {
          StructureChanged(this);
          bDoneExtRefs = 0;
          }
        }
      DCB.pC = DutyVar.sVar();
      return 1;
    }

  if (HSC.DataXchg(DCB))
    return 1;
  
  if (MN_BstRes::DataXchg(DCB))
    return 1;
  if (m_FEP.DataXchg(DCB))
    return 1;

  return 0;
  }

//--------------------------------------------------------------------------

flag SimpleHeater::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_BstRes::ValidateData(VDB);
  if (!m_FEP.ValidateData(VDB))
    OK=False;
  return OK;
  }

//---------------------------------------------------------------------------
//
//class CHtXRefItem : public CXRefItem
//  {
//  public:
//    CHtXRefItem() 
//      { 
//      };
//    CHtXRefItem(SimpleHeater * Info, bool MustSet, bool MustGet) :
//      CXRefItem(MustSet, MustGet) 
//      { 
//      };
//    virtual ~CHtXRefItem() {};
//    //LPCTSTR   GetRefID() const   { return ""; };
//    bool IsXRefActive() const    { return true;  };
//
//    SimpleHeater * m_pInfo;
//    DEFINE_SPARES(CHtXRefItem);
//  };
//IMPLEMENT_SPARES(CHtXRefItem, 1000);

//---------------------------------------------------------------------------

void SimpleHeater::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
  if (bOnLine && iDutyMethod==HDM_DutyTag && !GetActiveHold() && ICGetTimeInc() > 0.0)
    {

    GetNearXRefValues();
    //for (int i=0; i<m_NearXRefs.GetSize(); i++)
    //  if (m_NearXRefs[i]->bMustGet)
    //    m_NearXRefs[i]->GetNearXRefValue();

    //solve pgm functions...
    CGExecContext ECtx(this);
    ECtx.dIC_Time.Seconds = ICGetTime();
    ECtx.dIC_dTime.Seconds  = ICGetTimeInc();
    ECtx.OnStart = bAboutToStart;
    ECtx.m_HoldNearXRefGet=true;
    ECtx.m_HoldNearXRefSet=true;
    FnMngr().Execute(ECtx);
    bAboutToStart = 0;
    /*if (ECtx.DoXStop)
      {
      LogError(Tag(), 0, "SysCAD stopped by function");
      ExecObj()->XStop();
      }
    if (ECtx.DoXIdle)
      {
      LogError(Tag(), 0, "SysCAD paused by function");
      ExecObj()->XIdle();
      }*/

    if (bDutyVarValid)
      {
      DutyVar.GetValue(dFarDutyVal);
      }
    else
      dFarDutyVal = 0.0;
    }
  MN_BstRes::EvalCtrlActions(Tasks);
  }

//---------------------------------------------------------------------------

void SimpleHeater::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (iDutyMethod==HDM_DutyTag)
    {
    dDuty_Rqd = dFarDutyVal * dDutyFactor;
    dTout_Rqd = dNAN;
    }
  MN_BstRes::EvalProducts(NEI);
  HSC.SetMdlDuty(dDuty);
  }

//--------------------------------------------------------------------------

bool SimpleHeater::TestXRefListActive() { return SetXRefListActive(iDutyMethod==HDM_DutyTag && !GetActiveHold()); }

//---------------------------------------------------------------------------

int SimpleHeater::UpdateXRefLists(CXRefBuildResults & Results)
  {
  if (1)
    {
    FnMngrClear();
    int FunctNo = 0;
    Strng S(Tag());
    S+=".Duty";
    int RetCode = DutyVar.UpdateXRef(NULL, 0, 1, FunctNo, this, -1, S(), S(), "SimpleHeater::Duty", Results);
    bDutyVarValid = RetCode==BXR_OK;
    FnMngrTryUpdateXRefLists(Results);
    }
  return Results.m_nMissing;
  }

//--------------------------------------------------------------------------

void SimpleHeater::UnlinkAllXRefs()
  {
  FnMngrClear();
  FnMngrTryUnlinkAllXRefs();

  DutyVar.UnlinkXRefs();

  CNodeXRefMngr::UnlinkAllXRefs();
  };

//--------------------------------------------------------------------------

void SimpleHeater::EvalCtrlStrategy(eScdCtrlTasks Tasks)
  {
  MN_BstRes::EvalCtrlStrategy(Tasks);
  if (XRefListActive())
    {

  /*if (bOnLine && iDutyMethod==HDM_DutyTag && !GetActiveHold() && ICGetTimeInc() > 0.0)
    {

    for (int i=0; i<NearGCXRefs.GetSize(); i++)
      if (NearGCXRefs[i].bMustGet)
        GetNearXRefValue(NearGCXRefs[i]);

    //solve pgm functions...
    CGExecContext ECtx(this);
    ECtx.dIC_Time = ICGetTime();
    ECtx.dIC_dTime = ICGetTimeInc();
    ECtx.OnStart = bAboutToStart;
    FnMngr().IM.Execute(ECtx);
    bAboutToStart = 0;
    if (ECtx.DoXStop)
      {
      LogError(Tag(), 0, "SysCAD stopped by function");
      pExecObj->XStop();
      }
    if (ECtx.DoXIdle)
      {
      LogError(Tag(), 0, "SysCAD paused by function");
      pExecObj->XIdle();
      }

    if (bDutyVarValid)
      {
      if (DutyVar.bIsFunct)
        {
        dFarDutyVal = DutyVar.pFunctVar->getD();
        }
      else
        {
        CXRefItem& Meas = (DutyVar.bIsNear ? NearGCXRefs[DutyVar.iXRefIndex] : FarGCXRefs[DutyVar.iXRefIndex]);
        dFarDutyVal = Cnvs[Meas.iCnvIndex]->Human(Meas.dVal, Meas.sCnv());
        }
      }
    else
      dFarDutyVal = 0.0;
    }*/
    }
  }

//--------------------------------------------------------------------------

int SimpleHeater::ChangeTag(pchar pOldTag, pchar pNewTag)
  {
  if (DutyVar.DoChangeTag(pOldTag, pNewTag))
    {
    PreStartCheck();
    bAboutToStart = 0;
    }
  return MdlNode::ChangeTag(pOldTag, pNewTag);
  }

//--------------------------------------------------------------------------

int SimpleHeater::DeleteTag(pchar pDelTag)
  {
  if (DutyVar.ContainsTag(pDelTag))
    {
    LogNote(Tag(), 0, "Delete tag '%s' affects DutyTag '%s'.", pDelTag, Tag());
    UnlinkAllXRefs(); //PreStartCheck();
    bAboutToStart = 0;
    }
  return MdlNode::DeleteTag(pDelTag);
  }

//---------------------------------------------------------------------------

flag SimpleHeater::PreStartCheck()
  {
  for (int i=0; i<3; i++)
    m_StateLine[i] = "";
  m_StateLine[0] = "OK";
  //bDoneExtRefs = 0;
  //m_NearXRefs.SetSize(0, 16);
  //m_FarXRefs.SetSize(0, 16);
  if (bOnLine && iDutyMethod==HDM_DutyTag)
    {
    bAboutToStart = 1;
    }
  return True;
  }

//---------------------------------------------------------------------------

dword SimpleHeater::ModelStatus()
  {
  dword Status=MN_BstRes::ModelStatus();
  return Status;
  }

//--------------------------------------------------------------------------

flag SimpleHeater::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\t?????????"; return 1;
    //case  2: pS="E\tErrors in evaluating functions"; return 1;
    //case  3: pS="E\tEngineering units invalid"; return 1;
    default:
      return FlwNode::CIStrng(No, pS);
    }
  }

//==========================================================================
//
//
//
//==========================================================================

#endif