//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __SR_EQN_CPP
#include "sp_db.h"
#include "sp_model.h"
#include "sr_eqn.h"
#include "sr_ctrl.h"
//#include "optoff.h"

#define dbgReaction  WITHDEBUG
#if dbgReaction
#include "dbgmngr.h"
static CDbgMngr dbgRctSolve("RctEqns", "Solve");
static CDbgMngr dbgRctSolveEqn("RctEqns", "SolveEqn");
static CDbgMngr dbgRctSolveEqnSpc("RctEqns", "SolveEqnSpc");
static CDbgMngr dbgRctSolveEqnCnvg("RctEqns", "SolveEqnCnvg");
static CDbgMngr dbgRctSolveEqnXfer("RctEqns", "SolveEqnXfer");
static CDbgMngr dbgRctSolveEqnHeat("RctEqns", "SolveEqnHeat");
static CDbgMngr dbgRctSolveIter("RctEqns", "SolveIter");
static CDbgMngr dbgRctSolveSimplex("RctEqns", "SolveSimplex");
static CDbgMngr dbgRctDumpSimplex("RctEqns", "DumpSimplex");
#endif

//===========================================================================
//
//                       Equation Control Class Factory
//
//===========================================================================

IMPLEMENT_CLASSFACTORY1(CR_EqnControl, pCR_Eqn)

//---------------------------------------------------------------------------

CR_EqnControl::CR_EqnControl(CR_EqnControlFactory * Factory, pCR_Eqn pEqn) :
  Eq(*pEqn),
  RB(pEqn->RB)
  {
  m_pFactory=Factory, 
  Clear();
  }

//---------------------------------------------------------------------------

void CR_EqnControl::Clear()
  {
  m_iXSpcId=-1;
  m_iXReactTerm=-1;
  m_iXProdTerm=-1;
  m_ddExtentRqd.Clear(&Eq);
  m_bDoResTime=False;
  m_ddFracRate.Clear(&Eq);
  m_bDoRate=false;
  m_bStablize=false;
  m_dKStart=0.0;
  m_dKEOStep=0.0;
  m_dKFinal=dNAN;
  
  m_dAlpha=1.0;
  m_dRelRate=1.0;
  
  m_ddActualResTime.Clear(&Eq);
  m_ddActualResTime=1;
  m_ddDefinedResTime.Clear(&Eq);
  m_ddDefinedResTime=-1.0;
  m_sExtentSpc="";
  m_bExtentType=RBXT_Strict;
  };

//---------------------------------------------------------------------------

flag CR_EqnControl::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xid_ExtentError: 
      DCB.D=ExtentError();
      return 1;
    }
  if (m_ddFracRate.DataXchg(DCB, xid_RCTFracRate, &Eq))
     return 1;
  return 0;
  }

//---------------------------------------------------------------------------

flag CR_EqnControl::ValidateData(ValidateDataBlk & VDB) 
  {
  return True;
  };

//---------------------------------------------------------------------------

double CR_EqnControl::ExtentDynamicScale(double K)
  {
  double KScl=K;
  if (m_bDoRate)
    {
    if (RB.CSB())
      {
      switch (RB.CSB()->Cmd())
        {
        case eStateAdvance:
          {
          //double y=-log(1.0-GEZ(m_ddFracRate()));
          //double Frac=1.0-exp(-y*RB.CSB()->TimeInc());
          //double y=-log(1.0-GEZ(m_ddFracRate()));
          double Frac;
          if (m_bStablize)
            Frac=1.0-exp(-GEZ(m_ddFracRate()*RB.CSB()->TimeInc()));
          else
            Frac=Range(0.0, m_ddFracRate()*RB.CSB()->TimeInc(), 1.0);
          KScl=m_dKStart+(K-m_dKStart)*Range(0.0, Frac, 1.0);
          break;
          }
        case eStateConverge:
          if (RB.CSB()->TimeInc()<=0.0)
            KScl=m_dKStart;
          break;
        }
      }
    }
  else if (m_bDoResTime)
    {
    m_ddDefinedResTime.SetVal(Max(1.0e-3, m_ddDefinedResTime.Val()), &Eq);
    if (Valid(m_ddDefinedResTime.Ovr()))
      m_ddDefinedResTime.SetOvr(Max(1.0e-3, m_ddDefinedResTime.Ovr()), &Eq);
    if (RB.InEvalDerivs())
      m_ddActualResTime.SetVal(Sqrt(MeanResidenceTimeReact()*MeanResidenceTimeProd()), &Eq);
    else
      m_ddActualResTime.SetVal(m_ddDefinedResTime(), &Eq);
    //if (!Valid(m_ddActualResTime))
    //  m_ddActualResTime=Max(1.0e-3, m_ddDefinedResTime);
    KScl=K*(1.0-Exps(-m_dAlpha*m_ddActualResTime()/m_ddDefinedResTime()));
    }
  return KScl;
  }

//---------------------------------------------------------------------------

double CR_EqnControl::MolarKRatio(double ProdMoles)
  {
  CDVector & Moles = RB.Moles;
  CR_EqnTermArray & Reactant = Eq.Reactant;
  CR_EqnTermArray & Product  = Eq.Product;

  double R=1.0;
  for (int i = 0; i < Eq.NReactants(); i++)
    {
    double StMoles=GTZ(Reactant[i].NoOfMoles);
    double Lim=Exps(Min(230.0, log(1e100)/StMoles));
    double D=Moles[Reactant[i].SpcId] - ProdMoles*StMoles;
    R *= Pow(Min(D, Lim), StMoles);
    //R *= Pow(D, StMoles);
    }

  double P=1.0;
  for (i = 0; i < Eq.NProducts(); i++)
    {
    double StMoles=GTZ(Product[i].NoOfMoles);
    double Lim=Exps(Min(230.0, log(1e100)/StMoles));
    double D=Moles[Product[i].SpcId] + ProdMoles*StMoles;
    P *= Pow(Min(D, Lim), StMoles);
    //P *= Pow(D, StMoles);
    }

  return P/(R > 1.0e-10 ? R : 1.0e-10);
  }

//---------------------------------------------------------------------------

void CR_EqnControl::CalcKs(double ProdMoles, double &EqK, double &K)
  {
  EqK=1.0;
  K=MolarKRatio(ProdMoles);
  }

//---------------------------------------------------------------------------

void CR_EqnControl::ApplyChanges(double dProdMoles, SpMArray & M)
  {
  CDVector & Moles = RB.Moles;
  CR_EqnTermArray & Reactant = Eq.Reactant;
  CR_EqnTermArray & Product  = Eq.Product;

  for (int i=0; i<SVSpcCount(); i++)
    M.VValue[i]=Moles[i];

  for (int i = 0; i < Eq.NReactants(); i++)
    M.VValue[Reactant[i].SpcId] += - dProdMoles*Reactant[i].NoOfMoles;

  for (i = 0; i < Eq.NProducts(); i++)
    M.VValue[Product[i].SpcId] += + dProdMoles*Product[i].NoOfMoles;
  }

//---------------------------------------------------------------------------

double CR_EqnControl::LimitingReactant()
  {
  CDVector & Moles = RB.Moles;
  CR_EqnTermArray & Reactant = Eq.Reactant;
  double MinMoles=0.0;

  // Find the reactant with the smallest stoichiometric moles available
  for (int i = 0; i < Eq.NReactants(); i++)
    {
    double ReacMols = Moles[Reactant[i].SpcId] / Reactant[i].NoOfMoles;
    MinMoles = Min(MinMoles, ReacMols);
    }
  return MinMoles;
  }

//---------------------------------------------------------------------------

double CR_EqnControl::MeanResidenceTimeReact()
  {
  CDVector & SpMRT = *RB.m_pSpMRT;
  CR_EqnTermArray & Reactant = Eq.Reactant;
  CR_EqnTermArray & Product = Eq.Product;

  int i;
  double TR=1.0;
  for (i = 0; i < Eq.NReactants(); i++)
    TR *= GTZ(SpMRT[Reactant[i].SpcId]);

  return Pow(TR, 1.0/(Eq.NReactants()));
  }

//---------------------------------------------------------------------------

double CR_EqnControl::MeanResidenceTimeProd()
  {
  CDVector & SpMRT = *RB.m_pSpMRT;
  CR_EqnTermArray & Reactant = Eq.Reactant;
  CR_EqnTermArray & Product = Eq.Product;

  int i;
  double TP=1.0;
  for (i = 0; i < Eq.NProducts(); i++)
    TP *= GTZ(SpMRT[Product[i].SpcId]);

  return Pow(TP, 1.0/(Eq.NProducts()));
  }


//---------------------------------------------------------------------------

void CR_EqnControl::StartStep()
  {
  }

//---------------------------------------------------------------------------

void CR_EqnControl::EvalDiscrete()
  {
  }

//---------------------------------------------------------------------------

flag CR_EqnControl::SetReactProdInx(int SpcId, int &XReactTerm, int &XProdTerm)
  {
  if (SpcId!=m_iXSpcId)
    Eq.StructureChanged(&Eq);
  XReactTerm=-1;
  XProdTerm=-1;

  for (XReactTerm=Eq.NReactants()-1; XReactTerm>=0; XReactTerm--)
    if (Eq.Reactant[XReactTerm].SpcId==SpcId)
      return True;

  for (XProdTerm=Eq.NProducts()-1; XProdTerm>=0; XProdTerm--)
    if (Eq.Product[XProdTerm].SpcId==SpcId)
      return True;

  return False;
  }

//---------------------------------------------------------------------------

flag CR_EqnControl::SetExtent(int SpcId, double Frac)
  {
  if (SetReactProdInx(SpcId, m_iXReactTerm, m_iXProdTerm))
    {
    m_iXSpcId=SpcId;
    m_sExtentSpc=SDB[m_iXSpcId].SymOrTag();
    m_ddExtentRqd.SetVal(Frac, &Eq);
    return true;
    }

  m_sExtentSpc="";
  return False;
  }

//---------------------------------------------------------------------------

double CR_EqnControl::FwdRctExtent()
  {
  CR_EqnTerm &R=Eq.Reactant[m_iXReactTerm];
  switch (RB.Version())
    {
    case 3:
      {
      if (1)
        {
        double MaxExtentMols=RB.Moles[R.SpcId];
        double MaxExtentNoOfMoles=R.NoOfMoles;
        for (int i=0; i<Eq.Reactant.GetSize(); i++)
          {
          CR_EqnTerm &Ri=Eq.Reactant[i];
          double RMoles=RB.Moles_BS[Ri.SpcId]/Ri.NoOfMoles*MaxExtentNoOfMoles;
          MaxExtentMols=Min(MaxExtentMols, RMoles);
          //dbgpln("------ %2i %4i %4i %12.4f %12.4f %12.4f %12.4f", i, R.SpcId, Ri.SpcId, Ri.NoOfMoles, RB.Moles_BS[Ri.SpcId], RB.Moles[Ri.SpcId], MaxExtentMols);
          }

        double Ret=Min(MaxExtentMols, ExtentReqd()*RB.Moles[R.SpcId])/R.NoOfMoles;
        //dbgpln("====== %12.4f", Ret);
        return Ret;
        }
      else
        return (RB.Moles[R.SpcId]-(1.0-ExtentReqd())*RB.Moles_BS[R.SpcId])/R.NoOfMoles; //hss 14/8/98
      }

    case 2:
    default:
      {
      double LimitReactMls = Eq.FwdRct() ? Eq.LimitReactantMoles() : 0.0;
      return Min((m_ddExtentRqd()*RB.Moles[R.SpcId])/R.NoOfMoles, LimitReactMls);
      }
    }
  }

//---------------------------------------------------------------------------

double CR_EqnControl::RevRctExtent()
  {
  CR_EqnTerm &P=Eq.Product[m_iXProdTerm];
  switch (RB.Version())
    {
    case 3:
      {
      if (1)
        {
        double MaxExtentMols=RB.Moles[P.SpcId];
        double MaxExtentNoOfMoles=P.NoOfMoles;
        for (int i=0; i<Eq.Product.GetSize(); i++)
          {
          CR_EqnTerm &Ri=Eq.Product[i];
          double RMoles=RB.Moles_BS[Ri.SpcId]/Ri.NoOfMoles*MaxExtentNoOfMoles;
          MaxExtentMols=Min(MaxExtentMols, RMoles);
          }

        return -Min(MaxExtentMols, ExtentReqd()*RB.Moles_BS[P.SpcId])/P.NoOfMoles;
        //return -(MaxExtentMols-(1.0-ExtentReqd())*RB.Moles_BS[P.SpcId])/P.NoOfMoles;
        }
      else
        return -(RB.Moles[P.SpcId]-(1.0-ExtentReqd())*RB.Moles_BS[P.SpcId])/P.NoOfMoles;
      }
    case 2:
    default:
      {
      double LimitProdMls  = Eq.RevRct() ? Eq.LimitProductMoles() : 0.0;
      return -Min((m_ddExtentRqd()*RB.Moles[P.SpcId])/P.NoOfMoles, LimitProdMls);
      }
    }
  }

//---------------------------------------------------------------------------

double CR_EqnControl::ExtentError()
  {
  if (Eq.RctOn())
    {
    if (Valid(m_dKEOStep))
      {
      // Old 
      //if (fabs(ExtentReqd())>0.0)
      //  return Range(-9999.0, m_dKEOStep/ExtentReqd()-1.0, 9999.0);
      //else     
      //  return 0.0;
      const double Vt = Max(fabs(m_dKEOStep), fabs(ExtentReqd()));
      double Err = (m_dKEOStep-ExtentReqd()) / GTZ(Vt);
      return Range(-9999.0, Err, 9999.0);
      }
    return dNAN; 
    }
  else
    return 0.0;
  }

//---------------------------------------------------------------------------

flag CR_EqnControl::ExtentType()
  {
  return Eq.RB.ExtentType()==RBXT_Individual ? m_bExtentType : Eq.RB.ExtentType();
  }

