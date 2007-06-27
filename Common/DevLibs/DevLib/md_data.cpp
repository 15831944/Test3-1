//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __MD_SPDATA_CPP
#include "md_share1.h"
#include "md_share2.h"
#include "md_share3.h"
#include "md_share4.h"
#include "md_defn.h"
#include "md_data.h"

#include "md_defn.h"
#include "md_base.h"
#include "md_impl.h"
//#include "md_method.h"
//#include "m_base.h"


#include "scdmacros.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

void MDataDefn::Double(LPCSTR Tag, LPCSTR Sym, double* Data, MD_Flags Flags, MCnv & Cnv)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Double((LPSTR)Tag, (LPSTR)Sym, Cnv.m_Index, Cnv.m_Txt.GetLength()?(LPTSTR)(LPCTSTR)Cnv.m_Txt:Cnvs[Cnv.m_Index]->SICnv()->Txt(), Data, m_pCommon->m_pTO, (Flags));
  //Cnv.m_pTxt=NULL;
  }

void MDataDefn::Float(LPCSTR Tag, LPCSTR Sym, float* Data, MD_Flags Flags, MCnv & Cnv)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Float((LPSTR)Tag, (LPSTR)Sym, Cnv.m_Index, Cnv.m_Txt.GetLength()?(LPTSTR)(LPCTSTR)Cnv.m_Txt:Cnvs[Cnv.m_Index]->SICnv()->Txt(), Data, m_pCommon->m_pTO, (Flags));
  //Cnv.m_pTxt=NULL;
  }

void MDataDefn::Long(LPCSTR Tag, LPCSTR Sym, long* Data, MD_Flags Flags, MDDValueLst * Values)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Long((LPSTR)Tag, (LPSTR)Sym, DC_, "", Data, m_pCommon->m_pTO, (Flags), (DDBValueLst*)Values);
  }

void MDataDefn::Short(LPCSTR Tag, LPCSTR Sym, short* Data, MD_Flags Flags, MDDValueLst * Values)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Short((LPSTR)Tag, (LPSTR)Sym, DC_, "", Data, m_pCommon->m_pTO, (Flags), (DDBValueLst*)Values);
  }

void MDataDefn::Bool(LPCSTR Tag, LPCSTR Sym, bool* Data, MD_Flags Flags, MDDValueLst * Values)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    {
    *Data = (*Data!=0 ? true : false);
    m_pCommon->m_pDDB->Bool((LPSTR)Tag, (LPSTR)Sym, DC_, "", Data, m_pCommon->m_pTO, (Flags), (DDBValueLst*)Values);
    }
  }

void MDataDefn::CheckBox(LPCSTR Tag, LPCSTR Sym, bool* Data, MD_Flags Flags)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    {
    *Data = (*Data!=0 ? true : false);
    m_pCommon->m_pDDB->CheckBoxBtn((LPSTR)Tag, (LPSTR)Sym, DC_, "", Data, m_pCommon->m_pTO, (Flags), (DDBValueLst*)NULL);
    }
  }

//---------------------------------------------------------------------------

void MDataDefn::Double(LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags, MCnv & Cnv)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Double((LPSTR)Tag, (LPSTR)Sym, Cnv.m_Index, Cnv.m_Txt.GetLength()?(LPTSTR)(LPCTSTR)Cnv.m_Txt:Cnvs[Cnv.m_Index]->SICnv()->Txt(), SMDKXID(DataHandle), m_pCommon->m_pTO, (Flags));
  //Cnv.m_pTxt=NULL;
  }

void MDataDefn::Float(LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags, MCnv & Cnv)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Float((LPSTR)Tag, (LPSTR)Sym, Cnv.m_Index, Cnv.m_Txt.GetLength()?(LPTSTR)(LPCTSTR)Cnv.m_Txt:Cnvs[Cnv.m_Index]->SICnv()->Txt(), SMDKXID(DataHandle), m_pCommon->m_pTO, (Flags));
  //Cnv.m_pTxt=NULL;
  }

void MDataDefn::Long(LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags, MDDValueLst * Values)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Long((LPSTR)Tag, (LPSTR)Sym, DC_, "", SMDKXID(DataHandle), m_pCommon->m_pTO, (Flags), (DDBValueLst*)Values);
  }

void MDataDefn::Short(LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags, MDDValueLst * Values)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Short((LPSTR)Tag, (LPSTR)Sym, DC_, "", SMDKXID(DataHandle), m_pCommon->m_pTO, (Flags), (DDBValueLst*)Values);
  }

void MDataDefn::Bool(LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags, MDDValueLst * Values)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Bool((LPSTR)Tag, (LPSTR)Sym, DC_, "", SMDKXID(DataHandle), m_pCommon->m_pTO, (Flags), (DDBValueLst*)Values);
  }

void MDataDefn::CheckBox(LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->CheckBoxBtn((LPSTR)Tag, (LPSTR)Sym, DC_, "", SMDKXID(DataHandle), m_pCommon->m_pTO, (Flags), (DDBValueLst*)NULL);
  }

