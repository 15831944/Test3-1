#pragma once

#include "scdver.h"

//========================================================================

#if SYSCAD10

//#include "NETServerU.h"

//========================================================================

class CSvcConnectCLR
  {
  public:
    CSvcConnectCLR(void);

  public:
    ~CSvcConnectCLR(void);

    bool Startup(CSvcConnect * pConn, LPCSTR projectPath, LPCSTR configPath);
    void Shutdown();

    void Sync(__int64 requestId);

    //Groups
    void DoCreateGroup(__int64 & requestId, CString & GroupGuid, LPCSTR Tag, LPCSTR Path,
                       const CRectangleF & boundingRect);
    
    //----------------------------------------------------------------------------------
    //Items
    void DoCreateItem(__int64 & requestId, CString & ItemGuid, LPCSTR Tag, LPCSTR Path, 
                      LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
                      float Angle, const CRectangleF & textArea, COLORREF FillColor, 
                      bool MirrorX, bool MirrorY);
    void DoDeleteItem(__int64 & requestId, LPCSTR ItemGuid);

    void DoModifyItemPosition(__int64 & requestId, LPCSTR ItemGuid, Pt_3f Delta);

    //----------------------------------------------------------------------------------
    //Links
    void DoCreateLink(__int64 & requestId, CString & LinkGuid, LPCSTR Tag, LPCSTR Path, 
                      LPCSTR ClassId, 
                      LPCSTR OriginGuid, LPCSTR DestinationGuid, LPCSTR OriginPort, LPCSTR DestinationPort, 
                      CPointFList & ControlPoints);
    void DoDeleteLink(__int64 & requestId, LPCSTR ItemGuid);
    void DoModifyLink(__int64 & requestId, CString & LinkGuid, LPCSTR Tag, LPCSTR Path, 
                      LPCSTR ClassId, 
                      LPCSTR OriginGuid, LPCSTR DestinationGuid, LPCSTR OriginPort, LPCSTR DestinationPort, 
                      CPointFList & ControlPoints);


    //----------------------------------------------------------------------------------
    //Things

    //----------------------------------------------------------------------------------
    //.....

    //void DoModifyItem(__int64 & requestId, LPCSTR ItemGuid, LPCSTR Tag, LPCSTR Path, 
    //                  LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect, 
    //                  float Angle, const CRectangleF & textArea, COLORREF FillColor, 
    //                  bool MirrorX, bool MirrorY);

    // ..........
    //static void CreateItem(__int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR path, LPCSTR model, LPCSTR stencil, PKRectangleF boundingRect, float angle, int fillColor, int fillMode, bool mirrorX, bool mirrorY);
    //static void CreateLink(__int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR classID, LPCSTR origin, LPCSTR destination, LPCSTR originPort, LPCSTR destinationPort, PKPointF controlPoints[], int controlPointsLength);

    //static void DeleteItem(__int64 requestId, LPCSTR guid);
    //static void DeleteLink(__int64 requestId, LPCSTR guid);

    void Load();
    void Save();

    //static void ModifyItemBoundingRect(__int64 requestId, LPCSTR guid, PKRectangleF boundingRect);
    //static void ModifyLinkControlPoints(__int64 requestId, LPCSTR guid, PKPointF controlPoints[], int controlPointsLength);

    //void Export(LPCSTR projectPath, LPCSTR configPath);

  public:

    CSvcConnect   * m_pConn;

  };

//========================================================================

#endif

//========================================================================
