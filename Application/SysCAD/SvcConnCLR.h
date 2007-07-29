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

    void Startup(CSvcConnect * pConn, char* projectPath, char* configPath, bool ImportScd9);
    void Shutdown();

    void Sync(__int64 requestID);

    //static void CreateItem(__int64 requestID, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, LPCTSTR model, LPCTSTR stencil, PKRectangleF boundingRect, float angle, int fillColor, int fillMode, bool mirrorX, bool mirrorY);
    //static void CreateLink(__int64 requestID, LPCTSTR guid, LPCTSTR tag, LPCTSTR classID, LPCTSTR origin, LPCTSTR destination, LPCTSTR originPort, LPCTSTR destinationPort, PKPointF controlPoints[], int controlPointsLength);

    //static void DeleteItem(__int64 requestID, LPCTSTR guid);
    //static void DeleteLink(__int64 requestID, LPCTSTR guid);

    void Load();
    void Save();

    //static void ModifyItemBoundingRect(__int64 requestID, LPCTSTR guid, PKRectangleF boundingRect);
    //static void ModifyLinkControlPoints(__int64 requestID, LPCTSTR guid, PKPointF controlPoints[], int controlPointsLength);

    void Export(char* projectPath, char* configPath);

  public:

    CSvcConnect   * m_pConn;

  };

//========================================================================

#endif

//========================================================================
