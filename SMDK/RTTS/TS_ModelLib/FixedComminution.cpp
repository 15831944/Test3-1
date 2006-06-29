//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __FixedComminution_CPP
#include "FixedComminution.h"

//====================================================================================
//             Fixed Comminution
//====================================================================================

CComminution_Fixed::CComminution_Fixed()
{

	m_lDischargeSel = 0;
	m_DDPSDDischarge = 0;

	// Initialise discharge partitions for all possible
	// PSD Definitions that could be used for this project
	int n = gs_PSDDefns.Count();
	m_CDischarge.SetSize(n);
	m_DDPSDDischarge = new MDDValueLst[n+1];
	m_DDPSDDischarge[n].m_dwFlags = 0;
	m_DDPSDDischarge[n].m_lVal = 0;
	m_DDPSDDischarge[n].m_pStr = 0;

	for (int i=0; i< n; i++)
	{
		MPSDDefn& PSDDefn = gs_PSDDefns[i]; 
		int NIntervals = PSDDefn.getSizeCount();
		m_CDischarge[i].m_dDischarge.SetSize(NIntervals);
		m_CDischarge[i].m_dCSS = 0.005;
		m_CDischarge[i].m_dTestCSS = 0.005;
		m_CDischarge[i].m_bBypassOn = true;
		m_CDischarge[i].m_Name = PSDDefn.Name();
		m_DDPSDDischarge[i].m_dwFlags = 0;
		m_DDPSDDischarge[i].m_lVal = i;
		const LPCSTR s = m_CDischarge[i].m_Name.GetString();
		m_DDPSDDischarge[i].m_pStr = (LPTSTR)s;
		
		
	}
}

//====================================================================================

CComminution_Fixed::~CComminution_Fixed()

{
	delete m_DDPSDDischarge;
}

//====================================================================================

void CComminution_Fixed::BuildDataFields(MDataDefn &DB)
	
{ 
	DB.ObjectBegin("TS_Fixed", "Fixed");
 
	DB.Page("Fixed Discharge");
    CString Tg1,Tg2;

    DB.Long  ("Discharge", "", &m_lDischargeSel  , MF_PARAMETER, m_DDPSDDischarge );
	for (int i = 0; i < m_CDischarge.GetSize() ; i++)
	{
		DWORD flags;
		if ( i == m_lDischargeSel )
			flags = MF_PARAMETER;
		else
			flags = MF_PARAMETER | MF_NO_VIEW;

		Tg1.Format("TS_Fixed%02d",i);
		Tg2.Format("Fixed%02d",i);
		DB.ObjectBegin(Tg1, Tg2);

		DB.Double("CSS",  "", &m_CDischarge[i].m_dCSS,     flags ,  MC_L("mm"));
		DB.Double("TCSS", "", &m_CDischarge[i].m_dTestCSS, flags ,  MC_L("mm"));
		DB.CheckBox("ByPassOn", "", &m_CDischarge[i].m_bBypassOn, flags);

		for (int j = (m_CDischarge[i].m_dDischarge.GetSize()-1); j >= 0; j--)
		{
			Tg1.Format("I%02d", j);

			//
			// Check value is in range i.e. Betwee 0 and 1 and
			// larger sizes passings are >= smaller size cumulative percents
			//
			if (m_CDischarge[i].m_dDischarge[j] > 1.0)
				m_CDischarge[i].m_dDischarge[j] = 1.0;
			if (m_CDischarge[i].m_dDischarge[j] < 0.0)
				m_CDischarge[i].m_dDischarge[j] = 0.0;

			//if ((j>1) && (m_CDischarge[i].m_dDischarge[j] < m_CDischarge[i].m_dDischarge[j-1]))
			//	m_CDischarge[i].m_dDischarge[j] = m_CDischarge[i].m_dDischarge[j-1];

			DB.Double((char*)(const char*)Tg1, "", &m_CDischarge[i].m_dDischarge[j], flags , MC_Frac("%"));

		}
		DB.ObjectEnd();


	}
	DB.ObjectEnd();

    // Checks
	for (int i = 0; i < m_CDischarge.GetSize() ; i++)
	{

		// Force distribution to ascend
		int nsizes = m_CDischarge[i].m_dDischarge.GetSize();
		for (int j = 0; j < nsizes; j++)
		{
			for (int k = j; k < nsizes; k++ )
			{
				double val1 = m_CDischarge[i].m_dDischarge[j];
				double val2 = m_CDischarge[i].m_dDischarge[k];
				if (val1 > val2)
				{
					// Swap
					m_CDischarge[i].m_dDischarge[j] = val2;
					m_CDischarge[i].m_dDischarge[k] = val1;
				}

			}

		}
		m_CDischarge[i].m_dDischarge[nsizes-1] = 1.0;
	}

}

