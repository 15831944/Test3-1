//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "DemoSM.h"
#include "math.h"
#include "md_headers.h"
#pragma comment(lib, "rpcrt4.lib")

//===========================================================================
//
//
//
//===========================================================================

DEFINE_SPECIEMODEL(CDemoSM)
IMPLEMENT_SPECIEMODEL(CDemoSM, "DemoSM", "*Demonstration", "Demonstration User Specie Model", DLL_GroupName)

CDemoSM::CDemoSM(TaggedObject *pNd)
  {
  MInitialise(); // this must be called to initialise the SMDK library if neccessary
  m_dDummyPropertyF=456;
  m_dDummyPropertyS=567;
  }

//---------------------------------------------------------------------------

CDemoSM::~CDemoSM()
  {
  }

//---------------------------------------------------------------------------

bool CDemoSM::get_IsBaseClassOf(LPCTSTR OtherProgID)
	{
  return false;
  //return S_OK;
	}

//---------------------------------------------------------------------------

LPCTSTR CDemoSM::get_PreferredModelProgID()
	{
  return NULL;
  }

//---------------------------------------------------------------------------

void CDemoSM::BuildDataFields()
  { 
  };              

//---------------------------------------------------------------------------

bool CDemoSM::ExchangeDataFields()
  {
  return false;
  };

//---------------------------------------------------------------------------

bool CDemoSM::ValidateDataFields()
  { 
  return true; 
  }; 

//---------------------------------------------------------------------------

double CDemoSM::get_Density(long Phases, double T, double P, MArray *pMA)
  {
  MArray MA(pMA ? *pMA : this);
  double MSol=0;
  double MLiq=0;
  double MGas=0;
  long   SpecieCount=gs_MVDefn.Count();
  //double * M=MassVector;
  for (int i=0; i<SpecieCount; i++)
    {
    long Ph=gs_MVDefn[i].Phase();
    if (Ph & MP_Sol)
      MSol+=MA[i];
    else if (Ph & MP_Liq)
      MLiq+=MA[i];
    else
      MGas+=MA[i];
    }

  double DSol=2000;
  double DLiq=1000;
  double DGas=1;

  return (MSol+MLiq+MGas)/GTZ(MSol/DSol+ MLiq/DLiq+ MGas/DGas);
  }

//---------------------------------------------------------------------------

double CDemoSM::get_msEnthalpy(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_msEnthalpy(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double CDemoSM::get_msEntropy(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_msEntropy(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double CDemoSM::get_msCp(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_msCp(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double CDemoSM::get_SaturationT(double P, MArray *pMA)
  {
  return MSpModelBase::get_SaturationT(P, pMA);
  }

//---------------------------------------------------------------------------

double CDemoSM::get_SaturationP(double T, MArray *pMA)
  {
  return MSpModelBase::get_SaturationP(T, pMA);
  }

//---------------------------------------------------------------------------

double CDemoSM::get_DynamicViscosity(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_DynamicViscosity(Phases, T, P, pMA);
  }

//---------------------------------------------------------------------------

double CDemoSM::get_ThermalConductivity(long Phases, double T, double P, MArray *pMA)
  {
  return MSpModelBase::get_ThermalConductivity(Phases, T, P, pMA);
  }

//===========================================================================
//
//
//
//===========================================================================

long CDemoSM::DefinedPropertyCount()
  {
  return 2; 
  }

//---------------------------------------------------------------------------

long CDemoSM::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {
  long Inx=Index;//(Index-MSpModelBase::DefinedPropertyCount());
  switch (Inx)
    {
    case 0: Info.SetStructName("Props"); //NB: The "struct name" is used as part of tag! Recomend "Props" for compatability with other properties models.
            Info.Set(ePT_Double,    "Fixed",       "", MC_,    "",       0,      0,  MP_Result|MP_NoFiling, "A Desc"); return Inx;
    case 1: Info.Set(ePT_Double,    "Settable",    "", MC_,    "",       0,      0,  MP_HardMin|MP_Parameter, "A Desc"); return Inx;
    }
  return -1;
  };

//---------------------------------------------------------------------------

DWORD CDemoSM::GetPropertyVisibility(long Index)
  {
  return ePVis_All;
  };

//---------------------------------------------------------------------------

void CDemoSM::GetPropertyValue(long Index, ULONG Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {
  switch (Index)
    {
    case   0: Value=m_dDummyPropertyF;  return ;
    case   1: Value=m_dDummyPropertyS;  return ;
    };
  };

//---------------------------------------------------------------------------

void CDemoSM::PutPropertyValue(long Index, MPropertyValue & Value)
  {
  switch (Index)
    {
    case  1: m_dDummyPropertyS=Value; return;
    };
  };

//---------------------------------------------------------------------------