void MDataDefn::String(LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->String((LPSTR)Tag, (LPSTR)Sym, DC_, "", SMDKXID(DataHandle), m_pCommon->m_pTO, (Flags), (DDBValueLst*)NULL);
  }

void MDataDefn::Button(LPCSTR Tag, LPCSTR Sym, long DataHandle, MD_Flags Flags)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Button((LPSTR)Tag, (LPSTR)Sym, DC_, "", SMDKXID(DataHandle), m_pCommon->m_pTO, (Flags), (DDBValueLst*)NULL);
  }

//---------------------------------------------------------------------------

void MDataDefn::Show(bool ViewVisible, bool FileVisible, bool SnapVisible)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    {
    m_pCommon->m_pDDB->Visibility(NSHM_All, ViewVisible, FileVisible, SnapVisible, SnapVisible);
    }
  }

void MDataDefn::Text(LPCSTR pName, MD_Flags Flags/*=0*/)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Text((LPSTR)pName, (Flags));
  }

void MDataDefn::Page(LPCSTR pName, bool Optional/*=false*/, MD_Flags Flags/*=0*/)
  {
  if (m_pCommon->m_pDDB->SearchingAtLevel())
    m_pCommon->m_pDDB->Page((LPSTR)pName, Optional ? DDB_OptPage:DDB_RqdPage, (Flags));
  }

void MDataDefn::StructBegin(LPCSTR pName, MD_Flags Flags/*=0*/)
  {
  m_pCommon->m_pDDB->BeginStruct(m_pCommon->m_pTO, (LPSTR)pName, NULL, DDB_NoPage, -1, (Flags));
  }
void MDataDefn::StructEnd()
  {
  m_pCommon->m_pDDB->EndStruct();
  }

void MDataDefn::ObjectBegin(LPCSTR pClassName, LPCSTR pName, MD_Flags Flags/*=0*/)
  {
  m_pCommon->m_pDDB->BeginObject(m_pCommon->m_pTO, (LPSTR)pName, (LPSTR)pClassName, NULL, DDB_NoPage, -1, (Flags));
  }
void MDataDefn::ObjectEnd()
  {
  m_pCommon->m_pDDB->EndObject();
  }

void MDataDefn::Object(LPTAGGEDOBJECT Object, MDDPages Pg/*=MDD_OptPage*/, MD_Flags Flags/*=0*/)
  {
  m_pCommon->m_pDDB->Object(
    Object, m_pCommon->m_pTO,
    NULL,//(LPSTR)Tag,
    NULL,//(LPSTR)TagComment,
    Pg==MDD_RqdPage?DDB_RqdPage : Pg==MDD_NoPage?DDB_NoPage:DDB_OptPage,
    -1, Flags);
  }

void MDataDefn::ArrayBegin(LPCSTR pClassName, LPCSTR pName, long ElementCount, long LengthHandle, MD_Flags Flags)
  {
  m_pCommon->m_pDDB->BeginArray(m_pCommon->m_pTO, (LPSTR)pName, (LPSTR)pClassName, ElementCount, LengthHandle, DDB_NoPage, (Flags));
  };
void MDataDefn::ArrayElementStart(LPCSTR sIndex, MD_Flags Flags)
  {
  m_pCommon->m_pDDB->BeginElement(m_pCommon->m_pTO, (LPSTR)sIndex, NULL, -1, DDB_NoPage, (Flags));
  };
void MDataDefn::ArrayElementStart(long   iIndex, MD_Flags Flags)
  {
  m_pCommon->m_pDDB->BeginElement(m_pCommon->m_pTO, iIndex, NULL, -1, DDB_NoPage, (Flags));
  };
void MDataDefn::ArrayElementEnd(MD_Flags Flags)
  {
  m_pCommon->m_pDDB->EndElement(Flags);
  };
void MDataDefn::ArrayEnd(MD_Flags Flags)
  {
  m_pCommon->m_pDDB->EndArray(Flags);
  };

void MDataDefn::MatrixBegin(LPCSTR pClassName, LPCSTR pName, long ColCount, long RowCount, long ColWidth, long Gap, MD_Flags Flags)
  {
  m_pCommon->m_pDDB->BeginMatrix(m_pCommon->m_pTO, (LPSTR)pName, (LPSTR)pClassName, ColCount, RowCount, ColWidth, Gap, DDB_NoPage, Flags);
  };

void MDataDefn::MatrixBegin(LPCSTR pClassName, LPCSTR pName, long ColCount, long RowCount, long ColWidth, long Gap, LPCSTR ColHdr, LPCSTR RowHdr, MD_Flags Flags)
  {
  m_pCommon->m_pDDB->BeginMatrix(m_pCommon->m_pTO, (LPSTR)pName, (LPSTR)pClassName, ColCount, RowCount, ColWidth, Gap, (LPSTR)ColHdr, (LPSTR)RowHdr, DDB_NoPage, Flags);
  };

