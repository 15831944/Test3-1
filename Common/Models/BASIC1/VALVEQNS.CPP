//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define __VALVEQNS_CPP
#include "valveqns.h"
#include "m_bstres.h"

//#include "optoff.h"

#define dbgValve 01

inline double Deg2Rad(double x) { return x*PI/180; };

//==========================================================================
//
//
//
//==========================================================================

struct CraneVlvInfo
  {
  byte    iType;
  LPTSTR  pName;
  bool    bBetaReqd;
  bool    bThetaReqd;
  bool    bAlphaReqd;
  double  dKConstDefault;
  double  dBetaDefault;
  double  dThetaAlphaDefault;

  };

#define DllImportExport
DEFINE_TAGOBJ(VE_Crane);
class VE_Crane : public CFlwEqn
  {
  enum ValveCharacteristics {
    VC_Linear,
    VC_EqPerc,
    VC_Quick
    };

  enum ValveTypes {
    CVT_Gate,
    CVT_Globe,
    CVT_Angle,
    CVT_Ball,
    CVT_ButterFly,
    CVT_Plug,
    };

  static CraneVlvInfo CVI[];
  static DDBValueLst DDBCVT[];

  public:
    byte            iType;
    double          dK1;
    double          dK2;
    double          dVlvArea;
    double          dPipeArea;
    double          dBeta;
    double          dTheta;
    DualDbl         KConst;
    double          dDensMeas;
    double          dViscMeas;
    double          dVelMeas;
    double          dFricFac;
    CFrictionFactor m_FF;
    double          m_dDP;
    byte            m_Character;

    VE_Crane(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~VE_Crane();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);

  };

CraneVlvInfo VE_Crane::CVI[]=
  {
    {CVT_Gate     , "Gate",       true, true, false,   8.0, 0.707, Deg2Rad(90)},
    {CVT_Globe    , "Globe",      true, true, false, 340.0, 0.707, Deg2Rad(90)},
    {CVT_Angle    , "Angle",      true, true, false,  55.0, 0.707, Deg2Rad(90)},
    {CVT_Ball     , "Ball",       true, true, false,   3.0,   1.0, Deg2Rad(90)},
    {CVT_ButterFly, "ButterFly",  false,false,false,  45.0,   1.0, Deg2Rad(0)},
    {CVT_Plug     , "Plug",       true, false,false,  18.0, 0.707, Deg2Rad(90)},
  };

DDBValueLst VE_Crane::DDBCVT[]={
    {CVI[0].iType, CVI[0].pName},
    {CVI[1].iType, CVI[1].pName},
    {CVI[2].iType, CVI[2].pName},
    {CVI[3].iType, CVI[3].pName},
    {CVI[4].iType, CVI[4].pName},
    {CVI[5].iType, CVI[5].pName},
    {0}};


IMPLEMENT_FLWEQN(VE_Crane, ValveGroup.Name(), "VE_Crane", "", TOC_SYSTEM,
                 "Crane K Factor Valve",
                 "Crane K Factor Valve");

VE_Crane::VE_Crane(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  iType=CVT_Globe;
  dBeta=1.0;
  dTheta=Deg2Rad(90);
  dVlvArea=0.1;
  dPipeArea=0.1;
  dK1=0;
  dK2=0;
  dDensMeas=0;
  dViscMeas=0;
  dVelMeas=0;
  dFricFac=0;
  m_dDP=0;
  m_Character=VC_EqPerc;
  KConst.SetVal(CVI[iType].dKConstDefault, this);
  }

//--------------------------------------------------------------------------

VE_Crane::~VE_Crane()
  {
  }

//--------------------------------------------------------------------------

XID xidVType          = MdlBsXID(17000);
XID xidKConst         = MdlBsXID(17001);
XID xidKFact          = MdlBsXID(17002);
XID xidPDiam          = MdlBsXID(17003);
XID xidVDiam          = MdlBsXID(17004);

void VE_Crane::BuildDataDefn(DataDefnBlk & DDB)
  {
  //if (ProbalMode())
  //  {
  //  }
  //else if (DynFlowMode())
  //  {
  //  //DDB.Visibility(SM_DynBoth|HM_All, !IsSrcSnk && (IsXfer/* || dbgfile()*/));
  //  DDB.Double ("Qm_RqdCapacity", "Qm_Capacity",  DC_Qm,   "kg/s",  &m_RqdQmCap,     this, isParm|NAN_OK, DDBNAN_NotSpecd);
  //  DDB.Double ("Qm_MinCapacity", "Qm_MinCap",    DC_Qm,   "kg/s",  &m_MinQmCap,     this, isParm);
  //  DDB.Double ("Qm_MaxCapacity", "Qm_MaxCap",    DC_Qm,   "kg/s",  xidQmMaxCapacity, this, isParm|NAN_OK, DDBNAN_NotSpecd);
  //  //if (IsXfer)
  //  //  {
  //  //  static DDBValueLst DDBXfCapOpt[]={
  //  //    {XCO_Spill,   "Spill"   },
  //  //    {XCO_Accept,  "Accept"  },
  //  //    {0}};
  //  //  DDB.Byte  ("OverCapacity", "",     DC_,     "",      &m_iXferCapOption, this, isParm|SetOnChange, DDBXfCapOpt);
  //  //  }
  //  }
  //else
  //  {
  //  DDB.Double ("",                "Qm_Rqd",      DC_Qm,   "kg/s",  &m_ReqdQm,           this, isParm|NAN_OK, DDBNAN_NotSpecd);
    static DDBValueLst DDB0[]={
      {VC_Linear, "Linear"},
      {VC_EqPerc, "EqualPerc"},
      {VC_Quick,  "Quick"},
      {0}};
    DDB.Byte   ("Characteristic", "Char",  DC_     , "",        &m_Character, NULL, isParm, DDB0);
    DDB.Byte   ("",           "Type",       DC_,      "",       xidVType,   NULL, isParm|SetOnChange, DDBCVT);
    KConst.BuildDataDefn(DDB,  "KConst", "", DC_,      "",       xidKConst,  NULL, "Initial", "Required");
    if (CVI[iType].bBetaReqd)
      DDB.Double ("",         "Beta",       DC_,      "",       &dBeta,     NULL, isParm);
    if (CVI[iType].bThetaReqd)
      DDB.Double ("",         "Theta",      DC_Ang,   "deg",    &dTheta,    NULL, isParm);
    DDB.Double ("PipeDiam",   "D-Pipe",     DC_L,     "mm",     xidPDiam,   NULL, 0);
    DDB.Double ("ValveDiam",  "D-Vlv",      DC_L,     "mm",     xidVDiam,   NULL, 0);
    DDB.Double ("",           "K1",         DC_,      "",       &dK1,       NULL, 0);
    DDB.Double ("",           "K2",         DC_,      "",       &dK2,       NULL, 0);
    DDB.Double ("Density",     "",          DC_Rho,   "kg/m^3", &dDensMeas, NULL, isResult);
    DDB.Double ("Viscosity",   "",          DC_Visc,  "cP",     &dViscMeas, NULL, isResult);
    DDB.Double ("Velocity",   "Vel",        DC_Ldt,   "m/s",    &dVelMeas,  NULL, isResult);
    DDB.Double ("FricFac",     "fT",        DC_,      "",       &dFricFac,  NULL, isResult);
    m_PhD.KFact.BuildDataDefn(DDB, "ResistCoeff", "K", DC_, "",   xidKFact,   NULL, "Calculated", "Required");
    DDB.Double ("PressDrop",   "DP",        DC_DP,    "kPa",    &m_dDP,     NULL, isResult);
    BuildDataDefnOveride(DDB);
  //  }
  }

