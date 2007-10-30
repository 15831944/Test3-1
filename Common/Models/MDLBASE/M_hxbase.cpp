//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#include "datacnvs.h"
#include "flwnode.h"
#define  __M_HXBASE_CPP
#include "m_hxbase.h"
#include "vlebase.h"
//#include "optoff.h"

//==========================================================================

#define dbgModels         WITHDEBUG
#include "dbgmngr.h"
#if dbgModels
static CDbgMngr dbgFlashTrain ("HXBASE", "FlashTrain");
#endif

#define dbgHeatX           WITHDEBUG
#if dbgHeatX
static CDbgMngr  dbgHeater   ("HXBASE",  "HeatXchg");
static CDbgMngr  dbgHeaterCnv("HXBASE",  "HeatXchgCnv");
#endif

//---------------------------------------------------------------------------

static inline void SetTOut(CHXSide & Sd, double T)
  {
  if (Sd.m_pCd)                 
    {                          
    Sd.m_pCd->SetTemp(T);       
    Sd.To=T;                    
    Sd.Ho=Sd.m_pCd->totHf();     
    }                          
  else                         
    {                          
    Sd.To=T;                    
    Sd.Ho=Sd.Hi+Sd.Ci*(Sd.To-Sd.Ti);
    };
  }

//---------------------------------------------------------------------------

static inline void Set_Enth_and_TOut(CHXSide & Sd, double H)
  {
  if (Sd.m_pCd)
    {
    Sd.m_pCd->Set_totHf(H);
    Sd.Ho=H;
    Sd.To=Sd.m_pCd->Temp();
    }
  else
    {
    Sd.Ho=H;
    Sd.To=Sd.Ti+(Sd.Ho-Sd.Hi)/GTZ(Sd.Ci);
    };
  };


//============================================================================
//
//
//
//============================================================================

static double LMTDSaturation(double Ti, double To, double Ts)
  {
  if (Ti>Ts)
    {
    Ts=Min(Ts, Ti-0.01);
    To=Range(Ts+1.0e-6, To, Ti-1.0e-6);
    }
  else
    {
    Ts=Max(Ts, Ti+0.01);
    To=Range(Ti+1.0e-6, To, Ts-1.0e-6);
    }
  const double Diff1=(Ts-Ti);
  const double Diff2=(Ts-To);
  double DT=(To-Ti)/NZ(Ln(Diff1/Diff2));
  return DT;
  };

//============================================================================
//
//
//
//============================================================================

CToleranceBlock CHXDutyFinder::s_Tol(TBF_Both, "HeatExchange:DutyFinder", 0, 1.0e-8);

//============================================================================
//
//
//
//============================================================================

const pchar CEnvironHX::GroupName="EnvirnHX";

IMPLEMENT_EHX(CEnvironHX, "EHXNone", "", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "None",  " ", EHXF_Node|EHXF_Inline);


CEnvironHX::CEnvironHX(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  dHeatFlow=0.0;
  TempKFeed=Std_T;
  TempKProd=Std_T;
  }

//--------------------------------------------------------------------------

CEnvironHX::~CEnvironHX()
  {
  };

//--------------------------------------------------------------------------

void CEnvironHX::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "EHX", NULL, DDB_NoPage))
    {
    DDB.Double ("",     "HeatFlow",   DC_Pwr,  "kW",    &dHeatFlow,  this, isResult|0);
    //DDB.Double ("TempFeed", "Feed.T",    DC_T,    "C",       &TempKFeed,  this, isResult);//|noFileAtAll);
    //DDB.Double ("TempProd", "Prod.T",    DC_T,    "C",       &TempKProd,  this, isResult);//|noFileAtAll);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

void CEnvironHX::EvalProducts(SpConduit &Qf, double FinalTEst)
  {
  dHeatFlow=0.0;
  //TempKFeed=Qf.Temp();
  //TempKProd=TempKFeed;
  };

void CEnvironHX::EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst)
  {
  dHeatFlow=0.0;
  //TempKFeed=Qf.Temp();
  //TempKProd=TempKFeed;
  };

//============================================================================
//
//
//
//============================================================================

IMPLEMENT_EHX(CEHX_LossPerQm, "EHXLPQ", "", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "LossPerQm",  " ", EHXF_Node|EHXF_Inline);

CEHX_LossPerQm::CEHX_LossPerQm(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  CEnvironHX(pClass_, Tag_, pAttach, eAttach)
  {
  dHperQm = 0.0;
  }

//--------------------------------------------------------------------------

CEHX_LossPerQm::~CEHX_LossPerQm()
  {
  }

//--------------------------------------------------------------------------

void CEHX_LossPerQm::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "EHX", NULL, DDB_NoPage))
    {
    DDB.Double ("",     "LossPerQm",  DC_HMs, "kJ/kg", &dHperQm,    this, isParm);
    DDB.Double ("",     "HeatFlow",   DC_Pwr,  "kW",    &dHeatFlow,  this, isResult|0);
    DDB.Double ("TempFeed", "Feed.T", DC_T,    "C",     &TempKFeed,  this, isResult);//|noFileAtAll);
    DDB.Double ("TempProd", "Prod.T", DC_T,    "C",     &TempKProd,  this, isResult);//|noFileAtAll);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CEHX_LossPerQm::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  }

//--------------------------------------------------------------------------

void CEHX_LossPerQm::EvalProducts(SpConduit &Qf, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  TempKFeed=Qf.Temp();
  if (fabs(dHperQm)>1.0e-9)
    {
    dHeatFlow=-Qf.QMass()*dHperQm;
    Qf.Set_totHf(Qf.totHf()+dHeatFlow);
    TempKProd=Qf.Temp();
    }
  else
    {
    dHeatFlow=0.0;
    TempKProd=TempKFeed;
    }
  //if (fabs(TempKProd-TempKFeed)>20.0)
  //  Qf.Set_totHf(Qf.totHf()+dHeatFlow);
  };

void CEHX_LossPerQm::EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  TempKFeed=Qf.Temp();
  if (fabs(dHperQm)>1.0e-9)
    {
    dHeatFlow=-Qf.QMass()*dHperQm;
    Qf.Set_totHf(Qf.totHf()+dHeatFlow);
    TempKProd=Qf.Temp();
    }
  else
    {
    dHeatFlow=0.0;
    TempKProd=TempKFeed;
    }
  //if (fabs(TempKProd-TempKFeed)>20.0)
  //  Qf.Set_totHf(Qf.totHf()+dHeatFlow);
  };

//============================================================================
//
//
//
//============================================================================

IMPLEMENT_EHX(CEHX_ProductT, "EHXProdT", "", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "ProductTemp",  " ", EHXF_Node|EHXF_Inline);

CEHX_ProductT::CEHX_ProductT(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  CEnvironHX(pClass_, Tag_, pAttach, eAttach)
  {
  dProdT = C_2_K(25.0);
  }

//--------------------------------------------------------------------------

CEHX_ProductT::~CEHX_ProductT()
  {
  }

//--------------------------------------------------------------------------

void CEHX_ProductT::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "EHX", NULL, DDB_NoPage))
    {
    DDB.Double ("",     "ProductTemp",   DC_T,    "C",       &dProdT,     this, isParm);
    DDB.Double ("",     "HeatFlow",      DC_Pwr,  "kW",      &dHeatFlow,  this, isResult|0);
    DDB.Double ("TempFeed", "Feed.T",    DC_T,    "C",       &TempKFeed,  this, isResult);//|noFileAtAll);
    DDB.Double ("TempProd", "Prod.T",    DC_T,    "C",       &TempKProd,  this, isResult);//|noFileAtAll);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CEHX_ProductT::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  }

//--------------------------------------------------------------------------

void CEHX_ProductT::EvalProducts(SpConduit &Qf, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  TempKFeed=Qf.Temp();
  dHeatFlow=Qf.totHf();
  Qf.SetTemp(dProdT);
  dHeatFlow=Qf.totHf()-dHeatFlow;
  TempKProd=Qf.Temp();
  //if (fabs(TempKProd-TempKFeed)>20.0)
  //  Qf.Set_totHf(Qf.totHf()+dHeatFlow);
  };

void CEHX_ProductT::EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  TempKFeed=Qf.Temp();
  dHeatFlow=Qf.totHf();
  Qf.SetTemp(dProdT);
  dHeatFlow=Qf.totHf()-dHeatFlow;
  TempKProd=Qf.Temp();
  //if (fabs(TempKProd-TempKFeed)>20.0)
  //  Qf.Set_totHf(Qf.totHf()+dHeatFlow);
  };

//============================================================================
//
//
//
//============================================================================

IMPLEMENT_EHX(CEHX_TDrop, "EHXTempD", "", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "TempDrop",  " ", /*EHXF_Node|*/EHXF_Inline);

CEHX_TDrop::CEHX_TDrop(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  CEnvironHX(pClass_, Tag_, pAttach, eAttach)
  {
  dTDrop = 0.5;
  }

//--------------------------------------------------------------------------

CEHX_TDrop::~CEHX_TDrop()
  {
  }

//--------------------------------------------------------------------------

void CEHX_TDrop::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "EHX", NULL, DDB_NoPage))
    {
    DDB.Double ("",     "TempDrop",      DC_dT,   "C",       &dTDrop,      this, isParm);
    DDB.Double ("",     "HeatFlow",      DC_Pwr,  "kW",      &dHeatFlow,   this, isResult|0);
    DDB.Double ("TempFeed", "Feed.T",    DC_T,    "C",       &TempKFeed,  this, isResult);//|noFileAtAll);
    DDB.Double ("TempProd", "Prod.T",    DC_T,    "C",       &TempKProd,  this, isResult);//|noFileAtAll);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CEHX_TDrop::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  }

//--------------------------------------------------------------------------

void CEHX_TDrop::EvalProducts(SpConduit &Qf, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  TempKFeed=Qf.Temp();
  dHeatFlow=Qf.totHf();
  Qf.SetTemp(TempKFeed-dTDrop);
  dHeatFlow=Qf.totHf()-dHeatFlow;
  TempKProd=Qf.Temp();
  //if (fabs(TempKProd-TempKFeed)>20.0)
  //  Qf.Set_totHf(Qf.totHf()+dHeatFlow);
  };

void CEHX_TDrop::EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  TempKFeed=Qf.Temp();
  dHeatFlow=Qf.totHf();
  Qf.SetTemp(TempKFeed-dTDrop);
  dHeatFlow=Qf.totHf()-dHeatFlow;
  TempKProd=Qf.Temp();
  //if (fabs(TempKProd-TempKFeed)>20.0)
  //  Qf.Set_totHf(Qf.totHf()+dHeatFlow);
  };

//============================================================================
//
//
//
//============================================================================

class CEHXLossFinder : public MRootFinderBase
  {
  protected:
    double          dUA;
    double          dAmbT;
    double          dTi;
    double          dP;
    double        & dPwr;
    SpConduit     & Cd;

    static CToleranceBlock s_Tol;

  public:

    CEHXLossFinder(double UA, double AmbT, double Ti, double P, double &Pwr, SpConduit &rCd) :
      MRootFinderBase("DutyFinder", s_Tol), 
        Cd(rCd), dPwr(Pwr)
        {
        dUA=UA;
        dAmbT=AmbT;
        dTi=Ti;
        dP=P;

        }
    double Function(double x)
      {
      double LMTD=LMTDSaturation(dTi, x, dAmbT);
      double Q1=dUA*LMTD;
      double Q2=Cd.totHf(som_ALL, x, dP)-Cd.totHf(som_ALL, dTi, dP);
      dPwr=Q1;
      return Q1-Q2;
      };
  };

CToleranceBlock CEHXLossFinder::s_Tol(TBF_Both, "EHX:LossFInder", 0, 1.0e-8);

