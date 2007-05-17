#pragma once

#include "scdver.h"

//========================================================================

#if SYSCAD10

//#include "NETServerU.h"

//========================================================================

struct PKPointF
{
  float x;
  float y;
};

struct PKRectangleF
{
  float x;
  float y;
  float w;
  float h;
};

class CNETServer
  {
  public:
    CNETServer();
  public:
    ~CNETServer();

    static void Startup();
    static void Shutdown();

    static void Sync(__int64 requestID);

    static void CreateItem(__int64 requestID, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, LPCTSTR model, LPCTSTR stencil, PKRectangleF boundingRect, float angle, int fillColor, int fillMode, bool mirrorX, bool mirrorY);
    static void CreateLink(__int64 requestID, LPCTSTR guid, LPCTSTR tag, LPCTSTR classID, LPCTSTR origin, LPCTSTR destination, LPCTSTR originPort, LPCTSTR destinationPort, PKPointF controlPoints[], int controlPointsLength);

    static void DeleteItem(__int64 requestID, LPCTSTR guid);
    static void DeleteLink(__int64 requestID, LPCTSTR guid);

    static void Load();
    static void Save();

    static void ModifyItemBoundingRect(__int64 requestID, LPCTSTR guid, PKRectangleF boundingRect);
    static void ModifyLinkControlPoints(__int64 requestID, LPCTSTR guid, PKPointF controlPoints[], int controlPointsLength);

};

//========================================================================

#endif

//========================================================================
