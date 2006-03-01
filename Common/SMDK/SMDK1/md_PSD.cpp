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
  if (iDist<0)
    {
    SQSzDist1 * pSz = SQSzDist1::Ptr(pVector->SpMdl, false);
    if (pSz)
      {
      iDist = pSz->iDistUsed;
      }
    }
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
/*
void MIPSD::SetToZero()
  {
  //do we want this???
  const long NSz=getSizeCount();
  const long N=getPSDVectorCount();
  for (int n=0; n<N; n++)
    {
    double * FP=getFracVector(n);
    for (int i=0; i<NSz; i++)
      FP[i]=0.0;
    //FP[0]=1.0;???
    }
  }
*/
//---------------------------------------------------------------------------

CSD_Distribution & MIPSD::GetDist(MVector * pVec, long iDist) const
  {
  SQSzDist1 * pSz = SQSzDist1::Ptr(pVec->SpMdl, false);

  if (pSz==NULL)
    throw MMdlException(0, "Missing PSD");
  const long DistIndex = (iDist>=0) ? iDist : pSz->iDistUsed;
  if (!pSz->DistExists(DistIndex))
    throw MMdlException(0, "Missing PSD Distribution");

  return pSz->Dist(DistIndex);
  }

//---------------------------------------------------------------------------

inline long RetIndex(const MIPSD * This, long i) { return This->getSizeCount()-1-i; };

MPSDDefn &  MIPSD::getDefn() const                                             { return gs_PSDDefns[m_iDist]; };
long        MIPSD::getPSDVectorCount() const                                   { return GetDist(m_pVector, m_iDist).NPriIds();};
long        MIPSD::getSpecieCount(long iPSDVec) const                          { return GetDist(m_pVector, m_iDist).NSecIds(iPSDVec);};
long        MIPSD::getSpecieIndex(long iPSDVec, long iSecSpecie) const         { return GetDist(m_pVector, m_iDist).SzId(iPSDVec, iSecSpecie);};
long        MIPSD::getSizeCount() const                                        { return GetDist(m_pVector, m_iDist).NIntervals(); };
double      MIPSD::getSize(long iSz) const                                     { return GetDist(m_pVector, m_iDist).Intervals()[iSz]; };
double      MIPSD::getBottomSize() const                                       { return GetDist(m_pVector, m_iDist).BottomSize(); };
double      MIPSD::getGeometricMean(long iSz) const                            { return GetDist(m_pVector, m_iDist).GeometricMean(iSz); };

double      MIPSD::getSizePassingFraction(double Frac) const
  {
  SQSzDist1 * pSz = SQSzDist1::Ptr(m_pVector->SpMdl, false);
  if (pSz==NULL)
    throw MMdlException(0, "Missing PSD");
  return pSz->SizePassingFraction(m_iDist, 0, Frac);
  };
double      MIPSD::getSizePassingFraction(long iPSDVec, double Frac) const
  {
  SQSzDist1 * pSz = SQSzDist1::Ptr(m_pVector->SpMdl, false);
  if (pSz==NULL)
    throw MMdlException(0, "Missing PSD");
  return pSz->SizePassingFraction(m_iDist, iPSDVec, Frac);
  };
double      MIPSD::getFractionPassingSize(double Size) const
  {
  SQSzDist1 * pSz = SQSzDist1::Ptr(m_pVector->SpMdl, false);
  if (pSz==NULL)
    throw MMdlException(0, "Missing PSD");
  return pSz->FractionPassingSize(m_iDist, 0, Size);
  };
double      MIPSD::getFractionPassingSize(long iPSDVec, double Size) const
  {
  SQSzDist1 * pSz = SQSzDist1::Ptr(m_pVector->SpMdl, false);
  if (pSz==NULL)
    throw MMdlException(0, "Missing PSD");
  return pSz->FractionPassingSize(m_iDist, iPSDVec, Size);
  };

double      MIPSD::getFrac(long iSz) const                                    { return GetDist(m_pVector, m_iDist).PriSp[0]->FracPass[iSz]; };
double      MIPSD::getFrac(long iPSDVec, long iSz) const                      { return GetDist(m_pVector, m_iDist).PriSp[iPSDVec]->FracPass[iSz]; };
void        MIPSD::putFrac(long iSz, double F)                                { GetDist(m_pVector, m_iDist).PriSp[0]->FracPass[iSz]=F; };
void        MIPSD::putFrac(long iPSDVec, long iSz, double F)                  { GetDist(m_pVector, m_iDist).PriSp[iPSDVec]->FracPass[iSz]=F; };
double      MIPSD::getMass(long iSz) const
  {
  CSD_Distribution &D=GetDist(m_pVector, m_iDist);
  return D.PriSp[0]->FracPass[iSz]*m_pVector->SpMdl->VMass[D.PriSzId(0)];
  };
double      MIPSD::getMass(long iPSDVec, long iSz) const
  {
  CSD_Distribution &D=GetDist(m_pVector, m_iDist);
  return D.PriSp[iPSDVec]->FracPass[iSz]*m_pVector->SpMdl->VMass[D.PriSzId(iPSDVec)];
  };

