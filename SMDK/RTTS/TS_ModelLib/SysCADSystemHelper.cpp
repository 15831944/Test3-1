//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================


#include "stdafx.h"
#define  __SysCADSystemHelper_CPP
#include "SysCADSystemHelper.h"
#include "oreprops.h"

using namespace RioTintoTS;
#pragma optimize("", off)

//---------------------------------------------------------------------------

RioTintoTS::Matrix A; // Appearance Matrix
const double DEFAULT_BD   = 2.6;    // Bulk Density
const double DEFAULT_A    = 70.2;   // Breakage A Parameter
const double DEFAULT_B    = 3.28;   // Breakage b Parameter
const double DEFAULT_TA   = 2.19;   // Abrasion Parameter
const double DEFAULT_M    = 0.0677; // Not Used yet
const double DEFAULT_C    = 1.2511; // Not Used yet

void SysCADSystemHelper::PopulateMaterialInfo(MBaseMethod &M, RioTintoTS::PStreamInfo1 MatInfo)

{
	// We only support a 3 by 5 appearance matrix at this stage
	A.resize(3,5);
    // Reference appearance matrix rows
    VectorView R1 = A[0];
    VectorView R2 = A[1];
    VectorView R3 = A[2];

    RT_OreProps* OreProps = (RT_OreProps*)GetGlobalOther("RioTintoOreProps");
	if (OreProps != NULL )
	{

// Start Testing
		double BD_Val			= DEFAULT_BD;
		double A_Val			= DEFAULT_A;
		double b_Val			= DEFAULT_B;
		double Ta_Val			= DEFAULT_TA;
		double M_Val			= DEFAULT_M;
		double C_Val			= DEFAULT_C;
		double T_10_75_Val		= 2.00;
		double T_10_50_Val		= 2.70;
		double T_10_25_Val		= 4.70;
		double T_10_4_Val		= 27.90;
		double T_10_2_Val		= 55.90;
		double T_20_75_Val		= 5.20;
		double T_20_50_Val		= 6.70;
		double T_20_25_Val		= 10.50;
		double T_20_4_Val		= 49.30;
		double T_20_2_Val		= 81.40;
		double T_30_75_Val		= 8.30;
		double T_30_50_Val		= 10.70;
		double T_30_25_Val		= 16.40;
		double T_30_4_Val		= 65.60;
		double T_30_2_Val		= 98.50;
		
		int TestIndex			= OreProps->FindOtherProp("Test");
		int BulkDensity_index	= OreProps->FindOtherProp("Bulk_Density");
		int A_index				= OreProps->FindOtherProp("A");
		int b_index				= OreProps->FindOtherProp("b");
		int Ta_index			= OreProps->FindOtherProp("ta");
		int T_10_75_index		= OreProps->FindOtherProp("T_10_75");
		int T_10_50_index		= OreProps->FindOtherProp("T_10_50");
		int T_10_25_index		= OreProps->FindOtherProp("T_10_25");
		int T_10_4_index		= OreProps->FindOtherProp("T_10_4");
		int T_10_2_index		= OreProps->FindOtherProp("T_10_2");
		int T_20_75_index		= OreProps->FindOtherProp("T_20_75");
		int T_20_50_index		= OreProps->FindOtherProp("T_20_50");
		int T_20_25_index		= OreProps->FindOtherProp("T_20_25");
		int T_20_4_index		= OreProps->FindOtherProp("T_20_4");
		int T_20_2_index		= OreProps->FindOtherProp("T_20_2");
		int T_30_75_index		= OreProps->FindOtherProp("T_30_75");
		int T_30_50_index		= OreProps->FindOtherProp("T_30_50");
		int T_30_25_index		= OreProps->FindOtherProp("T_30_25");
		int T_30_4_index		= OreProps->FindOtherProp("T_30_4");
		int T_30_2_index		= OreProps->FindOtherProp("T_30_2");

		int k = 0;
		for (int i=0; i< OreProps->Count() ; i++)
		{
			RT_OreProp& Prop = OreProps->OrePropByIndex(i);
			if ( Prop.OK() )
			{
        if (gs_MVDefn[i].HasSizeData())
          {
          if ( BulkDensity_index > -1 )
					  BD_Val = Prop.OtherProp(BulkDensity_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: BD missing");
				  if ( A_index > -1 )
					  A_Val = Prop.OtherProp(A_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: A missing");
				  if ( b_index > -1 )
					  b_Val = Prop.OtherProp(b_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: b missing");
				  if ( Ta_index > -1 )
					  Ta_Val = Prop.OtherProp(Ta_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: Ta missing");
				  if ( T_10_75_index > -1 )
					  T_10_75_Val = Prop.OtherProp(T_10_75_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_10_75 missing");
				  if ( T_10_50_index > -1 )
					  T_10_50_Val = Prop.OtherProp(T_10_50_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_10_50 missing");
				  if ( T_10_25_index > -1 )
					  T_10_25_Val = Prop.OtherProp(T_10_25_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_10_25 missing");
				  if ( T_10_4_index > -1 )
					  T_10_4_Val = Prop.OtherProp(T_10_4_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_10_4 missing");
				  if ( T_10_2_index > -1 )
					  T_10_2_Val = Prop.OtherProp(T_10_2_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_10_2 missing");
				  if ( T_20_75_index > -1 )
					  T_20_75_Val = Prop.OtherProp(T_20_75_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_20_75 missing");
				  if ( T_20_50_index > -1 )
					  T_20_50_Val = Prop.OtherProp(T_20_50_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_20_50 missing");
				  if ( T_20_25_index > -1 )
					  T_20_25_Val = Prop.OtherProp(T_20_25_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_20_25 missing");
				  if ( T_20_4_index > -1 )
					  T_20_4_Val = Prop.OtherProp(T_20_4_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_20_4 missing");
				  if ( T_20_2_index > -1 )
					  T_20_2_Val = Prop.OtherProp(T_20_2_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_20_2 missing");
				  if ( T_30_75_index > -1 )
					  T_30_75_Val = Prop.OtherProp(T_30_75_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_30_75 missing");
				  if ( T_30_50_index > -1 )
					  T_30_50_Val = Prop.OtherProp(T_30_50_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_30_50 missing");
				  if ( T_30_25_index > -1 )
					  T_30_25_Val = Prop.OtherProp(T_30_25_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_30_25 missing");
				  if ( T_30_4_index > -1 )
					  T_30_4_Val = Prop.OtherProp(T_30_4_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_30_4 missing");
				  if ( T_30_2_index > -1 )
					  T_30_2_Val = Prop.OtherProp(T_30_2_index);
				  else
					  M.Log.Message(MMsg_Warning, "OreProperties Database: T_30_2 missing");

				  // Fill with Appearance matrix
				  R1[0]=T_10_75_Val; R1[1]=T_10_50_Val; R1[2]=T_10_25_Val; R1[3]=T_10_4_Val; R1[4]=T_10_2_Val;
				  R2[0]=T_20_75_Val; R2[1]=T_20_50_Val; R2[2]=T_20_25_Val; R2[3]=T_20_4_Val; R2[4]=T_20_2_Val;
				  R3[0]=T_30_75_Val; R3[1]=T_30_50_Val; R3[2]=T_30_25_Val; R3[3]=T_30_4_Val; R3[4]=T_30_2_Val;

          if ( k < MatInfo->nType() )
            {
  				  PMineralInfo1& m = MatInfo->GetMineral(k);
		  		  m->SetMineralInfo(BD_Val,A_Val,b_Val,Ta_Val,M_Val,C_Val);
			  	  m->SetAppearance(R1,R2,A); // R1 and R2 are not used in SetAppearance
            }
          else
            {
					  M.Log.Message(MMsg_Warning, "OreProperties Database count missmatch with Material Info (%d and %d)!", k+1, MatInfo->nType());
            }
  			  k++;

          }
        else
          {
					M.Log.Message(MMsg_Warning, "No PSD data for material %s!", Prop.Material());
          }

			}
		}

// End Testing
#ifdef NEVER
		int k = 0;
		for (int i=0; i< OreProps->Count() ; i++)
		{
			// Something dodgy is going on here where OreProps has more than the 2 specified in the
			// database.
			RT_OreProp& Prop = OreProps->OrePropByIndex(i);
			
			if ( Prop.OK() )
			{
				PMineralInfo1& m = MatInfo->GetMineral(k);
				m->SetMineralInfo(Prop.Density(),Prop.A(),Prop.b(),0.,0.,0.);
				k++;
			}
		}
#endif

	}
	else
	{
		M.Log.Message(MMsg_Warning, "OreProperties Database: Missing Data, Using Defaults");

#ifdef NEVER
		int n = MatInfo->nType();
		for (int i=0; i<n ; i++)
		{
			PMineralInfo1& m = MatInfo->GetMineral(i);
			m->SetMineralInfo(2.6,70.2,3.28,2.19,0.,0.);
		}
#endif

		// Fill with Appearance matrix with default data
		// in case OreProperties database is empty
		R1[0]=2.00; R1[1]= 2.70; R1[2]= 4.70; R1[3]=27.90; R1[4]=55.90;
		R2[0]=5.20; R2[1]= 6.70; R2[2]=10.50; R2[3]=49.30; R2[4]=81.40;
		R3[0]=8.30; R3[1]=10.70; R3[2]=16.40; R3[3]=65.60; R3[4]=98.50;

		// Set each material with default Appearance and Breakage Data
		int n = MatInfo->nType();
		for (int i=0; i<n ; i++)
		{
			PMineralInfo1& m = MatInfo->GetMineral(i);
			m->SetMineralInfo(DEFAULT_BD,DEFAULT_A,DEFAULT_B,DEFAULT_TA,DEFAULT_M,DEFAULT_C);
			m->SetAppearance(R1,R2,A); // R1 and R2 are not used in SetAppearance
		}
	}
}

//---------------------------------------------------------------------------

bool SysCADSystemHelper::SysCADSizeDataToSystem(MIPSD &PSD,RioTintoTS::PStreamInfo1 SizInfo)

//
// Copy SysCAD Sieve Series Data to System
//

{

	const int NumSizes = PSD.getSizeCount(); // Size of sieve


	double *d;
	d = new double[NumSizes];
	double sz;

	/**** START TEST ****/
	double *syscadsz = new double[NumSizes];

	for (int i=0;i<(NumSizes);i++)
	{
		sz = PSD.getSize(i)*1000.;
		syscadsz[i] = sz;
	}
	delete syscadsz;
	/**** END TEST *****/

	sz = PSD.getSize(NumSizes-1)*1000.; 
	for (int i=0;i<(NumSizes-1);i++)
	{
		sz = PSD.getSize(NumSizes-i-2)*1000.;
		d[i] = sz;
	}
	d[NumSizes-1] = 0.0;

	RioTintoTS::VectorView& sizeData=RioTintoTS::VectorView(d,NumSizes,1);

	SizInfo->SetSizes(  sizeData );
	delete d;


	return(true);

}

//---------------------------------------------------------------------------

bool SysCADSystemHelper::SysCADSolidsToSystem(MVector &SysCAD,RioTintoTS::PFlowStream1 &System)

//
// Copy SysCAD solids size data to System Size Data
//

{
	MIPSD & PSD=*SysCAD.GetIF<MIPSD>();
	if (!IsNothing(PSD))
	{  
		const int NumComps = PSD.getPSDVectorCount();
		const int NumSizes = PSD.getSizeCount();

		// Copy the Material Info and Sizing Info from the SysCAD input

		// Get a reference to the solids FeedStream Matrix.
		// We will copy the SysCAD feed size distributions to this Matrix 
		RioTintoTS::MatrixView &FeedSolids = System->AccessSolidsMatrix();

		// For each component
		//double test1 = 0.0;
		//double test2 = 0.0;
		for (int c=0; c<NumComps; c++)
		{
			// Get the size data for component c
			// The size data is largest to smallest in units of kg/s
			//double* pDataI = SysCAD.GetSizeData(c);

			// Insert the size data into the FeedStream Solids Matrix
			RioTintoTS::VectorView iComp = FeedSolids.column(c);

			for (int s=0; s<NumSizes; s++)
			{
				double M = PSD.getMass(c, NumSizes-s-1/*s*/);
				iComp[s] = M;

				// Convert Size Data to t/h /1000 * 3600
				iComp[s] = iComp[s]*3.6;
				//test2 = test2 + pDataI[s];
				//test1 = test1 + iComp[s];

			}
		}

		return true;
	}

	return false;

}

//---------------------------------------------------------------------------

bool SysCADSystemHelper::SysCADLiquidToSystem(MVector &SysCAD,RioTintoTS::PFlowStream1 &System)

//
// Copy SysCAD liquid mass flow to System mass flow
//

{
	const int WaterIndex = gs_MVDefn.Lookup("H2O(l)");

	if ( WaterIndex >= 0 )
	{
		//double& pdLiquid = System->GetLiquidMass();

		// kg/s to t/h
		System->SetLiquidMass(SysCAD.getM(WaterIndex)*3.6);

		return true;
	}

	return false;
}

//---------------------------------------------------------------------------

bool SysCADSystemHelper::SystemLiquidToSysCAD(MVector &SysCAD,RioTintoTS::PFlowStream1 &System)

//
// Copy SysCAD liquid mass flow to System mass flow
//

{
	const int WaterIndex = gs_MVDefn.Lookup("H2O(l)");

	if ( WaterIndex >= 0 )
	{
		double pdLiquid = System->GetLiquidMass();
		// t/h to kg/s
		SysCAD.putM(WaterIndex, pdLiquid/3.6);

		return true;
	}

	return false;
}

//---------------------------------------------------------------------------


bool SysCADSystemHelper::SystemSolidsToSysCAD(MVector &SysCAD,RioTintoTS::PFlowStream1 &System)

//
// Copy System solids size data to SysCAD Size Data
//

{
	MIPSD & PSD=*SysCAD.GetIF<MIPSD>();
	if (!IsNothing(PSD))
	{  
		const int NumComps = PSD.getPSDVectorCount();
		const int NumSizes = PSD.getSizeCount();

		// Get a reference to the System solids Matrices.
		// copy these feed size distributions to the SysCAD solids size data
		RioTintoTS::MatrixView &SystemSolids = System->AccessSolidsMatrix();


		// For each component
		for (int c=0; c<NumComps; c++)
		{
			// Get the size data for component c
			// The size data is smallest to largest in units of kg/s

			// Index into SysCAD Component Masses
			const int SpId = PSD.getSpecieIndex(c);

			// Insert the resultant size data into the SysCAD Output Size Data
			RioTintoTS::VectorView iCompSize = SystemSolids.column(c);

			double m = 0.0;
			for (int s=0; s<NumSizes; s++)
			{
				// Accumulate each size fraction to the total mass for the component
				m += iCompSize[s];
			}
			SysCAD.putM(SpId, m*1000/3600);
			double f = 0.0;

			for (int s=0; s<NumSizes; s++)
			{
				const double d = m>0.0 ? iCompSize[s]/m : 0.0;
				f += d;
				PSD.putFrac(c,NumSizes-s-1, d);
			}

		}
		return true;
	}

	return false;
}

//====================================================================================

void SysCADParams::InitParams(struct Parameter* p, int n , double val[] )

{

int i;
bool* bptr;
int* iptr;
bool bSetToZero = false;

for (i = 0 ; i < n ; i++ )
{
	// Change context
	p[i].ptr = &val[i];
	// Make sure value is zero first
	long* lptr = (long*)p[i].ptr;
	*lptr = 0;

	double td;
  int ti;

	if ((p[i].value == NULL) || (bSetToZero))
	{
		// A value of NULL implies all successive values
		// should be set to zero
		*(double*)(p[i].ptr) = 0.0;
		bSetToZero = true;
	}
	else
	{

	switch (p[i].type)
	{
	case parREAL:
		
		sscanf(p[i].value,"%lf",(double*)p[i].ptr);
		td = *(double*)p[i].ptr;
		break;

	case parINT:
		sscanf(p[i].value,"%d",(int*)p[i].ptr);
		iptr = (int*)p[i].ptr;
		ti = *iptr;
		break;

	case parBOOL:
		bptr = (bool*)p[i].ptr;
		if (strcmp(p[i].value,"TRUE")==0)
			*bptr = true;
		else
			*bptr = false;

		break;
	default:
		long* lptr = (long*)p[i].ptr;
		*lptr = 0;
	}

	}
}

}

//====================================================================================

void SysCADParams::ConvertParamsToDoubles(struct Parameter* p, int n,double* d,double val[])

{

int i;
int *iptr;
bool *bptr;
int temp_int;
bool temp_bool;
for (i = 0 ; i < n ; i++ )
{
	// Change context
	p[i].ptr = &val[i];

	switch (p[i].type)
	{
	case parREAL:
		
		d[i] = *(double*)p[i].ptr;
		break;

	case parINT:

		iptr = (int*)p[i].ptr;
		temp_int = *iptr;
		d[i] = floor(temp_int + 0.5);
		break;

	case parBOOL:
		bptr = (bool*)p[i].ptr;
		temp_bool = *bptr;
		d[i] = floor(temp_bool + 0.5);
		break;
	default:
		d[i] = 0.0;
	}
}

}

//====================================================================================

void SysCADParams::BuildDataFieldsAutoName(struct Parameter* p, int n,MBaseMethod* m,char* base,double val[])


{

int i;
int test_int;
char buff[80];

for (i = 0 ; i < n ; i++ )
{
	// Change context
	p[i].ptr = &val[i];

	sprintf(buff,"%s[%d]",base,i);

   	if (p[i].value == NULL) p[i].flags = MF_PARAMETER;


		switch (p[i].type)
		{
		case parREAL:

			m->DD.Double(buff, "" ,(double*)p[i].ptr, p[i].flags,  MC_None);
			break;

		case parINT:
			test_int = *(int*)p[i].ptr;
			m->DD.Long(buff,	"", (long*)p[i].ptr, p[i].flags);
			break;

		case parBOOL:
			m->DD.Bool(buff, "",(bool*)p[i].ptr, p[i].flags);
			break;
		default:
			break;
		}


}

}

void SysCADParams::BuildDataFields(struct Parameter* p, int n,MBaseMethod* m,double val[])

{

int i;

int test_int;

for (i = 0 ; i < n ; i++ )
{
	// Change context
	p[i].ptr = &val[i];

   	/*if (p[i].value == NULL) */p[i].flags = MF_PARAMETER;

	switch (p[i].type)
	{
	case parREAL:

		m->DD.Double(p[i].name, "" ,(double*)p[i].ptr, p[i].flags,  MC_None);
		break;

	case parINT:
		test_int = *(int*)p[i].ptr;
		m->DD.Long(p[i].name,	"" , (long*)p[i].ptr, p[i].flags);
		break;

	case parBOOL:
		m->DD.Bool(p[i].name, "" ,(bool*)p[i].ptr, p[i].flags);
		break;
	default:
		break;
	}
}

}

//====================================================================================
//====================================================================================
//====================================================================================

SysCADParamsA::SysCADParamsA()

{
  val = NULL;
  npars = 0;
}

//====================================================================================

SysCADParamsA::SysCADParamsA(struct Parameter* p, int n )

{
  val = NULL;
  npars = 0;
  InitParams( p, n );

}
//====================================================================================

SysCADParamsA::~SysCADParamsA()

{
  if ( val != NULL )
    delete []val;
}

//====================================================================================

void SysCADParamsA::InitParams(struct Parameter* p, int n )

{

int i;
bool* bptr;
int* iptr;
bool bSetToZero = false;

if ( val != NULL ) delete []val;
val = new double[n];
npars = n;

for (i = 0 ; i < n ; i++ )
{
	// Change context
	p[i].ptr = &val[i];
	// Make sure value is zero first
	long* lptr = (long*)p[i].ptr;
	*lptr = 0;

	double td;
  int ti;

	if ((p[i].value == NULL) || (bSetToZero))
	{
		// A value of NULL implies all successive values
		// should be set to zero
		*(double*)(p[i].ptr) = 0.0;
		bSetToZero = true;
	}
	else
	{

	switch (p[i].type)
	{
	case parREAL:
		
		sscanf(p[i].value,"%lf",(double*)p[i].ptr);
		td = *(double*)p[i].ptr;
		break;

	case parINT:
		sscanf(p[i].value,"%d",(int*)p[i].ptr);
		iptr = (int*)p[i].ptr;
		ti = *iptr;
		break;

	case parBOOL:
		bptr = (bool*)p[i].ptr;
		if (strcmp(p[i].value,"TRUE")==0)
			*bptr = true;
		else
			*bptr = false;

		break;
	default:
		long* lptr = (long*)p[i].ptr;
		*lptr = 0;
	}

	}
}

}

//====================================================================================

void SysCADParamsA::ConvertParamsToDoubles(struct Parameter* p, int n,double* d)

{

int i;
int *iptr;
bool *bptr;
int temp_int;
bool temp_bool;

if ( n > npars ) return;

for (i = 0 ; i < n ; i++ )
{
	// Change context
	p[i].ptr = &val[i];

	switch (p[i].type)
	{
	case parREAL:
		
		d[i] = *(double*)p[i].ptr;
		break;

	case parINT:
  case parSELLIST:

		iptr = (int*)p[i].ptr;
		temp_int = *iptr;
		d[i] = floor(temp_int + 0.5);
		break;

	case parBOOL:
		bptr = (bool*)p[i].ptr;
		temp_bool = *bptr;
		d[i] = floor(temp_bool + 0.5);
		break;
	default:
		d[i] = 0.0;
	}
}

}

//====================================================================================

void SysCADParamsA::BuildDataFieldsAutoName(struct Parameter* p, int n,MBaseMethod* m,char* base)

{

int i;
int test_int;
char buff[80];

if ( n > npars ) return;

for (i = 0 ; i < n ; i++ )
{
	// Change context
	p[i].ptr = &val[i];

	sprintf(buff,"%s[%d]",base,i);

		switch (p[i].type)
		{
		case parREAL:

			m->DD.Double(buff, "", (double*)p[i].ptr, p[i].flags,  MC_None);
			break;

		case parINT:
    case parSELLIST:
			test_int = *(int*)p[i].ptr;
			m->DD.Long(buff,	"", (long*)p[i].ptr, p[i].flags);
			break;

		case parBOOL:
			m->DD.Bool(buff, "", (bool*)p[i].ptr, p[i].flags);
			break;
		default:
			break;
		}


}

}

void SysCADParamsA::BuildDataFields(struct Parameter* p, int n,MBaseMethod* m)

{

int i;
int test_int;

if ( n > npars ) return;

for (i = 0 ; i < n ; i++ )
{
	// Change context
	p[i].ptr = &val[i];

	switch (p[i].type)
	{
	case parREAL:

		m->DD.Double(p[i].name, "" ,(double*)p[i].ptr, p[i].flags,  MC_None);
		break;

	case parINT:
  case parSELLIST:
		test_int = *(int*)p[i].ptr;
		m->DD.Long(p[i].name,	"" , (long*)p[i].ptr, p[i].flags);
		break;

	case parBOOL:
		m->DD.Bool(p[i].name, "" ,(bool*)p[i].ptr, p[i].flags);
		break;
	default:
		break;
	}
}

}

//====================================================================================

