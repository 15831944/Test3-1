//   Time-stamp: <2006-11-01 11:54:09 Rod Stephenson Transcritical Pty Ltd>

#include "stdafx.h"
#include "QALHeater.h"
#include "..\TTCommon\hxutils.h"




// void CQALHeater::doGeometry() 
// {

//   // All calculations referenced to *clean* inside tube area
//   m_dHTArea = m_lNTubes*(m_dTubeLength-2*m_dTubePlateThk)*m_dTubeID*PI;
//   m_dHTExtArea = m_dHTArea*m_dTubeOD/m_dTubeID;
//   double di = m_dTubeID;  //Correct for scaling
//   if (m_bTSScaling) {
//     m_dScaleThickness = Range(0.0, m_dScaleThickness, 0.45*di);
//     di -= 2*m_dScaleThickness; 
//   }
//   m_dTSFlowArea = PI*di*di/4.*m_lNTubes/((double)m_lNPasses);

//   // Additional geometrical calculations for full analysis of 
//   // unit can be added here.

  
// }



void CQALHeater::flowRE(MStream & sIn, MStream & sOut) 
{
  double mdot = sIn.MassFlow();

  double visc = sIn.DynamicViscosity();
  double den = sIn.Density();
  double Pr, Nu;
     
  
   
  m_dG = mdot/m_dTSFlowArea;
  m_dTSVel = m_dG/den;
  m_dReIn = m_dG*m_dTubeID/visc;
  m_dftIn = ft(m_dReIn);
  m_dPdLIn = 0.5*m_dG*m_dG/den/m_dTubeID*m_dftIn/1000.;  //kPa/m
  
  visc = sOut.DynamicViscosity();
  den = sOut.Density();

  m_dReOut = m_dG*m_dTubeID/visc;
  m_dftOut = ft(m_dReOut);
  m_dPdLOut = 0.5*m_dG*m_dG/den/m_dTubeID*m_dftOut/1000.;  //kPa/m


  m_dP = .5*(m_dPdLIn+m_dPdLOut)*m_lNPasses*m_dTubeLength;
  
  if (m_lTSHxMode) {
    Pr = sIn.msCp()*sIn.DynamicViscosity()/m_dThermalConductivity;
    double dNuIn = NuG(m_dReIn, Pr) ;
    Pr = sOut.msCp()*sOut.DynamicViscosity()/m_dThermalConductivity;
    double dNuOut = NuG(m_dReIn, Pr);
    m_dTSHTC = (dNuIn+dNuOut)/2*m_dThermalConductivity/m_dTubeID;
  }



}
 

  
