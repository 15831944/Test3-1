#include "stdafx.h"

#define __EVAPBLOCK_CPP
#include "EvapBlock.h"
//#include "optoff.h"

//=========================================================================
//
//
//
//=========================================================================

XID xidEvapEnable   = EvapXID(1);
XID xidEvapEqnNm    = EvapXID(2);
XID xidCompCount    = EvapXID(3);
XID xidEBThermAmbT  = EvapXID(4);
XID xidEvapQm       = EvapXID(5);

//============================================================================
//
//
//
//============================================================================

const byte Dest_Mix     = -2;
const byte Dest_Discard = -1;
const byte Dest_IO1     = 0; //????????????????

// ===========================================================================

const pchar CEvapBlock::GroupName="EvapBlock";

IMPLEMENT_EVAP(CEvapBlock, "EvapNone", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "None",  " ");

CEvapBlock::CEvapBlock(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  m_pEvapBase = NULL;
  m_Type      = EBT_None; 
  m_Dest      = Dest_Mix;
  m_ShowMsgs  = true;

  m_Components.SetSize(gs_CDB.DDBCompListVapLiq.Length());
  for (int i=0; i<m_Components.GetSize(); i++)
    {
    CEvapComp  & EC = m_Components[i];
    EC.m_CIndex   = gs_CDB.DDBCompListVapLiq[i].m_lVal;
    EC.m_QmFrac   = 0.0;
    EC.m_QmRqd    = 0.0;
    EC.m_OpTDiff  = 10.0;
    EC.m_OpQm     = 0.0;
    EC.m_PwrLaw = 1.0;
    }

  m_EvapFrac   = 0.0;
  m_QmEvap     = 0.0;
  m_QmFeed     = 0.0;
  m_QmBleed    = 0.0;
  m_QmProd     = 0.0;
  m_HeatFlow   = 0.0;
  m_TempFeed   = C2K(0.0);
  m_TempProd   = C2K(0.0);
  m_PrevUsage  = dNAN;
  }

//--------------------------------------------------------------------------

CEvapBlock::~CEvapBlock()
  {
  };

//--------------------------------------------------------------------------

