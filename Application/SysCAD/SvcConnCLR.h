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

    //----------------------------------------------------------------------------------
    //Groups
    void AddCreateGroup(__int64 & requestId, LPCSTR GroupGuid, LPCSTR Tag, LPCSTR Path,
                         const CRectangleF & boundingRect);
    
    //----------------------------------------------------------------------------------
    //Nodes
    void AddCreateNode(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path, 
                      LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
                      double Angle, const CSvcTagBlk & TagBlk, COLORREF FillColor, 
                      bool MirrorX, bool MirrorY);
    void AddDeleteNode(__int64 & requestId, LPCSTR GraphicGuid);

    void AddModifyNodePosition(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta);
    void AddModifyTagG(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta, const CSvcTagBlk & TagBlk);

    //----------------------------------------------------------------------------------
    //Links
    void AddCreateLink(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path, 
                      LPCSTR ClassId, 
                      LPCSTR OriginMdlGuid, LPCSTR DestinationMdlGuid, 
                      LPCSTR OriginGrfGuid, LPCSTR DestinationGrfGuid, 
                      LPCSTR OriginPort, LPCSTR DestinationPort, 
                      CPointFList & ControlPoints, const CSvcTagBlk & TagBlk);
    void AddDeleteLink(__int64 & requestId, LPCSTR GraphicGuid);
    void AddModifyLink(__int64 & requestId, LPCSTR LinkGuid, LPCSTR Tag, LPCSTR Path, 
                      LPCSTR ClassId, 
                      const CSvcGuidPair & Guids, 
                      LPCSTR OriginPort, LPCSTR DestinationPort, 
                      CPointFList & ControlPoints, const CSvcTagBlk & TagBlk);


    //----------------------------------------------------------------------------------
    //Things

    //----------------------------------------------------------------------------------

    bool ProcessChangeLists(__int64 & requestId);


    void Load();
    void Save();

    void LogMessage(DWORD Type, LPCSTR Msg);

  public:

    CSvcConnect   * m_pConn;

  };

//========================================================================

#endif

//========================================================================
