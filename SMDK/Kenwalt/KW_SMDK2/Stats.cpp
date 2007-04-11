//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __STATS_CPP
#include "stats.h"
#include <math.h>
#include <limits>
#pragma optimize("", off)

//====================================================================================

//TODO: A Proper icon
static double Drw_Stats[] = { MDrw_Poly,  -2.,2.,  2.,2.,  2.,-2., -2.,-2., -2.,2., MDrw_End };

//---------------------------------------------------------------------------

DEFINE_CONTROL_UNIT(SingleVarStats, "Single Variable Statistics", DLL_GroupName)
void SingleVarStats_UnitDef::GetOptions()
{
	SetDefaultTag("SS");
	SetDrawing("Tank", Drw_Stats);
	SetTreeDescription("Demo:Single Variable Statistics");
	SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
	SetModelGroup(MGroup_General);
};

//---------------------------------------------------------------------------

SingleVarStats::SingleVarStats(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
	//default values...
	lTagID = -1;
	bOn = true;
	sTag = "";
	dHistoMin = 0;
	dHistoMax = 0;
	lHistoCount = 10;
	lRecordCount = 0;

	pHistoBucketBorders = NULL;
	pHistoBucketCounts = NULL;

	Reset();

	TagIO.Open(20);
}

//---------------------------------------------------------------------------

void SingleVarStats::Init()
{
}

//---------------------------------------------------------------------------

bool SingleVarStats::PreStartCheck()
{
	return true;
}

//---------------------------------------------------------------------------

void SingleVarStats::EvalCtrlInitialise(eScdCtrlTasks Tasks)
{
	//Reset();
}

//---------------------------------------------------------------------------

const long idDX_Tag = 1;
const long idDX_Reset = 2;
const long idDX_HistoCount = 3;
const long idDX_RecordCount = 4;
const long idDX_HistoMin = 5;
const long idDX_HistoMax = 6;

void SingleVarStats::BuildDataFields()
{
	DD.CheckBox("Logging","Logging", &bOn, MF_PARAMETER);
	DD.String("StatTag", "StatTag", idDX_Tag, MF_PARAM_STOPPED);
	DD.Button("Reset", "Reset", idDX_Reset);
	DD.Double("Histogram Minimum", "HistoMin", idDX_HistoMin, MF_PARAMETER);
	DD.Double("Histogram Maximum", "HistoMax", idDX_HistoMax, MF_PARAMETER);
	DD.Long("Histogram Buckets", "HistoCount", idDX_HistoCount, MF_PARAMETER | MF_SET_ON_CHANGE);

	DD.Text("");

	DD.Double("Average", "Avg", &dAverage, MF_RESULT);
	DD.Double("Standard Deviation", "StdDev", &dStdDev, MF_RESULT);
	DD.Double("Minimum", "Min", &dMin, MF_RESULT);
	DD.Double("Maximum", "Max", &dMax, MF_RESULT);
	DD.Long("Number of Records", "N", idDX_RecordCount, MF_RESULT | MF_SET_ON_CHANGE);

	DD.Text("");

	//DD.StructBegin("Histogram");
	DD.ArrayBegin("Histogram Buckets", "Buckets", lHistoCount + 2);	
	for (int i = 0; i < lHistoCount + 2; i++)
	{
		DD.ArrayElementStart(i);
		DD.Double("Minimum", "Min", pHistoBucketBorders + i, MF_RESULT);
		DD.Double("Maximum", "Max", pHistoBucketBorders + i + 1, MF_RESULT);
		DD.Long("Count", "Count", pHistoBucketCounts + i, MF_RESULT);
		DD.ArrayElementEnd();
	}
	DD.ArrayEnd();
	//DD.StructEnd();

	DD.Show(false);
	DD.Double("SumX2", "SumX2", &dSumX2, MF_RESULT);
	DD.Double("sumX", "SumX", &dSumX, MF_RESULT);

	//TODO: Investigate a better method to store the record.
	/*if (cRecord.size() > 0)
	{
		DD.ArrayBegin("Record", "Record", lRecordCount);
		list<double>::iterator endIterator = cRecord.end();
		int j = 0;
		for (list<double>::iterator iterator = cRecord.begin(); iterator != endIterator; iterator++) {
			DD.ArrayElementStart(j++);
			DD.Double("Val","Val", &(*iterator), MF_RESULT);
			DD.ArrayElementEnd();
		}
	}*/
	DD.Show(true);
}

