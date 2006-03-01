//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __SP_DBEXTRA_CPP
#include "sp_db.h"
#include "dbhelper.h"
#include "sp_dbextra.h"

//#include "optoff.h"

//===========================================================================

SpeciePropCfgItem::SpeciePropCfgItem()
  {
  iWidth=8;
  bOptional=0;
  bFound=0;
  bString=0;
  bDisplay=1;
  }

//---------------------------------------------------------------------------

SpeciePropCfgItem::SpeciePropCfgItem(char* pName, short Width, byte String, byte Display/*=1*/, byte Optional/*=0*/)
  {
  Name=pName;
  iWidth=Width;
  bOptional=Optional;
  bString=String;
  bDisplay=Display;
  bFound=0;
  }

//---------------------------------------------------------------------------

void SpeciePropCfgItem::Set(char* pName, short Width, byte String, byte Display/*=1*/, byte Optional/*=0*/)
  {
  Name=pName;
  iWidth=Width;
  bOptional=Optional;
  bString=String;
  bDisplay=Display;
  bFound=0;
  }

//===========================================================================

SpeciePropCfgHelper::SpeciePropCfgHelper()
  {
  iPropCnt = 0;
  pCfgItems = NULL;
  pPropNames = NULL;
  pOptPropNames = NULL;
  }

//---------------------------------------------------------------------------

SpeciePropCfgHelper::~SpeciePropCfgHelper()
  {
  delete []pPropNames;
  delete []pOptPropNames;
  }

//---------------------------------------------------------------------------

void SpeciePropCfgHelper::Init(SpeciePropCfgItem * CfgItems, int Count)
  {
  pCfgItems = CfgItems;
  iPropCnt = Count;
  int RqdCnt = 0;
  int OptCnt = 0;
  for (int i=0; i<iPropCnt; i++)
    {
    if (pCfgItems[i].bOptional)
      OptCnt++;
    else
      RqdCnt++;
    }
  pPropNames = new pchar[RqdCnt+1];
  pOptPropNames = new pchar[OptCnt+1];
  RqdCnt = 0;
  OptCnt = 0;
  for (i=0; i<iPropCnt; i++)
    {
    if (pCfgItems[i].bOptional)
      pOptPropNames[OptCnt++] = pCfgItems[i].Name();
    else
      pPropNames[RqdCnt++] = pCfgItems[i].Name();
    }
  pPropNames[RqdCnt] = NULL;
  pOptPropNames[OptCnt] = NULL;
  }

//---------------------------------------------------------------------------
//===========================================================================

SpecieProp::SpecieProp(SpeciePropCfgHelper* PropCfg, int SId)
  {
  pPropCfg = PropCfg;
  iSId = SId;
  bOK = 0;
  dProp = NULL;
  sProp = NULL;
  }

//---------------------------------------------------------------------------

SpecieProp::~SpecieProp()
  {
  delete []dProp;
  delete []sProp;
  }

//---------------------------------------------------------------------------
/*char* OreProp::Name()
  {
  return SDB[iSId].Tag();
  }*/
//---------------------------------------------------------------------------

int SpecieProp::Load(CStringArray & Values)
  {
  if (!dProp)
    {
    dProp = new double[pPropCfg->iPropCnt];
    sProp = new Strng[pPropCfg->iPropCnt];
    }
  ASSERT(Values.GetSize()>=pPropCfg->iPropCnt);
  int ColNo=0;
  for (int i=0; i<pPropCfg->iPropCnt; i++)
    {
    sProp[i] = (const char*)Values[i];
    dProp[i] = SafeAtoF((char*)(const char*)Values[i]);
    }

  bOK=1;
  return 0;
  }

//---------------------------------------------------------------------------
//===========================================================================

CSpeciePropDataBase::CSpeciePropDataBase()
  {
  iSpCnt = 0;
  //SDB.SetExtraProps(this); //assume SDB is valid
  }

