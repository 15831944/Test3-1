//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __COPYBLK_CPP
#include "sc_defs.h"
#include "execlib.h"
#include "executiv.h"
#include "copyblk.h"
#include "sfe_base.h"

//===========================================================================

const char* CCopyBlock::CopyBlkFileName = "Scd_CBk.ini";
const char* CCopyBlock::TempBlockName = "TempTag_$$";

const int CopyBlockVerNo = 1; //KGA 2/12/97 major error in stored data, delete old copyblks.ini if ver < 1

//---------------------------------------------------------------------------

//FixFileName()
//  {
//  Strng OldCBKFile(PrjDatalibDirectory());
//  OldCBKFile+="copyblks.ini";
//
//  struct _finddata_t c_file;
//  long hFile;
//  if((hFile = _findfirst( OldCBKFile(), &c_file )) != -1L )
//    {
//    Strng NewCBKFile(PrjDatalibDirectory());
//    NewCBKFile+=CopyBlkFileName;
//    rename(OldCBKFile(), NewCBKFile());
//    }
//  };

//---------------------------------------------------------------------------

CCopyBlock::CCopyBlock(char* ModelClass, CExecObj* EO)
  {
  sModelClass = ModelClass;
  pEO = EO;
  eSrc = CB_SelectedTag;
  eDst = CB_TagList;
  bSrcDatalib = TRUE;
  bDstDatalib = TRUE;
  }

//---------------------------------------------------------------------------
//enum CopyBlkTypes { CB_Tag, CB_Block, CB_List };

void CCopyBlock::SetSrcTag(char* Src)
  {
  sSrc = Src;
  eSrc = CB_SelectedTag;
  bSrcDatalib = TRUE;
  }

//---------------------------------------------------------------------------

void CCopyBlock::SetSrcBlock(char* Src, BOOL FromDatalib/*=TRUE*/)
  {
  sSrc = Src;
  eSrc = CB_Block;
  bSrcDatalib = FromDatalib;
  }

//---------------------------------------------------------------------------

void CCopyBlock::SetDstList()
  {
  sDst = "";
  eDst = CB_TagList;
  TagList.SetSize(0);
  }

//---------------------------------------------------------------------------

void CCopyBlock::SetDstList(CSVector& List)
  {
  sDst = "";
  eDst = CB_TagList;
  TagList = List;
  }

//---------------------------------------------------------------------------

void CCopyBlock::SetDstBlock(char* Dst, BOOL FromDatalib/*=TRUE*/)
  {
  sDst = Dst;
  eDst = CB_Block;
  bDstDatalib = FromDatalib;
  }

//---------------------------------------------------------------------------

int CCopyBlock::WriteTagValues(CSVector& Tags)
  {
  ASSERT(pEO && bSrcDatalib && eSrc==CB_SelectedTag);
  CProfINIFile PF(CfgFiles(), (char*)CopyBlkFileName);
  if (!CheckCopyBlockVer(PF, true))
    return 0;
  Strng_List sStrList;
  int WriteCnt = 0;
  char Buff[2];
  Buff[0] = 0;
  Buff[1] = 0;
  PF.WrSection(TempBlockName, Buff);
  char* p = Buff;
  for (int i=0; i<Tags.GetSize(); i++)
    {
    CXM_ObjectData ObjData;
    CXM_Route Route;
    CXM_ObjectTag ObjTag(Tags[i](), TABOpt_AllInfoOnce);//TABOpt_Parms);
    if (pEO->XReadTaggedItem(ObjTag, ObjData, Route))
      {
      CPkDataItem * pItem = ObjData.FirstItem();
      byte cType = pItem->Type();
      if (IsData(cType))
        {
        WriteCnt++;
        if (IsIntData(cType) && pItem->Contains(PDI_StrList))
          {
          const long strIndex = pItem->Value()->GetLong();
          pItem->GetStrList(sStrList);
          pStrng pS = (sStrList.Length()>0) ? sStrList.AtIndexVal(strIndex) : NULL;
          if (pS)
            PF.WrStr(TempBlockName, Tags[i](), pS->Str());
          else
            PF.WrLong(TempBlockName, Tags[i](), strIndex);
          }
        else if (IsStrng(cType))
          PF.WrStr(TempBlockName, Tags[i](), pItem->Value()->GetString());
        else if (IsFloatData(cType))
          PF.WrDouble(TempBlockName, Tags[i](), pItem->Value()->GetDouble());
        else
          PF.WrLong(TempBlockName, Tags[i](), pItem->Value()->GetLong());
        }
      }
    }
  return WriteCnt;
  }

//---------------------------------------------------------------------------

