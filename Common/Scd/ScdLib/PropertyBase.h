//#pragma once

#ifndef  __PROPERTYBASE_H
#define  __PROPERTYBASE_H

//---------------------------------------------------------------------------

#include "datacnvs.h"
#include "datatype.h"
#include "fixedit.h"
#include "errorlog.h"
#include "datatype.h"
#include "scdarray.h"
#include <comutil.h>

#if defined(__PROPERTYBASE_CPP)
  #define DllImportExport DllExport
#elif !defined(SCDLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif

class DllImportExport CPropertyMap 
  {
  friend class CPropertyMaps;

  protected:
    CString   m_Name;
    CMap<CString, LPCTSTR, long, long> m_Map;
    CArray<MPropertyInfo*, MPropertyInfo*> m_Props; 

  public:
    CPropertyMap(LPCTSTR Name)                      
      { 
      m_Name=Name;
      m_Map.InitHashTable(501); 
      };
    long          Add(long Index, MPropertyInfo * Info);
    long          Lookup(LPCTSTR Name)           { long i; return m_Map.Lookup(Name, i) ? i:-1; };
    long          Count()                        { return m_Props.GetCount(); };
    MPropertyInfo * operator[](long i)  { return m_Props[i];};
  };

class DllImportExport CPropertyMaps 
  {
  friend class CPropertyMap;
  protected:
    CMap<CString, LPCTSTR, long, long> m_Map2Maps;
    CArray<CPropertyMap*, CPropertyMap*> m_Array; 

  public:
    CPropertyMaps()         { m_Map2Maps.InitHashTable(501); };
    long          Add(CPropertyMap * pMap);
    CPropertyMap * Lookup(LPCTSTR Name) { long i; return m_Map2Maps.Lookup(Name, i) ? m_Array[i]:NULL; };
    long          Count()                        { return m_Array.GetCount(); };
  };

extern DllImportExport CPropertyMaps gs_PropertyMaps;



class DllImportExport CPropertyBase
  {
  public:
    CPropertyBase(void);
    virtual ~CPropertyBase(void);
    CPropertyMap &  PropertyMap();
    long            PropertyCount()                                       { return PropertyMap().Count(); }
    long            PropertyIndex(LPCTSTR Name)                           { return PropertyMap().Lookup(Name); }

    virtual LPCTSTR DefinedPropertyMapName()                              { return NULL; }
    virtual long    DefinedPropertyCount()                                { return 0; }
    virtual long    DefinedPropertyInfo(long Index, MPropertyInfo & Info) { return -1; }


    virtual DWORD   GetPropertyVisibility(long Index) throw(...);
    virtual void    GetPropertyValue(long Index, DWORD Phase, double T, double P, MPropertyValue &Value) throw(...);
    virtual void    PutPropertyValue(long Index, MPropertyValue &Value) throw(...);

    void            GetPropertyValue(LPCTSTR Name, DWORD Phase, double T, double P, MPropertyValue &Value) throw(...);
    void            PutPropertyValue(LPCTSTR Name, MPropertyValue &Value) throw(...);

  };

#undef DllImportExport 

//===========================================================================

#endif