//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"

#define  __DATAATTR_CPP

#include "datacnvs.h"
#include "dataattr.h"
//#include "optoff.h"

//===========================================================================

/*FmtAttribute DefinedFmts[]; = {{           "*.", 0, 0, 'f'},
                              {          "*.#", 0, 1, 'f'},
                              {         "*.##", 0, 2, 'f'},
                              {        "*.###", 0, 3, 'f'},
                              {       "*.####", 0, 4, 'f'},
                              {      "*.#####", 0, 5, 'f'},
                              {     "*.######", 0, 6, 'f'},
                              {       "#.##e#", 0, 2, 'e'},
                              {      "#.###e#", 0, 3, 'e'},
                              {     "#.####e#", 0, 4, 'e'},
                              {    "#.#####e#", 0, 5, 'e'},
                              {   "#.######e#", 0, 6, 'e'},
                              {         NULL, 0,0}}; */

//===========================================================================
//
//
//
//===========================================================================
#ifdef _DEBUG
#define DO_ATTRIB_MEM_TEST 0 //normally 0
#else
#define DO_ATTRIB_MEM_TEST 0
#endif

#if DO_ATTRIB_MEM_TEST
DbgMemHelper dv(false, 0);
#endif

UINT FmtAttribute::iCmdIndex=0;

void FmtAttribute::FormatFloat(double dVal, char * Str, short Len, Strng_List * NANStrLst)
  {
  if (Valid(dVal))
    {
    if (fabs(dVal)>=1.0e99)
      {
      if (dVal>0.0)
        strcpy(Str, "+Large");
      else
        strcpy(Str, "-Large");
      }
    else if (fabs(dVal)>=gs_DisplayZeroLimit)
      {
      double SclPrec[] = { 1.0e0, 1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5, 1.0e6, 1.0e7, 1.0e8 };
      char tType= cTyp;
      if ((tType=='f'||tType=='s') && (fabs(dVal) >= 1.0e12/SclPrec[Range(0, (int)iPrec, 8)]))
        tType='e';
      else if ((tType=='s') && (fabs(dVal) < 1.0e-9*SclPrec[Range(0, (int)iPrec, 8)]))
        tType='e';

      if (tType=='s')
        {
        iPrec=Min((byte)12,iPrec);
        char Buff[256];
        int Dec, Sign, i=0;

        strcpy(Buff, _ecvt(dVal, iPrec, &Dec, &Sign));
        if (Sign==0)
          strcpy(Str, "");
        else
          strcpy(Str, "-");

        i=strlen(Str);
        if (Dec<=0)
          {
          strcat(Str, "0.");
          i=strlen(Str);
          while (Dec++<0)
            Str[i++]='0';
          Str[i++]=0;
          strcat(Str, Buff);
          }
        else
          {
          if (Dec>=iPrec)
            strcpy(Buff, _ecvt(dVal, Dec+1, &Dec, &Sign));
          int j=0;
          while (Dec-->0 && Buff[j])
            Str[i++]=Buff[j++];
          while (Dec-->0)
            Str[i++]='0';
          Str[i++]='.';
          int iDec=i;
          while (Buff[j])
            Str[i++]=Buff[j++];
          if (iDec==i)
            Str[i++]='0';
          Str[i++]=0;
          }

        }
      if (tType!='s')
        {
        char FmtBuff[256];
        sprintf(FmtBuff, "%%.%i%c", iPrec, tType);
        sprintf(Str, FmtBuff, dVal);
        // reduce length of exponent
        while (1)
          {
          char *p=strstr(Str, "e+0");
          if (p && (strlen(p)>3))
            strcpy(p+2, p+3);
          else
            break;
          }
        while (1)
          {
          char *p=strstr(Str, "e-0");
          if (p && (strlen(p)>3))
            strcpy(p+2, p+3);
          else
            break;
          }
        }
      }
    else
      strcpy(Str, "0");
    }
  else
    {
    bool GotIt=false;
    if (NANStrLst)
      {
      int RqdIndex=-1;
      if (HasNANFlag(dVal, NF_No1))
        RqdIndex=1;
      else if (HasNANFlag(dVal, NF_No2))
        RqdIndex=2;
      else if (HasNANFlag(dVal, NF_No3))
        RqdIndex=3;
      else if (HasNANFlag(dVal, NF_No4))
        RqdIndex=4;
      else if (HasNANFlag(dVal, NF_No5))
        RqdIndex=5;
      else if (HasNANFlag(dVal, NF_No6))
        RqdIndex=6;
      else
        RqdIndex=0;
      if (RqdIndex>=0)
        {
        GotIt=false;
        for (Strng *p=NANStrLst->First(); p; p=NANStrLst->Next())
          {
          if (p->Index()==RqdIndex)
            {
            strcpy(Str, p->Str());
            GotIt=true;
            break;
            }
          }
        }
      }

    if (!GotIt)
      {
      if (HasNANFlag(dVal, NF_Free))
        strcpy(Str, "*Free");
      else if (HasNANFlag(dVal, NF_Hold))
        strcpy(Str, "*Hold");
      else if (HasNANFlag(dVal, NF_Pulse))
        strcpy(Str, "*Pulse");
      else if (HasNANFlag(dVal, NF_Ignore))
        strcpy(Str, "*Ignore");
      else if (HasNANFlag(dVal, NF_Block))
        strcpy(Str, "*Block");
      else if (HasNANFlag(dVal, NF_Plus ))
        strcpy(Str, "*+");
      else if (HasNANFlag(dVal, NF_Minus))
        strcpy(Str, "*-");
      else if (HasNANFlag(dVal, NF_Star ))
        strcpy(Str, "**");
      else if (HasNANFlag(dVal, NF_Pass ))
        strcpy(Str, "*Pass");
      else
        strcpy(Str, "*");
      }
    }
 }

