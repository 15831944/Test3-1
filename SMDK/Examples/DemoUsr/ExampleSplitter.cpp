//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __EXAMPLESPLITTER_CPP
#include "examplesplitter.h"
#pragma optimize("", off)

//====================================================================================

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",      0,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Product1",       "Product1",  1,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { "Product2",       "Product2",  2,   1,   4,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_Splitter[] = { MDrw_Poly,  -2.,2.,  2.,2.,  2.,-2., -2.,-2., -2.,2.,
MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(Splitter, "Splitter", DLL_GroupName)
void Splitter_UnitDef::GetOptions()
  {
  SetDefaultTag("SP");
  SetDrawing("Tank", Drw_Splitter);
  SetTreeDescription("Demo:Splitter");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
  SetModelGroup(MGroup_General);
  };

//---------------------------------------------------------------------------

Splitter::Splitter(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  //default values...
  bDoPhaseSplit = false;
  dRqdFracSplit = 0.5;
  dRqdSolSplit = 0.0;
  dRqdLiqSplit = 0.0;
  dRqdVapSplit = 1.0;

  dFeedQm = 0.0;
  dProdQm0 = 0.0;
  dProdQm1 = 0.0;
  }

//---------------------------------------------------------------------------

void Splitter::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void Splitter::BuildDataFields()
  {
  DD.Text("Requirements...");
  DD.CheckBox("SplitByPhase", "",  &bDoPhaseSplit, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Double("Split", "",       &dRqdFracSplit, MF_PARAMETER|(bDoPhaseSplit ? MF_NO_VIEW : 0), MC_Frac("%"));
  DD.Double("SolidSplit", "",  &dRqdSolSplit,  MF_PARAMETER|(bDoPhaseSplit ? 0 : MF_NO_VIEW), MC_Frac("%"));
  DD.Double("LiquidSplit", "", &dRqdLiqSplit,  MF_PARAMETER|(bDoPhaseSplit ? 0 : MF_NO_VIEW), MC_Frac("%"));
  DD.Double("VapourSplit", "", &dRqdVapSplit,  MF_PARAMETER|(bDoPhaseSplit ? 0 : MF_NO_VIEW), MC_Frac("%"));
  DD.Text("");
  DD.Text("Results...");
  DD.Double("FeedQm", "",  &dFeedQm,     MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("ProdQm0", "", &dProdQm0,    MF_RESULT|MF_NO_FILING, MC_Qm);
  DD.Double("ProdQm1", "", &dProdQm1,    MF_RESULT|MF_NO_FILING, MC_Qm);

  /* The 4 lines for "Split" to "VapourSplit" could be coded as follows:
  unsigned long FieldFlags;
  if (bDoPhaseSplit)
  FieldFlags = MF_PARAMETER|MF_NO_VIEW;
  else
  FieldFlags = MF_PARAMETER;
  DD.Double("Split", "",   &dRqdFracSplit, MC_Frac, FieldFlags);
  if (bDoPhaseSplit)
  FieldFlags = MF_PARAMETER;
  else
  FieldFlags = MF_PARAMETER|MF_NO_VIEW;
  DD.Double("SolidSplit", "",  &dRqdSolSplit, MC_Frac, FieldFlags);
  DD.Double("LiquidSplit", "", &dRqdLiqSplit, MC_Frac, FieldFlags);
  DD.Double("VapourSplit", "", &dRqdVapSplit, MC_Frac, FieldFlags);
  */
  }

//---------------------------------------------------------------------------

void Splitter::EvalProducts()
  {
  try
    {
    if (1)
      {
      // Examples of Environment Variables
      double X1=StdP;
      double X2=StdT;
      double X3=AtmosPress();
      double X4=AmbientTemp();
      double X5=BaseElevation();
      double X6=WindSpeed();
      double X7=WindDirection();
      }

    //get handles to input and output streams...
    MStream QI;
    FlwIOs.AddMixtureIn_Id(QI, 0);
    MStream & QO0 = FlwIOs[FlwIOs.First[1]].Stream;
    MStream & QO1 = FlwIOs[FlwIOs.First[2]].Stream;

    //make outlet temperature and pressure same as input
    QO0.SetTP(QI.T, QI.P);
    QO1.SetTP(QI.T, QI.P);

    //force input values to be in meaningful ranges...
    dRqdFracSplit = Range(0.0, dRqdFracSplit, 1.0);
    dRqdSolSplit = Range(0.0, dRqdSolSplit, 1.0);
    dRqdLiqSplit = Range(0.0, dRqdLiqSplit, 1.0);
    dRqdVapSplit = Range(0.0, dRqdVapSplit, 1.0);

    //do the work...
    const int NumSpecies = gs_MVDefn.Count();
    for (int i=0; i<NumSpecies; i++)
      {
      if (bDoPhaseSplit)
        {
        if (gs_MVDefn[i].Phase() & MP_Sol)
          QO0.M[i] = QI.M[i] * dRqdSolSplit;
        else if (gs_MVDefn[i].Phase() & MP_Liq)
          QO0.M[i] = QI.M[i] * dRqdLiqSplit;
        else
          QO0.M[i] = QI.M[i] * dRqdVapSplit;
        }
      else
        {
        QO0.M[i] = QI.M[i] * dRqdFracSplit;
        }
      QO1.M[i] = QI.M[i] - QO0.M[i]; //remainder goes to second outlet
      }

    //get display values...
    dFeedQm = QI.MassFlow();
    dProdQm0 = QO0.MassFlow();
    dProdQm1 = QO1.MassFlow();
    }
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
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

//====================================================================================
// Actions
bool Splitter::GetModelAction(CMdlActionArray & Acts)
  {
  int IOCnt = FlwIOs.getCount();
  if (IOCnt==3)
    {
    Acts.SetSize(0);
    Acts.Add(CMdlAction(0, MAT_State, true, "Mass Split", 0));
    Acts.Add(CMdlAction(1, MAT_State, true, "Phase Split", 1));
    if (!bDoPhaseSplit)
      {
      double M1=FlwIOs[1].Stream.MassFlow();
      double M2=FlwIOs[2].Stream.MassFlow();
      double Split=(M1)/GTZ(M1+M2);

      Acts.Add(CMdlAction(2, MAT_Value, !bDoPhaseSplit,  "Split (%)", true, dRqdFracSplit*100, 0.0, 100.0, Split*100));
      }
    return true;
    }
  return false;
  };

bool Splitter::SetModelAction(CMdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
    case 1:
      bDoPhaseSplit=Act.iValue!=0;
      break;
    case 2:
      dRqdFracSplit=Act.dValue*0.01;
      break;
    }
  return true;
  };

//====================================================================================
// Graphics

bool Splitter::GetModelGraphic(CMdlGraphicArray & Grfs)
  {
  CMdlGraphic G0(0, MGT_Simple, true, "Split Graphic", 1);

  Grfs.SetSize(0);
  Grfs.SetAtGrow(0, G0);
  return true;
  };

bool Splitter::OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf)
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
      break;
    case MGT_Size:
      break;
    case MGT_Move:
      break;
    case MGT_EraseBkgnd:
      // Use Wnd.m_pDC;
      Wnd.m_bReturn = 0;// 1 if erased else 0
      break; 
    case MGT_Paint:
      {
      double Total=0;
      int IOCnt = FlwIOs.getCount();
      if (IOCnt==3)
        {
        double M1=FlwIOs[1].Stream.MassFlow();
        double M2=FlwIOs[2].Stream.MassFlow();
        double Split=(M1)/GTZ(M1+M2);

        int charwide = Wnd.m_TextSize.x;

        int y0 = Wnd.m_ClientRect.top;
        int y1 = Wnd.m_TextSize.y+1;
        int y2 = Wnd.m_ClientRect.bottom;
        int x0 = Wnd.m_ClientRect.left;
        int xSplit = (int)(Split*Wnd.m_ClientRect.right);
        int x2 = Wnd.m_ClientRect.right;
        int y3 = 0;
        if (!bDoPhaseSplit)
          {
          y3=y1;
          y1+=Wnd.m_TextSize.y;
          }

        Wnd.m_pPaintDC->FillSolidRect(CRect(x0,y1,xSplit,y2), Blue);
        Wnd.m_pPaintDC->FillSolidRect(CRect(xSplit,y1,x2,y2), Cyan);

        Wnd.m_pPaintDC->FillSolidRect(CRect(x0,y0,x2,y1), Black);
        Wnd.m_pPaintDC->SetTextColor(Green);
        CString S;
        if (!bDoPhaseSplit && fabs(Split-dRqdFracSplit)>0.001)
          S.Format("%.1f %% (Rqd:%.1f %%)", Split*100.0, dRqdFracSplit*100.0);
        else
          S.Format("%.1f %%", Split*100.0);
        Wnd.m_pPaintDC->TextOut(x0,y0,S);

        if (!bDoPhaseSplit)
          {
          CPen penWhite(PS_SOLID, 0, White);
          CPen * oldPen=Wnd.m_pPaintDC->SelectObject(&penWhite);
          CBrush brushRed(Red);
          CBrush * oldBrush=Wnd.m_pPaintDC->SelectObject(&brushRed);

          int xSplitRqd = (int)(dRqdFracSplit*Wnd.m_ClientRect.right);

          POINT Arrow[] = 
            {
              {xSplitRqd,            y1},
              {xSplitRqd-charwide/2, y3},
              {xSplitRqd+charwide/2, y3},
              {xSplitRqd,            y1},
            };
          Wnd.m_pPaintDC->Polygon(Arrow, sizeof(Arrow)/sizeof(Arrow[0]));

          Wnd.m_pPaintDC->SelectObject(oldPen);
          Wnd.m_pPaintDC->SelectObject(oldBrush);
          }
        }
      else
        {
        int y0 = Wnd.m_ClientRect.top;
        int y2 = Wnd.m_ClientRect.bottom;
        int x0 = Wnd.m_ClientRect.left;
        int x2 = Wnd.m_ClientRect.right;
        Wnd.m_pPaintDC->FillSolidRect(CRect(x0,y0,x2,y2), Black);
        Wnd.m_pPaintDC->SetTextColor(Green);
        CString S;
        S.Format("Not Connected");
        Wnd.m_pPaintDC->TextOut(x0,y0,S);
        }

      break;
      }
    case MGT_MouseMove:
      m_LBtnDn=(Wnd.m_MouseFlags & MK_LBUTTON);
      if (m_LBtnDn && !bDoPhaseSplit)
        {
        m_MousePt=Wnd.m_MousePt;
        dRqdFracSplit = (double)m_MousePt.x/Wnd.m_ClientRect.right;
        Wnd.m_pWnd->RedrawWindow(0, 0, RDW_INVALIDATE|RDW_UPDATENOW);
        }
      break;
    case MGT_LButtonDown:
      m_LBtnDn=true;
      if (!bDoPhaseSplit)
        {
        m_MousePt=Wnd.m_MousePt;
        dRqdFracSplit = (double)m_MousePt.x/Wnd.m_ClientRect.right;
        Wnd.m_pWnd->RedrawWindow(0, 0, RDW_INVALIDATE|RDW_UPDATENOW);
        }
      break;
    case MGT_LButtonUp:
      m_LBtnDn=false;
      break;
    case MGT_RButtonDown:
      m_RBtnDn=true;
      break;
    case MGT_RButtonUp:
      m_RBtnDn=false;
      break;
    }
  return true; 
  };

//====================================================================================