// --------------------------------------------------------------------------

flag VE_Crane::DataXchg(DataChangeBlk & DCB)
  {
  if (KConst.DataXchg(DCB, xidKConst, this))
    return 1;
  if (m_PhD.KFact.DataXchg(DCB, xidKFact, this))
    return 1;
  switch (DCB.lHandle)
    {
    case xidVType:
      if (DCB.rB)
        {
        iType=*DCB.rB;
        KConst.SetVal(CVI[iType].dKConstDefault, this);
        dBeta=CVI[iType].dBetaDefault;
        dTheta=CVI[iType].dThetaAlphaDefault;
        }
      DCB.B=iType;
      return 1;
    case xidPDiam:
      DCB.D=Area2Diam(dPipeArea);
      return 1 ;
    case xidVDiam: DCB.D=Area2Diam(dVlvArea);
      return 1 ;
    //case xidQmMaxCapacity:
    //  {
    //  if (DCB.rD)
    //    m_MaxQmCap = Valid(*DCB.rD) ? Max(m_MinQmCap, *DCB.rD) : dNAN;
    //  DCB.D=m_MaxQmCap;
    //  return 1;
    //  }
    };
  return CFlwEqn::DataXchg(DCB);
  }

// --------------------------------------------------------------------------

// See ButterflyKConst.XLS
inline void SetButterFlyKConst(DualDbl & KConst, double Area, TaggedObject * This)
  {
  KConst.SetVal(49.998*exp(-1.2487*Area2Diam(Area)), This);
  }

flag VE_Crane::ValidateData(ValidateDataBlk & VDB)
  {
  dBeta=ValidateRange(VDB, "Beta", 1e-6, dBeta, 1.0);
  dTheta=ValidateRange(VDB, "Theta", 0.0, dTheta, 180.0/PI);

  if (iType==CVT_ButterFly)
    SetButterFlyKConst(KConst, dPipeArea, this);

  return true;
  }

//--------------------------------------------------------------------------

//  FrictionFactorTurbulent(                  y = 3.9738E-03x2 - 1.0265E-03x + 1.1624E-02

//static double  Reynolds(double Diam, double Vel, double Dens, double LVisc) { return Diam*Vel*Dens/LVisc; };
////static double  RelRough(double Diam) { return Roughness/Diam; };
////static double  FrictionFact(double Diam, double Vel, double Dens);
//
//static double FrictionFact(double Diam, double Roughness, double Vel, double Dens, double LVisc)
//  {
//  double Re = Reynolds(Diam, Vel, Dens, LVisc);
//  double A = Pow((-2.457 * log(Pow(7.0/Re,0.9) + 0.27 * Roughness/Diam)), 16);
//  double B = Pow((37530.0/Re), 16);
//  // Crane Eqn 1-4
//  double F_Fac = 8 * Pow((Pow((8/Re), 12) + 1.0/Pow((A + B), 1.5)),0.0833);
//  return F_Fac;
//  };
//
//static double  FrictionFactTurbulent(double Diam, double Roughness, double Dens, double LVisc) { return FrictionFact(Diam, Roughness, 1e3, Dens, LVisc); };

//--------------------------------------------------------------------------


flag VE_Crane::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 1.0);
  if (Regulation > MinValveOpening)
    {
    switch (m_Character)
      {
      case VC_EqPerc : Regulation=Sqr(Regulation);         break;
      case VC_Quick  : Regulation=sqrt(Regulation);        break;
      }

    FE.SetQmFree();

    dDensMeas=FE.MeanRho(pProps);
    dDensMeas=Min_FE_Rho(dDensMeas);
    dViscMeas=FE.MeanViscosity(pProps);

    if (pPhD0 && pPhD0->IsPipe())
      dPipeArea=pPhD0->Area();
    else if (pPhD1 && pPhD1->IsPipe())
      dPipeArea=pPhD1->Area();
    else
      {};

    dVlvArea=dPipeArea*dBeta*dBeta;
    double dVlvAreaUse=dVlvArea*Regulation;
    double D=Area2Diam(dPipeArea);
    dFricFac=m_FF.Turbulent(D, 5.0e-5, dDensMeas, dViscMeas);
    switch (iType)
      {
      case CVT_Gate:
      case CVT_Ball:
        dK1=KConst()*dFricFac;
        if (dBeta<0.9999)
          {
          double B2=dBeta*dBeta;
          if (dTheta*180/PI<45.0)
            dK2=(dK1 + sin(dTheta/2)*(0.8*(1-B2) + 2.6*(Sqr(1-B2)))) / (B2*B2);
          else
            dK2=(dK1 + 0.5*Sqrt(sin(dTheta/2))*(1-B2) + (Sqr(1-B2))) / (B2*B2);
          }
        else
          dK2=dK1;
        break;
      case CVT_Globe:
      case CVT_Angle:
        dK1=KConst()*dFricFac;
        if (dBeta<0.9999)
          {
          double B2=dBeta*dBeta;
          dK2=(dK1 + dBeta*(0.5*(1-B2) + (Sqr(1-B2)))) / (B2*B2);
          }
        else
          dK2=dK1;
        break;
      case CVT_ButterFly:
        SetButterFlyKConst(KConst, dPipeArea, this);
        dK1=KConst()*dFricFac;
        dK2=dK1;
        break;
      case CVT_Plug:
        dK1=KConst()*dFricFac;
        if (dBeta<0.9999)
          {
          double B2=dBeta*dBeta;
          dK2=(dK1 + 0.5*Sqrt(sin(dTheta/2))*(1-B2) + (Sqr(1-B2))) / (B2*B2);
          }
        else
          dK2=dK1;
        break;
      default:
        dK1=1000;
        dK2=dK1;
      }
    m_PhD.KFact.SetVal(dK2, this);

    FE.SetVelMeasRange(dDensMeas, dVlvAreaUse, 0.001);
    double Qm0=FE.QmMeas();
    double Qm1=FE.QmMeas(1.001);
    double Vel0=FE.VelMeas();
    double Vel1=FE.VelMeas(1.001);

    dVelMeas=Vel0;
    double DP0, DP1;
    if (FE.Linearised())
      {
      DP0=FE.LinearisedDP(Qm0, Regulation);
      DP1=FE.LinearisedDP(Qm1, Regulation);
      }
    else
      {
      DP0=PressDropKFact(Vel0, dDensMeas, m_PhD.KFact());
      DP1=PressDropKFact(Vel1, dDensMeas, m_PhD.KFact());
      }
    FE.SetDPq(DP0, (DP1-DP0)/FE.DQmMeas(1.001));
    //FE.SetDerivsBad(fabs(DP1-DP0)<1e-6*DP0);
    FE.SetDPb(0.0, 0.0);
    m_dDP=DP0;
    #if dbgValve
    if (0)//this->DoDbgBrk())
      {
      dbgpln("B_Valve Liq Qm:%12.5f Rho:%12.5f Visc:%12.5f Area:%12.5f D:%12.5f Beta:%12.5f Reg:%12.5f K1:%12.5f K2:%12.5f DP0:%12.5f %s",
        Qm0,dDensMeas,dViscMeas,dVlvAreaUse,D,dBeta,Regulation,dK1,dK2,DP0, FullObjTag());
      }
    #endif
    }
  else
    {
    //Cv=0.0;
    FE.SetQmReqd(0.0);
    }
  FE.SetRegulation(Regulation);

  return True;
  }

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:DP prop to Qm for a Valve
#X:#h<General Description>#nThis is method of modelling the pressure drop versus
flow relationship of a valve.  This flow equation assumes that the pressure drop
within the valve is a linearly proportional to the mass flow through the valve.#n

