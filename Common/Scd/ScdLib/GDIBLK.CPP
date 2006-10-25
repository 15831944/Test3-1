//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"

#define __GDIBLK_CPP

#include "sc_defs.h"
#include "gpfuncs.h"
#include "gdiblk.h"
 
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define BoxOverlap    1
#define NewFont       01
//===========================================================================

pGDIBlkCfg GDIBlkCfg::List=NULL;

GDIBlkCfg::GDIBlkCfg(pchar pName_)
  {
  strncpy(Name, pName_, sizeof(Name));
  crText[0]         = RGB(0x00, 0x00, 0x00);
  crText[1]         = RGB(0xff, 0xff, 0xff);
  crTextChg[0]      = RGB(0x00, 0x00, 0xff);
  crTextChg[1]      = RGB(0xff, 0xff, 0xff);
//  crTextChg[1]      = RGB(0x00, 0x00, 0xff);
//  crTextInvalid   = RGB(0x80, 0x80, 0x80);
  crTextInvalid[0]  = RGB(0x60, 0x60, 0x60);
  crTextInvalid[1]  = RGB(0xc0, 0xc0, 0xc0);
//  crTextInvalid[1]  = RGB(0xff, 0xff, 0xff);
  crTextError[0]    = RGB(0xff, 0x00, 0x00);
  crTextError[1]    = RGB(0xff, 0x00, 0x00);
  crTxtBackError    = RGB(0xc0, 0xc0, 0xc0);
  crTxtBackSelect   = RGB(0x00, 0x00, 0x80);
  crTxtFore         = RGB(0xff, 0xff, 0xff);
  crTxtBack         = RGB(0xc0, 0xc0, 0xc0);
  crTxtEdgeLight    = RGB(0xe0, 0xe0, 0xe0);
  crTxtEdgeDark     = RGB(0x60, 0x60, 0x60);
  crTxtBord         = RGB(0x00, 0x00, 0x00);
                    
  crGrfGrid         = RGB(0x80, 0x80, 0x80);
  crGrfBack         = RGB(0x00, 0x00, 0x00);
  crGrfAxes         = RGB(0xc0, 0xc0, 0xc0);

  crTrndGrid        = RGB(0x80, 0x80, 0x80);
  crTrndBack        = RGB(0x00, 0x00, 0x00);
  crTrndAxes        = RGB(0xc0, 0xc0, 0xc0);

  for (int i=0; i<MaxGDILines; i++)
    {
    LineLogBrush[i].lbStyle = PS_SOLID;
    LineLogBrush[i].lbColor = RGB(0xff, 0xff, 0xff); // All The Same For Now
    LineLogBrush[i].lbHatch = 0;
    LineLogBrushDim[i].lbStyle = PS_SOLID; // All The Same For Now
    LineLogBrushDim[i].lbColor = RGB(0x40, 0x40, 0x40);
    LineLogBrushDim[i].lbHatch = 0;
    }
  LineLogBrushDim[1].lbColor = RGB(0x80, 0x80, 0x80);
  LineLogBrushDim[2].lbColor = RGB(0xc0, 0xc0, 0xc0);
  LineLogBrushDim[3].lbColor = RGB(0xc0, 0x00, 0x00);
  LineLogBrushDim[4].lbColor = RGB(0x00, 0xc0, 0x00);
  LineLogBrushDim[5].lbColor = RGB(0x00, 0x00, 0xc0);
  LineLogBrush[3].lbColor = RGB(0xff, 0x00, 0x00);
  LineLogBrush[4].lbColor = RGB(0x00, 0xff, 0x00);
  LineLogBrush[5].lbColor = RGB(0x00, 0x00, 0xff);

#if NewFont
  FontHgt[0]= 9;
  SmlFontHgt[0]= 8;
#else
  FontHgt[0]= 7;
  SmlFontHgt[0]= 7;
#endif
  
  FontHgt[1]= 9;
  SmlFontHgt[1]= 9;
  FontHgt[2]= 11;
  SmlFontHgt[2]= 11;
//#if NewFont  
//  FontHgt[3]= 9;
//  SmlFontHgt[3]= 9;
//#else
//  FontHgt[3]= 7;
//  SmlFontHgt[3]= 7;
//#endif
//  FontHgt[4]= 9;
//  SmlFontHgt[4]= 9;
//  FontHgt[5]= 11;
//  SmlFontHgt[5]= 11;
  for (i=0; i<MaxGDIFonts ; i++)
    {
    LogFont[i].lfHeight         =0; 
    LogFont[i].lfWidth          =0;
    LogFont[i].lfEscapement     =0; 
    LogFont[i].lfOrientation    =0; 
    LogFont[i].lfWeight         =FW_REGULAR;
    LogFont[i].lfItalic         =0;// (i>=MaxGDIFonts/2) ? TRUE : FALSE;
    LogFont[i].lfUnderline      =0;
    LogFont[i].lfStrikeOut      =0;
    LogFont[i].lfCharSet        =1; 
    LogFont[i].lfOutPrecision   = OUT_DEFAULT_PRECIS;
    LogFont[i].lfClipPrecision  = CLIP_DEFAULT_PRECIS;   
    LogFont[i].lfQuality        = DEFAULT_QUALITY; 
    LogFont[i].lfPitchAndFamily = FIXED_PITCH|FF_MODERN;
//    strcpy(LogFont[i].lfFaceName, "Courier");
#if NewFont  
    strcpy(LogFont[i].lfFaceName, "Lucida Console");
//    strcpy(LogFont[i].lfFaceName, "MS Sans Serif");
#else
    strcpy(LogFont[i].lfFaceName, "Courier");
#endif
    }

  ScrollBarSize = 15;
  fGrfRevGrayScaleOnPrint = 1;
  
  Next=List;
  List=this;
  TRACE("Cons:%8x\n", this);
  };

