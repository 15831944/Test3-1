//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __MD_VECTOR_CPP
#include "md_share1.h"
#include "md_defn.h"
#include "md_vector.h"
#include "md_spmodel.h"
#include "md_method.h"
#include "md_PSD.h"
#include "scdmacros.h"
#define MDLBASE
#define SIZEDST1
#include "models.h"
//#include "optoff.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//===========================================================================
//
//
//
//===========================================================================

static void ConduitToVector(SpConduit & C, MVector & V, DWORD PhMsk)
  {
  for (int s=0; s<SDB.Count(); s++)
    V.M[s] = (SDB[s].m_PhMsk & PhMsk) ? C.VMass[s] : 0;
  V.SetTP(C.Temp(), C.Press());
  }

//--------------------------------------------------------------------------

static void VectorToConduit(MVector & V, SpConduit & C, DWORD PhMsk)
  {
  for (int s=0; s<SDB.Count(); s++)
    if (SDB[s].m_PhMsk & PhMsk)
      C.VValue[s] = V.M[s];
  C.SetTempPress(V.T, V.P);
  }

//===========================================================================
//
//
//
//===========================================================================

MProperty::MProperty(MVector * pVec, long iIndex)
  {
  m_pVec=pVec;
  m_iIndex=iIndex;
  };
MProperty::~MProperty()
  {
  };

//---------------------------------------------------------------------------

UCHAR MProperty::getType() const					{ MPropertyInfo *pI=m_pVec->SpMdl->PropertyMap()[m_iIndex]; return (UCHAR)(pI?pI->m_iType:ePT_Null);  };
LPCTSTR MProperty::getTag() const         { MPropertyInfo *pI=m_pVec->SpMdl->PropertyMap()[m_iIndex]; return pI?pI->m_sTag:"";  };
LPCTSTR MProperty::getSymbol() const      { MPropertyInfo *pI=m_pVec->SpMdl->PropertyMap()[m_iIndex]; return pI?pI->m_sSym:"";  };
LPCTSTR MProperty::getDescription() const { MPropertyInfo *pI=m_pVec->SpMdl->PropertyMap()[m_iIndex]; return pI?pI->m_sDesc:"";  };
long MProperty::getCnvIndex() const       { MPropertyInfo *pI=m_pVec->SpMdl->PropertyMap()[m_iIndex]; return pI?pI->m_iCnv:0;   };
bool MProperty::getSettable() const       { MPropertyInfo *pI=m_pVec->SpMdl->PropertyMap()[m_iIndex]; return pI?pI->Settable():false;  };
bool MProperty::getHardMinimum() const    { MPropertyInfo *pI=m_pVec->SpMdl->PropertyMap()[m_iIndex]; return pI?pI->HasHardMin():0;  };
bool MProperty::getSoftMinimum() const    { MPropertyInfo *pI=m_pVec->SpMdl->PropertyMap()[m_iIndex]; return pI?pI->HasSoftMin():0;  };
bool MProperty::getHardMaximum() const    { MPropertyInfo *pI=m_pVec->SpMdl->PropertyMap()[m_iIndex]; return pI?pI->HasHardMax():0;  };
bool MProperty::getSoftMaximum() const    { MPropertyInfo *pI=m_pVec->SpMdl->PropertyMap()[m_iIndex]; return pI?pI->HasSoftMax():0;  };
long MProperty::getIndex() const          { return m_iIndex;  };

//---------------------------------------------------------------------------

long MProperty::LValue(DWORD Phases, double T, double P) const
  {
  SpModel*p=m_pVec->SpMdl;
  MPropertyValue TheValue;
  p->GetPropertyValue(m_iIndex, Phases, Valid(T)?T:p->Temp(), Valid(P)?P:p->Press(), TheValue);
  return TheValue;
  };

//---------------------------------------------------------------------------

//ULONG MProperty::UValue(DWORD Phases, double T, double P) const
//  {
//  SpModel*p=m_pVec->SpMdl;
//  MPropertyValue TheValue;
//  p->GetPropertyValue(m_iIndex, Phases, Valid(T)?T:p->Temp(), Valid(P)?P:p->Press(), TheValue);
//  return TheValue;
//  };

//---------------------------------------------------------------------------

double MProperty::DValue(DWORD Phases, double T, double P) const
  {
  SpModel*p=m_pVec->SpMdl;
  MPropertyValue TheValue;
  p->GetPropertyValue(m_iIndex, Phases, Valid(T)?T:p->Temp(), Valid(P)?P:p->Press(), TheValue);
  return TheValue;
  };

//---------------------------------------------------------------------------

LPCTSTR MProperty::SValue(DWORD Phases, double T, double P) const
  {
  SpModel*p=m_pVec->SpMdl;
  MPropertyValue TheValue;
  p->GetPropertyValue(m_iIndex, Phases, Valid(T)?T:p->Temp(), Valid(P)?P:p->Press(), TheValue);
  return TheValue;
  };

