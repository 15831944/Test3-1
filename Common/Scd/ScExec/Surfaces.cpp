//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#define __SURFACES_CPP
#include "gpfuncs.h"
#include "dbgetc.h"
#include "dbgmngr.h"
#include "surfaces.h"
#include "errorlog.h"

//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define dbgSurfaces 01

#if dbgSurfaces
//static CDbgMngr dbgDump("Surfaces", "Dump");
static CDbgMngr dbgInitialise("Surfaces", "Initialise");
static CDbgMngr dbg3DTrack   ("Surfaces", "3DTrack");
static CDbgMngr dbg3DState   ("Surfaces", "3DState");
static CDbgMngr dbg3DMRU     ("Surfaces", "3DMRU");
#endif

#pragma chCHECKIT(Set these limits to something reasonable)
const long ALLOCDBGCNT = 100;
const long ALLOCCNTMAX = 5000;

//===========================================================================
//
//
//
//===========================================================================

double SfXFDirect(double X) { return X; };
double SfXFLogIn(double X) { return log(X); };
double SfXFLogOut(double X) { return exp(X); };

//===========================================================================
//
//
//
//===========================================================================

CArray<CSurfaceCommon*, CSurfaceCommon*> CSurfaceCommon::List;

bool CSurfaceCommon::m_UseSurface = false;
bool CSurfaceCommon::m_DoSmoothing = false;
double CSurfaceCommon::m_FitTolerance = 1.0e-4;

double CSurfaceCommon::m_ErrorMsgTol = 1.0e-3;

//===========================================================================
//
//
//
//===========================================================================

inline double FracErr(double A, double B) { return fabs(A-B)/Max(1.0e-10, Max(fabs(A), fabs(B))); };

flag CSurface2DElement::Fit(CSurface2DPoint L, CSurface2DPoint R)
  {
  m_A=(R.y-L.y)/(R.x-L.x);
  m_B=L.y-m_A*L.x;
  m_Inited=true;
  return true;
  }

double CSurface2DElement::CheckFit(long Ix, CSurface2D & Surface)
  {
  return FracErr(Surface.m_fnYO((*this)(0.5)), Surface.m_fnY(Surface.Map2X(Ix,0.5)));
  };

const double SmoothDX=0.5;

//===========================================================================
//
//
//
//===========================================================================

CSurface2D::CSurface2D()
  {
  m_nDivs=1;
  m_nDivsInited=0;
  m_dMinimum=0;
  m_dMaximum=1;
  m_dDivWidth=1;
    
  m_WorstErrorV=0;
  m_WorstErrorX=0;
  
  m_fnY=NULL;

  }

//---------------------------------------------------------------------------

CSurface2D::CSurface2D(LPCTSTR Name,
                       double MinX, double MaxX, long NDivs, TSurfaceXfFn fnXI, TSurfaceXfFn fnXO, 
                       TSurface2DFn Fn, TSurfaceXfFn fnYI, TSurfaceXfFn fnYO)
  {
  Initialise(Name, MinX, MaxX, NDivs, fnXI, fnXO, Fn, fnYI, fnYO);
  };

//---------------------------------------------------------------------------

bool CSurface2D::Initialise(LPCTSTR Name,
                            double MinX, double MaxX, long NDivs, TSurfaceXfFn fnXI, TSurfaceXfFn fnXO, 
                            TSurface2DFn Fn, TSurfaceXfFn fnYI, TSurfaceXfFn fnYO)
  {
  m_sName=Name; 
  m_fnXI=fnXI?fnXI:SfXFDirect;
  m_fnXO=fnXO?fnXO:SfXFDirect;

  m_nDivs=NDivs;
  m_nDivsInited=0;
  m_dMinimum=m_fnXI(MinX);
  m_dMaximum=m_fnXI(MaxX);
  
  m_fnYI=fnYI?fnYI:SfXFDirect;
  m_fnYO=fnYO?fnYO:SfXFDirect;
  
  m_DX=SmoothDX;

  if (NDivs>0 && m_dMaximum>m_dMinimum && Fn!=NULL)
    {
    m_dDivWidth=(m_dMaximum-m_dMinimum)/NDivs; 
    m_fnY=Fn;
    m_Elements.RemoveAll();// Reinitialise ...
    m_Elements.SetSize(NDivs);
    return true;
    }

  return false;
  };

//---------------------------------------------------------------------------

CSurface2DElement &CSurface2D::GetElement(long Ix)
  {
  CSurface2DElement &E=m_Elements[Ix];
  if (!E.m_Inited)
    {                                                           
    CSingleLock Lock(&m_TheLock, true);
    if (!E.m_Inited)
      {
      AddToList();

      CSurface2DPoint L(0.0, m_fnYI(m_fnY(Map2X(Ix,0))));
      CSurface2DPoint R(1.0, m_fnYI(m_fnY(Map2X(Ix,1))));
      E.Fit(L, R);
      double e=E.CheckFit(Ix, *this);
      if (e>m_WorstErrorV)
        {
        if (e>Max(m_ErrorMsgTol, m_WorstErrorV*1.1))
          LogWarning("InterpolationFn", 0, "%s has error of %.2f%% @%.3f", 
                     Name(), 100*e, Map2X(Ix, 0.5));
        m_WorstErrorV=e;
        m_WorstErrorX=Ix;
        }
      m_nDivsInited++;
      }
    }
  return E;
  }

//---------------------------------------------------------------------------

inline void GetDX(long I, double F, double D, long nDiv, long &IM, double &FM, long &IP, double &FP)
  {
  FM=F-D;
  if (FM<0)
    {
    IM=I-1;
    FM=FM+1;
    }
  else
    IM=I;

  FP=F+D;
  if (FP>1)
    {
    IP=I+1;
    FP=FP-1;
    }
  else
    IP=I;

  if (IM<0)
    {
    long d=-IM;
    IM+=d;
    FM-=d;
    }
  else if (IP>=nDiv)
    {
    long d=1+IP-nDiv;
    IP-=d;
    FP+=d;
    }
  }

//---------------------------------------------------------------------------

double CSurface2D::operator()(double X)
  {
  if (!m_UseSurface)
    return m_fnY(X);

  long Ix;
  double Fx;
  X2Map(X, Ix, Fx);

  ASSERT(Ix<m_Elements.GetSize());
//  int iFx=(int)floor(Fx*m_iDX);
//  if (m_DoSmoothing && (iFx==0 || iFx==(m_iDX-1))
  if (m_DoSmoothing)
    {
    long IxM, IxP;
    double FxM, FxP;
    GetDX(Ix, Fx, m_DX, m_nDivs, IxM, FxM, IxP, FxP);
    CSurface2DElement &EM=GetElement(IxM);
    CSurface2DElement &EP=GetElement(IxP);

    //if (m_ForceFn) // Force Use Of Function
    //  return m_fnY(X);
  
    return m_fnYO(0.5*(EM(FxM)+EP(FxP)));
    }

  CSurface2DElement &E=GetElement(Ix);
  
  //if (m_ForceFn) // Force Use Of Function
  //  return m_fnY(X);
  return m_fnYO(E(Fx));
  };

//---------------------------------------------------------------------------
    
void CSurface2D::Dump()
  {
  dbgpln("Surface:%s", Name());
  dbgpln("Usage  : %10.5f%%  [%i of %i]", Usage()*100, m_nDivsInited, m_nDivs);
  dbgpln("Worst  : %10.5f%%  @%i = %g", 100*m_WorstErrorV, m_WorstErrorX, Map2X(m_WorstErrorX, 0.5));

  dbgpln("I:\tX\tY\tDiff");
  for (int i=0; i<=m_nDivs; i++)
    {
    CSurface2DElement &E=m_Elements[Min((long)i, m_nDivs-1L)];
    if (E.m_Inited)
      {
      for(int j=0; j<2; j++)
        {
        double di=j*0.5;
        double x1=Map2X(i,di);
        double y1=m_fnYO(E(di));
        dbgpln("%5.1f:\t%15.8f\t%15.8f\t%15.8f\t%8.5e", 
               i+di,x1,y1,y1-m_fnY(x1),100*FracErr(y1, m_fnY(x1)));
        }
      }
    else
      {
      //for(int j=0; j<2; j++)
      //  {
      //  double di=j*0.5;
      //  dbgpln("%5.1f:\t%15.8f\t\t\t", i+di,Map2X(i,di));
      //  }
      }
    }
  };

