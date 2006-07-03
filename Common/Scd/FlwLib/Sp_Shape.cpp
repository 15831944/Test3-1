//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"

#define  __SP_SHAPE_CPP
#include "sp_shape.h"
#include "errorlog.h"
#include "dbgmngr.h"
#include "executiv.h"
#include "sp_model.h"

#define  dbgSpModel   WITHDEBUG

#if dbgSpModel
//static CDbgMngr dbgAddDeriv("SpShape", "AddDeriv");
#endif

// ===========================================================================
//
//                              Container Shape
//
// ===========================================================================

inline SpModel *Model(SpShape * This)
  {
  SpModelOwner *pOwner=(SpModelOwner *)This->pAttachedTo;
  ASSERT(dynamic_cast<SpModelOwner *>(This->pAttachedTo));
  return pOwner ? pOwner->pModel : NULL;
  }  

const pchar SpShape::GroupName="SpShape";

IMPLEMENT_SPARES(SpShape, 100);
IMPLEMENT_SPSHAPE(SpShape, "VertCyl", "", TOC_SYSTEM, "Vert. Cyl.", "A Vertical Cylinder");
SpShape::SpShape(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  m_dVolume=10.0;
  m_dHeight=1.0;
  m_dVapVolScl=1.0;
  m_dLowestIOPt=PrjFileVerNo()<58 ? 0.0:0.01;
  m_bKeepLPTOnVChange=true;
  };

// --------------------------------------------------------------------------

SpShape::~SpShape() { };

// --------------------------------------------------------------------------

double SpShape::CurrentP()
  {
  SpModel *p=Model(this);
  if (p)
    return p->m_CurrentP;
  INCOMPLETECODE();
  return 0;
  }

// --------------------------------------------------------------------------

double SpShape::CurrentT()
  {
  SpModel *p=Model(this);
  if (p)
    return p->m_CurrentT;
  INCOMPLETECODE();
  return 0;
  }

// --------------------------------------------------------------------------

void SpShape::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double("Height",  PrjFileVerNo()>=44?"":"H", DC_L,    "m",   &m_dHeight,  this, isParmStopped);
  DDB.Double("Volume",  PrjFileVerNo()>=44?"":"V", DC_Vol,  "m^3", xidSSVolume, this, isParm);
  DDB.Double("Area",    "",                        DC_Area, "m^2", xidSSArea,   this, isResult);
  };

// --------------------------------------------------------------------------

flag SpShape::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidSSVolume:
      if (DCB.rD!=NULL)
        SetApparentVolume(*DCB.rD);
      DCB.D=ApparentVolume();
      return 1;
    case xidSSArea:
      DCB.D=ApparentVolume()/GTZ(m_dHeight);
      return 1;
    }
  return 0;
  }

// --------------------------------------------------------------------------

flag SpShape::ValidateData(ValidateDataBlk & VDB)
  {
  m_dVolume=Max(0.001, m_dVolume);
  m_dHeight=Max(0.01, m_dHeight);
  m_dVapVolScl=ValidateRange(VDB, "VapVolScl", 1.0, m_dVapVolScl, 1.0e6);
  return true;
  }

// --------------------------------------------------------------------------

const byte SpShape_ID   = 0x27;
//nst byte SpShape_Qual = 0x28;
const byte SpShape_End  = 0x29;


flag SpShape::GetOtherData(FilingControlBlock &FCB)
  {
  DWORD nBytes, Len=0xffffffff;
  DWORD StartPos=FCB.SetFilePointer(0, FILE_CURRENT);

  FCB.WriteFile(&SpShape_ID, sizeof(SpShape_ID), &nBytes);
  DWORD LenPos=FCB.SetFilePointer(0, FILE_CURRENT);
  FCB.WriteFile(&Len, sizeof(Len), &nBytes);

  FCB.WriteFile(&m_dVolume, sizeof(m_dVolume), &nBytes);
  FCB.WriteFile(&m_dHeight, sizeof(m_dHeight), &nBytes);
  FCB.WriteFile(&m_dVapVolScl, sizeof(m_dVapVolScl), &nBytes);

  FCB.WriteFile(&SpShape_End, sizeof(SpShape_End), &nBytes);
  DWORD EndPos=FCB.SetFilePointer(0, FILE_CURRENT);
  FCB.SetFilePointer(LenPos, FILE_BEGIN);
  Len=EndPos-StartPos;
  FCB.WriteFile(&Len, sizeof(Len), &nBytes);
  FCB.SetFilePointer(EndPos, FILE_BEGIN);

  return true;
  };

