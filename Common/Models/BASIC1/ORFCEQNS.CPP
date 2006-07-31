//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdarg.h>

#define __ORFCEQNS_CPP

#include "orfceqns.h"
#include "m_bstres.h"

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:DP Prop to Qm for an Orifice
#X:#h<General Description>#nThis is a flow equation for an orifice.
It is a linear flow equation, as it assumes that the pressure drop within an
orifice is proportional to the mass flow within the unit.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Resistn> : The resistance within the orifice.#n
#n
#n#h<Other>#n
Short name:OE_Line#n
Model type:Orifice Flow Equation#n
#G:Orifice Flow Equations
*/
//IMPLEMENT_FLWEQN(OE_Linear, OrificeGroup.Name(), "OE_Line", "", TOC_SYSTEM,
//                 "DP prop to Qm",
//                 "DP prop to Qm model for an Orifice");
//
//OE_Linear::OE_Linear(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
//  CFlwEqn(pClass_, pTag, pAttach, eAttach)
//  {
//  m_RFxd=0.0;
//  }
//
////--------------------------------------------------------------------------
//
//OE_Linear::~OE_Linear()
//  {
//  }
//
////--------------------------------------------------------------------------
//
//void OE_Linear::BuildDataDefn(DataDefnBlk & DDB)
//  {
//  DDB.Double ("",     "Resistn",     DC_     , "",     &m_RFxd      ,this  ,isParm);
//  }
//
////--------------------------------------------------------------------------
//
//flag OE_Linear::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
//  {
//  FE.SetDPb(0.0, 0.0);
//  FE.SetDPq(-FE.QmSign()*m_RFxd*FE.QmMeas()/Regulation, -FE.QmSign()*m_RFxd/Regulation);
//
//  return True;
//  }

//==========================================================================
//
//
//
//==========================================================================

IMPLEMENT_FLWEQN(OE_QvOp, OrificeGroup.Name(), "OE_QvOp", "", TOC_SYSTEM,
                 "VolFlw Op Pt",
                 "Volume Flow Operating Pt");

OE_QvOp::OE_QvOp(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_dQvOp=1;
  m_dDPOp=1;
  m_dPwrLaw=2;
  m_dDP=0;
  }

//--------------------------------------------------------------------------

OE_QvOp::~OE_QvOp()
  {
  }

//--------------------------------------------------------------------------

void OE_QvOp::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("",           "DPOp",  DC_DP,   "kPa",    &m_dDPOp,             this  ,isParm);
  DDB.Double ("",           "QvOp",  DC_Qv,   "L/s",    &m_dQvOp,             this  ,isParm);
  DDB.Double ("PowerLaw",   "Pwr",   DC_,     "",       &m_dPwrLaw,           this  ,isParm);
  DDB.Double ("Density",    "",      DC_Rho,  "kg/m^3", &m_dDensMeas,         NULL, isResult);
  DDB.Double ("Velocity",   "Vel",   DC_Ldt,  "m/s",    &m_dVelMeas,          NULL, isResult);
  DDB.Double ("PressDrop",  "DP",    DC_DP,   "kPa",    &m_dDP,               this, isResult);
  }

//--------------------------------------------------------------------------

//void OE_QvOp::StartSolution(rFlwBlkBase FE)
//  {
//  };

//--------------------------------------------------------------------------

flag OE_QvOp::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  m_dPwrLaw=Range(0.5,m_dPwrLaw,3.0);
  m_dQvOp=Max(1.0e-6, m_dQvOp);
  double Qm1 = FE.QmMeas();
  double Qm2 = Max(FE.QmMeas()*1.001, 1.0e-6);
  double Qv1 = Qm1/Min_FE_Rho(FE.MeanRho(pProps));
  double Qv2 = Qm2/Min_FE_Rho(FE.MeanRho(pProps));
  double QvRatio1=Qv1/m_dQvOp;
  double QvRatio2=Qv2/m_dQvOp;

  m_dDP = m_dDPOp*Pow(QvRatio1, m_dPwrLaw);
  double DPDQ = (m_dDPOp*Pow(QvRatio2, m_dPwrLaw)-m_dDP)/(Qm2-Qm1);

  m_dDensMeas=Max(0.001, FE.MeanRho(pProps));
  m_dVelMeas=FE.QmMeas()/(m_dDensMeas*FE.Area());

  FE.SetDPb(0.0, 0.0);
  FE.SetDPq(-FE.QmSign()*m_dDP, -FE.QmSign()*DPDQ);
  FE.SetVelocity(m_dVelMeas);

  return True;
  }

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Diameter Ratio Eqn for an Orifice
#X:#h<General Description>#nThis is a flow equation for an orifice.
This calculates the pressure drop within an orifice, according to the ratio of
the orifice diameter to the pipe inner diameter.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Diam_Orif> : The orifice diameter.#n
#i<m_dDiamPipe> : The inner pipe diameter.#n
#n#l<#b<Note>> : This ratio is ranged. The Diam_Orif is ranged between the values
of 0.2 #* m_dDiamPipe and 0.8 #* m_dDiamPipe.#n
#n
#n#h<Other>#n
Short name:OE_DRat#n
Model type:Orifice Flow Equation#n
#G:Orifice Flow Equations
*/
IMPLEMENT_FLWEQN(OE_DiamRatio, OrificeGroup.Name(), "OE_DRat", "", TOC_SYSTEM,
                 "Orifice D/D",
                 "Diameter Ratio Orifice");