//===========================================================================
//
//
//
//===========================================================================

#define  DET(a11,a12,a21,a22)   (((a11) * (a22)) - ((a21) * (a12)))

bool CSurface3DTriangle::Fit(/*long No,*/ CSurface3DPoint &P1, CSurface3DPoint &P2, CSurface3DPoint &P3)
  {
  double a = DET(P2.y-P1.y,P2.z-P1.z,P3.y-P1.y,P3.z-P1.z);
  double b = DET(P2.z-P1.z,P2.x-P1.x,P3.z-P1.z,P3.x-P1.x);
  double c = DET(P2.x-P1.x,P2.y-P1.y,P3.x-P1.x,P3.y-P1.y);

  if (fabs(c) < 1.0e-30)
    return false;

  m_A = (a/c) * (-1.0);
  m_B = (b/c) * (-1.0);
  m_C = (a*P1.x + b*P1.y + c*P1.z)/c;
  //m_No = No;
  return true;
  }

//---------------------------------------------------------------------------

double CSurface3DTriangle::operator()(double mx, double my)
  { 
  return m_A*mx+m_B*my+m_C;
  };
    
//===========================================================================
//
//
//
//===========================================================================
#if UseNewSurfaces

CSurface3DPanel::CSurface3DPanel()
  {
  m_pFrame=NULL;
  m_pMRUNext=NULL;
  m_pMRUPrev=NULL;

  m_bLOK=false;
  m_bROK=false;
  m_bTOK=false;
  m_bBOK=false;
  m_bFitOK=false;
  };

//---------------------------------------------------------------------------

CSurface3DPanel::CSurface3DPanel(CSurface3D *p3D, CSurface3DFrame *pFrame,
                                long nX, double XMin, double XMax, long nY, double YMin, double YMax)
  {
  m_pFrame=pFrame;
  m_pMRUNext=NULL;
  m_pMRUPrev=NULL;

  CSurface3DPoint BL(0.0, 0.0, (p3D->m_fnZ(XMin, YMin)));
  CSurface3DPoint BR(1.0, 0.0, (p3D->m_fnZ(XMax, YMin)));
  CSurface3DPoint TL(0.0, 1.0, (p3D->m_fnZ(XMin, YMax)));
  CSurface3DPoint TR(1.0, 1.0, (p3D->m_fnZ(XMax, YMax)));
  CSurface3DPoint CC(0.5, 0.5, (p3D->m_fnZ(0.5*(XMin+XMax), 0.5*(YMin+YMax))));
  m_bLOK=m_tL.Fit(BL, TL, CC);
  m_bROK=m_tR.Fit(BR, TR, CC);
  m_bTOK=m_tT.Fit(TL, TR, CC);
  m_bBOK=m_tB.Fit(BL, BR, CC);
  m_bFitOK=true;
  if (!m_bLOK || !m_bROK || !m_bTOK || !m_bBOK)
    throw "BAD 3DFit";
  };

//---------------------------------------------------------------------------

bool CSurface3DPanel::Fit(CSurface3D *p3D, CSurface3DFrame *pFrame,
                         long nX, double XMin, double XMax, long nY, double YMin, double YMax)
  {
  m_pFrame=pFrame;

  CSurface3DPoint BL(0.0, 0.0, (p3D->m_fnZ(XMin, YMin)));
  CSurface3DPoint BR(1.0, 0.0, (p3D->m_fnZ(XMax, YMin)));
  CSurface3DPoint TL(0.0, 1.0, (p3D->m_fnZ(XMin, YMax)));
  CSurface3DPoint TR(1.0, 1.0, (p3D->m_fnZ(XMax, YMax)));
  CSurface3DPoint CC(0.5, 0.5, (p3D->m_fnZ(0.5*(XMin+XMax), 0.5*(YMin+YMax))));
  m_bLOK=m_tL.Fit(BL, TL, CC);
  m_bROK=m_tR.Fit(BR, TR, CC);
  m_bTOK=m_tT.Fit(TL, TR, CC);
  m_bBOK=m_tB.Fit(BL, BR, CC);
  m_bFitOK=m_bLOK && m_bROK && m_bTOK && m_bBOK;
  return m_bFitOK;
  };
//---------------------------------------------------------------------------

bool CSurface3DPanel::CheckFit(CSurface3DFrame *pFrame)
  {
  const double Pos[][2]=
    {
      {0.50, 0.33},
      {0.50, 0.67},
      {0.33, 0.50},
      {0.67, 0.50},
      {-1,0}
    };

  double err=0.0;
  for (int i=0; Pos[i][0]>=0.0; i++)
    {
    double Fx=Pos[i][0];
    double Fy=Pos[i][1];
    double X=pFrame->Map2X(Fx);
    double Y=pFrame->Map2Y(Fy);
    double FVal=ValueFxFy(Fx, Fy);
    double Val=pFrame->m_p3D->m_fnZ(X, Y);
    double e=FracErr(FVal, Val);
    err=Max(err, e);

    #if dbgSurfaces
    if (dbgInitialise())
      dbgpln("e:%18.9f %18.9f = %18.9f\n"
             "  %18.9f %18.9f = %18.9f [%18.9f]: %12.4e %12.4e", 
              X,Y,Val,Fx,Fy,FVal,FVal-Val,e, err);
    #endif
    }                                                                    
  return err<pFrame->m_p3D->m_FitTolerance;//1.0e-4;
  };

//---------------------------------------------------------------------------

double CSurface3DPanel::ValueFxFy(double Fx, double Fy)
  {
  return (Fx+Fy>1.0) ? (Fx>Fy ? m_tR(Fx,Fy):m_tT(Fx,Fy)) : (Fx>Fy ? m_tB(Fx, Fy):m_tL(Fx,Fy));
  }

//===========================================================================
//
//
//
//===========================================================================

long CSurface3DFrame::m_nGlblElementCnt = 0;
long CSurface3DFrame::m_nGlblFrameCnt = 0;

CSurface3DFrame::CSurface3DFrame()
  {
  m_eDivision=C3D_DivNone;
  m_p3D=NULL;
  m_iLevel=-1;
  m_bInitDone=false;

  m_nDivsX=0;
  m_dXMin=0;
  m_dXMax=1;
  m_dDivWidthX=1;
    
  m_nDivsY=0;
  m_dYMin=0;
  m_dYMax=1;
  m_dDivWidthY=1;

  m_Frames=NULL;
  //m_nGlblElementCnt++;
  m_nGlblFrameCnt++;
  };

//---------------------------------------------------------------------------

CSurface3DFrame::~CSurface3DFrame()
  {
  long nElements=m_nDivsX*m_nDivsY;
  if (m_Frames)
    {
    for (long i=0; i<nElements; i++)
      {
      delete m_Frames[i];
      m_nGlblElementCnt--;
      m_p3D->m_nFrameCnt--;
      }
    delete m_Frames;
    }
  m_nGlblFrameCnt--;
  };

//---------------------------------------------------------------------------

CSurface3DFrame::CSurface3DFrame(CSurface3D * p3D, long Level,
                                double MinX, double MaxX, long NDivsX, 
                                double MinY, double MaxY, long NDivsY)
  {
  m_bInitDone=true;
  
  m_p3D=p3D;
  m_iLevel=Level;
  m_eDivision=C3D_DivNone;

  m_nDivsX=NDivsX;
  m_dXMin=MinX;
  m_dXMax=MaxX;
  m_dDivWidthX=1;
    
  m_nDivsY=NDivsY;
  m_dYMin=MinY;
  m_dYMax=MaxY;
  m_dDivWidthY=1;

  m_dDivWidthX=(m_dXMax-m_dXMin)/NDivsX; 
  m_dDivWidthY=(m_dYMax-m_dYMin)/NDivsY; 

  m_Frames=NULL;
  m_pPanel=NULL;
  #if dbgSurfaces
  if (dbgInitialise())
    dbgpln("I:%18.9f > %18.9f | %18.9f > %18.9f %s[Lvl:%2i] [Cnt:%6i Calls:%8i]", 
            MinX,MaxX,MinY,MaxY, (LPCTSTR)m_p3D->m_sName, m_iLevel, m_p3D->m_nFrameCnt, m_p3D->m_nCalls);
  #endif

  if (m_p3D)
    {
    m_nGlblElementCnt++;
    m_p3D->m_nFrameCnt++;
    }
  m_nGlblFrameCnt++;
  };