// --------------------------------------------------------------------------

flag SpShape::PutOtherData(FilingControlBlock &FCB)
  {
  DWORD StartPos=FCB.SetFilePointer(0, FILE_CURRENT);
  DWORD nBytes;
  byte What;
  FCB.ReadFile(&What, sizeof(What), &nBytes);
  if (What!=SpShape_ID)
    {
    FCB.SetFilePointer(StartPos, FILE_BEGIN);
    return false;
    }

  DWORD Len;
  FCB.ReadFile(&Len, sizeof(Len), &nBytes);

  FCB.ReadFile(&m_dVolume, sizeof(m_dVolume), &nBytes);
  FCB.ReadFile(&m_dHeight, sizeof(m_dHeight), &nBytes);
  FCB.ReadFile(&m_dVapVolScl, sizeof(m_dVapVolScl), &nBytes);

  DWORD EndPos=FCB.SetFilePointer(0, FILE_CURRENT);
  if (Len!=(EndPos-StartPos))
    {
    LogError(FullObjTag(), 0, "SpShape:PutOtherData - Bad Length");
    FCB.SetFilePointer(StartPos+Len, FILE_BEGIN);
    }
  return true;
  };

// --------------------------------------------------------------------------

void SpShape::CopyOnChange(SpShape * pShape)
  {
  m_dVolume    = pShape->m_dVolume;
  m_dHeight    = pShape->m_dHeight;
  m_dVapVolScl = pShape->m_dVapVolScl;
  };

// --------------------------------------------------------------------------

void SpShape::SetApparentVolume(double v_)
  {
  SpModelOwner *pOwner=(SpModelOwner *)pAttachedTo;
  double OrigT, OrigP;
  if (pOwner)
    pOwner->MCB_GetOriginalTP(OrigT, OrigP);

  double OrigV=m_dVolume;
  double newVol=v_ * m_dVapVolScl;
  m_dVolume=newVol;

  if (pOwner)
    pOwner->MCB_OnSetVolume(m_bKeepLPTOnVChange, OrigT, OrigP, OrigV, m_dVolume);
  }

// --------------------------------------------------------------------------

void SpShape::SetVapVolScale(double S_)
  {
  double newVVS=Range(1.0, S_, 1.0e6);
  m_dVolume*=newVVS/m_dVapVolScl;
  m_dVapVolScl=newVVS;
  };

// --------------------------------------------------------------------------

double SpShape::ActualVolume()                              { return m_dVolume; };
double SpShape::DatumPt()                                   { return 0.0; };
double SpShape::Height()                                    { return m_dHeight; };
double SpShape::Level(double SLVolume)                      { return m_dVapVolScl*SLVolume/m_dVolume; };
double SpShape::RLevel(double SLVolume)                     { return Level(SLVolume); };

// ===========================================================================
//
//                              Pipe Shape
//
// ===========================================================================

IMPLEMENT_SPARES(SpShPipe, 100);
IMPLEMENT_SPSHAPE(SpShPipe, "Pipe", "", TOC_SYSTEM, "Pipe", "A Pipe Cylinder");
SpShPipe::SpShPipe(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  SpShape(pClass_, Tag_, pAttach, eAttach)
  {
  };

// --------------------------------------------------------------------------

SpShPipe::~SpShPipe() { };

// --------------------------------------------------------------------------

void SpShPipe::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double("Height",  PrjFileVerNo()>=44?"":"H", DC_L,    "m",   &m_dHeight,  this, isParmConstruct);
  DDB.Double("Volume",  PrjFileVerNo()>=44?"":"V", DC_Vol,  "m^3", xidSSVolume, this, isParmConstruct);
  DDB.Double("Area",    "",                        DC_Area, "m^2", xidSSArea,   this, isResult);
  };

