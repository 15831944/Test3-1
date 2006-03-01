#include <stdafx.h>
#include <math.h>

#include "ZMat.h"
#include "Optlib\OptLib.h"

#define DEBUG_MESSAGES

//=============================================================================
// PROCEDURE
//    CostFunction
//-----------------------------------------------------------------------------
// ARGUMENTS
//    mX - The current iteration of the n-dimensional X vector of stockpile 
//       percentages.
//    mAssay - The m x n matrix of assays of the stockpiles. This matrix may
//       contain all the assays including the irrelevant elements.
//    mSoftConstraints - a 4 x m matrix of constraints of the assay elements.
//       The 1st row is a row of minima (greater-than) constraints. The 2nd row
//       is contains the maxima (less-than) constraints. The 3rd and 4th rows
//       contain the constraint weightings (I call alpha's) for the 1st and 2nd
//       rows respectively. If an element of the 3rd or 4th row is 0, the
//       corresponding constraint in the 1st or 2nd row is ignored.
//    mSoftRatios - a p x 5 matrix of ratio constraints where p is the number
//       of constraints. The 1st and 2nd columns are the indices of the
//       dividend (numerator, I call gamma_i) and divisor (denominator, I call
//       gamma_j) of the ratio respectively. 
//       The 3rd column is the ratio constraint (I call r). 
//       The 4th column is the weighting (alpha).
//       The 5th (last) column is a boolean representing whether the ratio
//       constraint is a greater-than (1) or less-than (0) constraint. i.e. if
//       column 5 = 1, then the inequality constraint is:
//             (gamma_i/gamma_j) > r
//    dCost - return value - the calculated cost in.
//    mFirstDeriv - return value - the vector first partial derivative of the
//       cost function at the current vector x.
//-----------------------------------------------------------------------------
// RETURN
//    Success or otherwise of the call.
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This version calculates the derivative.
//=============================================================================
bool CostFunction(const ZMat & mX, 
                  const ZMat & mAssays, 
                  const ZMat & mSoftConstraints, 
                  const ZMat & mSoftRatios, 
                  double & dCost, 
                  ZMat & mFirstDeriv)
{
   try
   {
      ZMat  mOutAssay = mAssays*mX;
      int   n = mX.RowCount();
      int   m = mAssays.RowCount();

      if (mAssays.ColCount() != n)
      {
         char  szMsg[300];

         sprintf(szMsg, "Incompatible dimensions between X vector (%d,%d) and Assay Vector (%d,%d)!",
            n, mX.ColCount(), mAssays.RowCount(), mAssays.ColCount());
         throw ZException(szMsg);
      }

      //cost = 0;
      //Del = zeros(n, 1);

      dCost = 0;

      for (int i = 0; i < mFirstDeriv.RowCount(); i++)
         for (int j = 0; j < mFirstDeriv.ColCount(); j++)
            mFirstDeriv[i][j] = 0.0;


      for  (int nElementIndex = 0; nElementIndex < m; nElementIndex++)
      {
         //-----------------
         // Matlab code
         //% greater-than constraint
         //if (SoftConstraints(3,ei) > 0)
         //   alpha = SoftConstraints(3,ei)*int_alpha/SoftConstraints(1,ei); % normalise
         //   dExp = exp(alpha*(SoftConstraints(1,ei) - elements(ei)));
         //   if (dExp > epsilon)
         //         cost = cost + dExp;
         //         Del = Del - alpha*dExp*assays(ei,:)';
         //   end
         //end
         //-----------------

         // First the minima (greater-than constraint)
         if (mSoftConstraints[2][nElementIndex] > 0)
         {
            double   dAlpha;
            if (mSoftConstraints[0][nElementIndex] == 0.0)
               dAlpha = mSoftConstraints[2][nElementIndex]*dInternalAlpha; // Can't normalise it for zero.
            else
               dAlpha = mSoftConstraints[2][nElementIndex]*dInternalAlpha/mSoftConstraints[0][nElementIndex];

            double   dExp = exp(dAlpha*(mSoftConstraints[0][nElementIndex] - mOutAssay[nElementIndex][0]));

            dCost += dExp;
            mFirstDeriv -= dAlpha*dExp*mAssays.SubMat(nElementIndex, nElementIndex, 0, mAssays.ColCount()-1).Transpose();
         }

         //-----------------
         // Matlab code
         //% less-than constraint
         //if (SoftConstraints(4,ei) > 0)
         //   alpha = SoftConstraints(4,ei)*int_alpha/SoftConstraints(2,ei); % normalise
         //   dExp = exp(alpha*(elements(ei) - SoftConstraints(2,ei)));
         //   if (dExp > epsilon)
         //         cost = cost + dExp;
         //         Del = Del + alpha*dExp*assays(ei,:)';
         //   end
         //end
         //-----------------

         // ... next the maxima (less-than constraint)
         if (mSoftConstraints[3][nElementIndex] > 0)
         {
            double   dAlpha;
            if (mSoftConstraints[1][nElementIndex] == 0.0)
               dAlpha = mSoftConstraints[3][nElementIndex]*dInternalAlpha; // Can't normalise it for zero.
            else
               dAlpha = mSoftConstraints[3][nElementIndex]*dInternalAlpha/mSoftConstraints[1][nElementIndex];

            double   dExp = exp(dAlpha*(mOutAssay[nElementIndex][0] - mSoftConstraints[1][nElementIndex]));

            dCost += dExp;
            ZMat  mSubMat(mAssays.SubMat(nElementIndex, nElementIndex, 0, mAssays.ColCount()-1).Transpose());
            mFirstDeriv += dAlpha*dExp*(mAssays.SubMat(nElementIndex, nElementIndex, 0, mAssays.ColCount()-1).Transpose());
         }
      }  // FOR

      //-----------------
      // Matlab code
      //% Now do ratios. Are there any specified?
      //ratioSize = size(SoftRatios);
      //if (ratioSize(1) > 0) & (SoftRatios(1,4) > 0)
      //   for nRatio = 1:ratioSize(1),
      //      ei = SoftRatios(nRatio,1);
      //      ej = SoftRatios(nRatio,2);
      //      ratio_target = SoftRatios(nRatio,3);
      //      alpha = SoftRatios(nRatio,4)*int_alpha/ratio_target; % normalise
      //      eTop = elements(ei);
      //      eBot = elements(ej);
      //      ratio = eTop/eBot;
      //        
      //      if (SoftRatios(nRatio,5) == 1) % then greater-than
      //            dExp = exp(alpha*(ratio_target - ratio));
      //	
      //            cost = cost + dExp;
      //            
      //            Del = Del + (alpha*dExp/(eBot*ratio_target))*(ratio*assays(ej,:)' - assays(ei,:)');
      //      else
      //            dExp = exp(alpha*(ratio - ratio_target));
      //	
      //            cost = cost + dExp;

      //            Del = Del + (alpha*dExp/(eBot*ratio_target))*(assays(ei,:)' - ratio*assays(ej,:)');
      //      end
      //   end
      //end
      //-----------------
      if ((mSoftRatios.ColCount() == 5) && (mSoftRatios.RowCount() > 0) && (mSoftRatios[0][3] > 0))
      {
         for (int nRatio = 0; nRatio < mSoftRatios.RowCount(); nRatio++)
         {
            int      ei             = int(mSoftRatios[nRatio][0]);
            int      ej             = int(mSoftRatios[nRatio][1]);
            double   ratio_target   = mSoftRatios[nRatio][2];
            if (ratio_target == 0)
               throw ZException("Ratio constraint of 0 is illegal");
            double   alpha          = mSoftRatios[nRatio][3]*dInternalAlpha/ratio_target; // normalise
            bool     bGreaterThan   = (mSoftRatios[nRatio][4] != 0.0);
            ZMat     mP(mAssays.SubMat(ei,ei,0,mAssays.ColCount()-1).Transpose()); // the ith row of the assay
            ZMat     mQ(mAssays.SubMat(ej,ej,0,mAssays.ColCount()-1).Transpose()); // the jth row of the assay
            double   eTop           = mOutAssay[ei][0];
            double   eBot           = mOutAssay[ej][0];
            double   ratio          = eTop/eBot;
            double   dExp;
            
            if (bGreaterThan) // then greater-than
            {
               dExp = exp(alpha*(ratio_target - ratio));
               dCost += dExp;
                  
               mFirstDeriv += (alpha*dExp/eBot)*(ratio*mQ - mP);
            }
            else
            {
               dExp = exp(alpha*(ratio - ratio_target));
               dCost += dExp;

//               Del = Del + (alpha*dExp/(eBot*ratio_target))*(assays(ei,:)' - ratio*assays(ej,:)');
               mFirstDeriv += (alpha*dExp/eBot)*(mP*ratio - mQ);
            }  // ELSE
         }  // FOR
      }  // IF
   }  // TRY
   catch(ZException & e)
   {
      OptimiseSetLastErr( e.Source.c_str() );
#ifdef DEBUG_MESSAGES
      printf("%s\n", e.Source.c_str());
#endif
      return false;
   }
   return true;
}

//=============================================================================
FUNCEXPORT
int  CostFunction0a( struct Matrix * mX,
                     struct Matrix * mAssays, 
                     struct Matrix * mSoftConstraints, 
                     struct Matrix * mSoftRatios,
                     double * dCost,
                     struct Matrix * mFirstDeriv)
{
   bool  bResult;
   ZMat  zmAssays;
   ZMat  zmSoftConstraints;
   ZMat  zmSoftRatios;
   ZMat  zmX;
   ZMat  zmFirstDeriv;

   Convert(*mX, zmX);
   Convert(*mAssays, zmAssays);
   Convert(*mSoftConstraints, zmSoftConstraints);
   Convert(*mSoftRatios, zmSoftRatios);
   Convert(*mFirstDeriv, zmFirstDeriv);

   bResult = CostFunction(zmX, zmAssays, zmSoftConstraints, zmSoftRatios, *dCost, zmFirstDeriv);

   if (bResult)
   {
      Convert(zmFirstDeriv, *mFirstDeriv);
      return 1;
   }
   else
      return 0;
}