//void MDataDefn::MatrixElementStart(LPCSTR sColIndex, LPCSTR sRowIndex, MD_Flags Flags)
//  {
//  m_pCommon->m_pDDB->BeginMatrixElement(m_pCommon->m_pTO, (LPSTR)sIndex, NULL, -1, DDB_NoPage, (Flags));
//  };
void MDataDefn::MatrixElementStart(long iColIndex, long iRowIndex, MD_Flags Flags)
  {
  m_pCommon->m_pDDB->BeginMatrixElement(m_pCommon->m_pTO, iColIndex, iRowIndex, NULL, -1, DDB_NoPage, Flags);
  };
void MDataDefn::MatrixElementEnd(MD_Flags Flags)
  {
  m_pCommon->m_pDDB->EndMatrixElement(Flags);
  };
void MDataDefn::MatrixEnd(MD_Flags Flags)
  {
  m_pCommon->m_pDDB->EndMatrix(Flags);
  };


void MDataDefn::GridBegin(LPCSTR GridNameText, long MaxCols, long MaxRows, long ColWidth, long DefaultGap)
  {
  m_pCommon->m_pDDB->BeginGrid((LPSTR)GridNameText, MaxCols, MaxRows, ColWidth, DefaultGap);
  };
void MDataDefn::GridColumnHeader(LPCSTR ColHdrText, long Width, long Gap, long Justification)
  {
  m_pCommon->m_pDDB->ColumnHeader((LPSTR)ColHdrText, Width, Gap, Justification);
  };
void MDataDefn::GridRowHeader(LPCSTR RowHdrText)
  {
  m_pCommon->m_pDDB->RowHeader((LPSTR)RowHdrText);
  };
void MDataDefn::GridRowStart()
  {
  m_pCommon->m_pDDB->RowStart("");
  };
void MDataDefn::GridEnd()
  {
  m_pCommon->m_pDDB->EndGrid("");
  };


bool MDataDefn::getForNDB()           { return m_pCommon->m_pDDB->ForNDB()!=0;           };
bool MDataDefn::getForFiling()        { return m_pCommon->m_pDDB->ForFiling()!=0;        };
bool MDataDefn::getForView()          { return m_pCommon->m_pDDB->ForView()!=0;          };
//bool MDataDefn::getForViewConstruct() { return m_pCommon->m_pDDB->ForViewConstruct()!=0; };
bool MDataDefn::getForSnapShot()      { return m_pCommon->m_pDDB->ForSnapShot()!=0;      };
bool MDataDefn::getForScenario()      { return m_pCommon->m_pDDB->ForScenario()!=0;      };
bool MDataDefn::getForFileSnpScn()    { return m_pCommon->m_pDDB->ForFileSnpScn()!=0;    };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

double MDataValidate::ValidateRange(LPCTSTR What, double MinV, double V, double MaxV, bool *pOK)
  {
  flag OK=1;
  double D=m_pCommon->m_pTO->ValidateRange(*m_pCommon->m_pVDB, What, MinV, V, MaxV, &OK);
  if (pOK)
    *pOK=OK!=0;
  return D;
  }

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

long    MDataChange::getHandle()          { return m_pCommon->m_pDCB->lHandle-SMDKXID(0); };
bool    MDataChange::getHasReqdValue()    { return m_pCommon->m_pDCB->rC!=NULL; };

double  MDataChange::getDouble()          { return *m_pCommon->m_pDCB->rD; };
float   MDataChange::getFloat()           { return *m_pCommon->m_pDCB->rF; };
long    MDataChange::getLong()            { return *m_pCommon->m_pDCB->rL; };
short   MDataChange::getShort()           { return *m_pCommon->m_pDCB->rS; };
bool    MDataChange::getBool()            { return (*m_pCommon->m_pDCB->rB)!=0 ? true : false; };
LPCTSTR MDataChange::getString()          { return m_pCommon->m_pDCB->rpC; };
void    MDataChange::putDouble(double V)  { m_pCommon->m_pDCB->D=V; };
void    MDataChange::putFloat(float  V)   { m_pCommon->m_pDCB->F=V; };
void    MDataChange::putLong(long   V)    { m_pCommon->m_pDCB->L=V; };
void    MDataChange::putShort(short  V)   { m_pCommon->m_pDCB->S=V; };
void    MDataChange::putBool(bool   V)    { m_pCommon->m_pDCB->B=(V!=0 ? true : false); };
void    MDataChange::putString(LPCTSTR V) { m_pCommon->m_pDCB->pC=(LPTSTR)V; };

bool    MDataChange::getForNDB()           { return m_pCommon->m_pDDB->ForNDB()!=0;           };
bool    MDataChange::getForFiling()        { return m_pCommon->m_pDDB->ForFiling()!=0;        };
bool    MDataChange::getForView()          { return m_pCommon->m_pDDB->ForView()!=0;          };
//bool    MDataChange::getForViewConstruct() { return m_pCommon->m_pDDB->ForViewConstruct()!=0; };
bool    MDataChange::getForSnapShot()      { return m_pCommon->m_pDDB->ForSnapShot()!=0;      };
bool    MDataChange::getForScenario()      { return m_pCommon->m_pDDB->ForScenario()!=0;      };
bool    MDataChange::getForFileSnpScn()    { return m_pCommon->m_pDDB->ForFileSnpScn()!=0;    };

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------

//===========================================================================
//
//
//
//===========================================================================
