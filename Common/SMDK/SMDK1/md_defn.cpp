//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"

#define __MD_DEFN_CPP
//#define LIBCOMMENTHD "..\\"
#include "scdmacros.h"

#define MDLBASE
#define MDLBASE
#define SIZEDST1
//#define BLACKBOX1
#include "models.h"
//#include "md_headers.h"
//#include "models.h"

#include "md_base.h"
#include "md_defn.h"
#include "md_vector.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================

double NAN=dNAN;
double ZeroC     = 273.16;
//double StdP      = 101.325;
//double StdT      = 298.15;
//
//double AtmosP(double Altitude) { return AtmosPress(Altitude); };

//===========================================================================
//
//
//
//===========================================================================

MCnv MC_         ;
MCnv MC_None     ;
MCnv MC_Ang      ;
MCnv MC_DP       ;
MCnv MC_PpS      ;
MCnv MC_PpM      ;
MCnv MC_PpVol    ;
MCnv MC_E        ;
MCnv MC_Frac     ;
MCnv MC_FracRate ;
MCnv MC_Assay    ;
MCnv MC_HCap     ;
MCnv MC_HCapF    ;
MCnv MC_L        ;
MCnv MC_Ldt      ;
MCnv MC_M        ;
MCnv MC_Qm       ;
MCnv MC_QmSlew   ;
MCnv MC_Qv       ;
MCnv MC_QvSlew   ;
MCnv MC_Qe       ;
MCnv MC_NQv      ;
MCnv MC_NVol     ;
MCnv MC_P        ;
MCnv MC_Pwr      ;
MCnv MC_pS       ;
MCnv MC_Rho      ;
MCnv MC_T        ;
MCnv MC_Time     ;
MCnv MC_Vol      ;
MCnv MC_WI       ;
MCnv MC_HMs      ;
MCnv MC_HMl      ;
MCnv MC_HV       ;
MCnv MC_HNV      ;
MCnv MC_SMs      ;
MCnv MC_SMl      ;
MCnv MC_CpMs     ;
MCnv MC_CpMl     ;
MCnv MC_Conc     ;
MCnv MC_Area     ;
MCnv MC_HTC      ;
MCnv MC_HCond    ;
MCnv MC_UA       ;
MCnv MC_dT       ;
MCnv MC_dTdt     ;
MCnv MC_I        ;
MCnv MC_V        ;
MCnv MC_Epm      ;
MCnv MC_Visc     ;
MCnv MC_MpL      ;
MCnv MC_KgMl     ;
MCnv MC_QKgMl    ;
MCnv MC_Cond     ;
MCnv MC_SpCnt    ;
MCnv MC_Cnt      ;
MCnv MC_CntRate  ;
MCnv MC_SurfTens ;
MCnv MC_EntRate  ;
MCnv MC_DPperVol ;
MCnv MC_SurfAreaM;
MCnv MC_SurfAreaL;
MCnv MC_FracSol  ;
MCnv MC_FracLiq  ;
MCnv MC_FracVap  ;
MCnv MC_FracPh00 ;
MCnv MC_FracPh01 ;
MCnv MC_FracPh02 ;
MCnv MC_FracPh03 ;
MCnv MC_FracPh04 ;
MCnv MC_FracPh05 ;
MCnv MC_FracPh06 ;
MCnv MC_FracPh07 ;
MCnv MC_FracPh08 ;
MCnv MC_FracPh09 ;
MCnv MC_FracPh10 ;
MCnv MC_FracPh11 ;
MCnv MC_FracPh12 ;
MCnv MC_FracPh13 ;
MCnv MC_FracPh14 ;
MCnv MC_FracPh15 ;
MCnv MC_FracPh16 ;
MCnv MC_FracPh17 ;
MCnv MC_FracPh18 ;
MCnv MC_FracPh19 ;
MCnv MC_FracPh20 ;
MCnv MC_FracPh21 ;
MCnv MC_FracPh22 ;
MCnv MC_FracPh23 ;
MCnv MC_FracPh24 ;
MCnv MC_FracPh25 ;
MCnv MC_FracPh26 ;
MCnv MC_FracPh27 ;
MCnv MC_FracPh28 ;
MCnv MC_FracPh29 ;
MCnv MC_FracPh30 ;
MCnv MC_FracPh31 ;
MCnv MC_ThermTau ;
MCnv MC_CosPhi   ;
MCnv MC_Ohms     ;
MCnv MC_Bytes    ;
MCnv MC_ConcRate ;
MCnv MC_KinVisc  ;
MCnv MC_ElectVA  ;
MCnv MC_Humidity ;
MCnv MC_Money    ;
MCnv MC_MoneyFlow;

