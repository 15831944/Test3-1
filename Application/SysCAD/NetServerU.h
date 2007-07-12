#pragma once

#include "scdver.h"
#include "neutralcommon.h"

//========================================================================

#if SYSCAD10

//========================================================================

struct CRectangleF
  {
  CRectangleF()
    {
    m_Left=0.0;
    m_Right=0.0;
    m_Top=0.0;
    m_Bottom=0.0;
    };
  CRectangleF(double L, double R, double T, double B)
    {
    m_Left=L;
    m_Right=R;
    m_Top=T;
    m_Bottom=B;
    }
  double  MidX() const { return 0.5*(m_Left+m_Right); };
  double  MidY() const { return 0.5*(m_Top+m_Bottom); };

  double   m_Left;
  double   m_Right;
  double   m_Top;
  double   m_Bottom;
  };

void SS_CreateItem(__int64 eventId, __int64 requestId, LPCTSTR guid, LPCTSTR tag, LPCTSTR path, 
                            LPCTSTR model, LPCTSTR shape, const CRectangleF & boundingRect, 
                            //Model^ model, Shape^ shape, RectangleF boundingRect, 
                            double angle, COLORREF Colour, /*System::Drawing::Color fillColor, */
                            bool mirrorX, bool mirrorY);
//  {
////    //Chris: This is an example of handling the ItemCreated event.
////    CreateItem9(eventId, requestId, guid, tag, path, model, shape, boundingRect, angle, fillColor, mirrorX, mirrorY)l
//    int yyy=0;
//  }

//class CGrfTagInfo;
//class CLinePointsArray;
//class CNSMdlNode;
//class CNSMdlLink;
//class FlwNode;
//
////========================================================================
//
//class CNSGrfItem
//  {
//  public:
//    CNSGrfItem();
//    CNSGrfItem(LPCTSTR Page);
//    ~CNSGrfItem();
//
//  public:
//    CString           m_Page;
//  };
//
//class CNSGrfNode  : public CNSGrfItem
//  {
//  public:
//    CNSGrfNode();
//    CNSGrfNode(LPCTSTR Page, CNSMdlNode * pMdl, CGrfTagInfo & Info);
//    ~CNSGrfNode();
//
//  public:
//
//	float			  m_Left;
//	float			  m_Top;
//	float			  m_Width;
//	float			  m_Height;
//
//	float			  m_TagLeft;
//	float			  m_TagTop;
//	float			  m_TagWidth;
//	float			  m_TagHeight;
//
//    float             m_Rotation;
//
//    CNSMdlNode      * m_pMdl;
//  };
//
//class CNSGrfLink : public CNSGrfItem
//  {
//  public:
//
//    CNSGrfLink();
//    CNSGrfLink(LPCTSTR Page, CNSMdlLink * pMdl, CGrfTagInfo & Info, CLinePointsArray & LPA);
//    virtual ~CNSGrfLink();
//
//  public:
//
//    CNSMdlLink      * m_pMdl;
//
//    class CPt 
//      { 
//      public: 
//        CPt()
//          {
//          m_X=0;
//          m_Y=0;
//          };
//        CPt(double X, double Y)
//          {
//          m_X=(float)X;
//          m_Y=(float)Y;
//          };
//      public: 
//        float m_X, m_Y; 
//      };
//    CArray <CPt, CPt&> m_Pts;
//  };
//
//class CNSGrfThing  : public CNSGrfItem
//  {
//  public:
//    CNSGrfThing();
//    CNSGrfThing(LPCTSTR Tag, LPCTSTR Guid, LPCTSTR Page, CGrfTagInfo & Info);
//    ~CNSGrfThing();
//
//  public:
//
//  CString           m_Tag;
//  CString           m_Guid;
//
//	float			  m_Left;
//	float			  m_Top;
//	float			  m_Width;
//	float			  m_Height;
//
//    float             m_Rotation;
//  };
//
////========================================================================
//
//class CNSGuidItem
//  {
//  public:
//    CNSGuidItem(LPCTSTR Tag, LPCTSTR Guid, LPCTSTR ClassID) : \
//      m_Tag(Tag),
//      m_Guid(Guid),
//      m_ClassID(ClassID)
//      {
//      m_IsLink=false;
//      //m_IsMdl=false;
//      }
//    virtual ~CNSGuidItem()
//      {
//      }
//  public:
//    CString           m_Tag;
//    CString           m_Guid;
//    CString           m_ClassID;
//    bool              m_IsLink;
//    //bool              m_IsMdl;
//  };
//
//class CNSMdlNode : public CNSGuidItem
//  {
//  public:
//
//    CNSMdlNode(LPCTSTR Tag, LPCTSTR Guid, LPCTSTR ClassID);
//    virtual ~CNSMdlNode();
//
//    FlwNode         * m_pNd; // eventually use this for direct access 
//
//    CArray <CNSGrfNode*, CNSGrfNode*> m_pGrfs; 
//
//    //
//
//  };
//
//class CNSMdlLink  : public CNSGuidItem
//  {
//  public:
//
//    CNSMdlLink(LPCTSTR Tag, LPCTSTR Guid, LPCTSTR ClassID, LPCTSTR SrcGuid, LPCTSTR DstGuid, LPCTSTR SrcPort, LPCTSTR DstPort);
//    virtual ~CNSMdlLink();
//
//    //CString m_Tag;
//    //CString m_Guid;
//    //CString m_ClassID;
//    CString           m_SrcGuid;
//    CString           m_DstGuid;
//    CString           m_SrcPort;
//    CString           m_DstPort;
//
//	CNSGrfLink      * m_pGrf; 
//    //FlwNode * m_pNd;
//  };
//
////========================================================================
//
//class CNETServerU : public CNeutralGroupTagEditor
//  {
//  public:
//    CNETServerU(void);
//  public:
//    ~CNETServerU(void);
//
//    void LoadItems();
//    void SaveItems(CGrfDoc * pDoc);
//    void UpdateItems();
//    void Startup();
//    void Shutdown();
//
//    void Add(CNSGuidItem* pMdl);
//
//  public:
//    CString   m_PrjName;
//
//    CList <CNSGuidItem*, CNSGuidItem*> m_Guids;
//    CList <CNSGrfThing*, CNSGrfThing*> m_Things;
//    CMap  <CNSGuidItem*, CNSGuidItem*, LPCTSTR*, LPCTSTR*> m_TagMap;
//    CMap  <CNSGuidItem*, CNSGuidItem*, LPCTSTR*, LPCTSTR*> m_GuidMap;
//
//    //CList <CNSMdlNode*, CNSMdlNode*> m_MdlItems;
//
//    //CMap  <CNSMdlNode*, CNSMdlNode*, LPCTSTR*, LPCTSTR*> m_MdlTagMap;
//    //CMap  <CNSMdlNode*, CNSMdlNode*, LPCTSTR*, LPCTSTR*> m_MdlGuidMap;
//  };

//========================================================================

//extern CString FixSymbol(LPCTSTR m_sSymbol, LPCTSTR m_sClassID);


//========================================================================

#endif

//========================================================================
