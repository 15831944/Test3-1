//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __BIVARIATE_STATS_CPP
#include "Bivariate Stats.h"
#include <math.h>
#include <limits>
#pragma optimize("", off)

//====================================================================================

//TODO: A Proper icon
static double Drw_Stats[] = { MDrw_Poly,  -2.,2.,  2.,2.,  2.,-2., -2.,-2., -2.,2., MDrw_End };

//---------------------------------------------------------------------------

DEFINE_CONTROL_UNIT(BivarStats, "Bivariate Statistics", DLL_GroupName)
void BivarStats_UnitDef::GetOptions()
{
	SetDefaultTag("BS");
	SetDrawing("Tank", Drw_Stats);
	SetTreeDescription("Demo:Bivariate Statistics");
	SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
	SetModelGroup(MGroup_General);
};

//---------------------------------------------------------------------------

BivarStats::BivarStats(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
	//default values...
	lTagID[0] = lTagID[1] = -1;
	bOn = true;
	sTag[0] = "";
	sTag[1] = "";
	dHistoMinX = dHistoMinY= 0;
	dHistoMaxX = dHistoMaxY = 0;
	lHistoCount = 20;
	lRecordCount = 0;
	graphType = GT_SCATTER;

	//pHistoBucketBorders = NULL;
	pHistoBucketCounts = NULL;

	Reset();

	TagIO.Open(2);
}

//---------------------------------------------------------------------------

void BivarStats::Init()
{
}

//---------------------------------------------------------------------------

bool BivarStats::PreStartCheck()
{
	return true;
}

//---------------------------------------------------------------------------

void BivarStats::EvalCtrlInitialise(eScdCtrlTasks Tasks)
{
	//Reset();
}

//---------------------------------------------------------------------------

const long idDX_Tag = 10;
//const long idDX_Tag2 = 2;
const long idDX_Reset = 3;
const long idDX_RecordCount = 5;
const long idDX_HistoMinX = 6;
const long idDX_HistoMinY = 7;
const long idDX_HistoMaxX = 8;
const long idDX_HistoMaxY = 9;

void BivarStats::BuildDataFields()
{
	static MDDValueLst GraphValues[] = {
		{GT_SCATTER, "Scatter Plot"},
		{GT_DENSITY, "Density Plot"}};
	DD.CheckBox("Logging","Logging", &bOn, MF_PARAMETER);
	DD.String("StatTag1", "StatTag1", idDX_Tag, MF_PARAM_STOPPED);
	DD.String("StatTag2", "StatTag2", idDX_Tag + 1, MF_PARAM_STOPPED);
	DD.Button("Reset", "Reset", idDX_Reset);
	DD.Double("Graph x Minimum", "GraphMinx", idDX_HistoMinX, MF_PARAMETER);
	DD.Double("Graph x Maximum", "GraphMaxx", idDX_HistoMaxX, MF_PARAMETER);
	DD.Double("Graph y Minimum", "GraphMiny", idDX_HistoMinY, MF_PARAMETER);
	DD.Double("Graph y Maximum", "GraphMaxy", idDX_HistoMaxY, MF_PARAMETER);
	DD.Long("GraphType", "GraphType", (long*)&graphType, MF_PARAMETER, GraphValues);

	DD.Text("");
	DD.Double("Correlation", "Corr", &dCorrelation, MF_RESULT);
	DD.Text("Variable 1");
	DD.Double("Average", "Avg1", dAverage, MF_RESULT);
	DD.Double("Standard Deviation", "StdDev1", dStdDev, MF_RESULT);
	DD.Double("Minimum", "Min1", dMin, MF_RESULT);
	DD.Double("Maximum", "Max1", dMax, MF_RESULT);
	DD.Text("");
	DD.Text("Variable 2");
	DD.Double("Average", "Avg2", dAverage + 1, MF_RESULT);
	DD.Double("Standard Deviation", "StdDev2", dStdDev + 1, MF_RESULT);
	DD.Double("Minimum", "Min2", dMin + 1, MF_RESULT);
	DD.Double("Maximum", "Max2", dMax + 1, MF_RESULT);
	DD.Text("");
	DD.Long("Number of Records", "N", idDX_RecordCount, MF_RESULT | MF_SET_ON_CHANGE);

	DD.Show(false);
	DD.ArrayBegin("Histogram Rows", "Rows", lHistoCount);	
	for (int i = 0; i < lHistoCount; i++)
	{
		DD.ArrayElementStart(i);
		DD.ArrayBegin("Histogram Buckets", "Buckets", lHistoCount);
		for (int j = 0; j < lHistoCount; j++)
		{
			DD.ArrayElementStart(j);
			DD.Long("Count", "Count", pHistoBucketCounts + i * lRecordCount + j, MF_RESULT);
			DD.ArrayElementEnd(j);
		}
		DD.ArrayEnd();
		DD.ArrayElementEnd();
	}
	DD.ArrayEnd();

	DD.Double("SumX2_1", "SumX2_1", dSumX2, MF_RESULT);
	DD.Double("SumX2_2", "SumX2_2", dSumX2 + 1, MF_RESULT);
	DD.Double("SumX_1", "SumX_1", dSumX, MF_RESULT);
	DD.Double("SumX_2", "SumX_2", dSumX + 1, MF_RESULT);

	//TODO: Investigate a better method to store the record.
	/*if (cRecord.size() > 0)
	{
		DD.ArrayBegin("Record", "Record", lRecordCount);
		list<ValuePair>::iterator endIterator = cRecord.end();
		int j = 0;
		for (list<ValuePair>::iterator iterator = cRecord.begin(); iterator != endIterator; iterator++) {
			DD.ArrayElementStart(j++);
			DD.Double("Val1","Val1", &(iterator->v1), MF_RESULT);
			DD.Double("Val2","Val2", &(iterator->v2), MF_RESULT);
			DD.ArrayElementEnd();
		}
		DD.ArrayEnd();
	}*/
	DD.Show(true);
}

