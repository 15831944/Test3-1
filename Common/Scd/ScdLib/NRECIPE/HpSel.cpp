#define NRANSI

#define __NRECIPE_C
#include "nrecipe.h"

DllExport void HpSel(unsigned long m, unsigned long n, void ** arr, void ** heap, HpSortTestLT TestLT)
  {
  unsigned long i,j,k;
  void *swap;
      
  // Change array for a Base of 1
  arr--;
  heap--;
  //if (m > n/2 || m < 1) nrerror("probable misuse of hpsel");
  if (m < 1) return;
  for (i=1;i<=m;i++) heap[i]=arr[i];
  HpSort(m,(heap+1), TestLT);
  for (i=m+1;i<=n;i++) 
    {
//    if (arr[i] > heap[1]) 
    if (TestLT(heap[1], arr[i])) 
      {
      heap[1]=arr[i];
      for (j=1;;) 
        {
        k=j << 1;
        if (k > m) break;
//        if (k != m && heap[k] > heap[k+1]) k++;
        if (k != m && (TestLT(heap[k+1], heap[k]))) k++;
//        if (heap[j] <= heap[k]) break;
//        if (!(heap[j] > heap[k])) break;
        if (!TestLT(heap[k], heap[j])) break;
        swap=heap[k];
        heap[k]=heap[j];
        heap[j]=swap;
        j=k;
        }
      }
    }
  }
