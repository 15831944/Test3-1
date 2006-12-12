////////////////////////////////////////////////////////////////////////////////////////////////////
//////
//////
#include <stdafx.h>

#ifndef LIMNDW
	#include "globals.h"
#endif
//#pragma optimize("", off)

#include "LimnModel_DiamondWizard_CommonFunctions.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///			
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


CLimn_ModelData_Common::CLimn_ModelData_Common()
  {
  }

CLimn_ModelData_Common::~CLimn_ModelData_Common()
  {
  for (int i=0; i<m_Defs.GetCount(); i++)
    delete m_Defs[i];
  }

//-----------------------------------------------------------------------------------------------------------

CLimn_ModelData_Base::CRefBase::CRefBase()
  {
  m_pBase=NULL;
  };

bool CLimn_ModelData_Base::CRefBase::Initialise(CLimn_ModelData_Base * pBase, int Count)
  {
  m_pBase = pBase;
  m_iParm = m_pBase->AddParms(Count); 

  return (m_pBase->m_Common.m_Defs[m_iParm]==NULL);
  //return (m_iParm>=m_pBase->m_Common.m_Defs.GetCount());
  };

//-----------------------------------------------------------------------------------------------------------

CLimn_ModelData_ParmDef::CLimn_ModelData_ParmDef()
  { 
  m_IsBool = false; 
  m_Scale  = 1.0; 
  m_iIndex = -1;
  };
CLimn_ModelData_ParmDef::CLimn_ModelData_ParmDef(LPCSTR Class, LPCSTR Tag, int Index, LPCTSTR CnvStr, bool IsBool, bool Hide, CArray<CLimn_ModelData_ParmHdr, CLimn_ModelData_ParmHdr&> & RqdHdrs) 
  {
  m_Class = "DW_";
  m_Class += Class;
  m_Tag = Tag;
  m_iIndex = Index;
  m_IsBool=IsBool;
  m_Hide=Hide;
  m_ArrayBegin=false;
  m_ArrayEnd=false;
  m_ArrayLen=0;

  for (int i=0; i<RqdHdrs.GetCount(); i++)
     m_Hdrs.Add(RqdHdrs[i]);
  RqdHdrs.SetSize(0);

#ifdef LIMNDW
  gs_Cnvs.Create(CnvStr, m_Cnv); 
  if (m_Cnv.m_Txt.GetLength()>0)
    { int xxx=0; }
  m_Scale = gs_Cnvs.Scale(m_Cnv);
#endif // LIMNDW
  };

CLimn_ModelData_ParmDef::CLimn_ModelData_ParmDef(LPCSTR Class, LPCSTR Tag, LPCSTR Index, LPCTSTR CnvStr, bool IsBool, bool Hide, CArray<CLimn_ModelData_ParmHdr, CLimn_ModelData_ParmHdr&> & RqdHdrs) 
  {
  m_Class = "DW_";
  m_Class += Class;
  m_Tag = Tag;
  m_iIndex = -1;
  m_sIndex = Index;
  m_IsBool=IsBool;
  m_Hide=Hide;
  m_ArrayBegin=false;
  m_ArrayEnd=false;
  m_ArrayLen=0;

  for (int i=0; i<RqdHdrs.GetCount(); i++)
     m_Hdrs.Add(RqdHdrs[i]);
  RqdHdrs.SetSize(0);

#ifdef LIMNDW
  gs_Cnvs.Create(CnvStr, m_Cnv); 
  if (m_Cnv.m_Txt.GetLength()>0)
    { int xxx=0; }
  m_Scale = gs_Cnvs.Scale(m_Cnv);
#endif // LIMNDW
  };
//-----------------------------------------------------------------------------------------------------------

CLimn_ModelData_Base::CBooleanRef::CBooleanRef() {};
void CLimn_ModelData_Base::CBooleanRef::Initialise(CLimn_ModelData_Base * pBase, LPCSTR Tag, const bool DefaultValue, bool Hide)
  {
  if (CRefBase::Initialise(pBase, 1))
    m_pBase->m_Defs[m_iParm]=new CLimn_ModelData_ParmDef("", Tag, -1, "", true, Hide, m_pBase->m_RqdHdrs);
  if (m_pBase->m_LocalData)
    m_pBase->m_Data[m_iParm]=DefaultValue?1:0;
  };

//-----------------------------------------------------------------------------------------------------------

CLimn_ModelData_Base::CDoubleRef::CDoubleRef() {};
void CLimn_ModelData_Base::CDoubleRef::Initialise(CLimn_ModelData_Base * pBase, LPCSTR Tag, const double DefaultValue, LPCTSTR CnvStr, bool Hide) 
  {
  if (CRefBase::Initialise(pBase, 1))
    m_pBase->m_Defs[m_iParm]=new CLimn_ModelData_ParmDef("", Tag, -1, CnvStr, false, Hide, m_pBase->m_RqdHdrs);
  if (m_pBase->m_LocalData)
    m_pBase->m_Data[m_iParm]=DefaultValue/m_pBase->m_Defs[m_iParm]->m_Scale;
  };

