//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "sc_defs.h"

#define  __SR_CTRL_CPP
#include "sp_db.h"
#include "sp_model.h"
#include "sr_ctrl.h"
#include "errorlog.h"
#include "dbgmngr.h"
//#include "optoff.h"

#define dbgReaction  WITHDEBUG
#if dbgReaction
static CDbgMngr dbgRctSolve("Reactions", "Solve");
#endif

//===========================================================================
//
//
//
//===========================================================================

static DDBValueLst DDBExtentTypes[] = {
  { RBXT_Strict,   "Strict"},
  { RBXT_Target,   "Target"},
  //{ RBXT_Individual,  "Individual"},
  {0}};

flag CEqnSpRef::SetUp(CReactionBlock & RB, CR_Eqn & Eq)
  {
  flag OK=1;
  if (m_Name.Length()>0)
    {
    m_SpcId=SDB.Find(m_Name());
    if (m_SpcId<0)
      {
      CReactionBlock::Error(&RB, 'B', 3, m_Name());
      OK=0;
      }
    m_ProdTerm=Eq.SpecieProductIndex(m_Name);
    m_ReactTerm=Eq.SpecieReactantIndex(m_Name);
    if (m_ProdTerm<0 && m_ReactTerm<0)
      {
      Strng S;
      S.Set("%s not in this Reaction", m_Name());
      CReactionBlock::Error(&RB, 'B', -1, S());
      OK=0;
      }
    return True;
    }
  return False;
  };

//---------------------------------------------------------------------------

//LPCTSTR CEC_Equilibrium::GetRctString()
//  {
//  return "CEC_Equilibrium::GetRctString";
//  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_CLASSBUILDER1(CEC_Equilibrium, CR_EqnControl, "Equilibrium", pCR_Eqn)

void CEC_Equilibrium::Clear()
  {
  CR_EqnControl::Clear();
  m_dK_Eq=1.0;
  m_dK_Rt=1.0;
  };

//---------------------------------------------------------------------------

void CEC_Equilibrium::Parse(CRCTTokenFile &TF)
  {
  Clear();

  TF.TestToken("=");

  m_dK_Eq=TF.DoubleToken();

  if (TF.TokenIs("Rate"))
    {
    TF.TestToken("=");
    double r=GEZ(TF.DoubleToken());
    if (TF.TokenIs("%"))
      r*=0.01;
    m_ddFracRate=r;
    m_bDoRate=true;
    m_bStablize=TF.TokenIs("Stabilised")!=0;
    }
  else if (TF.TokenIs("@"))
    {
    m_ddDefinedResTime=Max(1.0e-3, TF.DoubleToken());
    TF.CheckToken("=");
    m_dK_Rt=TF.DoubleToken();
    if (m_dK_Rt>=0.99999999 * m_dK_Eq)
      m_ddDefinedResTime=-1;
    else
      m_bDoResTime=1;
    }

  };

//---------------------------------------------------------------------------

void CEC_Equilibrium::SetUp()
  {
  //todo: Error messages for using incorrect solver / surge-unit / xfer-unit combinations...
  /*if (m_bDoRate && TaggedObject::GlblProbalMode())
    {
    CReactionBlock::Error(&RB, 'B', -1, "Reaction 'Rate' should not be specified in ProBal solve mode");
    }
  else ???*/

  if (m_bDoRate)
    {
    if (m_ddFracRate<0)
      CReactionBlock::Error(&RB, 'B', -1, "Invalid Rate");
    }
  else if (m_bDoResTime)
    {
    double R=(m_dK_Eq-m_dK_Rt)/m_dK_Eq;
    if ((R>1.0e-8) && R<(1.0-1.0e-9))
      m_dAlpha=-log(R);
    else
      CReactionBlock::Error(&RB, 'B', -1, "Invalid Ratios");
    }
  };

//---------------------------------------------------------------------------

LPCTSTR CEC_Equilibrium::GetRctString()
  {
  m_AsString=m_pFactory ? m_pFactory->ClassId() : "";
  return m_AsString;
  }

//---------------------------------------------------------------------------