//---------------------------------------------------------------------------

void CSurface3DFrame::Initialise(CSurface3D * p3D, long Level,
                                double MinX, double MaxX, long NDivsX, 
                                double MinY, double MaxY, long NDivsY)
  {
  m_bInitDone=true;
  
  m_p3D=p3D;
  m_iLevel=Level;
  m_eDivision=C3D_DivNone;

  m_nDivsX=NDivsX;
  m_dXMin=MinX;
  m_dXMax=MaxX;
  m_dDivWidthX=1;
    
  m_nDivsY=NDivsY;
  m_dYMin=MinY;
  m_dYMax=MaxY;
  m_dDivWidthY=1;

  m_dDivWidthX=(m_dXMax-m_dXMin)/NDivsX; 
  m_dDivWidthY=(m_dYMax-m_dYMin)/NDivsY; 

  m_Frames=NULL;
  m_pPanel=NULL;
  #if dbgSurfaces
  if (dbgInitialise())
    dbgpln("I:%18.9f > %18.9f | %18.9f > %18.9f %s[Lvl:%2i] [Cnt:%6i Calls:%8i]", 
            MinX,MaxX,MinY,MaxY, (LPCTSTR)m_p3D->m_sName, m_iLevel, m_p3D->m_nFrameCnt, m_p3D->m_nCalls);
  #endif
  };

//---------------------------------------------------------------------------

double CSurface3DFrame::ValueXY(double X, double Y)
  {
  double Fx=X2MapF(X);
  double Fy=Y2MapF(Y);
  return m_pPanel->ValueFxFy(Fx, Fy);
  }

//---------------------------------------------------------------------------
  
CSurface3DPanel * CSurface3DFrame::BuildPanel()
  {
  m_p3D->m_nBuildCnt++;
  return new CSurface3DPanel(m_p3D, this, m_nDivsX, m_dXMin, m_dXMax, m_nDivsY, m_dYMin, m_dYMax);
  }

//---------------------------------------------------------------------------

bool CSurface3DFrame ::FitAndTestPanels(long NX, long NY, CSurface3DFrame Frames[])
  {
  bool OK=true;
  CSurface3DPanel Panel[4];
  for (int i=0; OK && (i<NX*NY); i++)
    {
    Panel[i].Fit(m_p3D, &Frames[i], NX, Frames[i].m_dXMin, Frames[i].m_dXMax, NY, Frames[i].m_dYMin, Frames[i].m_dYMax);
    if (!Panel[i].CheckFit(this))
      OK=false;
    };
  if (OK)
    {
    m_nDivsX=NX;
    m_nDivsY=NY;
    m_Frames=new LPCSurface3DFrame[3];
    for (int i=0; OK && (i<NX*NY); i++)
      {
      m_Frames[i]=new CSurface3DFrame(m_p3D, m_iLevel+1, 
                                      Frames[i].m_dXMin, Frames[i].m_dXMax, Frames[i].m_nDivsX, 
                                      Frames[i].m_dYMin, Frames[i].m_dYMax, Frames[i].m_nDivsY);
      m_Frames[i]->m_eDivision=C3D_Div1x1;
      m_nGlblElementCnt++;
      m_p3D->m_nFrameCnt++;
      };
    }
  return OK;
  }

//---------------------------------------------------------------------------

struct PanelLayoutItem
  {
  byte Divs, NX, NY;
  };
static PanelLayoutItem PanelLayout[] =
  //          B T R L 
  {  
    { C3D_Div1x1 , 1,1}, // 0 0 0 0   = 0
    { C3D_Div2H  , 2,1}, // 0 0 0 1   = 1
    { C3D_Div2H  , 2,1}, // 0 0 1 0   = 2
    { C3D_Div3H  , 3,1}, // 0 0 1 1   = 3
    { C3D_Div2V  , 1,2}, // 0 1 0 0   = 4
    { C3D_Div2x2 , 2,2}, // 0 1 0 1   = 5
    { C3D_Div2x2 , 2,2}, // 0 1 1 0   = 6
    { C3D_Bad    , 0,0}, // 0 1 1 1   = 7
    { C3D_Div2V  , 1,2}, // 1 0 0 0   = 8
    { C3D_Div2x2 , 2,2}, // 1 0 0 1   = 9
    { C3D_Div2x2 , 2,2}, // 1 0 1 0   = 10
    { C3D_Bad    , 0,0}, // 1 0 1 1   = 11
    { C3D_Div3V  , 1,3}, // 1 1 0 0   = 12
    { C3D_Bad    , 0,0}, // 1 1 0 1   = 13
    { C3D_Bad    , 0,0}, // 1 1 1 0   = 14
    { C3D_Bad    , 0,0}, // 1 1 1 1   = 15
  };

static char * s_sDivText[]=
  {
  "None",
  "1x1",
  "2x2",
  "2H",
  "3H",
  "2V",
  "3V",
  "NxN",
  "Other",
  "Bad"
  };