//---------------------------------------------------------------------------

void MProperty::putReqdLValue(long Value)
  {
  SpModel*p=m_pVec->SpMdl;
  p->PutPropertyValue(m_iIndex, MPropertyValue(Value));
  };

//---------------------------------------------------------------------------

//void MProperty::putReqdUValue(ULONG Value)
//  {
//  SpModel*p=m_pVec->SpMdl;
//  p->PutPropertyValue(m_iIndex, MPropertyValue(Value));
//  };

//---------------------------------------------------------------------------

void MProperty::putReqdDValue(double Value)
  {
  SpModel*p=m_pVec->SpMdl;
  p->PutPropertyValue(m_iIndex, MPropertyValue(Value));
  };

//---------------------------------------------------------------------------

void MProperty::putReqdSValue(LPCTSTR Value)
  {
  SpModel*p=m_pVec->SpMdl;
  p->PutPropertyValue(m_iIndex, MPropertyValue(Value));
  };

//===========================================================================
//
//
//
//===========================================================================

inline long MProperties::Count() { return m_pVec->SpMdl->PropertyCount(); };

//---------------------------------------------------------------------------

inline long MProperties::Index(LPCTSTR Name) { return m_pVec->SpMdl->PropertyIndex(Name); };

//---------------------------------------------------------------------------

inline MProperty MProperties::operator[](int Index) { return MProperty(m_pVec, Index); }; //perhaps test for valid index and throw exception!?

//---------------------------------------------------------------------------

MProperty MProperties::operator[](LPCTSTR Name)
  {
  const long i=m_pVec->SpMdl->PropertyIndex(Name);
  if (i<0)
    throw new MMdlException(0, CString("Unknown Property Name:")+Name);
  return MProperty(m_pVec, i);
  }

//---------------------------------------------------------------------------
//===========================================================================
//
//
//
//===========================================================================

MArray::MArray()                                { m_pSysVec=NULL; };
MArray::MArray(CSysVector * p)                  { m_pSysVec=p; };
MArray::MArray(MVector * p)                     { m_pSysVec=p->SpMdl->pMArray(); };
MArray::MArray(MSpModelBase * p)                { m_pSysVec=p->m_pSpMdlX->pMArray(); };
MArray::~MArray()                               { m_pSysVec=NULL; };

double & MArray::operator [](int i)             { return *m_pSysVec->VPtr[i]; }

double MArray::Mass(DWORD Phases)               { return m_pSysVec->Mass(Phases);      };
double MArray::Moles(DWORD Phases)              { return m_pSysVec->Moles(Phases);     };
double MArray::MassFrac(DWORD Phases)           { return m_pSysVec->MassFrac(Phases);  };
double MArray::MoleFrac(DWORD Phases)           { return m_pSysVec->MoleFrac(Phases);  };

MArray & MArray::operator=(MArray & M)
  {
  for (int i=0; i<SDB.Count(); i++)
    (*this)[i]=M[i];
  return *this;
  };

MArray & MArray::operator=(MVector & M)
  {
  for (int i=0; i<SDB.Count(); i++)
    (*this)[i]=M.M[i];
  return *this;
  };

void MArray::Normalise()
  {
  double T=0;
  for (int i=0; i<SDB.Count(); i++)
    T+=(*this)[i];
  T=GTZ(T);
  for (int i=0; i<SDB.Count(); i++)
    (*this)[i]/=T;
  };

//===========================================================================

MArrayI::MArrayI()                               
  { 
  m_pSysVec=new SpVector; 
  };

MArrayI::MArrayI(CSysVector * p)
  {
  m_pSysVec=new SpVector;
  *m_pSysVec=*p;
  };

MArrayI::MArrayI(MVector * p)                     
  { 
  m_pSysVec=new SpVector; 
  *((MArray*)this)=*p;
  };

MArrayI::MArrayI(MSpModelBase * p)                
  { 
  m_pSysVec=new SpVector; 
  *((MArray*)this)=p->MassArray;
  //*m_pSysVec=*p->m_pSpMdlX->pMArray(); 
  };

MArrayI::~MArrayI()
  { 
  delete (SpVector*)m_pSysVec; 
  };

//===========================================================================
//
//
//
//===========================================================================

MVector::MVector() : Properties(this)//, Interfaces(this)
  {
  m_pSpMdl=NULL;
  //m_pBayer=NULL;
  m_pPSD=NULL;
  }

//---------------------------------------------------------------------------

MVector::MVector(SpModel * pMdl) : Properties(this)
  {
  m_pSpMdl=pMdl;
  //m_pBayer=NULL;
  m_pPSD=NULL;
  }

