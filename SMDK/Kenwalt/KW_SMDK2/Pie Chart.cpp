//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __PIE_CHART_CPP
#include "Pie Chart.h"
#include <math.h>
#pragma optimize("", off)

#pragma warning (disable: 4018 4244)

//====================================================================================

//TODO: A Proper icon
static double Drw_PieChart[] = { MDrw_Poly,  -2.,2.,  2.,2.,  2.,-2., -2.,-2., -2.,2., MDrw_End };

//---------------------------------------------------------------------------

DEFINE_CONTROL_UNIT(PieChart, "Pie Chart", DLL_GroupName)
void PieChart_UnitDef::GetOptions()
{
	SetDefaultTag("PC");
	SetDrawing("Tank", Drw_PieChart);
	SetTreeDescription("Demo:Pie Chart");
	SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
	SetModelGroup(MGroup_General);
};

//---------------------------------------------------------------------------

PieChart::PieChart(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
	bByCompound = false;
	nPhase = MP_All;
	dMaxFlow = 100.0;
	dMinTemp = C_2_K(-20.0);
	dMaxTemp = C_2_K(200.0);
  
  // TagIO.Open must be called here to Initialise the TagIO infrastructure which needs to be 
  // done while syscad is stopped. If the first MTagIOItem or MTagIODirect will only be created 
  // while running then this call is required. If they are created while stopped then their 
  // constructors will do the Initislisation.
   
  TagIO.Open();                                        /*CNM*/
}

//---------------------------------------------------------------------------

void PieChart::Init()
{
}

//---------------------------------------------------------------------------

const int idDX_PipeName = 1;

void PieChart::BuildDataFields()
{
	static MDDValueLst MDDPhases[] = {
		{MP_All, "All"},
		{MP_Sol, "Solid"},
		{MP_Liq, "Liquid"},
		{MP_Gas, "Vapour"},
		{MP_SL,  "Solids & Liquids"},
		{0}};
	DD.String("Pipe", "", idDX_PipeName, MF_PARAMETER);

	DD.CheckBox("Display_Individual_Compounds", "", &bByCompound, MF_PARAMETER);
	DD.Show(bByCompound);
	DD.Long("Phase", "", &nPhase, MF_PARAMETER, MDDPhases);
	DD.Show();
	DD.Text("");
	DD.Double("Max_Flow", "", &dMaxFlow, MF_PARAMETER, MC_Qm);
	DD.Double("Min_Temp", "", &dMinTemp, MF_PARAMETER, MC_T);
	DD.Double("Max_Temp", "", &dMaxTemp, MF_PARAMETER, MC_T);
}

bool PieChart::ExchangeDataFields()
{
	if (DX.Handle == idDX_PipeName)
	{
		if (DX.HasReqdValue)
			sPipeName = DX.String;
		DX.String = sPipeName;
		return true;
	}
	return false;
}

bool PieChart::ValidateDataFields()
{
	if (dMaxFlow < 0) dMaxFlow = 0;
	if (dMinTemp < 0) dMinTemp = 0;
	if (dMaxTemp < dMinTemp) dMaxTemp = 0;
	return true;
}

//---------------------------------------------------------------------------

bool PieChart::GetModelGraphic(CMdlGraphicArray & Grfs)
{
	CMdlGraphic G0(0, MGT_Simple, true, "Graphic", 1);

	Grfs.SetSize(0);
	Grfs.SetAtGrow(0, G0);
	return true;
}

