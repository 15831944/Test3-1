//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __EFFICIENCYCURVE_CPP
#include "efficiencycurve.h"
#include "limnstream.h"
//#pragma optimize("", off)

#define DoDbg 0 /*x7*/

//====================================================================================

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",   0,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Oversize",       "Overs",  1,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { "Undersize",      "Unders", 2,   1,   4,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_EfficiencyCurve[] = { MDrw_Poly,  -12,6,  12,2,  12,-2, 0,-6, -12,2, -12,6,
                                        MDrw_Poly,  -12,4,  12,0,
                                        MDrw_Poly,  -12,2,  12,-2,
                                        MDrw_End };

//CLimn_ModelData_Common C_ModelParameters_DiamondWizard_EfficiencyCurve::sm_Common;

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(CEfficiencyCurve, "EfficiencyCurve", DLL_GroupName)
void CEfficiencyCurve_UnitDef::GetOptions()
  {
  SetDefaultTag("EC");
  SetDrawing("EfficiencyCurve", Drw_EfficiencyCurve);
  SetTreeDescription("LimnDW:EfficiencyCurve");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
  SetModelGroup(MGroup_General);
  };

//---------------------------------------------------------------------------

CEfficiencyCurve::CEfficiencyCurve(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  gs_DWCfg.Initialise();
  m_DWParms.Initialise(gs_DWCfg.nSGs());

  m_LBtnDn = false;
  m_RBtnDn = false;
  }

//---------------------------------------------------------------------------

void CEfficiencyCurve::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void CEfficiencyCurve::BuildDataFields()
  {
  m_DWParms.BuildDataFields(DD);
  }
bool CEfficiencyCurve::ExchangeDataFields()
  {
  if (m_DWParms.ExchangeDataFields(DX))
    return true;

  return false;
  }

//---------------------------------------------------------------------------

void CEfficiencyCurve::EvalProducts()
  {
  try
    {
    //get handles to input and output streams...
    MStreamI QI;
    FlwIOs.AddMixtureIn_Id(QI, 0);
    MStream & Q0 = FlwIOs[FlwIOs.First[1]].Stream;
    MStream & Q1 = FlwIOs[FlwIOs.First[2]].Stream;

    //make outlet temperature and pressure same as input
    Q0.SetTP(QI.T, QI.P);
    Q1.SetTP(QI.T, QI.P);

    if (QI.IFExists<CLimnStream>())
      {
      CLimnStream & LSIn =  *QI.GetIF<CLimnStream>();
      CLimnStream & LSO0 =  *Q0.CreateIF<CLimnStream>();
      CLimnStream & LSO1 =  *Q1.CreateIF<CLimnStream>();

      if (DoDbg)
        LSIn.Dump("In", DoDbg);

      LSIn.ConvertToMassForm(QI);
      LSO0.ConvertToMassForm(Q0);
      LSO1.ConvertToMassForm(Q1);

      if (DoDbg)
        LSIn.Dump("In", DoDbg);

      m_DWParms.OversizeStreamID()=1;
      int Ret = _Model_DiamondWizard_EfficiencyCurve (gs_DWCfg.RowCount(),
					 		            			                      gs_DWCfg.ColCount(),
                                                      m_DWParms.DataCount(),    // int nParameters,
                                                      0,                        // int nReturns,
                                                      CLimn_ModelData_Access(m_DWParms),
                                                      NULL,                     // double* ModelReturn,
                                                      LSIn.Data(),              // double* CombinedFeed,
                                                      LSO0.Data(),              // double* Product1,
                                                      LSO1.Data(),              // double* Product2,
                                                      NULL,                     // double* Product3,
                                                      NULL,                     // double* Product4,
                                                      NULL,                     // double* Product5,
                                                      NULL                      // double* Product6 
                                                      );
      
      if (DoDbg)
        {
        LSO0.Dump("Product0", DoDbg);
        LSO1.Dump("Product1", DoDbg);
        }

      LSIn.ConvertToFracForm(QI);
      LSO0.ConvertToFracForm(Q0);
      LSO1.ConvertToFracForm(Q1);

      if (DoDbg)
        {
        LSO0.Dump("Product0", DoDbg);
        LSO1.Dump("Product1", DoDbg);
        }
      }
    else
      {
      Q0 = QI;
      Q1 *= 0.0;
      }

    //get display values...
    //dFeedQm = QI.MassFlow();
    //dProdQm0 = QO0.MassFlow();
    //dProdQm1 = QO1.MassFlow();
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
bool CEfficiencyCurve::GetModelAction(CMdlActionArray & Acts)
  {
  int IOCnt = FlwIOs.getCount();
  if (IOCnt==3)
    {
    Acts.SetSize(0);
    //Acts.Add(CMdlAction(0, MAT_State, true, "Mass Split", 0));
    //Acts.Add(CMdlAction(1, MAT_State, true, "Phase Split", 1));
    //if (!bDoPhaseSplit)
    //  {
    //  double M1=FlwIOs[1].Stream.MassFlow();
    //  double M2=FlwIOs[2].Stream.MassFlow();
    //  double Split=(M1)/GTZ(M1+M2);

    //  Acts.Add(CMdlAction(2, MAT_Value, !bDoPhaseSplit,  "Split (%)", true, dRqdFracSplit*100, 0.0, 100.0, Split*100));
    //  }
    return true;
    }
  return false;
  };

bool CEfficiencyCurve::SetModelAction(CMdlAction & Act)
  {
  switch (Act.iIndex)
    {
    case 0:
    case 1:
      //bDoPhaseSplit=Act.iValue!=0;
      break;
    case 2:
      //dRqdFracSplit=Act.dValue*0.01;
      break;
    }
  return true;
  };

//====================================================================================
// Graphics

bool CEfficiencyCurve::GetModelGraphic(CMdlGraphicArray & Grfs)
  {
  CMdlGraphic G0(0, MGT_Simple, true, "Split Graphic", 1);

  Grfs.SetSize(0);
  Grfs.SetAtGrow(0, G0);
  return true;
  };

bool CEfficiencyCurve::OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf)
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
        if (1)//!bDoPhaseSplit)
          {
          y3=y1;
          y1+=Wnd.m_TextSize.y;
          }

        Wnd.m_pPaintDC->FillSolidRect(CRect(x0,y1,xSplit,y2), Blue);
        Wnd.m_pPaintDC->FillSolidRect(CRect(xSplit,y1,x2,y2), Cyan);

        Wnd.m_pPaintDC->FillSolidRect(CRect(x0,y0,x2,y1), Black);
        Wnd.m_pPaintDC->SetTextColor(Green);
        CString S;
        //if (/*!bDoPhaseSplit &&*/ fabs(Split-dRqdFracSplit)>0.001)
        //  S.Format("%.1f %% (Rqd:%.1f %%)", Split*100.0, dRqdFracSplit*100.0);
        //else
          S.Format("%.1f %%", Split*100.0);
        Wnd.m_pPaintDC->TextOut(x0,y0,S);

        //if (!bDoPhaseSplit)
        //  {
        //  CPen penWhite(PS_SOLID, 0, White);
        //  CPen * oldPen=Wnd.m_pPaintDC->SelectObject(&penWhite);
        //  CBrush brushRed(Red);
        //  CBrush * oldBrush=Wnd.m_pPaintDC->SelectObject(&brushRed);

        //  int xSplitRqd = (int)(dRqdFracSplit*Wnd.m_ClientRect.right);

        //  POINT Arrow[] =
        //    {
        //      {xSplitRqd,            y1},
        //      {xSplitRqd-charwide/2, y3},
        //      {xSplitRqd+charwide/2, y3},
        //      {xSplitRqd,            y1},
        //    };
        //  Wnd.m_pPaintDC->Polygon(Arrow, sizeof(Arrow)/sizeof(Arrow[0]));

        //  Wnd.m_pPaintDC->SelectObject(oldPen);
        //  Wnd.m_pPaintDC->SelectObject(oldBrush);
        //  }
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
      //if (m_LBtnDn && !bDoPhaseSplit)
      //  {
      //  m_MousePt=Wnd.m_MousePt;
      //  dRqdFracSplit = (double)m_MousePt.x/Wnd.m_ClientRect.right;
      //  Wnd.m_pWnd->RedrawWindow(0, 0, RDW_INVALIDATE|RDW_UPDATENOW);
      //  }
      break;
    case MGT_LButtonDown:
      m_LBtnDn=true;
      //if (!bDoPhaseSplit)
      //  {
      //  m_MousePt=Wnd.m_MousePt;
      //  dRqdFracSplit = (double)m_MousePt.x/Wnd.m_ClientRect.right;
      //  Wnd.m_pWnd->RedrawWindow(0, 0, RDW_INVALIDATE|RDW_UPDATENOW);
      //  }
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