void CEvapBlock::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, NULL/*"Evap"*/, NULL, DDB_NoPage))
    {
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

static DDBValueLst DDBDestinations[] =
  {
    { Dest_Mix      , "Mixture"},
    //{ Dest_Discard  , "Vent"},
    { 0 }
  };


void CEvapBlock::DoBuildDataDefn(DataDefnBlk& DDB)
  {
  DDB.Text("Requirements");

  //if (m_Type==EBT_TDiff)
  m_AmbientT.BuildDataDefn(DDB, "", "AmbientT", DC_T, "C", xidEBThermAmbT, this, "Atmos", "LclAmb");
  DDB.Byte        ("", "Destination",   DC_,     "",  &m_Dest,      this, isParmStopped, DDBDestinations);
  DDB.CheckBoxBtn ("", "ShowLimitMsgs", DC_,     "",  &m_ShowMsgs,  this, isParm);
  DDB.Text(" ");

  if (DDB.BeginArray(this, "Comp", "EVB_Comps", m_Components.GetSize()))
    {
    for (int i=0; i<m_Components.GetSize(); i++)
      {
      LPTSTR Tg=gs_CDB[m_Components[i].m_CIndex].SymOrTag();
      if (DDB.BeginElement(this, Tg, NULL, i))
        {
        CEvapComp  & EC = m_Components[i];
        switch (m_Type)
          {
          case EBT_Frac: 
            DDB.Double("", "Fraction",    DC_Frac, "%", &EC.m_QmFrac,  this, isParm);
            break;
          case EBT_Flow: 
            DDB.Double("", "Qm",          DC_Qm, "kg/s", &EC.m_QmRqd,  this, isParm);
            break;
          case EBT_TDiff: 
            DDB.Double("", "Oper.Qm",     DC_Qm, "kg/s", &EC.m_OpQm,     this, isParm);
            DDB.TagComment("@ TDiff");
            DDB.Double("", "Oper.TDiff",  DC_dT, "C",    &EC.m_OpTDiff,  this, isParm);
            DDB.Double("", "PwrLaw",      DC_,   "",     &EC.m_PwrLaw,   this, isParm);
            break;
          }
        //DDB.Double("", "Fraction",    DC_Frac, "%", &m_Components[i].m_Fraction,  this, isParm);
        }
      }
    }
  DDB.EndArray();

  if (StateSemanticsOn())
    {
    DDB.Text("");
    DDB.Text("Results");
    DDB.Double ("Evap.Qm",            "", DC_Qm,   "kg/s",    xidEvapQm,     this, isResult);
    DDB.Double ("Evap.Frac",          "", DC_Frac, "%",       &m_EvapFrac,   this, isResult);
    DDB.Text(" ");
    DDB.Text("Total mass:");
    DDB.Double ("Mass.Init",          "", DC_Qm,   "kg/s",    &m_QmFeed,     this, isResult);
    //DDB.Double ("Qm.Vent",            "", DC_Qm,   "kg/s",    &m_QmBleed,    this, isResult);
    DDB.Double ("Mass.Final",         "", DC_Qm,   "kg/s",    &m_QmProd,     this, isResult);
    if (!HeatSkipMethod())
      {
      DDB.Text("");
      DDB.Text("Temperatures:");
      //DDB.Double ("Temp.Feed",          "", DC_T,    "C",       &m_TempFeed,  this, isResult);//|noFileAtAll);  b0yk1e

      //DDB.Double ("HeatFlow",           "", DC_Pwr,  "kW",      &m_HeatFlow,   this, isResult);
      DDB.Double ("Temp.Final",          "", DC_T,    "C",       &m_TempProd,  this, isResult);//|noFileAtAll);
      }
    }
  else
    {
    DDB.Text("");
    DDB.Text("Results");
    DDB.Double ("Evap.Qm",            "", DC_Qm,   "kg/s",    xidEvapQm,     this, isResult);
    DDB.Double ("Evap.Frac",          "", DC_Frac, "%",       &m_EvapFrac,   this, isResult);
    DDB.Text(" ");
    DDB.Text("Total mass flow:");
    DDB.Double ("Qm.Feed",            "", DC_Qm,   "kg/s",    &m_QmFeed,     this, isResult);
    //DDB.Double ("Qm.Vent",           "", DC_Qm,   "kg/s",     &m_QmBleed,    this, isResult);
    DDB.Double ("Qm.Prod",            "", DC_Qm,   "kg/s",    &m_QmProd,     this, isResult);
    if (!HeatSkipMethod())
      {
      DDB.Text("");
      DDB.Text("Temperatures:");
      DDB.Double ("Temp.Feed",          "", DC_T,    "C",       &m_TempFeed,  this, isResult);//|noFileAtAll);
      //DDB.Double ("HeatFlow",           "", DC_Pwr,  "kW",      &m_HeatFlow,   this, isResult);
      DDB.Double ("Temp.Prod",          "", DC_T,    "C",       &m_TempProd,  this, isResult);//|noFileAtAll);
      }
    };
  }

//--------------------------------------------------------------------------

flag CEvapBlock::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidEvapQm:
      DCB.D=m_QmEvap;
      return 1;
    case xidEBThermAmbT:
      m_AmbientT.SetVal(AmbientTemp(), this);
      if (m_AmbientT.DataXchg(DCB,xidEBThermAmbT, this))
        return 1;
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag CEvapBlock::ValidateData(ValidateDataBlk & VDB)
  {
  m_AmbientT.SetVal(AmbientTemp(), this);

  for (int i=0; i<m_Components.GetSize(); i++)
    {
    CEvapComp  & EC = m_Components[i];
    switch (m_Type)
      {
      case EBT_Frac:  
        EC.m_QmFrac   = Range(0.0, EC.m_QmFrac, 1.0);  
        break;
      case EBT_Flow: 
        EC.m_QmRqd    = Max(0.0, EC.m_QmRqd);    
        break;
      case EBT_TDiff:
        EC.m_OpQm     = Max(0.0, EC.m_OpQm);    
        EC.m_OpTDiff  = Max(0.0, EC.m_OpTDiff);    
        EC.m_PwrLaw   = Range(0.1, EC.m_PwrLaw, 10.0);    
        break;
      }
    }
  return TaggedObject::ValidateData(VDB); 
  }

//--------------------------------------------------------------------------

class CEvapFinderQm : public MRootFinderBase
  {
  public:
    CEvapFinderQm(CEvapBlock & EB, SpModel & Qf, SpModel & Discard, SpMArray & OldVap, double Po, double & TtlLiq, double & TtlEvap, double Scale) : \
      MRootFinderBase("EvapFinder", s_Tol), m_EB(EB), 
      m_Qf(Qf), m_Discard(Discard), m_OldVap(OldVap),
      m_Po(Po), m_TtlLiq(TtlLiq), m_TtlEvap(TtlEvap),
      m_Scale(Scale)
      {
      m_HfInit = m_Qf.totHf();
      m_Qf.SetBadTempOK(true);

      for (int i=0; i<m_EB.m_Components.GetSize(); i++)
        {
        CEvapBlock::CEvapComp  & EC = m_EB.m_Components[i];
        CComponent & C   = gs_CDB[EC.m_CIndex];
        int iLiq         = C.LiqPhInx();
        int iVap         = C.VapPhInx();
        m_MassInit[iLiq] = m_Qf.VMass[iLiq];
        m_MassInit[iVap] = m_Qf.VMass[iVap];
        }
      };
    ~CEvapFinderQm()
      {
      m_Qf.SetBadTempOK(false);
      }
    LPCTSTR ObjTag() { return (LPCTSTR)m_EB.m_pEvapBase->Nd().FullObjTag(); };
    double Function(double x)
      {
      m_TtlLiq  = 0.0;
      m_TtlEvap = 0.0;

      for (int i=0; i<m_EB.m_Components.GetSize(); i++)
        {
        CEvapBlock::CEvapComp  & EC = m_EB.m_Components[i];
        CComponent & C  = gs_CDB[EC.m_CIndex];
        int iLiq        = C.LiqPhInx();
        int iVap        = C.VapPhInx();
        double Liq      = m_MassInit[iLiq];
        double Vap      = m_MassInit[iVap];
        double D;
        switch (m_EB.m_Type)
          {
          case EBT_Frac:    D=Liq*EC.m_QmFrac*x;             break;
          case EBT_Flow:    D=Min(EC.m_QmRqd*m_Scale*x, Liq);        break;
          case EBT_TDiff: 
            //NB NB NB This needs to be implemented!
            _asm int 3;
            break;
          default:
            _asm int 3;
            break;
          }

        m_TtlLiq += Liq;
        m_TtlEvap += D;

        m_Qf.SetVValue(iLiq, Liq-D);
        m_Qf.SetVValue(iVap, Vap+D);

        m_Discard.SetVValue(iVap, D);
        m_OldVap.SetVValue(iVap, Vap);
        }
      m_Qf.Set_totHf(m_HfInit);
      double Diff=m_Qf.Temp()-m_EB.m_AmbientT();
      dbgpln("%4i %15.6f %15.6f %15.6f %15.6f", Iterations(), x, Diff, m_Qf.Temp(), m_EB.m_AmbientT());
      return Diff;
      };

    CEvapBlock & m_EB;
    SpModel   & m_Qf;
    SpModel   & m_Discard;
    SpMArray  & m_OldVap;
    
    double m_MassInit[MaxSpecies];
    double m_HfInit;
    double m_Po;
    double & m_TtlLiq;
    double & m_TtlEvap;
    double   m_Scale;

    static CToleranceBlock s_Tol;

  };

CToleranceBlock CEvapFinderQm::s_Tol(TBF_BothSys, "EvapFinder:FinderQm", 0, 1.0e-9, 100, TBF_Fixed|TBF_UseAbs|TBF_UseRel|TBF_UseMax);

//--------------------------------------------------------------------------

class CEvapFinderTmp : public MRootFinderBase
  {
  public:
    CEvapFinderTmp (CEvapBlock & EB, SpModel & Qf, SpModel & Discard, SpMArray & OldVap, double Po, double & TtlLiq, double & TtlEvap, double Scale) : \
      MRootFinderBase("EvapFinder", s_Tol), m_EB(EB), 
      m_Qf(Qf), m_Discard(Discard), m_OldVap(OldVap),
      m_Po(Po), m_TtlLiq(TtlLiq), m_TtlEvap(TtlEvap),
      m_Scale(Scale)
      {
      m_HfInit = m_Qf.totHf();

      for (int i=0; i<m_EB.m_Components.GetSize(); i++)
        {
        CEvapBlock::CEvapComp  & EC = m_EB.m_Components[i];
        CComponent & C   = gs_CDB[EC.m_CIndex];
        int iLiq         = C.LiqPhInx();
        int iVap         = C.VapPhInx();
        m_MassInit[iLiq] = m_Qf.VMass[iLiq];
        m_MassInit[iVap] = m_Qf.VMass[iVap];
        }
      };
    LPCTSTR ObjTag() { return (LPCTSTR)m_EB.m_pEvapBase->Nd().FullObjTag(); };
    double Function(double x)
      {
      m_TtlLiq  = 0.0;
      m_TtlEvap = 0.0;
      for (int i=0; i<m_EB.m_Components.GetSize(); i++)
        {
        CEvapBlock::CEvapComp  & EC = m_EB.m_Components[i];
        CComponent & C  = gs_CDB[EC.m_CIndex];
        int iLiq        = C.LiqPhInx();
        int iVap        = C.VapPhInx();
        double Liq      = m_MassInit[iLiq];
        double Vap      = m_MassInit[iVap];
        double D        = Min(EC.m_OpQm*m_Scale*pow(GEZ(x-m_EB.m_AmbientT())/EC.m_OpTDiff, EC.m_PwrLaw), Liq);     

        m_TtlLiq += Liq;
        m_TtlEvap += D;

        m_Qf.SetVValue(iLiq, Liq-D);
        m_Qf.SetVValue(iVap, Vap+D);

        m_Discard.SetVValue(iVap, D);
        m_OldVap.SetVValue(iVap, Vap);
        }
      m_Qf.SetTempPress(x, m_Po);
      double Diff=m_Qf.totHf()-m_HfInit;
      dbgpln("%4i %15.6f %15.6f %15.6f %15.6f", Iterations(), x, Diff, m_Qf.totHf(), m_HfInit);
      return Diff;
      };

    CEvapBlock & m_EB;
    SpModel   & m_Qf;
    SpModel   & m_Discard;
    SpMArray  & m_OldVap;
    
    double m_MassInit[MaxSpecies];
    double m_HfInit;
    double m_Po;
    double & m_TtlLiq;
    double & m_TtlEvap;
    double   m_Scale;

    static CToleranceBlock s_Tol;

  };

CToleranceBlock CEvapFinderTmp::s_Tol(TBF_BothSys, "EvapFinder:FinderTmp", 0, 1.0e-9, 100, TBF_Fixed|TBF_UseAbs|TBF_UseRel|TBF_UseMax);

//--------------------------------------------------------------------------

void CEvapBlock::EvalProducts(SpModelOwner &MdlOwn, double Po, double FinalTEst)
  {
  m_AmbientT.SetVal(AmbientTemp(), this);

  SpModel & Mdl = *MdlOwn.Model();

  m_QmFeed = Mdl.Mass();
  m_TempFeed = Mdl.Temp();
  const double HzIn = Mdl.totHz();
  double TtlLiq = 0.0;
  double TtlEvap = 0.0;
  double Scale=StateSemanticsOn() ? ICGetTimeInc() : 1.0;

  if (m_TempFeed>m_AmbientT())
    {
    SpConduit & Discard = m_pEvapBase->DiscardCd();
    Discard.QZero();
    SpMArray OldVap;

    Mdl.SetPress(Po);
    
    flag Ok = false;
    if (m_Type==EBT_TDiff)
      {
      CEvapFinderTmp EvTmp(*this, Mdl, Discard, OldVap, Po, TtlLiq, TtlEvap, Scale);
      EvTmp.SetTarget(0.0);
      int iRet=EvTmp.Start(m_AmbientT(), m_TempFeed);
      Ok=(iRet==RF_OK && EvTmp.Solve_Brent()==RF_OK);
      ClrCI(2);
      }
    else 
      {
      CEvapFinderQm EvQm(*this, Mdl, Discard, OldVap, Po, TtlLiq, TtlEvap, Scale);
      if (EvQm.Function(1.0)<1e-10) 
        {
        EvQm.SetTarget(0.0);
        int iRet=EvQm.Start(0.0, 1.0);
        Ok=(iRet==RF_OK && EvQm.Solve_Brent()==RF_OK);
        SetCI(2, m_ShowMsgs);
        }
      else
        {
        Ok=true;
        ClrCI(2);
        }
      }

    const double T=Mdl.Temp();
    const double P=Mdl.Press();
    for (int i=0; i<m_Components.GetSize(); i++)
      {
      CEvapComp  & EC = m_Components[i];
      const int iVap = gs_CDB[EC.m_CIndex].VapPhInx();
      switch (m_Dest)
        {
        case Dest_Mix:
          // Qf is at correct conditions
          Discard.SetVValue(iVap, 0.0);
          break;
        case Dest_Discard:
          Mdl.SetVValue(iVap, OldVap[iVap]); // reset to original vapour mass
          break;
        }
      }
    Mdl.SetTempPress(T, P);
    Discard.SetTempPress(T, P);
    SetCI(1, !Ok);
    ClrCI(3);
    }
  else
    SetCI(3, m_ShowMsgs);

  m_QmEvap     = TtlEvap/Scale;
  m_EvapFrac   = (TtlLiq>SmallPosFlow ? TtlEvap/TtlLiq : 0.0);
  m_QmProd     = Mdl.Mass();
  m_QmBleed    = m_QmFeed-m_QmProd;
  m_TempProd  = Mdl.Temp();
  m_HeatFlow   = Mdl.totHz() - HzIn;
  };

void CEvapBlock::EvalProductsInline(SpModelOwner & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
  m_AmbientT.SetVal(AmbientTemp(), this);
  };

//--------------------------------------------------------------------------

void CEvapBlock::ConvergeStates(CConvergeStateBlk &CSB, SpModelOwner & Cn, double EstFinalT)
  {
  EvalProducts(Cn, Cn.Model()->Press(), EstFinalT);
  };

//--------------------------------------------------------------------------

flag CEvapBlock::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tEvaporation:Solution Failed";                    return 1;
    case  2: pS="W\tEvaporation:Temperature Limited";                return 1;
    case  3: pS="W\tEvaporation:Feed Temperature below Ambient";     return 1;
    default:
      return TaggedObject::CIStrng(No, pS);
    }
  };

