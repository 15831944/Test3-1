//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __TAILINGS_GRAPHIC_CPP
#include "Tailings Graphic.h"
#include <math.h>
#pragma optimize("", off)

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define BOUND(n, lo, hi) (n < lo ? lo : (n > hi ? hi : n))

#define TAG_OK(item) (item.CheckTag() == MTagIO_OK || item.CheckTag() == MTagIO_ReadOnly)

//====================================================================================

//TODO: A Proper icon
static double Drw_TailingsDam[] = { MDrw_Poly,  -2.,2.,  2.,2.,  2.,-2., -2.,-2., -2.,2., MDrw_End };

//---------------------------------------------------------------------------

DEFINE_CONTROL_UNIT(TailingsGraphic, "Tailings Dam Graphic", DLL_GroupName)
void TailingsGraphic_UnitDef::GetOptions()
{
	SetDefaultTag("TG");
	SetDrawing("Tank", Drw_TailingsDam);
	SetTreeDescription("Demo:Tailings Graphic");
	SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
	SetModelGroup(MGroup_General);
};

//---------------------------------------------------------------------------

TailingsGraphic::TailingsGraphic(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd),
	FluidMassSubs(TagIO), SolidMassSubs(TagIO),
	ConcSubs(TagIO), VolSubs(TagIO), LiquidDensitySubs(TagIO)
{
	dMoistFrac = 0;
	dSatConc = 1;
	dEvapRate = dRainRate = 0;
	nDataPointCount = 2;
	vInterpolationHeights.push_back(0);
	vInterpolationHeights.push_back(1);
	vInterpolationAreas.resize(2, 1);
	vVolumeLookup.resize(2);
	SortInterpolationPoints();
	lEvapRateUnits = 0;

	eIntMethod = IM_Linear;

	dFluidLevel = dSolidLevel = dFSA = 0;
	dFluidMass = dSolidMass = 0;

	RecalculateVolumes(); //This is called simply to ensure that it has been called once, and so the arrays are in the proper state for any other function.
}

//---------------------------------------------------------------------------

void TailingsGraphic::Init()
{
}

//---------------------------------------------------------------------------

bool TailingsGraphic::PreStartCheck()
{
	return true;
}

//---------------------------------------------------------------------------

const int MaxIntPoints = 100;

const int idDX_TankString = 1;
const int idDX_DataPointCount = 2;
const int idDX_ConcSpecies = 3;
const int idDX_InterpolationHeight = MaxIntPoints;
const int idDX_InterpolationArea = 2 * MaxIntPoints;

