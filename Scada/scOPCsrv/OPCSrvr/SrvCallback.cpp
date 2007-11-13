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
#include "sc_defs.h"
//#include "xafxtempl.h"
#include "srvCallback.h"
#if WITHOPC
#include "OPCError.h"
#include "OPCProps.h"
#include "scd_wm.h"
#include "srvmngr.h"
#include "atlconv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define dbgOPCSrvr  0


long giBrowseTagCount;
long giBranchCount;


extern CTagTrees TagTrees;


/*class ValuesData
{
public:
char ValueName[256];
DWORD ValueNameLen;
BYTE Data[1024];
DWORD DataLen;
DWORD DataTyp;
};*/

//==========================================================================

CScdOPCTag::CScdOPCTag(CScdOPCCallBack * pCB, LPCTSTR name, Branch* branch) :
m_pCallBack(pCB)
  {
#if dbgOPCSrvr
  dbgpln("CScdOPCTag::CScdOPCTag(?,%s,?)", name);
#endif
  TaggedObject::SplitTagCnv(LPTSTR(name), m_sTagOnly, m_sCnvTxt);

#ifndef _RELEASE
  if (m_sTagOnly.XStrICmp("Train1.O:030/05")==0)
    {
    int b=0;
    }
#endif

  if (m_sCnvTxt())
    m_name.Format("%s (%s)", m_sTagOnly(), m_sCnvTxt());
  else
    m_name=m_sTagOnly();
  m_fullname = branch->GetPath() + m_name;

  m_bChecked=false;
  m_bIsValid=false;
  m_nativeType=VT_NULL;
  CheckTag();
  }

//--------------------------------------------------------------------------

CScdOPCTag::~CScdOPCTag()
  {
#if dbgOPCSrvr
  dbgpln("CScdOPCTag::~CScdOPCTag(%s)", m_sTagOnly());
#endif
  }

//--------------------------------------------------------------------------

bool CScdOPCTag::CheckTag()
  {
  if (m_sTagOnly.Len()>0)
    {
    char *s = m_fullname.GetBuffer( m_fullname.GetLength() );
    CXM_ObjectTag  ObjTag(s , TABOpt_AllInfoOnce);
    m_fullname.ReleaseBuffer();

    CXM_ObjectData ObjData;
    CXM_Route      Route;
    CPkDataItem  * pItem = NULL;

    m_cType = VT_NULL;
    m_CnvInx = 0;
    //m_sCnvTxt = "";
    m_nativeType = VT_NULL;

    if (CScdOPCManager::sm_pTheMngr->Enabled())
      {
      m_bChecked=true;

      if (m_pCallBack->XReadTaggedItem(ObjTag, ObjData, Route))
        {
        m_bIsValid=true;
        pItem = ObjData.FirstItem();
        m_cType = pItem->Type();
        m_CnvInx = pItem->CnvIndex();
        if (m_sCnvTxt())
          {
          // User has specified conversion text with tag check that it is valid
          if (!gs_CnvsMngr.FindSecCnv((m_CnvInx), m_sCnvTxt()))
            {
            // The provided conversion is not valid mark tag as invalid
            m_bIsValid=false;
            m_sCnvTxt="???";
            }
          }
        else
          {
          // Use the Items Default SI Conversion 
          m_sCnvTxt = pItem->CnvTxt(); 
          }

        m_nativeType = VT_NULL;
        switch (m_cType)
          {
          case tt_Char     : m_nativeType = VT_UI1; break;
          case tt_Bool     : m_nativeType = VT_UI1; break;
          case tt_Bit      : m_nativeType = VT_UI1; break;
          case tt_Byte     : m_nativeType = VT_UI1; break;
          case tt_Word     : m_nativeType = VT_UI2; break;
          case tt_DWord    : m_nativeType = VT_UI4; break;
          case tt_Int      : m_nativeType = VT_I4; break;
          case tt_Short    : m_nativeType = VT_I2; break;
          case tt_Long     : m_nativeType = VT_I4; break;
          case tt_Flt16    : m_nativeType = VT_R4; break;
          case tt_Float    : m_nativeType = VT_R4; break;
          case tt_Double   : m_nativeType = VT_R8; break;
          case tt_pChar    : m_nativeType = VT_BSTR; break;
          case tt_Strng    : m_nativeType = VT_BSTR; break;
          default          :
            m_nativeType = VT_NULL;
            m_bIsValid=false;
            break;
          }
        }
      }

    if (CScdOPCManager::sm_pTheMngr->AllowInvalidTags() || m_bIsValid)
      {
      SYSTEMTIME ST;
      FILETIME FT;
      //__time64_t t      = (__time64_t)gs_TimeCB.m_Time;
      __time64_t t      = (__time64_t)gs_TimeCB.m_TheTime.Seconds;
      struct tm *pT     = _localtime64(&t);
      ST.wSecond        = pT->tm_sec;
      ST.wMinute        = pT->tm_min;
      ST.wHour          = pT->tm_hour;
      ST.wDay           = pT->tm_mday;
      ST.wMonth         = pT->tm_mon;
      ST.wYear          = pT->tm_year+1900;
      ST.wMilliseconds  = 0;

      SystemTimeToFileTime(&ST, &FT);

      CSLock wait( &m_pCallBack->m_CS );    // protect tag access

      LoadLocal(pItem, FT);
      }
    }
  return m_bIsValid;
  }

//--------------------------------------------------------------------------