byte CSurface3DFrame::FitPanel()//Rectangle()
  {
  byte eTryDiv=C3D_DivNxN;
  if (m_p3D->m_pBoundary)
    {
    CSurface3DBoundary &B=*m_p3D->m_pBoundary;
    if (m_dXMin>=B.m_XMin && m_dXMax<=B.m_XMax &&
        m_dYMin>=B.m_YMin && m_dYMax<=B.m_YMax)
      {
      double BYxmn=B.m_FnYx(m_dXMin);
      double BYxmx=B.m_FnYx(m_dXMax);
      double BXymn=B.m_FnXy(m_dYMin);
      double BXymx=B.m_FnXy(m_dYMax);
      byte LCross=(m_dYMin<BYxmn && BYxmn<m_dYMax) ? 1 : 0;
      byte RCross=(m_dYMin<BYxmx && BYxmx<m_dYMax) ? 2 : 0;
      byte BCross=(m_dXMin<BXymn && BXymn<m_dXMax) ? 4 : 0;
      byte TCross=(m_dXMin<BXymx && BXymx<m_dXMax) ? 8 : 0;
      eTryDiv=PanelLayout[LCross+RCross+TCross+BCross].Divs;

      double Val1=dNAN;
      double Val2=dNAN;
      switch (eTryDiv)
        {
        case C3D_Div2V:
          {
          Val1=BCross ? BXymn : BXymx;
          CSurface3DFrame Frames[2];
          Frames[0].Initialise(m_p3D, m_iLevel+1, m_dXMin, Val1,    1, m_dYMin, m_dYMax, 1);
          Frames[1].Initialise(m_p3D, m_iLevel+1, Val1,    m_dXMax, 1, m_dYMin, m_dYMax, 1);
          if (!FitAndTestPanels(2,1, Frames))
            eTryDiv=C3D_DivNxN;
          }
          break;
        case C3D_Div3V:
          {
          Val1=Min(BXymn, BXymx);
          Val2=Max(BXymn, BXymx);
          CSurface3DFrame Frames[3];
          Frames[0].Initialise(m_p3D, m_iLevel+1, m_dXMin, Val1,    1, m_dYMin, m_dYMax, 1);
          Frames[1].Initialise(m_p3D, m_iLevel+1, Val1,    Val2,    1, m_dYMin, m_dYMax, 1);
          Frames[2].Initialise(m_p3D, m_iLevel+1, Val2,    m_dXMax, 1, m_dYMin, m_dYMax, 1);
          if (!FitAndTestPanels(3,1, Frames))
            eTryDiv=C3D_DivNxN;
          }
          break;
        case C3D_Div2H:
          {
          Val1=LCross ? BYxmn : BYxmx;
          CSurface3DFrame Frames[2];
          Frames[0].Initialise(m_p3D, m_iLevel+1, m_dXMin, m_dXMax, 1, m_dYMin,   Val1, 1);
          Frames[1].Initialise(m_p3D, m_iLevel+1, m_dXMin, m_dXMax, 1, Val1,   m_dYMax, 1);
          if (!FitAndTestPanels(1,2, Frames))
            eTryDiv=C3D_DivNxN;
          }
          break;
        case C3D_Div3H:
          {
          Val1=Min(BYxmn, BYxmx);
          Val2=Max(BYxmn, BYxmx);
          CSurface3DFrame Frames[3];
          Frames[0].Initialise(m_p3D, m_iLevel+1, m_dXMin, m_dXMax, 1, m_dYMin,   Val1, 1);
          Frames[1].Initialise(m_p3D, m_iLevel+1, m_dXMin, m_dXMax, 1, Val1,      Val2, 1);
          Frames[2].Initialise(m_p3D, m_iLevel+1, m_dXMin, m_dXMax, 1, Val2,   m_dYMax, 1);
          if (!FitAndTestPanels(1,3, Frames))
            eTryDiv=C3D_DivNxN;
          }
          break;
        case C3D_Div2x2:
          {
          Val1=BCross ? BXymn : BXymx;
          Val2=LCross ? BYxmn : BYxmx;
          CSurface3DFrame Frames[4];
          Frames[0].Initialise(m_p3D, m_iLevel+1, m_dXMin,    Val1, 1, m_dYMin,    Val2, 1);
          Frames[1].Initialise(m_p3D, m_iLevel+1, Val1,    m_dXMax, 1, m_dYMin,    Val2, 1);
          Frames[2].Initialise(m_p3D, m_iLevel+1, m_dXMin,    Val1, 1, Val2,    m_dYMax, 1);
          Frames[3].Initialise(m_p3D, m_iLevel+1, Val1,    m_dXMax, 1, Val2,    m_dYMax, 1);
          if (!FitAndTestPanels(2,2, Frames))
            eTryDiv=C3D_DivNxN;
          }
          break;
        case C3D_DivNxN:
          if (m_nDivsX*m_nDivsY<2)
            DoBreak();
          break;
        case C3D_Bad:
          DoBreak();
        }
      }
    else
      eTryDiv=C3D_Div1x1;
    }
  else
    eTryDiv=C3D_Div1x1;
  
  if (eTryDiv==C3D_Div1x1)
    {
    CSurface3DPanel * pPanel=BuildPanel();
    
    if (m_iLevel>=8)
      DoBreak();

    if (pPanel->CheckFit(this))
      {
      m_eDivision=eTryDiv;
      m_pPanel=pPanel;
      #if dbgSurfaces
      if (dbg3DTrack())
        dbgpln("Add new Panel %-20s %-5s %08x [%2i] %14.8f %14.8f",
               m_p3D->Name(), s_sDivText[m_eDivision], m_pPanel, m_iLevel, 0.5*(m_dXMin+m_dXMax), 0.5*(m_dYMin+m_dYMax));
      #endif
      }
    else
      {
      delete pPanel;
      eTryDiv= C3D_DivNxN;
      #if dbgSurfaces
      if (dbg3DTrack())
        dbgpln("Drill Down    %-20s %-5s %8s [%2i] %14.8f %14.8f",
               m_p3D->Name(), s_sDivText[eTryDiv], "", m_iLevel, 0.5*(m_dXMin+m_dXMax), 0.5*(m_dYMin+m_dYMax));
      #endif
      }
    }
  
  if (eTryDiv==C3D_DivNxN)
    {
    if (m_nDivsX*m_nDivsY<2)
      DoBreak();
    ASSERT(m_Frames==NULL);
    long nElements=m_nDivsX*m_nDivsY;
    m_Frames=new LPCSurface3DFrame[nElements];
    for (int i=0; i<nElements; i++)
      m_Frames[i]= NULL;
    }
  m_eDivision=eTryDiv;
  return eTryDiv;
  }

//---------------------------------------------------------------------------

double CSurface3DFrame::DoOp(double X, double Y, LPCSurface3DFrame &pFrame)
  {
  if (m_iLevel>=8)
    DoBreak();

  if (m_eDivision==C3D_DivNone)
    m_eDivision=FitPanel();

  switch (m_eDivision)
    {
    case C3D_DivNxN:
      {
      if (m_nDivsX*m_nDivsY<2)
        DoBreak();
      long Ix=X2MapI(X);
      long Iy=Y2MapI(Y);
      long i=Index(Ix,Iy);
      CSurface3DFrame *&pE=m_Frames[i];
      if (pE==NULL)
        {
        pE = new CSurface3DFrame(m_p3D, m_iLevel+1,
                                 m_dXMin+Ix*m_dDivWidthX, m_dXMin+(Ix+1)*m_dDivWidthX, m_nDivsX,
                                 m_dYMin+Iy*m_dDivWidthY, m_dYMin+(Iy+1)*m_dDivWidthY, m_nDivsY);
        m_nGlblElementCnt++;
        m_p3D->m_nFrameCnt++;
        }
      
      ASSERT(pE->m_bInitDone);
      return pE->DoOp(X,Y, pFrame);
      }
    case C3D_Div2V:
      {
      if (X<=m_Frames[0]->m_dXMax)
        return m_Frames[0]->DoOp(X,Y, pFrame);
      else
        return m_Frames[1]->DoOp(X,Y, pFrame);
      }
    case C3D_Div3V:
      {
      if (X<=m_Frames[0]->m_dXMax)
        return m_Frames[0]->DoOp(X,Y, pFrame);
      else if (X>=m_Frames[2]->m_dXMin)
        return m_Frames[2]->DoOp(X,Y, pFrame);
      else
        return m_Frames[1]->DoOp(X,Y, pFrame);
      }
    case C3D_Div2H:
      {
      if (Y<=m_Frames[0]->m_dYMax)
        return m_Frames[0]->DoOp(X,Y, pFrame);
      else
        return m_Frames[1]->DoOp(X,Y, pFrame);
      }
    case C3D_Div3H:
      {
      if (Y<=m_Frames[0]->m_dYMax)
        return m_Frames[0]->DoOp(X,Y, pFrame);
      else if (Y>=m_Frames[2]->m_dYMin)
        return m_Frames[2]->DoOp(X,Y, pFrame);
      else
        return m_Frames[1]->DoOp(X,Y, pFrame);
      }
    case C3D_Div2x2:
      {
      if (X<=m_Frames[0]->m_dXMax)
        {
        if (Y<=m_Frames[0]->m_dYMax)
          return m_Frames[0]->DoOp(X,Y, pFrame);
        else
          return m_Frames[2]->DoOp(X,Y, pFrame);
        }
      else 
        {
        if (Y<=m_Frames[1]->m_dYMax)
          return m_Frames[1]->DoOp(X,Y, pFrame);
        else
          return m_Frames[3]->DoOp(X,Y, pFrame);
        }
      }
    case C3D_Div1x1:
      {
      if (m_pPanel==NULL)
        {
        m_pPanel=BuildPanel();
        ASSERT(pPanel->CheckFit());
        #if dbgSurfaces
        if (dbg3DTrack())
          dbgpln("Replace Panel %-20s %-5s %08x [%2i] %14.8f %14.8f",
                 m_p3D->Name(), s_sDivText[m_eDivision], m_pPanel, m_iLevel, 0.5*(m_dXMin+m_dXMax), 0.5*(m_dYMin+m_dYMax));
        #endif
        }

      double Val=ValueXY(X,Y);
      //dbgpln("== %12.3f %12.3f = %12.3f %s", X,Y,Val, (LPCTSTR)m_3D.m_sName);
      pFrame=this;
      
      m_p3D->MRU_Move2Head(m_pPanel);
      if (m_p3D->m_nPanelCnt>=m_p3D->m_nPanelCntMax)
        {
        CSurface3DPanel *pLast=m_p3D->MRU_RemoveTail();
        if (pLast)
          {
          pLast->m_pFrame->m_pPanel=NULL;
          #if dbgSurfaces
          if (dbg3DTrack())
            dbgpln("Remove  Panel %-20s %-5s %08x [%2i] %14.8f %14.8f",
                   m_p3D->Name(), s_sDivText[m_eDivision], pLast, m_iLevel,
                   0.5*(pLast->m_pFrame->m_dXMin+pLast->m_pFrame->m_dXMax), 
                   0.5*(pLast->m_pFrame->m_dYMin+pLast->m_pFrame->m_dYMax));
          #endif
          delete pLast;
          }
        }
      #if dbgSurfaces
      if (dbg3DState())
        if (/*m_p3D->m_nFrameCnt%ALLOCDBGCNT==0 ||*/ (GetTickCount()-m_p3D->m_dwLastTicks>60000))
          {
          m_p3D->m_dwLastTicks=GetTickCount();
          dbgpln("Surface:%-20s has Frames=%7i[%5i] Panels=%7i[%5i] Calls=%10i Builds=%10i", 
                  m_p3D->Name(), 
                  m_p3D->m_nFrameCnt,m_p3D->m_nFrameCnt*sizeof(CSurface3DFrame)/1024, 
                  m_p3D->m_nPanelCnt,m_p3D->m_nPanelCnt*sizeof(CSurface3DPanel)/1024, 
                  m_p3D->m_nCalls, m_p3D->m_nBuildCnt);
          }
      #endif
      return Val;
      }
    default:
      DoBreak();
      return dNAN;
    }
  }