void TailingsGraphic::BuildDataFields()
{
	static MCnvFamily qmFamily = gs_Cnvs[MC_Qm.Index];
	static MDDValueLst* DDBQm = NULL;
	if (DDBQm == NULL)
	{
		DDBQm = new MDDValueLst[qmFamily.Count() + 1];
		for (int i = 0; i < qmFamily.Count(); i++)
		{
			int dstSize = strlen(qmFamily[i].Name()) + 1;
			char* nonConst = new char[dstSize];
			strcpy(nonConst, qmFamily[i].Name());

			MDDValueLst cur = {i, nonConst};
			DDBQm[i] = cur;
		}
		MDDValueLst terminator = {0};
		DDBQm[qmFamily.Count()] = terminator;
	}

	// -- User entered data --
	//Area calc: Number of interpolation points, and points
	//solid moisture fraction, saturation concentration
	//Rate of evaporation, rate of rainfall
	//Tank name.
	// -- Data read from tank -- 
	//liquid mass, solid mass, concentrations, densities.
	DD.String("Tank", "", idDX_TankString, MF_PARAMETER);
	DD.Double("Moisture_Fraction", "", &dMoistFrac, MF_PARAMETER, MC_Frac);
	DD.String("Specie_Of_Interest", "", idDX_ConcSpecies, MF_PARAMETER);
	DD.Double("Saturation_Concentration", "", &dSatConc, MF_PARAMETER, MC_Conc);
	DD.Double("Evaporation_Rate", "", &dEvapRate, MF_PARAMETER, MC_Qm);
	DD.Double("Rainfall_Rate", "", &dRainRate, MF_PARAMETER, MC_Qm);
	DD.Long("Evap_And_Rain_Rate_Units", "", &lEvapRateUnits, MF_PARAMETER, DDBQm);
	DD.Double("Max_Capacity", "", &dMaxCapacity, MF_RESULT, MC_Vol);
	DD.Text("");
	DD.Double("Fluid_Level", "", &dFluidLevel, MF_RESULT, MC_L);
	DD.Double("Solid_Level", "", &dSolidLevel, MF_RESULT, MC_L);
	DD.Double("Fluid_Surface_Area", "", &dFSA, MF_RESULT, MC_Area);
	DD.Double("Concentration_Of_Interest", "", &dConc, MF_RESULT, MC_Conc);

	DD.Page("Surface_Volume_Calcs");
	DD.Long("Data_Point_Count", "", idDX_DataPointCount, MF_PARAMETER);
	//Todo: Interpolation type.
	DD.ArrayBegin("Data_Points", "Data_Points", nDataPointCount);
	for (int i = 0; i < nDataPointCount; i++)
	{
		DD.ArrayElementStart(i);
		DD.Double("Height", "", idDX_InterpolationHeight + i, i == 0 ? MF_RESULT : MF_PARAMETER, MC_L);
		DD.Double("Area", "", idDX_InterpolationArea + i, MF_PARAMETER, MC_Area);
		DD.ArrayElementEnd();
	}
	DD.ArrayEnd();
}

bool TailingsGraphic::ExchangeDataFields()
{
	if (DX.Handle == idDX_TankString)
	{
		if (DX.HasReqdValue)
		{
			sTankName = DX.String;
			SetTags();
		}
		DX.String = sTankName;
		return true;
	}
	if (DX.Handle == idDX_DataPointCount)
	{
		if (DX.HasReqdValue)
		{
			SetIntPtCount(DX.Long);
			RecalculateVolumes();
			RecalculateLevels();
		}
		DX.Long = nDataPointCount;
		return true;
	}
	if (idDX_InterpolationHeight <= DX.Handle && DX.Handle <= idDX_InterpolationHeight + nDataPointCount)
	{
		if (DX.HasReqdValue)
		{
			if (DX.Double < 0)
				vInterpolationHeights.at(DX.Handle - idDX_InterpolationHeight) = 0;
			else
				vInterpolationHeights.at(DX.Handle - idDX_InterpolationHeight) = DX.Double;
			SortInterpolationPoints();
			RecalculateVolumes();
			RecalculateLevels();
		}
		DX.Double = vInterpolationHeights.at(DX.Handle - idDX_InterpolationHeight);
		return true;
	}
	if (idDX_InterpolationArea <= DX.Handle && DX.Handle <= idDX_InterpolationArea + nDataPointCount)
	{
		if (DX.HasReqdValue)
		{
			if (DX.Double < 0)
				vInterpolationAreas.at(DX.Handle - idDX_InterpolationArea) = 0;
			else
				vInterpolationAreas.at(DX.Handle - idDX_InterpolationArea) = DX.Double;
			SortInterpolationPoints();	//Because an update is needed.
			RecalculateVolumes();
			RecalculateLevels();
		}
		DX.Double = vInterpolationAreas.at(DX.Handle - idDX_InterpolationArea);
		return true;
	}
	if (DX.Handle == idDX_ConcSpecies)
	{
		if (DX.HasReqdValue)
		{
			sConcSpecies = DX.String;
			int loc = sConcSpecies.ReverseFind('.');
			if (loc > 0);
				sConcSpecies = sConcSpecies.Right(sConcSpecies.GetLength() - loc - 1);

			/*if (sConcSpecies.GetLength() > 0 && 
				(sConcSpecies.GetLength() < 4 || sConcSpecies.Right(4) != "(aq)"))
				sConcSpecies.Append("(aq)");*/
		}
		DX.String = sConcSpecies;
		SetTags();
		return true;
	}
	return false;
}

