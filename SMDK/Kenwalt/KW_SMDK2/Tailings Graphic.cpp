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
#define TAG_OK(item) (item.CheckTag() == MTagIO_OK || item.CheckTag() == MTagIO_ReadOnly)

//====================================================================================

//TODO: A Proper icon
static double Drw_TailingsDam[] = { MDrw_Poly,  -2.,2.,  2.,2.,  2.,-2., -2.,-2., -2.,2., MDrw_End };

//---------------------------------------------------------------------------

DEFINE_CONTROL_UNIT(TailingsGraphic, "Tailings Damn Graphic", DLL_GroupName)
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
	FluidMassItem(TagIO), SolidMassItem(TagIO),
	ConcItem(TagIO), VolItem(TagIO), LiquidDensityItem(TagIO)
{
	FluidMassItem.Tag = "bleh";
	SolidMassItem.Tag = "bleh";
	ConcItem.Tag = "bleh";
	VolItem.Tag = "bleh";
	LiquidDensityItem.Tag = "bleh";
	dMoistFrac = 0;
	dSatConc = 1;
	dEvapRate = dRainRate = 0;
	nDataPointCount = 2;
	vInterpolationHeights.push_back(0);
	vInterpolationHeights.push_back(1);
	vInterpolationAreas.resize(2, 1);
	vVolumeLookup.resize(2);

	eIntMethod = IM_Linear;

	dFluidLevel = dSolidLevel = dFSA = 0;
	dFluidMass = dSolidMass = 0;

	RecalculateVolumes();
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
const int idDX_InterpolationHeight = MaxIntPoints;
const int idDX_InterpolationArea = 2 * MaxIntPoints;

void TailingsGraphic::BuildDataFields()
{
	// -- User entered data --
	//Area calc: Number of interpolation points, and points
	//solid moisture fraction, saturation concentration
	//Rate of evaporation, rate of rainfall
	//Tank name.
	// -- Data read from tank -- 
	//liquid mass, solid mass, concentrations, densities.
	DD.String("Tank", "", idDX_TankString, MF_PARAMETER);
	DD.Double("Moisture_Fraction", "", &dMoistFrac, MF_PARAMETER, MC_Frac);
	DD.Double("Saturation_Concentration", "", &dSatConc, MF_PARAMETER, MC_Conc);
	DD.Double("Evaporation_Rate", "", &dEvapRate, MF_PARAMETER, MC_Qm);
	DD.Double("Rainfall_Rate", "", &dRainRate, MF_PARAMETER, MC_Qm);
	DD.Double("Max_Capacity", "", &dMaxCapacity, MF_RESULT, MC_Vol);
	DD.Text("");
	DD.Double("Fluid_Level", "", &dFluidLevel, MF_RESULT, MC_L);
	DD.Double("Solid_Level", "", &dSolidLevel, MF_RESULT, MC_L);
	DD.Double("Fluid_Surface_Area", "", &dFSA, MF_RESULT, MC_Area);

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
			//SortInterpolationPoints();
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
			RecalculateVolumes();
			RecalculateLevels();
		}
		DX.Double = vInterpolationAreas.at(DX.Handle - idDX_InterpolationArea);
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
	FluidMassItem.Tag = temp;
	temp.Format("%s%s", sTankName, ".Content.Solids");
	SolidMassItem.Tag = temp;
	temp.Format("%s%s", sTankName, ".Content.Vt");
	VolItem.Tag = temp;
	temp.Format("%s%s", sTankName, ".Content.LRho");
	LiquidDensityItem.Tag = temp;

	RecalculateLevels();
}

bool TailingsGraphic::ValidateDataFields()
{
	return true;
}

