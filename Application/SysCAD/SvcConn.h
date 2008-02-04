#pragma once

#include "scdver.h"
#include "grfdoc.h"
#include "msgwnd.h"
#include "neutralcommon.h"
#include "grfcmds.h"

//========================================================================

#if SYSCAD10

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

    void Startup();
    bool ConfigSetup();
    void Shutdown();
    //void Initialise();
    //void Terminate();

    //bool RequestItemDefinitions(LPSTR Group, CFlwNodeDefinitionList & Defns);

    void Upgrade2Scd10(LPCSTR projectPath, LPCSTR configPath);
    void Attach2Scd10();
    void Load();
    void Save();

    void LogMessage(DWORD Type, LPCSTR Msg);
    void dbgPrintLn(LPCSTR Fmt, LPCSTR S0=NULL, LPCSTR S1=NULL, LPCSTR S2=NULL, LPCSTR S3=NULL, LPCSTR S4=NULL);

    //------------------------------------------------------------------------

    void DoOpenProject(LPCSTR tag, LPCSTR path);
    void Remove9Graphics();

    //------------------------------------------------------------------------

    void OnCreateGroup(__int64 eventId, __int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR path, const CRectangleF & boundingRect);

    //------------------------------------------------------------------------
    // Items
    // When actions are initiated in Scd9
    // if DoUpdate it true then the callback's must 'redo' the work else they should be filtered out

    // Utils ------
    //static CString MakePath(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page);
    static CString        MakePath(LPCSTR Part1=NULL, LPCSTR Part2=NULL, LPCSTR Part3=NULL);
    CRectangleF           GetPageRect(LPCSTR PgName);
    CRectangleF           GetContainingPageRect(const CRectangleF & rct);
    CsGrfGroup          * GetContainingGroup(const CRectangleF & rct);
    static CString        ExtractPageName(LPCSTR Path);
    static CGrfDoc      * FindGrfDoc(LPCSTR PageName);
    static CGrfWnd      * FindGrfWnd(LPCSTR PageName);
    static CString        ExtractShape(LPCSTR Symbol);

    // Groups ----------------------------------------------------------------
    // Operations
    void GCBCreateGroup(LPCSTR Guid, LPCSTR Prj, LPCSTR Page, const CRectangleF & boundingRect);
    //void GCBDeleteGroup(DXF_ENTITY eEntity, LPCSTR Tag);

    // Items -----------------------------------------------------------------
    // Operations
    void GCBCreateNode(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page, LPCSTR Tag, LPCSTR Symbol, LPCSTR ClassId, Pt_3f Pt, Pt_3f Scl, float Angle);
    void GCBDeleteNode(DXF_ENTITY eEntity, LPCSTR Tag);
    void GCBModifyNodePosition(CGrfDoc *pDoc, DXF_ENTITY eEntity, LPCSTR Tag, Pt_3f Delta);

    // CallBack's
    void OnCreateNodeM(__int64 eventId, __int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR model);
    void OnDeleteNodeM(__int64 eventId, __int64 requestId, LPCSTR guid);
    void OnModifyNodeM(__int64 eventId, __int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR model);

    void OnCreateNodeG(__int64 eventId, __int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR path, 
      LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
      float angle, const CRectangleF & tagArea, float tagAngle, COLORREF Colour, 
      bool mirrorX, bool mirrorY);
    void OnDeleteNodeG(__int64 eventId, __int64 requestId, LPCSTR guid);
    void OnModifyNodeG(__int64 eventId, __int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR path, 
      LPCSTR model, LPCSTR shape, const CRectangleF & boundingRect, 
      float angle, const CRectangleF & tagArea, float tagAngle, COLORREF Colour, 
      bool mirrorX, bool mirrorY);

    // Links -----------------------------------------------------------------
    // Operations
    void GCBCreateLink(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page, LPCSTR Tag, LPCSTR ClassId, 
                      LPCSTR SrcTag, LPCSTR DstTag, LPCSTR SrcPort, LPCSTR DstPort, 
                      CPointFList & ControlPoints);//, const CRectangleF & tagArea);
    void GCBDeleteLink(DXF_ENTITY eEntity, LPCSTR Tag);
    //void GCBDoModifyLinkPosition(CGrfDoc *pDoc, DXF_ENTITY eEntity, LPCSTR Tag, Pt_3f Delta);
    void GCBModifyLinkPts(CGrfDoc *pDoc, LPCSTR Prj, LPCSTR Page, LPCSTR Tag, /*LPCSTR ClassId, 
                      LPCSTR SrcTag, LPCSTR DstTag, LPCSTR SrcPort, LPCSTR DstPort, */
                      CPointFList & ControlPoints, const CRectangleF & tagArea, float tagAngle);


    // CallBack's
    void OnCreateLinkG(__int64 eventId, __int64 requestId, LPCSTR LinkGuid, LPCSTR Tag, /*LPCSTR Path,*/ 
                      LPCSTR ClassId, 
                      LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                      CPointFList & ControlPoints, const CRectangleF & tagArea, float tagAngle);
    void OnDeleteLinkG(__int64 eventId, __int64 requestId, LPCSTR guid);
    void OnModifyLinkG(__int64 eventId, __int64 requestId, LPCSTR LinkGuid, LPCSTR Tag, /*LPCSTR Path,*/ 
                      LPCSTR ClassId, 
                      LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                      CPointFList & ControlPoints, const CRectangleF & tagArea, float tagAngle);
    
    void OnCreateLinkM(__int64 eventId, __int64 requestId, LPCSTR LinkGuid, LPCSTR Tag, /*LPCSTR Path,*/ 
                      LPCSTR ClassId, 
                      LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                      LPCSTR OriginPort, LPCSTR DestinationPort);
    void OnDeleteLinkM(__int64 eventId, __int64 requestId, LPCSTR guid);
    void OnModifyLinkM(__int64 eventId, __int64 requestId, LPCSTR LinkGuid, LPCSTR Tag, /*LPCSTR Path,*/ 
                      LPCSTR ClassId, 
                      LPCSTR OriginGuid, LPCSTR DestinationGuid, 
                      LPCSTR OriginPort, LPCSTR DestinationPort);

    //------------------------------------------------------------------------

  //protected:
    CSvcConnectCLR    * m_pCLR;
    
    CsGrfGroupMap   m_GrfGrpsNames;
    CsGrfGroupMap   m_GrfGrpsGuids;
    __int64         m_lEventId;
    //__int64         m_lRequestId;
    __int64         m_lRequestIdRet;
    //bool            m_bExportBusy;

    CSvcExecCtrl    m_Ctrl;

  };

