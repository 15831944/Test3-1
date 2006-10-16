//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __WHITENCRUSHER_CPP
#include "whitencrusher.h"
#include "limnstream.h"
#pragma optimize("", off)

#define DoDbg 0x7

//====================================================================================

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;       Id; Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",  0,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Product",        "Prod",  1,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_WhitenCrusher[] = { MDrw_Poly,  -12,7,  -7,0,  -12,-7, -12,7,
                                      MDrw_Poly,  12,7,  7,0,  12,-7, 12,7,
                                      MDrw_Poly,  -7,7, -5,0, -7,-7, 7,-7, 5,0, 7,7, -7,7,
                                      MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(CWhitenCrusher, "WhitenCrusher", DLL_GroupName)
void CWhitenCrusher_UnitDef::GetOptions()
  {
  SetDefaultTag("WC");
  SetDrawing("WhitenCrusher", Drw_WhitenCrusher);
  SetTreeDescription("LimnDW:WhitenCrusher");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
  SetModelGroup(MGroup_General);
  };

//---------------------------------------------------------------------------

CWhitenCrusher::CWhitenCrusher(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  gs_DWCfg.Initialise();
  m_DWParms.Initialise();
  m_DWReturns.Initialise();

  m_LBtnDn = false;
  m_RBtnDn = false;
  }

//---------------------------------------------------------------------------

void CWhitenCrusher::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

const int xidRedDensimetrics  = 1;
const int xidCrusherCSS       = 2;
const int xidK1a0             = 3;
const int xidK1a1             = 4;
const int xidK1a2             = 5;
const int xidK1a3             = 6;
const int xidK2a0             = 7;
const int xidK2a1             = 8;
const int xidK2a2             = 9;
const int xidK2a3             =10;
const int xidK3a0             =11;
const int xidT10a0            =12;
const int xidT10a1            =13;
const int xidT10a2            =14;
const int xidT10a3            =15;
const int xidTnt00            =16;

void CWhitenCrusher::BuildDataFields()
  {
  DD.Text("Parameters...");
  DD.Text("");

  CString Tg;
  
  DD.Bool  ("RedDensimetrics","",  xidRedDensimetrics     ,  MF_PARAMETER);
  DD.Double("CSS",            "",  xidCrusherCSS          ,         MF_PARAMETER, MC_L("mm"));
  DD.Double("K1a0 ",          "",  &m_DWParms.K1_a(0)             ,         MF_PARAMETER, MC_(""));
  DD.Double("K1a1 ",          "",     &m_DWParms.K1_a(1)             ,         MF_PARAMETER, MC_(""));
  DD.Double("K1a2 ",          "",     &m_DWParms.K1_a(2)             ,         MF_PARAMETER, MC_(""));
  DD.Double("K1a3 ",          "",     &m_DWParms.K1_a(3)             ,         MF_PARAMETER, MC_(""));
  DD.Double("K2a0 ",          "",     &m_DWParms.K2_a(0)             ,         MF_PARAMETER, MC_(""));
  DD.Double("K2a1 ",          "",     &m_DWParms.K2_a(1)             ,         MF_PARAMETER, MC_(""));
  DD.Double("K2a2 ",          "",     &m_DWParms.K2_a(2)             ,         MF_PARAMETER, MC_(""));
  DD.Double("K2a3 ",          "",     &m_DWParms.K2_a(3)             ,         MF_PARAMETER, MC_(""));
  DD.Double("K3a0 ",          "",     &m_DWParms.K3_a()              ,         MF_PARAMETER, MC_(""));
  DD.Double("T10a0",          "",     &m_DWParms.T10_a(0)            ,         MF_PARAMETER, MC_(""));
  DD.Double("T10a1",          "",     &m_DWParms.T10_a(1)            ,         MF_PARAMETER, MC_(""));
  DD.Double("T10a2",          "",     &m_DWParms.T10_a(2)            ,         MF_PARAMETER, MC_(""));
  DD.Double("T10a3",          "",     &m_DWParms.T10_a(3)            ,         MF_PARAMETER, MC_(""));
  for (int i=0; i<5; i++)
    {
    for (int j=0; j<5; j++)
      {
      DD.Double("Tnt44",          "",     xidTnt00+j+(i*5)  ,         MF_PARAMETER, MC_(""));
      }
    }

//									0. Flag to indicate whether Redistribution of Densimetrics is required
//									1. Crusher CSS
//									2-5. K1 parameters a0 - a3
//									6-9. K2 parameters a0 - a3
//									10.  K3 parameter  a0
//									11-14. T10 parameters a0 - a3
//									15-39. Array of Tn v T10 (Spreadsheet vector form - i.e. Row Major order

  }
bool CWhitenCrusher::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
  //DD.Text("Parameters...");
  //DD.Text("");

  //CString Tg;
  //
    case xidRedDensimetrics: 
      if (DX.HasReqdValue)
        m_DWParms.redistributeDensimetrics() = DX.Bool?1:0;
      DX.Bool = m_DWParms.redistributeDensimetrics()!=0;
      return true;
    case xidCrusherCSS:
      if (DX.HasReqdValue)
        m_DWParms.CSS() = DX.Double*1000; // to mm
      DX.Double = m_DWParms.CSS()*0.001;
      return true;


      //DD.Double("CSS",          "",  xidCrusherCSS          ,         MF_PARAMETER, MC_L("mm"));
  //DD.Double("K1a0 ",          "",     xidK1a0             ,         MF_PARAMETER, MC_(""));
  //DD.Double("K1a1 ",          "",     xidK1a1             ,         MF_PARAMETER, MC_(""));
  //DD.Double("K1a2 ",          "",     xidK1a2             ,         MF_PARAMETER, MC_(""));
  //DD.Double("K1a3 ",          "",     xidK1a3             ,         MF_PARAMETER, MC_(""));
  //DD.Double("K2a0 ",          "",     xidK2a0             ,         MF_PARAMETER, MC_(""));
  //DD.Double("K2a1 ",          "",     xidK2a1             ,         MF_PARAMETER, MC_(""));
  //DD.Double("K2a2 ",          "",     xidK2a2             ,         MF_PARAMETER, MC_(""));
  //DD.Double("K2a3 ",          "",     xidK2a3             ,         MF_PARAMETER, MC_(""));
  //DD.Double("K3a0 ",          "",     xidK3a0             ,         MF_PARAMETER, MC_(""));
  //DD.Double("T10a0",          "",     xidT10a0            ,         MF_PARAMETER, MC_(""));
  //DD.Double("T10a1",          "",     xidT10a1            ,         MF_PARAMETER, MC_(""));
  //DD.Double("T10a2",          "",     xidT10a2            ,         MF_PARAMETER, MC_(""));
  //DD.Double("T10a3",          "",     xidT10a3            ,         MF_PARAMETER, MC_(""));
  
    default:
      if (DX.Handle>=xidTnt00 && DX.Handle<xidTnt00+25)
        {
        int i=(DX.Handle-xidTnt00)/5;
        int j=(DX.Handle-xidTnt00)%5;
        if (DX.HasReqdValue)
          m_DWParms.Tn_V_T10(i,j) = DX.Double;
        DX.Double = m_DWParms.Tn_V_T10(i,j);
        return true;
        }
    }