OE_DiamRatio::OE_DiamRatio(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_dDiamOrifice=0.05;//50mm
  m_dDiamPipe=0.1;//100mm
  m_dDP=0.0;
  }

//--------------------------------------------------------------------------

OE_DiamRatio::~OE_DiamRatio()
  {
  }

//--------------------------------------------------------------------------

void OE_DiamRatio::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Orifice Diameter", "D-Orif",  DC_L,  "mm",  &m_dDiamOrifice, this, isParm);
  DDB.Double ("Pipe Diameter",    "D-Pipe",  DC_L,  "mm",  &m_dDiamPipe,    this, isParm);
  DDB.Double ("Density",  "",      DC_Rho,  "kg/m^3", &m_dDensMeas,        NULL, isResult);
  DDB.Double ("Velocity", "Vel",   DC_Ldt,  "m/s",    &m_dVelMeas,         NULL, isResult);
  DDB.Double ("PressDrop",        "DP",      DC_DP, "kPa", &m_dDP,         NULL, isResult);
  }

//--------------------------------------------------------------------------

//void OE_DiamRatio::StartSolution(rFlwBlkBase FE)
//  {
//
//  };

//--------------------------------------------------------------------------

flag OE_DiamRatio::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  double dPipeArea=Diam2Area(m_dDiamPipe);
  if (pPhD0 && pPhD0->IsPipe())
    dPipeArea=pPhD0->Area();
  else if (pPhD1 && pPhD1->IsPipe())
    dPipeArea=pPhD1->Area();
  else
    {};
  m_dDiamPipe=Area2Diam(dPipeArea);

  //double aQm = fabs(FE.Qm);
  //int    sQm = Sign(FE.Qm);
  double Rho = FE.MeanRho(pProps);
  //double Rho=
  m_dDiamOrifice = Range(0.2*m_dDiamPipe, m_dDiamOrifice, 0.8*m_dDiamPipe);
  double DR = m_dDiamOrifice/GTZ(m_dDiamPipe);//diameter ratio
  double DRpowFour = Pow(DR,4.0);//diameter ratio raised to power of 4
  double DRexpr = 1.0-DRpowFour;//1-(diam ratio to power of 4)
  double E = Pow(DRexpr,-0.5);//Velocity_of_Approach factor
  double DC = 0.5959+0.0312*Pow(DR,2.1)-0.184*Pow(DR,8.0)+0.039
             *DRpowFour*Pow(DRexpr,-1.0)-0.01584*Pow(DR,3.0);//Discharge coefficient
  m_dDP=Sqr((4000.0*FE.QmMeas())/(DC*E*PI*m_dDiamOrifice*Rho))/(2.0*Rho);
  double DPDQ=(FE.QmMeas()/Rho)*Sqr(4000.0/(DC*E*PI*m_dDiamOrifice*Rho));
  
  m_dDensMeas=Max(0.001, FE.MeanRho(pProps));
  m_dVelMeas=FE.QmMeas()/(m_dDensMeas*FE.Area());

  FE.SetDPb(0.0, 0.0);
  FE.SetDPq(-FE.QmSign()*m_dDP, -FE.QmSign()*DPDQ);
  FE.SetVelocity(m_dVelMeas);

  return True;
  }

//==========================================================================
//
//
//
//==========================================================================

XID xidClean =  FlwXID(11000);
XID xidDPOp  =  FlwXID(11001);
IMPLEMENT_FLWEQN(OE_InlineFilter, OrificeGroup.Name(), "OE_InlineFilter", "", TOC_SYSTEM,
                 "InlineFilter",
                 "Volume InlineFilter");

OE_InlineFilter::OE_InlineFilter(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  m_dQvOp         = 1;
  m_dDPOp         = 1;
  m_dPwrLaw       = 2;
  m_dDP           = 0;

  m_iGrowthMode   = 0;
  m_dGrowthRate   = 0.001;
  m_dGrowthVRate  = 0.001;
  m_dGrowthMRate  = 0.001;
  m_dCurrentDPOp  = 0;
  m_bClean        = 1;

  }

//--------------------------------------------------------------------------

OE_InlineFilter::~OE_InlineFilter()
  {
  }

//--------------------------------------------------------------------------

const byte GM_NoGrowth           = 0;
const byte GM_PerTime            = 1;
const byte GM_PerMass            = 2;
const byte GM_PerVolume          = 3;

static DDBValueLst DDBGM[] =
  {
    {0, "NoGrowth"},
    {1, "GrowthPerTime"},
    {2, "GrowthPerMass"},
    {3, "GrowthPerVolume"},
    {0}
  };