#n#h<Variables to be supplied by the user>#n
#i<Charctr> : Select the flow characteristic of the valve, ie how the flow
through the valve varies with the amount of regulation offered by the valve.
For #u<Linear> Flow characteristic, the flow is directly proportional to
the amount of regulation offered by the valve, e.g. for use in liquid
level control. For #u<Equal Percentage> Flow characteristic, equal changes
in the position of the valve produce equal percentage changes in the
existing flow, e.g. for use in an environment where highly varying
pressure drop conditions are expected. For #u<Quick opening> flow
characteristic, initially large increases in the flow rate are achieved
as the valve is opened, the changes in flow rate then approach zero as
the valve approaches the fully opened position, e.g. for use in on-off
applications.#n
#i<Resistn> : The resistance within the valve.  The larger the number the
greater the resistance to flow offered by the valve in all positions.#n

#n#h<Other>#n
Short name:VE_Line#n
Model type:Valve Flow Equation#n
#G:Valve Flow Equations
*/

//--------------------------------------------------------------------------

IMPLEMENT_FLWEQN(VE_Linear, ValveGroup.Name(), "VE_Line", "", TOC_SYSTEM,
                 "DP prop to Qm",
                 "DP prop to Qm model for a Valve");

VE_Linear::VE_Linear(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  R_Fxd=0.001;
  Character = VC_Linear;
  }

//--------------------------------------------------------------------------

VE_Linear::~VE_Linear()
  {
  }

//--------------------------------------------------------------------------

void VE_Linear::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDB0[]={
    {VC_Linear, "Linear"},
    {VC_EqPerc, "EqualPerc"},
    {VC_Quick,  "Quick"},
    {0}};
  DDB.Byte   ("",     "Charctr",  DC_     , "",     &Character, this  ,isParm, DDB0);
  DDB.Double ("",     "Resistn",     DC_     , "",     &R_Fxd      ,this  ,isParm);

  BuildDataDefnOveride(DDB);
  }

//--------------------------------------------------------------------------

//void VE_Linear::StartSolution(rFlwBlkBase FE)
//  {
//  //Diam = Max(Diam,1.0e-3);
//  //FE.Length=Max(Max(fabs(FE.dZ), Len_Rqd)+Len_Fit, 0.1*Diam);
//  };

//--------------------------------------------------------------------------

flag VE_Linear::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 1.0);
  if (Regulation > MinValveOpening)
    {
    FE.SetQmFree();
    switch (Character)
      {
      case VC_EqPerc : Regulation=Sqr(Regulation);         break;
      case VC_Quick  : Regulation=sqrt(Regulation);        break;
      }

    //double aQm = fabs(FE.Qm);
    //double SQm = Sign(FE.Qm);
    FE.SetDPb(0.0, 0.0);
    //FE.R = R_Fxd/Regulation;
    FE.SetDPq(-FE.QmSign()*R_Fxd*FE.QmMeas()/Regulation, -FE.QmSign()*R_Fxd/Regulation);
    }
  else
    FE.SetQmReqd(0.0);

  #if dbgValve
  if (this->DoDbgBrk())
    {
    //dbgpln("B_Valve Liq Qm:%8g Rho:%8g OpCv:%8g dPq:%8g dPqdQ:%8g  %s",aQm,Rhoi,OpCv,dPq,dPqdQ, FullTag());
    }
  #endif
  return True;
  }

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Cv-Km DP Eqn for a Valve
#X:#h<General Description>#nThis is method of modelling the pressure drop versus
flow relationship of a valve.  This calculates the pressure drop within a valve,
using actual manufacturers data.  The US form of the equation reuires the user to
specify the valve sizing coeffient, CV, and the valve recovery coeffient,km, of the
valve.  The larger the valve, the larger the value of Cv will be.  The SI form of
the equation requires the user to specify the Cv and the flow area of the valve.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Charctr> : Select the flow characteristic of the valve, ie how the flow
through the valve varies with the amount of regulation offered by the valve.
For #u<Linear> flow characteristic, the flow is directly proportional to
the amount of regulation offered by the valve, e.g. for use in liquid
level control. For #u<Equal Percentage> flow characteristic, equal changes
in the position of the valve produce equal percentage changes in the
existing flow, e.g. for use in an environment where highly varying
pressure drop conditions are expected. For #u<Quick opening> flow
characteristic, initially large increases in the flow rate are achieved
as the valve is opened, the changes in flow rate then approach zero as
the valve approaches the fully opened position, e.g. for use in on-off
applications.#n
#i<Cv_Eqn> : Select #u<US_Form> or #u<SI_Form>.  The form selected will
depend on the information available from the valve manufacturer.#n
#i<Cv> : The valve sizing coeffient.#n
#i<Km> : The valve recovery coeffient, this is required for the US from
of the equation.#n
#i<Area> : The flow area of the valve, required when using the SI form of
the equation.#n
#n#n
These are the equations used for dP :#w#n
                                                                                #n
 SI Units : dP = 0.5 #* CV1 #* pv^2            dP  : delta P (Pascal)              #n
                                             p   : density (kg/m3)               #n
                                             v   : velocity (m/s)                #n
                                             CV1 : dimensionless                 #n
                                                                                 #n
 US Units :  Q = CV2 #* Sqrt(dP/G)            dP  : delta P (psi)                 #n
                                             Q   : volumetric flow (US gal/min)  #n
                                             G   : sp gravity (dimensionless)    #n
                                             CV2 : not dimensionless             #n