//---------------------------------------------------------------------------

void FmtAttribute::FormatFloat(double dVal, Strng& sValue, Strng_List * NANStrLst)
  {
  char Buff[512];
  FormatFloat(dVal, Buff, sizeof(Buff), NANStrLst);
  sValue=Buff;
  }

//---------------------------------------------------------------------------

int FmtAttribute::FindFmt(char* pNm)
  {
  if (pNm)
    for (int i=0; i<DefinedFmts.GetSize(); i++)
      if (_stricmp(pNm, DefinedFmts[i].Name())==0)
        return i;
  return -1;
  }

//---------------------------------------------------------------------------

flag FmtAttribute::SetFmt(char* pNm)
  {
  int i = FindFmt(pNm);
  if (i>=0)
    {
    *this = DefinedFmts[i];
    return True;
    }
  return False;
  }

//---------------------------------------------------------------------------

flag FmtAttribute::ParseStr(char* pFmtStr)
  {
  if (strlen(pFmtStr)>1)
    switch (pFmtStr[0])
      {
      case 'e' : cTyp='e'; iPrec=(byte)(atol(pFmtStr+1)); BuildStr(sNm); return true;
      case 'f' : cTyp='f'; iPrec=(byte)(atol(pFmtStr+1)); BuildStr(sNm); return true;
      case 's' : cTyp='s'; iPrec=(byte)(atol(pFmtStr+1)); BuildStr(sNm); return true;
      default:;
      }

  char *p=pFmtStr;
  cTyp=0;
  iWide=0;
  iPrec=0;
  while (p)
    {
    switch (*p)
      {
      case '*' : cTyp='f'; break;
      case '~' : cTyp='s'; break;
      case '#' : iPrec++; break;
      case 'e' : iPrec--; cTyp='e'; goto Done;
      case 0: goto Done;
      }
    p++;
    }
Done:
  if (cTyp!=0)
    return true;
  Set("", 0,2,'f');
  return false;
  };

//---------------------------------------------------------------------------

char * FmtAttribute::BuildStr(Strng & S)
  {
  switch (cTyp)
    {
    case 'e': S.Set("e%i",iPrec); return S();
    case 'f': S.Set("f%i",iPrec); return S();
    case 's': S.Set("s%i",iPrec); return S();
    }
  //switch (cTyp)
  //  {
  //  case 'f':
  //    S.Set("*.%.*s",iPrec,"##########");
  //    return S();
  //  case 'e':
  //    S.Set("#.%.*se#",iPrec,"##########");
  //    return S();
  //  case 's':
  //    S.Set("%.*s~%.*s",Max(0,iPrec-1),"##########",1,"##########");
  //    return S();
  //  }
  return false;
  };

