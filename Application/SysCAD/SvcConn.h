#pragma once

#include "scdver.h"
#include "grfdoc.h"
#include "msgwnd.h"
#include "neutralcommon.h"
#include "grfcmds.h"

//========================================================================

#if SYSCAD10

//========================================================================

class CRectangleF
  {
  public:
    CRectangleF()
      {
      m_Left    = 0.0;
      m_Right   = 0.0;
      m_Bottom  = 0.0;
      m_Top     = 0.0;
      };
    CRectangleF(double L, double R, double B, double T)
      {
      m_Left    = L;
      m_Right   = R;
      m_Bottom  = B;
      m_Top     = T;
      }

    void Set(double Left,double Bottom,double Width,double Height)
      {
      m_Left    = Left;
      m_Right   = Left+Width;
      m_Bottom  = Bottom;
      m_Top     = Bottom+Height;
      }

    double  Left()    const { return m_Left;   };
    double  Right()   const { return m_Right;  };
    double  Bottom()  const { return m_Bottom; };
    double  Top()     const { return m_Top;    };

    double  Width()   const { return (m_Right-m_Left); };
    double  Height()  const { return (m_Top-m_Bottom); };
    double  MidX()    const { return 0.5*(m_Left+m_Right); };
    double  MidY()    const { return 0.5*(m_Bottom+m_Top); };

  protected:
    double   m_Left;
    double   m_Right;
    double   m_Bottom;
    double   m_Top;
  };

//========================================================================

class CsGrfGroup
  {
  public:
    CsGrfGroup()
      {}
  public:
    CString       m_Guid;
    CString       m_Name;
    CString       m_Path;

    CRectangleF   m_Bounds;
  };


class CsGrfGroupMap : public CMap <LPCSTR, LPCSTR, CsGrfGroup*, CsGrfGroup*> {};

//========================================================================


class CsGrfGuidEntry
  {
  public:
    CsGrfGuidEntry()
      {
      m_Entity  = NULL;
      m_pDoc    = NULL;
      }

  public:
    CString       m_Guid;
    DXF_ENTITY    m_Entity;
    CGrfDoc     * m_pDoc;
    //CString       m_Name;
    //CString       m_Path;

  };

class CSvcConnectCLR;

class CSvcConnect
  {
  public:
    CSvcConnect();

  public:
    ~CSvcConnect();

    void Startup(char* projectPath, char* configPath, bool ImportScd9);
    void Shutdown();
    //void Initialise();
    //void Terminate();

    //------------------------------------------------------------------------

    void OnCreateGroup(bool DoingExport, __int64 eventId, __int64 requestId, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, 
      const CRectangleF & boundingRect);

    //------------------------------------------------------------------------

    void OnCreateItem(bool DoingExport, __int64 eventId, __int64 requestId, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, 
      LPCTSTR model, LPCTSTR shape, const CRectangleF & boundingRect, 
      double angle, COLORREF Colour, 
      bool mirrorX, bool mirrorY);

    void OnDeleteItem(__int64 eventId, __int64 requestId, LPCTSTR guid);

    void OnModifyItem(__int64 eventId, __int64 requestId, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, 
      LPCTSTR model, LPCTSTR shape, const CRectangleF & boundingRect, 
      double angle, COLORREF Colour, 
      bool mirrorX, bool mirrorY);

    void DoCreateItem(DXF_ENTITY eEntity, CGrfDoc *pDoc);


    void Export(char* projectPath, char* configPath);
    void Load();
    void Save();

  protected:
    CSvcConnectCLR    * m_pCLR;
    
    CsGrfGroupMap   m_GrfGrpsNames;
    CsGrfGroupMap   m_GrfGrpsGuids;

  };

//========================================================================
//
//
//
//========================================================================

class CGetExistingItems
  {
  public:
    enum eType { eIsError, eIsLink, eIsNode, eIsNull };
    CGetExistingItems();

    bool            GetOne();

    int             PageCount()  { return m_nPages;           };
    int             PageNo()     { return m_iPage;            };
    LPCSTR          PageName()   { return m_sPage;            };
    CGrfTagInfo   & Item()       { return m_GTIA[m_iInArray]; };
    LPCSTR          Guid()       { return m_Guid;             };
    eType           Type()       { return m_Type;             };
    CRectangleF   & PageRct()    { return m_PageRct;          };

  protected:
    
    int               m_nPages;
    CDocTemplate    & m_GrfTemplate;
    POSITION          m_GrfDocPos;
    CGrfDoc         * m_pDoc;
    CGrfTagInfoArray  m_GTIA;
    CRectangleF       m_PageRct;
    
    int               m_nInArray;
    int               m_iInArray;

    int               m_iPage;
    CString           m_sPage;
    CString           m_Guid;
    eType             m_Type;

  };

//========================================================================

#endif

//========================================================================
