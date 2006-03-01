#include <stdafx.h>
#include <math.h>


#include "ZMat.h"
#include "InlineUtils.hpp"

#include "OptLib.h"


//__declspec( dllimport ) int LogNote(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
//__declspec( dllimport ) int LogError(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);
//__declspec( dllimport ) int LogWarning(LPCTSTR Where, UINT BoxFlags, LPCTSTR fmt, ...);


#define DEBUG_MESSAGES

static ZString g_OptimiseLastErrString;
static ZString g_OptimiseLastStatusString;

FUNCEXPORT
ZString OptimiseLastErr(void)
  {
    return g_OptimiseLastErrString;
  }

FUNCEXPORT
ZString OptimiseLastStatus(void)
  {
    return g_OptimiseLastStatusString;
  }

void OptimiseSetLastErr(ZString s)
  {
    g_OptimiseLastErrString = s;
  }

void OptimiseSetLastStatus(ZString s)
  {
    g_OptimiseLastStatusString = s;
  }

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
//=============================================================================
FUNCEXPORT
bool Optimise1stOrder(  double dBlendTonnes,
                        const ZMat & mAssays, 
                        const ZMat & mHardConstraints,
                        const ZMat & mSoftConstraints, 
                        const ZMat & mSoftRatios,
                        ZMat & mX)
{
   bool  bReturn = true;

   ZMat  *pAssays       = NULL;
   ZMat  *px            = NULL;
   ZMat  *pHardMaxs     = NULL;
   ZMat  *pHardMins     = NULL;
   ZMat  *pconstrained  = NULL;

   OptimiseSetLastErr("");


   try
   {
      int   m = mAssays.RowCount();
      int   n_orig = mAssays.ColCount();
      int   n;
      int   nXi;
      int   nRow;

      if (mX.RowCount() != n_orig)
      {
         //printf("X row dimension incompatible with Assay matrix\n");
         OptimiseSetLastErr("X row dimension incompatible with Assay matrix");
		 OptimiseSetLastErr("Last Search Fail: Incompatible Inputs");
         return false;
      }

      ZMat  HardMins_orig(mHardConstraints.Transpose().SubMat(":,0"));  // 1st column is mins.
      ZMat  HardMaxs_orig(mHardConstraints.Transpose().SubMat(":,1"));  // 2nd column is max's

      //% Check. If the sum of the elements of the HardMaxs_orig < 1 or the sum of
      //% the elements of HardMins_orig > 1, we have an unsolvable solution.
      //if (sum(HardMins_orig) > 1) | (sum(HardMaxs_orig) < 1)
      //   error ('Unsolveable problem due to hard constraints');
      //end
      if ((ZMat::Sum(HardMins_orig) > 1) || (ZMat::Sum(HardMaxs_orig) < 1))
      {
         printf("The hard contraints are inconsistent\n");
         OptimiseSetLastErr("The hard contraints are inconsistent");
	     OptimiseSetLastStatus("Last Search Fail: Hard Constraint Inconsistent");
         return false;
      }

      //% Check. If any of the maxs are less than the mins, we also have a
      //% nonsolvable problem.
      //for j = 1:n_orig,
      //   if (HardMins_orig(j) > HardMaxs_orig(j))
      //      error (['Unsolveable problem due to hard constraints for stockpile ', j]);
      //   end
      //end
      for (int j = 0; j < n_orig; j++)
      {
         if (HardMins_orig[j][0] > HardMaxs_orig[j][0])
         {
            //printf("The hard contraints are inconsistent\n");
            OptimiseSetLastErr("The hard contraints are inconsistent");
			OptimiseSetLastStatus("Last Search Fail: Hard Constraint Inconsistent");
            return false;
         }
      }

      //% Start x off with valid values. That is the sum of the elements = 1
      //% and all elements lie between their associated hard minima and maxima.
      //HardRange = HardMaxs_orig - HardMins_orig;
      //temp = (1 - sum(HardMins_orig))/sum(HardRange);
      //x_orig = HardMins_orig + temp*HardRange;
      ZMat HardRange = HardMaxs_orig - HardMins_orig;
      double temp = (1.0 - ZMat::Sum(HardMins_orig))/ZMat::Sum(HardRange);
      ZMat  x_orig(HardMins_orig + HardRange*temp);
      //% DEBUG
      // ZMat  x_orig("[0.4;0.6]");
      //% END DEBUG

      // Do some checks on the SoftConstraints matrix for consistency. In the
      // algorithm we check to see if the weighting is non-zero and, if so, we
      // 

      //% Next we determine which elements of the x vector are fully
      //% constrained. If we find any, we'll reduce the order of the problem.
      //% We store the constraint info in the constrained_orig vector below, one
      //% for each element of the x vector. 0 is not constrained, 1 is 
      //% constrained at a minima and 2 is constrained at a maxima. If 
      //% minima = maxima, then the value will be 3.
      //constrained_orig = zeros(n_orig,1);
      //constrained_count = 0;
      //for index = 1:n_orig,
      //    if ((HardMaxs_orig(index) - HardMins_orig(index)) < epsilon)
      //        constrained_orig(index) = 3;
      //        constrained_count = constrained_count + 1;
      //    end
      //end
      ZMat  constrained_orig(n_orig, 1, 0.0);
      int   constrained_count = 0;
      for (int index = 0; index < n_orig; index++)
      {
         if ((HardMaxs_orig[index][0] - HardMins_orig[index][0]) < dEpsilon)
         {
            constrained_orig[index][0] = 3;
            constrained_count++;
         }  // IF
      }


      if (constrained_count > 0)
      {
         // % we need to reduce the order of the problem
         // n = n_orig - constrained_count;
         // nXi = 1;
         // assays = zeros(m,n);
         // x = zeros(n,1);
         // HardMaxs = ones(n,1);
         // HardMins = zeros(n,1);
         // constrained = zeros(n,1);

         n           = n_orig - constrained_count;
         nXi         = 0;
         pAssays     = new ZMat(m, n, 0.0);
         px          = new ZMat(n, 1, 0.0);
         pHardMaxs   = new ZMat(n, 1, 1.0);
         pHardMins   = new ZMat(n,1, 0.0);
         pconstrained = new ZMat(n,1, 0.0);

         //for i = 1:n_orig,
         //   if (constrained_orig(i) ~= 3)
         //         x(nXi) = x_orig(i);
         //         assays(:,nXi) = assays_orig(:,i);
         //         HardMaxs(nXi) = HardMaxs_orig(i);
         //         HardMins(nXi) = HardMins_orig(i);
         //         nXi = nXi + 1;
         //   end
         //end
         for (int i = 0; i < n_orig; i++)
            if (constrained_orig[i][0] != 3)
            {
               (*px)[nXi][0] = x_orig[i][0];
               for (nRow = 0; nRow < pAssays->RowCount(); nRow++)
                  (*pAssays)[nRow][nXi] = mAssays[nRow][i];
               (*pHardMaxs)[nXi][0] = HardMaxs_orig[i][0];
               (*pHardMins)[nXi][0] = HardMins_orig[i][0];
               nXi++;
            }
      }
      else
      {
         //n = n_orig;
         //x = x_orig;
         //assays = assays_orig;
         //constrained = constrained_orig;
         //HardMaxs = HardMaxs_orig;
         //HardMins = HardMins_orig;
         n = n_orig;
         px          = new ZMat(x_orig);
         pAssays     = new ZMat(mAssays);
         pconstrained= new ZMat(constrained_orig);
         pHardMaxs   = new ZMat(HardMaxs_orig);
         pHardMins   = new ZMat(HardMins_orig);
      }


      //% Now we begin the iterative search.
      //% First some initial conditions
      //iter = 1;
      //cost_km1 = 1e12;
      //[cost_k, Del] = CostFunction12(x, assays, SoftConstraints, SoftAlphas, SoftRatios, 0);

      //% We record the current maximum step size we will take along the normalised
      //% derivative vector in the negative direction (i.e. towards zero) in:
      //max_step_size = n*1/sqrt(2);
      int      iter = 1;
      double   cost_km1 = 1e12;
      double   cost_k;

      ZMat  mFirstDeriv(n,1,0.0);

      if (!CostFunction(*px, *pAssays, mSoftConstraints, mSoftRatios, cost_k, mFirstDeriv))
      {
         OptimiseSetLastErr("Cost Function Failed");
         bReturn = false;
         goto __FreeMemory;
      }

      //% We record the current maximum step size we will take along the normalised
      //% derivative vector in the negative direction (i.e. towards zero) in:
      double   max_step_size = n*1/sqrt(2.0);
      ZMat     NextStep(n, 1, 0.0);

      // while (abs(cost_k - cost_km1) > epsilon) & (iter < 100)
      while ((fabs(cost_k - cost_km1) > dEpsilon) && (iter < 100))
      {
         //cost_km1 = cost_k;
         //% Calculate the conventional Newton step
         //NextStep = -1*Del;
         cost_km1 = cost_k;
         // % Calculate the conventional Newton step
         NextStep = mFirstDeriv*(-1.0);
           
         //% Next we constrain the step to lying on the x1 + x2 + ... + xn =
         //% 1 surface. To do this, the NextStep vector must have a net sum
         //% of zero.
         //NextStep = NextStep - ones(n,1)*sum(NextStep/n);
         ZMat  ones(n,1,1.0);
         NextStep -= ones*(ZMat::Sum(NextStep)/double(n));
          
         //% Next we move through the step and work out which elements of the
         //% x vector are in constraint. If they are, we work out if the
         //% current step will maintain this constraint and if so we remove
         //% that element from the adjusted step.
         //constraint_count = 0;
         //for index = 1:n,
         //   if (constrained(index) > 0)
         //         if (constrained(index) == 3)
         //            NextStep(index) = 0;
         //            constraint_count = constraint_count + 1;
         //         elseif (constrained(index) == 1) % minima, greater-than
         //            if (NextStep(index) > 0)
         //               constrained(index) = 0;
         //            else
         //               NextStep(index) = 0;
         //               constraint_count = constraint_count + 1;
         //            end
         //         elseif (constrained(index) == 2) % maxima, less-than
         //            if (NextStep(index) < 0)
         //               constrained(index) = 0;
         //            else
         //               NextStep(index) = 0;
         //               constraint_count = constraint_count + 1;
         //            end
         //         end
         //   end
         int   constraint_count = 0;
         for (int index = 0; index < n; index++)
         {
            int   nConstraintType = Round((*pconstrained)[index][0]);
            switch(nConstraintType)
            {
            case 3:
               NextStep[index][0] = 0;
               constraint_count++;
               break;
            case 1:  //  minima, greater-than
               if (NextStep[index][0] > 0)
                  (*pconstrained)[index][0] = 0;
               else
               {
                  NextStep[index][0] = 0;
                  constraint_count++;
               }
               break;
            case 2:  // maxima, less-than
               if (NextStep[index][0] < 0)
                  (*pconstrained)[index][0] = 0;
               else
               {
                  NextStep[index][0] = 0;
                  constraint_count++;
               }
               break;
            case 0:
            default:
               break;
            }  // SWITCH
         }  // FOR
          
         // % If all elements are in constraint, we terminate.
         //if (constraint_count >= (n-1))
         //   break;  
         //end
         if (constraint_count >= (n-1))
		 {
		  //LogWarning("Optimise1stOrder", 0,"Optimiser Exiting At Iteration %d: All elements in constraint",iter);
			OptimiseSetLastErr("All elements in constraint");
			OptimiseSetLastStatus("Last Search Fail: Exit on All Elements under Constraint");
			bReturn = false;
            break;  
		 }
          
         //% We correct those elements of the NextStep that correspond to
         //% elements of the x vector not in constraint, so that the
         //% NextStep vector again complies with the equality condition.
         //correction = sum(NextStep)/(n - constraint_count);
         //for index = 1:n,
         //   if (constrained(index) == 0) % not constrained
         //         NextStep(index) = NextStep(index) - correction;
         //   end
         //end
         double correction = ZMat::Sum(NextStep)/double(n - constraint_count);
         for (index = 0; index < n; index++)
            if ((*pconstrained)[index][0] == 0)  // % not constrained
               NextStep[index][0] -= correction;
          
         //% If the NextStep at this point is so pathetically small as to generate
         //% significant errors because of rounding problems, we call it a day.
         //significant_element_found = 0;
         //for index = 1:n,
         //   if (abs(NextStep(index)) > epsilon)
         //         significant_element_found = 1;
         //         break;
         //   end
         //end
         //if (significant_element_found == false)
         //   break;  % Terminate the optimisation search.
         //end
         bool  significant_element_found = false;
         for (index = 0; index < n; index++)
         {
            if (fabs(NextStep[index][0]) > dEpsilon)
            {
               significant_element_found = true;
               break;
            }
         }
         if (!significant_element_found)
		 {
		  ////LogWarning("Optimise1stOrder", 0,"Optimiser next step to small - search complete"
			OptimiseSetLastStatus("Last Search Fail: Step Too Small");
            break;  // Terminate the optimisation search.
		 } 
         // Next we normalise the NextStep vector.
         // NextStep = NextStep/sqrt(NextStep'*NextStep);
         NextStep /= sqrt((NextStep.Transpose()*NextStep)[0][0]);

         //% Next we work how far we can go before out current vector runs into a
         //% hard constraint. We will use gamma as the scalar multiplier of the
         //% NextStep vector to specify the distance we'll move. This will be
         //% constrained to the range 0 to gamma_max where gamma_max corresponds
         //% to the movement along the vector to the nearest constraint, or to
         //% max_step_size.
         //gamma_max = max_step_size; 
         //for index = 1:n,
         //   if (constrained(index) == 0) % Not currently constrained.
         //         if (abs(NextStep(index)) > epsilon)
         //            if (NextStep(index) > 0)
         //               this_gamma_max = (HardMaxs(index) - x(index))/NextStep(index);
         //            else % NextStep(index) < 0
         //               this_gamma_max = (HardMins(index) - x(index))/NextStep(index);
         //            end
         //            if (this_gamma_max < gamma_max)
         //               gamma_max = this_gamma_max;
         //            end
         //         end
         //   end
         //end
         double   gamma_max = max_step_size; 
         double   this_gamma_max;
         for (index = 0; index < n; index++)
         {
            if ((*pconstrained)[index][0] == 0) //  % Not currently constrained.
            {
               if (fabs(NextStep[index][0]) > dEpsilon)
               {
                  if (NextStep[index][0] > 0)
                     this_gamma_max = ((*pHardMaxs)[index][0] - (*px)[index][0])/NextStep[index][0];
                  else //  NextStep(index) < 0
                     this_gamma_max = ((*pHardMins)[index][0] - (*px)[index][0])/NextStep[index][0];

				  if (this_gamma_max < dEpsilon)
			      {
				   // TO DO: Work out actual stockpile number from index
				   //LogWarning("Optimise1stOrder", 0,"Optimiser running into hard constraint for non-empty stockpile %d",index);
				  }

                  if (this_gamma_max < gamma_max)
				  {
                     gamma_max = this_gamma_max;
				  }

               }  // IF
            }
         }  // FOR

         // If we find the maximum step we can take is no step at all, we
         // terminate.
         if (gamma_max < dEpsilon)
		 {
      //LogWarning("Optimise1stOrder", 0,"Optimiser Exiting At Iteration %d: Running into hard constraint",iter);
      OptimiseSetLastErr("Running into hard constraint");
			OptimiseSetLastStatus("Last Search Fail: Exit on Hard Constraint");
			bReturn = false;
            break;
		 }
          
         //% If the NextStep at this point is so pathetically small as to generate
         //% significant errors because of rounding problems, we call it a day.
         //significant_element_found = 0;
         //for index = 1:n,
         //   if (abs(NextStep(index)) > epsilon)
         //         significant_element_found = 1;
         //         break;
         //   end
         //end
         //if (significant_element_found == false)
         //   break;  % Terminate the optimisation search.
         //end
         significant_element_found = false;
         for (index = 0; index < n; index++)
         {
            if (fabs(NextStep[index][0]) > dEpsilon)
            {
               significant_element_found = true;
               break;
            }
         }
         if (!significant_element_found)
		 {
	    ////LogWarning("Optimise1stOrder", 0,"Optimiser next step to small - exiting"
      OptimiseSetLastErr("Step Too Small");
			OptimiseSetLastStatus("Last Search Fail: Step Too Small");
			bReturn = false;
            break;  // Terminate the optimisation search.
		 }
         //% Now we implement a Golden section search. We currently have two
         //% bounds. The current position x, and x + NextStep*gamma_max. We want
         //% to find the value of gamma <= gamma_max that minimises the cost
         //% function.
         //gamma_lo = 0;
         //gamma_hi = gamma_max;
         //gamma_range = gamma_max;
         //gamma_1 = gamma_hi - gamma_range*golden_ratio;
         //gamma_2 = gamma_lo + gamma_range*golden_ratio;
         //cost_lo = cost_k;
         //cost_hi = CostFunction1(x + gamma_hi*NextStep, assays, SoftConstraints, SoftAlphas, SoftRatios, 0);
         //cost_1 =  CostFunction1(x + gamma_1 *NextStep, assays, SoftConstraints, SoftAlphas, SoftRatios, 0);
         //cost_2 =  CostFunction1(x + gamma_2 *NextStep, assays, SoftConstraints, SoftAlphas, SoftRatios, 0);
         double   gamma;
         double   gamma_lo = 0;
         double   gamma_hi = gamma_max;
         double   gamma_range = gamma_max;
         double   gamma_1 = gamma_hi - gamma_range*golden_ratio;
         double   gamma_2 = gamma_lo + gamma_range*golden_ratio;
         double   cost_lo = cost_k;
         double   cost_hi;
         double   cost_1;
         double   cost_2;
         CostFunction((*px) + NextStep*gamma_hi, *pAssays, mSoftConstraints, mSoftRatios, cost_hi);
         CostFunction((*px) + NextStep*gamma_1 , *pAssays, mSoftConstraints, mSoftRatios, cost_1);
         CostFunction((*px) + NextStep*gamma_2 , *pAssays, mSoftConstraints, mSoftRatios, cost_2);

         //for i = 0:20,
         //   if (cost_1 < cost_2)
         //         gamma_hi = gamma_2;
         //         cost_hi = cost_2;
         //         gamma_range = gamma_hi - gamma_lo;
         //         gamma_2 = gamma_1;
         //         cost_2 = cost_1;
         //         gamma_1 = gamma_hi - gamma_range*golden_ratio;
         //         cost_1 = CostFunction1(x + gamma_1 *NextStep, assays, SoftConstraints, SoftAlphas, SoftRatios, 0);
         //   else
         //         gamma_lo = gamma_1;
         //         cost_lo = cost_1;
         //         gamma_range = gamma_hi - gamma_lo;
         //         gamma_1 = gamma_2;
         //         cost_1 = cost_2;
         //         gamma_2 = gamma_lo + gamma_range*golden_ratio;
         //         cost_2 = CostFunction1(x + gamma_2 *NextStep, assays, SoftConstraints, SoftAlphas, SoftRatios, 0);
         //   end
         //end
         for (int i = 0; i < 20; i++)
         {
            if (cost_1 < cost_2)
            {
               gamma_hi = gamma_2;
               cost_hi = cost_2;
               gamma_range = gamma_hi - gamma_lo;
               gamma_2 = gamma_1;
               cost_2 = cost_1;
               gamma_1 = gamma_hi - gamma_range*golden_ratio;
               CostFunction((*px) + NextStep*gamma_1, *pAssays, mSoftConstraints, mSoftRatios, cost_1);
            }
            else
            {
               gamma_lo = gamma_1;
               cost_lo = cost_1;
               gamma_range = gamma_hi - gamma_lo;
               gamma_1 = gamma_2;
               cost_1 = cost_2;
               gamma_2 = gamma_lo + gamma_range*golden_ratio;
               CostFunction((*px) + NextStep*gamma_2, *pAssays, mSoftConstraints, mSoftRatios, cost_2);
            }
         }
          
         //if (cost_1 < cost_2)
         //   gamma = gamma_1;
         //   cost_k = cost_1;
         //else
         //   gamma = gamma_2;
         //   cost_k = cost_2;
         //end
         if (cost_1 < cost_2)
         {
            gamma = gamma_1;
            cost_k = cost_1;
         }
         else
         {
            gamma = gamma_2;
            cost_k = cost_2;
         }

         //% Just in case the cost function is montonically decreasing in the
         //% direction of the vector, we also check to see if the constrained
         //% limit is in fact the lowest point.
         //if (cost_hi < cost_k) 
         //   gamma = gamma_hi;
         //   cost_k = cost_hi;
         //end
         if (cost_hi < cost_k) 
         {
            gamma = gamma_hi;
            cost_k = cost_hi;
         }
          
         // Now we actually perform the step ...
         // x = x + gamma*NextStep;
         (*px) += NextStep*gamma;
          
         //% ... and we re-call the CostFunction since we need the first
         //% derivative.
         //[cost_k, Del] = CostFunction12(x, assays, SoftConstraints, SoftAlphas, SoftRatios, 0);
         CostFunction(*px, *pAssays, mSoftConstraints, mSoftRatios, cost_k, mFirstDeriv);
          
         //% Reduce the maximum step size by the maximum of 2/3 of the current max
         //% size or the actual step taken.
         //max_step_size = max(max_step_size*0.2, gamma);
         max_step_size = Max(max_step_size*0.2, gamma);

         // Now we update the constraint vector to reflect the new constraint status.
         for (int index = 0; index < n; index++)
         {
            double   dx = (*px)[index][0];
            (*pconstrained)[index][0] = 0;
            if (dx <= ((*pHardMins)[index][0] + dEpsilon))
            {
               if (dx >= ((*pHardMaxs)[index][0] - dEpsilon))
                  (*pconstrained)[index][0] = 3;
               else
                  (*pconstrained)[index][0] = 1;
            }
            else if (dx >= ((*pHardMaxs)[index][0] - dEpsilon))
               (*pconstrained)[index][0] = 2;
         }  // FOR

         iter++;
      }  //% WHILE

      // Now we reconstruct the original x vector
      //nXi = 1;
      //for i = 1:n_orig,
      //   if (constrained_orig(i) ~= 3)
      //      x_orig(i) = x(nXi);
      //      nXi = nXi + 1;
      //   end
      //end
      nXi = 0;
      for (int i = 0; i < n_orig; i++)
      {
         if (constrained_orig[i][0] != 3)
         {
            x_orig[i][0] = (*px)[nXi][0];
            nXi++;
         }  // IF
      }  // FOR

      mX = x_orig;

	  if (iter > 100)
	  {
		//LogWarning("Optimise1stOrder", 0,"Optimiser finished searching in %d iterations",iter);
    OptimiseSetLastErr("Exit after 100 iterations");
    OptimiseSetLastStatus("Last Search Fail: Exit after 100 iterations");
		bReturn = false;
	  }
	  else
	  {
	    if ((fabs(cost_k - cost_km1) <= dEpsilon) && (bReturn))
			OptimiseSetLastStatus("Last Search OK: Converged");
	  }

   }  // TRY
   catch(...)
   {
      OptimiseSetLastErr("An exception occurred during calculation");
      OptimiseSetLastStatus("Last Search Fail: Exit on Exception");
      bReturn = false;
   }

__FreeMemory:
   if (pAssays != NULL)
      delete pAssays;
   if (px != NULL)
      delete px;
   if (pHardMaxs != NULL)
      delete pHardMaxs;
   if (pHardMins != NULL)
      delete pHardMins;
   if (pconstrained != NULL)
      delete pconstrained;

   return bReturn;
}