//---------------------------------------------------------------------------

GDIBlkCfg::~GDIBlkCfg() 
  {
  TRACE("Delete GDIBLKCfg:%8x\n", this);
  if (List==this)
    List=Next;
  else 
    {
    pGDIBlkCfg p=List;
    while (p->Next!=this)
      p=p->Next;
    if (p->Next!=NULL)
      p->Next=p->Next->Next;
    }
  //Save();
  };


//---------------------------------------------------------------------------

GDIBlkCfg::GDIBlkCfg(const GDIBlkCfg &S)
  { 
  *this=S;
  Next=List;
  List=this;
  }

//---------------------------------------------------------------------------

GDIBlkCfg& GDIBlkCfg::operator=(const GDIBlkCfg &S)
  {
  strncpy(Name,S.Name,sizeof(Name));
  crText[0]         = S.crText[0];
  crText[1]         = S.crText[1];
  crTextChg[0]      = S.crTextChg[0];
  crTextChg[1]      = S.crTextChg[1];
  crTextInvalid[0]  = S.crTextInvalid[0];
  crTextInvalid[1]  = S.crTextInvalid[1];
  crTextError[0]    = S.crTextError[0];
  crTextError[1]    = S.crTextError[1];
  crTxtBackError    = S.crTxtBackError;
  crTxtBackSelect   = S.crTxtBackSelect;
  crTxtFore         = S.crTxtFore;
  crTxtBack         = S.crTxtBack;
  crTxtEdgeLight    = S.crTxtEdgeLight;
  crTxtEdgeDark     = S.crTxtEdgeDark;
  crTxtBord         = S.crTxtBord;
                    
  crGrfGrid         = S.crGrfGrid;
  crGrfBack         = S.crGrfBack;
  crGrfAxes         = S.crGrfAxes;

  crTrndGrid         = S.crTrndGrid;
  crTrndBack         = S.crTrndBack;
  crTrndAxes         = S.crTrndAxes;

  for (int i=0; i<MaxGDILines; i++)
    {
    LineLogBrush[i]=S.LineLogBrush[i];
    LineLogBrushDim[i]=S.LineLogBrushDim[i];
    }

  for (i=0; i<MaxGDIFonts ; i++)
    {
    FontHgt[i] = S.FontHgt[i];
    SmlFontHgt[i] = S.SmlFontHgt[i];
    LogFont[i] = S.LogFont[i];
    }

  ScrollBarSize = S.ScrollBarSize;
  fGrfRevGrayScaleOnPrint = S.fGrfRevGrayScaleOnPrint;
  return *this;
  }

