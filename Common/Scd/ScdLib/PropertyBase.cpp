
#include "stdafx.h"
#define __PROPERTYBASE_CPP
#include "sc_defs.h"
#include ".\propertybase.h"

//=========================================================================
//
//
//
//=========================================================================

static DWORD SetPropFlags(DWORD m_dwFlags, DWORD dwFlags)
  {
  if (dwFlags & MP_ConfigProp|MP_UserProp|MP_GlobalProp)
    m_dwFlags = dwFlags;
  else
    m_dwFlags = (m_dwFlags & (MP_ConfigProp|MP_UserProp|MP_GlobalProp)) | dwFlags ;
  return m_dwFlags;
  }

MPropertyInfo::MPropertyInfo()                     
  {
  m_iType     = ePT_Null;
  m_iCnv      = 0;
  m_dMin      = 0;
  m_dMax      = 0;
  m_dwFlags   = 0;
  m_ePgOpt    = ePPg_Null;
  m_bIsObject = false;
  };

MPropertyInfo::MPropertyInfo(ePropertyTypes iType, LPCTSTR sTag, LPCTSTR sSym, short iCnv, LPCTSTR sCnv, double dMin, double dMax, ULONG dwFlags, LPCTSTR sDesc )
  {
  m_iType     = iType;
  m_sTag      = sTag;
  m_sSym      = sSym;
  m_iCnv      = iCnv;
  m_sCnv      = sCnv;
  m_dMin      = dMin;
  m_dMax      = dMax;
  m_dwFlags   = SetPropFlags(m_dwFlags, dwFlags);
  m_sDesc     = sDesc;
  m_ePgOpt    = ePPg_Null;
  //m_bIsObject = false;
  };

void MPropertyInfo::Set(ePropertyTypes iType, LPCTSTR sTag, LPCTSTR sSym, short iCnv, LPCTSTR sCnv, double dMin, double dMax, ULONG dwFlags, LPCTSTR sDesc )
  {
  m_iType     = iType;
  m_sTag      = sTag;
  m_sSym      = sSym;
  m_iCnv      = iCnv;
  m_sCnv      = sCnv;
  m_dMin      = dMin;
  m_dMax      = dMax;
  m_dwFlags   = SetPropFlags(m_dwFlags, dwFlags);
  m_sDesc     = sDesc;
  //m_bIsObject = false;
  };

void MPropertyInfo::Set(ePropertyTypes iType, LPCTSTR sTag, LPCTSTR sSym, MStringValueP * StrValues, ULONG dwFlags, LPCTSTR sDesc )
  {
  m_iType     = iType;
  m_sTag      = sTag;
  m_sSym      = sSym;
  m_iCnv      = 0;
  m_sCnv      = "";
  m_dMin      = 0;
  m_dMax      = 0;
  m_dwFlags   = SetPropFlags(m_dwFlags, dwFlags);
  m_sDesc     = sDesc;
  //m_bIsObject = false;
  for (int i=0; i<1000 && StrValues[i].m_sID!=NULL; i++)
    {}
  m_StrValues.SetSize(i); 
  for (int j=0; j<i; j++)
    {
    m_StrValues[j].m_sID=StrValues[j].m_sID;
    m_StrValues[j].m_iValue=StrValues[j].m_iValue;
    m_StrValues[j].m_dwFlags=StrValues[j].m_dwFlags;
    }
  };


void MPropertyInfo::SetStructName(LPCTSTR Nm, ULONG dwFlags, LPCTSTR TableSuffix) 
  {
  m_sStruct       = Nm; 
  m_sTableSuffix  = TableSuffix;
  m_dwFlags       = SetPropFlags(m_dwFlags, dwFlags);
  m_bIsObject     = TableSuffix!=NULL && strlen(TableSuffix)>0;
  };
void MPropertyInfo::SetText(LPCTSTR Txt, ULONG dwFlags)
  {
  m_sText     = Txt; 
  m_dwFlags   = SetPropFlags(m_dwFlags, dwFlags);
  //m_bIsObject = false;
  };
void MPropertyInfo::SetPage(LPCTSTR Txt, ePropertyPageTypes Pg, ULONG dwFlags) 
  { 
  m_sText     = Txt; 
  m_ePgOpt    = Pg; 
  m_dwFlags   = SetPropFlags(m_dwFlags, dwFlags);
  //m_bIsObject = false;
  };