#a#n
The latter translates for gases into :#w#n
                                             dP  : delta P (bar)                 #n
                Q        (     GT       )    P1  : upstream pressure (bar)       #n
        CV2 =  ---  Sqrt ( -------------)    P2  : downstream pressure (bar)     #n
               295       ( dP (P1 + P2) )    Q   : volumetric flow (Nm3/hr)      #n
                                             (15degC + 101325 Pa)                #n
                                             G   : gas sp gravity (rel to air)   #n
                                             T   : temperature (K)               #n
#a#n
#n#h<Other>#n
Short name:VE_CvKm#n
Model type:Valve Flow Equation#n
#G:Valve Flow Equations
*/

/*
  These are the equations used for dP :
                                2
  SI Units :  dP = 0.5* CV1 * pv                  dP  : delta P (Pascal)
                                                  p   : density (kg/m3)
                                                  v   : velocity (m/s)
                                                  CV1 : dimensionless

  US Units :  Q  = CV2 * Sqrt(dP/G)               dP  : delta P (psi)
                                                  Q   : volumetric flow (US gal/min)
                                                  G   : sp gravity (dimensionless)
                                                  CV2 : not dimensionless

  The latter translates for gases into :
                                                  dP  : delta P (bar)
             CV2 =  Q        (     GT       )     P1  : upstream pressure (bar)
                   ---  Sqrt ( -------------)     P2  : downstream pressure (bar)
                   295       ( dP (P1 + P2) )     Q   : volumetric flow (Nm3/hr)
                                                  (15øC + 101325 Pa)
                                                  G   : gas sp gravity (rel to air)
                                                  T   : temperature (K)
*/

//--------------------------------------------------------------------------

IMPLEMENT_FLWEQN(VE_CvKm, ValveGroup.Name(), "VE_CvKm", "", TOC_SYSTEM,
                 "Cv-Km DP Eqn",
                 "Cv-Km DP Eqn model for a Valve");

VE_CvKm::VE_CvKm(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  fUSCvForm=1;
  Cv=100.0;
  Km=0.5;
  Area=0.01;
  Character = VC_Linear;
  }

//--------------------------------------------------------------------------

VE_CvKm::~VE_CvKm()
  {
  }

//--------------------------------------------------------------------------

void VE_CvKm::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDB0[]={
    {VC_Linear, "Linear"},
    {VC_EqPerc, "EqualPerc"},
    {VC_Quick,  "Quick"},
    {0}};
  DDB.Byte   ("",     "Charctr",  DC_     , "",     &Character, this  ,isParm, DDB0);
  static DDBValueLst DDB1[]={
    {True,  "US_Form"},
    {False, "SI_Form"},
    {0}};
  DDB.Bool   ("",     "Cv_Eqn",   DC_     , "",     &fUSCvForm   ,this  ,isParm, DDB1);
  DDB.Double ("",     "Cv",    DC_     , "",     &Cv       ,this  ,isParm);
  DDB.Double (""  ,   "Km",    DC_     , "",     &Km       ,this  ,isParm);
  DDB.Visibility(SHM_All, !fUSCvForm);
  DDB.Double (""  ,   "Area",  DC_     , "",     &Area     ,this  ,isParm);
  DDB.Visibility();

  // Km used for the sizing of the valve
  BuildDataDefnOveride(DDB);
  }

//--------------------------------------------------------------------------

//void VE_CvKm::StartSolution(rFlwBlkBase FE)
//  {
//  //Diam = Max(Diam,1.0e-3);
//  //FE.Length=Max(Max(fabs(FE.dZ), Len_Rqd)+Len_Fit, 0.1*Diam);
//  };

//--------------------------------------------------------------------------

flag VE_CvKm::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 1.0);
  if (Regulation > MinValveOpening)
    {
    FE.SetQmFree();
    switch (Character)
      {
      case VC_EqPerc : Regulation=Sqr(Regulation);         break;
      case VC_Quick  : Regulation=sqrt(Regulation);        break;
      }

    double Rho=FE.MeanRho(pProps);
    Rho = Min_FE_Rho(Rho);
    Rho=Range(0.01, Rho, 10000.0);
    double RD  = Rho/1000.0;

    if (fUSCvForm)
      {
      // does the necessary SI conversions & common calc
      double OpCv = Regulation*Cv;
      double Ka   = RD*Sqr(GPM_per_m3s/(Rho*OpCv))/PSI_per_kPa;
      FE.SetDPb(0.0, 0.0);
      FE.SetDPq(Ka*Sqr(FE.QmMeas()), 2.0*Ka*FE.QmMeas());
      }
    else
      {
      double OpCv = Cv/Regulation;
      double RhoA = Max(1.0e-6, Rho*Area);
      double K    = 0.001*0.5*OpCv*Rho/Sqr(RhoA);
      FE.SetDPb(0.0, 0.0);
      FE.SetDPq(K * Sqr(FE.QmMeas()), 2.0 * K * FE.QmMeas());
      }
    }
  else
    FE.SetQmReqd(0.0);

  #if dbgValve
  if (this->DoDbgBrk())
    {
    //dbgpln("B_Valve Liq Qm:%8g Rho:%8g OpCv:%8g dPq:%8g dPqdQ:%8g  %s",FE.QmMeas(),Rhoi,OpCv,dPq,dPqdQ, FullTag());
    }
  #endif
  return True;
  }

//--------------------------------------------------------------------------