//---------------------------------------------------------------------------

void GDIBlkCfg::Save()
  {
  };

//---------------------------------------------------------------------------

void GDIBlkCfg::Restore()
  {
  };

//---------------------------------------------------------------------------

void GDIBlkCfg::DoConfigDialog()
  {
  AfxMessageBox("Configuration Dialog to come here !");
  };

//---------------------------------------------------------------------------

void GDIBlkCfg::SaveAll()
  {
  pGDIBlkCfg p=List;
  while (p)
    {
    p->Save();
    p=p->Next;
    }
  };

//---------------------------------------------------------------------------

void GDIBlkCfg::RestoreAll()
  {
  pGDIBlkCfg p=List;
  while (p)
    {
    p->Restore();
    p=p->Next;
    }
  };

//===========================================================================

CRITICAL_SECTION GDIBlk::cbDrawSect;
int GDIBlk::iDrawSectBlkCnt=0;
flag GDIBlk::bIsLowRes=1;

//---------------------------------------------------------------------------

GDIBlk::GDIBlk()
  {
  pGBC=NULL;
  pWnd=NULL;
  iAttachNo=-1;
  //pDC=NULL;
  //pPInfo=NULL;
  bValid=0;
  iAttachCount=0;
  if (iDrawSectBlkCnt==0)
    {
    InitializeCriticalSection(&cbDrawSect);
    dbgpln("InitializeCriticalSection(&cbDrawSect);");
    }
  iDrawSectBlkCnt++;
  };

//---------------------------------------------------------------------------

GDIBlk::~GDIBlk()
  {
  iDrawSectBlkCnt--;
  Clear();
  if (iDrawSectBlkCnt==0)
    {
    DeleteCriticalSection(&cbDrawSect);
    dbgpln("DeleteCriticalSection(&cbDrawSect);");
    }
  }

//---------------------------------------------------------------------------

void GDIBlk::Clear()
  {
  if (bValid)
    {
//    delete         pPenTxt;
    delete         pBrushTxtFore;
    delete         pBrushTxtBack;
    delete         pBrushTxtBackError;
    delete         pBrushTxtBackSelect;
    delete         pBrushTxtEdgeLight;
    delete         pBrushTxtEdgeDark;
    delete         pPenTxtBord;
    delete         pPenTxtBack;

    delete         pPenGrfGrid;
    delete         pPenGrfAxes;
    delete         pBrushGrfBack;

    delete         pPenTrndGrid;
    delete         pPenTrndAxes;
    delete         pBrushTrndBack;

    for (int i=0; i<MaxGDILines; i++)
      {
      DeleteObject(hLinePen[i]);
      DeleteObject(hLinePenDim[i]);
      }

    for (i=0; i<MaxGDIFonts ; i++)
      {
      delete pFont[i];
      delete pSmlFont[i];
      delete pItlFont[i];
      }
    }
  bValid=0;
  };

//---------------------------------------------------------------------------