//===========================================================================
//
//                Equation HeatofReaction Class Factory
//
//===========================================================================

IMPLEMENT_CLASSFACTORY1(CR_EqnHeat, pCR_Eqn)

//---------------------------------------------------------------------------

CR_EqnHeat::CR_EqnHeat(CR_EqnHeatFactory *Factory, pCR_Eqn pEqn) :
  m_Eq(*pEqn),
  m_RB(pEqn->RB)
  {
  m_pFactory=Factory;
  Clear();
  }

//---------------------------------------------------------------------------

void CR_EqnHeat::BuildDataDefn(DataDefnBlk & DDB)
  {
  CCnvIndex dcA;
  pchar pCnvTxtA;
  LPTSTR Tag;
  if (m_bProdTerm)
    Tag=SDB[m_Eq.Product[m_iTermNo].SpcId].SymOrTag();
  else
    Tag=SDB[m_Eq.Reactant[m_iTermNo].SpcId].SymOrTag();
  SDB.AddSpCnv(m_iCnvBasisAct, Tag, "", dcA, pCnvTxtA);
  if (CReactionBlock::sm_AllVars & SRSX_UsrDefInfo)
    DDB.Double("msdH",      "HoR@UsrDef",     dcA,     pCnvTxtA,  xid_msHORUsrDef,  &m_Eq, isResult|noFile|noSnap);
  DDB.Double("msdH@Fd",    "HoR@Fd",          dcA,     pCnvTxtA,  xid_msHORRctFd,   &m_Eq, isResult|noFile|noSnap);
  DDB.Double("msdH@Pr",    "HoR@Pr",          dcA,     pCnvTxtA,  xid_msHORRctPr,   &m_Eq, isResult|noFile|noSnap);
  if (CReactionBlock::sm_AllVars & SRSX_UsrDefInfo)
    DDB.Double("mldH",      "mlHoR@UsrDef",   DC_HMl, "kJ/kmol",  xid_mlHORUsrDef,  &m_Eq, isResult|noFile|noSnap|InitHidden);
  DDB.Double("mldH@Fd",    "mlHoR@Fd",        DC_HMl, "kJ/kmol",  xid_mlHORRctFd,   &m_Eq, isResult|noFile|noSnap|InitHidden);
  DDB.Double("mldH@Pr",    "mlHoR@Pr",        DC_HMl, "kJ/kmol",  xid_mlHORRctPr,   &m_Eq, isResult|noFile|noSnap|InitHidden);
  if (CReactionBlock::sm_AllVars & SRSX_UsrDefInfo)
    DDB.Double("totdH",     "HsGain@UsrDef",  DC_Pwr, "kJ/s",     xid_totHORUsrDef, &m_Eq, isResult|noFile|noSnap|InitHidden);
  DDB.Double("totdH@Fd",   "HsGain@Fd",       DC_Pwr, "kJ/s",     xid_totHORRctFd,  &m_Eq, isResult|noFile|noSnap|InitHidden);
  DDB.Double("totdH@Pr",   "HsGain@Pr",       DC_Pwr, "kJ/s",     xid_totHORRctPr,  &m_Eq, isResult|noFile|noSnap|InitHidden);
  //DDB.Double("tot_dH",     "HsGain",     DC_Pwr, "kJ/s",      xid_totHORRct, &m_Eq, isResult|noFile|noSnap|InitHidden);
  };

//---------------------------------------------------------------------------

flag CR_EqnHeat::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xid_msHORUsrDef:   DCB.D=msDHf(m_RB.Model, m_RB.m_UsrDefT, m_RB.m_UsrDefP);  return 1;
    case xid_mlHORUsrDef:   DCB.D=mlDHf(m_RB.Model, m_RB.m_UsrDefT, m_RB.m_UsrDefP);  return 1;
    case xid_totHORUsrDef:  DCB.D=totDHf(m_RB.Model, m_RB.m_UsrDefT, m_RB.m_UsrDefP)/ICGetTimeInc(); return 1;
    case xid_msHORRctFd:    DCB.D=msDHf(m_RB.Model, m_RB.m_TempKFeed, m_RB.m_Press);  return 1;
    case xid_mlHORRctFd:    DCB.D=mlDHf(m_RB.Model, m_RB.m_TempKFeed, m_RB.m_Press);  return 1;
    case xid_totHORRctFd:   DCB.D=totDHf(m_RB.Model, m_RB.m_TempKFeed, m_RB.m_Press)/ICGetTimeInc(); return 1;
    case xid_msHORRctPr:    DCB.D=msDHf(m_RB.Model, m_RB.m_TempKProd, m_RB.m_Press);  return 1;
    case xid_mlHORRctPr:    DCB.D=mlDHf(m_RB.Model, m_RB.m_TempKProd, m_RB.m_Press);  return 1;
    case xid_totHORRctPr:   DCB.D=totDHf(m_RB.Model, m_RB.m_TempKProd, m_RB.m_Press)/ICGetTimeInc(); return 1;
    }
  return 0;
  };

//---------------------------------------------------------------------------

double CR_EqnHeat::msDHf(SpModel *Mdl, double T, double P)
  {
  CR_EqnTerm &Trm=m_bProdTerm ? m_Eq.Product[m_iTermNo] : m_Eq.Reactant[m_iTermNo];
  return totDHf(Mdl, T, P)/GTZ(fabs(Trm.dMlXfer)*SDB[Trm.SpcId].MoleWt());
  };

//---------------------------------------------------------------------------

double CR_EqnHeat::mlDHf(SpModel *Mdl, double T, double P)
  {
  CR_EqnTerm &Trm=m_bProdTerm ? m_Eq.Product[m_iTermNo] : m_Eq.Reactant[m_iTermNo];
  return totDHf(Mdl, T, P)/GTZ(fabs(Trm.dMlXfer));
  };

//---------------------------------------------------------------------------

double CR_EqnHeat::totDHf(SpModel *Mdl, double T, double P)
  {
  m_Eq.dwState=!m_Eq.CpsOK() ? m_Eq.dwState|CRS_BadCps : m_Eq.dwState&~(CRS_BadCps);
  m_Eq.dwState=!m_Eq.HfsOK() ? m_Eq.dwState|CRS_BadHfs : m_Eq.dwState&~(CRS_BadHfs);

  double Ht=0.0;
  if (dynamic_cast<SpModelEx*>(Mdl)!=NULL)
    {
    SpMArray Mass, Moles;
    Moles=m_RB.Moles;
    Mass.ToMass(Moles);
    double Hf0=Mdl->totHf(som_ALL, T, P, &Mass);
    for (int l=0; l<m_Eq.NReactants(); l++)
      {
      CR_EqnTerm &R=m_Eq.Reactant[l];
      Moles.VValue[R.SpcId]-=/*Xf**/R.dMlXfer;
      }
    for (int r=0; r<m_Eq.NProducts(); r++)
      {
      CR_EqnTerm &P=m_Eq.Product[r];
      Moles.VValue[P.SpcId]-=/*Xf**/P.dMlXfer;
      }
    Mass.ToMass(Moles);
    double Hf1=Mdl->totHf(som_ALL, T, P, &Mass);
    Ht=-(Hf1-Hf0);
    }
  else
    {
    for (int l=0; l<m_Eq.NReactants(); l++)
      {
      CR_EqnTerm &R=m_Eq.Reactant[l];
      double H=SDB[R.SpcId].mlHf(m_RB.Fidelity(), T, P, m_RB.Ovr(), m_RB.MassArray());
      Ht+=R.dMlXfer*H;
      }
    for (int r=0; r<m_Eq.NProducts(); r++)
      {
      CR_EqnTerm &R=m_Eq.Product[r];
      double H=SDB[R.SpcId].mlHf(m_RB.Fidelity(), T, P, m_RB.Ovr(), m_RB.MassArray());
      Ht+=R.dMlXfer*H;
      }
    }
  //dbgpln("Ht            @ %8.2f %8.2f = %15.6f", T, P, Ht);
  return Ht;
  }

//===========================================================================
//
//
//
//===========================================================================

const unsigned long NF_Feed    = NF_No1;
const unsigned long NF_Prod    = NF_No2;
const unsigned long NF_Std     = NF_No3;
const unsigned long NF_Always  = NF_No4;

IMPLEMENT_CLASSBUILDER1(CR_EqnHeatFixedMs, CR_EqnHeat, "msFixed", pCR_Eqn)

//---------------------------------------------------------------------------

CR_EqnHeatFixedMs::CR_EqnHeatFixedMs(CR_EqnHeatFactory * Factory, pCR_Eqn pEqn) :
  CR_EqnHeat(Factory, pEqn)
  {
  m_ddRqddH.SetVal(0.0, &m_Eq);
  m_dDefT=SetNANFlag(NF_Feed);
  m_dDefP=SetNANFlag(NF_Feed);
  }

//---------------------------------------------------------------------------

CR_EqnHeatFixedMs::CR_EqnHeatFixedMs(CR_EqnHeatFactory * Factory, pCR_Eqn pEqn, double msdH) :
  CR_EqnHeat(Factory, pEqn)
  {
  m_ddRqddH.SetVal(msdH, &m_Eq);
  m_dDefT=SetNANFlag(NF_Feed);
  m_dDefP=SetNANFlag(NF_Feed);
  }

//---------------------------------------------------------------------------

void CR_EqnHeatFixedMs::Clear()
  {
  m_ddRqddH.Clear(&m_Eq);
  m_dDefT=SetNANFlag(NF_Feed);
  m_dDefP=SetNANFlag(NF_Feed);
  m_bProdTerm=false;
  m_iTermNo=0;
  };

//---------------------------------------------------------------------------

void CR_EqnHeatFixedMs::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDBNANT[] = {{0, "??"},{1, "FeedTemp"},{2, "ProdTemp"},{3, "StdTemp"}, {4, "Always"}, {0}};
  DDB.Double("HoRRefT",       "",  DC_T, "C", &m_dDefT, &m_Eq, NAN_OK|DDEF_WRITEPROTECT, DDBNANT);
  static DDBValueLst DDBNANP[] = {{0, "??"},{1, "FeedPress"},{2, "ProdPress"},{3, "StdPress"}, {4, "Always"}, {0}};
  DDB.Double("HoRRefP",       "",  DC_P, "kPa", &m_dDefP, &m_Eq, NAN_OK|DDEF_WRITEPROTECT, DDBNANP);

  CCnvIndex dcR;
  pchar pCnvTxtR;
  int s=m_bProdTerm ? m_Eq.Product[m_iTermNo].SpcId : m_Eq.Reactant[m_iTermNo].SpcId;
  SDB.AddSpCnv(m_iCnvBasisRqd, SDB[s].SymOrTag(), "", dcR, pCnvTxtR);
  m_ddRqddH.BuildDataDefn(DDB, "HoR@RefRqd", "Rqd_dH",  dcR, pCnvTxtR, xid_HORRqdDH,&m_Eq, "Cfg", "Rqd");

  CR_EqnHeat::BuildDataDefn(DDB);
  };

//---------------------------------------------------------------------------

flag CR_EqnHeatFixedMs::DataXchg(DataChangeBlk & DCB)
  {
  if (CR_EqnHeat::DataXchg(DCB))
    return 1;
  if (m_ddRqddH.DataXchg(DCB, xid_HORRqdDH, &m_Eq))
    return 1;
  //if (m_ddDefT.DataXchg(DCB, xid_HORDefT, &m_Eq))
  //  return 1;
  return 0;
  };

//---------------------------------------------------------------------------

void CR_EqnHeatFixedMs::Parse(CRCTTokenFile &TF)
  {
  Strng S=TF.GetSeperators();
  Strng SNew(S);
  SNew+="/";
  TF.SetSeperators(SNew());  // add '/' to separs

  TF.CheckToken("=");
  m_ddRqddH.SetVal(TF.DoubleToken(), &m_Eq);
  m_dDefT=SetNANFlag(NF_Feed);
  m_dDefP=SetNANFlag(NF_Feed);

  m_bProdTerm=false;
  m_iTermNo=-1;
  if (TF.TokenIs("/"))
    {
    Strng Spc(TF.NextToken());
    int i=SDB.Find(Spc());
    if (i<0)
      CReactionBlock::Error(&m_RB, 'B', 3, Spc());

    for (int r=0; r<m_Eq.NReactants(); r++)
      if (m_Eq.Reactant[r].SpcId == i)
        {
        m_iTermNo=r;
        break;
        }
    for (int r=0; r<m_Eq.NProducts(); r++)
      if (m_Eq.Product[r].SpcId == i)
        {
        m_bProdTerm=true;
        m_iTermNo=r;
        break;
        }
    if (m_iTermNo<0)
      CReactionBlock::Error(&m_RB, 'B', 8, Spc());
    }
  else
    m_iTermNo=0;

  if (TF.TokenIs("Always"))
    {
    m_dDefT=SetNANFlag(NF_Always);
    m_dDefP=SetNANFlag(NF_Always);
    }
  else if (TF.TokenIs("At"))
    {
    if (TF.TokenIs("Feed"))
      {
      m_dDefT=SetNANFlag(NF_Feed);
      m_dDefP=SetNANFlag(NF_Feed);
      }
    else if (TF.TokenIs("Prod"))
      {
      m_dDefT=SetNANFlag(NF_Prod);
      m_dDefP=SetNANFlag(NF_Prod);
      }
    else if (TF.TokenIs("Std"))
      {
      m_dDefT=SetNANFlag(NF_Std);
      m_dDefP=SetNANFlag(NF_Std);
      }
    else
      {
      m_dDefT=GEZ(C2K(TF.DoubleToken()));
      m_dDefP=SetNANFlag(NF_Std);
      if (TF.TokenIs("K"))
        m_dDefT=K2C(m_dDefT);
      if (TF.TokenIs(","))
        m_dDefP=GEZ(C2K(TF.DoubleToken()));
      }
    }
  else
    {
    m_dDefT=SetNANFlag(NF_Feed);
    m_dDefP=SetNANFlag(NF_Feed);
    }

  TF.SetSeperators(S()); // restore old separs
  };