void TailingsGraphic::SetTags()
{
	CString temp;
	int i = 0;
	sTankName = sTankName.Tokenize(".", i);

	temp.Format("%s%s", sTankName, ".Content.Liquids");
	FluidMassSubs.Tag = temp;

	temp.Format("%s%s", sTankName, ".Content.Solids");
	SolidMassSubs.Tag = temp;

	temp.Format("%s%s", sTankName, ".Content.Vt");
	VolSubs.Tag = temp;

	temp.Format("%s%s", sTankName, ".Content.LRho");
	LiquidDensitySubs.Tag = temp;

	temp.Format("%s%s%s", sTankName, ".Content.", sConcSpecies);
	ConcSubs.Tag = temp;

	RecalculateLevels();
}

bool TailingsGraphic::ValidateDataFields()
{
	if (dMoistFrac < 0) dMoistFrac = 0;
	if (dEvapRate < 0) dEvapRate = 0;
	if (dRainRate < 0) dRainRate = 0;
	if (dSatConc < 0) dSatConc = 0;
	if (TagIO.ValidateReqd())
	{
		if (TagIO.StartValidateDataFields())
		{
			FluidMassSubs.Configure(0, NULL, "FluidMassTag", MTagIO_Get);
			SolidMassSubs.Configure(1, NULL, "SolidMassTag", MTagIO_Get);
			ConcSubs.Configure(2, NULL, "ConcTag", MTagIO_Get);
			VolSubs.Configure(3, NULL, "VolTag", MTagIO_Get);
			LiquidDensitySubs.Configure(4, NULL, "LiquidDensityTag", MTagIO_Get);
		}
		TagIO.EndValidateDataFields();
	}
	RecalculateVolumes();
	bool ret = RecalculateLevels();
	Log.SetCondition(!ret, 0, MMsg_Error, "Unable to retrieve critical tank information");
	return ret;

	return true;
}

void TailingsGraphic::SortInterpolationPoints()
{
	//We need a stable sort of a relatively small dataset, which should not be called too often.
	bool changed = true;
	vSortedHeights.clear();
	vSortedAreas.clear();
	
	vSortedHeights.push_back(0);
	vSortedAreas.push_back(vInterpolationAreas.at(0));

	//We can use an insertion sort on a vector (even though this is inefficient), since it should be a small dataset, mostly ordered, and a seldom called function.
	for (int i = 1; i < nDataPointCount; i++)
	{
		double h = vInterpolationHeights.at(i),
			a = vInterpolationAreas.at(i);
		vector<double>::iterator hIterator = vSortedHeights.begin();
		vector<double>::iterator aIterator = vSortedAreas.begin();
		while (hIterator != vSortedHeights.end() && *hIterator <= h)
		{
			hIterator++;
			aIterator++;
		}
		/*hIterator--;
		aIterator--;*/
		vSortedHeights.insert(hIterator, h);
		vSortedAreas.insert(aIterator, a);
	}
}

//---------------------------------------------------------------------------

bool TailingsGraphic::GetModelGraphic(CMdlGraphicArray & Grfs)
{
	CMdlGraphic G0(0, MGT_Simple, true, "Graphic", 1);

	Grfs.SetSize(0);
	Grfs.SetAtGrow(0, G0);
	return true;
}