//---------------------------------------------------------------------------

CSpeciePropDataBase::~CSpeciePropDataBase()
  {
  for (int i=0; i<iSpCnt; i++)
    delete SP[i];
  iSpCnt = 0;
  SDB.SetExtraProps(NULL); //assume SDB is valid
  }

//---------------------------------------------------------------------------

int CSpeciePropDataBase::Init(SpeciePropCfgItem * CfgItems, int Count)
  {
  PropCfg.Init(CfgItems, Count);
  iSpCnt = SVSpcCount();
  for (int j=0; j<iSpCnt; j++)
    {
    SP[j] = new SpecieProp(&PropCfg, j);
    }
  return 0;
  }

//---------------------------------------------------------------------------

SpecieProp * CSpeciePropDataBase::FindSolid(char* Tag)
  {
  const int len = strlen(Tag);
  for (int j=0; j<iSpCnt; j++)
    {
    CSpecie & psd = SDB[j];
    //if (psd.IsSol() && strncmp(Tag, psd.CmpTag())==0)
    if (psd.IsSol() && strlen(psd.SymOrTag())==size_t(len+3) && strnicmp(Tag, psd.SymOrTag(), len)==0)
      return SP[j];
    }
  return NULL;
  }

//---------------------------------------------------------------------------

SpecieProp * CSpeciePropDataBase::Find(int SpId)
  {
  for (int j=0; j<iSpCnt; j++)
    {
    if (SP[j]->iSId==SpId)
      return SP[j];
    }
  return NULL;
  }

//---------------------------------------------------------------------------

int CSpeciePropDataBase::Load(char* DataFile, char* TableName)
  {
  int RetCode = 0;
  CFileStatus State;
  if (CFile::GetStatus(DataFile, State))
    {
    CDBHelper DB(DataFile, TableName);
    if (DB.Open(DBConnect_Default, PropCfg.Name(0), DBH_HasHeadings|DBH_ReadOnly|DBH_ForwardOnly|DBH_TrimStrings))
      {
      if (DB.CheckFieldNames(PropCfg.PropNames()))
        {
        DB.CheckOptionalFieldNames(PropCfg.OptPropNames());
        for (int i=0; i<DB.NFieldIndexes(); i++)
          PropCfg.CfgItem(i)->bFound = (DB.FieldIndex(i)>=0);

        CStringArray Values;
        //flag DataReqd=0, EndFound=0, AppendComp=0, PrevWasLoaded=0;
        //pComponentD pC=NULL;
        while (!DB.IsEOF())
          {
          DB.GetNextRow(Values);
          const char* pTag = (const char*)Values[0];
          SpecieProp* pSP = FindSolid((char*)pTag);
          if (pSP && !pSP->bOK)
            {
            if (pSP->Load(Values)==0)
              {
              }
            else
              {
              LogError("SpeciePropDB", 0, "Error loading '%s'", pSP->Tag());
              RetCode = -4;
              }
            }
          }
        }
      else
        {
        Strng ProblemFields;
        for (int i=0; i<DB.NFieldIndexes(); i++)
          {
          const int indx = DB.FieldIndex(i);
          if (indx<0)
            {
            if (ProblemFields.Len()>0)
              ProblemFields += ", ";
            ProblemFields += PropCfg.Name(i);
            }
          }
        LogError("SpeciePropDB", 0, "Incorrect field names (%s): Table '%s' in '%s'", ProblemFields(), TableName, DataFile);
        RetCode = -3;
        }
      DB.Close();
      }
    else
      {
      LogError("SpeciePropDB", 0, "Unable to open Table '%s' in '%s'", TableName, DataFile);
      RetCode = -2;
      }
    }
  else
    {
    LogError("SpeciePropDB", 0, "File not found : '%s'", DataFile);
    RetCode = -1;
    }
  return RetCode;
  }

//---------------------------------------------------------------------------
//===========================================================================
