//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/*
 * Preprocessed by tbutoi version 3.43
 */
 
/*
 * Translated by tbxlator version 3.43
 */
 
#include "stdafx.h"
//#include <stddef.h>
//#include <alloc.h>
#include <stdio.h>
 
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
//#include <dos.h>
#include <math.h>
#include "nrecipe.h"
 
#include "sc_defs.h"
#include "debugnew.h" // must follow all IMPLEMENT_DYNCREATE & IMPLEMENT_SERIAL

#include "float.h"
#include "errorlog.h"

#include ".\indirect\include\compcol_double.h"
#include ".\indirect\include\iohb_double.h"
//#include "mv_blas1_double.h"  ???
#include ".\indirect\include\diagpre_double.h"
#include ".\indirect\include\icpre_double.h"
#include ".\indirect\include\ilupre_double.h"

#include ".\indirect\iml\bicg.h"
#include ".\indirect\iml\bicgstab.h"
#include ".\indirect\iml\ir.h"
#include ".\indirect\iml\cg.h"
#include ".\indirect\iml\gmres.h"


//----------------------------------------------------------------------------
/* Unit          :  CSparseSolver                   FILE : SparseSo.PAS        */
/* Dependancies  :  Matrix, Ordering, Globals.                                */
/* First Created :  January 1991                                              */
/* Documentation :  SS1.DOC                                                   */
/* Design & Implementation by : S.Pahl                                        */
//----------------------------------------------------------------------------
/*                                UPGRADES :                                  */
/* ÚÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿ */
/* ³ Version ³       Date :      ³         By :             ³ Document File:³ */
/* ÃÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´ */
/* ³  1.0    ³  14 January  1991 ³ S.Pahl                   ³  SS1.Doc      ³ */
/* ³  1.01   ³   8 February 1991 ³ S.Pahl                   ³  SS101.Doc    ³ */
/* ³         ³                   ³                          ³               ³ */
/* ³         ³                   ³                          ³               ³ */
/* ³         ³                   ³                          ³               ³ */
/* ÀÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ */
//----------------------------------------------------------------------------
 
#include ".\Direct\mtx_glbl.h"
#include ".\Direct\mtx_ordr.h"
#include ".\Direct\mtx_mtrx.h"
 
#define extern
#define NOTEXTERN
#include ".\sparseslv.h"
#undef NOTEXTERN
#undef extern

// #define dbginsert

//----------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------

IMPLEMENT_SPARES(CSparseSolverD, 50)
  
CSparseSolverD::CSparseSolverD(CSparseSolver * pSlv,  dword n) : CSparseSolverA(pSlv), SpM_Mtrx(n)
  {
  };


CSparseSolverD::~CSparseSolverD() 
  {
  };

//----------------------------------------------------------------------------

void CSparseSolverD::Clear(dword n)
  {
  SpM_Mtrx::Clear(n);
  };

//----------------------------------------------------------------------------

void CSparseSolverD::SetValue (dword rowpos, dword colpos, double value)
  {
  SpM_Mtrx::insertelement(rowpos,colpos,value);
  }

//----------------------------------------------------------------------------

double CSparseSolverD::GetValue (dword rowpos, dword colpos)
  {
  return SpM_Mtrx::getelement(rowpos,colpos);
  }

//----------------------------------------------------------------------------

int CSparseSolverD::Errors()
  {
  return errors;//SpM_Mtrx::Clear(n);
  };

//----------------------------------------------------------------------------

void CSparseSolverD::GetNextPivot(struct CBldSeqItem *pivot,
              dword diag)
  {
  dword  rownum, colnum, k;
  dword  bestsofar, max_fill_ins;
  flag   found, exists;
  double value;
  SpM_Ordr::getfirstrow(& rownum, 1, & exists);
 
  /* get m_Row with one entry */
  if (exists && (errors==SMTX_NONE))
    {
    SpM_Mtrx::getfirstcol(rownum, & colnum, & value);
    if (errors==SMTX_NONE)
      {
      pivot->m_Value = value;
      pivot->m_Col = colnum;
      pivot->m_Row = rownum;
      }
    return;
    /* Case 1 */
    }
  SpM_Ordr::getfirstcol(& colnum, 1, & exists); /* get m_Col with one entry */
  if (exists && (errors==SMTX_NONE))
    {
    SpM_Mtrx::getfirstrow(colnum, & rownum, & value);
    SpM_Mtrx::comparerow(rownum, colnum, value, & exists);
    if (exists && (errors==SMTX_NONE))
      {
      pivot->m_Value = value;
      pivot->m_Col = colnum;
      pivot->m_Row = rownum;
      return;
      /* Case 2 */
      }
    }
  k = 2;
  bestsofar = m_Slv.m_nSize * m_Slv.m_nSize;
 
  do
    {
    SpM_Ordr::getfirstrow(& rownum, k, & exists);  /* Search rows first */
    while (exists)
      {
      if (((k - 1) * (k - 1)) >= bestsofar)          /*RETURN Pivot*/
        return;
      SpM_Mtrx::searchrow(rownum, & colnum, & max_fill_ins, & value);
      if (max_fill_ins < bestsofar)
        {
        bestsofar = max_fill_ins;
        pivot->m_Value = value;
        pivot->m_Col = colnum;
        pivot->m_Row = rownum;
        }
      SpM_Ordr::getnextrow(& rownum, & exists);
      }
 
    SpM_Ordr::getfirstcol(& colnum, k, & exists);   /* Search cols next */
    while (exists)
      {
      if ((k - 1) * (k) >= bestsofar)               /*RETURN Pivot*/
        return;
      SpM_Mtrx::searchcol(colnum, diag, & rownum, & max_fill_ins,
        & value, & found);
      if (found && (max_fill_ins < bestsofar))
        {
        bestsofar = max_fill_ins;
        pivot->m_Value = value;
        pivot->m_Col = colnum;
        pivot->m_Row = rownum;
        }
      SpM_Ordr::getnextcol(& colnum, & exists);
      }
    k++;
    }
  while (! (FALSE));
  }
 

