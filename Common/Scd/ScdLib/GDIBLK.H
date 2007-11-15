//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef __GDIBLK_H
#define __GDIBLK_H

#ifndef __SC_DEFS_H
  #include "sc_defs.H"
#endif

#ifdef __GDIBLK_CPP
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ==========================================================================

_FWDDEF(GDIBlkCfg);
_FWDDEF(GDIBlk);

const int MaxGDIFonts = 3;
const int MaxGDILines = 8;
class DllImportExport GDIBlkCfg
  {
  public :
    char           Name[32];

    COLORREF       crText[2];
    COLORREF       crTextChg[2];
    COLORREF       crTextDef[2];
    COLORREF       crTextInvalid[2];
    COLORREF       crTextError[2];
    COLORREF       crTxtBackError;
    COLORREF       crTxtBackSelect;
    COLORREF       crTxtBackRmtData;
    COLORREF       crTxtBackRmtChgd;
    COLORREF       crTxtBack;
    COLORREF       crTxtFore;
    COLORREF       crTxtEdgeLight;
    COLORREF       crTxtEdgeDark;
    COLORREF       crTxtBord;

    COLORREF       crGrfGrid;
    COLORREF       crGrfBack;
    COLORREF       crGrfAxes;

    COLORREF       crTrndGrid;
    COLORREF       crTrndBack;
    COLORREF       crTrndAxes;

    short          FontHgt[MaxGDIFonts];    // FontHgt in Points;
    short          SmlFontHgt[MaxGDIFonts]; // FontHgt in Points;
    LOGFONT        LogFont[MaxGDIFonts];

    LOGBRUSH       LineLogBrush[MaxGDILines];
    LOGBRUSH       LineLogBrushDim[MaxGDILines];

    int            ScrollBarSize; // in tenths of Charwidths;
    flag           fGrfRevGrayScaleOnPrint;

    GDIBlkCfg(pchar pName_);
    virtual ~GDIBlkCfg();
    GDIBlkCfg(const GDIBlkCfg &S);
    GDIBlkCfg &GDIBlkCfg::operator=(const GDIBlkCfg &S);
    virtual void Save();
    virtual void Restore();
    virtual void DoConfigDialog();

    static pGDIBlkCfg List;
    pGDIBlkCfg     Next;
    static  void SaveAll();
    static  void RestoreAll();
  };

// ==========================================================================

const int MaxGDIAttach=4;
               
class DllImportExport GDIBlk
  {
  public :
//    pFxdEdtView    pBook;
    GDIBlkCfg*     pGBC;
    int            iAttachCount;

    static CRITICAL_SECTION cbDrawSect;
    static int     iDrawSectBlkCnt;

    CWnd*          pWnd;

    int            iAttachNo;
    struct tagAttach{
      CDC*         pDC;
      CPrintInfo*  pPInfo;
      flag         ToPrint;
      CFont*       pOldFont;
      }            Attached[MaxGDIAttach];

    flag           bValid;

    COLORREF       crText[2];
    COLORREF       crTextChg[2];
    COLORREF       crTextDef[2];
    COLORREF       crTextInvalid[2];
    COLORREF       crTextError[2];
    COLORREF       crTxtBackError;
    COLORREF       crTxtBackSelect;
    COLORREF       crTxtBackRmtData;
    COLORREF       crTxtBackRmtChgd;
    COLORREF       crTxtBack;
    COLORREF       crTxtFore;
    COLORREF       crTxtEdgeLight;
    COLORREF       crTxtEdgeDark;
    COLORREF       crTxtBord;

    COLORREF       crGrfGrid;
    COLORREF       crGrfBack;
    COLORREF       crGrfAxes;

    COLORREF       crTrndGrid;
    COLORREF       crTrndBack;
    COLORREF       crTrndAxes;

//    CPen*          pPenTxt;
    CBrush*        pBrushTxtFore;
    CBrush*        pBrushTxtBack;
    CBrush*        pBrushTxtBackError;
    CBrush*        pBrushTxtBackSelect;
    CBrush*        pBrushTxtBackRmtData;
    CBrush*        pBrushTxtBackRmtChgd;
    CBrush*        pBrushTxtEdgeLight;
    CBrush*        pBrushTxtEdgeDark;
    CPen*          pPenTxtBord;
    CPen*          pPenTxtBack;

    CPen*          pPenGrfGrid;
    CPen*          pPenGrfAxes;
    CBrush*        pBrushGrfBack;

    CPen*          pPenTrndGrid;
    CPen*          pPenTrndAxes;
    CBrush*        pBrushTrndBack;

    LOGBRUSH       LineLogBrush[MaxGDILines];
    LOGBRUSH       LineLogBrushDim[MaxGDILines];
    HPEN           hLinePen[MaxGDILines];
    HPEN           hLinePenDim[MaxGDILines];

    CFont*         pFont[MaxGDIFonts];
    CFont*         pSmlFont[MaxGDIFonts];
    CFont*         pItlFont[MaxGDIFonts];
    SIZE           CharSize[MaxGDIFonts];
    SIZE           LineSize[MaxGDIFonts];
    SIZE           CharSpc[MaxGDIFonts];
    int            EdgeW;

    SIZE           LuMargin, LuBorder, ChDataSize, ChScroll, ChScrollBy, LuDataSize;
    
    GDIBlk();
    virtual ~GDIBlk();
    
    virtual void Clear();
    virtual flag Create(pGDIBlkCfg pGBC, CWnd* pWnd_, CDC* pDC_, flag fForPrinter);
    virtual COLORREF MakeColor(COLORREF CR, flag fForPrinter);
    virtual flag Attach(CDC* pDC,CPrintInfo* pPInfo);
    virtual flag Detach();

    GDIBlkCfg&     GBC()            { return *pGBC; };
    CWnd&          Wnd()            { return *pWnd; };
    CDC&           DC()             { return *Attached[iAttachNo].pDC; };
    CPrintInfo&    PrintInfo()      { return *Attached[iAttachNo].pPInfo; };

    long           RowHgt()   { return LineSize[0].cy; };
    long           ColWdt()   { return LineSize[0].cx; };
    long           RowSpace() { return CharSpc[0].cy; };
    long           ColSpace() { return CharSpc[0].cx; };
    long           EdgeWdt()  { return EdgeW; };

    CRect          GetClientRect();
    CPoint         LuPt(const CPoint &Pt);
    CRect          ChVisibleRect(flag fVScrlBar, flag fHScrlBar);
    CRect          LuPageRect();
    CRect          LuDataRect();
    CRect          LuWorkRect(flag fVScrlBar, flag fHScrlBar);
    CRect          ChWorkRect(flag fVScrlBar, flag fHScrlBar);
    int            ChWorkWidth(flag fVScrlBar);
    int            ChWorkHeight(flag fHScrlBar);
    int            VScrlWidth();
    int            HScrlHeight();
    CRect          LuClipRect(flag fVScrlBar, flag fHScrlBar);

    flag           IsPrinting()     { return Attached[iAttachNo].ToPrint;}

    static flag    bIsLowRes;
    static flag    LowRes()   { return bIsLowRes; };
  };

// ==========================================================================

class DllImportExport CDCResChk
  {
  public:
    CDC & dc;
#ifdef _DEBUG
    CBitmap* pMemBitmap;
    CBrush*  pMemBrush;
    CFont*   pMemFont;
    CPen*    pMemPen;
#endif

    CDCResChk(CDC & DC);
    CDCResChk(CDC * pDC);
    ~CDCResChk();
  };

// ==========================================================================

#undef DllImportExport

#endif