bool TailingsGraphic::OperateModelGraphic(CMdlGraphicWnd &Wnd, CMdlGraphic &Grf)
{
	const COLORREF White = COLORREF(RGB(255,255,255));
	const COLORREF Black = COLORREF(RGB(0,0,0));
	const COLORREF Grey = COLORREF(RGB(192, 192, 192));
	const COLORREF Blue  = COLORREF(RGB(0,0,255));
	const COLORREF Cyan  = COLORREF(RGB(0,255,255));
	const COLORREF Red   = COLORREF(RGB(255,0,0));
	const COLORREF Green = COLORREF(RGB(0,255,0));
	const COLORREF Brown = COLORREF(RGB(205, 146, 53));
	switch (Wnd.m_eTask)
	{
	case MGT_Create:
		Wnd.m_pWnd->SetWindowPos(NULL, 0, 0, 300, 250, SWP_NOMOVE | SWP_NOZORDER);
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
		double dFlowFrac = 0.2;
		double dOverflowFrac = 0.07;
		double dOutYFrac = 0.15;
		double dOutXFrac = 0.2;

		CRect topRect = Wnd.m_ClientRect;
		topRect.left += nBorderSpace; topRect.top += nBorderSpace;
		topRect.bottom -= nBorderSpace; topRect.right -= nBorderSpace;
		topRect.bottom = topRect.top + topRect.Height() * dFlowFrac;

		CRect lowerRect = Wnd.m_ClientRect;
		lowerRect.top = topRect.bottom;
		lowerRect.bottom -= nBorderSpace;

		double maxHeight = vSortedHeights.back();
		if (maxHeight == 0) return true;

		CRect insideRect;
			insideRect.left = lowerRect.left + lowerRect.Width() * dOutXFrac;
			insideRect.top = lowerRect.top + lowerRect.Height() * dOverflowFrac;
			insideRect.right = lowerRect.right - lowerRect.Width() * dOutXFrac;
			insideRect.bottom = lowerRect.bottom;

		//Draw dam shape:
		int ptCount;
		POINT* insidePts = GetDamPoints(maxHeight, insideRect, ptCount);
		POINT* Dam = new POINT[2 + ptCount];
		Dam[0].x = lowerRect.left; Dam[1 + ptCount].x = lowerRect.right;
		Dam[0].y = Dam[1 + ptCount].y = lowerRect.top + lowerRect.Height() * (dOverflowFrac + dOutYFrac);
		memcpy(Dam + 1, insidePts, ptCount * sizeof(POINT));
		delete[] insidePts;
		
		CPen damPen(PS_SOLID, 3, Grey);
		CPen* oldPen = Wnd.m_pPaintDC->SelectObject(&damPen);
		Wnd.m_pPaintDC->Polyline(Dam, 2 + ptCount);
		delete[] Dam;

		CPen nullPen(PS_NULL, 0, White);
		Wnd.m_pPaintDC->SelectObject(nullPen);

		//Draw fluid:
		BYTE pureR = 0, pureG = 0, pureB = 255;
		BYTE satR = 255, satG = 0, satB = 0;
		POINT* fluid = GetDamPoints(dFluidLevel, insideRect, ptCount);

		//A dSatConc of zero means just display pure water...
		double concFrac = dSatConc >= 0 ? BOUND(dConc / dSatConc, 0, 1) : 0;
		COLORREF fluidColour = RGB(
			pureR * (1 - concFrac) + satR * concFrac,
			pureG * (1 - concFrac) + satG * concFrac,
			pureB * (1 - concFrac) + satB * concFrac);
		CBrush fluidBrush(fluidColour);
		CBrush* oldBrush = Wnd.m_pPaintDC->SelectObject(&fluidBrush);
		Wnd.m_pPaintDC->Polygon(fluid, ptCount);
		delete[] fluid;
				
		//Draw sediment:
		POINT* sediment = GetDamPoints(dSolidLevel, insideRect, ptCount);
		CBrush sedimentBrush(Brown);
		Wnd.m_pPaintDC->SelectObject(&sedimentBrush);
		Wnd.m_pPaintDC->Polygon(sediment, ptCount);
		delete[] sediment;

		//Draw overflow:
		/*if (bOverflowing)
		{
			Wnd.m_pPaintDC->SelectObject(&fluidBrush);
			POINT overflowLeft[] = {
				{lowerRect.left, lowerRect.top + lowerRect.Height() * (dOverflowFrac + dOutYFrac)},
				{lowerRect.left + lowerRect.Width() * dOutXFrac, lowerRect.top + lowerRect.Height() * dOverflowFrac},
				{lowerRect.left + lowerRect.Width() * dOutXFrac, lowerRect.top},
				{lowerRect.left, lowerRect.top + lowerRect.Height() * dOutYFrac}};
			POINT overflowRight[] = {
				{lowerRect.right, lowerRect.top + lowerRect.Height() * (dOverflowFrac + dOutYFrac)},
				{lowerRect.right - lowerRect.Width() * dOutXFrac, lowerRect.top + lowerRect.Height() * dOverflowFrac},
				{lowerRect.right - lowerRect.Width() * dOutXFrac, lowerRect.top},
				{lowerRect.right, lowerRect.top + lowerRect.Height() * dOutYFrac}};
				Wnd.m_pPaintDC->Polygon(overflowLeft, 4);
				Wnd.m_pPaintDC->Polygon(overflowRight, 4);
		}*/

		//Draw arrows:
		int nArrowSize = 8;

		double offset = 0, scale = 1;
		static MCnvFamily qmFamily = gs_Cnvs[MC_Qm.Index];

		if (qmFamily[lEvapRateUnits].Valid())
		{
			offset = qmFamily[lEvapRateUnits].Offset();
			scale = qmFamily[lEvapRateUnits].Scale();
		}

		//Evaporation:
		CPen arrowPen(PS_SOLID, 2, Blue);
		Wnd.m_pPaintDC->SelectObject(arrowPen);
		if (dRainRate > 0)
		{
			for (int i = 1; i < 4; i++)
			{
				POINT Arrow1[] = {
					{topRect.left + i * topRect.Width() / 10, topRect.top},
					{topRect.left + (i + 1) * topRect.Width() / 10, topRect.bottom}};
				POINT Arrow2[] = {
					{topRect.left + (i + 1) * topRect.Width() / 10 - nArrowSize + 1, topRect.bottom},
					{topRect.left + (i + 1) * topRect.Width() / 10 + 1, topRect.bottom},
					{topRect.left + (i + 1) * topRect.Width() / 10 + 1, topRect.bottom - nArrowSize}};

				Wnd.m_pPaintDC->Polyline(Arrow1, 2);
				Wnd.m_pPaintDC->Polyline(Arrow2, 3);
			}
			Wnd.m_pPaintDC->SetTextAlign(TA_CENTER | TA_TOP);
			CString rainString;
			rainString.Format("Rainfall: %.2f %s", dRainRate * scale + offset, qmFamily[lEvapRateUnits].Name());
			CSize txtSize = Wnd.m_pPaintDC->GetTextExtent(rainString);
			Wnd.m_pPaintDC->FillSolidRect(
				topRect.left + topRect.Width() / 4 - txtSize.cx / 2 - 1,
				topRect.CenterPoint().y - nTextSize / 2 - 1,
				txtSize.cx + 2, txtSize.cy + 2,
				Black);
			Wnd.m_pPaintDC->TextOut(
				topRect.left + topRect.Width() / 4,
				topRect.CenterPoint().y - nTextSize / 2,
				rainString);
		}
		if (dEvapRate > 0)
		{
			for (int i = 6; i < 9; i++)
			{
				POINT Arrow1[] = {
					{topRect.left + i * topRect.Width() / 10, topRect.bottom},
					{topRect.left + (i + 1) * topRect.Width() / 10, topRect.top}};
				POINT Arrow2[] = {
					{topRect.left + (i + 1) * topRect.Width() / 10 - nArrowSize + 1, topRect.top},
					{topRect.left + (i + 1) * topRect.Width() / 10 + 1, topRect.top},
					{topRect.left + (i + 1) * topRect.Width() / 10 + 1, topRect.top + nArrowSize}};

				Wnd.m_pPaintDC->Polyline(Arrow1, 2);
				Wnd.m_pPaintDC->Polyline(Arrow2, 3);
			}
			Wnd.m_pPaintDC->SetTextAlign(TA_CENTER | TA_TOP);
			CString evapString;
			evapString.Format("Evaporation: %.2f %s", dEvapRate * scale + offset, qmFamily[lEvapRateUnits].Name());
			CSize txtSize = Wnd.m_pPaintDC->GetTextExtent(evapString);
			Wnd.m_pPaintDC->FillSolidRect(
				topRect.left + 3 * topRect.Width() / 4 - txtSize.cx / 2 - 1,
				topRect.CenterPoint().y - nTextSize / 2 - 1,
				txtSize.cx + 2, txtSize.cy + 2,
				Black);
			Wnd.m_pPaintDC->TextOut(
				topRect.left + 3 * topRect.Width() / 4,
				topRect.CenterPoint().y - nTextSize / 2,
				evapString);
		}

		Wnd.m_pPaintDC->SelectObject(oldBrush);
		Wnd.m_pPaintDC->SelectObject(oldPen);
	}
	return true;
}