inline CSvcConnect & PrjSvc() { return *(gs_pPrj->m_pSvc); };
inline CSvcExecCtrl & PrjSvcCtrl() { return gs_pPrj->m_pSvc->m_Ctrl; };

//========================================================================
//
//
//
//========================================================================

class CExistingItems
  {
  public:

    class CGroup
      {
      public:
        CGroup()
          {
          m_No      = -1;
          m_pDoc    = NULL;
          m_XOff    = 0;
          m_YOff    = 0;
          }
        CGroup(LPCSTR Title, int No, CGrfDoc *pDoc)
          {
          m_sTitle  = Title;
          m_No      = No;
          m_pDoc    = pDoc;
          m_XOff    = 0;
          m_YOff    = 0;
          }

        CString         m_sTitle;
        CString         m_sSymbol;
        int             m_No;
        CGrfDoc       * m_pDoc;
        CRectangleF     m_PageRct;
        float           m_XOff;
        float           m_YOff;

        CGrfTagInfoArray  m_GTIA;
      };

    class CItem
      {
      public:
        CItem()
          {
          m_pNLItem = NULL;
          };
        CItem(LPCSTR Tag, CNodeListItem * pNLItem)
          {
          m_sTag    = Tag;
          m_pNLItem = pNLItem;
          };


        CString         m_sTag;
        CNodeListItem * m_pNLItem;
        CRectangleF     m_BndRct;
      };

    class CGroupIndex
      {
      public:
        CGroupIndex()
          {
          m_pGrp = NULL;
          m_iGTIA = -1;
          };
        CGroupIndex(CGroup *  pGrp, long iGTIA)
          {
          m_pGrp = pGrp;
          m_iGTIA = iGTIA;
          };
        CGroup *  m_pGrp;
        long      m_iGTIA;

      };

    CExistingItems();
    ~CExistingItems();

    bool        Get();

  public:
    int               m_nPages;

    CList<CGroup*, CGroup*>                           m_Groups; 
    CList<CItem*, CItem*>                             m_Items; 
    CNodeList                                         m_Nodes;
    CMap<LPCSTR, LPCSTR, CGroup*, CGroup*>            m_PageMap;
    CMap<LPCSTR, LPCSTR, CGroupIndex, CGroupIndex&>   m_TagMap;

  };

//========================================================================

#endif

//========================================================================
