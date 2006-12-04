//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define __MD_SPMODEL_CPP
#include "md_share1.h"
#include "md_defn.h"
#include "md_vector.h"
//#include "md_sizemtx.h"
#include "md_spmodel.h"
//#include "call2mspprops.h"

//#define LIBCOMMENTHD "..\\"
#include "scdmacros.h"
#include "..\..\scd\flwlib\sp_model.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#if 01

#define DllImportExport /*Used by DEFINE_SPMODEL*/


DEFINE_SPMODEL(CCall2MSpProps);

class MSpModelBase;
class CCall2MSpProps :  public SpModelEx
  {
	public:

    MSpModelBase  * m_pUserProps;

    //double          m_dDummyProperty;


		CCall2MSpProps(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
		virtual ~CCall2MSpProps();

    static flag BuildDataDefnGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataDefnBlk & DDB);
    static flag DataXchgGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataChangeBlk & DCB);
//    flag  SpModel::BuildDataDefnGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataDefnBlk & DDB)
//  {
//  //DDB.Add_PropertyMap(PropertyMap(), WhichProps, AsParms, xidSpUsrProps, pOwner, pOwner);
//  return false;
//  };
//flag  SpModel::DataXchgGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataChangeBlk & DCB)
//  {
//  return false;
//  };

    virtual void BuildDataDefn(DataDefnBlk & DDB) 
      {
      //SpModelEx::BuildDataDefn(
      m_pUserProps->m_pDDB=&DDB;
      m_pUserProps->BuildDataFields();
      m_pUserProps->m_pDDB=NULL;
      };
    //Does Nothing: 
    //virtual void   BuildDataDefn_Lcl(DataDefnBlk & DDB)
    //  {
    //  //DDB.Text("...");
    //  //DDB.Text("SMDK Qualities");
    //  //DDB.Text("...");
    //  DDB.Add_PropertyMap(PropertyMap(), 0/*WhichProps*/, true/*AsParms*/, xidSpUsrQuals, this, this);
    //  };
    virtual flag DataXchg(DataChangeBlk & DCB)
      {
      if (SpModelEx::DataXchg(DCB))
        return true;
      m_pUserProps->m_pDCB=&DCB;
      int ret=m_pUserProps->ExchangeDataFields();
      m_pUserProps->m_pDCB=NULL;
      if (ret)
        return ret;
      // ETC Properties
      return 0;
      }

    virtual flag ValidateData(ValidateDataBlk & VDB)
      {
      m_pUserProps->m_pVDB=&VDB;
      flag OK=SpModelEx::ValidateData(VDB);
      OK= OK && m_pUserProps->ValidateDataFields();
      m_pUserProps->m_pVDB=NULL;
      // ETC Properties
      //GetProperties etc
      return true;

      return OK;
      };

    void SetSubClass(LPTSTR MdlName);

		virtual bool   IsBaseClassOf(SpModel * pOther);
		virtual SpModel * PreferredModel(SpModel * pOther);

		virtual double RefTemp() { return C_2_K(0.0); };

		virtual double msCp(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);
		virtual double msHm(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL, double *pTotalM=NULL);

		virtual double Rho(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
		virtual double SaturationP(double T, CSysVector * pMA=NULL);
		virtual double SaturationT(double P, CSysVector * pMA=NULL);
    virtual double DynamicViscosity(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);
    virtual double ThermalConductivity(PhMask Phase, double T_, double P_, CSysVector * pMA=NULL);

    // CPropertyBase virtuals
    virtual LPCTSTR DefinedPropertyMapName()                              { return ClassId(); }
    virtual long    DefinedPropertyCount();
    virtual long    DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    virtual DWORD   GetPropertyVisibility(long Index);
    virtual void    GetPropertyValue(long Index, PhMask Phase, double T, double P, MPropertyValue & Value);
    virtual void    PutPropertyValue(long Index, MPropertyValue & Value);

    virtual MSpModelBase * GetMSpModelBase4Cast() { return m_pUserProps; };
    virtual MIBayer      * GetMIBayer();

  private:
    DEFINE_CI(CCall2MSpProps, SpModelEx, FirstUserCI+MaxUserCI);
    DEFINE_CIVIRT(CCall2MSpProps, SpModelEx);
  	DEFINE_SPARES(CCall2MSpProps)
	};


#undef DllImportExport

#endif

class CSpPropsClassDef : public TagObjClass
    {
    public:
      TaggedObject * ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      TaggedObject * ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      virtual TaggedObject * Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      CSpPropsClassDef(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClass, pchar pVersion_, pchar pDrwGroup_, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask);
    };

