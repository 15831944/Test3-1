//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __CSKOGasSep_CPP
#include "GasSep.h"
#include "dbgmngr.h"

#define dbgModels  0
#if dbgModels
static CDbgMngr dbgCSKOGasSep("Models", "GasSep");
#endif

//===========================================================================
//
//                                 CSKOGasSep
//
//==========================================================================

const byte idFeed     = 0;
const byte idRich     = 1;
const byte idStrip    = 2;
const byte idRec      = 3;

XID xid_GasCount      = MdlBsXID(0);

static IOAreaRec CSKOGasSepIOAreaList[] =
  {{"Feed Gas",      "Z" , idFeed,   LIO_In ,  nc_MLnk, 1, 1,            0, 0.0F},
  {"Enriched Gas",  "Y" , idRich,   LIO_Out,  nc_MLnk, 1, 1,            0, 0.0F},
  {"Stripped Gas",  "X" , idStrip,  LIO_Out,  nc_MLnk, 1, 1,            0, 0.0F}, //Out.Out"},
  {"Recycle Gas",   "R" , idRec,    LIO_Out,  nc_MLnk, 1, 1,            0, 0.0F}, 
  {NULL}};

static double Drw_CSKOGasSep[] = { DD_Poly, -6,10, 10,0, -6,-10, -6,10, DD_End };

//--------------------------------------------------------------------------

CArray<long, long&> CSKOGasSep::sm_GasId;
CArray<double, double&> CSKOGasSep::sm_S;
CArray<double, double&> CSKOGasSep::sm_Wt;

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(CSKOGasSep, "SKOGasSep", "1", Drw_CSKOGasSep, "MixerSplitter", "GS", TOC_ALL|TOC_SYSTEM,
                    "SKOGasSep", "")
                    CSKOGasSep::CSKOGasSep(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
MN_Xfer(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Process, CSKOGasSepIOAreaList);
  m_bDataOK=false;

  //m_lSpCarrier=-1;
  sm_GasId.SetSize(1);
  sm_GasId[0]=-1;

  m_dK_In=100;
  m_dK_L=4;
  m_dK_H=6;
  m_dK_Rec=3;


  m_dAbundFeed=0;
  m_dAbundRich=0;
  m_dAbundStrip=0;

  m_dTheta=0.5;
  m_dPhi=0.5;
  m_dAlpha=0;
  m_dBeta=1;
  m_dGamma=0;

  m_eBetaEqn=BE_Name0;
  m_dExponent=0.85;
  m_dMultiplier=2.2;
  m_dPRatio=0;

  m_ThetaLight=0;
  m_ThetaHeavy=0;

  };

//--------------------------------------------------------------------------

CSKOGasSep::~CSKOGasSep ()
  {
  };

//--------------------------------------------------------------------------