void MIPSD::Normalise(long iPSDVec/*=-1*/)
  {
  const long N=getSizeCount();
  const int Start = iPSDVec<0 ? 0 : iPSDVec;
  const int End = iPSDVec<0 ? getPSDVectorCount() : iPSDVec+1;
  for (int PSDVecCnt=Start; PSDVecCnt<End; PSDVecCnt++)
    {
    double * F=getFracVector(PSDVecCnt);
    double Sum=0.0;
    for (int i=0; i<N; i++)
      {
      F[i]=Max(F[i],0.0);
      Sum+=F[i];
      }
    const double diff=Sum-1.0;
    if (diff>1.0e-10)
      {//change is required...
      if (Sum<1.0e-12)
        {
        for (int i=1; i<N; i++)
          F[i]=0.0;
        F[0]=1.0;
        }
      else
        {
        double Sum2=0.0;
        for (int i=1; i<N; i++)
          {
          F[i]/=Sum;
          Sum2+=F[i];
          }
        F[0]=1.0-Sum2;
        }
      }
    }
  }

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

//---------------------------------------------------------------------------

// Memory must be able to Hold 1 double vector of 'SizeCount' elements
void MIPSD::ExtractSizes(double * Intervals, double Multiplier/*=1.0*/) const
  {
  CSD_Distribution &D=GetDist(m_pVector, m_iDist);
  double * SP=&(D.Intervals()[0]);
  const long N=getSizeCount();
  for (int i=0; i<N; i++)
    Intervals[i]=SP[i]*Multiplier;
  };
//---------------------------------------------------------------------------
void MIPSD::ExtractFracMatrix(double* F) const
  {
  const long NSz=getSizeCount();
  const long N=getPSDVectorCount();
  for (int n=0; n<N; n++)
    {
    double * FP=getFracVector(n);
    for (int i=0; i<NSz; i++)
      F[n*NSz+i]=FP[i];
    }
  };
//---------------------------------------------------------------------------
void MIPSD::ReplaceFracMatrix(double* F)
  {
  const long NSz=getSizeCount();
  const long N=getPSDVectorCount();
  for (int n=0; n<N; n++)
    {
    double * FP=getFracVector(n);
    for (int i=0; i<NSz; i++)
      FP[i] = F[n*NSz+i];
    }
  };
//---------------------------------------------------------------------------
void MIPSD::ExtractFracVector(double *F, int iPSDVec) const
  {
  const long N=getSizeCount();
  double * FP=getFracVector(iPSDVec);
  for (int i=0; i<N; i++)
    F[i]=FP[i];
  };
//---------------------------------------------------------------------------
void MIPSD::ReplaceFracVector(double *F, int iPSDVec)
  {
  const long N=getSizeCount();
  double * FP=getFracVector(iPSDVec);
  for (int i=0; i<N; i++)
    FP[i]=F[i];
  };

//---------------------------------------------------------------------------
// Populate a vector with the individual masses
// Memory must be able to Hold 1 double vector of 'SizeCount' elements
void MIPSD::ExtractMassVector(double *M, int iPSDVec ) const
  {
  // The underlying structure stores a Mass and Size Fractions
  // Here we populate the individual mass fractions from the underlying
  // Total mass and size fractions

  CSD_Distribution &D=GetDist(m_pVector, m_iDist);
  //double TotalMass = m_pVector->SpMdl->VMass[D.PriSzId(iPSDVec)];

  double TotalMass = m_pVector->getM(D.PriSzId(iPSDVec));

  const long N=getSizeCount();
  double * FP=getFracVector(iPSDVec);

  for (int i=0; i<N; i++)
    M[i]=FP[i]*TotalMass;
  }

//---------------------------------------------------------------------------
// Replace the vector with the specified individual masses
void MIPSD::ReplaceMassVector(double *M, int iPSDVec )
  {
  // The underlying structure stores a Mass and Size Fractions
  // Here we populate the size fractions from the specified masses

  CSD_Distribution &D=GetDist(m_pVector, m_iDist);
  double TotalMass = 0.0;

  // Calculate total mass
  const long N=getSizeCount();
  for (int i=0; i<N; i++)
    TotalMass += M[i];

  // Set total mass
  //m_pVector->SpMdl->VMass[D.PriSzId(iPSDVec)] = TotalMass;
  m_pVector->putM(D.PriSzId(iPSDVec),TotalMass);

  // Recalculate the underlying Fractions
  double * FP=getFracVector(iPSDVec);

  for (int i=0; i<N; i++)
    FP[i] = M[i]/GTZ(TotalMass);
  }

//---------------------------------------------------------------------------

double * MIPSD::getFracVector(long iPSDVec/*=0*/) const
  {
  return &GetDist(m_pVector, m_iDist).PriSp[iPSDVec]->FracPass[0];
  }

//===========================================================================
//
//
//
//===========================================================================