void CScdOPCTag::LoadLocal(CPkDataItem* pItem , FILETIME & FT)
  {
#if dbgOPCSrvr
  dbgpln("CScdOPCTag::LoadLocal(%s)", m_sTagOnly());
#endif

  if (!m_bChecked)//CScdOPCManager::sm_pTheMngr->Enabled())
    {
    m_timestamp = FT;
    m_quality = OPC_QUALITY_NOT_CONNECTED;
    m_nativeType = VT_NULL;// stops FS OPC Client from working
    //m_LclValue.Clear();
    //m_LclValue=CString("Bad Data Type");
    return;
    }

  PkDataUnion* pData = pItem->Value();
  if (pData==NULL)
    {//problem with tag (eg no longer exists?)
    m_timestamp = FT;
    m_quality = OPC_QUALITY_LAST_KNOWN;//OPC_QUALITY_BAD;
    return;
    }

  if ((IsIntData(m_cType) || IsUnSgnData(m_cType)) && pItem->Contains(PDI_StrList))
    {
    if (pItem->IndexedStrList())
      {
      m_nativeType = VT_BSTR;
      pItem->GetStrList(m_StrLst);
      }
    }
                
  if (IsData(m_cType))
    {
    if (m_nativeType == VT_BSTR)
      {
      CString temp;
      if (IsStrng(m_cType))
        {
        temp=pData->GetString();
        }
      else if (IsIntData(m_cType) || IsUnSgnData(m_cType))
        {
        temp = "?";
        int i=pData->GetLong();
        for (pStrng p= m_StrLst.First(); p; p=p->pNxt)
          if (p->Index()==i)
            {
            temp=p->Str();
            break;
            }
        }
      else
        {
        temp = "Bad Data Type";
        //DoBreak();
        }
      m_LclValue.Clear();
      //m_LclValue.vt = m_nativeType;  //if this line is included line below causes ""user breakpoint"" FIRST time LoadLocal is called !  trys to "free" old invalid string!
      m_LclValue=temp;
      //m_LclValue.SetString((const char*)temp, VT_BSTRT);
      //m_LclValue.SetString((const char*)temp, VT_BSTR);
      }
    else
      {
      m_LclValue.Clear();
      m_LclValue.vt = m_nativeType;
      switch (m_cType)
        {
        case tt_Char     :
        case tt_Bool     :
        case tt_Bit      :
        case tt_Byte     :
        case tt_Word     :
        case tt_DWord    :
        case tt_Int      :
        case tt_Short    :
        case tt_Long     :
          m_LclValue.lVal=pData->GetLong();
          break;
        case tt_Flt16    :
        case tt_Float    :
          m_LclValue.fltVal=(float)pData->GetDouble(m_CnvInx, m_sCnvTxt());
          break;
        case tt_Double   :
          m_LclValue.dblVal=pData->GetDouble(m_CnvInx, m_sCnvTxt());
          break;
        default          :
          //DoBreak();
          break;
        };
      }

    m_timestamp = FT;
    m_quality = OPC_QUALITY_GOOD;
    }
  }

//--------------------------------------------------------------------------
//==========================================================================
//*******************************************************************
Branch::Branch( LPCTSTR name ) : m_name(name),m_parent(NULL)
  {
  }

//*******************************************************************
Branch::~Branch()
  {
  while (!m_tags.IsEmpty())
    delete m_tags.RemoveTail();
  while (!m_branches.IsEmpty())
    delete m_branches.RemoveTail();
  }

//*******************************************************************
void Branch::AddTag(CScdOPCTag* pTag)
  {
#if FlatBranchStructure
  m_tags.AddTail(pTag);
#else
  m_tags.AddTail(pTag);
#endif
  }

//*******************************************************************
// Recursive function to search for a fully qualified tag name
CScdOPCTag* Branch::FindTag( const CString& target )
  {
#if FlatBranchStructure
  POSITION pos = m_tags.GetHeadPosition();
  while (pos)
    {
    CScdOPCTag* pTag = m_tags.GetNext(pos);
    if (pTag->m_name.CompareNoCase(target) == 0)
      return pTag;
    }
  return NULL;
#else
  //needs to be implemented properly...
  int delimiter = target.Find( _T('.') );


  //delimiter = -1; // Disable delimiters as they are a valid char in SysCAD Tags M.W. 24/6/3
  // Need to write code to handle this case... simple as assuming not fully qualified
  // and searching all tags. If one is not found then we traverse the branches searching???

  //if (delimiter == -1)   // tag name if no delimiter
  //  {


  // Check tag exists on this branch
  POSITION pos = m_tags.GetHeadPosition();
  while (pos)
    {
    CScdOPCTag* pTag = m_tags.GetNext(pos);
    if (pTag->m_name.CompareNoCase(target) == 0)
      return pTag;
    }
  //  }
  //else
  //  {


  // Did not exist on this branch so search branches
  CString branchName( target.Left( delimiter ) );
  /*POSITION*/ pos = m_branches.GetHeadPosition();
  while( pos )
    {
    Branch* pBranch = m_branches.GetNext( pos );
    if( pBranch->m_name.CompareNoCase( branchName ) == 0 )
      return pBranch->FindTag( target.Mid( delimiter+1 ) );
    }
  // }
  return NULL;
#endif
  }

//*******************************************************************
// Recursive function to search for a tag name or group name
CString Branch::FindTagName(const CString& target)
  {
#if FlatBranchStructure
  POSITION pos = m_tags.GetHeadPosition();
  while (pos)
    {
    CScdOPCTag* pTag = m_tags.GetNext(pos);
    if (pTag->m_name.CompareNoCase(target) == 0)
      return GetPath() + target;
    }
  return "";
#else
  //needs to be implemented properly...
  int delimiter = target.Find( /*_T('.')*/ "." ); // M.W. 23/06/03
  BOOL bFoundTag = TRUE;


  // Testing : Assume no delimiters at all times - SysCAD Tags can have . in them M.W. 24/6/03
  // Need to write code to handle this case

  //delimiter = -1;

  //  if (delimiter == -1)   // tag name if no delimiter
  //    {
  if (bFoundTag)
    {
    POSITION pos = m_tags.GetHeadPosition();
    while (pos)
      {
      CScdOPCTag* pTag = m_tags.GetNext(pos);
      if (pTag->m_name.CompareNoCase(target) == 0)
        {
        bFoundTag = FALSE;
        return GetPath() + target;
        }
      }
    }
  if(bFoundTag)
    {
    CString branchName = target;
    POSITION pos = m_branches.GetHeadPosition();
    while (pos)
      {
      Branch* pBranch = m_branches.GetNext(pos);
      if (pBranch->m_name.CompareNoCase( branchName ) == 0)
        return target;
      }
    }
  //    }
  //  else
  //    {
  CString branchName(target.Left(delimiter));
  POSITION pos = m_branches.GetHeadPosition();
  while (pos)
    {
    Branch* pBranch = m_branches.GetNext(pos);
    if (pBranch->m_name.CompareNoCase( branchName ) == 0)
      return pBranch->FindTagName(target.Mid(delimiter+1));
    }
  //    }
  return "";
#endif
  }

//*******************************************************************
CString Branch::GetPath()
  {
  if( m_parent )
    {
    return CString(m_parent->GetPath() + m_name + ".");
    }

  return "";
  }

//==========================================================================
//*******************************************************************
//*******************************************************************
// overrides for all COPCCallback virtual functions...

CScdOPCCallBack::CScdOPCCallBack(/*CScd * Plc*/)
: m_SubscribedTagsRoot( "scd" ), m_BrowseTagsRoot( "scd" )
//: m_root( "Scd" )
  {
#if dbgOPCSrvr
  dbgpln("CallBack::CallBack()");
#endif
  //CScdOPCTag* pTag = NULL;

  iTagCnt = 0;
  iExecuteCnt = 0;
  bForceReadRqd = false;
  dwMeasScanRate = 0;
  dwLastScan = GetTickCount();
  StatsCntReset();
  EO_Register("OPC", EOWrite_Msg|EORead_Msg|EOExec_Msg, /*Pri*/THREAD_PRIORITY_NORMAL, /*Stack*/10000);
  }

