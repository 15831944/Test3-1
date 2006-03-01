//**************************************************************************
//
//            Copyright (c) FactorySoft, INC. 1997, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Callback.cpp
//  $Author    :  Jim Hansen
//
//  Subsystem  :  Callback object for OPC Server DLL
//
//  Description:  This class interfaces to the application's data
//
//**************************************************************************

#include "stdafx.h"
#include <afxtempl.h>
#include "Callback.h"
#include "OPCError.h"
#include "OPCProps.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//*******************************************************************
CPLC5OPCTag::CPLC5OPCTag(CPLC5 * Plc, LPCTSTR name, Branch* branch)
  : m_name(name),
  m_Plc(Plc),
  m_Add(Plc)
{
   m_fullname = branch->GetPath() + m_name;
   CAddInfo *pAI=Plc->FindSymbol((LPTSTR)(LPCTSTR)m_name);
   if (pAI)
     m_Add.Parse((LPTSTR)pAI->Address());
   else
     m_Add.Parse((LPTSTR)(LPCTSTR)m_name);
}

//*******************************************************************
// Recursive function to search for a fully qualified tag name
CPLC5OPCTag* Branch::FindTag( const CString& target )
{
   int delimiter = target.Find( _T('.') );
   if( delimiter == -1 )   // tag name if no delimiter
   {
      POSITION pos = m_tags.GetHeadPosition();
      while( pos )
      {
         CPLC5OPCTag* pTag = m_tags.GetNext( pos );
         if( pTag->m_name.CompareNoCase( target ) == 0 )
            return pTag;
      }
   }
   else
   {
      CString branchName( target.Left( delimiter ) );
      POSITION pos = m_branches.GetHeadPosition();
      while( pos )
      {
         Branch* pBranch = m_branches.GetNext( pos );
         if( pBranch->m_name.CompareNoCase( branchName ) == 0 )
            return pBranch->FindTag( target.Mid( delimiter+1 ) );
      }
   }
   return NULL;
}

//*******************************************************************
CString Branch::GetPath()
{
   if( m_parent )
   {
     if (!m_SkipOnRebuild)
      return CString(m_parent->GetPath() + m_name + ".");
     else
      return CString(m_parent->GetPath());
   }

   return "";
}

//*******************************************************************

inline void SplitName(LPCTSTR Name, CString &sPlcName, CString &sName)
  {
  sPlcName=Name;
  int iDot=sPlcName.Find('.');
  if (iDot>=0)
    {
    sName=sPlcName.Right(sPlcName.GetLength()-iDot-1);
    sPlcName=sPlcName.Left(iDot);
    }
  else
    {
    sPlcName="";
    sName=Name;
    }
  }

inline CPLC5OPCTag * TryMakeTag(LPCTSTR Name, Branch * pBrnch, CPLC5Array & Plcs)
  {
  CString sPlcName, sName;
  SplitName(Name, sPlcName, sName);
  StripAddressZeros(sName);

  int iPlc=Plcs.Find(sPlcName);
  if (iPlc<0)
    return NULL;
  
    

  CPLC5*Plc=&Plcs[iPlc];
  CPLC5OPCTag * pTag=new CPLC5OPCTag(Plc, sName, pBrnch);
  if (pTag->m_Add.m_IsValid)
    {
    VARTYPE Typ=pTag->VarType();//(strnicmp(sName, "F8", 2)==0) ? VT_R4 : VT_I2;
    pTag->m_nativeType = Typ;
    pTag->m_value.vt = Typ;
    pBrnch->AddTag( pTag );
    }
  else
    {
    delete pTag;
    pTag=NULL;
    }
  return pTag;
  }