void CEC_Equilibrium::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Text  ("Equilibrium");
  DDB.Byte  ("ExtentType",        "",        DC_,     "",      &m_bExtentType,          &Eq, SetOnChange|isParm, DDBExtentTypes);
  DDB.Double("K_Eq",              "",        DC_,     "",      &m_dK_Eq,                &Eq, isResult);
  DDB.Double("K_EOStep",          "K_Act",   DC_,     "",      &m_dKEOStep,             &Eq, isResult);
  DDB.Double("K_Final",           "",        DC_,     "",      &m_dKFinal,              &Eq, isResult|NAN_OK);
  DDB.Double("ExtentError",       "",        DC_Frac, "%",     xid_ExtentError,         &Eq, isResult|noFile|noSnap|NAN_OK);
  if (m_bDoResTime)
    {
    DDB.Double("K_Rt",            "",        DC_,     "",      &m_dK_Rt,                &Eq, isResult);
    m_ddDefinedResTime.BuildDataDefn(DDB, "RT_Ref", "",  DC_Time, "s",  xid_RTRef,   &Eq, "Cfg", "Rqd");
    m_ddActualResTime.BuildDataDefn(DDB, "RT",      "",  DC_Time, "s",  xid_ResTime, &Eq, "Actual", "Rqd");
    DDB.Double("Alpha",           "",        DC_,     "",      &m_dAlpha,                 &Eq, isResult);
    }

  if (m_iXSpcId>=0)
    {
    DDB.String("ExtentSpc", "", DC_, "", &m_sExtentSpc, &Eq, 0|InitHidden|noFileAtAll);
    CCnvIndex dc;
    pchar pCnvTxt;
    SDB.AddSpCnv(DC_Frac, SDB[m_iXSpcId].SymOrTag(), "%", dc, pCnvTxt);
    m_ddExtentRqd.BuildDataDefn(DDB, "Extent", "",  dc, pCnvTxt,  xid_RCTExtent, &Eq, "Cfg", "Rqd");
    }
  };

//---------------------------------------------------------------------------

flag CEC_Equilibrium::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xid_ExtentError:
      DCB.D=ExtentError();
      return 1;
    }

  if (m_ddDefinedResTime.DataXchg(DCB, xid_RTRef, &Eq))
    return 1;
  if (m_ddActualResTime.DataXchg(DCB, xid_ResTime, &Eq))
    return 1;
  if (m_ddExtentRqd.DataXchg(DCB, xid_RCTExtent, &Eq))
    return 1;

  return 0;
  };

//---------------------------------------------------------------------------

flag CEC_Equilibrium::ValidateData(ValidateDataBlk & VDB)
  {
  return True;
  };

//---------------------------------------------------------------------------

void CEC_Equilibrium::CalcKs(double ProdMoles, double &EqK, double &K)
  {
  K=ExtentActual(ProdMoles);
  EqK=ExtentReqd();
  };

double CEC_Equilibrium::ExtentReqd()
  {
  return ExtentDynamicScale(m_dK_Eq);
  };

double CEC_Equilibrium::ExtentActual(double ProdMoles)
  {
  return MolarKRatio(ProdMoles);
  };

//===========================================================================
//
//
//
//===========================================================================

// This Needs Checking/Documenting etc
//IMPLEMENT_CLASSBUILDER1(CEC_EquilTXPoly, CR_EqnControl, "EquilTXPoly", CR_Eqn*)

void CEC_EquilTXPoly::Clear()
  {
  CR_EqnControl::Clear();
  dK_Eq=1.0;
  //dK_Rt=1.0;
  dK_Act=0.0;
  };

//---------------------------------------------------------------------------

void CEC_EquilTXPoly::Parse(CRCTTokenFile &TF)
  {
  Clear();

  dT_Off=TF.DoubleToken();
  TF.CheckToken(",");
  dFnA=TF.DoubleToken();
  TF.CheckToken(",");
  dFnB=TF.DoubleToken();
  dFnC=0.0;
  dFnD=0.0;
  dFnE=0.0;
  if (TF.TokenIs(","))
    {
    dFnC=TF.DoubleToken();
    if (TF.TokenIs(","))
      {
      dFnD=TF.DoubleToken();
      if (TF.TokenIs(","))
        dFnE=TF.DoubleToken();
      }
    }

  //if (TF.TokenIs("@"))
  //  {
  //  m_ddDefinedResTime=Max(1.0e-3, TF.DoubleToken());
  //  TF.CheckToken("=");
  //  dK_Rt=TF.DoubleToken();
  //  if (dK_Rt>=0.99999999 * dK_Eq)
  //    m_ddDefinedResTime=-1;
  //  else
  //    fDoRT=1;
  //  }

  };

//---------------------------------------------------------------------------

void CEC_EquilTXPoly::SetUp()
  {
  if (m_bDoResTime)
    {
    //double R=(dK_Eq-dK_Rt)/dK_Eq;
    //if ((R>1.0e-8) && R<(1.0-1.0e-9))
    //  m_dAlpha=-log(R);
    //else
    //  CReactionBlock::Error(&RB, 'B', -1, "Invalid Ratios");
    }
  };

//---------------------------------------------------------------------------

LPCTSTR CEC_EquilTXPoly::GetRctString()
  {
  m_AsString=m_pFactory ? m_pFactory->ClassId() : "";
  return m_AsString;
  }

//---------------------------------------------------------------------------