//---------------------------------------------------------------------------

MVector::MVector(MSpQualityBase * p) : Properties(this)
  {
  m_pSpMdl=p->m_pSpQual->pModel;
  //m_pBayer=NULL;
  m_pPSD=NULL;
  };

//---------------------------------------------------------------------------

MVector::~MVector()
  {
  //delete m_pSpMdl; // this would have been attached
  //delete m_pBayer; // this would have been new'd
  delete m_pPSD;   // this would have been new'd
  }

//---------------------------------------------------------------------------

long    MVector::Count(DWORD PhMsk)             { return gs_MVDefn.Count(PhMsk); };
LPCTSTR MVector::getTag()                       { return SpMdl->FullObjTag(); };

double  MVector::getT() const                   { return SpMdl->Temp(); };
void    MVector::putT(double T)                 { SpMdl->SetTempPress(T, SpMdl->Press()); };
double  MVector::getP() const                   { return SpMdl->Press(); };
void    MVector::putP(double P)                 { SpMdl->SetTempPress(SpMdl->Temp(), P); };
double  MVector::getM(long i) const             { return SpMdl->VValue[i]; };
void    MVector::putM(long i, double M)         { SpMdl->VValue[i] = M; };
double  MVector::getA(long i) const             { return SpMdl->VAttr[i]; };
void    MVector::putA(long i, double A)         { SpMdl->VAttr[i]=A; };
double  MVector::getMl(long i) const            { return SpMdl->VMole[i]; };
//void    MVector::putMl(long i, double M)        { SpMdl->VMole[i] = M; };
const double *MVector::getMassVector()                { return SpMdl->SVData(); };
void    MVector::MarkStateChanged()             { SpMdl->ClrStatesOK(); };
void    MVector::putSuppressErrors(bool On)     { SpMdl->SetBadTempOK(On); };


