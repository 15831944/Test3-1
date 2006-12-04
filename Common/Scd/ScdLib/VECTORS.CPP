//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
#include "stdafx.h"

#define __VECTORS_CPP

#include "gpfuncs.h"
#include "tknparse.h"
#include "errorlog.h"
#include "vectors.h"

// =========================================================================
/*#F:Constructor sets defaults.*/
CVMLoadHelper::CVMLoadHelper()
  {
  FName[0] = 0;
  strcpy(NullStr, "0");
  Vertical = True;
  Seperator = ',';
  StartRow = 1;
  StartCol = 1;
  Rows = 0;
  Cols = 0;
  IgnoreComments = True;
  bTest4Name=0;
  bGotName=0;
  strcpy(cVectorName, "");
  iNameColRow=-1;
  bTest4Cnv=0;
  bGotCnv=0;
  strcpy(cCnvTxt, "");
  iCnvColRow=-1;
  }

#ifdef _DEBUG
#define DO_VECTOR_MEM_TEST 0 //normally 0
#else
#define DO_VECTOR_MEM_TEST 0
#endif

#if DO_VECTOR_MEM_TEST
DbgMemHelper dv(false, 136);
#endif

// =========================================================================
int CDVector::m_Err = 0;
// -------------------------------------------------------------------------
/*#F:Constructor calls SetSize.*/
CDVector::CDVector( long NewLen, //Vector size/length
                    double InitVal) //Optional initial value for elements, default = 0.0
  {
  m_d = NULL;
  m_Len = 0;
  iCnvDC=DC_;
  //sCnvTxt=NULL;
  SetSize(NewLen, InitVal);
  }

// -------------------------------------------------------------------------
/*#F:Constructor initialises an empty array, the vector has size (length) 0.*/
CDVector::CDVector()
  {
  m_d = NULL;
  m_Len = 0;
  iCnvDC=DC_;
  //sCnvTxt;
  }

// -------------------------------------------------------------------------
/*#F:Constructor makes a copy of the vector supplied.*/
CDVector::CDVector(const CDVector &v) //The vector to be copied
  {
  m_d = NULL;
  m_Len = 0;
  iCnvDC=DC_;
  //pCnv=NULL;
  SetSize(v.m_Len);
  for (long i=0; i<m_Len; i++)
    m_d[i] = v.m_d[i];
  }

// -------------------------------------------------------------------------
/*#F:Destructor "frees" all memory.*/
CDVector::~CDVector()
  {
  iCnvDC=DC_;
  //pCnv=NULL;
  FreePts();
  }

// -------------------------------------------------------------------------
/*
double* CDVector::AllocPts(long Len_)
  {
  Len = Len_;
  return new double[Len_];
  }
*/
// -------------------------------------------------------------------------
/*#F:"Frees" all memory and sets length to 0.*/
void CDVector::FreePts()
  {
  if (m_d)
    {
#if DO_VECTOR_MEM_TEST
dv.DumpTest(m_Len*sizeof(double), (void*)m_d, "VectorDelete");
#endif
    delete []m_d;
    m_d = NULL;
    m_Len = 0;
    }
  }

// -------------------------------------------------------------------------

flag CDVector::SetCnvs(CCnvIndex CnvDC, char * CnvTxt)
  {
  iCnvDC=CnvDC;
  //pCnv=NULL;
  sCnvTxt=CnvTxt!=NULL ? CnvTxt : "";
  return (gs_CnvsMngr.FindSecCnv(iCnvDC, CnvTxt)!=NULL);
  }