//*******************************************************************
CScdOPCCallBack::~CScdOPCCallBack()
  {
#if dbgOPCSrvr
  dbgpln("CallBack::~CallBack()");
#endif
  /*if (1)
  {
  CSLock wait( &m_CS );    // protect tag access
  while (!m_root.m_tags.IsEmpty())
  delete m_root.m_tags.RemoveTail();
  while (!m_root.m_branches.IsEmpty())
  delete m_root.m_branches.RemoveTail();
  }*/
  EO_DeRegister();
  }

//*******************************************************************
void CScdOPCCallBack::ConnectNotify(bool Connecting)
  {
  dwMeasScanRate = 0;
  dwLastScan = GetTickCount();
  StatsCntReset();
  if (CScdOPCManager::sm_pTheMngr->bShowStatsOnConnect)
    //    ScdMainWnd()->PostMessage(WMU_CMD, SUB_CMD_OPCSRVRSTATS, (LPARAM)0);
    ::PostMessage(CScdCOCmdBase::s_hWnd4Msgs, WMU_CMD, SUB_CMD_OPCSRVRSTATS, (LPARAM)0);
  else
    CScdOPCManager::sm_pTheMngr->UpdateStatusWnd((WPARAM)SUB_UPDATE_REDRAW);
  if (CScdOPCManager::sm_pTheMngr->LogNotes())
    LogNote("OPC", 0, "OPC Client %s", Connecting ? "Connect" : "Disconnect");
  }

//*******************************************************************

void CScdOPCCallBack::BuildBrowserBranchRecursive( char* pBranchName , Branch* pParentBranch , CTagTreeItem *pRoot )

//
// pBranchName - Name of new branch
// pParentBranch - Pointer to parent branch
// pRoot - Pointer to parent TagTreeItem
//

  {



  CTagTreeItem* pItem;
  Branch* pBranch;
  //Strng tempS;

  // Traverse the children of the Root Tag Tree Item
  pItem = pRoot->Child();

  if ( pItem == NULL )
    {
    // Leaf
    MakeTag( pRoot->Tag() , pParentBranch );
    }
  else
    {
    // New branch
    pBranch = new Branch(pRoot->Tag());
    pParentBranch->AddBranch(pBranch);
    while (pItem != NULL)
      {
      if (pItem->Child())
        {
        // Has children so create another branch
        BuildBrowserBranchRecursive( pItem->Tag() , pBranch ,  pItem );

        }
      else
        {
        // Must be a leaf
        //pItem->GetFullTag(tempS);
        MakeTag( pItem->Tag() , pBranch );
        }
      pItem = pItem->Next();
      }
    }

  }

//*******************************************************************

void CScdOPCCallBack::PopulateBrowserBranch( Branch* pBranch , CTagTreeItem *pRoot )

//
// pBranch - Branch to populate
// pRoot   - Pointer to corresponding TagTreeItem
//

  {

  CTagTreeItem* pItem;
  Branch* pChildBranch;


  // Traverse the children of the Root Tag Tree Item
  pItem = pRoot->Child();

  if ( pItem == NULL )
    {
    // Do nothing
    // This should not happen
    }
  else
    {
    while (pItem != NULL)
      {
      if (pItem->Child())
        {
        // Has children - Just create the unpopulated branch at this stage
        // Only populate if traversed
        pChildBranch = new Branch(pItem->Tag());
        giBranchCount++;
        pBranch->AddBranch(pChildBranch);
        }
      else
        {
        // Must be a leaf
        giBrowseTagCount++;
        MakeTag( pItem->Tag() , pBranch );
        }
      pItem = pItem->Next();
      }
    }

  }

//*******************************************************************
COPCBrowser* CScdOPCCallBack::CreateBrowser()
  {
#if dbgOPCSrvr
  dbgpln("CallBack::CreateBrowser(?)");
#endif

  giBrowseTagCount = 0;
  giBranchCount = 0;

  class ShellBrowser* sb = new ShellBrowser(this);

  //#ifdef NEVER

  // Moved this code to ShellBrowse::MoveUp method
  // Moved it back as it was going all the way back to the top
  // when browsing making it very slow.

  // Clean up existing BrowseTags branch lists to avoid duplication
  CSLock wait( &m_CS );    // protect tag access
  while (!m_BrowseTagsRoot.m_tags.IsEmpty())
    delete m_BrowseTagsRoot.m_tags.RemoveTail();
  while (!m_BrowseTagsRoot.m_branches.IsEmpty())
    delete m_BrowseTagsRoot.m_branches.RemoveTail();

  // Rebuild our top level tag/model list from SysCAD

  TagTrees.Rebuild(RQ_Tags);

  // Build the first level branches
  // Shell Browse->Move Down will create the rest dynamically as required
  //
  CModelTypeListArray& List = TagTrees.GetList();
  CModelTypeList* pTagList = List[0];
  for (int i=0; i< pTagList->GetSize(); i++)
    {

    Branch* pChildBranch = new Branch(pTagList->GetTagAt(i));
    sb->m_pBranch->AddBranch(pChildBranch);

    }
  //#endif

  // Testing
  /*
  CTagTree* T;
  CModelTypeListArray& List = TagTrees.GetList();
  CModelTypeList* pTagList = List[0];
  for (int i=0; i< pTagList->GetSize(); i++)
  {
  T = new CTagTree;
  T->Build( this , pTagList->GetTagAt(i) );
  delete T;
  }
  */

  return sb;
  }

//*******************************************************************
OPCSERVERSTATE CScdOPCCallBack::GetServerState()
  {
#if dbgOPCSrvr
  dbgpln("CallBack::GetServerState(?)");
#endif
  if (!OPCServerAllowed())
    return OPC_STATUS_FAILED; //return failed because not licensed, or valid project with OPC not loaded
  if (XRunning())
    return OPC_STATUS_RUNNING;
  return OPC_STATUS_RUNNING;
  //return OPC_STATUS_SUSPENDED; some OPC clients don't like this state flag!?!?!
  }

//*******************************************************************
// SetUpdateRate returns a modified update rate
DWORD CScdOPCCallBack::SetUpdateRate( DWORD newUpdateRate )
  {
#if dbgOPCSrvr
  dbgpln("CallBack::SetUpdateRate(?)");
#endif
  if( newUpdateRate==0 )
    return 100; //a default
  if( newUpdateRate > 10 )
    return newUpdateRate;
  return 10; //minimum
  }