//============================================================================
//
//
//
//============================================================================


#if (1 && WithSimpleEvap)
IMPLEMENT_EVAP(CEvBlk_Fraction, "EvapFrac", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Fraction",  " ");
#endif

CEvBlk_Fraction::CEvBlk_Fraction(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
CEvapBlock(pClass_, Tag_, pAttach, eAttach)
  {
  m_Type      = EBT_Frac; 
  }

//--------------------------------------------------------------------------

CEvBlk_Fraction::~CEvBlk_Fraction()
  {
  }

//--------------------------------------------------------------------------

void CEvBlk_Fraction::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "EBFrac", NULL, DDB_NoPage))
    {
    DoBuildDataDefn(DDB);
    }
  DDB.EndStruct();
  }

//============================================================================
//
//
//
//============================================================================

#if (1 && WithSimpleEvap)
IMPLEMENT_EVAP(CEvBlk_Flow, "EvapFlow", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "FixedFlow",  " ");
#endif

CEvBlk_Flow::CEvBlk_Flow(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
CEvapBlock(pClass_, Tag_, pAttach, eAttach)
  {
  m_Type      = EBT_Flow; 
  }

//--------------------------------------------------------------------------

CEvBlk_Flow::~CEvBlk_Flow()
  {
  }

//--------------------------------------------------------------------------

void CEvBlk_Flow::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "EBFlow", NULL, DDB_NoPage))
    {
    DoBuildDataDefn(DDB);
    }
  DDB.EndStruct();
  }