//============================================================================
//
//
//
//============================================================================
IMPLEMENT_EHX(CEHX_Loss2Ambient, "Loss2Amb", "", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "Loss to Ambient",  "Loss to Ambient", EHXF_Node|EHXF_Inline);

CEHX_Loss2Ambient::CEHX_Loss2Ambient(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  CEnvironHX(pClass_, Tag_, pAttach, eAttach)
  {
  dPipeD      = 0.01;
  dPipeL      = 10.0;
  dAmbTOvr    = (PrjFileVerNo()<77 ? C_2_K(25.0) : dNAN);
  dAmbT       = AmbientTemp();
  dPipe_Thick =   5.0e-3;
  dIns_Thick  =  15.0e-3;
  dK_Pipe     =  45.0e-3;
  dK_Ins      = 0.035e-3;
  dH_2        =  1400.0e-3;
  dH_4        =  18.0e-3;
  dH_Rad      =  6.10e-3;
  dUA         =   0.0;
  dPwr        = 0.0;
  }

//--------------------------------------------------------------------------

CEHX_Loss2Ambient::~CEHX_Loss2Ambient()
  {
  }

//--------------------------------------------------------------------------

void CEHX_Loss2Ambient::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "EHX", NULL, DDB_NoPage))
    {
    DDB.Visibility(NM_Probal|SM_All|HM_All);
    DDB.Double("Diam",                "", DC_L,     "mm",      &dPipeD,      this, isParm);
    DDB.Double("Length",              "", DC_L,     "m",       &dPipeL,      this, isParm);
    DDB.Visibility();                               
    DDB.Double ("Ambient_T",          "", DC_T,     "C",       &dAmbT,       this, isParm);
    DDB.Double ("Pipe_Thick",         "", DC_L,     "mm",      &dPipe_Thick, this, isParm);
    DDB.Double ("Ins_Thick",          "", DC_L,     "mm",      &dIns_Thick,  this, isParm);
    DDB.Double ("Pipe_Cond",          "", DC_HCond, "W/m.K",   &dK_Pipe,     this, isParm);
    DDB.Double ("Ins_Cond",           "", DC_HCond, "W/m.K",   &dK_Ins,      this, isParm);
    DDB.Double ("CnvHTC_Liq_Pipe",    "", DC_HTC,   "W/m^2.K", &dH_2,        this, isParm);
    DDB.Double ("CnvHTC_Lag_Air",     "", DC_HTC,   "W/m^2.K", &dH_4,        this, isParm);
    DDB.Double ("CnvHTC_Radiation",   "", DC_HTC,   "W/m^2.K", &dH_Rad,      this, isParm);
                                                    
    DDB.Double ("UA",                 "", DC_UA,    "kW/K",    &dUA,         this, 0);
    DDB.Double ("Power",              "", DC_Pwr,   "kW",      &dPwr,        this, 0);
    DDB.Double ("",           "HeatFlow", DC_Pwr,   "kW",      &dHeatFlow,   this, isResult|0);
    DDB.Double ("TempFeed", "Feed.T",    DC_T,    "C",       &TempKFeed,  this, isResult);//|noFileAtAll);
    DDB.Double ("TempProd", "Prod.T",    DC_T,    "C",       &TempKProd,  this, isResult);//|noFileAtAll);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CEHX_Loss2Ambient::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  }

//--------------------------------------------------------------------------

void CEHX_Loss2Ambient::EvalProducts(SpConduit &Qf, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  if (0)
    {
    dHeatFlow=0.0;
    TempKFeed=Qf.Temp();
    TempKProd=TempKFeed;
    }
  else
    {
    TempKFeed=Qf.Temp();
    dHeatFlow=Qf.totHf();
    dAmbT=(Valid(dAmbTOvr) ? dAmbTOvr : AmbientTemp());

    if (NetProbalMethod())
      {
      dPipeD = Max(dPipeD, 0.001);
      dPipeL = Max(dPipeL, 0.001);
      }
    const double Diam = dPipeD;
    const double Len = dPipeL;

    dK_Pipe = Max(dK_Pipe, 1.0e-9);
    dK_Ins = Max(dK_Ins, 1.0e-9);
    dH_2 = Max(dH_2, 1.0e-9);
    dH_4 = Max(dH_4, 1.0e-9);

    const double R2=Diam/2.0;
    const double R3=R2+dPipe_Thick;
    const double R4=R3+dIns_Thick;
    const double A1=2.0*PI*Len;
    const double A2=A1*R2;
    const double A3=A1*R3;
    const double A4=A1*R4;
    const double A23=(A2+A3)/2.0;
    const double A34=(A3+A4)/2.0;
    // Ref - KCG
    dUA=1.0/(1/(dH_2*A2) + (R3-R2)/(dK_Pipe*A23) + (R4-R3)/(dK_Ins*A34) + 1/((dH_4+0.95*dH_Rad)*A4));

    const double Ti=Qf.Temp();
    CEHXLossFinder LF(dUA, dAmbT, Ti, Qf.Press(), dPwr, Qf);
    LF.SetTarget(0.0);

    const double TLo=Ti < dAmbT ? Ti : dAmbT;
    const double THi=Ti < dAmbT ? dAmbT : Ti;
    switch (LF.Start(TLo, THi))
      {
      case RF_OK:
        if (LF.Solve_Brent()==RF_OK)
          Qf.SetTemp(LF.Result());
          break;
        //SetCondition(2);
        break;
      case RF_LoLimit:
  //      LF.Function(TLo);
        Qf.SetTemp(TLo);
        break;
      case RF_HiLimit:
        // Flash the output
        Qf.SetTemp(THi);
        //LF.Function(THi);
        break;
      }

    dHeatFlow=Qf.totHf()-dHeatFlow;
    TempKProd=Qf.Temp();
    }
  //if (fabs(TempKProd-TempKFeed)>20.0)
  //  Qf.Set_totHf(Qf.totHf()+dHeatFlow);
  };

void CEHX_Loss2Ambient::EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  TempKFeed=Qf.Temp();
  dHeatFlow=Qf.totHf();
  dAmbT=(Valid(dAmbTOvr) ? dAmbTOvr : AmbientTemp());

  if (NetProbalMethod())
    {
    dPipeD = Max(dPipeD, 0.001);
    dPipeL = Max(dPipeL, 0.001);
    Diam = dPipeD;
    Len = dPipeL;
    }

  dK_Pipe = Max(dK_Pipe, 1.0e-9);
  dK_Ins = Max(dK_Ins, 1.0e-9);
  dH_2 = Max(dH_2, 1.0e-9);
  dH_4 = Max(dH_4, 1.0e-9);

  const double R2=Diam/2.0;
  const double R3=R2+dPipe_Thick;
  const double R4=R3+dIns_Thick;
  const double A1=2.0*PI*Len;
  const double A2=A1*R2;
  const double A3=A1*R3;
  const double A4=A1*R4;
  const double A23=(A2+A3)/2.0;
  const double A34=(A3+A4)/2.0;
  // Ref - KCG
  dUA=1.0/(1/(dH_2*A2) + (R3-R2)/(dK_Pipe*A23) + (R4-R3)/(dK_Ins*A34) + 1/((dH_4+0.95*dH_Rad)*A4));

  const double Ti=Qf.Temp();
  CEHXLossFinder LF(dUA, dAmbT, Ti, Qf.Press(), dPwr, Qf);
  LF.SetTarget(0.0);

  const double TLo=Ti < dAmbT ? Ti : dAmbT;
  const double THi=Ti < dAmbT ? dAmbT : Ti;
  switch (LF.Start(TLo, THi))
    {
    case RF_OK:
      if (LF.Solve_Brent()==RF_OK)
        Qf.SetTemp(LF.Result());
        break;
      //SetCondition(2);
      break;
    case RF_LoLimit:
//      LF.Function(TLo);
      Qf.SetTemp(TLo);
      break;
    case RF_HiLimit:
      // Flash the output
      Qf.SetTemp(THi);
      //LF.Function(THi);
      break;
    }

  dHeatFlow=Qf.totHf()-dHeatFlow;
  TempKProd=Qf.Temp();
  //if (fabs(TempKProd-TempKFeed)>20.0)
  //  Qf.Set_totHf(Qf.totHf()+dHeatFlow);
  };

//============================================================================
//
//
//
//============================================================================
IMPLEMENT_EHX(CEHX_Loss2AmbientTank, "Loss2Amb2", "", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "Loss to Ambient2",  "Loss to Ambient2", EHXF_Node|EHXF_Inline);

CEHX_Loss2AmbientTank::CEHX_Loss2AmbientTank(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  CEnvironHX(pClass_, Tag_, pAttach, eAttach)
  {
  dAmbTOvr    = dNAN;
  dAmbT       = AmbientTemp();
  dArea       = 10.0;
  dHTC        = 1.0;
  }

//--------------------------------------------------------------------------

CEHX_Loss2AmbientTank::~CEHX_Loss2AmbientTank()
  {
  }

//--------------------------------------------------------------------------

void CEHX_Loss2AmbientTank::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "EHX", NULL, DDB_NoPage))
    {
    DDB.Visibility();                               
    DDB.Double ("AmbTOverride",       "", DC_T,     "C",       &dAmbTOvr,    this, isParm|NAN_OK);
    DDB.Double ("Ambient_T",          "", DC_T,     "C",       &dAmbT,       this, isResult);
    DDB.Double ("HTC",                "", DC_HTC,   "kW/m^2.K",&dHTC,        this, isParm);
    DDB.Double ("Area",               "", DC_Area,  "m^2",     &dArea,       this, isParm);
    DDB.Double ("",           "HeatFlow", DC_Pwr,   "kW",      &dHeatFlow,   this, isResult);
    DDB.Double ("TempFeed", "Feed.T",     DC_T,     "C",       &TempKFeed,  this, isResult);//|noFileAtAll);
    DDB.Double ("TempProd", "Prod.T",     DC_T,     "C",       &TempKProd,  this, isResult);//|noFileAtAll);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CEHX_Loss2AmbientTank::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  }

//--------------------------------------------------------------------------

void CEHX_Loss2AmbientTank::EvalProducts(SpConduit &Qf, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  TempKFeed=Qf.Temp();
  const double Hf0=Qf.totHf();
  dAmbT=(Valid(dAmbTOvr) ? dAmbTOvr : AmbientTemp());
  dHeatFlow=dHTC*dArea*(dAmbT-TempKFeed);
  if (fabs(dHeatFlow)<1.0e-12)
    {
    dHeatFlow=0.0;
    TempKProd=TempKFeed;
    }
  else
    {
    Qf.Set_totHf(Qf.totHf()+dHeatFlow);
    TempKProd=Qf.Temp();
    // Detect and Fix CrossOver
    if ((TempKProd-dAmbT)*(TempKFeed-dAmbT)<0)
      {
      Qf.SetTempPress(dAmbT, Qf.Press());
      TempKProd=Qf.Temp();
      dHeatFlow=Qf.totHf()-Hf0;
      }
    }
  };

void CEHX_Loss2AmbientTank::EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  TempKFeed=Qf.Temp();
  const double Hf0=Qf.totHf();
  dAmbT=(Valid(dAmbTOvr) ? dAmbTOvr : AmbientTemp());
  dHeatFlow=dHTC*dArea*(dAmbT-TempKFeed);
  if (fabs(dHeatFlow)<1.0e-12)
    {
    dHeatFlow=0.0;
    TempKProd=TempKFeed;
    }
  else
    {
    Qf.Set_totHf(Qf.totHf()+dHeatFlow);
    TempKProd=Qf.Temp();
    // Detect and Fix CrossOver
    if ((TempKProd-dAmbT)*(TempKFeed-dAmbT)<0)
      {
      Qf.SetTempPress(dAmbT, Qf.Press());
      TempKProd=Qf.Temp();
      dHeatFlow=Qf.totHf()-Hf0;
      }
    }
  };

//============================================================================
//
//
//
//============================================================================

