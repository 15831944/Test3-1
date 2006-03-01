#define NRANSI

#define __NRECIPE_C
#include "nrecipe.h"

DllExport void HpSort(unsigned long n, void ** ra, HpSortTestLT TestLT)
  {
  unsigned long i,ir,j,l;
  void * rra;

  // Change array for a Base of 1
  ra--;

  if (n < 2) return;
  l=(n >> 1)+1;
  ir=n;
  for (;;) 
    {
    if (l > 1) 
      {
      rra=ra[--l];
      }
    else
      {
      rra=ra[ir];
      ra[ir]=ra[1];
      if (--ir == 1) 
        {
        ra[1]=rra;
        break;
        }
      }
    i=l;
    j=l+l;
    while (j <= ir) 
      {
//      if (j < ir && ra[j].dVal < ra[j+1].dVal) j++;
      if (j < ir && TestLT(ra[j], ra[j+1])) j++;
//      if (rra.dVal < ra[j].dVal) 
      if (TestLT(rra, ra[j])) 
        {
        ra[i]=ra[j];
        i=j;
        j <<= 1;
        }
      else 
        j=ir+1;
      }
    ra[i]=rra;
    }
  }
