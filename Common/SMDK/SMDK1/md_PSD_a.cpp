//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __MD_PSD_CPP
#include "md_share1.h"
#include "md_defn.h"
#include "md_vector.h"
#include "md_spmodel.h"
#include "md_PSD.h"
//#include "alspmdlb.h"

//#define LIBCOMMENTHD "..\\"
#include "scdmacros.h"

#define MDLBASE
#define SIZEDST1
#include "models.h"


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

MIPSD::MIPSD(MVector * pVector, long iDist)
  {
  m_pVector=pVector;
  m_iDist=iDist;
  }

//---------------------------------------------------------------------------

MIPSD::~MIPSD()
  {
  }

//---------------------------------------------------------------------------

MIPSD &MIPSD::operator=(const MIPSD &V) 
  { 
  INCOMPLETECODE();
  return *this; 
  }

//---------------------------------------------------------------------------

void MIPSD::SetToZero()
  {
  const int NumInts= SizeCount;
  const int NumSpcs = PSDVectorCount;
  for (int s=0; s<NumSpcs; s++)
    {
    double * F=FracVector;
    for (int i=0; i<NumInts; i++)
      F[i] = 0.0;
    }
  }

//---------------------------------------------------------------------------

CSD_Distribution & MIPSD::GetDist(MVector * pVec, long iDist) const
  {
  SQSzDist1 * pSz = SQSzDist1::Ptr(pVec->SpMdl, false);

  if (pSz==NULL || !pSz->DistributionsExist())
    throw MMdlException(0, "Missing PSD");

  long iSz = (iDist>=0) ? iDist:pSz->iDistUsed;
  return pSz->Dist(iSz);
  }

//---------------------------------------------------------------------------

inline long RetIndex(const MIPSD * This, long i) { return This->getSizeCount()-1-i; };

MPSDDefn  * MIPSD::getDefn() const                                             { return &gs_PSDDefns[m_iDist]; };
long        MIPSD::getSizeCount() const                                        { return GetDist(m_pVector, m_iDist).NIntervals(); };
long        MIPSD::getPSDVectorCount() const                                   { return GetDist(m_pVector, m_iDist).NPriIds();};
//long        MIPSD::getSpecieIndex(long iSzVec) const                           { return GetDist(m_pVector, m_iDist).PriSzId(iSzVec);};

long        MIPSD::getSpecieCount(long iSzVec) const                           { return GetDist(m_pVector, m_iDist).NSecIds(iSzVec);};
long        MIPSD::getSpecieIndex(long iSzVec, long iSecSpecie) const          { return GetDist(m_pVector, m_iDist).SzId(iSzVec, iSecSpecie);};
MSpecieDefn * MIPSD::getSpecieDefn(long iSzVec, long iSecSpecie) const         { return &gs_MVDefn[GetDist(m_pVector, m_iDist).SzId(iSzVec, iSecSpecie)];};

double      MIPSD::getSizePassing(long iInterval) const                        { return GetDist(m_pVector, m_iDist).Intervals()[iInterval]; }
double      MIPSD::getFracPassing(long iInterval) const                        { return GetDist(m_pVector, m_iDist).PriSp[0]->FracPass[iInterval]; };
double      MIPSD::getFracPassing(long iSzVec, long iInterval) const           { return GetDist(m_pVector, m_iDist).PriSp[iSzVec]->FracPass[iInterval]; };

double      MIPSD::getMassPassing(long iInterval) const                        
  { 
  CSD_Distribution &D=GetDist(m_pVector, m_iDist);
  return D.PriSp[0]->FracPass[iInterval]*m_pVector->SpMdl->VMass[D.PriSzId(0)]; 
  };
double      MIPSD::getMassPassing(long iSzVec, long iInterval) const      
  { 
  CSD_Distribution &D=GetDist(m_pVector, m_iDist);
  return D.PriSp[iSzVec]->FracPass[iInterval]*m_pVector->SpMdl->VMass[D.PriSzId(iSzVec)]; 
  };

double    * MIPSD::getSizePassingVector() const          
  { 
  return &(GetDist(m_pVector, m_iDist).Intervals()[0]); 
  }
double    * MIPSD::getFracPassingVector(long iSzVec) const                 
  { 
  return &GetDist(m_pVector, m_iDist).PriSp[iSzVec]->FracPass[0];
  };

//---------------------------------------------------------------------------

//void MIPSD::putSieveDef(MPSDDefn* pNewSieveDef)
//  {
//  if (pSieveDefUsed!=pNewSieveDef || (pSizes && pNewSieveDef==NULL))
//    {
//    if (pSizes)
//      {
//      for (int c=0; c<iSzCompCnt; c++)
//        delete []pSizes[c];
//      delete []pSizes;
//      pSizes = NULL;
//      }
//    iSzCompCnt = 0;
//    }
//
//  pSieveDefUsed = pNewSieveDef;
//  if (pSieveDefUsed)
//    {
//    const int NumSizes = pSieveDefUsed->Count();
//    iSzCompCnt = pSieveDefUsed->SpecieSzCount();
//    typedef double* pdbl;
//    pSizes = new pdbl[iSzCompCnt];
//    for (int c=0; c<iSzCompCnt; c++)
//      pSizes[c] = new double[NumSizes];
//    }
//  }