// ===========================================================================
//
//                          Plug Flow Shape
//
// ===========================================================================

IMPLEMENT_SPARES(SpShPlugFlw, 100);
IMPLEMENT_SPSHAPE(SpShPlugFlw, "PlugFlow", "", TOC_SYSTEM, "PlugFlow", "A PlugFlow Cylinder");
SpShPlugFlw::SpShPlugFlw(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  SpShape(pClass_, Tag_, pAttach, eAttach)
  {
  m_dHeight=0.1;
  };

// --------------------------------------------------------------------------

SpShPlugFlw::~SpShPlugFlw() { };

// --------------------------------------------------------------------------

void SpShPlugFlw::BuildDataDefn(DataDefnBlk & DDB)
  {
//  DDB.Double("Volume",  "", DC_Vol,  "m^3", xidSSVolume, this, isParm);
  DDB.Double("Height",  PrjFileVerNo()>=44?"":"H", DC_L,    "m",   &m_dHeight,  this, DDB.ForDisplayOnly() ? 0 : isParm);
  DDB.Double("Volume",  PrjFileVerNo()>=44?"":"V", DC_Vol,  "m^3", xidSSVolume, this, isParm);
  DDB.Double("Area",    "",                        DC_Area, "m^2", xidSSArea,   this, isResult);
  };

// ===========================================================================
//
//
//
// ===========================================================================

#define DllImportExport

// ===========================================================================
//
//                            VertCyl with Extention
//
// ===========================================================================

DEFINE_SPSHAPE(SpShVertCylX)
class DllImportExport SpShVertCylX : public SpShape
  {
  DEFINE_SPARES(SpShVertCylX)
  public:
    double         m_dBaseHeight;
    double         m_dBaseVolFrac;
    
    SpShVertCylX(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpShVertCylX();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual double ActualVolume() 
      {
      return m_dVolume;
      };
    virtual double DatumPt()
      {
      return m_dBaseHeight;
      };
    double BaseVol()
      {
      return m_dVolume*m_dBaseVolFrac;
      };
    double TopVol()
      {
      return m_dVolume*(1-m_dBaseVolFrac);
      };
    virtual double Level(double SLVolume) 
      {
      double L1=Range(0.0,SLVolume/GTZ(BaseVol()/m_dVapVolScl), 1.0);
      double L2=Range(0.0,(SLVolume-BaseVol()/m_dVapVolScl)/GTZ(TopVol()/m_dVapVolScl), 1.0);
      return (L1*m_dBaseHeight+L2*m_dHeight)/GTZ(m_dBaseHeight+m_dHeight);
      };
    virtual double Height()
      {
      return m_dHeight+m_dBaseHeight;
      };
    virtual double RLevel(double SLVolume) 
      { 
      double L1=Range(0.0,SLVolume/GTZ(BaseVol()/m_dVapVolScl), 1.0);
      double L2=Range(0.0,(SLVolume-BaseVol()/m_dVapVolScl)/GTZ(TopVol()/m_dVapVolScl), 1.0);
      return L2>0.0 ? L2 : L1-1;
      };
    virtual void   SetLowestIOPt(double L) { m_dLowestIOPt=Range(-1.0, L, -0.9); }
    virtual double LowestIOPt() { return m_dLowestIOPt; }
  };

IMPLEMENT_SPARES(SpShVertCylX, 100);
IMPLEMENT_SPSHAPE(SpShVertCylX, "VertCylBaseExtn", "", TOC_SYSTEM, "VertCyl+BaseExtn", "A Vertical Cylinder with Base Extn");
SpShVertCylX::SpShVertCylX(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  SpShape(pClass_, Tag_, pAttach, eAttach)
  {
  m_dBaseHeight=0;
  m_dBaseVolFrac=0;
  m_dLowestIOPt=PrjFileVerNo()<58 ? -1.0 : -0.99;
  };

// --------------------------------------------------------------------------

SpShVertCylX::~SpShVertCylX()
  {
  };

// --------------------------------------------------------------------------

void SpShVertCylX::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double("Height",     PrjFileVerNo()>=44?"":"H",      DC_L,    "m",   &m_dHeight,      this, isParmStopped);
  DDB.Double("BaseHeight", PrjFileVerNo()>=44?"":"BaseH",  DC_L,    "m",   &m_dBaseHeight,  this, isParmStopped);
  DDB.Double("Volume",     PrjFileVerNo()>=44?"":"V",      DC_Vol,  "m^3", xidSSVolume,     this, isParm);
  DDB.Double("BaseVolume", PrjFileVerNo()>=44?"":"BaseV",  DC_Vol,  "m^3", xidSSVolumeB,    this, isParm);
  DDB.Double("Area",       "",                             DC_Area, "m^2", xidSSArea,        this, isResult);
  DDB.Double("BaseArea",   "",                             DC_Area, "m^2", xidSSAreaB,           this, isResult);
  };

// --------------------------------------------------------------------------

flag SpShVertCylX::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidSSVolume:
      if (DCB.rD!=NULL)
        SetApparentVolume(*DCB.rD);
      DCB.D=ApparentVolume();
      return 1;
    case xidSSVolumeB:
      if (DCB.rD!=NULL)
        m_dBaseVolFrac=(*DCB.rD)/GTZ(ApparentVolume());
      DCB.D=ApparentVolume()*m_dBaseVolFrac;
      return 1;
    case xidSSArea:
      DCB.D=(ApparentVolume()*(1-m_dBaseVolFrac))/GTZ(m_dHeight);
      return 1;
    case xidSSAreaB:
      DCB.D=(ApparentVolume()*m_dBaseVolFrac)/GTZ(m_dBaseHeight);
      return 1;
    }
  return 0;
  }