#ifdef lkjlkjlkjlkj



      else if (Valid(Cg) && Valid(C1))
        {
        double OpCg = Regulation*Cg+(Valid(BCg) ? BScl*BCg : 0.0);

        Pi       = kPa_2_PSI(Pi);
        Rhoi     = kg_m3_2_lb_ft3(Rhoi);
        DpChoked = kPa_2_PSI(DpChoked);
        QmChoked = kg_s_2_lb_hr(QmChoked);
        //dPqC     = kPa_2_PSI(dPqC);
        AQm      = kg_s_2_lb_hr(AQm);

        Ka       = 1.06*sqrt(Rhoi*Pi)*OpCg;
        Kb       = C1/59.64;

        QRatioMx = (1.0-1.0e-15);
        QRatioMx = (1.0-1.0e-6);
        QmChoked = Ka*QRatioMx;
        DpChoked = Pi*Sqr(Kb*asin(QRatioMx));
        Set_Choked_Qm(lb_hr_2_kg_s(QmChoked), PSI_2_kPa(DpChoked));

        QRatio   =Min(AQm/Ka, QRatioMx);

        dPq      = -SQm*(Pi*Sqr(Kb*asin(QRatio)));
        dPqdQ    = -SQm*Pi*Sqr(Kb)*2.0*asin(QRatio)/(sqrt(1.0-Sqr(QRatio))*Ka);

        dPq      = PSI_2_kPa(dPq);
        dPqdQ    = PSI_lbhr_2_kPa_kgs(dPqdQ);

        Pi       = PSI_2_kPa(Pi);
        Rhoi     = lb_ft3_2_kg_m3(Rhoi);
        DpChoked = PSI_2_kPa(DpChoked);
        QmChoked = lb_hr_2_kg_s(QmChoked);
        //dPqC     = PSI_2_kPa(dPqC);
        AQm      = lb_hr_2_kg_s(AQm);

        #if dbgValve
        if (this->DoDbgBrk())
          {
          dbgpln("B_Valve %s Gas Qm :%8g Dp :%8g dPq :%8g dPqdQ:%8g OpCg:%8g %s",
                  (GFI->Choked>1.0e-6)?"Choked":"      ",AQm,
                  fabs(IOP_Self(0)-IOP_Self(1)),dPq,dPqdQ,OpCg,FullTag());
          dbgpln("                 QmC:%8g DpC:%8g Pi:%8g Rho:%8g %s",
                  QmChoked, DpChoked, Pi, Rhoi,FullTag());
          }
        #endif
        }
      /********************* to be done
      else if (Valid(Cs))
        {
        }
      **********************/
      else
        {
        dPq=-SQm*Regulation*R_Fxd*AQm;
        dPqdQ=-SQm*Regulation*R_Fxd;
        }
      }
      break;
    case FlwRgm_Gas:
      {
      dPq   = 0.0;
      dPqdQ = 0.0;   // ????????????????????????????????????????????????????????
      }
      break;
    default:
      dPq   = 0.0;
      dPqdQ = 0.0;
      break;
    }
  }

#endif
//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Steam (below 1000 Psi) DP Eqn for a Valve
#X:#h<General Description>#nThis is a flow equation for a valve.
This calculates the pressure drop within a valve, by using the an adaption
of the Universal Gas Sizing Equation for valves for Steam below 1000 Psig
(6803 kPag). For steam above 1000 Psig (6803 kPag) the Cg-C1 equation
must be used.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Charctr> : Select the flow characteristic of the valve, ie how the flow
through the valve varies with the amount of regulation offered by the valve.
For #u<Linear> flow characteristic, the flow is directly proportional to
the amount of regulation offered by the valve, e.g. for use in liquid
level control. For #u<Equal Percentage> flow characteristic, equal changes
in the position of the valve produce equal percentage changes in the
existing flow, e.g. for use in an environment where highly varying
pressure drop conditions are expected. For #u<Quick opening> flow
characteristic, initially large increases in the flow rate are achieved
as the valve is opened, the changes in flow rate then approach zero as
the valve approaches the fully opened position, e.g. for use in on-off
applications.#n
#i<C1> : The the ratio between the gas sizing coefficient and liquid sizing
coefficient of the valve.#n
#i<Cs> : The steam sizing coeffient for the valve. This is equivalent to
the gas sizing coefficient divided by 20. This relationship only holds because
the pressure of the steam has been restricted to below 1000 Psi (6803 kPag).#n
#n
#n#h<Other>#n
Short name:VE_Steam#n
Model type:Valve Flow Equation#n
#G:Valve Flow Equations
*/
//--------------------------------------------------------------------------

IMPLEMENT_FLWEQN(VE_Steam, ValveGroup.Name(), "VE_Steam", "", TOC_SYSTEM,
                 "Steam below 1000 Psig",
                 "Steam (below 1000 Psi) DP Eqn model for a Valve");

VE_Steam::VE_Steam(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  Cs=100.0; //check
  C1=20;    // check
  Character = VC_Linear;
  }

//--------------------------------------------------------------------------

VE_Steam::~VE_Steam()
  {
  }

//--------------------------------------------------------------------------

void VE_Steam::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDB1[]={
    {VC_Linear, "Linear"},
    {VC_EqPerc, "EqualPerc"},
    {VC_Quick,  "Quick"},
    {0}};
  DDB.Byte   ("",     "Charctr",  DC_     , "",     &Character, this  ,isParm, DDB1);
  DDB.Double ("",     "C1",    DC_     , "",     &C1       ,this  ,isParm);
  DDB.Double (""  ,   "Cs",    DC_     , "",     &Cs       ,this  ,isParm);
  // Km used for the sizing of the valve
  BuildDataDefnOveride(DDB);
  }

//--------------------------------------------------------------------------

//void VE_Steam::StartSolution(rFlwBlkBase FE)
//  {
//  //Diam = Max(Diam,1.0e-3);
//  //FE.Length=Max(Max(fabs(FE.dZ), Len_Rqd)+Len_Fit, 0.1*Diam);
//  };

//--------------------------------------------------------------------------