// ----------------------------- Properties
double  MVector::RefTemp() const                                               { return SpMdl->RefTemp();                                                               };
double  MVector::msCp(DWORD Phases, double T, double P) const                  { return SpMdl->msCp(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());       };
double  MVector::msHs(DWORD Phases, double T, double P) const                  { return SpMdl->msHs(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());       };
double  MVector::msHz(DWORD Phases, double T, double P) const                  { return SpMdl->msHz(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());       };
double  MVector::msHf(DWORD Phases, double T, double P) const                  { return SpMdl->msHf(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());       };
double  MVector::msSf(DWORD Phases, double T, double P) const                  { return SpMdl->msSf(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());       };
double  MVector::totCp(DWORD Phases, double T, double P) const                 { return SpMdl->totCp(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());      };
double  MVector::totHs(DWORD Phases, double T, double P) const                 { return SpMdl->totHs(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());      };
double  MVector::totHz(DWORD Phases, double T, double P) const                 { return SpMdl->totHz(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());      };
double  MVector::totHf(DWORD Phases, double T, double P) const                 { return SpMdl->totHf(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());      };
double  MVector::totSf(DWORD Phases, double T, double P) const                 { return SpMdl->totSf(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());      };
double  MVector::TCritical(DWORD Phase, eScdGasMixingRule Rule) const          { return SpMdl->TCritical(Phase, Rule);                                                  };
double  MVector::PCritical(DWORD Phase, eScdGasMixingRule Rule) const          { return SpMdl->PCritical(Phase, Rule);                                                  };
double  MVector::VCritical(DWORD Phase, eScdGasMixingRule Rule) const          { return SpMdl->VCritical(Phase, Rule);                                                  };
double  MVector::AccentricFactor(DWORD Phase, eScdGasMixingRule Rule) const    { return SpMdl->AccentricFactor(Phase, Rule);                                            };
double  MVector::msdHIdeal(DWORD Phase, double T1, double T2) const            { return SpMdl->msdHIdeal(Phase, T1, T2);                                                };
double  MVector::msdSIdeal(DWORD Phase, double T1, double T2) const            { return SpMdl->msdSIdeal(Phase, T1, T2);                                                };
double  MVector::msdHResidual(DWORD Phase, double T, double P, eScdGasMixingRule Rule, eScdEqnOfState Eqn) const { return SpMdl->msdHResidual(Phase, T, P, Rule, Eqn);  };
double  MVector::msdSResidual(DWORD Phase, double T, double P, eScdGasMixingRule Rule, eScdEqnOfState Eqn) const { return SpMdl->msdSResidual(Phase, T, P, Rule, Eqn);  };
double  MVector::PartialPress(int IncIds, int ExcIds, double T) const           { return SpMdl->PartialPress(IncIds, ExcIds, Valid(T)?T:SpMdl->Temp());                              };
double  MVector::Mass(DWORD Phases) const                                      { return SpMdl->Mass(Phases);                                                            };
double  MVector::Moles(DWORD Phases) const                                     { return SpMdl->Moles(Phases);                                                           };
double  MVector::MoleWt(DWORD Phases) const                                    { return SpMdl->MoleWt(Phases);                                                          };
double  MVector::Volume(DWORD Phases, double T, double P) const                { return SpMdl->Volume(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());     };
double  MVector::NVolume(DWORD Phases) const                                   { return SpMdl->NVolume(Phases);                                                         };
double  MVector::Density(DWORD Phases, double T, double P) const               { return SpMdl->Rho(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());        };
double  MVector::NDensity(DWORD Phases) const                                  { return SpMdl->NRho(Phases);                                                            };
double  MVector::MassFrac(DWORD Phases) const                                  { return SpMdl->MassFrac(Phases);                                                        };
double  MVector::MoleFrac(DWORD Phases) const                                  { return SpMdl->MoleFrac(Phases);                                                        };
double  MVector::VolFrac(DWORD Phases, double T, double Press) const           { return SpMdl->VolFrac(Phases, Valid(T)?T:SpMdl->Temp(), Press);                        };
double  MVector::SpecieConc(DWORD Phases, int iSpNo, double Temp) const        { return SpMdl->SpecieConc(Valid(T)?T:SpMdl->Temp(), iSpNo, Phases);                     };
double  MVector::PhaseConc(DWORD Phases, DWORD Phase, double Temp) const       { return SpMdl->PhaseConc(Valid(T)?T:SpMdl->Temp(), Phase, Phases);                      };
double  MVector::DynamicViscosity(DWORD Phases, double T, double P) const      { return SpMdl->DynamicViscosity(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());  };
double  MVector::KinematicViscosity(DWORD Phases, double T, double P) const    { return SpMdl->KinematicViscosity(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());  };
double  MVector::ThermalConductivity(DWORD Phases, double T, double P) const   { return SpMdl->ThermalConductivity(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());  };
double  MVector::PrandtlNo(DWORD Phases, double T, double P) const             { return SpMdl->PrandtlNo(Phases, Valid(T)?T:SpMdl->Temp(), Valid(P)?P:SpMdl->Press());  };
double  MVector::SurfaceTension(DWORD Phases, double T, double P) const        { return SpMdl->SurfaceTension(Phases, Valid(T)?T:SpMdl->Temp(), P);                     };
double  MVector::HeatOfCombustionHi(DWORD Phases, double T, double P) const    { return SpMdl->HeatOfCombustionHi(Phases, Valid(T)?T:SpMdl->Temp(), P);                 };
double  MVector::HeatOfCombustionLo(DWORD Phases, double T, double P) const    { return SpMdl->HeatOfCombustionLo(Phases, Valid(T)?T:SpMdl->Temp(), P);                 };
double  MVector::SaturationTotalP(double T) const                              { return SpMdl->SaturationTotalP(Valid(T)?T:SpMdl->Temp());                                   };
double  MVector::SaturationP(double T) const                                   { return SpMdl->SaturationP(Valid(T)?T:SpMdl->Temp());                                   };
double  MVector::SaturationT(double P) const                                   { return SpMdl->SaturationT(Valid(P)?P:SpMdl->Press());                                  };
double  MVector::PureSaturationP(double T) const                               { return SpMdl->PureSaturationP(Valid(T)?T:SpMdl->Temp());                               };
double  MVector::PureSaturationT(double P) const                               { return SpMdl->PureSaturationT(Valid(P)?P:SpMdl->Press());                              };
double  MVector::BoilingPtElevation(double P) const                            { return SpMdl->BoilingPtElevation(Valid(P)?P:SpMdl->Press());                           };

#if DBG_MVECTOR
double  MVector::msHfRaw() const                                               { return SpMdl->m_msHf;      };
#endif

