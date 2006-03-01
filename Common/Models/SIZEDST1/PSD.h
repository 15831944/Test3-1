//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __PSD_H
#define  __PSD_H

#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif
#ifndef __FLWNODE_H
  #include "flwnode.h"
#endif

//===========================================================================

//
// TO DO - Change Static Arrays Sizes to Dynamic
//

extern DDBValueLst DDB_TestDataType[];
extern DDBValueLst DDB_TestEntryType[];
extern DDBValueLst DDB_FunctionType[];
#define CPSDMaxTestData 64

class CPSDTestData
{
public:
	CPSDTestData(void);
	~CPSDTestData(void);

	enum eFunctionType { eFunc_RR, eFunc_GS };
	//enum eActionType   { eA_Params, eA_Interpolate , eA_Regression };
  enum eEntryType    { eEntry_Test, eEntry_Function };
  enum eTestDataType { eTest_WeightPercent , eTest_WeightPercentPassing };

  // Data Member Access

  void  SetName(Strng Name)                     {m_Name = Name;};
  Strng GetName(void)                           {return(m_Name);};
  const Strng* GetNameRef(void)                 {return(&m_Name);};


  void  SetSizeCnvTxt(Strng s)                  {m_SizeCnvTxt=s;};
  Strng GetSizeCnvTxt(void)                     {return(m_SizeCnvTxt);};
  void  SetFracCnvTxt(Strng s)                  {m_FracCnvTxt=s;};
  Strng GetFracCnvTxt(void)                     {return(m_FracCnvTxt);};

  void  SetSizeCnv(CnvAttribute c)              {m_SizeCnv=c;};
  CnvAttribute GetSizeCnv(void)                 {return(m_SizeCnv);};
  CnvAttribute* GetSizeCnvRef(void)              {return(&m_SizeCnv);};

  void  SetFracCnv(CnvAttribute c)              {m_FracCnv=c;};
  CnvAttribute GetFracCnv(void)                 {return(m_FracCnv);};
  CnvAttribute* GetFracCnvRef(void)              {return(&m_FracCnv);};


  void SetTestDataCount(long count)             { m_lNTestData=count;};
  long GetTestDataCount(void)                   { return(m_lNTestData);};

  void SetTestDataSz(long i,double sz)          { m_eTestDataType==eTest_WeightPercent ? m_Size_Wt_Percent[i] = sz : m_Size_Wt_Passing[i] = sz;  };
  double GetTestDataSz(long i)                  { return m_eTestDataType==eTest_WeightPercent ? m_Size_Wt_Percent[i]: m_Size_Wt_Passing[i]; };

  void SetTestDataFrac(long i,double wtfrac)    { m_Wt_Percent[i] = wtfrac; };
  double GetTestDataFrac(long i)                { return(m_Wt_Percent[i]); };
  void SetTestDataCumFrac(long i,double wtpass) { m_Wt_Passing[i] = wtpass; };
  double GetTestDataCumFrac(long i)             { return(m_Wt_Passing[i]); };

  void SetTestDataType(
                 eTestDataType iTestDataType )   { m_eTestDataType = iTestDataType; };
  eTestDataType GetTestDataType(void)            { return(m_eTestDataType); };

  void SetTestEntryType(
                 eEntryType iEntryType )   { m_eEntryType = iEntryType; };
  eEntryType GetTestEntryType(void)            { return(m_eEntryType); };

 void SetFunctionType(
                 eFunctionType iFunctionType )   { m_eFuncType = iFunctionType; };
  eFunctionType GetFunctionType(void)            { return(m_eFuncType); };


  Strng GetTypeString(void)                      { return(DDB_TestDataType[m_eTestDataType].m_pStr); }
  eTestDataType GetTypeStringType(const Strng &s);

  Strng GetEntryString(void)                      { return(DDB_TestEntryType[m_eEntryType].m_pStr); }
  eEntryType                                      GetEntryStringType(const Strng &s);

  Strng GetFunctionString(void)                    { return(DDB_FunctionType[m_eFuncType].m_pStr); }
  eFunctionType                                    GetFunctionStringType(const Strng &s);

  double getRR_a(void)                                { return(m_dRR_a); }
  double getRR_b(void)                                { return(m_dRR_b); }
  double getGS_k(void)                                { return(m_dGS_k); }
  double getGS_m(void)                                { return(m_dGS_m); }
  void setRR_a(double d)                              { m_dRR_a=d; }
  void setRR_b(double d)                              { m_dRR_b=d; }
  void setGS_k(double d)                              { m_dGS_k=d; }
  void setGS_m(double d)                              { m_dGS_m=d; }

	// Functions to Generate Passing Data from Specified Function
	void RosinRammlerData(void);
	void GaudinSchuhmann(void);


  double PercentPassing(double x);
  double RosinRammlerPassing(double x);
  double GaudinSchuhmannPassing(double x);
	double TestDataPercentPassingRR(double x);
  void WeightToPassing(void);
  void PassingToWeight(void);

  // This method sorts the data into the correct order
  // i.e. Lowest size to maximum size
  void Sort(void);

public:

  // Species Name
  Strng m_Name;

  // Species Size Units Text
  Strng m_SizeCnvTxt;

  // Species Fraction Units Text
  Strng  m_FracCnvTxt;

  // Conversion Types
  CnvAttribute m_SizeCnv;
  CnvAttribute m_FracCnv;

protected:

	eFunctionType m_eFuncType;
	//eActionType   m_eActionType;
  eEntryType    m_eEntryType;
  eTestDataType m_eTestDataType;
	


	// Rosin-Rammler Params
	double m_dRR_a;
	double m_dRR_b;

	// Gauding-Schulman Params
	double m_dGS_k;
	double m_dGS_m;

  // Test Input Data
  long   m_lNTestData;
  double m_Size_Wt_Percent[CPSDMaxTestData];
  double m_Wt_Percent[CPSDMaxTestData];
  double m_Size_Wt_Passing[CPSDMaxTestData];
  double m_Wt_Passing[CPSDMaxTestData];

	// Test Function Data
	long m_lNFunctionData;
	double m_X[CPSDMaxTestData];
	double m_Y[CPSDMaxTestData];

	// Number of Interpolated Data
	long m_lNInterpData;

	// Interpolated Data
	double m_Xr[CPSDMaxTestData];
	double m_Yr[CPSDMaxTestData];


};

typedef CArray<CPSDTestData,CPSDTestData> CPSDTestDataArray;

//===========================================================================

#endif