//============================================================================
//
//
//
//============================================================================

#if (1 && WithSimpleEvap)
IMPLEMENT_EVAP(CEvBlk_TDiff, "EvapTDiff", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "TempertureDiff",  " ");
#endif

CEvBlk_TDiff::CEvBlk_TDiff(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
CEvapBlock(pClass_, Tag_, pAttach, eAttach)
  {
  m_Type      = EBT_TDiff; 
  }

//--------------------------------------------------------------------------

CEvBlk_TDiff::~CEvBlk_TDiff()
  {
  }

//--------------------------------------------------------------------------

void CEvBlk_TDiff::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "EBTDiff", NULL, DDB_NoPage))
    {
    DoBuildDataDefn(DDB);
    }
  DDB.EndStruct();
  }

//============================================================================
//
//
//
//============================================================================

flag CEvapBase::Open(TagObjClass * pEvapClass, flag Fixed)
  {
  m_fEnabled=True;

  if (m_pEvapB && (pEvapClass==NULL || m_pEvapB->Class()==pEvapClass))
    return True;

  delete m_pEvapB;
  m_fFixed=Fixed;

  if (pEvapClass)
    m_pEvapB=(CEvapBlock*)pEvapClass->Construct(NULL, "Evap", m_pNd, TOA_Embedded);//pNd);
  else
    m_pEvapB=(CEvapBlock*)CEvapBlockClass.Construct(NULL, "Evap", m_pNd, TOA_Embedded);//pNd);
  m_pNd->StructureChanged(NULL);
  m_pEvapB->m_pEvapBase=this;

  m_fEnabled=m_pEvapB!=NULL;
  return m_fEnabled;
  };

