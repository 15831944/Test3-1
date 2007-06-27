//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __MD_METHOD_EX_CPP
#include "md_headers_ex.h"

#include "..\\devlib\\bbtransfer.h"
#include "..\\devlib\\bbsurge.h"
#include "..\\devlib\\bbcontrol.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

MUnitDefBaseEx::MUnitDefBaseEx(bool WithDevelopementChecks) : MUnitDefBase(WithDevelopementChecks)
  {
  }

MUnitDefBaseEx::~MUnitDefBaseEx()
  {
  }

void MUnitDefBaseEx::SetModelLicense(dword License)
  {
  MUnitDefBase::SetModelLicense(License);
  m_pClassDef->dwCategory = m_pClassDef->dwCategory &~TOC_SMDKRUNTIME; 
  }

//===========================================================================
//
//
//
//===========================================================================

static LPTSTR MethodBaseVersion="";
static double DefaultDrawing[] = { DD_Poly, -5,-5, -5,5, 5,5, 5,-5, -5,-5, DD_End };

//===========================================================================
//
//
//
//===========================================================================

class CTransferMethodClassDefEx : public TagObjClass
  {
  public:
    TaggedObject * ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
    TaggedObject * ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
    virtual TaggedObject * Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
    CTransferMethodClassDefEx(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClass, pchar pVersion_, pchar pDrwGroup_, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask);
  };