inline CPLC5OPCTag * MakeTag(LPCTSTR Name, WORD Typ, Branch * pBrnch, CPLC5Array &  Plcs)
  {
  CString sPlcName, sName;
  SplitName(Name, sPlcName, sName);
  StripAddressZeros(sName);

  int iPlc=Plcs.Find(sPlcName);
  if (iPlc<0)
    return NULL;

  CPLC5*Plc=&Plcs[iPlc];
  CPLC5OPCTag * pTag=new CPLC5OPCTag(Plc, sName, pBrnch);
  pTag->m_nativeType = Typ;
  pBrnch->AddTag( pTag );
  return pTag;
  }

CMyOPCCallBack::CMyOPCCallBack(CPLC5Array & Plc) :
  m_root( "PLC5" , true),
  m_Plc(Plc)
  {
   //Branch* pBranch = NULL;
  for (int p=0; p<Plc.GetSize(); p++)
    {
    CPLC5 & P=Plc[p];
    Branch * pPlc=new Branch(P.m_sPlcName, false);
    m_root.AddBranch(pPlc);
    for (int t=0; t<P.m_nTables; t++)
      {
      if (t!=2)    // Skip the S 
        {
        CTable & T=*P.m_Table[t];
        if (P.m_Table[t])
          {
          Branch * pTab=new Branch(T.m_sName, true);
          pPlc->AddBranch(pTab);
          for (int v=0; v<T.NVals(); v++)
            {
            CString V;
            V.Format(T.OctalAdd()?"%s:%02o":"%s:%03i", T.m_sName, v);
//            V.Format(T.OctalAdd()?"%02o":"%03i", T.m_sName, v);
            CPLC5OPCTag *pTg=new CPLC5OPCTag(&Plc[p], V, pTab);

            pTab->AddTag(pTg);
            }
          }
        }
      }
    }
  }

CMyOPCCallBack::~CMyOPCCallBack()
{
}

//*******************************************************************
COPCBrowser* CMyOPCCallBack::CreateBrowser()
  {
  return new ShellBrowser(this);
  }

//*******************************************************************
// SetUpdateRate returns a modified update rate
DWORD CMyOPCCallBack::SetUpdateRate( DWORD newUpdateRate )
{
   if( newUpdateRate > 10 )
      return newUpdateRate;
   else
      return 10;
}

//*******************************************************************
// AddTag creates a new tag or returns a pointer to an existing tag.
//
// Return NULL if the tag could not be created (bad name or accessPath)
// or the requested type is incompatible. The requested type is for this
// client, do not change the tag's type.
CTag* CMyOPCCallBack::AddTag(
                           LPCTSTR name,
                           LPCTSTR accessPath,
                           VARTYPE requestedType)
  {
  CSLock wait( &m_CS );    // protect tag access

  CString sName(name);
  StripAddressZeros(sName);

  CPLC5OPCTag* pTag = m_root.FindTag(sName);

  if (pTag)
    return pTag;

  // Initialize the tag's type, etc.
  // should store in a list
  if (pTag==NULL)
    {
    pTag=TryMakeTag(sName, &m_root/*pData*/, m_Plc);
    }

  return (CTag*)pTag;
  }

//*******************************************************************
// ValidateTag modifies pTag to contain the nativeType and access rights
// if successful. If the name and access path do not specify a valid tag,
// then return OPC_E_UNKNOWNITEMID or OPC_E_UNKNOWNPATH.
// If the data type is incompatible, return OPC_E_BADTYPE.
HRESULT CMyOPCCallBack::ValidateTag(
                           CTag* pTag,
                           LPCTSTR name,
                           LPCTSTR accessPath,
                           VARTYPE requestedType)
  {
  CSLock wait( &m_CS );    // protect tag access

  CString sName(name);
  StripAddressZeros(sName);

  CPLC5OPCTag* pFoundTag = m_root.FindTag(sName);
  if( pFoundTag )
    {
    pTag->m_nativeType = pFoundTag->m_nativeType;
    pTag->m_accessRights = pFoundTag->m_accessRights;
    // can use VariantChangeType to test data types:
    // get a current value for this tag and convert it to the requested type.
    // If VariantChangeType returns an error, return OPC_E_BADTYPE.
    return S_OK;
    }
  return OPC_E_UNKNOWNITEMID;
  }

