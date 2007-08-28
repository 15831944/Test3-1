//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __NDTREE_H
#define  __NDTREE_H


#include "sc_defs.h"
#include "datacnvs.h"
#include "nodedraw.h"
#include "elecbase.h"
#include "tagobj.h"
#include "sfe_base.h"
#include "pgm_e.h"
#include "xrefs.h"

#ifdef __NDTREE_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

// ===========================================================================

//_FWDDEF(CTNode)
class CTNode;

// ===========================================================================

#define   MaxTagDepth      20

// ----------------------------------------------------------

typedef CMap <char*, char*, CTNode*, CTNode*> CNdTagMap; 

// ----------------------------------------------------------

class DllImportExport CTNode: public TaggedObject, public CNodeXRefMngr
  {
  private:
    static CExecObj   * sm_pExecObj;
  public:
    static void SetExecObj(CExecObj * p) { sm_pExecObj=p; };
    static CExecObj   * ExecObj() { return sm_pExecObj; };


    CTNode(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~CTNode();
    void                AttachClassInfo(DWORD Class_);

    virtual void        SetActiveHold(flag Hold);

    virtual void        BuildDataDefn(DataDefnBlk & DDB);
    virtual TaggedObject * FindObjTag(pchar pSrchTag, flag SrchAll, int &ObjTagLen, int MinTagLen);
    pchar               TagOfParent() { return MyParent ? MyParent->Tag() : NULL; };

    //virtual LPSTR       Tag();
    virtual LPSTR       SetTag(LPSTR ReqdTag, bool AllowEmptyTag=false);

    //virtual LPSTR       Guid();
    virtual LPSTR       SetGuid(LPSTR ReqdGuid);

    CTNode*             Insert(CTNode*p, char IsInternal=1);
    CTNode*             Parent() { return MyParent;};
    CTNode*             FamilyHead() { return (MyParent ? MyParent->FamilyHead() : this);};
    void                Remove_MyChildren();
    void                Remove_Child(CTNode* pRem);
    void                Add_Child(CTNode* pAdd);

    char                GetObj(CTNode* &p) { p = ((p) ? p->Nxt : MyChildren); return (p!=NULL);};
    char                GetObj_All(CTNode* &p);
    pchar               InternalNodeTag();
    void                ShowStruct(char LclCall);

    void                SplitTag(pchar TagIn, SCDPartTag &FirstPart, pchar &Remainder);
    
    virtual CTNode*     FindObj(pchar TagIn);
    virtual CTNode*     FindObjGuid(pchar GuidIn);
    virtual flag        GetDataObj(pTaggedObject &p) { p = ((p) ? ((CTNode*)p)->Nxt : MyChildren); return (p!=NULL);};
    virtual pchar       FullTag();


    CNodeElectrics      m_Electrics;

    virtual void        OnElecComponentChange(eElecChanges EC, CECompData *pC) {};
    virtual void        CollectElectrics(CNodeElectricsArray & TSCA);
    virtual void        ConnectElectrics();
    virtual long        GetElectricsTopoCount()  { return m_Electrics.GetTopoCount(); };
    virtual void        BumpElectricsTopoCount() { return m_Electrics.BumpTopoCount(); };

    virtual void        MyTagsHaveChanged(bool FirstOne=true);


  public:
    void                Print_Node_Stats();
    void                Print_Family(int Lvl=0);

    DEFINE_CI(CTNode, TaggedObject, 4);

    DWORD               m_dwType;
    byte                Internal:1, Done:2, Busy:1, Break:1,
                        Done2:1, Reverse:1, Copied:1, Copy:1, AllDataHere:1;
    int                 NdInx;
    CTNode            * MyParent, *MyChildren, *Nxt, *LastChild;
    
    CNdTagMap         * m_pChildTagMap;
    static CNdTagMap  * sm_pChildGuidMap;

  protected:
    virtual CTNode*     FindChildTag(pchar pTag);
  };

#define IMPLEMENT_NODE(ClassName, Name, Version, TagInitialID, Cat, ShortDesc, Desc) \
  IMPLEMENT_TAGOBJ(ClassName, "Node", Name, Version, TagInitialID, Cat, ShortDesc, Desc)

// ===========================================================================

#undef DllImportExport 

#endif