IMPLEMENT_EHX(CEHX_FixedHeatFlow, "EHXFHF", "", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "FixedHeatFlow", " ", EHXF_Node|EHXF_Inline);

CEHX_FixedHeatFlow::CEHX_FixedHeatFlow(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  CEnvironHX(pClass_, Tag_, pAttach, eAttach)
  {
  dRqdHeatFlow = 0.0;
  }

//--------------------------------------------------------------------------

CEHX_FixedHeatFlow::~CEHX_FixedHeatFlow()
  {
  }

//--------------------------------------------------------------------------

void CEHX_FixedHeatFlow::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, "EHX", NULL, DDB_NoPage))
    {
    DDB.Double ("",         "RqdHeatFlow", DC_Pwr,  "kW",    &dRqdHeatFlow, this, isParm);
    DDB.Double ("",         "HeatFlow",    DC_Pwr,  "kW",    &dHeatFlow,    this, isResult|0);
    DDB.Double ("TempFeed", "Feed.T",      DC_T,    "C",     &TempKFeed,  this, isResult);//|noFileAtAll);
    DDB.Double ("TempProd", "Prod.T",      DC_T,    "C",     &TempKProd,  this, isResult);//|noFileAtAll);
    }
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CEHX_FixedHeatFlow::DataXchg(DataChangeBlk & DCB)
  {
  return 0;
  }

//--------------------------------------------------------------------------

void CEHX_FixedHeatFlow::EvalProducts(SpConduit &Qf, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  TempKFeed=Qf.Temp();
  dHeatFlow=dRqdHeatFlow;
  if (dRqdHeatFlow==0.0)
    {
    TempKProd=TempKFeed;
    }
  else
    {
    Qf.Set_totHf(Qf.totHf()+dHeatFlow);
    TempKProd=Qf.Temp();
    }
  //if (fabs(TempKProd-TempKFeed)>20.0)
  //  Qf.Set_totHf(Qf.totHf()+dHeatFlow);
  }

//--------------------------------------------------------------------------

void CEHX_FixedHeatFlow::EvalProductsInline(SpConduit & Qf, double Len, double Diam, double FinalTEst)
  {
  if (!Valid(FinalTEst))
    FinalTEst=Qf.Temp();

  TempKFeed=Qf.Temp();
  dHeatFlow=dRqdHeatFlow;
  if (dRqdHeatFlow==0.0)
    {
    TempKProd=TempKFeed;
    }
  else
    {
    Qf.Set_totHf(Qf.totHf()+dHeatFlow);
    TempKProd=Qf.Temp();
    }
  //if (fabs(TempKProd-TempKFeed)>20.0)
  //  Qf.Set_totHf(Qf.totHf()+dHeatFlow);
  }

//============================================================================
//============================================================================
//============================================================================
//
//
//
//============================================================================

CEnvironHXBase::CEnvironHXBase(FlwNode* pAttach, dword SelectMask, LPTSTR Name) : \
CBlockEvalBase(BEId_EHX, 0, Name, false/*true*/) 
  {
  pHL=NULL;
  pNd=pAttach;
  fEnabled=false;
  fFixed=False;
  m_dwSelectMask=SelectMask;
  };

//--------------------------------------------------------------------------

CEnvironHXBase::~CEnvironHXBase()
  {
  delete pHL;
  };

//--------------------------------------------------------------------------

flag CEnvironHXBase::Open(TagObjClass * pEHXClass, flag Fixed)
  {
  fEnabled=True;

  if (pHL && (pEHXClass==NULL || pHL->Class()==pEHXClass))
    return True;

  delete pHL;
  fFixed=Fixed;

  if (pEHXClass)
    pHL=(CEnvironHX*)pEHXClass->Construct(NULL, "EHX", pNd, TOA_Embedded);//pNd);
  else
    pHL=(CEnvironHX*)CEnvironHXClass.Construct(NULL, "EHX", pNd, TOA_Embedded);//pNd);
  pNd->StructureChanged(NULL);

  fEnabled=pHL!=NULL;
  return fEnabled;
  };

//--------------------------------------------------------------------------

flag CEnvironHXBase::Open(byte OnOffSeq, TagObjClass * pEHXClass, flag Fixed) 
  { 
  CBlockEvalBase::Open(OnOffSeq); 
  return Open(pEHXClass, Fixed); 
  }

//--------------------------------------------------------------------------

byte CEnvironHXBase::OpenStatus()
  {
  return CBlockEvalBase::OpenStatus(Enabled());
  }

//--------------------------------------------------------------------------

void CEnvironHXBase::Close()
  {
  if (fFixed)
    return;
  fEnabled=False;
  //if (pHL==NULL)
  //  return;
  ////pGSM->StructureChanged(NULL);
  ////pGSM->Gammas.SetSize(0);
  ////pGSM->IOAlpha.SetSize(0);
  //delete pHL;
  //pHL=NULL;
  };

//--------------------------------------------------------------------------

void CEnvironHXBase::Add_OnOff(DataDefnBlk& DDB, dword Flags)
  {
  DDB.Byte("",     "EnvironHX",  DC_,    "",     xidEnvHXEnable,  pNd, Flags, GetOnOffValLst());
  }

//--------------------------------------------------------------------------

void CEnvironHXBase::BuildDataDefn(DataDefnBlk& DDB, pchar pTag_, pchar pTagComment, DDBPages PageIs, dword UserInfo)
  {
  if (pTag_==NULL)
    pTag_="EHX";
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(NSHM_All, fEnabled);

  if (Enabled())//pHL)
    {
    if (DDB.BeginStruct(pNd, pTag_, pTagComment, PageIs))
      {
      DDBValueLstMem DDB0;
      TagObjClass::GetSDescValueLst(CEnvironHX::GroupName, DDB0, NULL, m_dwSelectMask);
      DDB.String  ("Model",      "",       DC_    , "",      xidEnvHXEqnNm  , pNd,fFixed ? 0 : isParm|SetOnChange, DDB0());

      if (pHL)
        {
        DDB.SuppressNextBlock();
        pHL->BuildDataDefn(DDB);
        }
      }
    DDB.EndStruct();
    }
  DDB.SetVisibility(Old);
  }

//--------------------------------------------------------------------------

flag CEnvironHXBase::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidEnvHXEnable:
      if (DCB.rB)
        if (*DCB.rB)
          Open(*DCB.rB);
        else
          Close();
      DCB.B=OpenStatus();// (Enabled());
      return 1;
    case xidEnvHXEqnNm:
      if (DCB.rpC && !fFixed)
        {
        int WasEnabled=fEnabled;
        TagObjClass * pC=CEnvironHXClass.FindGrpShortDesc(DCB.rpC);
        if (pC)
          Open(pC);
        else
          Close();
        //if (pC && (pC!=pHL->Class()))
        //  {
        //  Strng OldTag=pHL->Tag();
        //  delete pHL;
        //  pHL=(CEnvironHX*)pC->Construct(OldTag(), pNd, TOA_Embedded);
        //  pNd->StructureChanged(pNd);
        //  }
        }
      DCB.pC=pHL ? pHL->ShortDesc() : "";
      return 1;
    }
  return 0;
  }

//==========================================================================
//
//
//
//==========================================================================


#define BOX(x,y,w,h) x,y, x,y+h, x+w,y+h, x+w,y, x,y

XID xidHXSd_dT   = MdlBsXID(10004);
XID xidHXSd_Desc = MdlBsXID(10005);

const byte idPIn  = 0;
const byte idPOut = 1;
const byte idSIn  = 2;
const byte idSOut = 3;

//==========================================================================
//
// CHXSide
//
//==========================================================================

CHXSide::CHXSide(CHXBlock * pHX_)
  {
  pHX=pHX_;
  iMode=QPF_Error;
  Cp=4.186;
  Ci=0.0;
  Qm=0.0;
#if HX_MEASURE_LIQVAP
  m_LiqQm=0.0;
  m_VapQm=0.0;
#endif
  Ti=Std_T;
  To=Std_T;

  m_Pi=Std_P;
  m_Po=Std_P;

  m_VfIn=0.0;

  Duty=0.0;
  MaxFlDuty=0.0;
  MaxNfDuty=0.0;
  Hi=0.0;
  Ho=0.0;
  dHEnv=0.0;
  m_pNd=NULL;
  m_pCdIn=NULL;
  m_pCd=NULL;
  m_pByPass=NULL;
  m_pCn=NULL;
  m_pEHX=NULL;
  m_pRB=NULL;
  m_pWrkCd=NULL;
  m_pMemCd=NULL;
  fConnected=0;
  fPreReact=false;

  m_SatT=Std_T;
  m_SatP=Std_P;
  m_SatPP=Std_P;
  m_PPFrac=1.0;

  FlFrac=0.0;
  Sgn=1.0;
  dFinalTEst=dNAN;
  }

//--------------------------------------------------------------------------

CHXSide::~CHXSide()
  {
  //delete dMass;
  //delete pXDP;
  delete m_pByPass;
  delete m_pWrkCd;
  delete m_pMemCd;
  }

//--------------------------------------------------------------------------

void CHXSide::BuildDataDefn(char*Tag, DataDefnBlk & DDB, TaggedObject* pTagObj, flag Conduit, flag Connected, flag PriSide)
  {
  if (DDB.BeginStruct(pTagObj, Tag, NULL, DDB_NoPage, PriSide ? 1 : 2))
    {
    DDB.Visibility();
    static DDBValueLst DDB0[]={
      {QPF_Error,       "-"},
      {QPF_Sensible,    "Sensible"},
      {QPF_Condensing,  "Condensing"},
      {QPF_Evaporating, "Evaporating"},
      {0}};
    DDB.Bool("Mode",    "", DC_,     "",    &iMode, pTagObj, isParmConstruct, DDB0);
    if (iMode==QPF_Condensing)
      DDB.String("", "VapourCalcs",         DC_,    "",    xidHXSd_Desc,    pTagObj, isResult|noFileAtAll);

    DDB.Visibility(NSHM_All, Conduit);
    DDB.Double ("",               "Qm",     DC_Qm,    "kg/s",    &Qm,       pTagObj, Connected ? isParmConstruct : isParm);
  //  DDB.Double ("",               "QmVent", DC_Qm,    "kg/s",    &QmVent,   pTagObj, Connected ? isParmConstruct : isParm);
    DDB.Visibility();
    DDB.Double ("",               "Cp",     DC_CpMs,    "kJ/kg.C", &Cp,       pTagObj, Connected ? isParmConstruct : isParm);
    DDB.Visibility(NSHM_All, Conduit||!Conduit);
    DDB.Double ("",               "Ti",     DC_T,     "C",       &Ti,       pTagObj, Connected ? isParmConstruct : isParm);
    DDB.Visibility(NSHM_All, Conduit);
    DDB.Double ("",               "To",     DC_T,     "C",       &To,       pTagObj, isParmConstruct);
    DDB.Double ("",               "Pi",     DC_P,     "kPag",    &m_Pi,     pTagObj, isParmConstruct);
    DDB.Double ("",               "Po",     DC_P,     "kPag",    &m_Po,     pTagObj, isParmConstruct);
    DDB.Visibility(NSHM_All, !Conduit);
    DDB.Double ("",               "T",      DC_T,     "C",       &To,       pTagObj, Connected ? isParmConstruct : isParm);
    DDB.Double ("",               "P",      DC_P,     "kPag",    &m_Po,     pTagObj, Connected ? isParmConstruct : isParm);
    DDB.Visibility(NSHM_All, Conduit);
    DDB.Double ("",               "dT",     DC_dT,    "C",       xidHXSd_dT,  pTagObj, isParmConstruct);

    DDB.Visibility(NSHM_All, iMode!=QPF_Sensible);

    DDB.Visibility(NSHM_All, true);//Conduit);
    DDB.Double("SatT",            "",       DC_T,     "C",       &m_SatT,  pTagObj, isParmConstruct);
    DDB.Double("SatP",            "",       DC_P,     "kPag",    &m_SatP,  pTagObj, isParmConstruct);
    DDB.Double("SatPP",           "",       DC_P,     "kPag",    &m_SatPP, pTagObj, isParmConstruct);
    DDB.Double("PPFrac",          "",       DC_Frac,  "%",       &m_PPFrac, pTagObj, isParmConstruct|InitHidden);
    DDB.Double("Duty",            "",       DC_Pwr,   "kW",      &Duty,    pTagObj, isParmConstruct);

    DDB.Visibility();
    DDB.Text("");
    }
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

flag CHXSide::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidHXSd_Desc: 
      DCB.pC = m_pCd ? m_pCd->FlashDescription() : ""; 
      return 1;
    case xidHXSd_dT: 
      DCB.D = To-Ti;
      return 1;
    }
  return 0;
  }