//*******************************************************************
// AddTag creates a new tag or returns a pointer to an existing tag.
//
// Return NULL if the tag could not be created (bad name or accessPath)
// or the requested type is incompatible. The requested type is for this
// client, do not change the tag's type.
CTag* CScdOPCCallBack::AddTag(
                              LPCTSTR name,
                              LPCTSTR accessPath,
                              VARTYPE requestedType)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::AddTag(%s,%s)", name, accessPath);
#endif
  //DumpBranchs();
  CSLock wait( &m_CS );    // protect tag access


  CScdOPCTag * pTag = m_SubscribedTagsRoot.FindTag(name);
  if (pTag)
    {
    if (CScdOPCManager::sm_pTheMngr->LogNotes())
      LogNote("OPC", 0, "Add Tag: allready exists (%s)", name);

    return (CTag*)pTag;
    }


  // Initialize the tag's type, etc.
  // should store in a list
  if (pTag==NULL)
    {
    pTag = TryMakeTag(name, &m_SubscribedTagsRoot);
    if (pTag)
      {
      if (CScdOPCManager::sm_pTheMngr->LogNotes())
        LogNote("OPC", 0, "Add Tag (%s)", name);
      XBuildMyDataLists();
      CScdOPCManager::sm_pTheMngr->UpdateStatusWnd((WPARAM)SUB_UPDATE_REDRAW);
      }
    else
      {
      if (CScdOPCManager::sm_pTheMngr->LogNotes())
        LogNote("OPC", 0, "Add Tag: Failed to add tag (%s)", name);
      }
    }

  return (CTag*)pTag;
  }

//*******************************************************************
// ValidateTag modifies pTag to contain the nativeType and access rights
// if successful. If the name and access path do not specify a valid tag,
// then return OPC_E_UNKNOWNITEMID or OPC_E_UNKNOWNPATH.
// If the data type is incompatible, return OPC_E_BADTYPE.
HRESULT CScdOPCCallBack::ValidateTag(
                                     CTag* pTag,
                                     LPCTSTR name,
                                     LPCTSTR accessPath,
                                     VARTYPE requestedType)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::ValidateTag(?,%s,%s)", name, accessPath);
#endif
  CSLock wait( &m_CS );    // protect tag access
  CScdOPCTag* pFoundTag = m_BrowseTagsRoot.FindTag(name);
  if( pFoundTag )
    {
    pTag->m_nativeType = pFoundTag->m_nativeType;
    pTag->m_accessRights = pFoundTag->m_accessRights;
    // can use VariantChangeType to test data types:
    // get a current value for this tag and convert it to the requested type.
    // If VariantChangeType returns an error, return OPC_E_BADTYPE.
    return S_OK;
    }
  else
    {
    // This does not mean it does not exist in SysCAD
    // It just has not been added to the browse tree yet
    // For example the user may have specified units. i.e. Qm (t/h) instead of Qm

    // Check in subscribed tags
    pFoundTag = m_SubscribedTagsRoot.FindTag(name);
    if( pFoundTag )
      {
      pTag->m_nativeType = pFoundTag->m_nativeType;
      pTag->m_accessRights = pFoundTag->m_accessRights;
      // can use VariantChangeType to test data types:
      // get a current value for this tag and convert it to the requested type.
      // If VariantChangeType returns an error, return OPC_E_BADTYPE.
      return S_OK;
      }

    // Not subscribed already so may still be a valid tag.
    // Find out by adding the tag. AddTag will fail if it does not exist
    // NB: This is a cheat. We need a way of just testing the tag exists in
    // SysCAD....TO DO.

    CTag* pNewTag = AddTag(name,accessPath,requestedType);

    if( pNewTag )
      {
      pTag->m_nativeType = pNewTag->m_nativeType;
      pTag->m_accessRights = pNewTag->m_accessRights;
      // can use VariantChangeType to test data types:
      // get a current value for this tag and convert it to the requested type.
      // If VariantChangeType returns an error, return OPC_E_BADTYPE.
      return S_OK;
      }


    }

  return OPC_E_UNKNOWNITEMID;
  }

//*******************************************************************
// Remove is called when tags are released. This function must check
// if the tags are in use if AddTag may ever return a pointer to an
// existing tag. Additional clean up should occur here.
HRESULT CScdOPCCallBack::Remove(
                                DWORD           dwNumItems,
                                CTag         ** ppTags)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::Remove(?,)");
#endif
  CSLock wait( &m_CS );    // protect tag access
  for( DWORD index=0; index<dwNumItems; index++ )
    {
    // Not needed because CScdOPCTags are static
    CTag* pTag = ppTags[index];
    XBuildMyDataLists();
    //    if( !pTag->InUse() )
    //      delete pTag;
    }
  if (dwNumItems>0)
    XBuildMyDataLists();
  CScdOPCManager::sm_pTheMngr->UpdateStatusWnd((WPARAM)SUB_UPDATE_REDRAW);
  return S_OK;
  }

//*******************************************************************
LPCTSTR CScdOPCCallBack::GetTagName( CTag * pTag )
  {
#if dbgOPCSrvr
  dbgpln("CallBack::GetTagName(?,)");
#endif
  return ((CScdOPCTag*)pTag)->m_fullname;
  }

//*******************************************************************
// GetTagLimits should return TRUE if this tags has known limits.
BOOL CScdOPCCallBack::GetTagLimits(  CTag * pTag, double *pHigh, double *pLow )
  {
#if dbgOPCSrvr
  dbgpln("CallBack::GetTagLimits(?,)");
#endif
  *pHigh = 1.0;
  *pLow = -1.0;
  //return TRUE;
  return FALSE;
  }

//*******************************************************************
// Read each of these tags as a periodic scan
HRESULT CScdOPCCallBack::Scan(
                              DWORD           dwNumItems,
                              CTag         ** ppTags,
                              HRESULT       * pErrors)
  {
#if dbgOPCSrvr
  dbgp("CallBack::Scan(%d", dwNumItems);
#endif
  HRESULT hr = Read(dwNumItems, ppTags, pErrors);
  iScanCnt++;
  const DWORD Now = GetTickCount();
  dwMeasScanRate = Now - dwLastScan;
  dwLastScan = Now;
  bForceReadRqd = false;
  return hr;
  }