//---------------------------------------------------------------------------

void CR_EqnHeatFixedMs::SetUp()
  {
  };

//---------------------------------------------------------------------------

LPCTSTR CR_EqnHeatFixedMs::GetRctString()
  {
  m_AsString=m_pFactory ? m_pFactory->ClassId() : "";
  return m_AsString;
  }

//---------------------------------------------------------------------------

double CR_EqnHeatFixedMs::totDHf(SpModel *Mdl, double T, double P)
  {
  flag OK=m_Eq.CpsOK();
  m_Eq.dwState=!OK ? m_Eq.dwState|CRS_BadCps : m_Eq.dwState&~(CRS_BadCps);

  //should this be independent of T ?!?!?!?
  CR_EqnTerm &Trm=m_bProdTerm ? m_Eq.Product[m_iTermNo] : m_Eq.Reactant[m_iTermNo];
  double Ms=fabs(Trm.dMlXfer)*SDB[Trm.SpcId].MoleWt();

  double d=0;
  if (!HasNANFlag(m_dDefT, NF_Always))
    {
    double DefT=m_dDefT;
    if (HasNANFlag(m_dDefT, NF_Feed))
      DefT=m_RB.m_TempKFeed;
    else if (HasNANFlag(m_dDefT, NF_Prod))
      DefT=m_RB.m_TempKProd;
    else if (HasNANFlag(m_dDefT, NF_Std))
      DefT=StdT;
    else if (!Valid(m_dDefT))
      {
      m_dDefT=SetNANFlag(NF_Feed);
      DefT=m_RB.m_TempKFeed;
      }

    double DefP=m_dDefP;
    if (HasNANFlag(m_dDefP, NF_Feed))
      DefP=m_RB.m_Press;
    else if (HasNANFlag(m_dDefP, NF_Prod))
      DefP=m_RB.m_Press;
    else if (HasNANFlag(m_dDefP, NF_Std))
      DefP=StdP;
    else if (!Valid(m_dDefP))
      {
      m_dDefP=SetNANFlag(NF_Feed);
      DefP=m_RB.m_Press;
      }
    d=(m_ddRqddH * Ms) + (CR_EqnHeat::totDHf(Mdl, T, P)-CR_EqnHeat::totDHf(Mdl, DefT, DefP));
    }
  else
    d=(m_ddRqddH * Ms);
  return d;
  };

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_CLASSBUILDER1(CR_EqnHeatFixedMl, CR_EqnHeat, "mlFixed", pCR_Eqn)

//---------------------------------------------------------------------------

CR_EqnHeatFixedMl::CR_EqnHeatFixedMl(CR_EqnHeatFactory * Factory, pCR_Eqn pEqn) :
  CR_EqnHeatFixedMs(Factory, pEqn)
  {
  m_iCnvBasisRqd=DC_HMl;
  m_ddRqddH.SetVal(0.0, &m_Eq);
  m_dDefT=SetNANFlag(NF_Feed);
  m_dDefP=SetNANFlag(NF_Feed);
  }

//---------------------------------------------------------------------------

CR_EqnHeatFixedMl::CR_EqnHeatFixedMl(CR_EqnHeatFactory * Factory, pCR_Eqn pEqn, double mldH) :
  CR_EqnHeatFixedMs(Factory, pEqn)
  {
  m_ddRqddH.SetVal(mldH, &m_Eq);
  m_dDefT=SetNANFlag(NF_Feed);
  m_dDefP=SetNANFlag(NF_Feed);
  }

//---------------------------------------------------------------------------

void CR_EqnHeatFixedMl::Parse(CRCTTokenFile &TF)
  {
  Strng S=TF.GetSeperators();
  Strng SNew(S);
  SNew+="/";
  TF.SetSeperators(SNew());  // add '/' to separs

  TF.CheckToken("=");
  double DH=TF.DoubleToken();
  m_ddRqddH.SetVal(DH, &m_Eq);

  m_bProdTerm=false;
  m_iTermNo=-1;
  if (TF.TokenIs("/"))
    {
    Strng Spc(TF.NextToken());
    int i=SDB.Find(Spc());
    if (i<0)
      CReactionBlock::Error(&m_RB, 'B', 3, Spc());

    for (int r=0; r<m_Eq.NReactants(); r++)
      if (m_Eq.Reactant[r].SpcId == i)
        {
        m_iTermNo=r;
        break;
        }
    for (int r=0; r<m_Eq.NProducts(); r++)
      if (m_Eq.Product[r].SpcId == i)
        {
        m_bProdTerm=true;
        m_iTermNo=r;
        break;
        }
    if (m_iTermNo<0)
      CReactionBlock::Error(&m_RB, 'B', 8, Spc());
    }
  else
    m_iTermNo=0;

  if (TF.TokenIs("Always"))
    {
    m_dDefT=SetNANFlag(NF_Always);
    m_dDefP=SetNANFlag(NF_Always);
    }
  else if (TF.TokenIs("At"))
    {
    if (TF.TokenIs("Feed"))
      {
      m_dDefT=SetNANFlag(NF_Feed);
      m_dDefP=SetNANFlag(NF_Feed);
      }
    else if (TF.TokenIs("Prod"))
      {
      m_dDefT=SetNANFlag(NF_Prod);
      m_dDefP=SetNANFlag(NF_Prod);
      }
    else if (TF.TokenIs("Std"))
      {
      m_dDefT=SetNANFlag(NF_Std);
      m_dDefP=SetNANFlag(NF_Std);
      }
    else
      {
      m_dDefT=GEZ(C2K(TF.DoubleToken()));
      m_dDefP=SetNANFlag(NF_Std);
      if (TF.TokenIs("K"))
        m_dDefT=K2C(m_dDefT);
      if (TF.TokenIs(","))
        m_dDefP=GEZ(C2K(TF.DoubleToken()));
      }
    }
  else
    {
    m_dDefT=SetNANFlag(NF_Feed);
    m_dDefP=SetNANFlag(NF_Feed);
    }

  TF.SetSeperators(S()); // restore old separs
  };

//---------------------------------------------------------------------------

LPCTSTR CR_EqnHeatFixedMl::GetRctString()
  {
  m_AsString=m_pFactory ? m_pFactory->ClassId() : "";
  return m_AsString;
  }

//---------------------------------------------------------------------------

double CR_EqnHeatFixedMl::totDHf(SpModel *Mdl, double T, double P)
  {
  flag OK=m_Eq.CpsOK();
  m_Eq.dwState=!OK ? m_Eq.dwState|CRS_BadCps : m_Eq.dwState&~(CRS_BadCps);

  //should this be independent of T ?!?!?!?
  CR_EqnTerm &Trm=m_bProdTerm ? m_Eq.Product[m_iTermNo] : m_Eq.Reactant[m_iTermNo];
  double Ml=fabs(Trm.dMlXfer);

  double d=0;
  if (!HasNANFlag(m_dDefT, NF_Always))
    {
    double DefT=m_dDefT;
    if (HasNANFlag(m_dDefT, NF_Feed))
      DefT=m_RB.m_TempKFeed;
    else if (HasNANFlag(m_dDefT, NF_Prod))
      DefT=m_RB.m_TempKProd;
    else if (HasNANFlag(m_dDefT, NF_Std))
      DefT=StdT;
    else if (!Valid(m_dDefT))
      {
      m_dDefT=SetNANFlag(NF_Feed);
      DefT=m_RB.m_TempKFeed;
      }

    double DefP=m_dDefP;
    if (HasNANFlag(m_dDefP, NF_Feed))
      DefP=m_RB.m_Press;
    else if (HasNANFlag(m_dDefP, NF_Prod))
      DefP=m_RB.m_Press;
    else if (HasNANFlag(m_dDefP, NF_Std))
      DefP=StdP;
    else if (!Valid(m_dDefP))
      {
      m_dDefP=SetNANFlag(NF_Feed);
      DefP=m_RB.m_Press;
      }

    d=(m_ddRqddH * Ml) + (CR_EqnHeat::totDHf(Mdl, T, P)-CR_EqnHeat::totDHf(Mdl, DefT, DefP));

    //dbgpln(">>> %10.6f  %10.2f %10.2f %10.6f", Ms, K2C(T), P-101.325, d);
    }
  else
    d=(m_ddRqddH * Ml);

  return d;
  };

//===========================================================================
//
//                         Utility to Balance Equations
//
//===========================================================================

class BalanceData
  {
  CR_Eqn &R;
  public:
    Strng          ElemList;// list of elements names
    CLVector       ElemIds; // individual element ID#'s;
    CSVector       SpcName; // individual specie names
    CLVector       SpcIds;  // individual specie ID#'s;
    CDVector       Moles;// moles of each specie
    CDMatrix       Bm;      // species molecular formula vector
    int            NumSpc;
    int            NumElem;

    BalanceData(CR_Eqn * pR) : R(*pR) {};
    flag Check();
    int Balance();
  };

flag BalanceData::Check()
  {
  int BadElementCnt = 0;
  Strng BadElements;
  Strng BadAmounts;
  CReactionBlock::GetSpecieIDs(SpcName, SpcIds);
  CReactionBlock::FindElements(SpcIds, ElemList);
  CReactionBlock::SortElements(ElemList, ElemIds, NumElem, NumSpc);
  Bm.SetSize(NumElem, NumSpc);
  CReactionBlock::ElementComp(Bm, SpcIds, ElemList, NumElem);

  // for each reaction in the reaction block
  // Sum up quantity of elements on each side of each reaction
  bool BalanceOK;
  for (int Pass=0; ; Pass++)
    {
    BalanceOK=true;
    BadElementCnt = 0;
    BadElements = "";
    for (int j = 0; j < NumElem; j++)
      {
      double RMolSum = 0.0;
      double PMolSum = 0.0;
      for (int k = 0; k < R.NReactants(); k++)
        {
        RMolSum += Moles[k] * Bm(j,k);
        }

      for (k = R.NReactants(); k < (R.NReactants() + R.NProducts()); k++)
        {
        PMolSum += Moles[k] * Bm(j,k);
        }

      if (fabs(RMolSum - PMolSum)>RMolSum*1.0e-7)
        {//reaction not balanced!
        BalanceOK=false;
        BadElementCnt++;
        if (BadElementCnt>1)
          BadElements+=",";
        BadElements+=EDB[ElemIds[j]].Name;

        if (Pass==1)
          {//clearly report the error
          char bbb[128];
          Strng LHS;
          for (int k = 0; k < R.NReactants(); k++)
            {
            if (LHS.Length()>0)
              LHS+="+";
            sprintf(bbb, "%g*%g", Moles[k], Bm(j,k));
            LHS+=bbb;
            }
          Strng RHS;
          for (k = R.NReactants(); k < (R.NReactants() + R.NProducts()); k++)
            {
            if (RHS.Length()>0)
              RHS+="+";
            sprintf(bbb, "%g*%g", Moles[k], Bm(j,k));
            RHS+=bbb;
            }
          Strng Es;
          R.GetEqnAsString(Es, EAS_Rct);
          //R.SetCI(9, "E\t'%s' balance error. %g:%g (%s:%s) in %s", EDB[ElemIds[j]].Name, RMolSum, PMolSum, LHS(), RHS(), Es());
          LogError(R.BaseTag(), 0, "Reaction '%s' balance error. %g<=>%g  or  %s<=>%s in '%s'", EDB[ElemIds[j]].Name, RMolSum, PMolSum, LHS(), RHS(), Es());
          }
        }
      }

    // Now try balance the reactions if required...
    if (Pass==0 && !BalanceOK)// || R.fAutoBalance)
      {
      if (Balance() == 0)
        {
        Strng Es;
        R.GetEqnAsString(Es, EAS_Rct);
        R.SetCI(2, "W\t%s balance adjusted in %s", BadElements(), Es());
        }
      else
        {
        //Strng Es;
        //R.GetEqnAsString(Es);
        //LogWarning(FullObjTag, 0, "Unable to balance %s in %s", BadElements(), Es());
        }
      }
    else
      break;
    }

  if (!BalanceOK)
    {
    Strng Es;
    R.GetEqnAsString(Es, EAS_Rct);
    R.SetCI(2, "E\t'%s' %s not balanced in %s", BadElements(), BadElementCnt==1?"is":"are", Es());
    BalanceOK=false;
    }
  else
    {
    R.ClrCI(2);
    R.ClrCI(9);
    }
  return BalanceOK;
  }

//----------------------------------------------------------------------------