//--------------------------------------------------------------------------

void CHXSide::MeasureHXDataCd(SpConduit * pCd, CSaturationDefn * pSatPDefn)
  {
  if (pCd==NULL)
    pCd=m_pCd;
  SpConduit *In=m_pCdIn ? m_pCdIn : pCd;
  Ti = In->Temp();
  Hi = In->totHf();
  m_Pi = In->Press();
  m_VfIn = In->MassFrac(som_Gas);
  Ci = In->totCp();
  Cp = pCd->msCp();
  Qm = pCd->QMass();

  if (pSatPDefn && pSatPDefn->Method()==SMO_SatPPartialP)
    m_PPFrac = pCd->PartialPressFracC(pSatPDefn->CmpIndex(), -1);
  else
    m_PPFrac = 1;

#if HX_MEASURE_LIQVAP
  m_LiqQm = pCd->VMass[pCd->SatPLiqIndex()];
  m_VapQm = pCd->VMass[pCd->SatPVapIndex()];
#endif

  //dbgpln("CHXSide::MeasureHXDataCd Liq:%12.5f Vap:%12.5f Ti:%12.4f Pi:%12.4f %s", 
  //  pCd->VMass[pCd->SatPLiqIndex()], pCd->VMass[pCd->SatPVapIndex()], 
  //  Ti, m_Pi,
  //  pCd->FullObjTag());

  m_Po=m_Pi;
  if (m_PPFrac>1.0e-6)
    m_SatT = pCd->SaturationT(FlashPressOut(), pSatPDefn);
  else
    m_SatT = pCd->SaturationT(m_Po, pSatPDefn);
  m_pCd->QSaveMass(MassImg);

  To=Ti;
  Ho=Hi;
  }

//--------------------------------------------------------------------------

void CHXSide::MeasureHXDataCn(SpContainer * pCn, CSaturationDefn * pSatPDefn)
  {
  if (pCn==NULL)
    pCn=m_pCn;
  //SpConduit *In=m_pCdIn ? m_pCdIn : m_pCd;
  Ti = pCn->Temp();
  Hi = pCn->totHf();
  m_Pi = pCn->Press();
  m_VfIn = pCn->MassFrac(som_Gas);
  Ci = pCn->totCp();
  Cp = pCn->msCp();
  Qm = pCn->Mass();
  if (pSatPDefn && pSatPDefn->Method()==SMO_SatPPartialP)
    m_PPFrac = pCn->PartialPressFracC(pSatPDefn->CmpIndex(), -1);
  else
    m_PPFrac = 1.0;

#if HX_MEASURE_LIQVAP
  m_LiqQm = pCn->VMass[pCn->SatPLiqIndex()/* H2OLiq()*/];
  m_VapQm = pCn->VMass[pCn->SatPVapIndex()/* H2OVap()*/];
#endif
  m_Po=m_Pi;
  //m_SatT = pCn->SaturationT(FlashPressOut());
  _asm int 3;
  if (m_PPFrac>1.0e-6)
    m_SatT = pCn->SaturationT(FlashPressOut(), pSatPDefn);
  else
    m_SatT = pCn->SaturationT(m_Po, pSatPDefn);
  //m_pCnd->QSaveMass(MassImg);

  To=Ti;
  Ho=Hi;
  }

//--------------------------------------------------------------------------

void CHXSide::SetInput(SpConduit *pCdIn, SpContainer& rCn, PhMask CnPhase, double CnFraction, double PIn, CSaturationDefn * pSatPDefn, SpConduit * pFeed, CEnvironHX * EHX, CReactionBlock * RB, double FinalTEst)
  {
  fConnected=1;
  m_pCdIn=pCdIn;
  m_pCn=&rCn;
  m_pEHX=EHX;
  m_pRB=RB;

  m_Pi=PIn;
  m_Po=m_Pi;

  if (m_pWrkCd==NULL)
    m_pWrkCd=new SpConduit("Wrk", NULL, TOA_Free);
  m_pCd=m_pWrkCd;
  m_pCd->QZero();
  m_pCd->QSetF(*m_pCn, CnPhase, CnFraction, m_pCn->Press());
 // m_pWrkCd->QAddF(*m_pCd, som_ALL, 1.0);
  
  dFinalTEst=FinalTEst;

  MeasureHXDataCd(m_pWrkCd, pSatPDefn);
  }

//--------------------------------------------------------------------------

void CHXSide::SetInput(SpConduit *pCdIn, SpConduit & Cd, double PIn, CSaturationDefn * pSatPDefn, CEnvironHX * EHX, CReactionBlock * RB, double FinalTEst, bool PreReact)
  {
  fConnected=1;
  fPreReact=PreReact;

  m_pCdIn=pCdIn;
  m_pCd=&Cd;
  m_pCn=NULL;
  m_pEHX=EHX;
  m_pRB=RB;

  m_Pi=PIn;
  m_Po=m_Pi;


  if (m_pRB && m_pCd && fPreReact)
    m_pRB->EvalProducts(*m_pCd, FinalTEst);

  dFinalTEst=FinalTEst;

  MeasureHXDataCd(m_pCd, pSatPDefn);

  }

//--------------------------------------------------------------------------

void CHXSide::ClrInput()
  {
  fConnected=0;
  m_pCdIn=NULL;
  m_pCd=NULL;
  m_pCn=NULL;
  m_pEHX=NULL;
  m_pRB=NULL;

  Ci = 0.0;
  Cp = 0.0;
  Qm = 0.0;
#if HX_MEASURE_LIQVAP
  m_LiqQm = 0.0;
  m_VapQm = 0.0;
#endif
  Hi = 0.0;

  MassImg.Zero();
  }

//--------------------------------------------------------------------------

void CHXSide::NoJoinInput()
  {
  }

//--------------------------------------------------------------------------

void CHXSide::EvalProducts(double FinalTEst)
  {
  if (m_pEHX && m_pCd)
    m_pEHX->EvalProducts(*m_pCd, FinalTEst);
  if (m_pRB && m_pCd)
    m_pRB->EvalProducts(*m_pCd, FinalTEst);
  }

//--------------------------------------------------------------------------

double CHXSide::KeepByPassLiquor(double Press)
  {
  double QmByPass=0.0;
  if (m_pCd)
    {
    if (m_pByPass==NULL)
      m_pByPass=new SpConduit("ByPass", NULL, TOA_Free);
    QmByPass=m_pCd->QMass(som_SL);
    if (QmByPass>0.0)//UsableMass);
      {
      m_pByPass->QSetM(*m_pCd, som_SL, QmByPass, Press);
      m_pCd->QAdjustQmTo(som_SL, 0.0);
      MeasureHXDataCd(NULL, NULL);
      }
    else
      m_pByPass->QZero();
    /*if (QmByPass>0.0)
      {
      m_pByPass->QSetM(*m_pCd, som_SL, QmByPass, Press);
      m_pCd->QAdjustQmTo(som_Gas, 0.0);
      MeasureHXDataCd(NULL);
      }
    else
      m_pByPass->QZero();*/
    }
  return QmByPass;
  }

//--------------------------------------------------------------------------

void CHXSide::AddByPassLiquor()
  {
  if (m_pCd && m_pByPass)
    {
    m_pCd->QAddF(*m_pByPass, som_ALL, 1.0);
    }
  }

//--------------------------------------------------------------------------

double CHXSide::SendGasToVent(bool FullyCondensing)
  {
  SpConduit * pVent     = pHX->m_pVent;
  double &    QmVent    = pHX->m_QmVent;
  double &    QmVentRqd = pHX->m_QmVentRqd; //bypass steam?!
  double &    VentPress = pHX->m_VentPress;

  QmVent=0.0;
  if (pVent)
    {
    pVent->QZero();
    if (m_pCd)
      {
      if (FullyCondensing)
        {
        CVLEBase & VLE = *m_pCd->VLEBlk();
        const double QmTotalVap=m_pCd->QMass(som_Gas);
        const double QmSteam=m_pCd->VMass[VLE.SatPVapIndex()/*H2OVap()*/];
        const double QmOtherVap=QmTotalVap-QmSteam;
        if (QmVentRqd>0.0 || (pHX->m_dNonCondVentFrac>0.0 && QmOtherVap>0.0))
          {
          pVent->ChangeModel(m_pCd);
          const double QmSteamVent = Range(0.0, QmVentRqd, 0.9*QmSteam); //maximum steam vent = 90%
          const double qm = QmTotalVap*pHX->m_dNonCondVentFrac;
          pVent->QSetM(*m_pCd, som_Gas, qm, VentPress);
          m_pCd->QAdjustQmTo(som_Gas, QmTotalVap-qm);
          //fix steam...
          pVent->VValue[VLE.SatPVapIndex()/*H2OVap()*/] = QmSteamVent;
          m_pCd->VValue[VLE.SatPVapIndex()/*H2OVap()*/] = QmSteam-QmSteamVent;
          // Remeasure Total Measurements
          //MeasureHXDataCd(NULL);
          QmVent = QmSteamVent;
          }
        }
      else if (QmVentRqd>0.0)
        {
        pVent->ChangeModel(m_pCd);
        const double QmVap=m_pCd->QMass(som_Gas);
        QmVent=Range(0.0, QmVentRqd, 0.9*QmVap);
        pVent->QSetM(*m_pCd, som_Gas, QmVent, VentPress);
        m_pCd->QAdjustQmTo(som_Gas, QmVap-QmVent);
        // Remeasure Total Measurements
        //MeasureHXDataCd(NULL);
        }

      }
    }
  return QmVent;
  }

//--------------------------------------------------------------------------

double CHXSide::AddGasToVent(double VentFlow, bool MixVentBack)
  {
  const bool DoVent=!MixVentBack;
  SpConduit * pVent     = pHX->m_pVent;
  double &    QmVent    = pHX->m_QmVent;
  if (m_pCd && VentFlow>1.0e-12)
    {
    const double OldQmVent=QmVent;
    const double OldVapFlow=m_pCd->QMass(som_Gas);
    VentFlow=Min(OldVapFlow, VentFlow);
    if (DoVent)
      {
      if (pVent)
        {
        pVent->QAddM(*m_pCd, som_Gas, VentFlow);
        m_pCd->QAdjustQmTo(som_Gas, OldVapFlow-VentFlow); //is this logic correct for all conditions???
        }
      else 
        {
        //m_pCd->QAdjustQmTo(som_Gas, OldVapFlow+OldQmVent/*-QmVent*/); //is this logic correct for all conditions???
        pHX->SetCI(5);
        }

      QmVent+=VentFlow;
      //m_pCd->QAdjustQmTo(som_Gas, OldVapFlow-QmVent);
//      m_pCd->QAdjustQmTo(som_Gas, OldVapFlow+OldQmVent/*-QmVent*/); //is this logic correct for all conditions???
      ////m_pCd->QScaleMass(som_ALL, 1.0-VentFrac);
      }
    }

  return QmVent;
  }