void TailingsGraphic::SortInterpolationPoints()
{
	//We need a stable sort of a relatively small dataset, which should not be called too often.
	//So I think a bubble sort is acceptable.
	//Given we should only have one element out of place at any point in time, this should work.
	bool changed = true;
	while (changed)
	{
		changed = false;
		for (int i = 0; i < nDataPointCount - 1; i++)
			if (vInterpolationHeights.at(i) > vInterpolationHeights.at(i + 1))
			{
				changed = true;
				double tH = vInterpolationHeights.at(i);
				vInterpolationHeights.at(i) = vInterpolationHeights.at(i + 1);
				vInterpolationHeights.at(i + 1) = tH;

				double tA = vInterpolationAreas.at(i);
				vInterpolationAreas.at(i) = vInterpolationAreas.at(i + 1);
				vInterpolationAreas.at(i + 1) = tA;
			}
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
		Wnd.m_pWnd->SetWindowPos(NULL, 0, 0, 300, 150, SWP_NOMOVE | SWP_NOZORDER);
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
		double dOverflowFrac = 0.07;
		double dOutYFrac = 0.15;
		double dOutXFrac = 0.2;

		CRect rect = Wnd.m_ClientRect;
		rect.top += nBorderSpace;
		rect.bottom -= nBorderSpace;

		double maxHeight = vInterpolationHeights.back();
		if (maxHeight == 0) return true;

		CRect insideRect;
			insideRect.left = rect.left + rect.Width() * dOutXFrac;
			insideRect.top = rect.top + rect.Height() * dOverflowFrac;
			insideRect.right = rect.right - rect.Width() * dOutXFrac;
			insideRect.bottom = rect.bottom;

		//Draw dam shape:
		int ptCount;
		POINT* insidePts = GetDamPoints(maxHeight, insideRect, ptCount);
		POINT* Dam = new POINT[2 + ptCount];
		Dam[0].x = rect.left; Dam[1 + ptCount].x = rect.right;
		Dam[0].y = Dam[1 + ptCount].y = rect.top + rect.Height() * (dOverflowFrac + dOutYFrac);
		memcpy(Dam + 1, insidePts, ptCount * sizeof(POINT));
		delete[] insidePts; //Do we have to worry about destructors at all?
		
		CPen damPen(PS_SOLID, 3, Grey);
		CPen* oldPen = Wnd.m_pPaintDC->SelectObject(&damPen);
		Wnd.m_pPaintDC->Polyline(Dam, 2 + ptCount);
		delete[] Dam;

		CPen nullPen(PS_NULL, 0, White);
		Wnd.m_pPaintDC->SelectObject(nullPen);

		//Draw fluid:
		POINT* fluid = GetDamPoints(dFluidLevel, insideRect, ptCount);

		CBrush fluidBrush(Blue);
		CBrush* oldBrush = Wnd.m_pPaintDC->SelectObject(&fluidBrush);
		Wnd.m_pPaintDC->Polygon(fluid, ptCount);
		delete[] fluid;
				
		//Draw sediment:
		POINT* sediment = GetDamPoints(dSolidLevel, insideRect, ptCount);
		CBrush sedimentBrush(HS_DIAGCROSS, Brown);
		Wnd.m_pPaintDC->SelectObject(&sedimentBrush);
		Wnd.m_pPaintDC->Polygon(sediment, ptCount);
		delete[] sediment;

		//Draw overflow:
		/*if (bOverflowing)
		{
			Wnd.m_pPaintDC->SelectObject(&fluidBrush);
			POINT overflowLeft[] = {
				{rect.left, rect.top + rect.Height() * (dOverflowFrac + dOutYFrac)},
				{rect.left + rect.Width() * dOutXFrac, rect.top + rect.Height() * dOverflowFrac},
				{rect.left + rect.Width() * dOutXFrac, rect.top},
				{rect.left, rect.top + rect.Height() * dOutYFrac}};
			POINT overflowRight[] = {
				{rect.right, rect.top + rect.Height() * (dOverflowFrac + dOutYFrac)},
				{rect.right - rect.Width() * dOutXFrac, rect.top + rect.Height() * dOverflowFrac},
				{rect.right - rect.Width() * dOutXFrac, rect.top},
				{rect.right, rect.top + rect.Height() * dOutYFrac}};
				Wnd.m_pPaintDC->Polygon(overflowLeft, 4);
				Wnd.m_pPaintDC->Polygon(overflowRight, 4);
		}*/

		Wnd.m_pPaintDC->SelectObject(oldBrush);
		Wnd.m_pPaintDC->SelectObject(oldPen);
	}
	return true;
}

POINT* TailingsGraphic::GetDamPoints(double damHeight, CRect insideRect, int& ptCount)
{
	double maxHeight = vInterpolationHeights.back();
	
	double maxArea = 0;
	for (int i = 0; i < nDataPointCount; i++)
		if (vInterpolationAreas.at(i) > maxArea)
			maxArea = vInterpolationAreas.at(i);
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
	while (damHeight > vInterpolationHeights.at(c))
		c++;
	c++;
	
	POINT* ret = new POINT[2 * c];
	ptCount = 2 * c;

	for (int i = 0; i < c; i++)
	{
		int Afactor = 0.5 * insideRect.Width() * vInterpolationAreas.at(i) / maxArea;
		ret[c - 1 - i].x = insideRect.CenterPoint().x - Afactor;
		ret[c   +   i].x = insideRect.CenterPoint().x + Afactor;
		ret[c - 1 - i].y = ret[c + i].y = insideRect.bottom - insideRect.Height() * vInterpolationHeights.at(i) / maxHeight;
	}

	//Final points are interpolated:
	if (c == 1) //If we have only one point, we have a level of zero and don't need to interpolate.
		return ret;

	ret[0].y = ret[2 * c - 1].y = insideRect.bottom - insideRect.Height() * damHeight / maxHeight;
	double heightDiff = damHeight - vInterpolationHeights.at(c - 2);
	double Agrad;
	if (vInterpolationHeights.at(c - 1) == vInterpolationHeights.at(c - 2))
		Agrad = 0; //The easy way to avoid divide by zero errors.
	else
		Agrad = (vInterpolationAreas.at(c - 1) - vInterpolationAreas.at(c - 2)) / (vInterpolationHeights.at(c - 1) - vInterpolationHeights.at(c - 2));

	double AatH = vInterpolationAreas.at(c - 2) + heightDiff * Agrad;
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
	
	double topHeight = vInterpolationHeights.back();
	double topArea = vInterpolationAreas.back();
	vInterpolationHeights.resize(count, topHeight);
	vInterpolationAreas.resize(count, topArea);
	vVolumeLookup.resize(count);

	nDataPointCount = count;
}

