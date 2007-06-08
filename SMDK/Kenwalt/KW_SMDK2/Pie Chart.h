//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __PIE_CHART_H
#define  __PIE_CHART_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#include <vector>

#ifdef __PIE_CHART_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

using namespace std;

//---------------------------------------------------------------------------

class PieChart : public MBaseMethod
  {
  public:
    PieChart(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
	virtual bool	ValidateDataFields();

	//virtual bool PreStartCheck();

    //virtual void    EvalProducts();
    //virtual void    EvalCtrlInitialise(eScdCtrlTasks Tasks=CO_All); // each Start of Run
    //virtual void    EvalCtrlActions(eScdCtrlTasks Tasks=CO_All)   ; // each Iteration - To apply result of control
    //virtual void    EvalCtrlStrategy(eScdCtrlTasks Tasks=CO_All); // each Iteration - To calculate required control actions
    //virtual void    EvalCtrlTerminate(eScdCtrlTasks Tasks=CO_All)           {}; // each End of Run
    //virtual void    EvalStatistics(eScdCtrlTasks Tasks=CO_All)              ; // each Iteration End to calculate Stats relevant to model

	//virtual void SetState(MStatesToSet SS);
	
	virtual bool GetModelGraphic(CMdlGraphicArray &Grfs);
	virtual bool OperateModelGraphic(CMdlGraphicWnd &Wnd, CMdlGraphic &gfx);

  protected:
	//User entered data:
	CString sPipeName;

	bool bByCompound;
	long nPhase;

	double dMaxFlow;
	double dMinTemp, dMaxTemp;
  };

//Not sure if these are defined anywhere, so I grabbed this from the .NET colour list
struct Colours
{
	static CONST COLORREF ActiveBorder = COLORREF(RGB(212, 208, 200));
	static CONST COLORREF ActiveCaption = COLORREF(RGB(0, 84, 227));
	static CONST COLORREF ActiveCaptionText = COLORREF(RGB(255, 255, 255));
	static CONST COLORREF AppWorkspace = COLORREF(RGB(128, 128, 128));
	static CONST COLORREF Control = COLORREF(RGB(236, 233, 216));
	static CONST COLORREF ControlDark = COLORREF(RGB(172, 168, 153));
	static CONST COLORREF ControlDarkDark = COLORREF(RGB(113, 111, 100));
	static CONST COLORREF ControlLight = COLORREF(RGB(241, 239, 226));
	static CONST COLORREF ControlLightLight = COLORREF(RGB(255, 255, 255));
	static CONST COLORREF ControlText = COLORREF(RGB(0, 0, 0));
	static CONST COLORREF Desktop = COLORREF(RGB(0, 78, 152));
	static CONST COLORREF GrayText = COLORREF(RGB(172, 168, 153));
	static CONST COLORREF Highlight = COLORREF(RGB(49, 106, 197));
	static CONST COLORREF HighlightText = COLORREF(RGB(255, 255, 255));
	static CONST COLORREF HotTrack = COLORREF(RGB(0, 0, 128));
	static CONST COLORREF InactiveBorder = COLORREF(RGB(212, 208, 200));
	static CONST COLORREF InactiveCaption = COLORREF(RGB(122, 150, 223));
	static CONST COLORREF InactiveCaptionText = COLORREF(RGB(216, 228, 248));
	static CONST COLORREF Info = COLORREF(RGB(255, 255, 225));
	static CONST COLORREF InfoText = COLORREF(RGB(0, 0, 0));
	static CONST COLORREF Menu = COLORREF(RGB(255, 255, 255));
	static CONST COLORREF MenuText = COLORREF(RGB(0, 0, 0));
	static CONST COLORREF ScrollBar = COLORREF(RGB(212, 208, 200));
	static CONST COLORREF Window = COLORREF(RGB(255, 255, 255));
	static CONST COLORREF WindowFrame = COLORREF(RGB(0, 0, 0));
	static CONST COLORREF WindowText = COLORREF(RGB(0, 0, 0));
	static CONST COLORREF Transparent = COLORREF(RGB(255, 255, 255));
	static CONST COLORREF AliceBlue = COLORREF(RGB(240, 248, 255));
	static CONST COLORREF AntiqueWhite = COLORREF(RGB(250, 235, 215));
	static CONST COLORREF Aqua = COLORREF(RGB(0, 255, 255));
	static CONST COLORREF Aquamarine = COLORREF(RGB(127, 255, 212));
	static CONST COLORREF Azure = COLORREF(RGB(240, 255, 255));
	static CONST COLORREF Beige = COLORREF(RGB(245, 245, 220));
	static CONST COLORREF Bisque = COLORREF(RGB(255, 228, 196));
	static CONST COLORREF Black = COLORREF(RGB(0, 0, 0));
	static CONST COLORREF BlanchedAlmond = COLORREF(RGB(255, 235, 205));
	static CONST COLORREF Blue = COLORREF(RGB(0, 0, 255));
	static CONST COLORREF BlueViolet = COLORREF(RGB(138, 43, 226));
	static CONST COLORREF Brown = COLORREF(RGB(165, 42, 42));
	static CONST COLORREF BurlyWood = COLORREF(RGB(222, 184, 135));
	static CONST COLORREF CadetBlue = COLORREF(RGB(95, 158, 160));
	static CONST COLORREF Chartreuse = COLORREF(RGB(127, 255, 0));
	static CONST COLORREF Chocolate = COLORREF(RGB(210, 105, 30));
	static CONST COLORREF Coral = COLORREF(RGB(255, 127, 80));
	static CONST COLORREF CornflowerBlue = COLORREF(RGB(100, 149, 237));
	static CONST COLORREF Cornsilk = COLORREF(RGB(255, 248, 220));
	static CONST COLORREF Crimson = COLORREF(RGB(220, 20, 60));
	static CONST COLORREF Cyan = COLORREF(RGB(0, 255, 255));
	static CONST COLORREF DarkBlue = COLORREF(RGB(0, 0, 139));
	static CONST COLORREF DarkCyan = COLORREF(RGB(0, 139, 139));
	static CONST COLORREF DarkGoldenrod = COLORREF(RGB(184, 134, 11));
	static CONST COLORREF DarkGray = COLORREF(RGB(169, 169, 169));
	static CONST COLORREF DarkGreen = COLORREF(RGB(0, 100, 0));
	static CONST COLORREF DarkKhaki = COLORREF(RGB(189, 183, 107));
	static CONST COLORREF DarkMagenta = COLORREF(RGB(139, 0, 139));
	static CONST COLORREF DarkOliveGreen = COLORREF(RGB(85, 107, 47));
	static CONST COLORREF DarkOrange = COLORREF(RGB(255, 140, 0));
	static CONST COLORREF DarkOrchid = COLORREF(RGB(153, 50, 204));
	static CONST COLORREF DarkRed = COLORREF(RGB(139, 0, 0));
	static CONST COLORREF DarkSalmon = COLORREF(RGB(233, 150, 122));
	static CONST COLORREF DarkSeaGreen = COLORREF(RGB(143, 188, 139));
	static CONST COLORREF DarkSlateBlue = COLORREF(RGB(72, 61, 139));
	static CONST COLORREF DarkSlateGray = COLORREF(RGB(47, 79, 79));
	static CONST COLORREF DarkTurquoise = COLORREF(RGB(0, 206, 209));
	static CONST COLORREF DarkViolet = COLORREF(RGB(148, 0, 211));
	static CONST COLORREF DeepPink = COLORREF(RGB(255, 20, 147));
	static CONST COLORREF DeepSkyBlue = COLORREF(RGB(0, 191, 255));
	static CONST COLORREF DimGray = COLORREF(RGB(105, 105, 105));
	static CONST COLORREF DodgerBlue = COLORREF(RGB(30, 144, 255));
	static CONST COLORREF Firebrick = COLORREF(RGB(178, 34, 34));
	static CONST COLORREF FloralWhite = COLORREF(RGB(255, 250, 240));
	static CONST COLORREF ForestGreen = COLORREF(RGB(34, 139, 34));
	static CONST COLORREF Fuchsia = COLORREF(RGB(255, 0, 255));
	static CONST COLORREF Gainsboro = COLORREF(RGB(220, 220, 220));
	static CONST COLORREF GhostWhite = COLORREF(RGB(248, 248, 255));
	static CONST COLORREF Gold = COLORREF(RGB(255, 215, 0));
	static CONST COLORREF Goldenrod = COLORREF(RGB(218, 165, 32));
	static CONST COLORREF Gray = COLORREF(RGB(128, 128, 128));
	static CONST COLORREF Green = COLORREF(RGB(0, 128, 0));
	static CONST COLORREF GreenYellow = COLORREF(RGB(173, 255, 47));
	static CONST COLORREF Honeydew = COLORREF(RGB(240, 255, 240));
	static CONST COLORREF HotPink = COLORREF(RGB(255, 105, 180));
	static CONST COLORREF IndianRed = COLORREF(RGB(205, 92, 92));
	static CONST COLORREF Indigo = COLORREF(RGB(75, 0, 130));
	static CONST COLORREF Ivory = COLORREF(RGB(255, 255, 240));
	static CONST COLORREF Khaki = COLORREF(RGB(240, 230, 140));
	static CONST COLORREF Lavender = COLORREF(RGB(230, 230, 250));
	static CONST COLORREF LavenderBlush = COLORREF(RGB(255, 240, 245));
	static CONST COLORREF LawnGreen = COLORREF(RGB(124, 252, 0));
	static CONST COLORREF LemonChiffon = COLORREF(RGB(255, 250, 205));
	static CONST COLORREF LightBlue = COLORREF(RGB(173, 216, 230));
	static CONST COLORREF LightCoral = COLORREF(RGB(240, 128, 128));
	static CONST COLORREF LightCyan = COLORREF(RGB(224, 255, 255));
	static CONST COLORREF LightGoldenrodYellow = COLORREF(RGB(250, 250, 210));
	static CONST COLORREF LightGray = COLORREF(RGB(211, 211, 211));
	static CONST COLORREF LightGreen = COLORREF(RGB(144, 238, 144));
	static CONST COLORREF LightPink = COLORREF(RGB(255, 182, 193));
	static CONST COLORREF LightSalmon = COLORREF(RGB(255, 160, 122));
	static CONST COLORREF LightSeaGreen = COLORREF(RGB(32, 178, 170));
	static CONST COLORREF LightSkyBlue = COLORREF(RGB(135, 206, 250));
	static CONST COLORREF LightSlateGray = COLORREF(RGB(119, 136, 153));
	static CONST COLORREF LightSteelBlue = COLORREF(RGB(176, 196, 222));
	static CONST COLORREF LightYellow = COLORREF(RGB(255, 255, 224));
	static CONST COLORREF Lime = COLORREF(RGB(0, 255, 0));
	static CONST COLORREF LimeGreen = COLORREF(RGB(50, 205, 50));
	static CONST COLORREF Linen = COLORREF(RGB(250, 240, 230));
	static CONST COLORREF Magenta = COLORREF(RGB(255, 0, 255));
	static CONST COLORREF Maroon = COLORREF(RGB(128, 0, 0));
	static CONST COLORREF MediumAquamarine = COLORREF(RGB(102, 205, 170));
	static CONST COLORREF MediumBlue = COLORREF(RGB(0, 0, 205));
	static CONST COLORREF MediumOrchid = COLORREF(RGB(186, 85, 211));
	static CONST COLORREF MediumPurple = COLORREF(RGB(147, 112, 219));
	static CONST COLORREF MediumSeaGreen = COLORREF(RGB(60, 179, 113));
	static CONST COLORREF MediumSlateBlue = COLORREF(RGB(123, 104, 238));
	static CONST COLORREF MediumSpringGreen = COLORREF(RGB(0, 250, 154));
	static CONST COLORREF MediumTurquoise = COLORREF(RGB(72, 209, 204));
	static CONST COLORREF MediumVioletRed = COLORREF(RGB(199, 21, 133));
	static CONST COLORREF MidnightBlue = COLORREF(RGB(25, 25, 112));
	static CONST COLORREF MintCream = COLORREF(RGB(245, 255, 250));
	static CONST COLORREF MistyRose = COLORREF(RGB(255, 228, 225));
	static CONST COLORREF Moccasin = COLORREF(RGB(255, 228, 181));
	static CONST COLORREF NavajoWhite = COLORREF(RGB(255, 222, 173));
	static CONST COLORREF Navy = COLORREF(RGB(0, 0, 128));
	static CONST COLORREF OldLace = COLORREF(RGB(253, 245, 230));
	static CONST COLORREF Olive = COLORREF(RGB(128, 128, 0));
	static CONST COLORREF OliveDrab = COLORREF(RGB(107, 142, 35));
	static CONST COLORREF Orange = COLORREF(RGB(255, 165, 0));
	static CONST COLORREF OrangeRed = COLORREF(RGB(255, 69, 0));
	static CONST COLORREF Orchid = COLORREF(RGB(218, 112, 214));
	static CONST COLORREF PaleGoldenrod = COLORREF(RGB(238, 232, 170));
	static CONST COLORREF PaleGreen = COLORREF(RGB(152, 251, 152));
	static CONST COLORREF PaleTurquoise = COLORREF(RGB(175, 238, 238));
	static CONST COLORREF PaleVioletRed = COLORREF(RGB(219, 112, 147));
	static CONST COLORREF PapayaWhip = COLORREF(RGB(255, 239, 213));
	static CONST COLORREF PeachPuff = COLORREF(RGB(255, 218, 185));
	static CONST COLORREF Peru = COLORREF(RGB(205, 133, 63));
	static CONST COLORREF Pink = COLORREF(RGB(255, 192, 203));
	static CONST COLORREF Plum = COLORREF(RGB(221, 160, 221));
	static CONST COLORREF PowderBlue = COLORREF(RGB(176, 224, 230));
	static CONST COLORREF Purple = COLORREF(RGB(128, 0, 128));
	static CONST COLORREF Red = COLORREF(RGB(255, 0, 0));
	static CONST COLORREF RosyBrown = COLORREF(RGB(188, 143, 143));
	static CONST COLORREF RoyalBlue = COLORREF(RGB(65, 105, 225));
	static CONST COLORREF SaddleBrown = COLORREF(RGB(139, 69, 19));
	static CONST COLORREF Salmon = COLORREF(RGB(250, 128, 114));
	static CONST COLORREF SandyBrown = COLORREF(RGB(244, 164, 96));
	static CONST COLORREF SeaGreen = COLORREF(RGB(46, 139, 87));
	static CONST COLORREF SeaShell = COLORREF(RGB(255, 245, 238));
	static CONST COLORREF Sienna = COLORREF(RGB(160, 82, 45));
	static CONST COLORREF Silver = COLORREF(RGB(192, 192, 192));
	static CONST COLORREF SkyBlue = COLORREF(RGB(135, 206, 235));
	static CONST COLORREF SlateBlue = COLORREF(RGB(106, 90, 205));
	static CONST COLORREF SlateGray = COLORREF(RGB(112, 128, 144));
	static CONST COLORREF Snow = COLORREF(RGB(255, 250, 250));
	static CONST COLORREF SpringGreen = COLORREF(RGB(0, 255, 127));
	static CONST COLORREF SteelBlue = COLORREF(RGB(70, 130, 180));
	static CONST COLORREF Tan = COLORREF(RGB(210, 180, 140));
	static CONST COLORREF Teal = COLORREF(RGB(0, 128, 128));
	static CONST COLORREF Thistle = COLORREF(RGB(216, 191, 216));
	static CONST COLORREF Tomato = COLORREF(RGB(255, 99, 71));
	static CONST COLORREF Turquoise = COLORREF(RGB(64, 224, 208));
	static CONST COLORREF Violet = COLORREF(RGB(238, 130, 238));
	static CONST COLORREF Wheat = COLORREF(RGB(245, 222, 179));
	static CONST COLORREF White = COLORREF(RGB(255, 255, 255));
	static CONST COLORREF WhiteSmoke = COLORREF(RGB(245, 245, 245));
	static CONST COLORREF Yellow = COLORREF(RGB(255, 255, 0));
	static CONST COLORREF YellowGreen = COLORREF(RGB(154, 205, 50));
	static CONST COLORREF ButtonFace = COLORREF(RGB(236, 233, 216));
	static CONST COLORREF ButtonHighlight = COLORREF(RGB(255, 255, 255));
	static CONST COLORREF ButtonShadow = COLORREF(RGB(172, 168, 153));
	static CONST COLORREF GradientActiveCaption = COLORREF(RGB(61, 149, 255));
	static CONST COLORREF GradientInactiveCaption = COLORREF(RGB(157, 185, 235));
	static CONST COLORREF MenuBar = COLORREF(RGB(236, 233, 216));
	static CONST COLORREF MenuHighlight = COLORREF(RGB(49, 106, 197));
};

#endif