//==========================================================================
//
// CHXSide
//
//==========================================================================
/*#D:#T:Heat Exchange Block
#X:#h<General Description>#nGeneral purpose splitter model. ???
#n#n
#h<Variables to be supplied by the user>#n
#i<Mode> : ???#n
#i<HTC> : ???#n
#i<Area> : ???#n
#i<LMTDFact> : ???#n
#n#n
#h<Associated variables>#n
#i<Effect> : ???#n
#i<U#*A> : ???#n
#i<Duty> : ???#n
#i<LMTD> : ???#n
#i<Effect> : ???#n
#nThe following variables for the primaray and secondary streams:#n
#i<Mode> : ???#n
#i<Qm> : ???#n
#i<Cp> : ???#n
#i<Ti> : Inlet temperature.#n
#i<To> : Outlet temperature.#n
#i<Pi> : Inlet pressure.#n
#i<Po> : Outlet pressure.#n
#i<dT> : ???#n
#i<SatT> : ???#n
#i<SatP> : ???#n
#n
#n#h<Other>#n
Short name:HXBlock#n
Model type:Block#n
#G:Common model blocks
*/

//const int CHXBlock::iP=0;
//const int CHXBlock::iS=1;
const pchar CHXBlock::GroupName="HeatXBlk";

IMPLEMENT_HXB(CHXBlock, "HXBlock", "", TOC_ALL|TOC_GRP_ENERGY|TOC_STD_KENWALT, "HeatExchangeBlk", "HeatExchangeBlock");
CHXBlock::CHXBlock(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, Tag_, pAttach, eAttach),
  m_EffectCurve("DataOnly", "Effective", pAttach, TOA_Embedded, "NTU", "C", "Eff"),
  m_Pri(this),
  m_Sec(this)
  {
  m_dDuty=0.0;
  m_dDutySens=0.0;
  m_dTheoryDuty=0.0;
  m_dLMTD=0.0;
  m_dLMTDFactor=1.0;
  m_dHTC=0.980;
  m_dArea=10.0;
  m_dApproachDT=0.1;

  m_bLvlEffOn=false;
  m_iXSect=HXLC_CircXSect;
  m_dMxEffLvl=0.1;
  m_dMnEffLvl=0.9;
  m_dMxEff=1.0;
  m_dMnEff=0.01;
  m_dLvl=m_dMxEffLvl;
  m_dCondensableFrac=1.0;
  m_dEffect=0.0;
  m_dMaxDuty=0.0;
  m_dMaxNfDuty=0.0;
  m_dMaxFlDuty=0.0;

  ClrVent();

  m_iSolveMode=HXM_LMTD;
  m_iConfig=HXC_ShellTube;
  m_fOn=true;

  m_EffectCurve.SetLength(2);
  m_EffectCurve.SetPt(0,0.0,0.0,0.0);
  m_EffectCurve.SetPt(1,1.0,1000.0,0.0);

  m_fPreReact=0;
  m_fPriIsHot=0;
  }

//--------------------------------------------------------------------------

CHXBlock::~CHXBlock()
  {
  };

//--------------------------------------------------------------------------

DDBValueLst CHXBlock::DDBEffect[]={
    {HXM_Effectiveness, "Effectiveness"},
    {HXM_LMTD, "Log_Mean_Temp_Diff"},
    {0}};
DDBValueLst CHXBlock::DDBFlwTypes[]={
    {HXC_ParFlw_1Pass , "ParallelFlw"},
    {HXC_CntrFlw_1Pass, "CounterFlw"},
    {HXC_ShellTube    , "Shell/Tube"},
    {HXC_nShellTube   , "nShell/Tube"},
    {HXC_Curve        , "Curve"},
    {0}};
DDBValueLst CHXBlock::DDBXSect[]={
    {CHXBlock::HXLC_RectXSect, "Rectangular"},
    {CHXBlock::HXLC_CircXSect, "Circular"},
    {0}};

void CHXBlock::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this, NULL, NULL, DDB_NoPage);
  DDB.Text(" ");
  DDB.Byte("Mode",    "", DC_,     "",    &m_iSolveMode, this, isParm|AffectsStruct, DDBEffect);
  DDB.Text(" ");

  DDB.Visibility(NSHM_All, m_iSolveMode==HXM_Effectiveness);
  DDB.Bool   ("Config(P/S)", "",       DC_,     "",  &m_iConfig, this, isParm, DDBFlwTypes);
  DDB.Double ("",            "Effect", DC_Frac, "%", &m_dEffect, this, 0);
  DDB.Text(" ");

  DDB.Visibility();
  DDB.CheckBox("On",          "",       DC_,     "",          &m_fOn,         this, isParm|SetOnChange);
  DDB.Visibility(NM_Probal|SM_All|HM_All); //should this HX sequencing option be enabled for dynamic?
  DDB.CheckBox("PreReact",    "",       DC_,     "",          &m_fPreReact,   this, isParm|SetOnChange);
  DDB.Visibility();
  DDB.Double("HTC",           "",       DC_HTC,  "kW/m^2.K",  &m_dHTC,        this, isParm);
  DDB.Double("Area",          "",       DC_Area, "m^2",       &m_dArea,       this, isParm);

  DDB.Visibility(NM_Dynamic|SM_All|HM_All, m_bLvlEffOn);
  DDB.Byte("CrossSection",    "",       DC_,     "",          &m_iXSect,      this, isParm|AffectsStruct, DDBXSect);
  DDB.Double ("MaxEffLvl",    "",       DC_Frac, "%",         &m_dMxEffLvl,   this, isParm);
  DDB.Double ("MinEffLvl",    "",       DC_Frac, "%",         &m_dMnEffLvl,   this, isParm);
  DDB.Double ("MaxEff",       "",       DC_Frac, "%",         &m_dMxEff,      this, isParm);
  DDB.Double ("MinEff",       "",       DC_Frac, "%",         &m_dMnEff,      this, isParm);
  DDB.Double ("Level",        "",       DC_Frac, "%",         xidHXLevel,     this, isResult);
  DDB.Double ("NonCondense",  "",       DC_Frac, "%",         xidHXNonCondenseFrac, this, isResult);
  DDB.Double ("EffectArea",   "",       DC_Area, "m^2",       xidHXEffArea,   this, isResult);

  DDB.Visibility();
  DDB.Double("U*A",           "",       DC_UA,   "kW/K",      xidHXEffUA,     this, isResult);
  DDB.Double("Duty",          "",       DC_Pwr,  "kW",        &m_dDuty,       this, isResult);
  DDB.Double("LMTD",          "",       DC_dT,   "C",         &m_dLMTD,       this, isResult);
  DDB.Double("LMTDFact",      "",       DC_Frac, "%",         &m_dLMTDFactor, this, (m_iSolveMode==HXM_LMTD) ? isParm : isResult|0);
  DDB.Double("",              "QmVent", DC_Qm,   "kg/s",      &m_QmVent,      this, isResult);//Connected ? 0 : isParm);
  DDB.Text(" ");

  m_Pri.BuildDataDefn("Pri", DDB, this, True, m_Pri.fConnected, True);
  m_Sec.BuildDataDefn("Sec", DDB, this, True, m_Sec.fConnected, False);

  if (m_iSolveMode==HXM_Effectiveness)
    DDB.Object(&m_EffectCurve, (TaggedObject*)this, NULL, NULL, DDB_RqdPage);

  DDB.Text(" ");
  DDB.EndStruct();
  }

//---------------------------------------------------------------------------

flag CHXBlock::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidHXLevel:
      DCB.D=Level();
      return 1;
    case xidHXNonCondenseFrac:
      DCB.D=NonCondensableFrac();
      return 1;
    case xidHXEffArea:
      DCB.D=EffArea();
      return 1;
    case xidHXEffUA:
      DCB.D=EffUA();
      return 1;
    case xidHXAreaUsed:
      DCB.D=m_dDuty/GTZ(m_dHTC*m_dLMTD);
      return 1;
    }
  if (TaggedObject::DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==1 && m_Pri.DataXchg(DCB))
    return 1;
  if (DCB.dwUserInfo==2 && m_Sec.DataXchg(DCB))
    return 1;
  return 0;
  }

// -------------------------------------------------------------------------

flag CHXBlock::ValidateData(ValidateDataBlk & VDB)
  {
//  dUA=fOn ? dHTC*m_dArea : 0.0;
  m_dMxEffLvl=Range(0.0, Min(m_dMxEffLvl, m_dMnEffLvl), 0.9);
  m_dMnEffLvl=Range(m_dMxEffLvl+0.1, m_dMnEffLvl, 1.0);
  m_dApproachDT=Max(0.01, m_dApproachDT);

  return true;
  }

//--------------------------------------------------------------------------

double CHXBlock::EffArea()
  {
  double Eff;
  if (m_bLvlEffOn && SolveBufferedMethod())//NetDynamicMethod())
    {
    double LvlDiff=m_dMnEffLvl-m_dMxEffLvl;
    double EffDiff=m_dMxEff-m_dMnEff;
    switch (m_iXSect)
      {
      case HXLC_RectXSect:
        Eff=Range(m_dMnEff, m_dMnEff+(m_dLvl-m_dMxEffLvl)/LvlDiff*EffDiff, m_dMxEff);
        break;
      case HXLC_CircXSect:
        {
        double L=Range(0.0, (m_dLvl-m_dMxEffLvl)/LvlDiff, 1.0);
        // translate for circle r= 0.5 H = Lvl;
        double ASeg=0.5*0.5*acos((0.5-L)/0.5)-(0.5-L)*sqrt(2*0.5*L-L*L);
        double ACir=0.5*0.5*PI;
        Eff=Range(m_dMnEff, m_dMnEff+(1.0-ASeg/ACir)*EffDiff, m_dMxEff);
        }
        break;
      default:
        Eff=1.0;
      };
    }
  else
    Eff=1.0;
  return m_dArea*Range(m_dMnEff, Eff, m_dMxEff)*(m_dCondensableFrac);
  }

//--------------------------------------------------------------------------

void CHXBlock::SetVent(SpConduit & rCd/*, double QVentRqd*/, double VentP)
  {
  m_pVent=&rCd;
  m_VentPress=VentP;
  }

//--------------------------------------------------------------------------

void CHXBlock::ClrVent()
  {
  m_QmVent=0.0;
  m_QmVentRqd=0.0;
  m_dNonCondVentFrac=1.0;
  m_VentPress=Std_P;
  m_pVent=NULL;
  }

//--------------------------------------------------------------------------

double CHXBlock::Effectiveness(double NTU, double C)
  {
  if (m_iConfig==HXC_Curve)
    return m_EffectCurve.Zxy(NTU, C);
  if ((NTU<1.0e-6) && (C<1.0e-6))
    return 0.0;
  else
    {
    C=Min(C, 0.9999999);
    double A,B;
    switch (m_iConfig)
      {
      case HXC_ParFlw_1Pass:
        return (1.0-exp(-NTU*(1.0+C)))/(1.0+C);
      case HXC_CntrFlw_1Pass:
        B=exp(-NTU*(1.0-C));
        return (1.0-B)/(1.0-C*B);
      case HXC_ShellTube:
        A=Sqrt(1.0+Sqr(C));
        B=exp(-NTU*A);
        return 2.0*(1.0-B)/((1.0+C)*(1.0-B)+(1.0+B)*A);
      case HXC_nShellTube:
        return 0.0;
      case HXC_Curve:
        ASSERT(0);
      }
    }
  return 0.0;
  }

//--------------------------------------------------------------------------