int BalanceData::Balance()
  {
  //TODO: This needs improving!!!
  //User needs to be given the balanced reaction string so that they can put this in the reaction file
  CDVector B, Coeffs;
  CDMatrix A, Ain;
  int      IndepSpc, DepSpc;
  int      i, j, k, m, n;
  byte     error=0;
  double   sum;
  int      Ret=-1; // initialise as unsuccessful

  IndepSpc = NumSpc -1;
  DepSpc = 1;

  // leave one specie on the LHS as the dependent specie
  // matrix A (below) must be square else can't get the inverse

  if (IndepSpc == NumElem)
    {
    // Get local data structure memory and initialise
    A.SetSize(NumElem, IndepSpc, 0.0);
    Ain.SetSize(NumElem, IndepSpc, 0.0);
    B.SetSize(NumElem, 0.0);
    Coeffs.SetSize(IndepSpc, 0.0);

    // Copy values into local structures

    for (j = 0; j < NumElem; j++)
      {
      k = 0;
      // Get single dependent specie
      // 1st reactant specie will represent the dependent specie  (LHS)
      B[j] = Bm(j,k);
      // The rest of the species will represent the independent species  (RHS)
      for (k = 1; k <  NumSpc; k++)
        A(j, k-1) = Bm(j,k);
      }
      // for each element

    Ain=A;
    if (Ain.LUInvert()) //  Inverse(IndepSpc,&A,&Ain,&error);
      {
      Ret=0;
      for (i = 0; i < IndepSpc; i++)
        {
        sum = 0.0;
        for (j = 0; j < NumElem; j++)
          sum = sum + B[j] * Ain(i,j);
        Coeffs[i] = sum;

        // Coeffs should contain the balanced stoichiometric coefficients for
        // each specie in the Reaction. If the coefficient is negative the
        // the specie belongs on the LHS of the reaction Reaction.
        }

      // Write the balanced reaction back into the reaction data structure

      if (R.Reactant[0].SpcName.XStrCmp(SpcName[0]()) != 0)
        R.Reactant[0].NoOfMoles=1.0;

      // For the rest of the species
      n = 1;
      m = 0;
      for (i = 0; i < IndepSpc; i++)
        {
        if (Coeffs[i] < 0.0) // if negative - Reactant
          {
          if ((n < R.NReactants()))
            {
            if (R.Reactant[n].SpcId!=SpcIds[i+1])
              {
              R.Reactant[n].SpcId=SpcIds[i+1];
              R.Reactant[n].SpcName=SpcName[i+1].Str();
              }
            R.Reactant[n].NoOfMoles=fabs(Coeffs[i]);
            }
          else
            {
            dbgpln("must still figure out what to do if the number of reactant terms is changed");
            }
          n++;
          }
        else    // if not negative - Product
          {
          if ((m < R.NProducts()))
            {
            if (R.Product[m].SpcId!=SpcIds[i+1])
              {
              R.Product[m].SpcId=SpcIds[i+1];
              R.Product[m].SpcName=SpcName[i+1].Str();
              }
            R.Product[m].NoOfMoles=Coeffs[i];
            }
          else
            {
            dbgpln("must still figure out what to do if the number of product terms is changed");
            };
          m++;
          }
        }
      }
    // if no error
    }
  else if ((IndepSpc == 1))
    {
    R.Reactant[0].NoOfMoles = 1.0;
    R.Product[0].NoOfMoles = 1.0;
    Ret=0;
    }
  else
    {
    // Cannot AutoBalance
    }
  return Ret;
  }

//===========================================================================
//
//                          Equation Based Reactions
//
//===========================================================================

IMPLEMENT_REACTION(CR_Eqn, "SR_React", "", TOC_SYSTEM, "Reactions", "Stoichiometric Equations");

CToleranceBlock CR_Eqn::sm_ExtentTol(TBF_Both, "RB:ExtentTest", 0.00005, 1.0e-9, 100);


CR_Eqn::CR_Eqn(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CReaction(pClass_, Tag_, pAttach, eAttach),
  Reactant(*(new CR_EqnTermArray)),
  Product(*(new CR_EqnTermArray))
  {
  pControl=NULL;
  pHeat=NULL;
  pBlk=NULL;
  Clear();
  }

//---------------------------------------------------------------------------

CR_Eqn::~CR_Eqn()
  {
  Clear();
  delete &Product;
  delete &Reactant;
  }

//---------------------------------------------------------------------------

void CR_Eqn::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this, NULL, NULL, DDB_OptPage))
    {
    Strng Es;
    GetEqnAsString(Es, EAS_Rct);
    if (Es.Length()>40)
      {
      Es.SetLength(37);
      Es+="...";
      }
    DDB.Text(Es());
    DDB.String("Reaction", "", DC_, "", xid_Reaction, this, 0|InitHidden|noFileAtAll);
    DDB.String("Sequence", "", DC_, "", xid_RctSeq,   this, 0|InitHidden|noFileAtAll);
    DDB.String("State",    "", DC_, "", xid_RctState, this, 0|noFileAtAll);
    DDB.CheckBoxBtn("On",  "", DC_, "", &fRctOn,      this, isParm, DDBOnOff);

    if (HasControl())
      Control().BuildDataDefn(DDB);

    bool UseProd=false;
    int iTrmNo=0;
    if (pHeat)
      {
      UseProd=Heat().m_bProdTerm;
      iTrmNo=Heat().m_iTermNo;
      }
    int iRctSpc = UseProd ? Product[iTrmNo].SpcId : Reactant[iTrmNo].SpcId;

    DDB.PushUserInfo(iTrmNo);
    CCnvIndex Ldc, Mdc;
    pchar pLCnvTxt, pMCnvTxt;
    SDB.AddSpCnv(DC_QKgMl, SDB[iRctSpc].SymOrTag(), "", Ldc, pLCnvTxt);
    SDB.AddSpCnv(DC_Qm,    SDB[iRctSpc].SymOrTag(), "", Mdc, pMCnvTxt);
    DDB.Double("ReactingMoles", "mlXfer", Ldc, pLCnvTxt, xid_dMlXfer, this, isResult|noFile|noSnap|InitHidden);
    DDB.Double("ReactingMass",  "msXfer", Mdc, pMCnvTxt, xid_dMsXfer, this, isResult|noFile|noSnap);
    if (DDB.ForFileSnpScn())
      DDB.Double("",  "mlXferInt", Ldc, pLCnvTxt, xid_dMlXferInt, this, isResult);
    DDB.PopUserInfo();
    if (pHeat)
      Heat().BuildDataDefn(DDB);
    if (HasControl())
      Control().BuildDataDefnEnd(DDB);
    }
  DDB.EndStruct();
  };

//---------------------------------------------------------------------------

flag CR_Eqn::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xid_Reaction:
      {
      //if(DCB.rpC != NULL)
      //  {
      //  }
      GetEqnAsString(m_DispEqnStr, EAS_Rct);
      DCB.pC = m_DispEqnStr();
      return 1;
      }
    case xid_RctSeq:
      {
      DCB.pC = m_SeqStr();
      return 1;
      }
    case xid_RctState:
      {
      sState="";
      GetState();
      if ((dwState & (CRS_BadCps|CRS_BadHfs))==(CRS_BadCps|CRS_BadHfs))
        sState+="Bad Hf's & Cp's";
      else if (dwState & CRS_BadCps)
        sState+="Bad Cp's";
      else if (dwState & CRS_BadHfs)
        sState+="Bad Hf's";
      if (sState.Length() > 0)
        DCB.dwRetFlags=DDEF_INERROR;
      else
        sState="OK";
      DCB.pC=sState();
      return 1;
      }
    case xid_dMlXferInt:
      {
      CR_EqnTerm &R=Reactant[DCB.dwUserInfo];
      if (DCB.rD)
        dMlXfer=*DCB.rD;
      DCB.D=dMlXfer;
      return 1;
      }
    case xid_dMlXfer:
      {
      CR_EqnTerm &R=Reactant[DCB.dwUserInfo];
      DCB.D=dMlXfer*R.NoOfMoles/ICGetTimeInc();
      return 1;
      }
    case xid_dMsXfer:
      {
      CR_EqnTerm &R=Reactant[DCB.dwUserInfo];
      DCB.D=dMlXfer*R.NoOfMoles*SDB[R.SpcId].MoleWt()/ICGetTimeInc();
      return 1;
      }
    }
  if (HasControl())
    if (Control().DataXchg(DCB))
      return 1;
  if (pHeat)
    if (Heat().DataXchg(DCB))
      return 1;
  return 0;
  };

//---------------------------------------------------------------------------

flag CR_Eqn::ValidateData(ValidateDataBlk & VDB)
  {
  return True;
  };

//---------------------------------------------------------------------------

void CR_Eqn::Clear()
  {
  ClrCI(1);
  ClrCI(2);
  iDirn=0;

  Reactant.SetSize(0);
  Product.SetSize(0);

  delete pBlk;
  delete pControl;
  delete pHeat;
  pControl=NULL;
  pHeat=NULL;
  pBlk=NULL;

  dRctScl=1.0;

  fFwdRct=0;
  fRevRct=0;
  fRctOn=1;
  fAutoBalance=0;
  fDone=0;
  iLimitSpc=-1;
  dMlXfer=0.0;
  dRctScl=1.0;
  m_dRelRate=1.0;

  sState="";
  dwState=0;
  StructureChanged(this);
  }

//---------------------------------------------------------------------------

eScdReactionBasis CR_Eqn::Basis()
  {
  if (HasControl())
    return Control().Basis();
  return RctEquilibrium;
  };

//---------------------------------------------------------------------------

void CR_Eqn::GetEqnAsString(Strng & Es, dword Options)
  {
  char Buff[4096];
  char *p=Buff;
  if (Options & EAS_Rct)
    {
    for (int k = 0; k < NReactants(); k++)
      {
      if (fabs(Reactant[k].NoOfMoles-1.0) > 1.0e-3)
        p+=sprintf(p,"%g ",Reactant[k].NoOfMoles);
      p+=sprintf(p,"%s ",Reactant[k].SpcName());
      if (k < NReactants()-1)
        p+=sprintf(p, "+ ");
      }
    if (FwdRct() && RevRct())
      p+=sprintf(p,"<=> ");
    else if (FwdRct())
      p+=sprintf(p,"=> ");
    else if (RevRct())
      p+=sprintf(p,"<= ");
    for (k = 0; k < NProducts(); k++)
      {
      if (fabs(Product[k].NoOfMoles-1.0) > 1.0e-3)
        p+=sprintf(p,"%g ",Product[k].NoOfMoles);
      p+=sprintf(p,"%s ",Product[k].SpcName());
      if (k < NProducts()-1)
        p+=sprintf(p, "+ ");
      }
    }
  if (Options & EAS_Extent)
    {
    if (HasControl() && strlen(Control().GetRctString())>0)
      p+=sprintf(p, "Extent: %s ", Control().GetRctString());
    }
  if (Options & EAS_HOR)
    {
    if (pHeat && strlen(Heat().GetRctString())>0)
      p+=sprintf(p, "HeatOfReaction: %s ", Heat().GetRctString());
    }

  Es=Buff;
  };

//---------------------------------------------------------------------------

void CR_Eqn::dbgDump(pchar pTitle)
  {
  dbgp("%s",pTitle);
  for (int k = 0; k < NReactants(); k++)
    dbgp("%s %3.1f %s",k>0 ? "+" : " ", Reactant[k].NoOfMoles,Reactant[k].SpcName());
  dbgp(" = ");
  for (k = 0; k < NProducts(); k++)
    dbgp("%s %3.1f %s",k>0 ? "+" : " ", Product[k].NoOfMoles,Product[k].SpcName());
  dbgpln("");
  };

//---------------------------------------------------------------------------

flag CR_Eqn::ParseTerm(CR_EqnTerm &Term)
  {
  CRCTTokenFile &TF=*RB.pTkns;
  Strng Tkn(TF.NextToken());

  Term.SpcId=SDB.Find(Tkn());

  if (Term.SpcId<0) // Tkn does not contain a valid Specie Name
    {
    int i=0;
    while (i<Tkn.Length() && (isdigit(Tkn[i]) || Tkn[i]=='.' || Tkn[i]=='/')) i++;
    if (i>0)
      {
      if (i==Tkn.Length())
        {
        Term.NoOfMoles=FractionToDouble(Tkn());
        Term.SpcName=TF.NextToken();
        }
      else
        {
        Strng sss;
        sss = Tkn.Left(i);
        Term.NoOfMoles=FractionToDouble(sss());
        Term.SpcName=Tkn.Right(Tkn.Length()-i);
        }
      Term.SpcId=SDB.Find(Term.SpcName());
      }
    else
      Term.SpcName=Tkn();
    }
  else
    {
    Term.NoOfMoles=1.0;
    Term.SpcName=Tkn();
    }

  if (Term.SpcId<0)
    CReactionBlock::Error(&RB, 'B', 3, Term.SpcName());
  if (Term.NoOfMoles<1.0e-6)
    CReactionBlock::Error(&RB, 'B', 12, Term.SpcName());

  return (Term.SpcId>=0);
  }

//---------------------------------------------------------------------------

void CR_Eqn::Parse(CRCTTokenFile &TF)
  {
  Strng Tkn;
  CR_EqnTerm ATerm;
  Clear();

  flag fFwdRctDef=True;
  flag fRevRctDef=True;

  //Reactants
  Reactant.SetSize(0, 5);
  ParseTerm(ATerm);
  Reactant.Add(ATerm);
  while (TF.TokenIs("+"))
    {
    ParseTerm(ATerm);
    Reactant.Add(ATerm);
    }

  if (TF.TokenIs("<->"))
    {
    fFwdRctDef=true;
    fRevRctDef=true;
    }
  else if (TF.TokenIs("<-"))
    {
    fFwdRctDef=false;
    fRevRctDef=true;
    }
  else if(TF.TokenIs("->"))
    {
    fFwdRctDef=true;
    fRevRctDef=false;
    }
  else
    TF.CheckToken("=");

  //Products
  Product.SetSize(0, 5);
  ParseTerm(ATerm);
  Product.Add(ATerm);
  while (TF.TokenIs("+"))
    {
    ParseTerm(ATerm);
    Product.Add(ATerm);
    }

  for (flag Done=0; !Done; )
    {
    if (TF.TokenIs("Extent"))
      {
      fFwdRct=fFwdRctDef;
      fRevRct=fRevRctDef;
      if (TF.TokenIs("Fwd"))
        {
        fFwdRct=True;
        fRevRct=TF.TokenIs("Rev");
        }
      else if (TF.TokenIs("Rev"))
        {
        fFwdRct=True;
        fRevRct=False;
        }

      TF.CheckToken(":");

      Strng EqnName(TF.NextToken());
      pControl=CR_EqnControlFactory::Construct(EqnName(), this);
      if (pControl)
        Control().Parse(TF);
      else
        CReactionBlock::Error(&RB, 'B', 5);

      }
    else if (TF.TokenIs("HeatOfReaction"))
      {
      TF.CheckToken(":");
      Strng EqnName(TF.NextToken());
      pHeat=CR_EqnHeatFactory::Construct(EqnName(), this);
      if (pHeat)
        Heat().Parse(TF);
      else
        CReactionBlock::Error(&RB, 'B', 5);
      }

    else
      Done=1;
    }


//  if (RB.fHtOfFormsUseFinalised)
//    {
////    if (RB.fUseHtOfForms && (pHeat!=NULL))
////      CReactionBlock::Error(&RB, 'B', 14);
////    else if (!RB.fUseHtOfForms && (pHeat==NULL))
////      CReactionBlock::Error(&RB, 'B', 15);
//    }
//  else
//    {
//    //RB.fUseHtOfForms=(pHeat==NULL);
//    //RB.fHtOfFormsUseFinalised=true;
//    }
  if (pHeat==NULL)
    pHeat=new CR_EqnHeat(NULL, this);

//  if (RB.fUseHtOfForms)
//    {
//    //for (int k = 0; k < NReactants(); k++)
//    //  {
//    //  if (!SDB[Reactant[k].SpcId].HfOK())
//    //    CReactionBlock::Error(&RB, 'B', 16, Reactant[k].SpcName());
//    //  }
//    //for (k = 0; k < NProducts(); k++)
//    //  {
//    //  if (!SDB[Product[k].SpcId].HfOK())
//    //    CReactionBlock::Error(&RB, 'B', 16, Product[k].SpcName());
//    //  }
//    }
  if (NReactants()>0 && NProducts()>0)
    {
    //double NMoles=0;
    //for (int k = 0; k < NReactants(); k++)
    //  NMoles+=Reactant[k].NoOfMoles;
    //double Scl=1.0/GTZ(NMoles);

    //for (int k = 0; k < NReactants(); k++)
    //  Reactant[k].NoOfMoles*=Scl;
    //for (k = 0; k < NProducts(); k++)
    //  Product[k].NoOfMoles*=Scl;
    }
  StructureChanged(this);
  }