//*******************************************************************
// Remove is called when tags are released. This function must check
// if the tags are in use if AddTag may ever return a pointer to an
// existing tag. Additional clean up should occur here.
HRESULT CMyOPCCallBack::Remove(
    DWORD           dwNumItems,
    CTag         ** ppTags)
{
   CSLock wait( &m_CS );    // protect tag access
   for( DWORD index=0; index<dwNumItems; index++ )
   {
 //     // Not needed because CPLC5OPCTags are static
 //     CTag* pTag = ppTags[index];
 //     if( !pTag->InUse() )
 //       delete pTag;
   }
   return S_OK;
}

//*******************************************************************
LPCTSTR CMyOPCCallBack::GetTagName( CTag * pTag )
{
   return ((CPLC5OPCTag*)pTag)->m_fullname;
}

//*******************************************************************
// GetTagLimits should return TRUE if this tags has known limits.
BOOL CMyOPCCallBack::GetTagLimits(  CTag * pTag, double *pHigh, double *pLow )
{
   *pHigh = 1.0;
   *pLow = -1.0;
   return TRUE;
}

//*******************************************************************
const double pi = 3.1415926535;

HRESULT CMyOPCCallBack::ReadTag(
    CTag * pTag)
{
   //if (dbgf)
   //  fprintf(dbgf, "ReadTag %i",pTag->m_nativeType);

   CPLC5OPCTag* pPlcTag = (CPLC5OPCTag*)(pTag);
   
   // TRACE2(">>> %8.2f %s\n", GetTickCount()/1000.0, (LPCTSTR)pPlcTag->m_name);
   
   CSLock wait( &m_CS );    // protect tag access
   VariantClear( &pTag->m_value );
   pTag->m_value.vt = pTag->m_nativeType;
   switch( pTag->m_nativeType )
     {
       case VT_EMPTY:
       case VT_NULL:
       case VT_BSTR:
          {
          CTime now( CTime::GetCurrentTime() );
          CString temp( now.Format(_T("%S Seconds")) );
          pTag->m_value.bstrVal = temp.AllocSysString();
          }
          break;
       case VT_BOOL:
          if (pPlcTag && pPlcTag->m_Add.m_IsValid)
            pTag->m_value.boolVal = pPlcTag->m_Add.BValue() ? VARIANT_TRUE : VARIANT_FALSE;
          else
            pTag->m_value.boolVal = VARIANT_FALSE;
          break;
       case VT_UI1: // uchar
          if (pPlcTag && pPlcTag->m_Add.m_IsValid)
            pTag->m_value.bVal = (unsigned char)pPlcTag->m_Add.IValue();
          else
            pTag->m_value.bVal = 0;
          break;
       case VT_I2 :   // short
          if (pPlcTag && pPlcTag->m_Add.m_IsValid)
            pTag->m_value.iVal = pPlcTag->m_Add.IValue();
          else
            pTag->m_value.iVal = 0;
          break;
       case VT_I4 :  // long
          if (pPlcTag && pPlcTag->m_Add.m_IsValid)
            pTag->m_value.lVal = pPlcTag->m_Add.IValue();
          else
            pTag->m_value.lVal = 0;
          break;
       case VT_R4 :
          if (pPlcTag && pPlcTag->m_Add.m_IsValid)
            pTag->m_value.fltVal = pPlcTag->m_Add.ValueAsFlt();
          else
            pTag->m_value.fltVal = 0;
          break;
       case VT_R8 :
          if (pPlcTag && pPlcTag->m_Add.m_IsValid)
            pTag->m_value.dblVal = pPlcTag->m_Add.ValueAsFlt();
          else
            pTag->m_value.dblVal = 0;
          break;
       default: ASSERT( FALSE );
     }
   CoFileTimeNow( &pTag->m_timestamp );
   pTag->m_quality = OPC_QUALITY_GOOD;

   //if (dbgf)
   //  fprintf(dbgf, ">> %i\n",pTag->m_value.vt);

   return S_OK;
}