// ----------------------------- Operations
void    MVector::SetTP(double Temp, double Pres)                               { SpMdl->SetTempPress(Temp, Pres);                      };
void    MVector::Set_msHs(double H, double Pres, MVector * pRefMdl)            { SpMdl->Set_msHs(H, Pres, pRefMdl?pRefMdl->m_pSpMdl:NULL);      };
void    MVector::Set_msHz(double H, double Pres, MVector * pRefMdl)            { SpMdl->Set_msHz(H, Pres, pRefMdl?pRefMdl->m_pSpMdl:NULL);      };
void    MVector::Set_msHf(double H, double Pres, MVector * pRefMdl)            { SpMdl->Set_msHf(H, Pres, pRefMdl?pRefMdl->m_pSpMdl:NULL);      };
void    MVector::Set_totHs(double H, double Pres, MVector * pRefMdl)           { SpMdl->Set_totHs(H, Pres, pRefMdl?pRefMdl->m_pSpMdl:NULL);     };
void    MVector::Set_totHz(double H, double Pres, MVector * pRefMdl)           { SpMdl->Set_totHz(H, Pres, pRefMdl?pRefMdl->m_pSpMdl:NULL);     };
void    MVector::Set_totHf(double H, double Pres, MVector * pRefMdl)           { SpMdl->Set_totHf(H, Pres, pRefMdl?pRefMdl->m_pSpMdl:NULL);     };
void    MVector::ZeroMass()                                                    { SpMdl->ZeroMass();                                    };
void    MVector::ZeroDeriv()                                                   { SpMdl->ZeroDeriv();                                   };
void    MVector::ScaleMass(DWORD Phases, double Mult)                          { SpMdl->ScaleMass(Phases, Mult);                       };
void    MVector::AdjustMassTo(DWORD Phases, double RqdMass)                    { SpMdl->AdjustMassTo(Phases, RqdMass);                 };
void    MVector::SetTraceMass()                                                { SpMdl->SetTraceMass();                                };
void    MVector::SetF(MVector &  Other, DWORD Phases, double Frac, double P)   { SpMdl->SetMassF(Other.SpMdl, Phases, Frac, Finite(P) ? P:Other.P);  };
void    MVector::SetM(MVector &  Other, DWORD Phases, double Mass, double P)   { SpMdl->SetMassM(Other.SpMdl, Phases, Mass, Finite(P) ? P:Other.P);  };
void    MVector::AddF(MVector &  Other, DWORD Phases, double Frac)             { SpMdl->AddMassF(Other.SpMdl, Phases, Frac);           };
void    MVector::AddM(MVector &  Other, DWORD Phases, double Mass)             { SpMdl->AddMassM(Other.SpMdl, Phases, Mass);           };
void    MVector::SubF(MVector &  Other, DWORD Phases, double Frac)             { SpMdl->SubMassF(Other.SpMdl, Phases, Frac);           };
void    MVector::SubM(MVector &  Other, DWORD Phases, double Mass)             { SpMdl->SubMassM(Other.SpMdl, Phases, Mass);           };

//---------------------------------------------------------------------------

MVector & MVector::operator=(const MVector & V)
  {
  for (int s=0; s<SDB.Count(); s++)
    M[s]=V.M[s];
  SetTP(V.T, V.P);
  return *this;
  };

//---------------------------------------------------------------------------

long MVector::Extract(DWORD PhMsk, double *pT, double *pP, double *pM, long MCount)
  {
  long N=SDB.Count(PhMsk);
  if (pT)
    *pT=T;
  if (pP)
    *pP=P;
  if (pM)
    {
    if (MCount!=N)
      throw new MMdlException(0, "Specie Count Mismatch");
    for (int s=0; s<SDB.Count(); s++)
      if (SDB[s].m_PhMsk & PhMsk)
        *pM++=M[s];
    }
  return N;
  };

//---------------------------------------------------------------------------

long MVector::Replace(DWORD PhMsk, double TRqd, double PRqd, double *pMRqd, long MCount)
  {
  long N=SDB.Count(PhMsk);
  if (!Valid(TRqd))
    TRqd=T;
  if (!Valid(PRqd))
    PRqd=P;
  if (pMRqd)
    {
    if (MCount!=N)
      throw new MMdlException(0, "Specie Count Mismatch");
    for (int s=0; s<SDB.Count(); s++)
      if (SDB[s].m_PhMsk & PhMsk)
        M[s]=*pMRqd++;
    }
  SetTP(TRqd, PRqd);
  return N;
  };

//---------------------------------------------------------------------------

MXSpModel *       MVector::GetSpModel4Cast()                              { return SpMdl;  };
MSpModelBase *    MVector::GetMSpModelBase4Cast()                         { return SpMdl->GetMSpModelBase4Cast(); };
MXSpQuality *     MVector::GetSpQuality4Cast(long i, bool Required)       { return SpMdl->GetSpQuality4Cast(i, Required); };
MSpQualityBase *  MVector::GetMSpQualityBase4Cast(long i, bool Required)  { return SpMdl->GetMSpQualityBase4Cast(i, Required); };
long MVector::GetSpQualityCount4Cast()                                    { return SpMdl->GetSpQualityCount4Cast(); };

//---------------------------------------------------------------------------

MIPSD * MVector::CreatePSD()
  {
  SQSzDist1::Ptr(SpMdl, true);
  return getPSD();
  }

//---------------------------------------------------------------------------

MIPSD * MVector::getPSD(long Index)
  {
  SQSzDist1 * pSz = SQSzDist1::Ptr(SpMdl, false);
  if (pSz)
    {
    if (m_pPSD==NULL)
      m_pPSD=new MIPSD(this, Index);
    }
  else
    {
    if (m_pPSD)
      {
      delete m_pPSD;
      m_pPSD=NULL;
      }
    }
  return m_pPSD;
  };


