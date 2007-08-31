#include "stdafx.h"
#include "STHx.h"
#define PI 3.14159265358979

CSTHx::CSTHx() 
{

}

CSTHx::~CSTHx() 
{

}



void CSTHx::doGeometry() 
{

  // All calculations referenced to *clean* inside tube area
  m_dHTArea = m_lNTubes*(m_dTubeLength-2*m_dTubePlateThk)*m_dTubeID*PI;
  m_dHTExtArea = m_dHTArea*m_dTubeOD/m_dTubeID;
  double di = m_dTubeID;  //Correct for scaling
  if (m_bTSScaling) {
    if (m_dScaleThickness < 0.45*di)
      di -= 2*m_dScaleThickness; 
  }
  m_dTSFlowArea = PI*di*di/4.*m_lNTubes/((double)m_lNPasses);

  // Additional geometrical calculations for full analysis of 
  // unit can be added here.

  
}