flag VE_Steam::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 1.0);
  if ( (Regulation > MinValveOpening) && (Valid(Cs) && Valid(C1)) )
    {
//  if ( inlet pressure < 1000 psig & its steam)
    FE.SetQmFree();
    switch (Character)
      {
      case VC_EqPerc : Regulation=Sqr(Regulation);         break;
      case VC_Quick  : Regulation=sqrt(Regulation);        break;
      }

  //  double Rhoi = Min_FE_Rho(C.QRho(som_ALL));
  //  double RD  = Rhoi/1000.0;
    double aQm = FE.QmMeas();
    //double SQm = Sign(FE.Qm);

    double OpCs = Regulation*Cs; // ??? +(Valid(BCg) ? BScl*BCg : 0.0);
//    double Pi = Min(FE.PIn(), FE.POut());
    double Pi = FE.MeanFBPress();
    double Tsh = FE.MeanTemp(pProps);
//    double Cs = Cg/20;

    aQm      = kg_s_2_lb_hr(aQm);
    Pi       = kPa_2_PSI(Pi);  // absolute pressure
    Tsh      = K_2_F(Tsh);
//    Rhoi     = kg_m3_2_lb_ft3(Rhoi);
//    DpChoked = kPa_2_PSI(DpChoked);
//    QmChoked = kg_s_2_lb_hr(QmChoked);
    //dPqC     = kPa_2_PSI(dPqC);

    double Ka       = (1 + 0.00065*Tsh)/(OpCs*Pi);
    double Kb       = (Pi*Sqr(C1))/2985.5296;

//    QRatioMx = (1.0-1.0e-15);
//    QRatioMx = (1.0-1.0e-6);
//    QmChoked = Ka*QRatioMx;
//    DpChoked = Pi*Sqr(Kb*asin(QRatioMx));
//   Set_Choked_Qm(lb_hr_2_kg_s(QmChoked), PSI_2_kPa(DpChoked));

//    QRatio   =Min(AQm/Ka, QRatioMx);

    double dPq      = Kb*Sqr(asin(aQm*Ka));
    double dPqdQ    = Kb*2.0*((asin(aQm*Ka))/(sqrt(1.0-Sqr(aQm*Ka))));

//    FE.R = PSI_2_kPa(dPq)/fabs(FE.Qm);
    FE.SetDPq(PSI_2_kPa(dPq), PSI_lbhr_2_kPa_kgs(dPqdQ));

// only convert if parameer  will be used at a later stage
//    Pi       = PSI_2_kPa(Pi);
//    Tsh      = F_2_K(Tsh);
//    Rhoi     = lb_ft3_2_kg_m3(Rhoi);
//    DpChoked = PSI_2_kPa(DpChoked);
//    QmChoked = lb_hr_2_kg_s(QmChoked);
//dPqC     = PSI_2_kPa(dPqC);
//    AQm      = lb_hr_2_kg_s(AQm);

    #if dbgValve
    if (this->DoDbgBrk())
      {
      //dbgpln("B_Valve %s Gas Qm :%8g Dp :%8g dPq :%8g dPqdQ:%8g OpCg:%8g %s",
      //        (GFI->Choked>1.0e-6)?"Choked":"      ",aQm,
      //        fabs(IOP_Self(0)-IOP_Self(1)),dPq,dPqdQ,OpCg,FullTag());
      //dbgpln("                 QmC:%8g DpC:%8g Pi:%8g Rho:%8g %s",
      //        QmChoked, DpChoked, Pi, Rhoi,FullTag());
      }
    #endif

    }
  else
    FE.SetQmReqd(0.0);

  #if dbgValve
  if (this->DoDbgBrk())
    {
    //dbgpln("B_Valve Liq Qm:%8g Rho:%8g OpCv:%8g dPq:%8g dPqdQ:%8g  %s",aQm,Rhoi,OpCv,dPq,dPqdQ, FullTag());
    }
  #endif
  return True;
  };

//--------------------------------------------------------------------------
//#endif

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:CvOpening DP Curve
#X:#h<General Description>#nThis calculates the pressure drop within a valve using
actual manufacturers data.  The US form of the equation reuires the user to specify
the valve recovery coeffient,km, of the valve, while the SI form of the equation
requires the user to specify the flow area of the valve.  The user must specify the
valve Cv as a function of the valve opening, i.e. the valve has a variable Cv value.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Charctr> : Select the flow characteristic of the valve, ie how the flow
through the valve varies with the amount of regulation offered by the valve.
For #u<Linear> flow characteristic, the flow is directly proportional to
the amount of regulation offered by the valve, e.g. for use in liquid
level control. For #u<Equal Percentage> flow characteristic, equal changes
in the position of the valve produce equal percentage changes in the
existing flow, e.g. for use in an environment where highly varying
pressure drop conditions are expected. For #u<Quick opening> flow
characteristic, initially large increases in the flow rate are achieved
as the valve is opened, the changes in flow rate then approach zero as
the valve approaches the fully opened position, e.g. for use in on-off
applications.#n
#i<Cv_Eqn> : Select #u<US_Form> or #u<SI_Form>.  The form selected will
depend on the information available from the valve manufacturer.#n
#i<Cv> : The valve sizing coeffient.#n
#i<Km> : The valve recovery coeffient, this is required for the US from
of the equation.#n
#i<Area> : The flow area of the valve, required when using the SI form of
the equation.#n
#i<CvCurve> : The Cv versus valve opening curve.  The valve opening should be normalized
between 0 and 1.#n

#n#h<Other>#n
Short name:VE_CvKm#n
Model type:Valve Flow Equation#n
#G:Valve Flow Equations
*/

/*
  These are the equations used for dP :
                                2
  SI Units :  dP = 0.5* CV1 * pv                  dP  : delta P (Pascal)
                                                  p   : density (kg/m3)
                                                  v   : velocity (m/s)
                                                  CV1 : dimensionless

  US Units :  Q  = CV2 * Sqrt(dP/G)               dP  : delta P (psi)
                                                  Q   : volumetric flow (US gal/min)
                                                  G   : sp gravity (dimensionless)
                                                  CV2 : not dimensionless

  The latter translates for gases into :
                                                  dP  : delta P (bar)
             CV2 =  Q        (     GT       )     P1  : upstream pressure (bar)
                   ---  Sqrt ( -------------)     P2  : downstream pressure (bar)
                   295       ( dP (P1 + P2) )     Q   : volumetric flow (Nm3/hr)
                                                  (15øC + 101325 Pa)
                                                  G   : gas sp gravity (rel to air)
                                                  T   : temperature (K)
*/

//--------------------------------------------------------------------------

IMPLEMENT_FLWEQN(VE_VarCv, ValveGroup.Name(), "VE_VarCv", "", TOC_SYSTEM,
                 "CvOpening DP Curve",
                 "CvOpening DP curve for a Valve");

VE_VarCv::VE_VarCv(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach),
  CvCurve("DataOnly", "CvCurve", this, TOA_Embedded, "Opening", "Cv")
  {
  Cv=100.0;
  Km=0.5;
  Area=0.01;
  }

//--------------------------------------------------------------------------

VE_VarCv::~VE_VarCv()
  {
  }

//--------------------------------------------------------------------------

void VE_VarCv::BuildDataDefn(DataDefnBlk & DDB)
  {
  static DDBValueLst DDB1[]={
    {True,  "US_Form"},
    {False, "SI_Form"},
    {0}};
  DDB.Bool   ("",     "Cv_Eqn",   DC_     , "",     &fUSCvForm   ,this  ,isParm, DDB1);

  DDB.Double (""  ,   "Km",    DC_     , "",     &Km       ,this  ,isParm);
  DDB.Visibility(SHM_All, !fUSCvForm);
  DDB.Double (""  ,   "Area",  DC_     , "",     &Area     ,this  ,isParm);
  DDB.Visibility();
  DDB.Double (""  ,   "Cv",    DC_     , "",     &Cv       ,this  ,0);
  BuildDataDefnOveride(DDB);

  DDB.Object(&CvCurve,  this, NULL, NULL, DDB_RqdPage);
  }

//--------------------------------------------------------------------------

//void VE_VarCv::StartSolution(rFlwBlkBase FE)
//  {
//  };
//
//--------------------------------------------------------------------------