TaggedObject * CSpPropsClassDef::ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrp(pClass, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CSpPropsClassDef::ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrpDesc(pShortDesc, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CSpPropsClassDef::Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  {
  SpModel * pn = new CCall2MSpProps(this, pTag, pAttach, eAttach);
  //if (pSubClass)
  //  pn->SetSubClass(pSubClass);
  //else
  if (SubClassId())
    pn->SetSubClass(SubClassId());

  return pn;
  };

CSpPropsClassDef::CSpPropsClassDef(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClassId_, pchar pVersion_, pchar pDrwGroup_, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask) 
  : TagObjClass(pClassName, pGroup_, pClassId_, pSubClassId_, pVersion_, pDrwGroup_, pDefTag_, dwCat, pShortDesc, pDesc, dwSelectMask)
  {
  //sSubClassId=pSubClassId_;
  sMdlLibName=MDLLIBNAME;
  };

//===========================================================================
//
//
//
//===========================================================================

MSpModelDefBase::MSpModelDefBase(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR Desc, unsigned long Flgs, LPCTSTR DLL)
  {
  m_pClassDef=new CSpPropsClassDef((LPTSTR)pClass, SpModel::GroupName, (LPTSTR)pClass, (LPTSTR)pSubClass, "", 
                       "", "UsrProps", TOC_ALL|TOC_GRP_GENERAL|TOC_SMDKRUNTIME, (LPTSTR)ShortDesc, (LPTSTR)Desc, 0xffffffff);
  m_pClassDef->SetMdlLibName(DLL);
  m_pClassDef->SetSubConstruct(this);

  sClassName = pClass;
  //sSpMdlName = pName;
  //sDataTag = pDataTag;
  iSpMdlFlags = Flgs;
  }

MSpModelDefBase::~MSpModelDefBase()
  {
  delete m_pClassDef;
  }

//===========================================================================
//
//
//
//===========================================================================

long MSpModelBase::sm_lCount=0;
long MSpModelBase::sm_lNo=0;
 
MSpModelBase::MSpModelBase() : DD(this), DX(this), DV(this)
  {
  sm_lCount++;
  m_lNo=sm_lNo++;
  //dbgpln("MSpModelBase::MSpModelBase : %6i %6i", m_lNo, sm_lCount);
  m_pSpMdlX=NULL;
  }
MSpModelBase::~MSpModelBase()
  {
  //dbgpln("MSpModelBase::~MSpModelBase : %6i %6i", m_lNo, sm_lCount);
  sm_lCount--;
  }
void MSpModelBase::Init(SpModelEx * pSpMdl)
  {
  MBaseDataCommon::Init(pSpMdl);

  //dbgpln("MSpModelBase::Init         : %08x", this);
  m_pSpMdlX=pSpMdl;
  Log.Init(pSpMdl);
  Dbg.Init(pSpMdl);
  }

//bool MSpModelBase::ValidateDataFields()
//  {
//  //_asm int 3;
//  //ValidateDataBlk VDB;
//  //return m_pSpMdlX->ValidateData(VDB)!=0;
//  return true;
//  }

//IScdSpVirtualProps
bool MSpModelBase::get_IsBaseClassOf(LPCTSTR OtherProgID)
  {
  INCOMPLETECODE(__FILE__, __LINE__);
  return m_pSpMdlX->SpModelEx::IsBaseClassOf(NULL);
  };
LPCTSTR MSpModelBase::get_PreferredModelProgID()
  {
  INCOMPLETECODE(__FILE__, __LINE__);
  return NULL;
  };

double MSpModelBase::get_Density(long Phases, double T, double P, MArray * pMA)             { return m_pSpMdlX->SpModel::Rho(Phases, T, P, pMA?(CSysVector*)(*pMA):NULL); };
double MSpModelBase::get_msEnthalpy(long Phases, double T, double P, MArray * pMA)          { return m_pSpMdlX->SpModel::msHz(Phases, T, P, pMA?(CSysVector*)(*pMA):NULL); };
double MSpModelBase::get_msEntropy(long Phases, double T, double P, MArray * pMA)           { return m_pSpMdlX->SpModel::msSf(Phases, T, P, pMA?(CSysVector*)(*pMA):NULL); };
double MSpModelBase::get_msCp(long Phases, double T, double P, MArray * pMA)                { return m_pSpMdlX->SpModel::msCp(Phases, T, P, pMA?(CSysVector*)(*pMA):NULL); };
double MSpModelBase::get_SaturationT(double P, MArray * pMA)                                { return m_pSpMdlX->SpModel::SaturationT(P, pMA?(CSysVector*)(*pMA):NULL); };
double MSpModelBase::get_SaturationP(double T, MArray * pMA)                                { return m_pSpMdlX->SpModel::SaturationP(T, pMA?(CSysVector*)(*pMA):NULL); };
double MSpModelBase::get_DynamicViscosity(long Phases, double T, double P, MArray * pMA)    { return m_pSpMdlX->SpModel::DynamicViscosity(Phases, T, P, pMA?(CSysVector*)(*pMA):NULL); };
double MSpModelBase::get_ThermalConductivity(long Phases, double T, double P, MArray * pMA) { return m_pSpMdlX->SpModel::ThermalConductivity(Phases, T, P, pMA?(CSysVector*)(*pMA):NULL); };


//double MSpModelBase::BoilPtElev(double T_, double MassL)
//  {
//  _asm int 3;
//  return -1;//m_pSpMdlX->SpModelEx::);
//  };
//
double MSpModelBase::getBoilingPtElevation(double P, MArray * pMA)
  {
  //return m_pSpMdlX->SpModel::SaturationT(P, pMA?(CSysVector*)(*pMA):NULL) - m_pSpMdlX->SpModel::PureSaturationT(P, pMA?(CSysVector*)(*pMA):NULL);
  return get_SaturationT(P, pMA) - m_pSpMdlX->SpModel::PureSaturationT(P, pMA?(CSysVector*)(*pMA):NULL);
  };

double MSpModelBase::DensityMix(double FSol, double Ds, double FLiq, double Dl, double FVap, double Dv, double T_, double P_, MArray & M)
  {
  return SDB.RhoMix(m_pSpMdlX->Fidelity(), FSol, Ds, FLiq, Dl, FVap, Dv, T_, P_, &m_pSpMdlX->m_Ovr, &M[0]);
  };

double MSpModelBase::msEnthalpyMix(double FSol, double Ds, double FLiq, double Dl, double FVap, double Dv, double T_, double P_, MArray & M)
  {
  return SDB.msHmMix(m_pSpMdlX->Fidelity(), FSol, Ds, FLiq, Dl, FVap, Dv, T_, P_, &m_pSpMdlX->m_Ovr, &M[0]);
  };

double MSpModelBase::msCpMix(double FSol, double Ds, double FLiq, double Dl, double FVap, double Dv, double T_, double P_, MArray & M)
  {
  return SDB.msCpMix(m_pSpMdlX->Fidelity(), FSol, Ds, FLiq, Dl, FVap, Dv, T_, P_, &m_pSpMdlX->m_Ovr, &M[0]);
  };

SpPropOveride * MSpModelBase::getPropOverides()
  {
  return &m_pSpMdlX->m_Ovr;
  };

LPCTSTR MSpModelBase::getTag()
  {
  return (LPCTSTR)m_pSpMdlX->FullObjTag();
  };

bool MSpModelBase::TestStateValid(int i)
  {
  return (SMVF_UserModel & m_pSpMdlX->m_dwValidFlags & (1<<i))!=0;
  };
void MSpModelBase::SetStateValid(int i, bool On)
  {
  DWORD D=(SMVF_UserModel & (1<<i));
  if (On)
    m_pSpMdlX->m_dwValidFlags |= D;
  else
    m_pSpMdlX->m_dwValidFlags &= ~D;
  };

double MSpModelBase::getM(long i) const                   { return m_pSpMdlX->m_M[i];                    };
void   MSpModelBase::putM(long i, double M) const         { m_pSpMdlX->VValue[i] = M;                    };
double MSpModelBase::getMl(long i) const                  { return m_pSpMdlX->m_M[i]/SDB[i].MoleWt();    };
void   MSpModelBase::putMl(long i, double M) const        { m_pSpMdlX->VValue[i] =  M*SDB[i].MoleWt();   };

MArray MSpModelBase::getMassArray()                       { return MArray(m_pSpMdlX->pMArray());         };
double MSpModelBase::getPressure()                        { return m_pSpMdlX->Press();                   };
double MSpModelBase::getTemperature()                     { return m_pSpMdlX->Temp();                    };

SV_View      MSpModelBase::getView()                      { return m_pSpMdlX->GetView();                 };
SV_ViewBasis MSpModelBase::getViewBasis()                 { return m_pSpMdlX->GetViewBasis();            };

double MSpModelBase::Mass(DWORD Phases) const             { return m_pSpMdlX->Mass(Phases);              };
double MSpModelBase::Moles(DWORD Phases) const            { return m_pSpMdlX->Moles(Phases);             };
double MSpModelBase::MoleWt(DWORD Phases) const           { return m_pSpMdlX->MoleWt(Phases);            };
double MSpModelBase::MassFrac(DWORD Phases) const         { return m_pSpMdlX->MassFrac(Phases);          };
double MSpModelBase::MoleFrac(DWORD Phases) const         { return m_pSpMdlX->MoleFrac(Phases);          };

void MSpModelBase::ScaleMass(DWORD Phases, double Scale)  { return m_pSpMdlX->ScaleMass(Phases, Scale);  };

MSMFnRanges & MSpModelBase::getSMFnRanges()               { return *m_pSpMdlX->SMFnRanges();                 };

//===========================================================================
//
//
//
//===========================================================================

long MSpQualityBase::sm_lCount=0;
long MSpQualityBase::sm_lNo=0;
 
MSpQualityBase::MSpQualityBase() : DD(this), DX(this), DV(this)   
  {
  sm_lCount++;
  m_lNo=sm_lNo++;
  //dbgpln("MSpQualityBase::MSpQualityBase : %6i %6i", m_lNo, sm_lCount);
  m_pSpQual=NULL;
  }
MSpQualityBase::~MSpQualityBase()
  {
  //dbgpln("MSpQualityBase::~MSpQualityBase : %6i %6i", m_lNo, sm_lCount);
  sm_lCount--;
  }
void MSpQualityBase::Init(SpQuality * pSpQual)
  {
  MBaseDataCommon::Init(pSpQual);

  //dbgpln("MSpQualityBase::Init         : %08x", this);
  m_pSpQual=pSpQual;
  Log.Init(pSpQual);
  Dbg.Init(pSpQual);
  }

LPCTSTR MSpQualityBase::getTag()    { return m_pSpQual->pModel->FullObjTag(); };
MVector MSpQualityBase::getVector() { return MVector(m_pSpQual->pModel); };

//int MSpQualityBase::GroupIdNo()
//  {
//  return m_pSpQual->Class()->GroupIdNo();
//  };
bool MSpQualityBase::TestStateValid()
  {
  return (SMVF_UserQuality & m_pSpQual->pModel->m_dwValidFlags & (1<<(8+m_pSpQual->Class()->GroupIdNo())))!=0;
  };
void MSpQualityBase::SetStateValid(bool On)
  {
  SpModel *p=m_pSpQual->pModel;
  DWORD D=(SMVF_UserQuality & (1<<(8+m_pSpQual->Class()->GroupIdNo())));
  if (On)
    p->m_dwValidFlags |= D;
  else
    p->m_dwValidFlags &= ~D;
  };

//===========================================================================
//
//
//
//===========================================================================


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#if 01
//DEFINE_SPMODEL(CCall2MSpProps);
IMPLEMENT_SPMODEL(CCall2MSpProps, "SMDKSpProps", "", TOC_ALL, "SMDKSpProps", "SMDKSpProps");
IMPLEMENT_SPARES(CCall2MSpProps,100);
IMPLEMENT_BUILDDATADEFNGLOBAL(CCall2MSpProps)
//
//IMPLEMENT_SPMODEL(CCall2MSpProps, "SpModelCOM", "", TOC_SYSTEM,
//                "SpModelCOM",
//                "Base COM Specie model");

CCall2MSpProps::CCall2MSpProps(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
SpModelEx(pClass_, Tag_, pAttach, eAttach)
  {
  m_pUserProps=NULL;

  //m_dDummyProperty=123;
  };

CCall2MSpProps::~CCall2MSpProps()
  {
  delete m_pUserProps;
  m_pUserProps=NULL; 

  }

//---------------------------------------------------------------------------

flag CCall2MSpProps::BuildDataDefnGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataDefnBlk & DDB)
  {
  if (pClass && pClass->GlobalFnData1())
    {
    CCall2MSpProps * pSpProps=static_cast<CCall2MSpProps *>(pClass->GlobalFnData1());
    MSpModelBase * pSpModelBase=static_cast<MSpModelBase*>(pClass->GlobalFnData2());

    Strng ObjTbl;
    ObjTbl.Set("%s_Glbl", pClass->ClassIdNoVersion());
    if (DDB.BeginObject(pOwner, pClass->ClassIdNoVersion(), ObjTbl()))
      {
      DDB.Add_PropertyMap(pSpProps->PropertyMap(), MP_GlobalProp, true, xidSpUsrProps, pSpProps, pOwner);
      }
    DDB.EndObject();

    //p->Propert
    int xxx=0;
    return true;
    }
  
  //if (pClass->GlobalFnBuildDataDefn())
  //  (*pClass->GlobalFnBuildDataDefn())(pClass, pOwner, DDB);
  //DDB.Add_PropertyMap(NULL/*pClass->PropertyMap()*/, 0, true, xidSpUsrProps, pOwner, pOwner);
  //DDB.Add_PropertyMap(NULL/*PropertyMap()*/, 0, true, xidSpUsrProps, pOwner, pOwner);
  return false;
  };

flag CCall2MSpProps::DataXchgGlobal(TagObjClass *pClass, TaggedObject * pOwner, DataChangeBlk & DCB)
  {
  if (pClass && pClass->GlobalFnData1())
    {
    CCall2MSpProps * pSpProps=static_cast<CCall2MSpProps *>(pClass->GlobalFnData1());
    MSpModelBase * pSpModelBase=static_cast<MSpModelBase*>(pClass->GlobalFnData2());

    int xxx=0;

    return pSpProps->DataXchg(DCB);
    }
  return false;
  };

//---------------------------------------------------------------------------

//flag CCall2MSpProps::ValidateData(ValidateDataBlk & VDB)
//  {
//  //delete m_pUserProps;
//  //m_pUserProps=NULL; // new ....
//
//  //m_pUserProps=(MSpModelBase*)Class()->SubConstruct()->Construct();
//  //m_pUserProps->m_pSpMdlX=this;
//
//  m_pUserProps->m_pVDB = &VDB;
//  flag OK=SpModelEx::ValidateData(VDB);
//  OK= OK && m_pUserProps->ValidateDataFields();
//  m_pUserProps->m_pVDB = NULL;
//  return OK;
//  };

//---------------------------------------------------------------------------

void CCall2MSpProps::SetSubClass(LPTSTR MdlName)
  {
  delete m_pUserProps;
  m_pUserProps=NULL; // new ....

  m_pUserProps=(MSpModelBase*)Class()->SubConstruct()->Construct(NULL);
  m_pUserProps->Init(this);
  m_pUserProps->m_pDDB=NULL;
  m_pUserProps->m_pDCB=NULL;
  m_pUserProps->m_pVDB=NULL;

  if (Class()->GlobalFnData1()==NULL)
    {
    MSpModelBase * p=static_cast<MSpModelBase*>(Class()->SubConstruct()->Construct(NULL));
    Class()->SetDataDefnGlobalFnData(this, static_cast<void*>(Class()->SubConstruct()->Construct(NULL)));
    Class()->SetDataDefnGlobalFn(BuildDataDefnGlobal, DataXchgGlobal);
    
    int xxx=0;
    }

  SpModelEx::SetSubClass(MdlName);
  };

//---------------------------------------------------------------------------

bool CCall2MSpProps::IsBaseClassOf(SpModel * pOther)
  {
  if (SpModel::iSpMdlMixStrategy==SPMMS_PreBld35)
    return false;

  CString ID;
  return m_pUserProps->get_IsBaseClassOf(ID);
  };

//---------------------------------------------------------------------------

SpModel * CCall2MSpProps::PreferredModel(SpModel * pOther)
  {
  return this;
  };

//---------------------------------------------------------------------------

double CCall2MSpProps::Rho(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  if (!m_pUserProps)
    return SpModel::Rho(Phase, T_, P_, pMA);
  return m_pUserProps->get_Density(Phase, T_, P_, pMA ? &::MArray(pMA):NULL);
  };

//---------------------------------------------------------------------------

double CCall2MSpProps::SaturationP(double T, CSysVector * pMA)
  {
  if (!m_pUserProps)
    return SpModel::SaturationP(T, pMA);
  return m_pUserProps->get_SaturationP(T, pMA ? &::MArray(pMA):NULL);
  };

//---------------------------------------------------------------------------

double CCall2MSpProps::SaturationT(double P, CSysVector * pMA)
  {
  if (!m_pUserProps)
    return SpModel::SaturationT(P, pMA);
  return m_pUserProps->get_SaturationT(P, pMA ? &::MArray(pMA):NULL);
  };

//---------------------------------------------------------------------------

double CCall2MSpProps::msCp(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  if (!m_pUserProps)
    return SpModel::msCp(Phase, T_, P_, pMA, pTotalM);
  if (pTotalM)
    *pTotalM=pMA ? pMA->Mass(Phase) : SpModel::Mass(Phase);
  return m_pUserProps->get_msCp(Phase, T_, P_, pMA ? &::MArray(pMA):NULL);
  };

//---------------------------------------------------------------------------

double CCall2MSpProps::msHm(PhMask Phase, double T_, double P_, CSysVector * pMA, double *pTotalM)
  {
  if (!m_pUserProps)
    return SpModel::msHf(Phase, T_, P_, pMA, pTotalM);
  if (pTotalM)
    *pTotalM=pMA ? pMA->Mass(Phase) : SpModel::Mass(Phase);
  return m_pUserProps->get_msEnthalpy(Phase, T_, P_, pMA ? &::MArray(pMA):NULL);
  };

//---------------------------------------------------------------------------

double CCall2MSpProps::DynamicViscosity(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  if (!m_pUserProps)
    return SpModel::DynamicViscosity(Phase, T_, P_, pMA);
  return m_pUserProps->get_DynamicViscosity(Phase, T_, P_, pMA ? &::MArray(pMA):NULL);
  };

//---------------------------------------------------------------------------

double CCall2MSpProps::ThermalConductivity(PhMask Phase, double T_, double P_, CSysVector * pMA)
  {
  if (!m_pUserProps)
    return SpModel::ThermalConductivity(Phase, T_, P_, pMA);
  return m_pUserProps->get_ThermalConductivity(Phase, T_, P_, pMA ? &::MArray(pMA):NULL);
  };

//---------------------------------------------------------------------------

MIBayer * CCall2MSpProps::GetMIBayer()
  {
  if (!m_pUserProps)
    return SpModel::GetMIBayer();
  //return m_pUserProps->GetMIBayer(Phase, T_, P_, pMA ? &::MArray(pMA):NULL);
  return dynamic_cast<MIBayer*>(m_pUserProps) ; 
  };

//---------------------------------------------------------------------------
//--------------------------------------------------------------------------

#define WithLcl 0

const long LclPropCount = WithLcl;

long CCall2MSpProps::DefinedPropertyCount()
  {
  return LclPropCount + m_pUserProps->DefinedPropertyCount() + SpModelEx::DefinedPropertyCount();
  }

//--------------------------------------------------------------------------

long CCall2MSpProps::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {
  long Inx=Index-SpModelEx::DefinedPropertyCount();
  if (Inx>=0)
    {
    #if WithLcl
    switch(Inx)
      {
      case  0: Info.SetStructName("MSpProps");
               Info.Set( ePT_Double,    "DummyProp2",       "", DC_,           0,      0,  PIF_HardMin|PIF_Settable,    "A Desc"); return Inx;
      }
    #endif
    return m_pUserProps->DefinedPropertyInfo(Inx-LclPropCount, Info);
    }
  else
    return SpModelEx::DefinedPropertyInfo(Index, Info);
  };

//--------------------------------------------------------------------------

DWORD CCall2MSpProps::GetPropertyVisibility(long Index)
  {
  long Inx=Index-SpModelEx::DefinedPropertyCount();
  if (Inx>=0)
    {
    #if WithLcl
    switch (Inx)
      {
      case   0: return ePVis_All;
      };
    #endif
    return m_pUserProps->GetPropertyVisibility(Inx-LclPropCount);
    }
  else
    return SpModelEx::GetPropertyVisibility(Index);
  }

void CCall2MSpProps::GetPropertyValue(long Index, PhMask Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {
  long Inx=Index-SpModelEx::DefinedPropertyCount();
  if (Inx>=0)
    {
    #if WithLcl
    switch (Inx)
      {
      case   0: Value=m_dDummyProperty;  return ;
      };
    #endif
    if (0)
      {}//m_pUserProps->GetPropertyValue(Inx-LclPropCount , Phase, T, P, Value);
    else
      m_pUserProps->GetPropertyValue(Inx-LclPropCount , Phase, T, P, Value);
    }
  else
    SpModelEx::GetPropertyValue(Index, Phase, T, P, Value);
  };

//--------------------------------------------------------------------------

void CCall2MSpProps::PutPropertyValue(long Index, MPropertyValue & Value)
  {
  long Inx=Index-SpModelEx::DefinedPropertyCount();
  if (Inx>=0)
    {
    #if WithLcl
    switch (Inx)
      {
      case  0: m_dDummyProperty=Value; return;
      };
    #endif
    m_pUserProps->PutPropertyValue(Inx-LclPropCount, Value);
    }
  else
    SpModelEx::PutPropertyValue(Index, Value);
  };


flag CCall2MSpProps::CIStrng(int No, pchar & pS)
  {
  int i=No-CBContext();
  switch (No-CBContext())
    {
    case  1: pS="E\t???????";  return 1;
    default:                                               
      if (i>=1/*FirstUserCI*/ && i<FirstUserCI+MaxUserCI)
        {
        pS="E\tA GFDGDGFDFG";
        return 1;
        }
      return SpModelEx::CIStrng(No, pS);
    }
  }

#endif

//===========================================================================
//
//
//
//===========================================================================

#define DllImportExport

DEFINE_TAGOBJ(CCall2MSpQuals);
class MSpQualityBase;
class CCall2MSpQuals :   public SpQuality//ModelEx
  {
 	public:
		CCall2MSpQuals(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
		virtual ~CCall2MSpQuals();

    void SetSubClass(LPTSTR MdlName);

    virtual MSpQualityBase  * GetMSpQualityBase4Cast()  { return m_pUserQual; };

    DEFINE_QUALITYPTR(CCall2MSpQuals, "SMDKQ")

    virtual void   BuildDataDefn(DataDefnBlk & DDB) 
      {
      m_pUserQual->m_pDDB=&DDB;
      m_pUserQual->BuildDataFields();
      m_pUserQual->m_pDDB=NULL;
      };
    //Does Nothing: 
    //virtual void   BuildDataDefn_Lcl(DataDefnBlk & DDB)
    //  {
    //  //DDB.Text("...");
    //  //DDB.Text("SMDK Qualities");
    //  //DDB.Text("...");
    //  DDB.Add_PropertyMap(PropertyMap(), 0/*WhichProps*/, true/*AsParms*/, xidSpUsrQuals, this, this);
    //  };
    virtual flag   DataXchg(DataChangeBlk & DCB)
      {
      m_pUserQual->m_pDCB=&DCB;
      int ret=m_pUserQual->ExchangeDataFields();
      m_pUserQual->m_pDCB=NULL;
      if (ret)
        return ret;

      if (DCB.lHandle>=xidSpUsrQuals && DCB.lHandle<xidSpUsrQualsLast)
        {
        long i=DCB.lHandle-xidSpUsrQuals;

        CPropertyMap &Map=PropertyMap();
        MPropertyInfo *I=Map[i];
        if (I && (I->m_dwFlags&MP_NoAccess)==0)
          {
          switch (I->m_iType)
            {
            case ePT_Bool:
              {
              if (I->Settable() && DCB.rB)
                {
                m_WorkQualValue=*DCB.rB;
                PutPropertyValue(i, m_WorkQualValue);
                }
              GetPropertyValue(i, som_ALL, pModel->Temp(), pModel->Press(), m_WorkQualValue);
              DCB.B=long(m_WorkQualValue)!=0;
              return 1;
              }
            case ePT_Long:
              {
              if (I->Settable() && DCB.rL)
                {
                m_WorkQualValue=*DCB.rL;
                PutPropertyValue(i, m_WorkQualValue);
                }
              GetPropertyValue(i, som_ALL, pModel->Temp(), pModel->Press(), m_WorkQualValue);
              DCB.L=m_WorkQualValue;
              return 1;
              }
            //case ePT_ULong:
            //  {
            //  if (I->Settable() && DCB.rDW)
            //    {
            //    m_WorkQualValue=*DCB.rDW;
            //    PutPropertyValue(i, m_WorkQualValue);
            //    }
            //  GetPropertyValue(i, som_ALL, Temp(), Press(), m_WorkQualValue);
            //  DCB.DW=m_WorkQualValue;
            //  return 1;
            //  }
            case ePT_Double:
              {
              if (I->Settable() && DCB.rD)
                {
                m_WorkQualValue=*DCB.rD;
                PutPropertyValue(i, m_WorkQualValue);
                }
              GetPropertyValue(i, som_ALL, pModel->Temp(), pModel->Press(), m_WorkQualValue);
              DCB.D=m_WorkQualValue;
              return 1;
              }
            case ePT_String:
              {
              if (I->Settable() && DCB.rpC)
                {
                m_WorkQualValue=(LPCTSTR)*DCB.rpC;
                PutPropertyValue(i, m_WorkQualValue);
                }
              GetPropertyValue(i, som_ALL, pModel->Temp(), pModel->Press(), m_WorkQualValue);
              DCB.pC=m_WorkQualValue;
              return 1;
              }
            }
          }
        }
      return 0;
      };

    virtual flag ValidateData(ValidateDataBlk & VDB)
      {
      m_pUserQual->m_pVDB = &VDB;
      flag ret=m_pUserQual->ValidateDataFields();
      m_pUserQual->m_pVDB = NULL;
      if (ret)
        return ret;
      //GetProperties etc
      return true;
      };
    
    virtual flag   GetOtherData(FilingControlBlock &FCB){ return false;};
    virtual flag   PutOtherData(FilingControlBlock &FCB){ return false;};

//    virtual void   SetStateAction(IE_Enables E);
//    virtual void   Enable(IE_Enables E);
//    virtual void   Disable();
    virtual void   ExchangeSpecies(SpQuality* Other)
      {
      ASSERT(dynamic_cast<CCall2MSpQuals*>(Other));
      m_pUserQual->ExchangeSpecies(dynamic_cast<CCall2MSpQuals*>(Other)->m_pUserQual);
      };

    virtual flag   EquilRqd4HEval()                     { return m_pUserQual->EquilRqd4HEval();       };
    virtual void   Equilibrate()                        { m_pUserQual->Equilibrate(); };
    virtual void   ZeroMass()                           { m_pUserQual->ZeroMass(); };
    virtual void   ZeroDeriv()                          { m_pUserQual->ZeroDeriv(); };
    virtual void   ScaleMass(PhMask Phase, double Mult) { m_pUserQual->ScaleMass(Phase, Mult); };
    virtual void   ScaleMass(CIArray & SpIds, double Mult) { m_pUserQual->ScaleMass(SpIds, Mult); };

    virtual void   SetMassF(CSysVector & M1, SpQuality* pQual2)
      {
      if (pQual2==NULL)
        return;
      ASSERT(dynamic_cast<CCall2MSpQuals*>(pQual2));
      m_pUserQual->SetMassF(dynamic_cast<CCall2MSpQuals*>(pQual2)->m_pUserQual, MArray(&M1));
      };
    virtual void   AddMassF(CSysVector & M1, SpQuality* pQual2, CSysVector & M2)
      {
      if (pQual2==NULL)
        return;
      ASSERT(dynamic_cast<CCall2MSpQuals*>(pQual2));
      m_pUserQual->AddMassF(dynamic_cast<CCall2MSpQuals*>(pQual2)->m_pUserQual, MArray(&M2));
      };
    virtual void   SubMassF(CSysVector & M1, SpQuality* pQual2, CSysVector & M2)
      {
      if (pQual2==NULL)
        return;
      ASSERT(dynamic_cast<CCall2MSpQuals*>(pQual2));
      m_pUserQual->SubMassF(dynamic_cast<CCall2MSpQuals*>(pQual2)->m_pUserQual, MArray(&M2));
      };

    virtual void   Copy(SpModel * pMdl2, SpQuality* pQual2)
      {
      if (pQual2==NULL)
        return;
      ASSERT(dynamic_cast<CCall2MSpQuals*>(pQual2));
      m_pUserQual->Copy(MVector(pMdl2), dynamic_cast<CCall2MSpQuals*>(pQual2)->m_pUserQual);
      };
    virtual void   AddDeriv(SpModel * pMdl2, SpQuality* pQual2, double Sgn_)
      {
      if (pQual2==NULL)
        return;
      ASSERT(dynamic_cast<CCall2MSpQuals*>(pQual2));
      //MVector v2(pQual2->pModel);
      m_pUserQual->AddDeriv(MVector(pMdl2), dynamic_cast<CCall2MSpQuals*>(pQual2)->m_pUserQual, Sgn_, ICGetTimeInc());
      };
    virtual void   AddDiscrete(SpModel * pMdl2, SpQuality* pQual2, double Sgn_)
      {
      if (pQual2==NULL)
        return;
      ASSERT(dynamic_cast<CCall2MSpQuals*>(pQual2));
      //MVector v2(pQual2->pModel);
      m_pUserQual->AddDiscrete(MVector(pMdl2), dynamic_cast<CCall2MSpQuals*>(pQual2)->m_pUserQual, Sgn_, ICGetTimeInc());
      };
    virtual void   ODEOperate(CODEDataBlock & ODB)
      {
      //INCOMPLETECODE(__FILE__, __LINE__);
      };  

    // Extras for Tear Management;
    virtual void   TearGetInputs(TearVarArray & TV, int n)
      {
      INCOMPLETECODE(__FILE__, __LINE__);
      };
    virtual void   TearGetOutputs(TearVarArray & TV, int n)
      {
      INCOMPLETECODE(__FILE__, __LINE__);
      };
    virtual void   TearSetOutputs(TearVarArray & TV, int n)
      {
      INCOMPLETECODE(__FILE__, __LINE__);
      };

    // Debug Stuff
    virtual void   Dump() {};
    virtual void   SetDebug(flag DbgOn_) {};

    //long           DefinedPropertyCount();
    //long           DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    //void           GetPropertyValue(long Index, ULONG Phase, double T, double P, MPropertyValue & Value);
    //void           PutPropertyValue(long Index, MPropertyValue & Value);


    // CPropertyBase virtuals
    virtual LPCTSTR DefinedPropertyMapName() { return ClassId(); }
    virtual long    DefinedPropertyCount();
    virtual long    DefinedPropertyInfo(long Index, MPropertyInfo & Info);

    virtual DWORD   GetPropertyVisibility(long Index);
    virtual void    GetPropertyValue(long Index, PhMask Phase, double T, double P, MPropertyValue & Value);
    virtual void    PutPropertyValue(long Index, MPropertyValue & Value);

 	public:
    MSpQualityBase  * m_pUserQual;
    MPropertyValue  m_WorkQualValue;

  private:
    DEFINE_CI(CCall2MSpQuals, SpQuality, FirstUserCI+MaxUserCI);
    DEFINE_CIVIRT(CCall2MSpQuals, SpQuality);
  	DEFINE_SPARES(CCall2MSpQuals)
	};

flag CCall2MSpQuals::CIStrng(int No, pchar & pS)
  {
  switch (No-CBContext())
    {
    case  1: pS="E\t???????";  return 1;
    default:                                               
      return SpQuality::CIStrng(No, pS);
    }
  }

//--------------------------------------------------------------------------

#undef DllImportExport

class CSpQualsClassDef : public TagObjClass
    {
    public:
      TaggedObject * ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      TaggedObject * ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      virtual TaggedObject * Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach);
      CSpQualsClassDef(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClass, pchar pVersion_, pchar pDrwGroup_, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask);
    };

TaggedObject * CSpQualsClassDef::ConstructGroup(pchar pClass, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrp(pClass, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CSpQualsClassDef::ConstructGroupDesc(pchar pShortDesc, pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  { return TagObjClass::ConstructGrpDesc(pShortDesc, pSubClass, pTag, pAttach, eAttach); };
TaggedObject * CSpQualsClassDef::Construct(pchar pSubClass, pchar pTag, TaggedObject * pAttach, TagObjAttachment eAttach)
  {
  SpQuality * pn = new CCall2MSpQuals(this, pTag, pAttach, eAttach);
  //if (pSubClass)
  //  pn->SetSubClass(pSubClass);
  //else
  if (SubClassId())
    pn->SetSubClass(SubClassId());

  return pn;
  //return NULL;
  };

CSpQualsClassDef::CSpQualsClassDef(pchar pClassName, pchar pGroup_, pchar pClassId_, pchar pSubClassId_, pchar pVersion_, pchar pDrwGroup_, pchar pDefTag_, dword dwCat, pchar pShortDesc, pchar pDesc, DWORD dwSelectMask) 
: TagObjClass(pClassName, pGroup_, pClassId_, pSubClassId_, pVersion_, pDrwGroup_, pDefTag_, dwCat, pShortDesc, pDesc, dwSelectMask)
  {
//  sSubClassId=pSubClassId_;
  sMdlLibName=MDLLIBNAME;
  };

//===========================================================================
//
//
//
//===========================================================================

MSpQualityDefBase::MSpQualityDefBase(LPCTSTR pClass, LPCTSTR pSubClass, LPCTSTR ShortDesc, LPCTSTR Desc, unsigned long Flgs, LPCTSTR DLL, int *pGroupIdNo)
  {
  m_pClassDef=new CSpQualsClassDef((LPTSTR)pClass, SpQuality::GroupName, (LPTSTR)pClass, (LPTSTR)pSubClass, "", "", "UsrProps", TOC_SYSTEM, (LPTSTR)ShortDesc, (LPTSTR)Desc, 0xffffffff);
  m_pClassDef->SetMdlLibName(DLL);
  m_pClassDef->SetSubConstruct(this);

  sClassName = pClass;
  //sSpMdlName = pName;
  //sDataTag = pDataTag;
  iSpQualFlags = Flgs;

  if (pGroupIdNo)
    *pGroupIdNo=m_pClassDef->GroupIdNo();
  }

MSpQualityDefBase::~MSpQualityDefBase()
  {
  delete m_pClassDef;
  }

//===========================================================================
//
//
//
//===========================================================================


#if 01

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//IMPLEMENT_QUALITYPTR(CCall2MSpQuals, "SMDKSpQuals", "", TOC_ALL, "SMDKSpQuals", "SMDKSpQuals");
IMPLEMENT_TAGOBJ(CCall2MSpQuals, "SMDKSpQuals", "SMDKSpQuals", "", "", "SMDKQ", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT,
                 "SMDKSpQuals",
                 "SMDKSpQuals");

IMPLEMENT_SPARES(CCall2MSpQuals,100);

CCall2MSpQuals::CCall2MSpQuals(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach)
 : SpQuality(pClass_, Tag_, pAttach, eAttach)
  {
  m_pUserQual=NULL;

  //m_dDummyProperty=123;
  };

CCall2MSpQuals::~CCall2MSpQuals()
  {

  }

//---------------------------------------------------------------------------

void CCall2MSpQuals::SetSubClass(LPTSTR MdlName)
  {
  delete m_pUserQual;
  m_pUserQual=NULL; // new ....

  m_pUserQual=(MSpQualityBase*)Class()->SubConstruct()->Construct(NULL);
  m_pUserQual->Init(this);
  //m_pUserQual->m_pSpQual=this;
  m_pUserQual->m_pDDB=NULL;
  m_pUserQual->m_pDCB=NULL;
  m_pUserQual->m_pVDB=NULL;

  //m_pMSpQualBase=m_pUserQual;

  SpQuality::SetSubClass(MdlName);
  };

//---------------------------------------------------------------------------
//
//bool CCall2MSpQuals::IsBaseClassOf(SpModel * pOther)
//  {
//  if (SpModel::iSpQualMixStrategy==SPMMS_PreBld35)
//    return false;
//
//  CString ID;
//  return m_pUserProps->get_IsBaseClassOf(ID);
//  };
//
////---------------------------------------------------------------------------
//
//SpModel * CCall2MSpQuals::PreferredModel(SpModel * pOther)
//  {
//  return this;
//  };
//
////---------------------------------------------------------------------------
//
//double CCall2MSpQuals::Rho(PhMask Phase, double T_, double P_, SpMArray * pMA)
//  {
//  if (!m_pUserProps)
//    return SpModel::Rho(Phase, T_, P_, pMA);
//  return m_pUserProps->get_Density(Phase, T_, P_);
//  };
//
////---------------------------------------------------------------------------
//
//double CCall2MSpQuals::SaturationP(double T, SpMArray * pMA)
//  {
//  if (!m_pUserProps)
//    return SpModel::SaturationP(T, pMA);
//  return m_pUserProps->get_SaturationP(T);
//  };
//
////---------------------------------------------------------------------------
//
//double CCall2MSpQuals::SaturationT(double P, SpMArray * pMA)
//  {
//  if (!m_pUserProps)
//    return SpModel::SaturationT(P, pMA);
//  return m_pUserProps->get_SaturationT(P);
//  };
//
////---------------------------------------------------------------------------
//
//double CCall2MSpQuals::msCp(PhMask Phase, double T_, double P_, SpMArray * pMA)
//  {
//  if (!m_pUserProps)
//    return SpModel::msCp(Phase, T_, P_, pMA);
//  return m_pUserProps->get_msCp(Phase, T_, P_);
//  };
//
////---------------------------------------------------------------------------
//
//double CCall2MSpQuals::msHm(PhMask Phase, double T_, double P_, SpMArray * pMA)
//  {
//  if (!m_pUserProps)
//    return SpModel::msHf(Phase, T_, P_, pMA);
//  return m_pUserProps->get_msEnthalpy(Phase, T_, P_);
//  };

//---------------------------------------------------------------------------
//--------------------------------------------------------------------------

#define WIthLcl 0

const long LclQPropCount = WithLcl;

long CCall2MSpQuals::DefinedPropertyCount()
  {
  //return m_pUserQual->DefinedPropertyCount();
  return LclQPropCount + m_pUserQual->DefinedPropertyCount() + SpQuality::DefinedPropertyCount();
  }

//--------------------------------------------------------------------------

long CCall2MSpQuals::DefinedPropertyInfo(long Index, MPropertyInfo & Info)
  {
  long Inx=Index-SpQuality::DefinedPropertyCount();
  if (Inx>=0)
    {
    #if WithLcl
    switch(Inx)
      {
      case  0: Info.SetStructName("MSpQuals");
               Info.Set( ePT_Double,    "DummyQual2",       "", DC_,           0,      0,  MP_HardMin|MP_Parameter, "A Desc"); return Inx;
      }
    #endif
    return m_pUserQual->DefinedPropertyInfo(Inx-LclQPropCount, Info);
    }
  else
    return SpQuality::DefinedPropertyInfo(Index, Info);
  return -1;
  };

//--------------------------------------------------------------------------

DWORD CCall2MSpQuals::GetPropertyVisibility(long Index)
  {
  long Inx=Index-SpQuality::DefinedPropertyCount();
  if (Inx>=0)
    {
    #if WithLcl
    switch (Inx)
      {
      case   0: return ePVis_All;
      };
    #endif
    return m_pUserQual->GetPropertyVisibility(Inx-LclQPropCount);
    }
  else
    return SpQuality::GetPropertyVisibility(Index);
  };

void CCall2MSpQuals::GetPropertyValue(long Index, PhMask Phase/*=MP_All*/, double T/*=NAN*/, double P/*=NAN*/, MPropertyValue & Value)
  {
  long Inx=Index-SpQuality::DefinedPropertyCount();
  if (Inx>=0)
    {
    #if WithLcl
    switch (Inx)
      {
      case   0: Value=m_dDummyProperty;  return ;
      };
    #endif
    m_pUserQual->GetPropertyValue(Inx-LclQPropCount , Phase, T, P, Value);
    }
  else
    SpQuality::GetPropertyValue(Index, Phase, T, P, Value);
  };

//--------------------------------------------------------------------------

void CCall2MSpQuals::PutPropertyValue(long Index, MPropertyValue & Value)
  {
  return m_pUserQual->PutPropertyValue(Index, Value);
  //long Inx=Index-SpQuality::DefinedPropertyCount();
  //if (Inx>=0)
  //  {
  //  #if WithLcl
  //  switch (Inx)
  //    {
  //    case  0: m_dDummyProperty=Value; return;
  //    };
  //  #endif
  //  m_pUserProps->PutPropertyValue(Inx-LclPropCount, Value);
  //  }
  //else
  //  SpQuality::PutPropertyValue(Index, Value);
  };
#endif

//===========================================================================
//
//
//
//===========================================================================