//===========================================================================
//
//
//
//===========================================================================

MStream::MStream()
  {
  m_pMethod=NULL;
  m_pCd=NULL;
  //m_bOwned=false;
  //Allocate();
  }

//---------------------------------------------------------------------------

MStream::MStream(const MStream &Cd)
  {
  m_pMethod=NULL;
  m_pCd=Cd.m_pCd;
  //m_bOwned=false;
  //Attach(Cn);
  //Allocate();
  //*this=Cd;
  }

//---------------------------------------------------------------------------

MStream::MStream(SpConduit * pCd)
  {
  m_pMethod=NULL;
  m_pCd=pCd;
  //m_bOwned=false;
  }

//---------------------------------------------------------------------------

//MStream::MStream(MBaseMethod * Method, LPCSTR Tag, LPCSTR SpMdlId)
//  {
//  ASSERT(Method!=NULL);
//  m_pMethod=Method;
//  m_pCd=NULL;
//  m_bOwned=false;
//  Allocate(Tag, SpMdlId);
//  };

//---------------------------------------------------------------------------

//MStream::MStream(MBaseMethod * Method, LPCSTR Tag, MVector &V)
//  {
//  ASSERT(Method!=NULL);
//  m_pMethod=Method;
//  m_pCd=NULL;
//  m_bOwned=false;
//  Allocate(Tag, V);
//  };

//---------------------------------------------------------------------------

MStream::~MStream()
  {
  //Detach();
  }

//---------------------------------------------------------------------------

LPCTSTR MStream::getModelId() const { return m_pCd->Model()->ClassId(); };
void MStream::putModelId(LPCSTR ClassId)  { m_pCd->SetModel((LPTSTR)ClassId, true); };
void MStream::putModelId(MVector & V)  { m_pCd->SelectModel(V.SpMdl, false); };

//---------------------------------------------------------------------------

void MStream::SelectModel(MStream *V, double MassFlow)
  {
  CSpMdlSlct Slct;
  Slct.Add(V->SpMdl, MassFlow);

  m_pCd->SelectModel(Slct);
  }

//---------------------------------------------------------------------------

void MStream::SelectModel(long NStreams, MStream *V, double *MassFlow)
  {
  CSpMdlSlct Slct;
  for (int i=0; i<NStreams; i++)
    Slct.Add(V[i].SpMdl, MassFlow[i]);

  m_pCd->SelectModel(Slct);
  }

//---------------------------------------------------------------------------

SpModel * MStream::getSpMdl() const
  {
  SpModel *p=m_pCd->Model();
  if (!p) throw MMdlException(0,"Null Model Pointer");
  return p;
  } ;

//---------------------------------------------------------------------------

//void MStream::Attach(const MStream &Cn)
//  {
//  if (m_bOwned)
//    delete m_pCd;
//  m_bOwned=false;
//  m_pCd=Cn.m_pCd;
//  };
//
////---------------------------------------------------------------------------
//
//void MStream::Attach(SpConduit * pCd)
//  {
//  if (m_bOwned)
//    delete m_pCd;
//  m_bOwned=false;
//  m_pCd=pCd;
//  };
//
//---------------------------------------------------------------------------

static long ls_iTmpStreamTagNo=0;

void MStream::Allocate(LPCSTR Tag, LPCSTR SpMdlId)
  {
  //if (m_bOwned)
  //  delete m_pCd;

  //m_bOwned=true;
  if (Tag==NULL)
    {
    CString T;
    T.Format("WorkStream%i", InterlockedIncrement(&ls_iTmpStreamTagNo));
    m_pCd=new SpConduit((LPSTR)(LPCTSTR)T, m_pMethod ? m_pMethod->m_pNd:NULL, m_pMethod ? TOA_Embedded:TOA_Unknown);
    }
  else
    m_pCd=new SpConduit((LPSTR)Tag, m_pMethod ? m_pMethod->m_pNd:NULL, m_pMethod ? TOA_Embedded:TOA_Unknown);
  if (SpMdlId)
    m_pCd->ChangeModel((LPTSTR)SpMdlId, true);
  }

//---------------------------------------------------------------------------

void MStream::Allocate(LPCSTR Tag, MVector &V)
  {
  //if (m_bOwned)
  //  delete m_pCd;

  //m_bOwned=true;
  if (Tag==NULL)
    {
    CString T;
    T.Format("%i", InterlockedIncrement(&ls_iTmpStreamTagNo));
    m_pCd=new SpConduit((LPSTR)Tag, m_pMethod ? m_pMethod->m_pNd:NULL, m_pMethod ? TOA_Embedded:TOA_Unknown);
    }
  else
    m_pCd=new SpConduit((LPSTR)Tag, m_pMethod ? m_pMethod->m_pNd:NULL, m_pMethod ? TOA_Embedded:TOA_Unknown);
  m_pCd->ChangeModel(V.SpMdl->ClassId(), true);
  }