//----------------------------------------------------------------------------

int CSparseSolverD::Initialise(flag Direct)
  {
  errors = SMTX_NONE;
  POSITION Pos=m_Slv.m_BuildSeq.GetHeadPosition();
  while (Pos)
    {
    CBldSeqItem &B=m_Slv.m_BuildSeq.GetNext(Pos);
    SetValue(B.m_Row, B.m_Col, B.m_Value);
    }
  return errors;
  };

//----------------------------------------------------------------------------
 
int CSparseSolverD::Analyse()
  {
  CDArray & b = m_Slv.m_Vect;

  dword  diag, rownum;
  double temp, mult;
  struct CBldSeqItem pivot;
  flag exists;

  if ((errors != SMTX_NONE))
    {                            /* Possible Insert error */
    return errors;
    }
  if ((SpM_Mtrx::Singular() == TRUE))
    {                            /* Row/Col in the matrix is a zero vector */
    return SMTX_SINGULAR;        /* Checked after inserting all the values */
    }
  SpM_Mtrx::createordering();
  for (diag = 1; diag <= m_Slv.m_nSize - 1 && (errors==SMTX_NONE); diag++)
    {
    /*N-1*/                     /* Matrix reduction */
 
    GetNextPivot(& pivot, diag);
    if (pivot.m_Row != diag && (errors==SMTX_NONE))
      {                                              
      SpM_Mtrx::swoprow(diag, pivot.m_Row);    /* swop rows/cols  */
      temp = b[ (diag) - 1];                 /* according to the*/
      b[ (diag) - 1] = b[ (pivot.m_Row) - 1];  /* pivot found.    */
      b[ (pivot.m_Row) - 1] = temp;
      }
    if (pivot.m_Col != diag && (errors==SMTX_NONE))
      {
      SpM_Mtrx::swopcol(diag, pivot.m_Col);
      }
    SpM_Mtrx::getfirstactiverow(diag, & rownum, & exists);
    /* -- Reduce -- */
    while (exists && (errors==SMTX_NONE))
      {
      SpM_Mtrx::addrows(rownum, diag, & mult);
      b[ (rownum) - 1] = b[ (rownum) - 1] + b[ (diag) - 1] * mult;
      SpM_Mtrx::getnextactiverow(& rownum, & exists);
      }
    SpM_Ordr::rowremove(diag, SpM_Mtrx::rownonzerocount(diag));
    SpM_Ordr::colremove(diag, SpM_Mtrx::colnonzerocount(diag));
    }
  m_Slv.m_bAnalysedOK=(errors==SMTX_NONE);
  return errors;
  }
 
//----------------------------------------------------------------------------
 
//int CSparseSolverD::Analyse()
//  {
//  return Analyse(m_Slv.m_Vect);
//  }

//----------------------------------------------------------------------------
 
int CSparseSolverD::Solve()
  {

  //DWORD FPP_State=FPP_EnableExceptions();
  DWORD FPP_State=_controlfp(0,0);                                          // Get Old fpControl
  _clearfp();                                                               // clear any outstanding exceptions
  _controlfp(/*_EM_ZERODIVIDE|_EM_OVERFLOW|*/_EM_DENORMAL|/*_EM_UNDERFLOW|*/_EM_INEXACT, _MCW_EM);	//Set New fpControl

  try
    {
    for (dword i = 1; i <= m_Slv.m_nSize; i++)    /* zero input vector ready for next solution */
      m_Slv.m_Soln[(i)-1] = 0.0;
    SpM_Mtrx::solve(m_Slv.m_Soln, m_Slv.m_Vect);
    for (dword i = 1; i <= m_Slv.m_nSize; i++)    /* zero input vector ready for next solution */
      m_Slv.m_Vect[(i)-1] = 0.0;
    FPP_RestoreExceptions(FPP_State);
    return errors;
    }
  catch (MFPPException &e)
    {
    errors=SMTX_FPP_ERROR;
    e.ClearFPP();
    LogError("FlwNet Solve", 0, e.Description);
    goto Problem;
    }
  catch (MSysException &e)
    {
    LogError("FlwNet Solve", 0, e.Description);
    goto Problem;
    }
  catch (CException *e)
    {
    char Buff[1024];
    if (e->GetErrorMessage(Buff, sizeof(Buff)))
      LogError("FlwNet Solve", 0, Buff);
    else
      LogError("FlwNet Solve", 0, "Unknown Exception");
    e->Delete();
    goto Problem;
    }
  catch (...)
    {
    LogError("FlwNet Solve", 0, "Unknown Exception");
    goto Problem;
    }

Problem:
  FPP_RestoreExceptions(FPP_State);

//m_Slv.m_Errors=SMTX_SINGULAR;
//    e->Delete();
//    throw;
  
  return errors;
  }