// --------------------------------------------------------------------------

flag SpShVertCylX::ValidateData(ValidateDataBlk & VDB)
  {
  m_dVolume=Max(0.001, m_dVolume);
  m_dHeight=Max(0.01, m_dHeight);
  m_dBaseVolFrac=Range(0.0, m_dBaseVolFrac, 1.0);
  m_dBaseHeight=Max(0.0, m_dBaseHeight);
  m_dVapVolScl=ValidateRange(VDB, "VapVolScl", 1.0, m_dVapVolScl, 1.0e6);
  return true;
  }

// ===========================================================================
//
//                            Volume Prop to Press
//
// ===========================================================================

/**
DEFINE_SPSHAPE(SpShVolFnP)
class DllImportExport SpShVolFnP : public SpShVertCylX
  {
  DEFINE_SPARES(SpShVolFnP)
  public:
    double         m_dBaseHeight;
    double         m_dBaseVolFrac;
    double         m_dPressAtVol;
    
    SpShVolFnP(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpShVolFnP();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual double ActualVolume() 
      {
      SpModel *p=Model(this);
      if (p)
        return m_dVolume*p->Press()/m_dPressAtVol;
      return m_dVolume;
      };
    virtual double DatumPt()
      {
      return m_dBaseHeight;
      };
    double BaseVol()
      {
      return ActualVolume()*m_dBaseVolFrac;
      };
    double TopVol()
      {
      return ActualVolume()*(1-m_dBaseVolFrac);
      };
    virtual double Level(double SLVolume) 
      {
      double L1=Range(0.0,SLVolume/GTZ(BaseVol()/m_dVapVolScl), 1.0);
      double L2=Range(0.0,(SLVolume-BaseVol()/m_dVapVolScl)/GTZ(TopVol()/m_dVapVolScl), 1.0);
      return (L1*m_dBaseHeight+L2*m_dHeight)/GTZ(m_dBaseHeight+m_dHeight);
      };
    virtual double Height()
      {
      return m_dHeight+m_dBaseHeight;
      };
    virtual double RLevel(double SLVolume) 
      { 
      double L1=Range(0.0,SLVolume/GTZ(BaseVol()/m_dVapVolScl), 1.0);
      double L2=Range(0.0,(SLVolume-BaseVol()/m_dVapVolScl)/GTZ(TopVol()/m_dVapVolScl), 1.0);
      return L2>0.0 ? L2 : L1-1;
      };
  };

IMPLEMENT_SPARES(SpShVolFnP, 100);
IMPLEMENT_SPSHAPE(SpShVolFnP, "VolProp2Press", "", TOC_SYSTEM, "VolProp to Press", 
                  "A Vertical Cylinder with Base Extn & Proportional Pressure");
SpShVolFnP::SpShVolFnP(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  SpShVertCylX(pClass_, Tag_, pAttach, eAttach)
  {
  m_dBaseHeight=0;
  m_dBaseVolFrac=0;
  m_dPressAtVol=Std_P;
  };

// --------------------------------------------------------------------------

SpShVolFnP::~SpShVolFnP()
  {
  };

// --------------------------------------------------------------------------

void SpShVolFnP::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double("Height",     PrjFileVerNo()>=44?"":"H",      DC_L,    "m",   &m_dHeight,      this, isParmStopped);
  DDB.Double("BaseHeight", PrjFileVerNo()>=44?"":"BaseH",  DC_L,    "m",   &m_dBaseHeight,  this, isParmStopped);
  DDB.Double("Volume",     PrjFileVerNo()>=44?"":"V",      DC_Vol,  "m^3", xidSSVolume,     this, 0);//isParm);
  DDB.Double("BaseVolume", PrjFileVerNo()>=44?"":"BaseV",  DC_Vol,  "m^3", xidSSVolumeB,    this, isParm);
  DDB.Double("Area",       "",                             DC_Area, "m^2", xidSSArea,        this, isResult);
  DDB.Double("BaseArea",   "",                             DC_Area, "m^2", xidSSAreaB,           this, isResult);
  };

// --------------------------------------------------------------------------

flag SpShVolFnP::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidSSVolume:
      if (DCB.rD!=NULL)
        {
        SpModelOwner *pOwner=(SpModelOwner *)pAttachedTo;
        if (pOwner)
          pOwner->MCB_SetVolume(*DCB.rD);
        else
          SetApparentVolume(*DCB.rD);
        }
      DCB.D=ApparentVolume();
      return 1;
    case xidSSVolumeB:
      if (DCB.rD!=NULL)
        m_dBaseVolFrac=(*DCB.rD)/GTZ(ApparentVolume());
      DCB.D=ApparentVolume()*m_dBaseVolFrac;
      return 1;
    case xidSSArea:
      DCB.D=(ApparentVolume()*(1-m_dBaseVolFrac))/GTZ(m_dHeight);
      return 1;
    case xidSSAreaB:
      DCB.D=(ApparentVolume()*m_dBaseVolFrac)/GTZ(m_dBaseHeight);
      return 1;
    }
  return 0;
  }

// --------------------------------------------------------------------------

flag SpShVolFnP::ValidateData(ValidateDataBlk & VDB)
  {
  m_dVolume=Max(0.001, m_dVolume);
  m_dHeight=Max(0.01, m_dHeight);
  m_dBaseVolFrac=Range(0.0, m_dBaseVolFrac, 1.0);
  m_dBaseHeight=Max(0.0, m_dBaseHeight);
  m_dVapVolScl=ValidateRange(VDB, "VapVolScl", 1.0, m_dVapVolScl, 1.0e6);
  m_dPressAtVol=ValidateRange(VDB, "PressAtVol", 1.0, m_dPressAtVol, 10000.0);
  return true;
  }
*/
// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_SPSHAPE(SpShVolFnP)
class DllImportExport SpShVolFnP : public SpShape
  {
  DEFINE_SPARES(SpShVolFnP)
  public:
    static const pchar GroupName;

    double m_dMinVol, m_dMinPPt, m_dMaxVol, m_dMaxPPt;

    SpShVolFnP(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpShVolFnP();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    //virtual flag   GetOtherData(FilingControlBlock &FCB);
    //virtual flag   PutOtherData(FilingControlBlock &FCB);

    virtual void   CopyOnChange(SpShape * pShape);
    virtual void   SetActualVolume(double V_)             { m_dVolume=V_; };;
    virtual void   SetHeight(double H_)                   { m_dHeight=H_; };;
    virtual void   SetVapVolScale(double s_);   
    virtual void   SetApparentVolume(double V_);
    void           SetVolumeParameter(double v1, double p1, double v2, double p2);

    virtual double VapVolScale()                          { return m_dVapVolScl; };
    virtual double ApparentVolume()                       { return ActualVolume()/m_dVapVolScl; };

    virtual byte   VolumeType()                           { return VVT_FunctP; };
    virtual double ActualVolume(double T, double P);
    virtual double ActualVolume();
    virtual double DatumPt();
    virtual double Level(double SLVolume);
    virtual double Height();
    virtual double RLevel(double SLVolume);

  };

IMPLEMENT_SPARES(SpShVolFnP, 100);
IMPLEMENT_SPSHAPE(SpShVolFnP, "Vert Cyl Vol(P)", "", TOC_SYSTEM, "Vert Cyl Vol(P)", 
                  "A Vertical Cylinder & Volume Function of Pressure");
SpShVolFnP::SpShVolFnP(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  SpShape(pClass_, Tag_, pAttach, eAttach)
  {
  m_dMinVol=10;
  m_dMinPPt=Std_P;
  m_dMaxVol=20;
  m_dMaxPPt=Std_P+100;
  };

// --------------------------------------------------------------------------

SpShVolFnP::~SpShVolFnP() { };

// --------------------------------------------------------------------------

void SpShVolFnP::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double("MinVol",   "",    DC_Vol,  "m^3",  xidSSVPt1, this, isParmStopped);
  DDB.Double("MinPPt",   "",    DC_P,    "kPag", xidSSPPt1, this, isParmStopped);
  DDB.Double("MaxVol",   "",    DC_Vol,  "m^3",  xidSSVPt2, this, isParmStopped);
  DDB.Double("MaxPPt",   "",    DC_P,    "kPag", xidSSPPt2, this, isParmStopped);
  DDB.Double("Height",   "H",   DC_L,    "m",   &m_dHeight,  this, isParmStopped);
  DDB.Double("Volume",   "V",   DC_Vol,  "m^3", xidSSVolume, this, 0);//isParm);
  DDB.Double("Area",      "",   DC_Area, "m^2", xidSSArea,   this, isResult);
  };

// --------------------------------------------------------------------------

flag SpShVolFnP::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidSSVolume:
      DCB.D=ApparentVolume();
      return 1;
    case xidSSArea:
      DCB.D=ApparentVolume()/GTZ(m_dHeight);
      return 1;

    case xidSSVPt1:
      if (DCB.rD)
        SetVolumeParameter(*DCB.rD, m_dMinPPt, m_dMaxVol, m_dMaxPPt);
      DCB.D=m_dMinVol;
      return 1;
    case xidSSPPt1:
      if (DCB.rD)
        SetVolumeParameter(m_dMinVol, *DCB.rD, m_dMaxVol, m_dMaxPPt);
      DCB.D=m_dMinPPt;
      return 1;
    case xidSSVPt2:
      if (DCB.rD)
        SetVolumeParameter(m_dMinVol, m_dMinPPt, *DCB.rD, m_dMaxPPt);
      DCB.D=m_dMaxVol;
      return 1;
    case xidSSPPt2:
      if (DCB.rD)
        SetVolumeParameter(m_dMinVol, m_dMinPPt, m_dMaxVol, *DCB.rD);
      DCB.D=m_dMaxPPt;
      return 1;
    }
  return 0;
  }

