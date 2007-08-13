#pragma once

#include "scdver.h"

//========================================================================

#if SYSCAD10

//#include "NETServerU.h"

//========================================================================

//struct PKPointF
//{
//  float x;
//  float y;
//};
//
//struct PKRectangleF
//{
//  float x;
//  float y;
//  float w;
//  float h;
//};

class CSvcConnectCLR
  {
  public:
    CSvcConnectCLR(void);

  public:
    ~CSvcConnectCLR(void);

    bool Startup(CSvcConnect * pConn, LPCSTR projectPath, LPCSTR configPath, bool ImportScd9);
    void Shutdown();

    void Sync(__int64 requestId);

    void DoCreateGroup(__int64 & requestId, CString & GroupGuid, LPCSTR Tag, LPCSTR Path, const CRectangleF & boundingRect);

    void DoCreateItem(__int64 & requestId, CString & ItemGuid, LPCSTR Tag, LPCSTR Path, 
                   LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
                   float Angle, const CRectangleF & textArea, COLORREF FillColor, 
                   bool MirrorX, bool MirrorY);

    void DoDeleteItem(__int64 & requestId, LPCSTR ItemGuid);

    //static void CreateItem(__int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR path, LPCSTR model, LPCSTR stencil, PKRectangleF boundingRect, float angle, int fillColor, int fillMode, bool mirrorX, bool mirrorY);
    //static void CreateLink(__int64 requestId, LPCSTR guid, LPCSTR tag, LPCSTR classID, LPCSTR origin, LPCSTR destination, LPCSTR originPort, LPCSTR destinationPort, PKPointF controlPoints[], int controlPointsLength);

    //static void DeleteItem(__int64 requestId, LPCSTR guid);
    //static void DeleteLink(__int64 requestId, LPCSTR guid);

    void Load();
    void Save();

    //static void ModifyItemBoundingRect(__int64 requestId, LPCSTR guid, PKRectangleF boundingRect);
    //static void ModifyLinkControlPoints(__int64 requestId, LPCSTR guid, PKPointF controlPoints[], int controlPointsLength);

    void Export(LPCSTR projectPath, LPCSTR configPath);

  public:

    CSvcConnect   * m_pConn;

  };

//========================================================================

#endif

//========================================================================