//-----------------------------------------------------------------------------------------------------------

#if LIMNDW
	static bool GetIndexTag(LPCTSTR Tag, const CLimn_ModelData_Base::eDataIndex DI, int i, CString & Tg)
	{
	switch (DI)
		{
		case CLimn_ModelData_Base::DI_None:  Tg.Format("%i",i);                              return false;
		case CLimn_ModelData_Base::DI_OSz:   Tg.Format("%s",gs_DWCfg.OreSizeText(i));        return true;
		case CLimn_ModelData_Base::DI_DSz:   Tg.Format("%s",gs_DWCfg.DiamondSizeText(i));    return true;
		case CLimn_ModelData_Base::DI_SG:    Tg.Format("%s",gs_DWCfg.SGTextShort(i));        return true;
		}
	Tg="?";
	return true;
	}

	//-----------------------------------------------------------------------------------------------------------

	static bool GetIndexTag(LPCTSTR Tag, const CLimn_ModelData_Base::eDataIndex DI0, const CLimn_ModelData_Base::eDataIndex DI1, int i0, int i1, CString & Tg)
	{
	CString Tg0;
	switch (DI0)
		{
		case CLimn_ModelData_Base::DI_None:  Tg0.Format("%i",i0);                              break;
		case CLimn_ModelData_Base::DI_OSz:   Tg0.Format("%s",gs_DWCfg.OreSizeText(i0));        break;
		case CLimn_ModelData_Base::DI_DSz:   Tg0.Format("%s",gs_DWCfg.DiamondSizeText(i0));    break;
		case CLimn_ModelData_Base::DI_SG:    Tg0.Format("%s",gs_DWCfg.SGTextShort(i0));        break;
		}
	switch (DI1)
		{
		case CLimn_ModelData_Base::DI_None:  Tg.Format("%s/%i",Tg0,i1);                            return true;
		case CLimn_ModelData_Base::DI_OSz:   Tg.Format("%s/%s",Tg0,gs_DWCfg.OreSizeText(i1));      return true;
		case CLimn_ModelData_Base::DI_DSz:   Tg.Format("%s/%s",Tg0,gs_DWCfg.DiamondSizeText(i1));  return true;
		case CLimn_ModelData_Base::DI_SG:    Tg.Format("%s/%s",Tg0,gs_DWCfg.SGTextShort(i1));      return true;
		}
	Tg="?";
	return true;
	}

#else
			/// Limn specific dummy versions of GetIndexTag
	static bool GetIndexTag(LPCTSTR Tag, const CLimn_ModelData_Base::eDataIndex DI, int i, CString & Tg)
	{
		return false ;
	}
	static bool GetIndexTag(LPCTSTR Tag, const CLimn_ModelData_Base::eDataIndex DI0, const CLimn_ModelData_Base::eDataIndex DI1, int i0, int i1, CString & Tg)
	{
		return false ;
	}
#endif // LIMNDW

//-----------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------

CLimn_ModelData_Base::CVectorRef::CVectorRef() {};
void CLimn_ModelData_Base::CVectorRef::Initialise(CLimn_ModelData_Base * pBase, LPCTSTR Class, int Len, const eDataIndex DI, LPCSTR Tag, const double DefaultValue, LPCTSTR CnvStr, bool Hide)
  {
  pBase->MarkMatrix(Tag, Len, 1);

  if (CRefBase::Initialise(pBase, Len))
    {
    CString Tg;
    //int iFirst=m_pBase->m_Defs.GetCount();
    for (int i=0; i<Len; i++)
      {
      if (GetIndexTag(Tag, DI, i, Tg))
        m_pBase->m_Defs[m_iParm+i]=new CLimn_ModelData_ParmDef(Class, Tag, Tg, CnvStr, false, Hide, m_pBase->m_RqdHdrs);
      else
        m_pBase->m_Defs[m_iParm+i]=new CLimn_ModelData_ParmDef(Class, Tag, i, CnvStr, false, Hide, m_pBase->m_RqdHdrs);
      }
    m_pBase->m_Defs[m_iParm]->m_ArrayLen=Len;
    m_pBase->m_Defs[m_iParm]->m_ArrayBegin=true;
    m_pBase->m_Defs[m_iParm+Len-1]->m_ArrayEnd=true;
    }
  if (m_pBase->m_LocalData)
    {
    for (int i=0; i<Len; i++)
      m_pBase->m_Data[m_iParm+i]=DefaultValue/m_pBase->m_Defs[m_iParm+i]->m_Scale;
    }
  };