//*******************************************************************
// The variant may contain data in any format the client sent.
// Use VariantChangeType to convert to a usable format.
HRESULT CMyOPCCallBack::WriteTag(
    CTag    * pTag,
    VARIANT & value)
  {

  CPLC5OPCTag* pPlcTag = (CPLC5OPCTag*)(pTag);

  HRESULT hr=VariantChangeType(&pPlcTag->m_value, &value, 0, pPlcTag->m_nativeType);
  if (0 && dbgf)
    fprintf(dbgf, "WrtTag hr:%08x %2i %2i %2i %s\n", 
            hr, 
            pPlcTag->m_value.vt, 
            value.vt, 
            pPlcTag->m_nativeType, 
            pPlcTag->m_Add.Text());
  if (SUCCEEDED(hr))
    {
    if (pPlcTag)
      {
      switch( pPlcTag->m_value.vt)
        {
        case VT_BOOL:
          {
          pPlcTag->m_Add.SetBValue(pPlcTag->m_value.bVal!=0, NULL, "OPC Write");
          break;
          }
        case VT_I2:
          pPlcTag->m_Add.SetIValue(pPlcTag->m_value.iVal);
          break;
        case VT_I4:
          pPlcTag->m_Add.SetIValue((short)value.lVal);
          break;
        case VT_BSTR:
          break;
        case VT_R4:
          pPlcTag->m_Add.SetFValue(value.fltVal);
          break;
        case VT_R8:
          pPlcTag->m_Add.SetFValue((float)value.dblVal);
          break;
        default:
          {
          int x=0;
          }
          break;
        }
      }
    }

  return hr;
  }

//*******************************************************************
// If the name is valid, return the number of item properties supported.
// If this returns successfully, QueryAvailableProperties will be called.
// To save lookups, put a hint into ppVoid.
HRESULT CMyOPCCallBack::QueryNumProperties(
                              LPCTSTR     name,
                              DWORD     * pdwNumItems,
                              LPVOID    * ppVoid)
{

  CString sName(name);
  StripAddressZeros(sName);


   CPLC5OPCTag* pFoundTag = m_root.FindTag(sName);
   if( pFoundTag )
   {
      *pdwNumItems = 6;
      *ppVoid = (LPVOID)pFoundTag;
      return S_OK;
   }
   return OPC_E_UNKNOWNITEMID;
}

//*******************************************************************
// The properties (from QueryNumProperties) now get filled in.
// pVoid contains the hint from QueryNumProperties.
// AllocString allocates COM memory to hold a string.
HRESULT CMyOPCCallBack::QueryAvailableProperties(
                              LPCTSTR     name,
                              DWORD       dwNumItems,
                              LPVOID      pVoid,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pDescriptions,
                              VARTYPE   * pDataTypes)
{
   if( pVoid == NULL )
      return OPC_E_UNKNOWNITEMID;

   CPLC5OPCTag* pTag = (CPLC5OPCTag*)pVoid;
   CString description;
   DWORD index=0;

   description = _T("Item Canonical DataType");
   pPropertyIDs[index] = OPC_PROP_CDT;
   pDescriptions[index] = AllocString(description);
   pDataTypes[index] = VT_I2;

   index++;
   if( index==dwNumItems ) return S_OK;
   description = _T("Item Value");
   pPropertyIDs[index] = OPC_PROP_VALUE;
   pDescriptions[index] = AllocString(description);
   pDataTypes[index] = pTag->m_nativeType;

   index++;
   if( index==dwNumItems ) return S_OK;
   description = _T("Item Quality");
   pPropertyIDs[index] = OPC_PROP_QUALITY;
   pDescriptions[index] = AllocString(description);
   pDataTypes[index] = VT_I2;

   index++;
   if( index==dwNumItems ) return S_OK;
   description = _T("Item Timestamp");
   pPropertyIDs[index] = OPC_PROP_TIME;
   pDescriptions[index] = AllocString(description);
   pDataTypes[index] = VT_DATE;

   index++;
   if( index==dwNumItems ) return S_OK;
   description = _T("Item Access Rights");
   pPropertyIDs[index] = OPC_PROP_RIGHTS;
   pDescriptions[index] = AllocString(description);
   pDataTypes[index] = VT_I4;

   index++;
   if( index==dwNumItems ) return S_OK;
   description = _T("Item Description");
   pPropertyIDs[index] = OPC_PROP_DESC;
   pDescriptions[index] = AllocString(description);
   pDataTypes[index] = VT_BSTR;

   return S_OK;
}