//---------------------------------------------------------------------------

void FmtAttribute::AddToMenu(CMenu& Menu, flag WithOverride)//, char* CheckItem)
  {
  /*
  Strng Nm;
  Nm = sNm;//CheckItem;
  Nm.Trim();
  const size_t lc = Nm.Length();
  for (int i=0; i<DefinedFmts.GetSize(); i++)
    {
    int Flag=MF_STRING;
    if (i>0 && (DefinedFmts[i].cTyp!=DefinedFmts[i-1].cTyp))
      Flag|=MF_MENUBARBREAK;
    if (strlen(DefinedFmts[i].Name())==lc && _stricmp(Nm(), DefinedFmts[i].Name())==0)
      Flag|=MF_CHECKED;
    Menu.AppendMenu(Flag, i+iCmdIndex, DefinedFmts[i].sNm());
    }
  */
//  Menu.AppendMenu(MF_STRING|MF_CHECKED, 1, "Override");
//  if (WithOverride)
//    {
//    Menu.AppendMenu(MF_STRING|MF_GRAYED, 1, "Override");
//    Menu.AppendMenu(MF_SEPARATOR, -1, "");
//    }

  Strng Nm;
  Nm = sNm;//CheckItem;
  Nm.Trim();

  CMenu AMenu;
  const size_t lc = Nm.Length();
  int iMenuItem=0;
  static const char* Heads[] = {
    "Fixed    (Dec)",
    "Fixed    (Sig)",
    "Floating (Dec)"};

  Strng Prec;
  for (int i=0; i<DefinedFmts.GetSize(); i++)
    {
    int Flag=MF_STRING;
    if (i==0 || (DefinedFmts[i].cTyp!=DefinedFmts[i-1].cTyp))
      {
      AMenu.Detach();
      AMenu.CreatePopupMenu();
      Menu.AppendMenu(MF_POPUP, (unsigned int)AMenu.m_hMenu, Heads[iMenuItem++]);
      }
    if (strlen(DefinedFmts[i].Name())==lc && _stricmp(Nm(), DefinedFmts[i].Name())==0)
      {
      Menu.CheckMenuItem(iMenuItem-1, MF_BYPOSITION |MF_CHECKED);
      Flag|=MF_CHECKED;
      }
    Prec.Set("%i", DefinedFmts[i].iPrec);
    AMenu.AppendMenu(Flag, i+iCmdIndex, Prec());

    }

  if (WithOverride)
    {
    // Temporary disabled
    Menu.InsertMenu(0, MF_SEPARATOR|MF_BYPOSITION);
    Menu.InsertMenu(0, MF_STRING|MF_BYPOSITION|MF_GRAYED, i,  "Override");
    //if (Cnvs[iIndex]->OverTxt())
    //  Menu.CheckMenuItem(i, MF_BYCOMMAND|MF_CHECKED);
    }
  AMenu.Detach();
  }

//===========================================================================
//
//
//
//===========================================================================

UINT CnvAttribute::iCmdIndex=0;

