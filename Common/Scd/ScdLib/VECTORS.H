//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __VECTORS_H
#define  __VECTORS_H

#ifndef __SC_DEFS_H
#include "sc_defs.h"
#endif  
#ifndef __DATACNVS_H
#include "datacnvs.h"
#endif  

#ifdef __VECTORS_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// -------------------------------------------------------------------------
//vector/matrix error list ...
const int VMErrSingular = 1001;
const int VMErrFileOpen = 1002;
const int VMErrLocation = 1003;

_FWDDEF(CDVector);
_FWDDEF(CLVector);
_FWDDEF(CDMatrix);
_FWDDEF(CLMatrix);

// -------------------------------------------------------------------------
//constants from REG.H KCG
/*const double  ad_C0 =2.515517;
const double  ad_C1 =.802853;
const double  ad_C2 =.010328;
const double  ad_D1 =1.432788;
const double  ad_D2 =.189269;
const double  ad_D3 =.001308;
const double  af_Stp =2.50662827465;
const double  af_Half =0.5;
const double  af_One =1.0;
const double  af_FpF =5.5;
const double  ag_ItMax =100;
const double  ag_Eps =3.0E-7;
const double  al_Tiny = 1e-20;
const double  ap_Pert =0.0001;
const double  ar_Perturb =1e-5;
const double  as_Alpha =0.0001;
const double  as_h =0.000001;*/

// -------------------------------------------------------------------------
_FWDDEF(CVMLoadHelper);
/*#C:This helper class is used by the Load method of the vector and matrix classes. It sets 
a number of default options which may be altered. The options are used to determine how values
are read from a file (typically a comma delimited spreadsheet) into the vector or matrix.*/
class DllImportExport CVMLoadHelper
  {
  public:
    CVMLoadHelper();
    char FName[256];   //Filename
    char NullStr[128]; //Value to be used if there is a null, default = "0"
    flag Vertical;     //default = True
    char Seperator;    //default = ','
    long StartRow;     //default = 1
    long StartCol;     //default = 1
    long Rows;         //default = 0 ie calculate the length/no of rows
    long Cols;         //default = 0 ie calculate the no of columns, ONLY meaningful for matrices
    flag IgnoreComments; //default = True
    flag bTest4Name;   //default = 0
    flag bGotName;     //default = 0
    char cVectorName[128]; // Column Name
    int  iNameColRow;
    flag bTest4Cnv;    //default = 0
    flag bGotCnv;      //default = 0
    char cCnvTxt[128]; // Conversion Text
    int  iCnvColRow;

    void SetFindName() { bTest4Name=True; };
    flag GotName() { return bGotName; };
    char *VectorName() { return cVectorName; };

    void SetFindCnv() { bTest4Cnv=True; };
    flag GotCnv() { return bGotCnv; };
    char *CnvTxt() { return cCnvTxt; };

  };

// -------------------------------------------------------------------------
/*#C:This class is used for a vector of doubles. ie a variable length array of
doubles that has a number of useful manipulation methods. The internal management 
is made as fast as possible at the cost of using more memory. In the debug version
index ranges and valid vector operations are checked. In the release version invalid 
operations will cause fatal errors.*/

struct CDblVectorDbgArray
  {
  double DA[DEBUGARRAYLEN]; // For debugging - included for visibilty
  };