flag GDIBlk::Create(pGDIBlkCfg pGBC_, /*pFxdEdtView pView_,*/ CWnd* pWnd_, CDC* pDC_, flag fForPrinter)
  {
  EnterCriticalSection(&cbDrawSect);
  Clear();
  pGBC=pGBC_;
  pWnd=pWnd_;
//  pDC=pDC_;

  iAttachNo++;
  Attached[iAttachNo].pDC=pDC_;
  Attached[iAttachNo].pPInfo=NULL;//pPInfo_;
  Attached[iAttachNo].ToPrint=false;//(pPInfo!=NULL);
  Attached[iAttachNo].pOldFont=NULL;//pDC_->SelectObject(pFont[0]);

  CFont * pOldFont=pDC_->GetCurrentFont();
  for (int i=0; i<MaxGDIFonts; i++)
    {
    TEXTMETRIC tm;
    pFont[i]=new CFont();                                                                  
    pSmlFont[i]=new CFont();                                                                  
    pItlFont[i]=new CFont();                                                                  

    pGBC->LogFont[i].lfHeight = (pGBC->FontHgt[i] * pDC_->GetDeviceCaps(LOGPIXELSY)) / 72;
    pFont[i]->CreateFontIndirect(&pGBC->LogFont[i]);

    pGBC->LogFont[i].lfItalic = True;
    pItlFont[i]->CreateFontIndirect(&pGBC->LogFont[i]);
    
    pGBC->LogFont[i].lfItalic = False;
    pGBC->LogFont[i].lfHeight = (pGBC->SmlFontHgt[i] * pDC_->GetDeviceCaps(LOGPIXELSY)) / 72;
    pSmlFont[i]->CreateFontIndirect(&pGBC->LogFont[i]);

    pDC_->SelectObject(pFont[i]);
    pDC_->GetTextMetrics(&tm);
#if NewFont
    CharSize[i].cx=tm.tmAveCharWidth;  
#else
    CharSize[i].cx=tm.tmMaxCharWidth;  
#endif
    CharSize[i].cy=tm.tmHeight;
    CharSpc[i].cx=0;//Max(1, CharSize[i].cx/20);
    CharSpc[i].cy=Max(1L, long(CharSize[i].cy/6));
    LineSize[i].cx=CharSize[i].cx+CharSpc[i].cx;
    LineSize[i].cy=CharSize[i].cy+CharSpc[i].cy;
    }
  bIsLowRes = (LineSize[0].cy<17); //Crude method to test if high res graphics supported
  pDC_->SelectObject(pOldFont);

  if (LineSize[0].cx==0 || LineSize[0].cy==0)
    {
    AfxMessageBox("Aborting: Bad Font Size");
    exit(-1);
    }

  EdgeW=Max(0,1);//CharSpc[0].cx, CharSpc[0].cy);

  
  crText[0]          = /*MakeColor(*/pGBC->crText[0]/*, fForPrinter)*/;  
  crText[1]          = /*MakeColor(*/pGBC->crText[1]/*, fForPrinter)*/;  
  crTextChg[0]       = /*MakeColor(*/pGBC->crTextChg[0]/*, fForPrinter)*/;  
  crTextChg[1]       = /*MakeColor(*/pGBC->crTextChg[1]/*, fForPrinter)*/;  
  crTextInvalid[0]   = /*MakeColor(*/pGBC->crTextInvalid[0]/*, fForPrinter)*/;  
  crTextInvalid[1]   = /*MakeColor(*/pGBC->crTextInvalid[1]/*, fForPrinter)*/;  
  crTextError[0]     = /*MakeColor(*/pGBC->crTextError[0];
  crTextError[1]     = /*MakeColor(*/pGBC->crTextError[1];
  crTxtBackError     = /*MakeColor(*/pGBC->crTxtBackError;
  crTxtBackSelect    = /*MakeColor(*/pGBC->crTxtBackSelect;
  crTxtBack          = /*MakeColor(*/pGBC->crTxtBack/*, fForPrinter)*/;
  crTxtFore          = /*MakeColor(*/pGBC->crTxtFore/*, fForPrinter)*/;          
  crTxtEdgeLight     = /*MakeColor(*/pGBC->crTxtEdgeLight/*, fForPrinter)*/;
  crTxtEdgeDark      = /*MakeColor(*/pGBC->crTxtEdgeDark/*, fForPrinter)*/;
  crTxtBord          = /*MakeColor(*/pGBC->crTxtBord/*, fForPrinter)*/;

  crGrfGrid          = MakeColor(pGBC->crGrfGrid, fForPrinter);
  crGrfBack          = MakeColor(pGBC->crGrfBack, fForPrinter);
  crGrfAxes          = MakeColor(pGBC->crGrfAxes, fForPrinter);

  crTrndGrid          = MakeColor(pGBC->crTrndGrid, fForPrinter);
  crTrndBack          = MakeColor(pGBC->crTrndBack, fForPrinter);
  crTrndAxes          = MakeColor(pGBC->crTrndAxes, fForPrinter);

//  pPenTxt             = new CPen(PS_SOLID, EdgeW,  crText[0]);
  pBrushTxtFore       = new CBrush(crTxtFore); //on Windows 98 this changes the CurrentBrush!!!
  pBrushTxtBack       = new CBrush(crTxtBack);
  pBrushTxtBackError  = new CBrush(/*pDC_->GetNearestColor*/(crTxtBackError));
  pBrushTxtBackSelect = new CBrush(/*pDC_->GetNearestColor*/(crTxtBackSelect));
  pBrushTxtEdgeLight  = new CBrush(pDC_->GetNearestColor(crTxtEdgeLight));
  pBrushTxtEdgeDark   = new CBrush(pDC_->GetNearestColor(crTxtEdgeDark));
  pPenTxtBord         = new CPen(PS_SOLID, EdgeW,  crTxtBord);
  pPenTxtBack         = new CPen(PS_SOLID, EdgeW,  crTxtBack);

  pPenGrfGrid         = new CPen(PS_SOLID, 0,  crGrfGrid);
  pPenGrfAxes         = new CPen(PS_SOLID, 0,  crGrfAxes);
  pBrushGrfBack       = new CBrush(pDC_->GetNearestColor(crGrfBack));

  pPenTrndGrid        = new CPen(PS_SOLID, 0,  crTrndGrid);
  pPenTrndAxes        = new CPen(PS_SOLID, 0,  crTrndAxes);
  pBrushTrndBack      = new CBrush(pDC_->GetNearestColor(crTrndBack));

  for (i=0; i<MaxGDILines; i++)
    {
    LineLogBrush[i] = pGBC->LineLogBrush[i];
    LineLogBrush[i].lbColor = MakeColor(LineLogBrush[i].lbColor, fForPrinter);
    hLinePen[i] = ::ExtCreatePen(PS_GEOMETRIC|PS_SOLID, 1, &LineLogBrush[i], 0, NULL);
    LineLogBrushDim[i] = pGBC->LineLogBrushDim[i];
    LineLogBrushDim[i].lbColor = MakeColor(LineLogBrushDim[i].lbColor, fForPrinter);
    hLinePenDim[i] = ::ExtCreatePen(PS_GEOMETRIC|PS_SOLID, 1, &LineLogBrushDim[i], 0, NULL);;
    }
  
  LuMargin.cx=0;
  LuMargin.cy=0;
  LuBorder.cx=0;
  LuBorder.cy=0;

  ChScroll.cx=0;
  ChScroll.cy=0;
  ChScrollBy.cx=0;
  ChScrollBy.cy=0;

  bValid=1;
  LeaveCriticalSection(&cbDrawSect);  // Must be balanced
  return True;
  };