void CnvAttribute::AddToMenu(CMenu& Menu, flag WithOverride)
  {
  if ((iIndex)>0)
    {
    Strng Txt;
    Txt = sTxt;//CheckItem;
    Txt.Trim();
    const int lc = Txt.Length();
    pCDataCnv pC = Cnvs[iIndex];
    for (UINT i=iCmdIndex; pC; i++, pC=pC->Next())
      {
      Strng CnvTxt;
      CnvTxt= pC->Txt();
      CnvTxt.Trim();
      const int lt = CnvTxt.Length();
      if (lt==0)
        CnvTxt = ".";
      for (dword ii=0; ii<Menu.GetMenuItemCount(); ii++)
        {
        CString S;
        Menu.GetMenuString(ii, S, MF_BYPOSITION);
        if (S.CompareNoCase(CnvTxt())>=0)
          break;
        }
      if (ii<Menu.GetMenuItemCount())
        Menu.InsertMenu(ii, MF_STRING|MF_BYPOSITION, i, CnvTxt());
      else
        Menu.AppendMenu(MF_STRING, i, CnvTxt());
      if (((lt>0) && (lc>0) && (Txt.XStrCmp(CnvTxt())==0)) || ((lt==0) && (lc==0)))
        Menu.CheckMenuItem(i/*+iCmdIndex*/, MF_BYCOMMAND|MF_CHECKED);
      }
    if (WithOverride)
      {
      Menu.InsertMenu(0, MF_SEPARATOR|MF_BYPOSITION);
      Menu.InsertMenu(0, MF_STRING|MF_BYPOSITION, i,  "Override");
      if (Cnvs[iIndex]->OverTxt())
        Menu.CheckMenuItem(i, MF_BYCOMMAND|MF_CHECKED);
//      Menu.AppendMenu(MF_SEPARATOR);
//      Menu.AppendMenu(MF_STRING, i,  "Override");
//      if (Cnvs[iIndex]->OverTxt())
//        Menu.CheckMenuItem(i, MF_BYCOMMAND|MF_CHECKED);
      }
    }
  }

//---------------------------------------------------------------------------

double CnvAttribute::Human(double D)
  {
  if (Valid(D))
    return Cnvs[iIndex]->Human(D, sTxt());
  return dNAN;
  }

//---------------------------------------------------------------------------

double CnvAttribute::Normal(double D)
  {
  if (Valid(D))
    return Cnvs[iIndex]->Normal(D, sTxt());
  return dNAN;
  }

//---------------------------------------------------------------------------

double CnvAttribute::Cnv(double D, char* ReqdCnvTxt)
  {
  return Cnvs[iIndex]->Human(Cnvs[iIndex]->Normal(D, sTxt()), ReqdCnvTxt);
  }

//---------------------------------------------------------------------------

int CnvAttribute::SetIndex(char * FamName)
  {
  iIndex=gs_CnvsMngr.FindCnv(FamName);
  return iIndex;
  }
//===========================================================================
//
//
//
//===========================================================================

FmtAttrArray DefinedFmts;

class InitDefinedFmts
  {
  public:
  InitDefinedFmts()
    {
    //.int i=0;
    DefinedFmts.AddOne( "f0", 0, 0, 'f');
    DefinedFmts.AddOne( "f1", 0, 1, 'f');
    DefinedFmts.AddOne( "f2", 0, 2, 'f');
    DefinedFmts.AddOne( "f3", 0, 3, 'f');
    DefinedFmts.AddOne( "f4", 0, 4, 'f');
    DefinedFmts.AddOne( "f5", 0, 5, 'f');
    DefinedFmts.AddOne( "f6", 0, 6, 'f');
    DefinedFmts.AddOne( "f7", 0, 7, 'f');
    DefinedFmts.AddOne( "f8", 0, 8, 'f');
    DefinedFmts.AddOne( "f9", 0, 9, 'f');
    DefinedFmts.AddOne( "f10", 0, 10, 'f');

    DefinedFmts.AddOne( "s1", 0, 1, 's');
    DefinedFmts.AddOne( "s2", 0, 2, 's');
    DefinedFmts.AddOne( "s3", 0, 3, 's');
    DefinedFmts.AddOne( "s4", 0, 4, 's');
    DefinedFmts.AddOne( "s5", 0, 5, 's');
    DefinedFmts.AddOne( "s6", 0, 6, 's');
    DefinedFmts.AddOne( "s7", 0, 7, 's');
    DefinedFmts.AddOne( "s8", 0, 8, 's');
    DefinedFmts.AddOne( "s9", 0, 9, 's');
    DefinedFmts.AddOne( "s10", 0, 10, 's');

    DefinedFmts.AddOne( "e0", 0, 0, 'e');
    DefinedFmts.AddOne( "e1", 0, 1, 'e');
    DefinedFmts.AddOne( "e2", 0, 2, 'e');
    DefinedFmts.AddOne( "e3", 0, 3, 'e');
    DefinedFmts.AddOne( "e4", 0, 4, 'e');
    DefinedFmts.AddOne( "e5", 0, 5, 'e');
    DefinedFmts.AddOne( "e6", 0, 6, 'e');
    DefinedFmts.AddOne( "e7", 0, 7, 'e');
    DefinedFmts.AddOne( "e8", 0, 8, 'e');
    DefinedFmts.AddOne( "e9", 0, 9, 'e');
    DefinedFmts.AddOne( "e10", 0, 10, 'e');
/*
    DefinedFmts.AddOne(        "*.", 0, 0, 'f');
    DefinedFmts.AddOne(       "*.#", 0, 1, 'f');
    DefinedFmts.AddOne(      "*.##", 0, 2, 'f');
    DefinedFmts.AddOne(     "*.###", 0, 3, 'f');
    DefinedFmts.AddOne(    "*.####", 0, 4, 'f');
    DefinedFmts.AddOne(   "*.#####", 0, 5, 'f');
    DefinedFmts.AddOne(  "*.######", 0, 6, 'f');
    //DefinedFmts.AddOne(        "#:", 0, 1, 's');
    DefinedFmts.AddOne(       "#~#", 0, 2, 's');
    DefinedFmts.AddOne(      "##~#", 0, 3, 's');
    DefinedFmts.AddOne(     "###~#", 0, 4, 's');
    DefinedFmts.AddOne(    "####~#", 0, 5, 's');
    DefinedFmts.AddOne(   "#####~#", 0, 6, 's');
    DefinedFmts.AddOne(  "######~#", 0, 7, 's');
    DefinedFmts.AddOne( "#######~#", 0, 8, 's');
    DefinedFmts.AddOne(    "#.##e#", 0, 2, 'e');
    DefinedFmts.AddOne(   "#.###e#", 0, 3, 'e');
    DefinedFmts.AddOne(  "#.####e#", 0, 4, 'e');
    DefinedFmts.AddOne( "#.#####e#", 0, 5, 'e');
    DefinedFmts.AddOne("#.######e#", 0, 6, 'e');
*/
    }
  };

