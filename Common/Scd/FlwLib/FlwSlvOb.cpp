//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#include "datacnvs.h"
#define  __FLWSLVOB_CPP
#include "flwslvob.h"
#include "flw_err.h"
#include "resource.h"
#include "executiv.h"
#include "sfe_init.h"
#include "scd_wm.h"
#include "nrecipe.h"
#include "flwsolve.h"
#include "power.h"
#include "sp_dbextra.h"
#include "dbgmngr.h"

//#include "optoff.h"

//===========================================================================
//
//                                  SDBObject
//
//===========================================================================

IMPLEMENT_TAGOBJEDT(SDBObject, "FS_SDB", "FS_SDB", "", "SDB", TOC_SYSTEM, 
                    SDBObjectEdt, "The SDB", "The SDB");

SDBObject::SDBObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CTNode(pClass_, TagIn, pAttach, eAttach)
  {
  bDoSystemDefn=false;
  m_dDisplayT=Std_T;
  m_dDisplayP=Std_P;
  //m_iFidelity=0;
  m_bShowMs=true;
  m_bHiFidelity=SpModel::Fidelity()==1?1:0;
  }

//--------------------------------------------------------------------------

SDBObject::~SDBObject()
  {
  }

//--------------------------------------------------------------------------

XID xidMolWt     = ModelXID(1000);
XID xidLoT       = ModelXID(1000) +  1*MaxSpecies;
XID xidHiT       = ModelXID(1000) +  2*MaxSpecies;
XID xidmlHf25    = ModelXID(1000) +  3*MaxSpecies;
XID xidmlHf      = ModelXID(1000) +  4*MaxSpecies;
XID xidmlHs      = ModelXID(1000) +  5*MaxSpecies;
XID xidmlHz      = ModelXID(1000) +  6*MaxSpecies;
XID xidmlS       = ModelXID(1000) +  7*MaxSpecies;
XID xidmlG       = ModelXID(1000) +  8*MaxSpecies;
XID xidmlCp      = ModelXID(1000) +  9*MaxSpecies;
XID xidmsHf25    = ModelXID(1000) + 10*MaxSpecies;
XID xidmsHf      = ModelXID(1000) + 11*MaxSpecies;
XID xidmsHs      = ModelXID(1000) + 12*MaxSpecies;
XID xidmsHz      = ModelXID(1000) + 13*MaxSpecies;
XID xidmsS       = ModelXID(1000) + 14*MaxSpecies;
XID xidmsG       = ModelXID(1000) + 15*MaxSpecies;
XID xidmsCp      = ModelXID(1000) + 16*MaxSpecies;
XID xidRho       = ModelXID(1000) + 17*MaxSpecies;
XID xidVp        = ModelXID(1000) + 18*MaxSpecies;
XID xidVt        = ModelXID(1000) + 19*MaxSpecies;

XID xidExtraProp = ModelXID(1000)+20*MaxSpecies; //first ExtraProps ID
// NBNB Do NOT have Ids after xidExtraProp for MaxExtraProps*MaxSpecies !!!

DDBFnParms Parms0[]   = { {tt_NULL}}; 
DDBFnParms Parms00[]  = { {tt_Long,   (DC_),  "",    0    }, 
                          {tt_NULL}}; 
DDBFnParms Parms1[]   = { {tt_Double, (DC_T), "C",  Std_T}, 
                          {tt_Long,   (DC_),  "",    0    }, 
                          {tt_NULL}}; 
DDBFnParms Parms11[]  = { {tt_Double, (DC_P), "kPa",  Std_P}, 
                          {tt_Long,   (DC_),  "",    0    }, 
                          {tt_NULL}}; 
DDBFnParms Parms2[]   = { {tt_Double, (DC_T), "C",  Std_T}, 
                          {tt_Double, (DC_P), "kPa", Std_P}, 
                          {tt_Long,   (DC_),  "",    0    }, 
                          {tt_NULL}}; 

void SDBObject::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.Text  ("");
    DDB.CheckBoxBtn("MassBasis",  "", DC_, "",  &m_bShowMs  ,  this, isParm);
    DDB.Double("DisplayT",    "", DC_T, "C",    &m_dDisplayT,  this, isParm);
    DDB.Double("DisplayP",    "", DC_P, "kPa",  &m_dDisplayP,  this, isParm);
    
    DDB.Visibility(NSHM_All, TRUE, FALSE, FALSE, FALSE);
    for (int i=0; i<SDB.Count(); i++) 
      {
      if (DDB.BeginStruct(this, SDB[i].SymOrTag(), NULL, DDB_NoPage))
        {
        DDB.FnDouble("MoleWt",  "",       DC_,     "",              xidMolWt +i,   this, 0, Parms0);
        DDB.FnDouble("",        "LoT",    DC_T,    "C",             xidLoT   +i,   this, 0, Parms00);
        DDB.FnDouble("",        "HiT",    DC_T,    "C",             xidHiT   +i,   this, 0, Parms00);
        DDB.FnDouble("",        "mlHf25", DC_HMl,  "kJ/kmol",       xidmlHf25+i,   this, 0, Parms00);
        DDB.FnDouble("",        "mlHf",   DC_HMl,  "kJ/kmol",       xidmlHf  +i,   this, 0, Parms2);
        DDB.FnDouble("",        "mlHs",   DC_HMl,  "kJ/kmol",       xidmlHs  +i,   this, 0, Parms2);
        DDB.FnDouble("",        "mlHz",   DC_HMl,  "kJ/kmol",       xidmlHz  +i,   this, 0, Parms2);
        DDB.FnDouble("",        "mlSf",   DC_CpMl, "kJ/kmol.C",     xidmlS   +i,   this, 0, Parms2);
        DDB.FnDouble("",        "mlGf",   DC_HMl,  "kJ/kmol",       xidmlG   +i,   this, 0, Parms2);
        DDB.FnDouble("",        "mlCp",   DC_CpMl, "kJ/kmol.C",     xidmlCp  +i,   this, 0, Parms2);
        DDB.FnDouble("",        "msHf25", DC_HMs,  "kJ/kg",         xidmsHf25+i,   this, 0, Parms00);
        DDB.FnDouble("",        "msHf",   DC_HMs,  "kJ/kg",         xidmsHf  +i,   this, 0, Parms2);
        DDB.FnDouble("",        "msHs",   DC_HMs,  "kJ/kg",         xidmsHs  +i,   this, 0, Parms2);
        DDB.FnDouble("",        "msHz",   DC_HMs,  "kJ/kg",         xidmsHz  +i,   this, 0, Parms2);
        DDB.FnDouble("",        "msSf",   DC_CpMs, "kJ/kg.C",       xidmsS   +i,   this, 0, Parms2);
        DDB.FnDouble("",        "msGf",   DC_HMs,  "kJ/kg",         xidmsG   +i,   this, 0, Parms2);
        DDB.FnDouble("",        "msCp",   DC_CpMs, "kJ/kg.C",       xidmsCp  +i,   this, 0, Parms2);
        DDB.FnDouble("Density", "Rho",    DC_Rho,  "kg/m^3",        xidRho   +i,   this, 0, Parms2);
        DDB.FnDouble("VapourP", "Vp",     DC_P,    "kPa",           xidVp    +i,   this, 0, Parms1);
        DDB.FnDouble("VapourT", "Vt",     DC_T,    "C",             xidVt    +i,   this, 0, Parms11);
        }
      DDB.EndStruct();
      }    
    DDB.Visibility();
    
    if (SDB.ExtraProps())
      {
      CSpeciePropDataBase* pEP    = SDB.ExtraProps();
      const int MaxProps         = pEP->PropCount();
      SpeciePropCfgHelper* pPCfg = pEP->GetPropCfg();
      DDB.Visibility(NSHM_All, TRUE, FALSE, FALSE, FALSE);
      if (DDB.BeginStruct(this, "EP", NULL, DDB_NoPage))
        {
        for (int iSp=0; iSp<SDB.Count(); iSp++) 
          {
          SpecieProp * pPro1 = pEP->Find(iSp);
          SpecieProp * pProp = pEP->GetProp(iSp);
          ASSERT_RDB(pProp==pPro1, "XXX");
          if (pProp && pProp->OK())
            {
            if (DDB.BeginStruct(this, SDB[iSp].SymOrTag(), NULL, DDB_NoPage))
              {
              for (int j=1; j<MaxProps; j++)
                if (pPCfg->Display(j) && pPCfg->Found(j))
                  {
                  if (pPCfg->String(j))
                    DDB.String(pEP->PropName(j), "", DC_, "", xidExtraProp+(j*MaxSpecies)+iSp, this, 0);
                  else
                    DDB.Double(pEP->PropName(j), "", DC_, "", xidExtraProp+(j*MaxSpecies)+iSp, this, 0);
                  }
              }
            DDB.EndStruct();
            }
          }
        }
      DDB.EndStruct();
      DDB.Visibility();
      }
    }
  DDB.EndStruct();
  }

//--------------------------------------------------------------------------

#define PARM(N) DCB.pDataInfo->m_dParms[N]
#define FIDELITY(N) Range(0L, (long)DCB.pDataInfo->m_dParms[N], 1L)

flag SDBObject::DataXchg(DataChangeBlk & DCB)
  {
  if (CTNode::DataXchg(DCB))
    return true;

  if (DCB.lHandle>=xidMolWt && DCB.lHandle<xidExtraProp)
    {
    int s=(DCB.lHandle-xidMolWt)%MaxSpecies;
    int i=(DCB.lHandle-xidMolWt)/MaxSpecies;

    switch (i*MaxSpecies+xidMolWt)
      {
      case xidMolWt:  DCB.D=SDB[s].MoleWt(); return true;
      case xidLoT:    DCB.D=SDB[s].LoT(FIDELITY(0));                                  return true;
      case xidHiT:    DCB.D=SDB[s].HiT(FIDELITY(0));                                  return true;
      case xidmlHf25: DCB.D=SDB[s].mlHf(FIDELITY(0), C_2_K(25), 101.325, NULL, NULL); return true;
      case xidmlHf:   DCB.D=SDB[s].mlHf(FIDELITY(2), PARM(0), PARM(1), NULL, NULL); return true;
      case xidmlHs:   DCB.D=SDB[s].mlHs(FIDELITY(2), PARM(0), PARM(1), NULL, NULL); return true;
      case xidmlHz:   DCB.D=SDB[s].mlHz(FIDELITY(2), PARM(0), PARM(1), NULL, NULL); return true;
      case xidmlS:    DCB.D=SDB[s].mlSf(FIDELITY(2), PARM(0), PARM(1), NULL, NULL)-
                        (SDB[s].m_Data[FIDELITY(2)].m_dCSf[0]); return true;
      case xidmlG:    DCB.D=SDB[s].mlGf(FIDELITY(2), PARM(0), PARM(1), NULL, NULL); return true;
      case xidmlCp:   DCB.D=SDB[s].mlCp(FIDELITY(2), PARM(0), PARM(1), NULL, NULL); return true;
      case xidmsHf25: DCB.D=SDB[s].msHf(FIDELITY(0), C_2_K(25), 101.325, NULL, NULL); return true;
      case xidmsHf:   DCB.D=SDB[s].msHf(FIDELITY(2), PARM(0), PARM(1), NULL, NULL); return true;
      case xidmsHs:   DCB.D=SDB[s].msHs(FIDELITY(2), PARM(0), PARM(1), NULL, NULL); return true;
      case xidmsHz:   DCB.D=SDB[s].msHz(FIDELITY(2), PARM(0), PARM(1), NULL, NULL); return true;
      case xidmsS:    DCB.D=SDB[s].msSf(FIDELITY(2), PARM(0), PARM(1), NULL, NULL)-
                            (SDB[s].m_Data[FIDELITY(2)].m_dCSf[0]/SDB[s].m_dMoleWt); return true;
      case xidmsG:    DCB.D=SDB[s].msGf(FIDELITY(2), PARM(0), PARM(1), NULL, NULL); return true;
      case xidmsCp:   DCB.D=SDB[s].msCp(FIDELITY(2), PARM(0), PARM(1), NULL, NULL); return true;
      case xidRho:   
        if (SDB[s].DensityDirect())
          DCB.D=SDB[s].Density(FIDELITY(2), PARM(0), PARM(1), NULL, NULL); 
        else
          DCB.D=dNAN;
        return true;
      case xidVp:    DCB.D=SDB[s].VapourP(FIDELITY(1), PARM(0)); return true;
      case xidVt:    DCB.D=SDB[s].VapourT(FIDELITY(1), PARM(0)); return true;
      }
    }
  else if (DCB.lHandle>=xidExtraProp && SDB.ExtraProps())
    {
    CSpeciePropDataBase* pEP    = SDB.ExtraProps();
    const int MaxProps         = pEP->PropCount();
    SpeciePropCfgHelper* pPCfg = pEP->GetPropCfg();
    ASSERT_RDB(DCB.lHandle<xidExtraProp+(MaxProps*MaxSpecies), "Unexpected xid!");
    int iSp=(DCB.lHandle-xidExtraProp)%MaxSpecies;
    int iEp=(DCB.lHandle-xidExtraProp)/MaxSpecies;
    SpecieProp * pPro1 = pEP->Find(iSp);
    SpecieProp * pProp = pEP->GetProp(iSp);
    ASSERT_RDB(pProp==pPro1, "XXX");
    if (pProp && pProp->OK())
      {
      if (pPCfg->String(iEp))
        DCB.pC=pProp->StrProp(iEp);
      else
        DCB.D=pProp->Prop(iEp);
      }
    else
      {
      if (pPCfg->String(iEp))
        DCB.pC="?";
      else
        DCB.D=0.0;
      }
    return true;
    }
  return false;
  }

//--------------------------------------------------------------------------

flag SDBObject::ValidateData(ValidateDataBlk & VDB)
  {

  flag OK=CTNode::ValidateData(VDB);
  return OK;
  }


//===========================================================================
//
//
//
//===========================================================================

const int Id_DisplayT        =   1;
const int Id_DisplayP        =   2;
const int Id_ShowMass        =   3;
const int Id_HiFidelity      =   4;
                                  
const int Id_Name1           =   1*MaxSpecies;
//const int Id_SpSrc1          =   2*MaxSpecies;
const int Id_LoT             =   3*MaxSpecies;
const int Id_HiT             =   4*MaxSpecies;
const int Id_MolWt1          =   5*MaxSpecies;
const int Id_Sf1             =   6*MaxSpecies;
const int Id_Hf1             =   7*MaxSpecies;
const int Id_Gf1             =   8*MaxSpecies;
const int Id_Cp1             =   9*MaxSpecies;
const int Id_Hz1             =  10*MaxSpecies;
const int Id_Hs1             =  11*MaxSpecies;
const int Id_Vp1             =  12*MaxSpecies;
const int Id_Vt1             =  13*MaxSpecies;
const int Id_Rho1            =  14*MaxSpecies;
const int Id_RelSGs1         =  15*MaxSpecies;

const int Id_ExtraProp1      =  20*MaxSpecies; //first ExtraProps ID
// NBNB Do NOT have Ids after Id_ExtraProp1 for MaxExtraProps*MaxSpecies !!!


//------------------------------------------------------------------------------

struct MappingSave
  {
  POINT    VOrg;
  SIZE     VExt;
  POINT    WOrg;
  SIZE     WExt;
  };

//---------------------------------------------------------------------------

SDBObjectEdt::SDBObjectEdt(pFxdEdtView pView_, pSDBObject pSDBO_) :
  FxdEdtBookRef(pView_),
  rSDBO(*pSDBO_)
  {
  TCnv.Set    (DC_T,    "C");
  PCnv.Set    (DC_P,    "kPa");
  CpCnvMl.Set (DC_CpMl, "kJ/kmol.K");
  HCnvMl.Set  (DC_HMl,  "kJ/kmol");
  SCnvMl.Set  (DC_SMl,  "kJ/kmol.K");
  CpCnvMs.Set (DC_CpMs, "kJ/kg.K");
  HCnvMs.Set  (DC_HMs,  "kJ/kg");
  SCnvMs.Set  (DC_SMs,  "kJ/kg.K");
  RhoCnv.Set  (DC_Rho,  "kg/m^3");
  VpCnv.Set   (DC_P,    "kPa");
  VtCnv.Set   (DC_T,    "C");
  MlFmt.Set   ("", 0, 2, 'f');
  TFmt.Set    ("", 0, 2, 'f');
  PFmt.Set    ("", 0, 2, 'f');
  CpFmt.Set   ("", 0, 2, 'f');
  HFmt.Set    ("", 0, 2, 'f');
  RhoFmt.Set  ("", 0, 2, 'f');
  VpFmt.Set   ("", 0, 2, 'f');

  EPFmt.Set   ("", 0, 2, 'f'); //shared ExtraProps Fmt

  iPg1=0;
  iPgExtraProps=1;//page number of first extra properties page
  iNameWidth=12;
  iSpBlkCnt=0;
  iSpBlkLen[iSpBlkCnt]=0;
  iSpBlkLen[iSpBlkCnt+1]=-1;

  for (int i=0; i<SVCfgCount(); i++)
    {
    CSysVecItem &I=*SVI.Cfg(i);
    switch (I.Type())
      {
      case SVT_Specie:
        {
        int iSp=I.SDBIndex();
        iNameWidth=Max(iNameWidth, (int)(strlen(SDB[iSp].SymOrTag())+1));
        iSpBlkLen[iSpBlkCnt]++;
        iSpBlkLen[iSpBlkCnt+1]=-1;
        }
        break;
      case SVT_Annotation:
        {
        switch (I.Annotation())
          {
          case SVA_Text:    
            iSpBlkCnt++;
            iSpBlkLen[iSpBlkCnt]=0;
            break;
          default:
            break;
          }
        break;
        }
      }
    }
  
  CProfINIFile PF(PrjIniFile());

  ObjectAttribute *pAttr=ObjAttributes.FindObject("SDBObjectEdt");
  if (pAttr)
    {
    pAttr->FieldFmtCnvs("T",     TFmt,   TCnv);
    pAttr->FieldFmtCnvs("P",     PFmt,   PCnv);
    pAttr->FieldFmtCnvs("CpMl",  CpFmt,  CpCnvMl);
    pAttr->FieldFmtCnvs("HMl",   HFmt,   HCnvMl);
    pAttr->FieldFmtCnvs("SMl",   SFmt,   SCnvMl);
    pAttr->FieldFmtCnvs("CpMs",  CpFmt,  CpCnvMs);
    pAttr->FieldFmtCnvs("HMs",   HFmt,   HCnvMs);
    pAttr->FieldFmtCnvs("SMs",   SFmt,   SCnvMs);
    pAttr->FieldFmtCnvs("Rho",   RhoFmt, RhoCnv);
    pAttr->FieldFmtCnvs("Vp",    VpFmt,  VpCnv);
    pAttr->FieldFmtCnvs("Vt",    VtFmt,  VtCnv);
    //pAttr->FieldFmtCnvs("EP",    EPFmt,  NULL);
    }
  }

//---------------------------------------------------------------------------

SDBObjectEdt::~SDBObjectEdt()
  {
  ObjectAttribute *pAttr=ObjAttributes.FindObject("SDBObjectEdt");
  if (pAttr)
    {
    pAttr->SetFieldFmt  ("T",    TFmt);
    pAttr->SetFieldCnvs ("T",    TCnv);
    pAttr->SetFieldFmt  ("P",    PFmt);
    pAttr->SetFieldCnvs ("P",    PCnv);
    pAttr->SetFieldFmt  ("CpMl", CpFmt);
    pAttr->SetFieldCnvs ("CpMl", CpCnvMl);
    pAttr->SetFieldFmt  ("HMl",  HFmt);
    pAttr->SetFieldCnvs ("HMl",  HCnvMl);
    pAttr->SetFieldFmt  ("SMl",  SFmt);
    pAttr->SetFieldCnvs ("SMl",  SCnvMl);
    pAttr->SetFieldCnvs ("CpMs", CpCnvMs);
    pAttr->SetFieldCnvs ("HMs",  HCnvMs);
    pAttr->SetFieldCnvs ("SMs",  SCnvMs);
    pAttr->SetFieldFmt  ("Rho",  RhoFmt);
    pAttr->SetFieldCnvs ("Rho",  RhoCnv);
    pAttr->SetFieldFmt  ("Vp",   VpFmt);
    pAttr->SetFieldCnvs ("Vp",   VpCnv);
    pAttr->SetFieldFmt  ("Vt",   VtFmt);
    pAttr->SetFieldCnvs ("Vt",   VtCnv);
    //pAttr->SetFieldFmt  ("EP",   EPFmt);
    }

  }

//---------------------------------------------------------------------------

int SDBObjectEdt::DistFromPgNo(int Pg)
  {
  return -1;
  }

//---------------------------------------------------------------------------

void SDBObjectEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void SDBObjectEdt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void SDBObjectEdt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void SDBObjectEdt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void SDBObjectEdt::StartBuild()
  {
  }

//static int iWd_Src        =  3; 
static int iWd_LoHiT      =  4;
static int iWd_MoleWt     =  7; 
static int iWd_Sf         =  10;
static int iWd_Hf         =  10;
static int iWd_Gf         =  10;
static int iWd_H          =  10;
static int iWd_Cp         =  10;
static int iWd_Vp         =  8; 
static int iWd_Vt         =  8; 
static int iWd_Rho        =  8; 
static int iWd_Corr       =  30;

int GetSp(int i) { return i<MaxSpecies ? i :(i%MaxSpecies);};
int GetID(int i) { return i<MaxSpecies ? i :(i/MaxSpecies)*MaxSpecies;};

//---------------------------------------------------------------------------