flag VE_VarCv::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 1.0);
  if (Regulation > MinValveOpening)
    {
    FE.SetQmMeasRange(Max(0.001, FE.MeanRho(pProps)), 1.0e-6);
    FE.SetQmFree();

    double Rho=FE.MeanRho(pProps);
    Rho = Min_FE_Rho(Rho);
    double RD  = Rho/1000.0;

    if (fUSCvForm)
      {
      // does the necessary SI conversions & common calc
      Cv = CvCurve.Yx(Regulation);
      double OpCv = Max(1.0e-6, Regulation*Cv);
      double Ka   = RD*Sqr(GPM_per_m3s/(Rho*OpCv))/PSI_per_kPa;
      FE.SetDPb(0.0, 0.0);
      FE.SetDPq(Ka*Sqr(FE.QmMeas()), 2.0*Ka*FE.QmMeas());
      }
    else
      {
      Cv = CvCurve.Yx(Regulation);
      double OpCv = Cv/Regulation;
      double RhoA = Max(1.0e-6, Rho*Area);
      double K    = 0.001*0.5*OpCv*Rho/Sqr(RhoA);
      FE.SetDPb(0.0, 0.0);
      FE.SetDPq(K * Sqr(FE.QmMeas()), 2.0 * K * FE.QmMeas());
      }
    }
  else
    {
    Cv=0.0;
    FE.SetQmReqd(0.0);
    }

  #if dbgValve
  //dbgpln("B_Valve Liq Qm:%8g Rho:%8g OpCv:%8g dPq:%8g dPqdQ:%8g  %s",FE.QmMeas(),Rhoi,OpCv,dPq,dPqdQ, FullTag());
  #endif
  return True;
  }


//==========================================================================
//
//
//
//==========================================================================


#define DllImportExport
DEFINE_TAGOBJ(VC_Crane);
class VC_Crane : public CFlwEqn
  {
  enum ChkValveTypes
    {
    CVT_Swing,
    CVT_LiftVert,
    CVT_LiftAngle,
    CVT_TiltDisc,
    CVT_Stop,
    };

  static CraneVlvInfo CVI[];

  static DDBValueLst DDBCVT[];

  public:
    byte            iType;
    double          dK1;
    double          dK2;
    double          dVlvArea;
    double          dPipeArea;
    double          dBeta;
    double          dAlpha;
    DualDbl         KConst;
    double          dDensMeas;
    double          dViscMeas;
    double          dVelMeas;
    double          dFricFac;
    CFrictionFactor m_FF;
    double          m_dDP;

    VC_Crane(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~VC_Crane();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);
//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);

  };

CraneVlvInfo VC_Crane::CVI[]=
    {
      {CVT_Swing,    "Swing",     true, false,false,   50.0,   1.0, Deg2Rad(90)},
      {CVT_LiftVert, "LiftVert",  true, false,false,  600.0,   1.0, Deg2Rad(90)},
      {CVT_LiftAngle,"LiftAngle", true, false,false,   55.0,   1.0, Deg2Rad(90)},
      {CVT_TiltDisc, "TiltDisc",  false,false,true,   120.0,   1.0, Deg2Rad(5)},
      {CVT_Stop,     "Stop",      true, false,false,  200.0,   1.0, Deg2Rad(0)},
    };

DDBValueLst VC_Crane::DDBCVT[]={
      {CVI[0].iType, CVI[0].pName},
      {CVI[1].iType, CVI[1].pName},
      {CVI[2].iType, CVI[2].pName},
      {CVI[3].iType, CVI[3].pName},
      {CVI[4].iType, CVI[4].pName},
      {0}};

IMPLEMENT_FLWEQN(VC_Crane, ChkValveGroup.Name(), "VC_Crane", "", TOC_SYSTEM,
                 "Crane K Factor Valve",
                 "Crane K Factor Valve");

VC_Crane::VC_Crane(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  iType=CVT_Swing;
  dBeta=1.0;
  dAlpha=Deg2Rad(10);
  dVlvArea=0.1;
  dPipeArea=0.1;
  dK1=0;
  dK2=0;
  dDensMeas=0;
  dViscMeas=0;
  dVelMeas=0;
  dFricFac=0;
  m_dDP=0;
  KConst.SetVal(CVI[iType].dKConstDefault, this);
  }

//--------------------------------------------------------------------------

VC_Crane::~VC_Crane()
  {
  }

//--------------------------------------------------------------------------

//XID xidKConst = MdlBsXID(17001);
//XID xidKFact  = MdlBsXID(17000);
//XID xidPDiam  = MdlBsXID(17002);
//XID xidVDiam  = MdlBsXID(17003);

void VC_Crane::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Byte   ("",           "Type",       DC_,      "",       xidVType,     NULL, isParm|SetOnChange, DDBCVT);
  KConst.BuildDataDefn(DDB,  "KConst", "", DC_,      "",       xidKConst,  NULL, "Initial", "Required");
  if (CVI[iType].bBetaReqd)
    DDB.Double ("",         "Beta",       DC_,      "",       &dBeta,     NULL, isParm);
  if (CVI[iType].bAlphaReqd)
    DDB.Double ("",         "Alpha",      DC_Ang,   "deg",    &dAlpha,    NULL, isParm);
  DDB.Double ("PipeDiam",   "D-Pipe",     DC_L,     "mm",     xidPDiam,   NULL, 0);
  DDB.Double ("ValveDiam",  "D-Vlv",      DC_L,     "mm",     xidVDiam,   NULL, 0);
  DDB.Double ("",           "K1",         DC_,      "",       &dK1,       NULL, 0);
  DDB.Double ("",           "K2",         DC_,      "",       &dK2,       NULL, 0);
  DDB.Double ("Density",     "",          DC_Rho,   "kg/m^3", &dDensMeas, NULL, isResult);
  DDB.Double ("Viscosity",   "",          DC_Visc,  "cP",     &dViscMeas, NULL, isResult);
  DDB.Double ("Velocity",   "Vel",        DC_Ldt,   "m/s",    &dVelMeas,  NULL, isResult);
  DDB.Double ("FricFac",     "fT",        DC_,      "",       &dFricFac,  NULL, isResult);
  m_PhD.KFact.BuildDataDefn(DDB, "ResistCoeff", "K", DC_, "",   xidKFact,   NULL, "Calculated", "Required");
  DDB.Double ("PressDrop",   "DP",        DC_DP,    "kPa",    &m_dDP,     NULL, isResult);
  BuildDataDefnOveride(DDB);
  }

// --------------------------------------------------------------------------

flag VC_Crane::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidVType:
      if (DCB.rB)
        {
        iType=*DCB.rB;
        KConst.SetVal(CVI[iType].dKConstDefault, this);
        dBeta=CVI[iType].dBetaDefault;
        dAlpha=CVI[iType].dThetaAlphaDefault;
        }
      DCB.B=iType;
      return 1;
    case xidPDiam: DCB.D=Area2Diam(dPipeArea); return 1 ;
    case xidVDiam: DCB.D=Area2Diam(dVlvArea); return 1 ;
    };
  if (KConst.DataXchg(DCB, xidKConst, this))
    return 1;
  if (m_PhD.KFact.DataXchg(DCB, xidKFact, this))
    return 1;
  return CFlwEqn::DataXchg(DCB);
  }