//===========================================================================
//
//
//
//===========================================================================

MCnvs  gs_Cnvs;

MCnvs::MCnvs()
  {
  };
int MCnvs::FindPrimary(LPCTSTR Name)
  {
  return gs_CnvsMngr.CnvCnt();
  }
int MCnvs::Count()
  {
  return gs_CnvsMngr.CnvCnt();
  }
bool MCnvs::Create(LPCTSTR NameCnv, MCnv & Cnv)
  {
  CString Nm(NameCnv);
  Nm=Nm.Trim();
  CString Txt(Nm);

  if (Nm.Left(4)=="Frac")
    { int xxx=0; }

  int iParen=Nm.Find("(");
  if (iParen>0)
    {
    Nm=Nm.Left(iParen);
    Txt=Txt.Right(Txt.GetLength()-iParen);
    Cnv.m_Index=gs_CnvsMngr.FindCnv((LPTSTR)(LPCTSTR)Nm);
    Cnv.m_Txt=Txt.Trim("()");
    return Cnv.m_Index>0;
    }

  Cnv.m_Index=0;
  Cnv.m_Txt="";
  return true;
  }

double MCnvs::Scale(const MCnv & Cnv)
  {
  CDataCnv * p= gs_CnvsMngr.FindSecCnv(Cnv.m_Index, (LPTSTR)(LPCTSTR)Cnv.m_Txt);
  if (p)
    return p->SclValue();

  if (Cnv.m_Index==1)
    { int xxx=0; }

  if (Cnv.m_Index>0)       
    gs_Log.Message(MMsg_Error, "Bad Conversion %s", Cnv.m_Txt);
  return 1.0;
  };
double MCnvs::Offset(const MCnv & Cnv)
  {
  CDataCnv * p= gs_CnvsMngr.FindSecCnv(Cnv.m_Index, (LPTSTR)(LPCTSTR)Cnv.m_Txt);
  if (p)
    return p->OffValue();
  if (Cnv.m_Index>0)
    gs_Log.Message(MMsg_Error, "Bad Conversion %s", Cnv.m_Txt);
  return 1.0;
  };


//class DllImportExport CCnvsMngr
//  {
//  public:
//    CCnvsMngr();
//    ~CCnvsMngr();
//    void Init();
//    void Reset();
//    CCnvIndex AddSI(char* Fam, char* Txt, char* Desc, double Min, double Max_, flag PrjCnv, long iXform=0);
//    int LoadCnvsADO(char* Filename, flag PrjCnv, BOOL LogErrs = TRUE);
//    void DumpCnvs();
//    CCnvIndex FindCnv(char* pFamName);
//    CCnvIndex AddPri(char* Fam, char* Txt, double Min, double Max_, char* Desc, flag PrjCnv);
//    CCnvIndex AddSec(CCnvIndex index, char* Txt, double Scl, double Off, flag PrjCnv);
//#if WITHCNVINDEXCLASS
//    inline CDataCnv* FindSecCnv(CCnvIndex DC, char* pSecTxt) { return Cnvs[DC.Index]->Find(pSecTxt); };
//    inline CCnvIndex CnvCnt() { return Cnvs.GetSize(); };
//    inline CDataCnv* SICnv(CCnvIndex DC) { return Cnvs[DC.Index]->SICnv(); };
//#else
//    inline CDataCnv* FindSecCnv(CCnvIndex DC, char* pSecTxt) { return Cnvs[DC]->Find(pSecTxt); };
//    inline CCnvIndex CnvCnt() { return Cnvs.GetSize(); };
//    inline CDataCnv* SICnv(CCnvIndex DC) { return Cnvs[DC]->SICnv(); };
//#endif
//    void  Clear();
//    void  Recover();
//    void  Save();

    //void SetAtmosPressMult(double M);
//===========================================================================
//
//
//
//===========================================================================

long MP_All  = 0;
long MP_Sol  = 0;
long MP_Liq  = 0;
long MP_Gas  = 0;
long MP_SL   = 0;


MToleranceBlock::MToleranceBlock(DWORD Use, LPCTSTR Name, double Abs, double Rel, long MaxIters,
                                 DWORD Flags, CCnvIndex AbsIndex, LPCTSTR AbsTxt)
  {
  m_TolBlk= new CToleranceBlock(Use, Name, Abs, Rel, MaxIters, Flags, AbsIndex, AbsTxt);
  };
MToleranceBlock::~MToleranceBlock() { delete m_TolBlk; };