//--------------------------------------------------------------------------

void CEvapBase::Close()
  {
  if (m_fFixed)
    return;
  m_fEnabled=False;
  };
//--------------------------------------------------------------------------

void CEvapBase::Add_OnOff(DataDefnBlk &DDB, dword Flags)
  {
  DDB.Byte("",     "Evaporator",  DC_,    "",     xidEvapEnable,  m_pNd, Flags, GetOnOffValLst());
  }

//--------------------------------------------------------------------------

void CEvapBase::BuildDataDefn(DataDefnBlk &DDB, char* pTag, char* pTagComment, DDBPages PageIs, dword UserInfo)
  {
  if (pTag==NULL)
    pTag="Evap";
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(NSHM_All, m_fEnabled);

  if (Enabled())//pHL)
    {
    if (DDB.BeginStruct(m_pNd, pTag, pTagComment, PageIs))
      {
      DDBValueLstMem DDB0;
      TagObjClass::GetSDescValueLst(CEvapBlock::GroupName, DDB0);
      DDB.String  ("Model",      "",       DC_    , "",      xidEvapEqnNm  , m_pNd,m_fFixed ? 0 : isParmStopped|SetOnChange, DDB0());

      if (m_pEvapB)
        {
        DDB.SuppressNextBlock();
        m_pEvapB->BuildDataDefn(DDB);
        }
      }
    DDB.EndStruct();
    }
  DDB.SetVisibility(Old);
  };

//--------------------------------------------------------------------------

flag CEvapBase::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidEvapEnable:
      if (DCB.rB)
        {
        if (*DCB.rB)
          Open(*DCB.rB);
        else
          Close();
        }
      DCB.B=OpenStatus();// (Enabled());
      return 1;
    case xidEvapEqnNm:
      if (DCB.rpC && !m_fFixed)
        {
        int WasEnabled=m_fEnabled;
        TagObjClass * pC=CEvapBlockClass.FindGrpShortDesc(DCB.rpC);
        if (pC)
          Open(pC);
        else
          Close();
        }
      DCB.pC = m_pEvapB ? m_pEvapB->ShortDesc() : "";
      return 1;
    }
  return 0;
  }

//=========================================================================
//
//
//
//=========================================================================