POINT* TailingsGraphic::GetDamPoints(double damHeight, CRect insideRect, int& ptCount)
{
	double maxHeight = vSortedHeights.back();
	
	double maxArea = 0;
	for (int i = 0; i < nDataPointCount; i++)
		if (vSortedAreas.at(i) > maxArea)
			maxArea = vSortedAreas.at(i);
	if (maxArea == 0)
	{
		POINT* nullRet = new POINT[2];
		nullRet[1].y = nullRet[0].y = insideRect.bottom; 
		nullRet[0].x = insideRect.left;
		nullRet[1].y = insideRect.right;
		ptCount = 2;
		return nullRet;
	}

	int c = 0;
	while (damHeight > vSortedHeights.at(c))
		c++;
	c++;
	
	POINT* ret = new POINT[2 * c];
	ptCount = 2 * c;

	for (int i = 0; i < c; i++)
	{
		int Afactor = 0.5 * insideRect.Width() * vSortedAreas.at(i) / maxArea;
		ret[c - 1 - i].x = insideRect.CenterPoint().x - Afactor;
		ret[c   +   i].x = insideRect.CenterPoint().x + Afactor;
		ret[c - 1 - i].y = ret[c + i].y = insideRect.bottom - insideRect.Height() * vSortedHeights.at(i) / maxHeight;
	}

	//Final points are interpolated:
	if (c == 1) //If we have only one point, we have a level of zero and don't need to interpolate.
		return ret;

	ret[0].y = ret[2 * c - 1].y = insideRect.bottom - insideRect.Height() * damHeight / maxHeight;
	double heightDiff = damHeight - vSortedHeights.at(c - 2);
	double Agrad;
	if (vSortedHeights.at(c - 1) == vSortedHeights.at(c - 2))
		Agrad = 0; //The easy way to avoid divide by zero errors.
	else
		Agrad = (vSortedAreas.at(c - 1) - vSortedAreas.at(c - 2)) / (vSortedHeights.at(c - 1) - vSortedHeights.at(c - 2));

	double AatH = vSortedAreas.at(c - 2) + heightDiff * Agrad;
	int Afactor2 = 0.5 * insideRect.Width() * AatH / maxArea;
	ret[0].x = insideRect.CenterPoint().x - Afactor2;
	ret[2 * c - 1].x = insideRect.CenterPoint().x + Afactor2;
	return ret;
}