void SDBObjectEdt::Build()
  {
  Strng S;

  iPg1=pView->Pages;
#define ShowSteamTableEntropy 0
#if ShowSteamTableEntropy
  int H2Og_i = -1;
  int H2Ol_i = -1;
#endif

  StartPage("Specie Properties");

  if (1) //data Blk
    {
    StartBlk(8, 0, NULL); 
    int L=0;

    SetSpace(L,8);
    L++;
    SetDesc(L,"Mass Basis",  -1, 12, 0, "");
    SetCheckBoxBtn(L, rSDBO.m_bShowMs? "1" : "0", Id_ShowMass, 5, 2, " ", true);
    L++;
    SetDesc(L,"HiFidelity",  -1, 12, 0, "");
    SetCheckBoxBtn(L, rSDBO.m_bHiFidelity? "1" : "0", Id_HiFidelity, 5, 2, " ", true);
    L++;
    SetDParm(L,"Temperature", 12, "", Id_DisplayT, 10, 2, " ");
    SetDesc(L, TCnv.Text(),   -1, 6,  0, "");

    L++;
    SetDParm(L,"Pressure"   , 12, "", Id_DisplayP, 10, 2, " ");
    SetDesc(L, PCnv.Text(),   -1, 6,  0, "");
    
    //headings...
    L++;
    L++;
    SetDesc(L, "Specie",        -1, iNameWidth ,  0, "");
    //SetDesc(L, "Src",           -1, iWd_Src    ,  0, "");
    SetDesc(L, "LoT",           -1, iWd_LoHiT,  2, "");
    SetDesc(L, "HiT",           -1, iWd_LoHiT,  2, "");
    SetDesc(L, "MoleWt",        -1, iWd_MoleWt ,  2, "");
    SetDesc(L, "Hf",            -1, iWd_Hf     ,  2, "");
    SetDesc(L, "Hz",            -1, iWd_H      ,  2, "");
    SetDesc(L, "Hs",            -1, iWd_H      ,  2, "");
    SetDesc(L, "Entropy",       -1, iWd_Sf     ,  2, "");
    SetDesc(L, "Gf",            -1, iWd_Gf     ,  2, "");
    SetDesc(L, "Cp",            -1, iWd_Cp     ,  2, "");
    SetDesc(L, "VapourP",       -1, iWd_Vp     ,  2, "");
    SetDesc(L, "VapourT",       -1, iWd_Vt     ,  2, "");
    SetDesc(L, "Density",       -1, iWd_Rho    ,  2, " ");
    SetDesc(L, "Corrections @ 10% Mass",  -1, iWd_Corr,  0, "");
    
    L++;
    SetSpace(L,iNameWidth);
    //SetDesc(L, "",              -1, iWd_Src    ,  0, "");
    SetDesc(L, TCnv.Text(),     -1, iWd_LoHiT  ,  2, "");
    SetDesc(L, TCnv.Text(),     -1, iWd_LoHiT  ,  2, "");
    SetDesc(L, "",              -1, iWd_MoleWt ,  2, "");
    if (rSDBO.m_bShowMs)
      {
      SetDesc(L, HCnvMs.Text(),     -1, iWd_Hf     ,  2, "");
      SetDesc(L, HCnvMs.Text(),     -1, iWd_H      ,  2, "");
      SetDesc(L, HCnvMs.Text(),     -1, iWd_H      ,  2, "");
      SetDesc(L, SCnvMs.Text(),     -1, iWd_Sf     ,  2, "");
      SetDesc(L, HCnvMs.Text(),     -1, iWd_Gf     ,  2, "");
      SetDesc(L, CpCnvMs.Text(),    -1, iWd_Cp     ,  2, "");
      }
    else
      {
      SetDesc(L, HCnvMl.Text(),     -1, iWd_Hf     ,  2, "");
      SetDesc(L, HCnvMl.Text(),     -1, iWd_H      ,  2, "");
      SetDesc(L, HCnvMl.Text(),     -1, iWd_H      ,  2, "");
      SetDesc(L, SCnvMl.Text(),     -1, iWd_Sf     ,  2, "");
      SetDesc(L, HCnvMl.Text(),     -1, iWd_Gf     ,  2, "");
      SetDesc(L, CpCnvMl.Text(),    -1, iWd_Cp     ,  2, "");
      }
    SetDesc(L, VpCnv.Text(),    -1, iWd_Vp     ,  2, "");
    SetDesc(L, VtCnv.Text(),    -1, iWd_Vt     ,  2, "");
    SetDesc(L, RhoCnv.Text(),   -1, iWd_Rho    ,  2, " ");
    }

  if (1) // Other Blk
    {
    Strng Tg;

    bool DidHeadings = false;
  
    //int b=0;
    int L=0;
    StartBlk(iSpBlkCnt, 0, NULL);


    for (int i=0; i<SVCfgCount(); i++)
      {
      CSysVecItem &I=*SVI.Cfg(i);
      switch (I.Type())
        {
        case SVT_Specie:
          {
          int iSp=I.SDBIndex();
          //iNameWidth=Max(iNameWidth, (int)(strlen(SDB[iSp].SymOrTag())+1));
          //iSpBlkLen[j]++;
          //iSpBlkLen[j+1]=-1;

          SetDesc(L, SDB[iSp].SymOrTag(),  -1, iNameWidth,  0, "");
          //SetParm(L, "", Id_SpSrc1+iSp, iWd_Src   , 0, "");

          SetParm(L, "", Id_LoT+iSp   , iWd_LoHiT, 2, "");   // MoleWt
          Tg.Set("$SDB.%s.LoT()", SDB[iSp].SymOrTag());
          SetTag(Tg());

          SetParm(L, "", Id_HiT+iSp   , iWd_LoHiT, 2, "");   // MoleWt
          Tg.Set("$SDB.%s.HiT()", SDB[iSp].SymOrTag());
          SetTag(Tg());

          SetParm(L, "", Id_MolWt1+iSp   , iWd_MoleWt, 2, "");   // MoleWt
          Tg.Set("$SDB.%s.MoleWt()", SDB[iSp].SymOrTag());
          SetTag(Tg());
          #if ShowSteamTableEntropy
          if (H2Og_i<0 && strcmp(SDB[iSp].SymOrTag(), "H2O(g)")==0)
            H2Og_i = iSp;
          if (H2Ol_i<0 && strcmp(SDB[iSp].SymOrTag(), "H2O(l)")==0)
            H2Ol_i = iSp;
          #endif
          SetParm(L, "", Id_Hf1+iSp      , iWd_Hf, 2, "");       // Hf
          Tg.Set(rSDBO.m_bShowMs ? "$SDB.%s.msHf(%.2f,%.2f)":"$SDB.%s.mlHf(%.2f,%.2f)", SDB[iSp].SymOrTag(), rSDBO.m_dDisplayT, rSDBO.m_dDisplayP);
          SetTag(Tg(), rSDBO.m_bShowMs ? HCnvMs.Text() : HCnvMl.Text());                                                           
                                                                                                                                 
          SetParm(L, "", Id_Hz1+iSp      , iWd_H, 2, "");        // Hz                                                           
          Tg.Set(rSDBO.m_bShowMs ? "$SDB.%s.msHz(%.2f,%.2f)":"$SDB.%s.mlHz(%.2f,%.2f)", SDB[iSp].SymOrTag(), rSDBO.m_dDisplayT, rSDBO.m_dDisplayP);
          SetTag(Tg(), rSDBO.m_bShowMs ? HCnvMs.Text() : HCnvMl.Text());                                                           
                                                                                                                                 
          SetParm(L, "", Id_Hs1+iSp      , iWd_H, 2, "");        // Hs                                                           
          Tg.Set(rSDBO.m_bShowMs ? "$SDB.%s.msHs(%.2f,%.2f)":"$SDB.%s.mlHs(%.2f,%.2f)", SDB[iSp].SymOrTag(), rSDBO.m_dDisplayT, rSDBO.m_dDisplayP);
          SetTag(Tg(), rSDBO.m_bShowMs ? HCnvMs.Text() : HCnvMl.Text());                                                           
                                                                                                                                 
          SetParm(L, "", Id_Sf1+iSp       , iWd_Sf, 2, "");      // Sf                                                           
          Tg.Set(rSDBO.m_bShowMs ? "$SDB.%s.msSf(%.2f,%.2f)":"$SDB.%s.mlSf(%.2f,%.2f)", SDB[iSp].SymOrTag(), rSDBO.m_dDisplayT, rSDBO.m_dDisplayP);
          SetTag(Tg(), rSDBO.m_bShowMs ? SCnvMs.Text() : SCnvMl.Text());                                                           
                                                                                                                                 
          SetParm(L, "", Id_Gf1+iSp       , iWd_Gf, 2, "");      // Gf                                                           
          Tg.Set(rSDBO.m_bShowMs ? "$SDB.%s.msGf(%.2f,%.2f)":"$SDB.%s.mlGf(%.2f,%.2f)", SDB[iSp].SymOrTag(), rSDBO.m_dDisplayT, rSDBO.m_dDisplayP);
          SetTag(Tg(), rSDBO.m_bShowMs ? HCnvMs.Text() : HCnvMl.Text());                                                           
                                                                                                                                 
          SetParm(L, "", Id_Cp1+iSp      , iWd_Cp, 2, "");       // Cp                                                           
          Tg.Set(rSDBO.m_bShowMs ? "$SDB.%s.msCp(%.2f,%.2f)":"$SDB.%s.mlCp(%.2f,%.2f)", SDB[iSp].SymOrTag(), rSDBO.m_dDisplayT, rSDBO.m_dDisplayP);
          SetTag(Tg(), rSDBO.m_bShowMs ? CpCnvMs.Text() : CpCnvMl.Text());
        
          SetParm(L, "", Id_Vp1+iSp      , iWd_Vp, 2, "");       // VapourP
          Tg.Set("$SDB.%s.Vp(%.2f)"  , SDB[iSp].SymOrTag(), rSDBO.m_dDisplayT);
          SetTag(Tg(), VpCnv.Text());
        
          SetParm(L, "", Id_Vt1+iSp      , iWd_Vt, 2, "");       // VapourT
          Tg.Set("$SDB.%s.Vt(%.2f)"  , SDB[iSp].SymOrTag(), rSDBO.m_dDisplayP);
          SetTag(Tg(), VtCnv.Text());
        
          SetParm(L, "", Id_Rho1+iSp     , iWd_Rho, 2, " ");     // Density
          Tg.Set("$SDB.%s.Rho(%.2f,%.2f)", SDB[iSp].SymOrTag(), rSDBO.m_dDisplayT, rSDBO.m_dDisplayP);
          SetTag(Tg(), RhoCnv.Text());
        
          SetDesc(L, "", Id_RelSGs1+iSp, iWd_Corr, 0, "");          // DensityCorrs
          L++;
          //DidHeadings = false;

          }
          break;
        case SVT_Annotation:
          {
          switch (I.Annotation())
            {
            case SVA_Text:    
              if (DidHeadings)
                {
                SetDesc(L, S(),             -1, S.Length() ,  0, "");
                }
              else
                {
                //headings...
                SetDesc(L, S(),             -1, iNameWidth/*+iWd_Src*/,  0, "");
                SetDesc(L, "LoT",           -1, iWd_LoHiT  ,  2, "");
                SetDesc(L, "HiT",           -1, iWd_LoHiT  ,  2, "");
                SetDesc(L, "MoleWt",        -1, iWd_MoleWt ,  2, "");
                SetDesc(L, "Hf",            -1, iWd_Hf     ,  2, "");
                SetDesc(L, "Hz",            -1, iWd_H      ,  2, "");
                SetDesc(L, "Hs",            -1, iWd_H      ,  2, "");
                SetDesc(L, "Entropy",       -1, iWd_Sf     ,  2, "");
                SetDesc(L, "Gf",            -1, iWd_Gf     ,  2, "");
                SetDesc(L, "Cp",            -1, iWd_Cp     ,  2, "");
                SetDesc(L, "VapourP",       -1, iWd_Vp     ,  2, "");
                SetDesc(L, "VapourT",       -1, iWd_Vt     ,  2, "");
                SetDesc(L, "Density",       -1, iWd_Rho    ,  2, " ");
                SetDesc(L, "Corrections @ 10% Mass",  -1, iWd_Corr,  0, "");
                DidHeadings = true;
                }
              L++;
              //j++;
              //iSpBlkLen[j]=0;
              break;
            default:
              break;
            }
          break;
          }
        }
      }
    }
  
#if ShowSteamTableEntropy
  if (1)
    {//debug
//    const int H2Og_i = H2OVap();
//    const int H2Ol_i = H2OLiq();
    Strng ss;
    int L=0;
    StartBlk(12, 0, NULL);
    L++;
    double T = rSDBO.m_dDisplayT;
    double P = rSDBO.m_dDisplayP;
    ss.Set("Values at Temperature:%g   Pressure:%g", T, P);
    SetDesc(L++, ss(),  -1, ss.Len(),  0, "");
    ss.Set("Steam entropy constant  :%g", SDB[H2Og_i].dCSf[0]);
    SetDesc(L++, ss(),  -1, ss.Len(),  0, "");
    ss.Set("Steam entropy           :%g", SDB[H2Og_i].msSf(T, P, NULL, NULL)-(SDB[H2Og_i].dCSf[0]/SDB[H2Og_i].dMoleWt));
    SetDesc(L++, ss(),  -1, ss.Len(),  0, "");
    ss.Set("Steam enthalpy constant :%g", SDB[H2Og_i].dCHf[0]);
    SetDesc(L++, ss(),  -1, ss.Len(),  0, "");
    ss.Set("Steam enthalpy          :%g", SDB[H2Og_i].msHf(T, P, NULL, NULL)-(SDB[H2Og_i].dCHf[0]/SDB[H2Og_i].dMoleWt));
    SetDesc(L++, ss(),  -1, ss.Len(),  0, "");

    ss.Set("Water entropy constant  :%g", SDB[H2Ol_i].dCSf[0]);
    SetDesc(L++, ss(),  -1, ss.Len(),  0, "");
    ss.Set("Water entropy           :%g", SDB[H2Ol_i].msSf(T, P, NULL, NULL)-(SDB[H2Ol_i].dCSf[0]/SDB[H2Ol_i].dMoleWt));
    SetDesc(L++, ss(),  -1, ss.Len(),  0, "");
    ss.Set("Water enthalpy constant :%g", SDB[H2Ol_i].dCHf[0]);
    SetDesc(L++, ss(),  -1, ss.Len(),  0, "");
    ss.Set("Water enthalpy          :%g", SDB[H2Ol_i].msHf(T, P, NULL, NULL)-(SDB[H2Ol_i].dCHf[0]/SDB[H2Ol_i].dMoleWt));
    SetDesc(L++, ss(),  -1, ss.Len(),  0, "");
    }
#endif

  iPgExtraProps=pView->Pages;
  if (SDB.ExtraProps())
    {
    CSpeciePropDataBase* pEP    = SDB.ExtraProps();
    const int MaxProps         = pEP->PropCount();
    const int MaxSp            = pEP->GetCount();
    SpeciePropCfgHelper* pPCfg = pEP->GetPropCfg();

    const int iPropWidth = 12;
    StartPage("Extra Properties");
    if (1) // ExtraProps heading Blk
      {
      StartBlk(3, 0, NULL);
      int L=1;
      SetDesc(L, "Extra Specie Properties DataBase ($ESPDB)...", -1, 60,  0, "");
      L++;
      //headings...
      SetDesc(L, "Specie",        -1, iNameWidth ,  0, "");
      for (int j=1; j<MaxProps; j++)
        if (pPCfg->Display(j) && pPCfg->Found(j))
          SetDesc(L, pEP->PropName(j), -1, pPCfg->Width(j),  0, "");
      }

    if (1) // ExtraProps data Blk
      {
      Strng Tg;
      int L=0;
      StartBlk(iSpBlkCnt, 0, NULL);

      for (int i=0; i<SVCfgCount(); i++)
        {
        CSysVecItem &I=*SVI.Cfg(i);
        switch (I.Type())
          {
          case SVT_Specie:
            {
            int iSp=I.SDBIndex();
            SpecieProp * pPro1 = pEP->Find(iSp);
            SpecieProp * pProp = pEP->GetProp(iSp);
            ASSERT_RDB(pProp==pPro1, "XXX");
            if (pProp)
              {
              SetDesc(L, SDB[iSp].SymOrTag(),  -1, iNameWidth,  0, "");
              //SetDesc(L, pProp->Tag(), -1, iNameWidth ,  0, "");
              if (pProp->OK())
                {
                for (int j=1; j<MaxProps; j++)
                  if (pPCfg->Display(j) && pPCfg->Found(j))
                    {
                    //SetDesc(L, pProp->StrProp(j), -1, pPCfg->Width(j) ,  0, "");
                    SetParm(L, "", Id_ExtraProp1+(j*MaxSpecies)+iSp, pPCfg->Width(j), 2, "");
                    Tg.Set("$SDB.EP.%s.%s", SDB[iSp].SymOrTag(), pEP->PropName(j));
                    SetTag(Tg());
                    }
                  
                }
              else
                SetDesc(L, "  Data not loaded!", -1, 20,  0, "");
              L++;
              }
            }
          }
        }
      }
    }
  }

//---------------------------------------------------------------------------

static inline void FixWide(int &W, Strng &Str, FxdEdtView &View) 
  { 
  int x=W; 
  W=Max(W, Min(14, Str.Length())+1); 
  if (x!=W) 
    View.DoRebuild(); 
  };


void SDBObjectEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (EI.PageNo<iPgExtraProps)
    {
    if (CurrentBlk(EI))
      {//data
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_ShowMass:  
          Str=rSDBO.m_bShowMs? "1" : "0";
          break;
        case Id_HiFidelity:  
          Str=rSDBO.m_bHiFidelity? "1" : "0";
          break;
        case Id_DisplayT:  
          TFmt.FormatFloat(TCnv.Human(rSDBO.m_dDisplayT), Str);
          break;
        case Id_DisplayP:  
          PFmt.FormatFloat(PCnv.Human(rSDBO.m_dDisplayP), Str);
          break;
        }
      }

    if (CurrentBlk(EI))
      {//other
      int p=EI.PageNo;
      int iSp=GetSp(EI.FieldId);
      int iId=GetID(EI.FieldId);
      double D;
      //long m_lFidelity=0;
      switch (iId)
        {
        //case Id_SpSrc1:
        //  Str=SDB[iSp].PrjDB() ? "Prj" : "Lib";
        //  FixWide(iWd_Src, Str, View());
        //  break;
        case Id_LoT:
          TFmt.FormatFloat(TCnv.Human(SDB[iSp].LoT(rSDBO.m_bHiFidelity?1:0)), Str);
          FixWide(iWd_LoHiT, Str, View());
          break;
        case Id_HiT:
          TFmt.FormatFloat(TCnv.Human(SDB[iSp].HiT(rSDBO.m_bHiFidelity?1:0)), Str);
          FixWide(iWd_LoHiT, Str, View());
          break;
        case Id_MolWt1:
          MlFmt.FormatFloat(SDB[iSp].MoleWt(), Str);
          FixWide(iWd_MoleWt, Str, View());
          break;
        case Id_Sf1:
          if (rSDBO.m_bShowMs)
            D=SDB[iSp].msSf(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL)-(SDB[iSp].m_Data[rSDBO.m_bHiFidelity?1:0].m_dCSf[0]/SDB[iSp].m_dMoleWt);
          else
            D=SDB[iSp].mlSf(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL)-(SDB[iSp].m_Data[rSDBO.m_bHiFidelity?1:0].m_dCSf[0]);
          /*if (rSDBO.m_bShowMs)
            D=SDB[iSp].msSf(rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL);
          else
            D=SDB[iSp].mlSf(rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL);*/
          HFmt.FormatFloat(D, Str);
          FixWide(iWd_Hf, Str, View());
          break;
        case Id_Gf1:
          if (rSDBO.m_bShowMs)
            D=/*HCnvMs.Human*/(SDB[iSp].msGf(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL));
          else
            D=/*HCnvMl.Human*/(SDB[iSp].mlGf(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL));
          HFmt.FormatFloat(D, Str);
          FixWide(iWd_Hf, Str, View());
          break;
        case Id_Hf1:
          if (rSDBO.m_bShowMs)
            D=HCnvMs.Human(SDB[iSp].msHf(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL));
          else
            D=HCnvMl.Human(SDB[iSp].mlHf(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL));
          HFmt.FormatFloat(D, Str);
          FixWide(iWd_Hf, Str, View());
          break;
        case Id_Hz1:
          if (rSDBO.m_bShowMs)
            D=HCnvMs.Human(SDB[iSp].msHz(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL));
          else
            D=HCnvMl.Human(SDB[iSp].mlHz(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL));
          HFmt.FormatFloat(D, Str);
          FixWide(iWd_H, Str, View());
          break;
        case Id_Hs1:
          if (rSDBO.m_bShowMs)
            D=HCnvMs.Human(SDB[iSp].msHs(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL));
          else
            D=HCnvMl.Human(SDB[iSp].mlHs(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL));
          HFmt.FormatFloat(D, Str);
          FixWide(iWd_H, Str, View());
          break;
        case Id_Cp1:
          if (rSDBO.m_bShowMs)
            D=CpCnvMs.Human(SDB[iSp].msCp(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL));
          else
            D=CpCnvMl.Human(SDB[iSp].mlCp(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL));
          CpFmt.FormatFloat(D, Str);
          FixWide(iWd_Cp, Str, View());
          break;
        case Id_Vp1:
          VpFmt.FormatFloat(VpCnv.Human(SDB[iSp].VapourP(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT)), Str);
          FixWide(iWd_Vp, Str, View());
          break;
        case Id_Vt1:
          VtFmt.FormatFloat(VtCnv.Human(SDB[iSp].VapourT(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayP)), Str);
          FixWide(iWd_Vt, Str, View());
          break;
        case Id_Rho1:
          if (SDB[iSp].DensityDirect())
            RhoFmt.FormatFloat(RhoCnv.Human(SDB[iSp].Density(rSDBO.m_bHiFidelity?1:0, rSDBO.m_dDisplayT, rSDBO.m_dDisplayP, NULL, NULL)), Str);
          else
            Str="-";
          FixWide(iWd_Rho, Str, View());
          break;
        case Id_RelSGs1:
          {
          Str="";
          Strng T;
          for (int j=0; j<SDB[iSp].DensCorrCount(); j++)
            {
            CDensCorr & SI=SDB[iSp].DensCorr(j);
            if (j>0)
              Str+=",";
  //            Str+=SDB[SDB[iSp].SolnItem(j).iSpecie].SymOrTag();

            if (SI.m_pDensCorrFn)// || SI.m_pCpCorrFn)
              {
              //Strng buff1,buff2; 
              //if (SI.m_pDensCorrFn) buff1.Set("%10.2f", 1.+SI.DensCorr(0.1)); else buff1="";
              //if (SI.m_pDensCorrFn) buff2.Set("%10.2f", 1.+SI.DensCorr(0.1)); else buff2="";
              //buff1.Trim();
              //buff2.Trim();
              //T.Set("%s{%s,%s}", SDB[SI.m_iSoluteA].SymOrTag(), buff1(), buff2());
              Strng buff1,buff2; 
              if (SI.m_pDensCorrFn) buff1.Set("%10.2f", 1.+SI.DensCorr(0.1)); else buff1="";
              buff1.Trim();
              T.Set("%s{%s}", SDB[SI.m_iSoluteA].SymOrTag(), buff1());
              }
            Str+=T;
            }
          break;
          }
        }
      EI.Fld->fEditable=false;
      }
    }
  else
    {
    if (SDB.ExtraProps())
      {
      CSpeciePropDataBase* pEP    = SDB.ExtraProps();
      SpeciePropCfgHelper* pPCfg = pEP->GetPropCfg();
      if (CurrentBlk(EI))
        {//ExtraProps heading
        }
      if (CurrentBlk(EI))
        {//ExtraProps
        int iSp=(EI.FieldId-Id_ExtraProp1)%MaxSpecies;
        int iEp=(EI.FieldId-Id_ExtraProp1)/MaxSpecies;
        ASSERT(iSp<MaxSpecies);
        SpecieProp * pPro1 = pEP->Find(iSp);
        SpecieProp * pProp = pEP->GetProp(iSp);
        ASSERT_RDB(pProp==pPro1, "XXX");
        if (pProp && pProp->OK())
          {
          if (pPCfg->String(iEp))
            Str = pProp->StrProp(iEp);
          else
            EPFmt.FormatFloat(pProp->Prop(iEp), Str);
          }
        else
          {
          Str="???";
          }
        EI.Fld->fEditable=false;
        }
      }
    }
  }

//---------------------------------------------------------------------------

long SDBObjectEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; 
  if (EI.PageNo<iPgExtraProps)
    {
    if (CurrentBlk(EI))
      {//data
      int p=EI.PageNo;
  //    int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_ShowMass:
          Str.LTrim();
          rSDBO.m_bShowMs= Str() && (Str[0]=='1');
          View().DoRebuild();
          break;
        case Id_HiFidelity:  
          Str.LTrim();
          rSDBO.m_bHiFidelity = Str() && (Str[0]=='1');
          View().DoRebuild();
          break;
        case Id_DisplayT:  
          rSDBO.m_dDisplayT=TCnv.Normal(SafeAtoF(Str, rSDBO.m_dDisplayT));
          View().DoRebuild();
          break;
        case Id_DisplayP:  
          rSDBO.m_dDisplayP=PCnv.Normal(SafeAtoF(Str, rSDBO.m_dDisplayP));
          View().DoRebuild();
          break;
        }
      }

    if (CurrentBlk(EI))
      {//other
      }
    }
  else
    {
    if (SDB.ExtraProps())
      {
      CSpeciePropDataBase* pEP    = SDB.ExtraProps();
      SpeciePropCfgHelper* pPCfg = pEP->GetPropCfg();
      if (CurrentBlk(EI))
        {//ExtraProps heading
        }
      if (CurrentBlk(EI))
        {//ExtraProps
        }
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

long SDBObjectEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph
  if (EI.PageNo<iPgExtraProps)
    {
    if (CurrentBlk(EI))
      {//data
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_ShowMass:
          rSDBO.m_bShowMs=!rSDBO.m_bShowMs;
          View().DoRebuild();
          break;
        case Id_HiFidelity:  
          rSDBO.m_bHiFidelity=!rSDBO.m_bHiFidelity;
          View().DoRebuild();
          break;
        }
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

flag SDBObjectEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag SDBObjectEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag SDBObjectEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag SDBObjectEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag SDBObjectEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView &Vw=View();
  if (Vw.CPgNo<0)
    return false;
  FxdEdtInfo EI;
  if (!Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
    return false;
  flag ret=true;
  if (EI.Fld)
    {
    flag TagOnly=false;
    if (Vw.CPgNo<iPgExtraProps)
      {
      int iId=GetID(EI.FieldId);
      switch (iId)
        {
        case Id_DisplayT:
        case Id_LoT:
        case Id_HiT:
          WrkIB.Set(EI.Fld->Tag, &TCnv, &TFmt);
          break;
        case Id_DisplayP:
          WrkIB.Set(EI.Fld->Tag, &PCnv, &PFmt);
          break;
        case Id_MolWt1:
          WrkIB.Set(EI.Fld->Tag, NULL, &MlFmt);
          //TagOnly=true;
          break;
        case Id_Sf1:
          if (rSDBO.m_bShowMs)
            WrkIB.Set(EI.Fld->Tag, &HCnvMs, &HFmt);
          else
            WrkIB.Set(EI.Fld->Tag, &HCnvMs, &HFmt);
          break;
        case Id_Hf1:
        //case Id_Sf1:
        case Id_Gf1:
        case Id_Hz1:
        case Id_Hs1:
          if (rSDBO.m_bShowMs)
            WrkIB.Set(EI.Fld->Tag, &HCnvMs, &HFmt);
          else
            WrkIB.Set(EI.Fld->Tag, &HCnvMs, &HFmt);
          break;
        case Id_Cp1:
          if (rSDBO.m_bShowMs)
            WrkIB.Set(EI.Fld->Tag, &CpCnvMs, &CpFmt);
          else
            WrkIB.Set(EI.Fld->Tag, &CpCnvMl, &CpFmt);
          break;
        case Id_Rho1:
          WrkIB.Set(EI.Fld->Tag, &RhoCnv, &RhoFmt);
          break;
        case Id_Vp1:
          WrkIB.Set(EI.Fld->Tag, &VpCnv, &VpFmt);
          break;
        case Id_Vt1:
          WrkIB.Set(EI.Fld->Tag, &VtCnv, &VtFmt);
          break;
        default: 
          WrkIB.Set(EI.Fld->Tag);
        }
      }
    else if (SDB.ExtraProps())
      {
      //int iSp=(EI.FieldId-Id_ExtraProp1)%MaxSpecies;
      //int iEp=(EI.FieldId-Id_ExtraProp1)/MaxSpecies;
      WrkIB.Set(EI.Fld->Tag, NULL, &EPFmt);
      }

    if (WrkIB.FmtOK() || TagOnly)
      {
      CRect WRect;
      Vw.GetWindowRect(&WRect);
      CPoint  RBPoint;
      RBPoint.x = WRect.left+point.x;
      RBPoint.y = WRect.top+point.y;

      CMenu Menu;
      Menu.CreatePopupMenu();
      CMenu FormatMenu;
      FormatMenu.CreatePopupMenu();
      if (WrkIB.FmtOK())
        WrkIB.Fmt().AddToMenu(FormatMenu);
      CMenu CnvMenu;
      CnvMenu.CreatePopupMenu();
      if (WrkIB.CnvOK())
        WrkIB.Cnv().AddToMenu(CnvMenu);

      Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
      if (!WrkIB.CnvOK() || (WrkIB.CnvOK() && (WrkIB.Cnv().Index())<=0) || TagOnly)
        Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);

      Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");
      if (!WrkIB.FmtOK() || TagOnly)
        Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);

      Menu.AppendMenu(MF_SEPARATOR);
      Menu.AppendMenu(MF_STRING, pView->iCmdCopyTag, "Copy Full &Tag");
      Menu.AppendMenu(MF_STRING, pView->iCmdCopyRHSTag, "Copy Tag");
      if (EI.Fld->Tag==NULL)
        {
        Menu.EnableMenuItem(pView->iCmdCopyTag, MF_BYCOMMAND|MF_GRAYED);
        Menu.EnableMenuItem(pView->iCmdCopyRHSTag, MF_BYCOMMAND|MF_GRAYED);
        }
      Menu.AppendMenu(MF_STRING, pView->iCmdCopyVal, "Copy &Value");

      Menu.AppendMenu(MF_STRING, ID_EDIT_COPY, "Copy");
      Menu.AppendMenu(MF_STRING, ID_EDIT_PASTE, "Paste");
      
      Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, &View());
      Menu.DestroyMenu();                                           
      }

    }
  return ret;
  }

//---------------------------------------------------------------------------

flag SDBObjectEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag SDBObjectEdt::DoAccCnv(UINT Id) 
  {
  pCDataCnv pC=Cnvs[(WrkIB.Cnv().Index())];
  for (int i=Id; i>0; i--)
    pC=pC->Next();
    
  if (pC)
    {
    WrkIB.Cnv().SetText(pC->Txt());
    }
  View().DoRebuild();
  return true;
  };

//---------------------------------------------------------------------------

flag SDBObjectEdt::DoAccFmt(UINT Id) 
  {
  for (UINT i=0; i<(UINT)DefinedFmts.GetSize(); i++) 
    if (i==Id) 
      break;

  if (i<(UINT)DefinedFmts.GetSize())
    {
    WrkIB.Fmt()=DefinedFmts[i];
    View().DoRebuild();
    }
  return true;
  };

//---------------------------------------------------------------------------

flag SDBObjectEdt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  };

//===========================================================================
//
//                                  TearObject
//
//===========================================================================

IMPLEMENT_TAGOBJEDT(TearObject, "FS_Tears", "FS_Tears", "", "$Tears", TOC_SYSTEM, 
                    TearObjectEdt, "The Tears", "The Tears");
//IMPLEMENT_TAGOBJ(TearObject, "FS_Tears", "FS_Tears", "", "$Tears", TOC_SYSTEM, 
//                    "The Tears", "The Tears");

TearObject::TearObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CTNode(pClass_, TagIn, pAttach, eAttach)
  {
  bDoSystemDefn=false;
  };

TearObject::~TearObject()
  {
  }

//--------------------------------------------------------------------------

void TearObject::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {
/*    Strng Commnt, Nm;
    int i=0,iPg=0;
    TearVarBlkIter TCBs(TearVarBlk::List);
    if (DDB.ForView())
      {
      TearVarBlk *pT;
      for (pT=TCBs.First(); pT; pT=TCBs.Next())
        if (!pT->IsGroupBlk() && pT->IsTear())
          {
          if (i>0 && (i)%30==0)
            {
     	      Nm.Set("Tears%i", iPg++);
            DDB.Page(Nm(), DDB_RqdPage);
            }
   	      Nm.Set("T_%02i", i);
          DDBValueLst DDB0[] = {{ 0, pT->Tag()}, {1,  "Busy..."}, {0}};
          DDB.Button(Nm(),   "",  DC_ , "",   xidATearTag,   this, isParm|isTag, DDB0);
          i++;
          }
      }*/
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag TearObject::DataXchg(DataChangeBlk & DCB)
  {
  if (CTNode::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidATearTag:
      DCB.pC="";  // Ignore
      return 1;
    }

  return 0;
  }

//--------------------------------------------------------------------------

flag TearObject::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=CTNode::ValidateData(VDB);
  return OK;
  }