void CSKOGasSep::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Visibility(SM_DynBoth|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Visibility();

  DDB.Text   ("Sep Gases");
  DDB.Long   ("",             "NoGasses",  DC_,     "",   xid_GasCount,     this, isParmStopped|SetOnChange);
  //DDB.Long   ("",             "SpCarrier",  DC_,     "",   &m_lSpCarrier,     this, isParm, SDB.DDBSpListDash());
  for (int i=0; i<sm_GasId.GetSize(); i++)
    {
    Strng S;
    S.Set("Gas%i",i);
    DDB.Long   ("",             S(),   DC_,     "",   &sm_GasId[i],    this, isParm|SetOnChange, SDB.DDBSpListDash());
    }

  DDB.Text   ("");
  DDB.Text   ("Calc Types");
  static DDBValueLst DDBBetaCalcs[]={
    {BE_Name0, "Standard"},
    {BE_Name1, "Pi_Po_Ratio"},
    {0}};
    DDB.Long   ("",             "BetaCalcs",  DC_,     "", (long*)&m_eBetaEqn, this, isParm, DDBBetaCalcs);
    DDB.Double ("",             "Beta_Exp",   DC_,    "", &m_dExponent,       this, isParm);
    DDB.Double ("",             "Beta_Mult",  DC_,    "", &m_dMultiplier,     this, isParm);
    DDB.Double ("",             "PRatio",     DC_,    "", &m_dPRatio,         this, 0);
    DDB.Text   ("");
    DDB.Text   ("Betas");
    for (int i=0; i<m_Beta.GetSize(); i++)
      {
      Strng S;
      //int j = i+1;
      //pchar SStrt = "Beta%i%_%j";
      S.Set("Beta%i_%i",i,i+1);
      DDB.Double("",             S(),    DC_,     "",   &m_Beta[i],    this, 0);
      }

    DDB.Text   ("Flow_Config");
    DDB.Double ("",            "K_Inlet",  DC_L,    "",   &m_dK_In,       this, isParm);
    DDB.Double ("",            "K_Recycle",DC_L,    "",   &m_dK_Rec,      this, isParm);
    DDB.Double ("",            "K_Lite",   DC_L,    "",   &m_dK_L,        this, isParm);
    DDB.Double ("",            "K_Heavy",  DC_L,    "",   &m_dK_H,        this, isParm);
    DDB.Text   ("");
    DDB.Double ("",            "ThetaLight",  DC_Frac,    "%",   &m_ThetaLight,  this, 0);
    DDB.Double ("",            "ThetaHeavy",  DC_Frac,    "%",   &m_ThetaHeavy,  this, 0);

    //enums          { BE_Name0, BE_Name1 }; // KCG to change

    //  DDB.Double ("IsoCut",       "Theta",   DC_Frac, "%",  &m_dTheta,      this, isParm);
    //  DDB.Double ("",             "Beta",    DC_,     "",   &m_dBeta,       this, isParm);
    //  DDB.Double ("",             "Alpha",   DC_,     "",   &m_dAlpha,      this, 0);
    //  DDB.Double ("",             "Gamma",   DC_,     "",   &m_dGamma,      this, 0);
    //  DDB.Text   ("Carrier");
    //  DDB.Double ("CarrierCut",   "Phi",     DC_Frac, "%",  &m_dPhi,        this, isParm);

    DDB.Text   ("");
    //DDB.Text   ("Enriched Fractions");
    if (DDB.BeginStruct(this, "RichFrac"))
      {
      for (int i=1; i<m_RichFrac.GetSize(); i++)
        if (sm_GasId[i]>=0)
          DDB.Double ("", SDB[sm_GasId[i]].SymOrTag(),   DC_Frac,     "%",   &m_RichFrac[i],    this, 0);
      }
    DDB.EndStruct();

    //DDB.Text   ("");
    //DDB.Double ("FeedAbundance",  "",      DC_,     "",   &m_dAbundFeed,  this, 0);
    //DDB.Double ("RichAbundance",  "",      DC_,     "",   &m_dAbundRich,  this, 0);
    //DDB.Double ("StripAbundance", "",      DC_,     "",   &m_dAbundStrip, this, 0);

    DDB.Text   ("");
    BuildDataDefnShowIOs(DDB);

    DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag CSKOGasSep::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Xfer::DataXchg(DCB))
    return 1;
  switch (DCB.lHandle)
    {
    case xid_GasCount:
      if (DCB.rL)
        {
        int newn=Max(2L,*DCB.rL);
        int n=sm_GasId.GetSize();
        sm_GasId.SetSize(newn);
        sm_S.SetSize(newn);
        sm_Wt.SetSize(newn);
        m_Beta.SetSize(newn-1);
        m_Y.SetSize(newn);
        m_J.SetSize(newn);
        m_RichFrac.SetSize(newn);
        for (int i=n; i<sm_GasId.GetCount(); i++)
          {
          sm_GasId[i]   = -1;
          sm_S[i]       = 1;
          sm_Wt[i]      = 1;
          m_RichFrac[i] = 0;
          }

        for (int i=n; i<m_Beta.GetCount(); i++)
          {
          m_Beta[i]=1;
          m_J[i]=1;
          }
        }
      DCB.L=sm_GasId.GetSize();
      return 1;
    }
  return 0;
  }

// --------------------------------------------------------------------------

