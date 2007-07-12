#pragma once

#include "scdver.h"
#include "neutralcommon.h"

//========================================================================

#if SYSCAD10

//========================================================================

void SS_CreateItem(bool DoingImport, __int64 eventId, __int64 requestId, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, 
                   LPCTSTR model, LPCTSTR shape, const CRectangleF & boundingRect, 
                   double angle, COLORREF Colour, 
                   bool mirrorX, bool mirrorY);

//bool SS_GetExistingItem(FlwNode * & NdPtr, CString & tag, CString & page, 
//                        CString & model, CString & shape, CRectangleF & boundingRect,
//                        double & angle, bool & mirrorX, bool & mirrorY);

class CGetExistingItems
  {
  public:
    enum eType { eIsError, eIsLink, eIsNode, eIsNull };
    CGetExistingItems();

    bool            GetOne();

    CGrfTagInfo   & Item() { return m_GTIA[m_iInArray]; };
    LPCSTR          Guid() { return m_Guid();           };
    eType           Type() { return m_Type;             }

  protected:
    
    CDocTemplate    & m_GrfTemplate;
    POSITION          m_GrfDocPos;
    CGrfDoc         * m_pDoc;
    CGrfTagInfoArray  m_GTIA;
    int               m_nInArray;
    int               m_iInArray;

    Strng             m_Guid;
    eType             m_Type;

  };



//========================================================================

#endif

//========================================================================