void CLimn_ModelData_Base::CVectorRef::Initialise(CLimn_ModelData_Base * pBase, LPCTSTR Class, int Len, const eDataIndex DI, LPCSTR Tag, const double * DefaultValue, LPCTSTR CnvStr, bool Hide)
  {
  pBase->MarkMatrix(Tag, Len, 1);

  if (CRefBase::Initialise(pBase, Len))
    {
    CString Tg;
    //int iFirst=m_pBase->m_Defs.GetCount();
    for (int i=0; i<Len; i++)
      {
      if (GetIndexTag(Tag, DI, i, Tg))
        m_pBase->m_Defs[m_iParm+i]=new CLimn_ModelData_ParmDef(Class, Tag, Tg, CnvStr, false, Hide, m_pBase->m_RqdHdrs);
      else
        m_pBase->m_Defs[m_iParm+i]=new CLimn_ModelData_ParmDef(Class, Tag, i, CnvStr, false, Hide, m_pBase->m_RqdHdrs);
      }
    m_pBase->m_Defs[m_iParm]->m_ArrayLen=Len;
    m_pBase->m_Defs[m_iParm]->m_ArrayBegin=true;
    m_pBase->m_Defs[m_iParm+Len-1]->m_ArrayEnd=true;
    }
  if (m_pBase->m_LocalData)
    {
    for (int i=0; i<Len; i++)
      m_pBase->m_Data[m_iParm+i]=DefaultValue ? DefaultValue[i]/m_pBase->m_Defs[m_iParm+i]->m_Scale:0.0;
    }
  };

//-----------------------------------------------------------------------------------------------------------

CLimn_ModelData_Base::CMatrixRef::CMatrixRef() {};
void CLimn_ModelData_Base::CMatrixRef::Initialise(CLimn_ModelData_Base * pBase, LPCTSTR Class, int Rows, int Cols, const eDataIndex RDI, const eDataIndex CDI, LPCSTR Tag, const double DefaultValue, LPCTSTR CnvStr, bool Hide)
  {
  pBase->MarkMatrix(Tag, Rows, Cols);
  
  m_nCols=Cols;
  if (CRefBase::Initialise(pBase, Rows*Cols))
    {
    CString Tg;
    for (int r=0; r<Rows; r++)
      {
      for (int c=0; c<Cols; c++)
        {
        GetIndexTag(Tag, RDI, CDI, r, c, Tg);
        int i=r*Cols+c;
        m_pBase->m_Defs[m_iParm+i]=new CLimn_ModelData_ParmDef(Class, Tag, Tg, CnvStr, false, Hide, m_pBase->m_RqdHdrs);
        }
      }
    m_pBase->m_Defs[m_iParm]->m_ArrayLen=Rows*Cols;
    m_pBase->m_Defs[m_iParm]->m_ArrayBegin=true;
    m_pBase->m_Defs[m_iParm+(Rows*Cols)-1]->m_ArrayEnd=true;
    }
  if (m_pBase->m_LocalData)  
    {
    for (int r=0; r<Rows; r++)
      {
      for (int c=0; c<Cols; c++)
        {
        int i=r*Cols+c;
        m_pBase->m_Data[m_iParm+i]=DefaultValue/m_pBase->m_Defs[m_iParm+i]->m_Scale;
        }
      }
    }
  };

void CLimn_ModelData_Base::CMatrixRef::Initialise(CLimn_ModelData_Base * pBase, LPCTSTR Class, int Rows, int Cols, const eDataIndex RDI, const eDataIndex CDI, LPCSTR Tag, const double * DefaultValue, LPCTSTR CnvStr, bool Hide)
  {
  pBase->MarkMatrix(Tag, Rows, Cols);
  
  m_nCols=Cols;
  if (CRefBase::Initialise(pBase, Rows*Cols))
    {
    CString Tg;
    int iFirst=m_pBase->m_Defs.GetCount();
    for (int r=0; r<Rows; r++)
      {
      for (int c=0; c<Cols; c++)
        {
        GetIndexTag(Tag, RDI, CDI, r, c, Tg);
        int i=r*Cols+c;
        m_pBase->m_Defs[m_iParm+i]=new CLimn_ModelData_ParmDef(Class, Tag, Tg, CnvStr, false, Hide, m_pBase->m_RqdHdrs);
        }
      }
    m_pBase->m_Defs[m_iParm]->m_ArrayLen=Rows*Cols;
    m_pBase->m_Defs[m_iParm]->m_ArrayBegin=true;
    m_pBase->m_Defs[m_iParm+(Rows*Cols)-1]->m_ArrayEnd=true;
    }
  if (m_pBase->m_LocalData)
    {
    for (int r=0; r<Rows; r++)
      {
      for (int c=0; c<Cols; c++)
        {
        int i=r*Cols+c;
        m_pBase->m_Data[m_iParm+i]=DefaultValue ? DefaultValue[i]/m_pBase->m_Defs[m_iParm+i]->m_Scale:0.0;
        }
      }
    }
  };