// --------------------------------------------------------------------------

// See ButterflyKConst.XLS
inline void SetTiltDiscKConst(DualDbl & KConst, double Area, double Alpha, TaggedObject * This)
  {
  KConst.SetVal(45.433*exp(-1.4734*Area2Diam(Area))*Alpha/Deg2Rad(5), This);
  }

flag VC_Crane::ValidateData(ValidateDataBlk & VDB)
  {
  dBeta=ValidateRange(VDB, "Beta", 1e-6, dBeta, 1.0);
  dAlpha=ValidateRange(VDB, "Alpha", 0.0, dAlpha, 25.0/PI);

  if (iType==CVT_TiltDisc)
    SetTiltDiscKConst(KConst, dPipeArea, dAlpha, this);

  return true;
  }

//--------------------------------------------------------------------------


flag VC_Crane::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 1.0);
  if (Regulation > MinValveOpening)
    {
    //FE.SetQmMeasRange(1.0e-6);
    FE.SetQmFree();

    dDensMeas=FE.MeanRho(pProps);
    dDensMeas=Min_FE_Rho(dDensMeas);
    dViscMeas=FE.MeanViscosity(pProps);
    //double RD  = Rho/1000.0;

    if (pPhD0 && pPhD0->IsPipe())
      dPipeArea=pPhD0->Area();
    else if (pPhD1 && pPhD1->IsPipe())
      dPipeArea=pPhD1->Area();
    else
      {};
    dVlvArea=dPipeArea*dBeta*dBeta;
    double dUseValveArea=dVlvArea*Regulation;
    
    FE.SetVelMeasRange(dDensMeas, dUseValveArea, 0.001);

    double D=Area2Diam(dPipeArea);
    dFricFac=m_FF.Turbulent(D, 5.0e-5, dDensMeas, dViscMeas);
    switch (iType)
      {
      case CVT_Swing:
        dK1=KConst()*dFricFac;
        dK2=dK1;
        break;
      case CVT_LiftVert:
      case CVT_LiftAngle:
      case CVT_Stop:
        dK1=KConst()*dFricFac;
        if (dBeta<0.9999)
          {
          double B2=dBeta*dBeta;
          dK2=(dK1 + dBeta*(0.5*(1-B2) + (Sqr(1-B2)))) / (B2*B2);
          }
        else
          dK2=dK1;
        break;
      case CVT_TiltDisc:
        SetTiltDiscKConst(KConst, dPipeArea, dAlpha, this);
        dK1=KConst()*dFricFac;
        dK2=dK1;
        break;
      default:
        dK1=1000;
        dK2=dK1;
      }
    m_PhD.KFact.SetVal(dK2, this);

    double Vel0=FE.VelMeas();
    double Vel1=FE.VelMeas(1.001);
    double DP0=PressDropKFact(Vel0, dDensMeas, m_PhD.KFact());
    double DP1=PressDropKFact(Vel1, dDensMeas, m_PhD.KFact());
    dVelMeas=Vel0;

    FE.SetDPb(0.0, 0.0);
    FE.SetDPq(-FE.QmSign()*DP0, -FE.QmSign()*(DP1-DP0)/NZ(FE.DQmMeas()));
    m_dDP=DP0;
    }
  else
    {
    //Cv=0.0;
    FE.SetQmReqd(0.0);
    }

//  #if dbgValve
//  dbgpln("B_Valve Liq Qm:%8g Rho:%8g OpCv:%8g dPq:%8g dPqdQ:%8g  %s",FE.QmMeas(),Rhoi,OpCv,dPq,dPqdQ, FullTag());
//  #endif
  return True;
  }

//============================================================================
//
//
//
// ===========================================================================

DEFINE_TAGOBJ(VC_DPQM);
class VC_DPQM : public CFlwEqn
  {
//  enum ValveCharacteristics {VC_DPQM, VC_EqPerc, VC_Quick};
  public:
    double         R_Fxd;
  //  byte           Character;

    VC_DPQM(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~VC_DPQM();
    virtual void   BuildDataDefn(DataDefnBlk & DDB);

//    virtual void   StartSolution(rFlwBlkBase FE);
    virtual flag   EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1);
  };

// ===========================================================================

IMPLEMENT_FLWEQN(VC_DPQM, ChkValveGroup.Name(), "VC_Line", "", TOC_SYSTEM,
                 "DP prop to Qm",
                 "DP prop to Qm model for a Valve");

VC_DPQM::VC_DPQM(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  R_Fxd=0.001;
//  Character = VC_DPQM;
  }

//--------------------------------------------------------------------------

VC_DPQM::~VC_DPQM()
  {
  }

//--------------------------------------------------------------------------

void VC_DPQM::BuildDataDefn(DataDefnBlk & DDB)
  {
  //static DDBValueLst DDB0[]={
  //  {VC_DPQM, "Linear"},
  //  {VC_EqPerc, "EqualPerc"},
  //  {VC_Quick,  "Quick"},
  //  {0}};
  //DDB.Byte   ("",     "Charctr",  DC_     , "",     &Character, this  ,isParm, DDB0);
  DDB.Double ("",     "Resistn",     DC_     , "",     &R_Fxd      ,this  ,isParm);
  BuildDataDefnOveride(DDB);
  }

//--------------------------------------------------------------------------

//void VC_DPQM::StartSolution(rFlwBlkBase FE)
//  {
//  //Diam = Max(Diam,1.0e-3);
//  //FE.Length=Max(Max(fabs(FE.dZ), Len_Rqd)+Len_Fit, 0.1*Diam);
//  };

//--------------------------------------------------------------------------

flag VC_DPQM::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  Regulation=Range(0.0, Regulation, 1.0);
  if (Regulation > MinValveOpening)
    {
    FE.SetQmFree();
    //switch (Character)
    //  {
    //  case VC_EqPerc : Regulation=Sqr(Regulation);         break;
    //  case VC_Quick  : Regulation=sqrt(Regulation);        break;
    //  }

    //double aQm = fabs(FE.Qm);
    //double SQm = Sign(FE.Qm);
    FE.SetDPb(0.0, 0.0);
    //FE.R = R_Fxd/Regulation;
    FE.SetDPq(-FE.QmSign()*R_Fxd*FE.QmMeas()/Regulation, -FE.QmSign()*R_Fxd/Regulation);
    }
  else
    FE.SetQmReqd(0.0);

  #if dbgValve
  //dbgpln("B_Valve Liq Qm:%8g Rho:%8g OpCv:%8g dPq:%8g dPqdQ:%8g  %s",aQm,Rhoi,OpCv,dPq,dPqdQ, FullTag());
  #endif
  return True;
  }

// ===========================================================================
//
//
//
// ===========================================================================
