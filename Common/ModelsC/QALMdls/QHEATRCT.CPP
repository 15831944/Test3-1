//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
//#include "flwnode.h"
#define  __QHEATRCT_CPP
#include "qheatrct.h"

//===========================================================================
//
//
//
//===========================================================================

static MInitialiseTest InitTest("QAL:Reactions");
SPECIEBLK_S(InitTest, Boehmite,    "AlO.OH(s)",   true);
SPECIEBLK_S(InitTest, BoehmiteX,   "AlO.OH*(s)",  true);
SPECIEBLK_S(InitTest, Gibbsite,    "Al[OH]3(s)",  true);
SPECIEBLK_S(InitTest, Alumina,     "Al2O3(s)",    false);

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_CLASSBUILDER1(QGibbsiteHOR, CR_EqnHeat, "GibbsiteHOR", pCR_Eqn)

//---------------------------------------------------------------------------

double QGibbsiteHOR::totDHf(SpModel *Mdl, double T, double P)
  {
  CR_EqnTerm *pGibbTrm=NULL;
  for (int l=0; l<m_Eq.NReactants(); l++)
    if (m_Eq.Reactant[l].SpcId==Gibbsite.SolPhInx())
      {
      pGibbTrm=&m_Eq.Reactant[l];
      break;
      }

  if (!pGibbTrm)
    {//check RHS...
    for (int l=0; l<m_Eq.NProducts(); l++)
      if (m_Eq.Product[l].SpcId==Gibbsite.SolPhInx())
        {
        pGibbTrm=&m_Eq.Product[l];
        break;
        }
    }

  if (pGibbTrm)
    {
    double Ht = 631.64 + 1.3949 * K_2_C(m_RB.EstFinalT());
    double msGibbs = pGibbTrm->dMlXfer * Gibbsite.MW;
    double msAlumina = msGibbs * Alumina.MW / (2.0*Gibbsite.MW);
    return -Ht * msAlumina;
    }

  return CR_EqnHeat::totDHf(Mdl, T, P);
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_CLASSBUILDER1(QBoehmiteHOR, CR_EqnHeat, "BoehmiteHOR", pCR_Eqn)

//---------------------------------------------------------------------------

double QBoehmiteHOR::totDHf(SpModel *Mdl, double T, double P)
  {
  CR_EqnTerm *pBoehTrm=NULL;
  for (int l=0; l<m_Eq.NReactants(); l++)
    if (m_Eq.Reactant[l].SpcId==Boehmite.SolPhInx() || m_Eq.Reactant[l].SpcId==BoehmiteX.SolPhInx())
      {
      pBoehTrm=&m_Eq.Reactant[l];
      break;
      }

  if (!pBoehTrm)
    {//check RHS...
    for (int l=0; l<m_Eq.NProducts(); l++)
      if (m_Eq.Product[l].SpcId==Boehmite.SolPhInx() || m_Eq.Product[l].SpcId==BoehmiteX.SolPhInx())
        {
        pBoehTrm=&m_Eq.Product[l];
        break;
        }
    }

  if (pBoehTrm)
    {
    double Ht = 170.4738 + 1.0212 * K_2_C(m_RB.EstFinalT());
    double msBoehm = pBoehTrm->dMlXfer * Boehmite.MW;
    double msAlumina = msBoehm * Alumina.MW / (2.0*Boehmite.MW);
    return -Ht * msAlumina;
    }

  return CR_EqnHeat::totDHf(Mdl,T,P);
  }

//===========================================================================
//
//
//
//===========================================================================
/*
IMPLEMENT_CLASSBUILDER1(CEC_GibbACEq, CR_EqnControl, "GibbsiteACEq", pCR_Eqn)

void CEC_GibbACEq::Clear()
  {
  dRqdExtent=0.0;
  dACEq=0.0;
  };

//---------------------------------------------------------------------------

void CEC_GibbACEq::Parse(CRCTTokenFile &TF)
  {
  Clear();
  TF.CheckToken("=");
  dRqdExtent=TF.DoubleToken();
  if (TF.TokenIs("%"))
    dRqdExtent *= 0.01;
  dRqdExtent=Range(0.0, dRqdExtent, 1.0);
  };

//---------------------------------------------------------------------------

void CEC_GibbACEq::SetUp()
  {
  };

//---------------------------------------------------------------------------

void CEC_GibbACEq::BuildDataDefn(DataDefnBlk & DDB)
  {
  Strng T;
  T.Set("Gibbsite A/C Eq Extent");

  DDB.Text  (T());
  DDB.Double("A/CEq",    "",        DC_,     "",      &dACEq,                &Eq, 0);
  //if (fDoRT)
  //  {
  //  DDB.Double("Frac_Rt",        "",        DC_,     "",      &dFracRt,                &Eq, 0);
  //  DDB.Double("Defined_RT",     "",        DC_Time, "s",  &dDefinedResTime,           &Eq, isParm);
  //  DDB.Double("Actual_RT",      "",        DC_Time, "s",     &dActualResTime,         &Eq, isParm);
  //  DDB.Double("Alpha",          "",        DC_,     "",      &dAlpha,                 &Eq, 0);
  //  }
  if (iXSpcId>=0)
    {
    CCnvIndex dc;
    pchar pCnvTxt;
    SDB.AddSpCnv(DC_Frac, SDB[iXSpcId].SymOrTag(), "%", dc, pCnvTxt);
    }
  };

//---------------------------------------------------------------------------

flag CEC_GibbACEq::DataXchg(DataChangeBlk & DCB)
  {
  if (ddExtentRqd.DataXchg(DCB, xid_RCTExtent, &Eq))
    return 1;
  return 0;
  };

//---------------------------------------------------------------------------

flag CEC_GibbACEq::VerifyData()
  {
  return True;
  };

//---------------------------------------------------------------------------

void CEC_GibbACEq::CalcKs(double ProdMoles, double &EqK, double &K);
  {
  m_RB.ModelOwner().ChangeModel(&SMBayerClass);
  SMBayer &Mdl=*(SMBayer*)m_RB.Model();

  Eq.AdjustModel(dProdMoles, m_RB.EstFinalT(), Mdl.Press());

  double Tc=GTZ(K_2_C(Mdl.Temp()));
  double C=Mdl.SpecieConc(Mdl.Temp(), CausticSoda.li(), som_ALL);
  dACEq=GEZ(log(Tc)*(0.66433-0.00048*C)+0.00335*C-2.72616);

  EqK=dACEq;
  K=Mdl.AtoC();
  Extent=dRqdExtent*EquilK_DynamicScale();
  }

//===========================================================================
//
//
//
//===========================================================================

IMPLEMENT_CLASSBUILDER1(CEC_BoehmACEq, CR_EqnControl, "BoehmiteACEq", pCR_Eqn)

void CEC_BoehmACEq::Clear()
  {
  dRqdExtent=1.0;
  dACEq=0.0;

  dK0=6e8;
  dPercM45=0.2;
  dResTime=300.0;
  dVLiq=0.0;
  dSSA=0.0; 
  dTSA=0.0; 
  dMSolids=0.0; 

  dACOut=0.0;
  dTRct=Std_T;
  };

//---------------------------------------------------------------------------

void CEC_BoehmACEq::Parse(CRCTTokenFile &TF)
  {
  Clear();
  TF.CheckToken("=");
  dRqdExtent=TF.DoubleToken();
  if (TF.TokenIs("%"))
    dRqdExtent *= 0.01;
  dRqdExtent=Range(0.0, dRqdExtent, 1.0);
  while (TF.TokenIs(","))
    {
    if (TF.TokenIs("K0"))
      {
      TF.CheckToken("=");
      dK0=TF.DoubleToken();
      }
    else if (TF.TokenIs("ResTime"))
      {
      TF.CheckToken("=");
      dResTime=TF.DoubleToken()*3600.0;
      }
    }
  };

//---------------------------------------------------------------------------

void CEC_BoehmACEq::SetUp()
  {
  };

//---------------------------------------------------------------------------

void CEC_BoehmACEq::BuildDataDefn(DataDefnBlk & DDB)
  {
  Strng T;
  T.Set("Gibbsite Reversion");

  DDB.Text  (T());
  DDB.Double("A/CEq",            "",DC_,     "",      &dACEq      , &Eq, 0);
  DDB.Double("K0",	             "",DC_,     "",      &dK0        , &Eq,	0);
  DDB.Double("PercM45",	         "",DC_Frac, "%",     &dPercM45   , &Eq,	0);
  DDB.Double("ResTime",	         "",DC_Time, "hr",    &dResTime   , &Eq,	0);
  
  DDB.Double("TotSeedSurfArea",  "",DC_TSA,  "",      &dTSA       , &Eq,	0);
  DDB.Double("MSolids",          "",DC_Qm,   "",      &dMSolids   , &Eq, 0);

  DDB.Double("TRct",             "",DC_T,    "C",     &dTRct      , &Eq, 0);
  
  //if (fDoRT)
  //  {
  //  DDB.Double("Frac_Rt",        "",        DC_,     "",      &dFracRt,                &Eq, 0);
  //  DDB.Double("Defined_RT",     "",        DC_Time, "secs",  &dDefinedResTime,        &Eq, isParm);
  //  DDB.Double("Actual_RT",      "",        DC_Time, "secs",  &dActualResTime,         &Eq, isParm);
  //  DDB.Double("Alpha",          "",        DC_,     "",      &dAlpha,                 &Eq, 0);
  //  }
  if (iXSpcId>=0)
    {
    CCnvIndex dc;
    pchar pCnvTxt;
    SDB.AddSpCnv(DC_Frac, SDB[iXSpcId].SymOrTag(), "%", dc, pCnvTxt);
    }
  };

//---------------------------------------------------------------------------

flag CEC_BoehmACEq::DataXchg(DataChangeBlk & DCB)
  {
  if (ddExtentRqd.DataXchg(DCB, xid_RCTExtent, &Eq))
    return 1;
  return 0;
  };

//---------------------------------------------------------------------------

flag CEC_BoehmACEq::VerifyData()
  {
  return True;
  };

//---------------------------------------------------------------------------
flag DoCsv=0;
void CEC_BoehmACEq::CalcKs(double ProdMoles, double &EqK, double &K);
  {
  if (DoCsv)
    {
    DoCsv=0;
    for (double d=-1.0; d<1.0; d+=0.05)
      {
      double EqK, K, Extent=1.0;
      CalcKs(d, EqK, K, Extent);
      dbgpln("%g,%g,%g,%g",d, EqK, K, Extent);
      }
    }


  m_RB.ModelOwner().ChangeModel(&SMBayerClass);
  SMBayer &Mdl=*(SMBayer*)m_RB.Model();

  Eq.AdjustModel(dProdMoles, m_RB.EstFinalT(), Mdl.Press());

  double Tk=Mdl.Temp();
  double Tc=GTZ(K_2_C(Tk));

  double a=0.11992 + 0.00266125*Tc;
  double b=1.0+0.0869*Mdl.TotalNaOnCS();
  double c=0.0;//(10.22+0.02975*Tc)/Mdl.CausticConc();
  double d=0.0;//0.05785*();
  dACEq=GEZ(a*b-c+d);
  
  if (dProdMoles<0.0)
    {
    double ActivationEnergy=7600.0;
    dMSolids=Mdl.M[Boehmite.SolPhInx()]*3600.0/1000.0;  // t/h
  
    // From BCPREC6.XLS
    dVLiq=Mdl.Volume(som_Liq)*3600.;      // kL/h
    dSSA=(0.0298+0.00077*dPercM45*100.0);  // m^2/g
    dTSA=dSSA*dMSolids*1000./GTZ(dVLiq);     // m^2/L
  
    // MJM 24/6/94 Use volume leaving to calculate Tau  which must be > 0.0
    double C=Mdl.CausticConc(Mdl.Temp()); 
    double CpS=Mdl.CtoS();       
    double ClpC=Mdl.CltoC();     
    double TOOCpC=Mdl.TOOCtoC(); 
    dResTime=Max(1.0, dResTime);

    double K1 = dK0*exp(-ActivationEnergy/Tk)*dTSA*GEZ(1.0295-0.0022*C+0.8109*TOOCpC-0.0043*TOOCpC*C);

    double dACIn=Mdl.AtoC();
    double dACOut=dACIn;

    double Tau=Max(1.0e-10, K1*dResTime/3600.0);
    if ((dACIn-dACEq)>1.0e-6)
      {
      dACOut=dACEq+(-1.0+Sqrt(1.0+4.0*Tau*(dACIn-dACEq)))/(2.0*Tau);
      Extent=(dACIn-dACOut)/(dACIn-dACEq);
      }
    else 
      {
      dACOut=dACIn;
      Extent=0.0;
      }
    }
  else
    Extent=1.0;

  EqK=dACEq;
  K=Mdl.AtoC();
  Extent*=dRqdExtent*EquilK_DynamicScale();

  dTRct=Tk;
  }
*/
//===========================================================================
//
//
//
//===========================================================================