// --------------------------------------------------------------------------

flag SpShVolFnP::ValidateData(ValidateDataBlk & VDB)
  {
  m_dVolume=Max(0.001, m_dVolume);
  m_dHeight=Max(0.01, m_dHeight);
  m_dVapVolScl=ValidateRange(VDB, "VapVolScl", 1.0, m_dVapVolScl, 1.0e6);
  m_dMinVol   =ValidateRange(VDB, "MinVol",         0.1, m_dMinVol, 1.0e6);
  m_dMinPPt   =ValidateRange(VDB, "MinPPt",        10.0, m_dMinPPt, 1.0e4);
  m_dMaxVol   =ValidateRange(VDB, "MaxVol", m_dMinVol+1, m_dMaxVol, 1.0e6);
  m_dMaxPPt   =ValidateRange(VDB, "MaxPPt", m_dMinPPt+1, m_dMaxPPt, 1.0e4);
  return SpShape::ValidateData(VDB);
  }

// --------------------------------------------------------------------------

void SpShVolFnP::CopyOnChange(SpShape* pShape)
  {
  m_dMinVol    = pShape->m_dVolume;
  m_dMinPPt    = pShape->CurrentP();
  m_dMaxVol    = pShape->m_dVolume*2;
  m_dMaxPPt    = pShape->CurrentP()+100;
  m_dHeight    = pShape->m_dHeight;
  m_dVapVolScl = pShape->m_dVapVolScl;
  };

