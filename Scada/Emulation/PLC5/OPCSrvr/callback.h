//**************************************************************************
//**************************************************************************
//
//            Copyright (c) FactorySoft, INC. 1997, All Rights Reserved
//
//**************************************************************************
//
//  Filename   :  Callback.h
//  $Author    :  Jim Hansen
//
//  Subsystem  :  Callback object for OPC Server DLL
//
//  Description:  This class interfaces to the application's data
//
//**************************************************************************

#if !defined(_SHELLCALLBACK_H_INCLUDED_)
#define _SHELLCALLBACK_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OPCDa.h"
#include "scdopcsrv.h"
#include "..\PLC.h"
class Branch;

//*******************************************************************
// Override CTag to add a name 
class CPLC5OPCTag : public CTag
{
public:
   CPLC5OPCTag(CPLC5 * Plc, LPCTSTR name, Branch* branch);

   CString   m_name;
   CString   m_fullname;
   CPLC5   * m_Plc;
   CAddress  m_Add;

   VARTYPE   VarType() { return m_Add.VarType(); };
};

typedef CTypedPtrList<CPtrList, CPLC5OPCTag*> TagList;
typedef CTypedPtrList<CPtrList, Branch*> BranchList;

//*******************************************************************
// Branch class to hold branches and leaves

class Branch
{
public:
   Branch( LPCTSTR name, BOOL SkipOnRebuild) :
      m_name(name),
      m_parent(NULL)
        {
        m_SkipOnRebuild=SkipOnRebuild;
        }
   ~Branch()
      { while( !m_tags.IsEmpty() )
           delete m_tags.RemoveTail();
        while( !m_branches.IsEmpty() )
           delete m_branches.RemoveTail();
      }
   void AddTag( CPLC5OPCTag* pTag )
     {
     m_tags.AddTail(pTag);
     }
   void AddBranch( Branch* pBranch)
     {
     m_branches.AddTail(pBranch);
     pBranch->m_parent = this;
     }
   CPLC5OPCTag* FindTag( const CString& target );
   CString   GetPath();

   CString     m_name;
   TagList     m_tags;
   BranchList  m_branches;
   Branch*     m_parent;
   BOOL        m_SkipOnRebuild;

};

//*******************************************************************

class CMyOPCCallBack : public COPCCallback
  {
  friend class ShellBrowser;

  public:
    CMyOPCCallBack(CPLC5Array & Plc);
    virtual ~CMyOPCCallBack();

    // New 2.0 Item Properties
    virtual HRESULT QueryNumProperties(
                              LPCTSTR     name,
                              DWORD     * pdwNumItems,
                              LPVOID    * ppVoid);
    virtual HRESULT QueryAvailableProperties(
                              LPCTSTR     name,
                              DWORD       dwNumItems,
                              LPVOID      pVoid,
                              DWORD     * pPropertyIDs,
                              LPWSTR    * pDescriptions,
                              VARTYPE   * pDataTypes);
    virtual HRESULT GetItemProperties(
                              LPCTSTR     name,
                              DWORD       dwNumItems,
                              DWORD     * pPropertyIDs,
                              VARIANT   * pData,
                              HRESULT   * pErrors);

    virtual DWORD SetUpdateRate( DWORD newUpdateRate );
    virtual COPCBrowser* CreateBrowser();
    virtual CTag* AddTag(   LPCTSTR name,
                           LPCTSTR accessPath,
                           VARTYPE requestedType);
    virtual HRESULT ValidateTag(
                           CTag* pTag,
                           LPCTSTR name,
                           LPCTSTR accessPath,
                           VARTYPE requestedType);
    virtual HRESULT Remove(
                           DWORD       dwNumItems,
                           CTag     ** ppTags);
    virtual LPCTSTR GetTagName( CTag * pTag );
    virtual BOOL    GetTagLimits(  CTag * pTag, double *pHigh, double *pLow );
    /*
    virtual HRESULT Read(
                           DWORD       dwNumItems,
                           CTag     ** ppTags,
                           HRESULT   * pErrors);
    */
    virtual HRESULT ReadTag(CTag      * pTag);
    /*
    virtual HRESULT Write(
                           DWORD       dwNumItems,
                           CTag     ** ppTags,
                           VARIANT   * pItemValues,
                           HRESULT   * pErrors);
    */
    virtual HRESULT WriteTag(
                           CTag      * pTag,
                           VARIANT   & value);
    virtual LPCTSTR GetErrorString(
                              HRESULT     dwError,
                              LCID        dwLocale);
    virtual LPCTSTR GetVendorString();

    // PLC5 Stuff
    //   void    InitAPLCTag(LPCTSTR Branch, 
    //                       LPCTSTR Tag); 
    //virtual void  StartTheCommand();
    //virtual void  FinishTheCommand(long Rsp);

  protected:
    CString error;
    Branch  m_root;
    CPLC5Array & m_Plc; 
  };

//*******************************************************************

class ShellBrowser : public COPCBrowser
{
public:
           ShellBrowser(CMyOPCCallBack* parent)
                  :m_parent(parent) {m_pBranch = &m_parent->m_root;
                                     Reset();}
//   virtual ~ShellBrowser();
   virtual OPCNAMESPACETYPE QueryOrganization();
   virtual BOOL MoveUp();
   virtual BOOL MoveDown(LPCTSTR branch);
   virtual HRESULT GetNames(  OPCBROWSETYPE type,
                              LPCTSTR stringFilter,
                              VARTYPE dataTypeFilter,
                              DWORD accessFilter );
   virtual LPCTSTR GetItemID( LPCTSTR name );
   virtual LPCTSTR Next();
   virtual void Reset();

private:
   CMyOPCCallBack* m_parent;
   POSITION       m_pos;
   Branch*        m_pBranch;

   // For browsing flat
   CStringList    m_paths;
   void           AddTags( Branch* pBranch );

   OPCBROWSETYPE  m_type;
   CString        m_stringFilter;
   VARTYPE        m_dataTypeFilter;
   DWORD          m_accessFilter;
   TCHAR          m_name[256];
};


#endif