#pragma once

#include "scdver.h"

//========================================================================

#if SYSCAD10

#include "NETServerU.h"

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

    static void CreateItem(__int64 requestID, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, LPCTSTR model, LPCTSTR stencil, PKRectangleF boundingRect, float angle, int fillColor, int fillMode, bool mirrorX, bool mirrorY);
    static void CreateLink(__int64 requestID, LPCTSTR guid, LPCTSTR tag, LPCTSTR classID, LPCTSTR origin, LPCTSTR destination, LPCTSTR originPort, LPCTSTR destinationPort, PKPointF controlPoints[], int controlPointsLength);
};

//========================================================================

#endif

//========================================================================
