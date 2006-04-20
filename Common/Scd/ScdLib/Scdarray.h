//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __SCDARRAY_H
#define  __SCDARRAY_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  

#ifndef __AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif

#ifdef __SCDARRAY_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CDArray : public CArray <double, double>
  {
  protected:
    //byte iCnvDC;           // ConversionGroup
    //Strng sCnvTxt;         // Conversion
  public:
    CDArray() {};//iCnvDC=0;};
    CDArray(const CDArray & Other);
    CDArray(const int Size, double Init=0.0);

    void Extend(int n=1) { SetSize(GetSize()+n, m_nGrowBy); };
    void ExpandTo(int n) { if (GetSize()<n) { SetSize(n, m_nGrowBy); for (int i=0; i<n; i++) m_pData[i]=0; }; };

    //flag SetCnvs(byte CnvDC, char * CnvTxt);

    double ScanMax(int SkipStart=0, int SkipEnd=0);
    double ScanMin(int SkipStart=0, int SkipEnd=0);
    flag Swop(long i1, long i2);
    flag Sort(flag Ascending = TRUE);
    flag Reverse();
    flag SetAll(double d = 0.0);
    flag OpAdd(double d);
    flag OpAdd(CDArray &v, double Scl=1.0, double Off=0.0);
    flag OpSub(CDArray &v);
    flag OpMult(double d);
    flag OpMult(CDArray &v);
    flag OpDiv(CDArray &v);
    //flag Load(rCVMLoadHelper H, byte CnvDC=DC_);
    flag operator==(const CDArray &v);
    CDArray& operator=(const CDArray &v);
    CDArray& operator+=(const CDArray &v);
    CDArray& operator-=(const CDArray &v);
    CDArray& operator*=(const CDArray &v);
    CDArray& operator/=(const CDArray &v);
    CDArray& operator+=(const double v);
    CDArray& operator-=(const double v);
    CDArray& operator*=(const double v);
    CDArray& operator/=(const double v);
    double Normalise();
    double Sum();
    double ToCumulative(double ConstInteg=0.0, double Total=1.0, flag Inverse=false, flag Shift=false);
    //double Interpolate(CDArrayWork & Wrk, double XValue);
    double LinearInterpolate(double XValue, CDArray &X, flag Extrapolate);
    
    void dbgDump( pchar Desc = "",
                  flag Horizontal = TRUE);

    //inline methods...
    /*#F:#R:An element in an array as specified by the index value.*/
    //double& operator[](long i) //Zero based index
    //  { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    //double operator[](long i) const { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    /*#F:#R:Length of the array (ie number of elements).*/
    //long GetLen() { return m_Len; };
    /*#F:#R:The last error code. Only valid if the previous vector operation returns False.*/
    int GetErr() { return m_Err; };

    //double Human(int i) { return Cnvs[iCnvDC]->Human(m_pData[i], False, False, sCnvTxt()); };
    //void SetNormal(int i, double d) { m_pData[i]= Cnvs[iCnvDC]->Normal(d, False, False, sCnvTxt()); };

    flag AdjustCumulativeEnd(double RqdCum, double ValMin, double ValMax, double & Err);
    flag AdjustCumulativeEnd(double RqdCum, double ValMin, double ValMax);
    //double* m_d;           //Pointer to array of doubles
  protected:
    //void FreePts();
    //long m_Len;            //Current length of array
    static int m_Err;   //Last error code only valid after a function returned False.
  };

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CDMatrx : public CArray <CDArray, CDArray&>
  {
  public:
    void SetSizes(int n, int m, int GrowBy=-1) { SetSize(n, -1); for (int i=0 ;i<n; i++) m_pData[i].SetSize(m, GrowBy); };
    //flag SetCnvs(byte CnvDC, char * CnvTxt);

    double ScanMax();
    double ScanMin();
    //flag Swop(long i1, long i2);
    //flag Sort(flag Ascending = TRUE);
    //flag Reverse();
    //flag SetAll(double d = 0.0);
    //flag OpAdd(double d);
    //flag OpAdd(CDArray &v);
    //flag OpSub(CDArray &v);
    //flag OpMult(double d);
    //flag OpMult(CDArray &v);
    //flag OpDiv(CDArray &v);
    //flag Load(rCVMLoadHelper H, byte CnvDC=DC_);
    flag operator==(const CDMatrx &v);
    CDMatrx & operator=(const CDMatrx &v);
    //double Normalise();
    //double Sum();
    //void dbgDump( pchar Desc = "",
    //              flag Horizontal = TRUE);

    //inline methods...
    /*#F:#R:An element in an array as specified by the index value.*/
    //double& operator[](long i) //Zero based index
    //  { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    //double operator[](long i) const { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    /*#F:#R:Length of the array (ie number of elements).*/
    //long GetLen() { return m_Len; };
    /*#F:#R:The last error code. Only valid if the previous vector operation returns False.*/
    //int GetErr() { return m_Err; };
    //
    //double Human(int i) { return Cnvs[iCnvDC]->Human(m_pData[i], False, False, sCnvTxt()); };
    //void SetNormal(int i, double d) { m_pData[i]= Cnvs[iCnvDC]->Normal(d, False, False, sCnvTxt()); };

    //double* m_d;           //Pointer to array of doubles
    //byte iCnvDC;           // ConversionGroup
    //Strng sCnvTxt;         // Conversion
  protected:
    //void FreePts();
    //long m_Len;            //Current length of array
    //static int m_Err;   //Last error code only valid after a function returned False.
  };

