//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __ALSPMDLB_CPP
#include "sp_db.h"
#include "alspmdlb.h"
//#include "errorlog.h"

// ==========================================================================
//
//                      Base Bayer Liquor Specie Model
//
// ==========================================================================

SMBayerRqdComp::SMBayerRqdComp()
  {
  m_rh    = 0; 
  m_rl    = 1; 
  m_ra    = 0; 
  m_rc    = 0; // mass Na2O/mass slurry ratio
  m_rCl   = 0; // mass NaCl/mass slurry ratio
  m_rF    = 0; // mass NaF/mass slurry ratio
  m_rCO3  = 0; // mass Na2CO3/mass slurry ratio
  m_rSO4  = 0; // mass Na2SO4/mass slurry ratio
  m_rSi   = 0; // mass SiO2(impurity)/mass slurry ratio
  m_rFe   = 0; // mass Fe2O3(impurity)/mass slurry ratio
  m_rOX   = 0; // mass Na2C2O4/mass slurry ratio
  m_rOC   = 0; // mass other carbon/mass slurry ratio
  m_rSoda = 0; // mass soda/mass slurry ratio
  }

// ==========================================================================
//
//                      Base Bayer Liquor Specie Model
//
// ==========================================================================

SMBayerBase::SMBayerBase(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach):
  SpModelEx(pClass_, Tag_, pAttach, eAttach)
  {
  }

// --------------------------------------------------------------------------

SMBayerBase::~SMBayerBase()
  {
  }

// --------------------------------------------------------------------------

void SMBayerBase::BuildDataDefn_Vars(DataDefnBlk & DDB)
  {                                                                                                  
  }

// --------------------------------------------------------------------------

flag SMBayerBase::DataXchg(DataChangeBlk & DCB)
  {
  if (SpModelEx::DataXchg(DCB))
    return true;

  return false;
  }


#pragma chCHECKIT(Must put Tearing methods into place)

// ==========================================================================
//
//                                  End
//
// ==========================================================================
//

