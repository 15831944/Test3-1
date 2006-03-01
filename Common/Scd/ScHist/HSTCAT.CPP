//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#if ForSysCAD
#include "sc_defs.h"
#include "errorlog.h"
#endif
#include "hstcat.h"
#include "hstmain.h"

//===========================================================================
template <> UINT AFXAPI HashKey<WORD> (WORD Key)
  { 
  return (UINT)Key; 
  };
//===========================================================================

IMPLEMENT_MEMSTATS(CHistCat)
IMPLEMENT_MEMSTATS(CCatItem)

//===========================================================================

CCatItem::CCatItem()
  {
  }

//---------------------------------------------------------------------------

bool CCatItem::m_bFixedDataExtn=true;
CString CCatItem::m_sFilenameExtn =".dat";

char* CCatItem::CatNoStr(WORD CatNo)
  {
  static char Buff[5];
  Buff[4] = 0;
  int End;
  if (CatNo<36)
    {
    End = 3;
    Buff[1] = '0';
    Buff[2] = '0';
    }
  else if (CatNo<1296)
    {
    End = 2;
    Buff[1] = '0';
    }
  else if (CatNo<46656)
    End = 1;
  else if (CatNo<1679616)
    End = 0;
  else
    {
    LogError("History", 0, "Historian catalog number exceeded maximum!");
    VERIFY(FALSE);
    return NULL;
    }

  for (int i=3; i>=End; i--)
    {
    ldiv_t n = ldiv(CatNo, 36);
    CatNo = (WORD)n.quot;
    if (n.rem<=9)
      Buff[i] = '0' + (char)n.rem;
    else
      Buff[i] = 'a' + (char)n.rem - 10;
    }
  if (CatNo<46656)
    return &Buff[1];
  return Buff;
  }

//---------------------------------------------------------------------------

WORD CCatItem::CatNoFromStr(char* Str)
  {
  if (Str==NULL)
    return 0;
  while (*Str!=0 && isspace(*Str))
    Str++;
  if (Str && (isalpha(*Str) || isdigit(*Str)))
    {
    const char* p = Str;
    WORD n = 0;
    while (isalpha(*Str) || isdigit(*Str))
      {
      n *= 36;
      if (isdigit(*Str))
        n += (char)toupper(*Str)-(char)'0';
      else
        n += (char)toupper(*Str)-(char)'A'+10;
      Str++;
      }
    return n;
    }
  return 0;
  }

//---------------------------------------------------------------------------

void CCatItem::MakeFilename(WORD CatNo, CString &s)
  {
  s = pHistory->sDataPath;
  s += pHistory->Data.sScenName;
  s += '.';
  s += CCatItem::CatNoStr(CatNo);
  if (CCatItem::FixedDataExtn())
    s += m_sFilenameExtn;
  }

//---------------------------------------------------------------------------

void CCatItem::MakeCatFilename(WORD CatNo, CString &s)
  {
  s = pHistory->sCatPath;
  s += pHistory->Data.sScenName;
  s += '.';
  s += CCatItem::CatNoStr(CatNo);
  if (CCatItem::FixedDataExtn())
    s += m_sFilenameExtn;
  }

//---------------------------------------------------------------------------

void CCatItem::MakeMapName(CString& MapName)
  {
  MakeFilename(Data.iCatNo, MapName);
  int i = MapName.ReverseFind('\\');
  if (i>=0)
    MapName = MapName.Mid(i+1);
  MapName += "_MAP";
  }

//---------------------------------------------------------------------------

void CCatItem::UpdateTime(pCEvBlk pEvBlk)
  {
  Data.dStartTime = pEvBlk->pEvHead->dStartTime;
  Data.dLastTime = pEvBlk->pEvHead->dLastTime;
  }

//---------------------------------------------------------------------------

void CCatItem::dbgDump()
  {
  dbgpln("%6d%6d%15.2f%15.2f%12.2f", lRecNo, Data.iCatNo, Data.dStartTime, Data.dLastTime, Data.dLastTime - Data.dStartTime);
  //  char      sScenName[9];  //scenario name
  //  long      lCreateTime;
  //pCEvBlk       pEvBlk;   //pointer to open EvBlk if available
  }

//===========================================================================

CHistCat::CHistCat() :
  ItemsMap(32)
  {
  CatItems = NULL;
  iCatItemCnt = 0;
  lAllocCnt = 0;
  Initialise();
  }

//---------------------------------------------------------------------------

void CHistCat::Initialise()
  {
  memset(Data.sSpare, 0, sizeof(Data.sSpare));
  ClearDataLists();
  }

//---------------------------------------------------------------------------

CHistCat::~CHistCat()
  {
  ClearDataLists();
  }

//---------------------------------------------------------------------------