static InitDefinedFmts InitFmts;

//===========================================================================
//
//
//
//===========================================================================

void FixFldName(char* pFldName, char *TagBuff)
  {
  strcpy(TagBuff, pFldName);
  XStrTrim(TagBuff);

  // Excape char0 if '['
  if (TagBuff[0]=='[')
    {
    memmove(TagBuff+1, TagBuff, strlen(TagBuff)+1);
    TagBuff[0]='~';
    }

  char *p=TagBuff;
  while (p=strchr(p, ' '))
    *p='_';
  }

//---------------------------------------------------------------------------

DataAttribute::DataAttribute(char * pFldName)
  {
  #if DO_ATTRIB_MEM_TEST
  dv.AllocTest(sizeof(*this), (void*)this, "CreateDA");
  #endif
  char Tag[1024];
  FixFldName(pFldName, Tag);
  sName=Tag;
  fVis=1;
  Fmt.Set("", 0, 2, 'f');
  dMin=0.0;
  dMax=1.0;
  };

//===========================================================================
//
//
//
//===========================================================================

ObjectAttribute::~ObjectAttribute()
  {
  DataAttributeIter Data(DataAttributes);
  for (DataAttribute *p=Data.First(); p; p=Data.Next())
    delete p;
  };

//---------------------------------------------------------------------------

pDataAttribute ObjectAttribute::FieldInList(char * pFldName)
  {
  DataAttribute *p=NULL;
  if (pFldName)
    {
    char Tag[1024];
    FixFldName(pFldName, Tag);
    DataAttributeIter Data(DataAttributes);
    for (p=Data.First(); p; p=Data.Next())
      if (p->sName.XStrICmp(Tag)==0)
        break;
    }

  return p;
  };

//---------------------------------------------------------------------------

DataAttribute * ObjectAttribute::SetBase(char * pFldName)
  {
  ASSERT(pFldName);

  Changed=1;
  DataAttribute *p=FieldInList(pFldName);

  if (p==NULL)
    {
    p=new DataAttribute(pFldName);
    DataAttributes.AddTail(p);
    if (p->sName.Length()==0)
      DoBreak();
    }
  return p;
  };

//---------------------------------------------------------------------------

