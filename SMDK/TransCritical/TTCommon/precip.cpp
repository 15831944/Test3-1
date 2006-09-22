//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2005-03-18 11:17:57 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#include "Precip.h"

//=========================================================================== 
//#pragma optimize("", off)

double CPrecip::m_dConvergenceLimit = 1.0e-8;


CPrecip::CPrecip()
{   

  /// Initialize stuff with sensible defaults
  m_dDamping = 0.0;
  m_dMassDamping = 0.0;

  iThermalLossMethod=0;
  iGrowthRateMethod=0;
  iCoolMethod=0;
  iCoolType=0;
  


}

CPrecip::~CPrecip()
{
}



//---------------------------------------------------------------------------