//---------------------------------------------------------------------------

//void MStream::Detach()
//  {
//  if (m_bOwned)
//    delete m_pCd;
//  m_bOwned=false;
//  m_pCd=NULL;
//  }

//---------------------------------------------------------------------------

void MStream::CheckAttached()
  {
  if (m_pCd==NULL)
    {
    //m_bOwned=true;
    //m_pCd=new SpConduit("Tag", m_pMethod ? m_pMethod->m_pNd:NULL, m_pMethod ? TOA_Embedded:TOA_Unknown);
    }
  };

//---------------------------------------------------------------------------

//MStream & MStream::operator=(const MStream & C)
//  {
//  m_pCd->QCopy(*C.m_pCd, C.P);
//  return *this;
//  }

//---------------------------------------------------------------------------

MStream & MStream::operator*=(const double &D)
  {
  m_pCd->pModel->ScaleMass(som_ALL, D);
  return *this;
  };

//---------------------------------------------------------------------------

MStream & MStream::operator=(const MStream & S)
  {
  Cd.QCopy(S.Cd, S.Cd.Press());
  return *this;
  };

//---------------------------------------------------------------------------

void MStream::Copy(const MStream & S, double P)
  {
  Cd.QCopy(S.Cd, Finite(P) ? P : S.Cd.Press());
  };

//---------------------------------------------------------------------------

MStream::operator LPTAGGEDOBJECT() { return m_pCd; };

//---------------------------------------------------------------------------

double  MStream::MassFlow(DWORD PhMsk) const { return m_pCd->QMass(PhMsk); };
double  MStream::VolumeFlow(DWORD PhMsk, double T, double P) const
  {
  return m_pCd->QVolume(PhMsk, Valid(T)?T:m_pCd->Temp(), Valid(P)?P:m_pCd->Press());
  };
double  MStream::Density(DWORD PhMsk, double T, double P) const { return m_pCd->Rho(PhMsk, Valid(T)?T:m_pCd->Temp(), Valid(P)?P:m_pCd->Press()); };
double  MStream::msCp(DWORD PhMsk, double T, double P) const { return m_pCd->msCp(PhMsk, Valid(T)?T:m_pCd->Temp(), Valid(P)?P:m_pCd->Press()); };

//===========================================================================

MStreamI::MStreamI()
  {
  m_pMethod=NULL;
  m_pCd=NULL;
  Allocate();
  }

//---------------------------------------------------------------------------

MStreamI::MStreamI(const MStream &Cd)
  {
  m_pMethod=NULL;
  m_pCd=NULL;//Cd.m_pCd;
  //m_bOwned=false;
  ///Attach(Cn);
  Allocate();
  *(MStream*)m_pCd=Cd;
  }

//---------------------------------------------------------------------------

//MStreamI::MStreamI(SpConduit * pCd)
//  {
//  m_pMethod=NULL;
//  m_pCd=pCd;
//  //m_bOwned=false;
//  }

//---------------------------------------------------------------------------

MStreamI::MStreamI(MBaseMethod * Method, LPCSTR Tag, LPCSTR SpMdlId)
  {
  ASSERT(Method!=NULL);
  m_pMethod=Method;
  m_pCd=NULL;
  //m_bOwned=false;
  Allocate(Tag, SpMdlId);
  };

//---------------------------------------------------------------------------

MStreamI::MStreamI(MBaseMethod * Method, LPCSTR Tag, MVector &V)
  {
  ASSERT(Method!=NULL);
  m_pMethod=Method;
  m_pCd=NULL;
  //m_bOwned=false;
  Allocate(Tag, V);
  };

//---------------------------------------------------------------------------

MStreamI::~MStreamI()
  {
  delete m_pCd;
  //Detach();
  }
//===========================================================================
//
//
//
//===========================================================================

MContainer::MContainer()
  {
  m_pMethod=NULL;
  m_pCn=NULL;
  //m_bOwned=false;
  //Allocate();
  }

//---------------------------------------------------------------------------

MContainer::MContainer(const MContainer &Cn)
  {
  m_pMethod=NULL;
  m_pCn=&Cn.Cn;
  //m_bOwned=false;
  ////Attach(Cn);
  //Allocate();
  //*this=Cn;
  }

//---------------------------------------------------------------------------

MContainer::MContainer(SpContainer* pCn)
  {
  m_pMethod=NULL;
  m_pCn=pCn;
  //m_bOwned=false;
  }

//---------------------------------------------------------------------------

MContainer::~MContainer()
  {
  }

//---------------------------------------------------------------------------