flag ObjectAttribute::ValidateCnv(char * pFldName, CCnvIndex Index)
  {
  ASSERT(pFldName);

  DataAttribute *p=FieldInList(pFldName);
  if (p && Index>0 && p->Cnv.TextLength()>0)
    {
    if (Cnvs[Index]->Find(p->Cnv.Text())==NULL)
      {
      p->Cnv.SetText(Cnvs[Index]->Txt()); // Set to Primary Value
      return false;
      }
    }
  return True;
  }

//---------------------------------------------------------------------------

flag ObjectAttribute::FieldVisible(char * pFldName, DDEF_Flags dwFlags, flag fDeflt)
  {
  ASSERT(pFldName);

  DataAttribute *p=FieldInList(pFldName);
  return (p==NULL) ? fDeflt : p->fVis;
  };

//---------------------------------------------------------------------------

void ObjectAttribute::SetFieldCnvs(char * pFldName, CnvAttribute &Cnv)
  {
  DataAttribute *p=SetBase(pFldName);
  p->Cnv=Cnv;
  };

//---------------------------------------------------------------------------

void ObjectAttribute::SetFieldFmt(char * pFldName, FmtAttribute &Fmt)
  {
  DataAttribute *p=SetBase(pFldName);
  p->Fmt=Fmt;
  };

//---------------------------------------------------------------------------

flag ObjectAttribute::FieldFmt(char * pFldName, FmtAttribute & Fmt)
  {
  ASSERT(pFldName);

  DataAttribute *p=FieldInList(pFldName);
  if (p)
    {
    if (p->Fmt.Type()!='-')
      Fmt=p->Fmt;
    return True;
    }
  return False;
  };

//---------------------------------------------------------------------------

flag ObjectAttribute::FieldCnvs(char * pFldName, CnvAttribute &Cnv)
  {
  ASSERT(pFldName);

  DataAttribute *p=FieldInList(pFldName);
  if (p)
    {
    if (p->Cnv.TextLength()>0)
      Cnv.SetText(p->Cnv.Text());
    return True;
    }
  return False;
  };

//---------------------------------------------------------------------------

flag ObjectAttribute::FieldFmtCnvs(char * pFldName, FmtAttribute & Fmt, CnvAttribute &Cnv)
  {
  ASSERT(pFldName);

  DataAttribute *p=FieldInList(pFldName);
  if (p)
    {
    if (p->Fmt.Type()!='-')
      Fmt=p->Fmt;
    if (p->Cnv.TextLength()>0)
      Cnv.SetText(p->Cnv.Text());
    return True;
    }
  return False;
  };

//---------------------------------------------------------------------------

void ObjectAttribute::SetFieldMin(char * pFldName, double Min)
  {
  DataAttribute *p=SetBase(pFldName);
  p->dMin=Min;
  };

//---------------------------------------------------------------------------

void ObjectAttribute::SetFieldMax(char * pFldName, double Max)
  {
  DataAttribute *p=SetBase(pFldName);
  p->dMax=Max;
  };

//---------------------------------------------------------------------------

flag ObjectAttribute::FieldRange(char * pFldName, double & Min, double & Max)
  {
  ASSERT(pFldName);

  DataAttribute *p=FieldInList(pFldName);
  if (p)
    {
    Min=p->dMin;
    Max=p->dMax;
    return True;
    }
  return False;
  };

//---------------------------------------------------------------------------

flag ObjectAttribute::SetFieldVisible(char * pFldName, DDEF_Flags dwFlags, flag fVisible)
  {
  DataAttribute *p=SetBase(pFldName);
  p->fVis=fVisible;

  return p->fVis;
  };

//---------------------------------------------------------------------------