//=========================================================================
//
//
//
//=========================================================================

CPropertyMaps gs_PropertyMaps;

//=========================================================================
//
//
//
//=========================================================================

long CPropertyMap::Add(long Index, MPropertyInfo * Info)
  { 
  if ((Info->m_dwFlags&(MP_UserProp|MP_ConfigProp|MP_GlobalProp))==0)
    Info->m_dwFlags|=MP_UserProp; //assume MP_UserProp

  m_Props.SetAtGrow(Index, Info); 
  if (m_Props[Index]->m_sTag.GetLength()>0)
    m_Map.SetAt(m_Props[Index]->m_sTag, Index); 
  if (m_Props[Index]->m_sSym.GetLength()>0)
    m_Map.SetAt(m_Props[Index]->m_sSym, Index); 
  //ASSERT_ALWAYS(i==Index, "Bad Property Index", __FILE__, __LINE__); 
  return Index; 
  };

//-------------------------------------------------------------------------

CPropertyMap & CPropertyBase::PropertyMap()
  {
  CPropertyMap * p=gs_PropertyMaps.Lookup(DefinedPropertyMapName());
  if (!p)
    {
    p=new CPropertyMap(DefinedPropertyMapName());
    gs_PropertyMaps.Add(p);
    MPropertyInfo *Info=new MPropertyInfo;
    for (int i=0 ; ; i++)
      {
      switch (DefinedPropertyInfo(i, *Info))
        {
        case MPI_EndOfProps: 
          delete Info; 
          return *p;
        case MPI_MoreProps:   
          break;
        default:
          {
          CString Struct=Info->m_sStruct;
          p->Add(i, Info); 
          Info=new MPropertyInfo;
          Info->m_sStruct=Struct;
          break;
          }
        }
      }
    }
  return *p;
  };

//=========================================================================
//
//
//
//=========================================================================

long CPropertyMaps::Add(CPropertyMap * pM)
  { 
  long i=m_Array.Add(pM); 
  m_Map2Maps.SetAt(pM->m_Name, i); 
  return i; 
  };

//=========================================================================
//
//
//
//=========================================================================

MPropertyValue::operator bool() /*const*/ 
  { 
  switch (m_eType)
    {
    //case ePT_Long:   m_Long=m_Long!=0; break;
    //case ePT_ULong:  m_Long=m_ULong; break;
    case ePT_Double: m_Long=m_Double!=0; break;
    case ePT_String: m_Long=-1; break;
    }
  return m_Long!=0; 
  };     
MPropertyValue::operator int() /*const*/ 
  { 
  switch (m_eType)
    {
    //case ePT_ULong:  m_Long=m_ULong; break;
    case ePT_Double: m_Long=-1; break;
    case ePT_String: m_Long=-1; break;
    }
  return m_Long; 
  };     
MPropertyValue::operator long() /*const*/ 
  { 
  switch (m_eType)
    {
    //case ePT_ULong:  m_Long=m_ULong; break;
    case ePT_Double: m_Long=-1; break;
    case ePT_String: m_Long=-1; break;
    }
  return m_Long; 
  };     
//MPropertyValue::operator ULONG() /*const*/ 
//  { 
//  switch (m_eType)
//    {
//    case ePT_Long:   m_ULong=m_Long; break;
//    case ePT_Double: m_ULong=-1; break;
//    case ePT_String: m_ULong=-1; break;
//    }
//  return m_ULong; 
//  };     
MPropertyValue::operator double() /*const*/ 
  { 
  switch (m_eType)
    {
    case ePT_Bool: m_Double=m_Long!=0; break;
    case ePT_Long: m_Double=m_Long; break;
    //case ePT_ULong: m_Double=m_ULong; break;
    case ePT_String: m_Double=dNAN; break;
    }
  return m_Double; 
  };     
MPropertyValue::operator LPCTSTR() /*const*/ 
  { 
  switch (m_eType)
    {
    case ePT_Bool: m_String=m_Long!=0?"1":"0"; break;
    case ePT_Long: m_String.Format("%i", m_Long); break;
    //case ePT_ULong: m_String.Format("%ui", m_ULong); break;
    case ePT_Double: m_String.Format("%g", m_Double); break;
    }
  return m_String; 
  };     