//---------------------------------------------------------------------------

int CR_Eqn::SpecieReactantIndex(Strng &SpcName)
  {
  int i;
  for (i=NReactants()-1; i>=0; i--)
    if (Reactant[i].SpcName.XStrCmp(SpcName())==0)
      break;
  return i;
  }

int CR_Eqn::SpecieProductIndex(Strng &SpcName)
  {
  int i;
  for (i=NProducts()-1; i>=0; i--)
    if (Product[i].SpcName.XStrCmp(SpcName())==0)
      break;
  return i;
  }

//---------------------------------------------------------------------------

int CR_Eqn::SpecieReactantIndex(int SpcId)
  {
  int i;
  for (i=NReactants()-1; i>=0; i--)
    if (Reactant[i].SpcId==SpcId)
      break;
  return i;
  }

int CR_Eqn::SpecieProductIndex(int SpcId)
  {
  int i;
  for (i=NProducts()-1; i>=0; i--)
    if (Product[i].SpcId==SpcId)
      break;
  return i;
  }

//---------------------------------------------------------------------------

void CR_Eqn::SetUp()
  {
  if (HasControl())
    Control().SetUp();
  if (pHeat)
    Heat().SetUp();
  }

//---------------------------------------------------------------------------

dword CR_Eqn::Check()
  {
  BalanceData BD(this);

  BD.NumSpc = NReactants() + NProducts();
  BD.SpcName.SetSize(BD.NumSpc);
  BD.Moles.SetSize(BD.NumSpc);

  if (NReactants()>0)
    {
    // Normalize no of moles (stoichiometric) to 1 for the first reactant
    double Scl=1.0/GTZ(Reactant[0].NoOfMoles);

    int j = 0;
    for (int k = 0; k < NReactants(); k++)
      {
      BD.SpcName[j]=Reactant[k].SpcName();
      BD.Moles[j] = Reactant[k].NoOfMoles*Scl;
      j++;
      }
    for (k = 0; k < NProducts(); k++)
      {
      BD.SpcName[j]=Product[k].SpcName();
      BD.Moles[j] = Product[k].NoOfMoles*Scl;
      j++;
      }
    }
 // end copy specie names over

  dwState=0;
  if (!pControl)
    dwState|=CRS_NoExtent;
  if (!BD.Check())
    dwState|=CRS_NoBalance;

//  if (!pControl)
//    SetCI(1, "Reaction has no extent");
//  else
//    ClrCI(1);
//

  return dwState;//BD.Check() && pControl;
  }

//----------------------------------------------------------------------------

void CR_Eqn::Zero()
  {
  dMlXfer=0.0;
  for (int i=0; i<NReactants(); i++)
    Reactant[i].Zero();
  for (int i=0; i<NProducts(); i++)
    Product[i].Zero();
  ClrCI(4);
  ClrCI(5);
  ClrCI(6);
  ClrCI(7);
  ClrCI(8);
  };

//---------------------------------------------------------------------------

dword  CR_Eqn::GetState()
  {
//  if (!CpsOK())
//    dwState|=CRS_BadCps;
//  else
//    dwState=dwState&~CRS_BadCps;
//  if (!HfsOK())
//    dwState|=CRS_BadHfs;
//  else
//    dwState=dwState&~CRS_BadHfs;

  return dwState;
  };

//---------------------------------------------------------------------------

double CR_Eqn::LimitReactantMoles()
  {
  CDVector & Moles = RB.Moles;
  CDVector & DMoles = RB.DMoles;
  int n=NReactants();
  double R=n>0 ? 1.0e100 : 0.0;
  for (int i = 0; i < n; i++)
    {
    CR_EqnTerm Rt=Reactant[i];
    R = Min(R, (Moles[Rt.SpcId]) / GTZ(Rt.NoOfMoles));
    }
  return R;
  }

//---------------------------------------------------------------------------

double CR_Eqn::LimitProductMoles()
  {
  CDVector & Moles = RB.Moles;
  CDVector & DMoles = RB.DMoles;
  int n=NProducts();
  double P=n>0 ? 1.0e100 : 0.0;
  for (int i = 0; i < n; i++)
    {
    CR_EqnTerm Pt=Product[i];
    P = Min(P, (Moles[Pt.SpcId]) / GTZ(Pt.NoOfMoles));
    }
  return P;
  }

//---------------------------------------------------------------------------
// Result (x) is the # Moles to remove from Reactants & add to Products
class CReactingMolesSolver : public MRootFinder
  {
  public:
    CR_Eqn & E;
    CToleranceBlock &m_Tol;
    CReactingMolesSolver(CR_Eqn * pE, CToleranceBlock &Tol) :
      MRootFinder("CR_Eqn.EquilFnd", Tol),
      m_Tol(Tol),
      E(*pE)
      { 
      };
    double Function(double dProdMoles)
      {
      double EqK=0.0;
      double K=0.0;
      E.Control().CalcKs(dProdMoles, EqK, K);
      //dbgpln("Fn:[%3i] %*s %20.14f %20.14f %20.14f %20.14f",Iterations(), 1,"", dProdMoles, m_EqK, m_K, 1+(m_EqK-m_K) - (E.dEqK0-E.dK0)*(1.0-Extent));
      return 1.0+(EqK-K);
      };
    bool Converged(double X1, double X2, double F1, double F2)
      {
      if (1)
        {
        //dbgpln("Converged: %*s %s @%5.2f",80,"", m_Tol.ConvergedVV(F1, F2)?"OK":"  ", m_Tol.GetNormalError());
        }
      return m_Tol.ConvergedVV(F1, F2);
      }
  };

//---------------------------------------------------------------------------

double CR_Eqn::ReactingMoles()
  {
  // (+)dm means remove from Reactants
  if (!fRctOn || (RB.CSB() && RB.CSB()->Cmd()==eStateConverge))
  //if (!fRctOn)
    return 0.0;

  double LimitProdMls  = /*1 ||*/ RevRct() ? LimitProductMoles() : 0.0;
  double LimitReactMls = /*1 ||*/ FwdRct() ? LimitReactantMoles() : 0.0;

  if (DoFwdRctExtent())
    return Control().FwdRctExtent();

  if (DoRevRctExtent())
    return Control().RevRctExtent();

  CReactingMolesSolver EqF(this, RB.m_RctTol);
  switch (EqF.FindRoot(1.0, -LimitProdMls, LimitReactMls))
    {
    case RF_OK:
      return EqF.Result();
    case RF_LoLimit:
      return -LimitProdMls;
    case RF_HiLimit:
      return LimitReactMls;
    default: 
      LogError(FullObjTag(), 0, "Rct not solved - RootFinder:%s", EqF.ResultString(EqF.Error())); 
      return EqF.Result();
    }
  }


//---------------------------------------------------------------------------

flag CR_Eqn::SetReactProdInx(int SpcId, int &XReactTerm, int &XProdTerm)
  {
  if (pControl==NULL)
    {
    pControl= new CR_EqnControl(NULL, this);
    StructureChanged(this);
    }

  return Control().SetReactProdInx(SpcId, XReactTerm, XProdTerm);
  }

//---------------------------------------------------------------------------

flag CR_Eqn::SetExtent(int SpcId, double Frac)
  {
  if (pControl==NULL)
    {
    pControl= new CR_EqnControl(NULL, this);
    StructureChanged(this);
    }

  return Control().SetExtent(SpcId, Frac);
  }

//---------------------------------------------------------------------------

void CR_Eqn::Set_msDHf(int SpcNo, double msdH)
  {
  delete pHeat;
  pHeat = new CR_EqnHeatFixedMs(&CR_EqnHeatFixedMsClassDef, this, msdH);
  StructureChanged(this);
  }

//---------------------------------------------------------------------------

void CR_Eqn::Set_mlDHf(int SpcNo, double mldH)
  {
  delete pHeat;
  pHeat = new CR_EqnHeatFixedMl(&CR_EqnHeatFixedMlClassDef, this, mldH);
  StructureChanged(this);
  }

//---------------------------------------------------------------------------

flag CR_Eqn::CpsOK()
  {
  for (int k = 0; k < NReactants(); k++)
    if (!SDB[Reactant[k].SpcId].CpOK())
      return false;
  for (k = 0; k < NProducts(); k++)
    if (!SDB[Product[k].SpcId].CpOK())
      return false;
  return true;
  }

//---------------------------------------------------------------------------

flag CR_Eqn::HfsOK()
  {
  for (int k = 0; k < NReactants(); k++)
    if (!SDB[Reactant[k].SpcId].H25OK())
      return false;
  for (k = 0; k < NProducts(); k++)
    if (!SDB[Product[k].SpcId].H25OK())
      return false;
  return true;
  };

//===========================================================================
// Performs the reaction, diminishing reactants and making products,
// as well as doing a tally of the overall Heat of Reaction.
//
// deltaHeat := ProductHeat - ReactantHeat;
// The normal convention requires that if the result of this subtraction
// is negative then the reaction is exothermic, if positive then
// endothermic. In our case we will reverse the convention i.e. subtract
// ProductHeat from ReactantHeat and have a positive value equal to
// an increase of Heat released (exothermic) and vice versa
//===========================================================================

