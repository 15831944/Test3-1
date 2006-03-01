//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#define  __RL_BASIC_CPP
#include "rl_basic.h"

//===========================================================================
//
//
//
//===========================================================================

/*#D:#T:Simple Regulation Logic
#X:#h<General Description>#nThis is a regulation logic equation. Simple... ???
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Y> : ???#n
#n
#n#h<Other>#n
Short name:RL_Basic#n
Model type:Regulation Logic#n
#G:Regulation Logic
*/

IMPLEMENT_REGLTNLOGIC(RL_Basic, "RL_Basic", "", TOC_SYSTEM, "Simple", "Simple Regulation Logic") 

RL_Basic::RL_Basic(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CIO_Logic(pClass_, pTag, pAttach, eAttach)
  {
  //bOn=1;
  dOut=1.0;
  dMin=0.0;
  dMax=1.0;
  //Initialise(pAttach);
  };

//---------------------------------------------------------------------------

RL_Basic::~RL_Basic()
  {
  };

//---------------------------------------------------------------------------

void RL_Basic::BuildDataDefn(DataDefnBlk & DDB)
  {
  //DDBAdd_OnOff(DDB, "State", &bOn);
  DDB.Double("",    "Y",  DC_Frac,    "%",     &dOut,     this, isParm);
  }

//---------------------------------------------------------------------------

flag RL_Basic::DataXchg(DataChangeBlk & DCB)
  {
  return False;
  }

//---------------------------------------------------------------------------

flag RL_Basic::CopyFrom(CIO_Logic * pOther)
  {
  RL_Basic * pO=dynamic_cast<RL_Basic*>(pOther);
  if (pO)
    {
    dOut  = pO->dOut;
    dMin  = pO->dMin;
    dMax  = pO->dMax;
    return true;
    }
  return false;
  };

//---------------------------------------------------------------------------

//void RL_Basic::EvalCtrlActions(pFlwNode pFNode)
//  {
//  };

//---------------------------------------------------------------------------

void RL_Basic::SetOutput(double O) 
  { 
  dOut = Range(dMin, O, dMax); 
  };

//---------------------------------------------------------------------------

double RL_Basic::Output()
  {
  dOut=Range(dMin, dOut, dMax);
  return dOut;//bOn ? dOut : 0.0;
  };

//---------------------------------------------------------------------------

void RL_Basic::SetRange(double Min, double Max)
  {
  dMin=Min;
  dMax=Max;
  };

//---------------------------------------------------------------------------

double RL_Basic::RangeMin() 
  { 
  return dMin; 
  }; 

//---------------------------------------------------------------------------

double RL_Basic::RangeMax() 
  { 
  return dMax; 
  };

//===========================================================================
//
//
//
//===========================================================================

/*#D:#T:Simple Regulation Logic
#X:#h<General Description>#nThis is a regulation logic equation. Simple... ???
#n#n
#n#h<Variables to be supplied by the user>#n
#i<Y> : ???#n
#n
#n#h<Other>#n
Short name:RL_Basic#n
Model type:Regulation Logic#n
#G:Regulation Logic
*/

IMPLEMENT_REGLTNLOGIC(RL_Stroke, "RL_Stroke", "", TOC_SYSTEM, "Stroking", "Stroking Regulation Logic") 

RL_Stroke::RL_Stroke(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  RL_Basic(pClass_, pTag, pAttach, eAttach)
  {
  //bOn=1;
  dOut=1.0;
  dOutRqd=1.0;
  dMin=0.0;
  dMax=1.0;
  dStrkUp=10.0;
  dStrkDn=10.0;
  //Initialise(pAttach);
  };

//---------------------------------------------------------------------------

RL_Stroke::~RL_Stroke()
  {
  };

//---------------------------------------------------------------------------

void RL_Stroke::BuildDataDefn(DataDefnBlk & DDB)
  {
  //DDBAdd_OnOff(DDB, "State", &bOn);
  DDB.Double("",    "Y",        DC_Frac, "%",     &dOut,        this, isResult|0);
  DDB.Double("",    "YReqd",    DC_Frac, "%",     &dOutRqd,     this, isParm);
  DDB.Double("",    "StokeUp",  DC_T,    "s",     &dStrkUp,     this, isParm);
  DDB.Double("",    "StokeDn",  DC_T,    "s",     &dStrkDn,     this, isParm);
  }

//---------------------------------------------------------------------------

flag RL_Stroke::DataXchg(DataChangeBlk & DCB)
  {
  return False;
  }

//---------------------------------------------------------------------------

void RL_Stroke::EvalCtrlActions(pFlwNode pFNode)
  {
  double dDiff = dOutRqd-dOut;
  // limit the diff by the stroketime
//  dDiff = Sign(dDiff)*Min(fabs(dDiff),(ICGetTimeInc()/GTZ(dDiff>0.0 ? dStrkUp : dStrkDn)));
  double D=dMax-dMin;
  if (dDiff>0)
    dDiff = Min(dDiff, D*ICGetTimeInc()/GTZ(dStrkUp));
  else
    dDiff = Max(dDiff, -D*ICGetTimeInc()/GTZ(dStrkDn));
  // apply it;
  dOut+=dDiff;
  };

//---------------------------------------------------------------------------

void RL_Stroke::SetOutput(double O) 
  { 
  dOut = Range(dMin, O, dMax); 
  };

//---------------------------------------------------------------------------

double RL_Stroke::Output()
  {
  dOut=Range(dMin, dOut, dMax);
  return dOut;//bOn ? dOut : 0.0;
  };

//---------------------------------------------------------------------------

void RL_Stroke::SetRange(double Min, double Max)
  {
  dMin=Min;
  dMax=Max;
  };

//---------------------------------------------------------------------------

double RL_Stroke::RangeMin() 
  { 
  return dMin; 
  }; 

//---------------------------------------------------------------------------

double RL_Stroke::RangeMax() 
  { 
  return dMax; 
  };

//===========================================================================


