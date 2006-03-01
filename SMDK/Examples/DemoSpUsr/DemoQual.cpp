//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "DemoQual.h"
#include "math.h"
#include "md_headers.h"
#pragma comment(lib, "rpcrt4.lib")

//===========================================================================
//
//
//
//===========================================================================

DEFINE_SPECIEQUALITY(CDemoQual)
IMPLEMENT_SPECIEQUALITY(CDemoQual, "DemoQual", "DemoQual ShortDesc", "CDemoQual Desc", DLL_GroupName)

CDemoQual::CDemoQual(TaggedObject * pNd)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary

  m_dSG=0;
  m_dSetProp=2;
  }

//---------------------------------------------------------------------------

CDemoQual::~CDemoQual()
  {
  }

//---------------------------------------------------------------------------

//===========================================================================
//
//
//
//===========================================================================

long CDemoQual::DefinedPropertyCount()
  {
  return 2; 
  }

//---------------------------------------------------------------------------

long CDemoQual::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {
  long Inx=Index;//(Index-MSpModelBase::DefinedPropertyCount());
  switch (Inx)
    {
    case 0: Info.SetStructName("DemoQuals");
            Info.Set(ePT_Double,    "SG",        "", MC_,    "",       0,      0,  MP_Result|MP_NoFiling, "A Desc"); return Inx;
    case 1: Info.Set(ePT_Double,    "Set",       "", MC_,    "",       0,      0,  MP_HardMin|MP_Parameter, "A Desc"); return Inx;
    }
  return -1;
  };

//---------------------------------------------------------------------------

DWORD CDemoQual::GetPropertyVisibility(long Index)
  {
  return ePVis_All;
  };

//---------------------------------------------------------------------------

void CDemoQual::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {
  switch (Index)
    {
    case   0: Value=m_dSG;  return ;
    case   1: Value=m_dSetProp;  return ;
    };
  };

//---------------------------------------------------------------------------

void CDemoQual::PutPropertyValue(long Index, MPropertyValue & Value)
  {
  switch (Index)
    {
    case  1: m_dSetProp=Value; return;
    };
  };

//---------------------------------------------------------------------------

void CDemoQual::ExchangeSpecies(MSpQualityBase * Other) 
  {
  };
bool CDemoQual::EquilRqd4HEval()
  {
  return false;
  };
void CDemoQual::Equilibrate() 
  {
  MVector M1(this);
  m_dSG=M1.Density()/1000;
  };
void CDemoQual::ZeroMass() 
  {
  };
void CDemoQual::ZeroDeriv() 
  {
  };
void CDemoQual::ScaleMass(long Phase, double Mult) 
  {
  };
void CDemoQual::ScaleMass(CArray<int,int> &SpIds, double Mult)
  {
  };
void CDemoQual::SetMassF(MSpQualityBase * QualSet) 
  {
  CDemoQual * pQ2=dynamic_cast<CDemoQual*>(QualSet);
  m_dSG=pQ2->m_dSG;
  m_dSetProp=pQ2->m_dSetProp;
  };
void CDemoQual::AddMassF(MSpQualityBase * QualAdd, MArray & MAdd) 
  {
  MVector M1(this);
  CDemoQual * pQualAdd=dynamic_cast<CDemoQual*>(QualAdd);
  
  double m1    = M1.Mass();
  double m2    = MAdd.Mass();
  double Frac1 = m1/GTZ(m1+m2);
  double Frac2 = 1.0-Frac1;
  
  m_dSG       = Frac1*m_dSG + Frac2*pQualAdd->m_dSG;
  m_dSetProp  = Frac1*m_dSetProp + Frac2*pQualAdd->m_dSetProp;
  };
void CDemoQual::SubMassF(MSpQualityBase * QualSub, MArray & M2) 
  {
  };
void CDemoQual::Copy(MSpQualityBase * QualCopy) 
  {
  CDemoQual * pQ2=dynamic_cast<CDemoQual*>(QualCopy);
  m_dSG=pQ2->m_dSG;
  m_dSetProp=pQ2->m_dSetProp;
  };
void CDemoQual::AddDeriv(MSpQualityBase * pQual2, double Sgn_) 
  {
  };
void CDemoQual::AddDiscrete(MSpQualityBase * pQual2, double Sgn_) 
  {
  };