//---------------------------------------------------------------------------

double CSurface3DFrame::operator()(double X, double Y)
  {
  CSingleLock Lock(&m_p3D->m_TheLock, true);
  m_p3D->m_nCalls++;
  CSurface3DFrame *pFrame;
  double Val=DoOp(X, Y, pFrame);
  if (CSurfaceCommon::m_DoSmoothing)
    {
    double dx=pFrame->m_dDivWidthX*pFrame->m_nDivsX/6;
    double dy=pFrame->m_dDivWidthY*pFrame->m_nDivsY/6;
    double x1=X-dx;
    double x2=X+dx;
    double y1=Y-dy;
    double y2=Y+dy;
    CSurface3DFrame *pElement1, *pElement2, *pElement3, *pElement4;
    double Val1=DoOp(X-dx, Y-dy, pElement1);
    double Val2=DoOp(X-dx, Y+dy, pElement2);
    double Val3=DoOp(X+dx, Y+dy, pElement3);
    double Val4=DoOp(X+dx, Y-dy, pElement4);
    Val=0.25*(Val1+Val2+Val3+Val4);
//    if (pElement2->m_iLevel!=pElement1->m_iLevel ||
//        pElement3->m_iLevel!=pElement1->m_iLevel ||
//        pElement4->m_iLevel!=pElement1->m_iLevel)
//      { 
//      int x=0; 
//      dbgpln("Smooth Level Mismatch %i %i %i %i %i",
//        pFrame->m_iLevel, pElement1->m_iLevel, pElement2->m_iLevel, pElement3->m_iLevel, pElement4->m_iLevel);
//      }
//    Val=0.25*(Val1+Val2+Val3+Val4);
    int xxx=0; 
    }
  return Val;
  }

//===========================================================================
//
//
//
//===========================================================================

CSurface3D::CSurface3D()
  {
  m_nCalls=0;
  m_fnZ=NULL;
  m_nFrameCnt=0;
  m_nBuildCnt=0;
  m_nPanelCnt=0;
  m_nPanelCntMax=ALLOCCNTMAX;
  m_pMRUHead=NULL;
  m_pMRUTail=NULL;
  m_dwLastTicks=GetTickCount();
  }

//---------------------------------------------------------------------------

CSurface3D::CSurface3D(LPCTSTR Name, 
                        double MinX, double MaxX, long NDivsX, TSurfaceXfFn fnXI, TSurfaceXfFn fnXO, 
                        double MinY, double MaxY, long NDivsY, TSurfaceXfFn fnYI, TSurfaceXfFn fnYO, 
                        TSurface3DFn Fn,
                        CSurface3DBoundary *pBoundary) :
  m_E(this, 0, MinX, MaxX, NDivsX, MinY, MaxY, NDivsY)
  {
  m_sName=Name; 

  m_nCalls=0;
  m_nFrameCnt=0;
  m_nBuildCnt=0;
  m_nPanelCnt=0;
  m_nPanelCntMax=ALLOCCNTMAX;
  m_pMRUHead=NULL;
  m_pMRUTail=NULL;


  m_fnZ=Fn;
  m_pBoundary=pBoundary;
  //RemoveElements();
  m_dwLastTicks=GetTickCount();
  AddToList();
  };

//---------------------------------------------------------------------------

CSurface3D::~CSurface3D()
  {
  };

//---------------------------------------------------------------------------
    
void CSurface3D::Dump()
  {
/*
  char DoItX[4096];
  for (int ix=0; ix<=m_nDivsX; ix++)
    {
    DoItX[ix]=0;
    for (int iy=0; iy<=m_nDivsY; iy++)
      {
      if (m_Frames[Index(Min(ix, m_nDivsX-1), Min(iy, m_nDivsY-1))])
        {
          DoItX[ix]=1;
          break;
          }
        }
    }
 
  dbgpln("Surface:%s", Name());
  dbgpln("Usage  : %10.5f%%  [%i of %i]", Usage()*100, m_nElementsInited, m_nElements);
  dbgpln("Worst  : %10.5f%%  @%i,%i = %g,%g", 100*m_WorstErrorV, m_WorstErrorX,m_WorstErrorY, Map2X(m_WorstErrorX, 0.5), Map2Y(m_WorstErrorY, 0.5));

  dbgp("\t\t");
  for (ix=0; ix<=m_nDivsX; ix++)
    {
    if (DoItX[ix])
      {
      for(int jx=0; jx<2; jx++)
        {
        double dx=jx*0.5;
        double x1=Map2X(ix, dx);
        dbgp("%g\t",x1);
        }
      }
    }
  dbgpln("");

    
  for (int iy=0; iy<=m_nDivsY; iy++)
    {
    for(int jy=0; jy<2; jy++)
      {
      int DoIt=0;
      for (int ix=0; ix<=m_nDivsX; ix++)
        {
        if (m_Elements[Index(Min(ix, m_nDivsX-1), Min(iy, m_nDivsY-1))])
          {
          DoIt=1;
          break;
          }
        }

      if (!DoIt)
        continue;

      double dy=jy*0.5;
      double y1=Map2Y(iy, dy);
      dbgp("%5.1f:\t%15.8f\t", iy+dy,y1);
      for (ix=0; ix<=m_nDivsX; ix++)
        {
        if (DoItX[ix])
          {
          CSurface3DFrame *E=m_Elements[Index(Min(ix, m_nDivsX-1), Min(iy, m_nDivsY-1))];
          if (E)
            {
            for(int jx=0; jx<2; jx++)
              {
              double dx=jx*0.5;
              dbgp("%g\t", m_fnZO((*E)(dx,dy)));
              }
            }
          else
            {
            for(int jx=0; jx<2; jx++)
              dbgp("\t");
            }
          }
        }
      dbgpln("");
      }
    }
*/
  };

CSurface3DPanel *CSurface3D::MRU_RemoveTail()
  {
  CSurface3DPanel *T=m_pMRUTail;
  #if dbgSurfaces
  if (dbg3DMRU())
    dbgpln("MRU_RemoveTail:%-20s %08x %08x H:%08x T:%08x", Name(), T, T->m_pFrame, m_pMRUHead, m_pMRUTail);
  #endif
  if (m_pMRUTail)
    {
    m_nPanelCnt--;
    if (m_pMRUTail==m_pMRUHead)
      {
      m_pMRUHead=NULL;
      m_pMRUTail=NULL;
      }
    else
      {
      if (m_pMRUTail->m_pMRUNext)
        m_pMRUTail->m_pMRUNext->m_pMRUPrev=NULL;
      m_pMRUTail=m_pMRUTail->m_pMRUNext;
      }
    }
  #if dbgSurfaces
  if (dbg3DMRU())
    dbgpln("               %-20s                   H:%08x T:%08x", "", m_pMRUHead, m_pMRUTail);
  #endif
  return T;
  };