// Memory must be able to Hold 1 double vector of 'SizeCount' elements
void MIPSD::ExtractSizeVector(double *M, double Multiplier, bool AsSizeRetained) const
  {
  long N=SizeCount;
  double * SP=SizeVector; 
  if (AsSizeRetained)
    {
    for (int i=0; i<N-1; i++)
      M[i]=SP[N-i-2]*Multiplier;
    M[N-1]=0;
    }
  else
    {
    for (int i=0; i<N; i++)
      M[i]=SP[i]*Multiplier;
    }
  };
void MIPSD::ExtractPSDMatrix(double* M, bool AsSizeRetained) const
  {
  long NSz=SizeCount;
  long N=PSDVectorCount;
  for (int n=0; n<N; n++)
    {
    double * FP=FracVector[n]; 
    if (AsSizeRetained)
      {
      for (int i=0; i<NSz; i++)
        M[n*NSz+i]=FP[NSz-1-i];
      }
    else
      {
      for (int i=0; i<NSz; i++)
        M[n*NSz+i]=FP[i];
      }
    }
  };
void MIPSD::ReplacePSDMatrix(double* M, bool AsSizeRetained)
  {
  long NSz=SizeCount;
  long N=PSDVectorCount;
  for (int n=0; n<N; n++)
    {
    double * FP=FracVector[n]; 
    if (AsSizeRetained)
      {
      for (int i=0; i<NSz; i++)
        FP[N-1-i]= M[n*NSz+i];
      }
    else
      {
      for (int i=0; i<NSz; i++)
        FP[i] = M[n*NSz+i];
      }
    }
  };

void MIPSD::ExtractPSDVector(double *V, int iPSDVec, bool AsSizeRetained) const
  {
  long N=SizeCount;
  double * FP=FracVector; 
  if (AsSizeRetained)
    {
    for (int i=0; i<N; i++)
      V[i]=FP[N-1-i];
    }
  else
    {
    for (int i=0; i<N; i++)
      V[i]=FP[i];
    }
  };
void MIPSD::ReplacePSDVector(double *V, int iPSDVec, bool AsSizeRetained)
  {
  long N=SizeCount;
  double * FP=SizeVector; 
  if (AsSizeRetained)
    {
    for (int i=0; i<N; i++)
      FP[N-1-i]=V[i];
    }
  else
    {
    for (int i=0; i<N; i++)
      FP[i]=V[i];
    }
  };

//===========================================================================
//
// Start Change MRW 7/9/4
//

// Populate a vector with the individual masses
// Memory must be able to Hold 1 double vector of 'SizeCount' elements
void MIPSD::ExtractMassVector(double *M, int iPSDVec, bool AsSizeRetained ) const

  {
      
  // The underlying structure stores a Mass and Size Fractions
  // Here we populate the individual mass fractions from the underlying
  // Total mass and size fractions

  CSD_Distribution &D=GetDist(m_pVector, m_iDist);
  //double TotalMass = m_pVector->SpMdl->VMass[D.PriSzId(iPSDVec)]; 

  double TotalMass = m_pVector->getM(D.PriSzId(iPSDVec));

  long N=SizeCount;
  double * FP=getFracPassingVector(iPSDVec);
  if (AsSizeRetained)
    {
    for (int i=0; i<N; i++)
      M[i]=FP[N-1-i]*TotalMass;
    }
  else
    {
    for (int i=0; i<N; i++)
      M[i]=FP[i]*TotalMass;
    }

  }

// Replace the vector with the specified individual masses
void MIPSD::ReplaceMassVector(double *M, int iPSDVec, bool AsSizeRetained )

  {
  // The underlying structure stores a Mass and Size Fractions
  // Here we populate the size fractions from the specified masses

  CSD_Distribution &D=GetDist(m_pVector, m_iDist);
  double TotalMass = 0.0;

  // Calculate total mass
  long N=SizeCount;
  for (int i=0; i<N; i++)
      TotalMass += M[i];

  // Set total mass
  //m_pVector->SpMdl->VMass[D.PriSzId(iPSDVec)] = TotalMass;
  m_pVector->putM(D.PriSzId(iPSDVec),TotalMass);

  // Recalculate the underlying Fractions
  double * FP=getFracPassingVector(iPSDVec);
  if (AsSizeRetained)
    {
    for (int i=0; i<N; i++)
      FP[N-1-i] = M[i]/GTZ(TotalMass);
    }
  else
    {
    for (int i=0; i<N; i++)
      FP[i] = M[i]/GTZ(TotalMass);
    }

  }

//
// End Change MRW 7/9/4
//

//===========================================================================
//
//
//
//===========================================================================
