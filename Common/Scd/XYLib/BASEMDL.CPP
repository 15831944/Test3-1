//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992,1994
 
#include "stdafx.h"
#include <math.h>
#define __BASEMDL_CPP
#include "sc_defs.h"
#include "basemdl.h"

// =========================================================================

CBaseMdl::CBaseMdl(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  FxdEdtBookRef(NULL),
  TaggedObject(pClass_, pTag, pAttach, eAttach)
  {
  Clear();
  }

// -------------------------------------------------------------------------

void CBaseMdl::Clear()
  {
  SetNParms(0L);
  iMaxIter = 100L;
  iIter = 0L;
  dSa = 0.0;
  }

// -------------------------------------------------------------------------

void CBaseMdl::CopyModel(pCBaseMdl pMd)
  {
  if (pMd==this)
    return;
  iNParms = pMd->iNParms;
  Parms = pMd->Parms;
  MinParms = pMd->MinParms;
  MaxParms = pMd->MaxParms;
  ParmsTol = pMd->ParmsTol;
  iMaxIter = pMd->iMaxIter;
  }

// -------------------------------------------------------------------------

void CBaseMdl::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (iNParms>0)
    {
    DDB.Long("NParms",   "", DC_, "", &iNParms   , this, isParm);
    DDB.Long("MaxIter",  "", DC_, "", &iMaxIter  , this, isParm);
    DDB.BeginArray(this, "Parms", "Parms", iNParms, 0);
    for (long i=0; i<iNParms; i++)
      {
      DDB.BeginElement(this, i); 
      DDB.Double("Parm",   "",     DC_,    "",   &Parms.m_d[i],      this, isParm);
      DDB.Double("Min" ,   "",     DC_,    "",   &MinParms.m_d[i],   this, isParm);
      DDB.Double("Max" ,   "",     DC_,    "",   &MaxParms.m_d[i],   this, isParm);
      DDB.Double("Tol" ,   "",     DC_,    "",   &ParmsTol.m_d[i],   this, isParm);
      }
    DDB.EndArray();
    }
  }

// -------------------------------------------------------------------------

void CBaseMdl::SetNParms(long NParms)
  {
  iNParms = Max(0L, NParms);
  Parms.SetSize(iNParms, 0.0);
  MinParms.SetSize(iNParms, -1000.0);
  MaxParms.SetSize(iNParms, 1000.0);
  ParmsTol.SetSize(iNParms, 1.0e-6);
  }

// -------------------------------------------------------------------------

const int Id_MaxIter=100;
const int Id_Iter=101;
const int Id_Sa=102;
const int Id_Parm=103;
const int Id_MinParm=104;
const int Id_MaxParm=105;
const int Id_ParmTol=106;

void CBaseMdl::BuildParmsBlk(flag bShowEqn)
  {
  flag ShowAndParms = bShowEqn && iNParms>0;
  StartBlk(ShowAndParms ? 2+(int)iNParms : 0, ShowAndParms ? 2 : 0, NULL);
  if (ShowAndParms)
    {
    int L=0;
    SetDParm(L,"Max Iter", 9, "", Id_MaxIter ,  5, 2, "");
    SetDParm(L,"Iter", 5, "", Id_Iter ,  5, 2, "", False);
    SetDParm(L,"Sa", 3, "", Id_Sa ,  12, 2, "", False);
    L++;
    SetSpace(L,8);
    //SetDesc(L, "Parameters", -1, 10, 2, "");
    SetDesc(L,"Parameter", -1, 7, 2, "");
    SetDesc(L,"Min", -1, 10, 2, "");
    SetDesc(L,"Max", -1, 10, 2, "");
    SetDesc(L,"Tol", -1, 10, 2, "");
    for (long i=0; i<iNParms; i++)
      {
      L++;
      SetDesc(L, GetParmDesc(i) , -1 , 10, 2, "");
      SetParm(L, "", Id_Parm, 12, 2, "");
      SetParm(L, "", Id_MinParm, 10, 2, "");
      SetParm(L, "", Id_MaxParm, 10, 2, "");
      SetParm(L, "", Id_ParmTol, 10, 2, "");
      }
    }
  }

// -------------------------------------------------------------------------

void CBaseMdl::LoadParmsBlk(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_MaxIter:  Str.Set("%i", iMaxIter);     break;
      case Id_Iter:     Str.Set("%i", iIter);        break;
      case Id_Sa:       Str.Set("%g", dSa);          break;
      case Id_Parm:     Str.Set("%g", Parms[i]);     break;
      case Id_MinParm:  Str.Set("%g", MinParms[i]);  break;
      case Id_MaxParm:  Str.Set("%g", MaxParms[i]);  break;
      case Id_ParmTol:  Str.Set("%g", ParmsTol[i]);  break;
      }
    }
  }

// -------------------------------------------------------------------------

long CBaseMdl::ParseParmsBlk(FxdEdtInfo &EI, Strng & Str, flag &bObjModified)
  {
  if (CurrentBlk(EI))
    {
    bObjModified=1;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_MaxIter: iMaxIter = Str.SafeAtoL();     break;
      case Id_Parm   : Parms[i] = Str.SafeAtoF();     break;
      case Id_MinParm: MinParms[i] = Str.SafeAtoF();  break;
      case Id_MaxParm: MaxParms[i] = Str.SafeAtoF();  break;
      case Id_ParmTol: ParmsTol[i] = Str.SafeAtoF();  break;
      }
    }
  return 1;
  }

// -------------------------------------------------------------------------

long CBaseMdl::ButtonPushedParmsBlk(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {
    }
  return 0;
  }

// -------------------------------------------------------------------------