//----------------------------------------------------------------------------

//void CSparseSolverD::RowPrint (char Symbolic, int Wide, int Prec)
//  {
//  SpM_Mtrx::rowprint (m_Slv.m_Vect, Symbolic, Wide, Prec);
//  };

//----------------------------------------------------------------------------
 
void CSparseSolverD::Report_Stats(Mtx_Info *table)
  {
  SpM_Mtrx::report_stats(table);
  }

//----------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------

IMPLEMENT_SPARES(CSparseSolverI, 50)

CSparseSolverI::CSparseSolverI(CSparseSolver * pSlv,  dword n) : CSparseSolverA(pSlv) 
  {
  };

//----------------------------------------------------------------------------

CSparseSolverI::~CSparseSolverI()
  {
  };

//----------------------------------------------------------------------------

void CSparseSolverI::Clear(dword n)
  {
  m_Val.SetSize(0);
  m_RowPtr.SetSize(0);
  m_ColInd.SetSize(0);
  m_iError=SMTX_NONE;
  };

//----------------------------------------------------------------------------

void CSparseSolverI::SetValue (dword rowpos, dword colpos, double value)
  {
  };

//----------------------------------------------------------------------------

double CSparseSolverI::GetValue (dword rowpos, dword colpos)
  { 
  return dNAN; 
  };

//----------------------------------------------------------------------------

int CSparseSolverI::Initialise(flag Direct)
  {
  m_iError= SMTX_NONE;
  return m_iError;
  };

//----------------------------------------------------------------------------

void CSparseSolverI::SwapRows(int i, int j, LPCTSTR Desc)
  {
  Exchange(m_Slv.m_Rows[i], m_Slv.m_Rows[j]);
  Exchange(m_Slv.m_RowPos[i], m_Slv.m_RowPos[j]);
  Exchange(m_Slv.m_Vect[i-1], m_Slv.m_Vect[j-1]);
  Exchange(m_Slv.m_Soln[i-1], m_Slv.m_Soln[j-1]);

  if (1)
    {
    dbgpln("Swap1 %4i * %-4i  %s", i,j,Desc); 
    if (0)
      m_Slv.RowPrint (true, -1, -1, false);
    }
  };

inline int FindColInRow(CSSColSeq * Row, int ColRqd) 
  {
  for (int ci=0; ci<Row->GetSize(); ci++)
    if (Row->GetAt(ci)->m_Col == ColRqd)
      return ci;
  return -1;
  }

flag CSparseSolverI::SwapRows(CSSRowSeq & Rows, CArray<bool, bool> & RowPin, int r1, int r2, int rtop, int rend)
  {
  // row r1 is fixed 
  // row r2 is tentative
  int rlo=Min(r1,r2);
  int rhi=Max(r1,r2);
  int c2=r2;

  if (1)
    {
    dbgp("Swap2 %4i %4i %4i [", r1, r2, rtop);
    for (int i=1; i<Rows.GetSize(); i++)
      dbgp(RowPin[i]&&RowPin[i-1]?" %i":RowPin[i]?"%i":".",i);
    dbgpln("]");
    }

  RowPin[r1]=true;
  SwapRows(r1, r2, "Lo");

  ASSERT(FindColInRow(Rows[r1], r1)>=0);
  // was there an element in col(rlo);
  if (FindColInRow(Rows[r1], r1)>=0 && FindColInRow(Rows[r2], r2)>=0)
    goto Done;

  // check above top - untouched
  for (int r = rtop+1; r <= rend; r++)
    {
    if (FindColInRow(Rows[r], c2)>=0)
      {
      SwapRows(r2, r, "HiX");
      goto Done;
      }
    }

  // check Hi
  for (int r = rhi+1; r <= rtop; r++)
    {
    if (!RowPin[r] && (FindColInRow(Rows[r], c2)>=0))
      {
      if (SwapRows(Rows, RowPin, r2, r, rtop, rend))
        goto Done;
      }
    }

  // look lo
  for (int r = rhi-1; r >= 1; r--)
    {
    if (!RowPin[r] && (FindColInRow(Rows[r], c2)>=0))
      {
      if (SwapRows(Rows, RowPin, r2, r, rtop, rend))
        goto Done;
      }
    }

  SwapRows(r1, r2, "Lo - Restore"); // put it back
  RowPin[r1]=false;
  return false;

Done:
  RowPin[r1]=false;
  return true;
  }