//---------------------------------------------------------------------------

COLORREF GDIBlk::MakeColor(COLORREF CR, flag fForPrinter) 
  {
  if (pGBC->fGrfRevGrayScaleOnPrint && fForPrinter)
    {
    BYTE r=GetRValue(CR);
    BYTE g=GetGValue(CR);
    BYTE b=GetBValue(CR);
    if (r==g && g==b)
      return RGB(255-r, 255-g, 255-b);
    }
  return CR;
  };

//---------------------------------------------------------------------------

flag GDIBlk::Attach(CDC* pDC_,CPrintInfo* pPInfo_)
  {
  EnterCriticalSection(&cbDrawSect);

  iAttachNo++;
  if (iAttachNo>=MaxGDIAttach)
    DoBreak();
  
  Attached[iAttachNo].pDC=pDC_;
  Attached[iAttachNo].pPInfo=pPInfo_;
  Attached[iAttachNo].ToPrint=(pPInfo_!=NULL);
  Attached[iAttachNo].pOldFont=pDC_->SelectObject(pFont[0]);

  return True;
  };

//---------------------------------------------------------------------------

flag GDIBlk::Detach()
  {
  Attached[iAttachNo].pDC->SelectObject(Attached[iAttachNo].pOldFont);
  Attached[iAttachNo].pDC=NULL;
  Attached[iAttachNo].pPInfo=NULL;
  iAttachNo--;

  LeaveCriticalSection(&cbDrawSect);  // Must be balanced

  return True;
  };

