//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include ".\psd.h"
//#include "optoff.h"

//===========================================================================


	DDBValueLst DDB_TestEntryType[]={
    {CPSDTestData::eTest_WeightPercent,        "Test Data"},
    {CPSDTestData::eTest_WeightPercentPassing, "Function"},
    {0}};

  DDBValueLst DDB_TestDataType[]={
    {CPSDTestData::eTest_WeightPercent,        "Wt  %"},
    {CPSDTestData::eTest_WeightPercentPassing, "Cum %"},
    {0}};

	DDBValueLst DDB_FunctionType[]={
    {CPSDTestData::eFunc_RR, "Rosin-Rammler"},
    {CPSDTestData::eFunc_GS, "Gaudin-Schuhmann"},
    {0}};


/*

' Function to determine the %passing a given size by interpolation
' from a given %passing size distribution
' Original code from RTD 1999

Function PercentPassing(SizeRequired As Double, SizeData As Object, PassingData As Object) As Double

    Dim n As Integer                            ' Number of points in distribution
    Dim i As Integer                            ' Current point in distribution

    Dim x1 As Double                            ' Interpolation stuff
    Dim x2 As Double
    Dim y1 As Double
    Dim y2 As Double
    Dim x As Double
    Dim y As Double
    Dim slope As Double
    
    ' Check arguments
    If IsError(SizeData(1)) Or IsError(PassingData(1)) Then
        PercentPassing = 0
        Exit Function
    End If
    
    ' Count number of items in distribution
    n = SizeData.Count
        
    ' Check if size required is outside of
    ' the given cumulative distribution
    If SizeRequired >= SizeData(1) Then         ' greater than largest size ?
        PercentPassing = PassingData(1)
        Exit Function
    ElseIf SizeRequired <= SizeData(n) Then     ' less than smallest size ?
        PercentPassing = PassingData(n)
        Exit Function
    End If
    
    ' Locate size above and below given size
    For i = 1 To n - 1
        x1 = SizeData(i)
        x2 = SizeData(i + 1)
        y1 = PassingData(i)
        y2 = PassingData(i + 1)

        If y1 >= 99.999999 Then y1 = 99.999999
        If y2 >= 99.999999 Then y2 = 99.999999
        If y1 <= 0.000001 Then y1 = 0.000001
        If y2 <= 0.000001 Then y2 = 0.000001
        
        If x1 = SizeRequired Then
            ' Exactly equal to existing size
            PercentPassing = PassingData(i)
            Exit Function
        ElseIf x1 > SizeRequired And x2 < SizeRequired Then
            ' Located between two existing sizes
            ' Construct RR interpolation (x1-x2) -> PassingData
            If (y1 = y2) Then
                PercentPassing = y1
                Exit Function
            Else
                x1 = Log(x1)
                x = Log(SizeRequired)
                x2 = Log(x2)
                y1 = Log(-Log(y1 / 100))
                y2 = Log(-Log(y2 / 100))
                slope = (y2 - y1) / (x2 - x1)
                y = y1 + (x - x1) * slope
                PercentPassing = 100 * Exp(-Exp(y))
                Exit Function
            End If
        End If
    Next i
    
    ' Still Here? Return an #Num error
    PercentPassing = CVErr(xlErrNum)

End Function
*/

//===========================================================================

CPSDTestData::CPSDTestData(void)
{

	m_eFuncType = eFunc_RR;
	//m_eActionType = eA_Params;
  m_eEntryType = eEntry_Test;
  m_eTestDataType = eTest_WeightPercent;
	m_lNFunctionData = 2;
	m_lNInterpData = 0;
  m_lNTestData = 0;

	// Rosin-Rammler Params
	m_dRR_a = 10.4;
	m_dRR_b = 1.298;

	// Gaudin-Schulman Params
	m_dGS_k = 22.9;
	m_dGS_m = 0.715;

	for (int i = 1; i < 16; i++ )
	{
		m_X[i] = 0.0;
		m_Y[i] = 0.0;
		m_Xr[i] = 0.0;
		m_Yr[i] = 0.0;
    m_Size_Wt_Percent[i] = 0.0;
    m_Wt_Percent[i] = 0.0;
    m_Size_Wt_Passing[i] = 0.0;
    m_Wt_Passing[i] = 0.0;

	}

}

//===========================================================================

CPSDTestData::~CPSDTestData(void)
{
}

//===========================================================================

CPSDTestData::eTestDataType CPSDTestData::GetTypeStringType(const Strng &s)

  {
    for (int i = 0; DDB_TestDataType[i].m_pStr != NULL; i++ )
      {

        Strng stype = DDB_TestDataType[i].m_pStr;
        if ( stype == (Strng)s)
          return (eTestDataType)DDB_TestDataType[i].m_lVal;
      }

    return(eTestDataType::eTest_WeightPercent);
  }

//===========================================================================

CPSDTestData::eEntryType  CPSDTestData::GetEntryStringType(const Strng &s)
  {
    for (int i = 0; DDB_TestEntryType[i].m_pStr != NULL; i++ )
      {

        Strng stype = DDB_TestEntryType[i].m_pStr;
        if ( stype == (Strng)s)
          return (eEntryType)DDB_TestEntryType[i].m_lVal;
      }

    return(eEntry_Test);
  }