int CSparseSolverI::Analyse ()
  {
  m_Slv.SetupRows();

  static int DoReverseTest=0;
  if (DoReverseTest)
    {
    CSSRowSeq TmpRows;
    int n=m_Slv.m_Rows.GetSize();
    TmpRows.SetSize(n);
    for (int i=1, j=n-1; i<n; i++, j--)
      TmpRows[i]=m_Slv.m_Rows[j];
    for (int i=1; i<n; i++)
      m_Slv.m_Rows[i]=TmpRows[i];
    }

  CBuildSeqList & BuildSeq=m_Slv.m_BuildSeq;
  CSSRowSeq     & Rows=m_Slv.m_Rows;
  dword           Size=m_Slv.m_nSize;

  // Sort Rows so that all diagonal elements are nonzero
  for (dword r = 1; r <= Size; r++)
    {
    if (FindColInRow(Rows[r], r)>=0)
      goto NextRow; // got diagonal
    else
      {
      // look Hi
      for (dword r1 = r+1; r1 <= Size; r1++)
        {
        if (FindColInRow(Rows[r1], r)>=0)
          {
          SwapRows(r, r1, "Hi");
          goto NextRow; // got diagonal
          }
        }
      // Could not find Hi - look Lo
      for (dword r1 = r-1; r1>=1; r1--)
        {
        if (FindColInRow(Rows[r1], r)>=0)
          {
          if (SwapRows(Rows, m_Slv.m_RowPin, r, r1, r, Size))
            goto NextRow; // got diagonal
          }
        }

      DoBreak();
      }
NextRow:
    // Checks;
    for (dword r1 = 1; r1<=r; r1++)
      if (FindColInRow(Rows[r1], r1)<0)
        DoBreak();
    }

  // Build Compressed Row Storage
  m_Val.SetSize(0, BuildSeq.GetSize());
  m_RowPtr.SetSize(0, m_Slv.m_nSize+1);
  m_ColInd.SetSize(0, m_Val.GetSize());

  int rp=0;
  for (dword r = 1; r <= Size; r++)
    {
    int index=0;
    m_RowPtr.Add(rp);
    int ccnt=Rows[r]->GetSize();
    rp+=ccnt;

    for (int ci=0; ci<ccnt; ci++)
      {
      CBldSeqItem &BI=*Rows[r]->ElementAt(ci);
      m_Val.Add(BI.m_Value);
      m_ColInd.Add(BI.m_Col-1);
      }
    }
  m_RowPtr.Add(rp);

  if (1)
    {
    dbgpln("Size     %6i", Size); 
    dbgpln("ValSz    %6i", m_Val.GetSize()); 
    dbgpln("RowPtrSz %6i", m_RowPtr.GetSize()); 
    dbgpln("ColIndSz %6i", m_ColInd.GetSize()); 

    for (int i=0; i<m_Val.GetSize(); i++)
      {
      if (i%10==0)
        dbgp(i==0?"Val   %6i:":"\n      %6i:",i);
      dbgp(" %#12.4lg", m_Val[i]); 
      }
    dbgpln("");

    for (int i=0; i<m_RowPtr.GetSize(); i++)
      {
      if (i%10==0)
        dbgp(i==0?"RowPtr%6i:":"\n      %6i:",i);
      dbgp(" %5i", m_RowPtr[i]); 
      }
    dbgpln("");

    for (int i=0; i<m_ColInd.GetSize(); i++)
      {
      if (i%10==0)
        dbgp(i==0?"ColInd%6i:":"\n      %6i:",i);
      dbgp(" %5i", m_ColInd[i]); 
      }
    dbgpln("");
    }

  return 0; 
  };

//----------------------------------------------------------------------------