//-----------------------------------------------------------------------------------------------------------

CLimn_ModelData_Base::CLimn_ModelData_Base(CLimn_ModelData_Common * pCommon, double * pData/*=NULL*/) : \
m_Common(*pCommon),
m_Defs(pCommon->m_Defs),
m_RqdHdrs(pCommon->m_RqdHdrs)    
  {
  m_LocalData  = pData==NULL;
  m_pData      = pData;
  m_nDataCount = 0;
  }

CLimn_ModelData_Base::~CLimn_ModelData_Base(void)
  {
  }

void CLimn_ModelData_Base::Initialise()
  {
  m_nDataCount = 0;
  }

void CLimn_ModelData_Base::MarkText(LPCTSTR Text)
  {
  m_RqdHdrs.Add(CLimn_ModelData_ParmHdr(Text));
  };

void CLimn_ModelData_Base::MarkMatrix(LPCTSTR Header, int Rows, int Cols)
  {
  m_RqdHdrs.Add(CLimn_ModelData_ParmHdr(Header, Rows, Cols));
  }

void CLimn_ModelData_Base::MarkParameterGap(int Count)
  {
  int Start=AddParms(Count); 
  for (int i=0; i<Count; i++)
    {
    m_Defs[Start+i]=NULL;
    }
  }

int CLimn_ModelData_Base::AddParms(int Count)
  {
  int Start = m_nDataCount;
  m_nDataCount += Count;

  if (m_Defs.GetCount()<m_nDataCount)
    m_Defs.SetSize(m_nDataCount,64);

  if (m_LocalData && m_Data.GetCount()<m_nDataCount)
    {
    m_Data.SetSize(m_nDataCount);
    m_pData = &m_Data[0];
    }
  return Start;
  }

#ifndef LIMNDW  //////////////////Limn versions of SysCAD data exchange class method implementations
void CLimn_ModelData_Base::BuildDataFields(MDataDefn & DD)
  {
  }
bool CLimn_ModelData_Base::ExchangeDataFields(MDataChange & DX)
  {
	  return true ;
  }
#endif //LIMNDW

#ifdef LIMNDW  //////////////////SysCAD data exchange class method implementations
void CLimn_ModelData_Base::BuildDataFields(MDataDefn & DD)
  {

  DD.Text("Parameters...");
  //DD.Text("");

  bool InArray=false;
  int m_ColCount=0;
  int m_ValCount=0;
  int m_iCol=0;
  int m_iRow=0;
  for (int i=0; i<DataCount(); i++)
    {
    if (m_Defs[i])
      {
      CLimn_ModelData_ParmDef & D = *m_Defs[i];

      for (int j=0; j<D.m_Hdrs.GetCount(); j++)
        {
        CLimn_ModelData_ParmHdr & H=D.m_Hdrs[j];
        CString Txt(H.m_Text);
        if (H.m_nRows>1 && H.m_nCols>1)
          {
          Txt= "Matrix: ";
          m_ColCount=H.m_nCols;
          m_ValCount=H.m_nRows*H.m_nCols; 
          m_iCol=0;
          m_iRow=0;
          }
        else if (H.m_nRows>1 || H.m_nCols>1)
          {
          Txt= "Vector: ";
          m_ColCount=H.m_nCols;
          m_ValCount=H.m_nRows*H.m_nCols; 
          m_iCol=0;
          m_iRow=0;
          }
        Txt+=H.m_Text;
        DD.Page(H.m_Text, true);
        DD.Text(Txt);
        }

      if (!D.m_Hide)
        {
        MD_Flags F=MF_PARAMETER;
        MD_Flags FS=0;
        MD_Flags FE=0;
        if (m_ValCount>0)
          {
          if (m_iCol==0)
            FS=MF_StartRow;
          if (++m_iCol>=m_ColCount)
            {
            m_iCol=0;
            m_iRow++;
            }
          if (--m_ValCount==0)
            {
            FE=MF_EndRows;
            }
          }

        if (D.m_ArrayBegin)
          {
          InArray=true;
          //CString Cls;
          //Cls.Format("DW_%s",D.m_Class);
          DD.ArrayBegin(D.m_Class, D.m_Tag, D.m_ArrayLen,0);

          gs_Dbg.PrintLn("ArrayBegin %s %s %i >>>>>>>>>>>", D.m_Class, D.m_Tag, D.m_ArrayLen);
          }

        if (InArray)
          {
          if (D.m_iIndex>=0)
            DD.ArrayElementStart(D.m_iIndex, FS);
          else
            DD.ArrayElementStart(D.m_sIndex, FS);
          gs_Dbg.PrintLn("ArrayEle  %s %i %s", D.m_Tag, D.m_iIndex, D.m_sIndex);
          CString S;
          //S.Format("%s[%s]", D.m_Tag, D.m_Index);
          //S=D.m_Index;
          S="V";
          if (D.m_IsBool)
            DD.Bool  (S, "",  1+i,  F);
          else
            DD.Double(S, "",  1+i,  F, D.m_Cnv);
          DD.ArrayElementEnd(FE);
          }
        else
          {
          if (D.m_IsBool)
            DD.Bool  (D.m_Tag, "",  1+i,  F|FS|FE);
          else
            DD.Double(D.m_Tag, "",  1+i,  F|FS|FE, D.m_Cnv);
          }
        
        if (D.m_ArrayEnd)
          {
          DD.ArrayEnd(0);
          gs_Dbg.PrintLn("ArrayEnd   %s %s <<<<<<<<<<<", "", "");
          InArray=false;
          }
        }
      }
    }
  };