//---------------------------------------------------------------------------

//Should this be in GetModelAction rather than EvalCtrlActions?
void TailingsGraphic::EvalCtrlActions(eScdCtrlTasks Tasks)
{
	RecalculateLevels(); //TODO: Check whether we should report a zero fluid surface area if it is solid on the surface.
}

void TailingsGraphic::SetIntPtCount(int count)
{
	if (count < 2)
		count = 2;
	if (count > MaxIntPoints)
		count = MaxIntPoints;
	
	double topHeight = vSortedHeights.back();
	double topArea = vSortedAreas.back();
	vInterpolationHeights.resize(count, topHeight);
	vInterpolationAreas.resize(count, topArea);
	vSortedHeights.resize(count, topHeight);
	vSortedAreas.resize(count, topHeight);
	vVolumeLookup.resize(count);

	nDataPointCount = count;
}

void TailingsGraphic::RecalculateVolumes()
{
	double curVol = 0;
	vVolumeLookup.at(0) = 0;
	switch (eIntMethod)
	{
	case IM_Linear:
		for (int i = 0; i < nDataPointCount - 1; i++)
		{
			curVol += 0.5 * (vSortedAreas.at(i) + vSortedAreas.at(i + 1)) * (vSortedHeights.at(i + 1) - vSortedHeights.at(i));
			vVolumeLookup.at(i + 1) = curVol;
		}
		break;
	}
	dMaxCapacity = vVolumeLookup.back();
}