//===========================================================================
//
//
//
//===========================================================================


const int Id_TDfTearMethod        = 1;
const int Id_TDfCnvgdIters        = 2;
const int Id_TDfMaxIters          = 3;
const int Id_TDfDamping           = 4;
const int Id_TDfDamping_Growth    = 5;
const int Id_TDfDamping_Decay     = 6;
const int Id_TDfWegstein_Delay    = 7;
const int Id_TDfWegstein_Bound    = 8;
const int Id_TDfWegstein_Clamp    = 9;
const int Id_TDfRel               = 10;
const int Id_TDfAbs               = 11;

const int Id_TearBlkMethod        = 20;
const int Id_TearBlkNVars         = 21;
const int Id_TearBlkCnvCnt        = 22;
const int Id_TearBlkDampReqd      = 23;
const int Id_TearBlkGroup         = 24;
const int Id_TearBlkStrategy      = 25;
const int Id_TearBlkAbs           = 26;
const int Id_TearBlkRel           = 27;
const int Id_TearBlkFldBtns       = 28;
const int Id_TearBlkFind1Btn      = 29;
const int Id_TearBlkFind2Btn      = 30;
                                  
const int Id_TearDmpTearMeth      = 40;
const int Id_TearDmpDampAsGroup   = 41;
const int Id_TearDmpBlkDamp       = 42;

const int Id_TearDmpGlblDamp      = 45;
const int Id_TearDmpRqdDamp       = 46;
const int Id_TearDmpCurDamp       = 47;
const int Id_TearDmpInputs        = 48;

const int Id_TearTolGlblEPSRel    = 60;
const int Id_TearTolEPSRel        = 61;
const int Id_TearTolCurTol        = 62;
const int Id_TearTolDiff          = 63;
const int Id_TearTolConvergedCnt  = 64;
const int Id_TearTolMeas          = 65;

//const int Id_TearBlkEPSStrat          = 55;
//const int Id_TearBlkEPSRel            = 56;
//const int Id_TearBlkEPSAbs            = 57;
//const int Id_TearEPSAbs               = 58;
//const int Id_TearGlblEPSAbs           = 59;
//const int Id_TearOutputs              = 60;

//------------------------------------------------------------------------------

int TearObjectEdt::m_iSlct = -1; //global index to currently selected Tear

TearObjectEdt::TearObjectEdt(pFxdEdtView pView_, pTearObject pTolO_) :
  FxdEdtBookRef(pView_),
  rTearO(*pTolO_)
  {
  TCnv.Set  (DC_Frac,      "%");
  DCnv.Set  (DC_Frac,      "%");
  TFmt.Set ("", 0, 2, 'e');
  DFmt.Set ("", 0, 2, 'f');
  m_iSlctPg1=0;
  m_pSlctTVB=NULL;

  ObjectAttribute *pAttr=ObjAttributes.FindObject("TearObjectEdt");
  if (pAttr)
    {
    pAttr->FieldFmtCnvs("T",   TFmt,   TCnv);
    pAttr->FieldFmtCnvs("M",   DFmt,   DCnv);
    }
  }

//---------------------------------------------------------------------------

TearObjectEdt::~TearObjectEdt()
  {
  ObjectAttribute *pAttr=ObjAttributes.FindObject("TearObjectEdt");
  if (pAttr)
    {
    pAttr->SetFieldFmt  ("T",    TFmt);
    pAttr->SetFieldCnvs ("T",    TCnv);
    pAttr->SetFieldFmt  ("M",    DFmt);
    pAttr->SetFieldCnvs ("M",    DCnv);
    }
  }

//---------------------------------------------------------------------------

int TearObjectEdt::DistFromPgNo(int Pg)
  {
  return -1;
  }

//---------------------------------------------------------------------------

void TearObjectEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void TearObjectEdt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void TearObjectEdt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void TearObjectEdt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void TearObjectEdt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

void TearObjectEdt::Build()
  {
  Strng S, Tg, Tag;
  int i;
  int L=0;

  m_iSlctPg1=-1;

  m_Blks.SetSize(0, 64);//TearVarBlk::List.GetCount());

  m_pSlctTVB=NULL;

  TearVarBlkIter TCBs(TearVarBlk::List);
  TearVarBlk* pT;
  TearVarBlk* pT0=NULL;
  for (pT=TCBs.First(); pT; pT=TCBs.Next())
    {
    if (!pT->IsGroupBlk() && pT->IsTear())
      {
      if (pT0==NULL)
        pT0=pT;
      if (m_Blks.GetSize()==m_iSlct)
        m_pSlctTVB=pT;
      m_Blks.Add(pT);
      }
    }
  if (m_pSlctTVB==NULL)
    {
    m_pSlctTVB=pT0;
    m_iSlct=(m_pSlctTVB==NULL ? -1 : 0);
    }

  long TagWide=8;
  for (i=0; i<m_Blks.GetSize(); i++)
    if (m_Blks[i])
      TagWide=Max(TagWide, (long)strlen(m_Blks[i]->Tag()));

  if (1)
    {
    StartPage("Tears");
    StartBlk(16, 0, NULL); //global header
    L=0;
    SetSpace(L,2); 
    L++;
    SetSpace(L,1); SetDesc (L, "Convergence Defaults:",      -1,  24, 0, " ");
    L++;
    SetSpace(L,2); SetDParm(L, "TearMethod",    16, "", Id_TDfTearMethod, 10, 2, " ");
    for (i=1; TearMethodStrings[i]!=NULL; i++)
      FldHasFixedStrValue(i, TearMethodStrings[i]);
    SetTag("$PB.Method");
    L++;                                                  
    SetSpace(L,2); SetDParm(L, "CnvgdIters",    16, "", Id_TDfCnvgdIters, 10, 2, " ");
    SetTag("$PB.CnvgdIters");
    L++;                                                  
    SetSpace(L,2); SetDParm(L, "MaxIters",      16, "", Id_TDfMaxIters,   10, 2, " ");
    SetTag("$PB.MaxIters");
    L++;                                                  
    SetSpace(L,2); SetDParm(L, "Damping",       16, "", Id_TDfDamping,    10, 2, " ");
    SetTag("$PB.Damping", DCnv.Text()); SetDesc(L, DCnv.Text(), -1, 5, 0, " ");
    L++;
    SetSpace(L,2); SetDParm(L, "DampingGrowth", 16, "", Id_TDfDamping_Growth, 10, 2, " ");
    SetTag("$PB.DampingGrowth");
    L++;
    SetSpace(L,2); SetDParm(L, "DampingDecay",  16, "", Id_TDfDamping_Decay,  10, 2, " ");
    SetTag("$PB.DampingDecay");
    L++;
    SetSpace(L,2); SetDParm(L, "WegsteinDelay", 16, "", Id_TDfWegstein_Delay, 10, 2, " ");
    SetTag("$PB.WegsteinDelay");
    L++;
    SetSpace(L,2); SetDParm(L, "WegsteinBound", 16, "", Id_TDfWegstein_Bound, 10, 2, " ");
    SetTag("$PB.WegsteinBound");
    L++;
    SetSpace(L,2); SetDParm(L, "WegsteinClamp", 16, "", Id_TDfWegstein_Clamp, 10, 2, " ");
    SetTag("$PB.WegsteinClamp");
    L++;
    SetSpace(L,2); 
    L++;
    SetSpace(L,1); SetDesc (L, "Tolerance Defaults:",        -1,  24, 0, " ");
    L++;
    SetSpace(L,2); SetDParm(L, "Rel",  16, "", Id_TDfRel, 10, 2, " ");
    SetTag("$PB.EPS_Rel"); SetDesc(L, TCnv.Text(), -1, 5, 0, " ");
    L++;
    SetSpace(L,2); SetDParm(L, "Abs",  16, "", Id_TDfAbs, 10, 2, " ");
    SetTag("$PB.EPS_Abs"); SetDesc(L, TCnv.Text(), -1, 5, 0, " ");
    L++;
    SetSpace(L,2);  
    L++;
    }

  CRect CRect;
  pView->GetClientRect(&CRect);
  const int LinesVis=CRect.Height()/pView->ScrGB.RowHgt();
  const long MaxPgLen=Max(20, LinesVis-5);
  const int NN=m_Blks.GetSize();
  if (NN>MaxPgLen-18)
    {//force table onto next page
    StartPage("Tears...");
    }
  for (i=0; i<NN; i++)
    {
    if (i==0)
      {
      StartBlk(2+NN, 2, NULL); //tears summary table data
      L=0;//L++;
      SetSpace(L,6);
      SetDesc(L, "",          -1,  TagWide, 0, " ");
      SetDesc(L, "",          -1,       10, 0, "  ");
      SetDesc(L, "",          -1,        4, 0, " ");
      SetDesc(L, "",          -1,        4, 0, " ");
      SetDesc(L, "Damping",   -1,       10, 0, "  ");
      SetDesc(L, "",          -1,        3, 0, "  ");
      SetDesc(L, "Tolerance", -1,       10, 0, "  ");
      SetDesc(L, "",          -1,       10, 0, "  ");
      SetDesc(L, "",          -1,       10, 0, "  ");
      L++;
      SetSpace(L,6);
      SetDesc(L, "Tear Tag",  -1,  TagWide, 0, " ");
      SetDesc(L, "Method",    -1,       10, 0, "  ");
      SetDesc(L, "NVar",      -1,        4, 0, " ");
      SetDesc(L, "Cnvd",      -1,        4, 0, " ");
      SetDesc(L, "Rqd",       -1,        3, 0, " ");  SetDesc(L, DCnv.Text(), -1,       6, 2, "  ");
      SetDesc(L, "Grp",       -1,        3, 0, "  ");
      SetDesc(L, "Strategy",  -1,       10, 0, "  ");
      SetDesc(L, "Abs",       -1,        3, 0, " ");  SetDesc(L, TCnv.Text(), -1,       6, 2, "  ");
      SetDesc(L, "Rel",       -1,        3, 0, " ");  SetDesc(L, TCnv.Text(), -1,       6, 2, "  ");
      L++;
      }

    TearVarBlk*p=m_Blks[i];
    bool Editable = true;//((p->GetFlags()&TBF_Fixed)==0);

    SetBitmapButton1(L, (m_iSlct==i)?IDB_TEARSLCT_BTNDOWN:IDB_TEARSLCT_BTN, Id_TearBlkFldBtns, 2, 0, "");
    SetBitmapButton1(L, IDB_TEARFIND_LOWRESBTN, Id_TearBlkFind1Btn, 2, 0, "");
    if (p->iTearTagTyp==TTT_Tear)
      SetBitmapButton1(L, IDB_TEARFIND_LOWRESBTN, Id_TearBlkFind2Btn, 2, 0, "");
    else
      SetSpace(L, 2);

    SetDesc(L, (char*)p->Tag(), -1, TagWide, 0, " ");
    
    SetParm(L, "", Id_TearBlkMethod, 10, 0, "  ");
    for (int itr=0; DDBTearMethod[itr].m_pStr; itr++)
      FldHasFixedStrValue(DDBTearMethod[itr].m_lVal, DDBTearMethod[itr].m_pStr);
    Tag.Set("%s.TearMethod", (char*)p->Tag()); 
    SetTag(Tag());
    SetParm(L, "", Id_TearBlkNVars,      4, 2, " ", false);
    Tag.Set("%s.NVars", (char*)p->Tag()); 
    SetTag(Tag());
    SetParm(L, "", Id_TearBlkCnvCnt,     4, 2, " ", false);
    Tag.Set("%s.ConvergedCnt", (char*)p->Tag());
    SetTag(Tag());
    SetParm(L, "", Id_TearBlkDampReqd,  10, 2, "  ");
    Tag.Set("%s.DampingRqd", (char*)p->Tag()); 
    SetTag(Tag(), DCnv.Text());
    SetParm(L, "", Id_TearBlkGroup,      3, 2, "  ");
    FldHasFixedStrValue(0, "-");
    FldHasFixedStrValue(1, "Yes");
    Tag.Set("%s.DampAsGroup", (char*)p->Tag()); 
    SetTag(Tag());
    SetParm(L, "", Id_TearBlkStrategy,  10, 0, "  ");
    for (itr=0; DDBEPSStrategy[itr].m_pStr; itr++)
      FldHasFixedStrValue(DDBEPSStrategy[itr].m_lVal, DDBEPSStrategy[itr].m_pStr);
    Tag.Set("%s.EPS_Strategy", (char*)p->Tag()); 
    SetTag(Tag());
    SetParm(L, "", Id_TearBlkAbs,       10, 2, "  ");
    Tag.Set("%s.EPS_Abs", (char*)p->Tag()); 
    SetTag(Tag(), TCnv.Text());
    SetParm(L, "", Id_TearBlkRel,       10, 2, "  ");
    Tag.Set("%s.EPS_Rel", (char*)p->Tag()); 
    SetTag(Tag(), TCnv.Text());
    L++;
    }

  m_iSlctPg1=pView->Pages;  
  if (m_pSlctTVB==NULL)
    return;

  
  Strng GrpTag;
  m_pSlctTVB->GetGroupGlblTag(GrpTag);

  TearVarBlk * pGroupInfo=TearVarBlk::Find(GrpTag());

  TearVarArray &TV = m_pSlctTVB->TV;

  const int N=m_pSlctTVB->NVariables();
  int iNameWidth=Max(8, (int)strlen(m_pSlctTVB->Tag()));
  for (i=0; i<N; i++)
    iNameWidth=Max(iNameWidth, (int)(strlen(GetDisplayTag(TV[i].Tag, TV[i].Sym))+1));

  flag TearAdvRqd = m_pSlctTVB->bUsedByBlk & TVU_AdvanceVars; // Page 0
  flag TearTstRqd = m_pSlctTVB->bUsedByBlk & TVU_TestVars; // Page 0

  if (TearAdvRqd)
    {
    StartPage("Damping");
    StartBlk(N>1 ? 4 : 1, 0, NULL); //damping page header
    int L=0;
    SetSpace(L, 1);
    L++;
    if (N>1)
      {
      Strng sTM;
      if (m_pSlctTVB->m_iTearMethod==TCM_Default)
        sTM.Set("  (%s)",  TearMethodStrings[EqnCB().Cfg.iConvergeMeth]);
      SetDParm(L, "Tear Method",    15, "", Id_TearDmpTearMeth,   10, 2, sTM());//" ");
      for (i=0; TearMethodStrings[i]!=NULL; i++)
        FldHasFixedStrValue(i, TearMethodStrings[i]);
      Tag.Set("%s.TearMethod", m_pSlctTVB->FullObjTag()); 
      SetTag(Tag());

      L++;
      SetDParm(L, "Damp As Group", 15, "", Id_TearDmpDampAsGroup, 10, 2, "");
      FldHasFixedStrValue(0, "No");
      FldHasFixedStrValue(1, "Yes");
      Tag.Set("%s.DampAsGroup", m_pSlctTVB->FullObjTag()); 
      SetTag(Tag());

      L++;
      SetDParm(L, "Block Damp",    15, "", Id_TearDmpBlkDamp,     10, 2, " ");
      Tag.Set("%s.DampingRqd", m_pSlctTVB->FullObjTag()); 
      SetTag(Tag(), DCnv.Text());

      SetDesc(L, DCnv.Text(),  -1, 8, 0, "");
      L++;
      }

    StartBlk(N, 3, NULL); //damping page data
    L=0;
    SetSpace(L,2); 
    L++;
    SetDesc(L, "Variable",  -1, iNameWidth, 0, "");
    SetDesc(L, "Global",    -1,    8, 2, "");
    SetDesc(L, "Local",     -1,    8, 2, "");
    SetDesc(L, "Current",   -1,    8, 2, " ");
    SetDesc(L, "Value",     -1,   10, 2, "");
    L++;

    SetDesc(L, m_pSlctTVB->Tag(),  -1, iNameWidth, 0, "");
    SetDesc(L, DCnv.Text(),  -1, 8,  2, "");
    SetDesc(L, DCnv.Text(),  -1, 8,  2, "");
    SetDesc(L, DCnv.Text(),  -1, 8,  2, " ");
    L++;

    for (i=0; i<N; i++)
      {
      SetDesc(L, GetDisplayTag(TV[i].Tag, TV[i].Sym),    -1, iNameWidth, 0, "");
      SetParm(L, "", Id_TearDmpGlblDamp,  8, 2, "");
      if (pGroupInfo)
        {
        Tag.Set("%s.V.[%s].%s", pGroupInfo->FullObjTag(), pGroupInfo->SymOrTag(i)(), "DampingRqd"); 
        SetTag(Tag());
        }
      
      SetParm(L, "", Id_TearDmpRqdDamp,   8, 2, "");
      Tag.Set("%s.V.[%s].%s", m_pSlctTVB->FullObjTag(), m_pSlctTVB->SymOrTag(i)(), "DampingRqd"); 
      SetTag(Tag(), DCnv.Text());

      SetParm(L, "", Id_TearDmpCurDamp,   8, 2, " ");
      Tag.Set("%s.V.[%s].%s", m_pSlctTVB->FullObjTag(), m_pSlctTVB->SymOrTag(i)(), "DampFactor"); 
      SetTag(Tag(), DCnv.Text());

      SetParm(L, "", Id_TearDmpInputs ,   10, 2, " ");
      Tag.Set("%s.V.[%s].%s", m_pSlctTVB->FullObjTag(), m_pSlctTVB->SymOrTag(i)(), "Y0"); 
      SetTag(Tag());

      SetDesc(L, TV[i].AdvCnv->Text(),  -1, 8,  0, " ");
      L++;
      }
    }
  
  if (TearTstRqd) // Page 1
    {
    StartPage("Tolerance");
    int DoBlk=True;
    int L=0;

    StartBlk(N, 3, NULL); //tolerance page data
    L=0;
//    SetSpace(L,2); 
    SetDesc(L, m_pSlctTVB->Tag(),  -1, 50, 0, "");
    L++;
    SetDesc(L, "Variable",  -1, iNameWidth, 0, "");
    SetDesc(L, "Global",    -1,     8, 2, "");
    SetDesc(L, "Local",     -1,     8, 2, "");
    SetDesc(L, "Current",   -1,     8, 2, " ");
    SetDesc(L, "Error",     -1,    10, 2, " ");
    SetDesc(L, "Cnt",       -1,     3, 0, " ");
    SetDesc(L, "Value",     -1,    10, 2, "");
    L++;

    SetDesc(L, m_pSlctTVB->Tag(),  -1, iNameWidth, 0, "");
    SetDesc(L, TCnv.Text(),  -1,  8,  2, "");
    SetDesc(L, TCnv.Text(),  -1,  8,  2, "");
    SetDesc(L, TCnv.Text(),  -1,  8,  2, " ");
//    SetDesc(L, DiffCnv.Text(), -1, 10,  2, " ");
    L++;

    for (i=0; i<N; i++)
      {
      SetDesc(L, GetDisplayTag(TV[i].Tag, TV[i].Sym),    -1, iNameWidth, 0, "");
      SetParm(L, "", Id_TearTolGlblEPSRel,  8, 2, "");
      if (pGroupInfo)
        {
        Tag.Set("%s.V.[%s].%s", pGroupInfo->FullObjTag(), pGroupInfo->SymOrTag(i)(), "EPS_Rel"); 
        SetTag(Tag());
        }

      SetParm(L, "", Id_TearTolEPSRel,   8, 2, "");
      Tag.Set("%s.V.[%s].%s", m_pSlctTVB->FullObjTag(), m_pSlctTVB->SymOrTag(i)(), "EPS_Rel"); 
      SetTag(Tag());

      SetParm(L, "", Id_TearTolCurTol,   8, 2, " ");
      Tag.Set("%s.V.[%s].%s", m_pSlctTVB->FullObjTag(), m_pSlctTVB->SymOrTag(i)(), "CurTol"); 
      SetTag(Tag());

      SetParm(L, "", Id_TearTolDiff,    10, 2, " ");
      Tag.Set("%s.V.[%s].%s", m_pSlctTVB->FullObjTag(), m_pSlctTVB->SymOrTag(i)(), "Error"); 
      SetTag(Tag());

      SetParm(L, "", Id_TearTolConvergedCnt,    3, 0, " ");
      Tag.Set("%s.V.[%s].%s", m_pSlctTVB->FullObjTag(), m_pSlctTVB->SymOrTag(i)(), "ConvergedCnt"); 
      SetTag(Tag());

      SetParm(L, "", Id_TearTolMeas,    10, 2, " ");
      Tag.Set("%s.V.[%s].%s", m_pSlctTVB->FullObjTag(), m_pSlctTVB->SymOrTag(i)(), "Meas"); 
      SetTag(Tag());

      SetDesc(L, TV[i].MeasCnv->Text(),  -1, 8,  0, "");
      L++;
      }
    }
  else
    {
    }
  }

//---------------------------------------------------------------------------

void TearObjectEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (EI.PageNo<m_iSlctPg1)
    {
    if (EI.PageNo==0 && CurrentBlk(EI))
      { //global header
      switch (EI.FieldId)
        {
        case Id_TDfTearMethod:
          Str=TearConvergeMethodStr(EqnCB().Cfg.iConvergeMeth);
          break;
        case Id_TDfCnvgdIters:
          Str.Set("%i", EqnCB().Cfg.iRqdCnvrgdIters);
          break;
        case Id_TDfMaxIters:
          Str.Set("%i", EqnCB().Cfg.iMaxIters);
          break;
        case Id_TDfDamping:
          DFmt.FormatFloat(DCnv.Human(EqnCB().Cfg.dDamping), Str);
          break;
        case Id_TDfDamping_Growth:
          DFmt.FormatFloat(EqnCB().Cfg.dDampFctGrowth, Str);
          break;
        case Id_TDfDamping_Decay:
          DFmt.FormatFloat(EqnCB().Cfg.dDampFctDecay, Str);
          break;
        case Id_TDfWegstein_Delay:
          Str.Set("%i", EqnCB().Cfg.iWA_Delay);
          break;
        case Id_TDfWegstein_Bound:
          Str.Set("%.2f", EqnCB().Cfg.dWA_Bound);
          break;
        case Id_TDfWegstein_Clamp:
          Str.Set("%.2f", EqnCB().Cfg.dWA_Clamping);
          break;
        case Id_TDfRel:
          TFmt.FormatFloat(TCnv.Human(EqnCB().Cfg.dEps_Rel), Str);
          break;
        case Id_TDfAbs:
          TFmt.FormatFloat(TCnv.Human(EqnCB().Cfg.dEps_Abs), Str);
          break;
        }
      }

    if (CurrentBlk(EI))
      { //tears summary table data
      int pg=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_TearBlkFldBtns:
          Str=(i==m_iSlct)?"1":"0";
          break;
        case Id_TearBlkMethod:
          Str=m_Blks[i]->ConvergeMethodStr();
          break;
        case Id_TearBlkNVars:
          Str.Set("%i", m_Blks[i]->NVariables());
          break;
        case Id_TearBlkCnvCnt:
          Str.Set("%i", m_Blks[i]->SeqConvergedCnt());
          break;
        case Id_TearBlkDampReqd:
          DFmt.FormatFloat(DCnv.Human(m_Blks[i]->DampingRqdCfg()), Str);
          break;
        case Id_TearBlkGroup:
          Str=m_Blks[i]->DampAsGroupCfg() ? "Yes":"-";
          break;
        case Id_TearBlkStrategy:
          Str=m_Blks[i]->EPSStrategyStr();
          break;
        case Id_TearBlkAbs:
          TFmt.FormatFloat(TCnv.Human(m_Blks[i]->EPS_AbsCfg()), Str);
          break;
        case Id_TearBlkRel:
          TFmt.FormatFloat(TCnv.Human(m_Blks[i]->EPS_RelCfg()), Str);
          break;
        }
      }
    }
  else if (m_pSlctTVB && EI.PageNo-m_iSlctPg1==0)
    {
    if (CurrentBlk(EI))
      {//damping page header
      const flag IsAdaptSubs = (m_pSlctTVB->m_iTearMethod==TCM_AdaptSubs || (m_pSlctTVB->m_iTearMethod==TCM_Default && EqnCB().Cfg.iConvergeMeth==TCM_AdaptSubs));
      switch (EI.FieldId)
        {
        case Id_TearDmpTearMeth:
          Str=TearMethodStrings[m_pSlctTVB->m_iTearMethod];
          break;
        case Id_TearDmpDampAsGroup:
          Str=m_pSlctTVB->fDampAsGroup ? "Yes" : "No";
          EI.Fld->fEditable=(IsAdaptSubs);
          break;
        case Id_TearDmpBlkDamp:
          DFmt.FormatFloat(DCnv.Human(m_pSlctTVB->m_dDampingRqd), Str);
          EI.Fld->fEditable=(IsAdaptSubs);
          break;
        }
      }

    if (CurrentBlk(EI))
      {//damping page data
      const long i=(EI.BlkRowNo-EI.Index);
      TearVar &TVi = m_pSlctTVB->TV[i];
      switch (EI.FieldId)
        {
        case Id_TearDmpGlblDamp:
          {
          Strng GrpTag;
          m_pSlctTVB->GetGroupGlblTag(GrpTag);
          TearVarBlk * pGroupInfo=TearVarBlk::Find(GrpTag());
          if (pGroupInfo)
            DFmt.FormatFloat(DCnv.Human(pGroupInfo->TV[i].m_DampingRqd), Str);
          else
            Str="?";
          EI.Fld->fEditable=(m_pSlctTVB->m_iTearMethod==TCM_AdaptSubs || EqnCB().Cfg.iConvergeMeth==TCM_AdaptSubs);
          }
          break;
        case Id_TearDmpRqdDamp:
          DFmt.FormatFloat(DCnv.Human(TVi.m_DampingRqd), Str);
          EI.Fld->fEditable=(m_pSlctTVB->m_iTearMethod==TCM_AdaptSubs || (m_pSlctTVB->m_iTearMethod==TCM_Default && EqnCB().Cfg.iConvergeMeth==TCM_AdaptSubs));
          break;
        case Id_TearDmpCurDamp:
          DFmt.FormatFloat(DCnv.Human(TVi.m_DampFactor), Str);
          EI.Fld->fEditable=False;
          break;
        case Id_TearDmpInputs:
          TVi.AdvFmt->FormatFloat(TVi.AdvCnv->Human(TVi.Y[0]), Str);
          EI.Fld->fEditable=False;
          break;
        }
      }
    }
  else if (m_pSlctTVB && EI.PageNo-m_iSlctPg1==1)
    {
    if (CurrentBlk(EI))
      {//tolerance page data
      const long i=(EI.BlkRowNo-EI.Index);
      TearVar &TVi = m_pSlctTVB->TV[i];
      switch (EI.FieldId)
        {
        case Id_TearTolGlblEPSRel:
          {
          Strng GrpTag;
          m_pSlctTVB->GetGroupGlblTag(GrpTag);
          TearVarBlk * pGroupInfo=TearVarBlk::Find(GrpTag());
          if (pGroupInfo)
            DFmt.FormatFloat(DCnv.Human(pGroupInfo->TV[i].EPS_Rel), Str);
          else
            Str="?";
          }
          break;
        case Id_TearTolEPSRel:
          TFmt.FormatFloat(TCnv.Human(TVi.EPS_Rel), Str);
          break;
        case Id_TearTolCurTol:
          TFmt.FormatFloat(TCnv.Human(TVi.CurTol), Str);
          EI.Fld->fEditable=False;
          break;
        case Id_TearTolDiff:
          //TVi.MeasFmt->FormatFloat(TVi.MeasCnv->Human(TVi.m_Error[0]), Str);
          TFmt.FormatFloat(TCnv.Human(TVi.m_Error[0]), Str);
          EI.Fld->fEditable=False;
          break;
        case Id_TearTolConvergedCnt:
          Str.Set("%3d", TVi.m_iConvergedCnt);
          EI.Fld->fEditable=False;
          break;
        case Id_TearTolMeas:
          TVi.MeasFmt->FormatFloat(TVi.MeasCnv->Human(TVi.m_Meas[0]), Str);
          EI.Fld->fEditable=False;
          break;
        }
      }
    }