bool SingleVarStats::ExchangeDataFields()
{
	switch (DX.Handle)
	{
	case (idDX_Tag):
		if (DX.HasReqdValue)
		{
			sTag = DX.String;

			int curTag = TagIO.FindTag(DX.String);
			if (curTag >= 0)
			{
				DX.String = sTag;
				return true;
			}
			else
			{
				if (lTagID >= 0)
					TagIO.Remove(lTagID);
				lTagID = TagIO.Add(DX.String, "TagToGet", MTIO_Get);
			}
			Reset();
		}
		DX.String = sTag;
		return true;
	case (idDX_Reset):
		if (DX.HasReqdValue && DX.Bool == true)
			Reset();
		DX.Bool = false;
		return true;
	case (idDX_HistoCount):
		if (DX.HasReqdValue)
		{
			lHistoCount = DX.Long;
			RecalculateHistoBuckets();
		}
		DX.Long = lHistoCount;
		return true;
	case (idDX_HistoMax):
		if (DX.HasReqdValue)
		{
			dHistoMax = DX.Double;
			RecalculateHistoBuckets();
		}
		DX.Double = dHistoMax;
		return true;
	case (idDX_HistoMin):
		if (DX.HasReqdValue)
		{
			dHistoMin = DX.Double;
			RecalculateHistoBuckets();
		}
		DX.Double = dHistoMin;
		return true;
	case(idDX_RecordCount):
		if (DX.HasReqdValue)	//Should only be called on initilisation
		{
			lRecordCount = DX.Long;
			cRecord.clear();
			for (int i = 0; i < lRecordCount; i++)
				cRecord.push_back(0);
		}
		DX.Long = lRecordCount;
		return true;
	}
	return false;
}

bool SingleVarStats::ValidateDataFields()
{
	if (lHistoCount < 1)
		lHistoCount = 1;
	return true;
}

void SingleVarStats::EvalCtrlStrategy(eScdCtrlTasks Tasks)
{
	try
	{
		if (lTagID < 0 || !bOn)
			return;
		double dVal = TagIO.DValue[lTagID];
		RecalculateStats(dVal);
		//InvalidateHistogram();
	}
	catch (MMdlException &ex)
	{
		Log.Message(MMsg_Error, ex.Description);
	}
	catch (MFPPException &e)
	{
		e.ClearFPP();
		Log.Message(MMsg_Error, e.Description);
	}
	catch (MSysException &e)
	{
		Log.Message(MMsg_Error, e.Description);
	}
	catch (...)
	{
		Log.Message(MMsg_Error, "Some Unknown Exception occured");
	}
}

void SingleVarStats::Reset()
{
	dMin = dMax = dNAN;
	dStdDev = dNAN;
	dAverage = dNAN;
	lRecordCount = 0;
	cRecord.clear();
	dSumX = dSumX2 = 0;
	RecalculateHistoBuckets();
}

void SingleVarStats::RecalculateStats(double newEntry)
{
	// Update min and max
	if (_isnan(dMin) || (dMin > newEntry))
		dMin = newEntry;
	if (_isnan(dMax) || (dMax < newEntry))
		dMax = newEntry;

	// Update records
	lRecordCount++;
	cRecord.push_back(newEntry);
	dSumX += newEntry;
	dSumX2 += newEntry * newEntry;

	// Update average and std dev.
	dAverage = dSumX / lRecordCount;
	if (lRecordCount <= 1)
		dStdDev = dNAN;
	else
	{
		if (dSumX2 - lRecordCount * dAverage * dAverage > 0) //If we are logging a zero std dev. parameter, rounding errors can cause an exception
			dStdDev = sqrt((dSumX2 - lRecordCount * dAverage * dAverage)/(lRecordCount - 1));
		else
			dStdDev = 0;
	}

	// Update histogram
	for (int i = 0; i < lHistoCount + 2; i++)
		if (newEntry < pHistoBucketBorders[i+1])
		{
			pHistoBucketCounts[i]++;
			break;
		}
}