void TailingsGraphic::RecalculateVolumes()
{
	int sH = vInterpolationHeights.size();
	int sA = vInterpolationAreas.size();
	int sV = vVolumeLookup.size();
	double curVol = 0;
	vVolumeLookup.at(0) = 0;
	switch (eIntMethod)
	{
	case IM_Linear:
		for (int i = 0; i < nDataPointCount - 1; i++)
		{
			curVol += 0.5 * (vInterpolationAreas.at(i) + vInterpolationAreas.at(i + 1)) * (vInterpolationHeights.at(i + 1) - vInterpolationHeights.at(i));
			vVolumeLookup.at(i + 1) = curVol;
		}
		break;
	}
	dMaxCapacity = vVolumeLookup.back();
}

double TailingsGraphic::CalcLevel(double volume)
{
	if (volume >= dMaxCapacity)
		return vInterpolationHeights.back();
	if (volume <= 0)
		return 0;
	int i = 0;
	while (vVolumeLookup.at(i) < volume) //This finds the first interpolation point that is above the current volume.
		i++;
	i--;	//But because it's easier to think as extrapolating up, I'm going to decrement it.

	if (vInterpolationHeights.at(i + 1) == vInterpolationHeights.at(i)) //Although this should be impossible.
		return vInterpolationHeights.at(i);

	switch (eIntMethod)
	{
	case IM_Linear:
	default:
		double deltaV = volume - vVolumeLookup.at(i);
		double A0 = vInterpolationAreas.at(i);
		
		double Agrad = (vInterpolationAreas.at(i + 1) - vInterpolationAreas.at(i)) / (vInterpolationHeights.at(i + 1) - vInterpolationHeights.at(i));

		if (Agrad != 0)
			return vInterpolationHeights.at(i) + (-A0 + sqrt(A0 * A0 + 4 * Agrad * deltaV)) / (2 * Agrad);
		else if (A0 != 0)
			return vInterpolationHeights.at(i) + deltaV / A0;
		else
			return vInterpolationHeights.at(i); //Well I go no idea how A0 can be zero. But It's easier to simply make sure we never divide by zero.
	}
}

double TailingsGraphic::CalcArea(double height)
{
	if (height >= vInterpolationHeights.back())
		return vInterpolationAreas.back();
	if (height <= 0)
		return vInterpolationAreas.at(0);
	int i = 0;
	while (vInterpolationHeights.at(i) < height)
		i++;
	i--;

	if (vInterpolationHeights.at(i + 1) == vInterpolationHeights.at(i)) //Although this should be impossible.
		return vInterpolationAreas.at(i);

	switch (eIntMethod)
	{
	case IM_Linear:
	default:
		double deltaH = height - vInterpolationHeights.at(i);
		double Agrad = (vInterpolationAreas.at(i + 1) - vInterpolationAreas.at(i)) / (vInterpolationHeights.at(i + 1) - vInterpolationHeights.at(i));
		return vInterpolationAreas.at(i) + deltaH * Agrad;
	}
}

void TailingsGraphic::RecalculateLevels()
{
	if (!TAG_OK(LiquidDensityItem)) //If we can't get these critical values, no point even trying.
		return;
	if (!TAG_OK(VolItem)) return;
	if (!TAG_OK(FluidMassItem)) return;
	if (!TAG_OK(SolidMassItem)) return;

	dFluidMass = FluidMassItem.DoubleSI;
	dSolidMass = SolidMassItem.DoubleSI;
	//dConc = TAG_OK(ConcItem) ? ConcItem.DoubleSI : 0;

	double dTotalVol = VolItem.DoubleSI;
	double dFluidDensity = LiquidDensityItem.DoubleSI;

	double dFreeFluidVol = MAX(0, (dFluidMass - dSolidMass * dMoistFrac) / MAX(0.1, dFluidDensity));
	double dSolidVol = dTotalVol - dFreeFluidVol;

	dFluidLevel = CalcLevel(dTotalVol);
	dSolidVol = CalcLevel(dSolidVol);
	dFSA = CalcArea(dFluidLevel);
}