// ==========================================================================
//
//
//
// ==========================================================================

//const int CDW_Linear=0;
//const int CDW_TSpline=1;
//
//class CDArray;
//
//class CDArrayWork
//  {
//  protected:
//    int iMethod;
//    CDArray * pX;
//  public:
//    CDArrayWork() {iMethod=CDW_Linear; pX=NULL;};
//    CDArrayWork(int Method, CDArray & X) { pX=&X; iMethod=Method; };
//    void SetXArray(CDArray & X) { pX=&X; };
//    void SetMethod(int Method) { iMethod=Method; };
//  };

class DllImportExport CIArray : public CArray <int, int>
  {
  protected:
    //byte iCnvDC;           // ConversionGroup
    //Strng sCnvTxt;         // Conversion
  public:
    CIArray() {};//iCnvDC=0;};
    CIArray(const CIArray & Other);

    void Extend(int n=1) { SetSize(GetSize()+n, m_nGrowBy); };

    //flag SetCnvs(byte CnvDC, char * CnvTxt);

    int ScanMax(int SkipStart=0, int SkipEnd=0);
    int ScanMin(int SkipStart=0, int SkipEnd=0);
    flag Swop(long i1, long i2);
    flag Sort(flag Ascending = TRUE);
    flag Reverse();
    flag SetAll(int d = 0);
    flag OpAdd(int d);
    flag OpAdd(CIArray &v, int Scl=1, int Off=0);
    flag OpSub(CIArray &v);
    flag OpMult(int d);
    flag OpMult(CIArray &v);
    flag OpDiv(CIArray &v);
    //flag Load(rCVMLoadHelper H, byte CnvDC=DC_);
    flag operator==(const CIArray &v);
    CIArray& operator=(const CIArray &v);
    //int Normalise();
    int Sum();
    //int ToCumulative(double ConstInteg=0.0, double Total=1.0);
    //double Interpolate(CIArrayWork & Wrk, double XValue);
    //int LinearInterpolate(double XValue, CIArray &X, flag Extrapolate);
    
    void dbgDump( pchar Desc = "",
                  flag Horizontal = TRUE);

    //inline methods...
    /*#F:#R:An element in an array as specified by the index value.*/
    //double& operator[](long i) //Zero based index
    //  { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    //double operator[](long i) const { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    /*#F:#R:Length of the array (ie number of elements).*/
    //long GetLen() { return m_Len; };
    /*#F:#R:The last error code. Only valid if the previous vector operation returns False.*/
    int GetErr() { return m_Err; };

    //double Human(int i) { return Cnvs[iCnvDC]->Human(m_pData[i], False, False, sCnvTxt()); };
    //void SetNormal(int i, double d) { m_pData[i]= Cnvs[iCnvDC]->Normal(d, False, False, sCnvTxt()); };

    //void AdjustCumulativeEnd(double RqdCum, double ValMin, double ValMax);
    //double* m_d;           //Pointer to array of doubles
  protected:
    //void FreePts();
    //long m_Len;            //Current length of array
    static int m_Err;   //Last error code only valid after a function returned False.
  };


// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CLArray : public CArray <long, long>
  {
  };

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CIMatrx : public CArray <CIArray, CIArray&>
  {
  public:
    void SetSizes(int n, int m, int GrowBy=-1) { SetSize(n, -1); for (int i=0 ;i<n; i++) m_pData[i].SetSize(m, GrowBy); };
    //flag SetCnvs(byte CnvDC, char * CnvTxt);

    int ScanMax();
    int ScanMin();
    //flag Swop(long i1, long i2);
    //flag Sort(flag Ascending = TRUE);
    //flag Reverse();
    //flag SetAll(double d = 0.0);
    //flag OpAdd(double d);
    //flag OpAdd(CIArray &v);
    //flag OpSub(CIArray &v);
    //flag OpMult(double d);
    //flag OpMult(CIArray &v);
    //flag OpDiv(CIArray &v);
    //flag Load(rCVMLoadHelper H, byte CnvDC=DC_);
    flag operator==(const CIMatrx &v);
    CIMatrx & operator=(const CIMatrx &v);
    //double Normalise();
    //double Sum();
    //void dbgDump( pchar Desc = "",
    //              flag Horizontal = TRUE);

    //inline methods...
    /*#F:#R:An element in an array as specified by the index value.*/
    //double& operator[](long i) //Zero based index
    //  { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    //double operator[](long i) const { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    /*#F:#R:Length of the array (ie number of elements).*/
    //long GetLen() { return m_Len; };
    /*#F:#R:The last error code. Only valid if the previous vector operation returns False.*/
    //int GetErr() { return m_Err; };
    //
    //double Human(int i) { return Cnvs[iCnvDC]->Human(m_pData[i], False, False, sCnvTxt()); };
    //void SetNormal(int i, double d) { m_pData[i]= Cnvs[iCnvDC]->Normal(d, False, False, sCnvTxt()); };

    //double* m_d;           //Pointer to array of doubles
    //byte iCnvDC;           // ConversionGroup
    //Strng sCnvTxt;         // Conversion
  protected:
    //void FreePts();
    //long m_Len;            //Current length of array
    //static int m_Err;   //Last error code only valid after a function returned False.
  };

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CSArray : public CArray <Strng, Strng&>
  {
  public:
    CSArray() {};
    CSArray(const CSArray &v) { SetSize(v.GetSize()); for (int i=0; i<v.GetSize(); i++) SetAt(i, (Strng &)v[i]); };
    void Extend(int n=1) { SetSize(GetSize()+n, m_nGrowBy); };

    //flag Swop(long i1, long i2);
    //flag Sort(flag Ascending = TRUE);
    //flag Reverse();
    void AddStrng(char * p);
    flag SetAll(char * p= "");
    //flag OpAdd(double d);
    //flag OpAdd(CDArray &v);
    //flag OpSub(CDArray &v);
    //flag OpMult(double d);
    //flag OpMult(CDArray &v);
    //flag OpDiv(CDArray &v);
    //flag Load(rCVMLoadHelper H, byte CnvDC=DC_);
    flag operator==(const CSArray &v);
    CSArray& operator=(const CSArray &v);
    //double Normalise();
    //double Sum();
    //void dbgDump( pchar Desc = "",
    //              flag Horizontal = TRUE);

    int Find(Strng & S, flag CaseSensitive)
      { return Find(S(), CaseSensitive); }
    int Find(const char* s, flag CaseSensitive)
      {
      if (CaseSensitive)
        {
        for (int i=0; i<GetSize(); i++)
          if (strcmp(m_pData[i](), s)==0) 
            return i;
        }
      else
        for (int i=0; i<GetSize(); i++)
          if (_stricmp(m_pData[i](), s)==0) 
            return i;
      return -1;
      }

    //inline methods...
    /*#F:#R:An element in an array as specified by the index value.*/
    //double& operator[](long i) //Zero based index
    //  { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    //double operator[](long i) const { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    /*#F:#R:Length of the array (ie number of elements).*/
    //long GetLen() { return m_Len; };
    /*#F:#R:The last error code. Only valid if the previous vector operation returns False.*/
    //int GetErr() { return m_Err; };
    //
    //double Human(int i) { return Cnvs[iCnvDC]->Human(m_pData[i], False, False, sCnvTxt()); };
    //void SetNormal(int i, double d) { m_pData[i]= Cnvs[iCnvDC]->Normal(d, False, False, sCnvTxt()); };
    //
    ////double* m_d;           //Pointer to array of doubles
    //byte iCnvDC;           // ConversionGroup
    //Strng sCnvTxt;         // Conversion
  protected:
    //void FreePts();
    //long m_Len;            //Current length of array
    //static int m_Err;   //Last error code only valid after a function returned False.
  };