//*******************************************************************
HRESULT CMyOPCCallBack::GetItemProperties(
                              LPCTSTR     name,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              VARIANT   * pData,
                              HRESULT   * pErrors)
{

   CString sName(name);
   StripAddressZeros(sName);

   CPLC5OPCTag* pTag = m_root.FindTag(sName);
   if( !pTag )
      return OPC_E_UNKNOWNITEMID;

   DATE date;
   WORD dosDate=0, dosTime=0;
   for( DWORD index=0; index<dwNumItems; index++ )
   {
      pErrors[index] = S_OK;
      switch(pPropertyIDs[index])
      {
         case OPC_PROP_CDT:
            pData[index].vt = VT_I2;
            pData[index].iVal = pTag->m_nativeType;
            break;
         case OPC_PROP_VALUE:
            VariantCopy( &pData[index], &pTag->m_value );
            break;
         case OPC_PROP_QUALITY:
            pData[index].vt = VT_I2;
            pData[index].iVal = pTag->m_quality;
            break;
         case OPC_PROP_TIME:
            pData[index].vt = VT_DATE;
            FileTimeToDosDateTime( &pTag->m_timestamp, &dosDate, &dosTime);
            DosDateTimeToVariantTime( dosDate, dosTime, &date);
            pData[index].date = date;
            break;
         case OPC_PROP_RIGHTS:
            pData[index].vt = VT_I4;
            pData[index].lVal = OPC_READABLE | OPC_WRITEABLE;
            break;
         case OPC_PROP_DESC:
            {
            CString description(_T("Item Description goes here"));
            pData[index].vt = VT_BSTR;
            pData[index].bstrVal = description.AllocSysString();
            }
            break;
      }
   }
   return S_OK;
}

//*******************************************************************
// return a string if the error code is recognized
LPCTSTR CMyOPCCallBack::GetErrorString(
                              HRESULT     dwError,
                              LCID        dwLocale)
{
   error = _T("Vendor Error string");
   return error;
}

//*******************************************************************
// return your vendor string
LPCTSTR CMyOPCCallBack::GetVendorString()
{
   return _T("Kenwalt (Pty) Ltd.");
}
//*******************************************************************
//
//*******************************************************************
// Do The Command

//void CMyOPCCallBack::StartTheCommand()
//  {
//
//
//  }
//
//void CMyOPCCallBack::FinishTheCommand(long Rsp)
//  {
//  m_Rsp=Rsp;
//  // Done
//  m_Cmd=0;
//  m_DataRqd=0;
//  m_DataRcvd=0;
//  int xx=0;
//  }

//*******************************************************************
//  ShellBrowser class
//*******************************************************************
OPCNAMESPACETYPE ShellBrowser::QueryOrganization()
{
   return OPC_NS_HIERARCHIAL;
}


//*******************************************************************
HRESULT ShellBrowser::GetNames(  OPCBROWSETYPE type,
                   LPCTSTR stringFilter,
                   VARTYPE dataTypeFilter,
                   DWORD accessFilter )
{
   m_type = type;
   m_stringFilter = stringFilter;
   m_dataTypeFilter = dataTypeFilter;
   m_accessFilter = accessFilter;
   if( m_type == OPC_FLAT )
   {
      m_paths.RemoveAll();
      m_pBranch = &m_parent->m_root;
      AddTags( m_pBranch );
   }
   Reset();
   return S_OK;
}