void CEC_EquilTXPoly::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Text  ("EquilTXPoly");
  DDEF_Flags VFlags=DDB.GetVisibility();
  DDB.Visibility(SHM_All, Eq.RB.ExtentType()==RBXT_Individual);
  DDB.Byte  ("ExtentType",        "",        DC_,     "",      &m_bExtentType,          &Eq, SetOnChange|isParm, DDBExtentTypes);
  DDB.SetVisibility(VFlags);

  DDB.Double("K_Eq",             "",        DC_,     "",      &dK_Eq,                  &Eq, isResult);
  DDB.Double("K_Act",            "",        DC_,     "",      &dK_Act,                 &Eq, isResult);
  DDB.Double("ExtentError",      "",        DC_Frac, "%",     xid_ExtentError,         &Eq, isResult|noFile|noSnap|NAN_OK);
  if (m_bDoResTime)
    {
    //DDB.Double("K_Rt",           "",        DC_,     "",      &dK_Rt,                  &Eq, isResult);
    //DDB.Double("Defined_RT",     "",        DC_Time, "s",     &m_ddDefinedResTime,        &Eq, isParm);
    //m_ddDefinedResTime.BuildDataDefn(DDB, "RT_Ref", "",  DC_Time, "s",  xid_RTRef,   &Eq, "Cfg", "Rqd");
    //m_ddActualResTime.BuildDataDefn(DDB, "RT",      "",  DC_Time, "s",  xid_ResTime, &Eq, "Actual", "Rqd");
    //DDB.Double("Actual_RT",      "",        DC_Time, "s",     &m_ddActualResTime,         &Eq, isParm);
    DDB.Double("Alpha",          "",        DC_,     "",      &m_dAlpha,                 &Eq, isResult);
    }

//  if (m_iXSpcId>=0)
//    {
//    DDB.String("ExtentSpc", "", DC_, "", &m_sExtentSpc, &Eq, 0|InitHidden|noFileAtAll);
//    CCnvIndex dc;
//    pchar pCnvTxt;
//    SDB.AddSpCnv(DC_Frac, SDB[m_iXSpcId].SymOrTag(), "%", dc, pCnvTxt);
//    m_ddExtentRqd.BuildDataDefn(DDB, "Extent", "",  dc, pCnvTxt,  xid_RCTExtent, &Eq, "Cfg", "Rqd");
//    }
  };

//---------------------------------------------------------------------------

flag CEC_EquilTXPoly::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xid_ExtentError:
      DCB.D=ExtentError();
      return 1;
    }

  if (m_ddDefinedResTime.DataXchg(DCB, xid_RTRef, &Eq))
    return 1;
  if (m_ddActualResTime.DataXchg(DCB, xid_ResTime, &Eq))
    return 1;
  if (m_ddExtentRqd.DataXchg(DCB, xid_RCTExtent, &Eq))
    return 1;

  return 0;
  };

//---------------------------------------------------------------------------

flag CEC_EquilTXPoly::ValidateData(ValidateDataBlk & VDB)
  {
  return True;
  };

//---------------------------------------------------------------------------

void CEC_EquilTXPoly::CalcKs(double ProdMoles, double &EqK, double &K)
  {
  K=MolarKRatio(ProdMoles);
  double Z=dT_Off/RB.EstFinalT()-1.0;
  dK_Eq=exp(Min(87.0, dFnA+Z*(dFnB+Z*(dFnC+Z*(dFnD+Z*dFnE)))));
  EqK=ExtentReqd();
  dK_Act=K;
  };

double CEC_EquilTXPoly::ExtentReqd()
  {
  return ExtentDynamicScale(dK_Eq);
  };

double CEC_EquilTXPoly::ExtentActual(double ProdMoles)
  {
  return dK_Act;
  };

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_CLASSBUILDER1(CEC_Ratio, CR_EqnControl, "Ratio", pCR_Eqn)

void CEC_Ratio::Clear()
  {
  CR_EqnControl::Clear();
  m_ddKEq.Clear(&Eq);
  m_dKRt=0;

  m_Sp.SetSize(0);
  m_RatioEq.SetSize(0);
  m_RatioRt.SetSize(0);
  };

//---------------------------------------------------------------------------

void CEC_Ratio::Parse(CRCTTokenFile &TF)
  {
  Clear();

  CEqnSpRef Ref(TF.NextToken());
  m_Sp.Add(Ref);
  while (TF.TokenIs(":"))
    {
    CEqnSpRef Ref(TF.NextToken());
    m_Sp.Add(Ref);
    }

  TF.CheckToken("=");

  m_RatioEq.SetSize(m_Sp.GetSize());
  double R=TF.DoubleToken();
  int i=0;
  if (i<m_RatioEq.GetLen())
    m_RatioEq[i]=R;
  i++;
  while (TF.TokenIs(":"))
    {
    R=TF.DoubleToken();
    if (i<m_RatioEq.GetLen())
      m_RatioEq[i]=R;
    i++;
    }

  if (i!=m_Sp.GetSize())
    {
    CReactionBlock::Error(&RB, 'B', -1, "Incorrect Number of Equilibrium Ratios");
    }

  if (TF.TokenIs("Rate"))
    {
    TF.TestToken("=");
    double r=GEZ(TF.DoubleToken());
    if (TF.TokenIs("%"))
      r*=0.01;
    m_ddFracRate=r;
    m_bDoRate=true;
    m_bStablize=TF.TokenIs("Stabilised")!=0;
    }
  else if (TF.TokenIs("@"))
    {
    m_bDoResTime=1;
    m_ddDefinedResTime=Max(1.0e-3, TF.DoubleToken());
    TF.CheckToken("=");
    m_RatioRt.SetSize(m_Sp.GetSize());
    double R=TF.DoubleToken();
    int i=0;
    if (i<m_RatioRt.GetLen())
      m_RatioRt[i]=R;
    i++;
    while (TF.TokenIs(":"))
      {
      R=TF.DoubleToken();
      if (i<m_RatioRt.GetLen())
        m_RatioRt[i]=R;
      i++;
      }

    if (i!=m_Sp.GetSize())
      CReactionBlock::Error(&RB, 'B', -1, "Incorrect Number of Residence time Ratios");
    }
  };

