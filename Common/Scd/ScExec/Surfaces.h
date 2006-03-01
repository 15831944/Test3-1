//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef __SURFACES_H
#define __SURFACES_H

#ifndef __AFXTEMPL_H__
  #include <afxtempl.h> // "xafxtempl.h"
#endif

#ifdef __SURFACES_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

#define UseNewSurfaces 0

//===========================================================================
//
//
//
//===========================================================================

class DllImportExport CSurfaceCommon
  {
  public:
    bool    m_bInList;
    static CArray<CSurfaceCommon*, CSurfaceCommon*> List;

    static bool    m_UseSurface;
    static bool    m_DoSmoothing;
    static double  m_ErrorMsgTol;
    static double  m_FitTolerance;

    CSurfaceCommon()
      {
      m_bInList=false;
      }

    ~CSurfaceCommon()
      {
      }

    virtual void AddToList()
      {
      if (!m_bInList)
        {
        m_bInList=true;
        List.Add(this);
        }
      };
    virtual void Dump()=0;
  };

//===========================================================================
//
//
//
//===========================================================================

typedef double(*TSurfaceXfFn)(double X);

extern DllImportExport double SfXFDirect(double X);
extern DllImportExport double SfXFLogIn(double X);
extern DllImportExport double SfXFLogOut(double X);

//---------------------------------------------------------------------------

typedef double(*TSurface2DFn)(double X);

struct CSurface2DPoint
  {
  double x,y;

  CSurface2DPoint(double X, double Y)
    {
    x=X;
    y=Y;
    };
  };

//---------------------------------------------------------------------------

class DllImportExport CSurface2DElement
  {
  friend class CSurface2D;
  protected:
    bool   m_Inited;
    double m_A, m_B;

  public:
    CSurface2DElement()
      {
      m_Inited=false;
      }
    flag Fit(CSurface2DPoint L, CSurface2DPoint R);
    double CheckFit(long Ix, CSurface2D & Surface);
    double operator()(double x)
      {
      return m_A*x+m_B;
      }
  };

typedef CArray<CSurface2DElement, CSurface2DElement&> CSurface2DArray;

//---------------------------------------------------------------------------

class DllImportExport CSurface2D : public CSurfaceCommon
  {
  friend class CSurface2DElement;
  protected:
    CString       m_sName;
    long          m_nDivs;
    long          m_nDivsInited;
    double        m_dMinimum;
    double        m_dMaximum;
    double        m_dDivWidth;
                  
    double        m_WorstErrorV;
    long          m_WorstErrorX;
    
    TSurfaceXfFn  m_fnXI;
    TSurfaceXfFn  m_fnXO;

    TSurfaceXfFn  m_fnYI;
    TSurfaceXfFn  m_fnYO;

    TSurface2DFn  m_fnY;

    double        m_DX;

    CSurface2DArray m_Elements;

    CCriticalSection m_TheLock;

  public:
    CSurface2D();
    CSurface2D(LPCTSTR Name, 
               double MinX, double MaxX, long NDivs, TSurfaceXfFn fnXI, TSurfaceXfFn fnXO, 
               TSurface2DFn Fn, TSurfaceXfFn fnYI, TSurfaceXfFn fnYO);
    bool Initialise(LPCTSTR Name,
                    double MinX, double MaxX, long NDivs, TSurfaceXfFn fnXI, TSurfaceXfFn fnXO, 
                    TSurface2DFn Fn, TSurfaceXfFn fnYI, TSurfaceXfFn fnYO);
    double Map2X(long Ix, double fx)
      {
      return m_fnXO(m_dMinimum+(Ix+fx)*m_dDivWidth);
      };

    void X2Map(double X, long &Ix, double &Fx)
      {
      double DX=(m_fnXI(X)-m_dMinimum)/m_dDivWidth;
      Ix=Range(0L, (long)floor(DX), m_nDivs-1L);
      Fx=DX-Ix;
      }
    CSurface2DElement &CSurface2D::GetElement(long Ix);
    double operator()(double X);
    double Usage() { return m_nDivsInited/Max(1.0, (double)m_nDivs);};
    void   Dump();
    LPCTSTR Name() { return m_sName; };
  };

//===========================================================================
//
//
//
//===========================================================================

typedef double(*TSurface3DFn)(double X, double Y);
typedef double(*TSurface3DBnd)(double A);