double CHXBlock::LMTD(flag CoCurrent, double PTi, double PTo, double STi, double STo)
  {
  double LSign, Diff1, Diff2, LogDif, TmpDiff;

  if (PTi > STi)
    LSign = 1; //     hot side is 2
  else
    LSign = -1; //    hot side is 1

  if (CoCurrent)
    { // Co Current
    Diff1 = LSign * (PTi - STi);
    Diff2 = LSign * (PTo - STo);
    }
  else
    { // Counter Current
    Diff1 = LSign * (PTi - STo);
    Diff2 = LSign * (PTo - STi);
    }

  double BigDiff=Max(Diff1, Diff2);
  double SmallDiff=1.0e-20;
//  Diff1=Max(Diff1, SmallDiff);
//  Diff1=(Diff1 >= 0.0) ? Max(Diff1, SmallDiff) : Min(Diff1, SmallDiff);
//  Diff2=(Diff2 >= 0.0) ? Max(Diff2, SmallDiff) : Min(Diff2, SmallDiff);
  Diff1=Max(Diff1, SmallDiff);
  Diff2=Max(Diff2, SmallDiff);
  if (Diff1<Diff2)
    Exchange(Diff1, Diff2);

  double LMTDCalc=1.0e-20;
  double COSign=1.0;
  if (fabs(Diff2) > 1.0e-20)
    {
    // if temperatures crossover then return (-) LMTD
    TmpDiff = (Diff1 / Diff2);
    if (TmpDiff < 0.0)
      {
      COSign = -1.0;
      TmpDiff = fabs(TmpDiff);
      Diff1 = fabs(Diff1);
      Diff2 = fabs(Diff2);
      }
    if (TmpDiff > 1.0e-20)
      {
      if (fabs(1.0 - TmpDiff) > 0.01)
        {
        LogDif = log(TmpDiff);
        LMTDCalc = (Diff1 - Diff2) / LogDif;
        }
      else
        LMTDCalc = (Diff1 + Diff2) / 2.0;
      }
    };

  return LMTDCalc*COSign;
  };

//--------------------------------------------------------------------------

double CHXBlock::LMTD1(flag CoCurrent, double PTi, double PTo, double STi, double STo)
  {
  double LSign, Diff1, Diff2, LogDif, TmpDiff;

  if (PTi > STi)
    LSign = 1; //     hot side is 2
  else
    LSign = -1; //    hot side is 1

  if (CoCurrent)
    { // Co Current
    Diff1 = LSign * (PTi - STi);
    Diff2 = LSign * (PTo - STo);
    }
  else
    { // Counter Current
    Diff1 = LSign * (PTi - STo);
    Diff2 = LSign * (PTo - STi);
    }

  double BigDiff=Max(Diff1, Diff2);
  double SmallDiff=1.0e-20;
  Diff1=(Diff1 >= 0.0) ? Max(Diff1, SmallDiff) : Min(Diff1, SmallDiff);
  Diff2=(Diff2 >= 0.0) ? Max(Diff2, SmallDiff) : Min(Diff2, SmallDiff);
  if (Diff1<Diff2)
    Exchange(Diff1, Diff2);

  double LMTDCalc=1.0e-20;
  double COSign=1.0;
  if (fabs(Diff2) > 1.0e-20)
    {
    // if temperatures crossover then return (-) LMTD
    TmpDiff = (Diff1 / Diff2);
    if (TmpDiff < 0.0)
      return 0.0;
    if (TmpDiff > 1.0e-20)
      {
      if (fabs(1.0 - TmpDiff) > 0.01)
        {
        LogDif = log(TmpDiff);
        LMTDCalc = (Diff1 - Diff2) / LogDif;
        }
      else
        LMTDCalc = (Diff1 + Diff2) / 2.0;
      }
    };

  return LMTDCalc*COSign;
  };

//--------------------------------------------------------------------------

double CHXBlock::LMTDSat(double Ti, double To, double Ts)
  {
  double Diff1;
  double Diff2;
  if (Ti<Ts)
    {
    Ts=Max(Ts, Ti+0.001); // Prevent overflow
    if (fabs(To-Ti)<1.0e-6)
      To=Ti+Sign(To-Ti)*1.0e-6;
    Diff1=GEZ(Ts-Ti);
    Diff2=GEZ(Ts-To);
    if (Diff2<1.0e-300)
      Diff2=1.0e-300;
    }
  else
    {
    Ts=Min(Ts, Ti-0.001); // Prevent overflow
    if (fabs(To-Ti)<1.0e-6)
      To=Ti+Sign(To-Ti)*1.0e-6;
    Diff1=LEZ(Ts-Ti);
    Diff2=LEZ(Ts-To);
    if (Diff2>-1.0e-300)
      Diff2=-1.0e-300;
    }

  double DT=fabs(To-Ti)/NZ(Ln(Diff1/Diff2));
//  if (fabs(DT)>100000.0)
//    DoBreak();
  return DT;
  };

//--------------------------------------------------------------------------

double CHXBlock::LMTDSat1(double Ti, double To, double Ts)
  {
  double Diff1;
  double Diff2;
  if (Ti>=Ts)
    {
    Ts=Max(Ts, Ti+0.001); // Prevent overflow
    if (fabs(To-Ti)<1.0e-6)
      To=Ti+Sign(To-Ti)*1.0e-6;
    Diff1=GEZ(Ts-Ti);
    Diff2=GEZ(Ts-To);
    if (Diff2<1.0e-300)
      Diff2=1.0e-300;
    }
  else
    {
    Ts=Min(Ts, Ti-0.001); // Prevent overflow
    if (fabs(To-Ti)<1.0e-6)
      To=Ti+Sign(To-Ti)*1.0e-6;
    Diff1=LEZ(Ts-Ti);
    Diff2=LEZ(Ts-To);
    if (Diff2>-1.0e-300)
      Diff2=-1.0e-300;
    }

  double DT=fabs(To-Ti)/NZ(Ln(Diff1/Diff2));

  // check for cross over
  if (Ti<Ts && To>Ts || Ti>Ts && To<Ts)
    DT+=-fabs(To-Ts);

//  if (fabs(DT)>100000.0)
//    DoBreak();
  return DT;
  };

//---------------------------------------------------------------------------
//
// Map x as follows
//  0 -> 1 Sensible Heat Xfer on both sides
//  1 -> 2 Sensible Heat Xfer on one side &
//         Boiling / Condensing on the other
//
//---------------------------------------------------------------------------

double CHXDutyFinder::TheoryDuty()
  {
  double TDuty=0.0;
  if (m_Blk.EffUA()>1.0e-6)
    {
    m_Blk.m_dLMTD=CHXBlock::LMTD(m_iFlwMode==HXF_CoCurrent, m_Pri.Ti, m_Pri.To, m_Sec.Ti, m_Sec.To);
    if (m_iCalcMeth==HXM_LMTD)
      TDuty = m_Blk.EffUA() * m_Blk.m_dLMTD * m_Blk.m_dLMTDFactor;
    else
      {
      double EffCpP=Max(m_Pri.Ci, fabs(m_Pri.Hi-m_Pri.Ho)/GTZ(fabs(m_Pri.To-m_Pri.Ti)));
      double EffCpS=Max(m_Sec.Ci, fabs(m_Sec.Hi-m_Sec.Ho)/GTZ(fabs(m_Sec.To-m_Sec.Ti)));

      double C, Cmin, NTU;
      Cmin = (EffCpP > EffCpS) ? EffCpS : EffCpP;
      if (Cmin >1.0e-6)
        {
        NTU = Min(m_Blk.EffUA()/Cmin, 5.5);
        C = (EffCpP > EffCpS) ? EffCpS/EffCpP : EffCpP/EffCpS;
        // Find e from the appropriate heat exchanger curve.
        m_Blk.m_dEffect = m_Blk.Effectiveness(NTU,C);
        // Calculates duty
        TDuty = m_Blk.m_dEffect * Cmin * fabs(m_Pri.Ti - m_Sec.Ti);
        }
      else
        {
        m_Blk.m_dEffect = 0.0;
        TDuty = 0.0;
        }
      m_Blk.m_dLMTDFactor = fabs(TDuty/NZ(m_Blk.EffUA()*m_Blk.m_dLMTD));
      }
    }
  else
    {
    m_Blk.m_dLMTD=0.0;
    m_Blk.m_dEffect = 0.0;
    TDuty = 0.0;
    if (m_iCalcMeth!=HXM_LMTD)
      m_Blk.m_dLMTDFactor=1.0;
    }

  return TDuty;
  };

//---------------------------------------------------------------------------

double CHXDutyFinder::Function(double Extent)
  {
  m_Pri.Sgn=(m_Blk.m_fPriIsHot ? -1 : 1);
  m_Sec.Sgn=-m_Pri.Sgn;

  // Sensible Heat Xfer Only
  m_Blk.m_dDuty=fabs(m_Blk.m_dMaxNfDuty)*Extent;
  m_Pri.Duty=m_Blk.m_dDuty*m_Pri.Sgn + m_Pri.dHEnv;
  m_Sec.Duty=m_Blk.m_dDuty*m_Sec.Sgn + m_Sec.dHEnv;
  if (m_Pri.m_pCd)
    m_Pri.m_pCd->QRestoreMass(m_Pri.MassImg);
//    m_Pri.m_pCd->SetSatPVapFraction(m_Pri.FlFrac);
  if (m_Sec.m_pCd)
    m_Sec.m_pCd->QRestoreMass(m_Sec.MassImg);
//    m_Sec.m_pCd->SetSatPVapFraction(m_Sec.FlFrac);
  Set_Enth_and_TOut(m_Pri, m_Pri.Hi+m_Pri.Duty);
  Set_Enth_and_TOut(m_Sec, m_Sec.Hi+m_Sec.Duty);

  if (m_Pri.m_pCd)
    m_Pri.m_pCd->SetPress(m_Pri.m_Po);
  if (m_Sec.m_pCd)
    m_Sec.m_pCd->SetPress(m_Sec.m_Po);
  m_Pri.iMode=QPF_Sensible;
  m_Sec.iMode=QPF_Sensible;

  m_Pri.Duty=m_Blk.m_dDuty*m_Pri.Sgn + m_Pri.dHEnv;
  m_Sec.Duty=m_Blk.m_dDuty*m_Sec.Sgn + m_Sec.dHEnv;

  double TDuty=TheoryDuty();
  double Diff=fabs(m_Blk.m_dDuty)-TDuty;
  return Diff;
  };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

