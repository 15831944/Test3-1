//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "sc_defs.h"
#define  __M_LINK_CPP
#include "m_link.h"
#include "dbgmngr.h"

//#include "optoff.h"

#define dbgMLink        (0 || WITHDEBUG)    

#if dbgMLink
static CDbgMngr dbgEvalJoinPress   ("ProBal", "EvalJoinPress");
//static CDbgMngr dbgDumpChanges     ("ProBal", "DumpChanges");
//static CDbgMngr dbgDumpChangesIn   ("ProBal", "DumpChangesIn");
static CDbgMngr dbgVol_FlwFX          ("Models",      "Vol_FlwFX");
#endif

//============================================================================
//
//
//
//============================================================================


void CPlugFlowInfo::SetMethod(byte Meth)//, double Length, double XArea, double Rise)
  {
  m_iMethod=Meth;

  switch (m_iMethod)
    {
    case VFFX_PreMix:
    case VFFX_PostMix:    Allocate(true, false); break;
    case VFFX_Plugflow:   Allocate(false, true); break;
    default:              Allocate(false, false); break;
    }

  }

// --------------------------------------------------------------------------

void CPlugFlowInfo::SetType(byte Type)
  {
  m_iType=Type;
  }

// --------------------------------------------------------------------------

void CPlugFlowInfo::SetSectionCount(long N)
  {
  m_nSections=Range(2L, N, 1000L);
  }

// --------------------------------------------------------------------------

void CPlugFlowInfo::InitialisePlugFlow(SpConduit & Fi, double FillFrac)
  {
  if (CnExists)
    {
    //    m_PFI.Cn.Tag(PrjFileVerNo()>=47 ? "Cn" : "Content");
    if (m_bInitReqd)
      {
      double Dens=Fi.Rho();
      double ShVol=Cn.ShapeVolume();
      double Mass=ShVol*Dens*FillFrac;

      Cn.SetM(Fi, som_ALL, Mass, Fi.Press());
      Cn.EvalHStates();
      }
    }

  if (PfExists)
    {
    // Should never gethere
    }

  m_bInitReqd=false;
  }

// --------------------------------------------------------------------------

void CPlugFlowInfo::InitialisePlugFlow(double Length, double XArea, double Rise, SpConduit & Fi, double FillFrac, double PIn, double POut, double VelIn, double VelOut)
  {
  m_dLength=Length;
  if (CnExists)
    {
    //    m_PFI.Cn.Tag(PrjFileVerNo()>=47 ? "Cn" : "Content");
    Cn.SetVolume(Max(0.1, Length*XArea));
    Cn.SetHeight(Max(0.01, Rise/*VertLength*/));
    if (m_bInitReqd)
      {
      double Dens=Fi.Rho();
      double ShVol=Cn.ShapeVolume();
      double Mass=ShVol*Dens*FillFrac;

      Cn.SetM(Fi, som_ALL, Mass, 0.5*(PIn+POut));
      Cn.EvalHStates();
      }
    }

  if (PfExists)
    {
    //    m_PFI.Cn.Tag(PrjFileVerNo()>=47 ? "Cn" : "Content");
    Pf.SetLength(Max(0.1, Length));
    Pf.SetVolume(Max(0.001, Pf.Length()*XArea));

    if (m_bInitReqd)
      {
      double Dens=Fi.Rho();
      double ShVol=Pf.Volume();
      double Mass=ShVol*Dens*FillFrac;

      Pf.Fill(Fi, som_ALL, FillFrac, PIn, POut, VelIn, VelOut);
      }
    }

  m_bInitReqd=false;
  }

// --------------------------------------------------------------------------

void CPlugFlowInfo::Allocate(bool CnRqd, bool PfRqd)
  {
  if (!m_pContents && CnRqd)
    {
    m_pContents=new SpContainer(PrjFileVerNo()>=47 ? "Cn":"Content", m_pFlwNd, TOA_Embedded);
    m_pContents->ChangeShape(&SpShPipeClass);
    m_pContents->SetClosed(true);
    m_pContents->SetPressAsSet(true);
    m_pContents->SetStateAction(IE_SaveState);
    m_pContents->dwDisplay = SPDF_ShapeNameDisplay|SPDF_ShapeData|SPDF_DynMode;
    m_pContents->m_bKeepLPTOnVChange=true;
    m_bInitReqd=true;
    }
  else if (CnExists && !CnRqd)
    {
    if (m_bOwner)
      delete m_pContents;
    m_pContents=NULL;
    m_bInitReqd=true;
    }
  if (!m_pPlugFlow && PfRqd)
    {
    m_pPlugFlow = new CSpPlugFlow("Pf", m_pFlwNd, TOA_Embedded);
    //m_pPlugFlow->ChangeShape(&SpShPipeClass);
    //m_pPlugFlow->SetClosed(true);
    //m_pPlugFlow->SetPressAsSet(true);
    //m_pPlugFlow->SetStateAction(IE_SaveState);
    //m_pPlugFlow->dwDisplay = SPDF_ShapeNameDisplay|SPDF_ShapeData|SPDF_DynMode;
    //m_pPlugFlow->m_bKeepLPTOnVChange=true;
    m_bInitReqd=true;
    }
  else if (m_pPlugFlow && !PfRqd)
    {
    if (m_bOwner)
      delete m_pPlugFlow;
    m_pPlugFlow=NULL;
    m_bInitReqd=true;
    }
  m_bOwner=true;
  }

void CPlugFlowInfo::Clear()
  {

  if (m_bOwner)
    {
    delete m_pContents;
    delete m_pPlugFlow;
    }
  m_pContents=NULL;
  m_pPlugFlow=NULL;
  }

void CPlugFlowInfo::SetData(SpContainer *pContents, CSpPlugFlow *pPlugFlow, bool Owner)
  {
  if (m_bOwner)
    Clear();
  m_pContents = pContents;
  m_pPlugFlow = pPlugFlow;
  m_bOwner    = Owner;
  };


// --------------------------------------------------------------------------

double CPlugFlowInfo::DoPlugFlowProducts(SpConduit & Fi, SpConduit & Fo, double QmIn, double QmOut, double PIn, double POut, double VelS, double VelE)
  {
  if (Method==VFFX_Off)
    return QmIn;

  #if dbgMLink
  double xM,xV,xT;
  if (CnExists && m_pFlwNd->DoDbgBrk() && dbgVol_FlwFX())
    {
    xM=Cn.Mass();
    xV=Cn.Volume();
    xT=Cn.Temp();
    };
  #endif

  byte M=Method;
  switch (M)
    {
    case VFFX_PreMix:
      {
      #if dbgMLink
      if (m_pFlwNd->DoDbgBrk() && dbgVol_FlwFX())
        {
        xM=Cn.Mass();
        xV=Cn.Volume();
        xT=Cn.Temp();
        };
      #endif

      Cn.SetHoldVentExpand();
      Cn.AddM(Fo, som_ALL, QmIn*ICGetTimeInc());
      Cn.ClrHoldVentExpand();

      double T=Cn.Temp();
      double Pm=0.5*(PIn+POut);
      double VIn=Fo.QVolume(som_ALL)*ICGetTimeInc();
      double VCn=Cn.Model()->Volume(som_ALL, T, Pm);
      double VSh=Cn.ShapeVolume();

      double Scl=VCn/GTZ(VSh);
      //double SclR=Range(1.0e-6, Scl, 1.0);
      double SclR=1.0/Max(1.0,Scl);
      //double SclR=1.0/Max(1.0,1+0.5*(Scl-1));

      Cn.SetHoldVentExpand();
      Cn.ScaleMass(som_ALL, SclR);
      Cn.SetTempPress(T,Pm);
      Cn.ClrHoldVentExpand();

      QmOut*=Min(Scl,1.0);
      Fo.QSetM(Cn, som_ALL, QmOut, Pm);
      #if dbgMLink
      if (m_pFlwNd->DoDbgBrk() && dbgVol_FlwFX())
        {
        dbgpln("PlugFlow Pre Qm:%12.5f VIn:%12.5f VCn:%12.5f VSh:%12.5f Scl:%12.5f    :%12s M:%12.5f[%12.5f] V:%12.5f[%12.5f] T:%12.5f[%12.5f] Fo: M:%12.5f T:%12.5f %s",
          QmOut, VIn, VCn, VSh, Scl, "", Cn.Mass(), Cn.Mass()-xM, Cn.Volume(), Cn.Volume()-xV, Cn.Temp(),Cn.Temp()-xT, Fo.QMass(), Fo.Temp(), m_pFlwNd->FullObjTag());
        };
      #endif
      }
      break;
    case VFFX_PostMix:
      {
      double Pm=0.5*(PIn+POut);
      double VIn=Fo.QVolume(som_ALL, Cn.Temp(), Cn.Press())*ICGetTimeInc();
      double VCn=Cn.Model()->Volume(som_ALL);//, Fo.Temp(), Fo.Press());
      double VSh=GTZ(Cn.ShapeVolume());
      double OVolCn=Range(0.0, VIn+VCn-VSh, VCn);
      double OVolFi=GEZ(VIn+VCn-VSh-OVolCn);
      double OVolFracCn=Range(0.0, GEZ(OVolCn)/GTZ(VSh), 1.0);
      double OVolFracFi=Range(0.0, GEZ(OVolFi)/GTZ(VIn), 1.0);

      StkSpConduit FoTmp("Tmp", chLINEID(), m_pFlwNd);
      FoTmp().QSetF(Cn, som_ALL, OVolFracCn/ICGetTimeInc(), Pm);
      FoTmp().QAddM(Fo, som_ALL, OVolFracFi*QmIn);

      Cn.SetHoldVentExpand();
      Cn.ScaleMass(som_ALL, 1.0-OVolFracCn);
      Cn.Temp();
      Cn.AddM(Fo, som_ALL, (1.0-OVolFracFi)*QmIn*ICGetTimeInc());
      Cn.ClrHoldVentExpand();

      Fo.QCopy(FoTmp());
      QmOut=Fo.QMass();
      #if dbgMLink
      if (m_pFlwNd->DoDbgBrk() && dbgVol_FlwFX())
        {
        dbgpln("PlugFlow Pst Qm:%12.5f VIn:%12.5f VCn:%12.5f VSh:%12.5f OCn:%12.5f OFi:%12.5f M:%12.5f[%12.5f] V:%12.5f[%12.5f] T:%12.5f[%12.5f] Fo: M:%12.5f T:%12.5f %s",
          QmOut, VIn, VCn, VSh, OVolFracCn, OVolFracFi, Cn.Mass(), Cn.Mass()-xM, Cn.Volume(), Cn.Volume()-xV, Cn.Temp(),Cn.Temp()-xT, Fo.QMass(), Fo.Temp(), m_pFlwNd->FullObjTag());
        };
      #endif
      }
      break;
    case VFFX_Plugflow:
      {
      m_pFi=&Fi;
      m_pFo=&Fo;
      m_dQmIn=QmIn;
      m_dQmOut=QmOut;
      m_dPIn=PIn;
      m_dPOut=POut;
      m_dVelS=VelS;
      m_dVelE=VelE;

      if (m_dQmIn<=0)
        Pf.SetOutput(0, -m_dQmIn, m_pFi);
      if (m_dQmOut>=0)
        Pf.SetOutput(1, m_dQmOut, m_pFo);

      #if dbgMLink
      if (m_pFlwNd->DoDbgBrk() && dbgVol_FlwFX())
        {
        //dbgpln("PlugFlow Dis Qm:%12.5f VIn:%12.5f VCn:%12.5f VSh:%12.5f OCn:%12.5f OFi:%12.5f M:%12.5f[%12.5f] V:%12.5f[%12.5f] T:%12.5f[%12.5f] Fo: M:%12.5f T:%12.5f %s",
        //  QmOut, VIn, VCn, VSh, OVolFracCn, OVolFracFi, Cn.Mass(), Cn.Mass()-xM,
        //  Cn.Volume(), Cn.Volume()-xV, Cn.Temp(),Cn.Temp()-xT, Fo.QMass(), Fo.Temp(), FullObjTag());
        };
      #endif
      }
      break;
    }

  return QmOut;
  }

// --------------------------------------------------------------------------

void CPlugFlowInfo::DoPlugFlowDiscrete(double TimeInc, bool Moved)
  {
  //ASSERT(get_Method()==VFFX_PlugFlow);

  if (m_dQmIn>0)
    Pf.GetInput(0, m_dQmIn, m_pFi);
  if (m_dQmOut<0)
    Pf.GetInput(1, -m_dQmOut, m_pFo);

  Pf.Advance(TimeInc, m_dVelS, m_dVelE, (Type==PFT_Incompressible), m_dPIn, m_dPOut, Moved);

  //switch (Type)
  //  {
  //  case PFT_Incompressible:
  //    break;
  //  case PFT_Compressible:
  //    break;
  //  }


    //m_Q.SetMaxSpillLoading(m_MaxSpillLoading);
  //int Feed=0;
  //for (int i=0; i<NoFlwIOs(); i++)
  //  if (IOId_Self(i)==idFeed)
  //    m_Q.SetFeed(Feed++, IOQm_In(i), IOConduit(i));

  //flag Moved=(m_fWasRunning && (m_Q.TotalMass()<m_MaxStallLoading || m_fStartIfStalled));
  //m_Q.DumpProfile(0, "Before", 0);
  //m_Q.Advance(ICGetTimeInc(), Moved);//, m_MaxVelocity, m_MaxVelocity*Speed);

  //m_fWasRunning = fabs(m_MSB.Speed(this))>0.0;

  //m_Power=(m_NLPower+(m_FLPower-m_NLPower)* m_Q.AverageLoading()/m_MaxSpillLoading)*Range(-1.0, m_MSB.Speed(this), 1.0);

  //m_Q.DumpProfile(1, "After", 0);
  //m_MSB.EvalDiscrete(this);


  };

// --------------------------------------------------------------------------

void CPlugFlowInfo::SetPlugFlowOut(SpConduit & Fo, /*SpContainer & Cn,*/ double Qm, double P)
  {
  Cn.SetHoldVentExpand();
  //Cn.AddM(Fi, som_ALL, QmIn*ICGetTimeInc());
  double T=Cn.Temp();
  double V=Cn.Model()->Volume(som_ALL, T, P);

  double Scl=V/GTZ(Cn.ShapeVolume());
  //double SclR=Range(1.0e-6, Scl, 1.0);
  //Cn.ScaleMass(som_ALL, 1.0/Max(1.0,1+0.5*(Scl-1)));
  //Cn.SetTempPress(T,POut);
  //
  //// Debug
  //Cn.Temp();
  //
  //dbgpln("== %15.10f", 1.0/Max(1.0,1+0.5*(Scl-1)));

//  if (Scl>1.0)
//    Fo.QSetM(Cn, som_ALL, QmOut, POut);
//  else
//    Fo.QSetM(Cn, som_ALL, QmOut*1.0e-6, POut); // Only for the properties
  Fo.QSetM(Cn, som_ALL, Qm*Min(Scl,1.1), P);
  Cn.ClrHoldVentExpand();
  }

void CPlugFlowInfo::Sample(CFlowSamplePt & Pt)
  {
  Pf.Sample(Pt);
  };
void CPlugFlowInfo::MeasureProperties(CSpPropInfoArray & Props, double StartPos, double EndPos)
  {
  Pf.MeasureProperties(Props, StartPos*m_dLength, EndPos*m_dLength);
  };

double CPlugFlowInfo::getSrcPressure() const
  {
  double P0=m_dPIn;
  double P1=m_dPIn+(1.0/m_nSections)*(m_dPOut-m_dPIn);
  double P=0.5*(P0+P1);

P=Max(1.0, P);

  return P;
  };

double CPlugFlowInfo::getDstPressure() const
  {
  double P0=m_dPIn;
  double P1=m_dPIn+(m_nSections/(m_nSections+1.0))*(m_dPOut-m_dPIn);
  double P=0.5*(P0+P1);

P=Max(1.0, P);

  return P;
  };

//==========================================================================
//
//
//==========================================================================

CSrcSnk::CSrcSnk()
  {
  dPress=Std_P;
  dTemp=Std_T;
  bViewFrac=1;
  bKeepImage=0;
  pImage=NULL;
#if WITHSRCSNKDIFFS
  dQmDiff=0;
  dTempDiff=0;
  pSpDiff=NULL;
#endif
  };
CSrcSnk::~CSrcSnk()
  {
  DeleteImage();
  };
void CSrcSnk::CreateImage(TaggedObject * pObj)
  {
  pImage=new SpImage("Image", pObj, TOA_Embedded);
  pImage->SetView(SVV_AsMassFrac);
  pImage->bSpeciesOnly=true;
#if WITHSRCSNKDIFFS
  pSpDiff=new SpImage("SpDiff", pObj, TOA_Embedded);
  pSpDiff->SetView(SVV_AsRawFrac);
  pSpDiff->bSpeciesOnly=true;
#endif
  bKeepImage=true;
  };
void CSrcSnk::DeleteImage()
  {
  delete pImage;
  pImage=NULL;
#if WITHSRCSNKDIFFS
  delete pSpDiff;
  pSpDiff=NULL;
#endif
  };

//==========================================================================
//
//
//==========================================================================