MPropertyValue::operator LPTSTR() /*const*/ 
  { 
  switch (m_eType)
    {
    case ePT_Bool: m_String=m_Long!=0?"1":"0"; break;
    case ePT_Long: m_String.Format("%i", m_Long); break;
    //case ePT_ULong: m_String.Format("%ui", m_ULong); break;
    case ePT_Double: m_String.Format("%g", m_Double); break;
    }
  return (LPTSTR)(LPCTSTR)m_String; 
  };     

MPropertyValue & MPropertyValue::operator=(bool V) 
  { 
  m_eType=ePT_Bool;
  m_Long=V;
  return *this; 
  };      
MPropertyValue & MPropertyValue::operator=(int V) 
  { 
  m_eType=ePT_Long;
  m_Long=V;
  return *this; 
  };      
MPropertyValue & MPropertyValue::operator=(long V) 
  { 
  m_eType=ePT_Long;
  m_Long=V;
  return *this; 
  };      
MPropertyValue & MPropertyValue::operator=(ULONG V) 
  { 
  m_eType=ePT_Long;//ePT_ULong;
  m_ULong=V;
  return *this; 
  };      
MPropertyValue & MPropertyValue::operator=(double V) 
  { 
  m_eType=ePT_Double;
  m_Double=V;
  return *this; 
  };      
MPropertyValue & MPropertyValue::operator=(LPCTSTR V) 
  { 
  m_eType=ePT_String;
  m_String=V;
  return *this; 
  };      
MPropertyValue & MPropertyValue::operator=(LPTSTR V) 
  { 
  m_eType=ePT_String;
  m_String=V;
  return *this; 
  };      

//=========================================================================
//
//
//
//=========================================================================

CPropertyBase::CPropertyBase(void)
  {
  }

CPropertyBase::~CPropertyBase(void)
  {
  }

//-------------------------------------------------------------------------

/*virtual*/ DWORD CPropertyBase::GetPropertyVisibility(long Index)
  {
  return ePVis_All;
  };

//-------------------------------------------------------------------------

/*virtual*/ void CPropertyBase::GetPropertyValue(long Index, DWORD Phase, double T, double P, MPropertyValue & Value)
  {
  if (Index<0 || Index>=PropertyCount())
    {
    CString S;
    S.Format("%s Index %i not Valid", DefinedPropertyMapName(), Index);
    throw MMdlException(0, (LPCTSTR)S);
    }
  CString S;
  S.Format("%s Index %i Unknown Error", DefinedPropertyMapName(), Index);
  throw MMdlException(0, S);
  };

//-------------------------------------------------------------------------

void CPropertyBase::GetPropertyValue(LPCTSTR Name, DWORD Phase, double T, double P, MPropertyValue & Value)
  {
  long i=PropertyIndex(Name);
  if (i>=0)
    {
    GetPropertyValue(i, Phase, T, P, Value);
    return;
    }
  CString S;
  S.Format("%s '%s' not valid", DefinedPropertyMapName(), Name);
  throw MMdlException(0, (LPCTSTR)S);
  };

//-------------------------------------------------------------------------

/*virtual*/ void CPropertyBase::PutPropertyValue(long Index, MPropertyValue & Value)
  {
  if (Index<0 || Index>=PropertyCount())
    {
    CString S;
    S.Format("%s Index %i not Valid", DefinedPropertyMapName(), Index);
    throw MMdlException(0, (LPCTSTR)S);
    }
  if (!PropertyMap()[Index]->Settable())
    {
    CString S;
    S.Format("%s Index %i not Settable", DefinedPropertyMapName(), Index);
    throw MMdlException(0, S);
    }
  CString S;
  S.Format("%s Index %i Unknown Error", DefinedPropertyMapName(), Index);
  throw MMdlException(0, S);
  };

//-------------------------------------------------------------------------

void CPropertyBase::PutPropertyValue(LPCTSTR Name, MPropertyValue & Value)
  {
  long i=PropertyIndex(Name);
  if (i>=0)
    {
    if (PropertyMap()[i]->Settable())
      PutPropertyValue(i, Value); 
    else
      {
      CString S;
      S.Format("%s '%s' not Settable", DefinedPropertyMapName(), Name);
      throw MMdlException(0, S);
      }
    }
  CString S;
  S.Format("%s '%s' not valid", DefinedPropertyMapName(), Name);
  throw MMdlException(0, (LPCTSTR)S);
  };

//=========================================================================
//
//
//
//=========================================================================

