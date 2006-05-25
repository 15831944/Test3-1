//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994

#ifndef  __HSTVECT_H
#define  __HSTVECT_H

#if !ForSysCAD
#include "float.h"
#include "math.h"

#ifdef __HSTVECT_CPP
  #define DllImportExport DllExport
#elif !defined(SCHIST)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


inline void dbgpln(char * fmt, ...) {};
inline void dbgp(char * fmt, ...) {};

inline const char * FindWinError(DWORD Code) { return ""; };

inline flag Valid(float a) { return !_isnan(a); };
inline flag Valid(double a) { return !_isnan(a); };
const double ZeroLimit = 1.0e-30;//the numerical limit below which all numbers are considered to be zero
inline float  fNAN(void) { return (float)0xffffffffffffffff;; };
inline double dNAN(void) { return (double)0xffffffffffffffff; };
inline double ChkZero(double x) { return ((fabs(x) > ZeroLimit) ? x : 0.0);}
inline double GTZ(double x=0.0) { return (x > ZeroLimit) ? x : ZeroLimit;}
inline double LTZ(double x=0.0) { return (x < -ZeroLimit) ? x : -ZeroLimit;}
inline double NZ(double x=0.0) { return (x >= 0.0) ? GTZ(x) : LTZ(x);}

inline flag ConvergedVV(double V1, double V2, double ATol, double RTol, double & Err)
  {
  const double Vt = Max(fabs(V1), fabs(V2));
  Err = fabs(V2-V1) / (ATol + Vt * RTol);
  return (Err < 1.0);
  }


int  LogNote(char * Where, UINT BoxFlags, char * fmt, ...);
int  LogError(char * Where, UINT BoxFlags, char * fmt, ...);
int  LogWarning(char * Where, UINT BoxFlags, char * fmt, ...);

char* SecstoHMS(double t, char* buff, flag TruncDate=false);
char* SecstoHMSDate(double time_in, char* buff, flag WithDate=true);
char* SecstoHMSDate(CTimeValue time_in, char* buff, flag WithDate=true);

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
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

    void SetFindName() { bTest4Name=true; };
    flag GotName() { return bGotName; };
    char *VectorName() { return cVectorName; };
  };

// -------------------------------------------------------------------------
/*#C:This class is used for a vector of doubles. ie a variable length array of
doubles that has a number of useful manipulation methods. The internal management 
is made as fast as possible at the cost of using more memory. In the debug version
index ranges and valid vector operations are checked. In the release version invalid 
operations will cause fatal errors.*/
class DllImportExport CDVector
  {
  friend class CDMatrix;
  public:
    CDVector( long NewLen,
              double InitVal = 0.0);
    CDVector();
    CDVector(const CDVector &v);
    ~CDVector();
    
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
    flag Load(rCVMLoadHelper H);
    flag operator==(const CDVector &v);
    CDVector& operator=(const CDVector &v);
    double Normalise();
    double Sum();
    void dbgDump( char* Desc = "",
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

    double* m_d;           //Pointer to array of doubles
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
    void dbgDump( char* Desc = "",
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
                        flag Retain = false);
    //flag GaussInvert();
    flag Load(rCVMLoadHelper H);
    CDMatrix& operator=(const CDMatrix &m);
    flag operator==(const CDMatrix &m);
    void dbgDump( char* Desc = "");
    
    //inline methods...
    /*#F:#R:A row array within the matrix as specified by the row index value.*/
    double* operator[](long i) const //row index
      { ASSERT(i>=0 && i<m_Rows); return m_d[i]; };
    //double operator()(long i,long j) const { ASSERT(i>=0 && i<Len && j>=0 && j<Wid); return d[i][j]; };
    /*#F:#R:An element within the matrix as specified by the index values.*/
    double& operator()( long i, //row index
                        long j) //col index
      { ASSERT(i>=0 && i<m_Rows && j>=0 && j<m_Cols); return m_d[i][j]; };
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
    void dbgDump( char* Desc = "");
    
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

#endif