//-----------------------------------------------------------------------------------------------------------

bool CLimn_ModelData_Base::ExchangeDataFields(MDataChange & DX)
  {
  if (DX.Handle>=1 && DX.Handle<=DataCount())
    {
    int i=DX.Handle-1;
    CLimn_ModelData_ParmDef & D = *m_Defs[i];
    if (D.m_IsBool)
      {
      if (DX.HasReqdValue)
        m_Data[i] = DX.Bool ? 1.0:0.0;
      DX.Bool = m_Data[i]>0.5;
      }
    else
      {
      if (DX.HasReqdValue)
        m_Data[i] = DX.Double;
      DX.Double = m_Data[i];
      }
    return true;
    }
  return false;
  };

#endif // LIMNDW



//////     
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///			Calculation functions for Limn Diamond Wizard models
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///					MeanEfficiencyPartitionFraction
///						Calculates a mean partition fraction for a size interval by linear interpolation over 29 sub intervals
double MeanEfficiencyPartitionFraction(double Rf, double Alpha, double D50c, double intervalTopSize, double intervalBottomSize)
{
    double partitionFraction = 0.0 ;
    double increment ;
    double meanSize ;
    double topSize ;
    double bottomSize ;
    double alpha_d_d50c ;

    increment = (intervalTopSize - intervalBottomSize) / 30.0 ;
    topSize = intervalTopSize ;
    
    for ( int i = 1 ; i < 30 ; i++ )       
	{
        bottomSize = topSize - increment ;
        
        meanSize = sqrt(topSize * bottomSize) ;
    
        alpha_d_d50c = (Alpha * meanSize / D50c) ;  // intermediate calc
        
        if (alpha_d_d50c < 700)                     // clip to reasonable values
		{
            partitionFraction = partitionFraction + ((1 - Rf) * ((exp(Alpha) - 1) / (exp(alpha_d_d50c) + exp(Alpha) - 2))) ;
		}
        
        topSize = bottomSize ;
	}
	
    return partitionFraction / 29.0	;				// return the average

}
///					MeanTrompPartitionFraction
///						Calculates a mean partition fraction for a density interval by linear interpolation over 29 sub intervals
double MeanTrompPartitionFraction(double EpD, double Rho50D, double intervalTopSG, double intervalBottomSG)
{
    double partitionFraction = 0.0 ;
    double increment ;
    double meanSG ;
    double topSG ;
    double bottomSG ;

    increment = (intervalTopSG - intervalBottomSG) / 30.0 ;
    topSG = intervalTopSG ;
    
    for ( int i = 1 ; i < 30 ; i++ )       
	{
        bottomSG = topSG - increment ;
        
        meanSG = (topSG + bottomSG) / 2.0 ;
    
        partitionFraction = partitionFraction + (1 - 1 / (1 + exp(1.098 * (Rho50D - meanSG) / EpD))) ;
    
        topSG = bottomSG ;
	}
	
    return partitionFraction / 29.0	;				// return the average

}



