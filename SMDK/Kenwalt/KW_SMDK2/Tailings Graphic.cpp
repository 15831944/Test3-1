//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __TAILINGS_GRAPHIC_CPP
#include "Tailings Graphic.h"
#include <math.h>
#pragma optimize("", off)

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

TailingsGraphic::TailingsGraphic(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
	dTopArea = 1;
	dBottomArea = 0.5;
	dMaxCapacity = 1;
	dFluidVolume = dSedimentVolume = 0;
	bOverflowing = false;
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

void TailingsGraphic::BuildDataFields()
{
	DD.Double("Top_Area", "A_top", &dTopArea, MF_PARAMETER, MC_Area(""));
	DD.Double("Bottom_Area", "A_bot", &dBottomArea, MF_PARAMETER, MC_Area(""));
	DD.Double("Maximum_Capacity", "V_max", &dMaxCapacity, MF_PARAMETER, MC_Vol(""));
	DD.Double("Fluid_Volume", "V_fluid", &dFluidVolume, MF_PARAMETER, MC_Vol(""));
	DD.Double("Sediment_Volume", "V_sed", &dSedimentVolume, MF_PARAMETER, MC_Vol(""));
	DD.Bool("Overflowing", "", &bOverflowing, MF_PARAMETER);
}

bool TailingsGraphic::ExchangeDataFields()
{
	return false;
}

bool TailingsGraphic::ValidateDataFields()
{
	return true;
}

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

		if (dTopArea == 0 || dMaxCapacity == 0)
			return true;
		double dBottomFrac = sqrt(dBottomArea / dTopArea) * (1 - 2 * dOutXFrac);
		double dSedimentFrac = dSedimentVolume / dMaxCapacity;

		CRect rect = Wnd.m_ClientRect;
		rect.top += nBorderSpace;
		rect.bottom -= nBorderSpace;
		//Draw dam shape:
		POINT Dam[] = {
			{rect.left, rect.top + rect.Height() * (dOverflowFrac + dOutYFrac)},
			{rect.left + rect.Width() * dOutXFrac, rect.top + rect.Height() * dOverflowFrac},
			{rect.left + rect.Width() * (1 - dBottomFrac) / 2, rect.bottom},
			{rect.right - rect.Width() * (1 - dBottomFrac) / 2, rect.bottom},
			{rect.right - rect.Width() * dOutXFrac, rect.top + rect.Height() * dOverflowFrac},
			{rect.right, rect.top + rect.Height() * (dOverflowFrac + dOutYFrac)}};
		CPen damPen(PS_SOLID, 3, Grey);
		CPen* oldPen = Wnd.m_pPaintDC->SelectObject(&damPen);
		Wnd.m_pPaintDC->Polyline(Dam, sizeof(Dam) / sizeof(POINT));

		CRect insideRect;
			insideRect.left = rect.left + rect.Width() * dOutXFrac;
			insideRect.top = rect.top + rect.Height() * dOverflowFrac;
			insideRect.right = rect.right - rect.Width() * dOutXFrac;
			insideRect.bottom = rect.bottom;
		double invGradient = (double) insideRect.Width() * (1 - sqrt(dBottomArea / dTopArea)) / 2;
		CPen nullPen(PS_NULL, 0, White);
		Wnd.m_pPaintDC->SelectObject(nullPen);

		//Draw fluid:
		double dFluidFrac = bOverflowing ? rect.Height() / insideRect.Height() : dFluidVolume / dMaxCapacity;

		POINT fluid[] = {
			{insideRect.left + invGradient * (1-dFluidFrac), insideRect.bottom - insideRect.Height() * dFluidFrac},
			{insideRect.right - invGradient * (1-dFluidFrac), insideRect.bottom - insideRect.Height() * dFluidFrac},
			{insideRect.right - invGradient, insideRect.bottom},
			{insideRect.left + invGradient, insideRect.bottom}};
		CBrush fluidBrush(Blue);
		CBrush* oldBrush = Wnd.m_pPaintDC->SelectObject(&fluidBrush);
		Wnd.m_pPaintDC->Polygon(fluid, 4);
				
		//Draw sediment:
		POINT sediment[] = {
			{insideRect.left + invGradient * (1-dSedimentFrac), insideRect.bottom - insideRect.Height() * dSedimentFrac},
			{insideRect.right - invGradient * (1-dSedimentFrac), insideRect.bottom - insideRect.Height() * dSedimentFrac},
			{insideRect.right - invGradient, insideRect.bottom},
			{insideRect.left + invGradient, insideRect.bottom}};
		CBrush sedimentBrush(HS_DIAGCROSS, Brown);
		Wnd.m_pPaintDC->SelectObject(&sedimentBrush);
		Wnd.m_pPaintDC->Polygon(sediment, 4);

		//Draw overflow:
		if (bOverflowing)
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
		}

		Wnd.m_pPaintDC->SelectObject(oldBrush);
		Wnd.m_pPaintDC->SelectObject(oldPen);
	}
	return true;
}