int CSparseSolverI::Solve ()
  {
  double tol=1e-12;
  int    result=0;
  int    maxit=150;

  unsigned int FPP_State=_controlfp(0,0);             // Get Old fpControl
  _clearfp();                                         // clear any outstanding exceptions
  _controlfp(/*_EM_ZERODIVIDE|_EM_OVERFLOW|*/_EM_DENORMAL|_EM_UNDERFLOW|_EM_INEXACT, _MCW_EM);	//Set New fpControl

  CompRow_Mat_double A(m_Slv.m_nSize, m_Slv.m_nSize, m_Val.GetSize(), &m_Val[0], &m_RowPtr[0], &m_ColInd[0]);
  VECTOR_double b(A.dim(1), 0.0);
  VECTOR_double x(A.dim(1), 0.0);

  for (dword i=0; i<m_Slv.m_nSize; i++)
    {
    b[i]=m_Slv.m_Vect[i]; 
    x[i]=m_Slv.m_Soln[i]; 
    }

  if (1)
    {
    for (dword i=0; i<m_Slv.m_nSize; i++)
      {
      if (i%10==0)
        dbgp(i==0?"b: %4i:":"\n   %4i:",i);
      dbgp(" %#12.4g", b[i]);
      }
    dbgpln("");
    for (dword i=0; i<m_Slv.m_nSize; i++)
      {
      if (i%10==0)
        dbgp(i==0?"x: %4i:":"\n   %4i:",i);
      dbgp(" %#12.4g", x[i]);
      }
    dbgpln("");
    }

  if (1)
    {
    try
      {
      CompRow_ILUPreconditioner_double D(A);
      result=BiCGSTAB(A,x,b,D, maxit, tol);
      }
    catch (...)
      {
      _clearfp();                                       // clear any outstanding exceptions
      result = 1;
      }
    }
  else
    result = 1;
  if (result>=1)
    {
    DiagPreconditioner_double DD(A);

    // Init x to zero;
    x=0;
    // Reset Tols etc.
    maxit=150;
    tol=1e-9;

    try
      {
      //result=BiCG(A,x,b,DD, maxit, tol);
      result=BiCGSTAB(A,x,b,DD, maxit, tol);
      //result=CG(A,x,b,DD, maxit, tol);
      }
    catch (...)
      {
      _clearfp();                                       // clear any outstanding exceptions
      result=1;
      _asm int 3;
      }
    }

  if (result==0)
    {
    for (dword i=0; i<m_Slv.m_nSize; i++)
      m_Slv.m_Soln[i]=x[i]; 
    }
  FPP_RestoreExceptions(FPP_State);

  return 0; 
  };

//----------------------------------------------------------------------------

void CSparseSolverI::Report_Stats (Mtx_Info *table)
  {
  table->nonzerocount=-1;
  table->num_fill_ins=-1;
  table->sparcity_after=-1;
  table->sparcity_before=-1;
  };

//----------------------------------------------------------------------------

int CSparseSolverI::Errors()
  {
  return 0; 
  };

//----------------------------------------------------------------------------
//
//
//
//----------------------------------------------------------------------------

IMPLEMENT_SPARES(CSparseSolver, 50)
  
CSparseSolver::CSparseSolver(dword n, double ChgRTol, double ChgATol) //: SpM_Mtrx(n)
  {
  m_bDirect =  true;
  m_pSlv = NULL;
  m_nSize = n; 
  m_Soln.SetSize(m_nSize+BUG_EXTRA);
  m_Vect.SetSize(m_nSize+BUG_EXTRA);
  m_bReAnalyse=0;
  m_bStartBuild=1;
  m_bAnalysedOK=0;
  m_iChanges=0;
  m_bRowSetupReqd=true;

  m_dChgRTol=ChgRTol;
  m_dChgATol=ChgATol;
  if (m_dChgRTol < 1.0)
    m_dChgRTol=1.0/Range(1.0e-3, m_dChgRTol, 1.0);
  else
    m_dChgRTol=Range(1.0, m_dChgRTol, 1.0e3);
  
  m_RPSymolic=true;
  m_RPWide=10;
  m_RPPrec=2;
  };

//----------------------------------------------------------------------------

CSparseSolver::~CSparseSolver() 
  { 
  for (int i=0; i<m_Rows.GetSize(); i++)
    delete m_Rows[i];
  delete m_pSlv;
  //if (m_Soln) 
  //  delete[] m_Soln; 
  //if (m_Vect) 
  //  delete[] m_Vect; 
  };

//----------------------------------------------------------------------------

void CSparseSolver::Clear(dword n, double ChgRTol, double ChgATol)
  {
  m_BuildSeq.RemoveAllToFreeList();
  m_nSize = n; 

  if (m_pSlv)
    m_pSlv->Clear(n);//SpM_Mtrx::Clear(n);

  m_Soln.SetSize(m_nSize+BUG_EXTRA);
  m_Vect.SetSize(m_nSize+BUG_EXTRA);
  for (dword i=0; i<m_nSize; i++)
    {
    m_Soln[i]=0;
    m_Vect[i]=0;
    }

  m_bRowSetupReqd=true;
  m_bReAnalyse=0;
  m_bStartBuild=1;
  m_bAnalysedOK=0;
  m_iChanges=0;

  m_dChgRTol=ChgRTol;
  m_dChgATol=ChgATol;
  if (m_dChgRTol < 1.0)
    m_dChgRTol=1.0/Range(1.0e-3, m_dChgRTol, 1.0);
  else
    m_dChgRTol=Range(1.0, m_dChgRTol, 1.0e3);
  };

//----------------------------------------------------------------------------