//*******************************************************************
// Recursive function to add tag names from all groups to a list.
// This is only called when browsing OPC_FLAT
// Added 2.0
void  ShellBrowser::AddTags( Branch* pBranch )
{
   // First add full path names for this group's tags
   CString path(pBranch->GetPath());
   POSITION pos = pBranch->m_tags.GetHeadPosition();
   while( pos )
   {
      CPLC5OPCTag* pTag = pBranch->m_tags.GetNext( pos );
      CString name( path + pTag->m_name );
      m_paths.AddTail( name );
   }

   // And recurse into the child groups
   pos = pBranch->m_branches.GetHeadPosition();
   while( pos )
   {
      AddTags( pBranch->m_branches.GetNext( pos ) );
   }
}

//**************************************************************************
BOOL ShellBrowser::MoveUp()
{
   CSLock wait( &m_parent->m_CS );
   if( m_pBranch->m_parent )
   {
      m_pBranch = m_pBranch->m_parent;
      Reset();
      return TRUE;
   }
   // at the "root" level, can't go up
   return FALSE;
}

//**************************************************************************
BOOL ShellBrowser::MoveDown(LPCTSTR branch)
{
   CSLock wait( &m_parent->m_CS );
   POSITION pos = m_pBranch->m_branches.GetHeadPosition();
   while( pos )
   {
      Branch* pBranch = m_pBranch->m_branches.GetNext( pos );
      if( pBranch->m_name.CompareNoCase( branch ) == 0 )
      {
         if (pBranch->m_tags.GetCount()==0 && pBranch->m_branches.GetCount()==0)
           {

           }
         m_pBranch = pBranch;
         Reset();
         return TRUE;
      }
   }
   return FALSE;
}

//*******************************************************************
// if name is valid, return the path+name
LPCTSTR ShellBrowser::GetItemID( LPCTSTR name )
{
   CSLock wait( &m_parent->m_CS );
   // Search for the name
   CPLC5OPCTag* pTag = m_pBranch->FindTag(name);
   if( !pTag )
      return "";

   // The name is valid
   _tcscpy( m_name, (m_pBranch->GetPath() + name) );
   return m_name;
}

//*******************************************************************
LPCTSTR ShellBrowser::Next()
{
   CSLock wait( &m_parent->m_CS );
   while( m_pos )
   {
      if( m_type==OPC_BRANCH )
      {
         Branch* pBranch = m_pBranch->m_branches.GetNext(m_pos);
         _tcscpy( m_name, pBranch->m_name );
         if( m_stringFilter.IsEmpty() || MatchPattern( m_name, m_stringFilter, FALSE) )
            return m_name;
      }
      else if( m_type==OPC_LEAF )
      {
         CPLC5OPCTag* pTag = m_pBranch->m_tags.GetNext(m_pos);
         _tcscpy( m_name, pTag->m_name );
         if( m_stringFilter.IsEmpty() || MatchPattern( m_name, m_stringFilter, FALSE) )
            return m_name;
      }
      else if( m_type == OPC_FLAT )
      {
         CString name = m_paths.GetNext( m_pos );
         _tcscpy( m_name, name );
         if( m_stringFilter.IsEmpty() || MatchPattern( m_name, m_stringFilter, FALSE) )
            return m_name;
      }
      else
         m_pos = NULL;
   }
   return _T("");
}

//*******************************************************************
void ShellBrowser::Reset()
{
   CSLock wait( &m_parent->m_CS );
   if( m_type==OPC_BRANCH )
      m_pos = m_pBranch->m_branches.GetHeadPosition();
   else if( m_type==OPC_LEAF )
      m_pos = m_pBranch->m_tags.GetHeadPosition();
   else if( m_type == OPC_FLAT )
      m_pos = m_paths.GetHeadPosition();
}