//---------------------------------------------------------------------------

void CEC_Ratio::SetUp()
  {
  flag OK=1;
  for (int i=0; i<m_Sp.GetSize(); i++)
    OK=OK && m_Sp[i].SetUp(RB, Eq);

  if (OK)
    {
    // NBNBNB Check that Balancing has been done by this point
    //flag fDoRT=RatioRt.GetLen()>0;
    double KEq=1.0;
    double KRt=1.0;
    double K_EqD=1.0;
    double K_RtD=1.0;
    for (int i=0; i<m_Sp.GetSize(); i++)
      if (m_Sp[i].m_ProdTerm>=0)
        {
        KEq *= m_RatioEq[i];
        if (m_bDoResTime)
          KRt *= m_RatioRt[i];
        }
      else
        {
        K_EqD *= m_RatioEq[i];
        if (m_bDoResTime)
          K_RtD *= m_RatioRt[i];
        }

    //dK_Eq /= GTZ(Pow(RatioEq[i], StMoles));
    KEq /= Max(1.0e-10, K_EqD);
    m_ddKEq = KEq;
    m_dKRt = KRt;
    if (m_bDoResTime)
      {
      m_dKRt /= Max(1.0e-10, K_RtD);
      m_dAlpha=-log((m_ddKEq-m_dKRt)/m_ddKEq);
      int ccc=0;
      }
    }
  };

//---------------------------------------------------------------------------

LPCTSTR CEC_Ratio::GetRctString()
  {
  m_AsString=m_pFactory ? m_pFactory->ClassId() : "";
  return m_AsString;
  }

//---------------------------------------------------------------------------

void CEC_Ratio::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Text  ("Ratio");
  DDEF_Flags VFlags=DDB.GetVisibility();
  DDB.Visibility(SHM_All, Eq.RB.ExtentType()==RBXT_Individual);
  DDB.Byte  ("ExtentType",        "",                       DC_,     "",    &m_bExtentType,   &Eq, SetOnChange|isParm, DDBExtentTypes);
  DDB.SetVisibility(VFlags);

  m_ddKEq.BuildDataDefn(DDB, "K_Eq",  "EquilbriumRatio",    DC_,     "",    xid_RCTKEquilibrium, &Eq, "Cfg", "Rqd");
  DDB.Double("K_EOStep",          "K_Act",   DC_,     "",      &m_dKEOStep,             &Eq, isResult);
  DDB.Double("K_Final",           "",        DC_,     "",      &m_dKFinal,              &Eq, isResult|NAN_OK);
  if (!DynamicRate())
    DDB.Double("ExtentError",       "",                     DC_Frac, "%",   xid_ExtentError,  &Eq, isResult|noFile|noSnap|NAN_OK);
  else
    {
    if (m_bDoRate)
      {
      m_ddFracRate.BuildDataDefn(DDB, "Rate", "",           DC_FracRate, "%/s",  xid_RCTFracRate, &Eq, "Cfg", "Rqd");
      }
    if (m_bDoResTime)
      {
      DDB.Double("K_Rt",           "",                      DC_,     "",    &m_dKRt,          &Eq, isResult);
      m_ddDefinedResTime.BuildDataDefn(DDB, "RT_Ref", "",   DC_Time, "s",   xid_RTRef,        &Eq, "Cfg", "Rqd");
      m_ddActualResTime.BuildDataDefn(DDB,  "RT",     "",   DC_Time, "s",   xid_ResTime,      &Eq, "Actual", "Rqd");
      DDB.Double("Alpha",          "",                      DC_,     "",    &m_dAlpha,        &Eq, isResult);
      }
    }

  if (m_iXSpcId>=0)
    {
    DDB.String("ExtentSpc", "",                             DC_, "", &m_sExtentSpc, &Eq, 0|InitHidden|noFileAtAll);
    CCnvIndex dc;
    pchar pCnvTxt;
    SDB.AddSpCnv(DC_Frac, SDB[m_iXSpcId].SymOrTag(), "%",     dc, pCnvTxt);
    m_ddExtentRqd.BuildDataDefn(DDB, "Extent", "",          dc, pCnvTxt,  xid_RCTExtent, &Eq, "Cfg", "Rqd");
    }
  };

//---------------------------------------------------------------------------