flag CSKOGasSep::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MN_Xfer::ValidateData(VDB);

  m_bDataOK = true;
  for (int i=0; i<sm_GasId.GetSize(); i++)
    m_bDataOK = m_bDataOK && (sm_GasId[i]>=0 && sm_GasId[i]<SDB.Count() && SDB[sm_GasId[i]].IsGas());

  if (m_bDataOK)
    {
    int i=1;
    while (i<sm_GasId.GetSize())
      {
      if (SDB[sm_GasId[i-1]].MoleWt() > SDB[sm_GasId[i]].MoleWt())
        {
        Exchange(sm_GasId[i-1], sm_GasId[i]);
        if (i>1)
          i--;
        }
      else
        i++;
      }

    for (int s=0; s<sm_GasId.GetSize(); s++)
      {
      sm_S[s]  = SDB[sm_GasId[s]].MolecularDiam()*1.0e10;
      sm_Wt[s] = SDB[sm_GasId[s]].MoleWt();
      }
    }
  m_dBeta   = ValidateRange(VDB, "Beta",  0.01, m_dBeta,  100.0);
  m_dTheta  = ValidateRange(VDB, "Theta", 0.01, m_dTheta, 0.99);
  m_dPhi    = ValidateRange(VDB, "Phi",   0.01, m_dPhi,   0.99);

  SetCI(1, !m_bDataOK);
  return OK;
  }

// -------------------------------------------------------------------------

flag CSKOGasSep::ChangeMode(dword NewMode, dword OldMode)
  {
  //  if (NewMode==SM_Direct)
  //    GSM.Open();

  return True;
  };

//---------------------------------------------------------------------------

void CSKOGasSep::ConfigureJoins()
  {
  MN_Xfer::ConfigureJoins();
  //  Init_NJoins(1);
  //  int i;
  //  switch (NetMethod())
  //    {
  //    case SM_Direct:
  //    //case SSMODE:
  //      for (i=0; (i<NoFlwIOs()); i++)
  //        SetIO_Join(i, 0, 0);
  //      break;
  //    case SM_Inline:
  //case SM_Buffered:
  //case DYNMODE:
  //      for (i=0; (i<NoFlwIOs()); i++)
  //          {
  //          SetIO_Join(i, 0, 0);
  ////          if (IOId_Self(i)==idRich)
  ////            SetJoinQm_QmRatio (0, i);
  //          }
  //
  //      break;
  //    }
  };

//--------------------------------------------------------------------------

void CSKOGasSep::EvalJoinPressures(long JoinMask)
  {
  MN_Xfer::EvalJoinPressures(JoinMask);
  //for (int i = 0; i < NoFlwIOs(); i++)
  //  Set_IOP_Self(i,Std_P);
  };

//--------------------------------------------------------------------------

flag CSKOGasSep::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  switch (Task)
    {
    case FET_GetMode:
      {
      FlwBlk * pFB = IOFB(IONo, FE);
      pFB->SetQmFree();
      return true;
      }
    case FET_CalcDP:
      {
      FlwBlk * pFB = IOFB(IONo, FE);
      pFB->ClrDerivsBad();
      pFB->SetDPb(0.0, 0.0); // In is Positive

      SpConduit & QFeed = *IOConduit(IONo);

      double Q=GEZ(fabs(pFB->GetQm()));
      if (Q>1.0e-12)
        {
        double DQ=Max(1.0e-8, 1.0e-2*Q);

        //Meth[iMethod]->Init(Q, QFeed);

        static double XScl=1.0;
        double Y[4]={m_dK_In, m_dK_L,m_dK_H,m_dK_Rec};

        double X=XScl*Y[IOId_Self(IONo)];


        double FlowLoss0=-pFB->QmSign()*X*Q;//Meth[iMethod]->DP(Q, QFeed);
        double FlowLoss1=-pFB->QmSign()*X*(Q+DQ);//Meth[iMethod]->DP(Q+DQ, QFeed);
        double DFlowLoss=(FlowLoss1-FlowLoss0)/DQ;

        pFB->SetDPq(FlowLoss0, DFlowLoss);
        }
      else
        pFB->SetDPq(0.0, 0.0);
      return true;
      }
    }
  return MN_Xfer::EvalFlowEquations(Task, pProps, IONo, FE, LnkNo);
  };

