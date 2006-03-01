#define EPS 1.0e-6
#include <float.h>
//#pragma optimize("g", off)

void simp2(double **a, int n, int l2[], int nl2, int *ip, int kp, double *q1)
  {
  int i;

  *ip=0;
  for (i=1;i<=nl2;i++)
    if (a[l2[i]+1][kp+1] < -EPS) 
      break;
  if (i>nl2) 
    return;
  *q1 = -a[l2[i]+1][1]/a[l2[i]+1][kp+1];
  *ip=l2[i];
  for (i=i+1;i<=nl2;i++)
    {
    int ii=l2[i];
    if (a[ii+1][kp+1] < -EPS) 
      {
      double q = -a[ii+1][1]/a[ii+1][kp+1];
      if (q < *q1) 
        {
        *ip=ii;
        *q1=q;
        } 
      else if (q == *q1) 
        {
        double qp=0,q0=0;   // CNM - THIS INITIALISE FIXES A COMPILER BUG WHEN OPTIMISED
        int k;
        for (k=1;k<=n;k++) 
          {
          qp = -a[*ip+1][k+1]/a[*ip+1][kp+1];
          q0 = -a[ii+1][k+1]/a[ii+1][kp+1];
          if (q0 != qp) 
            break;
          }
        if (q0 < qp)
          *ip=ii;
        }
      }
    }
  }
#undef EPS