flag CEC_Ratio::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xid_ExtentError:
      DCB.D=ExtentError();
      return 1;
    }

  if (m_ddKEq.DataXchg(DCB, xid_RCTKEquilibrium, &Eq))
    return 1;
  if (m_ddDefinedResTime.DataXchg(DCB, xid_RTRef, &Eq))
    return 1;
  if (m_ddActualResTime.DataXchg(DCB, xid_ResTime, &Eq))
    return 1;
  if (m_ddExtentRqd.DataXchg(DCB, xid_RCTExtent, &Eq))
    return 1;
  return CR_EqnControl::DataXchg(DCB);
  };

//---------------------------------------------------------------------------

flag CEC_Ratio::ValidateData(ValidateDataBlk & VDB)
  {
  return True;
  };

//---------------------------------------------------------------------------

void CEC_Ratio::CalcKs(double ProdMoles, double &EqK, double &K)
  {
  K=ExtentActual(ProdMoles);
  EqK=ExtentReqd();
  }

double CEC_Ratio::ExtentReqd()
  {
  return ExtentDynamicScale(m_ddKEq);
  };

double CEC_Ratio::ExtentActual(double ProdMoles)
  {
  CDVector & Moles = RB.Moles;
  CR_EqnTermArray & Reactant = Eq.Reactant;
  CR_EqnTermArray & Product  = Eq.Product;

  double R=1.0;
  double P=1.0;
  for (int i=0; i<m_Sp.GetSize(); i++)
    {
    double Ml=Moles[m_Sp[i].m_SpcId];
    if (m_Sp[i].m_ProdTerm>=0)
      {
      double StMoles=Eq.Product[m_Sp[i].m_ProdTerm].NoOfMoles;
      P *= Ml + ProdMoles*StMoles;
      }
    else
      {
      double StMoles=Eq.Reactant[m_Sp[i].m_ReactTerm].NoOfMoles;
      R *= Ml - ProdMoles*StMoles;
      }
    }
  return P/(R > 1.0e-10 ? R : 1.0e-10);
  };

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_CLASSBUILDER1(CEC_FracExt, CR_EqnControl, "Fraction", pCR_Eqn)

//---------------------------------------------------------------------------

void CEC_FracExt::Clear()
  {
  CR_EqnControl::Clear();
  m_dRqdExtDT=-1.0;
  m_dRqdExtentLcl=0.0;
  m_dRefMoles=0;
  m_Spc.Clear();
  };

//---------------------------------------------------------------------------

void CEC_FracExt::Parse(CRCTTokenFile &TF)
  {
  Clear();

  LPCSTR Tkn=TF.NextToken();
  m_bExtentType=RBXT_Target;
  if (stricmp(Tkn, "Strict")==0)
    {
    if (DynamicRate())
      CReactionBlock::Error(&RB, 'B', -1, "'Strict' not Allowed in a container");
    else
      m_bExtentType=RBXT_Strict;
    Tkn=TF.NextToken();
    }
  else if (stricmp(Tkn, "Target")==0)
    {
    if (DynamicRate())
      CReactionBlock::Error(&RB, 'B', -1, "'Target' not Allowed in a container");
    else
      m_bExtentType=RBXT_Target;
    Tkn=TF.NextToken();
    }

  m_Spc.m_Name=Tkn;

  m_dRqdExtentLcl=1.0;
  m_dRelRate=1;
  if (DynamicRate())
    {
    if (TF.TokenIs("="))
      {
      m_dRqdExtentLcl=TF.DoubleToken();
      if (TF.TokenIs("%"))
        m_dRqdExtentLcl *= 0.01;
      }
    }
  else
    {
    TF.CheckToken("=");
    m_dRqdExtentLcl=TF.DoubleToken();
    if (TF.TokenIs("%"))
      m_dRqdExtentLcl *= 0.01;
    }
  m_dRqdExtentLcl=Range(0.0, m_dRqdExtentLcl, 1.0);
  if (!DynamicRate() && TF.TokenIs(","))
    {
    m_dRelRate=TF.DoubleToken();
    if (TF.TokenIs("%"))
      m_dRelRate*= 0.01;
    m_dRelRate=Range(0.0, m_dRelRate, 1e6);
    }

  if (DynamicRate())
    {
    if (TF.TokenIs("Rate"))
      {
      TF.TestToken("=");
      double r=GEZ(TF.DoubleToken());
      if (TF.TokenIs("%"))
        r*=0.01;
      m_ddFracRate=r;
      m_bDoRate=true;
      m_bStablize=TF.TokenIs("Stabilised")!=0;
      }
    //else if (TF.TokenIs("@"))
    //  {
    //  m_dRqdExtDT=Max(1.0e-3, TF.DoubleToken());
    //  }
    }
  };

//---------------------------------------------------------------------------