flag ObjectAttribute::Load()
  {
  char Buffer[65536];
  char *pBuff=Buffer;
  if (GetPrivateProfileSection(Name(), Buffer, sizeof(Buffer), PrjAccFmtsIniFile())>0)
    {
    while (strlen(pBuff)>0)
      {
      char * p=pBuff;
      pBuff+=strlen(pBuff)+1;
      char * pFldName=p;
      char * pData=strchr(p, '=');
      if(pFldName && pData)
        {
        XStrTrim(pFldName);
        char* pFld[10];
        for (int nFlds=0; pData; nFlds++)
          {
          *pData=0; // Terminate Fld
          pData++; // Point @ Data
          pFld[nFlds]=pData;
          pData=strchr(pData, ',');
          }
        //nFlds++; // for Last Field
        for (int i=0; i<nFlds; i++)
          XStrTrim(pFld[i]);

        DataAttribute *p=new DataAttribute(pFldName);
        p->fVis=pFld[0] ? (atol(pFld[0])!=0) : True;
        p->Fmt.SetType('-');
        if (nFlds > 1)
          {
          p->Fmt.ParseStr(pFld[1]);
          if (nFlds > 2  && pFld[2])
            p->Cnv.SetText(pFld[2]);
          if (nFlds > 3  && pFld[3])
            p->dMin=SafeAtoF(pFld[3]);
          if (nFlds > 4  && pFld[4])
            p->dMax=SafeAtoF(pFld[4]);
          }

        DataAttributes.AddTail(p);
        if (p->sName.Length()==0)
          DoBreak();
        }
      }
    }

  return True;
  };

//---------------------------------------------------------------------------

flag ObjectAttribute::Save()
  {
  char Buffer[65536];
  char *pBuff=Buffer;
  Strng SFmt;

  // Visible , Format, Conversions, min, max
  DataAttributeIter Data(DataAttributes);
  for (DataAttribute *p=Data.First(); p && (pBuff-Buffer)<sizeof(Buffer)-256; p=Data.Next())
    {
    char Tag[1024];
    FixFldName(p->sName(), Tag);
    p->Fmt.BuildStr(SFmt);
    pBuff+=1+sprintf(pBuff, "%s=%i,%s,%s,%g,%g",
      Tag, p->fVis, SFmt() ? SFmt() : "",
      p->Cnv.Text() ? p->Cnv.Text() : "", p->dMin, p->dMax);
    }

  //blank line at end
  pBuff+=1+sprintf(pBuff, " "); 
  pBuff+=1+sprintf(pBuff, ""); //must be two NULL characters at end

  ASSERT((pBuff-Buffer)<sizeof(Buffer));

  flag Ok=WritePrivateProfileSection(Name(), Buffer, PrjAccFmtsIniFile());
  return Ok;
  };

//---------------------------------------------------------------------------
//===========================================================================

ObjectAttributeList::~ObjectAttributeList()
  {
  ObjectAttributeIter Obj(*this);
  for (pObjectAttribute p=Obj.First(); p; )
    {
    pObjectAttribute pDel = p;
    p=Obj.Next();
    #if DO_ATTRIB_MEM_TEST
    dv.DumpTest(sizeof(*pDel), (void*)pDel, "DeleteOA");
    #endif
    delete pDel;
    }
  }

//---------------------------------------------------------------------------

ObjectAttribute *ObjectAttributeList::FindObject(char * pObjName)
  {
  ObjectAttributeIter Obj(*this);

  Strng FullObjName(pObjName);
  FullObjName+=".Attributes";

  for (pObjectAttribute p=Obj.First(); p; p=Obj.Next())
    if (p->Name.XStrICmp(FullObjName())==0)
      break;

  if (p==NULL)
    {
    p = new ObjectAttribute(FullObjName());
    #if DO_ATTRIB_MEM_TEST
    dv.AllocTest(sizeof(*p), (void*)p, "CreateOA");
    #endif
    p->Load();
    AddTail(p);
    }
  else
    {
    }
  return p;
  };

//---------------------------------------------------------------------------

flag ObjectAttributeList::Load()
  {
  return True;
  };

//---------------------------------------------------------------------------

flag ObjectAttributeList::Save()
  {
  flag Ok=1;
  ObjectAttributeIter Obj(*this);
  for (pObjectAttribute p=Obj.First(); p; p=Obj.Next())
    if (p->Changed)
      {
      p->Changed=0;
      if (!p->Save())
        Ok=0;
      }

  return Ok;
  };

//---------------------------------------------------------------------------
//===========================================================================

ObjectAttributeList ObjAttributes;

//===========================================================================
//
//
//
//===========================================================================