/*old tear vars, may want to add back later !?!??
        case Id_TearBlkEPSStrat:
          Str=EPSStrategyStrings[m_pSlctTVB->m_iEPSStrategy];
          break;
        case Id_TearBlkEPSRel:
          TFmt.FormatFloat(TCnv.Human(m_pSlctTVB->dEPS_Rel), Str);
          break;
        case Id_TearBlkEPSAbs:
          TFmt.FormatFloat(TCnv.Human(m_pSlctTVB->dEPS_Abs), Str);
          break;

        case Id_TearEPSAbs:
          TFmt.FormatFloat(TCnv.Human(TVi.EPS_Abs), Str);
          break;
        case Id_TearGlblEPSAbs:
          {
          Strng GrpTag;
          m_pSlctTVB->GetGroupGlblTag(GrpTag);
          TearVarBlk * pGroupInfo=TearVarBlk::Find(GrpTag());
          if (pGroupInfo)
            DFmt.FormatFloat(DCnv.Human(pGroupInfo->TV[i].EPS_Abs), Str);
          else
            Str="?";
          }
          break;
        case Id_TearOutputs:
          TVi.AdvFmt->FormatFloat(TVi.AdvCnv->Human(TVi.X[0]), Str);
          EI.Fld->fEditable=False;
          break;*/

  }

//---------------------------------------------------------------------------

long TearObjectEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; 
  if (EI.PageNo<m_iSlctPg1)
    {
    if (EI.PageNo==0 && CurrentBlk(EI))
      { //global header
      int p=EI.PageNo;
      int i=(int)(EI.BlkRowNo+EI.Index);
      switch (EI.FieldId)
        {
        case Id_TDfTearMethod:
          {
          int j=FindTearConvergeMethod(Str());
          if (j>=0)
            EqnCB().Cfg.iConvergeMeth=(byte)j;
          break;
          }
        case Id_TDfCnvgdIters:
          EqnCB().Cfg.iRqdCnvrgdIters=(short)Range(1L, SafeAtoL(Str(), EqnCB().Cfg.iRqdCnvrgdIters), 9999L);
          break;
        case Id_TDfMaxIters:
          EqnCB().Cfg.iMaxIters=Range(3L, SafeAtoL(Str(), EqnCB().Cfg.iMaxIters), 20000L);
          break;
        case Id_TDfDamping:
          EqnCB().Cfg.dDamping=DCnv.Normal(SafeAtoF(Str(), EqnCB().Cfg.dDamping));
          break;
        case Id_TDfDamping_Growth:
          EqnCB().Cfg.dDampFctGrowth=SafeAtoF(Str(), EqnCB().Cfg.dDampFctGrowth);
          break;
        case Id_TDfDamping_Decay:
          EqnCB().Cfg.dDampFctDecay=SafeAtoF(Str(), EqnCB().Cfg.dDampFctDecay);
          break;
        case Id_TDfWegstein_Delay:
          EqnCB().Cfg.iWA_Delay=(short)Max(0L, SafeAtoL(Str(), EqnCB().Cfg.iWA_Delay));
          break;
        case Id_TDfWegstein_Bound:
          EqnCB().Cfg.dWA_Bound=SafeAtoF(Str(), EqnCB().Cfg.dWA_Bound);
          break;
        case Id_TDfWegstein_Clamp:
          EqnCB().Cfg.dWA_Bound=SafeAtoF(Str(), EqnCB().Cfg.dWA_Bound);
          break;
        case Id_TDfRel:
          EqnCB().Cfg.dEps_Rel=TCnv.Normal(SafeAtoF(Str(), EqnCB().Cfg.dEps_Rel));
          break;
        case Id_TDfAbs:
          EqnCB().Cfg.dEps_Abs=TCnv.Normal(SafeAtoF(Str(), EqnCB().Cfg.dEps_Abs));
          break;
        }
      }
  
    if (CurrentBlk(EI))
      { //tears summary table data
      int pg=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_TearBlkMethod:
          m_Blks[i]->SetConvergeMethod(Str());
          break;
        case Id_TearBlkNVars:
          break;
        case Id_TearBlkCnvCnt:
          break;
        case Id_TearBlkDampReqd:
          m_Blks[i]->SetDampingRqdCfg(DCnv.Normal(SafeAtoF(Str, m_Blks[i]->DampingRqdCfg())));
          break;
        case Id_TearBlkGroup:
          m_Blks[i]->SetDampAsGroupCfg(Str.XStrICmp("Yes")==0);
          break;
        case Id_TearBlkStrategy:
          m_Blks[i]->SetEPSStrategy(Str());
          break;
        case Id_TearBlkAbs:
          m_Blks[i]->SetEPS_AbsCfg(TCnv.Normal(SafeAtoF(Str(), m_Blks[i]->EPS_AbsCfg())));
          break;
        case Id_TearBlkRel:
          m_Blks[i]->SetEPS_RelCfg(TCnv.Normal(SafeAtoF(Str(), m_Blks[i]->EPS_RelCfg())));
          break;
        }
      }
    }
  else if (m_pSlctTVB && EI.PageNo-m_iSlctPg1==0)
    {
    if (CurrentBlk(EI))
      {//damping page header
      switch (EI.FieldId)
        {
        case Id_TearDmpTearMeth:
          m_pSlctTVB->SetConvergeMethod(Str());
          break;
        case Id_TearDmpDampAsGroup:
          m_pSlctTVB->SetDampAsGroupCfg(Str.XStrICmp("Yes")==0);
          break;
        case Id_TearDmpBlkDamp:
          m_pSlctTVB->SetDampingRqdCfg(DCnv.Normal(SafeAtoF(Str, m_pSlctTVB->DampingRqdCfg())));
          break;
        }
      }

    if (CurrentBlk(EI))
      {//damping page data
      const long i=(EI.BlkRowNo-EI.Index);
      TearVar &TVi = m_pSlctTVB->TV[i];
      Strng GrpTag;
      m_pSlctTVB->GetGroupGlblTag(GrpTag);
      TearVarBlk * pGroupInfo=TearVarBlk::Find(GrpTag());
      switch (EI.FieldId)
        {
        case Id_TearDmpGlblDamp:
          if (pGroupInfo)
            pGroupInfo->TV[i].m_DampingRqd=DCnv.Normal(SafeAtoF(Str));
          break;
        case Id_TearDmpRqdDamp:
          TVi.m_DampingRqd=DCnv.Normal(SafeAtoF(Str, TVi.m_DampingRqd));
          break;
        case Id_TearDmpCurDamp:
          TVi.m_DampFactor=DCnv.Normal(SafeAtoF(Str, TVi.m_DampFactor));
          break;
        /*case Id_TearDmpInputs:
          break;*/
        }
      }
    }
  else if (m_pSlctTVB && EI.PageNo-m_iSlctPg1==1)
    {
    if (CurrentBlk(EI))
      {//tolerance page data
      const long i=(EI.BlkRowNo-EI.Index);
      TearVar &TVi = m_pSlctTVB->TV[i];
      Strng GrpTag;
      m_pSlctTVB->GetGroupGlblTag(GrpTag);
      TearVarBlk * pGroupInfo=TearVarBlk::Find(GrpTag());
      switch (EI.FieldId)
        {
        case Id_TearTolGlblEPSRel:
          if (pGroupInfo)
            pGroupInfo->TV[i].EPS_Rel=TCnv.Normal(SafeAtoF(Str));
          break;
        case Id_TearTolEPSRel:
          TVi.EPS_Rel=TCnv.Normal(SafeAtoF(Str, TVi.EPS_Rel));
          break;
/*        case Id_TearTolCurTol:
        case Id_TearTolDiff:
        case Id_TearTolConvergedCnt:
        case Id_TearTolMeas:*/
        }
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

long TearObjectEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  if (EI.PageNo<m_iSlctPg1)
    {
    if (EI.PageNo==0 && CurrentBlk(EI))
      { //global header
      }

    if (CurrentBlk(EI))
      { //tears summary table data
      int pg=EI.PageNo;
      int i=(int)(EI.BlkRowNo-EI.Index);
      switch (EI.FieldId)
        {
        case Id_TearBlkFldBtns:
          Str.LTrim();
          //if (Str() && (Str[0]=='1'))
          m_iSlct = i;
          View().DoRebuild();
          break;
        case Id_TearBlkFind1Btn:
        case Id_TearBlkFind2Btn:
          {
          Strng s;
          if (EI.FieldId==Id_TearBlkFind1Btn)
            m_Blks[i]->LHSTag(s);
          else
            m_Blks[i]->RHSTag(s);
          if (s.Len())
            {
            char* pTxt=new char[s.Len()+1];
            strcpy(pTxt, s());
            ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAGACTION_FIND, (LPARAM)pTxt);
            }
          break;
          }
        }
      }
    }
  else if (m_pSlctTVB)
    {
    }
  return 0;
  }

//---------------------------------------------------------------------------

flag TearObjectEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TearObjectEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TearObjectEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TearObjectEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TearObjectEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView &Vw=View();
  flag ret=false;
  if (Vw.CPgNo>=0)
    {
    flag NulTg=true;
    flag TagOnly=false;
    if (Vw.CPgNo<m_iSlctPg1)
      {
      FxdEdtInfo EI;
      if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
        {
        ret=true;
        if (EI.Fld)
          {
          switch (EI.FieldId)
            {
            case Id_TDfTearMethod        :
            case Id_TDfCnvgdIters        :
            case Id_TDfMaxIters          :
            case Id_TDfDamping_Growth    :
            case Id_TDfDamping_Decay     :
            case Id_TDfWegstein_Delay    :
            case Id_TDfWegstein_Bound    :
            case Id_TDfWegstein_Clamp    :
              WrkIB.Set(EI.Fld->Tag, NULL, NULL);
              TagOnly = true;
              break;
            case Id_TDfDamping           :
              WrkIB.Set(EI.Fld->Tag, &DCnv, &DFmt);
              break;
            case Id_TDfRel               :
            case Id_TDfAbs               :
              WrkIB.Set(EI.Fld->Tag, &TCnv, &TFmt);
              break;

            case Id_TearBlkMethod        :
            case Id_TearBlkNVars         :
            case Id_TearBlkCnvCnt        :
            case Id_TearBlkGroup         :
            case Id_TearBlkStrategy      :
            //case Id_TearBlkFldBtns       :
              WrkIB.Set(EI.Fld->Tag, NULL, NULL);
              TagOnly = true;
              break;
            case Id_TearBlkDampReqd          :
              WrkIB.Set(EI.Fld->Tag, &DCnv, &DFmt);
              break;
            case Id_TearBlkAbs           :
            case Id_TearBlkRel           :
              WrkIB.Set(EI.Fld->Tag, &TCnv, &TFmt);
              break;

  //          case Id_TearMethod:
  //            {
  //            int i=(int)(EI.BlkRowNo+EI.Index);
  //            TearVarBlk*p=m_Blks[i];
  //            WrkIB.Set(EI.Fld->Tag, &p->GetAbsCnv(), &TFmt);
  //            break;
  //            }
  //          case Id_TearCnvCnt:
  //            WrkIB.Set(EI.Fld->Tag, &TCnv, &TFmt);
  //            break;
            }
          }
        NulTg = (EI.Fld->Tag==NULL);
        }
      }
    else if (m_pSlctTVB)
      {
      FxdEdtInfo EI;
      if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
        {
        ret=true;
        if (EI.Fld)
          {
          switch (EI.FieldId)
            {
            case Id_TearDmpTearMeth      :
            case Id_TearDmpDampAsGroup   :
            case Id_TearDmpInputs        :
              WrkIB.Set(EI.Fld->Tag, NULL, NULL);
              TagOnly = true;
              break;
            case Id_TearDmpBlkDamp       :
            case Id_TearDmpGlblDamp      :
            case Id_TearDmpRqdDamp       :
            case Id_TearDmpCurDamp       :
              WrkIB.Set(EI.Fld->Tag, &DCnv, &DFmt);
              break;
            
            case Id_TearTolGlblEPSRel    :
              WrkIB.Set(EI.Fld->Tag, &DCnv, &DFmt);
              break;
            case Id_TearTolEPSRel        :
            case Id_TearTolCurTol        :
            case Id_TearTolDiff          :
              WrkIB.Set(EI.Fld->Tag, &TCnv, &TFmt);
              break;
            case Id_TearTolConvergedCnt  :
              WrkIB.Set(EI.Fld->Tag, NULL, NULL);
              TagOnly = true;
              break;
            case Id_TearTolMeas          :
              {
              const long i=(EI.BlkRowNo-EI.Index);
              TearVar &TVi = m_pSlctTVB->TV[i];
              WrkIB.Set(EI.Fld->Tag, TVi.MeasCnv, TVi.MeasFmt);
              break;
              }
            }
          }
        NulTg = (EI.Fld->Tag==NULL);
        }
      }

    if (ret && (WrkIB.FmtOK() || TagOnly))
      {
      CRect WRect;
      Vw.GetWindowRect(&WRect);
      CPoint  RBPoint;
      RBPoint.x = WRect.left+point.x;
      RBPoint.y = WRect.top+point.y;

      CMenu Menu;
      Menu.CreatePopupMenu();
      CMenu FormatMenu;
      FormatMenu.CreatePopupMenu();
      if (WrkIB.FmtOK())
        WrkIB.Fmt().AddToMenu(FormatMenu);
      CMenu CnvMenu;
      CnvMenu.CreatePopupMenu();
      if (WrkIB.CnvOK())
        WrkIB.Cnv().AddToMenu(CnvMenu);

      Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
      if ((WrkIB.CnvOK() && (WrkIB.Cnv().Index())<=0) || TagOnly)
        Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);

      Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");
      if (TagOnly)
        Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);
    
      Menu.AppendMenu(MF_SEPARATOR);
      Menu.AppendMenu(MF_STRING, pView->iCmdCopyTag, "Copy Full &Tag");
      Menu.AppendMenu(MF_STRING, pView->iCmdCopyRHSTag, "Copy Tag");
      if (NulTg)
        {
        Menu.EnableMenuItem(pView->iCmdCopyTag, MF_BYCOMMAND|MF_GRAYED);
        Menu.EnableMenuItem(pView->iCmdCopyRHSTag, MF_BYCOMMAND|MF_GRAYED);
        }
      Menu.AppendMenu(MF_STRING, pView->iCmdCopyVal, "Copy &Value");
    
      Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, &View());
      Menu.DestroyMenu();                                           
      }
    }
  return ret;
  }

//---------------------------------------------------------------------------

flag TearObjectEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TearObjectEdt::DoAccCnv(UINT Id) 
  {
  CDataCnv *pC=Cnvs[(WrkIB.Cnv().Index())];
  for (int i=Id; i>0; i--)
    pC=pC->Next();
    
  if (pC)
    {
    WrkIB.Cnv().SetText(pC->Txt());
    }
  View().DoRebuild();
  return true;
  };

//---------------------------------------------------------------------------

flag TearObjectEdt::DoAccFmt(UINT Id) 
  {
  for (UINT i=0; i<(UINT)DefinedFmts.GetSize(); i++) 
    if (i==Id) 
      break;

  if (i<(UINT)DefinedFmts.GetSize())
    {
    WrkIB.Fmt()=DefinedFmts[i];
    View().DoRebuild();
    }
  return true;
  };

//---------------------------------------------------------------------------

flag TearObjectEdt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  };

//===========================================================================
//
//                                  ChgObject
//
//===========================================================================
#if WITHCHANGEOBJ

CTraceList <CTraceArray*, CTraceArray*> ChgObject::ChgStatsArrayList;
long ChgObject::iTheStats=0;
CArray <CTraceItem*, CTraceItem*> ChgObject::TheStats;
CArray <CTraceItem*, CTraceItem*> ChgObject::SortedErrStats;
CArray <CTraceItem*, CTraceItem*> ChgObject::SortedChgStats;
CArray <CTraceItem*, CTraceItem*> ChgObject::SortedOscStats;

IMPLEMENT_TAGOBJEDT(ChgObject, "FS_Chgs", "FS_Chgs", "", "Changes", TOC_SYSTEM,
                    ChgObjectEdt, "The Changes", "The Changes");

ChgObject::ChgObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CTNode(pClass_, TagIn, pAttach, eAttach)
  {
  bDoSystemDefn=false;

  fAllowCollection=false;
  iTheStats=0;
  fBlocked=false;
  dShowTol=1.0e-5;
  dSmoothing=0.0;
  nNumStats=20;
  }

//--------------------------------------------------------------------------

ChgObject::~ChgObject()
  {
  }

//--------------------------------------------------------------------------

XID xidErrTag    = ModelXID(1000);
XID xidErrVal    = ModelXID(1000)+1*MaxChgStats;

void ChgObject::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.Text  ("");
    DDB.Bool  ("Collect",   "", DC_, "",  &fAllowCollection,  this, isParm);
    DDB.Long  ("Number",    "", DC_, "",  &nNumStats,         this, isParm);
    DDB.Bool  ("Blocked",   "", DC_, "",  &fBlocked,          this, isParm);
    DDB.Double("ShowTol",   "", DC_, "",  &dShowTol,          this, isParm);
    DDB.Double("Smoothing", "", DC_, "",  &dSmoothing,        this, isParm);
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag ChgObject::DataXchg(DataChangeBlk & DCB)
  {
  if (CTNode::DataXchg(DCB))
    return 1;

//  int i=(DCB.lHandle-xidErrTag)%MaxChgStats;
//  int j=i*MaxChgStats+xidErrTag;
//
//  switch (j)
//    {
//    case xidErrTag:
////      if (DCB.rpC)
////        {
////        SortedErrStats[i]->Val(), SortedErrStats[i]->RelErr(), SortedErrStats[i]->Tag());
//        DCB.pC="xxx";//i< // Ignore
//      return 1;
////    }
//    }
  return 0;
  }

//--------------------------------------------------------------------------

flag ChgObject::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=CTNode::ValidateData(VDB);
  return OK;
  }

//--------------------------------------------------------------------------

void ChgObject::StartAddition()
  {
  ChgStatsArrayList.RemoveAllToFreeList();
  if (iTheStats==0)
    TheStats.SetSize(0, 1024);
  if (!fAllowCollection)
    {
    SortedErrStats.SetSize(0, 1024);
    SortedChgStats.SetSize(0, 1024);
    SortedOscStats.SetSize(0, 1024);
    }
  iTheStats=0;
  }

//--------------------------------------------------------------------------

int ChgObject::DoAddition(dword Flags, CTraceArray * pSA)
  {
  int N=0;
  if (pSA)
    {
    int i;

    ChgStatsArrayList.AddTail(pSA);
    CTraceArray & A=*pSA;
    //double 
    if (Flags & TCO_DoCalcs)
      for (i=0; i<A.GetSize(); i++)
        A[i].DoCalculations(dSmoothing);

    int i0=0;
    int i1=A.GetSize();
    if (fBlocked)
      {
//        double WorstE=-1.0;
//        double WorstC=-1.0;
//        double WorstO=-1.0;
//        for (i=0; i<A.GetSize(); i++)
//          {
//          if (Valid(A[i].RelErr()) && A[i].RelErr()>WorstE)
//            {
//            WorstE=A[i].RelErr();
//            SortedErrStats.SetAtGrow(iSortedErrStats, &A[i]);
//            }
//          if (A[i].RelChg()>WorstC)
//            {
//            WorstC=A[i].RelChg();`
//            SortedChgStats.SetAtGrow(iSortedChgStats, &A[i]);
//            }
//          if (A[i].RelChg()>WorstO)
//            {
//            WorstO=A[i].RelOsc();
//            SortedOscStats.SetAtGrow(iSortedOscStats, &A[i]);
//            }
//          }
//        if (WorstE>=0.0)
//          iSortedErrStats++;
//        iSortedChgStats++;
//        iSortedOscStats++;
      N++;
      }
    else
      {
      for (i=i0; i<i1; i++)
        {
        TheStats.SetAtGrow(iTheStats++, &A[i]);
        N++;
//          if (Valid(A[i].RelErr()))
//          SortedErrStats.SetAtGrow(iSortedErrStats++, &A[i]);
//          SortedChgStats.SetAtGrow(iSortedChgStats++, &A[i]);
//          SortedOscStats.SetAtGrow(iSortedOscStats++, &A[i]);
        }
      }

    }
  return N;
  }

//--------------------------------------------------------------------------

int TestErrLT(void * p, void * q) { return fabs(((CTraceItem*)p)->RelErr())<fabs(((CTraceItem*)q)->RelErr()); };
int TestChgLT(void * p, void * q) { return fabs(((CTraceItem*)p)->RelChg())<fabs(((CTraceItem*)q)->RelChg()); };
int TestOscLT(void * p, void * q) { return fabs(((CTraceItem*)p)->RelOsc())<fabs(((CTraceItem*)q)->RelOsc()); };

int TestErrGT(void * p, void * q) { return fabs(((CTraceItem*)p)->RelErr())>fabs(((CTraceItem*)q)->RelErr()); };
int TestChgGT(void * p, void * q) { return fabs(((CTraceItem*)p)->RelChg())>fabs(((CTraceItem*)q)->RelChg()); };
int TestOscGT(void * p, void * q) { return fabs(((CTraceItem*)p)->RelOsc())>fabs(((CTraceItem*)q)->RelOsc()); };

//--------------------------------------------------------------------------

void ChgObject::DoSort()
  {
  if (0)
    {
    int i;
    dbgpln("    %3s %16s  %16s  %16s  %16s  %s", "", "Val", "Err%", "Chg%", "Osc%", "Tag");
    for (i=0; i<iTheStats; i++)
      dbgpln("    %3i %16.8f  %16.8f  %16.8f  %16.8f  %s", 
      i,
      TheStats[i]->Val(), 
      TheStats[i]->RelErr()*100, 
      TheStats[i]->RelChg()*100,   
      TheStats[i]->RelOsc()*100, 
      TheStats[i]->FullTag());
    }
  
  int iSorted=Min(iTheStats, nNumStats); 

  SortedErrStats.SetSize(iSorted, 1024);
  SortedChgStats.SetSize(iSorted, 1024);
  SortedOscStats.SetSize(iSorted, 1024);

  HpSel(iSorted, iTheStats, (void**)&TheStats[0], (void**)&SortedErrStats[0], TestErrLT);
  HpSel(iSorted, iTheStats, (void**)&TheStats[0], (void**)&SortedChgStats[0], TestChgLT);
  HpSel(iSorted, iTheStats, (void**)&TheStats[0], (void**)&SortedOscStats[0], TestOscLT);

  HpSort(iSorted, (void**)&SortedErrStats[0], TestErrGT);
  HpSort(iSorted, (void**)&SortedChgStats[0], TestChgGT);
  HpSort(iSorted, (void**)&SortedOscStats[0], TestOscGT);

  if (1)
    {
    dbgpln("------");
    int i;
    for (i=0; i<iSorted; i++)
      if (fabs(SortedErrStats[i]->RelErr())>1.0e-50)
        dbgpln("Err %3i %12.6e  (%12.6e)  %s", i, SortedErrStats[i]->RelErr(), SortedErrStats[i]->Val(), SortedErrStats[i]->FullTag());
    for (i=0; i<iSorted; i++)
      if (fabs(SortedErrStats[i]->RelChg())>1.0e-50)
        dbgpln("Chg %3i %12.6e  (%12.6e)  %s", i, SortedChgStats[i]->RelChg(), SortedChgStats[i]->Val(), SortedChgStats[i]->FullTag());
    for (i=0; i<iSorted; i++)
      if (fabs(SortedErrStats[i]->RelOsc())>1.0e-50)
        dbgpln("Osc %3i %12.6e  (%12.6e)  %s", i, SortedOscStats[i]->RelOsc(), SortedOscStats[i]->Val(), SortedOscStats[i]->FullTag());
    }

  if (1)
    {
    const int nLogErr=5;
    const int nLogChg=5;
    const int nLogOsc=0;
    int nLogOut=0;
    int nLog;
    int i;
    
    nLog=0;
    for (i=0; i<iSorted; i++)
      if (fabs(SortedErrStats[i]->RelErr())>1.0e-50)
        {
        if (nLog++>=nLogErr)
          break;
        if (nLogOut++==0)
          LogNote("",0,"------------");
        LogNote((LPTSTR)SortedErrStats[i]->FullTag(), 0, "Err %12.4g%%  (%12.4g) ", SortedErrStats[i]->RelErr()*100, SortedErrStats[i]->Val());
        }
    nLog=0;
    for (i=0; i<iSorted; i++)
      if (fabs(SortedErrStats[i]->RelChg())>1.0e-50)
        {
        if (nLog++>=nLogChg)
          break;
        if (nLogOut++==0)
          LogNote("",0,"------------");
        LogNote((LPTSTR)SortedErrStats[i]->FullTag(), 0, "Chg %12.4g%%  (%12.4g) ", SortedChgStats[i]->RelChg()*100, SortedChgStats[i]->Val());
        }
    nLog=0;
    for (i=0; i<iSorted; i++)
      if (fabs(SortedErrStats[i]->RelOsc())>1.0e-50)
        {
        if (nLog++>=nLogOsc)
          break;
        if (nLogOut++==0)
          LogNote("",0,"------------");
        LogNote((LPTSTR)SortedErrStats[i]->FullTag(), 0, "Osc %12.4g%%  (%12.4g) ", SortedOscStats[i]->RelOsc()*100, SortedOscStats[i]->Val());
        }
    }

  }

//===========================================================================
//
//
//
//===========================================================================

const int Id_Block             = 1;
const int Id_Collect           = 2;
const int Id_NumStats          = 3;
const int Id_ShowTol           = 4;
const int Id_Smoothing         = 5;
const int Id_StatErrTag        = 10;
const int Id_StatErrVal        = 11;
const int Id_StatChgTag        = 12;
const int Id_StatChgVal        = 13;
const int Id_StatOscTag        = 14;
const int Id_StatOscVal        = 15;


//------------------------------------------------------------------------------

ChgObjectEdt::ChgObjectEdt(pFxdEdtView pView_, pChgObject pChgO_) :
  FxdEdtBookRef(pView_),
  rChgO(*pChgO_)
  {
  iPg1=0;
  }

//---------------------------------------------------------------------------

ChgObjectEdt::~ChgObjectEdt()
  {
  }

//---------------------------------------------------------------------------

int ChgObjectEdt::DistFromPgNo(int Pg)
  {
  return -1;
  }

//---------------------------------------------------------------------------

void ChgObjectEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void ChgObjectEdt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void ChgObjectEdt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void ChgObjectEdt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void ChgObjectEdt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