void CEC_FracExt::SetUp()
  {
  flag OK=m_Spc.SetUp(RB, Eq);
  //if (RB.m_RctBase->Nd())
    {
    if (m_bDoRate && TaggedObject::GlblProbalMode())
      {
      CReactionBlock::Error(&RB, 'B', -1, "Extent:Fraction with 'Rate' not allowed in ProBal");
      }
    else if (m_bDoRate && !RB.m_RctBase->IsSurgeType())
      {
      CReactionBlock::Error(&RB, 'B', -1, "'Rate' only allowed in surge unit");
      }
    else if (!m_bDoRate && !TaggedObject::GlblProbalMode() && RB.m_RctBase->IsSurgeType())
      {
      CReactionBlock::Error(&RB, 'B', -1, "Extent:Fraction requires 'Rate' term");
      }
    }

  if (OK)
    {
    if (m_Spc.m_ReactTerm<0)
      {
      Strng S;
      S.Set("Fraction specie %s not a Reactant", m_Spc.m_Name());
      CReactionBlock::Error(&RB, 'B', -1, S());
      }

    CEqnSpRef RSpc(m_Spc.m_Name());
    OK=RSpc.SetUp(RB, Eq);

    // Set Extent Reqd
    Eq.SetExtent(RSpc.m_SpcId, m_dRqdExtentLcl);
    }
  };

//---------------------------------------------------------------------------

LPCTSTR CEC_FracExt::GetRctString()
  {
  m_AsString="";
  if (m_pFactory)
    {
    m_AsString=m_pFactory->ClassId();

    switch (m_bExtentType)
      {
      case RBXT_Target:
        m_AsString += " Strict";
        break;
      case RBXT_Strict:
        m_AsString += " Target";
      }
    }

  CString S;
  S.Format(" %s = %s%%", m_Spc.m_Name(), RctStrippedDouble(m_dRqdExtentLcl*100));
  m_AsString += S;
  
  if (!DynamicRate() && m_dRelRate!=1.0)
    {
    S.Format(", %s%%", RctStrippedDouble(m_dRelRate*100));
    m_AsString += S;
    }

  if (DynamicRate() && m_bDoRate)
    {
    S.Format(" Rate = %s%% %s", RctStrippedDouble(m_ddFracRate*100), m_bStablize?"Stabilised":"");
    m_AsString += S;
    }

  return m_AsString;
  }

//---------------------------------------------------------------------------

void CEC_FracExt::BuildDataDefn(DataDefnBlk & DDB)
  {
  Strng T;
  T.Set("Fractional Extent %s", m_Spc.m_Name());

  DDB.Text  (T());
  DDEF_Flags VFlags=DDB.GetVisibility();
  DDB.Visibility(SHM_All, Eq.RB.ExtentType()==RBXT_Individual);
  DDB.Byte  ("ExtentType",        "",        DC_,     "",      &m_bExtentType,          &Eq, SetOnChange|isParm, DDBExtentTypes);
  DDB.SetVisibility(VFlags);

  if (m_iXSpcId>=0)
    {
    DDB.String("ExtentSpc", "", DC_, "", &m_sExtentSpc, &Eq, 0|InitHidden|noFileAtAll);
    CCnvIndex dc;
    pchar pCnvTxt;
    SDB.AddSpCnv(DC_Frac, SDB[m_iXSpcId].SymOrTag(), "%", dc, pCnvTxt);
    m_ddExtentRqd.BuildDataDefn(DDB, "Extent", "",  dc, pCnvTxt,  xid_RCTExtent, &Eq, "Cfg", "Rqd");
    DDB.Double("ExtentAchieved",   "",     DC_Frac, "%",  &m_dKEOStep,  &Eq, isResult);

    if (!DynamicRate())
      DDB.Double("ExtentError",       "",        DC_Frac, "%",     xid_ExtentError,         &Eq, isResult|noFile|noSnap|InitHidden|NAN_OK);
    else
      {
      if (m_bDoRate)
        {
        m_ddFracRate.BuildDataDefn(DDB, "Rate", "",  DC_FracRate, "%/s",  xid_RCTFracRate, &Eq, "Cfg", "Rqd");
        }
      }
    }
  }

void CEC_FracExt::BuildDataDefnEnd(DataDefnBlk & DDB)
  {
  if (!DDB.ForFileSnpScn() && RB.sm_ChgVars)
    {
    if (RB.sm_ChgVars & SRSC_Mass)
      {
      if (DDB.BeginStruct(&Eq, "MsChg", NULL, DDB_NoPage, -1, 0|MarkerClosed))
        {
        for (int i=0; i<Eq.Reactant.GetSize(); i++)
          {
          DDB.Double(SDB[Eq.Reactant[i].SpcId].SymOrTag(), "", DC_Qm, "kmol/s", &Eq.Reactant[i].m_dMsXfer, &Eq, isResult|noFileAtAll);
          }
        for (int i=0; i<Eq.Product.GetSize(); i++)
          DDB.Double(SDB[Eq.Product[i].SpcId].SymOrTag(), "", DC_Qm, "kmol/s", &Eq.Product[i].m_dMsXfer, &Eq, isResult|noFileAtAll);
        }
      DDB.EndStruct();
      }

    if (RB.sm_ChgVars & SRSC_Molar)
      {
      if (DDB.BeginStruct(&Eq, "MlChg", NULL, DDB_NoPage, -1, 0|MarkerClosed))
        {
        for (int i=0; i<Eq.Reactant.GetSize(); i++)
          {
          DDB.Double(SDB[Eq.Reactant[i].SpcId].SymOrTag(), "", DC_QKgMl, "kmol/s", &Eq.Reactant[i].dMlXfer, &Eq, isResult|noFileAtAll);
          }
        for (int i=0; i<Eq.Product.GetSize(); i++)
          DDB.Double(SDB[Eq.Product[i].SpcId].SymOrTag(), "", DC_QKgMl, "kmol/s", &Eq.Product[i].dMlXfer, &Eq, isResult|noFileAtAll);
        }
      DDB.EndStruct();
      }
    }
  };