//--------------------------------------------------------------------------

void CSKOGasSep::EvalProducts(long JoinMask)
  {
  if (GSM.Enabled())
    {
    if (NJoins()>0)
      switch (SolveMethod())
      {
        case SM_Direct:
          if (NJoins()>=1)
            Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, NULL, NULL, GSM(), NULL);
          break;
        case SM_Inline:
        case SM_Buffered:
          for (int j=0; j<NJoins(); j++)
            Xfer_EvalProducts(j, Joins[j].Pressure(), NULL, NULL, NULL, GSM(), NULL);
          break;
      }
    }
  else if (m_bDataOK && IO_In(idFeed) && IO_Out(idStrip) && IO_Out(idRich) && IO_Out(idRec))
    {
    ///SigmaQInPMin(m_QFeed, som_ALL, Id_2_Mask(idFeed));

    const int iFeed  = IOWithId_Self(idFeed);
    const int iStrip = IOWithId_Self(idStrip);
    const int iRich  = IOWithId_Self(idRich);
    const int iRec   = IOWithId_Self(idRec);
    SpConduit &QFeed  =*IOConduit(iFeed);
    SpConduit &QRich  =*IOConduit(iRich);
    SpConduit &QStrip =*IOConduit(iStrip);
    SpConduit &QRec   =*IOConduit(iRec);

    if (QFeed.MoleWt() > 1e-10)
      {
      double  QmFeedEst  = IOQmEst_In(iFeed);
      double  QmRichEst  = IOQmEst_Out(iRich);
      double  QmStripEst = IOQmEst_Out(iStrip);
      double  QmRecEst   = IOQmEst_Out(iRec);

      //check for valid feed...

      const double QmFd = QFeed.QMass();
      QRich.QZero();
      QStrip.QZero();
      QRich.SelectModel(&QFeed, true);
      QStrip.SelectModel(&QFeed, true);
      double  Pin_Feed = QFeed.Press();
      double  Pout_Lite = QRich.Press();
      m_dPRatio = 3.0;
      if (Pout_Lite > 1.0)
        m_dPRatio = Range(2.0,Pin_Feed/Pout_Lite,6.0);

      double MMInlet = GTZ(QFeed.MoleWt());
      int ij = m_Beta.GetCount();
      for (int i=0; i<ij; i++)
        {
        switch (m_eBetaEqn)
          {
          case BE_Name0:
            {
            double A=Pow((sm_Wt[i+1]/sm_S[i+1]) - (sm_Wt[i]/sm_S[i]), 0.85);
            double B= Pow((sm_S[i+1]+sm_S[i])/2.0, 0.85);
            m_Beta[i]= exp(( A * B * 0.22) / MMInlet);
            break;
            }
          case BE_Name1:
            {
            double A=Pow((sm_Wt[i+1]/sm_S[i+1]) - (sm_Wt[i]/sm_S[i]), m_dExponent);
            double B= Pow((sm_S[i+1]+sm_S[i])/2.0, m_dExponent);
            m_Beta[i]= Pow(m_dPRatio,(( A * B * m_dMultiplier) / MMInlet));
            break;
            }
          }
        }

      //        double MMM = QFeed.VMole[sm_GasId[i]]/QFeed.Moles();      
      int Last=m_J.GetCount()-1;
      for (int i=0; i<Last; i++)
        {
        m_J[i]=QFeed.VMole[sm_GasId[i]]/GTZ(QFeed.VMole[sm_GasId[Last]]);
        for (int j=i; j<Last; j++)
          m_J[i]*=m_Beta[j];
        }

      double SigmaJ=0.0;
      for (int i=0; i<Last; i++)
        SigmaJ+=m_J[i];

      m_Y[Last]=1.0/(1.0+SigmaJ);

      double LastY = 1.0/(1.0+SigmaJ);

      double yFrac = 0.0;
      int ik = m_Y.GetCount()-1;
      for (int i=0; i<ik; i++)
        {
        m_Y[i]=m_J[i]*LastY;
        yFrac = yFrac + m_Y[i];
        }
      m_Y[i]=LastY;
      yFrac = yFrac + m_Y[i];

      double MYMoles = 0.0;
      for (int i=0; i<=ik; i++)
        {
        MYMoles = MYMoles + sm_Wt[i]* m_Y[i];
        }

      QmRecEst=Range(0.0, QmRecEst, QFeed.QMass()); // ensure mass balance
      QRec.QSetM(QFeed,som_ALL,QmRecEst);

      //dbgpln("--------------------------------");
      double SilaneFeed=0.0;
      double SilaneRich=0.0;
      double SilaneStrip=0.0;
      for (int i=0; i<=ik; i++)
        {
        double SpcInFeed=QFeed.VMass[sm_GasId[i]];
        double Spc2Rec=QRec.VMass[sm_GasId[i]];
        double Spc2Rich=QmRichEst*m_Y[i]*sm_Wt[i]/MYMoles;
        double Spc2Strip=GEZ(SpcInFeed-Spc2Rich-Spc2Rec);

        if (Spc2Rec<0 || Spc2Rich<0 || Spc2Strip<0)
          { int xxx=0; };

        //dbgpln(" %i %20s %20s %20s %20s %20s", i, DbgFltString(SpcInFeed), DbgFltString(Spc2Rec), DbgFltString(Spc2Rich), DbgFltString(Spc2Strip), DbgFltString(Range(0.0, Spc2Rich, SpcInFeed)));
        Spc2Rich=Range(0.0, Spc2Rich, SpcInFeed);
        QRich.VValue[sm_GasId[i]]=Spc2Rich;
        QStrip.VValue[sm_GasId[i]]=Spc2Strip;
        if (i>0) // ? Skip the carrier
          {
          SilaneFeed  += SpcInFeed;
          SilaneRich  += Spc2Rich;
          SilaneStrip += Spc2Strip;
          }
        }

      QRich.SetTempPress(QFeed.Temp(), QFeed.Press());
      QStrip.SetTempPress(QFeed.Temp(), QFeed.Press());
      QRec.SetTempPress(QFeed.Temp(), QFeed.Press());
      //      m_dAbundRich=AbundanceRatio(QRich);
      //      m_dAbundStrip=AbundanceRatio(QStrip);

      m_ThetaLight=SilaneRich/GTZ(SilaneFeed);
      m_ThetaHeavy=SilaneStrip/GTZ(SilaneFeed);

      double TotSepMass=0;
      for (int i=1; i<=ik; i++) // ? Skip the carrier
        TotSepMass+=QRich.VMass[sm_GasId[i]];

      m_RichFrac[0]=0.0;
      for (int i=1; i<=ik; i++) // ? Skip the carrier
        m_RichFrac[i]=QRich.VMass[sm_GasId[i]]/GTZ(TotSepMass);

      }
    else
      {
      MN_Xfer::EvalProducts(JoinMask);
      return;
      }
    }
  else
    {
    MN_Xfer::EvalProducts(JoinMask);
    //Xfer_EvalProducts(0, Joins[0].Pressure(), &QFeed, NULL, NULL, NULL, NULL);
    }
  }

//--------------------------------------------------------------------------

dword CSKOGasSep::ModelStatus()
  {
  dword Status=MN_Xfer::ModelStatus();
  if (NoFlwIOs())
    {
    int HasFlw=0;
    for (int i=0; i<NoFlwIOs(); i++)
      {
      if (IOConduit(i)->QMass()>UsableMass)
        HasFlw=1;
      }
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    }
  return Status;
  }

//--------------------------------------------------------------------------

flag CSKOGasSep::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tIncomplete Configuration"; return 1;
    case  2: pS="W\tRequired Conditions not met"; return 1;
    default:
      return MN_Xfer::CIStrng(No, pS);
    }
  };

//--------------------------------------------------------------------------
//===========================================================================