//*******************************************************************
HRESULT CScdOPCCallBack::ReadTag(CTag * pTag)
  {
#if dbgOPCSrvr
  dbgp("CallBack::ReadTag(");
#endif
  //#define OPC_E_INVALIDHANDLE              ((HRESULT)0xC0040001L)
  /*if (m_root.m_tags.IsEmpty())
  return S_OK;//OPC_E_???;*/

  // The work to set the data is done in EO_WriteSubsData
  // This does the actual data transfer to the OPC client

  CSLock wait( &m_CS );    // protect tag access

  CScdOPCTag* pScdTag = (CScdOPCTag*)(pTag);
#if dbgOPCSrvr
  dbgpln("%s)", pScdTag->m_name);
#endif

#ifndef _RELEASE
  if (pScdTag->m_sTagOnly.XStrICmp("Train1.O:030/05")==0)
    {
    int b=0;
    }
#endif

  if (bForceReadRqd)
    {
    //after a ReadTag, the ScdOpcSrvDLL only sends data to client if value (subject to bandwidth) or quality changed!
    //todo: extend functionality of CTag to have a m_ForceReadOnce flag.
    pTag->m_quality = OPC_QUALITY_LAST_KNOWN; //temporary
    }
  VariantCopy(&pTag->m_value, pScdTag->m_LclValue);
  iClientReadOKCnt++;

  return S_OK;
  }

//*******************************************************************
// The variant may contain data in any format the client sent.
// Use VariantChangeType to convert to a usable format.
HRESULT CScdOPCCallBack::WriteTag(
                                  CTag    * pTag,
                                  VARIANT & value)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::WriteTag(?,)");
#endif
  iClientWriteOKCnt++;

  CScdOPCTag &T=*((CScdOPCTag*)(pTag));

  HRESULT hr = S_OK;
  bool UseCnv=T.m_sCnvTxt.GetLength()>0;
  CXM_ObjectTag ObjTag(T.m_sTagOnly(), (UseCnv ? TABOpt_ValCnvsOnce : 0));
  CXM_ObjectData ObjData;
  CXM_Route Route;
  if (XReadTaggedItem(ObjTag, ObjData, Route))
    {
    PkDataUnion DU;
    switch (value.vt)
      {
      case VT_BOOL:
        DU.SetTypeLong(T.m_cType, value.boolVal);
        break;
      case VT_UI1:
        DU.SetTypeLong(T.m_cType, value.bVal);
        break;
      case VT_UI2:
        DU.SetTypeLong(T.m_cType, value.iVal);
        break;
      case VT_UI4:
        DU.SetTypeLong(T.m_cType, value.lVal);
        break;
      case VT_I2:
        DU.SetTypeLong(T.m_cType, value.iVal);
        break;
      case VT_I4:
        DU.SetTypeLong(T.m_cType, value.lVal);
        break;
      case VT_R4:
        if (UseCnv)
          DU.SetTypeDouble(T.m_cType, value.fltVal, (T.m_CnvInx), T.m_sCnvTxt());
        else
          DU.SetTypeDouble(T.m_cType, value.fltVal);
        break;
      case VT_R8:
        if (UseCnv)
          DU.SetTypeDouble(T.m_cType, value.dblVal, (T.m_CnvInx), T.m_sCnvTxt());
        else
          DU.SetTypeDouble(T.m_cType, value.dblVal);
        break;
      case VT_BSTR:
        {
        USES_CONVERSION;
        //COleVariant X(value.bstrVal);
        DU.SetTypeString(tt_Generic, OLE2T(value.bstrVal));
        break;
        }

      default          :
        break;

      }

    CXM_ObjectData ObjData(0, 0, T.m_sTagOnly(), 0, DU);
    if (XWriteTaggedItem(ObjData, Route)==TOData_OK)
      {
      iScdWriteOKCnt++;
      }
    else
      {
      hr = E_INVALIDARG;
      iWriteFailCnt++;
      }
    }
  else
    {
    iWriteFailCnt++;
    }

  if (SUCCEEDED(hr))
    {//update our copy of this tag...
    CSLock wait( &m_CS );    // protect tag access

    T.m_LclValue.Clear();
    VariantCopy(&T.m_LclValue, &value);
    }
  else
    {
    //warning message?
    }

  return hr;
  }

//*******************************************************************
// If the name is valid, return the number of item properties supported.
// If this returns successfully, QueryAvailableProperties will be called.
// To save lookups, put a hint into ppVoid.
HRESULT CScdOPCCallBack::QueryNumProperties(
  LPCTSTR     name,
  DWORD     * pdwNumItems,
  LPVOID    * ppVoid)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::QueryNumProperties(?,)");
#endif
  CScdOPCTag* pFoundTag = m_BrowseTagsRoot.FindTag(name);
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
HRESULT CScdOPCCallBack::QueryAvailableProperties(
  LPCTSTR     name,
  DWORD       dwNumItems,
  LPVOID      pVoid,
  DWORD     * pPropertyIDs,
  LPWSTR    * pDescriptions,
  VARTYPE   * pDataTypes)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::QueryAvailableProperties(?,)");
#endif
  if( pVoid == NULL )
    return OPC_E_UNKNOWNITEMID;

  CScdOPCTag* pTag = (CScdOPCTag*)pVoid;
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
HRESULT CScdOPCCallBack::GetItemProperties(
  LPCTSTR     name,
  DWORD       dwNumItems,
  DWORD     * pPropertyIDs,
  VARIANT   * pData,
  HRESULT   * pErrors)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::GetItemProperties(?,)");
#endif
  CScdOPCTag* pTag = m_BrowseTagsRoot.FindTag(name);
  if (!pTag)
    return OPC_E_UNKNOWNITEMID;

  DATE date;
  //WORD dosDate=0, dosTime=0;
  for (DWORD index=0; index<dwNumItems; index++)
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
        {
        FILETIME filetimeLocal;
        if (FileTimeToLocalFileTime(&pTag->m_timestamp, &filetimeLocal))
          {
          SYSTEMTIME systime;
          if (FileTimeToSystemTime(&filetimeLocal, &systime))
            {
            SystemTimeToVariantTime(&systime, &date);
            }
          }
        pData[index].vt = VT_DATE;
        pData[index].date = date;
        break;
        }
      /*case OPC_PROP_TIME:
      pData[index].vt = VT_DATE;
      FileTimeToDosDateTime( &pTag->m_timestamp, &dosDate, &dosTime);
      DosDateTimeToVariantTime( dosDate, dosTime, &date);
      pData[index].date = date;
      break;*/
      case OPC_PROP_RIGHTS:
        pData[index].vt = VT_I4;
        pData[index].lVal = OPC_READABLE | OPC_WRITEABLE;
        break;
      case OPC_PROP_DESC:
        {
        CString description(_T("Item Description goes here"));
        pData[index].vt = VT_BSTR;
        pData[index].bstrVal = description.AllocSysString();
        break;
        }
      }
    }
  return S_OK;
  }