//---------------------------------------------------------------------------

flag CEC_FracExt::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xid_ExtentError:
      DCB.D=ExtentError();
      return 1;
    default:
      break;
    }
  if (m_ddFracRate.DataXchg(DCB, xid_RCTFracRate, &Eq))
    return 1;
  if (m_ddExtentRqd.DataXchg(DCB, xid_RCTExtent, &Eq))
    return 1;
  return 0;
  };

//---------------------------------------------------------------------------

flag CEC_FracExt::ValidateData(ValidateDataBlk & VDB)
  {
  return True;
  };

//---------------------------------------------------------------------------

void CEC_FracExt::CalcKs(double ProdMoles, double &EqK, double &K)
  {
  K=ExtentActual(ProdMoles);
  EqK=ExtentReqd();

  if (m_iXSpcId>=0)
    {
    CDVector & Moles = RB.Moles;
    CDVector & Moles_S = RB.Moles_S;
    CDVector & Moles_BS = RB.Moles_BS;  //hss 14/8/98
    switch (RB.Version())
      {
      case 3:
        {
        m_dRefMoles=Moles_BS[m_iXSpcId];
        }
        break;
      case 2:
      default:
        break;
      }
    }

  //dbgpln("CEC_FracExt::CalcKs %2i %10.3f %25.18g   %25.18g  %25.18g  %25.18g %14.8g ",
  //  RB.CSB() ? RB.CSB()->Cmd():-1,
  //  RB.CSB() ? RB.CSB()->TimeInc():-1.0,
  //  ProdMoles, m_dKStart, K, EqK, fabs((EqK-m_dKStart)/NZ(m_ddExtentRqd-m_dKStart)));
  }

double CEC_FracExt::ExtentReqd()
  {
  return ExtentDynamicScale(m_ddExtentRqd);
  };

double CEC_FracExt::ExtentActual(double ProdMoles)
  {
  double EA=0;
  if (m_iXSpcId>=0)
    {
    if (RB.Version()==3)
      {
      double MolesUsed=0;
      if (m_iXReactTerm>=0)
        MolesUsed=Eq.dMlXfer*Eq.Reactant[m_iXReactTerm].NoOfMoles;
      else if (m_iXProdTerm>=0)
        MolesUsed=Eq.dMlXfer*Eq.Product[m_iXProdTerm].NoOfMoles;
      else
        {
        ASSERT_ALWAYS(0, "FracExt not setup");
        }
      if (RB.Moles_BS[m_iXSpcId]>=ZeroLimit)
        EA=MolesUsed/RB.Moles_BS[m_iXSpcId];
      else
        EA=ExtentReqd();
      }
    }
  if (0)
    {
    dbgp("FracExt:Actual:  %10.4f  %10.4f  %10.4f  ",
          ProdMoles, EA, ExtentReqd());
    dbgpln("");
    }
  return EA;
  };

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_CLASSBUILDER1(CEC_FinalConc, CR_EqnControl, "FinalConc", pCR_Eqn)

//---------------------------------------------------------------------------

void CEC_FinalConc::Clear()
  {
  CR_EqnControl::Clear();
  m_ddRqdConc.SetVal(0.0, &Eq);
  m_iFReactTerm=-1;
  m_iFProdTerm=-1;
  m_dRqdTemp=dNAN;
  m_Spc.Clear();
  };

//---------------------------------------------------------------------------

void CEC_FinalConc::Parse(CRCTTokenFile &TF)
  {
  Clear();
  m_Spc.m_Name=TF.NextToken();
  TF.CheckToken("=");
  m_ddRqdConc.SetVal(GEZ(TF.DoubleToken()), &Eq);
  m_iFReactTerm=-1;
  m_iFProdTerm=-1;
  if (TF.TokenIs("At"))
    {
    m_dRqdTemp=GEZ(C2K(TF.DoubleToken()));
    if (TF.TokenIs("K"))
      m_dRqdTemp=K2C(m_dRqdTemp);
    }
  };

//---------------------------------------------------------------------------

void CEC_FinalConc::SetUp()
  {
  flag OK=m_Spc.SetUp(RB, Eq);
  if (OK)
    {
    OK=Eq.SetReactProdInx(m_Spc.m_SpcId, m_iFReactTerm, m_iFProdTerm);
    }
//  if (OK)
//    iFSpcId=Spc.ReactTerm>=0 ? Spc.ReactTerm : Spc.ProdTerm;
//  else
//    iFSpcId=-1;
  };

