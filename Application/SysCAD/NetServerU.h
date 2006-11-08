#pragma once

#include "scdver.h"

//========================================================================

#if CREATENETSERVER

//========================================================================

class CGrfTagInfo;
class CLinePointsArray;
class CNSMdlNode;
class CNSMdlLink;
class FlwNode;

//========================================================================

class CNSGrfItem
  {
  public:
    CNSGrfItem();
    CNSGrfItem(LPCTSTR Page);
    ~CNSGrfItem();

  public:
    CString           m_Page;
  };

class CNSGrfNode  : public CNSGrfItem
  {
  public:
    CNSGrfNode();
    CNSGrfNode(LPCTSTR Page, CNSMdlNode * pMdl, CGrfTagInfo & Info);
    ~CNSGrfNode();

  public:

	float			  m_Left;
	float			  m_Top;
	float			  m_Width;
	float			  m_Height;

    float             m_Rotation;

    CNSMdlNode      * m_pMdl;
  };

class CNSGrfLink : public CNSGrfItem
  {
  public:

    CNSGrfLink();
    CNSGrfLink(LPCTSTR Page, CNSMdlLink * pMdl, CGrfTagInfo & Info, CLinePointsArray & LPA);
    virtual ~CNSGrfLink();

  public:

    CNSMdlLink      * m_pMdl;

    class CPt 
      { 
      public: 
        CPt()
          {
          m_X=0;
          m_Y=0;
          };
        CPt(double X, double Y)
          {
          m_X=(float)X;
          m_Y=(float)Y;
          };
      public: 
        float m_X, m_Y; 
      };
    CArray <CPt, CPt&> m_Pts;
  };

class CNSGrfThing  : public CNSGrfItem
  {
  public:
    CNSGrfThing();
    CNSGrfThing(LPCTSTR Tag, LPCTSTR Guid, LPCTSTR Page, CGrfTagInfo & Info);
    ~CNSGrfThing();

  public:

  CString           m_Tag;
  CString           m_Guid;

	float			  m_Left;
	float			  m_Top;
	float			  m_Width;
	float			  m_Height;

    float             m_Rotation;
  };

//========================================================================

class CNSGuidItem
  {
  public:
    CNSGuidItem(LPCTSTR Tag, LPCTSTR Guid, LPCTSTR ClassID) : \
      m_Tag(Tag),
      m_Guid(Guid),
      m_ClassID(ClassID)
      {
      m_IsLink=false;
      //m_IsMdl=false;
      }
    virtual ~CNSGuidItem()
      {
      }
  public:
    CString           m_Tag;
    CString           m_Guid;
    CString           m_ClassID;
    bool              m_IsLink;
    //bool              m_IsMdl;
  };

class CNSMdlNode : public CNSGuidItem
  {
  public:

    CNSMdlNode(LPCTSTR Tag, LPCTSTR Guid, LPCTSTR ClassID);
    virtual ~CNSMdlNode();

    FlwNode         * m_pNd; // eventually use this for direct access 

    CArray <CNSGrfNode*, CNSGrfNode*> m_pGrfs; 

    //

  };

class CNSMdlLink  : public CNSGuidItem
  {
  public:

    CNSMdlLink(LPCTSTR Tag, LPCTSTR Guid, LPCTSTR ClassID, LPCTSTR SrcGuid, LPCTSTR DstGuid, LPCTSTR SrcPort, LPCTSTR DstPort);
    virtual ~CNSMdlLink();

    //CString m_Tag;
    //CString m_Guid;
    //CString m_ClassID;
    CString           m_SrcGuid;
    CString           m_DstGuid;
    CString           m_SrcPort;
    CString           m_DstPort;

	CNSGrfLink      * m_pGrf; 
    //FlwNode * m_pNd;
  };

//========================================================================

class CNETServerU
  {
  public:
    CNETServerU(void);
  public:
    ~CNETServerU(void);

    void LoadItems();
    void UpdateItems();
    void Startup();
    void Shutdown();

    void Add(CNSGuidItem* pMdl);

  public:
    CString   m_PrjName;

    CList <CNSGuidItem*, CNSGuidItem*> m_Guids;
    CList <CNSGrfThing*, CNSGrfThing*> m_Things;
    CMap  <CNSGuidItem*, CNSGuidItem*, LPCTSTR*, LPCTSTR*> m_TagMap;
    CMap  <CNSGuidItem*, CNSGuidItem*, LPCTSTR*, LPCTSTR*> m_GuidMap;

    //CList <CNSMdlNode*, CNSMdlNode*> m_MdlItems;

    //CMap  <CNSMdlNode*, CNSMdlNode*, LPCTSTR*, LPCTSTR*> m_MdlTagMap;
    //CMap  <CNSMdlNode*, CNSMdlNode*, LPCTSTR*, LPCTSTR*> m_MdlGuidMap;
  };

//========================================================================

#endif

//========================================================================
