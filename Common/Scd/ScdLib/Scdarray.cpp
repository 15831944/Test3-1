//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define  __SCDARRAY_CPP

#include "sc_defs.h"
#include "scdarray.h"
#include "dbgmngr.h"

// =========================================================================
//
//
//
// =========================================================================

int CDArray::m_Err = 0;

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDArray::CDArray(const CDArray &Other)
  {
  SetSize(Other.GetSize());
  for (long i=0; i<GetSize(); i++)
    m_pData[i] = Other.m_pData[i];
  }

CDArray::CDArray(const int Size, double Init)
  {
  SetSize(Size);
  SetAll(Init);
  }
// -------------------------------------------------------------------------

//flag CDArray::SetCnvs(CnvIndex CnvDC, char * CnvTxt)
//  {
//  iCnvDC=CnvDC;
//  //pCnv=NULL;
//  sCnvTxt=CnvTxt!=NULL ? CnvTxt : "";
//  return (gs_CnvsMngr.FindSecCnv(iCnvDC, CnvTxt)!=NULL);
//  }

// -------------------------------------------------------------------------
/*#F:#R:The largest double within the vector.*/
double CDArray::ScanMax(int SkipStart, int SkipEnd)
  {
  if (m_pData==NULL)
    return 0.0;
  if (SkipStart>=GetSize())
    return 0.0;
  double temp = m_pData[SkipStart];
  for (long i=SkipStart+1; i<GetSize()-SkipEnd; i++)
    temp=Max(temp,m_pData[i]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:#R:The smallest double within the vector.*/
double CDArray::ScanMin(int SkipStart, int SkipEnd)
  {
  if (m_pData==NULL)
    return 0.0;
  if (SkipStart>=GetSize())
    return 0.0;
  double temp = m_pData[SkipStart];
  for (long i=SkipStart+1; i<GetSize()-SkipEnd; i++)
    temp=Min(temp,m_pData[i]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:Swops the elements at the specified index values.*/
flag CDArray::Swop( long i1,  //Index value one
                     long i2)  //Index value two
  {
  ASSERT(i1>=0 && i1<GetSize() && i2>=0 && i2<GetSize());  //check for valid index range
  double temp = m_pData[i1];
  m_pData[i1] = m_pData[i2];
  m_pData[i2] = temp;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sorts all the elements in the vector in ascending or decending sequence.*/
flag CDArray::Sort(flag Ascending)//True if vector is to be sorted from lowest to highest, default = True
  {
  for (long i=1; i<GetSize(); i++)
    for (long j=i; j>=1 && (m_pData[j]<m_pData[j-1]==Ascending); j--)
      {
      double temp = m_pData[j-1];
      m_pData[j-1] = m_pData[j];
      m_pData[j] = temp;
      }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Reverses all the elements in the vector.*/
flag CDArray::Reverse()
  {
  if (GetSize()>1)
    {
	  const long k = div(GetSize(),2).quot;
	  for (long i=0; i<k; i++)
	    {
	    double temp = m_pData[i];
	    m_pData[i] = m_pData[GetSize()-i-1];
	    m_pData[GetSize()-i-1] = temp;
	    }
	  }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets all elements to the value specified.*/
flag CDArray::SetAll(double d)
  {
  for (long i=0; i<GetSize(); i++)
    m_pData[i] = d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Adds the specified value to each element.*/
flag CDArray::OpAdd(double d)
  {
  for (long i=0; i<GetSize(); i++)
    m_pData[i] += d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Adds the specified vector to itself. Vector sizes MUST match.*/
flag CDArray::OpAdd(CDArray &v, double Scl, double Off)
  {
  ASSERT(GetSize() == v.GetSize()); //check that lengths are equal
  for (long i=0; i<GetSize(); i++)
    m_pData[i] += Scl*v.m_pData[i] + Off;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Subtracts the specified vector from itself. Vector sizes MUST match.*/
flag CDArray::OpSub(CDArray &v)
  {
  ASSERT(GetSize() == v.GetSize()); //check that lengths are equal
  for (long i=0; i<GetSize(); i++)
    m_pData[i] -= v.m_pData[i];
  return True;
  }

// -------------------------------------------------------------------------

flag CDArray::OpMult(double d)
  {
  for (long i=0; i<GetSize(); i++)
    m_pData[i] *= d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Multiplies the specified vector by itself. Vector sizes MUST match.*/
flag CDArray::OpMult(CDArray &v)
  {
  ASSERT(GetSize() == v.GetSize()); //check that lengths are equal
  for (long i=0; i<GetSize(); i++)
    m_pData[i] *= v.m_pData[i];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Divides the specified vector into itself. Vector sizes MUST match.*/
flag CDArray::OpDiv(CDArray &v)
  {
  ASSERT(GetSize() == v.GetSize()); //check that lengths are equal
  for (long i=0; i<GetSize(); i++)
    m_pData[i] /= v.m_pData[i];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Loads the vector from the specified file.*/
//flag CDArray::Load(rCVMLoadHelper H, //Helper class with all the options set
//                    CnvIndex CnvDC)       //conversion
//  {
//  m_Err = 0;
//  H.cVectorName[0]=0;
//  H.cCnvTxt[0]=0;
//  H.bGotName=False;
//  H.bGotCnv=False;
//
//  iCnvDC=CnvDC;
//  sCnvTxt="";
//
//  CTokenFile f(H.FName, H.IgnoreComments, False, False);
//  Strng s;
//  s = H.Seperator;
//  s += " \t";
//  f.SetSeperators(s());
//  if (f.Open())
//    {
//    SetSize(H.Rows);
//    long RowNo = 1;
//    while ((RowNo<H.StartRow) && (!f.AtEOF()))
//      {
//      f.GetNextLine();
//      RowNo++;
//      }
//    if (f.AtEOF())
//      {
//      m_Err = VMErrLocation;
//      return False;
//      }
//    long DoLen = H.Rows;
//    if (DoLen==0)
//      DoLen = LONG_MAX;
//    double NullVal = atof(H.NullStr);
//    long j = 0;
//    if (H.Vertical)
//      {
//      flag Done = False;
//      while ((j<DoLen) && (!Done) && (!f.AtEOF()))
//        {
//        long i = 1;
//        while ((i<H.StartCol) && (!Done))//(!f.AtEOF()) && (!f.AtLastToken()))
//          {
//          s = f.NextToken();
//          if (!f.AtLastToken() && s[0]!=H.Seperator)
//            f.NextToken();
//          i++;
//          if (f.AtLastToken() || f.AtEOF())
//            Done = True;
//          }
//        if (!Done)
//          {
//          s = f.NextToken();
//          if (s.Length()==0)
//            Done = True;
//          else 
//            {
//            // Check if not Valid # is it a name or a Conversion
//            if (s.StrSpn("+-0123456789e.*")<s.Length())
//              if (H.bTest4Name && !H.bGotName)
//                H.iNameColRow=RowNo;
//              else if (H.bTest4Cnv && !H.bGotCnv)
//                H.iCnvColRow=RowNo;
//
//            if (RowNo==H.iNameColRow)
//              {
//              strcpy(H.cVectorName, s());
//              H.bGotName=True;
//              }
//            else if (RowNo==H.iCnvColRow)
//              {
//              strcpy(H.cCnvTxt, s());
//              sCnvTxt=s();
//              if (s.Length()>0)
//                {
//                pCDataCnv pCnv=gs_CnvsMngr.FindSecCnv(iCnvDC, sCnvTxt());
//                if (pCnv==NULL && stricmp(sCnvTxt(), "(null)")!=0)
//                  LogError("Vectors", 0, "Conversion %s not Found", s());
//                }
//              H.bGotCnv=True;
//              }
//            else
//              {
//              if (j>=GetSize())
//                SetSize(GetSize()+16);
//              if (s[0]==H.Seperator)
//                m_pData[j] = NullVal;
//              else if (s[0]=='*')
//                m_pData[j] = dNAN;
//              else if (sCnvTxt.Length()>0)
//                SetNormal(j, atof(s()));
//              else
//                m_pData[j]=atof(s());
//              j++;
//              }
//            if (!f.AtLastToken())
//              f.GetNextLine();
//            RowNo++;
//            }
//          }
//        }
//      }
//    else
//      {
//      long i = 1;
//      while ((i<H.StartCol) && (!f.AtEOF()) && (!f.AtLastToken()))
//        {
//        s = f.NextToken();
//        if (!f.AtLastToken() && s[0]!=H.Seperator)
//          f.NextToken();
//        i++;
//        }
//      if (f.AtLastToken() || f.AtEOF())
//        {
//        m_Err = VMErrLocation;
//        return False;
//        }
//      flag Done = False;
//      while ((j<DoLen) && (!Done) && (!f.AtEOF()))
//        {
//        if (f.AtLastToken() || f.AtEOF())
//          Done = True;
//        else
//          {
//          s = f.NextToken();
//          if (j>=GetSize())
//            SetSize(GetSize()+16);
//          if (s[0]==H.Seperator)
//            m_pData[j] = NullVal;
//          else
//            m_pData[j] = atof(s());
//          j++;
//          if (!f.AtLastToken() && s[0]!=H.Seperator)
//            f.NextToken();
//          }
//        }
//      }
//    SetSize(j);
//    f.Close();
//    H.bTest4Name = False;
//    H.bTest4Cnv = False;
//    return True;
//    }
//  m_Err = VMErrFileOpen;
//  return False;
//  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDArray& CDArray::operator=(const CDArray &v)
  {
  SetSize(v.GetSize());
  for (long i=0; i<GetSize(); i++)
    m_pData[i] = v.m_pData[i];
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDArray& CDArray::operator+=(const CDArray &v)
  {
  int N=Min(GetSize(), v.GetSize());
  for (long i=0; i<N; i++)
    m_pData[i] += v.m_pData[i];
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDArray& CDArray::operator-=(const CDArray &v)
  {
  int N=Min(GetSize(), v.GetSize());
  for (long i=0; i<N; i++)
    m_pData[i] -= v.m_pData[i];
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDArray& CDArray::operator*=(const CDArray &v)
  {
  int N=Min(GetSize(), v.GetSize());
  for (long i=0; i<N; i++)
    m_pData[i] *= v.m_pData[i];
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDArray& CDArray::operator/=(const CDArray &v)
  {
  int N=Min(GetSize(), v.GetSize());
  for (long i=0; i<N; i++)
    m_pData[i] /= v.m_pData[i];
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDArray& CDArray::operator+=(const double v)
  {
  int N=GetSize();
  for (long i=0; i<N; i++)
    m_pData[i] += v;
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDArray& CDArray::operator-=(const double v)
  {
  int N=GetSize();
  for (long i=0; i<N; i++)
    m_pData[i] -= v;
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDArray& CDArray::operator*=(const double v)
  {
  int N=GetSize();
  for (long i=0; i<N; i++)
    m_pData[i] *= v;
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDArray& CDArray::operator/=(const double v)
  {
  int N=GetSize();
  for (long i=0; i<N; i++)
    m_pData[i] /= v;
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:#R:True if the vectors are the same length and the elements are equal 
(element difference is less than the constant ZeroLimit).*/
flag CDArray::operator==(const CDArray &v)
  {
  if (GetSize() != v.GetSize())
    return False;
  for (long i=0; i<GetSize(); i++)
    if (m_pData[i] - v.m_pData[i] > ZeroLimit)
	    return False;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Normalises the vector and returns the original sum of all the elements .*/
double CDArray::Normalise()
  {
  double t, total = 0.0;
  for (long i = 0; i < GetSize(); i++)
    total += m_pData[i];
  t = NZ(total);
  for (i = 0; i < GetSize(); i++)
    m_pData[i] /= t;
  return total;
  };

// -------------------------------------------------------------------------
/*#F:Normalises the vector and returns the original sum of all the elements .*/
double CDArray::Sum()
  {
  double total = 0.0;
  for (long i = 0; i < GetSize(); i++)
    total += m_pData[i];
  return total;
  };

// -------------------------------------------------------------------------

double CDArray::ToCumulative(double ConstInteg, double Total, flag Inverse, flag Shift)
  {
  if (GetSize()==0)
    return False;
  const long N = GetSize();
  double Yt;
  m_pData[0]+=ConstInteg;
  for (long i=1; i<N; i++)
    m_pData[i] += m_pData[i-1];//*(X[i]-X[i-1]);
  Yt = m_pData[N-1];
  if (Shift)
    {
    for (long i=N-1; i>0; i--)
      m_pData[i] = m_pData[i-1];//*(X[i]-X[i-1]);
    m_pData[i]=ConstInteg;
    }
  if (Inverse)
    {
    for (long i=0; i<N; i++)
      m_pData[i] = Yt-m_pData[i];
    }

  if (Valid(Total))
    {
    double Scale=Total/GTZ(Yt); 
    if (Scale >= 1.0e-10 && Scale <=1.0e10)
      OpMult(Scale); 
    return Scale;
    }
  
  return 1.0;
  }

// -------------------------------------------------------------------------

double CDArray::LinearInterpolate(double XValue, CDArray & X, flag Extrapolate)
  {
  const long len = GetSize();
  double * Y=m_pData;
  switch (len)
    {
    case 0 : return 0.0;
    case 1 : return Y[0];
    default :
      {
      for (long i=0; (i<len-2) && (XValue>=X[i+1]); i++) 
        { };
      int xx=0;
      if (!Extrapolate)
        XValue=Range(X[0], XValue, X[len-1]);
      return Y[i]+(Y[i+1] - Y[i]) * (XValue - X[i]) / GTZ(X[i+1] - X[i]);
      }
    }
  return 0.0;
  };

// -------------------------------------------------------------------------
/*#F:Outputs the vector contents (format acording to options) to the debug file.*/
void CDArray::dbgDump( pchar Desc,  //Optional text description, default = ''
                        flag Horizontal) //Optional, default = True
  {
  dbgp("%s:[%d]", Desc, GetSize());
  if (Horizontal)
    {
    for (long i=0; i<Min(50,GetSize()); i++)
	  dbgp("%11.3g ", m_pData[i]);
  	if (GetSize()>5)
  	  dbgp("...");
    dbgpln("");
    }
  else
    {
    dbgpln("");
    for (long i=0; i<GetSize(); i++)
      dbgpln("%4d: %11.3g", i, m_pData[i]);
    }
  }

// -------------------------------------------------------------------------

flag CDArray::AdjustCumulativeEnd(double RqdCum, double ValMin, double ValMax, double & Err)
  {
  double Sum=0.0;
  for (int j=0; j<GetSize(); j++)
    {
    m_pData[j]=Range(ValMin, m_pData[j], ValMax);
    Sum+=m_pData[j];
    }
  double diff=Sum-RqdCum;
  Err = diff;
  if (fabs(diff)>1.0e-6*RqdCum)
    {//change is required...
    flag SignificantChange = (fabs(diff)>1.0e-2*RqdCum);
    j=GetSize()-1;
    while (j>=0 && fabs(diff)>1.0e-6*RqdCum)
      {
      double Old=m_pData[j];
      m_pData[j]=Range(ValMin, m_pData[j]-diff, ValMax);
      diff += m_pData[j]-Old;
      j--;
      }
    //return true; //change was required
    return SignificantChange; //only return true for "significant" change
    }
  return false; //change was NOT required
  }

// -------------------------------------------------------------------------

flag CDArray::AdjustCumulativeEnd(double RqdCum, double ValMin, double ValMax)
  {
  double Err = 0.0;
  return AdjustCumulativeEnd(RqdCum, ValMin, ValMax, Err);
  };

// ==========================================================================
//
//
//
// ==========================================================================

/*#F:#R:The largest double within the vector.*/
double CDMatrx::ScanMax()
  {
  double temp = (m_pData ? m_pData[0].ScanMax() : 0.0);
  for (long i=1; i<GetSize(); i++)
    temp=Max(temp,m_pData[i].ScanMax());
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:#R:The smallest double within the vector.*/
double CDMatrx::ScanMin()
  {
  double temp = (m_pData ? m_pData[0].ScanMin(): 0.0);
  for (long i=1; i<GetSize(); i++)
    temp=Min(temp,m_pData[i].ScanMin());
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CDMatrx& CDMatrx::operator=(const CDMatrx &v)
  {
  SetSize(v.GetSize());
  for (long i=0; i<GetSize(); i++)
    {
    //m_pData[i].SetSize(v.m_pData[i].GetSize());
    m_pData[i] = v.m_pData[i];
    }
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:#R:True if the vectors are the same length and the elements are equal 
(element difference is less than the constant ZeroLimit).*/
flag CDMatrx::operator==(const CDMatrx &v)
  {
  if (GetSize() != v.GetSize())
    return False;
  for (long i=0; i<GetSize(); i++)
    if (!(m_pData[i] == v.m_pData[i]))
	    return False;
  return True;
  }

// =========================================================================
//
//
//
// =========================================================================

int CIArray::m_Err = 0;

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CIArray::CIArray(const CIArray &Other)
  {
  SetSize(Other.GetSize());
  for (long i=0; i<GetSize(); i++)
    m_pData[i] = Other.m_pData[i];
  }

// -------------------------------------------------------------------------
/*#F:#R:The largest int within the vector.*/
int CIArray::ScanMax(int SkipStart, int SkipEnd)
  {
  if (m_pData==NULL)
    return 0;
  if (SkipStart>=GetSize())
    return 0;
  int temp = m_pData[SkipStart];
  for (long i=SkipStart+1; i<GetSize()-SkipEnd; i++)
    temp=Max(temp,m_pData[i]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:#R:The smallest int within the vector.*/
int CIArray::ScanMin(int SkipStart, int SkipEnd)
  {
  if (m_pData==NULL)
    return 0;
  if (SkipStart>=GetSize())
    return 0;
  int temp = m_pData[SkipStart];
  for (long i=SkipStart+1; i<GetSize()-SkipEnd; i++)
    temp=Min(temp,m_pData[i]);
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:Swops the elements at the specified index values.*/
flag CIArray::Swop( long i1,  //Index value one
                     long i2)  //Index value two
  {
  ASSERT(i1>=0 && i1<GetSize() && i2>=0 && i2<GetSize());  //check for valid index range
  int temp = m_pData[i1];
  m_pData[i1] = m_pData[i2];
  m_pData[i2] = temp;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sorts all the elements in the vector in ascending or decending sequence.*/
flag CIArray::Sort(flag Ascending)//True if vector is to be sorted from lowest to highest, default = True
  {
  for (long i=1; i<GetSize(); i++)
    for (long j=i; j>=1 && (m_pData[j]<m_pData[j-1]==Ascending); j--)
      {
      int temp = m_pData[j-1];
      m_pData[j-1] = m_pData[j];
      m_pData[j] = temp;
      }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Reverses all the elements in the vector.*/
flag CIArray::Reverse()
  {
  if (GetSize()>1)
    {
	  const long k = div(GetSize(),2).quot;
	  for (long i=0; i<k; i++)
	    {
	    int temp = m_pData[i];
	    m_pData[i] = m_pData[GetSize()-i-1];
	    m_pData[GetSize()-i-1] = temp;
	    }
	  }
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Sets all elements to the value specified.*/
flag CIArray::SetAll(int d)
  {
  for (long i=0; i<GetSize(); i++)
    m_pData[i] = d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Adds the specified value to each element.*/
flag CIArray::OpAdd(int d)
  {
  for (long i=0; i<GetSize(); i++)
    m_pData[i] += d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Adds the specified vector to itself. Vector sizes MUST match.*/
flag CIArray::OpAdd(CIArray &v, int Scl, int Off)
  {
  ASSERT(GetSize() == v.GetSize()); //check that lengths are equal
  for (long i=0; i<GetSize(); i++)
    m_pData[i] += Scl*v.m_pData[i] + Off;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Subtracts the specified vector from itself. Vector sizes MUST match.*/
flag CIArray::OpSub(CIArray &v)
  {
  ASSERT(GetSize() == v.GetSize()); //check that lengths are equal
  for (long i=0; i<GetSize(); i++)
    m_pData[i] -= v.m_pData[i];
  return True;
  }

// -------------------------------------------------------------------------

flag CIArray::OpMult(int d)
  {
  for (long i=0; i<GetSize(); i++)
    m_pData[i] *= d;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Multiplies the specified vector by itself. Vector sizes MUST match.*/
flag CIArray::OpMult(CIArray &v)
  {
  ASSERT(GetSize() == v.GetSize()); //check that lengths are equal
  for (long i=0; i<GetSize(); i++)
    m_pData[i] *= v.m_pData[i];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Divides the specified vector into itself. Vector sizes MUST match.*/
flag CIArray::OpDiv(CIArray &v)
  {
  ASSERT(GetSize() == v.GetSize()); //check that lengths are equal
  for (long i=0; i<GetSize(); i++)
    m_pData[i] /= v.m_pData[i];
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Loads the vector from the specified file.*/
//flag CIArray::Load(rCVMLoadHelper H, //Helper class with all the options set
//                    CnvIndex CnvDC)       //conversion
//  {
//  m_Err = 0;
//  H.cVectorName[0]=0;
//  H.cCnvTxt[0]=0;
//  H.bGotName=False;
//  H.bGotCnv=False;
//
//  CTokenFile f(H.FName, H.IgnoreComments, False, False);
//  Strng s;
//  s = H.Seperator;
//  s += " \t";
//  f.SetSeperators(s());
//  if (f.Open())
//    {
//    SetSize(H.Rows);
//    long RowNo = 1;
//    while ((RowNo<H.StartRow) && (!f.AtEOF()))
//      {
//      f.GetNextLine();
//      RowNo++;
//      }
//    if (f.AtEOF())
//      {
//      m_Err = VMErrLocation;
//      return False;
//      }
//    long DoLen = H.Rows;
//    if (DoLen==0)
//      DoLen = LONG_MAX;
//    int NullVal = atof(H.NullStr);
//    long j = 0;
//    if (H.Vertical)
//      {
//      flag Done = False;
//      while ((j<DoLen) && (!Done) && (!f.AtEOF()))
//        {
//        long i = 1;
//        while ((i<H.StartCol) && (!Done))//(!f.AtEOF()) && (!f.AtLastToken()))
//          {
//          s = f.NextToken();
//          if (!f.AtLastToken() && s[0]!=H.Seperator)
//            f.NextToken();
//          i++;
//          if (f.AtLastToken() || f.AtEOF())
//            Done = True;
//          }
//        if (!Done)
//          {
//          s = f.NextToken();
//          if (s.Length()==0)
//            Done = True;
//          else 
//            {
//            // Check if not Valid # is it a name or a Conversion
//            if (s.StrSpn("+-0123456789e.*")<s.Length())
//              if (H.bTest4Name && !H.bGotName)
//                H.iNameColRow=RowNo;
//              else if (H.bTest4Cnv && !H.bGotCnv)
//                H.iCnvColRow=RowNo;
//
//            if (RowNo==H.iNameColRow)
//              {
//              strcpy(H.cVectorName, s());
//              H.bGotName=True;
//              }
//            else if (RowNo==H.iCnvColRow)
//              {
//              strcpy(H.cCnvTxt, s());
//              //sCnvTxt=s();
//              //if (s.Length()>0)
//              //  {
//              //  pCDataCnv pCnv=gs_CnvsMngr.FindSecCnv(iCnvDC, sCnvTxt());
//              //  if (pCnv==NULL && stricmp(sCnvTxt(), "(null)")!=0)
//              //    LogError("Vectors", 0, "Conversion %s not Found", s());
//              //  }
//              H.bGotCnv=True;
//              }
//            else
//              {
//              if (j>=GetSize())
//                SetSize(GetSize()+16);
//              if (s[0]==H.Seperator)
//                m_pData[j] = NullVal;
//              else if (s[0]=='*')
//                m_pData[j] = dNAN;
//              //else if (sCnvTxt.Length()>0)
//              //  SetNormal(j, atof(s()));
//              else
//                m_pData[j]=atof(s());
//              j++;
//              }
//            if (!f.AtLastToken())
//              f.GetNextLine();
//            RowNo++;
//            }
//          }
//        }
//      }
//    else
//      {
//      long i = 1;
//      while ((i<H.StartCol) && (!f.AtEOF()) && (!f.AtLastToken()))
//        {
//        s = f.NextToken();
//        if (!f.AtLastToken() && s[0]!=H.Seperator)
//          f.NextToken();
//        i++;
//        }
//      if (f.AtLastToken() || f.AtEOF())
//        {
//        m_Err = VMErrLocation;
//        return False;
//        }
//      flag Done = False;
//      while ((j<DoLen) && (!Done) && (!f.AtEOF()))
//        {
//        if (f.AtLastToken() || f.AtEOF())
//          Done = True;
//        else
//          {
//          s = f.NextToken();
//          if (j>=GetSize())
//            SetSize(GetSize()+16);
//          if (s[0]==H.Seperator)
//            m_pData[j] = NullVal;
//          else
//            m_pData[j] = atof(s());
//          j++;
//          if (!f.AtLastToken() && s[0]!=H.Seperator)
//            f.NextToken();
//          }
//        }
//      }
//    SetSize(j);
//    f.Close();
//    H.bTest4Name = False;
//    H.bTest4Cnv = False;
//    return True;
//    }
//  m_Err = VMErrFileOpen;
//  return False;
//  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CIArray& CIArray::operator=(const CIArray &v)
  {
  SetSize(v.GetSize());
  for (long i=0; i<GetSize(); i++)
    m_pData[i] = v.m_pData[i];
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:#R:True if the vectors are the same length and the elements are equal 
(element difference is less than the constant ZeroLimit).*/
flag CIArray::operator==(const CIArray &v)
  {
  if (GetSize() != v.GetSize())
    return False;
  for (long i=0; i<GetSize(); i++)
    if (m_pData[i] - v.m_pData[i] > ZeroLimit)
	    return False;
  return True;
  }

// -------------------------------------------------------------------------
/*#F:Normalises the vector and returns the original sum of all the elements .*/
int CIArray::Sum()
  {
  int total = 0;
  for (long i = 0; i < GetSize(); i++)
    total += m_pData[i];
  return total;
  };

// -------------------------------------------------------------------------
/*#F:Outputs the vector contents (format acording to options) to the debug file.*/
void CIArray::dbgDump( pchar Desc,  //Optional text description, default = ''
                        flag Horizontal) //Optional, default = True
  {
  dbgp("%s:[%d]", Desc, GetSize());
  if (Horizontal)
    {
    for (long i=0; i<Min(50,GetSize()); i++)
	  dbgp("%11.3g ", m_pData[i]);
  	if (GetSize()>5)
  	  dbgp("...");
    dbgpln("");
    }
  else
    {
    dbgpln("");
    for (long i=0; i<GetSize(); i++)
      dbgpln("%4d: %11.3g", i, m_pData[i]);
    }
  }

// ==========================================================================
//
//
//
// ==========================================================================

/*#F:#R:The largest int within the vector.*/
int CIMatrx::ScanMax()
  {
  int temp = (m_pData ? m_pData[0].ScanMax() : 0);
  for (long i=1; i<GetSize(); i++)
    temp=Max(temp,m_pData[i].ScanMax());
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:#R:The smallest int within the vector.*/
int CIMatrx::ScanMin()
  {
  int temp = (m_pData ? m_pData[0].ScanMin(): 0);
  for (long i=1; i<GetSize(); i++)
    temp=Min(temp,m_pData[i].ScanMin());
  return temp;
  }

// -------------------------------------------------------------------------
/*#F:Makes the vector a copy of the supplied vector.*/
CIMatrx& CIMatrx::operator=(const CIMatrx &v)
  {
  SetSize(v.GetSize());
  for (long i=0; i<GetSize(); i++)
    {
    //m_pData[i].SetSize(v.m_pData[i].GetSize());
    m_pData[i] = v.m_pData[i];
    }
  return *this;
  }

// -------------------------------------------------------------------------
/*#F:#R:True if the vectors are the same length and the elements are equal 
(element difference is less than the constant ZeroLimit).*/
flag CIMatrx::operator==(const CIMatrx &v)
  {
  if (GetSize() != v.GetSize())
    return False;
  for (long i=0; i<GetSize(); i++)
    if (!(m_pData[i] == v.m_pData[i]))
	    return False;
  return True;
  }

// ==========================================================================
//
//
//
// ==========================================================================

void CSArray::AddStrng(char * p)
  {
  Strng S(p);
  Add(S);
  }

// -------------------------------------------------------------------------
    
flag CSArray::SetAll(char * p)
  {
  for (long i=0; i<GetSize(); i++)
    m_pData[i] = p;
  return True;
  }

// -------------------------------------------------------------------------

CSArray& CSArray::operator=(const CSArray &v)
  {
  SetSize(v.GetSize());
  for (long i=0; i<GetSize(); i++)
    m_pData[i] = v.m_pData[i];
  return *this;
  };

// -------------------------------------------------------------------------

flag CSArray::operator==(const CSArray &v)
  {
  if (GetSize() != v.GetSize())
    return False;
  for (long i=0; i<GetSize(); i++)
    if (m_pData[i].XStrICmp(v.m_pData[i])==0)
	    return False;
  return True;
  };

// ==========================================================================
//
//
//
// ==========================================================================