bool PieChart::OperateModelGraphic(CMdlGraphicWnd &Wnd, CMdlGraphic &Grf)
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
		Wnd.m_pWnd->SetWindowPos(NULL, 0, 0, 250, 250, SWP_NOMOVE | SWP_NOZORDER);
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
		int nTextSize = Wnd.m_TextSize.y;
		int nBorderSpace = nTextSize;
		int nFlowIndicatorWidth = nTextSize;

		CRect activeRect = Wnd.m_ClientRect;
		activeRect.bottom -= nBorderSpace;
		activeRect.left += 2 * nBorderSpace + nFlowIndicatorWidth;
		activeRect.right -= nBorderSpace;
		activeRect.top += nBorderSpace;

		MTagIODirect test(TagIO);
		test.Tag = sPipeName + ".Qm";
		if (test.CheckTag() != MTagIO_OK) 
			Wnd.m_pPaintDC->TextOut(Wnd.m_ClientRect.CenterPoint().x, Wnd.m_ClientRect.CenterPoint().y, "Invalid Pipe Name");

		int radius = activeRect.Width() < activeRect.Height() ? activeRect.Width() /2 : activeRect.Height() / 2;
		int txtRadius = (int)(.66 * radius);
		const COLORREF ChartColours[] = {
			Colours::LightBlue, Colours::Magenta, Colours::Beige, Colours::LightGray, Colours::Violet,
			Colours::Pink, Colours::Blue, Colours::Gray, Colours::DarkBlue, Colours::LightPink,
			Colours::Yellow, Colours::Teal, Colours::Purple, Colours::DarkRed };
	
		vector<double> vFractions;
		vector<CString> vNames;

		double dTotFrac = 0;
		if (bByCompound)
		{
			bool bShowSolids = nPhase == MP_All || nPhase == MP_Sol || nPhase == MP_SL;
			bool bShowLiquids = nPhase == MP_All || nPhase == MP_Liq || nPhase == MP_SL;
			bool bShowVapours = nPhase == MP_All || nPhase == MP_Gas;
			for (int i = 0; i < gs_MVDefn.Count(); i++)
			{
				CString compTag = (sPipeName + ".Qi.") + gs_MVDefn[i].Symbol();         /*CNM*/
				MTagIODirect compItem(TagIO, compTag);                                  /*CNM*/
				if (compItem.ReadValue(false)==MTagIO_OK)                               /*CNM*/
				{                                                                       /*CNM*/
					double dFrac = compItem.DoubleSI;
					if (((bShowLiquids && gs_MVDefn[i].IsLiquid())
						|| (bShowSolids && gs_MVDefn[i].IsSolid())
						|| (bShowVapours && gs_MVDefn[i].IsGas()))
						&& dFrac > ZeroLimit)
					{
						vNames.push_back(gs_MVDefn[i].Symbol());
						vFractions.push_back(dFrac);
						dTotFrac += dFrac;
					}
				}                                                                       /*CNM*/
				else                                                                    /*CNM*/
				{                                                                       /*CNM*/
					Log.Message(MMsg_Error, "Tag %s not Read '%s'", compTag, MTagIOItem::ResultString(compItem.Result));   /*CNM*/
				}                                                                       /*CNM*/
			}
		}
		else
		{
			CString solidTag = sPipeName + ".Qi.Solids";
			CString liquidTag = sPipeName + ".Qi.Liquids";
			CString gasTag = sPipeName + ".Qi.Vapours";
			MTagIODirect solidItem(TagIO); solidItem.Tag = solidTag;
			MTagIODirect liquidItem(TagIO); liquidItem.Tag = liquidTag;
			MTagIODirect gasItem(TagIO, gasTag);                                     /*CNM*/
			if (gasItem.ReadValue(true)==MTagIO_OK)                                  /*CNM*/
			{                                                                        /*CNM*/
				double dSolidFrac = solidItem.DoubleSI;
				if (dSolidFrac > ZeroLimit)
				{
					vFractions.push_back(dSolidFrac);
					vNames.push_back("Solids");
				}
			}                                                                        /*CNM*/
			double dLiquidFrac = liquidItem.DoubleSI;                                /*CNM???*/
			if (dLiquidFrac > ZeroLimit)                                             /*CNM???*/
			{                                                                        /*CNM???*/
				vFractions.push_back(dLiquidFrac);                                     /*CNM???*/
				vNames.push_back("Liquids");                                           /*CNM???*/
			}                                                                        /*CNM???*/
			double dVapourFrac = gasItem.DoubleSI;                                   /*CNM???*/
			if (dVapourFrac > ZeroLimit)                                             /*CNM???*/
			{                                                                        /*CNM???*/
				vFractions.push_back(dVapourFrac);                                     /*CNM???*/
				vNames.push_back("Vapours");                                           /*CNM???*/
			}                                                                        /*CNM???*/
			dTotFrac = 1;
		}

		for (unsigned int i = 0; i < vFractions.size(); i++)
			vFractions.at(i) /= dTotFrac;

		CPen BlackPen(PS_SOLID, 0, Colours::Black);
		CBrush TempBrush(Colours::White);
		CPen* oldPen = Wnd.m_pPaintDC->SelectObject(&BlackPen);
		CBrush* oldBrush = Wnd.m_pPaintDC->SelectObject(&TempBrush);

		double curFrac = 0;
		for (unsigned int i = 0; i < vFractions.size(); i++)
		{
			POINT ptStart = {	activeRect.CenterPoint().x + (int) (radius * cos((curFrac + vFractions.at(i)) * 2 * PI)) ,
								activeRect.CenterPoint().y - (int) (radius * sin((curFrac + vFractions.at(i)) * 2 * PI)) };
			POINT ptEnd = {		activeRect.CenterPoint().x + (int) (radius * cos(curFrac * 2 * PI)) ,
								activeRect.CenterPoint().y - (int) (radius * sin(curFrac * 2 * PI)) };
			CBrush pieBrush(ChartColours[i % (sizeof(ChartColours) / sizeof(COLORREF))]);
			Wnd.m_pPaintDC->SelectObject(pieBrush);
			Wnd.m_pPaintDC->Pie(activeRect, ptStart, ptEnd);

			curFrac += vFractions.at(i);
		}
		
		curFrac = 0;
		for (unsigned int i = 0; i < vNames.size(); i++)
		{
			POINT c = {		activeRect.CenterPoint().x + (int)(txtRadius * sin((curFrac + vFractions.at(i) / 2) * 2 * PI)) ,
							activeRect.CenterPoint().y - (int)(txtRadius * cos((curFrac + vFractions.at(i) / 2) * 2 * PI)) };
			CSize txtSize = Wnd.m_pPaintDC->GetTextExtent(vNames.at(i));
			txtSize.cy *= 2;
			RECT txtBackground = {
				c.x - txtSize.cx / 2, c.y - txtSize.cy / 2,
				c.x + txtSize.cx / 2, c.y + txtSize.cy / 2};
			Wnd.m_pPaintDC->SetTextAlign(TA_BOTTOM | TA_CENTER);
			Wnd.m_pPaintDC->FillSolidRect(&txtBackground, Colours::Black);
			Wnd.m_pPaintDC->TextOut(c.x, c.y, vNames.at(i));
			CString PercentString;
			PercentString.Format("%i%%", (int)(vFractions.at(i) * 100.0));
			Wnd.m_pPaintDC->TextOut(c.x, c.y + nTextSize, PercentString);
		}

		//Flow Indicator:
		CPen penGreen(PS_SOLID, 0, White);
		Wnd.m_pPaintDC->SelectObject(&penGreen);
		CString TempTag = sPipeName + ".T";
		MTagIODirect TempItem(TagIO);                                                                  /*CNM???*/
		TempItem.Tag = TempTag;                                                                        /*CNM???*/
		double TempFrac = Range(0.0, (TempItem.DoubleSI - dMinTemp) / GTZ(dMaxTemp - dMinTemp), 1.0);  /*CNM???*/
		int coldR = 0, coldG = 0, coldB = 255;
		int hotR = 255, hotG = 0, hotB = 0;
		COLORREF fluidColour = RGB(
			coldR * (1 - TempFrac) + hotR * TempFrac,
			coldG * (1 - TempFrac) + hotG * TempFrac,
			coldB * (1 - TempFrac) + hotB * TempFrac);
		MTagIODirect FlowItem(TagIO);                                                                  /*CNM???*/
		FlowItem.Tag = sPipeName + ".Qm";                                                              /*CNM???*/
		double FlowFrac = Range(0.0, FlowItem.DoubleSI / GTZ(dMaxFlow), 1.0);                          /*CNM???*/
		CRect rectOutline;
		rectOutline.bottom = Wnd.m_ClientRect.bottom - nBorderSpace;
		rectOutline.top = Wnd.m_ClientRect.top + nBorderSpace;
		rectOutline.left = Wnd.m_ClientRect.left + nBorderSpace;
		rectOutline.right = rectOutline.left + nFlowIndicatorWidth;

		CRect rectToFill = rectOutline;
		rectToFill.top = rectOutline.bottom - rectOutline.Height() * FlowFrac;

		POINT outlinePoly[] = { {rectOutline.left, rectOutline.bottom},
			{rectOutline.right, rectOutline.bottom},
			{rectOutline.right, rectOutline.top},
			{rectOutline.left, rectOutline.top},
			{rectOutline.left, rectOutline.bottom} };
		Wnd.m_pPaintDC->FillSolidRect(&rectToFill, fluidColour);
		Wnd.m_pPaintDC->Polyline(outlinePoly, sizeof(outlinePoly) / sizeof (POINT));
	}
	return true;
}

//---------------------------------------------------------------------------
