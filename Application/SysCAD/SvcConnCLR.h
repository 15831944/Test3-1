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
                      double Angle, CSvcTagBlk & TagBlk, COLORREF FillColor, 
                      bool MirrorX, bool MirrorY);
    void AddDeleteNode(__int64 & requestId, LPCSTR GraphicGuid);

    void AddModifyNodePosition(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta);
    void AddModifyNodeSymbol(__int64 & requestId, LPCSTR GraphicGuid, LPCSTR Symbol);
    void AddModifyTagG(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta, CSvcTagBlk & TagBlk);

    //----------------------------------------------------------------------------------
    //Links
    void AddCreateLink(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path, 
                      LPCSTR ClassId, 
                      LPCSTR OriginMdlGuid, LPCSTR DestinationMdlGuid, 
                      LPCSTR OriginGrfGuid, LPCSTR DestinationGrfGuid, 
                      LPCSTR OriginPort, LPCSTR DestinationPort, 
                      CPointFList & ControlPoints, CSvcTagBlk & TagBlk);
    void AddDeleteLink(__int64 & requestId, LPCSTR GraphicGuid);
    void AddModifyLinkPts(__int64 & requestId, LPCSTR GraphicGuid, CPointFList & ControlPoints);


    //----------------------------------------------------------------------------------
    //Things

    //----------------------------------------------------------------------------------

    bool ProcessChangeLists(__int64 & requestId);
    void ProcessChangeListsHold(bool On);


    void Load();
    void Save();
    void SaveAs(LPCSTR name, LPCSTR path);

    void LogMessage(DWORD Type, LPCSTR Msg);

  public:

    CSvcConnect   * m_pConn;

  };

//========================================================================

#endif

//========================================================================