//*******************************************************************
HRESULT CScdOPCCallBack::LookupItemIDs(
                                       LPCTSTR     name,
                                       DWORD       dwNumItems,
                                       DWORD     * pPropertyIDs,
                                       LPWSTR    * pszNewItemIDs,
                                       HRESULT   * pErrors)
  {
  //Chandra add functionality to get itemID
  DWORD index = 0;
  CString description;
  CScdOPCTag* pTag = m_BrowseTagsRoot.FindTag(name);
  if (!pTag)
    return OPC_E_UNKNOWNITEMID;

  // The name is valid
  _tcscpy( description.GetBuffer(_MAX_PATH),m_BrowseTagsRoot.GetPath() + name);
  pszNewItemIDs[index] = AllocString(description);
  return S_OK;
  }

//*******************************************************************
// return a string if the error code is recognized
LPCTSTR CScdOPCCallBack::GetErrorString(
                                        HRESULT     dwError,
                                        LCID        dwLocale)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::GetErrorString(?,)");
#endif
  error = _T("Unknown SysCAD OPC Server Error");
  return error;
  }

//*******************************************************************
// return your vendor string
LPCTSTR CScdOPCCallBack::GetVendorString()
  {
#if dbgOPCSrvr
  dbgpln("CallBack::GetVendorString(?,)");
#endif
  return _T("Kenwalt (Pty) Ltd.");
  }

//--------------------------------------------------------------------------

inline CScdOPCTag * CScdOPCCallBack::TryMakeTag(LPCTSTR Name, Branch * pBrnch/*, CScd * Plc*/)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::TryMakeTag(%s,)", Name);
#endif
  CScdOPCTag * pTag = new CScdOPCTag(this, Name, pBrnch);
  if (CScdOPCManager::sm_pTheMngr->AllowInvalidTags() || pTag->m_bIsValid)
    //if (1) // always add pTag->m_bIsValid)
    {
    pBrnch->AddTag(pTag);
    iTagCnt++;
    }
  else
    {
    LogWarning("OPC", 0, "Failed to add tag (%s)", Name);
    delete pTag;
    pTag = NULL;
    }
  return pTag;
  }

//--------------------------------------------------------------------------

inline CScdOPCTag * CScdOPCCallBack::MakeTag(LPCTSTR Name, Branch * pBrnch/*, CScd * Plc*/)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::MakeTag(%s,)", Name);
#endif
  CScdOPCTag * pTag = new CScdOPCTag(this, Name, pBrnch);
  pBrnch->AddTag(pTag);
  return pTag;
  }

//--------------------------------------------------------------------------

void CScdOPCCallBack::DumpBranchs()
  {
#ifndef _RELEASE
  CSLock wait( &m_CS );    // protect tag access

  const int Cnt = m_BrowseTagsRoot.m_tags.GetCount();
  POSITION Pos = m_BrowseTagsRoot.m_tags.GetHeadPosition();
  while (Pos!=NULL)
    {
    CScdOPCTag * pTag = m_BrowseTagsRoot.m_tags.GetNext(Pos);
    dbgpln("Tag:%s", pTag->m_name);
    }
#endif
  }

void CScdOPCCallBack::SetEnable(bool On)
  {
  CSLock wait( &m_CS );    // protect tag access

  const int Cnt = m_SubscribedTagsRoot.m_tags.GetCount();
  POSITION Pos = m_SubscribedTagsRoot.m_tags.GetHeadPosition();
  while (Pos!=NULL)
    {
    CScdOPCTag * pTag = m_SubscribedTagsRoot.m_tags.GetNext(Pos);
    if (On && !pTag->m_bChecked)
      {
      pTag->CheckTag();
      }
    else if (!On && pTag->m_bIsValid)
      {
      pTag->m_bIsValid=false;
      pTag->m_bChecked=false;
      pTag->m_quality=OPC_QUALITY_NOT_CONNECTED;
      }
    }
  }

//--------------------------------------------------------------------------
//*******************************************************************
// CExecObj Overrides
//*******************************************************************

flag CScdOPCCallBack::EO_QueryTime(CXM_TimeControl &CB, CTimeValue &TimeRqd, CTimeValue &dTimeRqd)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::EO_QueryTime(?,)");
#endif
  return true;
  };

flag CScdOPCCallBack::EO_Start(CXM_TimeControl &CB)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::EO_Start(?,)");
#endif
  if (CScdOPCManager::sm_pTheMngr->bForceOnStart)
    bForceReadRqd = true;
  CScdOPCManager::sm_pTheMngr->UpdateStatusWnd((WPARAM)SUB_UPDATE_EOSTART);
  return true;
  };

void CScdOPCCallBack::EO_QuerySubsReqd(CXMsgLst &XM)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::EO_QuerySubsReqd(?,)");
#endif
  CSLock wait( &m_CS );    // protect tag access

  CXM_ReadIndexedData* pMsg=(CXM_ReadIndexedData *)XM.MsgPtr(XM_ReadIndexedData);

  if (pMsg->Start)
    {
    long thisIndex=0;
    m_TagA.SetSize(m_SubscribedTagsRoot.m_tags.GetCount());
    POSITION Pos=m_SubscribedTagsRoot.m_tags.GetHeadPosition();
    while (Pos!=NULL)
      m_TagA[thisIndex++]=m_SubscribedTagsRoot.m_tags.GetNext(Pos);
    }

  flag ReadAll=pMsg->ReadAll;
  long DataIndex=pMsg->Start ? 0 : pMsg->LastIndex+1;

  Strng Tg;
  XM.Clear();
  for (  ; DataIndex<m_TagA.GetSize(); DataIndex++)
    {
    CXM_DataRequest *DRqst=new CXM_DataRequest (DataIndex, LPTSTR(LPCTSTR(m_TagA[DataIndex]->m_name)), TABOpt_AllInfoOnce, XIO_In);
    if (!XM.PackMsg(DRqst))
      {
      delete DRqst;
      break;
      }
    }
  };

void CScdOPCCallBack::EO_QuerySubsAvail(CXMsgLst &XM, CXMsgLst &XMRet)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::EO_QuerySubsAvail(?,)");
#endif
  //XM.Clear();
  };

flag CScdOPCCallBack::EO_ReadSubsData(CXMsgLst &XM)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::EO_ReadSubsData(?,)");
#endif
  flag DataRead=0;
  return DataRead;
  };