void CHXBlock::EvalProducts(FlwNode* pNd)
  {
  m_fPriIsHot=(m_Pri.Ti>m_Sec.Ti);
  m_dMaxDuty=0.0;

  flag SecCondensing=0;

  flag CoCurrent=(m_iConfig==HXC_ParFlw_1Pass);

  if (TstNZ(m_Pri.Ci) && TstNZ(m_Sec.Ci) && m_fOn)
    {

    switch (m_iSolveMode)
      {
      case HXM_ApproachTemp:
        break;
      case HXM_Effectiveness:
      case HXM_LMTD:
        {
        m_dMaxDuty=EffUA()*fabs(m_Pri.Ti-m_Sec.Ti);
        m_dMaxNfDuty=m_dMaxDuty;
        m_dMaxFlDuty=m_dMaxNfDuty+0.0;

        m_Pri.EvalProducts(m_Pri.To);
        m_Sec.EvalProducts(m_Sec.To);

        m_Pri.dHEnv=m_Pri.m_pCd->totHf()-m_Pri.Hi;
        m_Sec.dHEnv=m_Sec.m_pCd ? m_Sec.m_pCd->totHf()-m_Sec.Hi : 0.0;

        // Find Mode Of Operation
        if (m_fPriIsHot)
          {
          SetTOut(m_Pri, m_Sec.Ti);
          m_dMaxDuty=m_Pri.Hi-m_Pri.Ho;
          SetTOut(m_Sec, m_Pri.Ti);
          m_dMaxDuty=m_Sec.Ho-m_Sec.Hi;
          }
        else
          {
          SetTOut(m_Sec, m_Pri.Ti);
          m_dMaxDuty=m_Sec.Hi-m_Sec.Ho;
          SetTOut(m_Pri, m_Sec.Ti);
          m_dMaxDuty=m_Pri.Ho-m_Pri.Hi;
          }

        double MaxHXExtent=1.0;

        CHXDutyFinder DutyFnd(this, CoCurrent, m_iSolveMode);
        DutyFnd.SetTarget(0.0);

        switch (DutyFnd.Start(0.0, MaxHXExtent))
          {
          case RF_OK:
            if (DutyFnd.Solve_Brent()==RF_OK)
              break;
            //SetCondition(2);
            break;
          case RF_LoLimit:
            DutyFnd.Function(0.0);
            break;
          case RF_HiLimit:
            // Flash the output
            DutyFnd.Function(MaxHXExtent);
            break;
          }

        if (m_Pri.m_pCd)
          {
          m_Pri.m_SatP = m_Pri.m_pCd->SaturationTotalP(m_Pri.To);
          m_Pri.m_SatPP = m_Pri.m_pCd->SaturationP(m_Pri.To);
          }
        if (m_Sec.m_pCd)
          {
          m_Sec.m_SatP = m_Sec.m_pCd->SaturationTotalP(m_Sec.To);
          m_Sec.m_SatPP = m_Sec.m_pCd->SaturationP(m_Sec.To);
          }
        }
      }
  // NB NB Only for connected parts

    m_dLMTD=LMTD(CoCurrent, m_Pri.Ti, m_Pri.To, m_Sec.Ti, m_Sec.To);
    m_dTheoryDuty=EffUA()*m_dLMTD;
    }
  else
    {
    m_Pri.To=m_Pri.Ti;
    m_Pri.Ho=m_Pri.Hi;
    m_Sec.To=m_Sec.Ti;
    m_Sec.Ho=m_Sec.Hi;
    m_dLMTD=0.0;
    m_dDuty=0.0;
    m_dTheoryDuty=EffUA()*m_dLMTD;
    m_Pri.iMode=QPF_Sensible;
    m_Sec.iMode=QPF_Sensible;
    m_Pri.Duty=0.0;
    m_Sec.Duty=0.0;
    }

  m_Pri.fConnected=0;
  m_Sec.fConnected=0;
  //m_Pri.QmVent=m_Pri.pVent ? m_Pri.pVent->QMass(): 0.0;
  //m_Sec.QmVent=m_Sec.pVent ? m_Sec.pVent->QMass(): 0.0;
  m_QmVent=m_pVent ? m_pVent->QMass(): 0.0;
  }

//---------------------------------------------------------------------------

void CHXBlock::EvalDerivs(pFlwNode pNd)
  {
  for (int s=0; s<2; s++)
    {
    CHXSide & Sd=(s==0 ? m_Pri : m_Sec);//Side[s];
    if (Sd.m_pCn)
      {
      SpContainer& Cn=*(Sd.m_pCn);
      Cn.SetDuty(!pNd->GetActiveHold()?Cn.Duty() + Sd.Duty : 0.0);// /IC.dTime;
      }
    }
  };


//--------------------------------------------------------------------------

flag CHXBlock::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case 1: pS="W\tCondensate Not Converged";                    return 1;
    case 2: pS="W\tVapour Not Condensed";                        return 1;
    case 3: pS="W\tTemperature Crossover";                       return 1;
    case 4: pS="W\tInvalid mode of operation";                   return 1;
    case 5: pS="W\tVenting - No Connection";                     return 1;
    case 6: pS="E\tDoes not support more than one vent connection"; return 1;
    case 7: pS="W\tVent Requirements not met";                   return 1;
    case 8: pS="E\tVapour present in condensate";                return 1;
    case 9: pS="E\tActual duty exceeds theoretical duty";        return 1;
    default:
      return TaggedObject::CIStrng(No, pS);
    }
  };

//============================================================================
//
//
//
//============================================================================

CHXBase::CHXBase(pFlwNode pAttach, TagObjClass *RqdHXClass, flag Fixed, LPTSTR Name) : CBlockEvalBase(BEId_HX, 0, Name, false)
  {
  pRqdHXClass=RqdHXClass;
  pHX=NULL;
  pNd=pAttach;
  fEnabled=false;
  fFixed=Fixed;
  if (Fixed)
    Open(Fixed);
  };

//--------------------------------------------------------------------------

CHXBase::~CHXBase()
  {
  delete pHX;
  };

//--------------------------------------------------------------------------

void CHXBase::SetRqdHXClass(TagObjClass * pHXClass)
  {
  pRqdHXClass=pHXClass;
  Open(/*pHXClass,*/ fFixed);
  };

//--------------------------------------------------------------------------

flag CHXBase::Open(TagObjClass * pHXClass, flag Fixed)
  {
  fEnabled=True;

  if (pHX && (pRqdHXClass==NULL || pHX->Class()==pRqdHXClass))
    return True;

  delete pHX;
  fFixed=Fixed;

  if (pRqdHXClass)
    pHX=(CHXBlock*)pRqdHXClass->Construct(NULL, "HX", pNd, TOA_Embedded);//pNd);
  else
    pHX=(CHXBlock*)CHXBlockClass.Construct(NULL, "HX", pNd, TOA_Embedded);//pNd);
  pNd->StructureChanged(NULL);

  fEnabled=pHX!=NULL;
  return fEnabled;
  };

//--------------------------------------------------------------------------

void CHXBase::Close()
  {
  if (fFixed)
    return;
  fEnabled=False;
//  if (pHX==NULL)
//    return;
//  delete pHX;
//  pHX=NULL;
  };

//--------------------------------------------------------------------------

void CHXBase::Add_OnOff(DataDefnBlk& DDB, dword Flags)
  {
  DDB.Byte("",     "HeatXChg",  DC_,    "",     xidHXEnable,  pNd, Flags, GetOnOffValLst());
  }

//--------------------------------------------------------------------------

void CHXBase::BuildDataDefn(DataDefnBlk& DDB, pchar pTag_, pchar pTagComment, DDBPages PageIs, dword UserInfo)
  {
  if (pTag_==NULL)
    pTag_="HX";
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(NSHM_All, fEnabled);

  DDB.BeginObject(pNd, pTag_, "HeatXBlk", NULL, PageIs);
//  DDBValueLstMem DDB0;
//  TagObjClass::GetSDescValueLst(CHXBlock::GroupName, DDB0);
//  DDB.String  ("Model",      "",       DC_    , "",      xidHXEqnNm  , pNd,fFixed ? 0 : isParm, DDB0());
  if (pHX)
    {
    DDB.SuppressNextBlock();
    pHX->BuildDataDefn(DDB);
    }
  DDB.EndObject();

  DDB.SetVisibility(Old);
  }

//--------------------------------------------------------------------------

flag CHXBase::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidHXEnable:
      if (DCB.rB)
        if (*DCB.rB)
          Open(*DCB.rB, NULL, fFixed);
        else if (!fFixed)
          Close();
      DCB.B=OpenStatus();// (Enabled());
      return 1;
    }
  return 0;//return pGSM ? pGSM->DataXchg(DCB) : 0;
  }

// ==========================================================================
//
//  HeatX Utilities
//
// ==========================================================================

CHXSdJoinInfo::CHXSdJoinInfo(pFlwNode Nd, int IdIn, int IdOut, int IdVent) :
  m_Qi("Qi", NULL, TOA_Unknown),
  m_Qo("Qo", NULL, TOA_Unknown)
  {
  pNd=Nd;
  m_ioFlwOut=-1;
  m_ioFlwVnt=-1;
  idMask=0;
  if (IdIn>=0)
    {
    for (int i=IdIn; i<=IdOut; i++)
      idMask|=Id_2_Mask(i);
    }
  idVentMask = IdVent>=0 ? Id_2_Mask(IdVent):0;
  };

//--------------------------------------------------------------------------

flag CHXSdJoinInfo::TstConnected()
  {
  int jJ=0, jI=0;
  m_ioFlwOut=-1;
  for (int i=0; i<pNd->NoFlwIOs(); i++)
    {
    if ((pNd->IOIdMask_Self(i)&idMask)!=0)
      {
      ioJoin[jJ++] = i;
      if (pNd->IO_In(i))
        ioFlwIn[jI++] = i;
      else if (pNd->IO_Out(i))
        m_ioFlwOut = i;
      }
    if ((pNd->IOIdMask_Self(i)&idVentMask)!=0)
      {
      ioJoin[jJ++] = i;
      if (pNd->IO_Out(i))
        m_ioFlwVnt= i;
      }
    }
  ioFlwIn[jI++] = -1;
  ioJoin[jJ++] = -1;
  return (ioJoin[0] >= 0);
  }

//--------------------------------------------------------------------------

void CHXJoinInfo::SetJoinIds(int JoinId0, int PriDirectConn, int SecDirectConn)
  {
  P.fOpen=P.TstConnected() && !PriDirectConn;
  S.fOpen=S.TstConnected() && !SecDirectConn;
  P.iJoinId = JoinId0++;
  S.iJoinId = JoinId0++;
  };

// ==========================================================================
//
//
//
//==========================================================================

CFlwEqnGrp HeatXDPGroup("HeatXDP", "HX_VFlw");

//==========================================================================
//
//
//
//==========================================================================

XID xidKConst = MdlBsXID(17001);
XID xidKFact  = MdlBsXID(17000);
XID xidPDiam  = MdlBsXID(17002);
XID xidVDiam  = MdlBsXID(17003);

#define DllImportExport

DEFINE_TAGOBJ(HX_KFact);
class HX_KFact : public CFlwEqn
  {
//  enum TeeTypes { TT_Straight, TT_Branch};
  static DDBValueLst DDBTT[];
  public:
    //byte           iType;
    double            dPipeArea;
    double            dDensMeas;
    double            dViscMeas;
    double            dVelMeas;
    double            dFricFac;
    DualDbl           KConst;
    CFrictionFactor   m_FF;
    double            m_dDP;

    HX_KFact(pTagObjClass pClass_, pchar pTag, TaggedObject* pAttach, TagObjAttachment eAttach);

    virtual ~HX_KFact();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(HX_KFact, HeatXDPGroup.Name(), "HX_KFact", "", TOC_SYSTEM,
                  "K Factor",
                  "K Factor - Darcy");

HX_KFact::HX_KFact(pTagObjClass pClass_, pchar pTag, TaggedObject* pAttach, TagObjAttachment eAttach) : \
CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  //iType=TT_Straight;
  KConst.SetVal(10, this);
  dPipeArea=0.1;
  dDensMeas=0;
  dViscMeas=0;
  dVelMeas=0;
  dFricFac=0;
  m_dDP=0;
  };
//--------------------------------------------------------------------------

HX_KFact::~HX_KFact()
  {
  }

//--------------------------------------------------------------------------

void HX_KFact::BuildDataDefn(DataDefnBlk & DDB)
  {
  KConst.BuildDataDefn(DDB, "KConst", "", DC_,    "",      xidKConst, NULL,   "Initial", "Required");
  //DDB.Byte   ("Type",       "",           DC_,    "",      &iType,  NULL,  isParm|SetOnChange, DDBTT);
  DDB.Double ("PipeDiam",   "D-Pipe",     DC_L,   "mm",    xidPDiam,  NULL,  isParmStopped);
  DDB.Double ("Density",    "Rho",        DC_Rho, "kg/m^3",&dDensMeas, NULL, isResult);
  DDB.Double ("Viscosity",  "Visc",       DC_Visc,"cP",    &dViscMeas, NULL, isResult);
  DDB.Double ("Velocity",   "Vel",        DC_Ldt, "m/s",   &dVelMeas,  NULL, isResult);
  DDB.Double ("FricFac",    "fT",         DC_,    "",      &dFricFac,  NULL, isResult);
  m_PhD.KFact.BuildDataDefn(DDB, "ResistCoeff", "K", DC_, "", xidKFact, NULL, "Calculated", "Required");
  DDB.Double ("PressDrop",   "DP",        DC_DP,    "kPa",    &m_dDP,     NULL, isResult);
  BuildDataDefnOveride(DDB);
  };