// -------------------------------------------------------------------------
/*#F:#R:The largest double within the vector.*/
double CDVector::ScanMax()
  {
  double temp = (m_d ? m_d[0] : 0.0);
  for (long i=1; i<m_Len; i++)
    temp=Max(temp,m_d[i]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:#R:The smallest double within the vector.*/
double CDVector::ScanMin()
  {
  double temp = (m_d ? m_d[0] : 0.0);
  for (long i=1; i<m_Len; i++)
    temp=Min(temp,m_d[i]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:Sets the length of the vector (array). If the previous length was greater, 
the array is truncated. If the new length is greater then all new elements are 
set to InitVal.*/
flag CDVector::SetSize( long NewLen,     //New length of the vector.
                        double InitVal)  //Value to initialise all new elements to. (default = 0)
  {
  ASSERT(NewLen>=0); //must have positive length
  if (NewLen==m_Len)
    return True;
  if (NewLen==0)
    {
    FreePts();
    return True;
    }
  double* nd = new double[NewLen];
#if DO_VECTOR_MEM_TEST
dv.AllocTest(NewLen*sizeof(double), (void*)nd, "VectorSetSize");
#endif
  long j = Min(m_Len,NewLen);
  for (long i=0; i<j; i++)
    nd[i] = m_d[i];
  j = (m_Len<NewLen) ? NewLen : 0;
  for (; i<j; i++)
    nd[i] = InitVal;
  FreePts();
  m_d = nd;
  m_Len = NewLen;
  return True;
  }

// -------------------------------------------------------------------------

flag CDVector::RemoveAt(long i, int nCount/*=1*/)
  {
  if (i<0 || i>=m_Len || nCount<1)
    return False;
  nCount = Min(nCount, (int)(m_Len-i));
  for (long k=i; k<i+nCount; k++)
    m_d[k] = m_d[k+nCount];
  SetSize(m_Len-nCount);
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Swops the elements at the specified index values.*/
flag CDVector::Swop( long i1,  //Index value one
                     long i2)  //Index value two
  {
  ASSERT(i1>=0 && i1<m_Len && i2>=0 && i2<m_Len);  //check for valid index range
  double temp = m_d[i1];
  m_d[i1] = m_d[i2];
  m_d[i2] = temp;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sorts all the elements in the vector in ascending or decending sequence.*/
flag CDVector::Sort(flag Ascending)//True if vector is to be sorted from lowest to highest, default = True
  {
  for (long i=1; i<m_Len; i++)
    for (long j=i; j>=1 && (m_d[j]<m_d[j-1]==Ascending); j--)
      {
      double temp = m_d[j-1];
      m_d[j-1] = m_d[j];
      m_d[j] = temp;
      }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Reverses all the elements in the vector.*/
flag CDVector::Reverse()
  {
  if (m_Len>1)
    {
    const long k = div(m_Len,2L).quot;
    for (long i=0; i<k; i++)
      {
      double temp = m_d[i];
      m_d[i] = m_d[m_Len-i-1];
      m_d[m_Len-i-1] = temp;
      }
    }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets all elements to the value specified.*/
flag CDVector::SetAll(double d)
  {
  for (long i=0; i<m_Len; i++)
    m_d[i] = d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Adds the specified value to each element.*/
flag CDVector::Add(double d)
  {
  for (long i=0; i<m_Len; i++)
    m_d[i] += d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Adds the specified vector to itself. Vector sizes MUST match.*/
flag CDVector::Add(CDVector &v)
  {
  ASSERT(m_Len == v.m_Len); //check that lengths are equal
  for (long i=0; i<m_Len; i++)
    m_d[i] += v.m_d[i];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Subtracts the specified vector from itself. Vector sizes MUST match.*/
flag CDVector::Sub(CDVector &v)
  {
  ASSERT(m_Len == v.m_Len); //check that lengths are equal
  for (long i=0; i<m_Len; i++)
    m_d[i] -= v.m_d[i];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Multiplies each element by specified value.*/
flag CDVector::Mult(double d)
  {
  for (long i=0; i<m_Len; i++)
    m_d[i] *= d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Multiplies the specified vector by itself. Vector sizes MUST match.*/
flag CDVector::Mult(CDVector &v)
  {
  ASSERT(m_Len == v.m_Len); //check that lengths are equal
  for (long i=0; i<m_Len; i++)
    m_d[i] *= v.m_d[i];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Divides the specified vector into itself. Vector sizes MUST match.*/
flag CDVector::Div(CDVector &v)
  {
  ASSERT(m_Len == v.m_Len); //check that lengths are equal
  for (long i=0; i<m_Len; i++)
    m_d[i] /= v.m_d[i];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Loads the vector from the specified file.*/
flag CDVector::Load(rCVMLoadHelper H, //Helper class with all the options set
                    CCnvIndex CnvDC)       //conversion
  {
  m_Err = 0;
  H.cVectorName[0]=0;
  H.cCnvTxt[0]=0;
  H.bGotName=False;
  H.bGotCnv=False;

  iCnvDC=CnvDC;
  sCnvTxt="";

  CTokenFile f(AF_All|AF_BackupFiles, H.FName, H.IgnoreComments, False, False);
  Strng s;
  s = H.Seperator;
  s += "\t";
  f.SetSeperators(s());
  if (f.Open())
    {
    SetSize(H.Rows);
    long RowNo = 1;
    while ((RowNo<H.StartRow) && (!f.AtEOF()))
      {
      f.GetNextLine();
      RowNo++;
      }
    if (f.AtEOF())
      {
      m_Err = VMErrLocation;
      return False;
      }
    long DoLen = H.Rows;
    if (DoLen==0)
      DoLen = LONG_MAX;
    double NullVal = atof(H.NullStr);
    long j = 0;
    if (H.Vertical)
      {
      flag Done = False;
      while ((j<DoLen) && (!Done) && (!f.AtEOF()))
        {
        long i = 1;
        while ((i<H.StartCol) && (!Done))//(!f.AtEOF()) && (!f.AtLastToken()))
          {
          s = f.NextToken();
          if (!f.AtLastToken() && s[0]!=H.Seperator)
            f.NextToken();
          i++;
          if (f.AtLastToken() || f.AtEOF())
            Done = True;
          }
        if (!Done)
          {
          s = f.NextToken();
          if (s.Length()==0)
            Done = True;
          else 
            {
            // Check if not Valid # is it a name or a Conversion
            if (s.XStrSpn("+-0123456789e.*")<s.Length())
              if (H.bTest4Name && !H.bGotName)
                H.iNameColRow=RowNo;
              else if (H.bTest4Cnv && !H.bGotCnv)
                H.iCnvColRow=RowNo;

            if (RowNo==H.iNameColRow)
              {
              strcpy(H.cVectorName, s());
              H.bGotName=True;
              }
            else if (RowNo==H.iCnvColRow)
              {
              strcpy(H.cCnvTxt, s());
              sCnvTxt=s();
              if (s.Length()>0)
                {
                pCDataCnv pCnv=gs_CnvsMngr.FindSecCnv(iCnvDC, sCnvTxt());
                if (pCnv==NULL && _stricmp(sCnvTxt(), "(null)")!=0)
                  LogError("Vectors", 0, "Conversion %s not Found", s());
                }
              H.bGotCnv=True;
              }
            else
              {
              if (j>=m_Len)
                SetSize(m_Len+16);
              if (s[0]==H.Seperator)
                m_d[j] = NullVal;
              else if (s[0]=='*')
                m_d[j] = dNAN;
              else if (sCnvTxt.Length()>0)
                SetNormal(j, atof(s()));
              else
                m_d[j]=atof(s());
              j++;
              }
            if (!f.AtLastToken())
              f.GetNextLine();
            RowNo++;
            }
          }
        }
      }
    else
      {
      long i = 1;
      while ((i<H.StartCol) && (!f.AtEOF()) && (!f.AtLastToken()))
        {
        s = f.NextToken();
        if (!f.AtLastToken() && s[0]!=H.Seperator)
          f.NextToken();
        i++;
        }
      if (f.AtLastToken() || f.AtEOF())
        {
        m_Err = VMErrLocation;
        return False;
        }
      flag Done = False;
      while ((j<DoLen) && (!Done) && (!f.AtEOF()))
        {
        if (f.AtLastToken() || f.AtEOF())
          Done = True;
        else
          {
          s = f.NextToken();
          if (j>=m_Len)
            SetSize(m_Len+16);
          if (s[0]==H.Seperator)
            m_d[j] = NullVal;
          else
            m_d[j] = atof(s());
          j++;
          if (!f.AtLastToken() && s[0]!=H.Seperator)
            f.NextToken();
          }
        }
      }
    SetSize(j);
    f.Close();
    H.bTest4Name = False;
    H.bTest4Cnv = False;
    return True;
    }
  m_Err = VMErrFileOpen;
  return False;
  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDVector& CDVector::operator=(const CDVector &v)
  {
  SetSize(v.m_Len);
  for (long i=0; i<m_Len; i++)
    m_d[i] = v.m_d[i];
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:#R:True if the vectors are the same length and the elements are equal 
(element difference is less than the constant ZeroLimit).*/
flag CDVector::operator==(const CDVector &v)
  {
  if (m_Len != v.m_Len)
    return False;
  for (long i=0; i<m_Len; i++)
    if (m_d[i] - v.m_d[i] > ZeroLimit)
      return False;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Normalises the vector and returns the original sum of all the elements .*/
double CDVector::Normalise()
  {
  double t, total = 0.0;
  for (long i = 0; i < m_Len; i++)
    total += m_d[i];
  t = NZ(total);
  for (i = 0; i < m_Len; i++)
    m_d[i] /= t;
  return total;
  }

// -------------------------------------------------------------------------
/*#F:Normalises the vector and returns the original sum of all the elements .*/
double CDVector::Sum()
  {
  double total = 0.0;
  for (long i = 0; i < m_Len; i++)
    total += m_d[i];
  return total;
  }

// -------------------------------------------------------------------------
/*#F:Outputs the vector contents (format acording to options) to the debug file.*/
void CDVector::dbgDump( pchar Desc,  //Optional text description, default = ''
                        flag Horizontal) //Optional, default = True
  {
  dbgp("%s:[%d]", Desc, m_Len);
  if (Horizontal)
    {
    for (long i=0; i<Min(50L,m_Len); i++)
      dbgp("%11.3g ", m_d[i]);
    if (m_Len>5)
      dbgp("...");
    dbgpln("");
    }
  else
    {
    dbgpln("");
    for (long i=0; i<m_Len; i++)
      dbgpln("%4d: %11.3g", i, m_d[i]);
    }
  }

// =========================================================================
int CLVector::m_Err = 0;
// -------------------------------------------------------------------------

CLVector::CLVector( long NewLen,
                    long InitVal)
  {
  m_d = NULL;
  m_Len = 0;
  SetSize(NewLen, InitVal);
  }

// -------------------------------------------------------------------------

CLVector::CLVector()
  {
  m_d = NULL;
  m_Len = 0;
  }

// -------------------------------------------------------------------------

CLVector::CLVector(const CLVector &v)
  {
  m_d = NULL;
  m_Len = 0;
  SetSize(v.m_Len);
  for (long i=0; i<m_Len; i++)
    m_d[i] = v.m_d[i];
  }

// -------------------------------------------------------------------------

CLVector::~CLVector()
  {
  FreePts();
  }

// -------------------------------------------------------------------------

void CLVector::FreePts()
  {
  if (m_d)
    {
    delete []m_d;
    m_d = NULL;
    m_Len = 0;
    }
  }

// -------------------------------------------------------------------------

long CLVector::ScanMax()
  {
  long temp = (m_d ? m_d[0] : 0);
  for (long i=1; i<m_Len; i++)
    temp=Max(temp,m_d[i]);
  return temp;
  }

// -------------------------------------------------------------------------

long CLVector::ScanMin()
  {
  long temp = (m_d ? m_d[0] : 0);
  for (long i=1; i<m_Len; i++)
    temp=Min(temp,m_d[i]);
  return temp;
  }

// -------------------------------------------------------------------------

flag CLVector::SetSize( long NewLen,     //New length of the vector.
                        long InitVal)  //Value to initialise all new elements to. (default = 0)
  {
  ASSERT(NewLen>=0); //must have positive length
  if (NewLen==m_Len)
    return True;
  if (NewLen==0)
    {
    FreePts();
    return True;
    }
  long* nd = new long[NewLen];
  long j = Min(m_Len,NewLen);
  for (long i=0; i<j; i++)
    nd[i] = m_d[i];
  j = (m_Len<NewLen) ? NewLen : 0;
  for (; i<j; i++)
    nd[i] = InitVal;
  FreePts();
  m_d = nd;
  m_Len = NewLen;
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::RemoveAt(long i, int nCount/*=1*/)
  {
  if (i<0 || i>=m_Len || nCount<1)
    return False;
  nCount = Min(nCount, (int)(m_Len-i));
  for (long k=i; k<i+nCount; k++)
    m_d[k] = m_d[k+nCount];
  SetSize(m_Len-nCount);
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::Swop( long i1,  //Index value one
                     long i2)  //Index value two
  {
  ASSERT(i1>=0 && i1<m_Len && i2>=0 && i2<m_Len);  //check for valid index range
  long temp = m_d[i1];
  m_d[i1] = m_d[i2];
  m_d[i2] = temp;
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::Sort(flag Ascending)//True if vector is to be sorted from lowest to highest, default = True
  {
  for (long i=1; i<m_Len; i++)
    for (long j=i; j>=1 && (m_d[j]<m_d[j-1]==Ascending); j--)
      {
      long temp = m_d[j-1];
      m_d[j-1] = m_d[j];
      m_d[j] = temp;
      }
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::Reverse()
  {
  if (m_Len>1)
    {
    const long k = div(m_Len,2L).quot;
    for (long i=0; i<k; i++)
      {
      long temp = m_d[i];
      m_d[i] = m_d[m_Len-i-1];
      m_d[m_Len-i-1] = temp;
      }
    }
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::SetAll(long d)
  {
  for (long i=0; i<m_Len; i++)
    m_d[i] = d;
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::Add(long d)
  {
  for (long i=0; i<m_Len; i++)
    m_d[i] += d;
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::Add(CLVector &v)
  {
  ASSERT(m_Len == v.m_Len); //check that lengths are equal
  for (long i=0; i<m_Len; i++)
    m_d[i] += v.m_d[i];
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::Sub(CLVector &v)
  {
  ASSERT(m_Len == v.m_Len); //check that lengths are equal
  for (long i=0; i<m_Len; i++)
    m_d[i] -= v.m_d[i];
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::Mult(long d)
  {
  for (long i=0; i<m_Len; i++)
    m_d[i] *= d;
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::Mult(CLVector &v)
  {
  ASSERT(m_Len == v.m_Len); //check that lengths are equal
  for (long i=0; i<m_Len; i++)
    m_d[i] *= v.m_d[i];
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::Div(CLVector &v)
  {
  ASSERT(m_Len == v.m_Len); //check that lengths are equal
  for (long i=0; i<m_Len; i++)
    m_d[i] /= v.m_d[i];
  return True;
  }

// -------------------------------------------------------------------------

flag CLVector::Load(rCVMLoadHelper H)
  {
  m_Err = 0;
  CTokenFile f(AF_All|AF_BackupFiles, H.FName, H.IgnoreComments, False, False);
  Strng s;
  s = H.Seperator;
  s += " \t";
  f.SetSeperators(s());
  if (f.Open())
    {
    SetSize(H.Rows);
    long i = 1;
    while ((i<H.StartRow) && (!f.AtEOF()))
      {
      f.GetNextLine();
      i++;
      }
    if (f.AtEOF())
      {
      m_Err = VMErrLocation;
      return False;
      }
    long DoLen = H.Rows;
    if (DoLen==0)
      DoLen = LONG_MAX;
    long NullVal = atol(H.NullStr);
    long j = 0;
    if (H.Vertical)
      {
      flag Done = False;
      while ((j<DoLen) && (!Done) && (!f.AtEOF()))
        {
        i = 1;
        while ((i<H.StartCol) && (!Done))//(!f.AtEOF()) && (!f.AtLastToken()))
          {
          s = f.NextToken();
          if (!f.AtLastToken() && s[0]!=H.Seperator)
            f.NextToken();
          i++;
          if (f.AtLastToken() || f.AtEOF())
            Done = True;
          }
        if (!Done)
          {
          s = f.NextToken();
          if (s.Length()==0)
            Done = True;
          else
            {
            if (j>=m_Len)
              SetSize(m_Len+16);
            if (s[0]==H.Seperator)
              m_d[j] = NullVal;
            else
              m_d[j] = atol(s());
            j++;
            if (!f.AtLastToken())
              f.GetNextLine();
            }
          }
        }
      }
    else
      {
      i = 1;
      while ((i<H.StartCol) && (!f.AtEOF()) && (!f.AtLastToken()))
        {
        s = f.NextToken();
        if (!f.AtLastToken() && s[0]!=H.Seperator)
          f.NextToken();
        i++;
        }
      if (f.AtLastToken() || f.AtEOF())
        {
        m_Err = VMErrLocation;
        return False;
        }
      flag Done = False;
      while ((j<DoLen) && (!Done) && (!f.AtEOF()))
        {
        if (f.AtLastToken() || f.AtEOF())
          Done = True;
        else
          {
          s = f.NextToken();
          if (j>=m_Len)
            SetSize(m_Len+16);
          if (s[0]==H.Seperator)
            m_d[j] = NullVal;
          else
            m_d[j] = atol(s());
          j++;
          if (!f.AtLastToken() && s[0]!=H.Seperator)
            f.NextToken();
          }
        }
      }
    SetSize(j);
    f.Close();
    return True;
    }
  m_Err = VMErrFileOpen;
  return False;
  }

// -------------------------------------------------------------------------

CLVector& CLVector::operator=(const CLVector &v)
  {
  SetSize(v.m_Len);
  for (long i=0; i<m_Len; i++)
    m_d[i] = v.m_d[i];
  return *this;
  }

// -------------------------------------------------------------------------

flag CLVector::operator==(const CLVector &v)
  {
  if (m_Len != v.m_Len)
    return False;
  for (long i=0; i<m_Len; i++)
    if (m_d[i] != v.m_d[i])
      return False;
  return True;
  }

// -------------------------------------------------------------------------

void CLVector::dbgDump( pchar Desc,
                        flag Horizontal)
  {
  /*
  Dbg->Ln("%s|Len:%d|", Desc, m_Len);
  if (Horizontal)
    {
    for (long i=0; i<Min(5L,m_Len); i++)
    Dbg->Ln("%8d ", m_d[i]);
    if (m_Len>5)
      Dbg->Ln("...");
    Dbg->Ln("\n");
    }
  else
    for (long i=0; i<m_Len; i++)
      Dbg->Ln("%4d: %8d\n", i, m_d[i]);
  */
  }

// =========================================================================
int CDMatrix::m_Err = 0;
// -------------------------------------------------------------------------
/*#F:Constructor calls SetSize.*/
CDMatrix::CDMatrix( long NewRows, //Number of rows
                    long NewCols, //Number of columns
                    double InitVal) //Optional initial value for elements, default = 0.0
  {
  m_Rows = 0;
  m_Cols = 0;
  m_d = NULL;
  SetSize(NewRows, NewCols, InitVal);
  }

// -------------------------------------------------------------------------
/*#F:Constructor creates empty matrix (Columns and rows = 0).*/
CDMatrix::CDMatrix()
  {
  m_Rows = 0;
  m_Cols = 0;
  m_d = NULL;
  }

// -------------------------------------------------------------------------
/*#F:Copy constructor, creates a copy of the supplied matrix.*/
CDMatrix::CDMatrix(const CDMatrix &m) //Matrix to copy
  {
  m_Rows = 0;
  m_Cols = 0;
  m_d = NULL;
  SetSize(m.m_Rows, m.m_Cols);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] = m.m_d[i][j];
  }

// -------------------------------------------------------------------------
/*#F:Destructor "frees" all memory by calling FreePts.*/
CDMatrix::~CDMatrix()
  {
  FreePts();
  }

// -------------------------------------------------------------------------
/*#F:"Frees" memory and sets size to 0 by 0.*/
void CDMatrix::FreePts()
  {
  if (m_d)
    {
    for (long i=0; i<m_Rows; i++)
      delete []m_d[i];
    delete []m_d;
    m_d = NULL;
    m_Rows = 0;
    m_Cols = 0;
    }
  }

// -------------------------------------------------------------------------
/*#F:#R:The largest element in the matrix.*/
double CDMatrix::ScanMax()
  {
  double temp = (m_d ? m_d[0][0] : 0.0);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      temp=Max(temp,m_d[i][j]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:#R:The smallest element in the matrix.*/
double CDMatrix::ScanMin()
  {
  double temp = (m_d ? m_d[0][0] : 0.0);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      temp=Min(temp,m_d[i][j]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:#R:The largest element in the specified row.*/
double CDMatrix::ScanRowMax(long i)  //row index
  {
  ASSERT(i>=0 && i<m_Rows); //check for valid row
  double temp = (m_d ? m_d[i][0] : 0.0);
  for (long j=1; j<m_Cols; j++)
    temp = Max(temp, m_d[i][j]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:#R:The largest element in the specified row.*/
double CDMatrix::ScanRowMin(long i)  //row index
  {
  ASSERT(i>=0 && i<m_Rows); //check for valid row
  double temp = (m_d ? m_d[i][0] : 0.0);
  for (long j=1; j<m_Cols; j++)
    temp = Min(temp, m_d[i][j]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:Sets the size of the matrix. If NewRows OR NewCols is zero, all elements are 
removed. Alternatively the matrix size is adjusted, new elements are initialised to
InitVal and size decreases causes truncation.*/
flag CDMatrix::SetSize( long NewRows,  //number of rows
                        long NewCols,  //number of columns
                        double InitVal) //Optional initial value for new elements, default = 0
  {
  ASSERT(NewRows>=0 && NewCols>=0);  //must be positive dimensions
  if ((NewRows==m_Rows) && (NewCols==m_Cols))
    return True;
  if ((NewRows==0) || (NewCols==0))
    {
    FreePts();
    return True;
    }
  long i,j;
  long k = Min(m_Cols,NewCols);
  double ** nd = new pdouble[NewRows];
  for (i=0; i<NewRows; i++)
    {
    nd[i] = new double[NewCols];
    if (i<m_Rows)
      {
      for (j=0; j<k; j++)
        nd[i][j] = m_d[i][j];
      for (; j<NewCols; j++)
        nd[i][j] = InitVal;
      }
    else
      for (j=0; j<NewCols; j++)
        nd[i][j] = InitVal;
    }
  FreePts();
  m_d = nd;
  m_Cols = NewCols;
  m_Rows = NewRows;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets all elements of the matrix to the specified value.*/
flag CDMatrix::SetAll(double d)  //Optional new value, default = 0
  {
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] = d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets all elements of the specified column to the specified value.
Column index j must be valid.*/
flag CDMatrix::SetCol( long j,  //column index 
                       double d)  //Optional new value, default = 0
  {
  ASSERT(j>=0 && j<m_Cols); //check for valid column
  for (long i=0; i<m_Rows; i++)
    m_d[i][j] = d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets all elements of the specified row to the specified value.
Row index i must be valid.*/
flag CDMatrix::SetRow( long i,    //row index
                       double d)  //Optional new value, default = 0
  {
  ASSERT(i>=0 && i<m_Rows); //check for valid row
  for (long j=0; j<m_Cols; j++)
    m_d[i][j] = d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets the specified column to the vector values. Column index j must be valid.
The vector length must be the same as the number of rows.*/
flag CDMatrix::SetCol( long j, //column index
                       const CDVector &v)  //input vector
  {
  ASSERT(j>=0 && j<m_Cols); //check for valid column
  ASSERT(m_Rows==v.m_Len); //check that vector len = matrix row count
  for (long i=0; i<m_Rows; i++)
    m_d[i][j] = v.m_d[i];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets the specified row to the vector values. Row index i must be valid.
The vector length must be the same as the number of columns.*/
flag CDMatrix::SetRow( long i, //row index
                       const CDVector &v) //input vector
  {
  ASSERT(i>=0 && i<m_Rows); //check for valid row
  ASSERT(m_Cols==v.m_Len); //check that vector len = matrix column count
  for (long j=0; j<m_Cols; j++)
    m_d[i][j] = v.m_d[j];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:#R:The specified column as a vector. The length of the vector is set to the number of rows.*/
flag CDMatrix::GetCol( long j, //column index
                       CDVector &v) //ouput vector
  {
  ASSERT(j>=0 && j<m_Cols); //check for valid column
  v.SetSize(m_Rows);
  for (long i=0; i<m_Rows; i++)
    v.m_d[i] = m_d[i][j];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:#R:The specified row as a vector. The length of the vector is set to the number of columns.*/
flag CDMatrix::GetRow( long i, //row index
                       CDVector &v) //output vector
  {
  ASSERT(i>=0 && i<m_Rows); //check for valid row
  v.SetSize(m_Cols);
  for (long j=0; j<m_Cols; j++)
    v.m_d[j] = m_d[i][j];//bga
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Swops the specified columns. The column index j1 and j2 must be valid.*/
flag CDMatrix::SwopCols( long j1, //index to column one
                         long j2) //index to column two
  {
  ASSERT(j1>=0 && j1<m_Cols); //check for valid column
  ASSERT(j2>=0 && j2<m_Cols); //check for valid column
  for (long i=0; i<m_Rows; i++)
    {
    double temp = m_d[i][j1];
    m_d[i][j1] = m_d[i][j2];
    m_d[i][j2] = temp;
    }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Swops the specified rows. The row index i1 and i2 must be valid.*/
flag CDMatrix::SwopRows( long i1, //index to row one
                         long i2) //index to row two
  {
  ASSERT(i1>=0 && i1<m_Rows); //check for valid row
  ASSERT(i2>=0 && i2<m_Rows); //check for valid row
  for (long j=0; j<m_Cols; j++)
    {
    double temp = m_d[i1][j];
    m_d[i1][j] = m_d[i2][j];
    m_d[i2][j] = temp;
    }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sorts all the elements in the matrix by row i1 in ascending or decending
sequence. If i2>=0 a secondary sort by a different row is done (if a set of 
values in row i1 are equal)*/
flag CDMatrix::SortRows( long i1, //index to sort row
                         flag Ascending, //sequence
                         long i2, //index to sort 2nd row
                         flag Ascending2) //sequence
  
  {
  ASSERT(i1>=0 && i1<m_Rows); //check for valid row
  ASSERT(i2>=-1 && i2<m_Rows && i1!=i2); //check for valid row
  for (long i=1; i<m_Cols; i++)
    for (long j=i; j>=1 && (m_d[i1][j]<m_d[i1][j-1]==Ascending); j--)
      {
      for (long k=0; k<m_Rows; k++)
        {
        double temp = m_d[k][j-1];
        m_d[k][j-1] = m_d[k][j];
        m_d[k][j] = temp;
        }
      }
  if ((i2<0)||(i1==i2))
    return True;
  long lCount = 0;
  while (lCount<m_Cols)
    {
    long lTcount = 1;
    while (lTcount+lCount<m_Cols && m_d[i1][lTcount+lCount]==m_d[i1][lTcount+lCount-1])
      lTcount++;
    if (lTcount > 1)
      {
      long lStart = lCount+1;
      long lEnd = lCount + lTcount;
      for (i=lStart; i<lEnd; i++)
        for (long j=i; j>=lStart && (m_d[i2][j]<m_d[i2][j-1]==Ascending2); j--)
          {
          for (long k=0; k<m_Rows; k++)
            {
            double temp = m_d[k][j-1];
            m_d[k][j-1] = m_d[k][j];
            m_d[k][j] = temp;
            }
          }
      lCount += lTcount;
      }
    else 
      lCount++;
    }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sorts all the elements in the matrix by column j1 in ascending or decending sequence.*/
flag CDMatrix::SortCols( long j1, //index to sort column
                         flag Ascending, //sequence
                         long j2, //index to sort column2
                         flag Ascending2) //sequence
  {
  ASSERT(j1>=0 && j1<m_Cols); //check for valid column
  for (long i=1; i<m_Rows; i++)
    for (long j=i; j>=1 && (m_d[j][j1]<m_d[j-1][j1]==Ascending); j--)
      {
      for (long k=0; k<m_Cols; k++)
        {
        double temp = m_d[j-1][k];
        m_d[j-1][k] = m_d[j][k];
        m_d[j][k] = temp;
        }
      }
  if ((j2<0) || (j1==j2))
    return True;
  long lCount = 0;
  while (lCount<m_Rows)
    {
    long lTcount = 1;
    while (lTcount+lCount<m_Rows && m_d[lTcount+lCount][j1]==m_d[lTcount+lCount-1][j1])
      lTcount++;
    if (lTcount > 1)
      {
      long lStart = lCount+1;
      long lEnd = lCount + lTcount;
      for (i=lStart; i<lEnd; i++)
        for (long j=i; j>=lStart && (m_d[j][j2]<m_d[j-1][j2]==Ascending2); j--)
          {
          for (long k=0; k<m_Cols; k++)
            {
            double temp = m_d[j-1][k];
            m_d[j-1][k] = m_d[j][k];
            m_d[j][k] = temp;
            }
          }
      lCount += lTcount;
      }
    else 
      lCount++;
    }
  return True;
  }


// -------------------------------------------------------------------------
/*#F:Adds the specified value to each element of the matrix.*/
flag CDMatrix::Add(double d) //value to add
  {
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] += d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Adds the specified matrix element by element, the supplied matrix must have the same dimensions.*/
flag CDMatrix::Add(CDMatrix &m) //input matrix
  {
  ASSERT(m_Rows==m.m_Rows && m_Cols == m.m_Cols); //matrix dimensions must be equal
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] += m.m_d[i][j];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Subtracts the specified matrix element by element, the supplied matrix must have the same dimensions.*/
flag CDMatrix::Sub(CDMatrix &m)  //input matrix
  {
  ASSERT(m_Rows==m.m_Rows && m_Cols == m.m_Cols); //matrix dimensions must be equal
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] -= m.m_d[i][j];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Multiplies each element of the matrix by the specified value.*/
flag CDMatrix::Mult(double d)
  {
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] *= d;
  return True;
  }

// -------------------------------------------------------------------------

flag CDMatrix::Mult(CDMatrix &m) //input matrix
  {
  ASSERT(m_Cols==m.m_Rows); //no. of columns MUST equal no. of rows in m
  CDMatrix ab(m_Rows,m.m_Cols);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m.m_Cols; j++)
      for (long k=0; k<m_Cols; k++)
        ab[i][j] += (m_d[i][k] * m.m_d[k][j]);
  operator=(ab);
  return True;
  }

// -------------------------------------------------------------------------

flag CDMatrix::Div(CDMatrix &m) //input matrix
  {
  ASSERT(m_Cols==m.m_Rows); //no. of columns MUST equal no. of rows in m
  CDMatrix ab(m_Rows,m.m_Cols);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m.m_Cols; j++)
      for (long k=0; k<m_Cols; k++)
        ab[i][j] += (m_d[i][k] / m.m_d[k][j]);
  operator=(ab);
  return True;
  }

// -------------------------------------------------------------------------

flag CDMatrix::Transpose()
  {
  CDMatrix t(m_Cols,m_Rows);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      t[j][i] = m_d[i][j];
  operator=(t);
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Makes the matrix into the identity matrix. The matrix dimensions must be equal.*/
flag CDMatrix::MakeIntoIdentity()
  {
  ASSERT(m_Rows == m_Cols); //matrix dimensions must be equal
  for (long i=0; i<m_Rows; i++)
    {
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] = 0.0;
    m_d[i][i] = 1.0;
    }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Does a LU decomposition on a matrix. The matrix dimensions must be equal.
Used for efficiently solving a linear set of equations, such as solving for the 
inverse (ie inverting of NxN dimensional matrix). Used in conjunction with 
LUBacksub. The matrix should NOT be singular.
#R:A vector, Index, which records the row permutation effected by the partial pivoting.
An integer, D, which is set to +/-1 depending on whether the number of row interchanges was even or odd, respectively.
The function returns True if successfull or False if the matrix is singular (has row of zeros).
#S:CDMatrix::LUBacksub,CDMatrix::Invert*/
flag CDMatrix::LUDecompose( CLVector &Index,  //returned vector
                            int &D)           //returned value 
  {//see numerical recipes pg 39
  ASSERT(m_Rows == m_Cols); //matrix dimensions must be equal
  long N = m_Rows;
  long IMax = -1;
  double Big,Sum,Dum;
  CDVector Vv(N);
  Index.SetSize(N);

  D = 1;
  m_Err = 0;
  for (long i=0; i<N; i++)
    {
    Big = 0.0;
    for (long j=0; j<N; j++)
      if (fabs(m_d[i][j]) > Big)
        Big = fabs(m_d[i][j]);
    if (Big == 0.0)
      {
      m_Err = VMErrSingular;
      return False;
      }
    Vv.m_d[i] = 1.0 / Big;
    }
//Vv.dbgDump("vv");
  for (long j=0; j<N; j++)
    {//column j ...
//Dbg->Ln("==Col:%d==\n", j);
    for (i=0; i<j; i++)
      {
      Sum = m_d[i][j];
      for (long k=0; k<i; k++)
        Sum -= m_d[i][k] * m_d[k][j];
      m_d[i][j] = Sum;
      }
//dbgDump("aa");
    Big = 0.0;
    for (i=j; i<N; i++)
      {
      Sum = m_d[i][j];
      for (long k=0; k<j; k++)
        Sum -= m_d[i][k] * m_d[k][j];
      m_d[i][j] = Sum;
      Dum = Vv.m_d[i] * fabs(Sum);
      if (Dum>=Big)
        {
        IMax = i;
        Big = Dum;
        }
      }
//Dbg->Ln("IMax:%d  Big:%.3f\n", IMax, Big);
//dbgDump("bb");
    if (j!=IMax)
      {//interchange rows...
      //SwopRows(j,IMax);
      for (long k=0; k<N; k++)
        {
        Dum = m_d[IMax][k];
        m_d[IMax][k] = m_d[j][k];
        m_d[j][k] = Dum;
        }
      D =  -D;
      Vv.m_d[IMax] = Vv.m_d[j];
      }
    Index.m_d[j] = IMax;
//dbgDump("cc");
    if (m_d[j][j] == 0.0)
      m_d[j][j] = ZeroLimit;
    if (j != N-1)
      {
      Dum = 1.0 / (m_d[j][j]);
      for (i=j+1; i<N; i++)
        m_d[i][j] *= Dum;
      }
//dbgDump("dd");
//Vv.dbgDump("vv");
//Index.dbgDump("ix");
    }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Does the backsubstitution to find the solution to Ax=B assuming LuDecompose
has been called previously. The matrix dimensions must be equal. Used for efficiently
solving a linear set of equations and inverting a NxN dimensional matrix.
#R:The input vector B is overwritten with the result.
#S:CDMatrix::LUDecompose
*/
flag CDMatrix::LUBackSub(const CLVector &Index, //vector generated from LUDecompose
                         CDVector &B) //RHS column vector to solve
  {//see numerical recipes pg 44
  ASSERT(m_Rows == m_Cols); //matrix dimensions must be equal
  ASSERT(m_Rows == B.m_Len); //B vector length must be equal to matrix dimension
  ASSERT(m_Rows == Index.m_Len); //Index vector length must be equal to matrix dimension
  long N = m_Rows;
  long ii = -1;
  double Sum;
  for (long i=0; i<N; i++)
    {
    long ip = Index.m_d[i];
    ASSERT(ip>=0 && ip<N);//oops Index contains invalid stuff, did you do LUDecompose ?
    Sum = B.m_d[ip];
    B.m_d[ip] = B.m_d[i];
    if (ii != -1)
      for (long j=ii; j<=i-1; j++)
        Sum -= m_d[i][j] * B.m_d[j];
    else 
      if (Sum != 0.0)
        ii = i;
    B.m_d[i] = Sum;
    }
  for (i=N-1; i>=0; i--)
    {
    Sum = B.m_d[i];
    if (i<N-1)
      for (long j=i+1; j<N; j++)
        Sum -= m_d[i][j] * B.m_d[j];
    B.m_d[i] = Sum / m_d[i][i];
    }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:LUInvert finds the inverse of the matrix using the LUDecompisition and backsubtitution method. 
The matrix dimensions must be equal and non-singular.
#R:The function returns True if successfull or False if the matrix is singular.*/
flag CDMatrix::LUInvert()
  {//see numerical recipes pg 45
  ASSERT(m_Rows == m_Cols); //matrix dimensions must be equal
  long N = m_Rows;
  int D;
  CLVector Index(N);
  CDVector Tmp(N);
  CDMatrix Inv(N,N);

  Inv.MakeIntoIdentity();
  flag did = LUDecompose(Index, D);
//dbgDump("lud");
  if (did)
    {
    for (long j=0; j<N; j++)
      {
      for (long i=0; i<N; i++)
        Tmp.m_d[i] = Inv.m_d[i][j];
      LUBackSub(Index, Tmp);
//char Buff[256];
//sprintf(Buff,"t%d",j);
//Tmp.dbgDump(Buff);
      for (i=0; i<N; i++)
        Inv.m_d[i][j] = Tmp.m_d[i];
      }
    operator=(Inv);
    }
  return did;
  }

// -------------------------------------------------------------------------
/*#F:Calculates the determinant of the matrix using LUDecompose. The matrix 
dimensions must be equal and non-singular.The default for the optional parameter 
Retain is False which would cause the contents of the matrix to be altered as 
though LUDecompose had been called. If Retain is set to True, extra work is 
done to return the matrix to it's original values.
#R:Det, the determinant (0.0 if the matrix is not invertable), The function may
return False if the matrix is singular and Det will be set to zero.*/
flag CDMatrix::LUDeterminant( double &Det, //The resulting determinant
                              flag Retain) //Set to True if matrix contents must not be changed, default = False
  {//see numerical recipes pg 46
//TODO: check that Det doesn't underflow or overflow !!!!!!!
  ASSERT(m_Rows == m_Cols); //matrix dimensions must be equal
  Det = 0.0;
  int D;
  CLVector Index(m_Rows);
  flag did;
  if (Retain)
    {//work with a copy of the matrix...
    CDMatrix m(m_Rows, m_Rows);
    m = (*this);
    did = m.LUDecompose(Index, D);
    if (did)
      {
      Det = D;
      for (long k=0; k<m_Rows; k++)
        Det *= m.m_d[k][k];
      }
    }
  else
    {
    did = LUDecompose(Index, D);
    if (did)
      {
      Det = D;
      for (long k=0; k<m_Rows; k++)
        Det *= m_d[k][k];
      }
    }
  return did;
  }

// -------------------------------------------------------------------------
/*
flag CDMatrix::GaussInvert()
  {//see numerical recipes pg 36
//TODO: CHECK there are errors !!!
  ASSERT(m_Rows == m_Cols); //matrix dimensions must be equal
  m_Err = 0;
  long N = m_Rows;
  long iRow,iCol;
  double Pivinv;
  CLVector Indxc(N);
  CLVector Indxr(N);
  CLVector Ipiv(N);
  for (long i=0; i<N; i++)
    {
	double Big = 0.0;
	for (long j=0; j<N; j++)
	  if (Ipiv[j] != 1)
	    for (long k=0; k<N; k++)
	      {
	      if (Ipiv[k] == 0)
	        {
	        if (fabs(m_d[j][k]) >= Big)
	          {
	          Big = fabs(m_d[j][k]);
	          iRow = j;
	          iCol = k;
	          }
	        }
		  else if (Ipiv[k] > 1)
		    {
		    m_Err = VMErrSingular;
			return False;
		    }
	      }
	++(Ipiv[iCol]);
	if (iRow != iCol)
	  {
	  for (long l=0; l<N; l++)
	    {
	    double temp = m_d[iRow][l];
	    m_d[iRow][l] = m_d[iCol][l];
	    m_d[iCol][l] = temp;
	    }
	  }
	Indxr[i] = iRow;
	Indxc[i] = iCol;
	if (m_d[iCol][iCol] == 0.0)
	  {
	  m_Err = VMErrSingular;
	  return False;
	  }
	Pivinv = 1 / m_d[iCol][iCol];
	m_d[iCol][iCol] = 1.0;
	for (long l=0; l<N; l++)
	  m_d[iCol][l] *= Pivinv;
	for (long ll=0; ll<N; ll++)
	  {
	  double temp = m_d[ll][iCol];
	  m_d[ll][iCol] = 0.0;
	  for (l=0; l<N; l++)
	    m_d[ll][l] -= m_d[iCol][l]*temp;
	  }
	}
  for (long l=N-1; l>=0; l--)
    {
    if (Indxr[l] != Indxc[l])
      for (long k=0; k<N; k++)
        {
		double temp = m_d[k][Indxr[l]];
		m_d[k][Indxr[l]] = m_d[k][Indxc[l]];
		m_d[k][Indxc[l]] = temp;
	    }                                                    
    }
  return True;
  }
*/
// -------------------------------------------------------------------------
/*#F:Loads the matrix from the specified file.*/
flag CDMatrix::Load(rCVMLoadHelper H, dword BlankFlag)
  {
  m_Err = 0;
  Strng s;
  s = H.Seperator;
  s += " \t";
  long EstNRows=H.Rows;
  long EstNCols=H.Cols;
  char *Tkns[1024];
  char Buff[8000];
  if (1)
    {//determine estimate of number of rows and columns
    CTokenFile f(AF_All|AF_BackupFiles, H.FName, H.IgnoreComments, False, False);
    f.SetSeperators(s());
    s = H.Seperator;
    s += " \t";
    f.SetSeperators(s());
    if (f.Open())
      {
      long i = 1;
      while ((i<H.StartRow) && (!f.AtEOF()))
        {
        f.GetNextLine();
        i++;
        }
      if (f.AtEOF())
        {
        m_Err = VMErrLocation;
        return False;
        }
      long DoRows = H.Rows;
      long DoCols = H.Cols;
      if (DoRows==0)
        DoRows = LONG_MAX;
      if (DoCols==0)
        DoCols = LONG_MAX;
      double NullVal = atof(H.NullStr);
      long j = 0; //row count
      long k = 0; //col count
      if (H.Vertical)
        {
        for ( ; (j<DoRows) && !f.AtEOF(); j++)
          {
          f.GetNextLine();
          if (f.AtEOF())
            break;

          strncpy(Buff, f.GetWholeLine(), 8000);
          Buff[8000-1]=0;
          long NTkns = (Buff[0]==0) ? 0 : ParseTokenList(Buff, Tkns, " \t", 1024, H.Seperator);
          if (NTkns==0)
            break;

          EstNRows=Max(EstNRows, j+1);
          EstNCols=Range(EstNCols, NTkns, DoCols);
          }
        }
      else
        {
        ASSERT_ALWAYS(FALSE, "Horizontal load not implemented!", __FILE__, __LINE__); //not implemented KGA
        }
      f.Close();
      }
    }

  CTokenFile f(AF_All|AF_BackupFiles, H.FName, H.IgnoreComments, False, False);
  f.SetSeperators(s());
  if (f.Open())
    {
    SetSize(Max(H.Rows, EstNRows), Max(H.Cols, EstNCols));
    long i = 1;
    while ((i<H.StartRow) && (!f.AtEOF()))
      {
      f.GetNextLine();
      i++;
      }
    if (f.AtEOF())
      {
      m_Err = VMErrLocation;
      return False;
      }
    long DoRows = H.Rows;
    long DoCols = H.Cols;
    if (DoRows==0)
      DoRows = LONG_MAX;
    if (DoCols==0)
      DoCols = LONG_MAX;
    double NullVal = atof(H.NullStr);
    long j = 0; //row count
    long k = 0; //col count
    long NCols=0;
    long NRows=0;
    if (H.Vertical)
      {
      for ( ; (j<DoRows) && !f.AtEOF(); j++)
        {
        f.GetNextLine();
        if (f.AtEOF())
          break;

        strncpy(Buff, f.GetWholeLine(), 8000);
        Buff[8000-1]=0;
        int NTkns = (Buff[0]==0) ? 0 : ParseTokenList(Buff, Tkns, " \t", 1024, H.Seperator);
        if (NTkns==0)
          break;

        //dbgpln("=== [%i] %s", NTkns, f.GetWholeLine());
        //goto starting column...
        for (i = H.StartCol-1, k = 0; (k<DoCols) && (k<NTkns); i++, k++)
          {
          if (j>=m_Rows && k>=m_Cols)
            SetSize(m_Rows+16, m_Cols+16, (BlankFlag ? SetNANFlag(BlankFlag):0.0));
          if (j>=m_Rows)
            SetSize(m_Rows+16, m_Cols, (BlankFlag ? SetNANFlag(BlankFlag):0.0));
          if (k>=m_Cols)
            SetSize(m_Rows, m_Cols+16, (BlankFlag ? SetNANFlag(BlankFlag):0.0));

          if (Tkns[i][0]==H.Seperator)
            m_d[j][k] = NullVal;
          else if (BlankFlag && strlen(Tkns[i])==0)
            m_d[j][k] = SetNANFlag(BlankFlag);
          else if (Tkns[i][0]=='*')
            m_d[j][k] = dNAN;
          else if (_stricmp(Tkns[i], "Free")==0 || toupper(Tkns[i][0])=='F')
            m_d[j][k] = SetNANFlag(NF_Free);
          else if (_stricmp(Tkns[i], "Hold")==0 || toupper(Tkns[i][0])=='H')
            m_d[j][k] = SetNANFlag(NF_Hold);
          else if (_stricmp(Tkns[i], "Pulse")==0 || toupper(Tkns[i][0])=='P')
            m_d[j][k] = SetNANFlag(NF_Pulse);
          else if (strchr(Tkns[i], ':')!=0) // Time
            {
            CTimeValue V(Tkns[i]);
            m_d[j][k] = V.Seconds;
            }
          else
            m_d[j][k] = atof(Tkns[i]);
          NCols=Max(NCols, k+1);
          //dbgpln("[%3i %3i] = %10.4f", j,k,m_d[j][k]);
          }
        NRows=Max(NRows, j+1);
        }
      }
    else
      {
      ASSERT_ALWAYS(FALSE, "Horizontal load not implemented!", __FILE__, __LINE__); //not implemented KGA
      }
    SetSize(NRows, NCols);
    f.Close();
   
    return True;
    }

  m_Err = VMErrFileOpen;
  return False;

  /**
 
  m_Err = 0;
  CTokenFile f(AF_All|AF_BackupFiles, H.FName, H.IgnoreComments, False, False);
  Strng s;
  s = H.Seperator;
  s += " \t";
  f.SetSeperators(s());
  if (f.Open())
    {
    SetSize(H.Rows, H.Cols);
    long i = 1;
    while ((i<H.StartRow) && (!f.AtEOF()))
      {
      f.GetNextLine();
dbgpln("=== %s", f.GetWholeLine());
      i++;
      }
    if (f.AtEOF())
      {
      m_Err = VMErrLocation;
      return False;
      }
    long DoRows = H.Rows;
    long DoCols = H.Cols;
    if (DoRows==0)
      DoRows = LONG_MAX;
    if (DoCols==0)
      DoCols = LONG_MAX;
    double NullVal = atof(H.NullStr);
    long j = 0; //row count
    long k = 0; //col count
    if (H.Vertical)
      {
      flag RowsDone = False;
      while ((j<DoRows) && (!RowsDone) && (!f.AtEOF()))
        {
        //goto starting column...
        i = 1;
        while (i<H.StartCol)
          {
          s = f.NextToken();
          if (!f.AtLastToken() && s[0]!=H.Seperator)
            f.NextToken();
          if (f.AtEOF() || f.AtLastToken())
            i = H.StartCol;
          i++;
          if (f.AtLastToken() || f.AtEOF())
            RowsDone = True;
          }
        if (!RowsDone)
          {
          //read row...
          flag ColsDone = False;
          k = 0;
          while ((k<DoCols) && (!ColsDone) && (!f.AtEOF()))
            {
            if ((f.AtLastToken() && k>0) || f.AtEOF())
              ColsDone = True;
            else
              {
              s = f.NextToken();
              if (s.Length()==0)
                ColsDone = True;
              else
                {
                if (j>=m_Rows && k>=m_Cols)
                  SetSize(m_Rows+16, m_Cols+16);
                if (j>=m_Rows)
                  SetSize(m_Rows+16, m_Cols);
                if (k>=m_Cols)
                  SetSize(m_Rows, m_Cols+16);
                if (s[0]==H.Seperator)
                  m_d[j][k] = NullVal;
                else if (s[0]=='*')
                  m_d[j][k] = dNAN;
                else
                  m_d[j][k] = atof(s());
dbgpln("[%3i %3i] = %10.4f", j,k,m_d[j][k]);
                k++;
                if (!f.AtLastToken() && s[0]!=H.Seperator)
                  f.NextToken();
                }
              }
            }
          j++;
          if (j==1)
            {//read first row
            SetSize(m_Rows, k);
            if (k<DoCols && DoCols!=LONG_MAX)
              RowsDone = True;
            DoCols = k;
            }
          else
            {
            if (k<DoCols && k==0) //kga 30/10/01: changed from line below to this
            //if (k<DoCols)
              RowsDone = True; //reached "empty" row
            else
              { //kga 30/10/01: insert null values for missing columns on RHS
              while (k<DoCols)
                {
                m_d[j-1][k] = NullVal;
                k++;
                }
              }
            }
          if (RowsDone)
            j--;
          if (!f.AtLastToken())
            {
            f.GetNextLine();
dbgpln("=== %s", f.GetWholeLine());
            }
          }
        }
      }
    else
      {
      ASSERT_ALWAYS(FALSE, "Horizontal load not implemented!", __FILE__, __LINE__); //not implemented KGA
      }
    SetSize(j, m_Cols);
    f.Close();
    return True;
    }

  m_Err = VMErrFileOpen;
  return False;
**/
  }

// -------------------------------------------------------------------------

CDMatrix& CDMatrix::operator=(const CDMatrix &m)
  {
  SetSize(m.m_Rows, m.m_Cols);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] = m.m_d[i][j];
  return *this;
  }

// -------------------------------------------------------------------------

flag CDMatrix::operator==(const CDMatrix &m)
  {
  if (m_Rows != m.m_Rows || m_Cols != m.m_Cols)
    return False;
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      if (m_d[i][j] - m.m_d[i][j] < ZeroLimit)
        return False;
  return True;
  }

// -------------------------------------------------------------------------
/*double& CDMatrix::operator()(long i,long j)
{
 ASSERT(i>=0 && i<Len && j>=0 && j<Wid); 
return d[i][j]; 
};*/

void CDMatrix::dbgDump( pchar Desc)
  {
  char Buff[4096];
  dbgpln("%s[%d,%d]", Desc, m_Rows, m_Cols);
  for (long i=0;i<m_Rows; i++)
    {
    sprintf(Buff,"%4d)", i);
    for (long j=0; j<Min(255L,m_Cols); j++)
      if (Valid(m_d[i][j]))
        sprintf(Buff, "%s %11.3g", Buff, m_d[i][j]);
      else if (HasNANFlag(m_d[i][j]))
        {
        Strng S;
        if (HasNANFlag(m_d[i][j], NF_Pulse))
          S+="*Pulse";
        if (HasNANFlag(m_d[i][j], NF_Hold))
          S+="*Hold";
        if (HasNANFlag(m_d[i][j], NF_Free))
          S+="*Free";
        if (HasNANFlag(m_d[i][j], NF_Ignore))
          S+="*Ignore";
        if (HasNANFlag(m_d[i][j], NF_Block))
          S+="*Block";
        if (HasNANFlag(m_d[i][j], NF_Plus))
          S+="*+";
        if (HasNANFlag(m_d[i][j], NF_Minus))
          S+="*-";
        if (HasNANFlag(m_d[i][j], NF_Star))
          S+="**";
        if (HasNANFlag(m_d[i][j],NF_All&~(NF_Free|NF_Hold|NF_Pulse)))
          S+="???";
        sprintf(Buff, "%s %11s", Buff, S());
        }
      else
        sprintf(Buff, "%s %11s", Buff, "*");
    if (m_Cols>255L)
      sprintf(Buff, "%s ....", Buff);
    dbgpln("%s", Buff);
    }
  }

// =========================================================================
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// =========================================================================
// =========================================================================
int CLMatrix::m_Err = 0;
// -------------------------------------------------------------------------
/*#F:Constructor calls SetSize.*/
CLMatrix::CLMatrix( long NewRows, //Number of rows
                    long NewCols, //Number of columns
                    long InitVal) //Optional initial value for elements, default = 0.0
  {
  m_Rows = 0;
  m_Cols = 0;
  m_d = NULL;
  SetSize(NewRows, NewCols, InitVal);
  }

// -------------------------------------------------------------------------
/*#F:Constructor creates empty matrix (Columns and rows = 0).*/
CLMatrix::CLMatrix()
  {
  m_Rows = 0;
  m_Cols = 0;
  m_d = NULL;
  }

// -------------------------------------------------------------------------
/*#F:Copy constructor, creates a copy of the supplied matrix.*/
CLMatrix::CLMatrix(const CLMatrix &m) //Matrix to copy
  {
  m_Rows = 0;
  m_Cols = 0;
  m_d = NULL;
  SetSize(m.m_Rows, m.m_Cols);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] = m.m_d[i][j];
  }

// -------------------------------------------------------------------------
/*#F:Destructor "frees" all memory by calling FreePts.*/
CLMatrix::~CLMatrix()
  {
  FreePts();
  }

// -------------------------------------------------------------------------
/*#F:"Frees" memory and sets size to 0 by 0.*/
void CLMatrix::FreePts()
  {
  if (m_d)
    {
    for (long i=0; i<m_Rows; i++)
      delete []m_d[i];
    delete []m_d;
    m_d = NULL;
    m_Rows = 0;
    m_Cols = 0;
    }
  }

// -------------------------------------------------------------------------
/*#F:#R:The largest element in the matrix.*/
long CLMatrix::ScanMax()
  {
  long temp = (m_d ? m_d[0][0] : 0);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      temp=Max(temp,m_d[i][j]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:#R:The smallest element in the matrix.*/
long CLMatrix::ScanMin()
  {
  long temp = (m_d ? m_d[0][0] : 0);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      temp=Min(temp,m_d[i][j]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:#R:The largest element in the specified row.*/
long CLMatrix::ScanRowMax(long i)  //row index
  {
  ASSERT(i>=0 && i<m_Rows); //check for valid row
  long temp = (m_d ? m_d[i][0] : 0);
  for (long j=1; j<m_Cols; j++)
    temp = Max(temp, m_d[i][j]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:#R:The largest element in the specified row.*/
long CLMatrix::ScanRowMin(long i)  //row index
  {
  ASSERT(i>=0 && i<m_Rows); //check for valid row
  long temp = (m_d ? m_d[i][0] : 0);
  for (long j=1; j<m_Cols; j++)
    temp = Min(temp, m_d[i][j]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:Sets the size of the matrix. If NewRows OR NewCols is zero, all elements are 
removed. Alternatively the matrix size is adjusted, new elements are initialised to
InitVal and size decreases causes truncation.*/
flag CLMatrix::SetSize( long NewRows,  //number of rows
                        long NewCols,  //number of columns
                        long InitVal) //Optional initial value for new elements, default = 0
  {
  ASSERT(NewRows>=0 && NewCols>=0);  //must be positive dimensions
  if ((NewRows==m_Rows) && (NewCols==m_Cols))
    return True;
  if ((NewRows==0) || (NewCols==0))
    {
    FreePts();
    return True;
    }
  long i,j;
  long k = Min(m_Cols,NewCols);
  long ** nd = new plong[NewRows];
  for (i=0; i<NewRows; i++)
    {
    nd[i] = new long[NewCols];
    if (i<m_Rows)
      {
      for (j=0; j<k; j++)
        nd[i][j] = m_d[i][j];
      for (; j<NewCols; j++)
        nd[i][j] = InitVal;
      }
    else
      for (j=0; j<NewCols; j++)
        nd[i][j] = InitVal;
    }
  FreePts();
  m_d = nd;
  m_Cols = NewCols;
  m_Rows = NewRows;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets all elements of the matrix to the specified value.*/
flag CLMatrix::SetAll(long d)  //Optional new value, default = 0
  {
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] = d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets all elements of the specified column to the specified value.
Column index j must be valid.*/
flag CLMatrix::SetCol( long j,  //column index 
                       long d)  //Optional new value, default = 0
  {
  ASSERT(j>=0 && j<m_Cols); //check for valid column
  for (long i=0; i<m_Rows; i++)
    m_d[i][j] = d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets all elements of the specified row to the specified value.
Row index i must be valid.*/
flag CLMatrix::SetRow( long i,    //row index
                       long d)  //Optional new value, default = 0
  {
  ASSERT(i>=0 && i<m_Rows); //check for valid row
  for (long j=0; j<m_Cols; j++)
    m_d[i][j] = d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets the specified column to the vector values. Column index j must be valid.
The vector length must be the same as the number of rows.*/
flag CLMatrix::SetCol( long j, //column index
                       const CLVector &v)  //input vector
  {
  ASSERT(j>=0 && j<m_Cols); //check for valid column
  ASSERT(m_Rows==v.m_Len); //check that vector len = matrix row count
  for (long i=0; i<m_Rows; i++)
    m_d[i][j] = v.m_d[i];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets the specified row to the vector values. Row index i must be valid.
The vector length must be the same as the number of columns.*/
flag CLMatrix::SetRow( long i, //row index
                       const CLVector &v) //input vector
  {
  ASSERT(i>=0 && i<m_Rows); //check for valid row
  ASSERT(m_Cols==v.m_Len); //check that vector len = matrix column count
  for (long j=0; j<m_Cols; j++)
    m_d[i][j] = v.m_d[j];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:#R:The specified column as a vector. The length of the vector is set to the number of rows.*/
flag CLMatrix::GetCol( long j, //column index
                       CLVector &v) //ouput vector
  {
  ASSERT(j>=0 && j<m_Cols); //check for valid column
  v.SetSize(m_Rows);
  for (long i=0; i<m_Rows; i++)
    v.m_d[i] = m_d[i][j];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:#R:The specified row as a vector. The length of the vector is set to the number of columns.*/
flag CLMatrix::GetRow( long i, //row index
                       CLVector &v) //output vector
  {
  ASSERT(i>=0 && i<m_Rows); //check for valid row
  v.SetSize(m_Cols);
  for (long j=0; j<m_Cols; j++)
    v.m_d[j] = m_d[i][j];//bga
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Swops the specified columns. The column index j1 and j2 must be valid.*/
flag CLMatrix::SwopCols( long j1, //index to column one
                         long j2) //index to column two
  {
  ASSERT(j1>=0 && j1<m_Cols); //check for valid column
  ASSERT(j2>=0 && j2<m_Cols); //check for valid column
  for (long i=0; i<m_Rows; i++)
    {
    long temp = m_d[i][j1];
    m_d[i][j1] = m_d[i][j2];
    m_d[i][j2] = temp;
    }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Swops the specified rows. The row index i1 and i2 must be valid.*/
flag CLMatrix::SwopRows( long i1, //index to row one
                         long i2) //index to row two
  {
  ASSERT(i1>=0 && i1<m_Rows); //check for valid row
  ASSERT(i2>=0 && i2<m_Rows); //check for valid row
  for (long j=0; j<m_Cols; j++)
    {
    long temp = m_d[i1][j];
    m_d[i1][j] = m_d[i2][j];
    m_d[i2][j] = temp;
    }
  return True;
  }

// -------------------------------------------------------------------------bga
/*#F:Sorts all the elements in the matrix by row i1 in ascending or decending sequence.*/
flag CLMatrix::SortRows( long i1, //index to sort row
                         flag Ascending, //sequence
                         long i2, //index to sort row2
                         flag Ascending2) //sequence
  {
  ASSERT(i1>=0 && i1<m_Rows); //check for valid row
  for (long i=1; i<m_Cols; i++)
    for (long j=i; j>=1 && (m_d[i1][j]<m_d[i1][j-1]==Ascending); j--)
      {
      for (long k=0; k<m_Rows; k++)
        {
        long temp = m_d[k][j-1];
        m_d[k][j-1] = m_d[k][j];
        m_d[k][j] = temp;
        }
      }
  if ((i2<0)||(i1==i2))
    return True;
  long lCount = 0;
  while (lCount<m_Cols)
    {
    long lTcount = 1;
    while (lTcount+lCount<m_Cols && m_d[i1][lTcount+lCount]==m_d[i1][lTcount+lCount-1])
      lTcount++;
    if (lTcount > 1)
      {
      long lStart = lCount+1;
      long lEnd = lCount + lTcount;
      for (i=lStart; i<lEnd; i++)
        for (long j=lStart; j>=1 && (m_d[i2][j]<m_d[i2][j-1]==Ascending2); j--)
          {
          for (long k=0; k<m_Rows; k++)
            {
            long temp = m_d[k][j-1];
            m_d[k][j-1] = m_d[k][j];
            m_d[k][j] = temp;
            }
          }
      lCount += lTcount;
      }
    else 
      lCount++;
    }
  return True;
  }

// -------------------------------------------------------------------------bga
/*#F:Sorts all the elements in the matrix by column j1 in ascending or decending sequence.*/
flag CLMatrix::SortCols( long j1, //index to sort column
                         flag Ascending, //sequence
                         long j2, //index to sort column2
                         flag Ascending2) //sequence
  {
  ASSERT(j1>=0 && j1<m_Cols); //check for valid column
  for (long i=1; i<m_Rows; i++)
    for (long j=i; j>=1 && (m_d[j][j1]<m_d[j-1][j1]==Ascending); j--)
      {
      for (long k=0; k<m_Cols; k++)
        {
        long temp = m_d[j-1][k];
        m_d[j-1][k] = m_d[j][k];
        m_d[j][k] = temp;
        }
      }
  if ((j2<0)||(j1==j2))
    return True;
  long lCount = 0;
  while (lCount<m_Rows)
    {
    long lTcount = 1;
    while (lTcount+lCount<m_Rows && m_d[lTcount+lCount][j1]==m_d[lTcount+lCount-1][j1])
      lTcount++;
    if (lTcount > 1)
      {
      long lStart = lCount+1;
      long lEnd = lCount + lTcount;
      for (i=lStart; i<lEnd; i++)
        for (long j=lStart; j>=1 && (m_d[j][j2]<m_d[j-1][j2]==Ascending2); j--)
          {
          for (long k=0; k<m_Cols; k++)
            {
            long temp = m_d[j-1][k];
            m_d[j-1][k] = m_d[j][k];
            m_d[j][k] = temp;
            }
          }
      lCount += lTcount;
      }
    else 
      lCount++;
    }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Adds the specified value to each element of the matrix.*/
flag CLMatrix::Add(long d) //value to add
  {
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] += d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Adds the specified matrix element by element, the supplied matrix must have the same dimensions.*/
flag CLMatrix::Add(CLMatrix &m) //input matrix
  {
  ASSERT(m_Rows==m.m_Rows && m_Cols == m.m_Cols); //matrix dimensions must be equal
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] += m.m_d[i][j];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Subtracts the specified matrix element by element, the supplied matrix must have the same dimensions.*/
flag CLMatrix::Sub(CLMatrix &m)  //input matrix
  {
  ASSERT(m_Rows==m.m_Rows && m_Cols == m.m_Cols); //matrix dimensions must be equal
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] -= m.m_d[i][j];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Multiplies each element of the matrix by the specified value.*/
flag CLMatrix::Mult(long d)
  {
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] *= d;
  return True;
  }

// -------------------------------------------------------------------------

flag CLMatrix::Mult(CLMatrix &m) //input matrix
  {
  ASSERT(m_Cols==m.m_Rows); //no. of columns MUST equal no. of rows in m
  CLMatrix ab(m_Rows,m.m_Cols);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m.m_Cols; j++)
      for (long k=0; k<m_Cols; k++)
        ab[i][j] += (m_d[i][k] * m.m_d[k][j]);
  operator=(ab);
  return True;
  }

// -------------------------------------------------------------------------

flag CLMatrix::Div(CLMatrix &m) //input matrix
  {
  ASSERT(m_Cols==m.m_Rows); //no. of columns MUST equal no. of rows in m
  CLMatrix ab(m_Rows,m.m_Cols);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m.m_Cols; j++)
      for (long k=0; k<m_Cols; k++)
        ab[i][j] += (m_d[i][k] / m.m_d[k][j]);
  operator=(ab);
  return True;
  }

// -------------------------------------------------------------------------

flag CLMatrix::Transpose()
  {
  CLMatrix t(m_Cols,m_Rows);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      t[j][i] = m_d[i][j];
  operator=(t);
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Makes the matrix into the identity matrix. The matrix dimensions must be equal.*/
flag CLMatrix::MakeIntoIdentity()
  {
  ASSERT(m_Rows == m_Cols); //matrix dimensions must be equal
  for (long i=0; i<m_Rows; i++)
    {
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] = 0;
    m_d[i][i] = 1;
    }
  return True;
  }

// -------------------------------------------------------------------------

CLMatrix& CLMatrix::operator=(const CLMatrix &m)
  {
  SetSize(m.m_Rows, m.m_Cols);
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      m_d[i][j] = m.m_d[i][j];
  return *this;
  }

// -------------------------------------------------------------------------

flag CLMatrix::operator==(const CLMatrix &m)
  {
  if (m_Rows != m.m_Rows || m_Cols != m.m_Cols)
    return False;
  for (long i=0; i<m_Rows; i++)
    for (long j=0; j<m_Cols; j++)
      if (m_d[i][j] - m.m_d[i][j] < ZeroLimit)
        return False;
  return True;
  }

// -------------------------------------------------------------------------
/*long& CLMatrix::operator()(long i,long j)
{
 ASSERT(i>=0 && i<Len && j>=0 && j<Wid); 
return d[i][j]; 
};*/

void CLMatrix::dbgDump( pchar Desc)
  {
  dbgpln("%s:[%d,%d]", Desc, m_Rows, m_Cols);
  for (long i=0;i<m_Rows; i++)
    {
    dbgp("%4d)", i);
    for (long j=0; j<Min(255L,m_Cols); j++)
      if (m_d[i][j]==LONG_MAX)
        dbgp("    Max");
      else if (m_d[i][j]==-LONG_MAX)
        dbgp("   -Max");
      else
        dbgp(" %6i", m_d[i][j]);
    if (m_Cols>16L)
      dbgp(" ....");
    dbgpln("");
    }
  }

// =========================================================================
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// =========================================================================