void CSurface3D::MRU_Move2Head(CSurface3DPanel * E)
  {
  #if dbgSurfaces
  if (dbg3DMRU())
    dbgpln("MRU_Move2Head :%-20s %08x %08x H:%08x T:%08x", Name(), E, E->m_pFrame, m_pMRUHead, m_pMRUTail);
  //m_MRUList.AddHead(E);
  #endif

  if (m_pMRUHead!=E)
    {
    MRU_Remove(E);
    if (m_pMRUHead)
      m_pMRUHead->m_pMRUNext=E;
    E->m_pMRUNext=NULL;
    E->m_pMRUPrev=m_pMRUHead;
    m_pMRUHead=E;
    if (m_pMRUTail==NULL)
      m_pMRUTail=E;
    m_nPanelCnt++;
    }
  #if dbgSurfaces
  if (dbg3DMRU())
    dbgpln("               %-20s                   H:%08x T:%08x", "", m_pMRUHead, m_pMRUTail);
  #endif
  };

void CSurface3D::MRU_Remove(CSurface3DPanel * E)
  {
  #if dbgSurfaces
  if (dbg3DMRU())
    dbgpln("MRU_Remove    :%-20s %08x %08x H:%08x T:%08x", Name(), E, E->m_pFrame, m_pMRUHead, m_pMRUTail);
  #endif
  if (E==m_pMRUTail)
    {
    m_nPanelCnt--;
    if (E==m_pMRUHead)
      {
      m_pMRUHead=NULL;
      m_pMRUTail=NULL;
      }
    else
      {
      if (m_pMRUTail->m_pMRUNext)
        m_pMRUTail->m_pMRUNext->m_pMRUPrev=NULL;
      m_pMRUTail=m_pMRUTail->m_pMRUNext;
      }
    }
  else if (E==m_pMRUHead)
    {
    m_nPanelCnt--;
    if (m_pMRUHead->m_pMRUPrev)
      m_pMRUHead->m_pMRUPrev->m_pMRUNext=NULL;
    m_pMRUHead=m_pMRUHead->m_pMRUPrev;
    }
  else 
    {
    CSurface3DPanel * P=E->m_pMRUPrev;
    CSurface3DPanel * N=E->m_pMRUNext;
    if (P)
      {
      ASSERT(N!=NULL);
      m_nPanelCnt--;
      if (P)
        P->m_pMRUNext=N;
      if (N)
        N->m_pMRUPrev=P;
      }
    }
  #if dbgSurfaces
  if (dbg3DMRU())
    dbgpln("               %-20s                   H:%08x T:%08x", "", m_pMRUHead, m_pMRUTail);
  #endif
  }

#else //UseNewSurfaces

long CSurface3DElement::m_nGlblElementCnt = 0;

CSurface3DElement::CSurface3DElement(CSurface3D & r3D, CSurface3DElement *pParent, long lIndex, long Level):
  m_r3D(r3D)
  {
  m_iLevel=Level;
  m_bInitDone=false;
  m_bFitDone=false;
  m_bFitOK=false;

  m_nDivsX=1;
  m_dMinimumX=0;
  m_dMaximumX=1;
  m_dDivWidthX=1;
    
  m_nDivsY=1;
  m_dMinimumY=0;
  m_dMaximumY=1;
  m_dDivWidthY=1;

  m_pParent=pParent;
  m_lIndex=lIndex; 

  m_pMRUNext=NULL;
  m_pMRUPrev=NULL;
  //m_nElements=0;
  m_Elements=NULL;
  //m_nElementsInited=0;
  m_nGlblElementCnt++;
  m_r3D.m_nElementCnt++;
  };

//---------------------------------------------------------------------------

CSurface3DElement::~CSurface3DElement()
  {
  m_nGlblElementCnt--;
  m_r3D.m_nElementCnt--;
  RemoveElements();
  };

//---------------------------------------------------------------------------

bool CSurface3DElement::Initialise(double MinX, double MaxX, long NDivsX, 
                                   double MinY, double MaxY, long NDivsY)
  {
  m_bInitDone=true;
  m_nDivsX=NDivsX;
  m_dMinimumX=MinX;
  m_dMaximumX=MaxX;
  m_dDivWidthX=1;
    
  m_nDivsY=NDivsY;
  m_dMinimumY=MinY;
  m_dMaximumY=MaxY;
  m_dDivWidthY=1;

  m_dDivWidthX=(m_dMaximumX-m_dMinimumX)/NDivsX; 
  m_dDivWidthY=(m_dMaximumY-m_dMinimumY)/NDivsY; 

  m_bFitDone=false;
  m_bFitOK=false;
  m_bLOK=false;
  m_bROK=false;
  m_bTOK=false;
  m_bBOK=false;

  #if dbgSurfaces
  if (dbgInitialise())
    dbgpln("I:%18.9f > %18.9f | %18.9f > %18.9f %s[Lvl:%2i] [Cnt:%6i Calls:%8i]", 
            MinX,MaxX,MinY,MaxY, (LPCTSTR)m_r3D.m_sName, m_iLevel, m_r3D.m_nElementCnt, m_r3D.m_nCalls);
  #endif

  RemoveElements();
  return true;
  };

//---------------------------------------------------------------------------

void CSurface3DElement::InitElements()
  {
  //m_nElementsInited=0;
  long nElements=m_nDivsX*m_nDivsY;
  typedef CSurface3DElement*  CSurface3DElementPtr;
  m_Elements=new CSurface3DElementPtr[nElements];
  for (int i=0; i<nElements; i++)
    m_Elements[i]=NULL;
  m_r3D.m_nElementPtrCnt+=nElements;
  m_r3D.MRU_Remove(this);
  };

//---------------------------------------------------------------------------

void CSurface3DElement::RemoveElements()
  {
  long nElements=m_nDivsX*m_nDivsY;
  if (m_Elements)
    {
    for (int i=0; i<nElements; i++)
      delete m_Elements[i];
    m_r3D.m_nElementPtrCnt-=nElements;
    }
  delete m_Elements;
  m_Elements=NULL;
  };

//---------------------------------------------------------------------------

flag CSurface3DElement::Fit(long nX, long nY, CSurface3DPoint &BL, CSurface3DPoint &BR, CSurface3DPoint &TL, CSurface3DPoint &TR, CSurface3DPoint &CC)
  {
  m_bLOK=m_tL.Fit(BL, TL, CC);
  m_bROK=m_tR.Fit(BR, TR, CC);
  m_bTOK=m_tT.Fit(TL, TR, CC);
  m_bBOK=m_tB.Fit(BL, BR, CC);
  m_bFitDone=true;
  m_bFitOK=true;
  if (!m_bLOK || !m_bROK || !m_bTOK || !m_bBOK)
    throw "BAD 3DFit";
  return true;
  };

//---------------------------------------------------------------------------

CSurface3DElement & CSurface3DElement::GetElement(long Ix, long Iy)
  {
  long i=Index(Ix, Iy);
  CSurface3DElement *E=m_Elements[i];
  if (E==NULL)
    { 
    m_r3D.m_nElementAllocs++;
    E=new CSurface3DElement(m_r3D, this, i, m_iLevel+1);
    m_Elements[i]=E;
    if (m_r3D.m_nMRULen>=m_r3D.m_nMRULenMax)
      {
      CSurface3DElement *pLast=m_r3D.MRU_RemoveTail();
      if (pLast)
        {
        pLast->m_pParent->m_Elements[pLast->m_lIndex]=NULL;
        delete pLast;
        }
      }
    else
      {
      m_r3D.m_nMRULen++;
      }
    if (m_r3D.m_nElementAllocs%ALLOCDBGCNT==0 || (GetTickCount()-m_r3D.m_dwLastTicks>60000))
      {
      m_r3D.m_dwLastTicks=GetTickCount();
      dbgpln("Surface:%20s has %7i elements %7i allocs %5i MRULen", m_r3D.Name(), m_r3D.m_nElementCnt, m_r3D.m_nElementAllocs, m_r3D.m_nMRULen);
      }
    }
  if (E->m_Elements==NULL)
    m_r3D.MRU_Move2Head(E);
  return *E;
  }