flag CSparseSolver::SetValue (dword rowpos, dword colpos, double value)
  {
  #ifdef dbginsert
  dbgpln("Insert Val :[%i,%i] %#11.5g",rowpos,colpos,value);
  #endif
  #ifndef _RELEASE
  if (rowpos<1 || rowpos>m_nSize || colpos<1 || colpos>m_nSize || !Valid(value))
    DoBreak();
  #endif
  if (m_bAnalysedOK)
    {
    if (m_bStartBuild)
      {
      m_bStartBuild=0;
      m_bReAnalyse=0;
      m_iChanges=0;
      m_dBigRatio=0.0;
      m_Pos=m_BuildSeq.GetHeadPosition();
      }
    if (!m_bReAnalyse)
      if (m_Pos!=NULL)
        {
        CBldSeqItem &Ele=m_BuildSeq.GetNext(m_Pos);
        Ele.m_Value=value;
        double ratio = GTZ(fabs(Ele.m_OrigValue/NZ(value)));
        if (Ele.m_Row!=rowpos || Ele.m_Col!=colpos || ratio>m_dChgRTol || ratio<1.0/m_dChgRTol)
          {
          m_iChanges++;
          m_bReAnalyse=1;
          if (ratio > m_dBigRatio)
            {
            m_dBigRatio = ratio;
            m_ProbElement=Ele;
            }
          else if (ratio < 1.0/m_dBigRatio)
            {
            m_dBigRatio = 1.0/ratio;
            m_ProbElement=Ele;
            }
          }
        }
       else
        {
        m_iChanges++;
        m_bReAnalyse=1;
        }
    m_bRowSetupReqd=m_bReAnalyse;
    }
  else 
    {
    /*
    if (m_bStartBuild)
      {
      BuildSeq.RemoveAllToFreeList();
      m_bStartBuild=0;
      }
    */
    CBldSeqItem Ele={rowpos, colpos, value, value};
    m_BuildSeq.AddTail(Ele);
    m_bRowSetupReqd=true;
    }

  //if (m_pSlv)
  //  m_pSlv->SetValue(rowpos,colpos,value);
  return !m_bReAnalyse;
  }

//----------------------------------------------------------------------------

double CSparseSolver::GetValue (dword rowpos, dword colpos)
  {
  #ifndef _RELEASE
  if (rowpos<1 || rowpos>m_nSize || colpos<1 || colpos>m_nSize)
    DoBreak();
  #endif
  return m_pSlv ? m_pSlv->GetValue(rowpos,colpos) : dNAN;
  }

//----------------------------------------------------------------------------

int CSparseSolver::Initialise(flag Direct)
  {
  if (m_pSlv==NULL || (m_bDirect!=Direct))
    {
    m_bDirect=Direct;

    delete m_pSlv;
    if (Direct)
      m_pSlv = new CSparseSolverD(this, m_nSize);
    else
      m_pSlv = new CSparseSolverI(this, m_nSize);
    }
  return m_pSlv->Initialise(Direct);
  }

//----------------------------------------------------------------------------
 
int CSparseSolver::Analyse()
  {
#ifdef WITHTIMING 
  dbgpln("CSparseSolver::Analyse  >>>>>");
  CStopWatch SW(1);
#endif

  //SW.Stop();
  //dbgpln("CSparseSolver::Transfer %10.2f", SW.Secs()*1e6);
  //SW.ReStart();

  m_bAnalysedOK=0;
  m_Errors=m_pSlv->Analyse();
  m_bAnalysedOK=(m_Errors==SMTX_NONE);
  
#ifdef WITHTIMING 
  SW.Stop();
  dbgpln("CSparseSolver::Analyse  %10.2f", SW.Secs()*1e6);
#endif

  return m_Errors;
  }

//----------------------------------------------------------------------------
 
int CSparseSolver::Solve()
  {
#ifdef WITHTIMING 
  dbgpln("CSparseSolver::Solve    >>>>>");
  CStopWatch SW(1);
#endif

  m_Errors=m_pSlv->Solve();
  
#ifdef WITHTIMING 
  SW.Stop();
  dbgpln("CSparseSolver::Solve    %10.2f", SW.Secs()*1e6);
#endif

  m_bStartBuild=1;
  if (m_Errors!=SMTX_NONE)
    {

    dbgpln("Matrix Problem-----------------------------");
    RowPrint(true, 12, 4);
    
    //POSITION Pos=m_BuildSeq.GetHeadPosition();
    //dword lastrow=0;
    //while (Pos)
    //  {
    //  CBldSeqItem &Ele=m_BuildSeq.GetNext(Pos);
    //  while (Ele.m_Row>lastrow+1)
    //    dbgpln("Row %4i - Missing", lastrow++);
    //  dbgpln("%4i, %4i : %14.6g => %14.6g", Ele.m_Row, Ele.m_Col, Ele.m_OrigValue, Ele.m_Value);
    //  lastrow=Ele.m_Row;
    //  }

    dbgpln("Matrix Problem-----------------------------");
    }

  return m_Errors;
  }

//----------------------------------------------------------------------------

static int PosTest1(void * p, void * q) 
  { 
  CBldSeqItem* r1=(CBldSeqItem*)p;
  CBldSeqItem* r2=(CBldSeqItem*)q;
  ASSERT(r1->m_Row==r2->m_Row);
  if (r1->m_Row==r2->m_Row && r1->m_Col<=r2->m_Col)
    return true;
  return false; 
  }

