//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdarg.h>
#define __PC_COMPR_CPP

#include "pc_compr.h"

//==========================================================================
//
//
//
//==========================================================================
/*#D:#T:Fixed Boost
#X:#n#u<#l<#b<General Description>>>#PThis allows the user to set the required pressure boost
of the pump.
#P#u<#l<#b<Variables to be supplied by the user>>>#n
#p#b<Boost :>#NThe required pressure boost of the pump, in kiloPascals.
The default is 100 kPa
*/

IMPLEMENT_FLWEQN(PC_Compr, PumpGroup.Name(), "PC_Compr", "", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                 "Compressor",
                 "Compressor Model for Mossgas");

PC_Compr::PC_Compr(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FlwEqn(pClass_, pTag, pAttach, eAttach),
  P_SurgeCondition("DataOnly", "SurgeCondition", this, TOA_Embedded)
  {
  P_SetpointPressure = 85*101; // bara
  P_RecircRatio   = 0.0;
  }

//--------------------------------------------------------------------------

PC_Compr::~PC_Compr()
  {
  }

//--------------------------------------------------------------------------

void PC_Compr::BuildDataDefn(DataDefnBlk & DDB)
  {
    DDB.Double ("",   "SetpointPressure",    DC_P,"kPag",    &P_SetpointPressure, this  ,isParm);
    DDB.Double ("",   "RecircRatio",         DC_,"",         &P_RecircRatio,      this  ,isParm);
    DDB.Object(&P_SurgeCondition, this, NULL, NULL, DDB_RqdPage);
  }

//--------------------------------------------------------------------------

/*void PC_Compr::StartSolution(rFlwEqnBlk FE)
  {
 };*/

//--------------------------------------------------------------------------

flag PC_Compr::EvaluateFlwEqn(CSpPropInfo *pProps, FlwBlkBase & FE, double Regulation, FBPhysData *pPhD0, FBPhysData *pPhD1)
  {
  /*
  FE.SetQmFree();     // No restriction on flow. If fixed mass
                      // would set a fixed mass compressor FE.SetQmReqd(Reqd_Qm*Regulation);

  FE.dPb  =P_Boost*Regulation;     // this set the fixed boost
  FE.dPbdQ=0.0;
  //FE.dPq  =0.0;
  //FE.dPqdQ=0.0;



  // This code fragment is given to indicate the sign of the flow derivatives
  // are negative since they are drops
  // X is the resistance
  FE.dPq   = -FE.Qm*P_Resistance;
  FE.dPqdQ = -Sign(FE.Qm)*P_Resistance;

  */

  // Always want to maintain a constant pressure on the outlet as defined by input variable
  // P_SetpointPressure.The flow will consequently be determined by the environment
  FE.SetQmFree();
  FE.SetDPb(P_SetpointPressure, 0.0);
  double SurgeMassFlow = P_SurgeCondition.Xy(P_SetpointPressure);
  if( SurgeMassFlow < FE.GetQm() )
    {
    P_RecircRatio = 0.0;
    }
  else
    {
    P_RecircRatio = (SurgeMassFlow - FE.GetQm())/FE.GetQm();
    }


  return True;
  }

//--------------------------------------------------------------------------
