//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#include "dbgetc.h"

#define  __NRECIPE_CPP

#include "nrecipe.h"
extern "C" 
  {
  #define ANSI
  #include "nr.h"
  #include "nrutil.h"

  }

//===========================================================================
//
//
//
//===========================================================================

CNRSimplex::CNRSimplex()
  {
  m_N=0;
  m_M1=0;
  m_M2=0;
  m_M3=0;
  }

//---------------------------------------------------------------------------

CNRSimplex::CNRSimplex(int n, int m1, int m2, int m3)
  {
  m_N=0;
  m_M1=0;
  m_M2=0;
  m_M3=0;
  SetSize(n, m1, m2, m3);
  }

//---------------------------------------------------------------------------

CNRSimplex::~CNRSimplex()
  {
  Free();
  }

//---------------------------------------------------------------------------

void CNRSimplex::SetSize(int n, int m1, int m2, int m3)
  {
  if (m_N==n && m_M1==m1 && m_M2==m2 && m_M3==m3)
    return;

  if (m_N>0)
    Free();

  m_N=n;
  m_M=m1+m2+m3;
  m_M1=m1;
  m_M2=m2;
  m_M3=m3;
  m_MP=m_M+2;
  m_NP=m_N+1;
    
  m_l1=ivector(1,m_N+1);
  m_l2=ivector(1,m_M);
  m_l3=ivector(1,m_M);

  m_izrov=ivector(1,m_N);
  m_iposv=ivector(1,m_M);

//  m_c=malloc(m_MP*m_NP*sizeof(double));
  m_Prob=matrix(0,m_MP-1,0,m_NP-1);
  m_c=matrix(1,m_MP,1,m_NP);
//  m_a=convert_matrix(&m_c[0][0],1,m_MP,1,m_NP);

  for (int i=0; i<m_MP; i++)
    for (int j=0; j<m_NP; j++)
      m_Prob[i][j]=0.0;
  }

//---------------------------------------------------------------------------

void CNRSimplex::Free()
  {
  free_ivector(m_l3,1,m_M);
  free_ivector(m_l2,1,m_M);
  free_ivector(m_l1,1,m_N+1);

  free_ivector(m_izrov,1,m_N);
  free_ivector(m_iposv,1,m_M);

  free_matrix(m_Prob,0,m_MP-1,0,m_NP-1);
  free_matrix(m_c,1,m_MP,1,m_NP);
//  free_convert_matrix(m_a,1,m_MP,1,m_NP);
  m_N=0;
  }

//---------------------------------------------------------------------------

int CNRSimplex::Solve()
  {
  int icase;
  
  for (int i=0; i<m_MP; i++)
    for (int j=0; j<m_NP; j++)
      m_c[i+1][j+1]=m_Prob[i][j];

  simplx(m_c, m_M, m_N, m_M1, m_M2, m_M3, &icase, m_izrov, m_iposv, m_l1, m_l2, m_l3);

  return icase;
  }

//---------------------------------------------------------------------------

double CNRSimplex::Value(int iX)
  {
  int i;
  for (i=1;i<=m_M+1; i++)
    if (m_iposv[i-1]==iX) return m_c[i][1];
  return 0.0;
  }

//---------------------------------------------------------------------------

void CNRSimplex::Dump(char * Where, int Problem, char *Fmt)
  {
  if (dbgfile())
    {
    dbglock();
    dbgfileflush(1);
    if (Problem)
      DumpA(Where, dbgp, m_Prob, 0, m_MP-1, 0, m_NP-1, Fmt);
    else
      DumpA(Where, dbgp, m_c, 1, m_MP, 1, m_NP, Fmt);
    dbgfileflush(0);
    dbgunlock();
    }
  }

//---------------------------------------------------------------------------

void CNRSimplex::Example()
  {
  CNRSimplex S;
//  static double c[MP][NP]=
//    {    0.,    0.,    0.,    0.,    0.,   1.0,   1.0,  
//       10.0,    0.,    0.,    0.,    0.,  -1.0,    0.,  
//       20.0,    0.,    0.,    0.,    0.,    0.,  -1.0,  
//      999.0,  -1.0,    0.,    0.,    0.,  -1.0,  -1.0,  
//      999.0,    0.,  -1.0,    0.,    0.,  -1.0,    0.,  
//         0.,    0.,    0.,  -1.0,    0.,   2.0,  -1.0,  
//         0.,    0.,    0.,    0.,  -1.0,    0.,   2.0,  
//         0.,    0.,    0.,    0.,    0.,    0.,    0.};

  S.SetSize(6, 2, 0, 4);
  S.SetFnXiVal(5, 1);
  S.SetFnXiVal(6, 1);

  S.SetLEConst(1, 10);
  S.SetLEXiVal(1, 5, -1);
  S.SetLEConst(2, 20);
  S.SetLEXiVal(2, 6, -1);

  S.SetEqConst(1, 100);
  S.SetEqXiVal(1, 1, -1);
  S.SetEqXiVal(1, 5, -1);
  S.SetEqXiVal(1, 6, -1);
  S.SetEqConst(2, 100);
  S.SetEqXiVal(2, 2, -1);
  S.SetEqXiVal(2, 5, -1);
  S.SetEqConst(3, -0);
  S.SetEqXiVal(3, 3, -1);
  S.SetEqXiVal(3, 5,  2);
  S.SetEqXiVal(3, 6, -1);
  S.SetEqConst(4, 0);
  S.SetEqXiVal(4, 4, -1);
  S.SetEqXiVal(4, 6,  2);

  if (S.Solve()==0)
    {
    for (int i=1; i<=6; i++)
      {
      double xx=S.Value(i);
      int yy=0;
      }

    }
  }

//===========================================================================
//
//
//
//===========================================================================