void      MToleranceBlock::SetMaxIters(double MaxIters) { return m_TolBlk->SetMaxIters(MaxIters); };
void      MToleranceBlock::SetAbs(double Abs)           { return m_TolBlk->SetAbs(Abs);           };
void      MToleranceBlock::SetRel(double Rel)           { return m_TolBlk->SetRel(Rel);           };
void      MToleranceBlock::SetAbsMult(double Abs)       { return m_TolBlk->SetAbsMult(Abs);       };
void      MToleranceBlock::SetRelMult(double Rel)       { return m_TolBlk->SetRelMult(Rel);       };
void      MToleranceBlock::SetFlags(DWORD Flags)        { return m_TolBlk->SetFlags(Flags);       };

LPCTSTR   MToleranceBlock::GetName()                    { return m_TolBlk->GetName();             };
long      MToleranceBlock::GetMaxIters()                { return m_TolBlk->GetMaxIters();         };
double    MToleranceBlock::GetAbs()                     { return m_TolBlk->GetAbs();              };
double    MToleranceBlock::GetRel()                     { return m_TolBlk->GetRel();              };
double    MToleranceBlock::GetAbsMult()                 { return m_TolBlk->GetAbsMult();          };
double    MToleranceBlock::GetRelMult()                 { return m_TolBlk->GetRelMult();          };

double    MToleranceBlock::GetNormalError()             { return m_TolBlk->GetNormalError();      };
double    MToleranceBlock::GetRelativeError()           { return m_TolBlk->GetActualError();      };
DWORD     MToleranceBlock::GetFlags()                   { return m_TolBlk->GetFlags();            };

bool      MToleranceBlock::UseMax()                     { return m_TolBlk->UseMax();              };
bool      MToleranceBlock::UseAbs()                     { return m_TolBlk->UseAbs();              };
bool      MToleranceBlock::UseRel()                     { return m_TolBlk->UseRel();              };
bool      MToleranceBlock::Hide()                       { return m_TolBlk->Hide();                };
bool      MToleranceBlock::UseMaxParent()               { return m_TolBlk->UseMaxParent();        };
bool      MToleranceBlock::UseAbsParent()               { return m_TolBlk->UseAbsParent();        };
bool      MToleranceBlock::UseRelParent()               { return m_TolBlk->UseRelParent();        };
bool      MToleranceBlock::HasParent()                  { return m_TolBlk->HasParent();           };

bool MToleranceBlock::ConvergedDV(double D, double V)   { return m_TolBlk->ConvergedDV(D, V);   };
bool MToleranceBlock::ConvergedVV(double V1, double V2) { return m_TolBlk->ConvergedVV(V1, V2); };

//===========================================================================
//
//
//
//===========================================================================

MVectorDefn gs_MVDefn;

MVectorDefn::MVectorDefn()
  {
  m_bInitReqd=true;
  //  m_pImpl = new MVectorDefnImpl;
  }

//---------------------------------------------------------------------------

MVectorDefn::~MVectorDefn()
  {
  //  delete m_pImpl;
  }

void MVectorDefn::Initialise()
  {
  if (m_bInitReqd)
    {
    gs_MVDefn.m_Species.SetSize(SDB.Count());
    for (int s=0; s<SDB.Count(); s++)
      {
      gs_MVDefn.m_Species[s].m_iPSDDefn=-1;
      gs_MVDefn.m_Species[s].m_pSp=&SDB[s];
      }
    }
  m_bInitReqd=false;
  }

//---------------------------------------------------------------------------

//long MVectorDefn::FindTag(const char* Tag)
//  {
//  for (long i=0; i<m_pImpl->iCount; i++)
//    if (strcmp((LPCSTR)(m_pImpl->Species[i].sTag), Tag)==0)
//      return i;
//  return -1;
//  }

//---------------------------------------------------------------------------

//long MVectorDefn::FindSymbol(const char* Symbol)
//  {
//  for (long i=0; i<m_pImpl->iCount; i++)
//    if (strcmp((LPCSTR)(m_pImpl->Species[i].sSymbol), Symbol)==0)
//      return i;
//  return -1;
//  }

//---------------------------------------------------------------------------

long MVectorDefn::LookupNX(LPCTSTR Name)
  {
  return SDB.Lookup(Name);
  }

//---------------------------------------------------------------------------

long MVectorDefn::Lookup(LPCTSTR Name)
  {
  long i=SDB.Lookup(Name);
  if (i<0)
    {
    CString S;
    S.Format("%s Unknown Specie ", Name);
    throw MMdlException(0, S);
    }
  return i;
  }

//---------------------------------------------------------------------------