struct CSurface3DPoint
  {
  double x,y,z;

  CSurface3DPoint(double X, double Y, double Z)
    {
    x=X;
    y=Y;
    z=Z;
    };
  };

class DllImportExport CSurface3DBoundary
  {
  public:

    double m_XMin, m_XMax;
    double m_YMin, m_YMax;
    TSurface3DBnd m_FnYx;
    TSurface3DBnd m_FnXy;

  CSurface3DBoundary(double XMin, double XMax, TSurface3DBnd FnYx,
                     double YMin, double YMax, TSurface3DBnd FnXy)
    {
    m_XMin=XMin;
    m_XMax=XMax;
    m_FnYx=FnYx;
    m_YMin=YMin;
    m_YMax=YMax;
    m_FnXy=FnXy;
    };
  };

#if UseNewSurfaces

//---------------------------------------------------------------------------

const byte C3D_DivNone = 0;
const byte C3D_Div1x1  = 1;
const byte C3D_Div2x2  = 2;
const byte C3D_Div2H   = 3;
const byte C3D_Div3H   = 4;
const byte C3D_Div2V   = 5;
const byte C3D_Div3V   = 6;
const byte C3D_DivNxN  = 7;
const byte C3D_Other   = 8;
const byte C3D_Bad     = 9;

class CSurface3DFrame;
class CSurface3D; // forward

class DllImportExport CSurface3DTriangle
  {
  public:
    double m_A, m_B, m_C;
    bool Fit(CSurface3DPoint &P1, CSurface3DPoint &P2, CSurface3DPoint &P3);
    double operator()(double mx, double my);
  };

class CSurface3DPanel
  {
  public:
    CSurface3DPanel   *m_pMRUNext;
    CSurface3DPanel   *m_pMRUPrev;   
    CSurface3DFrame   *m_pFrame;
    
    flag               m_bFitOK:1,  m_bLOK:1, m_bROK:1, m_bTOK:1, m_bBOK:1;
    CSurface3DTriangle m_tL, m_tR, m_tT, m_tB;
    
    CSurface3DPanel();
    CSurface3DPanel(CSurface3D *p3D, CSurface3DFrame *pFrame, long nX, double XMin, double XMax, long nY, double YMin, double YMax);
    bool    Fit(CSurface3D *p3D, CSurface3DFrame *pFrame,
                long nX, double XMin, double XMax, long nY, double YMin, double YMax);
    bool    CheckFit(CSurface3DFrame *pFrame);
    double  ValueFxFy(double Fx, double Fy);
  };

typedef CSurface3DFrame* LPCSurface3DFrame;

class DllImportExport CSurface3DFrame
  {
  friend class CSurface3D;
  public:
    CSurface3DFrame();
    ~CSurface3DFrame();

    static long m_nGlblElementCnt;
    static long m_nGlblFrameCnt;

    byte    m_eDivision; 
    flag    m_bInitDone:1;

    //protected:
    long    m_nDivsX, m_nDivsY;         
    double  m_dXMin, m_dYMin;      
    double  m_dXMax, m_dYMax;  
    long    m_iLevel;

    double  m_dDivWidthX, m_dDivWidthY;     
    
    CSurface3D         *m_p3D;
    CSurface3DPanel    *m_pPanel;        
    CSurface3DFrame    **m_Frames;

  //protected:
    CSurface3DFrame(CSurface3D * p3D, long Level,
                    double MinX, double MaxX, long NDivsX, 
                    double MinY, double MaxY, long NDivsY);
    void Initialise(CSurface3D * p3D, long Level,
                    double MinX, double MaxX, long NDivsX, 
                    double MinY, double MaxY, long NDivsY);
    CSurface3DPanel * BuildPanel();
    byte    FitPanel();
    bool    FitAndTestPanels(long NX, long NY, CSurface3DFrame Frames[]);
    double  ValueXY(double X, double Y);
    long    Index(long Ix, long Iy) { return m_nDivsX*Iy+Ix; };
    long    XIndex(long I)          { return I % m_nDivsX; };
    long    YIndex(long I)          { return I / m_nDivsX; };
    double  DoOp(double X, double Y, LPCSurface3DFrame &pFrame);
    double  operator()(double X, double Y);

    double  Map2X(double fx);
    long    X2MapI(double X);
    double  X2MapF(double fx);
    double  Map2Y(double fy);
    long    Y2MapI(double Y);
    double  Y2MapF(double fy);
    CSurface3DFrame * CSurface3DFrame::GetElement(long Ix, long Iy) { return m_Frames[Index(Ix, Iy)]; }
  };