long CR_Eqn::SolveBlock1(CReactionBlock &RB, pCR_Eqn *Eqns, int NEqns, CRctSolveBlk &RSB)
  {
  double HeatR=0.0;
  /**
  #if dbgReaction
  if (RB.DoDbgBrk() && RB.DoDbgBrk() && dbgRctSolve())
    dbgpln("CR_Eqn::SolveBlock1 [%i]", NEqns);
  #endif

  // Initialise - All species available to all equations
  for (int e=0; e<NEqns; e++)
    {
    CR_Eqn &E=*(Eqns[e]);
    E.fDone=0;
    E.iLimitSpc=-1;
    for (int l=0; l<E.NReactants(); l++)
      {
      CR_EqnTerm &L=E.Reactant[l];
      L.dSpAvailToEqn=RB.Moles[L.SpcId];
      }
    }

  // Assume No Limiting Specie
  flag SpDone[MaxSpecies];
  SpMArray SpReqd, UseScl;
  RB.DMoles.Zero();

  //flag CalcControl=True;//(RB.IsDynamic() && IC.StepPreStart()) || !RB.IsDynamic();

  int LowSn=-1; // No Limiting Specie
  for (int s=0; s<SVSpcCount(); s++)
    SpDone[s]=0;
  for (int SpsDone=0; SpsDone<RB.NumSpecies; )
    {
    SpReqd.Zero();
    // Find
    for (int e=0; e<NEqns; e++)
      {
      if (!Eqns[e]->fDone)
        {
        CR_Eqn &E=*(Eqns[e]);
        E.dMlXfer=1.0e100;

        double EXfr=0.0;
        //if (CalcControl)
        //  {
        EXfr=E.ReactingMoles();
        if (!E.FwdRct())
          EXfr=LEZ(EXfr);
        if (!E.RevRct())
          EXfr=GEZ(EXfr);
        //  }

        for (int l=0; l<E.NReactants(); l++)
          {
          CR_EqnTerm &L=E.Reactant[l];
          L.dMlXfer=Min(EXfr, L.dSpAvailToEqn/L.NoOfMoles);

          if (L.dMlXfer<E.dMlXfer)
            {
            // Limiting specie for equation
            E.iLimitSpc=L.SpcId;
            E.dMlXfer=L.dMlXfer;
            }
          }
        if (E.iLimitSpc==LowSn)
          E.fDone=True;
        for (l=0; l<E.NReactants(); l++)
          {
          CR_EqnTerm &L=E.Reactant[l];
          L.dSpUsed=E.dMlXfer*L.NoOfMoles;
          SpReqd[L.SpcId] += L.dSpUsed;
          }
        }
      }

    //flag SpsDone=0;
    double ScaleOne=0.9999999999;
    double LowScl=ScaleOne;
    int NewLowSn=-1;
    for (int s=0; s<SVSpcCount(); s++)
      {
      #if dbgReaction
      if (RB.DoDbgBrk() && dbgRctSolve())
        if (!SpDone[s] && TstGTZ(UseScl[s]) && TstGTZ(SpReqd[s]))
          dbgpln("Use# %14.6g %14.6g %s", UseScl[s], SpReqd[s], SDB[s].SymOrTag());
      #endif

      if (s==LowSn)
        SpDone[s]=1;
      if (!SpDone[s])
        {
        UseScl[s]=(SpReqd[s]>1.0e-12) ? RB.Moles[s]/SpReqd[s] : 1.0;
        if (UseScl[s]<LowScl)
          {
          LowScl=UseScl[s];
          NewLowSn=s;
          }
        else
          SpDone[s]=(UseScl[s]>ScaleOne);
        }
      if (SpDone[s])
        SpsDone++;
      //#if dbgReaction
      //if (RB.DoDbgBrk() && dbgRctSolve())
      //  dbgpln("s# %14.6g %2i, %3i %s", UseScl[s], SpDone[s], SpsDone, SDB[s].Name());
      //#endif
      }
    LowSn=NewLowSn;
    #if dbgReaction
    if (RB.DoDbgBrk() && dbgRctSolve())
      dbgpln("NewLowSp %14.6g %s", LowScl, LowSn>=0 ? SDB[LowSn].SymOrTag() : "---");
    #endif

    for (e=0; e<NEqns; e++)
      if (!Eqns[e]->fDone)
        {
        CR_Eqn &E=*(Eqns[e]);
        //E.MXfer=1.0e100;
        flag EDone=1;
        for (int l=0; l<E.NReactants(); l++)
          {
          CR_EqnTerm &L=E.Reactant[l];
          EDone = EDone && SpDone[L.SpcId];
          if (L.SpcId==LowSn)
            L.dSpAvailToEqn=L.dSpUsed*UseScl[LowSn];
          }
        E.fDone=EDone;
        #if dbgReaction
        if (E.fDone)
          if (RB.DoDbgBrk() && dbgRctSolve())
            dbgpln("Eqn %2i Done ", e);
        #endif
        }
    }

  // Calculate Actual Transfers
  for (e=0; e<NEqns; e++)
    {
    CR_Eqn &E=*(Eqns[e]);
    //E.dHeatOfReact=0.0;
    #if dbgReaction
    if (RB.DoDbgBrk() && dbgRctSolve())
      dbgpln("2# %3i %14.6g ", e, E.dMlXfer);
    #endif
    for (int l=0; l<E.NReactants(); l++)
      {
      CR_EqnTerm &L=E.Reactant[l];
      int Sn=L.SpcId;
      L.dMlXfer=-E.dMlXfer*L.NoOfMoles; //??
      RB.DMoles[Sn] += L.dMlXfer;
      L.dUseFrac=L.dMlXfer/GTZ(RB.Moles[Sn]);
      //E.dHeatOfReact+=L.dMlXfer*SDB[Sn].mlHf(RB.RefTempK, RB.RefTempK);  //hss
      #if dbgReaction
      if (RB.DoDbgBrk() && dbgRctSolve())
        dbgpln("  r %14.6g [%14.6g%%] %s", L.dMlXfer, L.dUseFrac*100.0, SDB[Sn].SymOrTag());
      #endif

      }
    for (int r=0; r<E.NProducts(); r++)
      {
      CR_EqnTerm &R=E.Product[r];
      int Sn=R.SpcId;
      R.dMlXfer=E.dMlXfer*R.NoOfMoles; //??
      RB.DMoles[Sn] += R.dMlXfer;
      //E.dHeatOfReact+=R.dMlXfer*SDB[Sn].mlHf(RB.RefTempK, RB.RefTempK);  //hss
      R.dUseFrac=0.0;
      #if dbgReaction
      if (RB.DoDbgBrk() && dbgRctSolve())
        dbgpln("  p %14.6g %*.*s %s", R.dMlXfer, 14+2, 14+2, "", SDB[Sn].SymOrTag());
      #endif
      }
    HeatR+=E.Heat().totDHf(RB.TempKProd);//Feed);
    }

  for (s=0; s<SVSpcCount(); s++)
    RB.Moles[s]+=RB.DMoles[s];
***/
  return -1;
  };

//---------------------------------------------------------------------------

long CR_Eqn::SolveBlock2(CReactionBlock &RB, pCR_Eqn *Eqns, int NEqns, CRctSolveBlk &RSB)
  {
  double HeatR=0.0;
  /**

  #if dbgReaction
  if (RB.DoDbgBrk() && (dbgRctSolve() || dbgRctSolveIter()))
    dbgpln("CR_Eqn::SolveBlock2 [%i]", NEqns);
  #endif

  // Initialise - All species available to all equations
  for (int e=0; e<NEqns; e++)
    {
    CR_Eqn &E=*(Eqns[e]);
    E.fDone=0;
    E.iLimitSpc=-1;
    E.dRctScl=1.0;
    E.dMlXfer=0.0;
    E.dMlXferTot=0.0;
    for (int l=0; l<E.NReactants(); l++)
      {
      CR_EqnTerm &L=E.Reactant[l];
      L.dSpAvailToEqn=RB.Moles[L.SpcId];
      }
    }

  SpMArray SpScale, DMoles;
  DMoles=0.0;
  SpScale=1.0;
//  flag CalcControl=(RB.IsDynamic() && IC.StepPreStart()) || !RB.IsDynamic();
  //flag CalcControl=True;

  for (int RctConverged=False, Iter=100; !RctConverged && Iter; Iter--)
    {
    // HeatR=0.0; CNM 6 Feb 1999 Reactiojs below are cumulative
    RctConverged=True;
    RB.DMoles=DMoles;
    DMoles=0.0;
    for (int e=0; e<NEqns; e++)
      {
      CR_Eqn &E=*(Eqns[e]);
      E.dEqK0=0.0;
      E.dK0=0.0;
      E.dExtent=1.0;
      if (E.HasControl())
        E.Control().CalcKs(0.0, E.dEqK0, E.dK0, E.dExtent);
      }

    for (e=0; e<NEqns; e++)
      {
      CR_Eqn &E=*(Eqns[e]);

      double OldXfer=E.dMlXfer;
      double EXfr= E.ReactingMoles();
      E.dMlXfer=EXfr;
      if (!E.FwdRct())
        E.dMlXfer=LEZ(E.dMlXfer);
      if (!E.RevRct())
        E.dMlXfer=GEZ(E.dMlXfer);

      double Err;
      if (!ConvergedDV(OldXfer-E.dMlXfer, E.dMlXfer, 1.0e-8, 1.0e-8, Err))
        RctConverged=False;

      }

    for (e=0; e<NEqns; e++)
      {
      CR_Eqn &E=*(Eqns[e]);
      #if dbgReaction
      double EXfr=E.dMlXfer;
      #endif
      //E.dMlXfer/=E.dRctScl;
      E.dRctScl=1.0;
      if (E.dMlXfer>0.0)
        {
        for (int r=0; r<E.NReactants(); r++)
          E.dRctScl=Min(E.dRctScl, SpScale[E.Reactant[r].SpcId]);
        }
      else
        {
        for (int p=0; p<E.NProducts(); p++)
          E.dRctScl=Min(E.dRctScl, SpScale[E.Product[p].SpcId]);
        }
      E.dRctScl=GEZ(E.dRctScl);
      E.dMlXfer*=E.dRctScl;
      for (int r=0; r<E.NReactants(); r++)
        {
        CR_EqnTerm &T=E.Reactant[r];
        DMoles[T.SpcId] -= E.dMlXfer*T.NoOfMoles;
        }
      for (int p=0; p<E.NProducts(); p++)
        {
        CR_EqnTerm &T=E.Product[p];
        DMoles[T.SpcId] += E.dMlXfer*T.NoOfMoles;
        }
      #if dbgReaction
      if (RB.DoDbgBrk() && dbgRctSolveIter())
        dbgpln("Eqn:%2i %14.6g EqnMlXfer:%14.6g RctScl:%14.6g ",e, EXfr, E.dMlXfer, E.dRctScl);
      #endif
      }

    for (int s=0; s<RB.NumSpecies; s++)
      {
      double OldScl=SpScale[s];
      if (DMoles[s]+RB.Moles[s]<0.0)
        {
        SpScale[s]*=GEZ(RB.Moles[s])/GTZ(fabs(DMoles[s]));  // DMoles < 0.0
        RctConverged=False;
        }
      else if (SpScale[s]<1.0)
        {
        SpScale[s]*=1.0+(RB.Moles[s]+DMoles[s])/GTZ(RB.Moles[s]);  // DMoles < 0.0
        SpScale[s]=Range(0.0, SpScale[s], 1.0);
        }
      double Err;
      if (!ConvergedDV(OldScl-SpScale[s], SpScale[s], 1.0e-8, 1.0e-8, Err))
        RctConverged=False;
      #if dbgReaction
      if (RB.DoDbgBrk() && dbgRctSolveIter())
        if (SpScale[s]>0.0 && SpScale[s]<1.0)
          dbgpln("Scale:%14.6g%% dM:%14.6g M:%14.6g %s", SpScale[s]*100.0, DMoles[s], RB.Moles[s], SDB[s].SymOrTag());
      #endif
      }
    }

  // Calculate Actual Transfers
  RB.DMoles.Zero();
  for (e=0; e<NEqns; e++)
    {
    CR_Eqn &E=*(Eqns[e]);
    E.dMlXferTot+=E.dMlXfer;
    #if dbgReaction
    if (RB.DoDbgBrk() && dbgRctSolve())
      dbgpln("2# %3i %14.6g ", e, E.dMlXfer);
    #endif
    for (int l=0; l<E.NReactants(); l++)
      {
      CR_EqnTerm &L=E.Reactant[l];
      int Sn=L.SpcId;
      L.dMlXfer=-E.dMlXfer*L.NoOfMoles; //??
      RB.DMoles[Sn] += L.dMlXfer;
      L.dUseFrac=L.dMlXfer/GTZ(RB.Moles[Sn]);
      #if dbgReaction
      if (RB.DoDbgBrk() && dbgRctSolve())
        dbgpln("  r %14.6g %+14.6g [%14.6g%%] %s", RB.Moles[Sn], L.dMlXfer, L.dUseFrac*100.0, SDB[Sn].SymOrTag());
      #endif

      }
    for (int r=0; r<E.NProducts(); r++)
      {
      CR_EqnTerm &R=E.Product[r];
      int Sn=R.SpcId;
      R.dMlXfer=E.dMlXfer*R.NoOfMoles; //??
      RB.DMoles[Sn] += R.dMlXfer;
      R.dUseFrac=R.dMlXfer/GTZ(RB.Moles[Sn]);
      #if dbgReaction
      if (RB.DoDbgBrk() && dbgRctSolve())
        dbgpln("  p %14.6g %+14.6g [%14.6g%%] %s", RB.Moles[Sn], R.dMlXfer, R.dUseFrac*100.0, SDB[Sn].SymOrTag());
      #endif
      }
    HeatR+=E.Heat().totDHf(RB.TempKFeed);
    #if dbgReaction
    if (RB.DoDbgBrk() && dbgRctSolve())
      dbgpln("  h %14.6g %+14.6g ", HeatR, E.Heat().totDHf(RB.TempKFeed));
    #endif
    }

  for (int s=0; s<SVSpcCount(); s++)
    {
    RB.Moles[s]=GEZ(RB.Moles[s]+RB.DMoles[s]);
    }

  for (e=0; e<NEqns; e++)
    {
    CR_Eqn &E=*(Eqns[e]);
    E.dMlXfer=E.dMlXferTot;
    }
  **/
  return 0;
  };

//---------------------------------------------------------------------------

inline int Find(CIArray & A, int What)
  {
  for (int i=0; i<A.GetSize(); i++)
    if (A[i]==What)
      return i;
  return -1;
  }