void OE_InlineFilter::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Byte   ("Mode",         "",           DC_,      "",         &m_iGrowthMode,   this, isParm|SetOnChange, DDBGM);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All, SolveBufferedMethod() && (m_iGrowthMode==GM_PerTime));         
  DDB.Double ("DPRate",       "",           DC_PpS,   "kPa/s",    &m_dGrowthRate,   this, isParm);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All, SolveBufferedMethod() && (m_iGrowthMode==GM_PerMass));         
  DDB.Double ("DPMassRate",   "",           DC_PpM,   "kPa/kg",   &m_dGrowthMRate,  this, isParm);
  DDB.Visibility(NM_Dynamic|SM_All|HM_All, SolveBufferedMethod() && (m_iGrowthMode==GM_PerVolume));       
  DDB.Double ("DPVolumeRate", "",           DC_PpVol, "kPa/m^3",  &m_dGrowthVRate,  this, isParm);
                                                                                   
  DDB.Visibility(NM_Dynamic|SM_All|HM_All);                                                         
  DDB.CheckBoxBtn("Clean",    "",           DC_,      "",         xidClean,         this, isParm);
                                                                                         
  DDB.Double ("",          "DPOp",          DC_DP,    "kPa",      xidDPOp,          this, isParm);
  DDB.Double ("",          "QvOp",          DC_Qv,    "L/s",      &m_dQvOp,         this, isParm);
  DDB.Double ("PowerLaw",  "Pwr",           DC_,      "",         &m_dPwrLaw,       this, isParm);
                                                                                 
  DDB.Double ("",           "DPOpCurrent",  DC_DP,   "kPa",       &m_dCurrentDPOp,  this, InitHidden);
  DDB.Double ("PressDrop",  "DP",           DC_DP,   "kPa",       &m_dDP,           this, isResult);
  }

//--------------------------------------------------------------------------

flag OE_InlineFilter::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidDPOp:
      if (DCB.rD)
        {
        m_dDPOp=*DCB.rD;
        if (DCB.ForView())
          m_dCurrentDPOp=m_dDPOp;
        }
      DCB.D=m_dDPOp;
      return 1;
    case xidClean:
      if (DCB.rB)
        {
        m_bClean=*DCB.rB;
        if (m_bClean)
          m_dCurrentDPOp=m_dDPOp;
        }
      DCB.B=m_bClean;
      return 1;
    }

  if (CFlwEqn::DataXchg(DCB))
    return 1;
  return 0;
  };

//--------------------------------------------------------------------------

flag OE_InlineFilter::ValidateData(ValidateDataBlk & VDB)
  {
  return CFlwEqn::ValidateData(VDB);
  };

//--------------------------------------------------------------------------

void OE_InlineFilter::InitFlowInfo(CFlwBlkBase & FE)
  {
  if (m_bClean)
    m_dCurrentDPOp=m_dDPOp;
  };

//--------------------------------------------------------------------------

flag OE_InlineFilter::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  if (m_bClean)
    m_dCurrentDPOp=m_dDPOp;
  m_dPwrLaw=Range(0.5,m_dPwrLaw,3.0);
  m_dQvOp=Max(1.0e-6, m_dQvOp);
  double Qm1 = FE.QmMeas();
  double Qm2 = Max(FE.QmMeas()*1.001, 1.0e-6);
  double Qv1 = Qm1/Min_FE_Rho(FE.MeanRho(pProps));
  double Qv2 = Qm2/Min_FE_Rho(FE.MeanRho(pProps));
  double QvRatio1=Qv1/m_dQvOp;
  double QvRatio2=Qv2/m_dQvOp;

  m_dDP = m_dCurrentDPOp*Pow(QvRatio1, m_dPwrLaw);
  double DPDQ = (m_dCurrentDPOp*Pow(QvRatio2, m_dPwrLaw)-m_dDP)/(Qm2-Qm1);

  m_dDensMeas=Max(0.001, FE.MeanRho(pProps));
  m_dVelMeas=FE.QmMeas()/(m_dDensMeas*FE.Area());

  FE.SetDPb(0.0, 0.0);
  FE.SetDPq(-FE.QmSign()*m_dDP, -FE.QmSign()*DPDQ);
  FE.SetVelocity(m_dVelMeas);

  return True;
  }

//--------------------------------------------------------------------------

void OE_InlineFilter::EvalDiscrete(CFlwBlkBase & FE)
  {
  if (m_bClean)
    m_dCurrentDPOp=m_dDPOp;
  else
    {
    switch (m_iGrowthMode)
      {
      case GM_NoGrowth:
        m_dCurrentDPOp=m_dDPOp;
        break;
      case GM_PerTime:
        m_dCurrentDPOp+=m_dGrowthRate*ICGetTimeInc();
        break;
      case GM_PerMass:
        m_dCurrentDPOp+=m_dGrowthMRate*fabs(FE.m_Qm)*ICGetTimeInc();
        break;
      case GM_PerVolume:
        m_dCurrentDPOp+=m_dGrowthVRate*fabs(FE.m_Qm*m_dDensMeas)*ICGetTimeInc();
        break;
      };
    }
  }

//==========================================================================
//
//
//
//==========================================================================