class DllImportExport CSurface3D : public CSurfaceCommon
  {
  friend class CSurface3DFrame;
  //protected:
  public:
    CString           m_sName;
    
    TSurface3DFn      m_fnZ;
    CSurface3DBoundary *m_pBoundary;

    CSurface3DFrame   m_E;
   
    long              m_nCalls;
    long              m_nFrameCnt;
    long              m_nBuildCnt;
    long              m_nPanelCnt;
    long              m_nPanelCntMax;
    CSurface3DPanel  *m_pMRUHead;
    CSurface3DPanel  *m_pMRUTail;
                      
    DWORD             m_dwLastTicks;

    CCriticalSection m_TheLock;

  public:
    CSurface3D();
    CSurface3D(LPCTSTR Name, 
                    double MinX, double MaxX, long NDivsX, TSurfaceXfFn fnXI, TSurfaceXfFn fnXO, 
                    double MinY, double MaxY, long NDivsY, TSurfaceXfFn fnYI, TSurfaceXfFn fnYO, 
                    TSurface3DFn Fn,
                    CSurface3DBoundary *pBoundary);
    ~CSurface3D();
    double  operator()(double X, double Y) { return (m_UseSurface ? m_E(X, Y) : m_fnZ(X, Y)); };
    long    MemUsage() { return -1;};//m_nElementCnt * sizeof(CSurface3DFrame)+
                             //m_nElementPtrCnt * sizeof(CSurface3DFrame*); };
    long    FrameCount() 
      { 
      return m_nFrameCnt;
      };
    void    Dump();
    LPCTSTR Name() { return m_sName; };

    CSurface3DPanel *MRU_RemoveTail();
    void MRU_Move2Head(CSurface3DPanel * E);
    void MRU_Remove(CSurface3DPanel * E);
  };

//===========================================================================
//
//
//
//===========================================================================

inline double CSurface3DFrame::Map2X(double fx)
  {
  return (m_dXMin+fx*(m_dXMax-m_dXMin));
  };
inline long CSurface3DFrame::X2MapI(double X)
  {
  const double DX=((X)-m_dXMin)/m_dDivWidthX;
  return Range((long)0, (long)(floor(DX)), (long)(m_nDivsX-1));
  }
inline double CSurface3DFrame::X2MapF(double X)
  {
  return ((X)-m_dXMin)/(m_dXMax-m_dXMin);
  }
inline double CSurface3DFrame::Map2Y(double fy)
  {
  return (m_dYMin+fy*(m_dYMax-m_dYMin));
  };

inline long CSurface3DFrame::Y2MapI(double Y)
  {
  const double DY=((Y)-m_dYMin)/m_dDivWidthY;
  return Range((long)0, (long)floor(DY), (long)(m_nDivsY-1));
  }
inline double CSurface3DFrame::Y2MapF(double Y)
  {
  return ((Y)-m_dYMin)/(m_dYMax-m_dYMin);
  }

#else//UseNewSurfaces

//---------------------------------------------------------------------------

class DllImportExport CSurface3DTriangle
  {
  public:
    double m_A, m_B, m_C;
    bool Fit(CSurface3DPoint &P1, CSurface3DPoint &P2, CSurface3DPoint &P3);
    double operator()(double mx, double my);
  };

