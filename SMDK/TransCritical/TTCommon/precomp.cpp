#include <cmath>

class PreCondition 
{
  double ** cmapk;
  double ** cmapk1;
  int q;   // Grid Generation Factor
  double qf;
  int N;   // Number of grid points
  double alpha, alpha1;
  
  
  

public:
  PreCondition(int, int);
  ~PreCondition();
  inline int kmap(int i, int j) 
  {
    if (i==j) return i+q;
    return j + (int) (q*log(1.+(double)pow(2., (i-j)/qf))/log(2.));
  }
    
 
};
  



PreCondition::PreCondition(int _N, int _q): q(_q), qf(_q), N(_N) 
{
  alpha = pow(2.0, 1./qf);
  alpha1 = alpha-1;
  cmapk = new double *[N];
  cmapk1 = new double *[N];
  for (int i=0; i<N; i++) {
    cmapk[i]=new double[i+1];  
    cmapk1[i]=new double[i+1];  
    for (int j=0; j<=i+1; j++) {
      int k = kmap(i, i+j);
      cmapk[i][j] = (alpha-pow(alpha, i-k)-pow(alpha, i-j))/alpha1;
      cmapk[i][j] = (pow(alpha, i-k) + pow(alpha, i-j)-1)/alpha1;
    }
  }
}

