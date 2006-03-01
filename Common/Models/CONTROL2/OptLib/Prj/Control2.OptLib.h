#ifndef OPTIMISATION_LIB_H
#define OPTIMISATION_LIB_H
//=============================================================================
#define FUNCEXPORT _declspec(dllexport)
#define FUNCIMPORT _declspec(dllimport)
#define VAREXPORT _declspec(dllexport)
#define VARIMPORT _declspec(dllimport)

#include <math.h>

#include "ZString.h"
#include "ZMat.h"

   FUNCEXPORT
   ZString OptimiseLastErr(void);
   void OptimiseSetLastErr(ZString s);

extern "C"
{


   FUNCEXPORT
   bool Optimise1stOrder(  double dBlendTonnes,
                           const ZMat & mAssays, 
                           const ZMat & mHardConstraints,
                           const ZMat & mSoftConstraints, 
                           const ZMat & mSoftRatios,
                           ZMat & mX);

   FUNCEXPORT bool CostFunction(const ZMat & mX, 
                  const ZMat & mAssays, 
                  const ZMat & mSoftConstraints, 
                  const ZMat & mSoftRatios, 
                  double & dCost,
                  ZMat & mConstraintCosts,
                  ZMat & mRatioConstraintCosts);

   struct Matrix
   {
      int      m_nRowCount;
      int      m_nColCount;
      double   m_Data[1000];
   };

   const double dInternalAlpha = 0.5;
   const double dMachine_epsilon = 2.22045e-16;
   const double dEpsilon = dMachine_epsilon*100;
   const double golden_ratio = (sqrt(5.0) - 1.0)/2.0;

   // This version is so managed code can call the method. All it does is to translate
   // the Matrix structures to ZMat classes and call Optimise1stOrder.
   FUNCEXPORT
   int Optimise1stOrderM(  double dBlendTonnes,
                           struct Matrix * mAssays, 
                           struct Matrix * mHardConstraints,
                           struct Matrix * mSoftConstraints, 
                           struct Matrix * mSoftRatios,
                           struct Matrix * mX);


   FUNCEXPORT
   int  CostFunction0(  struct Matrix * mX,
                        struct Matrix * mAssays, 
                        struct Matrix * mSoftConstraints, 
                        struct Matrix * mSoftRatios,
                        double * dCost);

   FUNCEXPORT
   int  CostFunction0b( struct Matrix * mX,
                        struct Matrix * mAssays, 
                        struct Matrix * mSoftConstraints, 
                        struct Matrix * mSoftRatios,
                        double        * dCost,
                        struct Matrix * mConstraintCosts,
                        struct Matrix * mRatioConstraintCosts);
}

bool CostFunction(const ZMat & mX, 
                  const ZMat & mAssays, 
                  const ZMat & mSoftConstraints, 
                  const ZMat & mSoftRatios, 
                  double & dCost, 
                  ZMat & mFirstDeriv);

bool CostFunction(const ZMat & mX, 
                  const ZMat & mAssays, 
                  const ZMat & mSoftConstraints, 
                  const ZMat & mSoftRatios, 
                  double & dCost);

// Some utility functions for converting between Matrix and ZMat and vice versa
void Convert(const ZMat & mFrom, struct Matrix & mTo);
void Convert(const struct Matrix & mFrom, ZMat & mTo);

//=============================================================================
#endif OPTIMISATION_LIB_H