class CSurface3DElement;
typedef CSurface3DElement * LPCSurface3DElement;
class CSurface3D; // forward
class DllImportExport CSurface3DElement
  {
  friend class CSurface3D;
  public:
    CSurface3DElement(CSurface3D & r3D, CSurface3DElement *pParent, long lIndex, long Level);

    ~CSurface3DElement();
    double operator()(double X, double Y);
    double DX(double X, double Y);
    double DY(double X, double Y);
    
    static long m_nGlblElementCnt;

  protected:
    long    m_nDivsX, m_nDivsY;         
    double  m_dMinimumX, m_dMinimumY;      
    double  m_dMaximumX, m_dMaximumY;      
    double  m_dDivWidthX, m_dDivWidthY;     
    long    m_iLevel;
    flag    m_bInitDone:1,
            m_bFitDone:1,
            m_bFitOK:1,
            m_bLOK:1, m_bROK:1, m_bTOK:1, m_bBOK:1;

    CSurface3DTriangle m_tL, m_tR, m_tT, m_tB;

    long               m_lIndex;
    CSurface3DElement *m_pParent;
    CSurface3DElement *m_pMRUNext;
    CSurface3DElement *m_pMRUPrev;
    CSurface3DElement ** m_Elements;
    CSurface3D & m_r3D;

  protected:
    bool Initialise(double MinX, double MaxX, long NDivsX, 
                    double MinY, double MaxY, long NDivsY);
    void    InitElements();
    void    RemoveElements();
    flag    Fit(long nX, long nY, CSurface3DPoint &BL, CSurface3DPoint &BR, CSurface3DPoint &TL, CSurface3DPoint &TR, CSurface3DPoint &CC);
    bool    CheckFit();
    double  ValueXY(double X, double Y);
    double  ValueFxFy(double Fx, double Fy);
    long    Index(long x, long y) { return m_nDivsX*y+x; };
    double  DoOp(double X, double Y, LPCSurface3DElement &pElement);

    double  Map2X(double fx);
    long    X2MapI(double X);
    double  X2MapF(double fx);
    double  Map2Y(double fy);
    long    Y2MapI(double Y);
    double  Y2MapF(double fy);

    CSurface3DElement &GetElement(long Ix, long Iy);
  };

class DllImportExport CSurface3D : public CSurfaceCommon
  {
  friend class CSurface3DElement;
  protected:
    CString           m_sName;
    
    TSurface3DFn      m_fnZ;

    CSurface3DElement m_E;

    CSurface3DElement *m_pMRUHead;
    CSurface3DElement *m_pMRUTail;
    
    long    m_nMRULen;
    long    m_nMRULenMax;
    long    m_nElementCnt;
    long    m_nElementAllocs;
    long    m_nCalls;
    long    m_nElementPtrCnt;

    DWORD   m_dwLastTicks;

    CCriticalSection m_TheLock;

  public:
    CSurface3D();
    CSurface3D(LPCTSTR Name, 
                    double MinX, double MaxX, long NDivsX, TSurfaceXfFn fnXI, TSurfaceXfFn fnXO, 
                    double MinY, double MaxY, long NDivsY, TSurfaceXfFn fnYI, TSurfaceXfFn fnYO, 
                    TSurface3DFn Fn,
                    CSurface3DBoundary *pBoundary);
    ~CSurface3D();
    void    InitElements();
    void    RemoveElements();
    double  operator()(double X, double Y) { return (m_UseSurface ? m_E(X, Y) : m_fnZ(X, Y)); };
    double  DX(double X, double Y);
    double  DY(double X, double Y);
    long    MemUsage() { return m_nElementCnt * sizeof(CSurface3DElement)+
                             m_nElementPtrCnt * sizeof(CSurface3DElement*); };
    long    ElementCount() { return m_nElementCnt; };
    void    Dump();
    LPCTSTR Name() { return m_sName; };

    CSurface3DElement *MRU_RemoveTail();
    void MRU_Move2Head(CSurface3DElement * E);
    void MRU_Remove(CSurface3DElement * E);
  };

//===========================================================================
//
//
//
//===========================================================================

inline double CSurface3DElement::Map2X(double fx)
  {
  return (m_dMinimumX+fx*(m_dMaximumX-m_dMinimumX));
  };
inline long CSurface3DElement::X2MapI(double X)
  {
  const double DX=((X)-m_dMinimumX)/m_dDivWidthX;
  return Range((long)0, (long)(floor(DX)), (long)(m_nDivsX-1));
  }
inline double CSurface3DElement::X2MapF(double X)
  {
  return ((X)-m_dMinimumX)/(m_dMaximumX-m_dMinimumX);
  }
inline double CSurface3DElement::Map2Y(double fy)
  {
  return (m_dMinimumY+fy*(m_dMaximumY-m_dMinimumY));
  };

inline long CSurface3DElement::Y2MapI(double Y)
  {
  const double DY=((Y)-m_dMinimumY)/m_dDivWidthY;
  return Range((long)0, (long)floor(DY), (long)(m_nDivsY-1));
  }
inline double CSurface3DElement::Y2MapF(double Y)
  {
  return ((Y)-m_dMinimumY)/(m_dMaximumY-m_dMinimumY);
  }

#endif //UseNewSurfaces
//===========================================================================
//
//
//
//===========================================================================

#undef DllImportExport
#endif