double TailingsGraphic::CalcLevel(double volume)
{
	if (volume >= dMaxCapacity)
		return vSortedHeights.back();
	if (volume <= 0)
		return 0;
	int i = 0;
	while (vVolumeLookup.at(i) < volume) //This finds the first interpolation point that is above the current volume.
		i++;
	i--;	//But because it's easier to think as extrapolating up, I'm going to decrement it.

	if (vSortedHeights.at(i + 1) == vSortedHeights.at(i)) //Although this should be impossible.
		return vSortedHeights.at(i);

	switch (eIntMethod)
	{
	case IM_Linear:
	default:
		double deltaV = volume - vVolumeLookup.at(i);
		double A0 = vSortedAreas.at(i);
		
		double Agrad = (vSortedAreas.at(i + 1) - vSortedAreas.at(i)) / (vSortedHeights.at(i + 1) - vSortedHeights.at(i));

		if (Agrad != 0)
			return vSortedHeights.at(i) + (-A0 + sqrt(A0 * A0 + 4 * Agrad * deltaV)) / (2 * Agrad);
		else if (A0 != 0)
			return vSortedHeights.at(i) + deltaV / A0;
		else
			return vSortedHeights.at(i); //Well I go no idea how A0 can be zero. But It's easier to simply make sure we never divide by zero.
	}
}

double TailingsGraphic::CalcArea(double height)
{
	if (height >= vSortedHeights.back())
		return vSortedAreas.back();
	if (height <= 0)
		return vSortedAreas.at(0);
	int i = 0;
	while (vSortedHeights.at(i) < height)
		i++;
	i--;

	if (vSortedHeights.at(i + 1) == vSortedHeights.at(i)) //Although this should be impossible.
		return vSortedAreas.at(i);

	switch (eIntMethod)
	{
	case IM_Linear:
	default:
		double deltaH = height - vSortedHeights.at(i);
		double Agrad = (vSortedAreas.at(i + 1) - vSortedAreas.at(i)) / (vSortedHeights.at(i + 1) - vSortedHeights.at(i));
		return vSortedAreas.at(i) + deltaH * Agrad;
	}
}

bool TailingsGraphic::RecalculateLevels()
{
	if (!LiquidDensitySubs.IsActive) //If we can't get these critical values, no point even trying.
		return false;
	if (!VolSubs.IsActive) return false;
	if (!FluidMassSubs.IsActive) return false;
	if (!SolidMassSubs.IsActive) return false;

	dFluidMass = FluidMassSubs.DoubleSI;
	dSolidMass = SolidMassSubs.DoubleSI;

	double dTotalVol = VolSubs.DoubleSI;
	double dFluidDensity = LiquidDensitySubs.DoubleSI;

	double dFluidVol = dFluidDensity > 0 ? dFluidMass / dFluidDensity : 0;
	if (dFluidVol > 0)
	{
		dConc = ConcSubs.IsActive ? ConcSubs.DoubleSI : 0;
		dConc /= dFluidVol;
	}
	else
		dConc = 0;

	double dFreeFluidVol = dFluidDensity > 0 ? MAX(0, (dFluidMass - dSolidMass * dMoistFrac) / dFluidDensity) : 0;
	double dSolidVol = dTotalVol - dFreeFluidVol;

	dFluidLevel = CalcLevel(dTotalVol);
	dSolidLevel = CalcLevel(dSolidVol);
	dFSA = dFreeFluidVol > 0 ? CalcArea(dFluidLevel) : 0;
	return true;
}