void CHistCat::ClearDataLists()
  {
  if (CatItems)
    {
    for (long i=0; i<iCatItemCnt; i++)
      delete CatItems[i];
    delete []CatItems;
    CatItems = NULL;
    }
  iCatItemCnt = 0;
  lAllocCnt = 0;
  ItemsMap.RemoveAll();
  ItemsMap.InitHashTable(53);
  }

//---------------------------------------------------------------------------

void CHistCat::SetFilename(char* Filename)
  {
  sFilename = Filename;
  if (sFilename.Find('.')<0)
    sFilename += ".cat";
  }

//---------------------------------------------------------------------------

byte CHistCat::Load(flag HdOnly/*=false*/)
  {
  WIN32_FIND_DATA fd;
  HANDLE H;
  if ((H=FindFirstFile((const char*)sFilename, &fd))!=INVALID_HANDLE_VALUE)
    {
    FindClose(H);
    if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
      {
      LogError("History", 0/*LF_Exclamation*/, "Cannot open read-only historian catalog '%s'", (const char*)sFilename);
      return HST_ERR_OPENCAT;
      }
    DWORD dwFlags = CFile::shareCompat|CFile::modeReadWrite|CFile::typeBinary;
    CFile F;
    if (F.Open((const char*)sFilename, dwFlags))
      {
      //load all catalog items into memory...
      TRY
        {
        F.Read((void*)&Data, sizeof(HD_CatHead));
        if (Data.iVerNo != HstVerNo && Data.iVerNo==3 && HstVerNo==4)
          {
          Data.iVerNo = HstVerNo;
          LogNote("History", 0, "Historian catalog, auto upgrade version number");
          }
        if (Data.iVerNo != HstVerNo)
          {
          LogError("History", 0, "Historian version number mismatch in catalog");
          return HST_ERR_OPENCAT;
          }
        if (!HdOnly)
          {
          ClearDataLists();
          if (Data.iCatItemCnt>0)
            {
            HD_CatItem TempData;
            for (long i=0; i<Data.iCatItemCnt; i++)
              {
              F.Read((void*)&TempData, sizeof(HD_CatItem));
              if (stricmp(TempData.sScenName, pHistory->Data.sScenName)==0)
                {
                MakeCatItemSpace();
                CatItems[iCatItemCnt] = new CCatItem;
                CatItems[iCatItemCnt]->Data = TempData;
                CatItems[iCatItemCnt]->lRecNo = i;
                ItemsMap.SetAt(CatItems[iCatItemCnt]->Data.iCatNo, iCatItemCnt);
                iCatItemCnt++;
                }
              }
            }
          }
        F.Close();
        }
      CATCH (CException, e)
        {
        //LogError("History", 0, "File exception while loading catalog '%s'", (const char*)sFilename);
        char Buff[256];
        e->GetErrorMessage( Buff, sizeof(Buff));
        LogError("History", 0, "'%s':%s", (const char*)sFilename, Buff);
        return HST_ERR_OPENCAT;
        }
      END_CATCH
      return 0;
      }
    }
  else
    return HST_ERR_NOTEXISTS_CAT;
  return HST_ERR_OPENCAT;
  }

//---------------------------------------------------------------------------

