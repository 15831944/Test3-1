//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __SysCADSystemHelper_H
#define  __SysCADSystemHelper_H

#include <vector>
//#include "TS_ObjectLib.h"
//#include "TS_ModelUtility.h"
//#include "MinProc.h"
#include "StreamInfo1.h"
#include "FlowStream1.h"
#include "Vector.h"
#include "Matrix.h"
//#include "LDA2.h"

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif


//---------------------------------------------------------------------------

class SysCADSystemHelper
  {
  public:
	  SysCADSystemHelper();

	  static bool SysCADSizeDataToSystem(MIPSD &PSD,RioTintoTS::PStreamInfo1 SizInfo);
	  static bool SysCADSolidsToSystem(MVector &SysCAD,RioTintoTS::PFlowStream1 &System);
	  static bool SystemSolidsToSysCAD(MVector &SysCAD,RioTintoTS::PFlowStream1 &System);
	  static bool SysCADLiquidToSystem(MVector &SysCAD,RioTintoTS::PFlowStream1 &System);
	  static bool SystemLiquidToSysCAD(MVector &SysCAD,RioTintoTS::PFlowStream1 &System);
	  static void PopulateMaterialInfo(MBaseMethod &M, RioTintoTS::PStreamInfo1 MatInfo);

  protected:
  };

//---------------------------------------------------------------------------

//typedef CArray <Strng, Strng> StrngArray;

//---------------------------------------------------------------------------

enum ParameterType { parREAL, parINT, parBOOL , parSELLIST };

//---------------------------------------------------------------------------

struct Parameter
{
	char* name;				// Parameter Name to Display on Pages
	ParameterType type;
	char* value;			// Value in string format
	unsigned long flags;	// Flags specifying how param should be displayed
	void* ptr;				// Used to point to storage for paramater
};

//---------------------------------------------------------------------------

class SysCADParams
{
public:
	static void InitParams(struct Parameter* p, int n, double val[] );
	static void BuildDataFields(struct Parameter* p, int n, MBaseMethod* m,double val[]);
	static void BuildDataFieldsAutoName(struct Parameter* p, int n,MBaseMethod* m,char* base,double val[]);
	static void ConvertParamsToDoubles(struct Parameter* p, int n,double* d,double val[]);
};

//---------------------------------------------------------------------------

class SysCADParamsA
{
public:
  SysCADParamsA();
  SysCADParamsA(struct Parameter* p, int n );
  ~SysCADParamsA();
	void InitParams(struct Parameter* p, int n );
	void BuildDataFields(struct Parameter* p, int n, MBaseMethod* m );
	void BuildDataFieldsAutoName(struct Parameter* p, int n,MBaseMethod* m,char* base);
	void ConvertParamsToDoubles(struct Parameter* p, int n,double* d);

private:
  double* val;  // Storage for parameters
  int npars;    // Number of parameters
};

//---------------------------------------------------------------------------

class SysCADParamsTable
{
public:
  SysCADParamsTable();
  //SysCADParamsA(struct Parameter* p, int n );
  ~SysCADParamsTable();
  void InitParams(char* TableName_, char* Col0Name_, char* Col0Desc_, char* Col1Name_, char* Col1Desc_, int RowCount);
  void BuildDataFields(MDataDefn &DD);

  void SetData(double* d);
  void GetData(double* d);
  double* Val() { return val; };

private:
  double* val;  // Storage for parameters
  double dResult;
  char* TableName;
  char* Col0Name;
  char* Col0Desc;
  char* Col1Name;
  char* Col1Desc;
  int nrows;    // Number of rows for parameters
  int ncols;    // Number of columns
};

//---------------------------------------------------------------------------


#endif