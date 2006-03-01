//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MTX_SOLV_H
#define  __MTX_SOLV_H

#ifndef __MTX_GLBL_H
#include ".\direct\mtx_glbl.h"
#endif  
#ifndef __MTX_ORDR_H
#include ".\direct\mtx_ordr.h"
#endif  
#ifndef __MTX_MTRX_H
#include ".\direct\mtx_mtrx.h"
#endif  
#ifndef __SCDTEMPL_H
#include "scdtempl.h"
#endif  

typedef struct CBldSeqItem
  {
  dword   m_Row, m_Col;
  double  m_Value, m_OrigValue;
  } CBldSeqItem;

class CBuildSeqList : public CSCDList<CBldSeqItem, CBldSeqItem&> {};

class CSparseSolver;  // forward

//--------------------------------------------------------------------------------

class CSparseSolverA 
  {
  public:
    CSparseSolverA(CSparseSolver * pSlv) : m_Slv(*pSlv) {};
    virtual                 ~CSparseSolverA() {};
    virtual void            Clear(dword n) = 0;
    virtual void            SetValue (dword rowpos, dword colpos, double value) = 0;
    virtual double          GetValue (dword rowpos, dword colpos) = 0;

    virtual int             Initialise(flag Direct) = 0;
    virtual int             Analyse () = 0;
    virtual int             Solve () = 0;
    virtual void            Report_Stats (Mtx_Info *table) = 0;
    virtual int             Errors() = 0;

  protected:
    CSparseSolver   & m_Slv;
  };

//--------------------------------------------------------------------------------

class CSparseSolverD : public CSparseSolverA, public SpM_Mtrx
  {
  DEFINE_SPARES(CSparseSolverD)

  public:

    CSparseSolverD(CSparseSolver * pSlv,  dword n);
    ~CSparseSolverD();
    void            Clear(dword n);
    void            SetValue (dword rowpos, dword colpos, double value);
    double          GetValue (dword rowpos, dword colpos);

    int             Initialise(flag Direct);
    int             Analyse ();
    int             Solve ();
    void            Report_Stats (Mtx_Info *table);
    int             Errors();

  private :
    void            GetNextPivot(struct CBldSeqItem *pivot, dword diag);
  };  

//--------------------------------------------------------------------------------

class CSSColSeq : public CArray<CBldSeqItem*,CBldSeqItem*> {};
//class CSPSSColSeq : public CSmartPtrAllocate<CSSColSeq> {};
class CSSRowSeq : public CArray<CSSColSeq*,CSSColSeq*>     {};
                                                        

class CSparseSolverI : public CSparseSolverA
  {
  DEFINE_SPARES(CSparseSolverI)

  public:

    CSparseSolverI(CSparseSolver * pSlv,  dword n);
    ~CSparseSolverI();
    void            Clear(dword n);
    void            SetValue (dword rowpos, dword colpos, double value);
    double          GetValue (dword rowpos, dword colpos);

    int             Initialise(flag Direct);
    int             Analyse ();
    int             Solve ();
    void            Report_Stats (Mtx_Info *table);
    int             Errors();

  private :
    void            SwapRows(int i, int j, LPCTSTR Desc = "");
    flag            SwapRows(CSSRowSeq & Rows, CArray<bool, bool> & Pinned, int rhi, int rlo, int rtop, int rend);

    CArray<double, double> m_Val;
    CArray<int, int>  m_RowPtr;
    CArray<int, int>  m_ColInd;

    int               m_iError;
  };  

//--------------------------------------------------------------------------------

class CSparseSolver 
  {
  friend class CSparseSolverD;
  friend class CSparseSolverI;

  DEFINE_SPARES(CSparseSolver)

  public:

    CSparseSolver(dword n, double ChgRTol=100.0, double ChgATol=1.0e-3);
    ~CSparseSolver();
    void              Clear(dword n, double ChgRTol=100.0, double ChgATol=1.0e-3);
    //int             Analyse (CDArray & b);

    flag              SetValue (dword rowpos, dword colpos, double value);
    flag              NZValue (dword rowpos, dword colpos);
    double            GetValue (dword rowpos, dword colpos);

    void              SetVector(dword rowpos, double value);
    double            GetVector(dword rowpos);
    double          & Vector(dword rowpos);
    double            Solution(dword rowpos) { return ((m_Soln.GetSize()) ? m_Soln[rowpos-1] : 0.0);};

    flag              ReAnalysisReqd ()     { return m_bReAnalyse; };
    long              Changes()             { return m_iChanges; };
    double            BigRatio()            { return m_dBigRatio; };
    CBldSeqItem     & ProblemElement()      { return m_ProbElement; };

    int               Initialise(flag Direct);
    int               Analyse ();
    int               Solve();
    void              Report_Stats (Mtx_Info *table);

    flag              Direct() { return m_bDirect; };

    void              RowPrint (flag Symbolic, int Wide, int Prec, bool Primary=true);

    static void       SolveFromFile(flag Direct, char * FileName);
    static char     * ErrorString(int Err);

  protected:
    void              SetupRows();
  
  private :
    CSparseSolverA  * m_pSlv;

    dword             m_nSize;
    CDArray           m_Soln;
    CDArray           m_Vect;

    flag              m_bDirect;
    flag              m_bAnalysedOK;
    flag              m_bReAnalyse;
    flag              m_bStartBuild;
    long              m_iChanges;
    int               m_Errors;
    flag              m_bRowSetupReqd;

    double            m_dBigRatio;
    CBldSeqItem       m_ProbElement;
    CBuildSeqList     m_BuildSeq;

    CSSRowSeq         m_Rows;
    CArray<int,int>   m_RowCnt;
    CArray<int,int>   m_RowPos;
    CArray<bool,bool> m_RowPin;

    POSITION          m_Pos;

    double            m_dChgRTol;
    double            m_dChgATol;

    flag              m_RPSymolic;
    int               m_RPWide;
    int               m_RPPrec;
  };  

//--------------------------------------------------------------------------------

#endif