BOOL CCopyBlock::CheckCopyBlockVer(CProfINIFile& PF, BOOL TestReadOnly/*=FALSE*/)
  {
  if (TestReadOnly)
    {
    WIN32_FIND_DATA fd;
    if (FileExists(PF.Filename(), fd))
      {
      if (fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        {
        LogError("SysCAD", LF_Exclamation, "Cannot write to read-only file '%s'", PF.Filename());
        return false;
        }
      }
    }

  if (PF.RdInt("General", "CopyBlockVerNo", 0)<1)
    {
    //old data is likely to be wrong DELETE old file!
    DeleteFile(PF.Filename());
    LogNote("SysCAD", 0, "Old style 'copy data block' file deleted (%s)", PF.Filename());
    }
  PF.WrInt("General", "CopyBlockVerNo", CopyBlockVerNo);
  return true;
  }

//---------------------------------------------------------------------------

void CCopyBlock::BuildBlockList(char* ModelClass, CSVector& List, CWordArray& FlagList, int& DefaultIndex)
  {
  DefaultIndex = -1;
  Strng s,Block;
  s.Set("(%s)", ModelClass);
  List.SetSize(16);
  FlagList.SetSize(16);
  int Cnt = 0;
  char Buff[16384];
  char Buff1[16];
  for (int i=0; i<2; i++)
    {
    CProfINIFile PF(i==0 ? CfgFiles() : PrjFiles(), (char*)CopyBlkFileName);
    CheckCopyBlockVer(PF);
    DWORD dw = PF.RdSectionNames(Buff, sizeof(Buff));
    ASSERT(dw<sizeof(Buff)-2); //section too large!!!
    char* p = Buff;
    while (p[0])
      {
      int len = strlen(p);
      char* Nextp = p;
      Nextp += (len + 1);
      char* pp = strstr(p, s());
      if (pp)
        {
        if (PF.RdSection(p, Buff1, sizeof(Buff1))>0)
          {
          Block = p;
          Block.SetLength((int)(pp-p));
          if (i==0 || List.Find(Block())<0)
            {
            if (Cnt>=List.GetSize())
              {
              List.SetSize(List.GetSize()+8);
              FlagList.SetSize(FlagList.GetSize()+8);
              }
            List[Cnt] = Block;
            FlagList[Cnt] = (i==0 ? 1 : 0);
            Cnt++;
            }
          }
        }
      p = Nextp;
      }
    }
  List.SetSize(Cnt);
  FlagList.SetSize(Cnt);
  }

//---------------------------------------------------------------------------

void CCopyBlock::BuildBlockTagsList(char* ModelClass, char* BlockName, BOOL FromDatalib, CSVector& List)
  {
  int Cnt = 0;
  List.SetSize(16);
  Strng Section;
  Section.Set("%s(%s)", BlockName, ModelClass);
  char Buff[16384];
  CProfINIFile PF(FromDatalib ? CfgFiles() : PrjFiles(), (char*)CopyBlkFileName);
  CheckCopyBlockVer(PF);
  DWORD dw = PF.RdSection(Section(), Buff, sizeof(Buff));
  ASSERT(dw<sizeof(Buff)-2); //section too large!!!
  char* p = Buff;
  while (p[0])
    {
    int len = strlen(p);
    char* Nextp = p;
    Nextp += (len + 1);
    char* pp = strchr(p, '=');
    if (pp)
      {
      if (Cnt>=List.GetSize())
        List.SetSize(List.GetSize()+8);
      pp[0] = 0;
      char* pValue = &pp[1];
      char* ppp = strchr(p, '.');
      if (ppp)
        List[Cnt].Set("x%s (%s)", ppp, pValue);
      else
        List[Cnt].Set("%s (%s)", p, pValue);
      Cnt++;
      }
    p = Nextp;
    }
  List.SetSize(Cnt);
  }

//---------------------------------------------------------------------------

void CCopyBlock::RemoveBlock(char* ModelClass, char* BlockName)
  {
  Strng s;
  s.Set("%s(%s)", BlockName, ModelClass);
  char Buff[16];
  for (int i=0; i<2; i++)
    {
    CProfINIFile PF(i==0 ? CfgFiles() : PrjFiles(), (char*)CopyBlkFileName);
    if (CheckCopyBlockVer(PF, true) && PF.RdSection(s(), Buff, sizeof(Buff))>0)
      {
      Buff[0] = 0;
      Buff[1] = 0;
      PF.WrSection(s(), Buff);
      }
    }
  }

//---------------------------------------------------------------------------

void CCopyBlock::MoveBlockLocation(char* ModelClass, char* BlockName, BOOL ToDatalib)
  {
  Strng s;
  s.Set("%s(%s)", BlockName, ModelClass);
  char Buff1[16];
  CProfINIFile SrcPF(ToDatalib ? CfgFiles() : PrjFiles(), (char*)CopyBlkFileName);
  if (!CheckCopyBlockVer(SrcPF, true))
    return;
  if (SrcPF.RdSection(s(), Buff1, sizeof(Buff1))>0)
    {
    char Buff[16384];
    DWORD dw = SrcPF.RdSection(s(), Buff, sizeof(Buff));
    ASSERT(dw<sizeof(Buff)-2); //section too large!!!
    CProfINIFile DstPF(ToDatalib ? PrjFiles() : CfgFiles(), (char*)CopyBlkFileName);
    if (!CheckCopyBlockVer(DstPF, true))
      return;
    DstPF.WrSection(s(), Buff);
    Buff1[0] = 0;
    Buff1[1] = 0;
    SrcPF.WrSection(s(), Buff1);
    }
  }

//---------------------------------------------------------------------------

int CCopyBlock::CopyTagBlocks()
  {
  CWaitCursor Wait;
//  ASSERT(eSrc!=CB_List); //this src type not allowed
//  ASSERT(eDst!=CB_Tag); //this dst type not allowed
  ASSERT(pEO);
  CProfINIFile SrcPF(bSrcDatalib ? CfgFiles() : PrjFiles(), (char*)CopyBlkFileName);
  CheckCopyBlockVer(SrcPF);
  CProfINIFile DstPF(bDstDatalib ? CfgFiles() : PrjFiles(), (char*)CopyBlkFileName);
  if (!CheckCopyBlockVer(DstPF, true))
    return 0;
  int Cnt = 0;
  //gs_pTheSFELib->FE_SetHoldGlobalLinks(true);
  //gs_Exec.SetHoldValidateData(true, true, true);
  gs_Exec.BeginBulkChange();

  Strng Section;
  if (eSrc==CB_SelectedTag)
    Section = TempBlockName;
  else
    Section.Set("%s(%s)", sSrc(), sModelClass());
  char Buff[16384];
  DWORD dw = SrcPF.RdSection(Section(), Buff, sizeof(Buff));
  ASSERT(dw<sizeof(Buff)-2); //section too large!!!
  if (eDst==CB_Block)
    {
    Strng DstSection;
    DstSection.Set("%s(%s)", sDst(), sModelClass());
    DstPF.WrSection(DstSection(), Buff);
    }
  else
    {
    Strng_List sStrList;
    CXM_Route Route;
    CXM_ObjectData ObjData;
    char* p = Buff;
    while (p[0])
      {
      int len = strlen(p);
      char* Nextp = p;
      Nextp += (len + 1);
      char* pp = strchr(p, '=');
      if (pp)
        {
        pp[0] = 0;
        char* pValue = &pp[1];
        char* ppp = strchr(p, '.');
        Strng PartTag;
        PartTag = (ppp==NULL ? p : ppp);
        for (int i=0; i<TagList.GetSize(); i++)
          {
          Strng WrkTag(TagList[i]());
          WrkTag += PartTag;
          //CXM_ObjectTag ObjTag(WrkTag(), 0);
          //need to use TABOpt_AllInfoOnce because of tags that contain a strList!
          CXM_ObjectTag ObjTag(WrkTag(), TABOpt_AllInfoOnce);//0);//TABOpt_Exists);//TABOpt_Parms);//TABOpt_ValCnvsOnce);
          Route.Clear();
          if (pEO->XReadTaggedItem(ObjTag, ObjData, Route))
            {
            CPkDataItem * pItem = ObjData.FirstItem();
            byte cType = pItem->Type();
            PkDataUnion DU;
            if (IsStrng(cType))
              DU.SetTypeString(cType, pValue);
            else if (IsFloatData(cType))
              DU.SetTypeDouble(cType, SafeAtoF(pValue));
            else if (IsIntData(cType) && pItem->Contains(PDI_StrList))
              {
              pItem->GetStrList(sStrList);
              pStrng pS = sStrList.Find(pValue);
              //const int Indx = (pS==NULL ? 0 : sStrList.Index(pS));
              const int Indx = (pS==NULL ? 0 : pS->Index());
              DU.SetTypeLong(cType, Indx);
              }
            else
              DU.SetTypeLong(cType, SafeAtoL(pValue));
            CXM_ObjectData OD(0, 0, WrkTag(), 0, DU);
            if (pEO->XWriteTaggedItem(OD, Route)==TOData_NotFound)
              LogWarning(WrkTag(), 0, "Write tag failed");
            else
              Cnt++;
            //if (pEO->XWriteTaggedItem(OD, Route)!=TOData_OK)
            //  LogWarning(WrkTag(), 0, "Write tag failed (possibly invalid data)");
            }
          else
            LogWarning(WrkTag(), 0, "Unable to read tag");
          }
        }
      p = Nextp;
      }
    }
  
  //TaggedObject::SetHoldValidateData(false);
  //gs_Exec.SetHoldValidateData(false, true, true);
  //gs_pTheSFELib->FE_SetHoldGlobalLinks(false);
  gs_Exec.EndBulkChange();


  //TaggedObject::SetXWritesBusy(false);
  return Cnt;
  }

//===========================================================================