//---------------------------------------------------------------------------

LPCTSTR CEC_FinalConc::GetRctString()
  {
  m_AsString=m_pFactory ? m_pFactory->ClassId() : "";
  return m_AsString;
  }

//---------------------------------------------------------------------------

void CEC_FinalConc::BuildDataDefn(DataDefnBlk & DDB)
  {
  Strng T;
  T.Set("Final Conc %s", m_Spc.m_Name());

  DDB.Text  (T());
  DDEF_Flags VFlags=DDB.GetVisibility();
  DDB.Visibility(SHM_All, Eq.RB.ExtentType()==RBXT_Individual);
  DDB.Byte  ("ExtentType",        "",        DC_,     "",      &m_bExtentType,          &Eq, SetOnChange|isParm, DDBExtentTypes);
  DDB.SetVisibility(VFlags);

  if (m_Spc.m_ReactTerm>=0 || m_Spc.m_ProdTerm>=0)
    {
    CCnvIndex dc;
    pchar pCnvTxt;
    SDB.AddSpCnv(DC_Conc, SDB[m_Spc.m_SpcId].SymOrTag(), "g/L", dc, pCnvTxt);
    Strng S,C("Cfg"),R("Rqd");
    if (Valid(m_dRqdTemp))
      S.Set("@%.2f", K2C(m_dRqdTemp));
    else
      S="@FinalT";
    C+=S;
    R+=S;
    m_ddRqdConc.BuildDataDefn(DDB, "Conc_Rqd", "", dc, pCnvTxt, xid_RCTExtent, &Eq, C(), R());
    DDB.Double("Conc_MeasTemp",    "",          DC_T,     "C",  xid_RCTFinalConcT,  &Eq, isResult|noFile|noSnap|InitHidden);
    DDB.Double("Conc_EOStep",      "Conc_Act",  DC_,      "",   &m_dKEOStep,             &Eq, isResult);
    DDB.TagComment(S());
    DDB.Double("Conc_Final",       "",          DC_,      "",   &m_dKFinal,              &Eq, isResult|NAN_OK);
    DDB.TagComment(S());
    DDB.Double("ExtentError",      "",          DC_Frac,  "%",  xid_ExtentError, &Eq, isResult|noFile|noSnap|NAN_OK);
    }
  };

//---------------------------------------------------------------------------

flag CEC_FinalConc::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xid_ExtentError:
      DCB.D=ExtentError();
      return 1;
    case xid_RCTFinalConcT:
      DCB.D=m_dRqdTemp;
      return 1;
    }

  if (m_ddRqdConc.DataXchg(DCB, xid_RCTExtent, &Eq))
    return 1;
  return 0;
  };

//---------------------------------------------------------------------------

flag CEC_FinalConc::ValidateData(ValidateDataBlk & VDB)
  {
  return True;
  };

//---------------------------------------------------------------------------

void CEC_FinalConc::CalcKs(double ProdMoles, double &EqK, double &K)
  {
  K=ExtentActual(ProdMoles);
  EqK=ExtentReqd();
  }

double CEC_FinalConc::ExtentReqd()
  {
  return ExtentDynamicScale(m_ddRqdConc());
  };

double CEC_FinalConc::ExtentActual(double ProdMoles)
  {
  SpMArray NewMoles;
  NewMoles=RB.Moles;
  CR_EqnControl::ApplyChanges(ProdMoles, NewMoles);
  RB.AdjustForSrcSnks(NewMoles);

  double M=NewMoles[m_Spc.m_SpcId];
  double MW=SDB[m_Spc.m_SpcId].MoleWt();
  double V=RB.VolWithLockup(som_Liq, NewMoles, Valid(m_dRqdTemp)?m_dRqdTemp:RB.EstFinalT(), RB.m_Press);
  double C;
  if (m_Spc.m_ReactTerm>=0)
    {
    C=(M*MW)/GTZ(V);
    }
  else if (m_Spc.m_ProdTerm>=0)
    {
    C=(M*MW)/GTZ(V);
    }
  else
    C=0.0;

  if (10)
    {
    dbgp("FinalConc:Actual: %14.8f %14.8f %10.4f %10.4f %10.4f %10.4f %10.4f %10.4f|",
          ProdMoles, C, ExtentReqd(), M, MW, V, Valid(m_dRqdTemp)?m_dRqdTemp:RB.EstFinalT(), RB.m_Press);
    for (int i=0; i<SVSpcCount();i++)
      dbgp(" %10.4f", RB.Moles[i]);
    dbgpln("");
    dbgp("                  %14s %14s %10s %10s %10s %10s %10s %10s|",
          "", "", "", "", "", "", "", "");
    for (int i=0; i<SVSpcCount();i++)
      dbgp(" %10.4f", NewMoles[i]);
    dbgpln("");
    }
  return C;
  };

//===========================================================================
//
//
//
//===========================================================================