//---------------------------------------------------------------------------

CPoint GDIBlk::LuPt(const CPoint &Pt)
  {
  CPoint pt(LuMargin.cx+LuBorder.cx+(Pt.x-ChScroll.cx)*ColWdt(),
            LuMargin.cy+LuBorder.cy+(Pt.y-ChScroll.cy)*RowHgt());
  return pt;
  }

//---------------------------------------------------------------------------

CRect GDIBlk::GetClientRect()
  {
  if (IsPrinting())
    return Attached[iAttachNo].pPInfo->m_rectDraw;
  else
    {
    CRect cr;
    pWnd->GetClientRect(&cr);
    return cr;
    }
  }

//---------------------------------------------------------------------------

CRect GDIBlk::ChVisibleRect(flag fVScrlBar, flag fHScrlBar)
  {
  CRect cr=GetClientRect();
  CRect rd =LuDataRect();
  int cw=ColWdt();
  int rh=RowHgt();
  int rsbw=fVScrlBar ? VScrlWidth() : 0 ;
  int bsbh=fHScrlBar ? HScrlHeight() : 0 ;
  CRect rv(Max(0L, long((cr.left-rd.left+cw-1)/cw)),
           Max(0L, long((cr.top-rd.top+rh-1)/rh)),
           /*Min((int)ChDataSize.cx,*/ (int)((cr.right-rd.left+1-rsbw)/cw-1),
           /*Min((int)ChDataSize.cy,*/ (int)((cr.bottom-rd.top+1-bsbh)/rh-1));
  return rv;
  }

//---------------------------------------------------------------------------

CRect GDIBlk::LuPageRect()
  {
  CRect r(LuMargin.cx-ChScroll.cx*ColWdt(),
          LuMargin.cy-ChScroll.cy*RowHgt(),
          LuMargin.cx+2*LuBorder.cx+(ChDataSize.cx-ChScroll.cx)*ColWdt(),
          LuMargin.cy+2*LuBorder.cy+(ChDataSize.cy-ChScroll.cy)*RowHgt());
  return r;
  }

//---------------------------------------------------------------------------

CRect GDIBlk::LuDataRect()
  {
  CRect r(LuMargin.cx+LuBorder.cx-ChScroll.cx*ColWdt(),
          LuMargin.cy+LuBorder.cy-ChScroll.cy*RowHgt(),
          LuMargin.cx+LuBorder.cx+(ChDataSize.cx-ChScroll.cx)*ColWdt(),
          LuMargin.cy+LuBorder.cy+(ChDataSize.cy-ChScroll.cy)*RowHgt());
  return r;
  }

//---------------------------------------------------------------------------

CRect GDIBlk::LuWorkRect(flag fVScrlBar, flag fHScrlBar)
  {
  CRect cr=GetClientRect();
  CRect rd =LuDataRect();
  int cw=ColWdt();
  int rh=RowHgt();
  long rsbw=fVScrlBar ? VScrlWidth() : 0 ;
  long bsbh=fHScrlBar ? HScrlHeight() : 0 ;

  CRect rv(Max(cr.left+LuMargin.cx+LuBorder.cx, rd.left),
           Max(cr.top+LuMargin.cy+LuBorder.cy,rd.top),
           Min(cr.right-(LuMargin.cx+LuBorder.cx)-rsbw, rd.right),
           Min(cr.bottom-(LuMargin.cy+LuBorder.cy)-bsbh, rd.bottom));
  return rv;
  }

//---------------------------------------------------------------------------