void ChgObjectEdt::Build()
  {
  Strng S;
  int L, i;

  iPg1=pView->Pages;

  StartPage("Stats");
  StartBlk(6, 0, NULL);
  L=0;
  SetSpace(L, 8);
  L++;
  SetDesc(L, "Collect",  -1, 12, 0, "");
  SetCheckBoxBtn(L, rChgO.fAllowCollection ? "1" : "0", Id_Collect, 5, 2, " ", true);
  L++;
  SetDParm(L, "NumStats",  12, "", Id_NumStats, 5, 2, " ");
  L++;
  SetDParm(L, "ShowTol",   12, "", Id_ShowTol, 10, 2, " ");
  L++;
  SetDParm(L, "Smoothing", 12, "", Id_Smoothing, 10, 2, " ");


  if (1) // Base Blk
    {
    Strng Tg, D;
    StartPage("Errs");
    StartBlk(rChgO.nNumStats, 1, NULL);
    L=0;
    SetSpace(L, 1);
    L++;
    for (i=0; i<rChgO.nNumStats; i++)
      {
      D.Set("%3i)",i);

      SetDesc(L, D(),  -1, 3,  0, "");
      SetParm(L, "", Id_StatErrTag,  30, 0, "");
      SetParm(L, "", Id_StatErrVal,  10, 2, "");
      L++;
      }

    StartPage("Chngs");
    StartBlk(rChgO.nNumStats, 1, NULL);
    L=0;
    SetSpace(L, 1);
    L++;
    for (i=0; i<rChgO.nNumStats; i++)
      {
      D.Set("%3i)", i);

      SetDesc(L, D(),  -1, 3,  0, "");
      SetParm(L, "", Id_StatChgTag,  30, 0, "");
      //Tg.Set("ChgTag%03i", i);
      //SetTag(Tg()); todo
      SetParm(L, "", Id_StatChgVal,  10, 2, "");
      //Tg.Set("ChgVal%03i", i);
      //SetTag(Tg());
      L++;
      }

    StartPage("Osc");
    StartBlk(rChgO.nNumStats, 1, NULL);
    L=0;
    SetSpace(L, 1);
    L++;
    for (i=0; i<rChgO.nNumStats; i++)
      {
      D.Set("%3i)",i);

      SetDesc(L, D(),  -1, 3,  0, "");
      SetParm(L, "", Id_StatOscTag,  30, 0, "");
      SetParm(L, "", Id_StatOscVal,  10, 2, "");
      L++;
      }
    }
  }

//---------------------------------------------------------------------------

void ChgObjectEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {//data
    int pg=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);

    switch (EI.FieldId)
      {
      case Id_Block:
        Str=rChgO.fBlocked ? "Yes" : "No";
        break;
      case Id_Collect:
        Str=rChgO.fAllowCollection ? "1" : "0";
        break;
      case Id_NumStats:
        Str.Set("%i", rChgO.nNumStats);
        break;
      case Id_ShowTol:
        Str.Set("%8.2e", rChgO.dShowTol);
        break;
      case Id_Smoothing:
        Str.Set("%8.2f", rChgO.dSmoothing);
        break;
      case Id_StatErrTag:  
        if (i<rChgO.SortedErrStats.GetSize() &&
          fabs(rChgO.SortedErrStats[i]->RelErr()) >= rChgO.dShowTol)
          Str=rChgO.SortedErrStats[i]->FullTag();
        else
          Str="";
        break;
      case Id_StatErrVal:  
        if (i<rChgO.SortedErrStats.GetSize() &&
          fabs(rChgO.SortedErrStats[i]->RelErr()) >= rChgO.dShowTol)
          Str.Set("%6.2e", rChgO.SortedErrStats[i]->RelErr());
        else
          Str="";
        break;
      case Id_StatChgTag:  
        if (i<rChgO.SortedChgStats.GetSize() &&
          fabs(rChgO.SortedChgStats[i]->RelChg()) >= rChgO.dShowTol)
          Str=rChgO.SortedChgStats[i]->FullTag();
        else
          Str="";
        break;
      case Id_StatChgVal:  
        if (i<rChgO.SortedChgStats.GetSize() &&
          fabs(rChgO.SortedChgStats[i]->RelChg()) >= rChgO.dShowTol)
          Str.Set("%6.2e", rChgO.SortedChgStats[i]->RelChg());
        else
          Str="";
        break;
      case Id_StatOscTag:  
        if (i<rChgO.SortedOscStats.GetSize() &&
          fabs(rChgO.SortedOscStats[i]->RelOsc()) >= rChgO.dShowTol)
          Str=rChgO.SortedOscStats[i]->FullTag();
        else
          Str="";
        break;
      case Id_StatOscVal:  
        if (i<rChgO.SortedOscStats.GetSize() &&
          fabs(rChgO.SortedOscStats[i]->RelOsc()) >= rChgO.dShowTol)
          Str.Set("%6.2e", rChgO.SortedOscStats[i]->RelOsc());
        else
          Str="";
        break;
      }
    if (EI.Fld)
      EI.Fld->fEditable=(pg==0);
    }
  }

//---------------------------------------------------------------------------

long ChgObjectEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; 
  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_Block:
        Str.LTrim();
        rChgO.fBlocked = Str() && (Str[0]=='Y' || Str[0]=='y');
        View().DoRebuild();
        break;
      case Id_Collect:
        Str.LTrim();
        rChgO.fAllowCollection = Str() && (Str[0]=='1');
        FlwNode::fCollectChgStats = rChgO.fAllowCollection;
        View().DoRebuild();
        break;
      case Id_NumStats:
        rChgO.nNumStats = SafeAtoI(Str(), 10);
        View().DoRebuild();
        break;
      case Id_ShowTol:
        rChgO.dShowTol = Range(0.0, SafeAtoF(Str(), 1.0e-4), 1.0);
        View().DoRebuild();
        break;
      case Id_Smoothing:
        rChgO.dSmoothing = Range(0.0, SafeAtoF(Str(), 0), 1.0);
        View().DoRebuild();
        break;
      }
    }

  return Fix;
  }

//---------------------------------------------------------------------------

long ChgObjectEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph

  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_Collect:
        {
        flag PrevAllowCollection = rChgO.fAllowCollection;
        rChgO.fAllowCollection=!rChgO.fAllowCollection;
        FlwNode::fCollectChgStats = rChgO.fAllowCollection;
        if (PrevAllowCollection)
          {
          //rChgO.SortedErrStats.SetSize(0, 1024);
          //rChgO.SortedChgStats.SetSize(0, 1024);
          //rChgO.SortedOscStats.SetSize(0, 1024);
          }
//        View().DoRebuild();
        }
        break;
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

flag ChgObjectEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag ChgObjectEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag ChgObjectEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag ChgObjectEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag ChgObjectEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView &Vw=View();
  flag ret=false;
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      ret=true;
      if (EI.Fld)
        {
        flag TagOnly=false;

        }
      }
    }
  return ret;
  }

//---------------------------------------------------------------------------

flag ChgObjectEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag ChgObjectEdt::DoAccCnv(UINT Id) 
  {
  View().DoRebuild();
  return true;
  };

//---------------------------------------------------------------------------

flag ChgObjectEdt::DoAccFmt(UINT Id) 
  {
  return true;
  };

//---------------------------------------------------------------------------

flag ChgObjectEdt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  };
#endif

//===========================================================================
//
//                                  TolObject
//
//===========================================================================

IMPLEMENT_TAGOBJEDT(TolObject, "FS_Tols", "FS_Tols", "", "Tolerances", TOC_SYSTEM,
                    TolObjectEdt, "The Tolerances", "The Tolerances");

TolObject::TolObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CTNode(pClass_, TagIn, pAttach, eAttach)
  {
  bDoSystemDefn=false;

  }

//--------------------------------------------------------------------------

TolObject::~TolObject()
  {
  }

//--------------------------------------------------------------------------

XID xidTolAbs       = ModelXID(1000);
XID xidTolAbsCnv    = ModelXID(1001);
XID xidTolRel       = ModelXID(1002);
XID xidTolMaxIt     = ModelXID(1003);
XID xidTolParent    = ModelXID(1004);
XID xidTolAbsMult   = ModelXID(1005);
XID xidTolRelMult   = ModelXID(1006);

void TolObject::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {
    if (DDB.ForFileSnpScn())
      {
      if (DDB.BeginArray(this, "Tolerance", "FS_Tolerance", CToleranceBlock::s_Array.GetSize()))
        {
        for (int i=0; i<CToleranceBlock::s_Array.GetSize(); i++)
          {
          CToleranceBlock *p=CToleranceBlock::s_Array[i];
          if ((p->GetFlags()&TBF_Fixed)==0)
            {
            if (DDB.BeginElement(this, (LPTSTR)p->GetName(), NULL, (dword)p))
              {
              if (p->UseAbs())
                {
                DDB.String("AbsCnv", "", DC_, "", xidTolAbsCnv, this, 0);
                p->GetAbsDD().BuildDataDefn(DDB, "Abs", "", DC_, "", xidTolAbs, this, "Default", "User", DDEF_WRITEPROTECT);
                }
              if (p->UseRel())
                p->GetRelDD().BuildDataDefn(DDB, "Rel", "",   DC_Frac,   "%",  xidTolRel, this, "Default", "User", DDEF_WRITEPROTECT);

              if (p->UseMax())
                p->GetMaxItersDD().BuildDataDefn(DDB, "MaxIt", "",   DC_,   "",  xidTolMaxIt, this, "Default", "User", DDEF_WRITEPROTECT);

              if (p->HasParent())
                DDB.String("Parent",  "",   DC_,   "",  xidTolParent, this, isParm);
          
              if (p->UseAbsParent())
                p->GetAbsMultDD().BuildDataDefn(DDB, "AbsMult", "",   DC_Frac,   "%",  xidTolAbsMult, this, "Default", "User", DDEF_WRITEPROTECT);
              if (p->UseRelParent())
                p->GetRelMultDD().BuildDataDefn(DDB, "RelMult", "",   DC_Frac,   "%",  xidTolRelMult, this, "Default", "User", DDEF_WRITEPROTECT);
              }
            }
          }
        }
      DDB.EndArray();
      }
    else
      {
      Strng Tg;
      for (int i=0; i<CToleranceBlock::s_Array.GetSize(); i++)
        {
        CToleranceBlock *p=CToleranceBlock::s_Array[i];
        DDB.PushUserInfo((dword)p);
        if (p->UseAbs())
          {
          Tg.Set("%s.%s",p->GetName(), "Abs"); 
          p->GetAbsDD().BuildDataDefn(DDB, Tg(), "", p->GetAbsCnv().Index(), p->GetAbsCnv().Text(), xidTolAbs, this, "Default", "User");
          }
        if (p->UseRel())
          {
          Tg.Set("%s.%s",p->GetName(), "Rel"); 
          p->GetRelDD().BuildDataDefn(DDB, Tg(), "",   DC_Frac,   "%",  xidTolRel, this, "Default", "User");
          }
        if (p->UseMax())
          {
          Tg.Set("%s.%s",p->GetName(), "MaxIt"); 
          p->GetMaxItersDD().BuildDataDefn(DDB, Tg(), "",   DC_,   "",  xidTolMaxIt, this, "Default", "User");
          }
        if (p->HasParent())
          {
          Tg.Set("%s.%s",p->GetName(), "Parent"); 
          DDB.String(Tg(),  "",   DC_,   "",  xidTolParent, this, isParm);
          }
        if (p->UseAbsParent())
          {
          Tg.Set("%s.%s",p->GetName(), "AbsMult"); 
          p->GetAbsMultDD().BuildDataDefn(DDB, Tg(), "",   DC_Frac,   "%",  xidTolAbsMult, this, "Default", "User");
          }
        if (p->UseRelParent())
          {
          Tg.Set("%s.%s",p->GetName(), "RelMult"); 
          p->GetRelMultDD().BuildDataDefn(DDB, Tg(), "",   DC_Frac,   "%",  xidTolRelMult, this, "Default", "User");
          }
        DDB.PopUserInfo();
        }
      }
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag TolObject::DataXchg(DataChangeBlk & DCB)
  {
  if (CTNode::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidTolAbsCnv:
      {
      CToleranceBlock *p=(CToleranceBlock *)DCB.dwUserInfo;
      DCB.pC=p->GetAbsCnv().Text();
      return 1;
      }
    case xidTolAbs:
      {
      CToleranceBlock *p=(CToleranceBlock *)DCB.dwUserInfo;
      p->GetAbsDD().DataXchg(DCB,DCB.lHandle , this);
      return 1;
      }
    case xidTolRel:
      {
      CToleranceBlock *p=(CToleranceBlock *)DCB.dwUserInfo;
      p->GetRelDD().DataXchg(DCB,DCB.lHandle , this);
      return 1;
      }
    case xidTolMaxIt:
      {
      CToleranceBlock *p=(CToleranceBlock *)DCB.dwUserInfo;
      p->GetMaxItersDD().DataXchg(DCB,DCB.lHandle , this);
      return 1;
      }
    case xidTolParent:
      {
      CToleranceBlock *p=(CToleranceBlock *)DCB.dwUserInfo;
      DCB.pC=(LPTSTR)p->Parent()->GetName();
      return 1;
      }
    case xidTolAbsMult:
      {
      CToleranceBlock *p=(CToleranceBlock *)DCB.dwUserInfo;
      p->GetAbsMultDD().DataXchg(DCB,DCB.lHandle , this);
      return 1;
      }
    case xidTolRelMult:
      {
      CToleranceBlock *p=(CToleranceBlock *)DCB.dwUserInfo;
      p->GetRelMultDD().DataXchg(DCB,DCB.lHandle , this);
      return 1;
      }
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag TolObject::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=CTNode::ValidateData(VDB);
  return OK;
  }

//===========================================================================
//
//
//
//===========================================================================

const int Id_TolParent    = 10;
const int Id_TolAbs       = 11;
const int Id_TolRel       = 12;
const int Id_TolMax       = 13;
const int Id_TolAbsMult   = 14;
const int Id_TolRelMult   = 15;


//------------------------------------------------------------------------------

TolObjectEdt::TolObjectEdt(pFxdEdtView pView_, pTolObject pTolO_) :
  FxdEdtBookRef(pView_),
  rTolO(*pTolO_)
  {
  TCnv.Set  (DC_Frac,      "%");
  MCnv.Set  (DC_Frac,      "-");
  TFmt.Set ("", 0, 2, 'e');
  MFmt.Set ("", 0, 2, 'f');
  iPg1=0;
  
  //pWrkCnv=NULL;
  //pWrkFmt=NULL;

  ObjectAttribute *pAttr=ObjAttributes.FindObject("TolObjectEdt");
  if (pAttr)
    {
    pAttr->FieldFmtCnvs("T",   TFmt,   TCnv);
    pAttr->FieldFmtCnvs("M",   MFmt,   MCnv);
    }
  }

//---------------------------------------------------------------------------

TolObjectEdt::~TolObjectEdt()
  {
  ObjectAttribute *pAttr=ObjAttributes.FindObject("TolObjectEdt");
  if (pAttr)
    {
    pAttr->SetFieldFmt  ("T",    TFmt);
    pAttr->SetFieldCnvs ("T",    TCnv);
    pAttr->SetFieldFmt  ("M",    MFmt);
    pAttr->SetFieldCnvs ("M",    MCnv);
    }
  }

//---------------------------------------------------------------------------

int TolObjectEdt::DistFromPgNo(int Pg)
  {
  return -1;
  }

//---------------------------------------------------------------------------

void TolObjectEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void TolObjectEdt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void TolObjectEdt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void TolObjectEdt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void TolObjectEdt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

void TolObjectEdt::Build()
  {
  Strng S;
  int L, i;
  L=0;

  iPg1=pView->Pages;

  m_Blks.SetSize(0, 64);

  int j=0;
  m_Blks.SetAtGrow(j++, NULL);
  for (i=0; i<CToleranceBlock::s_Array.GetSize(); i++)
    {
    dword Flags=CToleranceBlock::s_Array[i]->GetFlags();
    if (((Flags&TBF_Sys)!=0) &&
        ((Flags&TBF_Hide)==0) &&
        (((Flags&TBF_PB)!=0) && DefNetProbalMode() || 
         ((Flags&TBF_Dyn)!=0) && DefNetDynamicMode()))
      m_Blks.SetAtGrow(j++, CToleranceBlock::s_Array[i]);
    }
  m_Blks.SetAtGrow(j++, NULL);
  for (i=0; i<CToleranceBlock::s_Array.GetSize(); i++)
    {
    dword Flags=CToleranceBlock::s_Array[i]->GetFlags();
    if (((Flags&TBF_Sys)==0) &&
        ((Flags&TBF_Hide)==0) &&
        (((Flags&TBF_PB)!=0) && DefNetProbalMode() || 
         ((Flags&TBF_Dyn)!=0) && DefNetDynamicMode()))
      m_Blks.SetAtGrow(j++, CToleranceBlock::s_Array[i]);
    }

  long TagWide=5;
  long CnvWide=1;
  for (i=0; i<m_Blks.GetSize(); i++)
    if (m_Blks[i])
      {
      CToleranceBlock *p=m_Blks[i];
      TagWide=Max(TagWide, (long)strlen(p->GetName()));
      CnvWide=Max(CnvWide, (long)p->GetAbsCnv().TextLength());
      }

  Strng Tg;
  StartPage("Tolerances");
  StartBlk(3, 0, NULL);
  L=0;
  SetSpace(L,2);
  L++;
  SetDesc(L, "Tag",       -1, TagWide, 0, " ");
  SetDesc(L, "",          -1, CnvWide, 0, " ");
  SetDesc(L, "Abs",       -1, 10, 2, "  ");
  SetDesc(L, "Rel",       -1, 10, 2, "  ");
  SetDesc(L, "MaxIt",     -1, 5, 2, "  ");
  SetDesc(L, "AbsMult",   -1, 10, 2, "  ");
  SetDesc(L, "RelMult",   -1, 10, 2, "  ");
  SetDesc(L, "Parent",    -1, TagWide, 0, " ");
  
  L++;
  SetDesc(L, "",          -1, TagWide, 0, " ");
  SetDesc(L, "",          -1, CnvWide, 0, " ");
  SetDesc(L, "",          -1, 10, 2, "  ");
  SetDesc(L, TCnv.Text(), -1, 10, 2, "  ");
  SetDesc(L, "",          -1,  5, 2, "  ");
  SetDesc(L, MCnv.Text(), -1, 10, 2, "  ");
  SetDesc(L, MCnv.Text(), -1, 10, 2, "  ");
  SetDesc(L, "",          -1, TagWide, 0, "");
  
  StartBlk(m_Blks.GetSize(), 0, NULL);
  L=0;
  
  for (i=0; i<m_Blks.GetSize(); i++)
    {
    if (m_Blks[i]==NULL)
      {
      SetDesc(L, i==0 ? "System" : "Models", -1, 20);
      }
    else
      {
      CToleranceBlock *p=m_Blks[i];
      bool Editable = ((p->GetFlags()&TBF_Fixed)==0);

      SetDesc(L, (char*)p->GetName(), -1, TagWide, 0, " ");
      SetDesc(L, (char*)p->GetAbsCnv().Text(), -1, CnvWide, 0, " ");
    
      if (p->UseAbs() || p->UseAbsParent())
        {
        SetParm(L, "", Id_TolAbs, 10, 2, Valid(p->GetAbsDD().dOvr)?"* ":"  ", Editable && p->UseAbs());
        Tg.Set("$Tol.%s.Abs", p->GetName());
        SetTag(Tg());
        }
      else
        SetDesc(L, "", -1, 10, 0, "  ");

      if (p->UseRel() || p->UseRelParent())
        {
        SetParm(L, "", Id_TolRel, 10, 2, Valid(p->GetRelDD().dOvr)?"* ":"  ", Editable && p->UseRel());
        Tg.Set("$Tol.%s.Rel", p->GetName());
        SetTag(Tg());
        }
      else
        SetDesc(L, "", -1, 10, 0, "  ");

      if (p->UseMax()||p->UseMaxParent())
        {
        SetParm(L, "", Id_TolMax, 5, 2, Valid(p->GetMaxItersDD().dOvr)?"* ":"  ", Editable && p->UseMax());
        Tg.Set("$Tol.%s.MaxIt", p->GetName());
        SetTag(Tg());
        }
      else
        SetDesc(L, "", -1, 5, 0, "  ");

      if (p->UseAbsParent())
        {
        SetParm(L, "", Id_TolAbsMult, 10, 2, Valid(p->GetAbsMultDD().dOvr)?"* ":"  ", Editable);
        Tg.Set("$Tol.%s.AbsMult", p->GetName());
        SetTag(Tg());
        }
      else
        SetDesc(L, "", -1, 10, 0);
    
      if (p->UseRelParent())
        {
        SetParm(L, "", Id_TolRelMult, 10, 2, Valid(p->GetRelMultDD().dOvr)?"* ":"  ", Editable);
        Tg.Set("$Tol.%s.RelMult", p->GetName());
        SetTag(Tg());
        }
      else
        SetDesc(L, "", -1, 10, 0);

      if (p->HasParent())
        {
        SetParm(L, "", Id_TolParent,  TagWide, 0, " ", false);
        Tg.Set("$Tol.%s.Abs", p->GetName());
        SetTag(Tg());
        }
      else
        SetDesc(L, "", -1, TagWide+1, 0);
      }
    L++;
    }

  StartBlk(3, 0, NULL);
  L=0;
  SetSpace(L,2);
  L++;
  SetDesc(L, " * implies Default is overidden (enter '*' to restore)",  -1, 60, 0, "");
 
  }

//---------------------------------------------------------------------------

void TolObjectEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {//data
    int pg=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    }

  if (CurrentBlk(EI))
    {//data
    int pg=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    CToleranceBlock *p;
    switch (EI.FieldId)
      {
      case Id_TolAbs:
        p=m_Blks[i];
        TFmt.FormatFloat(p->GetAbsCnv().Human(p->GetAbs()), Str);
        break;
      case Id_TolRel:
        TFmt.FormatFloat(TCnv.Human(m_Blks[i]->GetRel()), Str);
        break;
      case Id_TolMax:
        Str.Set("%i", m_Blks[i]->GetMaxIters());
        break;
      case Id_TolAbsMult:
        MFmt.FormatFloat(MCnv.Human(m_Blks[i]->GetAbsMult()), Str);
        break;
      case Id_TolRelMult:
        MFmt.FormatFloat(MCnv.Human(m_Blks[i]->GetRelMult()), Str);
        break;
      case Id_TolParent:
        Str=m_Blks[i]->Parent()->GetName();
        break;
      }
    }
  }

//---------------------------------------------------------------------------

long TolObjectEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; 
  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    }
  if (CurrentBlk(EI))
    {//data
    int pg=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    double d;
    CToleranceBlock *p;
    switch (EI.FieldId)
      {
      case Id_TolAbs:
        p=m_Blks[i];
        d=SafeAtoF(Str, 0.0);
        if (Valid(d))
          {
          d=p->GetAbsCnv().Normal(d);
          d=Range(0.0, d, 1.0);
          }
        p->SetAbs(d);
        View().DoRebuild();
        break;
      case Id_TolRel:
        d=SafeAtoF(Str, 1.0e-8);
        if (Valid(d))
          {
          d=TCnv.Normal(d); 
          d=Range(1.0e-12, d, 1.0e-3);
          }
        m_Blks[i]->SetRel(d);
        View().DoRebuild();
        break;
      case Id_TolMax:
        d=SafeAtoF(Str, 100);
        if (Valid(d))
          d=Range(0.0, d, 10000.0);
        m_Blks[i]->SetMaxIters(d);
        View().DoRebuild();
        break;
      case Id_TolAbsMult:
        d=SafeAtoF(Str, 0.10);
        if (Valid(d))
          {
          d=MCnv.Normal(d); 
          d=Range(1.0e-6, d, 0.5);
          }
        m_Blks[i]->SetAbsMult(d);
        View().DoRebuild();
        break;
      case Id_TolRelMult:
        d=SafeAtoF(Str, 0.10);
        if (Valid(d))
          {
          d=MCnv.Normal(d);
          d=Range(1.0e-6, d, 0.5);
          }
        m_Blks[i]->SetRelMult(d);
        View().DoRebuild();
        break;
      case Id_TolParent:
        break;
      }
    }

  return Fix;
  }

//---------------------------------------------------------------------------

long TolObjectEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TolObjectEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TolObjectEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TolObjectEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TolObjectEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TolObjectEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView &Vw=View();
  flag ret=false;
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      ret=true;
      if (EI.Fld)
        {
        flag TagOnly=false;
        switch (EI.FieldId)
          {
          case Id_TolAbs:
            {
            int i=(int)(EI.BlkRowNo-EI.Index);
            CToleranceBlock *p=m_Blks[i];
            WrkIB.Set(EI.Fld->Tag, &p->GetAbsCnv(), &TFmt);
            break;
            }
          case Id_TolRel:
            WrkIB.Set(EI.Fld->Tag, &TCnv, &TFmt);
            break;
          case Id_TolAbsMult:
          case Id_TolRelMult:
            WrkIB.Set(EI.Fld->Tag, &MCnv, &MFmt);
            break;
          }

        if (WrkIB.FmtOK() || TagOnly)
          {
          CRect WRect;
          Vw.GetWindowRect(&WRect);
          CPoint  RBPoint;
          RBPoint.x = WRect.left+point.x;
          RBPoint.y = WRect.top+point.y;

          CMenu Menu;
          Menu.CreatePopupMenu();
          CMenu FormatMenu;
          FormatMenu.CreatePopupMenu();
          if (WrkIB.FmtOK())
            WrkIB.Fmt().AddToMenu(FormatMenu);//, "");
          CMenu CnvMenu;
          CnvMenu.CreatePopupMenu();
          if (WrkIB.CnvOK())
            WrkIB.Cnv().AddToMenu(CnvMenu);//, d.iCnv, d.sCnvTxt());

          Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
          if ((WrkIB.CnvOK() && (WrkIB.Cnv().Index())<=0) || TagOnly)
            Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);

          Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");
          if (TagOnly)
            Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);
          
          Menu.AppendMenu(MF_SEPARATOR);
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyTag, "Copy Full &Tag");
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyRHSTag, "Copy Tag");
          if (EI.Fld->Tag==NULL)
            {
            Menu.EnableMenuItem(pView->iCmdCopyTag, MF_BYCOMMAND|MF_GRAYED);
            Menu.EnableMenuItem(pView->iCmdCopyRHSTag, MF_BYCOMMAND|MF_GRAYED);
            }
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyVal, "Copy &Value");
          
          Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, &View());
          Menu.DestroyMenu();                                           
          }

        }
      }
    }
  return ret;
  }

//---------------------------------------------------------------------------

flag TolObjectEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag TolObjectEdt::DoAccCnv(UINT Id) 
  {
  CDataCnv *pC=Cnvs[(WrkIB.Cnv().Index())];
  for (int i=Id; i>0; i--)
    pC=pC->Next();
    
  if (pC)
    {
    WrkIB.Cnv().SetText(pC->Txt());
    }
  View().DoRebuild();
  return true;
  };

//---------------------------------------------------------------------------

flag TolObjectEdt::DoAccFmt(UINT Id) 
  {
  for (UINT i=0; i<(UINT)DefinedFmts.GetSize(); i++) 
    if (i==Id) 
      break;

  if (i<(UINT)DefinedFmts.GetSize())
    {
    WrkIB.Fmt()=DefinedFmts[i];
    View().DoRebuild();
    }
  return true;
  };