bool BivarStats::ExchangeDataFields()
{
	if (DX.Handle == idDX_Tag || DX.Handle == idDX_Tag + 1)
	{
		if (DX.HasReqdValue)
		{
			SetTag(DX.Handle - idDX_Tag, DX.String);
			Reset();
		}
		DX.String = sTag[DX.Handle - idDX_Tag];
		return true;
	}
	switch (DX.Handle)
	{
	case (idDX_Reset):
		if (DX.HasReqdValue && DX.Bool == true)
			Reset();
		DX.Bool = false;
		return true;
	case (idDX_HistoMaxX):
		if (DX.HasReqdValue)
		{
			dHistoMaxX = DX.Double;
			RecalculateHistoBuckets();
		}
		DX.Double = dHistoMaxX;
		return true;
	case (idDX_HistoMinX):
		if (DX.HasReqdValue)
		{
			dHistoMinX = DX.Double;
			RecalculateHistoBuckets();
		}
		DX.Double = dHistoMinX;
		return true;
	case (idDX_HistoMaxY):
		if (DX.HasReqdValue)
		{
			dHistoMaxY = DX.Double;
			RecalculateHistoBuckets();
		}
		DX.Double = dHistoMaxY;
		return true;
	case (idDX_HistoMinY):
		if (DX.HasReqdValue)
		{
			dHistoMinY = DX.Double;
			RecalculateHistoBuckets();
		}
		DX.Double = dHistoMinY;
		return true;
	case(idDX_RecordCount):
		if (DX.HasReqdValue)	//Should only be called on initilisation
		{
			lRecordCount = DX.Long;
			cRecord.clear();
			ValuePair zero = {0,0};
			for (int i = 0; i < lRecordCount; i++)
				cRecord.push_back(zero);
		}
		DX.Long = lRecordCount;
		return true;
	}
	return false;
}

bool BivarStats::ValidateDataFields()
{
	if (lHistoCount < 1)
		lHistoCount = 1;
	return true;
}