// --------------------------------------------------------------------------

flag HX_KFact::DataXchg(DataChangeBlk & DCB)
  {
  if (KConst.DataXchg(DCB, xidKConst, this))
    return 1;
  if (m_PhD.KFact.DataXchg(DCB, xidKFact, this))
    return 1;
  switch (DCB.lHandle)
    {
    case xidPDiam: DCB.D=Area2Diam(dPipeArea); return 1 ;
    };
  return CFlwEqn::DataXchg(DCB);
  }

//--------------------------------------------------------------------------

flag HX_KFact::ValidateData(ValidateDataBlk & VDB)
  {
  //KConst.SetVal(iType==TT_Branch ? 50.0 : 10.0, this);
  //KConst.SetVal(10.0, this);
  KConst.SetVal(ValidateRange(VDB, "KConst", 0.001, KConst.Val(), 1000.0), this);
  if (Valid(m_PhD.KFact.Ovr()))
    m_PhD.KFact.SetOvr(ValidateRange(VDB, "K", 0.0, m_PhD.KFact.Ovr(), 1.0E6), this);
  return True;
  }

//--------------------------------------------------------------------------

flag HX_KFact::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  dDensMeas=Max(0.001, FE.MeanRho(pProps));
  dViscMeas=FE.MeanViscosity(pProps);
  if (pPhD0 && pPhD0->IsPipe())
    dPipeArea=pPhD0->Area();
  else if (pPhD1 && pPhD1->IsPipe())
    dPipeArea=pPhD1->Area();
  else
    {};

  m_PhD.SetArea(dPipeArea);

  dVelMeas=FE.SetVelMeasRange(dDensMeas, FE.Area(), 0.001);

  double DqScl=1.001;

  double D=Area2Diam(dPipeArea);
  dFricFac=m_FF.Turbulent(D, 5.0e-5, dDensMeas, dViscMeas);
  m_PhD.KFact.SetVal(KConst()*dFricFac, this);
  double DP=PressDropKFact(FE.VelMeas(), dDensMeas, m_PhD.KFact(), 1.0);
  double DP1=PressDropKFact(FE.VelMeas(DqScl), dDensMeas, m_PhD.KFact(), 1.0);

  double OnePhDPQ=-FE.QmSign()*DP;
  double OnePhDPZ=/*FE.QmSign()**/-0.001*9.81*FE.Rise()*dDensMeas;
  FE.SetVelocity(FE.QmSign()*FE.VelMeas());
  FE.SetDPq(OnePhDPQ, -FE.QmSign()*(DP1-DP)/(FE.DQmMeas(DqScl)));
  FE.SetDPa(0.0, 0.0);
  FE.SetSlipRatio();
  FE.SetVoidFraction();
  FE.SetOnePhPart();
  //FE.xSetTempOut(4);

  FE.SetDPz(OnePhDPZ);
  m_dDP=DP;
  ASSERT_ALWAYS(Valid(FE.DPq()) && _finite(FE.DPq()), "Bad Friction Value", __FILE__, __LINE__);

  return True;
  };

//==========================================================================
//
//  Heat Exchange Flow Drop Model
//
//==========================================================================
/*#D:#T:DP Prop to Qm for a heat exchanger
#X:#h<General Description>#nThis is a heat exchange flow equation.
It is a linear flow equation, as it assumes that the pressure drop within a
pipe is proportional to the mass flow within the pipe.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Resistn> : The resistance within the pipe.#n
#n
#n#h<Other>#n
Short name:HX_Line#n
Model type:Heat Flow Equation#n
#G:Heat Flow Equations
*/

IMPLEMENT_FLWEQN(HX_LinearEqn, HeatXDPGroup.Name(), "HX_Line", "", TOC_SYSTEM,
                  "DP Prop to Qm",
                  " Pressure drop relationship dP proportioal to Qm");

HX_LinearEqn::HX_LinearEqn(pTagObjClass pClass_, pchar pTag, TaggedObject* pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  R_Fxd=0.0;
  }

//--------------------------------------------------------------------------

HX_LinearEqn::~HX_LinearEqn()
  {
  }

//--------------------------------------------------------------------------

void HX_LinearEqn::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Resistance",   "Resistn",  DC_    , "",      &R_Fxd        ,this  ,isParm);
  };

//--------------------------------------------------------------------------

//void HX_LinearEqn::StartSolution(CFlwBlkBase & FE)
//  {
//  };

//--------------------------------------------------------------------------

flag HX_LinearEqn::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  FE.SetVelocity(dNAN);
  FE.SetQmMeasRange(Max(0.001, FE.MeanRho(pProps)), 1.0e-6);
  FE.SetDPq(-FE.QmSign()*FE.QmMeas()*R_Fxd, -FE.QmSign()*R_Fxd);
  return True;
  };


//==========================================================================
//
//  Heat Exchange Flow Drop Model
//
//==========================================================================
/*#D:#T:DP Curve for a heat exchanger
#X:#h<General Description>#nThis is a heat exchange flow equation.
Pressure drop vs flow curve. ???
#n#n
#n#h<Variables to be supplied by the user>#n
#i<No_of_units> : ???.#n
#n#n
#h<Associated variables>#n
#i<dP> : ???#n
#n
#n#h<Other>#n
Short name:HX_Other#n
Model type:Heat Flow Equation#n
#G:Heat Flow Equations
*/

IMPLEMENT_FLWEQN(HX_OtherEqn, HeatXDPGroup.Name(), "HX_Other", "", TOC_SYSTEM,
                  "dP curve",
                  "Pressure drop vs flow curve");

HX_OtherEqn::HX_OtherEqn(pTagObjClass pClass_, pchar pTag, TaggedObject* pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach),
  PresFlowCurve("DataOnly", "PQCurve", this, TOA_Embedded)
  {
  NoUnits = 1.0;
  m_dP    = 0.0;
  }

//--------------------------------------------------------------------------

HX_OtherEqn::~HX_OtherEqn()
  {
  }

//--------------------------------------------------------------------------

void HX_OtherEqn::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("No_of_units",   "NoUnits?",  DC_    , "",      &NoUnits,     this  ,isParm);
  DDB.Double ("Pressure_Drop", "dP",        DC_P   , "kPa",   &m_dP,        this  ,isResult|0);

  DDB.Object(&PresFlowCurve,  this, NULL, NULL, DDB_RqdPage);
  };

//--------------------------------------------------------------------------

//void HX_OtherEqn::StartSolution(rFlwBlkBase FE)
//  {
//  };
//
//--------------------------------------------------------------------------

flag HX_OtherEqn::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double Qflow, dQflow;
  FE.SetQmMeasRange(Max(0.001, FE.MeanRho(pProps)), 1.0e-6);
  Qflow  = FE.QmMeas() / NoUnits;
  dQflow = Qflow * 0.001;
  FE.SetVelocity(dNAN);
  if (Qflow < 1e-3)
    {
    FE.SetDPq(0.0, 0.0);
    }
  else
    {
    double P1 = -PresFlowCurve.Yx(Qflow + dQflow);
    double P2 = -PresFlowCurve.Yx(Qflow - dQflow);
    FE.SetDPq(-PresFlowCurve.Yx(Qflow), (P1 - P2) / (2.0 * dQflow));
    }
  m_dP = FE.DPq();
  return True;
  };

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:DP Prop to Qm for a pipe
#X:#h<General Description>#nThis is a flow equation for a pipe.
It is a linear flow equation, as it assumes that the pressure drop within a
pipe is proportional to the mass flow within the pipe.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Resistn> : The resistance within the pipe.#n
#i<Length> : The physical length of the pipe.#n
#i<Diam> : The inner pipe diameter.#n
#n
#n#h<Other>#n
Short name:FE_Line#n
Model type:Pipe Flow Equation#n
#G:Pipe Flow Equations
*/

IMPLEMENT_FLWEQN(HX_VolFlow, HeatXDPGroup.Name(), "HX_VFlw", "", TOC_SYSTEM,
                  "Simple Volumetric Flow",
                  "Simple Volumetric Flow");

HX_VolFlow::HX_VolFlow(pTagObjClass pClass_, pchar pTag, TaggedObject* pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  //Diam=0.1;
  PwrLaw=2.0;
  OpDP=1.0;
  OpVol=dNAN;
  OpNVol=10.0;
  m_DP=0;
  }

//--------------------------------------------------------------------------

HX_VolFlow::~HX_VolFlow()
  {
  }

//--------------------------------------------------------------------------

void HX_VolFlow::BuildDataDefn(DataDefnBlk & DDB)
  {
//  DDB.Double ("Diam",     "",   DC_L,   "mm",    &Diam         ,this  ,isParm);
  DDB.Double ("PowerLaw",  "",   DC_,    "",       &PwrLaw       ,this  ,isParm);
  DDB.Double ("Oper_DP",   "",   DC_DP,  "kPa",    &OpDP         ,this  ,isParm);
  DDB.Double ("Oper_Vol",  "",   DC_Qv,  "m^3/s",  &OpVol        ,this  ,isParm|NAN_OK);
  DDB.Double ("Oper_NVol", "",   DC_NQv, "Nm^3/s", &OpNVol       ,this  ,isParm|NAN_OK);
  //m_PhD.KFact.BuildDataDefn(DDB, "ResistCoeff", "K", DC_, "", xidKFact, NULL, "Calculated", "Required");
  DDB.Double ("PressDrop",   "DP",        DC_DP,    "kPa",    &m_DP,     NULL, isResult);
  BuildDataDefnOveride(DDB);
  };

//--------------------------------------------------------------------------

flag HX_VolFlow::ValidateData(ValidateDataBlk & VDB)
  {
  if (Valid(OpVol))
    OpNVol=dNAN;
  else if (!Valid(OpNVol))
    OpNVol=10.0;
//  if (!PhDOK())
//    return true;
  return True;
  }

//--------------------------------------------------------------------------

//void HX_VolFlow::StartSolution(rFlwEqnBlk FE)
//  {
//  };

//--------------------------------------------------------------------------

flag HX_VolFlow::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double dPq1, dPq2;
  if (Valid(OpVol))
    {
    double Rho=Max(0.1, FE.MeanRho(pProps));
    double K=fabs(OpDP)/Pow(fabs(OpVol), PwrLaw);
    FE.SetQvMeasRange(Rho, 1.0);
    double Vol1 = FE.QvMeas();
    double dQm  = FE.DQmMeas(1.001);
    double Vol2 = FE.QvMeas(1.001);

    dPq1 = -FE.QmSign()*K*Pow(Vol1,PwrLaw);
    dPq2 = -FE.QmSign()*K*Pow(Vol2,PwrLaw);
    FE.SetDPq(dPq1, (dPq2 - dPq1)/dQm);
    }
  else
    {
    double NRho=Max(0.1, FE.MeanRho(pProps)*Norm_P/GTZ(FE.MeanPress())*FE.MeanTemp(pProps)/Norm_T);
    double K=fabs(OpDP)/Pow(fabs(OpNVol), PwrLaw);

    FE.SetQvMeasRange(NRho, 1.0);
    double NVol1 = FE.QvMeas();
    double dQm  = FE.DQmMeas(1.001);
    double NVol2 = FE.QvMeas(1.001);

    //double NVol1 = RangeFlow(FE.QmMeas()/NRho, 1.0);
    //FE.SetQmMeas(NVol1 * NRho);
    //double dQm  = FE.QmMeas() * 0.001;
    //double NVol2 = (FE.QmMeas() + dQm)/NRho;

    dPq1 = -FE.QmSign()*K*Pow(NVol1,PwrLaw);
    dPq2 = -FE.QmSign()*K*Pow(NVol2,PwrLaw);
    FE.SetDPq(dPq1, (dPq2 - dPq1)/dQm);
    }

  m_DP = FE.DPq();
  FE.SetFunctOfPress();

  return True;
  };

//===========================================================================

