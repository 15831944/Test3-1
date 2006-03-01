//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2005-03-18 11:17:57 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#include "stdafx.h"
#include "Precip.h"

//=========================================================================== 
//#pragma optimize("", off)



CPrecip::CPrecip(TaggedObject * pNd) : MBaseMethod(pNd)
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


