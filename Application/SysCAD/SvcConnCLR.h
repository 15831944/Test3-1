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

    void Startup(CSvcConnect * pConn);
    bool ConfigSetup(CSvcConnect * pConn);
    void Shutdown();

    bool PrepareForUpgrade(LPCSTR projectName, LPCSTR projectPath);

    void Attach2Scd10();

    void Sync(__int64 requestId);

    //Groups
    void AddCreateGroup(__int64 & requestId, LPCSTR GroupGuid, LPCSTR Tag, LPCSTR Path,
                         const CRectangleF & boundingRect);
    
    //----------------------------------------------------------------------------------
    //Nodes
    void AddCreateNode(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path, 
                      LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
                      double Angle, const CRectangleF & tagArea, double tagAngle, bool tagVisible, COLORREF FillColor, 
                      bool MirrorX, bool MirrorY);
    void DoDeleteNode(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid);

    void DoModifyNodePosition(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta);
    void DoModifyTagG(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta, float tagHeight, double tagAngle, bool tagVisible);

    //----------------------------------------------------------------------------------
    //Links
    void AddCreateLink(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path, 
                      LPCSTR ClassId, 
                      LPCSTR OriginMdlGuid, LPCSTR DestinationMdlGuid, 
                      LPCSTR OriginGrfGuid, LPCSTR DestinationGrfGuid, 
                      LPCSTR OriginPort, LPCSTR DestinationPort, 
                      CPointFList & ControlPoints, const CRectangleF & tagArea, double tagAngle, bool tagVisible);
    //void DoDeleteLink(__int64 & requestId, LPCSTR ItemGuid);
    //void DoModifyLink(__int64 & requestId, LPCSTR LinkGuid, LPCSTR Tag, LPCSTR Path, 
    //                  LPCSTR ClassId, 
    //                  LPCSTR OriginGuid, LPCSTR DestinationGuid, 
    //                  LPCSTR OriginPort, LPCSTR DestinationPort, 
    //                  CPointFList & ControlPoints, const CRectangleF & tagArea, float tagAngle);


    //----------------------------------------------------------------------------------
    //Things

    //----------------------------------------------------------------------------------
    //.....

    //void DoModifyItem(__int64 & requestId, LPCSTR ItemGuid, LPCSTR Tag, LPCSTR Path, 
    //                  LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect, 
    //                  float Angle, const CRectangleF & tagArea, COLORREF FillColor, 
    //                  bool MirrorX, bool MirrorY);

    // ..........
    //static void CreateItem(__int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR path, LPCSTR model, LPCSTR stencil, PKRectangleF boundingRect, float angle, int fillColor, int fillMode, bool mirrorX, bool mirrorY);
    //static void CreateLink(__int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR classID, LPCSTR origin, LPCSTR destination, LPCSTR originPort, LPCSTR destinationPort, PKPointF controlPoints[], int controlPointsLength);

    //static void DeleteItem(__int64 requestId, LPCSTR guid);
    //static void DeleteLink(__int64 requestId, LPCSTR guid);

    bool ProcessChangeLists(__int64 & requestId);


    void Load();
    void Save();

    void LogMessage(DWORD Type, LPCSTR Msg);

    //static void ModifyItemBoundingRect(__int64 requestId, LPCSTR guid, PKRectangleF boundingRect);
    //static void ModifyLinkControlPoints(__int64 requestId, LPCSTR guid, PKPointF controlPoints[], int controlPointsLength);

    //void Export(LPCSTR projectPath, LPCSTR configPath);

  public:

    CSvcConnect   * m_pConn;

  };

//========================================================================

#endif

//========================================================================