long CR_Eqn::SolveBlock3(CReactionBlock &RB, pCR_Eqn *Eqns, int NEqns, CRctSolveBlk &RSB)
  {
  CRctSolveBlk RSBLcl;
  //double HeatR=0.0;

  #if dbgReaction
  if (RB.DoDbgBrk() && (dbgRctSolve() || dbgRctSolveIter()))
    dbgpln("CR_Eqn::SolveBlock3 [%i]", NEqns);
  #endif

  // Initialise - All species available to all equations
  flag DoInit=(Eqns[0]->pBlk==NULL);
  if (DoInit)
    Eqns[0]->pBlk=new CR_EqnBlk;

  // References
  CIArray    &L2SpId        = Eqns[0]->pBlk->L2SpId;
  CIArray    &SpId2L        = Eqns[0]->pBlk->SpId2L;
  CNRSimplex &SPlex         = Eqns[0]->pBlk->SPlex;
  CDVector   &SpcReqd       = Eqns[0]->pBlk->SpcReqd;
  CDVector   &SpcTotRate    = Eqns[0]->pBlk->SpcTotRate;
  CDVector   &Time2Exhaust  = Eqns[0]->pBlk->Time2Exhaust;
  CIArray    &IsEquil       = Eqns[0]->pBlk->IsEquil;
  CDVector   &EXfrReqd      = Eqns[0]->pBlk->EXfrReqd;
  CDVector   &EXfrRate      = Eqns[0]->pBlk->EXfrRate;
  CDVector   &EXfrReqdTarget  = Eqns[0]->pBlk->EXfrReqdTarget;
  CDVector   &EXfrReqdTot   = Eqns[0]->pBlk->EXfrReqdTot;
  CDVector   &EXfrReqdStep  = Eqns[0]->pBlk->EXfrReqdStep;

  if (DoInit)
    {
    SpId2L.SetSize(SVSpcCount());
    for (int e=0; e<NEqns; e++)
      {
      CR_Eqn &E=*(Eqns[e]);
      for (int l=0; l<E.NReactants(); l++)
        {
        int s=E.Reactant[l].SpcId;
        if (Find(L2SpId, s)<0)
          SpId2L[s]=L2SpId.Add(s);
        }
      for (l=0; l<E.NProducts(); l++)
        {
        int s=E.Product[l].SpcId;
        if (Find(L2SpId, s)<0)
          SpId2L[s]=L2SpId.Add(s);
        }
      }
    int NSpcs=L2SpId.GetSize();
    SpcReqd.SetSize(NSpcs);
    SpcTotRate.SetSize(NSpcs);
    Time2Exhaust.SetSize(NSpcs);
    IsEquil.SetSize(NEqns);
    EXfrReqd.SetSize(NEqns);
    EXfrRate.SetSize(NEqns);
    EXfrReqdTarget.SetSize(NEqns);
    EXfrReqdTot.SetSize(NEqns);
    EXfrReqdStep.SetSize(NEqns);
    }

  int NSpcs=L2SpId.GetSize();
  #if dbgReaction
  int TagSymLen=20;
  if (RB.DoDbgBrk() && dbgRctSolve())
    {
    for (int s=0; s<NSpcs; s++)
      TagSymLen=Max(TagSymLen, (int)strlen(SDB[L2SpId[s]].SymOrTag()));
    }
  #endif

  // X1..XNEqns  -- Transfer
  // XNEqns+1..  -- Moles of each Specie
  double TotMoles=0.0;
  for (int s=0; s<NSpcs; s++)
    //TotMoles+=RB.Moles_S[L2SpId[s]];
    TotMoles+=RB.Moles_BS[L2SpId[s]];  //hss 14/8/98

  //  Actual Moles in Reaction inc. all Srcs
  SpMArray MolesRct;
  MolesRct=RB.Moles;
  
  for (int e=0; e<NEqns; e++)
    {
    CR_Eqn &E=*(Eqns[e]);
    E.dMlXfer=0.0;
    E.fCnvrgd=true;
    for (int l=0; l<E.NReactants(); l++)
      E.Reactant[l].dMlXfer=0.0;
    for (int r=0; r<E.NProducts(); r++)
      E.Product[r].dMlXfer=0.0;
    }

  RB.DMoles.Zero();
  //RB.dRctTol=Range(1.0e-20, RB.dRctTol, 1.0);
  bool bConverged=false;
  bool bSpecOverflow=false;
  //double TotalStep=0.0;

  EXfrReqdTarget.SetAll(0.0);
  EXfrReqdTot.SetAll(0.0);

  //Example:
  //Eqn1 A+C=D+F   Extent C=1
  //Eqn2 B+C=E+F   Extent C=1

  //Specie                                          A           B           C
  //Step 1 -------------------------------------
  //SpcAvail                                        2         0.5           2
  //SpcReqd                                         2         0.5         2.5

  //RelRate                                         1           1           1
  //SpcTotRate   = sigma RelRate across eqns        1           1           2
  //Time2Exhaust = SpcAvail/SpcTotRate              2         0.5           1
  //MinTime2Exhaust                               ..........  0.5 ...........
  //SpcRqdInStep = MinTime2Exhaust*SpcTotRate     0.5         0.5           1
  //Step 2 -------------------------------------
  //SpcAvail     = SpcAvail-SpcRqdInStep          1.5           0           1
  //SpcReqd                                         1                       1

  //RelRate                                         1                       1
  //SpcTotRate   = sigma RelRate across eqns        1                       1
  //Time2Exhaust = SpcAvail/SpcTotRate            1.5                       1
  //MinTime2Exhaust                                 1 .......................
  //SpcRqdInStep = MinTime2Exhaust*SpcTotRate       1                       1
  //SpcAvail     = SpcAvail-SpcRqdInStep            0                       0

  for (int Iter=0; Iter<RB.m_RctTol.GetMaxIters(); Iter++)
    {
    bSpecOverflow=false;
    RSBLcl.Clear();

    //HeatR=0.0;
    //                   virtual bool   ExtentITOInitConds()      { return false; };
    //      if (Iter==0 || (E.HasControl() && !E.Control().ExtentITOInitConds()))

    // LE Equations - Limits of Reaction
    //flag CalcControl=True;
    if (Iter==0)
      {
      for (e=0; e<NEqns; e++)
        {
        CR_Eqn &E=*(Eqns[e]);
        if (E.RctOn())
          {
          if (E.HasControl())
            {
            E.Control().m_dKStart=E.Control().ExtentActual();
            //dbgpln("KStart = %25.18g", E.Control().m_dKStart);
            E.m_dRelRate=E.Control().m_dRelRate;
            IsEquil[e]=(E.Control().Basis()==RctEquilibrium);
            }
          EXfrReqd[e] = E.ReactingMoles();
          EXfrRate[e] = E.m_dRelRate;

          if (EXfrRate[e]<0.0 && !E.RevRct())
            {
            EXfrReqd[e]=0.0;
            EXfrRate[e]=0.0;
            }
          }
        else
          {
          EXfrReqd[e]=0.0;
          EXfrRate[e]=0;
          }
        EXfrReqdTarget[e]=EXfrReqd[e];
        E.iDirn=(int)Sign(EXfrReqd[e]);
        }
      }
    else
      {
      for (e=0; e<NEqns; e++)
        {
        CR_Eqn &E=*(Eqns[e]);
        if (IsEquil[e])
          EXfrReqdTarget[e] += E.ReactingMoles();
        
        EXfrReqd[e] = EXfrReqdTarget[e]-EXfrReqdTot[e];
        if (fabs(EXfrReqd[e])<1.0e-20*EXfrReqdTarget[e])
          EXfrRate[e]=0;
        Eqns[e]->iDirn=(int)Sign(EXfrReqd[e]);
        }
      }

    #if dbgReaction
    if (RB.DoDbgBrk() && dbgRctSolveEqn())
      {
      dbgpln("----------------------------------------------");
      for (e=0; e<NEqns; e++)
        dbgpln("1# %3i %2i Xfer                    Rq:%25.18f Rt:%25.18f ", e, Eqns[e]->iDirn, EXfrReqd[e], EXfrRate[e]);
        //dbgpln("1# %3i %2i Xfer                    Rq:%25.18f Rt:%25.18f Krat:%25.18f", e, Eqns[e]->iDirn, EXfrReqd[e], EXfrRate[e], Eqns[e]->dK0/GTZ(Eqns[e]->dEqK0));
      }
    #endif

    // scan thru all the species and see which are being used by more than one eqn
    // SpcReqd is the sum of the spc required for all reactions.
    SpcReqd.SetAll(0.0);
    SpcTotRate.SetAll(0);
    Time2Exhaust.SetAll(0);

    for (e=0; e<NEqns; e++)
      {
      CR_Eqn &E=*(Eqns[e]);
      if (EXfrReqd[e]>0)
        {
        for (int l=0; l<E.NReactants(); l++)
          {
          int s=SpId2L[E.Reactant[l].SpcId];
          double NMoles=E.Reactant[l].NoOfMoles;
          SpcReqd[s]+=NMoles*EXfrReqd[e];
          SpcTotRate[s]+=NMoles*EXfrRate[e];
          }
        }
      else if (EXfrReqd[e]<0)
        {
        for (int l=0; l<E.NProducts(); l++)
          {
          int s=SpId2L[E.Product[l].SpcId];
          double NMoles=E.Product[l].NoOfMoles;
          SpcReqd[s]+=NMoles*EXfrReqd[e];
          SpcTotRate[s]+=-NMoles*EXfrRate[e];
          }
        }
      }

    // SpcReqd[s] is NonZero and SpcAvail is NonZero for any specie
    // But SpcReqd[s] > SpcAvail then take a short step to  use up this specie

    double MinTime2ExhaustAny=1e100;
    for (s=0; s<NSpcs; s++)
      {
      if (SpcReqd[s]>0)
        {
        double SpcAvail=Min(SpcReqd[s], RB.Moles[L2SpId[s]]);
        if (SpcAvail>1e-20 && SpcTotRate[s]>1e-20)
          {
          Time2Exhaust[s]=SpcAvail/GTZ(SpcTotRate[s]);
          MinTime2ExhaustAny=Min(MinTime2ExhaustAny, Time2Exhaust[s]);
          }
        else
          Time2Exhaust[s]=1e100;
        }
      else if (SpcReqd[s]<0)
        {
        double SpcAvail=Min(-SpcReqd[s], RB.Moles[L2SpId[s]]);
        if (SpcAvail>1e-20 && -SpcTotRate[s]>1e-20)
          {
          Time2Exhaust[s]=SpcAvail/GTZ(-SpcTotRate[s]);
          MinTime2ExhaustAny=Min(MinTime2ExhaustAny, Time2Exhaust[s]);
          }
        else
          Time2Exhaust[s]=1e100;
        }
      else
        Time2Exhaust[s]=1e100;
      #if dbgReaction
      if (RB.DoDbgBrk() && dbgRctSolveEqnSpc())
        dbgpln("Spc : M:%25.18f Rq:%25.18f Tr:%25.18f Tx:%25.15g  %s",
              RB.Moles[L2SpId[s]], SpcReqd[s], SpcTotRate[s], Time2Exhaust[s], SDB[L2SpId[s]].SymOrTag());
      #endif
      }
    if (MinTime2ExhaustAny==1e100)
      { int xxx=0;};
    #if dbgReaction
    if (RB.DoDbgBrk() && dbgRctSolveEqn())
      dbgpln("MinTime2ExhaustAny : %25.15g", MinTime2ExhaustAny);
    #endif

    for (e=0; e<NEqns; e++)
      {
      double NewReqd;
      if (EXfrReqdTarget[e]>0)
        NewReqd=Range(0.0, MinTime2ExhaustAny*EXfrRate[e], EXfrReqdTarget[e]-EXfrReqdTot[e]);
      else
        NewReqd=Range(EXfrReqdTarget[e]-EXfrReqdTot[e], -(MinTime2ExhaustAny*EXfrRate[e]), 0.0);

      EXfrReqdStep[e]=NewReqd;
      EXfrReqdTot[e]+=NewReqd;

      #if dbgReaction
      if (RB.DoDbgBrk() && dbgRctSolveEqn())
        dbgpln("EXfrReqdStep: %2i %8s %25.18f Tot:%25.18f Init:%25.18f ", e, "", EXfrReqdStep[e], EXfrReqdTot[e], EXfrReqdTarget[e]);
      #endif

      }

    double SimPlxScl=1.0;///(TotMoles);
    SPlex.SetSize(NSpcs+NEqns, NEqns, 0, NSpcs);

    for (int e=0; e<NEqns; e++)
      {
      SPlex.SetFnXiVal(1+(e), 1); // Cost Function
      SPlex.SetLEConst(1+(e), Eqns[e]->iDirn*EXfrReqdStep[e]*SimPlxScl); // Extent of Reaction
      SPlex.SetLEXiVal(1+(e), 1+(e), -1);
      }

    // Equality Eqns - Stoichiometry
    for (s=0; s<NSpcs; s++)
      {
      if (fabs(RB.Moles[L2SpId[s]])>1.0e9)
        bSpecOverflow=true;

      SPlex.SetEqConst(1+s, Range(0.0, RB.Moles[L2SpId[s]]*SimPlxScl, 1.0e9)); // Inital Amount
      SPlex.SetEqXiVal(1+s, 1+NEqns+s, -1);
      }

    for (e=0; e<NEqns; e++)
      {
      CR_Eqn &E=*(Eqns[e]);
      int XEqn=1+(e);
      for (int l=0; l<E.NReactants(); l++)
        {
        int XSpc=1+Find(L2SpId, E.Reactant[l].SpcId);
        double Stoich=E.Reactant[l].NoOfMoles;
        SPlex.SetEqXiVal(XSpc, XEqn, -E.iDirn*Stoich);
        }
      for (l=0; l<E.NProducts(); l++)
        {
        int XSpc=1+Find(L2SpId, E.Product[l].SpcId);
        double Stoich=E.Product[l].NoOfMoles;
        SPlex.SetEqXiVal(XSpc, XEqn, +E.iDirn*Stoich);
        }
      }

    #if dbgReaction
    if (RB.DoDbgBrk() && dbgRctDumpSimplex())
      SPlex.Dump("Before", true, "%15.7f");
    #endif

    int Err=SPlex.Solve();
    Eqns[0]->SetCI(4,Err==1);
    Eqns[0]->SetCI(5,Err==-1);
    double XFScl=1.0;
    if (Err==0)
      {
      #if dbgReaction
      if (RB.DoDbgBrk() && dbgRctDumpSimplex())
        {
        SPlex.Dump("After", false, "%15.7f");
        }
      if (RB.DoDbgBrk() && dbgRctSolveSimplex())
        {
        for (e=0; e<NEqns; e++)
          {
          CR_Eqn &E=*(Eqns[e]);
          double xx=E.iDirn*SPlex.Value(1+e)*XFScl/SimPlxScl;
          dbgpln("Transfer Eqn[%3i] %15.7f", e, xx);
          }
        for (e=1+NEqns; e<=NSpcs+NEqns; e++)
          {
          int s=L2SpId[e-(1+NEqns)];
          double Moles0=SPlex.Value(e)*XFScl/SimPlxScl;
          dbgpln("%-*s %25.18f -> %25.18f [%+15.5e] Moles", TagSymLen, SDB[s].SymOrTag(), RB.Moles[s], Moles0, Moles0-RB.Moles[s]);
          }
        }
      #endif

      // Calculate Actual Transfers
      // RB.DMoles.Zero();
      RSBLcl.Clear();
      //HeatR=0.0;
      bConverged=true;
      for (e=0; e<NEqns; e++)
        {
        CR_Eqn &E=*(Eqns[e]);
        double DiffXfer=E.iDirn*SPlex.Value(1+e)*XFScl/SimPlxScl;
        E.dMlXfer+=DiffXfer;
        E.fCnvrgd=(fabs(DiffXfer)<(RB.m_RctTol.GetAbs()+RB.m_RctTol.GetRel()*fabs(E.dMlXfer))) &&
                  (fabs(EXfrReqd[e])<(RB.m_RctTol.GetAbs()+RB.m_RctTol.GetRel()*fabs(E.dMlXfer)));

        bConverged=bConverged && E.fCnvrgd;

        #if dbgReaction
        if (RB.DoDbgBrk() && dbgRctSolveEqnCnvg())
          dbgpln("EqnCnvg %3i %s %14.8f %14.8f %25.18f %25.18f %25.18f", 
                  e, E.fCnvrgd?"Cnvg":"    ", E.Control().ExtentReqd(), E.Control().ExtentActual(), DiffXfer, EXfrReqd[e], E.dMlXfer);

        double TotDMass=0.0;
        if (RB.DoDbgBrk() && dbgRctSolveEqnXfer())
          dbgpln("2# %3i XFer %*s %25.18f %25s %25s  %25s", e,
                TagSymLen-20, "",E.dMlXfer, "kgMoles", "kgMoles", "kg");
        #endif
        for (int l=0; l<E.NReactants(); l++)
          {
          CR_EqnTerm &L=E.Reactant[l];
          int Sn=L.SpcId;
          double LDiffXfer=-DiffXfer*L.NoOfMoles; //??
          L.dMlXfer+=LDiffXfer;
          RB.DMoles.VValue[Sn] += LDiffXfer;
          L.m_dMsXfer=L.dMlXfer*SDB[Sn].MoleWt();
          #if dbgReaction
          if (RB.DoDbgBrk() && dbgRctSolveEqnXfer())
            dbgpln("  %+5.1f %-*s %25.18f %+25.18f  %+25.18f",
            -L.NoOfMoles, TagSymLen, SDB[Sn].SymOrTag(), RB.Moles[Sn],
            LDiffXfer, LDiffXfer*SDB[Sn].MoleWt());
          TotDMass+=LDiffXfer*SDB[Sn].MoleWt();
          #endif
          }
        for (int r=0; r<E.NProducts(); r++)
          {
          CR_EqnTerm &R=E.Product[r];
          int Sn=R.SpcId;
          double RDiffXfer=DiffXfer*R.NoOfMoles; //??
          R.dMlXfer+=RDiffXfer;//*R.NoOfMoles; //??
          RB.DMoles.VValue[Sn] += RDiffXfer;
          R.m_dMsXfer=R.dMlXfer*SDB[Sn].MoleWt();

          #if dbgReaction
          if (RB.DoDbgBrk() && dbgRctSolveEqnXfer())
            dbgpln("  %+5.1f %-*s %25.18f %+25.18f  %+25.18f",
            R.NoOfMoles, TagSymLen, SDB[Sn].SymOrTag(), RB.Moles[Sn],
            RDiffXfer, RDiffXfer*SDB[Sn].MoleWt());
          TotDMass+=RDiffXfer*SDB[Sn].MoleWt();
          #endif
          }

      
        RSBLcl.m_HOR+=E.totDHf(RB.Model, ZeroCinK, StdP);
        RSBLcl.m_HORDiff+=E.totDHf(RB.Model, ZeroCinK, StdP)-E.totDHf(NULL, ZeroCinK, StdP);//E.HORDiff();
        RSBLcl.m_Hf_FdT+=E.totDHf(RB.Model, RB.m_TempKFeed, RB.m_Press);
        RSBLcl.m_Hf_PrT+=E.totDHf(RB.Model, RB.m_TempKProd, RB.m_Press);
        RSBLcl.m_Hf_RefT+=E.totDHf(RB.Model, RB.m_UsrDefT, RB.m_UsrDefP);
        RSBLcl.m_Hf_ZeroT+=E.totDHf(RB.Model, ZeroCinK, RB.m_Press);
        RSBLcl.m_dHfZeroTAtmP+=E.totDHf(RB.Model, ZeroCinK, StdP);
        RSBLcl.fCpsOK&=E.CpsOK();
        RSBLcl.fHfsOK&=E.HfsOK();
        //HeatR+=E.Heat().totdH(RB.TempKFeed);
        #if dbgReaction
        if (RB.DoDbgBrk() && dbgRctSolveEqnXfer())
          dbgpln("%*s    Sum:%+25.18f  dH :%25.18g  %+25.18g",
                 32+TagSymLen, "",
            TotDMass,RSBLcl.m_Hf_FdT,E.totDHf(RB.Model, RB.m_TempKFeed, RB.m_Press));
        if (RB.DoDbgBrk() && dbgRctSolveEqnHeat())
          dbgpln("dHfFdT:%+25.18g  dHfPrT:%25.18g  dHfRefT:%+25.18g dHfZeroT:%+25.18g FdT:%10.3f PrT:%10.3f UsrT:%10.3f",
                  RSBLcl.m_Hf_FdT,RSBLcl.m_Hf_PrT,RSBLcl.m_Hf_RefT,RSBLcl.m_Hf_ZeroT,
                  K2C(RB.m_TempKFeed),K2C(RB.m_TempKProd),K2C(RB.m_UsrDefT));
        #endif
        }

      for (int s=0; s<NSpcs; s++)
        {
        int s1=L2SpId[s]; // Inital Amount
        RB.Moles.VValue[s1]=GEZ(MolesRct[s1]+RB.DMoles[s1]);
        }

      if (bConverged)
        {
        // Fill in Gamma
        /*
        for (e=0; e<NEqns; e++)
          {
          CR_Eqn &E=*(Eqns[e]);
          double DiffXfer=E.iDirn*SPlex.Value(1+e);
          E.dMlXfer+=DiffXfer;
          bConverged=bConverged && (fabs(DiffXfer)<(1.0e-20+RB.dRctTol*TotMoles));

          #if dbgReaction
          double TotDMass=0.0;
          if (RB.DoDbgBrk() && dbgRctSolveEqn())
            dbgpln("2# %3i XFer %14.6f   %14s %14s  %14s", e, E.dMlXfer,
                    "kgMoles", "kgMoles", "kg");
          #endif
          for (int l=0; l<E.NReactants(); l++)
            {
            CR_EqnTerm &L=E.Reactant[l];
            int Sn=L.SpcId;
            double LDiffXfer=-DiffXfer*L.NoOfMoles; //??
            L.dMlXfer+=LDiffXfer;
            RB.DMoles[Sn] += LDiffXfer;
            L.dUseFrac=LDiffXfer/GTZ(Max(RB.Moles[Sn], RB.Moles[Sn]+RB.DMoles[Sn]));
            #if dbgReaction
            if (RB.DoDbgBrk() && dbgRctSolveEqn())
              dbgpln("  %+5.1f %-20s %14.6f %+14.6f  %+14.6f",
              -L.NoOfMoles, SDB[Sn].SymOrTag(), RB.Moles[Sn],
              LDiffXfer, LDiffXfer*SDB[Sn].MoleWt());
            TotDMass+=LDiffXfer*SDB[Sn].MoleWt();
            #endif
            }
          for (int r=0; r<E.NProducts(); r++)
            {
            CR_EqnTerm &R=E.Product[r];
            int Sn=R.SpcId;
            double RDiffXfer=DiffXfer*R.NoOfMoles; //??
            R.dMlXfer+=RDiffXfer;//*R.NoOfMoles; //??
            RB.DMoles[Sn] += RDiffXfer;
            R.dUseFrac=RDiffXfer/GTZ(Max(RB.Moles[Sn], RB.Moles[Sn]+RB.DMoles[Sn]));
            #if dbgReaction
            if (RB.DoDbgBrk() && dbgRctSolveEqn())
              dbgpln("  %+5.1f %-20s %14.6f %+14.6f  %+14.6f",
              R.NoOfMoles, SDB[Sn].SymOrTag(), RB.Moles[Sn],
              RDiffXfer, RDiffXfer*SDB[Sn].MoleWt());
            TotDMass+=RDiffXfer*SDB[Sn].MoleWt();
            #endif
            }
          HeatR+=E.Heat().totdH();
          #if dbgReaction
          if (RB.DoDbgBrk() && dbgRctSolveEqn())
            dbgpln("%s    Sum:%+14.6f  H :%14.6g  %+14.6g",
                   "                                                    ",
              TotDMass,HeatR,E.Heat().totdH());
          #endif
          }
        */
        goto Done;
        }
      }
    else
      {
      LogError("Reactions", 0, "Simplex Error %i", Err);
      break;
      }
    }
Done:
  bool Cnvg=true;
  bool ExtOK=true;
  for (e=0; e<NEqns; e++)
    {
    CR_Eqn &E=*(Eqns[e]);
    CR_EqnControl &C=E.Control();
    C.m_dKEOStep=C.ExtentActual();
    Cnvg = Cnvg && !E.fCnvrgd;
    if (E.RctOn() && !C.DynamicRate() && (C.ExtentType()==RBXT_Strict))
      ExtOK = ExtOK && sm_ExtentTol.ConvergedDV(C.ExtentError(), 1.0);
    }
  Eqns[0]->SetCI(7, bSpecOverflow);
  Eqns[0]->SetCI(8, !ExtOK);

  if (0)
    {
    dbgp("                  %14s %14s %10s %10s %10s %10s %10s %10s|", 
         "ProdMoles", "C", "ExtRqd", "M", "MW", "V", "T", "P");
    for (int i=0; i<SVSpcCount();i++)
      dbgp(" %10.10s", SVImg(i).SymOrTag());
    dbgpln("");
    dbgpln("--------------------------------------------");
    }

  RSB+=RSBLcl;
  return 0;
  };