void CSparseSolver::SetupRows()
  {
  if (m_bRowSetupReqd)
    {
    m_bRowSetupReqd=false;
    int GrowSize=4;
    int newSize=m_nSize+1;
    int oldSize=m_Rows.GetSize();
    for (int i=newSize; i<oldSize; i++)
      delete m_Rows[i];
    m_Rows.SetSize(newSize);
    m_RowPos.SetSize(newSize);
    m_RowPin.SetSize(newSize);
    m_RowCnt.SetSize(newSize);
    for (int i=oldSize; i<newSize; i++)
      {
      m_Rows[i]= new CSSColSeq;
      m_Rows[i]->SetSize(0, GrowSize);
      }

    for (dword r=0; r<=m_nSize; r++)
      m_RowCnt[r]=0;

    POSITION Pos=m_BuildSeq.GetHeadPosition();
    while (Pos)
      {
      CBldSeqItem * pBS=&m_BuildSeq.GetNext(Pos);
      int r=pBS->m_Row;
      m_Rows[r]->SetAtGrow(m_RowCnt[r]++, pBS);
      }
    for (dword r=1; r<=m_nSize; r++)
      {
      m_Rows[r]->SetSize(m_RowCnt[r], GrowSize);
      m_RowPos[r]=r;
      m_RowPin[r]=false;
      }
    
    for (dword r=1; r<=m_nSize; r++)
      if (m_Rows[r]->GetSize())
        HpSort(m_Rows[r]->GetSize(), (void**)&m_Rows[r]->ElementAt(0), PosTest1);
    }
  };

void CSparseSolver::RowPrint (flag Symbolic, int Wide, int Prec, bool Primary)
  {
  if (Primary)
    {
    m_RPSymolic = Symbolic;
    m_RPWide    = Wide;
    m_RPPrec    = Prec;
    SetupRows();
    }
  else
    {
    Symbolic = m_RPSymolic;
    Wide    = m_RPWide;
    Prec    = m_RPPrec;
    }

  /* Ver.1 */
  dword /*i, j,*/ k, kk;
  int MaxRowLen=0;
  //node *row;
  dbglock();
  if (Symbolic)
    {
    //dbgpln("Matrix:-----------------------");
    kk = (m_nSize >= 1000) ? 1000 : (m_nSize >= 100) ? 100 : (m_nSize >= 10) ? 10 : 1;

    //if (m_nSize >= 1000) kk = 1000
    //else if (m_nSize >= 100) kk = 100
    //else if (m_nSize >= 10) kk = 10
    //else kk = 1
    for (; kk >= 1; kk /= 10)
      {
      dbgp("          ");
      for (k = 1; k <= m_nSize; k++)
        if (k%10==0 || kk==1)
          dbgp("%i",(k/kk)%10);
        else
          dbgp(" ");
      dbgpln("");
      }

    for (dword r=1; r<=m_nSize; r++)
      MaxRowLen=Max(m_Rows[r]->GetSize(), MaxRowLen);
    }

  /*    FOR i:= 1 TO Size DO Write(Colarray^[i].NonZeroCount:7); Writeln; */
  //char * buff = new char[m_nSize];
  Strng Buff;
  Buff.SetLength(m_nSize+1);
  for (dword r = 1; r <= m_nSize; r++)
    {
    /*       WriteLn; */
    //row = rowarray[ (i) - 1].next;
    /*       Write(RowArray^[i].NonZeroCount,'       '); */
    if (Symbolic)
      {
      dbgp("%4i[%3i]:",r,m_RowPos[r]);
      int bi=0;
      
      int j=1;
      int index=0;
      for (dword c=1; c<=m_nSize; c++)
        {
        if (index < m_Rows[r]->GetSize() && m_Rows[r]->GetAt(index)->m_Col == c)
          {
          CBldSeqItem &BI=*m_Rows[r]->ElementAt(index);
          if (BI.m_Value > 0.0)
            Buff[bi++]='+';//dbgp("+");
          else if (BI.m_Value < 0.0)
            Buff[bi++]='-';//dbgp("-");
          else
            Buff[bi++]='0';//dbgp("0");
          index++;
          }
        else
          Buff[bi++]=(j%2)==1 ? '.' : ' ';//dbgp((j%2)==1 ? "." : " ");
        j++;
        }
      for (k = j; k <= m_nSize; k++)
        Buff[bi++]=(k%2)==1 ? '.' : ' ';//dbgp((k%2)==1 ? "." : " ");

      Buff[bi++]=0;
      dbgp("%s", Buff());
      dbgp(":",m_Vect[(r)-1]);
      
      int len=0;
      for (index=0; index<m_Rows[r]->GetSize(); index++)
        {
        double d=m_Rows[r]->GetAt(index)->m_Value;
        if (fabs(d)>0.0)
          {
          dbgp(" %#*.*lg",Wide,Prec,d);
          len++;
          }
        }
      while (len++<MaxRowLen)
        dbgp(" %*s",Wide,"");
      dbgp(" : %#*.*lg",Wide,Prec,m_Vect[(r)-1]);
      }  
    else  
      {
#ifdef LKJLKJLKJ
      while (row != NULL)
        {
        if (row->col == j)
          {
          dbgp("%#*.*lg ",Wide,Prec, row->value);
          row = row->right;
          }
        else
          dbgp("%#*.*lg ",Wide,Prec, 0.0);
        j++;
        }
      for (k = j; k <= m_nSize; k++)
        dbgp("%#*.*lg ",Wide,Prec, 0.0);
#endif
      }  
    dbgpln("");
    }

  //delete [] buff;
  dbgunlock();
  }

  //if (m_pSlv)
  //  m_pSlv->RowPrint (Symbolic, Wide, Prec);
  //};