//---------------------------------------------------------------------------

double CSurface3DElement::ValueXY(double X, double Y)
  {
  double Fx=X2MapF(X);
  double Fy=Y2MapF(Y);
  // QuickFix ?;
  //Fx=Range(0.0, Fx, 1.0);
  //Fy=Range(0.0, Fy, 1.0);
  return (Fx+Fy>1.0) ? (Fx>Fy ? m_tR(Fx,Fy):m_tT(Fx,Fy)) : (Fx>Fy ? m_tB(Fx, Fy):m_tL(Fx,Fy));
  }
double CSurface3DElement::ValueFxFy(double Fx, double Fy)
  {
  return (Fx+Fy>1.0) ? (Fx>Fy ? m_tR(Fx,Fy):m_tT(Fx,Fy)) : (Fx>Fy ? m_tB(Fx, Fy):m_tL(Fx,Fy));
  }

double CSurface3DElement::operator()(double X, double Y)
  {
  CSingleLock Lock(&m_r3D.m_TheLock, true);
  m_r3D.m_nCalls++;
  CSurface3DElement *pElement;
  double Val=DoOp(X, Y, pElement);
  if (CSurfaceCommon::m_DoSmoothing)
    {
    double dx=pElement->m_dDivWidthX*pElement->m_nDivsX/6;
    double dy=pElement->m_dDivWidthY*pElement->m_nDivsY/6;
    double x1=X-dx;
    double x2=X+dx;
    double y1=Y-dy;
    double y2=Y+dy;
    CSurface3DElement *pElement1, *pElement2, *pElement3, *pElement4;
    double Val1=DoOp(X-dx, Y-dy, pElement1);
    double Val2=DoOp(X-dx, Y+dy, pElement2);
    double Val3=DoOp(X+dx, Y+dy, pElement3);
    double Val4=DoOp(X+dx, Y-dy, pElement4);
    Val=0.25*(Val1+Val2+Val3+Val4);
//    if (pElement2->m_iLevel!=pElement1->m_iLevel ||
//        pElement3->m_iLevel!=pElement1->m_iLevel ||
//        pElement4->m_iLevel!=pElement1->m_iLevel)
//      { 
//      int x=0; 
//      dbgpln("Smooth Level Mismatch %i %i %i %i %i",
//        pElement->m_iLevel, pElement1->m_iLevel, pElement2->m_iLevel, pElement3->m_iLevel, pElement4->m_iLevel);
//      }
//    Val=0.25*(Val1+Val2+Val3+Val4);
    int xxx=0; 
    }
  return Val;
  }

double CSurface3DElement::DX(double X, double Y)
  {
  //CSingleLock Lock(&m_r3D.m_TheLock, true);
  //m_r3D.m_nCalls++;
  //CSurface3DElement *pElement;
  //double Val=DoOp(X, Y, pElement);
  return dNAN;//0Val;
  }

double CSurface3DElement::DY(double X, double Y)
  {
  //CSingleLock Lock(&m_r3D.m_TheLock, true);
  //m_r3D.m_nCalls++;
  //CSurface3DElement *pElement;
  //double Val=DoOp(X, Y, pElement);
  return dNAN;//0Val;
  }

double CSurface3DElement::DoOp(double X, double Y, LPCSurface3DElement &pElement)
  {
  if (!m_bFitDone)
    {
    long Ix=0;
    long Iy=0;
    CSurface3DPoint BL(0.0, 0.0, (m_r3D.m_fnZ(m_dMinimumX, m_dMinimumY)));
    CSurface3DPoint BR(1.0, 0.0, (m_r3D.m_fnZ(m_dMaximumX, m_dMinimumY)));
    CSurface3DPoint TL(0.0, 1.0, (m_r3D.m_fnZ(m_dMinimumX, m_dMaximumY)));
    CSurface3DPoint TR(1.0, 1.0, (m_r3D.m_fnZ(m_dMaximumX, m_dMaximumY)));
    CSurface3DPoint CC(0.5, 0.5, (m_r3D.m_fnZ(0.5*(m_dMinimumX+m_dMaximumX), 0.5*(m_dMinimumY+m_dMaximumY))));
    Fit(m_nDivsX, m_nDivsY, BL, BR, TL, TR, CC);
    m_bFitOK=CheckFit();
    }
  if (m_bFitOK)
    {
    double Val=ValueXY(X,Y);
    //dbgpln("== %12.3f %12.3f = %12.3f %s", X,Y,Val, (LPCTSTR)m_3D.m_sName);
    pElement=this;
    return Val;
    }

  if (m_Elements==NULL)
    {
    InitElements();    
    m_r3D.MRU_Remove(this);
    }

  long Ix=X2MapI(X);
  long Iy=Y2MapI(Y);
  CSurface3DElement &E=GetElement(Ix, Iy);
  if (!E.m_bInitDone)
    {
    E.Initialise(m_dMinimumX+Ix*m_dDivWidthX, m_dMinimumX+(Ix+1)*m_dDivWidthX, m_nDivsX,
                 m_dMinimumY+Iy*m_dDivWidthY, m_dMinimumY+(Iy+1)*m_dDivWidthY, m_nDivsY);
    }
  return E.DoOp(X,Y, pElement);
  }

//---------------------------------------------------------------------------

bool CSurface3DElement::CheckFit()
  {
  const double Pos[][2]=
    {
      {0.50, 0.33},
      {0.50, 0.67},
      {0.33, 0.50},
      {0.67, 0.50},
      {-1,0}
    };

  double err=0.0;
  for (int i=0; Pos[i][0]>=0.0; i++)
    {
    double Fx=Pos[i][0];
    double Fy=Pos[i][1];
    double X=Map2X(Fx);
    double Y=Map2Y(Fy);
    double FVal=ValueFxFy(Fx, Fy);
    double Val=m_r3D.m_fnZ(X, Y);
    double e=FracErr(FVal, Val);
    err=Max(err, e);

    #if dbgSurfaces
    if (dbgInitialise())
      dbgpln("e:%18.9f %18.9f = %18.9f\n"
             "  %18.9f %18.9f = %18.9f [%18.9f]: %12.4e %12.4e", 
              X,Y,Val,Fx,Fy,FVal,FVal-Val,e, err);
    #endif
    }                                                                    
  return err<m_r3D.m_FitTolerance;//1.0e-4;
  };

//===========================================================================
//
//
//
//===========================================================================

CSurface3D::CSurface3D():
  m_E(*this, NULL, -1, 0)
  {
  m_nCalls=0;
  m_fnZ=NULL;
  m_nElementPtrCnt=0;
  m_nMRULen=0;
  m_nMRULenMax=ALLOCCNTMAX;
  m_nElementCnt=0;
  m_nElementAllocs=0;
  m_pMRUHead=NULL;
  m_pMRUTail=NULL;
  m_dwLastTicks=GetTickCount();
  }

//---------------------------------------------------------------------------

CSurface3D::CSurface3D(LPCTSTR Name, 
                        double MinX, double MaxX, long NDivsX, TSurfaceXfFn fnXI, TSurfaceXfFn fnXO, 
                        double MinY, double MaxY, long NDivsY, TSurfaceXfFn fnYI, TSurfaceXfFn fnYO, 
                        TSurface3DFn Fn,
                        CSurface3DBoundary *pBoundary) :
  m_E(*this, NULL, -1, 0)
  {
  m_sName=Name; 

  m_nCalls=0;
  m_nElementPtrCnt=0;
  m_nMRULen=0;
  m_nMRULenMax=ALLOCCNTMAX;
  m_nElementCnt=0;
  m_nElementAllocs=0;
  m_pMRUHead=NULL;
  m_pMRUTail=NULL;

  m_E.Initialise(MinX, MaxX, NDivsX, MinY, MaxY, NDivsY);

  m_fnZ=Fn;
  RemoveElements();
  m_dwLastTicks=GetTickCount();
  };