//---------------------------------------------------------------------------

long CR_Eqn::SolveBlock(int Version, CReactionBlock &RB, CReaction **Eqns, int NEqns, CRctSolveBlk & RSB)
  {
  switch (Version)
    {
    case 3:
      return SolveBlock3(RB, (CR_Eqn**)Eqns, NEqns, RSB);
    default:
      //return SolveBlock1(RB, (CR_Eqn**)Eqns, NEqns, RSB);
      return SolveBlock2(RB, (CR_Eqn**)Eqns, NEqns, RSB);
    }
  };

//---------------------------------------------------------------------------

void CR_Eqn::Finalise(CReactionBlock &RB)
  {
  if (HasControl())
    {
    CR_EqnControl &C=Control();
    C.m_dKFinal=C.ExtentActual();
    }
  };

//---------------------------------------------------------------------------

void CR_Eqn::AdjustModel(double dProdMoles, double T, double P)
  {
  CDVector & Moles = RB.Moles;
  CDVector & DMoles = RB.DMoles;

  SpModel &Mdl=*RB.Model;

  for (int i=0; i<SVSpcCount(); i++)
    Mdl.VValue[i]=(Moles[i]+DMoles[i])*SDB[i].MoleWt();

  for (int j=0; j<Reactant.GetSize(); j++)
    {
    int SpId=Reactant[j].SpcId;
    double Ml=Moles[SpId]+DMoles[SpId];
    double StMoles=Reactant[j].NoOfMoles;
    Ml += (-dProdMoles+dMlXfer)*StMoles;
    Mdl.VValue[SpId]=Ml*SDB[SpId].MoleWt();
    }
  for (int k=0; k<Product.GetSize(); k++)
    {
    int SpId=Product[k].SpcId;
    double Ml=Moles[SpId]+DMoles[SpId];
    double StMoles=Product[k].NoOfMoles;
    Ml += (dProdMoles-dMlXfer)*StMoles;
    Mdl.VValue[SpId]=Ml*SDB[SpId].MoleWt();
    }
  Mdl.SetTemp(T);
  Mdl.SetPress(P);
  }

//---------------------------------------------------------------------------
#if DOPBSMOOTHING
void CR_Eqn::GetMsAGData(CMSJoin & AG)
  {
  double MassXfer=0.0;
  for (int r=0; r<NProducts(); r++)
    {
    CR_EqnTerm &P=Product[r];
    MassXfer+=P.dMlXfer*SDB[P.SpcId].MoleWt();
    }
  int NPs, NRs;
  CR_EqnTerm *Ps, *Rs;
  if (MassXfer>=0.0)
    {
    // Reacts>>Prods
    MassXfer=GTZ(MassXfer);
    NPs=NProducts();
    NRs=NReactants();
    Ps=&Product[0];
    Rs=&Reactant[0];
    }
  else
    {
    // Prods >> Reacts
    MassXfer=GTZ(fabs(MassXfer));
    NRs=NProducts();
    NPs=NReactants();
    Rs=&Product[0];
    Ps=&Reactant[0];
    }

  for (int p=0; p<NPs; p++)
    {
    CR_EqnTerm &P=Ps[p];
    int PSn=P.SpcId;
    double XP=fabs(P.dMlXfer)*SDB[PSn].MoleWt();
    for (int r=0; r<NRs; r++)
      {
      CR_EqnTerm &R=Rs[r];
      int RSn=R.SpcId;
      double XR=fabs(R.dMlXfer)*SDB[RSn].MoleWt();
      AG.m_Gamma[PSn][RSn] += XR*XP/MassXfer;///R.NoOfMoles;
      }
    }
//    }
//  else
//    {
//    }

  };
#endif
//--------------------------------------------------------------------------

flag CR_Eqn::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tReaction : No Extent control";                return 1;
    case  2: pS="E\tReaction : Not Balanced";                     return 1;
    case  3: pS="E\tReaction : Direction Limited Reaction";       return 1;
    case  4: pS="E\tReaction : Unbounded Problem";                return 1;
    case  5: pS="E\tReaction : No Solution found";                return 1;
    case  6: pS="E\tReaction : Not converged";                    return 1;
    case  7: pS="E\tReaction : Specie Overflow Moles > 1.0e6/s";  return 1;
    case  8: pS="E\tReaction : Extent not achieved";              return 1;
    default:
      return CReaction::CIStrng(No, pS);
    }
  };

//===========================================================================
//
//
//
//===========================================================================