// --------------------------------------------------------------------------

void SpShVolFnP::SetVolumeParameter(double v1, double p1, double v2, double p2)
  {
  SpModelOwner *pOwner=(SpModelOwner *)pAttachedTo;
  double OrigT, OrigP;
  if (pOwner)
    pOwner->MCB_GetOriginalTP(OrigT, OrigP);

  double OrigV=ActualVolume(OrigT, OrigP);
  
  m_dMinVol=v1;
  m_dMinPPt=p1;
  m_dMaxVol=v2;
  m_dMaxPPt=p2;

  double newVol=ActualVolume(OrigT, OrigP);
  
  if (pOwner)
    pOwner->MCB_OnSetVolume(m_bKeepLPTOnVChange, OrigT, OrigP, OrigV, newVol);
  }

// --------------------------------------------------------------------------

void SpShVolFnP::SetApparentVolume(double v_)
  {
  //double newVol=v_ * m_dVapVolScl;
  //m_dVolume=newVol;
  }

// --------------------------------------------------------------------------

void SpShVolFnP::SetVapVolScale(double S_)
  {
  double newVVS=Range(1.0, S_, 1.0e6);
  m_dVolume*=newVVS/m_dVapVolScl;
  m_dVapVolScl=newVVS;
  };

// --------------------------------------------------------------------------