CRect GDIBlk::ChWorkRect(flag fVScrlBar, flag fHScrlBar)
  {
//  CRect cr=GetClientRect();
//  CRect rd =LuDataRect();
  int cw=ColWdt();
  int rh=RowHgt();
//  int rsbw=fVScrlBar ? VScrlWidth() : 0 ;
//  int bsbh=fHScrlBar ? HScrlHeight() : 0 ;
//  CRect rv(0,//(cr.left-rd.left+cw-1)/cw,
//           0,//(cr.top-rd.top+rh-1)/rh,
//           (cr.right-Max(0L, rd.left-cr.left)-Max(0L, cr.right-rd.right)+1-rsbw)/cw-1,
//           (cr.bottom-Max(0L, rd.top-cr.top)-Max(0L, cr.bottom-rd.bottom)+1-bsbh)/rh-1);
//           //(cr.bottom-rd.top+1-bsbh)/rh-1);
  CRect wr=LuWorkRect(fVScrlBar, fHScrlBar);
  wr.left=wr.left/cw;
  wr.right=(wr.right)/cw;
  wr.top=wr.top/rh;
  wr.bottom=(wr.bottom)/rh;
  return wr;
  }

//---------------------------------------------------------------------------

int GDIBlk::ChWorkWidth(flag fVScrlBar)
  {
  CRect r=ChWorkRect(fVScrlBar, False);
  return r.right-r.left+1;
  }

//---------------------------------------------------------------------------

int GDIBlk::ChWorkHeight(flag fHScrlBar)
  {
  CRect r=ChWorkRect(False, fHScrlBar);
  return r.bottom-r.top+1;
  }

//---------------------------------------------------------------------------

CRect GDIBlk::LuClipRect(flag fVScrlBar, flag fHScrlBar)
  {
  CRect cr=GetClientRect();
  int rsbw=fVScrlBar ? VScrlWidth() : 0 ;
  int bsbh=fHScrlBar ? HScrlHeight() : 0 ;
  CRect rv(LuMargin.cx,
           LuMargin.cy,
           cr.right-rsbw,
           cr.bottom-bsbh);
  return rv;
  }

//---------------------------------------------------------------------------

int GDIBlk::VScrlWidth()  
  {
  int sbs=GBC().ScrollBarSize;
  return sbs>0 ? ColWdt()*sbs/10 : GetSystemMetrics(SM_CXVSCROLL);
  }

//---------------------------------------------------------------------------

int GDIBlk::HScrlHeight()
  {
  int sbs=GBC().ScrollBarSize;
  return sbs>0 ? ColWdt()*sbs/10 : GetSystemMetrics(SM_CXHSCROLL);
  }

//===========================================================================

CDCResChk::CDCResChk(CDC & DC):
  dc(DC)
  {
#ifdef _DEBUG
  pMemBitmap =dc.GetCurrentBitmap( );
  pMemBrush  =dc.GetCurrentBrush( );
  pMemFont   =dc.GetCurrentFont( );
  pMemPen    =dc.GetCurrentPen( );
#endif
  }

CDCResChk::CDCResChk(CDC * pDC):
  dc(*pDC)
  {
#ifdef _DEBUG
  pMemBitmap =dc.GetCurrentBitmap( );
  pMemBrush  =dc.GetCurrentBrush( );
  pMemFont   =dc.GetCurrentFont( );
  pMemPen    =dc.GetCurrentPen( );
#endif
  }

CDCResChk::~CDCResChk()
  {
#ifdef _DEBUG
  ASSERT_ALWAYS(pMemBitmap==dc.GetCurrentBitmap( ), "Bitmap not restored");
  ASSERT_ALWAYS(pMemBrush ==dc.GetCurrentBrush( ),  "Brush  not restored");
//  ASSERT_ALWAYS(pMemFont  ==dc.GetCurrentFont( ),   "Font   not restored");
  ASSERT_ALWAYS(pMemPen   ==dc.GetCurrentPen( ),    "Pen    not restored");
#endif
  }

//===========================================================================