//----------------------------------------------------------------------------
 
void CSparseSolver::Report_Stats(Mtx_Info *table)
  {
  if (m_pSlv)
    m_pSlv->Report_Stats(table);
  }

//----------------------------------------------------------------------------

char *CSparseSolver::ErrorString(int Err)
  {
  switch (Err)
    {
    case SMTX_NONE :                 return "NONE";
    case SMTX_SINGULAR :             return "SINGULAR";
    case SMTX_INSUFFICIENT_MEMORY :  return "INSUFFICIENT_MEMORY";
    case SMTX_INSERT_ERROR :         return "INSERT_ERROR";
    case SMTX_VECTOR_ERROR :         return "VECTOR_ERROR";
    }
  return "UNKNOWN";
  }

//----------------------------------------------------------------------------

void CSparseSolver::SolveFromFile(flag Direct, char * FileName)
  {
  char Buff[8192];
  FILE * File=fopen(FileName, "rt");
  if (!File)
    return ;

  CSparseSolver * SS=NULL;

  int Row=0;
  int Colon1=0;
  int Colon2=0;
  int Colon3=0;
  int NCols=-1;
  bool GotCol[512];
  bool GotRow[512];

  bool Busy=0;
  while (!feof(File))
    {
    fgets(Buff, sizeof(Buff), File); // Skip Conversions
    if (!Busy)
      Busy=(strstr(Buff, " 1:")!=NULL);
    if (Busy)
      {
      Row++;
      if (NCols==-1)
        {
        Colon1=strchr(Buff, ':')-Buff;
        Colon2=strchr(&Buff[Colon1+1], ':')-Buff;
        Colon3=strchr(&Buff[Colon2+1], ':')-Buff;
        NCols=Colon2-Colon1-1;
        SS = new CSparseSolver(NCols, 1000.0);
        for (int i=0; i<=NCols; i++)
          GotCol[i]=GotRow[i]=false;
        }
      char * pCoeff=&Buff[Colon2+1];
      char * pVect=&Buff[Colon3+1];
      for (int i=1; i<=NCols; i++)
        {
        if (Buff[Colon1+i]=='-' || Buff[Colon1+i]=='+')
          {
          pCoeff+=strspn(pCoeff, " ");
          double Val=SafeAtoF(pCoeff);
          GotCol[i]=true;
          GotRow[Row]=true;
          SS->SetValue(Row, i, Val);
          pCoeff+=strcspn(pCoeff, " ");
          }
        }
      double Val=SafeAtoF(pVect);
      SS->SetVector(Row, Val);
      }

    if (Row==NCols)
      break;
//  if (!feof(File))
//      fgets(Buff, m_nSizeof(Buff), File); // Skip Conversions
    }

  SS->RowPrint(true, 12, 5);
  for (int i=1; i<=NCols; i++)
     if (!GotCol[i])
       dbgpln("Col %i Empty", i);
  for (i=1; i<=NCols; i++)
     if (!GotRow[i])
       dbgpln("Row %i Empty", i);

  SS->Initialise(Direct);
  if (SS->Analyse()==SMTX_NONE)
    SS->Solve();

  delete SS;
  fclose(File);

  //static void SolveFromFile(char * FileName);
  };

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------

void CSparseSolver::SetVector(dword rowpos, double value )
  { 
  #ifndef _RELEASE
  if (rowpos<1 || rowpos>m_nSize || !Valid(value))
    DoBreak();
  #endif
  //m_Vect.ExpandTo(m_nSize+BUG_EXTRA);
  #ifdef dbginsert
  dbgpln("Set Vect   :[%i] %#11.5g",rowpos,value);
  #endif
  m_Vect[rowpos-1] = value;
  };

//----------------------------------------------------------------------------

double CSparseSolver::GetVector(dword rowpos)
  { 
  #ifndef _RELEASE
  if (rowpos<1 || rowpos>m_nSize)
    DoBreak();
  #endif
  //m_Vect.ExpandTo(m_nSize+BUG_EXTRA);
  #ifdef dbginsert
  dbgpln("Set Vect   :[%i] %#11.5g",rowpos,value);
  #endif
  return m_Vect[rowpos-1];
  };

//----------------------------------------------------------------------------

double &CSparseSolver::Vector(dword rowpos)
  { 
  //m_Vect.ExpandTo(m_nSize+BUG_EXTRA);
  #ifdef dbginsert
  dbgpln("Set Vect   :[%i] %#11.5g",rowpos,value);
  #endif
  return m_Vect[rowpos-1];
  };

 
