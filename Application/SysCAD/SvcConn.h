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
      m_X = 0.0;
      m_Y = 0.0;
      m_W = 0.0;
      m_H = 0.0;
      };
    CRectangleF(double x, double y, double w, double h)
      {
      m_X = float(x);
      m_Y = float(y);
      m_W = float(w);
      m_H = float(h);
      }
    CRectangleF(float x, float y, float w, float h)
      {
      m_X = x;
      m_Y = y;
      m_W = w;
      m_H = h;
      }

    void Set(double x,double y,double w,double h)
      {
      m_X = float(x);
      m_Y = float(y);
      m_W = float(w);
      m_H = float(h);
      }

    void Set(float x,float y,float w,float h)
      {
      m_X = x;
      m_Y = y;
      m_W = w;
      m_H = h;
      }

    float  Left()    const { return m_X;      };
    float  Bottom()  const { return m_Y;      };
    float  Right()   const { return m_X+m_W;  };
    float  Top()     const { return m_Y+m_H;  };

    float  Width()   const { return m_W; };
    float  Height()  const { return m_H; };
    float  MidX()    const { return m_X+0.5f*m_W; };
    float  MidY()    const { return m_Y+0.5f*m_H; };

  protected:
    float   m_X;
    float   m_Y;
    float   m_W;
    float   m_H;
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

    void Startup(LPCSTR projectPath, LPCSTR configPath, bool ImportScd9);
    void Shutdown();
    //void Initialise();
    //void Terminate();

    void Export(LPCSTR projectPath, LPCSTR configPath);
    void Load();
    void Save();

    //------------------------------------------------------------------------

    void OnCreateGroup(bool DoingExport, __int64 eventId, __int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR path, 
      const CRectangleF & boundingRect);

    //------------------------------------------------------------------------
    // Items
    // When actions are initiated in Scd9
    // if DoUpdate it true then the callback's must 'redo' the work else they should be filtered out

    // Utils ------
    //static CString MakePath(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page);
    CString               MakePath(LPCSTR Part1=NULL, LPCSTR Part2=NULL, LPCSTR Part3=NULL);
    CRectangleF           GetPageRect(LPCSTR PgName);
    CString               ExtractPageName(LPCSTR Path);
    CGrfDoc             * FindGrfDoc(LPCSTR PageName);
    CString               ExtractShape(LPCSTR Symbol);

    // Operations
    void DoCreateItem(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page, LPCSTR Tag, LPCSTR Symbol, LPCSTR ClassId, Pt_3f Pt, Pt_3f Scl, float Angle);
    void DoModifyItem(CGrfDoc *pDoc, DXF_ENTITY eEntity);
    void DoDeleteItem(LPCSTR Tag);

    // CallBack's
    void OnCreateItem(__int64 eventId, __int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR path, 
      LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
      float angle, const CRectangleF & textArea, COLORREF Colour, 
      bool mirrorX, bool mirrorY);

    void OnDeleteItem(__int64 eventId, __int64 requestId, LPCSTR guid);

    void OnModifyItem(__int64 eventId, __int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR path, 
      LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
      float angle, const CRectangleF & textArea, COLORREF Colour, 
      bool mirrorX, bool mirrorY);

    //------------------------------------------------------------------------

  protected:
    CSvcConnectCLR    * m_pCLR;
    
    CsGrfGroupMap   m_GrfGrpsNames;
    CsGrfGroupMap   m_GrfGrpsGuids;
    __int64         m_lEventId;
    __int64         m_lRequestId;
    __int64         m_lRequestIdRet;

    bool            m_bExportBusy;

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