//									0. Flag to indicate whether Redistribution of Densimetrics is required
//									1. Crusher CSS
//									2-5. K1 parameters a0 - a3
//									6-9. K2 parameters a0 - a3
//									10.  K3 parameter  a0
//									11-14. T10 parameters a0 - a3
//									15-39. Array of Tn v T10 (Spreadsheet vector form - i.e. Row Major order

  return false;
  }


//---------------------------------------------------------------------------

void CWhitenCrusher::EvalProducts()
  {
  try
    {
    //get handles to input and output streams...
    MStream QI;
    FlwIOs.AddMixtureIn_Id(QI, 0);
    MStream & Q0 = FlwIOs[FlwIOs.First[1]].Stream;

    //make outlet temperature and pressure same as input
    Q0.SetTP(QI.T, QI.P);

    if (QI.IFExists<CLimnStream>())
      {
      CLimnStream & LSIn  =  *QI.GetIF<CLimnStream>();
      CLimnStream & LSOut =  *Q0.CreateIF<CLimnStream>();

      if (DoDbg)
        {
        LSIn.Dump("In", DoDbg);
        Dbg.PrintLn("MassIn:%10.4f", QI.MassFlow());
        }
#if 10
      LSIn.ConvertToMassForm(QI);
      LSOut.ConvertToMassForm(Q0);

      if (DoDbg)
        LSIn.Dump("In", DoDbg);

      //m_DWParms.OversizeStreamID()=1;
      int Ret = _Model_DiamondWizard_WhitenCrusher (gs_DWCfg.RowCount(),
					 		            			                      gs_DWCfg.ColCount(),
                                                      m_DWParms.DataCount(),    // int nParameters,
                                                      m_DWReturns.DataCount(),  // int nReturns,
                                                      m_DWParms.GetDataPtr(),
                                                      m_DWReturns.GetDataPtr(), // double* ModelReturn,
                                                      LSIn.Data(),              // double* CombinedFeed,
                                                      LSOut.Data(),             // double* Product1,
                                                      NULL,                     // double* Product2,
                                                      NULL,                     // double* Product3,
                                                      NULL,                     // double* Product4,
                                                      NULL,                     // double* Product5,
                                                      NULL                      // double* Product6 
                                                      );
      
      if (DoDbg)
        LSOut.Dump("Product", DoDbg);

        Dbg.PrintLn("MassIn  X:%10.4f", LSIn.Vector.Mass());
        Dbg.PrintLn("MassOut X:%10.4f", LSOut.Vector.Mass());
      
      

      LSIn.ConvertToFracForm(QI);
      LSOut.ConvertToFracForm(Q0);
#else
      Q0.Copy(QI);
#endif

      if (DoDbg)
        {
        LSOut.Dump("Product", DoDbg);
        Dbg.PrintLn("MassIn  B:%10.4f", QI.MassFlow());
        Dbg.PrintLn("MassOut B:%10.4f", Q0.MassFlow());
        }
      }

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
bool CWhitenCrusher::GetModelAction(CMdlActionArray & Acts)
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

bool CWhitenCrusher::SetModelAction(CMdlAction & Act)
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

bool CWhitenCrusher::GetModelGraphic(CMdlGraphicArray & Grfs)
  {
  CMdlGraphic G0(0, MGT_Simple, true, "Split Graphic", 1);

  Grfs.SetSize(0);
  Grfs.SetAtGrow(0, G0);
  return true;
  };

bool CWhitenCrusher::OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf)
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