TaggedObject * CTransferMethodClassDefEx::ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrp(pClass, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CTransferMethodClassDefEx::ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrpDesc(pShortDesc, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CTransferMethodClassDefEx::Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  {
  CBBTransfer * pn = new CBBTransfer(this, pTag, pAttach, eAttach);
  SetTypeId(NULL); // will be set by Init
  if (SubClassId())
    pn->SetSubClass(SubClassId());

  return pn;
  };

CTransferMethodClassDefEx::CTransferMethodClassDefEx(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClassId_, pchar pVersion_, pchar pDrwGroup_, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask) : \
TagObjClass(pClassName, pGroup_, pClassId_, pSubClassId_, pVersion_, pDrwGroup_, pDefTag_, dwCat, pShortDesc, pDesc, dwSelectMask)
  {
//    sSubClassId=pSubClassId_;
  sMdlLibName=MDLLIBNAME;
  };

//---------------------------------------------------------------------------

MTransferUnitDefBaseEx::MTransferUnitDefBaseEx(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR DLL, bool WithDevelopementChecks) :
  MUnitDefBaseEx(WithDevelopementChecks)
  {
  m_pClassDef=new CTransferMethodClassDefEx((LPTSTR)pClass, FlwUnitGrp, (LPTSTR)pClass, (LPTSTR)pSubClass, MethodBaseVersion,
                     "Xfer", "X", TOC_PROBAL|TOC_GRP_GENERAL/*|TOC_SMDKRUNTIME*/, (LPTSTR)ShortDesc, (LPTSTR)ShortDesc, 0xffffffff);

  m_pClassDef->SetMdlLibName(DLL);
  m_pClassDef->SetSubConstruct(this);

  m_pDrawEntry = new NodeDrawEntry((LPTSTR)pClass, MethodBaseVersion, "SMDKXfer", DefaultDrawing);

  m_sClassName = pClass;
  }

//===========================================================================
//
//
//
//===========================================================================

class CSurgeMethodClassDefEx : public TagObjClass
    {
    public:
      TaggedObject * ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      TaggedObject * ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      virtual TaggedObject * Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      CSurgeMethodClassDefEx(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClass, pchar pVersion_, pchar pDrwGroup, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask);
    };

TaggedObject * CSurgeMethodClassDefEx::ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrp(pClass, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CSurgeMethodClassDefEx::ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrpDesc(pShortDesc, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CSurgeMethodClassDefEx::Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  {
  CBBSurge * pn = new CBBSurge(this, pTag, pAttach, eAttach);
  SetTypeId(NULL); // will be set by Init
  if (SubClassId())
    pn->SetSubClass(SubClassId());

  return pn;
  };

CSurgeMethodClassDefEx::CSurgeMethodClassDefEx(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClassId_, pchar pVersion_, pchar pDrwGroup, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask) : \
TagObjClass(pClassName, pGroup_, pClassId_, pSubClassId_, pVersion_, pDrwGroup, pDefTag_, dwCat, pShortDesc, pDesc, dwSelectMask)
  {
  //sSubClassId=pSubClassId_;
  sMdlLibName=MDLLIBNAME;
  };

//---------------------------------------------------------------------------

MSurgeUnitDefBaseEx::MSurgeUnitDefBaseEx(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR DLL, bool WithDevelopementChecks) :
  MUnitDefBaseEx(WithDevelopementChecks)
  {
  m_pClassDef= new CSurgeMethodClassDefEx((LPTSTR)pClass, FlwUnitGrp, (LPTSTR)pClass, (LPTSTR)pSubClass, MethodBaseVersion,
                       "Tank", "S", TOC_PROBAL|TOC_GRP_GENERAL/*|TOC_SMDKRUNTIME*/, (LPTSTR)ShortDesc, (LPTSTR)ShortDesc, 0xffffffff);
  m_pClassDef->SetMdlLibName(DLL);
  m_pClassDef->SetSubConstruct(this);

  m_pDrawEntry = new NodeDrawEntry((LPTSTR)pClass, MethodBaseVersion, "SMDKSurge", DefaultDrawing);

  m_sClassName = pClass;
  //m_iMethodFlags = 0;
  }


//===========================================================================
//
//
//
//===========================================================================

class CControlMethodClassDefEx : public TagObjClass
    {
    public:
      TaggedObject * ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      TaggedObject * ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      virtual TaggedObject * Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      CControlMethodClassDefEx(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClass, pchar pVersion_, pchar pDrwGroup, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask);
    };

TaggedObject * CControlMethodClassDefEx::ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrp(pClass, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CControlMethodClassDefEx::ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrpDesc(pShortDesc, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CControlMethodClassDefEx::Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  {
  CBBControl * pn = new CBBControl(this, pTag, pAttach, eAttach);
  SetTypeId(NULL); // will be set by Init
  if (SubClassId())
    pn->SetSubClass(SubClassId());

  return pn;
  };

CControlMethodClassDefEx::CControlMethodClassDefEx(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClassId_, pchar pVersion_, pchar pDrwGroup, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask) : \
TagObjClass(pClassName, pGroup_, pClassId_, pSubClassId_, pVersion_, pDrwGroup, pDefTag_, dwCat, pShortDesc, pDesc, dwSelectMask)
  {
  //sSubClassId=pSubClassId_;
  sMdlLibName=MDLLIBNAME;
  };

//---------------------------------------------------------------------------

MControlUnitDefBaseEx::MControlUnitDefBaseEx(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR DLL, bool WithDevelopementChecks) :
  MUnitDefBaseEx(WithDevelopementChecks)
  {
  m_pClassDef= new CControlMethodClassDefEx((LPTSTR)pClass, FlwUnitGrp, (LPTSTR)pClass, (LPTSTR)pSubClass, MethodBaseVersion,
                      "Ctrl", "C", TOC_PROBAL|TOC_DYNAMICFLOW|TOC_DYNAMICFULL|TOC_GRP_GENERAL/*|TOC_SMDKRUNTIME*/, (LPTSTR)ShortDesc, (LPTSTR)ShortDesc, 0xffffffff);

  m_pClassDef->SetMdlLibName(DLL);
  m_pClassDef->SetSubConstruct(this);

  m_pDrawEntry = new NodeDrawEntry((LPTSTR)pClass, MethodBaseVersion, "SMDKControl", DefaultDrawing);

  m_sClassName = pClass;
  //m_iMethodFlags = 0;
  }

