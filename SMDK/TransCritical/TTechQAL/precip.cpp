//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2005-02-18 09:43:53 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#include "stdafx.h"
#include "Precip.h"

//=========================================================================== 
//#pragma optimize("", off)



CPrecip::CPrecip(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{   

  /// Initialize stuff with sensible defaults
  m_dDamping = 0.0;
  m_dMassDamping = 0.0;


  iThermalLossMethod=0;
  iGrowthRateMethod=0;



}

CPrecip::~CPrecip()
{
}