CLinkStatInfo::CLinkStatInfo()
  {
  iType = SIWhatAll;
  ResetStats();
  }

//--------------------------------------------------------------------------

CLinkStatInfo::~CLinkStatInfo()
  {
  }

//--------------------------------------------------------------------------

void CLinkStatInfo::ResetStats()
  {
  dTtlTime = 0.0;
  dZeroTime = 0.0;
  dBelowCapTime = 0.0;
  dTotalMass = 0.0;
  dTotalCapMass = 0.0;
  dMaxQm = -1e99;
  dMinQm = 1e99;
  dTotalVol = 0.0;
  dTotalMassS = 0.0;
  dTotalMassL = 0.0;
  }

//--------------------------------------------------------------------------

void CLinkStatInfo::BuildDataDefn(DataDefnBlk & DDB, TaggedObject* pObj)
  {
  DDB.BeginStruct(pObj, "Stats", NULL, DDB_OptPage);
  //DDB.Text("----------------------------------------");
  //DDB.Byte  ("Type",             "",          DC_,     "",      xidSI_Type,     pObj, isParmStopped, DDBFlwStatsWhat); //type MUST be before tag, etc
  DDB.Button("Reset_Stats",      "",          DC_,     "",      xidSI_ResetStats, pObj, isParm);
  DDB.Double("Time",             "Tm",        DC_Time, "s",     &dTtlTime,      pObj, isResult);
  if (iType & SIWhatQm)
    {
    DDB.Double("TotalMass",        "Mt",        DC_M,    "kg",    &dTotalMass,    pObj, isResult);
    DDB.Double("MassFlow_Average", "QmAve",     DC_Qm,   "kg/s",  xidSI_QmAve,    pObj, isResult|noFileAtAll);
    DDB.Double("ZeroFlowTime",     "ZeroTm",    DC_Time, "s",     &dZeroTime,     pObj, isResult);
    DDB.Double("FlowTime",         "QmTm",      DC_Time, "s",     xidSI_QmTime,   pObj, isResult|noFileAtAll|InitHidden);
    DDB.Double("On_Average",       "OnQmAve",   DC_Qm,   "kg/s",  xidSI_OnQmAve,  pObj, isResult|InitHidden|noFileAtAll);
    DDB.Double("Min_MassFlow",     "QmMin",     DC_Qm,   "kg/s",  &dMinQm,        pObj, isResult|InitHidden);
    DDB.Double("Max_MassFlow",     "QmMax",     DC_Qm,   "kg/s",  &dMaxQm,        pObj, isResult|InitHidden);
    }
  if (iType & SIWhatQv)
    {
    DDB.Double("TotalVol",         "Vt",        DC_Vol,  "m^3",   &dTotalVol,     pObj, isResult);
    DDB.Double("VolFlow_Average",  "QvAve",     DC_Qv,   "m^3/s", xidSI_QvAve,    pObj, isResult|noFileAtAll);
    }
  if (iType & SIWhatQmSol)
    {
    DDB.Double("SolidTotalMass",   "SMt",       DC_M,    "kg",    &dTotalMassS,   pObj, isResult);
    DDB.Double("SolidMassFlowAve" ,"SQmAve",    DC_Qm,   "kg/s",  xidSI_SQmAve,   pObj, isResult|noFileAtAll);
    }
  if (iType & SIWhatQmLiq)
    {
    DDB.Double("LiquidTotalMass",  "LMt",       DC_M,    "kg",    &dTotalMassL,   pObj, isResult);
    DDB.Double("LiquidMassFlowAve","LQmAve",    DC_Qm,   "kg/s",  xidSI_LQmAve,   pObj, isResult|noFileAtAll);
    }
  if (iType & SIWhatQmCap)
    {
    DDB.Double("TotalQmCapacity",  "CapMt",     DC_M,    "kg",    &dTotalCapMass, pObj, isResult);
    DDB.Double("CapMassFlowAve",   "CapQmAve",  DC_Qm,   "kg/s",  xidSI_CapQmAve, pObj, isResult|noFileAtAll);
    DDB.Double("CapUtilisation",   "CapUtil",   DC_Frac, "%",     xidSI_CapUtil,  pObj, isResult|noFileAtAll);
    }
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CLinkStatInfo::DataXchg(DataChangeBlk & DCB, TaggedObject* pObj)
  {
  switch (DCB.lHandle)
    {
/*    case xidSI_Type:
      todo
      pObj->StructureChanged(pObj);
      return True;*/
    case xidSI_ResetStats:
      if (DCB.rB && *DCB.rB)
        ResetStats();
      DCB.B=0;
      return True;
    case xidSI_QmAve:
      DCB.D = dTotalMass/GTZ(dTtlTime);
      return True;
    case xidSI_QmTime:
      DCB.D = dTtlTime-dZeroTime;
      return True;
    case xidSI_OnQmAve:
      DCB.D = dTotalMass/GTZ(dTtlTime-dZeroTime);
      return True;
    case xidSI_CapQmAve:
      DCB.D = dTotalCapMass/GTZ(dTtlTime);
      return True;
    case xidSI_CapUtil:
      DCB.D = Valid(dTotalCapMass) ? dTotalMass/GTZ(dTotalCapMass) : dNAN;
      return True;
    case xidSI_QvAve:
      DCB.D = dTotalVol/GTZ(dTtlTime);
      return True;
    case xidSI_SQmAve:
      DCB.D = dTotalMassS/GTZ(dTtlTime);
      return True;
    case xidSI_LQmAve:
      DCB.D = dTotalMassL/GTZ(dTtlTime);
      return True;
    }
  return false;
  }

//--------------------------------------------------------------------------

void CLinkStatInfo::ExecIns(MN_Lnk* pPipe)
  {
  const double dT = ICGetTimeInc();
  if (dT > 0.0)
    {
    dTtlTime += dT;
    ASSERT(pPipe->NoProcessIOs() >= 2);
    const int i = (pPipe->IO_In(0) ? 0 : 1);
    SpConduit* pSpCond = pPipe->IOConduit(i);
    const double dMeasQm = pPipe->IOFB(0,0)->GetQm(); //xidQm code from m_base
    if (iType & SIWhatQm)
      {
      if (fabs(dMeasQm)<1e-6)
        dZeroTime += dT;
      dTotalMass += (dMeasQm*dT);
      if (dMeasQm>dMaxQm)
        dMaxQm = dMeasQm;
      if (dMeasQm<dMinQm)
        dMinQm = dMeasQm;
      }
    if (iType & SIWhatQmCap)
      {
      const double dMeasCapQm = pPipe->m_FEP.FlwRqd().m_RqdQmCap;// xRqd_QmCap;
      if (Valid(dMeasCapQm))
        {
        if (Valid(dTotalCapMass))
          dTotalCapMass += (dMeasCapQm*dT);
        }
      else
        dTotalCapMass = dMeasCapQm;
      }
    if (iType & SIWhatQv)
      {
      const double dMeasQv = dMeasQm/GTZ(pSpCond->Rho(som_ALL)); //xidQv code from m_base
      dTotalVol += (dMeasQv*dT);
      }
    if (iType & SIWhatQmSol)
      {
      const double dMeasQmS = pSpCond->QMass(som_Sol);
      dTotalMassS += (dMeasQmS*dT);
      }
    if (iType & SIWhatQmLiq)
      {
      const double dMeasQmL = pSpCond->QMass(som_Liq);
      dTotalMassL += (dMeasQmL*dT);
      }
    }
  }

//--------------------------------------------------------------------------
//==========================================================================
//
//
//==========================================================================

static double ChgOvrFracLen=0.01;

MN_Lnk::MN_Lnk(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach, double LengthIn_):
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  m_RB(this, false),
  m_VLE(this, VLEF_QPFlash),
  m_Evap(this),
  m_MFB("MFB", this, TOA_Embedded, 0, 1, &m_VLE),
  m_PFI(this),
  m_FEP(NULL, FBOpt_HasSimpleRes, 0.1),
  m_FTB(this, &m_FEP),
  m_Spill(eDIO_Spill, this, false, true, "Spill", IOId_Spill2Area, IOId_AreaSpillIn),
  m_Src(eDIO_Src, this, false, true, "Src"),
  m_Snk(eDIO_Snk, this, false, true, "Snk")
#if WITHLINKBLEED 
  , m_Bleed(eDIO_Bleed, this, false, true, "Bleed")
#endif
#if (WITHBLOCKEVALUATOR)
  , m_BlkEval(&m_RB, NULL, &EHX, &m_VLE, &m_Evap)
#endif
  {
  AttachClassInfo(nc_MLnk,NULL);
  dZ_Rqd=dNAN;

  m_iBlockSeq = (PrjFileVerNo()<79 ? BS_EHX_RB_VLE : BS_RB_EHX_VLE); //sequence of RB,EHX,VLE,etc
  m_iLF_Direction = LFD_Default;

  SetAllowedSolveModes(SM_Direct/*|SM_Inline*/);//"Transfer", "Inline", NULL);
  SetAllowedFlowModes(true);
  
  if (GlblDynFlowMode())
    {
    if (OrigPrjFileVerNo()<=82)
      SetFlowModeReqd(LFM_Xfer);
    else
      SetFlowModeReqd(LFM_Simple);// LFM_Xfer);
    }
  else
    SetFlowModeReqd(LFM_Full);

  //m_iLnkFlwModeRqd = LFM_CompleteDP;//(SolveInlineMethod() ? LFM_Xfer : LFM_CompleteDP);
#if WITHPBDPSTUFF
  iDP_Mode = LDP_Fixed_Drop;
  iCalc_K  = 0;
  iCalc_FFac=1;
  LnkP_S   = Std_P;
  FxddP    = 0.0;
  PipeD    = 0.1;
  PipeL    = 10.0;
  FitPipeL = 0.0;
  Visc     = 1e-6;
  Rough    = 0.00001;
  K        = 1.0;
  KMinorLoss = 0.0;
  //SQmCap   = 1.0;
  //SQmdP    = 0.0;
  //VQmCap   = 1.0;
  //VQmdP    = 0.0;
  PipeScale= 0.0;
  PipeDeltaZ=0.0;
  PB_dp    = 0.0;
  Vel      = 0.0;
  FricFac  = K*PipeD/(PipeL+FitPipeL); //note: DarcyFricFact = 4 * FanningFricFact
  Re       = 0.0;
#endif

  RhoL     = 1000.0;
  RhoG     = 1.0;

  //dPolyTropicN      = 1.0;
  m_QmSpilt         = 0.0;
  m_iXferCapOption  = XCO_Spill;
  //m_pSpill          = NULL;

  m_iFlwPhaseRqd    = FPh_Both;

#if WithOEP
  FixedVf  = 1.0;
#endif
  fHasSlope=False;

  dMkUpRqd = dNAN;

  m_bWithOP=false;

  cioOn.Set(1);
  //xReqd_Qm = dNAN;
  //xMin_QmCap = 0.0;
  //xMax_QmCap = dNAN;
  //xRqd_QmCap = dNAN;
  fAppRhoH = true;
  fIsDegFree = true;
  fFindRhoH = false;

  TotalLength=1.0;
  ActualLength=1.0;
  VertLength=0.0;
  FlatLength=1.0;
  FlatFrac=1.0;
  ChgOvrLen=0.0;
  TotalArea=0.1;

  //m_PFI.Cn=NULL;
  //m_PFI.Q=NULL;
  pStats = NULL;

  fVolPrsFX   = false;
  fVolIsSmall = true;
  fVapLocked  = false;
  fTwoPhase   = false;
  
  m_FEP.SetIsPipe(true);
  m_FEP.SetFlowModeSrc(this, false);


#if WithOEP
  if (OEPOptions())
    {
    bVolFlwFX   = VFFX_PreMix;
    fVolPrsFX   = true;
    }
#endif

  fActiveHoldOK_PB=true;
  fActiveHoldOK_Dyn=true;
  m_fFwdOnly=false;

  m_bAdjustToEstFlow=false;

  m_pLastFlw=NULL;
  //m_Spill.SetCd(m_SpillFlng.Conduit(false));
  };

// -------------------------------------------------------------------------

MN_Lnk::~MN_Lnk()
  {
  //delete m_PFI.Cn;
  //delete m_PFI.Q;
  delete pStats;
  delete m_pLastFlw;
  //ClearSpill();
  };

// -------------------------------------------------------------------------

void MN_Lnk::ResetData(flag Complete)
  {
  for (int i=0; i<NoFlwIOs(); i++)
    for (int j=0; j<NIOFBs(i); j++)
      IOFB(i,j)->ResetData(Complete);
//  m_FEP.ResetData();
  };

// -------------------------------------------------------------------------

void MN_Lnk::UpdateSQFlags()
  {
  if (NoFlwIOs()>0)
    {
    CFlange &F=*IOFlange(0);
    if (F.RqdTearType()>=TT_ManualTear)
      {
      F.TearImage()->SetSQFlags(SQF_Editable|SQF_TearSet|SQF_CreateOK|SQF_CreateDsp, true);
      IOConduit(0)->SetSQFlags(SQF_Editable|SQF_CreateDsp|SQF_RemoveDsp, false);
      }
    else
      {
      if (F.TearType()>=TT_SystemTear)
        {
        IOConduit(0)->SetSQFlags(SQF_Editable|SQF_CreateDsp|SQF_RemoveDsp, false);
        SpConduit* pCI0 = IOConduitIn(0);
        //20/4/05: Chris, I have added a test for NULL because in a dynamic project this is sometimes null causing a crash!?!
        if (pCI0)
          pCI0->SetSQFlags(SQF_Editable|SQF_CreateDsp|SQF_RemoveDsp, false);
        }
      else
        IOConduit(0)->SetSQFlags(SQF_Editable|SQF_CreateDsp|SQF_RemoveDsp, true);
      }

    if (NoFlwIOs()>1)
      {
      IOConduit(1)->SetSQFlags(SQF_Editable|SQF_CreateDsp|SQF_RemoveDsp, false);
      }
    }
  };

// --------------------------------------------------------------------------

flag MN_Lnk::DataXchg(DataChangeBlk & DCB)
  {
  if (NoFlwIOs()>0 && IOFBs(0)==NULL)
    {//TROUBLE !!!
    //LogError(FullObjTag(), LF_Exclamation, "BAD DATA, not connected to unit");
    return False; //try prevent crash, don't even try get/set data!!!
    }

  if (m_MFB.On())
    {
    if (DCB.dwUserInfo!=0)
      {
      int xxx=0;
      }
    }
  else
    {
    if (DCB.dwUserInfo==3 && m_FEP.DataXchg(DCB))
      return 1;
    if (DCB.dwUserInfo==4 && IOFB(0,1)->DataXchg(DCB))
      return true;
    }

  if (m_PFI.CnExists && DCB.dwUserInfo==100 && m_PFI.Cn.DataXchg(DCB))
    return 1;
  if (m_PFI.PfExists && DCB.dwUserInfo==200 && m_PFI.Pf.DataXchg(DCB))
    return 1;
  if (m_pLastFlw && DCB.dwUserInfo==250 && m_pLastFlw->DataXchg(DCB))
    return 1;
  if (m_FTB.DataXchg(DCB))
    return 1;
#if (WITHBLOCKEVALUATOR)    
  if (m_BlkEval.DataXchg(DCB))
    return 1;
#endif
  if (m_RB.DataXchg(DCB))
    return 1;
  if (m_VLE.DataXchg(DCB))
    return 1;
  if (m_Evap.DataXchg(DCB))
    return 1;


  if (DCB.dwUserInfo==300 && m_Spill.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==301 && m_Src.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==302 && m_Snk.DataXchg(DCB))
    return 1;
#if WITHLINKBLEED 
  if (DCB.dwUserInfo==303 && m_Bleed.DataXchg(DCB))
    return 1;
#endif

  if (DCB.lHandle>=xidFEFirst && DCB.lHandle<xidFELast)
    {
    XID Handle=((DCB.lHandle-xidFEFirst)/NxidFEIOs)*NxidFEIOs+xidFEFirst;
    //int IONo=(DCB.lHandle-xidFEFirst)%NxidFEIOs;
    if (NoProcessIOs()<2)
      {
      DCB.D=dNAN;
      return True;
      }
    switch (Handle)
      {
      case xidFEArea       :  DCB.D=IOFB(0,0)->Area();                      return 1;
      case xidFELength     :
        {
        DCB.D=0.0;
        for (int i=0; i<2; i++)
          for (int j=0; j<NIOFBs(i); j++)
            DCB.D+=IOFB(i,j)->ActLength();
        return 1;
        }
      case xidFEDZ         :
        {
        DCB.D=0.0;
        for (int j=0; j<NIOFBs(0); j++)
          DCB.D+=IOFB(0,j)->Rise();
        for (j=0; j<NIOFBs(1); j++)
          DCB.D-=IOFB(1,j)->Rise();
        return 1;
        }
      case xidFEVelocity   :  DCB.D=IOFB(0,0)->Velocity();                    return 1;
      case xidFEDPb        :  DCB.D=IOFB(0,0)->DPb()-IOFB(1,0)->DPb();        return 1;
      case xidFEDPbX       :  DCB.D=IOFB(0,0)->DPbX()-IOFB(1,0)->DPbX();      return 1;
      case xidFEDPbdQ      :  DCB.D=IOFB(0,0)->DPbdQ()-IOFB(1,0)->DPbdQ();    return 1;
      case xidFEDPq        :  DCB.D=IOFB(0,0)->DPq()-IOFB(1,0)->DPq();        return 1;
      case xidFEDPqdQ      :  DCB.D=IOFB(0,0)->DPqdQ()-IOFB(1,0)->DPqdQ();    return 1;
      case xidFEDPz        :  DCB.D=IOFB(0,0)->DPz()-IOFB(1,0)->DPz();        return 1;
      case xidFEDPzdQ      :  DCB.D=IOFB(0,0)->DPzdQ()-IOFB(1,0)->DPzdQ();    return 1;
      case xidFEDPmB       :  DCB.D=IOFB(0,0)->DPmB()-IOFB(1,0)->DPmB();      return 1;
      case xidFEDPmQ       :  DCB.D=IOFB(0,0)->DPmQ()-IOFB(1,0)->DPmQ();      return 1;
      case xidFEDPmdQ      :  DCB.D=IOFB(0,0)->DPmdQ()-IOFB(1,0)->DPmdQ();    return 1;
      case xidFEQm         :  DCB.D=IOFB(0,0)->GetQm();                     return 1;
//      case xidFEQmStb      :  DCB.D=IOFB(0,0)->GetQmStb();                  return 1;
//      case xidFEQmImp      :  DCB.D=IOFB(0,0)->GetQmImp();                  return 1;
      case xidFEQmEst      :  DCB.D=IOFB(0,0)->GetQmEst();                  return 1;
//      case xidFEQmAve      :  DCB.D=IOFB(0,0)->GetQmAve();                  return 1;
      case xidFEQmPrvPB    :  DCB.D=IOFB(0,0)->GetQmPrvPB();                return 1;
      case xidFEQmAvail    :  DCB.D=IOFB(0,0)->GetQmAvail();                return 1;
      case xidFEQmSpace    :  DCB.D=IOFB(0,0)->GetQmSpace();                return 1;
      //case xidFEQmMemPB    :  DCB.D=IOFB(0,0)->GetQmMemPB();                return 1;
      case xidFEBstDamping :  DCB.D=IOFB(0,0)->BstDamping();                return 1;
      }
    }

  switch (DCB.lHandle)
    {
    case xidFlwPhaseRqd:
      if (DCB.rB)
        m_iFlwPhaseRqd=*DCB.rB;
      DCB.B=m_iFlwPhaseRqd;
      return 1;
    case xidFlwPhaseAct:
      if (NoProcessIOs()>=2)
        DCB.B=IOFB(0,0)->m_iFlwPh_Act;
      else
        DCB.B=FPh_UnKnown;
      return 1;
    case xidQmEstSrc:
      DCB.pC=GetQmEstSrc2IO() ? GetQmEstSrc2IO()->FullObjTag() : "";
      return true;
    case xidQmEstType:
      DCB.pC=(LPTSTR)GetQmEstType2IO();
      return true;
    case xidQmEst:
      DCB.D=GetQmEst2IO(DCB.rD);
      return 1;
    case xidQm:
      DCB.D=GetQm2IO(DCB.rD);
      return 1;
    case xidQi:
      DCB.D=(NoProcessIOs()>0) ? IOQm_In(0) : dNAN;
      return 1;
    case xidQo:
      DCB.D=(NoProcessIOs()>1) ? IOQm_Out(1) : dNAN;
      return 1;
    case xidLnkMode:
      {
      if (DCB.rL)
        {
        //if (SolveInlineMethod())
        //  *DCB.rB = LFM_Xfer; //FORCE into Xfer mode
        if (*DCB.rL==0)
          {
          if (GlblDynFullMode())
            *DCB.rL=LFM_Full;
          else if (GlblDynFlowMode())
            *DCB.rL=LFM_Simple;
          }
        if (FlowMode()!=*DCB.rL)
          {
          StructureChanged(this);
          SetState(MSA_ZeroFlows);
          }
        SetFlowModeReqd(*DCB.rL);
        if (FlowMode()==LFM_SrcSnk)
          {
          SetFlowModeReqd(LFM_Full);
          SetActiveHold(true);
          }
        }
      DCB.L=FlowMode();
      return 1;
      }
    case xidPropertySelect:
      {
      if (NoProcessIOs()>0)
        {
        FlwBlk &FB=*IOFB(0,0);
        if (DCB.rB)
          {
          FB.m_PropSel.m_iReqd=((*DCB.rB) & ~FBPS_Inherit);
          FB.m_PropSel.m_iInUse=FB.m_PropSel.m_iReqd;
          }

        DCB.B=FB.m_PropSel.m_iInUse;
        if (FB.m_PropSel.m_iInUse!=FB.m_PropSel.m_iReqd)
          DCB.B|=FBPS_Inherit;

        return 1;
        }
      return 0;
      }
    case xidMFBOn:
      {
      if (DCB.rB)
        {
//        if (m_MFB.OnRqd()!=*DCB.rB)
//          StructureChanged(this);
//        m_MFB.SetOnRqd(*DCB.rB);
#if WITHMULTIFLWBLK
        m_MFB.SetOn(*DCB.rB);
#else
        m_MFB.SetOn(0);
#endif
        }
      DCB.B=m_MFB.On();
      return 1;
      }
    case xidPFMethod:
      {
      if (DCB.rB)
        SetPFMethod(*DCB.rB, fVolPrsFX);
      DCB.B=m_PFI.Method;
      return 1;
      }
    case xidVolFlwFX:
      {
      if (DCB.rB)
        {
        if (PrjFileVerNo()>=59)
          SetPFMethod(*DCB.rB, fVolPrsFX);
        else
          SetPFMethod((*DCB.rB!=0) ? VFFX_PreMix : VFFX_Off, fVolPrsFX);
        }
      DCB.B=m_PFI.Method;
      return 1;
      }
    case xidPlugFlowType:
      {
      if (DCB.rB)
        SetPFType(*DCB.rB);
      DCB.B=m_PFI.Type;
      return 1;
      }
    case xidPlugFlowSects:
      {
      if (DCB.rL)
        SetPFSectionCount(*DCB.rL);
      DCB.L=m_PFI.SectionCount;
      return 1;
      }
    case xidVolPrsFX:
      {
      if (DCB.rB)
        SetPFMethod(m_PFI.Method, *DCB.rB);
      DCB.B=fVolPrsFX;
      return 1;
      }
    case xidWithOP:
      {
      if (DCB.rB)
        {
        m_bWithOP = (*DCB.rB!=0);
        if (NoProcessIOs()>0)
          {
          if (m_bWithOP)
            {
            SetNIOFBs(0,2);
            IOFB(0,0)->AssignFlwEqnGroup(PipeGroup, PipeGroup.Default(), this);
            IOFB(0,1)->AssignFlwEqnGroup(OrificeGroup, OrificeGroup.Default(), this);
            }
          else
            {
            SetNIOFBs(0,1);
            IOFB(0,0)->AssignFlwEqnGroup(PipeGroup, PipeGroup.Default(), this);
            }
          }
        }
      DCB.B=m_bWithOP;
      return 1;
      }
   case xidSrcHFrc:
      if (NoProcessIOs()>=1)
        {
        if (DCB.rD)
          Nd_Rmt(0)->SetIORelFracHgt_Flng(IOIONo_Rmt(0), *DCB.rD);
        DCB.D= Nd_Rmt(0)->IORelFracHgt_Flng(IOIONo_Rmt(0));
        }
      else
        DCB.D=0.0;
      return 1;
    case xidDstHFrc:
      if (NoProcessIOs()>=2)
        {
        if (DCB.rD)
          Nd_Rmt(1)->SetIORelFracHgt_Flng(IOIONo_Rmt(1), *DCB.rD);
        DCB.D= Nd_Rmt(1)->IORelFracHgt_Flng(IOIONo_Rmt(1));
        }
      else
        DCB.D=0.0;
      return 1;
    case xidSrcHgt :
      if (NoProcessIOs()>=1)
        DCB.D= IODatum_Flng(0);
      else
        DCB.D=0.0;
      return 1;
    case xidDstHgt :
      if (NoProcessIOs()>=2)
        DCB.D= IODatum_Flng(1);
      else
        DCB.D=0.0;
      return 1;
    case xidSrcAperture:
      if (NoProcessIOs()>=1)
        {
        if (DCB.rD)
          Nd_Rmt(0)->SetIOAperture(IOIONo_Rmt(0), *DCB.rD);
        DCB.D= Nd_Rmt(0)->IOAperture(IOIONo_Rmt(0), !DCB.ForFileSnpScn());
        }
      else
        DCB.D=0.0;
      return 1;
    case xidDstAperture:
      if (NoProcessIOs()>=1)
        {
        if (DCB.rD)
          Nd_Rmt(1)->SetIOAperture(IOIONo_Rmt(1), *DCB.rD);
        DCB.D= Nd_Rmt(1)->IOAperture(IOIONo_Rmt(1), !DCB.ForFileSnpScn());
        }
      else
        DCB.D=0.0;
      return 1;
    //case xidFlashIt:
    //  {
    //  if (DCB.rB)
    //    {
    //    if (bFlashIt!=*DCB.rB)
    //      StructureChanged(this);
    //    bFlashIt=*DCB.rB;
    //    }
    //  DCB.B=bFlashIt;
    //  }
    //  return 1;
    case xidVelocity:
      if (NoProcessIOs()>=1)
        DCB.D=IOFB(0,0)->Velocity()*IOFB(0,0)->GetQm()/NZ(IOFB(0,0)->GetQmEst());
      else
        DCB.D=0.0;
      return 1;
    case xidChokeVelocity:
      if ((NoProcessIOs() > 1) && IOFlange(1))
        {
        //double X=C.MassFrac(som_Gas);
        //if (X>1.0e-10)
        //  {
        //  double LDens=C.Rho(som_SL);
        //  double GDens=C.Rho(som_Gas);
        //  double P=C.Press();
        //
        //  double Gamma=1.3;
        //  double Chi=Pow(2/(Gamma+1), Gamma/(Gamma-1));
        //  double VolCR=(1-X)/LDens+X*Pow(Chi, -1/Gamma)/GDens;
        //  double Flux1=(2*X*P/(0.001*GDens))*(Gamma/(Gamma-1));
        //  double Flux2=1-2/(Gamma+1);
        //  #pragma chCHECKIT(Paper == )
        ////  double FluxCr=Sqrt(1/VolCR*fabs(Flux1*Flux2));
        //  double FluxCr=(1/VolCR)*Sqrt(fabs(Flux1*Flux2));
        //  double VapVel=316.2*Sqrt(Gamma*P/(100*GDens));
        //  double TPVel=GDens*VapVel/(TPDens*Sqrt(X*Gamma));
        //
        //  double Flux=MassFlow/AreaO;
        //  if (VelMix>TPVel && Flux>1.3*FluxCr)
        //    {
        //    dbgpln("Choked VelR:%10.2f FluxR:%10.2f", VelMix/GTZ(TPVel), Flux/GTZ(1.3*FluxCr));
        //    int yyy=0;
        //    }
        //  int xxx=0 ;
        //
        //  }

        SpConduit &C=*IOConduit(1);
        DCB.D=Sqrt(1.4*C.Press()*1000/GTZ(C.Rho()));
        return 1;
        }
      DCB.D=dNAN;
      return 1;
    case xidCalcRes:
      {
      if (NoProcessIOs()>=2)
        DCB.D=fabs(IOP_Self(1)-IOP_Self(0))/GTZ(IOConduit(0)->QMass(som_ALL));
      else
        DCB.D=dNAN;
      return 1;
      }
    case xidKeepSrcSnk:
      if (DCB.rB)
        {
        SrcSnk.bKeepImage=*DCB.rB;
        if (SrcSnk.bKeepImage)
          SrcSnk.CreateImage(this);
        }
      DCB.B=SrcSnk.bKeepImage;
      return 1;
    case xidCopyCurrent:
      if (DCB.rB && (*DCB.rB!=0) && DCB.ForView())
        {
        if (NoProcessIOs()>=2)
          {
          SrcSnk.dPress=0.5*(IOConduit(0)->Press()+IOConduit(1)->Press());
          SrcSnk.dTemp=0.5*(IOConduit(0)->Temp()+IOConduit(1)->Temp());
          SrcSnk.pImage->Copy(*IOConduit(0), SrcSnk.dPress);
          }
        }
      return 1;
    case xidTearType:
      {
      int iT=0;
      if (DCB.rB && NoProcessIOs()>=2)
        {
        IOFlange(iT)->SetRqdTearType(*DCB.rB);
        if (IOFlange(iT)->RqdTearType()>=TT_ManualTear)
          IOFlange(iT)->SetTearImage(this);
        else
          IOFlange(iT)->ClrTearImage(this);
        UpdateSQFlags();
        }
      if (NoProcessIOs()>=2)
        DCB.B=Max(IOFlange(iT)->RqdTearType(), IOFlange(1)->RqdTearType());
      else
        DCB.B=TT_NoTear;
      return 1;
      }
    case xidTearState:
      {
      int iT=0;
      if (DCB.rB && NoProcessIOs()>=2)
        {
        IOFlange(iT)->SetTearType(*DCB.rB, this);
        if (IOFlange(iT)->TearType()!=TT_NoTear)
          IOFlange(iT)->SetTearImage(this);
        }
      if (NoProcessIOs()>=2)
        DCB.B=Max(IOFlange(iT)->TearType(), IOFlange(1)->TearType());
      else
        DCB.B=TT_NoTear;
      return 1;
      }
    case xidTearPriority:
      {
      int iT=0;
      if (DCB.rB && NoProcessIOs()>=2)
        IOFlange(iT)->SetTearPriority(*DCB.rB);
      if (NoProcessIOs()>=2)
        DCB.B=Max(IOFlange(iT)->TearPriority(), IOFlange(1)->TearPriority());
      else
        DCB.B=TP_Normal;
      return 1;
      }
    case xidTearInitWhen:
      {
      int iT=0;
      if (NoProcessIOs()>=2)
        {
        if (DCB.rB)
          IOFlange(iT)->SetTearInitWhen(*DCB.rB);
        DCB.B=IOFlange(iT)->TearInitWhen();
        DCB.dwRetFlags=(IOFlange(iT)->RqdTearType()==TT_ManualTear) ? isParm : 0;
        }
      else
        DCB.B=TIW_OnInit;
      return 1;
      }
    case xidTearInitHow:
      {
      int iT=0;
      if (NoProcessIOs()>=2)
        {
        if (DCB.rB)
          IOFlange(iT)->SetTearInitHow(*DCB.rB);
        DCB.B=IOFlange(iT)->TearInitHow();
        DCB.dwRetFlags=(IOFlange(iT)->RqdTearType()==TT_ManualTear) ? isParm : 0;
        }
      else
        DCB.B=TIH_ZeroNIters;
      return 1;
      }
    case xidTearInitEstUsage:
      {
      int iT=0;
      if (NoProcessIOs()>=2)
        {
        if (DCB.rB)
          {
          IOFlange(iT)->SetTearInitEstUsage(*DCB.rB);
          }
        DCB.B=IOFlange(iT)->TearInitEstUsage();
        //DCB.dwRetFlags=(IOFlange(iT)->RqdTearType()==TT_ManualTear) ? isParm : 0;
        }
      else
        DCB.B=TIEU_None;
      return 1;
      }
    case xidTearInitActive:
      {
      int iT=0;
      if (NoProcessIOs()>=2)
        {
        CFlange &F=*IOFlange(iT);
        if (F.TearInitActive())
          {
          DCB.B=F.TearInitHow();
          //switch (F.TearInitHow())
          //  {
          //  case TIH_ZeroNIters:
          //    DCB.B=F.TearHoldCount()<F.TearHoldRqdCnt() ? TIH_Zeroing : TIH_ZeroNIters;
          //    break;
          //  case TIH_HoldNIters:
          //    DCB.B=F.TearHoldCount()<F.TearHoldRqdCnt() ? TIH_Holding : TIH_HoldNIters;
          //    break;
          //  case TIH_RampNIters:
          //    DCB.B=F.TearHoldCount()<F.TearHoldRqdCnt() ? TIH_Ramping : TIH_RampNIters;
          //    break;
          //  }

          //DCB.dwRetFlags=(IOFlange(iT)->RqdTearType()==TT_ManualTear) ? isParm : 0;
          }
        else DCB.B=TIH_Off;
        }
      else
        DCB.B=0;//TI_OnInit|TI_Zero;
      return 1;
      }
    case xidTearInitActivate:
      {
      int iT=0;
      if (NoProcessIOs()>=2)
        {
        if (DCB.rB)
          IOFlange(iT)->SetTearInitActive(*DCB.rB!=0, !DCB.ForFileSnpScn());
        //DCB.B=IOFlange(iT)->TearInitActive();
        //DCB.dwRetFlags=(IOFlange(iT)->RqdTearType()==TT_ManualTear) ? isParm : 0;
        }
      else
        DCB.B=0;//TI_OnInit|TI_Zero;
      return 1;
      }
    case xidTearCntRqd:
      {
      int iT=0;
      if (NoProcessIOs()>=2)
        {
        if (DCB.rB)
          IOFlange(iT)->SetTearHoldRqdCnt(*DCB.rB);
        DCB.B=IOFlange(iT)->TearHoldRqdCnt();
        DCB.dwRetFlags=(IOFlange(iT)->RqdTearType()>=TT_ManualTear) ? isParm : 0;
        }
      else
        DCB.B=-1;
      return 1;
      }
    case xidTearCntAct:
      {
      int iT=0;
      if (NoProcessIOs()>=2)
        {
        if (DCB.rB)
          IOFlange(iT)->SetTearHoldCount(*DCB.rB);
        DCB.B=IOFlange(iT)->TearHoldCount();
        //DCB.dwRetFlags=(IOFlange(iT)->RqdTearType()>=TT_ManualTear) ? isParm : 0;
        }
      else
        DCB.B=-1;
      return 1;
      }
    case xidTearQmMode:
      {
      int iT=0;
      if (NoProcessIOs()>1 && IOFlange(iT)->TearImageExists())
        {
        SpImage &I=*IOFlange(iT)->TearImage();
        if (DCB.rB)
          I.SetQMode(SpImgMode(*DCB.rB));
        DCB.B=I.QMode();
        }
      else
        DCB.B=SPI_QModeNone;
      return 1;
      }
    case xidTearQmRqd:
      {
      int iT=0;
      if (NoProcessIOs()>1 && IOFlange(iT)->TearImageExists())
        {
        SpImage &I=*IOFlange(iT)->TearImage();
        if (DCB.rD)
          I.SetQmRqdTo(*DCB.rD, true, !DCB.ForFileSnpScn());
        DCB.D=I.QmRqd();
        }
      else
        DCB.D=dNAN;
      return 1;
      }
    case xidTearQvRqd:
      {
      int iT=0;
      if (NoProcessIOs()>1 && IOFlange(iT)->TearImageExists())
        {
        SpImage &I=*IOFlange(iT)->TearImage();
        if (DCB.rD)
          I.SetQvRqdTo(*DCB.rD, true, !DCB.ForFileSnpScn());
        DCB.D=I.QvRqd();
        }
      else
        DCB.D=dNAN;
      return 1;
      }
    case xidTearNQvRqd:
      {
      int iT=0;
      if (NoProcessIOs()>1 && IOFlange(iT)->TearImageExists())
        {
        SpImage &I=*IOFlange(iT)->TearImage();
        if (DCB.rD)
          I.SetNQvRqdTo(*DCB.rD, true, !DCB.ForFileSnpScn());
        DCB.D=I.NQvRqd();
        }
      else
        DCB.D=dNAN;
      return 1;
      }
    case xidTearTRqd:
      {
      int iT=0;
      if (NoProcessIOs()>1 && IOFlange(iT)->TearImageExists())
        {
        if (DCB.rD)
          IOFlange(iT)->TearImage()->SetTRqdTo(*DCB.rD);
        DCB.D=IOFlange(iT)->TearImage()->TRqd();
        }
      else
        DCB.D=dNAN;
      return 1;
      }
    case xidTearPRqd:
      {
      int iT=0;
      if (NoProcessIOs()>1 && IOFlange(iT)->TearImageExists())
        {
        if (DCB.rD)
          IOFlange(iT)->TearImage()->SetPRqdTo(*DCB.rD);
        DCB.D=IOFlange(iT)->TearImage()->PRqd();
        }
      else
        DCB.D=dNAN;
      return 1;
      }
    case xidOldQmReqd       :
      if (DCB.rD)
        m_FEP.FlwRqd().m_ReqdQm = Valid(*DCB.rD) ? *DCB.rD : dNAN;
      DCB.D=m_FEP.FlwRqd().m_ReqdQm;
      return 1;
    case xidOldQmRqdCapacity:
      if (DCB.rD)
        m_FEP.FlwRqd().m_RqdQmCap = Valid(*DCB.rD) ? *DCB.rD : dNAN;
      DCB.D=m_FEP.FlwRqd().m_RqdQmCap;
      return 1;
    case xidOldQmMinCapacity:
      if (DCB.rD)
        m_FEP.FlwRqd().m_MinQmCap = Valid(*DCB.rD) ? *DCB.rD : dNAN;
      DCB.D=m_FEP.FlwRqd().m_MinQmCap;
      return 1;
    case xidOldQmMaxCapacity:
      {
      if (DCB.rD)
        m_FEP.FlwRqd().m_MaxQmCap = Valid(*DCB.rD) ? Max(m_FEP.FlwRqd().m_MinQmCap, *DCB.rD) : dNAN;
      DCB.D=m_FEP.FlwRqd().m_MaxQmCap;// xMax_QmCap;
      return 1;
      }
    case xidSI_On:
      if (DCB.rB)
        {
        if (*DCB.rB && pStats==NULL)
          pStats = new CLinkStatInfo;
        if (*DCB.rB==0 && pStats)
          {
          delete pStats;
          pStats = NULL;
          }
        }
      DCB.B=(pStats!=NULL);
      return 1;
    case xidUDFOpenP:
      if (NoProcessIOs()>0)
        {
        if (DCB.rD)
          IOFB(0,0)->SetUDFOpenP(*DCB.rD);
        DCB.D=IOFB(0,0)->UDFOpenP();
        }
      else
        DCB.D=0.0;
      return 1;
    case xidUDFActCnt:
      if (NoProcessIOs()>0)
        {
        if (DCB.rL)
          IOFB(0,0)->SetUDFActionCount(*DCB.rL);
        DCB.L=IOFB(0,0)->UDFActionCount();
        }
      else
        DCB.L=0;
      return 1;
    case xidUDFState:
      if (NoProcessIOs()>0)
        {
        DCB.B=IOFB(0,0)->UDFIsClosed();
        }
      else
        DCB.B=0;
      return 1;
    case xidSamplePtCount:
      if (DCB.rL)
        SetSamplePtCount(*DCB.rL);
      DCB.L=GetSamplePtCount();
      return 1;

    //case xidSpillTarget:
    //  if (DCB.rpC!=NULL && (strlen(DCB.rpC)>0))
    //    m_Spill.SetTarget(DCB.rpC);  ??
    //  //if (SpillExists())
    //    DCB.pC=(LPTSTR)m_Spill.Target();
    //  //else
    //  //  DCB.pC="";
    //  return 1;
    //case xidSpillTotal:
    //  ////if (SpillExists())
    //  ////  {
    //  //  if (DCB.rD)
    //  //    m_Spill.SetTotal(*DCB.rD);
    //  //  DCB.D=m_Spill.Total();
    //  ////  }
    //  ////else
    //  ////  DCB.D=0.0;
    //  return 1;
    //case xidSpillRate:
    //  //if (SpillExists())
    //    DCB.D=m_Spill.Cd.QMass();
    //  //else
    //  //  DCB.D=0.0;
    //  return 1;
    //case xidSpillDiscard:
    //  ////if (SpillExists())
    //  ////  {
    //  //  if (DCB.rB)
    //  //    Spill.SetDiscard(*DCB.rB);
    //  //  DCB.B=Spill.Discard();
    //  ////  }
    //  ////else
    //  ////  DCB.B=0;
    //  return 1;
    }

  if (pStats)
    {
    if (pStats->DataXchg(DCB, this))
      return 1;
    }

  return MdlNode::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

//void MN_Lnk::CreateSpill()
//  {
//  m_pSpill=new SpDirectCd("SpillFlw", this, TOA_Free);
//  m_pSpill->Target().SetTag("Floor.Content");
//  StructureChanged(this);
//  };

//--------------------------------------------------------------------------

//void MN_Lnk::ClearSpill()
//  {
//  delete m_pSpill;
//  m_pSpill=NULL;
//  StructureChanged(this);
//  } ;
//
//--------------------------------------------------------------------------

//flag MN_Lnk::SpillExists()
//  {
//  return (m_pSpill!=NULL);
//  };
//flag MN_Lnk::SpillFlowing()
//  {
//  return m_pSpill!=NULL && (m_pSpill->QMass(som_ALL)>1.0e-12);
//  }
//rSpDirectCd MN_Lnk::Spill()
//  {
//  if (m_pSpill==NULL)
//    CreateSpill();
//  return *m_pSpill;
//  };
//void MN_Lnk::SpillClear()
//  {
//  if (m_pSpill)
//    m_pSpill->QSetTraceMass();
//  };
// -------------------------------------------------------------------------

void MN_Lnk::SetPFMethod(byte Meth, flag VolPrsFX)
  {
  if (VolPrsFX && Meth==VFFX_Off)
    Meth=VFFX_PostMix;

  byte OldMeth=m_PFI.Method;
  fVolPrsFX=VolPrsFX;
  m_PFI.SetMethod(Meth);
  if (OldMeth!=m_PFI.Method)
    {
    if (m_PFI.Method==VFFX_Plugflow)
      SetSamplePtCount(3);
    else
      SetSamplePtCount(0);
    }
  }

// -------------------------------------------------------------------------

void MN_Lnk::SetPFType(byte Type)
  {
  m_PFI.SetType(Type);
  }

// -------------------------------------------------------------------------

void MN_Lnk::SetPFSectionCount(long N)
  {
  m_PFI.SetSectionCount(N);
  }

// -------------------------------------------------------------------------

flag MN_Lnk::ValidateData(ValidateDataBlk & VDB)
  {
  if (NoProcessIOs()>0 && IOFBs(0)==NULL)
    {//TROUBLE !!!
    //LogError(FullObjTag(), LF_Exclamation, "BAD DATA, not connected to unit");
    SetCI(2, true);
    return False; //try prevent crash, don't continue!!!
    }
  SetCI(2, NoProcessIOs()<2);

  flag OK=MdlNode::ValidateData(VDB);

  if (FlowModeReqd()==0)
    {
    if (GlblDynFlowMode())
      {
      if (PrjFileVerNo()<94)
        SetFlowModeReqd(LFM_Xfer);
      else
        SetFlowModeReqd(LFM_Simple);
      }
    else
      SetFlowModeReqd(LFM_Full);
    }

  if (!m_FTB.ValidateData(VDB))
    OK=false;

  if (NoProcessIOs()>=2 && (IOISSETXFER(IODesc_Rmt(0)->dwOther) || IOISSETXFER(IODesc_Rmt(1)->dwOther)))
    SetFlowModeReqd(LFM_Xfer);

#if WITHPBDPSTUFF
  //ValidateRange(VDB, "V_MassFlw", 1.0e-10, VQmCap, dNAN, &OK);
  //ValidateRange(VDB, "S_MassFlw", 1.0e-10, SQmCap, dNAN, &OK);
#endif

  if (m_PFI.Method==VFFX_Plugflow)
    {
    m_RB.Disable();
    m_VLE.Disable();
    }
  else
    SetSamplePtCount(0);

  if (NoProcessIOs()>0 && IOFlange(0)->TearImageExists())
    {
    SpImage &I=*IOFlange(0)->TearImage();
    I.ValidateData(VDB);
    I.SetMassInUseOK(IOFlange(0)->TearInitEstUsage()==TIEU_PartEst);
    switch (I.QMode())
      {
      case SPI_QModeQv: I.SetView(SVV_AsVolFlow); break;
      case SPI_QModeNQv: I.SetView(SVV_AsNVolFlow); break;
      default: I.SetView(SVV_AsMassFlow); break;
      }
    }

  if (NoProcessIOs()>=2)
    {
    if (m_MFB.On())
      {
      m_MFB.AssignFlowGroups();
      m_MFB.SetPhysicalData();
      }
    else
      {
      for (int i=0; i<NoProcessIOs(); i++)
        {
        if (m_bWithOP && i==0)
          {
          SetNIOFBs(i,2);
          IOFB(i,0)->AssignFlwEqnGroup(PipeGroup, PipeGroup.Default(), this);
          IOFB(i,1)->AssignFlwEqnGroup(OrificeGroup, OrificeGroup.Default(), this);
          }
        else
          {
          SetNIOFBs(i,1);
          IOFB(i,0)->AssignFlwEqnGroup(PipeGroup, PipeGroup.Default(), this);
          }
        }

      DeltaZ=IODatum_Flng(1) - IODatum_Flng(0);

      m_FEP.SetPhysData(DeltaZ);

      ActualLength=Max(0.01, m_FEP.ActLength());
      //TotalLength=Max(0.01, ActualLength+D.XLength());
      VertLength=fabs(DeltaZ);
      FlatLength=ActualLength-VertLength;
      FlatFrac=FlatLength/GTZ(ActualLength);
      fHasSlope=(VertLength>1.0e-3);
      ChgOvrLen=ActualLength*ChgOvrFracLen;

      TotalArea=m_FEP.Area();

      Set_JoinP(0, 0.5*(IOP_Self(0)+IOP_Self(1)));
      }

    SpConduit &Fi=*IOConduit(IO_In(0) ? 0 : 1);

    m_PFI.InitialisePlugFlow(ActualLength, TotalArea, VertLength, Fi, 1.0, IOP_Flng(0), IOP_Flng(1), IOFB(0,0)->Velocity(), -IOFB(1,0)->Velocity());
    UpdateSamples();

    if (m_PFI.CnExists)
      m_PFI.Cn.Tag("Cn");
    if (m_PFI.PfExists)
      m_PFI.Pf.Tag("Pf");
    }

  flag ImageRqd=GetActiveHold() || SrcSnk.bKeepImage;
  if (SrcSnk.pImage==NULL && ImageRqd)
    {
    SrcSnk.CreateImage(this);
    SrcSnk.bKeepImage=true;
    }
  else if (SrcSnk.pImage && !ImageRqd)
    {
    SrcSnk.DeleteImage();
    }

  UpdateSQFlags();

  if (NoProcessIOs()>=2)
    {
    SpConduit &Fi=*IOConduit(0);
    SpConduit &Fo=*IOConduit(1);
    if (Fi.QMass()<1.0e-6 && Fo.QMass()>1.0e-6)
      Fi.QCopy(Fo);
    else if (Fo.QMass()<1.0e-6 && Fi.QMass()>1.0e-6)
      Fo.QCopy(Fi);
    }

#if (WITHBLOCKEVALUATOR)    
  if (!m_BlkEval.ValidateData(VDB))
    OK=false;
#endif

  return OK;
  }

//--------------------------------------------------------------------------

long MN_Lnk::CheckDataRanges(bool CheckIO)
  {
  return MdlNode::CheckDataRanges(true); 
  }

//--------------------------------------------------------------------------

flag MN_Lnk::GetModelAction(Strng & Tag, MdlActionArray & Acts)
  {
  MdlAction M0(0, MAT_State, !cioOn.On(), "Open", 1);
  MdlAction M1(1, MAT_State, cioOn.On(),  "Close", 0);
  MdlAction M2(2, MAT_Config, true,  "Flow Characteristics\tSet Properties", 0);
  MdlAction M3(3, MAT_Config, true,  "Flow Characteristics\tClr Properties", 0);
  MdlAction M4(4, MAT_Config, true,  "Flow Characteristics\tSet Linearisation", 0);
  MdlAction M5(5, MAT_Config, true,  "Flow Characteristics\tClr Linearisation", 0);

  Acts.SetSize(0);
  Acts.SetAtGrow(0, M0);
  Acts.SetAtGrow(1, M1);
  Acts.SetAtGrow(2, M2);
  Acts.SetAtGrow(3, M3);
  Acts.SetAtGrow(4, M4);
  Acts.SetAtGrow(5, M5);
  Acts.SetAtGrow(6, MdlAction(6, MAT_Switch));
  return True;
  };

//--------------------------------------------------------------------------

flag MN_Lnk::SetModelAction(Strng & Tag, MdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
    case 1:
      cioOn.Set(Act.iValue);
      break;
    case 2:
      {
      for (int i=0; i<NoProcessIOs(); i++)
        {
        SpConduit &Fi=*IOConduit(i);
        if (i==0)
          m_FEP.SetOveride(Fi.Rho(som_SL), Fi.Rho(som_Gas), Fi.Temp(), Fi.DynamicViscosity());
        for (int fe=0; fe<NIOFBs(i); fe++)
          IOFB(i,fe)->SetOveride(Fi.Rho(som_SL), Fi.Rho(som_Gas), Fi.Temp(), Fi.DynamicViscosity());
        for (fe=0; fe<NIOFBs_Rmt(i); fe++)
          IOFB_Rmt(i,fe)->SetOveride(Fi.Rho(som_SL), Fi.Rho(som_Gas), Fi.Temp(), Fi.DynamicViscosity());
        }
      }
      break;
    case 3:
      {
      m_FEP.ClrOveride();
      for (int i=0; i<NoProcessIOs(); i++)
        {
        for (int fe=0; fe<NIOFBs(i); fe++)
          IOFB(i,fe)->ClrOveride();
        for (fe=0; fe<NIOFBs_Rmt(i); fe++)
          IOFB_Rmt(i,fe)->ClrOveride();
        }
      }
    case 4:
      {
      for (int i=0; i<NoProcessIOs(); i++)
        {
        if (i==0)
          m_FEP.SetLinearised(true, IOFB(0,0)->DPq(), IOFB(0,0)->QmMeas(), IOFB(0,0)->Regulation());
        for (int fe=0; fe<NIOFBs(i); fe++)
          IOFB(i,fe)->SetLinearised(true, IOFB(i,fe)->DPq(), IOFB(i,fe)->QmMeas(), IOFB(i,fe)->Regulation());
        for (fe=0; fe<NIOFBs_Rmt(i); fe++)
          IOFB_Rmt(i,fe)->SetLinearised(true, IOFB_Rmt(i,fe)->DPq(), IOFB_Rmt(i,fe)->QmMeas(), IOFB_Rmt(i,fe)->Regulation());
        }
      }
      break;
    case 5:
      {
      m_FEP.ClrLinearised();
      for (int i=0; i<NoProcessIOs(); i++)
        {
        for (int fe=0; fe<NIOFBs(i); fe++)
          IOFB(i,fe)->ClrLinearised();
        for (fe=0; fe<NIOFBs_Rmt(i); fe++)
          IOFB_Rmt(i,fe)->ClrLinearised();
        }
      }
      break;
    case 6:
      cioOn.Set(!cioOn.On());
      break;
//    case 2:
//      VPB.SetManualPosition(Act.dValue*0.01);
//      break;
    }
  return true;
  };

//--------------------------------------------------------------------------

void MN_Lnk::GlobalConnect()
  {
  m_Src.SysEnable = m_RB.Enabled();
  m_Snk.SysEnable = m_RB.Enabled();
#if WITHLINKBLEED 
  m_Bleed.SysEnable = m_Evap.Enabled();
#endif
  MdlNode::GlobalConnect();
  }

//--------------------------------------------------------------------------

CFlange * MN_Lnk::GetFlange(int IOId)
  {
  switch (IOId)
    {
    case IOId_Spill2Area: return &m_Spill.Flange;
    }
  return NULL;
  };

//--------------------------------------------------------------------------

void MN_Lnk::PostConnect(int IONo)
  {
  if (!PostConnectDirect(IONo))
    {
    MdlNode::PostConnect(IONo);

    if (IOISSETXFER(IODesc_Rmt(IONo)->dwOther))
      SetFlowModeReqd(LFM_Xfer);

    IOConduit(IONo)->AttachMeToObj(this, TOA_Embedded);

    //for (int i=0; i<NoProcessIOs(); i++)
    //  {
    if (IsProcessIOId(IOId_Self(IONo)))
      {
      IOFB(IONo,0)->AssignFlwEqnGroup(PipeGroup, PipeGroup.Default(), this);
      IOFB(IONo,0)->SetParentFlwEqn(&m_FEP);
      IOFB(IONo,0)->SetFlowModeSrc(this, false);

      for (int fe=0; fe<NIOFBs_Rmt(IONo); fe++)
        IOFB_Rmt(IONo,fe)->SetFlowModeSrc(this, true);
      }
      //}
    UpdateSQFlags();

    switch (IOId_Self(IONo))
      {
      case 0:
        IOFB(IONo,0)->SetIsPipe(true);
        IOFB(IONo,0)->SetFBScales(0.5, 0.5);
        if (NIOFBs(IONo)>1)
          {
          IOFB(IONo,1)->SetIsPipe(false);
          IOFB(IONo,1)->SetFBScales(1.0, 1.0);
          }
        break;
      case 1:
        IOFB(IONo,0)->SetIsPipe(true);
        IOFB(IONo,0)->SetFBScales(0.5, -0.5);
        break;
      }
    }
  };

//---------------------------------------------------------------------------

void MN_Lnk::PreDisConnect(int IONo)
  {
  if (!PreDisConnectDirect(IONo) && Nd_Rmt(IONo))
    IOConduit(IONo)->DetachMeFromOwner();
  }

//--------------------------------------------------------------------------

flag MN_Lnk::PreStartCheck()
  {
  flag Ok=(NoProcessIOs()==2 && NoFlwIOs()>=2);
  if (!Ok)
    {
    LogError(FullObjTag(), 0, "Not correctly Connected");
    return Ok;
    }

  //if (IOFlange(0) && IOConduit(0)->Model())
  Ok=IOConduit(0)->Model()->Class()->LicOK();
  if (!Ok)
    {
    LogError(FullObjTag(), 0, "Specie model '%s' not enabled by licensing", IOConduit(0)->Model()->Class()->ShortDesc());
    }
  return MdlNode::PreStartCheck();
  };

//--------------------------------------------------------------------------

Z_States MN_Lnk::SetDatums(int Pass, int IOIn, double Zi, Z_States Z_State_Src)
  {
  if (m_MFB.On())
    return m_MFB.SetDatums(Pass, IOIn, Zi, Z_State_Src, dZ_Rqd);
  else
    return SetDatums_Link(Pass, dZ_Rqd, IOIn, Zi, Z_State_Src);

  };

//--------------------------------------------------------------------------

flag MN_Lnk::Set_Sizes()
  {
  return True;
  };

//--------------------------------------------------------------------------

void MN_Lnk::SetState(eScdMdlStateActs RqdState)
  {
  MdlNode::SetState(RqdState);
  switch (RqdState)
    {
    case MSA_PBInit:
    case MSA_ZeroFlows:
    case MSA_Empty:
    case MSA_PreSet:
      {
      ResetData(false);

      ////if (SpillExists())
      //  Spill.SetTotal(0.0);

      if (NoProcessIOs()<1)
        break;
      double P0 = AtmosPress(IODatum_Term(0));
      Set_JoinP(0, P0);
      Set_JoinP_Est(0, P0);
      Set_JoinP_Max(0, dNAN);
      Set_JoinP_MaxSet(0, dNAN);
      if (m_PFI.CnExists)
        m_PFI.Cn.ZeroMass();
      if (m_PFI.PfExists)
        m_PFI.Pf.Empty();
      RhoL=1000.0;
      RhoG=1.0;
      fHasSlope=False;

      Set_IOP_Flng(0, P0);
      Set_IOP_Self(0, P0);
      Set_IOP_Est_Self(0, P0);
      Set_IOP_Est_Flng(0, P0);
      SetIOQm_In(0, 0.0);
      SetIOQmEst_In(0, 0.0);
      SetIOQmSpace_Self(0, 0.0);
      SetIOQmAvail_Self(0, 0.0);

      IOFlange(0)->ClrMakeUpAvail();
      IOFlange(0)->ClrMakeUpReqd();
      //IOFlange(0)->SetMakeUpReqd(dNAN);

      if (1)
        {//kga 18/2/2003
        IOConduit(0)->Model()->QualitiesSetState(RqdState);
        if (IOConduitIn(0))
          IOConduitIn(0)->Model()->QualitiesSetState(RqdState);
        if (IOFlange(0)->IsTear())
          {
          /*
          Do we need this???
          SpImage *pImg = NULL;
          pImg = IOFlange(0)->TearImage();
          if (pImg)
            pImg->Model()->QualitiesSetState(RqdState);
          */
          if (IOFlange(0)->ConduitIO())
            IOFlange(0)->ConduitIO()->Model()->QualitiesSetState(RqdState);
          }
        }

      IOFlange(0)->SetTearInitCounters(false);

      if (NoProcessIOs()<2)
        break;

      double P1 = AtmosPress(IODatum_Term(1));
      Set_IOP_Self(1, P1);
      Set_IOP_Flng(1, P1);
      Set_IOP_Est_Self(1, P1);
      Set_IOP_Est_Flng(1, P1);
      SetIOQm_Out(1, 0.0);
      SetIOQmEst_Out(1, 0.0);
      SetIOQmSpace_Self(1, 0.0);
      SetIOQmAvail_Self(1, 0.0);

      IOFlange(1)->ClrMakeUpAvail();
      IOFlange(1)->ClrMakeUpReqd();
      //IOFlange(1)->SetMakeUpReqd(dNAN);
      IOConduit(1)->Model()->QualitiesSetState(RqdState); //kga 18/2/2003
      }
      break;
    case MSA_EmptySpillTargets:
      ////if (SpillExists())
      //  Spill.SetTotal(0.0);
      break;
    case MSA_SteadyState:
      LogNote(FullObjTag(), 0, "SteadyState Undefined");
      break;
    case MSA_DynStatsRunInit:
      if (pStats)
        pStats->ResetStats();
      break;
    }
  };

//--------------------------------------------------------------------------

flag MN_Lnk::InitialiseSolution()
  {
  return 1;
  };

//--------------------------------------------------------------------------

void MN_Lnk::StartSolution()
  {

  for (int i=0; i<NoFlwIOs(); i++)
    IOConduit(i)->SetStateAction(/*IsImpSolution() ? IE_Null :*/ IE_SaveState);
  }

//---------------------------------------------------------------------------

bool MN_Lnk::PropagateNetInfo(CPropagateNetInfoCtrl & Ctrl, long IONo)
  {
  if (!FlwNode::DoPropagateNetInfo(Ctrl, IONo, true))
    return false;

  for (int i=0; i<NoFlwIOs(); i++)
    {
    if (i!=IONo)
      Nd_Rmt(i)->PropagateNetInfo(Ctrl, IOIONo_Rmt(i));
    }

  return true;
  };

//---------------------------------------------------------------------------

void MN_Lnk::ConfigureJoins()
  {
  const int JnId_Pipe=0;
  const int JnId_Spill=1;
  if (GetActiveHold())
    {
    SetIO_Open(0, JnId_Pipe, false, ESS_Required, m_iFlwPhaseRqd);
    SetIO_Open(1, JnId_Pipe, false, ESS_Required, m_iFlwPhaseRqd);
    SetJoin_HasVolume(0, false, false);
    }
  else if (m_PFI.Method==VFFX_Plugflow && m_PFI.Type!=PFT_Incompressible)
    {
    SetIO_Open(0, JnId_Pipe, false, ESS_Required, m_iFlwPhaseRqd);
    SetIO_Open(1, JnId_Pipe, false, ESS_Required, m_iFlwPhaseRqd);
    SetJoin_HasVolume(0, true, false);
    }
  else
    {
    SetIO_Join(0, JnId_Pipe, ESS_Allowed, m_iFlwPhaseRqd);
    SetIO_Join(1, JnId_Pipe, ESS_Allowed, m_iFlwPhaseRqd);

    if (fVapLocked)
      {
      //TODO Should be Set...
      for (int j=0; j<NIOFBs(0); j++)
        IOFB(0,j)->VThrottle(1.0);
      for (j=0; j<NIOFBs(1); j++)
        IOFB(1,j)->VThrottle(1.0);
      }

    for (int j=0; j<NIOFBs(0); j++)
      IOFB(0,j)->SetTwoPhase(fTwoPhase);
    for (j=0; j<NIOFBs(1); j++)
      IOFB(1,j)->SetTwoPhase(fTwoPhase);

    SetJoin_HasVolume(JnId_Pipe, m_PFI.Method!=VFFX_Off, fVolIsSmall);

    if (m_fFwdOnly && NoProcessIOs()>0)
      IOFB(0,0)->SetUniDirectionalFlow(True, IOFB(0,0)->UDFOpenP(), 0.0);

    }
  
  if (NoFlwIOs()>2 && IOId_Self(2)==IOId_Spill2Area)
    {
    ASSERT_ALWAYS(IOId_Self(2)==IOId_Spill2Area, "Spill not Connected correctly");
    SetIO_Direct(2, JnId_Spill);
    }
  };

//--------------------------------------------------------------------------

void MN_Lnk::StartStep()
  {
  // in case no calcs done
  if (NoFlwIOs()>0)
    IOFB(0,0)->PhD().KFact.dVal=0;
  if (NoFlwIOs()>1)
    IOFB(1,0)->PhD().KFact.dVal=0;
  m_QmSpilt = 0.0;
  }

// -------------------------------------------------------------------------

void MN_Lnk::InitFlowInfo()
  {
  MdlNode::InitFlowInfo();
  };

void MN_Lnk::StepFlowInfo()
  {
  MdlNode::StepFlowInfo();
  };

void MN_Lnk::EvalFlowInfoStart()
  {
  #if dbgFlowInfo
  dbgpln("EvalFlowS Link : %2i %s", NoFlwIOs(), FullObjTag());
  #endif
  if (fDoDbgBrk)
    { int xxx=0; };

  //if (NoProcessIOs()==2)
  //  {
  //  for (int i=0; i<NoProcessIOs(); i++)
  //    {
  //    for (int j=0; j<NIOFBs(i); j++)
  //      IOFB(i, j)->m_iDirnRel2Connect = (i==0 ? 1 : -1);
  //    //FlwNode * pNd=Nd_Rmt(i);
  //    for (j=0; j<NIOFBs_Rmt(i); j++)
  //      IOFB_Rmt(i, j)->m_iDirnRel2Connect = (i==0 ? -1 : 1);
  //    }
  //  }

  if (m_PFI.Method==VFFX_Plugflow)
    {
    m_PFI.MeasureProperties(m_PFI.m_SegPropsS, 0.0, 0.5);
    m_PFI.MeasureProperties(m_PFI.m_SegPropsE, 0.5, 1.0);
    }

  MdlNode::EvalFlowInfoStart();
  };

void MN_Lnk::EvalFlowInfoEnd()
  {
  MdlNode::EvalFlowInfoEnd();
  };

flag MN_Lnk::TestFlowInfo()
  {
  return MdlNode::TestFlowInfo();
  };

//--------------------------------------------------------------------------

void MN_Lnk::EvalJoinPressures(long JoinMask)
  {
  switch (NetMethod())
    {
    case NM_Probal:
      {
      bool VelBAD=false;
#if WITHPBDPSTUFF
      const double MinProBalP = 2.0; //hard-wired minimum outlet pressure (kPa)
      double dp=0.0;
      switch (iDP_Mode)
        {
        case LDP_Const_P      : dp=0.0; break;
        case LDP_Fixed_Drop   : dp=(FxddP); break;
        case LDP_Fixed_Boost  : dp=-(FxddP); break;
        case LDP_Darcy_DP     :
          {
          #define OldDarcy 0
          #if OldDarcy
          //PipeD = Max(1e-6, PipeD);
          //PipeScale = Min(PipeScale, PipeD-1e-9);
          //double Diam = PipeD - PipeScale; // m
          //double Rho = IOConduit(0)->Rho(som_ALL);
          //double SG  = Rho/1000.0;
          //double rhoArea = Max(1e-6, Rho * PI * Sqr(Diam)/4.0);
          //Vel = IOConduit(0)->QMass(som_ALL)/rhoArea;
          //double Vf       =  IOConduit(0)->MassFrac(som_Gas);
          //double TestVel  = Vf*CFlwBlkData::s_dVelLimitV + (1.0-Vf)*CFlwBlkData::s_dVelLimitSL;
          //VelBAD = (Vel>TestVel);
          //if (iCalc_K)
          //  {
          //  Visc  = Max (1e-12, Visc);  // Absolute viscosity, 1e-3Ns/m^2 for water at 20 deg C
          //  Re    = Max(1e-9, (Diam*Vel*Rho)/Visc);       //Reynold's number
          //  const double A = Pow((-2.457 * Lns(Pow(7.0/Re,0.9) + 0.27 * Rough/Diam)), 16);
          //  const double B = Pow(37530.0/Re, 16);
          //  double F_Fac = 8.0 * Pow((A + B),-0.125);
          //  //double F_Fac = 8.0 * Pow((Pow((8/Re), 12) + 1.0/Pow((A + B), 1.5)),0.0833); ??? from pipeeqns.cpp
          //  K = F_Fac * PipeL/Diam;
          //  }
          //else
          //  Re = 0.0;
          //dp = 0.5 * K * SG * Sqr(Vel);
          #else
          //NOTE:For two phase flow these calculations are not correct, but lets attempt to produce "sensible" numbers
          double Qm = IOConduit(0)->QMass(som_ALL); // m^3/s
          if (Qm>1.0e-9)
            {
            PipeD = Max(1e-6, PipeD);
            PipeScale = Min(PipeScale, PipeD-1e-9);
            const double Diam = PipeD - PipeScale; // m
            const double Rho  = IOConduit(0)->Rho(som_ALL);
            const double Vf   = IOConduit(0)->MassFrac(som_Gas);
            const double Area = PI*Diam*Diam/4.0;
            const double Len = Max(1.0e-12, PipeL + FitPipeL);
            Vel = Qm/GTZ(Rho*Area);
            const double TestVel  = Vf*CFlwBlkData::sm_dVelLimitV + (1.0-Vf)*CFlwBlkData::sm_dVelLimitSL;
            VelBAD = (Vel>TestVel);
            if (iCalc_K)
              {
              if (iCalc_FFac)
                {
                Visc = Max(1e-12, Visc);  // Absolute viscosity, 1e-3Ns/m^2 for water at 20 deg C
                Re   = Max(1e-9, (Diam*Vel*Rho)/Visc);       //Reynold's number
                const double A = Pow((-2.457 * Lns(Pow(7.0/Re,0.9) + 0.27 * Rough/Diam)), 16);
                const double B = Pow(37530.0/Re, 16);
                FricFac = 8.0 * Pow((A + B),-0.125);
                //FricFac = 8.0 * Pow((Pow((8/Re), 12) + 1.0/Pow((A + B), 1.5)),0.0833); ??? from pipeeqns.cpp
                }
              K = FricFac * Len/Diam;
              }
            const double Ktotal = K + KMinorLoss;
            double DPv=0.0,DPl=0.0;
            if (Vf>=0.01)
              {//calculate vapour pressure drop
              //compress flow Eqn 1.7 CRANE
              const double QmV   = IOConduit(0)->QMass(som_Gas);
              const double P1    = IOP_Flng(0)*1000.0; //Pa
              const double AreaV = Area*Vf;
              const double spVol = 1.0/Rho;
              //const double X     = Diam*AreaV*AreaV/(spVol*F_Fac*PipeL);
              //const double X     = Diam*AreaV*AreaV/(spVol*K*Diam); // K*Diam=F_Fac*PipeL
              const double X     = AreaV*AreaV/(spVol*Ktotal); // cancel Diam
              const double Y     = P1*P1 - QmV*QmV*P1/X;
              double P2          = Sqrt(GTZ(Y));
              if (P2<P1*0.05)
                {
                P2=P1*0.05; //limit P2 to 5% of P1
                }
              DPv=(P1-P2)*0.001; // to kPa
              }
            if (Vf<=0.99)
              {//calculate slurry pressure drop
              const double SG = Rho/1000.0;
              const double dpHead = SG*Gc*PipeDeltaZ; //+ve DeltaZ indicates pipe outlet is higher than inlet
              DPl = dpHead + (Ktotal / 2.0 * SG * Sqr(Vel));
              }
            if (Vf>0.99)
              {
              dp = DPv;
              }
            else if (Vf<0.01)
              {
              dp=DPl;
              }
            else
              {//do something """reasonable""" for two phase flow
              dp=DPv*Vf + DPl*(1.0-Vf);
              }
            }
          else
            {
            Vel = 0.0;
            dp = 0.0;
            }
          #endif
          break;
          }
        //case LDP_Linear_DP    :
        //  {
        //  const double SQm=IOConduit(0)->QMass(som_SL);
        //  const double VQm=IOConduit(0)->QMass(som_Gas);
        //  dp = SQmdP*SQm/SQmCap+VQmdP*VQm/VQmCap;
        //  break;
        //  }
        //case LDP_SquareLaw_DP :
        //  {
        //  const double SQm=IOConduit(0)->QMass(som_SL);
        //  const double VQm=IOConduit(0)->QMass(som_Gas);
        //  dp=SQmdP*Sqr(SQm/SQmCap)+VQmdP*Sqr(VQm/VQmCap);
        //  break;
        //  }
        case LDP_BRC_DP://Bernoulli Rouse Corrected:
          {
          /*  Utilise l'équation de Bernoulli pour le calcul de la perte de charge
    			    pour un fluide incompressible. On assume que les pertes de charge
        			pour la différence d'élévation sont négligible.
              Use the equation of Bernoulli for the calculation of the pressure loss
              for an incompressible fluid. One assumes that the pressure losses for
              the difference in rise are negligible. A correction is applied by
              using a calculation in:
        			H. Rouse, Engineering Hydraulics, Wiley, 1961, PP 388-391.

; ======================================================================================
; Definitions:
	DOUBLE Leq("L","m")*		; Longeur équivalente de tuyauterie, mètre.
                            		; Equivalent Longor of piping, measures.
	DOUBLE ID_po("L","in")*		; Diamètre interne de la tuyauterie, pouces.
                             		; Diameter interns piping, inches.
	DOUBLE Tartre_po("L","in")*	; Épaisseur moyenne de la tartre dans la tuyaterie, pouce.
                               		; Average thickness of the tartar in the tuyatery, inch.
	DOUBLE f*		; Facteur de friction Fanning, #.
                		; Factor of Fanning friction
	DOUBLE ID		; Diamètre disponible, mètre.
               			; Diameter available, meter.
	DOUBLE k*@		; Rapport de Cv et Cp de la vapeur aux conditions d'entrées.
                 		; Ratio of Cv and CP of the vapor in the conditions of entries.
	DOUBLE Qv		; Débit volumétrique de vapeur, m3/h.
               			; Volumetric vapor flow, m3/h.
	DOUBLE Vitesse*@	; Vitesse de la vapeur dans la tuyauterie, m/s.
                     		; Speed of the vapor in piping, m/s.
	DOUBLE P_ent		; Pression d'entrée, kPa.
                    		; Pressure of entry, kPa.
	DOUBLE rho		; Densité de la vapeur, kg/m3.
                  		; Density of the vapor, kg/m3.
	DOUBLE deltaPi*@	; Perte de charge gaz incompressible, kPa.
                      		; Incompressible pressure loss gas, kPa.
	DOUBLE deltaPc*@	; Perte de charge corrigé pour gaz compressible, kPa.
                      		; Pressure loss corrected for compressible gas, kPa.
	DOUBLE r*@		; Rapport des pressions d'entrée et de sortie.
                  		; Report/ratio of the pressures of entry and exit.
	DOUBLE err*@		; Erreur pour gaz compressible, fraction.
                    		; Error for compressible gas, fraction.
	LONG ModeCal*		; Mode de calcul, 0 ne fait pas le calcul
				;		  1 le calcul se fait.


; ======================================================================================
; Lecture de valeurs:
	rho = GetTag("VapD140kPaUE.Rho (kg/m^3)")
	k = GetTag("VapD140kPaUE.Qi.CpCv")
	P_ent = GetTag("VapD140kPaUE.Pi (kPag)")
	Qv = GetTag("VapD140kPaUE.Qv (m^3/h)")


; ======================================================================================
; Calculs:
	if (ModeCal == 1)				; On calcul delta P.
		ID = (ID_po - Tartre_po) * 0.0254	; Calcul et conversion en m.
	; Calcul delta P pour cas de gaz incompressible.
	; Calculation delta P for incompressible case of gas.
		vitesse = Qv / Pow(ID,2) * 4 / PI / 3600
		deltaPi = rho * Pow(vitesse,2) / 2 * 4 * f * Leq / ID /1000
		r = (P_ent-deltaPi)/P_ent

	; Calcul l'erreur pour un gaz compressif.
	; Calculation the error for a compressive gas
		err = (1-k/(k+1)*(1-Pow(r,(k+1)/k))/(1-r)/(1+2*ID/k/f/Leq*ln(1/r)))

	; Calcul du delta P corrigée.
	; Calculation of the delta P corrected.
		deltaPc = deltaPi/(1-err)
	  else
	; Aucune perte de charge.
		deltaPc = 0
	endif

; ======================================================================================
; Transmetla valeurs:
;
	SetTag("VapD140kPaUE.Fxd_dP (kPa)",deltaPc)

          */
          double Qm       = IOConduit(0)->QMass(som_ALL); // m^3/s
          if (Qm>1.0e-9)
            {
            PipeD = Max(1e-6, PipeD);
            PipeScale = Min(PipeScale, PipeD-1e-9);
            double Diam     = PipeD - PipeScale; // m
            double Qv       = IOConduit(0)->QVolume(som_ALL); // m^3/s
            double Rho      = IOConduit(0)->Rho(som_ALL); // kg/m^3
            double k        = IOConduit(0)->CpCv(som_ALL);
      		  Vel = Qv / (Diam*Diam*PI)*4.0; // m/s
            double Vf       =  IOConduit(0)->MassFrac(som_Gas);
            double TestVel  = Vf*CFlwBlkData::sm_dVelLimitV + (1.0-Vf)*CFlwBlkData::sm_dVelLimitSL;
            VelBAD = (Vel>TestVel);
            //note: DarcyFricFact = 4 * FanningFricFact
            double deltaPi  = Rho/1000.0 * Vel*Vel / 2.0 * 4.0 * FricFac * PipeL / Diam; //kPa
            //deltaPi = incompressible pressure drop (requires reasonable input data for f, Diam & Length)
      	    // Now calculate the error for a compressive gas
            //double Pin_g    = IOP_Flng(0)-101.325;
		        //double r        = (Pin_g-deltaPi)/Pin_g;
            double Pin      = IOP_Flng(0);
            deltaPi = min(Pin - 10.0, deltaPi); //Pout MUST be greater than 10kPa !!!
            double r        = (Pin - deltaPi)/NZ(Pin);
            if (r>1.0)
              r = 1.0; //Pout MUST be lower than Pin!!!
		        //double err      = 1.0 - k/(k+1) * (1.0-Pow(r, (k+1)/k)) / (1.0-r) / (1.0+2.0*Diam/k/FricFac/PipeL*log(1.0/r));
		        double err      = 1.0 - k/(k+1) * (1.0-Pow(r, (k+1)/k)) / (1.0-r) / (1.0+2.0*Diam/k/FricFac/4.0/PipeL*log(1.0/r));
/*double Pin_     = IOP_Flng(0);
double r_       = (Pin_ - min(deltaPi,Pin_*0.1))/Pin_;
double err_     = 1.0 - k/(k+1) * (1.0-Pow(r, (k+1)/k)) / (1.0-r) / (1.0+2.0*Diam/k/FricFac/4.0/PipeL*log(1.0/r));
double err_a    = 1.0 - k/(k+1) * (1.0-Pow(r_, (k+1)/k)) / (1.0-r_) / (1.0+2.0*Diam/k/FricFac/PipeL*log(1.0/r_));
double err_b    = 1.0 - k/(k+1) * (1.0-Pow(r_, (k+1)/k)) / (1.0-r_) / (1.0+2.0*Diam/k/FricFac/4.0/PipeL*log(1.0/r_));
double dp_ = deltaPi/NZ(1.0-err_);
double dp_a = deltaPi/NZ(1.0-err_a);
double dp_b = deltaPi/NZ(1.0-err_b);*/
            if (err<0.0)
              err = 0.0; //corrected pressure drop should be greater than incompressible pressure drop!
            // Calculation of the delta P corrected.
		        dp = deltaPi/NZ(1.0-err);
            dp = min(Pin - 10.0, dp); //Pout MUST be greater than 10kPa !!!
            }
          else
            {
            Vel = 0.0;
            dp = 0.0;
            }
          break;
          }
        }
#endif

      IOFB(0, 0)->SetQmFree();

      double Po;
#if WITHPBDPSTUFF
      double Pi;
      if (IOFlange(0)->IsTear())
        {
        // Do Nothing : The flange Tearing will Transfer the Pressure Info
        Pi = IOP_Self(0);
        Po = (iDP_Mode==LDP_Const_P ? LnkP_S : Pi)-dp;
        }
      else
        {
        Pi = IOP_Flng(0);
        Set_IOP_Self(0, Pi);
        Po = (iDP_Mode==LDP_Const_P ? LnkP_S : Pi)-dp;
        }
      if (Po<MinProBalP)
        {
        Po = MinProBalP;
        dp = Pi - Po;
        }
      if (IOFlange(0)->IsTear())
        {
        // Do Nothing : The flange Tearing will Transfer the Pressure Info
        Po = (iDP_Mode==LDP_Const_P ? LnkP_S : IOP_Self(0))-dp;
        }
      else
        {
        Set_IOP_Self(0, IOP_Flng(0));
        Po = (iDP_Mode==LDP_Const_P ? LnkP_S : IOP_Flng(0))-dp;
        }
      PB_dp = dp;
#else
      if (IOFlange(0)->IsTear())
        Po = IOP_Self(0);
      else
        {
        Set_IOP_Self(0, IOP_Flng(0));
        Po = IOP_Flng(0);
        }
#endif
      //Set_IOP_Self(0, IOP_Flng(0));
      Set_IOP_Self(1, Po);
      Set_IOP_Flng(1, Po);
      // SetJoinPress(DynSoln, PB, 0.5*(IOP_Self(0)+IOP_Self(1)));

      #if dbgMLink
      if (dbgEvalJoinPress(Tag()))
        dbgpln("EBP: %12.2f  %12.2f  %12.2f  %12.2f  %s < %s < %s ", IOP_Flng(0), IOP_Self(0), IOP_Self(1), IOP_Flng(1), Nd_Rmt(0)->Tag(), Tag(), Nd_Rmt(1)->Tag());
      #endif

      Set_JoinP(0, 0.5*(IOP_Self(0)+IOP_Self(1)));
      if (fDoDbgBrk)
        {
        double XX=0.5*(IOP_Self(0)+IOP_Self(1));
        double YY=0.5*(IOP_Flng(0)+IOP_Flng(1));
        int xxx=0;
        }
      SetCI(3, VelBAD);
      break;
      }
    case NM_Dynamic:
      {
      if (fDoDbgBrk)
        { int xxx=0; }
      flag DoXfer=XferFlowMode();

      if (m_MFB.On())
        {
#if WITHMULTIFLWBLK
        m_MFB.EvalJoinPressures(DoXfer, fAppRhoH);
#endif
        }
      else
        {
        for (int i=0; i<NoFlwIOs(); i++)
          for (int j=0; j<NIOFBs(i); j++)
            {
            IOFB(i,j)->SetApplyStaticHead(fAppRhoH);
            IOFB(i,j)->SetFindStaticHead(fFindRhoH);
            }
        if (GetActiveHold())
          {
          Set_JoinP(0, SrcSnk.dPress);
          Set_IOP_RhoH_Self(0, SrcSnk.dPress, 1000.0, 0.0);
          Set_IOP_RhoH_Self(1, SrcSnk.dPress, 1000.0, 0.0);
          }
        else if (m_PFI.Method==VFFX_Plugflow && m_PFI.Type!=PFT_Incompressible)
          {
          Set_IOP_RhoH_Self(0, m_PFI.SrcPressure, 1000.0, 0.0);
          Set_IOP_RhoH_Self(1, m_PFI.DstPressure, 1000.0, 0.0);
          }
        else
          Set_JoinP(0, 0.5*(IOP_Self(0)+IOP_Self(1)));

        if (fDoDbgBrk)
          {
          double XX=0.5*(IOP_Self(0)+IOP_Self(1));
          double YY=0.5*(IOP_Flng(0)+IOP_Flng(1));
          int xxx=0;
          }
        }
      }
      break;
    }

  };


//--------------------------------------------------------------------------

void MN_Lnk::InitLocals()
  {
  }

//--------------------------------------------------------------------------

flag MN_Lnk::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  if (m_Spill.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (m_Src.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
  if (m_Snk.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
#if WITHLINKBLEED 
  if (m_Bleed.EvalFlowEquations(Task, pProps, IONo, FE, LnkNo))
    return true;
#endif

  FlwBlk & FB=*IOFB(IONo, FE);
  if (fDoDbgBrk)
    { int xxx=0; };

  flag DoXfer=XferFlowMode();

  switch (Task)
    {
    case FET_GetMode:
      {
      if (IONo==0 && FE==0)
        FB.EvalGetModeRqd(FB.FlowMode(), pProps, cioOn()!=0, fIsDegFree!=0, 1.0);// .EvaluateFlwEqn(Task, pProps, cioOn()?1.0:0.0, NULL, NULL);//Fill);
      else if (cioOn())
        FB.SetQmFree();
      else
        FB.SetQmReqd(0.0);

      //if (IONo==0 && FE==0)
      //  {
      //  if (FB.InXferNet() || DoXfer)
      //    {
      //    if (DoXfer)
      //      FB.SetXferMode();
      //    else
      //      FB.SetQmFree();
      //    if (cioOn())
      //      {
      //      double Cap = Valid(xMax_QmCap) ? xMax_QmCap : MaxXfCap;
      //      if (Valid(xRqd_QmCap))
      //        Cap = Range(xMin_QmCap, xRqd_QmCap, Cap);
      //      Cap *= (m_iLF_Direction==LFD_Reverse ? -1.0 : 1.0);
      //      FB.SetXferCapacity(Cap);
      //      }
      //    else
      //      FB.SetXferCapacity(0.0);
      //    return True;
      //    }
      //  else if (!cioOn())
      //    FB.SetQmReqd(0.0);
      //  else if (Valid(xReqd_Qm))
      //    FB.SetQmReqd(xReqd_Qm, fIsDegFree);
      //  else
      //    FB.SetQmFree();
      //  }
      //else
      //  {
      //  if (cioOn())
      //    FB.SetQmFree();
      //  else
      //    FB.SetQmReqd(0.0);
      //  }
      break;
      }
    case FET_SetQm:
      {
      FB.SetDPb(0.0, 0.0);
      FB.SetDPbX(0.0);
      FB.SetDPq(0.0, 0.0);
      break;
      }
    case FET_CalcDP:
      {
      if (DoXfer)
        {
        FB.SetDPb(0.0, 0.0);
        FB.SetDPbX(0.0);
        FB.SetDPq(0.0, 0.0);
        }
      else
        {
        if (m_PFI.Method==VFFX_Plugflow)
          {
          CSpPropInfo *pSegProps=(IONo==0)? &m_PFI.m_SegPropsS[0]:&m_PFI.m_SegPropsE[0];
          if (m_PFI.Type==PFT_Incompressible)
            {
            IOFB(IONo, FE)->EvaluateFlwEqn(Task, pSegProps, cioOn()!=0, fIsDegFree!=0, 1.0, NULL, NULL);//Fill);
            }
          else
            {
            IOFB(IONo, FE)->EvaluateFlwEqn(Task, pSegProps, cioOn()!=0, fIsDegFree!=0, 1.0, NULL, NULL);//Fill);
            }
          }
        else
          IOFB(IONo, FE)->EvaluateFlwEqn(Task, pProps, cioOn()!=0, fIsDegFree!=0, 1.0, NULL, NULL);//Fill);
        double K1=IOFB(0,0)->GetKFact();
        double K2=IOFB(1,0)->GetKFact();
        double K=dNAN;
        flag v1=Valid(K1);
        flag v2=Valid(K2);
        if (v1 && v2)
          K=K1+K2;
        else if (v1)
          K=K1;
        else if (v2)
          K=K2;
        m_FEP.SetKFact(K);;
        }
      break;
      }
    }
  return True;
  };

//--------------------------------------------------------------------------

void MN_Lnk::EvalProductsInit(EvalProductsInitTasks Task)
  {
  EvalProductsInit_Link(Task);
  }

//--------------------------------------------------------------------------

inline double DiffFrac(double A, double B) { return (A-B)/Max(1.0e-9, Max(fabs(A), fabs(B))); };

//--------------------------------------------------------------------------
#define WithPoFix 0
#define DBGCHG    0

void MN_Lnk::EvalProducts(long JoinMask)
  {

#if DBGCHG
  StkSpConduit TVi("TVi", "ABC", this);
  if (IOConduitIn(0))
    TVi->QCopy(*IOConduitIn(0));
  StkSpConduit Vi("Vi", "ABC", this);
  StkSpConduit Vo("Vo", "ABC", this);
  Vi->QCopy(*IOConduit(0));
  Vo->QCopy(*IOConduit(1));
#endif
  if (fDoDbgBrk)
    { int xxx=0; }

  const flag DoXfer=XferFlowMode();
  if (SolveDynamicMethod() && !DoXfer && (m_PFI.Method==VFFX_Plugflow))
    {
    if (GetActiveHold())
      {
      }
    else
      {
      const int iIn  = 0;
      const int iOut = 1;
      SpConduit & Fi=*IOConduit(iIn);
      SpConduit & Fo=*IOConduit(iOut);
      const double PIn=IOP_Flng(iIn);
      const double POut=IOP_Flng(iOut);
      double IOQInEst=IOQmEst_In(iIn);
      double IOQOutEst=IOQmEst_Out(iOut);

      double VelS=IOFB(0,0)->Velocity();
      double VelE=-IOFB(1,0)->Velocity();

      dbgpln("MLnk:EvapProd:Fi: T%6.2f P:%6.2f M:%6.2f", Fi.Temp(), Fi.Press(), Fi.QMass());
      dbgpln("              Fo: T%6.2f P:%6.2f M:%6.2f", Fo.Temp(), Fo.Press(), Fo.QMass());

      IOQOutEst=m_PFI.DoPlugFlowProducts(Fi, Fo, IOQInEst, IOQOutEst, PIn, POut, VelS, VelE);
      dbgpln("              Fo: T%6.2f P:%6.2f M:%6.2f", Fo.Temp(), Fo.Press(), Fo.QMass());
      }
    }
  else
    {
    //CHECK Perhaps for ProBal with Qi.Qm<UsableMass make Qo=Qi? but what about Tear, ehx, rb, pressure drop, etc?
    int iIn  = TwoIOInIndex(m_iLF_Direction==LFD_Reverse);
    int iOut = OtherEnd(iIn);
    SpConduit & Fi=*IOConduit(iIn);
    SpConduit & Fo=*IOConduit(iOut);
    double FIQm=Fi.QMass();
    if (GetActiveHold())
      {
      if (SrcSnk.pImage)
        {
        Fo.QSetM(*SrcSnk.pImage, som_ALL, IOQmEst_Out(iOut), SrcSnk.dPress);
        Fo.SetTempPress(SrcSnk.dTemp, SrcSnk.dPress);
        }
#if WITHSRCSNKDIFFS
      SpConduit & F0=*IOConduit(0);
      SpConduit & F1=*IOConduit(1);
      SrcSnk.dQmDiff=DiffFrac(F0.QMass(), F1.QMass());
      SrcSnk.dTempDiff=DiffFrac(F0.Temp(), F1.Temp());
      for (int s=0; s<SDB.No(); s++)
        SrcSnk.pSpDiff->SetMass(s, DiffFrac(F0.Qm(s), F1.Qm(s)));
#endif
      }
    //else if (SolveDirectMethod() && !IsUsableMass(FIQm))
    //{//do NOTHING!!!, set output=input, ignore m_RB, ehx, etc???
    //if (IOFlange(0)->IsTear())//if (IOFlange(0)->IsSystemTear())
    //  {
    //  int xxx=0;//Do something else???
    //  }
    //Fo.QSetTraceMass();
    //const double POut=IOP_Flng(iOut);
    //Fo.SetPress(POut);
    //Fo.SetTemp(Fi.Temp());
    //}
    else // Not Held
      {
      if (fDoDbgBrk)
        { int xxx=0; }

      double PIn=IOP_Flng(iIn);
      double POut=IOP_Flng(iOut);
      double IOQInEst=IOQmEst_In(iIn);
      double IOQOutEst=IOQmEst_Out(iOut);

      if (NetProbalMethod())
        {
        IOQInEst=(IsUsableMass(FIQm) ? FIQm : 0.0);
        IOQOutEst=IOQInEst;
        }
      else if (SolveInlineMethod() || (SolveBufferedMethod() && (IOFlange(iIn)->TearType()==TT_Break)))
        {
        //dbgpln("EvalProducts A  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
        //use IOQOutEst as defined
        if (IOFlange(iIn)->IsTear() &&  /*GlblDynamicMode() &&*/ (Fi.QMass()<=TraceMass) && 
          (IOQInEst>TraceMass) && m_pLastFlw)
          {
          //dbgpln("EvalProducts B  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
          // Put Something into the flow (ie whatever last flowed through the pipe)
          Fi.QSetM(*m_pLastFlw, som_ALL, IOQInEst);
          FIQm=Fi.QMass();
          IOQInEst=(IsUsableMass(FIQm) ? FIQm : 0.0);
          //dbgpln("    <<: %10.2f %10.2f %10.2f %s", Fi.Temp(), Fi.Press(), Fi.QMass(), Fi.FullObjTag());        
          }
        //else if (!DoXfer)
        //  {
        //  // this was put in because of closed recycles - it upsets other stuff
        //  //Fi.QScaleMass(som_ALL, IOQInEst/GTZ(Fi.QMass()));
        //  //dbgpln("EvalProducts C  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
        //  }

        IOQInEst=(IsUsableMass(FIQm) ? FIQm : 0.0);
        }
      else
        {
        //dbgpln("EvalProducts D  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
        // this will not work for an overflow stream (flow limited) which is also a tear stream
        //TODO THIS LINE NEEDS TO BE FIXED FOR FlowLimited lines [overflow stream]
        if (m_PFI.Method!=VFFX_Off)
          {
          IOQInEst=(IsUsableMass(FIQm) ? FIQm : 0.0);
          IOQOutEst=IOQInEst;
          }
        else if ((m_PFI.Method==VFFX_Off && !IOFlange(iIn)->IsTear()) || DoXfer)
          //  else if ((m_PFI.Method==VFFX_Off || DoXfer) && !IOFlange(iIn)->IsTear()) //for MG
          //  else if ((!fHasVolume || DoXfer))                             //for MatHand8
          //  else if ((!fHasVolume && !IOFlange(iIn)->IsTear()) || DoXfer)
          {
          IOQInEst=(IsUsableMass(FIQm) ? Min(FIQm, IOQInEst): 0.0);
          IOQOutEst=IOQInEst;
          }
        }

      //dbgpln("EvalProducts E  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());

      Fi.SetSQFlags(SQF_RemoveOK, false);
      Fo.SetSQFlags(SQF_RemoveOK, true);

      SetCI(1, FIQm<0.0);

      if (fDoDbgBrk)
        { int xxx=0; }

      double TPrev=Fo.Temp();
      
      //m_FTB.BeginEvalProducts(Fi);

      //dbgpln("---------------------------");
      //dbgpln("A  %12.3f  %12.3f  %12.3f  %12.3f", Fi.Temp(), Fi.Press(), Fo.Temp(), Fo.Press());

      if (IOQInEst<0.0 || IOQOutEst<0.0)
        {
        // Both In or Both Out
        // Adjust Input Flow if FlowOut
        Fi.QSetM(Fi, som_ALL, fabs(IOQInEst), PIn);
        Fo.QSetM(Fo, som_ALL, fabs(IOQOutEst), POut);
        // IsoThermal;
#if WithPoFix
        Fo.SetTempPress(Fi.Temp(), POut);
#else
        Fo.SetTemp(Fi.Temp());
#endif

        m_FTB.BeginEvalProducts(Fo);
        }
      //else if (IO_Zero(0))
      //  {
      //  if (FlowAsDrawn)
      //    {
      //    Fo.QSetTraceMass();
      //    Fo.SetPress(POut);
      //    }
      //  else
      //    {
      //    Fi.QSetTraceMass();
      //    Fo.QSetTraceMass();
      //    Fi.SetPress(PIn);
      //    Fo.SetPress(POut);
      //    }
      //  }
      else
        {
      //dbgpln("EvalProducts F  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
        // Remember the tear values for initialisation if a System Tear
        if (IOFlange(0)->IsSystemTear())
          {
      //dbgpln("EvalProducts G  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
          //can this be improved for (!IsUsableMass(IOQInEst)) ...?
          if (!IOFlange(0)->TearImageExists())
            IOFlange(0)->SetTearImage(this);
          SpImage & Img = *IOFlange(0)->TearImage();
          Img.SetF(Fi, som_ALL, 1.0, IOP_Self(iIn));
          Img.SetQMode(SPI_QModeQm);
          Img.SetQmRqdTo(FIQm, true, true);
          Img.SetTRqdTo(Fi.Temp());
          Img.SetPRqdTo(IOP_Self(iIn));
          Img.Model()->SetMassScale(1.0);

          if (NetProbalMethod() || m_bAdjustToEstFlow)
            Fi.QAdjustQmTo(som_ALL, IOQInEst);
          Fi.SetPress(PIn);
          }
        if (IOFlange(iIn)->IsTear() &&  GlblDynamicMode() && (Fi.QMass()<=TraceMass) && (IOQInEst>TraceMass) && m_pLastFlw)
          {
      //dbgpln("EvalProducts H  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
          // Put Something into the flow (ie the last conetent)
          Fi.QSetM(*m_pLastFlw, som_ALL, IOQInEst);
          //dbgpln("    <<: %10.2f %10.2f %10.2f %s", Fi.Temp(), Fi.Press(), Fi.QMass(), Fi.FullObjTag());        
          }

      //dbgpln("EvalProducts I  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
        double Ti=Fi.Temp();
        if (IOFlange(0)->IsTear())
          PIn=Fi.Press();
        Fi.SetTempPress(Ti, PIn);
        //dbgpln("A1 %12.3f  %12.3f  %12.3f  %12.3f", Fi.Temp(), Fi.Press(), Fo.Temp(), Fo.Press());
        //Fo.QSetF(Fi, som_ALL, 1.0, POut);
        if (DoXfer)
          {
      //dbgpln("EvalProducts J  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
          const double QmIn = Fi.QMass();
          if (fabs(QmIn)>1.0e-9 && QmIn-IOQOutEst>1.0e-12)
            {
            switch (m_iXferCapOption)
              {
              case XCO_Spill:
                Fo.QSetF(Fi, som_ALL, IOQOutEst/QmIn, PIn);
                m_QmSpilt=QmIn-Fo.QMass();
                if (QmIn-IOQOutEst>1.0e-6)//only complain for "significant" spill
                  {
                  Strng ErrMsg;
                  ErrMsg.Set("N\tMassflow into Pipe(%.3g) > Capacity(%.3g).  Extra Spilt", QmIn, IOQOutEst);

                  if (gs_SpillOption & SpillReports_Cnds)
                    SetCI(7, "%s", ErrMsg());
                  if (gs_SpillOption & SpillReports_Msgs)
                    LogWarning(FullObjTag(), 0, "%s", ErrMsg());//Massflow into Pipe > Capacity.  Extra Spilt");
                  if (!gs_FSIdleRequested && (gs_SpillOption & SpillReports_Idle) && (gs_SpillOption & SpillReports_Links))
                    {
                    LogWarning(FullObjTag(), LF_DoAfxMsgBox|LF_Exclamation, "Solver IDLE request due to spillage");
                    gs_FSIdleRequested=true;
                    ExecObj()->XIdle();
                    }
                  }
                ClrCI(8);
                m_Spill.Cd.QSetM(Fi, som_ALL, m_QmSpilt);
                break;
              case XCO_Accept:
                Fo.QSetF(Fi, som_ALL, 1.0, PIn);
                ClrCI(7);
                if (QmIn>IOQOutEst+1.0e-6)
                  {
                  Strng ErrMsg;
                  ErrMsg.Set("N\tMassflow into Pipe(%.3g) > Capacity(%.3g).  Capacity Ignored.  Extra Spilt", QmIn, IOQOutEst);
                  SetCI(8, "%s", ErrMsg());
                  }
                else
                  ClrCI(8);
                SpillClear();
                break;
              }
            }
          else
            {
      //dbgpln("EvalProducts K  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
            Fo.QSetF(Fi, som_ALL, 1.0, PIn);
            ClrCI(7);
            ClrCI(8);
            SpillClear();
            }
          m_Closure.SetTestDone(true);
          }
        else
          {
      //dbgpln("EvalProducts L  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
          if (0 && GlblDynamicMode())
            Fo.QSetM(Fi, som_ALL, IOQOutEst, PIn);
          else
            Fo.QSetF(Fi, som_ALL, 1.0, PIn);
          }
        //dbgpln("A2 %12.3f  %12.3f  %12.3f  %12.3f", Fi.Temp(), Fi.Press(), Fo.Temp(), Fo.Press());

        m_FTB.BeginEvalProducts(Fo);
      //dbgpln("EvalProducts M  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());

        if (!NetProbalMethod() && !DoXfer)    // Iso thermal etc
          IOFB(iIn,0)->EvalProducts(IOP_Flng(iIn), IOP_Self(iIn), Fo, m_FTB);
        //dbgpln("A3 %12.3f  %12.3f  %12.3f  %12.3f", Fi.Temp(), Fi.Press(), Fo.Temp(), Fo.Press());
      //dbgpln("EvalProducts M1 %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());

        if (NetProbalMethod()) // Limit Rates of Temp Change
          {
#if WithPoFix
          Fo.SetTempPress(Fo.Temp(), POut); // IsoThermal
#endif
          }
        else
          {
          double VelS=IOFB(0,0)->Velocity();
          double VelE=IOFB(1,0)->Velocity();
      //dbgpln("EvalProducts M2 %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
          IOQOutEst=m_PFI.DoPlugFlowProducts(Fi, Fo, IOQOutEst, IOQOutEst, PIn, POut, VelS, VelE);
      //dbgpln("EvalProducts M3 %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());
          if (!DoXfer)
            IOFB(iOut,0)->EvalProducts(IOP_Self(iOut), IOP_Flng(iOut), Fo, m_FTB);
          //Fo.SetPress(POut);
          }
        }
      //dbgpln("B  %12.3f  %12.3f  %12.3f  %12.3f", Fi.Temp(), Fi.Press(), Fo.Temp(), Fo.Press());
      //dbgpln("EvalProducts N  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());

#if WITHBLOCKEVALUATOR
      double ActLen=m_MFB.On() ? 0.0 : m_FEP.ActLength();
      m_BlkEval.EvalProductsPipe(m_FTB, Fo, ActLen, m_FEP.Diam(), IOP_Flng(iOut)); 
#else
      if (m_iBlockSeq==BS_EHX_RB_VLE && EHX.Enabled())
        {
        m_FTB.AddEHXBegin();
        double ActLen=m_MFB.On() ? 0.0 : m_FEP.ActLength();
        EHX.EvalProductsPipe(Fo, ActLen, m_FEP.Diam());
        m_FTB.AddEHXEnd();
        }
      //dbgpln("EvalProducts O  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());

      if (m_RB.Enabled())
        {
        m_FTB.AddRBBegin();
        m_RB.EvalProducts(Fo);
        m_FTB.AddRBEnd();
        }
      //dbgpln("EvalProducts P  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());

      if (m_iBlockSeq==BS_RB_EHX_VLE && EHX.Enabled())
        {
        m_FTB.AddEHXBegin();
        double ActLen=m_MFB.On() ? 0.0 : m_FEP.ActLength();
        EHX.EvalProductsPipe(Fo, ActLen, m_FEP.Diam());
        m_FTB.AddEHXEnd();
        }
      //dbgpln("EvalProducts Q  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());

      if (m_VLE.Enabled())
        {
        m_FTB.AddVLEBegin();
        m_VLE.QPFlash(Fo, IOP_Flng(iOut), 0.0, VLEF_Null);
        m_FTB.AddVLEEnd();
        }
#endif
      //dbgpln("EvalProducts R  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());

      //dbgpln("C  %12.3f  %12.3f  %12.3f  %12.3f", Fi.Temp(), Fi.Press(), Fo.Temp(), Fo.Press());

      if (NetProbalMethod()) // Limit Rates of Temp Change
        {
        const double MaxPBTChange=50.0;
        if (fabs(TPrev-Fo.Temp())>MaxPBTChange)
          {
          double TRqd=Range(TPrev-MaxPBTChange, Fo.Temp(), TPrev+MaxPBTChange);
          Fo.SetTemp(TRqd);
          if (PBEqnCB().NoteBadEqnLimit())
            {
            Strng Tg;
            Tg.Set("%s.TempChange", FullObjTag());
            PBEqnCB().CollectBadEqnLimit(Fo.Temp(), Tg(), TTT_Unknown);//TTT_Link);
            }
          }
        }
      //dbgpln("EvalProducts S  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());

      //dbgpln("D  %12.3f  %12.3f  %12.3f  %12.3f", Fi.Temp(), Fi.Press(), Fo.Temp(), Fo.Press());
      m_FTB.EndEvalProducts();
      //dbgpln("E  %12.3f  %12.3f  %12.3f  %12.3f", Fi.Temp(), Fi.Press(), Fo.Temp(), Fo.Press());

      SetIOQm_In(iIn, Fi.QMass());//IOQInEst);
      SetIOQm_Out(iOut, Fo.QMass());//IOQOutEst);

      Fo.SetSQFlags(SQF_RemoveOK, false);
      Fi.SetDataSign(IOSign(0));
      Fo.SetDataSign(IOSign(0));
      //dbgpln("EvalProducts T  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());

      if (GlblDynamicMode() && (Fi.QMass()>TraceMass))
        {
        if (!m_pLastFlw)
          m_pLastFlw=new SpConduit("LastFlw", this, TOA_Embedded);
        m_pLastFlw->QSetF(Fi, som_ALL, 1.0);
        //dbgpln("LstFlw: %10.2f %10.2f %10.2f %s", m_pLastFlw->Temp(), m_pLastFlw->Press(), m_pLastFlw->QMass(), m_pLastFlw->FullObjTag());        
        }
      if (fDoDbgBrk)
        { int xxx=0; }

      if (SolveBufferedMethod() && !DoXfer)
        {
        double Err;
        SetCI(5, !ConvergedVV(IOQOutEst, Fo.QMass(), 1e-6, 1e-3, Err));
        }
      else
        ClrCI(5);
      //dbgpln("EvalProducts U  %15.6f %15.6f %s", Fi.QMass(), Fo.QMass(), FullObjTag());

#if dbgDumpSpcInfo
      flag Forcedbg=0;
      if (Forcedbg)
        dbgSpFlows("MLnk", Tag(), *IOConduit(iOut), 1);
#endif

      //#ifndef _RELEASE
      //      double T1=Fi.Temp();
      //      double T2=Fo.Temp();
      //      ASSERT_ALWAYS(Valid(T1) && Valid(T2), "NAN FOR TEMPERATURE")
      //#endif
      }
    }

  if (m_RB.Enabled())
    {
    //TODO Hz
    if (fDoDbgBrk)
      BreakPoint();
    m_Src.Set(*m_RB.MsSumSrc(), 0);//m_RB.HfSumSrc();
    m_Snk.Set(*m_RB.MsSumSnk(), 0);
    }
  else
    {
    m_Src.Sum.ZeroFlows();
    m_Snk.Sum.ZeroFlows();
    }

  if (m_Evap.Enabled())
    m_Snk.Add(m_Evap.DiscardCd());//m_RB.HfSumSrc();

#if DBGCHG
  #define DBGIT(A,B,D,T)  \
            dbgpln("EP %18.10f > %18.10f %+15.6f %+25.16f *e-6 %+25.16f *e-6 %s %s", \
            A, B, (B-A), 1000000*(B-A)/GTZ(Max(fabs(A), fabs(B))), 1000000*(B-A)/GTZ(Max(fabs(A), fabs(B))), D, T)

  SpConduit &Ri=*IOConduit(0);
  SpConduit &Ro=*IOConduit(1);

  dbgpln("%s : %s", "---------------------------------------------------------------", FullObjTag());
  char *Tmp="";
  if (IOConduitIn(0))
    {
    SpConduit &TRi=*IOConduitIn(0);
    DBGIT(TVi->QMass(),          TRi.QMass(),            "Qm", Tmp);
    DBGIT(TVi->Temp(),            TRi.Temp(),             "T",  Tmp);
    DBGIT(TVi->Press(),           TRi.Press(),            "P",  Tmp);
    DBGIT(TVi->MassFrac(som_Sol), TRi.MassFrac(som_Sol),  "Sf", Tmp);
    DBGIT(TVi->MassFrac(som_Liq), TRi.MassFrac(som_Liq),  "Lf", Tmp);
    }
  DBGIT(Vi->QMass(),           Ri.QMass(),             "Qm", Tmp);
  DBGIT(Vi->Temp(),            Ri.Temp(),              "T",  Tmp);
  DBGIT(Vi->Press(),           Ri.Press(),             "P",  Tmp);
  DBGIT(Vi->MassFrac(som_Sol), Ri.MassFrac(som_Sol),   "Sf", Tmp);
  DBGIT(Vi->MassFrac(som_Liq), Ri.MassFrac(som_Liq),   "Lf", Tmp);
  DBGIT(Vo->QMass(),           Ri.QMass(),             "Qm", Tmp);
  DBGIT(Vo->Temp(),            Ri.Temp(),              "T",  Tmp);
  DBGIT(Vo->Press(),           Ri.Press(),             "P",  Tmp);
  DBGIT(Vo->MassFrac(som_Sol), Ri.MassFrac(som_Sol),   "Sf", Tmp);
  DBGIT(Vo->MassFrac(som_Liq), Ri.MassFrac(som_Liq),   "Lf", Tmp);

  #undef DBGIT
#endif
  //if (fabs(IOConduit(0)->QMass()-IOConduit(1)->QMass())>0.1*IOConduit(0)->QMass())
  //  {
  //  dbgpln("MN_Lnk::EvalProducts Qi:%15.6f Qo:%15.6f %s", 
  //    IOConduit(0)->QMass(), IOConduit(1)->QMass(), FullObjTag());
  //  }
  };

//--------------------------------------------------------------------------

void MN_Lnk::EvalPBMakeUpReqd(long JoinMask)
  {
  int iIn=IO_In(0) ? 0 : 1;
  int iOut=(iIn+1)%2;
  double Rqd=IOFlange(iOut)->MakeUpReqd();
  if (Valid(dMkUpRqd))
    {
    if (Valid(Rqd))
      IOFlange(iIn)->SetMakeUpReqd(Max(dMkUpRqd, Rqd));
    else
      IOFlange(iIn)->SetMakeUpReqd(dMkUpRqd);
    }
  else
    IOFlange(iIn)->SetMakeUpReqd(Rqd);
  };

//--------------------------------------------------------------------------

void MN_Lnk::EvalPBMakeUpAvail(long JoinMask)
  {
  MakeUpLinkTransferAvail();
  };

//--------------------------------------------------------------------------

void MN_Lnk::EvalDerivs(long JoinMask)
  {
  int iIn  = (IO_In(0) ? 0 : 1);
  int iOut = ((iIn+1) % 2);

  if (fDoDbgBrk)
    { int xxx=0; }
  };

//--------------------------------------------------------------------------

void MN_Lnk::ConvergeStates(CConvergeStateBlk &CSB)
  {
  }

//--------------------------------------------------------------------------

void MN_Lnk::EvalDiscrete()
  {
  if (fDoDbgBrk)
    { int xxx=0; }

  bool VelBAD=false;
  bool QmBAD=false;
  if (SolveDynamicMethod())
    {
    ////if (SpillExists())
    //  {
    //  SpDirectCd & S=Spill;
    //  S.SetTotal(S.Total()+S.Cd.QMass()*ICGetTimeInc());
    //  }

    if (m_PFI.Method==VFFX_Plugflow)
      {
      bool Moved=true;//
      m_PFI.DoPlugFlowDiscrete(ICGetTimeInc(), Moved);
      UpdateSamples();
      }

    if ((NoFlwIOs()==2) && !XferFlowMode())
      {
      for (int i=0; i<2 && (!VelBAD || !QmBAD); i++)
        {
        for (int j=0; j<NIOFBs(i) && !VelBAD && !QmBAD; j++)
          {
          FlwBlk *pFB=IOFB(i,j);
          if (!pFB->InXferNet())
            {
            double Vf=pFB->VapVolFrac(pFB->m_Qm>0 ? &pFB->m_FlwProps[pFB->m_iFwdFlwProps] :
                                                  &pFB->m_FlwProps[pFB->m_iRevFlwProps]);
            double TestVel=Vf*CFlwBlkData::sm_dVelLimitV + (1.0-Vf)*CFlwBlkData::sm_dVelLimitSL;
            if (Valid(pFB->Velocity()))
              {
              double ActualVel=pFB->Velocity()*fabs(pFB->m_Qm/NZ(pFB->m_QmEst));
              if (ActualVel>TestVel)
                VelBAD=true;
              }
            else
              VelBAD=true;
            if (CFlwBlkData::sm_iShowQmDiffAs!=FBB_ShowAs_Off &&
                !CFlwBlkData::m_QmMatchTol.ConvergedVV(pFB->m_Qm, pFB->m_QmEst))
              QmBAD=true;
            }
          }
        }
      }
    SetCI(3, VelBAD);
    if (NoFlwIOs()>=2)
      {
      double RBSrc=0, RBSnk=0, Err;
      if (m_RB.Enabled())
        {
        RBSrc=m_RB.MsSumSrc()->Mass();
        RBSnk=m_RB.MsSumSnk()->Mass();
        }

      int iIn  = (IOQmEst_In(0)>0.0 ? 0 : 1);
      if (IOFlange(iIn)->TearType()!=TT_Break && !TestCISet(7) && !TestCISet(8))
        SetCI(6, !ConvergedVV(IOConduit(0)->QMass()+RBSrc-m_QmSpilt, IOConduit(1)->QMass()+RBSnk, 1e-6, 1e-3, Err));
      }
    }
  SetCI(4, QmBAD);
  }

//---------------------------------------------------------------------------

void MN_Lnk::EvalStatistics(eScdCtrlTasks Tasks)
  {
  if (pStats)
    pStats->ExecIns(this);
  MdlNode::EvalStatistics(Tasks);
  }

//---------------------------------------------------------------------------

flag MN_Lnk::EvalPressureSens()
  {
  if (fDoDbgBrk)
    { int xxx=0; }

  double SensV, SensL;
  if (fVolPrsFX)
    {
    SensV=1.0e30;
    if (m_PFI.CnExists && m_PFI.Cn.Mass()>1.0e-6)
      {
      double CurrentP=0.5*(IOP_Self(0)+IOP_Self(1));

      double PipeVol=Max(0.1, TotalArea*ActualLength);
      double Dens=m_PFI.Cn.Rho(som_ALL);
      double Mass=PipeVol*Dens;
      double VFrac=m_PFI.Cn.MassFrac(som_Gas);
      // Assume All Vapour
      SensV=CurrentP*(1.0/Mass);
      }
    //TODO Sens to Vapours & liquids to be sorted out
    SensL=SensV*0.001;
    }
  else
    SensV=SensL=0.0;

  Set_IO_PSensVL(0, SensV, SensL);
  Set_IO_PSensVL(1, SensV, SensL);
  return True;
  };

//--------------------------------------------------------------------------

flag MN_Lnk::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tNegative Flows";                                   return 1;
    case 2: pS="W\tIncorrect connections";                            return 1;
    case 3:
      {
      switch (CFlwBlkData::sm_iShowVelLimitAs)
        {
        case FBB_ShowAs_Note:   pS="N\tVelocity Exceeds Limit";       return 1;
        case FBB_ShowAs_Warn:   pS="W\tVelocity Exceeds Limit";       return 1;
        case FBB_ShowAs_Error:  pS="E\tVelocity Exceeds Limit";       return 1;
        }
      return MdlNode::CIStrng(No, pS);
      }
    case 4:
      {
      switch (CFlwBlkData::sm_iShowQmDiffAs)
        {
        case FBB_ShowAs_Note:   pS="N\tEstimated flow not Achieved";  return 1;
        case FBB_ShowAs_Warn:   pS="W\tEstimated flow not Achieved";  return 1;
        case FBB_ShowAs_Error:  pS="E\tEstimated flow not Achieved";  return 1;
        }
      return MdlNode::CIStrng(No, pS);
      }
    case 5: pS="W\tMassflow achieved thru Pipe != Estimated flow (Pressure dependant)"; return 1;
    case 6: pS="W\tMassflow into Pipe != Massflow out of Pipe";       return 1;
    case 7: pS="N\tMassflow into Pipe > Capacity.  Extra Spilt";      return 1;
    case 8: pS="N\tMassflow into Pipe > Capacity.  Capacity Ignored"; return 1;
    case 9: pS="W\tSpill Target not found"; return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  };

//--------------------------------------------------------------------------

#if DOPBSMOOTHING
void MN_Lnk::EvalMsAGDatas()
  {
  MsJoins.SetSize(1);

  if (m_RB.Enabled())
    m_RB.GetMsAGData(MsJoins[0]);
  else
    {
    //MsJoins[0].Clear();
    MsJoins[0].m_fDataIsValid=false;
    MsJoins[0].m_ConstBG.Clear();
    MsJoins[0].m_ConstAG.Clear();
    MsJoins[0].m_PropBG.Clear();
    MsJoins[0].m_PropAG.Clear();
    MsJoins[0].m_Gamma.Clear();
    }

  //MsAGDatas.SetSize(0);
  MdlNode::EvalMsAGDatas();
  //MsAGDatas[0].SetUnity(true);
  if (IOConduitIn(0))
    IOMSIndex(0)->SetMustExist(true);
  if (IOConduitIn(1))
    IOMSIndex(1)->SetMustExist(true);
//  IOFlange(1)->MS.Switch(IOFlange(0)->MS());
  };

//--------------------------------------------------------------------------

#endif

//--------------------------------------------------------------------------

dword MN_Lnk::ModelStatus()
  {                           
  if (fDoDbgBrk)
    { int xxx=0; };
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs()>1 && IOFBs(0))
    {
    int iIn  = (IOQmEst_In(0)>0.0 ? 0 : 1);
    int iOut = OtherEnd(iIn);

    if (!cioOn())
      Status|=FNS_Off;
    else if (0 && (IO_Rmt(0).JoinTp()==IOJn_Closed || IO_Rmt(1).JoinTp()==IOJn_Closed))
      Status|=FNS_Off;

    if (GetStatusAsStopped())
      {
      Status |= (m_FEP.FlwRqdActive() ?  FNS_LFlwP /*: FNS_LFlwN)*/ : FNS_LNoFlw);
      if (FlowMode()==LFM_Simple)
        Status|=FNS_LnkSimple;
      else if (FlowMode()==LFM_Linear)
        Status|=FNS_LnkLinear;
      else if (FlowMode()==LFM_Full)
        Status|=FNS_LnkFull;
      else if (/*IOFB(0, 0)->m_fInXferNet || IOFB(1, 0)->m_fInXferNet ||*/ FlowMode()==LFM_Xfer)
        Status|=FNS_LnkXfer;
      }
    else
      {
      if (1)
        {
        if (IOFlange(iIn)->IsTear())
          Status|=FNS_Tear;
        else if (IOFlange(iOut)->IsTear())
          Status|=FNS_Tear;
        }
      Status |= (IOQm_In(iIn)>UsableMass ? (iIn==0 ? FNS_LFlwP : FNS_LFlwN) : FNS_LNoFlw);
      }
    if (GetActiveHold())
      Status |= FNS_IsSnk|FNS_IsSrc;
    else if (IOFB(0, 0)->m_fInXferNet || IOFB(1, 0)->m_fInXferNet || FlowMode()==LFM_Xfer)
      Status|=FNS_LnkXfer;
    }
  else
    Status|=FNS_Error;

  return Status;
  };

//--------------------------------------------------------------------------

void MN_Lnk::ClosureInfo()
  {
  bool InRange=true;
  for (int i=0; i<NoFlwIOs(); i++)
    InRange = InRange && IOConduit(i)->SMFnsInRange();
  FlwNode::SetCI(29, !InRange);

  if (m_Closure.DoFlows())
    {
    CClosureInfo &CI=m_Closure[0];
    if (m_RB.Enabled())
      m_RB.GetClosureInfo(CI);
    if (EHX.Enabled())
      CI.m_EHXPowerIn += EHX.HeatFlow();
    m_FTB.GetClosureInfo(CI, false, false);
    }
  };

//--------------------------------------------------------------------------

void MN_Lnk::SetSamplePtCount(long Count)
  {
  long OldCount=m_SamplePt.GetSize();

  for (int i=Count; i<OldCount; i++)
    delete m_SamplePt[i].m_pCnd;

  m_SamplePt.SetSize(Count);

  if (Count>OldCount)
    {
    double Pos=OldCount>0?m_SamplePt[OldCount-1].m_dFracPos:0.0;
    double End=1.0;
    double Delta=(End-Pos)/(Count-OldCount+1);
    for (i=OldCount; i<Count; i++)
      {
      Pos+=Delta;
      m_SamplePt[i].m_dFracPos = Pos;
      m_SamplePt[i].m_pCnd = new SpConduit("Pt", this, TOA_Embedded);
      }
    }
  };

//--------------------------------------------------------------------------

void MN_Lnk::UpdateSamples()
  {
  if (m_PFI.Method==VFFX_Plugflow)
    {
    for (int i=0; i<m_SamplePt.GetSize(); i++)
      m_PFI.Sample(m_SamplePt[i]);
    }
  };

//==========================================================================