double SpShVolFnP::ActualVolume(double T, double P)
  { 
  m_dVolume=m_dVapVolScl*(m_dMinVol+(m_dMaxVol-m_dMinVol)*Range(0.0, (P-m_dMinPPt)/(m_dMaxPPt-m_dMinPPt), 1.0));
  return m_dVolume; 
  };
double SpShVolFnP::ActualVolume()
  { 
  m_dVolume=m_dVapVolScl*(m_dMinVol+(m_dMaxVol-m_dMinVol)*Range(0.0, (CurrentP()-m_dMinPPt)/(m_dMaxPPt-m_dMinPPt), 1.0));
  return m_dVolume; 
  };
double SpShVolFnP::DatumPt()                                   
  { 
  return 0.0;
  };
double SpShVolFnP::Height()
  {
  return m_dHeight;
  };
double SpShVolFnP::Level(double SLVolume)
  {
  return m_dVapVolScl*SLVolume/ActualVolume();
  };
double SpShVolFnP::RLevel(double SLVolume)
  {
  return Level(SLVolume);
  };

// ===========================================================================
//
//
//
// ===========================================================================

DEFINE_SPSHAPE(SpShVolFixedP)
class DllImportExport SpShVolFixedP : public SpShape
  {
  DEFINE_SPARES(SpShVolFixedP)
  public:
    static const pchar GroupName;

    double         m_dPressPt;

    SpShVolFixedP(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach);
    virtual ~SpShVolFixedP();

    virtual void   BuildDataDefn(DataDefnBlk & DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    //virtual flag   GetOtherData(FilingControlBlock &FCB);
    //virtual flag   PutOtherData(FilingControlBlock &FCB);

    virtual void   CopyOnChange(SpShape * pShape);
    virtual void   SetActualVolume(double V_)             { m_dVolume=V_; };;
    virtual void   SetHeight(double H_)                   { m_dHeight=H_; };;
    virtual void   SetVapVolScale(double s_);   
    virtual void   SetApparentVolume(double V_);

    virtual double VapVolScale()                          { return m_dVapVolScl; };
    virtual double ApparentVolume()                       { return ActualVolume()/m_dVapVolScl; };

    virtual byte   VolumeType()                           { return VVT_FixedP; };
    virtual double ActualVolume(double T, double P);
    virtual double ActualVolume();
    virtual double DatumPt();
    virtual double Level(double SLVolume);
    virtual double Height();
    virtual double RLevel(double SLVolume);

  };

IMPLEMENT_SPARES(SpShVolFixedP, 100);
IMPLEMENT_SPSHAPE(SpShVolFixedP, "Vert Cyl Fixed P", "", TOC_SYSTEM, "Vert Cyl Fixed P", 
                  "A Vertical Cylinder - Fixed Pressure");
SpShVolFixedP::SpShVolFixedP(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  SpShape(pClass_, Tag_, pAttach, eAttach)
  {
  m_dPressPt=Std_P;
  };

// --------------------------------------------------------------------------

SpShVolFixedP::~SpShVolFixedP() { };

// --------------------------------------------------------------------------

void SpShVolFixedP::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.Double("FixedPress",  "PPt",  DC_P,    "kPag", &m_dPressPt, this, isParmStopped);
  DDB.Double("Height",      "H",    DC_L,    "m",   &m_dHeight,  this, isParmStopped);
  DDB.Double("Volume",      "V",    DC_Vol,  "m^3", xidSSVolume, this, 0);//isParm);
  DDB.Double("Area",        "",     DC_Area, "m^2", xidSSArea,   this, isResult);
  };

// --------------------------------------------------------------------------

flag SpShVolFixedP::DataXchg(DataChangeBlk & DCB)
  {
  if (TaggedObject::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidSSVolume:
      DCB.D=ApparentVolume();
      return 1;
    case xidSSArea:
      DCB.D=ApparentVolume()/GTZ(m_dHeight);
      return 1;
    }
  return 0;
  }

// --------------------------------------------------------------------------

flag SpShVolFixedP::ValidateData(ValidateDataBlk & VDB)
  {
  m_dVolume=Max(0.001, m_dVolume);
  m_dHeight=Max(0.01, m_dHeight);
  m_dVapVolScl=ValidateRange(VDB, "VapVolScl", 1.0, m_dVapVolScl, 1.0e6);
  return SpShape::ValidateData(VDB);
  }

// --------------------------------------------------------------------------

void SpShVolFixedP::CopyOnChange(SpShape* pShape)
  {
  m_dHeight    = pShape->m_dHeight;
  m_dVapVolScl = pShape->m_dVapVolScl;
  };

// --------------------------------------------------------------------------

void SpShVolFixedP::SetApparentVolume(double v_)
  {
  //double newVol=v_ * m_dVapVolScl;
  //m_dVolume=newVol;
  }

// --------------------------------------------------------------------------

void SpShVolFixedP::SetVapVolScale(double S_)
  {
  double newVVS=Range(1.0, S_, 1.0e6);
  m_dVolume*=newVVS/m_dVapVolScl;
  m_dVapVolScl=newVVS;
  };

// --------------------------------------------------------------------------

double SpShVolFixedP::ActualVolume(double T, double P)
  { 
  SpModel *p=Model(this);
  if (p)
    m_dVolume=p->Volume(som_ALL, T, m_dPressPt);
  return m_dVolume; 
  };
double SpShVolFixedP::ActualVolume()
  { 
  SpModel *p=Model(this);
  if (p)
    m_dVolume=p->Volume(som_ALL, CurrentT(), m_dPressPt);
  return m_dVolume; 
  };
double SpShVolFixedP::DatumPt()                                   
  { 
  return 0.0;
  };
double SpShVolFixedP::Height()
  {
  return m_dHeight;
  };
double SpShVolFixedP::Level(double SLVolume)
  {
  return m_dVapVolScl*SLVolume/Max(0.001, ActualVolume());
  };
double SpShVolFixedP::RLevel(double SLVolume)
  {
  return Level(SLVolume);
  };

// ===========================================================================
//
//
//
// ===========================================================================


#undef DllImportExport