//=============================================================================
// This version is so managed code can call the method. All it does is to
// translate the Matrix structures to ZMat classes and call Optimise1stOrder.
//=============================================================================
FUNCEXPORT
int  Optimise1stOrderM( double dBlendTonnes,
                        struct Matrix * mAssays, 
                        struct Matrix * mHardConstraints,
                        struct Matrix * mSoftConstraints, 
                        struct Matrix * mSoftRatios,
                        struct Matrix * mX)
{
   bool  bResult;
   ZMat  zmAssays;
   ZMat  zmHardConstraints;
   ZMat  zmSoftConstraints;
   ZMat  zmSoftRatios;
   ZMat  zmX;

   Convert(*mAssays, zmAssays);
   Convert(*mHardConstraints, zmHardConstraints);
   Convert(*mSoftConstraints, zmSoftConstraints);
   Convert(*mSoftRatios, zmSoftRatios);
   Convert(*mX, zmX);

   bResult = Optimise1stOrder(dBlendTonnes,
                              zmAssays, 
                              zmHardConstraints,
                              zmSoftConstraints, 
                              zmSoftRatios,
                              zmX);

   if (bResult)
   {
      Convert(zmX, *mX);
      return 1;
   }
   else
      return 0;
}

//=============================================================================
void Convert(const ZMat & mFrom, struct Matrix & mTo)
{
   mTo.m_nRowCount = mFrom.RowCount();
   mTo.m_nColCount = mFrom.ColCount();

   for (int i = 0; i < mTo.m_nRowCount; i++)
      for (int j = 0; j < mTo.m_nColCount; j++)
         mTo.m_Data[i*mTo.m_nColCount + j] = mFrom[i][j];
}

//=============================================================================
void Convert(const struct Matrix & mFrom, ZMat & mTo)
{
   mTo.SetDim(mFrom.m_nRowCount, mFrom.m_nColCount);

   for (int i = 0; i < mFrom.m_nRowCount; i++)
      for (int j = 0; j < mFrom.m_nColCount; j++)
         mTo[i][j] = mFrom.m_Data[i*mFrom.m_nColCount + j];
}