//---------------------------------------------------------------------------

flag TolObjectEdt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  };

//===========================================================================
//
//                                  CtrlObject
//
//===========================================================================

#if WITHCTRLOBJ

#if WITHCTRLEDIT
IMPLEMENT_TAGOBJEDT(CtrlObject, "FS_Ctrl", "FS_Ctrl", "", "Controls", TOC_SYSTEM,
                    CtrlObjectEdt, "The Controls", "The Controls");
#else
IMPLEMENT_TAGOBJ(CtrlObject, "FS_Ctrl", "FS_Ctrl", "", "Controls", TOC_SYSTEM,
                 "The Controls", "The Controls");
#endif
CtrlObject::CtrlObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CTNode(pClass_, TagIn, pAttach, eAttach)
  {
  bDoSystemDefn=false;
  };

CtrlObject::~CtrlObject()
  {
  }

//--------------------------------------------------------------------------

void CtrlObject::PrepareDefinedData() 
  {
  };

//--------------------------------------------------------------------------

XID xidSpt          = ModelXID(1000);
XID xidMeas         = ModelXID(1000)+1;
XID xidOutput       = ModelXID(1000)+2;
XID xidStatusWord   = ModelXID(1000)+3;

XID xidPlantGain    = ModelXID(1000)+4;
XID xidPlantDelay   = ModelXID(1000)+5;
XID xidPlantPeriod  = ModelXID(1000)+6;
XID xidPIDGain      = ModelXID(1000)+7;
XID xidPIDIc        = ModelXID(1000)+8;
XID xidPIDDc        = ModelXID(1000)+9;

XID xidSequencerActive = ModelXID(1000)+10;
XID xidHoldAll         = ModelXID(1000)+11;
XID xidTuneWithHold    = ModelXID(1000)+12;
                                                  
void CtrlObject::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.Text  ("");
    DDB.Bool  ("SequencerActive", "", DC_, "", xidSequencerActive, this, isParm);
    DDB.Bool  ("HoldAll",         "", DC_, "", xidHoldAll, this, isParm);
    DDB.Bool  ("TuneWithHold",    "", DC_, "", xidTuneWithHold, this, isParm);
    DDB.Long  ("MinSettleCount",  "", DC_, "", &CtrlSeq().m_MinSettleCount, this, isParm);
    DDB.Long  ("MaxSettleCount",  "", DC_, "", &CtrlSeq().m_MaxSettleCount, this, isParm);
    DDB.Double("MaxPBDerate",     "", DC_, "", &CtrlSeq().m_MaxPBDerate, this, isParm);
    DDB.Double("RetuneOutDelta",  "", DC_, "", &CtrlSeq().m_RetuneOutDelta, this, isParm);
    DDB.Double("TestDelta",       "", DC_, "", &CtrlSeq().m_TestDelta, this, isParm);

    if (!DDB.ForFileSnpScn())
      {
      CtrlSequencer::LockCSBs();
      for (int i=0; i<CtrlSequencer::NCSBs(); i++)
        {
        CtrlSeqBlk & T=CtrlSequencer::CSBlk(i);
        Strng S1=T.PIDTag();

        if (DDB.BeginStruct(this, S1(), NULL, DDB_NoPage, i, 0))
          {
          DDB.Double("Spt",     "", DC_, "", xidSpt, this, isParm);
          DDB.Range(T.InMin(), T.InMax());
          DDB.Double("Meas",    "", DC_, "", xidMeas, this, isParm);
          DDB.Range(T.InMin(), T.InMax());
          DDB.Double("Out",     "", DC_, "", xidOutput, this, isParm);
          DDB.Range(T.OutMin(), T.OutMax());

          DDB.Double("PlantGain",   "", DC_, "", xidPlantGain  , this, 0);
          DDB.Double("PlantDelay",  "", DC_, "", xidPlantDelay , this, 0);
          DDB.Double("PlantPeriod", "", DC_, "", xidPlantPeriod, this, 0);
          DDB.Double("PIDGain",     "", DC_, "", xidPIDGain    , this, isParm);
          DDB.Double("PIDIc",       "", DC_, "", xidPIDIc      , this, isParm);
          DDB.Double("PIDDc",       "", DC_, "", xidPIDDc      , this, isParm);

          DDB.DWord("StatusWord","", DC_, "", xidStatusWord, this, isParm);
          }
        DDB.EndStruct();
        }
      CtrlSequencer::UnlockCSBs();
      }
    }

  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CtrlObject::DataXchg(DataChangeBlk & DCB)
  {
  if (CTNode::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidSequencerActive:
      if (DCB.rB)
        CtrlSeq().SetSequencerActive(*DCB.rB);
      DCB.B=CtrlSeq().SequencerActive(); 
      return true;
    case xidHoldAll:
      if (DCB.rB)
        CtrlSeq().SetHoldAll(*DCB.rB);
      DCB.B=CtrlSeq().HoldAll(); 
      return true;
    case xidTuneWithHold:
      if (DCB.rB)
        CtrlSeq().SetTuneWithHold(*DCB.rB);
      DCB.B=CtrlSeq().TuneWithHold(); 
      return true;
    case xidSpt:
      if (DCB.rD)
        CtrlSequencer::CSBlk(DCB.dwUserInfo).SetSetPoint(*DCB.rD);
      DCB.D=CtrlSequencer::CSBlk(DCB.dwUserInfo).SetPoint(); 
      return true;
    case xidMeas:
      DCB.D=CtrlSequencer::CSBlk(DCB.dwUserInfo).Measure(); 
      return true;
    case xidOutput:
      if (DCB.rD)
        CtrlSequencer::CSBlk(DCB.dwUserInfo).SetOutput(*DCB.rD);
      DCB.D=CtrlSequencer::CSBlk(DCB.dwUserInfo).Output();
      return true;
    case xidPlantGain:
      DCB.D=CtrlSequencer::CSBlk(DCB.dwUserInfo).PlantGain();
      return true;
    case xidPlantDelay:
      DCB.D=CtrlSequencer::CSBlk(DCB.dwUserInfo).PlantDelay();
      return true;
    case xidPlantPeriod:
      DCB.D=CtrlSequencer::CSBlk(DCB.dwUserInfo).PlantPeriod();
      return true;
    case xidPIDGain:
      if (DCB.rD)
        {
        double G, I, D;
        CtrlSequencer::CSBlk(DCB.dwUserInfo).GetOpGain(G, I, D);
        G=GTZ(*DCB.rD);
        CtrlSequencer::CSBlk(DCB.dwUserInfo).SetOpGain(G, I, D);
        }
      DCB.D=CtrlSequencer::CSBlk(DCB.dwUserInfo).OpGain();
      return true;
    case xidPIDIc:
      if (DCB.rD)
        {
        double G, I, D;
        CtrlSequencer::CSBlk(DCB.dwUserInfo).GetOpGain(G, I, D);
        I=*DCB.rD;
        CtrlSequencer::CSBlk(DCB.dwUserInfo).SetOpGain(G, I, D);
        }
      DCB.D=CtrlSequencer::CSBlk(DCB.dwUserInfo).OpIc();
      return true;
    case xidPIDDc:
      if (DCB.rD)
        {
        double G, I, D;
        CtrlSequencer::CSBlk(DCB.dwUserInfo).GetOpGain(G, I, D);
        D=*DCB.rD;
        CtrlSequencer::CSBlk(DCB.dwUserInfo).SetOpGain(G, I, D);
        }
      DCB.D=CtrlSequencer::CSBlk(DCB.dwUserInfo).OpDc();
      return true;

    case xidStatusWord:
      if (DCB.rDW)
        {
        dword Mask=(*DCB.rDW >> 16) & 0xffff;
        dword Values=(*DCB.rDW & 0xffff);
        CtrlSequencer::CSBlk(DCB.dwUserInfo).SetStatusWord(Values, Mask);
        }
      DCB.DW=CtrlSequencer::CSBlk(DCB.dwUserInfo).StatusWord();
      return true;
    }

  return 0;
  }

//--------------------------------------------------------------------------

flag CtrlObject::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=CTNode::ValidateData(VDB);
  return OK;
  }

//===========================================================================
//
//
//
//===========================================================================

#if WITHCTRLEDIT

const int Id_View               = 1;
const int Id_TuneRqst           = 2;
const int Id_TuneState          = 3;
const int Id_Active             = 4;
const int Id_AutoMan            = 5;
const int Id_State              = 6;
const int Id_Reqd               = 7;
const int Id_Value              = 8;
const int Id_ValueLo            = 9;
const int Id_ValueHi            = 10;
const int Id_Control            = 11;
const int Id_ControlLo          = 12;
const int Id_ControlHi          = 13;
const int Id_AutoTune           = 20;
const int Id_MinSettleCount     = 21;
const int Id_MaxSettleCount     = 23;
const int Id_TestDelta          = 24;
const int Id_MaxPBDerate        = 26;
const int Id_RetuneOutDelta     = 27;
const int Id_DeTune             = 29;
const int Id_Gain               = 30;
const int Id_Ic                 = 31;
const int Id_Dc                 = 32;
const int Id_PlantGain          = 33;
const int Id_PlantDelay         = 34;
const int Id_PlantPeriod        = 35;
const int Id_TuneRule           = 36;
const int Id_TuneOut            = 37;
const int Id_EPSAbs             = 38;
const int Id_EPSRel             = 39;
const int Id_SptTag             = 40;
const int Id_MeasTag            = 41;
const int Id_OutTag             = 42;

//------------------------------------------------------------------------------

long CtrlObjectEdt::m_iView=0;

CtrlObjectEdt::CtrlObjectEdt(pFxdEdtView pView_, pCtrlObject pChgO_) :
  FxdEdtBookRef(pView_),
  rChgO(*pChgO_)
  {
  iPg1=0;
//  CtrlSeqBlkIter CSBs(CtrlSeqBlk::List);
//  for (CtrlSeqBlk* pT=CSBs.First(); pT; pT=CSBs.Next())
    //{
////  TearVarBlkIter TCBs(TearVarBlk::List);
////  for (TearVarBlk* pT=TCBs.First(); pT; pT=TCBs.Next())
    //if (pT->Active() && pT->TearSolveReqd())
    //if (pT->TearSolveReqd())
//    m_Ctrls.Add(pT);
  m_iView=Range(0L, m_iView, CtrlSequencer::NCSBs()-1L);
  }

//---------------------------------------------------------------------------

CtrlObjectEdt::~CtrlObjectEdt()
  {
  }

//---------------------------------------------------------------------------

int CtrlObjectEdt::DistFromPgNo(int Pg)
  {
  return -1;
  }

//---------------------------------------------------------------------------

void CtrlObjectEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void CtrlObjectEdt::PutDataDone()
  {
//  for (int i=0; i<m_Ctrls.GetSize(); i++)
//    {
////    CtrlSequencer::CSBlk(i).TearGetTuneInfo();
////    CtrlSequencer::CSBlk(i).TearGetErrors();
////    CtrlSequencer::CSBlk(i).TearGetOutputs();
//    }
  }

//---------------------------------------------------------------------------

void CtrlObjectEdt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void CtrlObjectEdt::GetDataDone()
  {
//  for (int i=0; i<m_Ctrls.GetSize(); i++)
//    {
////    CtrlSequencer::CSBlk(i).TearSetTuneInfo();
////    CtrlSequencer::CSBlk(i).TearSetOutputs();
//    }
  }

//---------------------------------------------------------------------------

void CtrlObjectEdt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

#define IVIEW_VALID (m_iView>=0 && m_iView<CtrlSequencer::NCSBs())

void CtrlObjectEdt::Build()
  {
  int L, i;

  iPg1=pView->Pages;

  CtrlSequencer::LockCSBs();
  int NCtrls=CtrlSequencer::NCSBs();
  int CTagWide=10;
  CtrlSeqBlk* pVwT= IVIEW_VALID ? CtrlSequencer::CSBs()[m_iView].Blk() : NULL;
  for (i=0; i<NCtrls; i++)
    CTagWide=Max(CTagWide, (int)strlen(CtrlSequencer::CSBlk(i).PIDTag()));

  Strng S("");
  Strng STag("SetPoint");//"P_5.Qi.T (C)");
  Strng MTag("Measure");//"P_5.Qi.T (C)");
  Strng CTag("Control");//"X_1.ReqdT (C)");

  if (pVwT)
    {
    S=pVwT->PIDTag();
//    if (pVwT->SptTag())
//      STag=pVwT->SptTag();
//    if (pVwT->MeasTag())
//      MTag=pVwT->MeasTag();
//    if (pVwT->OutTag())
//      CTag=pVwT->OutTag();
    }

  int TagWide=10;
//  int TagWide=Max(10, Max(MTag.Length(), CTag.Length()));
//  TagWide=Max(TagWide, STag.Length());
  TagWide=Max(TagWide, S.Length());
  TagWide=Max(TagWide, CTagWide);
  int Gap=3;
  int RTagWide=6;

  StartPage("Controls");
  StartBlk(15, 0, NULL);
  L=0;
  SetSpace(L,8);
  L++;
  SetDesc(L,S(),  -1, TagWide, 0, " ");
  SetDesc(L,"Value",  -1, 10, 2, " ");
  SetDesc(L,"Source",  -1, 40, 0, " ");
  //SetDesc(L,"LoRng",  -1, 10, 2, " ");
  //SetDesc(L,"HiRng",  -1, 10, 2, " ");
  L++;
  SetDesc(L,STag(),  -1, TagWide, 0, " ");
  SetParm(L, "", Id_Reqd    ,  10, 2, " ");
  //SetParm(L, "", Id_SptTag  ,  40, 0, " ");
  if (pVwT)
    SetDesc(L, (pVwT->SptTag() ? pVwT->SptTag() : ""),  -1, 40, 0, " ");
  L++;
  SetDesc(L,MTag(),     -1, TagWide, 0, " ");
  SetParm(L, "", Id_Value   ,  10, 2, " ");
  if (pVwT)
    SetDesc(L, (pVwT->MeasTag() ? pVwT->MeasTag() : ""),  -1, 40, 0, " ");
  //SetParm(L, "", Id_MeasTag ,  40, 0, " ");
  //SetParm(L, "", Id_ValueLo ,  10, 2, " ");
  //SetParm(L, "", Id_ValueHi ,  10, 2, " ");
  L++;
  SetDesc(L,CTag(),   -1, TagWide, 0, " ");
  SetParm(L, "", Id_Control ,  10, 2, " ");
  if (pVwT)
    SetDesc(L, (pVwT->OutTag() ? pVwT->OutTag() : ""),  -1, 40, 0, " ");
  //SetParm(L, "", Id_OutTag  ,  40, 0, " ");
  //SetParm(L, "", Id_ControlLo, 10, 2, " ");
  //SetParm(L, "", Id_ControlHi, 10, 2, " ");
  L++;
  SetSpace(L,8);
  L++;
  SetSpace(L,TagWide+1);
  SetDesc(L, "Plant", -1, 10,  0, " ");
  SetSpace(L,Gap+RTagWide+1);
  SetDesc(L, "Controller", -1, TagWide,  0, " ");
  L++;
  SetDesc(L, "Gain", -1, TagWide,  0, " ");
  SetParm(L, "", Id_PlantGain,  10, 2, " ");
  SetSpace(L,Gap);
  SetDesc(L, "Gain", -1, RTagWide,  0, " ");
  SetParm(L, "", Id_Gain,  10, 2, " ");
  L++;
  SetDesc(L, "Delay", -1, TagWide,  0, " ");
  SetParm(L, "", Id_PlantDelay,  10, 2, " ");
  SetSpace(L,Gap);
  SetDesc(L, "Ic", -1, RTagWide,  0, " ");
  SetParm(L, "", Id_Ic,  10, 2, " ");
  L++;
  SetDesc(L, "Period", -1, TagWide,  0, " ");
  SetParm(L, "", Id_PlantPeriod,  10, 2, " ");
  SetSpace(L,Gap);
  SetDesc(L, "Dc", -1, RTagWide,  0, " ");
  SetParm(L, "", Id_Dc,  10, 2, " ");
  L++;
  
  L++;
  SetDesc(L, "TuneRule", -1, TagWide,  0, " ");
  SetParm(L, "", Id_TuneRule,  10, 0, " ");
  for (int itr=0; DDBTuneRules[itr].m_pStr; itr++)
    FldHasFixedStrValue(DDBTuneRules[itr].m_lVal, DDBTuneRules[itr].m_pStr);

  SetSpace(L,Gap);
  SetDesc(L, "AbsTol", -1, RTagWide,  0, " ");
  SetParm(L, "", Id_EPSAbs, 10, 2, "%");

  L++;
  SetDesc(L, "DeTuning", -1, TagWide,  0, " ");
  SetParm(L, "", Id_DeTune,  10, 2, "%");
  SetSpace(L,Gap);
  SetDesc(L, "RelTol", -1, RTagWide,  0, " ");
  SetParm(L, "", Id_EPSRel, 10, 2, "%");

//  L++;
//  SetDesc(L, "TuneOutput", -1, TagWide,  0, " ");
//  SetParm(L, "", Id_TuneOut,  10, 2, " ");

  Strng Tg, D;
  StartBlk(NCtrls+1, 1, NULL);
  L=0;
  SetDesc(L,"Tag",      -1, CTagWide, 0, " ");
  SetDesc(L,"V",        -1, 2, 0, "");
  SetDesc(L,"T",        -1, 2, 0, "");
  SetDesc(L,"A",        -1, 2, 0, "");
  //SetDesc(L,"B",        -1, 2, 0, "");
  //SetDesc(L,"R",        -1, 2, 0, "");
  SetDesc(L,"State",    -1,  7, 2, "");
  SetDesc(L,"SetPoint", -1, 10, 2, "");
  SetDesc(L,"Measure",    -1, 10, 2, "");
  SetDesc(L,"Control",  -1, 10, 2, "");
  //SetDesc(L,"Gain",     -1, 10, 2, "");

  L++;
  for (i=0; i<NCtrls; i++)
    {
    //double G=CtrlSequencer::CSBlk(i).Gain(0.5);
    SetDesc(L, CtrlSequencer::CSBlk(i).PIDTag(),  -1, CTagWide,  0, " ");
    //SetCheckBoxBtn(L, CtrlSequencer::CSBlk(i).CtrlActive() ? "1" : "0", Id_Active  , 2, 2, "", false);
    SetCheckBoxBtn(L, i==m_iView ? "1" : "0", Id_View, 2, 2, "", true);
    SetCheckBoxBtn(L, CtrlSequencer::CSBlk(i).TuneRqst() ? "1" : "0", Id_TuneRqst   , 2, 2, "", true);
    //SetCheckBoxBtn(L, CtrlSequencer::CSBlk(i).TuneBusy() ? "1" : "0", Id_TuneBusy   , 2, 2, "", true);
    //SetCheckBoxBtn(L, G<0 ? "1" : "0", Id_Reverse , 2, 2, "", false);
    SetCheckBoxBtn(L, CtrlSequencer::CSBlk(i).AutoMan() ? "1":"0", Id_AutoMan, 2, 2, "", true);
    SetParm(L, "", Id_State,     7, 2, "");
    SetParm(L, "", Id_Reqd,     10, 2, "");
    SetParm(L, "", Id_Value,    10, 2, "");
    SetParm(L, "", Id_Control,  10, 2, "");
    //SetParm(L, "", Id_Gain,     10, 2, "");
    L++;
    }

  StartPage("Tuning");
  StartBlk(15, 0, NULL);
  L=0;
  SetDesc(L,"",  -1, 10, 0, "");
  L++;
  SetDesc(L,"Auto Tune",  -1, 18, 0, "");
  SetCheckBoxBtn(L, CtrlSeq().SequencerActive() ? "1" : "0", Id_AutoTune, 2, 2, "", true);

  L++;
  SetDParm(L,"MinSettleCount",    18, "", Id_MinSettleCount   , 10, 2, " ");
  L++;
  SetDParm(L,"MaxSettleCount",    18, "", Id_MaxSettleCount   , 10, 2, " ");
  L++;
  SetDParm(L,"TestDelta",         18, "", Id_TestDelta        , 10, 2, " ");
  L++;
  //SetDParm(L,"PBandMult",         18, "", Id_PBMult           , 10, 2, " ");
  //L++;
  SetDParm(L,"MaxPBDerate",       18, "", Id_MaxPBDerate      , 10, 2, " ");
  L++;
  SetDParm(L,"RetuneOutDelta",    18, "", Id_RetuneOutDelta   , 10, 2, " ");
  L++;

  CtrlSequencer::UnlockCSBs();

  }

//---------------------------------------------------------------------------

void CtrlObjectEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  switch (EI.PageNo)
    {
    case 0:
      {
      if (CurrentBlk(EI))
        {//data
        int pg=EI.PageNo;
        int i=(int)(EI.BlkRowNo-EI.Index);
        if (!IVIEW_VALID)
          {
          Str="";
          EI.Fld->fEditable=false;
          }
        else
          {
          CtrlSeqBlk & B=CtrlSequencer::CSBlk(m_iView);
          switch (EI.FieldId)
            {
            case Id_Reqd:
              {
              B.InFmt().FormatFloat(B.InCnv().Human(B.SetPoint()), Str);
              break;
              }
            case Id_Value:
            case Id_ValueLo:
            case Id_ValueHi:
              {
              //TearVar & C=B.Ctrl();
              double V;
              switch (EI.FieldId)
                {
                case Id_Value: V=B.Measure(); break;
                case Id_ValueLo: V=B.InMin(); break;
                case Id_ValueHi: V=B.InMax(); break;
                }
              B.InFmt().FormatFloat(B.InCnv().Human(V), Str);
              EI.Fld->fEditable=false;
              }
              break;
            case Id_Control:
            case Id_ControlLo:
            case Id_ControlHi:
              {
              //TearVar & C=B.Ctrl();
              double V;
              switch (EI.FieldId)
                {
                case Id_Control: V=B.Output(); break;
                case Id_ControlLo: V=B.OutMin(); break;
                case Id_ControlHi: V=B.OutMax(); break;
                }
              B.OutFmt().FormatFloat(B.OutCnv().Human(V), Str);
              EI.Fld->fEditable=(EI.FieldId==Id_Control);
              }
              break;
            case Id_PlantGain:
              {
              #if MultiGainValues
              double V=B.PlantGain(B.OutputN());
              #else
              double V=B.PlantGain();
              #endif
              if (fabs(V)>1.0e-30)
                Str.Set("%.3f", V);
              else
                Str="";
              EI.Fld->fEditable=false;
              }
              break;
            case Id_PlantDelay:
              {
              #if MultiGainValues
              double V=B.PlantDelay(B.OutputN());
              #else
              double V=B.PlantDelay();
              #endif
              if (fabs(V)>1.0e-30)
                Str.Set("%.3f", V);
              else
                Str="";
              EI.Fld->fEditable=false;
              }
              break;
            case Id_PlantPeriod:
              {
              #if MultiGainValues
              double V=B.PlantPeriod(B.OutputN());
              #else
              double V=B.PlantPeriod();
              #endif
              if (fabs(V)>1.0e-30)
                Str.Set("%.3f", V);
              else
                Str="";
              EI.Fld->fEditable=false;
              }
              break;
            case Id_TuneRule:
              {
              int j=B.TuneRule();
              for (int in=0; DDBTuneRules[in].m_pStr; in++) 
                if (j==DDBTuneRules[in].m_lVal)
                  break;
              if (DDBTuneRules[in].m_pStr)
                Str=DDBTuneRules[in].m_pStr;
              else
                Str="??";
              }
              break;
            case Id_TuneOut:
              {
              if (Valid(B.TuneOut()))
                Str.Set("%.3f", B.TuneOut());
              else
                Str="*";
              //EI.Fld->fEditable=false;
              }
              break;
            case Id_EPSAbs:
              {
              if (Valid(B.EPSAbs()))
                Str.Set("%i", B.EPSAbs()*100);
              else
                Str="*";
              //EI.Fld->fEditable=false;
              }
              break;
            case Id_EPSRel:
              {
              if (Valid(B.EPSRel()))
                Str.Set("%.3f", B.EPSRel()*100);
              else
                Str="*";
              //EI.Fld->fEditable=false;
              }
              break;
            //case Id_GainScl:
            //  Str.Set("%.3f", B.GainScale());
            //  //EI.Fld->fEditable=false;
            //  break;
            case Id_DeTune:
              Str.Set("%.3f", B.DeTune()*100);
              //EI.Fld->fEditable=false;
              break;
            case Id_Gain:
              #if MultiGainValues
              Str.Set("%.3f", B.Gain(B.OutputN())*B.Derate());
              #else
              Str.Set("%.3f", B.OpGain());
              #endif
              //EI.Fld->fEditable=false;
              break;
            case Id_Ic:
              #if MultiGainValues
              Str.Set("%.3f", B.ITime(B.OutputN()));
              #else
              Str.Set("%.3f", B.OpIc());
              #endif
              //EI.Fld->fEditable=false;
              break;
            case Id_Dc:
              #if MultiGainValues
              Str.Set("%.3f", B.DTime(B.OutputN()));
              #else
              Str.Set("%.3f", B.OpDc());
              #endif
              //EI.Fld->fEditable=false;
              break;
            case Id_SptTag:
              Str=B.SptTag();
              EI.Fld->fEditable=false;
              break;
            case Id_MeasTag:
              Str=B.MeasTag();
              EI.Fld->fEditable=false;
              break;
            case Id_OutTag:
              Str=B.OutTag();
              EI.Fld->fEditable=false;
              break;
  //          case Id_Gain:
  //            //Str.Set("%.3f", B.Gain(B.OutputN()));
  //            EI.Fld->fEditable=false;
  //            break;
  //          case Id_GainPt0:
  //          case Id_GainPt1:
  //          case Id_GainPt2:
  //          case Id_GainPt3:
  //          case Id_GainPt4:
  //          case Id_GainPt5:
  //            Str.Set("%.3f", B.Gain((EI.FieldId-Id_GainPt0)*0.2));
  //            EI.Fld->fEditable=false;
  //            break;
  //          case Id_IcPt0:
  //          case Id_IcPt1:
  //          case Id_IcPt2:
  //          case Id_IcPt3:
  //          case Id_IcPt4:
  //          case Id_IcPt5:
  //            Str.Set("%.3f", B.IntConst((EI.FieldId-Id_IcPt0)*0.2));
  //            EI.Fld->fEditable=false;
  //            break;
            }
          }
        };

      if (CurrentBlk(EI))
        {//data
        int pg=EI.PageNo;
        int i=(int)(EI.BlkRowNo-EI.Index);
        switch (EI.FieldId)
          {
          case Id_View:
            Str=(m_iView==i ? "1" : "0");
            break;
          case Id_TuneRqst:
            Str=CtrlSequencer::CSBlk(i).TuneRqst() ? "1" : "0";
            EI.Fld->fEditable=CtrlSeq().SequencerActive();
            break;
//          case Id_TuneBusy:
//            Str=CtrlSequencer::CSBlk(i).TuneBusy() ? "1" : "0";
//            EI.Fld->fEditable=false;//CtrlSeq().SequencerActive();
//            break;
//          case Id_Active:
//            Str=CtrlSequencer::CSBlk(i).CtrlActive() ? "1" : "0";
//            EI.Fld->fEditable=false;
//            break;
          case Id_AutoMan:
            Str=CtrlSequencer::CSBlk(i).AutoMan() ? "1" : "0";
            //EI.Fld->fEditable=false;
            break;
//          case Id_Reverse:
//            Str=CtrlSequencer::CSBlk(i).RevActing() ? "1" : "0";
//            EI.Fld->fEditable=false;
//            break;
            case Id_State:
              {
              CtrlSeqBlk & B=CtrlSequencer::CSBlk(i);
              switch (B.TuneBusy())
                {
                case 0:
                  Str= (B.CtrlActive()) ? "Control" : "";
                  break;
                case 1:
                  Str="Settle1";
                  break;
                case 2:
                  Str="Settle2";
                  break;
                default:
                  Str="????";
                  break;
                }
              EI.Fld->fEditable=false;
              break;
              }
          case Id_Reqd:
            {
            CtrlSeqBlk & B=CtrlSequencer::CSBlk(i);
            B.InFmt().FormatFloat(B.InCnv().Human(B.SetPoint()), Str);
            break;
            }
          case Id_Value:
            {
            CtrlSeqBlk & B=CtrlSequencer::CSBlk(i);
            B.InFmt().FormatFloat(B.InCnv().Human(B.Measure()), Str);
            EI.Fld->fEditable=false;
            }
            break;
          case Id_Control:
            {
            CtrlSeqBlk & B=CtrlSequencer::CSBlk(i);
            B.OutFmt().FormatFloat(B.OutCnv().Human(B.Output()), Str);
            }
            break;
          }
        }
      }
      break;
    case 1:
      {
      if (CurrentBlk(EI))
        {//data
        int pg=EI.PageNo;
        int i=(int)(EI.BlkRowNo-EI.Index);
        switch (EI.FieldId)
          {
          case Id_AutoTune:
            Str=(CtrlSeq().SequencerActive() ? "1" : "0");
            break;
          case Id_MinSettleCount:
            Str.Set("%i", CtrlSeq().m_MinSettleCount);
            break;
          case Id_MaxSettleCount:
            Str.Set("%i", CtrlSeq().m_MaxSettleCount);
            break;
          case Id_TestDelta:
            Str.Set("%10.3f", CtrlSeq().m_TestDelta);
            break;
          //case Id_PBMult:
          //  Str.Set("%10.3f", CtrlSeq().m_PBMult);
          //  break;
          case Id_MaxPBDerate:
            Str.Set("%10.3f", CtrlSeq().m_MaxPBDerate);
            break;
          case Id_RetuneOutDelta:
            Str.Set("%10.3f", CtrlSeq().m_RetuneOutDelta);
            break;
          }
        };
      }
      break;
    }
  }