void SingleVarStats::RecalculateHistoBuckets()
{
	if (pHistoBucketBorders != NULL)
		delete[] pHistoBucketBorders;
	if (pHistoBucketCounts != NULL)
		delete[] pHistoBucketCounts;
	pHistoBucketBorders = new double[lHistoCount + 3];
	for (int i = 1; i < lHistoCount + 2; i++)
		pHistoBucketBorders[i] = dHistoMin + (dHistoMax - dHistoMin) / lHistoCount * (i - 1);
	pHistoBucketBorders[0] = - numeric_limits<double>::infinity();
	pHistoBucketBorders[lHistoCount + 2] = numeric_limits<double>::infinity();
	pHistoBucketCounts = new long[lHistoCount + 2];
	for (int i = 0; i < lHistoCount + 2; i++)
		pHistoBucketCounts[i] = 0;
	double newEntry;
	list<double>::const_iterator endIterator = cRecord.end();
	for (list<double>::const_iterator iterator = cRecord.begin(); iterator != endIterator; iterator++)
	{
		newEntry = *iterator;
		for (int i = 0; i < lHistoCount + 2; i++)
			if (!(newEntry > pHistoBucketBorders[i+1]))
			{
				pHistoBucketCounts[i]++;
				break;
			}
	}
	if (cRecord.size() > 0)
	{
		newEntry = *endIterator;
		for (int i = 0; i < lHistoCount + 2; i++)
			if (!(newEntry > pHistoBucketBorders[i+1]))
			{
				pHistoBucketCounts[i]++;
				break;
			}
	}

	//InvalidateHistogram();
}

bool SingleVarStats::GetModelGraphic(CMdlGraphicArray & Grfs)
{
	CMdlGraphic G0(0, MGT_Simple, true, "Histogram", 1);

	Grfs.SetSize(0);
	Grfs.SetAtGrow(0, G0);
	return true;
}