// ==========================================================================
//
//
//
// ==========================================================================

class DllImportExport CSMatrx : public CArray <CSArray, CSArray&>
  {
  public:
    void SetSizes(int n, int m, int GrowBy=-1) { SetSize(n, -1); for (int i=0 ;i<n; i++) m_pData[i].SetSize(m, GrowBy); };
    //flag SetCnvs(byte CnvDC, char * CnvTxt);

    int ScanMax();
    int ScanMin();
    //flag Swop(long i1, long i2);
    //flag Sort(flag Ascending = TRUE);
    //flag Reverse();
    //flag SetAll(double d = 0.0);
    //flag OpAdd(double d);
    //flag OpAdd(CIArray &v);
    //flag OpSub(CIArray &v);
    //flag OpMult(double d);
    //flag OpMult(CIArray &v);
    //flag OpDiv(CIArray &v);
    //flag Load(rCVMLoadHelper H, byte CnvDC=DC_);
    flag operator==(const CSMatrx &v);
    CSMatrx & operator=(const CSMatrx &v);
    //double Normalise();
    //double Sum();
    //void dbgDump( pchar Desc = "",
    //              flag Horizontal = TRUE);

    //inline methods...
    /*#F:#R:An element in an array as specified by the index value.*/
    //double& operator[](long i) //Zero based index
    //  { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    //double operator[](long i) const { ASSERT(i>=0 && i<m_Len); return m_d[i]; };
    /*#F:#R:Length of the array (ie number of elements).*/
    //long GetLen() { return m_Len; };
    /*#F:#R:The last error code. Only valid if the previous vector operation returns False.*/
    //int GetErr() { return m_Err; };
    //
    //double Human(int i) { return Cnvs[iCnvDC]->Human(m_pData[i], False, False, sCnvTxt()); };
    //void SetNormal(int i, double d) { m_pData[i]= Cnvs[iCnvDC]->Normal(d, False, False, sCnvTxt()); };

    //double* m_d;           //Pointer to array of doubles
    //byte iCnvDC;           // ConversionGroup
    //Strng sCnvTxt;         // Conversion
  protected:
    //void FreePts();
    //long m_Len;            //Current length of array
    //static int m_Err;   //Last error code only valid after a function returned False.
  };

// ==========================================================================
//
//
//
// ==========================================================================


#undef DllImportExport

#endif