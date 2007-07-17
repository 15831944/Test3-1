#pragma once

#include "scdver.h"
#include "neutralcommon.h"

//========================================================================

#if SYSCAD10

//========================================================================

void SS_CreateItem(bool DoingExport, __int64 eventId, __int64 requestId, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, 
                   LPCTSTR model, LPCTSTR shape, const CRectangleF & boundingRect, 
                   double angle, COLORREF Colour, 
                   bool mirrorX, bool mirrorY);

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
    eType           Type()       { return m_Type;             }

  protected:
    
    int               m_nPages;
    CDocTemplate    & m_GrfTemplate;
    POSITION          m_GrfDocPos;
    CGrfDoc         * m_pDoc;
    CGrfTagInfoArray  m_GTIA;
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