class DllImportExport CDVector
  {
  friend class CDMatrix;
  public:
    CDVector( long NewLen,
              double InitVal = 0.0);
    CDVector();
    CDVector(const CDVector &v);
    ~CDVector();
    
    flag SetCnvs(CCnvIndex CnvDC, char * CnvTxt);
    //pCDataCnv Cnv() { return Cnvs[iCnvDC]; };

    double ScanMax();
    double ScanMin();
    flag SetSize( long NewLen,
                  double InitVal = 0.0);
    flag RemoveAt(long i, int nCount = 1);
    flag Swop(long i1, long i2);
    flag Sort(flag Ascending = TRUE);
    flag Reverse();
    flag SetAll(double d = 0.0);
    flag Add(double d);
    flag Add(CDVector &v);
    flag Sub(CDVector &v);
    flag Mult(double d);
    flag Mult(CDVector &v);
    flag Div(CDVector &v);
    flag Load(rCVMLoadHelper H, CCnvIndex CnvDC=DC_);
    flag Save(rCVMLoadHelper H);
    flag CopyToClipboard();
    flag operator==(const CDVector &v);
    CDVector& operator=(const CDVector &v);
    double Normalise();
    double Sum();
    void dbgDump( pchar Desc = "",
                  flag Horizontal = TRUE);

    //inline methods...
    /*#F:#R:An element in an array as specified by the index value.*/
    double& operator[](long i) //Zero based index
      { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    double operator[](long i) const { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    /*#F:#R:Length of the array (ie number of elements).*/
    long GetLen() { return m_Len; };
    /*#F:#R:The last error code. Only valid if the previous vector operation returns False.*/
    int GetErr() { return m_Err; };

    double Human(int i) { return Cnvs[(iCnvDC)]->Human(m_d[i], sCnvTxt()); };
    void SetNormal(int i, double d) { m_d[i]= Cnvs[(iCnvDC)]->Normal(d, sCnvTxt()); };

    union 
      {
      double* m_d;                      //Pointer to array of doubles
      double (*m_dbg)[DEBUGARRAYLEN];   // For debugging - included for visibilty
      };
    CCnvIndex iCnvDC;           // ConversionGroup
    Strng sCnvTxt;         // Conversion
  protected:
    void FreePts();
    long m_Len;            //Current length of array
    static int m_Err;   //Last error code only valid after a function returned False.
  };

// -------------------------------------------------------------------------
/*#C:This class is used for a vector of integers (long). ie a variable length array of
longs that has a number of useful manipulation methods. The internal management 
is made as fast as possible at the cost of using more memory. In the debug version
index ranges and valid vector operations are checked. In the release version invalid 
operations will cause fatal errors.#S:CDVector*/
class DllImportExport CLVector
  {
  friend class CDMatrix;
  friend class CLMatrix;
  public:
    CLVector( long NewLen,
              long InitVal = 0);
    CLVector();
    CLVector(const CLVector &v);
    ~CLVector();
    long ScanMax();
    long ScanMin();
    flag SetSize( long NewLen,
                  long InitVal = 0);
    flag RemoveAt(long i, int nCount = 1);
    flag Swop(long i1, long i2);
    flag Sort(flag Ascending = TRUE);
    flag Reverse();
    flag SetAll(long d = 0);
    flag Add(long d);
    flag Add(CLVector &v);
    flag Sub(CLVector &v);
    flag Mult(long d);
    flag Mult(CLVector &v);
    flag Div(CLVector &v);
    flag Load(rCVMLoadHelper H);
    flag operator==(const CLVector &v);
    CLVector& operator=(const CLVector &v);
    void dbgDump( pchar Desc = "",
                  flag Horizontal = TRUE);

    //inline methods...
    long& operator[](long i)
      { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    long operator[](long i) const { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    long GetLen() { return m_Len; };
    int GetErr() { return m_Err; };

    long* m_d;           //Pointer to array of longs
  protected:
    void FreePts();
    long m_Len;          //Current length of array
    static int m_Err;   //Last error code only valid after a function returned False.
  };

// -------------------------------------------------------------------------
/*#C:This class is used as a two dimensional matrix of doubles. ie a variable length 
two dimensional array of doubles that has a number of useful manipulation methods. 
The internal management is made as fast as possible at the cost of using more memory.
In the debug version index ranges and valid matrix operations are checked. In the 
release version invalid operations will cause fatal errors.*/
class DllImportExport CDMatrix
  {
  friend class CBaseFn; //allow xy functions direct access to the data
  friend class CZXYFn;  //allow xy functions direct access to the data
  friend class NXYFn;  //allow xy functions direct access to the data
  friend class CXYFn;  //allow xy functions direct access to the data
  friend class GCMatrix;
  public:
    CDMatrix( long NewRows,
              long NewCols,
              double InitVal = 0.0);
    CDMatrix();
    CDMatrix( const CDMatrix &m);
    ~CDMatrix();
    double ScanMax();
    double ScanMin();
    double ScanRowMax(long j);
    double ScanRowMin(long j);
    flag SetSize( long NewRows,
                  long NewCols,
                  double InitVal = 0.0);
    flag SetAll(double d = 0.0);
    flag SetRow(long j, double d = 0.0);
    flag SetCol(long i, double d = 0.0);
    flag SetRow(long j, const CDVector &v);
    flag SetCol(long i, const CDVector &v);
    flag GetRow(long i, CDVector &v);
    flag GetCol(long i, CDVector &v);
    flag SwopRows(long j1, long j2);
    flag SwopCols(long i1, long i2);
    flag SortRows(long j1, flag Ascending = TRUE, long j2 = -1, flag Ascending2 = TRUE); //bga
    flag SortCols(long i1, flag Ascending = TRUE, long i2 = -1, flag Ascending2 = TRUE); //bga
    flag Add(double d);
    flag Add(CDMatrix &m);
    flag Sub(CDMatrix &m);
    flag Mult(double d);
    flag Mult(CDMatrix &m);
    flag Div(CDMatrix &m);
    flag Transpose();
    flag MakeIntoIdentity();
    flag LUDecompose( CLVector &Index,
                      int &D);
    flag LUBackSub( const CLVector &Index,
                    CDVector &B);
    flag LUInvert();
    flag LUDeterminant( double &Det,
                        flag Retain = False);
    //flag GaussInvert();
    flag Load(rCVMLoadHelper H, dword BlankFlag=0);
    flag Save(rCVMLoadHelper H);
    flag CopyToClipboard();
    CDMatrix& operator=(const CDMatrix &m);
    flag operator==(const CDMatrix &m);
    void dbgDump( pchar Desc = "");
    
    //inline methods...
    /*#F:#R:A row array within the matrix as specified by the row index value.*/
    double* operator[](long i) const //row index
      { ASSERT(i>=0 && i<m_Rows); return m_d[i]; };
    //double operator()(long i,long j) const { ASSERT(i>=0 && i<Len && j>=0 && j<Wid); return d[i][j]; };
    /*#F:#R:An element within the matrix as specified by the index values.*/
    double& operator()( long i, //row index
                        long j) //col index
      { ASSERT(i>=0 && i<m_Rows && j>=0 && j<m_Cols); return m_d[i][j]; };
    double GetAt(long i, long j)
      { ASSERT(i>=0 && i<m_Rows && j>=0 && j<m_Cols); return m_d[i][j]; };
    void SetAt(long i, long j, double d)
      { ASSERT(i>=0 && i<m_Rows && j>=0 && j<m_Cols); m_d[i][j] = d; };
    /*#F:#R:The number of rows.*/
    long GetRows() { return m_Rows; };
    /*#F:#R:The number of columns.*/
    long GetCols() { return m_Cols; };
    /*#F:#R:The last error code. Only valid if the previous matrix operation returns False.*/
    int GetErr() { return m_Err; };
  protected:
    void FreePts();

    static int m_Err; //Last error code only valid after a function returned False.
    long m_Rows;   //Number of rows
    long m_Cols;   //Number of columns
    double** m_d; //Pointer to array of arrays of doubles (matrix)
  };

// -------------------------------------------------------------------------
/*#C:This class is used as a two dimensional matrix of longs. ie a variable length 
two dimensional array of longs that has a number of useful manipulation methods. 
The internal management is made as fast as possible at the cost of using more memory.
In the debug version index ranges and valid matrix operations are checked. In the 
release version invalid operations will cause fatal errors.#S:CDMatrix*/
class DllImportExport CLMatrix
  {
  public:
    CLMatrix( long NewRows,
            long NewCols,
            long InitVal = 0);
  CLMatrix();
  CLMatrix( const CLMatrix &m);
  ~CLMatrix();
  long ScanMax();
  long ScanMin();
  long ScanRowMax(long j);
  long ScanRowMin(long j);
  flag SetSize( long NewRows,
                long NewCols,
                long InitVal = 0);
  flag SetAll(long d = 0);
  flag SetRow(long j, long d = 0);
  flag SetCol(long i, long d = 0);
  flag SetRow(long j, const CLVector &v);
  flag SetCol(long i, const CLVector &v);
  flag GetRow(long i, CLVector &v);
  flag GetCol(long i, CLVector &v);
  flag SwopRows(long j1, long j2);
  flag SwopCols(long i1, long i2);
  flag SortRows(long j1, flag Ascending = TRUE, long j2 = -1, flag Ascending2 = TRUE); //bga
  flag SortCols(long i1, flag Ascending = TRUE, long i2 = -1, flag Ascending2 = TRUE); //bga
  flag Add(long d);
  flag Add(CLMatrix &m);
  flag Sub(CLMatrix &m);
  flag Mult(long d);
  flag Mult(CLMatrix &m);
  flag Div(CLMatrix &m);
  flag Transpose();
  flag MakeIntoIdentity();
  CLMatrix& operator=(const CLMatrix &m);
    flag operator==(const CLMatrix &m);
    void dbgDump( pchar Desc = "");
    
    //inline methods...
  long* operator[](long i) const //row index
    { ASSERT(i>=0 && i<m_Rows); return m_d[i]; };
  //long operator()(long i,long j) const { ASSERT(i>=0 && i<Len && j>=0 && j<Wid); return d[i][j]; };
  long& operator()( long i, //row index
                      long j) //col index
    { ASSERT(i>=0 && i<m_Rows && j>=0 && j<m_Cols); return m_d[i][j]; };
  long GetRows() { return m_Rows; };
  long GetCols() { return m_Cols; };
  int GetErr() { return m_Err; };
  protected:
    void FreePts();

  static int m_Err; //Last error code only valid after a function returned False.
  long m_Rows;   //Number of rows
  long m_Cols;   //Number of columns
  long** m_d; //Pointer to array of arrays of longs (matrix)
  };

// -------------------------------------------------------------------------

#undef DllImportExport 

#endif