//---------------------------------------------------------------------------

CSurface3D::~CSurface3D()
  {
  RemoveElements();
  };

//---------------------------------------------------------------------------

void CSurface3D::InitElements()
  {
  AddToList();
  };

//---------------------------------------------------------------------------

void CSurface3D::RemoveElements()
  {
  m_E.RemoveElements();
  };

//---------------------------------------------------------------------------

double CSurface3D::DX(double X, double Y)
  {
  return m_E.DX(X, Y);
  };
double CSurface3D::DY(double X, double Y)
  {
  return m_E.DY(X, Y);
  };

//---------------------------------------------------------------------------
    
void CSurface3D::Dump()
  {
/*
  char DoItX[4096];
  for (int ix=0; ix<=m_nDivsX; ix++)
    {
    DoItX[ix]=0;
    for (int iy=0; iy<=m_nDivsY; iy++)
      {
      if (m_Elements[Index(Min(ix, m_nDivsX-1), Min(iy, m_nDivsY-1))])
        {
          DoItX[ix]=1;
          break;
          }
        }
    }
 
  dbgpln("Surface:%s", Name());
  dbgpln("Usage  : %10.5f%%  [%i of %i]", Usage()*100, m_nElementsInited, m_nElements);
  dbgpln("Worst  : %10.5f%%  @%i,%i = %g,%g", 100*m_WorstErrorV, m_WorstErrorX,m_WorstErrorY, Map2X(m_WorstErrorX, 0.5), Map2Y(m_WorstErrorY, 0.5));

  dbgp("\t\t");
  for (ix=0; ix<=m_nDivsX; ix++)
    {
    if (DoItX[ix])
      {
      for(int jx=0; jx<2; jx++)
        {
        double dx=jx*0.5;
        double x1=Map2X(ix, dx);
        dbgp("%g\t",x1);
        }
      }
    }
  dbgpln("");

    
  for (int iy=0; iy<=m_nDivsY; iy++)
    {
    for(int jy=0; jy<2; jy++)
      {
      int DoIt=0;
      for (int ix=0; ix<=m_nDivsX; ix++)
        {
        if (m_Elements[Index(Min(ix, m_nDivsX-1), Min(iy, m_nDivsY-1))])
          {
          DoIt=1;
          break;
          }
        }

      if (!DoIt)
        continue;

      double dy=jy*0.5;
      double y1=Map2Y(iy, dy);
      dbgp("%5.1f:\t%15.8f\t", iy+dy,y1);
      for (ix=0; ix<=m_nDivsX; ix++)
        {
        if (DoItX[ix])
          {
          CSurface3DElement *E=m_Elements[Index(Min(ix, m_nDivsX-1), Min(iy, m_nDivsY-1))];
          if (E)
            {
            for(int jx=0; jx<2; jx++)
              {
              double dx=jx*0.5;
              dbgp("%g\t", m_fnZO((*E)(dx,dy)));
              }
            }
          else
            {
            for(int jx=0; jx<2; jx++)
              dbgp("\t");
            }
          }
        }
      dbgpln("");
      }
    }
*/
  };

CSurface3DElement *CSurface3D::MRU_RemoveTail()
  {
  CSurface3DElement *T=m_pMRUTail;
//  dbgpln("MRU_RemoveTail:%20s %08x %4i %08x H:%08x T:%08x", Name(), T, T->m_lIndex, T->m_pParent, m_pMRUHead, m_pMRUTail);
  if (m_pMRUTail)
    {
    m_nMRULen--;
    if (m_pMRUTail==m_pMRUHead)
      {
      m_pMRUHead=NULL;
      m_pMRUTail=NULL;
      }
    else
      {
      if (m_pMRUTail->m_pMRUNext)
        m_pMRUTail->m_pMRUNext->m_pMRUPrev=NULL;
      m_pMRUTail=m_pMRUTail->m_pMRUNext;
      }
    }
//  dbgpln("               %20s                        H:%08x T:%08x", "", m_pMRUHead, m_pMRUTail);
  return T;
  };

void CSurface3D::MRU_Move2Head(CSurface3DElement * E)
  {
//  dbgpln("MRU_Move2Head :%20s %08x %4i %08x H:%08x T:%08x", Name(), E, E->m_lIndex, E->m_pParent, m_pMRUHead, m_pMRUTail);
  if (m_pMRUHead!=E)
    {
    MRU_Remove(E);
    if (m_pMRUHead)
      m_pMRUHead->m_pMRUNext=E;
    E->m_pMRUNext=NULL;
    E->m_pMRUPrev=m_pMRUHead;
    m_pMRUHead=E;
    if (m_pMRUTail==NULL)
      m_pMRUTail=E;
    m_nMRULen++;
    }
//  dbgpln("               %20s                        H:%08x T:%08x", "", m_pMRUHead, m_pMRUTail);
  

  //
  //if (E->m_iMRUIndex!=m_iMRUItem)
  //  {
  //  m_iMRUItem=(m_iMRUItem+1)%ALLOCCNTMAX;
  //
  //  if (E->m_iMRUIndex>=0)
  //    { //swop
  //    CSurface3DElement *&A=m_MRU[E->m_iMRUIndex];
  //    CSurface3DElement *&B=m_MRU[m_iMRUItem];
  //    Exchange(A,B);
  //    Exchange(A->m_iMRUIndex, B->m_iMRUIndex);
  //    return NULL;
  //    }
  //  else
  //    {
  //    CSurface3DElement *R=m_MRU[m_iMRUItem];
  //    m_MRU[m_iMRUItem]=E;
  //    E->m_iMRUIndex=m_iMRUItem;
  //    R->m_iMRUIndex=-1;
  //    return R;
  //    }
  //  }
  };

void CSurface3D::MRU_Remove(CSurface3DElement * E)
  {
//  dbgpln("MRU_Remove    :%20s %08x %4i %08x H:%08x T:%08x", Name(), E, E->m_lIndex, E->m_pParent, m_pMRUHead, m_pMRUTail);
  m_nMRULen--;
  if (E==m_pMRUTail)
    {
    if (E==m_pMRUHead)
      {
      m_pMRUHead=NULL;
      m_pMRUTail=NULL;
      }
    else
      {
      if (m_pMRUTail->m_pMRUNext)
        m_pMRUTail->m_pMRUNext->m_pMRUPrev=NULL;
      m_pMRUTail=m_pMRUTail->m_pMRUNext;
      }
    }
  else if (E==m_pMRUHead)
    {
    if (m_pMRUHead->m_pMRUPrev)
      m_pMRUHead->m_pMRUPrev->m_pMRUNext=NULL;
    m_pMRUHead=m_pMRUHead->m_pMRUPrev;
    }
  else 
    {
    CSurface3DElement * P=E->m_pMRUPrev;
    CSurface3DElement * N=E->m_pMRUNext;
    if (P)
      P->m_pMRUNext=N;
    if (N)
      N->m_pMRUPrev=P;
    }
//  dbgpln("               %20s                        H:%08x T:%08x", "", m_pMRUHead, m_pMRUTail);
  }

#endif //UseNewSurfaces

//===========================================================================
//
//
//
//===========================================================================
  //
  //if (E->m_iMRUIndex!=m_iMRUItem)
  //  {
  //  m_iMRUItem=(m_iMRUItem+1)%ALLOCCNTMAX;
  //
  //  if (E->m_iMRUIndex>=0)
  //    { //swop
  //    CSurface3DFrame *&A=m_MRU[E->m_iMRUIndex];
  //    CSurface3DFrame *&B=m_MRU[m_iMRUItem];
  //    Exchange(A,B);
  //    Exchange(A->m_iMRUIndex, B->m_iMRUIndex);
  //    return NULL;
  //    }
  //  else
  //    {
  //    CSurface3DFrame *R=m_MRU[m_iMRUItem];
  //    m_MRU[m_iMRUItem]=E;
  //    E->m_iMRUIndex=m_iMRUItem;
  //    R->m_iMRUIndex=-1;
  //    return R;
  //    }
  //  }
//===========================================================================
//
//
//
//===========================================================================
