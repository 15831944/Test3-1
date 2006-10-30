//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdarg.h>

#define __PPPFTEQNS_CPP

#include "ppfteqns.h"
#include "m_link.h"

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:DP Prop to Qm for a pipe fitting
#X:#h<General Description>#nThis is a flow equation for a pipe fitting.
It is a linear flow equation, as it assumes that the pressure drop within a
pipe fitting is proportional to the mass flow within the unit.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Resistn> : The resistance within the unit.#n
#n
#n#h<Other>#n
Short name:PF_Line#n
Model type:Pipe Fitting Flow Equation#n
#G:Pipe Fitting Flow Equations
*/

IMPLEMENT_FLWEQN(PF_Linear, PipeFitGroup.Name(), "PF_Line", "", TOC_SYSTEM,
                 "DP prop to Qm",
                 "DP prop to Qm model for a Pipe fitting");

PF_Linear::PF_Linear(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  R_Fxd=1.0;
  }

//--------------------------------------------------------------------------

PF_Linear::~PF_Linear()
  {
  }

//--------------------------------------------------------------------------

void PF_Linear::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("",     "Resistn",     DC_     , "",     &R_Fxd      ,this  ,isParm);
  BuildDataDefnOveride(DDB);
  }

//--------------------------------------------------------------------------

flag PF_Linear::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  //double aQm = fabs(FE.Qm);
  //double sQm = Sign(FE.Qm);
  //FE.dPb=0.0;
  //FE.R  = R_Fxd/Regulation;
  FE.SetDPq(-FE.QmSign()*R_Fxd*FE.QmMeas()/Regulation, -FE.QmSign()*R_Fxd/Regulation);

  #if dbgValve
  dbgpln("B_Valve Liq Qm:%8g Rho:%8g OpCv:%8g dPq:%8g dPqdQ:%8g  %s",aQm,Rhoi,OpCv,dPq,dPqdQ, FullTag());
  #endif
  return True;
  }

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Reducer Cv Eqn for a pipe fitting
#X:#h<General Description>#nThis is a flow equation for a pipe fitting such as a reducer.
This calculates the pressure drop within a pipe fitting according to the flow
coefficient for the reducer.
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Cv> : The flow coefficient of the reducer.#n
#i<In_Diam> : The diameter of the downstream (inlet) pipe.#n
#n
#n#h<Other>#n
Short name:PF_RedCv#n
Model type:Pipe Fitting Flow Equation#n
#G:Pipe Fitting Flow Equations
*/

IMPLEMENT_FLWEQN(PF_ReducerCv, SizeChangeGroup.Name(), "PF_RedCv", "", TOC_SYSTEM,
                 "Reducer Cv",
                 "Cv model for Reducer");

PF_ReducerCv::PF_ReducerCv(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CFlwEqn(pClass_, pTag, pAttach, eAttach)
  {
  Cv=1.0;
  In_Diam=0.1;//100mm
  }

//--------------------------------------------------------------------------

PF_ReducerCv::~PF_ReducerCv()
  {
  }

//--------------------------------------------------------------------------

void PF_ReducerCv::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double ("Flow_Coefficient", "Cv",      DC_, "", &Cv      ,this  ,isParm);
  DDB.Double ("Inlet_Diameter",   "In_Diam", DC_L, "mm", &In_Diam ,this  ,isParm);
  BuildDataDefnOveride(DDB);
  }

//--------------------------------------------------------------------------

//void PF_ReducerCv::StartSolution(rFlwBlkBase FE)
//  {
//  };
//
//--------------------------------------------------------------------------

flag PF_ReducerCv::EvaluateFlwEqn(eScdFlwEqnTasks Task, CSpPropInfo *pProps, CFlwBlkBase & FE, bool On, double Regulation, CFBPhysData *pPhD0, CFBPhysData *pPhD1)
  {
  //double aQm = fabs(FE.Qm);
  //double Rho = Min_FE_Rho(C.Rho(som_ALL));

  double Rho=FE.MeanRho(pProps);
  double radius = In_Diam/2;
  double rhoArea = Rho*PI*Sqr(radius);
  double Flow_Vel = FE.QmMeas()/rhoArea; //v = Q/pPIr^2
  //FE.R = 0.0005*Cv*Rho*Sqr(Flow_Vel)/aQm;
  FE.SetDPq(-FE.QmSign()*0.0005*Cv*Rho*Sqr(Flow_Vel),
            -FE.QmSign()*(0.001*Cv*Rho*Pow(rhoArea,-2))*FE.QmMeas());
       // Cv*Rho*Qm/(1000*rhoArea²)
  #if dbgValve
  dbgpln("B_Valve Liq Qm:%8g Rho:%8g OpCv:%8g dPq:%8g dPqdQ:%8g  %s",aQm,Rhoi,OpCv,dPq,dPqdQ, FullTag());
  #endif
  return True;
  }

//==========================================================================
//
//
//
//==========================================================================