///
///
///     Function to calculate the transformation matrix X ( p = X.f )
///     according to the Whiten Crusher Model described in
///     the JKMRC monograph:
///         Napier-Munn et al.
///         "Mineral Comminution Circuits - Their Operation and Optimisation",
///         JKMRC monograph 1996,
///         p138ff
///
void WhitenCrusherTransformationMatrix(const LowerTriangularMatrix& B, const DiagonalMatrix& C, Matrix& X)
{
    int matrixSize = B.Nrows() ;
    IdentityMatrix I(matrixSize);					 
						            /// do the matrix calculations
	X = (I - C) * (I - (B * C)).i() ;

}
///
///
///     Function to calculate the Classification Function
///     for the Whiten Crusher Model.
///     Refer:
///         Napier-Munn et al.
///         "Mineral Comminution Circuits - Their Operation and Optimisation",
///         JKMRC monograph 1996,
///         p141
///
void WhitenCrusherClassificationFunction(int nSizes,
										 double* Size, 
                                         double K1, 
                                         double K2, 
                                         double K3,
										 DiagonalMatrix& C)
{
	double cVal ;
    for ( int i = 0 ; i < nSizes ; i++ )
	{
		if ( Size[i] < K1 )
		{
            cVal = 0.0 ;
		}
		else if ( Size[i] > K2 )
		{
            cVal = 1.0 ;
		}
        else
		{
            cVal = 1 - pow(((K2 - Size[i]) / (K2 - K1)), K3) ;
		}

		C(i+1,i+1) = cVal ;

	}
}
///
///
///     Function to calculate the value of the breakage distribution
///     function given T10 and a table ot T10 versus Tn
///     Refer:
///         Napier-Munn et al.
///         "Mineral Comminution Circuits - Their Operation and Optimisation",
///         JKMRC monograph 1996,
///         p143
///
///     Uses spline interpolation
///
void WhitenCrusherBreakageFunction(int nSizes, 
								   double* Size,
                                    double requiredT10,
                                     const Matrix& T10_V_Tn,
									  LowerTriangularMatrix& B)
{
    int nT10 = 5 ;
    int nTn = 7 ;
    
    int iT10 ;
    int iTn ;
    
    int i ;
    int j ;
    int k ;

    double S0 ;
    
    double interpTn[7] ;			 /// extra columns at T1000, T10=0 & T1, T10=100 to help extrapolation

									/// Pin down the end of the curve at T100000, T10=0 to help extrapolation
									/// Calculate 1/Tn fractions
									/// Pin down the end of the curve at T1, T10=100 to help extrapolation
	double fractionTn[7] =  {1.0 / 100000.0, 
							 1.0 / 75.0,     
							 1.0 / 50.0,
                             1.0 / 25.0,
							 1.0 / 4.0,
                             1.0 / 2.0,
							 1.0 } ;

	double X[5] = { 0.0, 10.0, 20.0, 30.0, 40.0 } ;               /// T10 data to interpolate on

	double Y[5] ;
    
    double cumulativeB[100] ;

    double fractionSize ;
    
    CSpline Spline ;
    
                            /// Interpolate to get a range of Tn data at the given T10

	for ( iTn = 0 ; iTn < nTn - 2 ; iTn++ )    /// Do all normal Tn values
	{
        for ( iT10 = 0 ; iT10 < nT10 ; iT10++ )             /// This particular Tn data slice
		{
            Y[iT10] = T10_V_Tn(iT10+1, iTn+1) ;
		}
        
        Spline.Make( X, Y, nT10 ) ;                             /// Calculate the spline
        
        interpTn[iTn+1] = Spline.Evaluate(requiredT10) ;     /// Evaluate the spline
	}
    
    interpTn[0] = 0.0000001 ;
    interpTn[6] = 100.0 ;

                            /// Then interpolate to get the CUMULATIVE breakage distibution function by size
    Spline.Make( fractionTn, interpTn, nTn ) ;                /// Calculate the spline
    S0 = Size[0] ;
    cumulativeB[0] = 1.0 ;                                     /// 100% passing top size
    for ( i = 1 ; i < nSizes ; i++)
	{
        fractionSize = Size[i] / S0 ;
        cumulativeB[i] = Spline.Evaluate(fractionSize) / 100.0 ;  /// Evaluate the spline for all size ratios
	}
        
	ColumnVector Bzero(nSizes) ;
	Bzero = 0.0 ;

	for ( i = 2 ; i <= nSizes ; i++ )	/// first entry in breakage distribution function always 0 (nothing breaks into the top size), so start one down
	{
		double bdv = cumulativeB[i - 2] - cumulativeB[i - 1] ;
		if ( bdv < 0.0 ) bdv = 0.0 ;				//Modification by Kurt Petersen 04/06/2002
		Bzero(i) = bdv ;			    /// convert cumulative to actual breakage distribution function
	}

	for ( i = 1 ; i <= nSizes ; i++ )
	{
		k = 1 ;
		for ( j = i ; j <= nSizes ; j++ )
		{
			B(j,i) = Bzero(k++) ;         /// fill columns of lower triangular
		}
	}

}
///
///     Function to perform the redistribution of densimetrics to match a feed stream
///     (this is a kludge to account for the fact that densimetrics are measured at
//       feed to the DMS. It also simulates liberation)
///

