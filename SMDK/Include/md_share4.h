//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#if !defined(MD_SHARE4_H_INCLUDED_)
#define MD_SHARE4_H_INCLUDED_

// ======================================================================
//
//
//
// ======================================================================

// ======================================================================
//
//
//
// ======================================================================

#undef DllImportExport
#if defined(__PROPERTYBASE_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

// ======================================================================
//
//
//
// ======================================================================

// ======================================================================
//
//
//
// ======================================================================

#undef DllImportExport
#if defined(__MD_DEFN_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCDLIB)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif


//---------------------------------------------------------------------------

#undef DllImportExport
#if defined(__MD_SHARE4_CPP)
#define DllImportExport __declspec(dllexport)
#elif !defined(SCEXEC)
#define DllImportExport __declspec(dllimport)
#else
#define DllImportExport
#endif

// ======================================================================
//
//  Model Actions (Context Menu)
//
// ======================================================================

enum eMdlActionTypes { MAT_NULL, MAT_State, MAT_Value, MAT_Follow, MAT_Config, MAT_Switch };

class DllImportExport CMdlAction
  {
  public:
    CMdlAction();
    CMdlAction(int Index, eMdlActionTypes Type, bool Enabled=0, LPCSTR Name="", int IntValue=0);
    CMdlAction(int Index, eMdlActionTypes Type, bool Enabled, LPCSTR Name, bool InvalidOK,
      double DblValue=0, double ValueMin=0, double ValueMax=0, double Bumpless=0);

  public:
    int            iIndex;
    eMdlActionTypes iType;
    bool           fEnabled;
    char           cName[64];
    union
      {
      int            iValue;
      bool           fInvalidOK;
      };
    double         dValue;
    double         dValueMin;
    double         dValueMax;
    double         dBumpless;
  };

class DllImportExport CMdlActionArray : public CArray<CMdlAction, CMdlAction&> {};

//===========================================================================
//
//  Model Graphics (Context Menu)
//
//===========================================================================

enum eMdlGraphicTypes { MGT_Simple };
enum eMdlGraphicTasks { MGT_Create, MGT_EraseBkgnd, MGT_Paint, MGT_Size, MGT_Move, MGT_MouseMove, MGT_LButtonDown, MGT_LButtonUp, MGT_RButtonDown, MGT_RButtonUp};

class DllImportExport CMdlGraphicWnd
  {
  public:
    CMdlGraphicWnd(eMdlGraphicTasks Task, CWnd * Wnd, CRect ClientRect, CDC * DC);
    CMdlGraphicWnd(eMdlGraphicTasks Task, CWnd * Wnd, CRect ClientRect, CPaintDC * DC, CRect TextSize);
    CMdlGraphicWnd(eMdlGraphicTasks Task, CWnd * Wnd, CRect ClientRect, CPoint MousePt, UINT MouseFlags);

  public:
    eMdlGraphicTasks  m_eTask;
    CWnd            * m_pWnd;
    CDC             * m_pDC;
    CPaintDC        * m_pPaintDC;
    CRect             m_ClientRect;
    CPoint            m_TextSize;
    CPoint            m_MousePt;
    UINT              m_MouseFlags;
    BOOL              m_bReturn;
  };

// ----------------------------------------------------------------------

class DllImportExport CMdlGraphic
  {
  public:
    CMdlGraphic();
    CMdlGraphic(int Index, eMdlGraphicTypes Type, bool Enabled = 0, LPCSTR Name = "", int IntValue = 0);
    CMdlGraphic(int Index, eMdlGraphicTypes Type, bool Enabled, LPCSTR Name, bool InvalidOK,
      double DblValue = 0, double ValueMin = 0, double ValueMax = 0, double Bumpless = 0);

  public:
    int               m_iIndex;
    eMdlGraphicTypes  m_iType;
    bool              m_fEnabled;
    char              m_cName[64];
  };

class DllImportExport CMdlGraphicArray : public CArray<CMdlGraphic, CMdlGraphic&> {};

// ======================================================================
//
//
//
// ======================================================================

#undef DllImportExport 

// ======================================================================
//
//
//
// ======================================================================

#endif // !defined(MD_SHARE4_H_INCLUDED_)