flag CScdOPCCallBack::EO_WriteSubsData(CXMsgLst &XM, flag FirstBlock, flag LastBlock)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::EO_WriteSubsData(%d,)", XM.NoMsgs());
#endif
  if (FirstBlock)
    {
    }

  static SYSTEMTIME STMem;// System Time converted from SysCAD Time
  SYSTEMTIME ST;// System Time converted from SysCAD Time
  FILETIME FT;  // Local File Time based on SysCAD time
  FILETIME UTCFT; // File time @ UTC

  //__time64_t t=(__time64_t)gs_TimeCB.m_Time;
  __time64_t t      = (__time64_t)gs_TimeCB.m_TheTime.Seconds;
  struct tm *pT=_localtime64(&t);


  // struct tm
  //int tm_sec;     /* seconds after the minute - [0,59] */
  //int tm_min;     /* minutes after the hour - [0,59] */
  //int tm_hour;    /* hours since midnight - [0,23] */
  //int tm_mday;    /* day of the month - [1,31] */
  //int tm_mon;     /* months since January - [0,11] */
  //int tm_year;    /* years since 1900 */
  //int tm_wday;    /* days since Sunday - [0,6] */
  //int tm_yday;    /* days since January 1 - [0,365] */
  //int tm_isdst;   /* daylight savings time flag */


  // SYSTEMTIME

  //wYear  Specifies the current year.
  //wMonth Specifies the current month; January = 1, February = 2, and so on.
  //wDayOfWeek Specifies the current day of the week; Sunday = 0, Monday = 1, and so on.
  //wDay Specifies the current day of the month.
  //wHour Specifies the current hour.
  //wMinute Specifies the current minute.
  //wSecond Specifies the current second.
  //wMilliseconds Specifies the current millisecond.

  if (pT)
    {
    ST.wSecond        = pT->tm_sec; /* seconds after the minute - [0,59] */
    ST.wMinute        = pT->tm_min; /* minutes after the hour - [0,59] */
    ST.wHour          = pT->tm_hour;/* hours since midnight - [0,23] */
    ST.wDay           = pT->tm_mday;/* day of the month - [1,31] */
    ST.wDayOfWeek     = pT->tm_wday; /* days since Sunday - [0,6] */
    ST.wMonth         = pT->tm_mon+1; /* months since January - [0,11] *//* Need to add 1 */
    ST.wYear          = pT->tm_year+1900;/* years since 1900 */
    ST.wMilliseconds  = 0;
    STMem=ST;
    }
  else
    {
    LogError("OPCCallBack", 0, "Time Wrap around");
    //STMem.wSecond++;
    ST=STMem;
    }


  DWORD lerr = SystemTimeToFileTime(&ST, &FT);

  lerr = LocalFileTimeToFileTime(&FT,&UTCFT);

  //todo: conversion between SysCAD time and local/GMT time not correct!!!

  for (long i=0; i<XM.NoMsgs(); i++)
    {
    CXM_ObjectData* pMsg=XM.ObjectData();
    long DataIndex=pMsg->Index;
    CPkDataItem* pItem=pMsg->FirstItem();
    PkDataUnion* pData=pMsg->FirstItem()->Value();

    CSLock wait( &m_CS );    // protect tag access

    CScdOPCTag & T=*m_TagA[DataIndex];
    T.LoadLocal(pItem, UTCFT);
    iScdReadOKCnt++;
    }

  //if (FirstBlock)
    {
    iExecuteCnt++;
    if (CScdOPCManager::sm_pTheMngr->iForceCnt>0 && iExecuteCnt>=CScdOPCManager::sm_pTheMngr->iForceCnt)
      {
      iExecuteCnt = 0;
      bForceReadRqd = true;
      }
    CScdOPCManager::sm_pTheMngr->UpdateStatusWnd((WPARAM)(SUB_UPDATE_SENDMSG|SUB_UPDATE_EOWRITE));
    }
#if dbgOPCSrvr
  dbgpln("CallBack::EO_WriteSubsData(?,)   ---Done---");
#endif
  return true;
  };

flag CScdOPCCallBack::EO_Execute(CXM_TimeControl &CB, CEOExecReturn &EORet)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::EO_Execute(?,)");
#endif
  //iExecuteCnt++;
  //CScdOPCManager::sm_pTheMngr->UpdateStatusWnd((WPARAM)(SUB_UPDATE_SENDMSG|SUB_UPDATE_EOEXEC));
  return false;
  };

flag CScdOPCCallBack::EO_Stop(CXM_TimeControl &CB)
  {
#if dbgOPCSrvr
  dbgpln("CallBack::EO_Stop(?,)");
#endif
  if (CScdOPCManager::sm_pTheMngr->bForceOnStop)
    bForceReadRqd = true;
  CScdOPCManager::sm_pTheMngr->UpdateStatusWnd((WPARAM)SUB_UPDATE_EOSTOP);
  return true;
  };

int CScdOPCCallBack::EO_CanClose(Strng_List & Problems)
  {
  /*if (1)
  {
  CSLock wait( &m_CS );    // protect tag access
  while (!m_root.m_tags.IsEmpty())
  delete m_root.m_tags.RemoveTail();
  while (!m_root.m_branches.IsEmpty())
  delete m_root.m_branches.RemoveTail();
  }*/
  return EO_CanClose_Yes;
  }


//--------------------------------------------------------------------------

flag CScdOPCCallBack::ForceWriteSubsDataAll()
  {
#if dbgOPCSrvr
  dbgpln("CallBack::ForceWriteSubsDataAll()");
#endif
  bForceReadRqd = true;
  return true;
  }

//---------------------------------------------------------------------------

char* GetVariantAsStr(COleVariant & Var, Strng &s)
  {
  switch (Var.vt)
    {
    case VT_EMPTY:
    case VT_NULL: s = ""; break;
    case VT_R8: s.Set("%g", Var.dblVal); break;
    case VT_I4:
    case VT_UI4: s.Set("%d", Var.lVal); break;
    case VT_I2:
    case VT_UI2: s.Set("%d", Var.iVal); break;
    case VT_BSTR:
      {
      USES_CONVERSION;
      LPCTSTR S=OLE2T(Var.bstrVal);
      s = S;//OLE2T(Var.bstrVal);
      break;
      }
    case VT_BOOL: s.Set("%d", Var.boolVal); break;
    case VT_R4: s.Set("%g", Var.fltVal); break;
    case VT_UI1: s.Set("%d", Var.bVal); break;
    default:
      ASSERT_ALWAYS(FALSE, "Variant Data Type?", __FILE__, __LINE__);
      s="?";
      break;
    }
  return s();
  }

void CScdOPCCallBack::BuildSubsList(CListBox* pList)
  {
  char Buff[1024];
  Strng s;
  CSLock wait( &m_CS );    // protect tag access
  for (long DataIndex=0; DataIndex<m_TagA.GetSize(); DataIndex++)
    {
    CScdOPCTag & T=*m_TagA[DataIndex];
    sprintf(Buff, "%s  (%s)", T.m_sTagOnly(), GetVariantAsStr(T.m_LclValue, s));
    pList->AddString(Buff);
    }
  //sprintf(Buff, "Subscription count : %d", m_TagA.GetSize());
  //pList->InsertString(0, Buff);
  }