flag CHistCat::Save(long CatNo/*=-1*/)
  {
  DWORD dwFlags = CFile::shareCompat|CFile::modeWrite|CFile::typeBinary;
  CFileStatus State;
  if (CFile::GetStatus((const char*)sFilename, State)==0)
    dwFlags |= CFile::modeCreate;
  CFile F;
  if (F.Open((const char*)sFilename, dwFlags))
    {
    TRY
      {
      F.Write((void*)&Data, sizeof(HD_CatHead));
      if (CatNo==-1)
        {//all data
        for (long i=0; i<iCatItemCnt; i++)
          {
          unsigned long Pos = sizeof(HD_CatHead) + (CatItems[i]->lRecNo * sizeof(HD_CatItem)); 
          if (F.GetLength() < Pos)
            F.SetLength(Pos);
          F.Seek(Pos, CFile::begin); 
          F.Write((void*)&(CatItems[i]->Data), sizeof(HD_CatItem));
          }
        }
      else if (CatNo>=0)
        {
        unsigned long Pos = sizeof(HD_CatHead) + (CatItems[CatNo]->lRecNo * sizeof(HD_CatItem)); 
        if (F.GetLength() < Pos)
          F.SetLength(Pos);
        F.Seek(Pos, CFile::begin); 
        F.Write((void*)&(CatItems[CatNo]->Data), sizeof(HD_CatItem));
        }
      F.Close();
      }
    CATCH (CException, e)
      {
      char Buff[256];
      e->GetErrorMessage( Buff, sizeof(Buff));
      LogError("History", 0, "'%s':%s", (const char*)sFilename, Buff);
      return false;
      }
    END_CATCH
    return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

void CHistCat::MakeCatItemSpace()
  {
  if (iCatItemCnt>=lAllocCnt)
    {
    lAllocCnt += 16;
    CCatItem ** pNewCatItems = new pCCatItem[lAllocCnt];
    if (CatItems)
      {
      for (long i=0; i<iCatItemCnt; i++)
        pNewCatItems[i] = CatItems[i];
      delete []CatItems;
      }
    CatItems = pNewCatItems;
    }
  }

//---------------------------------------------------------------------------

void CHistCat::AddCatItem(WORD CatNo)
  {
  MakeCatItemSpace();
  CatItems[iCatItemCnt] = new CCatItem();
  CatItems[iCatItemCnt]->Data.iCatNo = CatNo;
  strncpy(CatItems[iCatItemCnt]->Data.sScenName, pHistory->Data.sScenName, sizeof(pHistory->Data.sScenName));
  CatItems[iCatItemCnt]->Data.dStartTime = pHistory->dTime;
  CatItems[iCatItemCnt]->Data.dLastTime = pHistory->dTime;
  CatItems[iCatItemCnt]->lRecNo = Data.iCatItemCnt;
  time(&CatItems[iCatItemCnt]->Data.lCreateTime);
  ItemsMap.SetAt(CatNo, iCatItemCnt);
  iCatItemCnt++;
  Data.iCatItemCnt++;
  }

//---------------------------------------------------------------------------

void CHistCat::DeleteLastCatItem()
  {
  iCatItemCnt--;
  Data.iCatItemCnt--;
  delete CatItems[iCatItemCnt];
  CatItems[iCatItemCnt] = NULL;
  }

//---------------------------------------------------------------------------

pCCatItem CHistCat::Find(WORD CatNo)
  {
  long Index;
  if (ItemsMap.Lookup(CatNo, Index))
    return CatItems[Index];
  return NULL;
  /*for (long i=iCatItemCnt-1; i>=0; i--)
    {
    if (CatItems[i]->CatNo()==CatNo)
      return CatItems[i];
    }
  return NULL;*/
  }

//---------------------------------------------------------------------------

long CHistCat::GetCatItemByLastTime(double LastTime)
  {//returns index of catalog item that is closest to containing the given time
  for (long i=iCatItemCnt-1; i>=0; i--)
    {
    if (LastTime >= CatItems[i]->Data.dStartTime)
      return i;
    }
  return 0;
  }

//---------------------------------------------------------------------------

long CHistCat::GetCatItemByStartTime(double StartTime)
  {//returns index of catalog item that is closest to containing the given time
  for (long i=0; i<iCatItemCnt; i++)
    {
    if (StartTime <= CatItems[i]->Data.dLastTime)
      return i;
    }
  return iCatItemCnt - 1;
  }

//---------------------------------------------------------------------------
/*
flag CHistCat::CloseAllBlks(flag ForceAll, long CatNo, flag DoneLock)
  {
  if (!DoneLock)
    pHistory->Lock();
  flag AllClosed = True;
  __time64_t t;
  time(&t);
  for (long i=0; i<iCatItemCnt; i++)
    {
    if (ForceAll)
      {
      flag Closed = CatItems[i]->CloseBlk();
      while (!Closed)
        {
        LogError("History", 0, "Someone is preventing a Historian Block from closing!");
        //ASSERT(FALSE); //shouldn't get here, but you might when a client crashes while a query is open !!!
        CatItems[i]->pEvBlk->ReleaseBlk();
        Closed = CatItems[i]->CloseBlk();
        }
      }
    else
      {
      if (CatItems[i]->pEvBlk)
        {
        if (!CatItems[i]->pEvBlk->IsInUse())
          {
          if ( (CatItems[i]->pEvBlk->LastUseTime + pHistory->lBlkWaitTime < t) || 
               (!CatItems[i]->pEvBlk->pEvHead->bReducedSize) ||
               (CatNo == i))
            CatItems[i]->CloseBlk(); //block has been inactive for some time, close it
          }
        }
      }
    }
  if (!DoneLock)
    pHistory->Release();
  return AllClosed;
  }
*/

//---------------------------------------------------------------------------

void CHistCat::dbgDump(char* p)
  {
  dbgpln("---Historian catalog[%s] : %s", p, (const char*)sFilename);
  dbgpln("No of cat items  :%d", iCatItemCnt);
  Data.dbgDump();
  dbgpln("   Rec   Cat          start           last");   
  for (long i=0; i<iCatItemCnt; i++)
    CatItems[i]->dbgDump();
  }

//===========================================================================