//====================================================================================

void CComminution_Fixed::EvalProducts(MBaseMethod &M,
									  MStream &Feed , MStream &Product ,
		                              bool bInit)
	
{ 
	MIPSD & PSDin  = Feed.IF<MIPSD>(false);
	MIPSD & PSDout = Product.IF<MIPSD>(false);

	if ( IsNothing(PSDin)==false )
	{
		// Input Stream does have a PSD
		// which implies that some of the input stream species have associated size data

		long NumSizes = PSDin.getSizeCount();
		long NumComps = PSDin.getPSDVectorCount();

		if (bInit)
		{
			//m_dDischarge.SetSize(NumSizes);
		}

		for (int c=0; c<NumComps; c++)
		{

			// Determine which PSD we are using so we can
			// choose the correct discharge partition
			//
			// This is a temporary solution until we expose
			// PSDin.getDefnIndex();
			//
			int iPSD = 0;
			MPSDDefn& PSDDefn = PSDin.getDefn();
			int ndefs = gs_PSDDefns.Count();
			for (int i=0; i< ndefs ; i++)
			{
				MPSDDefn& PSDDefSrch = gs_PSDDefns[i];

				if (&PSDDefSrch == &PSDDefn)
				{
					iPSD = i;
					break;
				}
			}

			// Extract the specified Discharge PSD
			int n = m_CDischarge[iPSD].m_dDischarge.GetSize();
			double* Fcum = new double[n];          // User specified Cumulative Dischage
			double* FcumShift = new double[n];     // Shifted Cumulative Discharge with CSS
			double* Ffrac = new double[n];         // Calculated discharge fractions
			double* FeedMassRet = new double[n];   // Feed Mass Fractions
			double* ByPassMassRet = new double[n]; // Calculated Bypass Mass Fractions
			double* DischargeMassRet = new double[n]; // Final calculated discharge mass
			double* S = new double[n];			   // Size Intervals
			double* SShifted = new double[n];      // Shifted Size Intervals

			// Calculate Cumulative discharge PSD from specified fractions
			for (int i = 0; i < n; i++)
			{
				Fcum[i] = m_CDischarge[iPSD].m_dDischarge[i];
				FcumShift[i] = Fcum[i];
			}

			//
			// Optionally Calculate the ByPass Stream from the Feed Stream
			//
			PSDin.ExtractMassVector(FeedMassRet,c);
			PSDin.ExtractSizes(S,1.0);
			double ByPassTotal = 0.0;

			if (m_CDischarge[iPSD].m_bBypassOn==true)
			{
				double FeedMassCum1 = 0.0;
				double FeedMassCum2 = 0.0;
				ByPassMassRet[0] = FeedMassRet[0];
				ByPassTotal = ByPassMassRet[0];
				for (int i = 1; i < n; i++ )
				{
					double x1 = S[i-1];
					if (x1 < 0.0) x1 = 0.0;
					double x2 = S[i];

					FeedMassCum1 += FeedMassRet[i-1];
					FeedMassCum2 = FeedMassCum1 + FeedMassRet[i];

					if ((x1 < m_CDischarge[iPSD].m_dCSS)&&
						(x2 > m_CDischarge[iPSD].m_dCSS))
					{
						// CSS lies between 2 size intervals
						// Calculate split to bypass
						int xx =0;

						double logcss = log(GTZ(m_CDischarge[iPSD].m_dCSS));
						double logx1 = log(GTZ(x1));
						double logx2 = log(GTZ(x2));
						double logy1 = log(GTZ(FeedMassCum1));
						double logy2 = log(GTZ(FeedMassCum2));

						double logBypass = 0.0;
						double Bypass = 0.0;
						logBypass = (logy1+(logcss-logx1)*(logy2-logy1)/GTZ(logx2-logx1));
						Bypass = exp(logBypass) - FeedMassCum1;

						ByPassMassRet[i] = Bypass;
					}
					else if (x1 < m_CDischarge[iPSD].m_dCSS)
					{
						// Everything less than CSS to ByPass
						ByPassMassRet[i] = FeedMassRet[i];
					}
					else
					{
						// Everything Greater than CSS Stays
						ByPassMassRet[i] = 0.0;
					}


					ByPassTotal += ByPassMassRet[i];
				}
			}
			else
			{
				ByPassTotal = 0.0;
			}

			//
			// Calculate Remaining Mass to Be Used in for Fixed Discharge
			//
			double TotalFeed = Feed.getM(c);
			double FixedDischargeFeed = TotalFeed-ByPassTotal;

			//
			// Shift the Distribution Based on the Specified CSS
			//
			double scale = m_CDischarge[iPSD].m_dCSS/GTZ(m_CDischarge[iPSD].m_dTestCSS);
			PSDin.ExtractSizes(SShifted,scale);

			for (int i = 0; i < n; i++ )
			{
				//
				// Find shifted size intervals that our size lies
				// between for interpolation
				//
				double x = S[i];
				for (int j = 0; j < (n-1) ; j++ )
				{
					double x1 = SShifted[j];
					if (x1 < 0.0) x1 = 0.0;
					double x2 = SShifted[j+1];
					if ((x < x1)||
					   ((x >= x1) && ( x <= x2 ))||
					   (j >= (n-2)))
					{
						// Interpolate

						double y1 = Fcum[j];
						double y2 = Fcum[j+1];
						double ynew = log(GTZ(y1))+(log(GTZ(x))-log(GTZ(x1)))*
							                       (log(GTZ(y2))-log(GTZ(y1)))/GTZ(log(GTZ(x2))-log(GTZ(x1)));
						ynew = exp(ynew);
						FcumShift[i] = ynew;
						if (FcumShift[i] < 0.0) FcumShift[i] = 0.0;
						if  (FcumShift[i] > 1.0) FcumShift[i] = 1.0;
						break;
					}
				}
			}

			// Convert Cummulative PSD to Interval Fractions
			for (int i = 1; i < n; i++)
			{
				Ffrac[i] = FcumShift[i] - FcumShift[i-1];
			}
			Ffrac[0] = FcumShift[0];

			// Calculate ByPass + Fixed Discharge
			for (int i = 0; i < n ; i++)
			{
				if (m_CDischarge[iPSD].m_bBypassOn==true)
					DischargeMassRet[i] = ByPassMassRet[i] + Ffrac[i]*FixedDischargeFeed;
				else
					DischargeMassRet[i] = Ffrac[i]*FixedDischargeFeed;
			}

			// Set the Discharge PSD
			// SysCAD stores fractions per interval
			//PSDout.ReplaceFracVector(Ffrac,c);
			//Replace the Discharge Mass Fractions
			PSDout.ReplaceMassVector(DischargeMassRet,c);

			delete []S;
			delete []SShifted;
			delete []Fcum;
			delete []FcumShift;
			delete []Ffrac;
			delete []FeedMassRet;
			delete []ByPassMassRet;
			delete []DischargeMassRet;
		}

	}
}

//====================================================================================