//===========================================================================

CPSDTestData::eFunctionType  CPSDTestData::GetFunctionStringType(const Strng &s)
  {
    for (int i = 0; DDB_FunctionType[i].m_pStr != NULL; i++ )
      {

        Strng stype = DDB_FunctionType[i].m_pStr;
        if ( stype == (Strng)s)
          return (eFunctionType)DDB_FunctionType[i].m_lVal;
      }

    return(eFunc_RR);
  }

//===========================================================================

void CPSDTestData::RosinRammlerData(void)
{

	for (int i = 0 ; i < m_lNFunctionData ; i++ )
	{

		if ((m_dRR_a > 0.0) && ( m_dRR_b > 0.0 ) && ( m_X[i] > 0.0 ))
		{
			double sz = m_X[i];
			double Wr = /*100.0*/exp(-pow(sz/m_dRR_a,m_dRR_b));
		    m_Y[i] = 1.0 - Wr;
		}
	}

}

//===========================================================================

void CPSDTestData::GaudinSchuhmann(void)
{

	for (int i = 0 ; i < m_lNFunctionData ; i++ )
	{

		if ((m_dGS_k > 0.0) && ( m_dGS_m > 0.0 ) && ( m_X[i] > 0.0 ))
		{
			double sz = m_X[i];
			double Wp = pow(sz/m_dGS_k,m_dGS_m);
		    m_Y[i] = Wp;
		}
	}

}

//===========================================================================

double CPSDTestData::RosinRammlerPassing(double x)
{


		if ((m_dRR_a > 0.0) && ( m_dRR_b > 0.0 ) && ( x > 0.0 ))
		{
			double sz = x;
			double Wr = exp(-pow(sz/m_dRR_a,m_dRR_b));
		  return(1.0 - Wr);
		}

    return(0.0);

}

//===========================================================================

double CPSDTestData::GaudinSchuhmannPassing(double x)
{

		if ((m_dGS_k > 0.0) && ( m_dGS_m > 0.0 ) && ( x > 0.0 ))
		{
			double sz = x;
			double Wp = pow(sz/m_dGS_k,m_dGS_m);
		  return(Wp);
		}
    return(0.0);
}

//===========================================================================

double CPSDTestData::TestDataPercentPassingRR(double x)

{
  long NPass;
  double xmax = 0.0;
  double xmin = 0.0;
  double logx1 = 0.0;
  double logx2 = 0.0;
  double logx = 0.0;
  double loglogy1 = 0.0;
  double loglogy2 = 0.0;

  Sort();

  if ( x <= 0.0 ) return(0.0);

  if (m_eTestDataType == eTest_WeightPercent )
    {
      // Data has been entered as Weight Percent (Size Fractions)
      // Make sure data has been converted to percent passing
      WeightToPassing();
      NPass = m_lNTestData-1;
    }
  else
    {
      // Data is entered as % Passing
      NPass = m_lNTestData;
    }

  // Intervals are smallest to largest
  // Code assumes xi > xi+1
  // Need to add code that makes sure data is sorted as such
	for (int i = 0 ; i < (NPass-1) ; i++ )
	{
        double x1 = m_Size_Wt_Passing[i];
        double x2 = m_Size_Wt_Passing[i + 1];
        double y1 = m_Wt_Passing[i]*100.0;
        double y2 = m_Wt_Passing[i + 1]*100.0;

        if (x1 <= 0.0) x1 = 0.00000001;
        if (x2 <= 0.0) x2 = 0.00000001;
        if (y1 >= 99.999999) y1 = 99.999999;
        if (y2 >= 99.999999) y2 = 99.999999;
        if (y1 <= 0.000001)  y1 = 0.000001;
        if (y2 <= 0.000001)  y2 = 0.000001;
        
        // Record min and max data
        xmax = max(xmax,x1);
        xmax = max(xmax,x2);
        xmin = min(xmin,x1);
        xmin = min(xmin,x2);

        if (x1 == x)
          {
          // Exactly equal to existing size
          return(y1/100.0);
          }
        else if ( //(((x1 > x) && (x2 < x)))
                  (((x1 <= x) && (x2 >= x)))
                )
          {
          // Located between two existing sizes
          // Construct RR interpolation (x1-x2) -> PassingData
          if (y1 == y2)
            {
            return(y1/100.0);
            }
          else
            {
            logx1 = log(x1);
            logx =  log(x);
            logx2 = log(x2);
            loglogy1 = log(-log(y1 / 100));
            loglogy2 = log(-log(y2 / 100));
            double slope = (loglogy2 - loglogy1) / GTZ(logx2 - logx1);
            double y = loglogy1 + (logx - logx1) * slope;
            double passing = 100 * exp(-exp(y));
            return(passing/100.0);
            }

		    }
        else
          {
            ;
          }

	 }

  // Outside of test data range
  if (x >= xmax)
  {
      // Size is greater than largest size in sample data
      // Interpolate from last two know largest points

        double x1 = m_Size_Wt_Passing[NPass-2];
        double x2 = m_Size_Wt_Passing[NPass-1];
        double y1 = m_Wt_Passing[NPass-2]*100.0;
        double y2 = m_Wt_Passing[NPass-1]*100.0;

        if (x1 <= 0.0) x1 = 0.00000001;
        if (x2 <= 0.0) x2 = 0.00000001;
        if (y1 >= 99.999999) y1 = 99.999999;
        if (y2 >= 99.999999) y2 = 99.999999;
        if (y1 <= 0.000001)  y1 = 0.000001;
        if (y2 <= 0.000001)  y2 = 0.000001;

        logx1 = log(x1);
        logx =  log(x);
        logx2 = log(x2);
        loglogy1 = log(-log(y1 / 100));
        loglogy2 = log(-log(y2 / 100));
        double slope = (loglogy2 - loglogy1) / GTZ(logx2 - logx1);
        double y = loglogy1 + (logx - logx1) * slope;
        double passing = 100 * exp(-exp(y));
        if (passing > 100.0) passing = 100.0;
        return(passing/100.0);

        //return(1.0);
  }
  else
  {
      // Size is smaller than smallest size in sample data
      // Interpolate from smallest two points

        double x1 = m_Size_Wt_Passing[0];
        double x2 = m_Size_Wt_Passing[1];
        double y1 = m_Wt_Passing[0]*100.0;
        double y2 = m_Wt_Passing[1]*100.0;

        if (x1 <= 0.0) x1 = 0.00000001;
        if (x2 <= 0.0) x2 = 0.00000001;
        if (y1 >= 99.999999) y1 = 99.999999;
        if (y2 >= 99.999999) y2 = 99.999999;
        if (y1 <= 0.000001)  y1 = 0.000001;
        if (y2 <= 0.000001)  y2 = 0.000001;

        logx1 = log(x1);
        logx =  log(x);
        logx2 = log(x2);
        loglogy1 = log(-log(y1 / 100));
        loglogy2 = log(-log(y2 / 100));
        double slope = (loglogy2 - loglogy1) / GTZ(logx2 - logx1);
        double y = loglogy1 + (logx - logx1) * slope;
        double passing = 100 * exp(-exp(y));
        if (passing < 0.0) passing = 0.0;
        return(passing/100.0);

      //return(0.0);
  }
}