void RedistributeOreDensimetricToFeedDistribution(CDiamondWizardConfiguration* thisConfig,
												  double oreFlow,
												  Matrix& product,
												  ColumnVector& meanSizeDistribution,
												  ColumnVector& meanSGDistribution)
{    
    int iSG ;
    int iOSz ;

	int nOSz = thisConfig->nOreSizes() ;
	int nSGs  = thisConfig->nSGs() ;

	double sizeFlow ;

	if ( oreFlow > 0.0 )
	{
													// work out the mean size distribution
		for ( iOSz = 1 ; iOSz <= nOSz ; iOSz++ )
		{
			sizeFlow = 0.0 ;
			for ( iSG = 1 ; iSG <= nSGs ; iSG++ )
			{
	            sizeFlow += oreFlow * product(iOSz, iSG) * meanSGDistribution(iSG) / 10000.0 ;
			}
	        if ( sizeFlow < 0.0000001 ) sizeFlow = 0.0 ;
			meanSizeDistribution(iOSz) = 100.0 * sizeFlow / oreFlow ;
		}

		for ( iSG = 1 ; iSG <= nSGs ; iSG++ )
		{
			meanSGDistribution(iSG) = 0.0 ;        // new SG Distribution
			for ( iOSz = 1 ; iOSz <= nOSz ; iOSz++ )
			{
				meanSGDistribution(iSG) += thisConfig->ReferenceDensimetrics(iOSz-1, iSG-1) * meanSizeDistribution(iOSz) / 100.0  ;
			}
		}

		for ( iSG = 1 ; iSG <= nSGs ; iSG++ )		// redistribute feed densimetrics to new size and density distributions
		{
			for ( iOSz = 1 ; iOSz <= nOSz ; iOSz++ )
			{
				if ( meanSGDistribution(iSG) > 0.0 )
				{
					product(iOSz, iSG) = meanSizeDistribution(iOSz) * thisConfig->ReferenceDensimetrics(iOSz-1, iSG-1) / meanSGDistribution(iSG) ;
				}
				else
				{
					product(iOSz, iSG) = 0.0 ;
				}
			}
		}
	}	
}
/*
'''
'''     Function to return the diamond allocation as
'''     a result of a breakage process.
'''     i.e. Calculate p = x.f and allocate diamonds
'''     into the daughter fragments according to the
'''     distribution of diamonds in the parent sizes
'''
'''                 this function recalculates the breakage function
'''                 and transformation matrix for each SG, based on
'''                 differing T10 values for each SG
'''
Function SG_Dependent_DiamondMatrixCrusherModel(diamondFeedMass As Range, _
                                                 Size As Range, _
                                                  requiredT10 As Range, _
                                                   T10_V_Tn As Range, _
                                                    ClassificationFunction As Range) As Variant
    Dim diamondProduct() As Double
    
    Dim X As Variant                ''' transformation matrix
    Dim B As Variant                ''' breakage distribution function
    
    
    Dim productSum As Double
    Dim feedSum As Double
    
    Dim iDSz As Integer                 ''' Counters
    Dim iOSz As Integer
    Dim iPOsz As Integer
    Dim iSG As Integer
    Dim iOSindex As Integer             ''' Indices into 2D representation
    Dim iDSindex As Integer             ''' of 3D deportment matrix
    Dim iPOSindex As Integer            ''' Ore Size Index into Product matrix
    
    On Error Resume Next        '' ignore div by zero errors etc
    
    ReDim diamondProduct((nOreSizes * 2), (nDiamondSizes * Int((nSGs + 1) / 2)))
    
    
    For iSG = 1 To nSGs
                            ''''''''''''' modifications to calculate SG specific Breakage Function
                            ''''''''''''' and transformation matrix "on the fly"
                                           '' calculate b for one of the row of T10 values from spreadsheet
        B = WhitenCrusherBreakageFunction(Size, requiredT10(1, iSG), T10_V_Tn)
        X = VBA_WhitenCrusherTransformationMatrix(B, ClassificationFunction)
        
        For iDSz = 1 To nDiamondSizes
            iDSindex = DiamondSizeIndex(iDSz, iSG)
            
            productSum = 0
            feedSum = 0
            
            For iPOsz = 1 To nOreSizes - 1
                iPOSindex = OreSizeIndex(iPOsz, iSG)
                diamondProduct(iPOSindex, iDSindex) = 0
            
                For iOSz = 1 To iPOsz
                    iOSindex = OreSizeIndex(iOSz, iSG)
                    diamondProduct(iPOSindex, iDSindex) = _
                        diamondProduct(iPOSindex, iDSindex) + _
                            (diamondFeedMass(iOSindex, iDSindex) * _
                                X(iPOsz, iOSz))
                Next iOSz
                    
                productSum = productSum + diamondProduct(iPOSindex, iDSindex)
                feedSum = feedSum + diamondFeedMass(iOSindex, iDSindex)

            Next iPOsz
                                ''' Calculate finest fraction by difference
            iOSindex = OreSizeIndex(nOreSizes, iSG)
            feedSum = feedSum + diamondFeedMass(iOSindex, iDSindex)
            
            diamondProduct(iOSindex, iDSindex) = feedSum - productSum
            
            
        Next iDSz
    Next iSG
    
    SG_Dependent_DiamondMatrixCrusherModel = diamondProduct
    
End Function

'''
'''     See Kurt Petersen's workbook
'''     "comminution solution update.xls" 26Feb2002
'''     for the methodology!
'''
'''
Function DiamondBreakageModel(diamondFeedMass As Range, _
                               oreFeedDistribution As Range, _
                                oreProductDistribution As Range, _
                                 B As Range, _
                                  s As Range, _
                                   Tau As Double) As Variant
    Dim diamondProduct() As Double
    Dim FS() As Double
    
    Dim brokenOut() As Double
    Dim brokenSum() As Double
    Dim totalBroken() As Double
    Dim brokenIn As Double
    Dim productSum As Double
    Dim feedSum As Double
    
    Dim iDSz As Integer                 ''' Counters
    Dim iOSz As Integer
    Dim j As Integer
    Dim iSG As Integer
    Dim iOSindex As Integer             ''' Indices into 2D representation
    Dim iDSindex As Integer             ''' of 3D deportment matrix
    Dim jOSindex As Integer
    
    On Error Resume Next        '' ignore div by zero errors etc
    
    ReDim diamondProduct((nOreSizes * 2), (nDiamondSizes * Int((nSGs + 1) / 2)))
    ReDim SPon1pTS(nOreSizes)
        
    ReDim brokenOut(nOreSizes, nOreSizes)
    ReDim brokenSum(nOreSizes)
    ReDim totalBroken(nOreSizes)
    
    For iSG = 1 To nSGs
        
        For iOSz = 1 To nOreSizes
                                                                ''' Modified model 27Feb02
                                                                    ''' calculate the total broken
            totalBroken(iOSz) = oreFeedDistribution(iOSz, iSG) * (1 - (1 / (1 + Tau * s(iOSz, 1))))
                                                                    ''' initialise the matrix
            For j = 1 To nOreSizes
                brokenOut(iOSz, j) = 0#
            Next j
            
        Next iOSz
            
        totalBroken(nOreSizes) = 0#                             ''' nothing broken from lowest size!
        
        
        For iOSz = 2 To (nOreSizes - 1)         ''' distribution of material broken
            For j = 1 To (iOSz - 1)          ''' from each size fraction
                brokenOut(iOSz, j) = Tau * B(iOSz, j) * s(j, 1) * oreProductDistribution(j, iSG) / (1 + (Tau * s(iOSz)))
            Next j
        Next iOSz
        
                                                ''' calculate the last by difference
        For j = 1 To nOreSizes
            brokenSum(j) = 0#
            For iOSz = (j + 1) To (nOreSizes - 1)
                brokenSum(j) = brokenSum(j) + brokenOut(iOSz, j)
            Next iOSz
        Next j
        
        For j = 1 To nOreSizes - 1
            brokenOut(nOreSizes, j) = totalBroken(j) - brokenSum(j)
            brokenOut(nOreSizes, j) = brokenOut(nOreSizes, j)
        Next j
        
                                                ''' now do the allocation
        For iOSz = 1 To nOreSizes
        
            For iDSz = 1 To nDiamondSizes
                iDSindex = DiamondSizeIndex(iDSz, iSG)
            
                iOSindex = OreSizeIndex(iOSz, iSG)
                diamondProduct(iOSindex, iDSindex) = _
                    diamondFeedMass(iOSindex, iDSindex) * _
                       (1 - totalBroken(iOSz) / (oreFeedDistribution(iOSz, iSG) + 1E-31))
                       
                feedSum = feedSum + diamondFeedMass(iOSindex, iDSindex)
                       
                brokenIn = 0
                For j = 1 To (iOSz - 1)
                    jOSindex = OreSizeIndex(j, iSG)
                    brokenIn = brokenIn + (diamondFeedMass(jOSindex, iDSindex) * (brokenOut(iOSz, j) / (oreFeedDistribution(j, iSG) + 1E-31)))        '' + 1E-31 to avoid div 0)
                Next j
                    
                diamondProduct(iOSindex, iDSindex) = diamondProduct(iOSindex, iDSindex) + brokenIn
            
            Next iDSz
            
            If feedSum < 0.0000001 Then             '' Handle start up conditions
                    iOSindex = OreSizeIndex(iOSz, iSG)  '' If no feed, make no product!
                    diamondProduct(iOSindex, iDSindex) = 0#
            End If
        
        Next iOSz
    Next iSG
    
    DiamondBreakageModel = diamondProduct
    
End Function


*/
//////     
//
//