//---------------------------------------------------------------------------

long CtrlObjectEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; 
  switch (EI.PageNo)
    {
    case 0:
      {
      if (CurrentBlk(EI))
        {//data
        int p=EI.PageNo;
        int i=(int)(EI.BlkRowNo-EI.Index);
        if (!IVIEW_VALID)
          {
          Str="";
          EI.Fld->fEditable=false;
          }
        else
          {
          CtrlSeqBlk & B=CtrlSequencer::CSBlk(m_iView);
          switch (EI.FieldId)
            {
            case Id_TuneRule:
              {
              //B.TuneRule()=SafeAtoI(Str(), B.TuneRule());
              for (int in=0; DDBTuneRules[in].m_pStr; in++) 
                if (Str.XStrICmp(DDBTuneRules[in].m_pStr)==0)
                  break;
              if (DDBTuneRules[in].m_pStr)
                B.TuneRule()=(flag)DDBTuneRules[in].m_lVal;
              }
              break;
            case Id_TuneOut:
              B.TuneOut()=SafeAtoF(Str(), B.TuneOut());
              break;
            case Id_EPSAbs:
              B.EPSAbs()=SafeAtoF(Str(), Valid(B.EPSAbs()) ? B.EPSAbs()*100 : dNAN)/100;
              break;
            case Id_EPSRel:
              B.EPSRel()=SafeAtoF(Str(), Valid(B.EPSRel()) ? B.EPSRel()*100 : dNAN)/100;
              break;
            case Id_DeTune:
              B.DeTune()=SafeAtoF(Str(), B.DeTune()*100)/100;
              break;
            case Id_Gain:
              {
              double G, I, D;
              B.GetOpGain(G, I, D);
              G=SafeAtoF(Str, G);
              B.SetOpGain(G, I, D);
//              B.SetOpGain(SafeAtoF(Str, B.OpGain()));
              break;
              }
            case Id_Ic:
              {
              double G, I, D;
              B.GetOpGain(G, I, D);
              I=SafeAtoF(Str, I);
              B.SetOpGain(G, I, D);
              //B.OpIc()=SafeAtoF(Str, B.OpIc());
              break;
              }
            case Id_Dc:
              {
              double G, I, D;
              B.GetOpGain(G, I, D);
              D=SafeAtoF(Str, D);
              B.SetOpGain(G, I, D);
              //B.OpDc()=SafeAtoF(Str, B.OpDc());
              break;
              }
//            case Id_GainScl:
//              //CtrlSeqBlk & B=CtrlSequencer::CSBlk(m_iView);
//              B.SetGainScale(SafeAtoF(Str, B.GainScale()));
//
//              //EI.Fld->fEditable=false;
//              break;
            }
          }
        }

      if (CurrentBlk(EI))
        {//data
        int p=EI.PageNo;
        int i=(int)(EI.BlkRowNo-EI.Index);
        switch (EI.FieldId)
          {
          case Id_View:
            Str.LTrim();
            if (Str() && (Str[0]=='1'))
              m_iView = i;
            View().DoRebuild();
            break;
          case Id_Reqd:
            {
            CtrlSeqBlk & B=CtrlSequencer::CSBlk(i);
            //TearVar & C=CtrlSequencer::CSBlk(i).Ctrl();
            B.InCnv().Normal(SafeAtoF(Str, B.SetPoint()));
            B.SetPoint()=B.InCnv().Normal(SafeAtoF(Str, B.SetPoint()));
            break;
            }
          case Id_Value:
            {
//            TearVar & C=CtrlSequencer::CSBlk(i).Ctrl();
    //        C.InFmt().FormatFloat(C.InCnv().Human(CtrlSequencer::CSBlk(i).Measure()), Str);
    //        EI.Fld->fEditable=false;
            }
            break;
          case Id_Control:
            {
//            TearVar & C=CtrlSequencer::CSBlk(i).Ctrl();
    //        C.OutFmt().FormatFloat(C.OutCnv().Human(CtrlSequencer::CSBlk(i).Output()), Str);
            }
            break;
          }
        }
      }
      break;
    case 1:
      {
      if (CurrentBlk(EI))
        {//data
        int p=EI.PageNo;
        int i=(int)(EI.BlkRowNo-EI.Index);
        switch (EI.FieldId)
          {
          case Id_MinSettleCount:
            CtrlSeq().m_MinSettleCount=SafeAtoL(Str, CtrlSeq().m_MinSettleCount);
            break;
          case Id_MaxSettleCount:
            CtrlSeq().m_MaxSettleCount=SafeAtoL(Str, CtrlSeq().m_MaxSettleCount);
            break;
          case Id_TestDelta:
            CtrlSeq().m_TestDelta=SafeAtoF(Str, CtrlSeq().m_TestDelta);
            break;
          //case Id_PBMult:
          //  CtrlSeq().m_PBMult=SafeAtoF(Str, CtrlSeq().m_PBMult);
          //  break;
          case Id_MaxPBDerate:
            CtrlSeq().m_MaxPBDerate=SafeAtoF(Str, CtrlSeq().m_MaxPBDerate);
            break;
          case Id_RetuneOutDelta:
            CtrlSeq().m_RetuneOutDelta=SafeAtoF(Str, CtrlSeq().m_RetuneOutDelta);
            break;
          }
        }
      }
      break;
    }

  return Fix;
  }

//---------------------------------------------------------------------------

long CtrlObjectEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph

  switch (EI.PageNo)
    {
    case 0:
      {
      if (CurrentBlk(EI))
        {
        }

      if (CurrentBlk(EI))
        {//data
        int pg=EI.PageNo;
        int i=(int)(EI.BlkRowNo-EI.Index);
        switch (EI.FieldId)
          {
          case Id_View:
            Str.LTrim();
            if (Str() && (Str[0]=='1'))
              m_iView = i;
            View().DoRebuild();
            break;
          case Id_TuneRqst:
            Str.LTrim();
            CtrlSequencer::CSBlk(i).SetTuneRqst(Str() && (Str[0]=='1'));
            if (CtrlSequencer::CSBlk(i).TuneRqst())
              CtrlSequencer::CSBlk(i).AutoMan()=true;
            View().DoRebuild();
            break;
          case Id_AutoMan:
            Str.LTrim();
            CtrlSequencer::CSBlk(i).AutoMan()=(Str() && (Str[0]=='1'));
            if (!CtrlSequencer::CSBlk(i).AutoMan())
              CtrlSequencer::CSBlk(i).SetTuneRqst(false);
            View().DoRebuild();
            break;

//          case Id_TuneBusy:
//            Str.LTrim();
//            CtrlSequencer::CSBlk(i).SetTuneBusy(Str() && (Str[0]=='1'));
//            View().DoRebuild();
//            break;
          }
        }
      }
      break;
    case 1:
      {
      if (CurrentBlk(EI))
        {
        switch (EI.FieldId)
          {
          case Id_AutoTune:
            Str.LTrim();
            CtrlSeq().SetSequencerActive(Str() && (Str[0]=='1'));
            View().DoRebuild();
            break;
          }
        }
      }
      break;
    }
  return Fix;
  }

//---------------------------------------------------------------------------

flag CtrlObjectEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag CtrlObjectEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag CtrlObjectEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag CtrlObjectEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag CtrlObjectEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView &Vw=View();
  flag ret=false;
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      ret=true;
      if (EI.Fld)
        {
        flag TagOnly=false;

        }
      }
    }
  return ret;
  }

//---------------------------------------------------------------------------

flag CtrlObjectEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag CtrlObjectEdt::DoAccCnv(UINT Id) 
  {
  View().DoRebuild();
  return true;
  };

//---------------------------------------------------------------------------

flag CtrlObjectEdt::DoAccFmt(UINT Id) 
  {
  return true;
  };

//---------------------------------------------------------------------------

flag CtrlObjectEdt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  };

#endif
#endif

//===========================================================================
//
//                                  OrderObject
//
//===========================================================================

#if WITHORDEROBJ

IMPLEMENT_TAGOBJEDT(OrderObject, "FS_Order", "FS_Order", "", "Order", TOC_SYSTEM,
                    OrderObjectEdt, "The Order", "The Order");

OrderObject::OrderObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CTNode(pClass_, TagIn, pAttach, eAttach)
  {
  bDoSystemDefn=false;
  };

OrderObject::~OrderObject()
  {
  }

//--------------------------------------------------------------------------

XID xidOrderTag     = ModelXID(1000);
XID xidOrderInfo    = ModelXID(1001);

void OrderObject::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag OrderObject::DataXchg(DataChangeBlk & DCB)
  {
  if (CTNode::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidOrderTag:
      DCB.pC=((FlwNode*)DCB.dwUserInfo)->Tag();
      return 1;
    case xidOrderInfo:
      DCB.pC=((FlwNode*)DCB.dwUserInfo)->m_sEvalOrder();
      return 1;
    }
  return 0;
  }

//--------------------------------------------------------------------------

flag OrderObject::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=CTNode::ValidateData(VDB);
  return OK;
  }

//===========================================================================
//
//
//
//===========================================================================

//------------------------------------------------------------------------------

OrderObjectEdt::OrderObjectEdt(pFxdEdtView pView_, pOrderObject pOrdO_) :
  FxdEdtBookRef(pView_),
  rOrdO(*pOrdO_)
  {
  iPg1=0;
  }

//---------------------------------------------------------------------------

OrderObjectEdt::~OrderObjectEdt()
  {
  }

//---------------------------------------------------------------------------

int OrderObjectEdt::DistFromPgNo(int Pg)
  {
  return -1;
  }

//---------------------------------------------------------------------------

void OrderObjectEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void OrderObjectEdt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void OrderObjectEdt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void OrderObjectEdt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void OrderObjectEdt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

void OrderObjectEdt::Build()
  {
  Strng S;
//  int L, i;

  iPg1=pView->Pages;

  StartPage("Order");
  StartBlk(20, 0, NULL);
  for (int i=0; i<20; i++)
    SetSpace(i, 50);
  }

//---------------------------------------------------------------------------

void OrderObjectEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (CurrentBlk(EI))
    {//data
    int pg=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    }
  }

//---------------------------------------------------------------------------

long OrderObjectEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; 
  if (CurrentBlk(EI))
    {//data
    }

  return Fix;
  }

//---------------------------------------------------------------------------

static void PushScaling(rGDIBlk GB, RECT &GraphR, MappingSave &MapSv)
  {
  SetMapMode(GB.DC().m_hDC, MM_ANISOTROPIC);
  SetViewportOrgEx(GB.DC().m_hDC, GraphR.left, GraphR.top, &MapSv.VOrg);
  SetViewportExtEx(GB.DC().m_hDC, GraphR.right-GraphR.left, GraphR.bottom-GraphR.top, &MapSv.VExt);
  SetWindowOrgEx(GB.DC().m_hDC, GraphR.left, GraphR.top, &MapSv.WOrg);
  SetWindowExtEx(GB.DC().m_hDC, GraphR.right-GraphR.left, GraphR.bottom-GraphR.top, &MapSv.WExt);
//  SetWindowOrgEx(GB.DC().m_hDC, 0, C2DFnEdt::YPix, &MapSv.WOrg);
//  SetWindowExtEx(GB.DC().m_hDC, C2DFnEdt::XPix, -C2DFnEdt::YPix, &MapSv.WExt);
  }

//---------------------------------------------------------------------------

static void PopScaling(rGDIBlk GB, MappingSave &MapSv)
  {
  SetMapMode(GB.DC().m_hDC, MM_TEXT);
  SetViewportOrgEx(GB.DC().m_hDC, MapSv.VOrg.x,  MapSv.VOrg.y, NULL);
  SetViewportExtEx(GB.DC().m_hDC, MapSv.VExt.cx, MapSv.VExt.cy, NULL);
  SetWindowOrgEx(GB.DC().m_hDC, MapSv.WOrg.x,  MapSv.WOrg.y, NULL);
  SetWindowExtEx(GB.DC().m_hDC, MapSv.WExt.cx, MapSv.WExt.cy, NULL);
  }

//---------------------------------------------------------------------------

void OrderObjectEdt::OnOverDraw(rGDIBlk GB,int PgNo, CRgn &ClipRgn)
  {
  
//  if (PgNo-iPg1>=0)
    {

    CDC &DC = GB.DC();
    CDCResChk ResChk(DC);

    CRect GraphR;
    int CW = GB.ColWdt();
    int RH = GB.RowHgt();
    GraphR = GB.LuDataRect();
    //GraphR.top -= 1;
    //GraphR.right = GraphR.left+GrfCols*CW;
    //GraphR.bottom = GraphR.top+GrfRows*RH;
    //
    //GraphR.top+=View().CPg->FixedRows*RH;
    //GraphR.bottom+=View().CPg->FixedRows*RH;

    //SetPosition(GB); //calulate size of GraphR
//    flag DoGraph = DC.RectVisible(&GraphR);

    CGdiObject* OldBrush = (CGdiObject*)DC.SelectObject(GB.pBrushGrfBack);

//    if (DoGraph)
//      {//draw the curve...
      int Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgn.m_hObject, RGN_AND);

      COLORREF OldBkColor = DC.SetBkColor(GB.crGrfBack);
      CPen* OldPen = DC.SelectObject(GB.pPenTxtBord);
    
//      DC.DrawEdge(&GraphR, EDGE_SUNKEN, BF_RECT);
//      int EdgeX=GetSystemMetrics(SM_CXEDGE);
//      int EdgeY=GetSystemMetrics(SM_CYEDGE);
//      GraphR.InflateRect(-EdgeX*2, -EdgeY*2);
//
//      DC.Rectangle(&GraphR);
      MappingSave MapSv;
      PushScaling(GB, GraphR, MapSv);
//
      CRgn ClipRgnGrf;
      ClipRgnGrf.CreateRectRgnIndirect(&GraphR);
      Err = ExtSelectClipRgn(DC.m_hDC, (HRGN)ClipRgnGrf.m_hObject, RGN_AND);
//
      CPoint Org = DC.GetWindowOrg();
      CSize Ext = DC.GetWindowExt();
//
      DC.MoveTo(GraphR.left,GraphR.top);
      DC.LineTo(GraphR.right, GraphR.bottom);


//      SetRanges();
//      double XDiff = XDispMax()-XDispMin();
//      XMin = XDispMin()-0.05*XDiff;
//      XMax = XDispMax()+0.05*XDiff;
//      //XMin = XDispMin();
//      //XMax = XDispMax();
//      const double dX = GTZ(XMax-XMin);
//      XFactor = XPix/dX;
//
//
//      for (int c=0; c<3; c++)
//        {
//        double YDiff = YDispMax(c)-YDispMin(c);
//        YMin[c] = YDispMin(c)-0.05*YDiff;
//        YMax[c] = YDispMax(c)+0.05*YDiff;
//        //YMin[c] = YDispMin(c);
//        //YMax[c] = YDispMax(c);
//        const double dY = GTZ(YMax[c]-YMin[c]);
//        YFactor[c] = YPix/dY;
//        }
//  
//      //draw axis...
//      ::SelectObject(DC.m_hDC, GB.hLinePenDim[0]);
//      int x1,y1;
//      x1 = (int)Range((double)-INT_MAX, (0.0-XMin)*XFactor, (double)INT_MAX);
//      //x1 = (int)Range((double)-INT_MAX, (rSD.GetNormX(0.0)-XMin)*XFactor, (double)INT_MAX);
//      DC.MoveTo(x1,Org.y);
//      DC.LineTo(x1,Org.y+Ext.cy);
//      for (c=0; c<1; c++)
//        { // Fix This ??? Make Axes Coincide
//        y1 = (int)Range((double)-INT_MAX, (0.0-YMin[c])*YFactor[c], (double)INT_MAX);
//        //y1 = (int)Range((double)-INT_MAX, (rSD.GetNormY(0.0)-YMin)*YFactor, (double)INT_MAX);
//        DC.MoveTo(Org.x,y1);
//        DC.LineTo(Org.x+Ext.cx,y1);
//        }
//
//      const FirstPen=3;
//      
//      int NXs=50;
//      //draw the curves...
//      int iPen=FirstPen;
//      int XYLen=0;//Xs.GetSize();
//      int NDS=pPDB->NDataSets();
//
//      double LoVolFlw=Min(pPDB->LoVolFlw(0.0), pPDB->LoVolFlw(1.0));
//      double HiVolFlw=Max(pPDB->HiVolFlw(0.0), pPDB->HiVolFlw(1.0));
//
//      for (c=0; c<NDS; c++)
//        {
//        PumpDataSet &CDS=pPDB->DataSet(c);
//        POINT XY[CrvPts];
//        for (int i=0; i<2; i++)
//          {
//          double Reg;
//          if (i==0)
//            {
//            Reg=pPDB->DataSet(c).RegMin;
//            ::SelectObject(DC.m_hDC, GB.hLinePen[iPen]);
//            }
//          else
//            {
//            Reg=0.5*(CDS.RegMin+CDS.RegMax);
//            ::SelectObject(DC.m_hDC, GB.hLinePen[iPen+1]);
//            }
//
//          XYLen=0;
//          for (int j=0; j<=NXs; j++)
//            {
//            double Qv=XMin+(j*(XMax-XMin))/NXs;
//            double Y=pPDB->HeadAt(Qv, Reg);
//            int x=CalcXPix(Qv);
//            int y=CalcYPix(0, Y);
//            XY[XYLen].x = x;
//            XY[XYLen].y = y;
//            XYLen++;
//            }
//          DC.Polyline(XY, XYLen);
//          }
//        }
//      
//      // Restore State
//      PopScaling(GB, MapSv);
      DC.SelectObject(OldPen);
      DC.SetBkColor(OldBkColor);
//      }
      
    DC.SelectObject(OldBrush);
    }

  };

//---------------------------------------------------------------------------

long OrderObjectEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph

  if (CurrentBlk(EI))
    {//data
    }
  return Fix;
  }

//---------------------------------------------------------------------------

flag OrderObjectEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag OrderObjectEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag OrderObjectEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag OrderObjectEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag OrderObjectEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView &Vw=View();
  flag ret=false;
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      }
    }
  return ret;
  }

//---------------------------------------------------------------------------

flag OrderObjectEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

#endif

//===========================================================================
//
//                                  PwrObject
//
//===========================================================================

XID xidNSupplies  = ModelXID(1000);
XID xidPwrTag     = ModelXID(1001);
XID xidPwrOn      = ModelXID(1003);

IMPLEMENT_TAGOBJEDT(PwrObject, "FS_Pwr", "FS_Pwr", "", "Pwr", TOC_SYSTEM, 
                    PwrObjectEdt, "Power Supplies", "The Power/Air Supply systems");

PwrObject::PwrObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CTNode(pClass_, TagIn, pAttach, eAttach)
  {
  bDoSystemDefn=false;
  }
PwrObject::~PwrObject()
  {
  }

//--------------------------------------------------------------------------


void PwrObject::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.Long("NSupplies",  "",   DC_,   "",  xidNSupplies, this, isParmStopped);
    if (DDB.ForFileSnpScn())
      {
      if (DDB.BeginArray(this, "Supply", "FS_PwrSupply", gs_PwrSupplies.NSupplies()))
        {
        for (int j=0; j<gs_PwrSupplies.NSupplies(); j++)
          {
          int i=j;
          //if (DDB.DoingPutData())
          //  {
          //  i=FindSupply(Tag);
          //  }
          if (DDB.BeginElement(this, i, NULL, i))
            {

            
            DDB.String("Name",   "",  DC_,    "",   xidPwrTag,    this, isParm);
            DDB.Bool("On",       "",  DC_,    "",   xidPwrOn,   this, isParm);
            //DDB.Long("Type",     "",  DC_,    "",   (long*)&gs_PwrSupplies[i].m_Type, this, isParm);
            //if (gs_PwrSupplies[i].m_Type==CPS_Elec)
            //  {
            //  //DDB.Float("Voltage",   "",  DC_V,   "V",  &gs_PwrSupplies[i].m_dVoltage,  this, isParm);
            //  //DDB.Long("Phases",   "",  DC_,    "",   &gs_PwrSupplies[i].m_nPhases, this, isParm);
            //  //DDB.Float("Current", "",  DC_I,   "A",  &gs_PwrSupplies[i].m_dCurrent,   this, isParm);
            //  //DDB.Float("Power",   "",  DC_Pwr, "kW", &gs_PwrSupplies[i].m_dPower,  this, isParm);
            //  }
            }
          }
        }
      DDB.EndArray();
      }
    else
      {
      Strng Tg;
      for (int i=0; i<gs_PwrSupplies.NSupplies(); i++)
        {
        DDB.PushUserInfo(i);
        Tg=gs_PwrSupplies.GetTag(i);
        Tg+=".On";
        DDB.Bool(Tg(), "",   DC_,   "",  xidPwrOn,  this, isParm);
        DDB.PopUserInfo();
        //Tg=gs_PwrSupplies.GetTag(i);
        //Tg+=".Type";
        //DDB.Long(Tg(), "",   DC_,   "",  (long*)&gs_PwrSupplies[i].m_Type,  this, isParm);
        
        //Tg=gs_PwrSupplies.GetTag(i);
        //Tg+=".Voltage";
        //DDB.Float(Tg(), "",   DC_,   "",  &gs_PwrSupplies[i].m_dVoltage,  this, isParm);
        //Tg=gs_PwrSupplies.GetTag(i);
        //Tg+=".Phases";
        //DDB.Long(Tg(), "",   DC_,   "",  &gs_PwrSupplies[i].m_nPhases,  this, isParm);
        //Tg=gs_PwrSupplies.GetTag(i);
        //Tg+=".Current";
        //DDB.Float(Tg(), "",   DC_,   "",  &gs_PwrSupplies[i].m_dCurrent,  this, isParm);
        //Tg=gs_PwrSupplies.GetTag(i);
        //Tg+=".Power";
        //DDB.Float(Tg(), "",   DC_,   "",  &gs_PwrSupplies[i].m_dPower,  this, isParm);
        }
      }
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag PwrObject::DataXchg(DataChangeBlk & DCB)
  {
  if (CTNode::DataXchg(DCB))
    return true;

  switch (DCB.lHandle)
    {
    case xidNSupplies: 
      {
      if (DCB.rL)
        gs_PwrSupplies.SetNSupplies(*DCB.rL); 
      DCB.L=gs_PwrSupplies.NSupplies(); 
      return true;
      }
    case xidPwrTag:
      {
      if (DCB.rpC && strlen(DCB.rpC)>0)
        gs_PwrSupplies.SetTag(DCB.dwUserInfo, DCB.rpC);
      DCB.pC=(char*)gs_PwrSupplies.GetTag(DCB.dwUserInfo);
      return 1;
      }
    case xidPwrOn:
      {
      if (DCB.rB)
        gs_PwrSupplies[DCB.dwUserInfo].SetOn(*DCB.rB!=0);
      DCB.B=gs_PwrSupplies[DCB.dwUserInfo].On();
      return 1;
      }
//    case xidPwrType:
//      {
//      if (DCB.rL)
//        gs_PwrSupplies[DCB.dwUserInfo].m_Type=CPwrTypes(*DCB.rL);
//      DCB.L=gs_PwrSupplies[DCB.dwUserInfo].m_Type;
//      return 1;
//      }
    }
  return false;
  }

//--------------------------------------------------------------------------

flag PwrObject::ValidateData(ValidateDataBlk & VDB)
  {

  flag OK=CTNode::ValidateData(VDB);
  return OK;
  }


//===========================================================================
//
//
//
//===========================================================================

const int Id_Number    = 1;
const int Id_PwrName   = 2;
const int Id_PwrState  = 3;
const int Id_PwrType   = 4;
const int Id_PwrVolts  = 5;
const int Id_PwrPhases = 6;
const int Id_PwrAmps   = 7;
const int Id_PwrPower  = 8;

PwrObjectEdt::PwrObjectEdt(pFxdEdtView pView_, pPwrObject pPwrO_) :
  FxdEdtBookRef(pView_),
  rPwrO(*pPwrO_)
  {
  VoltCnv.Set   (DC_V,    "V");
  AmpCnv.Set    (DC_I,    "A");
  PwrCnv.Set    (DC_Pwr,    "kW");
  VoltFmt.Set   ("", 0, 2, 'f');
  AmpFmt.Set    ("", 0, 2, 'f');
  PwrFmt.Set    ("", 0, 2, 'f');
  iPg1=0;
  iNameWidth=12;
  int j=0;

  CProfINIFile PF(PrjIniFile());

  ObjectAttribute *pAttr=ObjAttributes.FindObject("PwrObjectEdt");
  if (pAttr)
    {
    pAttr->FieldFmtCnvs("Voltage",  VoltFmt, VoltCnv);
    pAttr->FieldFmtCnvs("Current",  AmpFmt,  AmpCnv);
    pAttr->FieldFmtCnvs("Power",    PwrFmt,  PwrCnv);
    }
  }

//---------------------------------------------------------------------------