long MVectorDefn::Count(DWORD PhMask/*=MP_All*/)
  {
  if (PhMask==MP_All)
    return SDB.Count();
  long n=0;
  for (long i=0; i<SDB.Count(); i++)
    if (SDB[i].m_PhMsk & PhMask)
      n++;
  return n;
  };

//---------------------------------------------------------------------------

LPCTSTR MSpecieDefn::Tag()                      { return (LPCSTR)(m_pSp->Tag()); };
LPCTSTR MSpecieDefn::Symbol()                   { return (LPCSTR)(m_pSp->Sym()); };
DWORD   MSpecieDefn::Phase()                    { return m_pSp->m_PhMsk; };
bool    MSpecieDefn::IsLiquid()                 { return m_pSp->IsLiq()!=0; };
bool    MSpecieDefn::IsSolid()                  { return m_pSp->IsSol()!=0; };
bool    MSpecieDefn::IsGas()                    { return m_pSp->IsGas()!=0; };

bool    MSpecieDefn::HasSizeData()              { return m_iPSDDefn>=0; };

double  MSpecieDefn::MolecularWt()              { return m_pSp->MoleWt(); };
double  MSpecieDefn::Density(double T, double P, MVector * MVec) { return m_pSp->Density(SpModel::Fidelity(), T, P, NULL, MVec?(MVec->SpMdl->SVData()):NULL); };
double  MSpecieDefn::NDensity(MVector * MVec)                    { return m_pSp->NDensity(SpModel::Fidelity(), NULL, MVec?(MVec->SpMdl->SVData()):NULL); };
double  MSpecieDefn::msVolume(double T, double P, MVector * MVec) { return m_pSp->msVolume(SpModel::Fidelity(), T, P, NULL, MVec?(MVec->SpMdl->SVData()):NULL); };
double  MSpecieDefn::msNVolume(MVector * MVec)                    { return 1.0/GTZ(m_pSp->NDensity(SpModel::Fidelity(), NULL, MVec?(MVec->SpMdl->SVData()):NULL)); };
//double  MSpecieDefn::Volume(double T, double P, MVector * MVec)  { return m_pSp->Volume(SpModel::Fidelity(), T, P, NULL, MVec?(MVec->SpMdl->SVData()):NULL); };
//double  MSpecieDefn::NVolume(MVector * MVec)                     { return m_pSp->NVolume(SpModel::Fidelity(), NULL, MVec?(MVec->SpMdl->SVData()):NULL); };
double  MSpecieDefn::TempLimitLo()              { return m_pSp->LoT(SpModel::Fidelity()); };
double  MSpecieDefn::TempLimitHi()              { return m_pSp->HiT(SpModel::Fidelity()); };
double  MSpecieDefn::TempLimitTestLo()          { return m_pSp->TestLoT(SpModel::Fidelity()); };
double  MSpecieDefn::TempLimitTestHi()          { return m_pSp->TestHiT(SpModel::Fidelity()); };
double  MSpecieDefn::mlHs(double T, double P)   { return m_pSp->mlHs(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::mlHz(double T, double P)   { return m_pSp->mlHz(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::mlHf(double T, double P)   { return m_pSp->mlHf(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::mlCp(double T, double P)   { return m_pSp->mlCp(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::mlSf(double T, double P)   { return m_pSp->mlSf(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::mlGf(double T, double P)   { return m_pSp->mlGf(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::msHs(double T, double P)   { return m_pSp->msHs(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::msHz(double T, double P)   { return m_pSp->msHz(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::msHf(double T, double P)   { return m_pSp->msHf(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::msCp(double T, double P)   { return m_pSp->msCp(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::msSf(double T, double P)   { return m_pSp->msSf(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::msGf(double T, double P)   { return m_pSp->msGf(SpModel::Fidelity(), T, P, NULL, NULL); };
double  MSpecieDefn::VapourP(double T)          { return m_pSp->VapourP(SpModel::Fidelity(), T); };
double  MSpecieDefn::VapourT(double P)          { return m_pSp->VapourT(SpModel::Fidelity(), P); };

double  MSpecieDefn::PCritical()                { return m_pSp->PCrit(); };
double  MSpecieDefn::TCritical()                { return m_pSp->TCrit(); };
double  MSpecieDefn::VCritical()                { return m_pSp->VCrit(); };
double  MSpecieDefn::Acentricity()              { return m_pSp->ACent(); };
double  MSpecieDefn::MolarVolume()              { return m_pSp->MolarVolume(); };
double  MSpecieDefn::MolecularDiam()            { return m_pSp->MolecularDiam(); };

bool    MSpecieDefn::PcOK()                     { return m_pSp->PcOK(); };
bool    MSpecieDefn::TcOK()                     { return m_pSp->TcOK(); };
bool    MSpecieDefn::VcOK()                     { return m_pSp->VcOK(); };
bool    MSpecieDefn::AcOK()                     { return m_pSp->AcOK(); };

//===========================================================================

//MSpeciePtr::MSpeciePtr(MSpModelDefBase & DefBase, LPCSTR Name, bool Optional)
MSpeciePtr::MSpeciePtr(MInitialiseTest & InitTest, LPCSTR Name, bool Optional, LPCTSTR FileName, LPCTSTR DllName):
  m_InitTest(InitTest)
  {
  m_pSB=new CSpecieBlk(InitTest, (char*)Name, (char*)FileName, (char*)DllName);//__FILE__, "SOMEDLL");
  m_pSB->m_fOpt=Optional;
  };
MSpeciePtr::~MSpeciePtr()
  {
  delete m_pSB;
  }
MSpecieDefn * MSpeciePtr::operator->()  { ChkIndex(); return (m_pSB->sid()>=0) ? &gs_MVDefn[m_pSB->sid()]: NULL; }
MSpeciePtr::operator int ()             { ChkIndex(); return m_pSB->sid();  };
int     MSpeciePtr::get_Index()         { ChkIndex(); return m_pSB->sid();  };
double  MSpeciePtr::get_MW()            { return (*this)->MolecularWt(); };

void MSpeciePtr::ChkIndex()
  {
  //if (m_iIndex==-2) // Lookup will be deferred until needed and Specie Loaded
  //  {
  //  m_iIndex=gs_MVDefn.Lookup(m_sName);
  if (m_pSB->sid()<0 && !m_pSB->m_fOpt)
    {
    INCOMPLETECODE();
    }
  //  }
  };
//===========================================================================

long    MVectorDefn::getFidelity() const           { return SpModel::Fidelity(); };
void    MVectorDefn::putFidelity(long F)           { SpModel::SetFidelity(F); };

//double  MVectorDefn::NormP() { return ::NormPress(); };
//double  MVectorDefn::NormT() { return ::NormTemp(); };
double  MVectorDefn::msHz(DWORD Phase, double T_, double P_, SpPropOveride *Ovr, MArray * M)
  {
  return SDB.msHz(SpModel::Fidelity(), Phase, T_, P_, Ovr, &(*M)[0]);
  };
double  MVectorDefn::msCp(DWORD Phase, double T_, double P_, SpPropOveride *Ovr, MArray * M)
  {
  return SDB.msCp(SpModel::Fidelity(), Phase, T_, P_, Ovr, &(*M)[0]);
  };

//===========================================================================

MDDValueLst * MVectorDefn::DDSpList()         { return SDB.DDBSpList.List()       ; };
MDDValueLst * MVectorDefn::DDSpListDash()     { return SDB.DDBSpListDash.List()   ; };
MDDValueLst * MVectorDefn::DDSolSpList()      { return SDB.DDBSolSpList.List()    ; };
MDDValueLst * MVectorDefn::DDSolSpListDash()  { return SDB.DDBSolSpListDash.List(); };
MDDValueLst * MVectorDefn::DDLiqSpList()      { return SDB.DDBLiqSpList.List()    ; };
MDDValueLst * MVectorDefn::DDLiqSpListDash()  { return SDB.DDBLiqSpListDash.List(); };
MDDValueLst * MVectorDefn::DDVapSpList()      { return SDB.DDBVapSpList.List()    ; };
MDDValueLst * MVectorDefn::DDVapSpListDash()  { return SDB.DDBVapSpListDash.List(); };

//===========================================================================

MPSDDefn::MPSDDefn()
  {
  m_iDist=-1;
  //iCount = 0;
  //pData = NULL;
  }

//---------------------------------------------------------------------------

MPSDDefn::~MPSDDefn()
  {
  }

//---------------------------------------------------------------------------

void MPSDDefn::Initialise(long iDist)
  {
  m_iDist=iDist;
  }

//---------------------------------------------------------------------------

inline long   MPSDDefn::getSizeCount() const                              { return SD_Defn.GetDist(m_iDist)->NIntervals(); };
inline double MPSDDefn::getSize(long iSz) const                           { return SD_Defn.GetDist(m_iDist)->Intervals[iSz]; };
inline double MPSDDefn::getBottomSize() const                             { return SD_Defn.GetDist(m_iDist)->BottomSize(); };
inline double MPSDDefn::getGeometricMean(long iSz) const                  { return SD_Defn.GetDist(m_iDist)->GeometricMean(iSz); };
inline long   MPSDDefn::getPSDVectorCount() const                         { return SD_Defn.GetDist(m_iDist)->NPriIds(); };
inline long   MPSDDefn::getSpecieCount(long iPSDVec) const                { return SD_Defn.GetDist(m_iDist)->NSecIds(iPSDVec); };
inline long   MPSDDefn::getSpecieIndex(long iPSDVec, long iSpecie) const  { return SD_Defn.GetDist(m_iDist)->SzId(iPSDVec, iSpecie); };
inline char*  MPSDDefn::Name() const                                      { return SD_Defn.GetDist(m_iDist)->Name(); };
/*
//---------------------------------------------------------------------------

bool MPSDDefn::HasSizeData(int SpecieIndex)
  {
  SD_Defn.GetDist(m_iDist)->SzId(iSzVec, iSpecie);
  for (int i=0; i<GetDist(m_iDist)->iCount; i++)
    if (pImpl->SpSzIndexes[i]==SpecieIndex)
      return true;
  return false;
  }
*/
//---------------------------------------------------------------------------
//===========================================================================

MPSDDefns gs_PSDDefns;

MPSDDefns::MPSDDefns()
  {
  m_bInitReqd = true;
  }

//---------------------------------------------------------------------------

MPSDDefns::~MPSDDefns()
  {
  }

//---------------------------------------------------------------------------

void MPSDDefns::Initialise()
  {
  if (m_bInitReqd)
    {
    m_Defs.SetSize(SD_Defn.NDistributions());
    for (int d=0; d<SD_Defn.NDistributions(); d++)
      {
      CSD_DistDefn &DD=*SD_Defn.GetDist(d);
      const int NumSizes = DD.NIntervals(); //number of sieve size intervals
      const int NumComps = DD.NPriIds(); //number of components with size data
      MPSDDefn &SD=m_Defs[d];
      SD.Initialise(d);
      for (int i=0; i<NumComps; i++)
        gs_MVDefn.m_Species[DD.PriSzId(i)].m_iPSDDefn=d;
      }
    m_bInitReqd=false;
    }
  }
//---------------------------------------------------------------------------

long MPSDDefns::Count() { return m_Defs.GetSize(); };
MPSDDefn & MPSDDefns::operator[](long index) { return m_Defs[index]; };

//---------------------------------------------------------------------------

bool MInitialise()
  {
  static bool InitReqd = true;
  if (InitReqd)
    {
    InitReqd = false;

    MC_.m_Index           = (DC_)         ;
    MC_None.m_Index       = (DC_)         ;
    MC_Ang.m_Index        = (DC_Ang)      ;
    MC_DP.m_Index         = (DC_DP)       ;
    MC_PpS.m_Index        = (DC_PpS)      ;
    MC_PpM.m_Index        = (DC_PpM)      ;
    MC_PpVol.m_Index      = (DC_PpVol)    ;
    MC_E.m_Index          = (DC_E)        ;
    MC_Frac.m_Index       = (DC_Frac)     ;
    MC_FracRate.m_Index   = (DC_FracRate) ;
    MC_Assay.m_Index      = (DC_Assay)    ;
    MC_HCap.m_Index       = (DC_HCap)     ;
    MC_HCapF.m_Index      = (DC_HCapF)    ;
    MC_L.m_Index          = (DC_L)        ;
    MC_Ldt.m_Index        = (DC_Ldt)      ;
    MC_M.m_Index          = (DC_M)        ;
    MC_Qm.m_Index         = (DC_Qm)       ;
    MC_QmSlew.m_Index     = (DC_QmSlew)   ;
    MC_Qv.m_Index         = (DC_Qv)       ;
    MC_QvSlew.m_Index     = (DC_QvSlew)   ;
    MC_Qe.m_Index         = (DC_Qe)       ;
    MC_NQv.m_Index        = (DC_NQv)      ;
    MC_NVol.m_Index       = (DC_NVol)     ;
    MC_P.m_Index          = (DC_P)        ;
    MC_Pwr.m_Index        = (DC_Pwr)      ;
    MC_pS.m_Index         = (DC_pS)       ;
    MC_Rho.m_Index        = (DC_Rho)      ;
    MC_T.m_Index          = (DC_T)        ;
    MC_Time.m_Index       = (DC_Time)     ;
    MC_Vol.m_Index        = (DC_Vol)      ;
    MC_WI.m_Index         = (DC_WI)       ;
    MC_HMs.m_Index        = (DC_HMs)      ;
    MC_HMl.m_Index        = (DC_HMl)      ;
    MC_HV.m_Index         = (DC_HV)       ;
    MC_HNV.m_Index        = (DC_HNV)      ;
    MC_SMs.m_Index        = (DC_SMs)      ;
    MC_SMl.m_Index        = (DC_SMl)      ;
    MC_CpMs.m_Index       = (DC_CpMs)     ;
    MC_CpMl.m_Index       = (DC_CpMl)     ;
    MC_Conc.m_Index       = (DC_Conc)     ;
    MC_Area.m_Index       = (DC_Area)     ;
    MC_HTC.m_Index        = (DC_HTC)      ;
    MC_HCond.m_Index      = (DC_HCond)    ;
    MC_UA.m_Index         = (DC_UA)       ;
    MC_dT.m_Index         = (DC_dT)       ;
    MC_dTdt.m_Index       = (DC_dTdt)     ;
    MC_I.m_Index          = (DC_I)        ;
    MC_V.m_Index          = (DC_V)        ;
    MC_Epm.m_Index        = (DC_Epm)      ;
    MC_Visc.m_Index       = (DC_Visc)     ;
    MC_MpL.m_Index        = (DC_MpL)      ;
    MC_KgMl.m_Index       = (DC_KgMl)     ;
    MC_QKgMl.m_Index      = (DC_QKgMl)    ;
    MC_Cond.m_Index       = (DC_Cond)     ;
    MC_SpCnt.m_Index      = (DC_SpCnt)    ;
    MC_Cnt.m_Index        = (DC_Cnt)      ;
    MC_CntRate.m_Index    = (DC_CntRate)  ;
    MC_SurfTens.m_Index   = (DC_SurfTens) ;
    MC_EntRate.m_Index    = (DC_EntRate)  ;
    MC_DPperVol.m_Index   = (DC_DPperVol) ;
    MC_SurfAreaM.m_Index  = (DC_SurfAreaM);
    MC_SurfAreaL.m_Index  = (DC_SurfAreaL);
    MC_FracSol.m_Index    = (DC_FracSol)  ;
    MC_FracLiq.m_Index    = (DC_FracLiq)  ;
    MC_FracVap.m_Index    = (DC_FracVap)  ;
    MC_FracPh00.m_Index   = (DC_FracPh00) ;
    MC_FracPh01.m_Index   = (DC_FracPh01) ;
    MC_FracPh02.m_Index   = (DC_FracPh02) ;
    MC_FracPh03.m_Index   = (DC_FracPh03) ;
    MC_FracPh04.m_Index   = (DC_FracPh04) ;
    MC_FracPh05.m_Index   = (DC_FracPh05) ;
    MC_FracPh06.m_Index   = (DC_FracPh06) ;
    MC_FracPh07.m_Index   = (DC_FracPh07) ;
    MC_FracPh08.m_Index   = (DC_FracPh08) ;
    MC_FracPh09.m_Index   = (DC_FracPh09) ;
    MC_FracPh10.m_Index   = (DC_FracPh10) ;
    MC_FracPh11.m_Index   = (DC_FracPh11) ;
    MC_FracPh12.m_Index   = (DC_FracPh12) ;
    MC_FracPh13.m_Index   = (DC_FracPh13) ;
    MC_FracPh14.m_Index   = (DC_FracPh14) ;
    MC_FracPh15.m_Index   = (DC_FracPh15) ;
    MC_FracPh16.m_Index   = (DC_FracPh16) ;
    MC_FracPh17.m_Index   = (DC_FracPh17) ;
    MC_FracPh18.m_Index   = (DC_FracPh18) ;
    MC_FracPh19.m_Index   = (DC_FracPh19) ;
    MC_FracPh20.m_Index   = (DC_FracPh20) ;
    MC_FracPh21.m_Index   = (DC_FracPh21) ;
    MC_FracPh22.m_Index   = (DC_FracPh22) ;
    MC_FracPh23.m_Index   = (DC_FracPh23) ;
    MC_FracPh24.m_Index   = (DC_FracPh24) ;
    MC_FracPh25.m_Index   = (DC_FracPh25) ;
    MC_FracPh26.m_Index   = (DC_FracPh26) ;
    MC_FracPh27.m_Index   = (DC_FracPh27) ;
    MC_FracPh28.m_Index   = (DC_FracPh28) ;
    MC_FracPh29.m_Index   = (DC_FracPh29) ;
    MC_FracPh30.m_Index   = (DC_FracPh30) ;
    MC_FracPh31.m_Index   = (DC_FracPh31) ;
    MC_ThermTau.m_Index   = (DC_ThermTau) ;
    MC_CosPhi.m_Index     = (DC_CosPhi)   ;
    MC_Ohms.m_Index       = (DC_Ohms)     ;
    MC_Bytes.m_Index      = (DC_Bytes)    ;
    MC_ConcRate.m_Index   = (DC_ConcRate) ;
    MC_KinVisc.m_Index    = (DC_KinVisc)  ;
    MC_ElectVA.m_Index    = (DC_ElectVA)  ;
    MC_Humidity.m_Index   = (DC_Humidity) ;
    MC_Money.m_Index      = (DC_Money)    ;
    MC_MoneyFlow.m_Index  = (DC_MoneyFlow);

    gs_MVDefn.Initialise();
    gs_PSDDefns.Initialise();

    MP_All  = som_ALL;
    MP_Sol  = som_Sol;
    MP_Liq  = som_Liq;
    MP_Gas  = som_Gas;
    MP_SL   = som_SL;
    }
  return true;
  };

//===========================================================================
//
//
//
//===========================================================================

#if MOVEDTO_MD_SHARE3

void MLog::Message(MMessageType Type, LPCSTR pFmt, ...)
  {
  char Buff[1024];
  va_list argptr;
  va_start(argptr, pFmt);
  vsprintf(Buff, pFmt, argptr);
  va_end(argptr);

  if (strlen(Buff)>0)
    {
    static DWORD What[/*MMessageType*/] = {LogFlag_Error, LogFlag_Error, LogFlag_Warning, LogFlag_Note };
    LogLn(LogFlag_FromBlackBox|LogFlag_Error, m_pObj->Tag(), 0/*BoxFlags*/, Buff);
    }
  }

//---------------------------------------------------------------------------

void MLog::SetCondition(bool On, long index, MMessageType Type, LPCSTR pFmt, ...)
  {
  if (index>=0 && index<MaxUserCI)
    {
    if (On)
      {
      char Buff[1024];
      va_list argptr;
      va_start(argptr, pFmt);
      vsprintf(Buff, pFmt, argptr);
      va_end(argptr);

      if (strlen(Buff)>0)
        {
        static LPCSTR What[/*MMessageType*/] = {"?", "E", "W", "N" };
        m_pObj->SetCIVirt(FirstUserCI+index+m_pObj->CBContextVirt(), "%s\t%s", What[Type], Buff);
        }
      }
    else
      {
      m_pObj->ClrCIVirt(FirstUserCI+index+m_pObj->CBContextVirt());
      }
    }
  }

//---------------------------------------------------------------------------

void MLog::SetCondition(long index, MMessageType Type, LPCSTR pFmt, ...)
  {
  if (index>=0 && index<MaxUserCI)
    {
    char Buff[1024];
    va_list argptr;
    va_start(argptr, pFmt);
    vsprintf(Buff, pFmt, argptr);
    va_end(argptr);

    if (strlen(Buff)>0)
      {
      static LPCSTR What[/*MMessageType*/] = {"?", "E", "W", "N" };
      m_pObj->SetCIVirt(FirstUserCI+index+m_pObj->CBContextVirt(), "%s\t%s", What[Type], Buff);
      }
    }
  }

//---------------------------------------------------------------------------

void MLog::ClearCondition(long index)
  {
  if (index>=0 && index<MaxUserCI)
    {
    m_pObj->ClrCIVirt(FirstUserCI+index+m_pObj->CBContextVirt());
    }
  }


//===========================================================================
//
//
//
//===========================================================================

MDebug::MDebug()                      { m_pObj=NULL; };
void MDebug::Init(TaggedObject * pObj)  { m_pObj=pObj; };
bool MDebug::Marked()                   { return m_pObj->DoDbgBrk()!=NULL; };

//---------------------------------------------------------------------------

void MDebug::Print(LPCSTR pFmt, ...)
  {
  char Buff[2048];
  va_list argptr;
  va_start(argptr, pFmt);
  vsprintf(Buff, pFmt, argptr);
  va_end(argptr);
  dbgp("%s", Buff);
  };

//---------------------------------------------------------------------------

void MDebug::PrintLn(LPCSTR pFmt, ...)
  {
  char Buff[2048];
  va_list argptr;
  va_start(argptr, pFmt);
  vsprintf(Buff, pFmt, argptr);
  va_end(argptr);
  dbgpln("%s", Buff);
  };
#endif

//===========================================================================
//
//
//
//===========================================================================

//===========================================================================
// Kludge for global pointer (currently used for RTTS OreProps DB)
static void* pGlobalOther = NULL;

void SetGlobalOther(char* pReferenceName, void* pGlobal)
  {
  pGlobalOther = pGlobal;
  }

void* GetGlobalOther(char* pReferenceName)
  {
  if (pReferenceName && pReferenceName[0]!=0)
    {
    if (strcmp(pReferenceName, "RioTintoOreProps")==0)
      return pGlobalOther;
    }
  return NULL;
  }

//===========================================================================