//--------------------------------------------------------------------------
//==========================================================================
//For now, browser only provides list of tags already subscribed by OPC client,
//in future this should be extended to support ALL tags in SysCAD.......

//*******************************************************************
//  ShellBrowser class
//*******************************************************************
ShellBrowser::ShellBrowser(CScdOPCCallBack* parent) : m_parent(parent)
  {
  m_pBranch = &m_parent->m_BrowseTagsRoot;
  Reset();
  }

//*******************************************************************
OPCNAMESPACETYPE ShellBrowser::QueryOrganization()
  {
  //return OPC_NS_HIERARCHIAL;
  //return OPC_NS_FLAT;
  return OPC_NS_HIERARCHIAL;
  }

//*******************************************************************
HRESULT ShellBrowser::GetNames(  OPCBROWSETYPE type,
                               LPCTSTR stringFilter,
                               VARTYPE dataTypeFilter,
                               DWORD accessFilter )
  {
#if dbgOPCSrvr
  dbgpln("ShellBrowser::GetNames(?)");
#endif
  m_type = type;
  m_stringFilter = stringFilter;
  m_dataTypeFilter = dataTypeFilter;
  m_accessFilter = accessFilter;
  m_bDoStringFilter = (m_stringFilter.GetLength()>0 && m_stringFilter!="*");
  m_iFilterLen = m_stringFilter.GetLength();
  if (m_bDoStringFilter && m_stringFilter[m_iFilterLen-1]=='*')
    m_iFilterLen--;
  if( m_type == OPC_FLAT )
    {
    m_paths.RemoveAll();
    m_pBranch = &m_parent->m_BrowseTagsRoot;
    AddTags( m_pBranch );
    }
  Reset();
  return S_OK;
  }

//*******************************************************************
// Recursive function to add tag names from all groups to a list.
// This is only called when browsing OPC_FLAT
// Added 2.0
void ShellBrowser::AddTags( Branch* pBranch )
  {
#if dbgOPCSrvr
  dbgpln("ShellBrowser::AddTags(?)");
#endif
  // First add full path names for this group's tags
  CString path(pBranch->GetPath());
  POSITION pos = pBranch->m_tags.GetHeadPosition();
  while( pos )
    {
    CScdOPCTag* pTag = pBranch->m_tags.GetNext( pos );
    if (m_dataTypeFilter==VT_EMPTY || m_dataTypeFilter==pTag->m_nativeType)
      {
      CString name( path + pTag->m_name );
      if (m_bDoStringFilter)
        {
        if (name.GetLength()>=m_iFilterLen && _strnicmp((const char*)name, (const char*)m_stringFilter, m_iFilterLen)==0)
          m_paths.AddTail(name);
        }
      else
        m_paths.AddTail(name);
      }
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
#if dbgOPCSrvr
  dbgpln("ShellBrowser::MoveUp(?)");
#endif
  CSLock wait( &m_parent->m_CS );
  if( m_pBranch->m_parent )
    {
    m_pBranch = m_pBranch->m_parent;
    Reset();
    return TRUE;
    }
  // at the "root" level, can't go up


  // We create the top level every time we browse to the top
  // level - no it seems to start at the top every time you browse down a level?????

  // Clean up existing BrowseTags branch lists to avoid duplication
  /*
  while (!m_pBranch->m_tags.IsEmpty())
  delete m_pBranch->m_tags.RemoveTail();
  while (!m_pBranch->m_branches.IsEmpty())
  delete m_pBranch->m_branches.RemoveTail();


  giBrowseTagCount = 0;
  giBranchCount = 0;


  // Rebuild our top level tag/model list from SysCAD

  TagTrees.Rebuild(RQ_Tags);


  // Build the first level branches
  // Shell Browse->Move Down will create the rest dynamically as required
  //
  CModelTypeListArray& List = TagTrees.GetList();
  CModelTypeList* pTagList = List[0];
  for (int i=0; i< pTagList->GetSize(); i++)
  {

  Branch* pChildBranch = new Branch(pTagList->GetTagAt(i));
  m_pBranch->AddBranch(pChildBranch);

  }
  */
  return FALSE;
  }

//**************************************************************************
BOOL ShellBrowser::MoveDown(LPCTSTR branch)
  {
#if dbgOPCSrvr
  dbgpln("ShellBrowser::MoveDown(?)");
#endif
  CSLock wait( &m_parent->m_CS );
  POSITION pos = m_pBranch->m_branches.GetHeadPosition();
  while( pos )
    {
    Branch* pBranch = m_pBranch->m_branches.GetNext( pos );
    if( pBranch->m_name.CompareNoCase( branch ) == 0 )
      {
      m_pBranch = pBranch;

      if ( pBranch->m_branches.IsEmpty() && pBranch->m_tags.IsEmpty() )
        {
        // Dynamically Create Next Level
        CTagTree* T = new CTagTree;
        CString full_name;
        full_name = pBranch->GetPath();
        T->Build( m_parent , full_name.GetBuffer( full_name.GetLength()) );
        full_name.ReleaseBuffer();
        m_parent->PopulateBrowserBranch(pBranch, T->Root() );
        delete T;
        }

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
#if dbgOPCSrvr
  dbgpln("ShellBrowser::GetItemID(?)");
#endif
  CSLock wait( &m_parent->m_CS );
  // Search for the name
  CString tagName = m_pBranch->FindTagName(name);

  // The name is valid
  _tcscpy(m_name, tagName.GetBuffer(_MAX_PATH));
  return m_name;
  }

//*******************************************************************
LPCTSTR ShellBrowser::Next()
  {
#if dbgOPCSrvr
  dbgpln("ShellBrowser::Next(?)");
#endif
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
      CScdOPCTag* pTag = m_pBranch->m_tags.GetNext(m_pos);
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
#if dbgOPCSrvr
  dbgpln("ShellBrowser::Reset(?)");
#endif
  CSLock wait( &m_parent->m_CS );
  if( m_type==OPC_BRANCH )
    m_pos = m_pBranch->m_branches.GetHeadPosition();
  else if( m_type==OPC_LEAF )
    m_pos = m_pBranch->m_tags.GetHeadPosition();
  else if( m_type == OPC_FLAT )
    m_pos = m_paths.GetHeadPosition();


  }

//*******************************************************************
HRESULT ShellBrowser::GetAccessPaths( LPCTSTR name )
  {
  name = GetItemID(name);
  _tcscpy(m_name, name);
  if(_tcscmp(m_name, "") != 0)
    return S_OK;
  else
    return E_INVALIDARG;
  }

//*******************************************************************
LPCTSTR ShellBrowser::NextAccessPath()
  {
  return _T("");
  }

#endif