//===========================================================================

void CPSDTestData::WeightToPassing(void)

  {

  // Assumes sizes are entered lowest to largest

  // This method is only called if m_eTestDataType == eTest_WeightPercent
  // When the data is entered as WeightPercent then the converted passing data
  // is only of length m_lNTestData-1

  // Example:

  // Weight Percent Data
  // -125 125 200 500 1000 2000
  //   10   5   8   4   10   xx

  // Passing Data
  // 125 200 500 1000 2000
  //  10  15   23  27   37

  m_Wt_Passing[0] = m_Wt_Percent[0];
  m_Size_Wt_Passing[0] = m_Size_Wt_Percent[1];
	for (int i = 1 ; i < (m_lNTestData-1) ; i++ )
    {
      m_Size_Wt_Passing[i] = m_Size_Wt_Percent[i+1];
      m_Wt_Passing[i] = m_Wt_Passing[i-1] + m_Wt_Percent[i];
      m_Wt_Passing[i] = min(1.0,m_Wt_Passing[i]);

    }

  }

//===========================================================================

void CPSDTestData::PassingToWeight(void)

  {
  // Assumes sizes are entered lowest to largest

// To DO

  }

//===========================================================================

inline void swap(double &d1, double &d2)

  {
    double temp;
    temp = d1;
    d1 = d2;
    d2 = temp;
  }

 void CPSDTestData::Sort(void)

   {

  // Sort sizes into lowest to largest
	for (int i = 1 ; i < (m_lNTestData) ; i++ )
    {
        
        for (int j = i+1 ; j < (m_lNTestData) ; j++ )
          {
            if (m_Size_Wt_Percent[i] > m_Size_Wt_Percent[j])
              {
                 // Swap
                 swap(m_Size_Wt_Percent[i],m_Size_Wt_Percent[j]);
                 swap(m_Wt_Percent[i],m_Wt_Percent[j]);
 
              }

            if (m_Size_Wt_Passing[i] > m_Size_Wt_Passing[j])
              {
                 // Swap
                 swap(m_Size_Wt_Passing[i],m_Size_Wt_Passing[j]);
                 swap(m_Wt_Passing[i],m_Wt_Passing[j]);
 
              }
          }

    }

   }

//===========================================================================

double CPSDTestData::PercentPassing(double x)

  {

    switch(m_eEntryType)
      {
      case eEntry_Test:
        return(TestDataPercentPassingRR(x));
        break;
      case eEntry_Function:
        if (m_eFuncType==eFunc_RR)
          return(RosinRammlerPassing(x));
        else if (m_eFuncType==eFunc_GS)
          return(GaudinSchuhmannPassing(x));

        break;
      }

    return(0.0);
  }

//===========================================================================