bool SingleVarStats::OperateModelGraphic(CMdlGraphicWnd &Wnd, CMdlGraphic &Grf)
{
	const COLORREF White = COLORREF(RGB(255,255,255));
	const COLORREF Black = COLORREF(RGB(0,0,0));
	const COLORREF Blue  = COLORREF(RGB(0,0,255));
	const COLORREF Cyan  = COLORREF(RGB(0,255,255));
	const COLORREF Red   = COLORREF(RGB(255,0,0));
	const COLORREF Green = COLORREF(RGB(0,255,0));
	switch (Wnd.m_eTask)
	{
	case MGT_Create:
		Wnd.m_pWnd->SetWindowPos(NULL, 0, 0, 200, 200, SWP_NOMOVE | SWP_NOZORDER);
		break;
	case MGT_Size:
		Wnd.m_pWnd->Invalidate();
		break;
	case MGT_Move:
		break;
	case MGT_EraseBkgnd:
		Wnd.m_bReturn = 0;
		break;
	case MGT_Paint:
		Wnd.m_pPaintDC->FillSolidRect(Wnd.m_ClientRect, Black);
		Wnd.m_pPaintDC->SetTextColor(Green);
		CPen penWhite(PS_SOLID, 0, White);
		CPen penGreen(PS_SOLID, 0, Green);
		int nTextSize = Wnd.m_TextSize.y;
		int nBorderSpace = nTextSize;
		int nTextSpace = 4;
		int nAxesSpace = 4;
		int nTopSpace = nBorderSpace + nTextSize;
		int nCheckSize = nAxesSpace - 1;
		int minSpace = 10;
		//Draw axes:
		int nAxesTop = nTopSpace,
			nAxesBottom = Wnd.m_ClientRect.bottom - (nBorderSpace + 2 * nTextSize + nTextSpace + nAxesSpace),
			nAxesLeft = nBorderSpace + nTextSize + nTextSpace + nAxesSpace,
			nAxesRight = Wnd.m_ClientRect.right - (nBorderSpace);
		if (nAxesBottom - nAxesTop <= 0 || nAxesRight - nAxesLeft <= 0)
			break;
		POINT axes[] = { 
			{nAxesLeft, nAxesTop},
			{nAxesLeft, nAxesBottom},
			{nAxesRight, nAxesBottom} };
		CPen* oldPen = Wnd.m_pPaintDC->SelectObject(&penGreen);
		Wnd.m_pPaintDC->Polyline(axes, 3);
		int nArrowSize = 3;
		POINT ArrowTop[] = {
			{nAxesLeft, nAxesTop - nArrowSize},
			{nAxesLeft - nArrowSize, nAxesTop},
			{nAxesLeft + nArrowSize, nAxesTop},
			{nAxesLeft, nAxesTop - nArrowSize} };
		POINT ArrowSide[] = {
			{nAxesRight + nArrowSize, nAxesBottom},
			{nAxesRight, nAxesBottom - nArrowSize},
			{nAxesRight, nAxesBottom + nArrowSize},
			{nAxesRight + nArrowSize, nAxesBottom} };
		Wnd.m_pPaintDC->Polygon(ArrowTop, 4);
		Wnd.m_pPaintDC->Polygon(ArrowSide, 4);

		//Draw blocks:
		float blockWidth = (float)(nAxesRight - nAxesLeft) / (lHistoCount + 2);
		int MaxCount = 0;
		for (int i = 0; i < lHistoCount + 2; i++)
			if (pHistoBucketCounts[i] > MaxCount) MaxCount = pHistoBucketCounts[i];
		int FullScale = (int)(MaxCount * 1.1);
		int nAxesHeight = nAxesBottom - nAxesTop;
		Wnd.m_pPaintDC->SelectObject(&penWhite);
		if (MaxCount > 0)
			for (int i = 0; i < lHistoCount + 2; i++)
				Wnd.m_pPaintDC->Rectangle(
					nAxesLeft + (int)(i*blockWidth),		nAxesBottom - nAxesHeight * pHistoBucketCounts[i] / FullScale, 
					nAxesLeft + (int)((i+1)*blockWidth),	nAxesBottom);

		//Draw checks and axis values:
		Wnd.m_pPaintDC->SetTextAlign(TA_TOP | TA_CENTER);
		Wnd.m_pPaintDC->SelectObject(&penGreen);
		int lastLabel = 0;
		for (int i = 1; i < lHistoCount + 2; i++)
		{
			POINT Checkline[] = {
				{nAxesLeft + (int)(i * blockWidth), nAxesBottom},
				{nAxesLeft + (int)(i * blockWidth), nAxesBottom + nCheckSize}};
			Wnd.m_pPaintDC->Polyline(Checkline, 2);
			
			CString value;
			if (pHistoBucketBorders[i] < 100) //Because the %.2g formatting doesn't work like it should
				value.Format("%.2g", pHistoBucketBorders[i]);
			else if (pHistoBucketBorders[i] < 1E4)
				value.Format("%.0f", pHistoBucketBorders[i]);
			else
				value.Format("%.1e", pHistoBucketBorders[i]);

			if (nAxesLeft + (int)(i * blockWidth) - lastLabel > Wnd.m_pPaintDC->GetTextExtent(value).cx / 2 + minSpace)
			{
				Wnd.m_pPaintDC->TextOut(nAxesLeft + (int)(i * blockWidth), nAxesBottom + nAxesSpace, value);
				lastLabel = nAxesLeft + (int)(i * blockWidth) + Wnd.m_pPaintDC->GetTextExtent(value).cx / 2;
			}
		}

		//Draw labels:
		CString xLabel, yLabel;
		CString units = TagIO.getCnvText(lTagID);
		if (lTagID > 0 && strcmp(units, "") != 0)
			xLabel.Format("Value (%s)", units);
		else
			xLabel.Format("Value");
		yLabel.Format("Count");
		
		Wnd.m_pPaintDC->SetTextAlign(TA_BOTTOM | TA_CENTER);
		Wnd.m_pPaintDC->TextOut((nAxesLeft + nAxesRight) / 2, Wnd.m_ClientRect.bottom - nBorderSpace, xLabel);
		
#if (_MSC_VER >= 1400)
		int oldMode = Wnd.m_pPaintDC->->SetGraphicsMode(GM_ADVANCED);
		XFORM rotation = {
			0, -1,
			1, 0,
			0, 0};
		XFORM identity = {
			1, 0,
			0, 1,
			0, 0};
		BOOL setTransformResult = Wnd.m_pPaintDC->SetWorldTransform(&rotation);

		Wnd.m_pPaintDC->SetTextAlign(TA_TOP | TA_CENTER);
		Wnd.m_pPaintDC->TextOut(-(nAxesTop + nAxesBottom) / 2, nBorderSpace, yLabel);
		Wnd.m_pPaintDC->SelectObject(oldPen);

		Wnd.m_pPaintDC->SetWorldTransform(&identity);
		Wnd.m_pPaintDC->SetGraphicsMode(oldMode);
#endif
	}
	return true;
}