LPCTSTR MContainer::getModelId() const { return m_pCn->Model()->ClassId(); };
void MContainer::putModelId(LPCSTR ClassId)  { m_pCn->SetModel((LPTSTR)ClassId, true); };
void MContainer::putModelId(MVector & V)  { m_pCn->SelectModel(V.SpMdl, false); };

//---------------------------------------------------------------------------

SpModel * MContainer::getSpMdl() const
  {
  SpModel *p=m_pCn->Model();
  if (!p) throw MMdlException(0,"Null Model Pointer");
  return p;
  } ;

//---------------------------------------------------------------------------

//void MContainer::Attach(const MContainer &Cn)
//  {
//  if (m_bOwned)
//    delete m_pCn;
//  m_bOwned=false;
//  m_pCn=Cn.m_pCn;
//  };
//
////---------------------------------------------------------------------------
//
//void MContainer::Attach(SpContainer * pCn)
//  {
//  if (m_bOwned)
//    delete m_pCn;
//  m_bOwned=false;
//  m_pCn=pCn;
//  };

//---------------------------------------------------------------------------

void MContainer::Allocate(LPCSTR Tag, LPCSTR SpMdlId)
  {
  //if (m_bOwned)
  //  delete m_pCn;

  //m_bOwned=true;
  m_pCn=new SpContainer((LPSTR)Tag, m_pMethod ? m_pMethod->m_pNd:NULL, m_pMethod ? TOA_Embedded:TOA_Unknown);
  if (SpMdlId)
    m_pCn->ChangeModel((LPTSTR)SpMdlId, true);
  }

//---------------------------------------------------------------------------

void MContainer::Allocate(LPCSTR Tag, MVector &V)
  {
  //if (m_bOwned)
  //  delete m_pCn;

  //m_bOwned=true;
  m_pCn=new SpContainer((LPSTR)Tag, m_pMethod ? m_pMethod->m_pNd:NULL, m_pMethod ? TOA_Embedded:TOA_Unknown);
  m_pCn->ChangeModel(V.SpMdl->ClassId(), true);
  }

//---------------------------------------------------------------------------

//void MContainer::Detach()
//  {
//  if (m_bOwned)
//    delete m_pCn;
//  m_bOwned=false;
//  m_pCn=NULL;
//  }
//
////---------------------------------------------------------------------------
//
void MContainer::CheckAttached()
  {
  if (m_pCn==NULL)
    {
    //m_bOwned=true;
    //m_pCn=new SpContainer("Tag", m_pMethod ? m_pMethod->m_pNd:NULL, m_pMethod ? TOA_Embedded:TOA_Unknown);
    }
  };

//---------------------------------------------------------------------------

MContainer & MContainer::operator*=(const double &D)
  {
  m_pCn->pModel->ScaleMass(som_ALL, D);
  return *this;
  };

//---------------------------------------------------------------------------

MContainer& MContainer::operator=(const MContainer & S)
  {
  Cn.Copy(S.Cn);
  return *this;
  };

//---------------------------------------------------------------------------

MContainer::operator LPTAGGEDOBJECT() { return m_pCn; };

//---------------------------------------------------------------------------

double  MContainer::Mass(DWORD PhMsk) const                         { return m_pCn->Mass(PhMsk); };
double  MContainer::Volume(DWORD PhMsk, double T, double P) const   { return m_pCn->Volume(PhMsk, Valid(T)?T:m_pCn->Temp(), Valid(P)?P:m_pCn->Press()); };
double  MContainer::Density(DWORD PhMsk, double T, double P) const  { return m_pCn->Rho(PhMsk, Valid(T)?T:m_pCn->Temp(), Valid(P)?P:m_pCn->Press()); };
double  MContainer::msCp(DWORD PhMsk, double T, double P) const     { return m_pCn->msCp(PhMsk, Valid(T)?T:m_pCn->Temp(), Valid(P)?P:m_pCn->Press()); };

//---------------------------------------------------------------------------

MContainerI::MContainerI(MBaseMethod * Method, LPCSTR Tag, LPCSTR SpMdlDesc)
  {
  ASSERT(Method!=NULL);
  m_pMethod=Method;
  m_pCn=NULL;
  //m_bOwned=false;
  Allocate(Tag, SpMdlDesc);
  };

//---------------------------------------------------------------------------

MContainerI::MContainerI(MBaseMethod * Method, LPCSTR Tag, MVector &V)
  {
  ASSERT(Method!=NULL);
  m_pMethod=Method;
  m_pCn=NULL;
  //m_bOwned=false;
  Allocate(Tag, V);
  };

//---------------------------------------------------------------------------

MContainerI::~MContainerI()
  {
  delete m_pCn;
  //Detach();
  }

//===========================================================================
//
//
//
//===========================================================================