PwrObjectEdt::~PwrObjectEdt()
  {
  ObjectAttribute *pAttr=ObjAttributes.FindObject("PwrObjectEdt");
  if (pAttr)
    {
    pAttr->SetFieldFmt  ("Voltage",   VoltFmt);
    pAttr->SetFieldCnvs ("Voltage",   VoltCnv);
    pAttr->SetFieldFmt  ("Current",   AmpFmt);
    pAttr->SetFieldCnvs ("Current",   AmpCnv);
    pAttr->SetFieldFmt  ("Power",   PwrFmt);
    pAttr->SetFieldCnvs ("Power",   PwrCnv);
    }

  }

//---------------------------------------------------------------------------

int PwrObjectEdt::DistFromPgNo(int Pg)
  {
  return -1;
  }

//---------------------------------------------------------------------------

void PwrObjectEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void PwrObjectEdt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void PwrObjectEdt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void PwrObjectEdt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void PwrObjectEdt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

void PwrObjectEdt::Build()
  {
  Strng S;

  iPg1=pView->Pages;

  StartPage("Power Supplies");

//  CPwrSupplies & PB=*CPwrSupplies::GetBlk();
  if (1)
    {
    StartBlk(5, 0, NULL);                                               
    int L=0;

    SetSpace(L,8);
    L++;
    SetDParm(L,"Number", 12, "", Id_Number, 3, 2, " ");
    L++;
    L++;
    SetDesc(L, "Name",    -1, 20,  0, " ");
    SetDesc(L, "On",      -1, 2,  0, " ");
    SetDesc(L, "Type",    -1, 10,  0, " ");
    SetDesc(L, "Voltage", -1, 10,  2, " ");
    SetDesc(L, "3Ph",     -1, 3,  0, " ");
    SetDesc(L, "Current", -1, 10,  2, " ");
    SetDesc(L, "Power",   -1, 10,  2, " ");
    L++;
    SetDesc(L, "",              -1, 20,  0, " ");
    SetDesc(L, "",              -1, 2,  0, " ");
    SetDesc(L, "",              -1, 10,  0, " ");
    SetDesc(L, VoltCnv.Text(),  -1, 10,  2, " ");
    SetDesc(L, "",     -1, 3,    0, " ");
    SetDesc(L, AmpCnv.Text(),   -1, 10,  2, " ");
    SetDesc(L, PwrCnv.Text(),   -1, 10,  2, " ");
    }

  if (1) // Base Blk
    {
    Strng Tg;

    int L=0;
    StartBlk(gs_PwrSupplies.NSupplies(), 0, NULL);
    
    for (int i=0; i<gs_PwrSupplies.NSupplies(); i++)
      {
      Tg.Set("$Pwr.%s.On", gs_PwrSupplies.GetTag(i));
      //SetDesc(L, (char*)gs_PwrSupplies.GetTag(i),  -1, iNameWidth,  0, "");
      SetParm(L, "", Id_PwrName,  20, 0, " ");   // MoleWt
      SetTag(Tg());
      SetCheckBoxBtn(L, gs_PwrSupplies[i].On() ? "1" : "0", Id_PwrState, 2, 2, " ", true);
      SetTag(Tg());
      SetParm(L, "", Id_PwrType,    10, 0, " ");
      //if (gs_PwrSupplies[i].m_Type==CPS_Elec)
      //  {
      //  //SetParm(L, "", Id_PwrVolts,   10, 2, " ");
      //  //SetCheckBoxBtn(L, gs_PwrSupplies[i].m_nPhases==3 ? "1" : "0", Id_PwrPhases, 2, 2, "  ", true);
      //  //SetParm(L, "", Id_PwrAmps,    10, 2, " ");
      //  //SetParm(L, "", Id_PwrPower,   10, 2, " ");
      //  }
      L++;
      }
    }
  }

//---------------------------------------------------------------------------

void PwrObjectEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {

  if (CurrentBlk(EI))
    {//data
//    CPwrSupplies & PB=*CPwrSupplies::GetBlk();
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_Number:  
        Str.Set("%i", gs_PwrSupplies.NSupplies());
        break;
      }
    }

  if (CurrentBlk(EI))
    {//other
//    CPwrSupplies & PB=*CPwrSupplies::GetBlk();
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    //double D;
    switch (EI.FieldId)
      {
      case Id_PwrName:
        Str=gs_PwrSupplies.GetTag(i);
        break;
      case Id_PwrState:
        Str=gs_PwrSupplies[i].On() ? "1":"0";
        break;
      case Id_PwrType:
        switch (gs_PwrSupplies[i].Type())
          {
          case CPS_Elec: Str="Electric"; break;
          case CPS_Air:  Str="Air";      break;
          default:       Str="???";      break;
          }
        break;
      //case Id_PwrVolts:
      //  VoltFmt.FormatFloat(VoltCnv.Human(gs_PwrSupplies[i].m_dVoltage), Str);
      //  break;
      //case Id_PwrPhases:
      //  Str.Set("%i", gs_PwrSupplies[i].m_nPhases);
      //  break;
      //case Id_PwrAmps:
      //  AmpFmt.FormatFloat(AmpCnv.Human(gs_PwrSupplies[i].m_dCurrent), Str);
      //  EI.Fld->fEditable=false;
      //  break;
      //case Id_PwrPower:
      //  PwrFmt.FormatFloat(PwrCnv.Human(gs_PwrSupplies[i].m_dPower), Str);
      //  EI.Fld->fEditable=false;
      //  break;
      //EI.Fld->fEditable=false;
      }
    }
  }

//---------------------------------------------------------------------------

long PwrObjectEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; 
  if (CurrentBlk(EI))
    {//data
//    CPwrSupplies & PB=*CPwrSupplies::GetBlk();
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_Number:  
        Str.LRTrim();
        gs_PwrSupplies.SetNSupplies(Range(1L, SafeAtoL(Str,gs_PwrSupplies.NSupplies()), 40L));
        break;
      }
    }

  if (CurrentBlk(EI))
    {//other
//    CPwrSupplies & PB=*CPwrSupplies::GetBlk();
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_PwrName:
        Str.LRTrim();
        gs_PwrSupplies.SetTag(i, Str());
        break;
      case Id_PwrState:
        Str.LTrim();
        gs_PwrSupplies[i].SetOn((Str() && (Str[0]=='1')));
        View().DoRebuild();
        break;
//      case Id_PwrType:
//        switch (gs_PwrSupplies[i].m_Type)
//          {
//          case CPS_Electric: Str="Electric"; break;
//          case CPS_CompAir:  Str="Electric"; break;
//          default:           Str="???";      break;
//          }
//        break;
//      case Id_PwrVolts:
//        gs_PwrSupplies[i].m_dVoltage=VoltCnv.Normal(SafeAtoF(Str, gs_PwrSupplies[i].m_dVoltage));
//        break;
//      case Id_PwrPhases:
//       // Str.Set("%i", gs_PwrSupplies[i].m_nPhases);
//        break;
//      case Id_PwrAmps:
//        gs_PwrSupplies[i].m_dCurrent=AmpCnv.Normal(SafeAtoF(Str, gs_PwrSupplies[i].m_dCurrent));
//        break;
//      case Id_PwrPower:
//        gs_PwrSupplies[i].m_dPower=PwrCnv.Normal(SafeAtoF(Str, gs_PwrSupplies[i].m_dPower));
//        break;
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

long PwrObjectEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph

  if (CurrentBlk(EI))
    {//data
    }

  if (CurrentBlk(EI))
    {//data
//    CPwrSupplies & PB=*CPwrSupplies::GetBlk();
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_PwrState:
        gs_PwrSupplies[i].SetOn(!gs_PwrSupplies[i].On());
        View().DoRebuild();
        break;
      }
    }
  
  return Fix;
  }

//---------------------------------------------------------------------------

flag PwrObjectEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag PwrObjectEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag PwrObjectEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag PwrObjectEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag PwrObjectEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView &Vw=View();
  flag ret=false;
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      ret=true;
      if (EI.Fld)
        {
        flag TagOnly=false;
        int iId=GetID(EI.FieldId);
        switch (iId)
          {
          case Id_PwrName:
          case Id_PwrState:
            WrkIB.Set(EI.Fld->Tag, NULL, NULL);
            TagOnly=true;
            break;
          }

        if (WrkIB.FmtOK() || TagOnly)
          {
          CRect WRect;
          Vw.GetWindowRect(&WRect);
          CPoint  RBPoint;
          RBPoint.x = WRect.left+point.x;
          RBPoint.y = WRect.top+point.y;

          CMenu Menu;
          Menu.CreatePopupMenu();
          CMenu FormatMenu;
          FormatMenu.CreatePopupMenu();
          if (WrkIB.FmtOK())
            WrkIB.Fmt().AddToMenu(FormatMenu);//, "");
          CMenu CnvMenu;
          CnvMenu.CreatePopupMenu();
          if (WrkIB.CnvOK())
            WrkIB.Cnv().AddToMenu(CnvMenu);//, d.iCnv, d.sCnvTxt());

          Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
          if (!WrkIB.CnvOK() || (WrkIB.CnvOK() && (WrkIB.Cnv().Index())<=0) || TagOnly)
            Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);

          Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");
          if (!WrkIB.FmtOK() || TagOnly)
            Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);

          Menu.AppendMenu(MF_SEPARATOR);
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyTag, "Copy Full &Tag");
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyRHSTag, "Copy Tag");
          if (EI.Fld->Tag==NULL)
            {
            Menu.EnableMenuItem(pView->iCmdCopyTag, MF_BYCOMMAND|MF_GRAYED);
            Menu.EnableMenuItem(pView->iCmdCopyRHSTag, MF_BYCOMMAND|MF_GRAYED);
            }
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyVal, "Copy &Value");

          Menu.AppendMenu(MF_STRING, ID_EDIT_COPY, "Copy");
          Menu.AppendMenu(MF_STRING, ID_EDIT_PASTE, "Paste");
          
          Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, &View());
          Menu.DestroyMenu();                                           
          }

        }
      }
    }

  return ret;
  }

//---------------------------------------------------------------------------

flag PwrObjectEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag PwrObjectEdt::DoAccCnv(UINT Id) 
  {
  View().DoRebuild();
  return true;
  };

//---------------------------------------------------------------------------

flag PwrObjectEdt::DoAccFmt(UINT Id) 
  {
  return true;
  };

//---------------------------------------------------------------------------

flag PwrObjectEdt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  };

//===========================================================================
//
//                                  IOObject
//
//===========================================================================

XID xidNIOTags    = ModelXID(1000);
XID xidIOTag      = ModelXID(1001);
XID xidIOValueS   = ModelXID(1002);
XID xidIOValueT   = ModelXID(1003);
XID xidIOInput    = ModelXID(1004);
XID xidIOType     = ModelXID(1005);
XID xidIORmtCfg   = ModelXID(1006);
XID xidIODesc     = ModelXID(1007);

IMPLEMENT_TAGOBJEDT(IOObject, "FS_IO", "FS_IO", "", "IO", TOC_SYSTEM, 
                    IOObjectEdt, "Discrete IO's", "Discrete IO's");

IOObject::IOObject(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CTNode(pClass_, TagIn, pAttach, eAttach)
  {
  bDoSystemDefn=false;
  m_IO.SetSize(0,32);
  }

IOObject::~IOObject()
  {
  }

//--------------------------------------------------------------------------


void IOObject::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this))
    {
    DDB.Long("NIOs",  "",   DC_,   "",  xidNIOTags, this, isParmStopped);
    if (NoIOs()>0)
      {
      if (DDB.ForView () || DDB.ForFileSnpScn())
        {
        if (DDB.BeginArray(this, "IO", "FS_IOPoints", NoIOs()))
          {
          for (int i=0; i<NoIOs(); i++)
            {
            if (DDB.BeginElement(this, i, NULL, i))
              {
              DDB.String("Name",    "",   DC_,   "",  xidIOTag,    this, isParm);
              DDB.Bool("Input",     "",   DC_,   "",  xidIOInput,  this, isParm);
              DDB.Byte("Type",      "",   DC_,   "",  xidIOType,   this, isParm);
              DDB.String("Value",   "",   DC_,   "",  xidIOValueS, this, isParm);
              DDB.String("Desc",    "",   DC_,   "",  xidIODesc,   this, m_IO[i].m_bRmtCfg?0:isParm);
              if (DDB.ForView ())
                {
                DDB.Bool("RmtCfg",  "",   DC_,   "",  xidIORmtCfg, this, isParm);
                }
              }
            }
          }
        DDB.EndArray();
        }
      else
        {
        //Strng Tg;
        LPCTSTR Tg;
        for (int i=0; i<NoIOs(); i++)
          {
          Tg=GetIOTag(i);
          if (Tg)
            {
            //Tg+=".On";
            DDB.PushUserInfo(i);
            if (IsIntData(GetIOType(i)))
              DDB.Long((LPTSTR)Tg, "",   DC_,   "",  xidIOValueT,  this, isParm);
            else
              DDB.Double((LPTSTR)Tg, "",   DC_,   "",  xidIOValueT,  this, isParm);
            DDB.PopUserInfo();
            }
          }
        }
      }
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag IOObject::DataXchg(DataChangeBlk & DCB)
  {
  if (CTNode::DataXchg(DCB))
    return true;

  switch (DCB.lHandle)
    {
    case xidNIOTags: 
      {
      if (DCB.rL)
        SetNoIOs(*DCB.rL); 
      DCB.L=NoIOs();
      return true;
      }
    case xidIOTag:
      {
      if (DCB.rpC)
        SetIOTag(DCB.dwUserInfo, DCB.rpC);
      DCB.pC=(LPTSTR)GetIOTag(DCB.dwUserInfo);
      return 1;
      }
    case xidIOInput:
      {
      if (DCB.rB)
        m_IO[DCB.dwUserInfo].m_bInput=*DCB.rB!=0;
      DCB.B=m_IO[DCB.dwUserInfo].m_bInput;
      return 1;
      }
    case xidIOType:
      {
      if (DCB.rB)
        IOValue(DCB.dwUserInfo).iType=*DCB.rB;
      DCB.B=IOValue(DCB.dwUserInfo).iType;
      return 1;
      }
    case xidIORmtCfg:
      {
      if (DCB.rB)
        m_IO[DCB.dwUserInfo].m_bRmtCfg=*DCB.rB!=0;
      DCB.B=m_IO[DCB.dwUserInfo].m_bRmtCfg;
      return 1;
      }
    case xidIODesc:
      {
      if (DCB.rpC)
        SetIODesc(DCB.dwUserInfo, DCB.rpC);
      DCB.pC=(LPTSTR)GetIODesc(DCB.dwUserInfo);
      return 1;
      }
    case xidIOValueS:
      {
      if (DCB.rpC)
        IOValue(DCB.dwUserInfo).SetTypeString(GetIOType(DCB.dwUserInfo), DCB.rpC);
      DCB.pC=IOValue(DCB.dwUserInfo).GetString("%i", "%.12g", NULL);
      return 1;
      }
    case xidIOValueT:
      {
      if (IsIntData(GetIOType(DCB.dwUserInfo)))
        {
        if (DCB.rL)
          IOValue(DCB.dwUserInfo).PutLong(*DCB.rL);
        DCB.L=IOValue(DCB.dwUserInfo).GetLong();
        }
      else
        {
        if (DCB.rD)
          IOValue(DCB.dwUserInfo).PutDouble(*DCB.rD);
        DCB.D=IOValue(DCB.dwUserInfo).GetDouble();
        }
      return 1;
      }
    }
  return false;
  }

//--------------------------------------------------------------------------

flag IOObject::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=CTNode::ValidateData(VDB);

  //for (int i=0; i<m_IO.GetSize(); i++)
  //  {
  //  if (GetIOTag(i)==NULL)
  //    {
  //    Strng Tg;
  //    Tg.Set("#%i", DCB.dwUserInfo);
  //    SetIOTag(DCB.dwUserInfo, Tg());
  //    m_IO[i].m_bBlank=true;
  //    }
  //  }

  return OK;
  }

//--------------------------------------------------------------------------

int IOObject::AddTag(LPCTSTR Tag, bool Input, LPCTSTR Desc, byte Type, LPCTSTR ValueStr, LPCTSTR ValueCnvs)
  {
  int i=FindTag(Tag);
  if (i>=0)
    m_IO[i].m_bRmtCfg=true;
  else
    {
    for (i=0; i<m_IO.GetSize(); i++)
      if (m_IO[i].m_sTag.Length()==0)
        break;
    if (i==m_IO.GetSize())
      m_IO.SetSize(i+1);
    m_IO[i].m_sTag=Tag;
    m_IO[i].m_bInput=Input;
    m_IO[i].m_sDesc=Desc;
    m_IO[i].m_Value.iType=Type;
    m_IO[i].m_bRmtCfg=true;
    if (ValueStr && strlen(ValueStr)>0)
      m_IO[i].m_Value.SetTypeString(Type, (LPTSTR)ValueStr);

    }
  return -1;
  };

//--------------------------------------------------------------------------

int IOObject::RemoveTag(LPCTSTR Tag)
  {
  return -1;
  };

//--------------------------------------------------------------------------

int IOObject::FindTag(LPCTSTR Tag)
  {
  for (int i=0; i<NoIOs(); i++)
    if (m_IO[i].m_sTag.XStrICmp(Tag)==0)
      return i;
  return -1;
  }

//===========================================================================
//
//
//
//===========================================================================

const int Id_IONumber   = 1;
const int Id_IOName     = 2;
const int Id_IOValue    = 3;

IOObjectEdt::IOObjectEdt(pFxdEdtView pView_, pIOObject pIOO_) :
  FxdEdtBookRef(pView_),
  rIOO(*pIOO_)
  {
  iPg1=0;
  iNameWidth=12;
  int j=0;

  CProfINIFile PF(PrjIniFile());

  ObjectAttribute *pAttr=ObjAttributes.FindObject("IOObjectEdt");
  if (pAttr)
    {
    }
  }

//---------------------------------------------------------------------------

IOObjectEdt::~IOObjectEdt()
  {
  ObjectAttribute *pAttr=ObjAttributes.FindObject("IOObjectEdt");
  if (pAttr)
    {
    }

  }

//---------------------------------------------------------------------------

int IOObjectEdt::DistFromPgNo(int Pg)
  {
  return -1;
  }

//---------------------------------------------------------------------------

void IOObjectEdt::PutDataStart()
  {
  }

//---------------------------------------------------------------------------

void IOObjectEdt::PutDataDone()
  {
  }

//---------------------------------------------------------------------------

void IOObjectEdt::GetDataStart()
  {
  }

//---------------------------------------------------------------------------

void IOObjectEdt::GetDataDone()
  {
  }

//---------------------------------------------------------------------------

void IOObjectEdt::StartBuild()
  {
  }

//---------------------------------------------------------------------------

void IOObjectEdt::Build()
  {
  Strng S;

  iPg1=pView->Pages;

  CRect CRect;
  pView->GetClientRect(&CRect);
  iPtsPerPg=Max(25L, long(CRect.Height()/pView->ScrGB.RowHgt())-5L);

  long TagWide=7;
  long ValWide=8;
  for (int iPt=0; iPt<NoIOs(); iPt++)
    {
    if (GetIOTag(iPt))
      TagWide=Max(TagWide, (long)strlen(GetIOTag(iPt)));
    if (rIOO.GetIOType(iPt)==tt_Bool)
      {
      long VW= 4;
      //if ()
      ValWide=Max(ValWide, VW+3L);
      }
    else
      {
      long VW= 10;
      //if ()
      ValWide=Max(ValWide, VW);

      }
    }

  Strng RTg, Tg;
  int iPg=-1;
  int L=0;
  for (iPt=0; iPt<NoIOs(); iPt++)
    {
    if (iPt%iPtsPerPg==0)
      {
      iPg++;
      Strng PgTg;
      PgTg.Set((iPg==0) ?"I/O Points":"(%i)",iPg);
      StartPage(PgTg());

      if (0 && iPg==0)
        {
        StartBlk(4, 0, NULL);
        L=0;

        SetSpace(L,8);
        L++;
        SetDParm(L,"Number", 12, "", Id_Number, 3, 2, " ");
        L++;
        L++;
        SetDesc(L, "Name",  -1, 20,  0, " ");
        SetDesc(L, "Value",    -1, 2,  0, " ");
        }
      
      // Base Blk
      L=0;
      StartBlk(iPtsPerPg, 0, NULL);
      }
       
    SetDesc(L, rIOO.m_IO[iPt].m_bRmtCfg?"*":" ", -1,  1, 0, "");
    if (GetIOTag(iPt))
      Tg=GetIOTag(iPt);
    else
      Tg.Set("#%i", iPt);
    RTg="$IO.";
    RTg+=Tg;
    SetDesc(L, Tg(),  -1, TagWide,  0, " ");
    SetTag(Tg());
    flag Inp=rIOO.m_IO[iPt].m_bInput;
    if (rIOO.GetIOType(iPt)==tt_Bool)
      {
      if (Inp)
        {
        SetCheckBoxBtn(L, 0 ? "1" : "0", Id_IOValue, 2, 2, " ", true);
        SetTag(RTg());
        }
      else
        SetDesc(L, "", -1, Inp?0:2,0, " ");
      SetData(L, "", Id_IOValue,  ValWide-3, 2, " ", false);
      SetTag(RTg());
      }
    else if (Inp)
      {
      SetData(L, "", Id_IOValue,  ValWide, 2, " ", true);
      SetTag(RTg());
      }
    else
      {
      SetData(L, "", Id_IOValue,  ValWide, 2, " ", false);
      SetTag(RTg());
      }

    LPTSTR pDesc=(LPTSTR)GetIODesc(iPt);
    SetDesc(L, pDesc,  -1, Max(20L, (long)(pDesc!=NULL ? strlen(pDesc):0)),  0, "");

    L++;
    }
  }

//---------------------------------------------------------------------------

void IOObjectEdt::Load(FxdEdtInfo &EI, Strng & Str)
  {
  if (0 && EI.PageNo==0 && CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index);
    switch (EI.FieldId)
      {
      case Id_Number:  
        Str.Set("%i", NoIOs());
        break;
      }
    }

  if (CurrentBlk(EI))
    {//other
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index)+p*iPtsPerPg;
    switch (EI.FieldId)
      {
      case Id_IOName:
        Str=GetIOTag(i);
        break;
      case Id_IOValue:
        Str=IOValue(i).GetString();
        break;
      //EI.Fld->fEditable=false;
      }
    }
  }

//---------------------------------------------------------------------------

long IOObjectEdt::Parse(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; 
  if (0 && EI.PageNo==0 && CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index)+p*iPtsPerPg;
    switch (EI.FieldId)
      {
      case Id_Number:  
        Str.LRTrim();
        SetNoIOs(Range(1L, SafeAtoL(Str, NoIOs()), 1000L));
        break;
      }
    }

  if (CurrentBlk(EI))
    {//other
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index)+p*iPtsPerPg;
    switch (EI.FieldId)
      {
      case Id_IOName:
        Str.LRTrim();
        SetIOTag(i, Str());
        break;
      case Id_IOValue:
        Str.LTrim();
        IOValue(i).SetTypeString(GetIOType(i), Str());
        //SetIOState(i, Str() && (Str[0]=='1'));
        //View().DoRebuild();
        break;
      }
    }
  return Fix;
  }

//---------------------------------------------------------------------------

long IOObjectEdt::ButtonPushed(FxdEdtInfo &EI, Strng & Str)
  {
  long Fix=0; //set Fix=1 to redraw graph

  if (0 && EI.PageNo==0 && CurrentBlk(EI))
    {//data
    }

  if (CurrentBlk(EI))
    {//data
    int p=EI.PageNo;
    int i=(int)(EI.BlkRowNo-EI.Index)+p*iPtsPerPg;
    switch (EI.FieldId)
      {
      case Id_IOValue:
        // assume of integral type
        IOValue(i).PutLong(!IOValue(i).GetLong());
        //View().DoRebuild();
        View().DoReload();
        break;
      }
    }
  
  return Fix;
  }

//---------------------------------------------------------------------------

flag IOObjectEdt::DoLButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag IOObjectEdt::DoLButtonUp(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag IOObjectEdt::DoLButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag IOObjectEdt::DoRButtonDown(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag IOObjectEdt::DoRButtonUp(UINT nFlags, CPoint point)
  {
  FxdEdtView &Vw=View();
  flag ret=false;
  if (Vw.CPgNo>=0)
    {
    FxdEdtInfo EI;
    if (Vw.LocateFromCR((int)Vw.ChEditPos.x, (int)Vw.ChEditPos.y, EI))
      {
      ret=true;
      if (EI.Fld)
        {
        flag TagOnly=false;
        int iId=GetID(EI.FieldId);
        switch (iId)
          {
          case Id_IOName:
          case Id_IOValue:
            WrkIB.Set(EI.Fld->Tag, NULL, NULL);
            TagOnly=true;
            break;
          }

        if (WrkIB.FmtOK() || TagOnly)
          {
          CRect WRect;
          Vw.GetWindowRect(&WRect);
          CPoint  RBPoint;
          RBPoint.x = WRect.left+point.x;
          RBPoint.y = WRect.top+point.y;

          CMenu Menu;
          Menu.CreatePopupMenu();
          CMenu FormatMenu;
          FormatMenu.CreatePopupMenu();
          if (WrkIB.FmtOK())
            WrkIB.Fmt().AddToMenu(FormatMenu);//, "");
          CMenu CnvMenu;
          CnvMenu.CreatePopupMenu();
          if (WrkIB.CnvOK())
            WrkIB.Cnv().AddToMenu(CnvMenu);//, d.iCnv, d.sCnvTxt());

          Menu.AppendMenu(MF_POPUP, (unsigned int)CnvMenu.m_hMenu, "&Conversions");
          if (!WrkIB.CnvOK() || (WrkIB.CnvOK() && (WrkIB.Cnv().Index())<=0) || TagOnly)
            Menu.EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);

          Menu.AppendMenu(MF_POPUP, (unsigned int)FormatMenu.m_hMenu, "&Format");
          if (!WrkIB.FmtOK() || TagOnly)
            Menu.EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);

          Menu.AppendMenu(MF_SEPARATOR);
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyTag, "Copy Full &Tag");
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyRHSTag, "Copy Tag");
          if (EI.Fld->Tag==NULL)
            {
            Menu.EnableMenuItem(pView->iCmdCopyTag, MF_BYCOMMAND|MF_GRAYED);
            Menu.EnableMenuItem(pView->iCmdCopyRHSTag, MF_BYCOMMAND|MF_GRAYED);
            }
          Menu.AppendMenu(MF_STRING, pView->iCmdCopyVal, "Copy &Value");

          Menu.AppendMenu(MF_STRING, ID_EDIT_COPY, "Copy");
          Menu.AppendMenu(MF_STRING, ID_EDIT_PASTE, "Paste");
          
          Menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, RBPoint.x, RBPoint.y, &View());
          Menu.DestroyMenu();                                           
          }

        }
      }
    }

  return ret;
  }

//---------------------------------------------------------------------------

flag IOObjectEdt::DoRButtonDblClk(UINT nFlags, CPoint point)
  {
  return 0;
  }

//---------------------------------------------------------------------------

flag IOObjectEdt::DoAccCnv(UINT Id) 
  {
  View().DoRebuild();
  return true;
  };

//---------------------------------------------------------------------------

flag IOObjectEdt::DoAccFmt(UINT Id) 
  {
  return true;
  };

//---------------------------------------------------------------------------

flag IOObjectEdt::DoAccRptTagLists()
  {
  return FxdEdtBookRef::DoAccRptTagLists();
  };

//===========================================================================
//
//
//
//===========================================================================
