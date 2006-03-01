//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __NRECIPE_H
#define  __NRECIPE_H

#define DllImport __declspec( dllimport )
#define DllExport __declspec( dllexport )

#if  defined(__NRECIPE_CPP) || defined(__NRECIPE_C)
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

//===========================================================================

// Chapter 8
typedef int (*HpSortTestLT)(void *p, void *q);
//extern "C"
//  {
DllImportExport void HpSort(unsigned long n, void ** ra, HpSortTestLT TestLT);
DllImportExport void HpSel(unsigned long m, unsigned long n, void ** arr, void ** heap, HpSortTestLT TestLT);

//  }

//===========================================================================

// Chapter 10

class DllImportExport CNRSimplex
  {
  protected:;
    int m_N, m_M, m_M1, m_M2, m_M3, m_MP, m_NP;
    int *m_l1, *m_l2, *m_l3;
    int *m_izrov, *m_iposv;
//    double **m_a, **m_c, **m_Prob;
    double **m_c, **m_Prob;

  public:
    CNRSimplex();
    CNRSimplex(int n, int m1, int m2, int m3);
    ~CNRSimplex();

    void SetSize(int n, int m1, int m2, int m3);
    void Free();

    // iEqn - Range 1...
    // iX   - Range 1...
    void SetFnXiVal(int iX, double Val)         { m_Prob[0][iX]=Val; };
    void SetLEConst(int iEqn, double Val)          { m_Prob[iEqn][0]=Val; };
    void SetLEXiVal(int iEqn, int iX, double Val)  { m_Prob[iEqn][iX]=Val; };
    void SetGEConst(int iEqn, double Val)          { m_Prob[m_M1+iEqn][0]=Val; };
    void SetGEXiVal(int iEqn, int iX, double Val)  { m_Prob[m_M1+iEqn][iX]=Val; };
    void SetEqConst(int iEqn, double Val)          { m_Prob[m_M1+m_M2+iEqn][0]=Val; };
    void SetEqXiVal(int iEqn, int iX, double Val)  { m_Prob[m_M1+m_M2+iEqn][iX]=Val; };

    int Solve();

    double Value(int iX);

    void Dump(char * Where, int Problem, char *Fmt);

    static void Example();

  };


#undef DllImportExport

#endif