void BivarStats::EvalCtrlStrategy(eScdCtrlTasks Tasks)
{
	try
	{
		if (lTagID[0] < 0 || lTagID[1] < 0 || !bOn)
			return;
		double dVal1 = TagIO.DValue[lTagID[0]];
		double dVal2 = TagIO.DValue[lTagID[1]];
		RecalculateStats(dVal1, dVal2);
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

void BivarStats::Reset()
{
	dMin[0] = dMin[1] = dMax[0] = dMax[1] = dNAN;
	dStdDev[0] = dStdDev[1] = dNAN;
	dAverage[0] = dAverage[1] = dNAN;
	dCorrelation = dNAN;
	lRecordCount = 0;
	cRecord.clear();
	dSumX[0] = dSumX[1] = dSumX2[0] = dSumX2[1] = dSumXY = 0;
	RecalculateHistoBuckets();
}

void BivarStats::SetTag(int n, CString newValue)
{
	sTag[n] = newValue;

	int curTag = TagIO.FindTag(sTag[n]);
	if (curTag >= 0)
	{
		lTagID[n] = curTag; //This could cause leaks, but it's unlikely and minor enough to leave.
	}
	else
	{
		if (lTagID[n] >= 0 && lTagID[0] != lTagID[1])
			TagIO.Remove(lTagID[n]);
		CString name;
		name.Format("TagToGet%d", n);
		lTagID[n] = TagIO.Add(DX.String, name, MTIO_Get);
	}
}

void BivarStats::RecalculateStats(double newEntry1, double newEntry2)
{
	double newEntries[2] = {newEntry1, newEntry2};
	
	// Update records
	lRecordCount++;
	ValuePair newPair = {newEntry1, newEntry2};
	cRecord.push_back(newPair);


	for (int i = 0; i < 2; i++)
	{
		// Update min and max
		if (_isnan(dMin[i]) || (dMin[i] > newEntries[i]))
			dMin[i] = newEntries[i];
		if (_isnan(dMax[i]) || (dMax[i] < newEntries[i]))
			dMax[i] = newEntries[i];
		// Update average and std dev.
		dSumX[i] += newEntries[i];
		dSumX2[i] += newEntries[i] * newEntries[i];
		dAverage[i] = dSumX[i] / lRecordCount;
		if (lRecordCount <= 1)
			dStdDev[i] = dNAN;
		else
		{
			double a = dSumX2[i] - lRecordCount * dAverage[i] * dAverage[i];
			if (a > 0) //If we are logging a zero std dev. parameter, rounding errors can cause an exception
				dStdDev[i] = sqrt(a/(lRecordCount - 1));
			else
				dStdDev[i] = 0;
		}
	}

	dSumXY += newEntry1 * newEntry2;
	if (_isnan(dStdDev[0]) || dStdDev[0] == 0 || _isnan(dStdDev[1]) || dStdDev[1] == 0)
		dCorrelation = dNAN;
	else
		dCorrelation = (dSumXY / lRecordCount - dAverage[0] * dAverage[1]) / (dStdDev[0] * dStdDev[1]);
	
	// Update histogram (Only if it is in range)
	if (newEntry1 > dHistoMinX && newEntry1 < dHistoMaxX
		&& newEntry2 > dHistoMinY && newEntry2 < dHistoMaxY)
		pHistoBucketCounts[
			(int)((newEntry2 - dHistoMinY) / (dHistoMaxY - dHistoMinY) * lHistoCount ) * lHistoCount
			+ (int)((newEntry1 - dHistoMinX) / (dHistoMaxX - dHistoMinX) * lHistoCount)]++;
}

void BivarStats::RecalculateHistoBuckets()
{
	if (pHistoBucketCounts != NULL)
		delete[] pHistoBucketCounts;
	pHistoBucketCounts = new long[lHistoCount * lHistoCount];
	for (int i = 0; i < lHistoCount * lHistoCount; i++)
		pHistoBucketCounts[i] = 0;
	list<ValuePair>::const_iterator endIterator = cRecord.end();
	for (list<ValuePair>::const_iterator iterator = cRecord.begin(); iterator != endIterator; iterator++)
	{
		double newEntry1 = iterator->v1;
		double newEntry2 = iterator->v2;
		if (newEntry1 > dHistoMinX && newEntry1 < dHistoMaxX
			&& newEntry2 > dHistoMinY && newEntry2 < dHistoMaxY)
			pHistoBucketCounts[
				(int)((newEntry2 - dHistoMinY) / (dHistoMaxY - dHistoMinY) * lHistoCount) * lHistoCount
				+ (int)((newEntry1 - dHistoMinX) / (dHistoMaxX - dHistoMinX) * lHistoCount)]++;
	}
}

bool BivarStats::GetModelGraphic(CMdlGraphicArray & Grfs)
{
	CMdlGraphic G0(0, MGT_Simple, true, "Graph", 1);

	Grfs.SetSize(0);
	Grfs.SetAtGrow(0, G0);
	return true;
}

bool BivarStats::OperateModelGraphic(CMdlGraphicWnd &Wnd, CMdlGraphic &Grf)
{
	const COLORREF White = COLORREF(RGB(255,255,255));
	const COLORREF Black = COLORREF(RGB(0,0,0));
	const COLORREF Blue  = COLORREF(RGB(0,0,255));
	const COLORREF Cyan  = COLORREF(RGB(0,255,255));
	const COLORREF Red   = COLORREF(RGB(255,0,0));
	const COLORREF Green = COLORREF(RGB(0,255,0));

	int nTextSize = Wnd.m_TextSize.y;
	int nBorderSpace = nTextSize;
	int nAxesSpace = 4;
	int nCheckSize = nAxesSpace - 1;
	int nArrowSize = 3;

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

		CPen* oldPen = Wnd.m_pPaintDC->SelectObject(&penGreen);
		int xLabelWidth = Wnd.m_pPaintDC->GetTextExtent(sTag[0]).cx;
		int nAxesLeft = nBorderSpace,
			nAxesTop = nBorderSpace + nTextSize + nAxesSpace + nArrowSize,
			nAxesBottom = Wnd.m_ClientRect.bottom - nBorderSpace,
			nAxesRight = Wnd.m_ClientRect.right - xLabelWidth - nBorderSpace - nArrowSize - nAxesSpace;
		POINT AxesCross = {(nAxesLeft + nAxesRight) / 2, (nAxesTop + nAxesBottom) / 2};
		SIZE AxesSize = {nAxesRight - nAxesLeft, nAxesBottom - nAxesTop};

		//DrawData:
		if (graphType == GT_SCATTER)
		{
			list<ValuePair>::iterator endIterator = cRecord.end();
			for (list<ValuePair>::iterator iterator = cRecord.begin(); iterator != endIterator; iterator++)
			{
				if (iterator->v1 > dHistoMinX && iterator->v1 < dHistoMaxX
					&& iterator->v2 > dHistoMinY && iterator->v2 < dHistoMaxY)
				{
					POINT pt = {
						nAxesLeft + (int)((iterator->v1 - dHistoMinX) / (dHistoMaxX - dHistoMinX) * AxesSize.cx),
						nAxesBottom - (int)((iterator->v2 - dHistoMinY) / (dHistoMaxY - dHistoMinY) * AxesSize.cy)};
					Wnd.m_pPaintDC->SetPixel(pt, White);
				}
			}
		}
		else //We are drawing a density plot...
		{
			int maxCount = 0;
			for (int i = 0; i < lHistoCount * lHistoCount; i++)
				if (pHistoBucketCounts[i] > maxCount)
					maxCount = pHistoBucketCounts[i];
			if (maxCount != 0)
				for (int i = 0; i < lHistoCount; i++)
					for (int j = 0; j < lHistoCount; j++)
					{
						int val = 0xFF * pHistoBucketCounts[i * lHistoCount + j] / maxCount;
						COLORREF blockColour = COLORREF(RGB(val,val,val));
						CBrush brush(blockColour);
						RECT rect = {
							nAxesLeft + j * AxesSize.cx / lHistoCount, nAxesBottom - i * AxesSize.cy / lHistoCount,
							nAxesLeft + (j + 1) * AxesSize.cx / lHistoCount, nAxesBottom - (i + 1) * AxesSize.cy / lHistoCount};
						Wnd.m_pPaintDC->FillRect(&rect, &brush);
					}
		}

		//Draw axes:
		POINT XAxis[] = {
			{nAxesLeft, AxesCross.y},
			{nAxesRight, AxesCross.y}};
		Wnd.m_pPaintDC->Polyline(XAxis, 2);
		POINT YAxis[] = {
			{AxesCross.x, nAxesTop},
			{AxesCross.x, nAxesBottom}};
		Wnd.m_pPaintDC->Polyline(YAxis, 2);

		POINT ArrowLeft[] = {
			{nAxesLeft - nArrowSize, AxesCross.y},
			{nAxesLeft, AxesCross.y + nArrowSize},
			{nAxesLeft, AxesCross.y - nArrowSize},
			{nAxesLeft - nArrowSize, AxesCross.y}};
		Wnd.m_pPaintDC->Polygon(ArrowLeft, 4);
		POINT ArrowRight[] = {
			{nAxesRight + nArrowSize, AxesCross.y},
			{nAxesRight, AxesCross.y + nArrowSize},
			{nAxesRight, AxesCross.y - nArrowSize},
			{nAxesRight + nArrowSize, AxesCross.y}};
		Wnd.m_pPaintDC->Polygon(ArrowRight, 4);
		POINT ArrowTop[] = {
			{AxesCross.x, nAxesTop - nArrowSize},
			{AxesCross.x + nArrowSize, nAxesTop},
			{AxesCross.x - nArrowSize, nAxesTop},
			{AxesCross.x, nAxesTop - nArrowSize}};
		Wnd.m_pPaintDC->Polygon(ArrowTop, 4);
		POINT ArrowBottom[] = {
			{AxesCross.x, nAxesBottom + nArrowSize},
			{AxesCross.x + nArrowSize, nAxesBottom},
			{AxesCross.x - nArrowSize, nAxesBottom},
			{AxesCross.x, nAxesBottom + nArrowSize}};


		//Draw labels:
		Wnd.m_pPaintDC->SetTextColor(Green);
		Wnd.m_pPaintDC->SetTextAlign(TA_BOTTOM | TA_LEFT);
		Wnd.m_pPaintDC->TextOut(AxesCross.x, nAxesTop - nArrowSize - nAxesSpace, sTag[1]);
		Wnd.m_pPaintDC->SetTextAlign(TA_TOP | TA_LEFT);
		Wnd.m_pPaintDC->TextOut(nAxesRight + nArrowSize + nAxesSpace, AxesCross.y, sTag[0]);
	}
	return true;
}