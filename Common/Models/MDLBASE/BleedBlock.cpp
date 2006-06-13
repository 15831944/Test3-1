#include "stdafx.h"

#define __XBLOCKBLOCK_CPP
#include "BleedBlock.h"

#if WITHBLOCKEVALUATOR

//=========================================================================
//
//
//
//=========================================================================

XID xidAdjustEnable = AdjustXID(1);
XID xidAdjustMdlNm  = AdjustXID(2);

//============================================================================
//
//
//
//============================================================================

const pchar CBleedBlock::GroupName="BleedBlock";

IMPLEMENT_BLEEDBLOCK(CBleedBlock, "BB_None", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "None",  " ");


CBleedBlock::CBleedBlock(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
TaggedObject(pClass_, Tag_, pAttach, eAttach)
  {
  m_pBleedBase = NULL;
  }

//--------------------------------------------------------------------------

CBleedBlock::~CBleedBlock()
  {
  };

//--------------------------------------------------------------------------

void CBleedBlock::BuildDataDefn(DataDefnBlk& DDB)
  {
  if (DDB.BeginStruct(this, NULL, NULL, DDB_NoPage))
    {
    }
  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

void CBleedBlock::EvalProducts(SpConduit &Qf, double Po, double FinalTEst)
  {
  };

void CBleedBlock::EvalProductsPipe(SpConduit & Qf, double Len, double Diam, double Po, double FinalTEst)
  {
  };

//============================================================================
//
//
//
//============================================================================

CBleedBase::CBleedBase(TaggedObject * pAttach, int Index) : CBlockEvalBase(BEId_Bleed, Index),
m_SnkIO(eDIO_Bleed, dynamic_cast<FlwNode*>(pAttach), false, true, 
      "Bleed", IOId_Bleed2Area+Index, IOId_AreaBleedI, "BleedSnk", "BleedSnk_1")
  { 
  m_pBleedB=NULL; 
  m_pNd=pAttach; 
  m_fEnabled=false;
  m_fFixed=false; 
  //m_Index=Index; 
  };
CBleedBase::~CBleedBase()  
  { 
  delete m_pBleedB; 
  };

Strng CBleedBase::Name()
  {
  Strng S;
  S.Set("B%i", m_Index+1);
  return S;
  }

flag CBleedBase::Open(TagObjClass * pAdjustClass, flag Fixed)
  {
  m_fEnabled=True;

  if (m_pBleedB && (pAdjustClass==NULL || m_pBleedB->Class()==pAdjustClass))
    return True;

  delete m_pBleedB;
  m_fFixed=Fixed;

  if (pAdjustClass)
    m_pBleedB=(CBleedBlock*)pAdjustClass->Construct(NULL, "Bleed", m_pNd, TOA_Embedded);//pNd);
  else
    m_pBleedB=(CBleedBlock*)CBleedBlockClass.Construct(NULL, "Bleed", m_pNd, TOA_Embedded);//pNd);
  m_pNd->StructureChanged(NULL);
  m_pBleedB->m_pBleedBase=this;

  m_fEnabled=m_pBleedB!=NULL;
  return m_fEnabled;
  };

//--------------------------------------------------------------------------

void CBleedBase::Close()
  {
  if (m_fFixed)
    return;
  m_fEnabled=False;
  };
//--------------------------------------------------------------------------

void CBleedBase::Add_OnOff(DataDefnBlk &DDB, dword Flags, int UserInfo)
  {
  Strng S;
  S.Set("Bleed%i", m_Index+1);
  DDB.PushUserInfo(UserInfo);
  DDB.Byte(S(), "",  DC_,    "",     xidAdjustEnable,  m_pNd, Flags, GetOnOffValLst());
  DDB.PopUserInfo();
  }

//--------------------------------------------------------------------------

void CBleedBase::BuildDataDefn(DataDefnBlk &DDB, char* pTag, char* pTagComment, DDBPages PageIs, dword UserInfo)
  {
  DDB.PushUserInfo(UserInfo);
  if (pTag==NULL)
    pTag="Xx";
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(SHM_All, m_fEnabled);
  if (Enabled())//pHL)
    {
    DDB.Text("");
    if (DDB.BeginObject(m_pNd, Name()(), "EB_Bleed", pTagComment, PageIs))
      {
      if (m_SnkIO.Enabled)
        m_SnkIO.BuildDataDefn(DDB, NULL, DDB_NoPage, UserInfo+102, DFIO_ShowQm);

      DDBValueLstMem DDB0;
      TagObjClass::GetSDescValueLst(CBleedBlock::GroupName, DDB0);
      DDB.String  ("Model",      "",       DC_    , "",      xidAdjustMdlNm  , m_pNd,m_fFixed ? 0 : isParm|SetOnChange, DDB0());

      if (m_pBleedB)
        {
        //DDB.SuppressNextBlock();
        m_pBleedB->BuildDataDefn(DDB);
        }
      }
    DDB.EndObject();
    }
  DDB.SetVisibility(Old);
  DDB.PopUserInfo();
  };

//--------------------------------------------------------------------------

flag CBleedBase::DataXchg(DataChangeBlk & DCB)
  {
  if (DCB.dwUserInfo%1000==102)
    {
    if (m_SnkIO.DataXchg(DCB))
      return 1;
    }

  switch (DCB.lHandle)
    {
    case xidAdjustEnable:
      if (DCB.rB)
        {
        if (*DCB.rB)
          Open(*DCB.rB);
        else
          Close();
        }
      DCB.B=OpenStatus();// (Enabled());
      return 1;
    case xidAdjustMdlNm:
      if (DCB.rpC && !m_fFixed)
        {
        int WasEnabled=m_fEnabled;
        TagObjClass * pC=CBleedBlockClass.FindGrpShortDesc(DCB.rpC);
        if (pC)
          Open(pC);
        else
          Close();
        }
      DCB.pC = m_pBleedB ? m_pBleedB->ShortDesc() : "";
      return 1;
    }
  return 0;
  }

//============================================================================
//
//
//
//============================================================================

#define DllImportExport /* */

class DllImportExport CXBlk_Bleed: public CBleedBlock
  {
  public:
    CXBlk_Bleed(TagObjClass* pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach);
    virtual ~CXBlk_Bleed();

    virtual void   BuildDataDefn(DataDefnBlk& DDB);
    virtual flag   DataXchg(DataChangeBlk & DCB);
    virtual flag   ValidateData(ValidateDataBlk & VDB);

    virtual void   EvalProducts(SpConduit & Fo, double Po, double FinalTEst=dNAN);
    virtual void   EvalProductsPipe(SpConduit & Fo, double Len, double Diam, double Po, double FinalTEst=dNAN);

  public:

    enum eDstType   { DstType_Area, DstType_BleedSnk };
    enum eType      { Type_QmBleed, Type_QmProduct, Type_FracBleed, };

    eDstType        m_DstType;
    Strng           m_Dst;
    eType           m_Type;
    double          m_QmBleed;
    double          m_QmProduct;
    double          m_Frac;
  };

DEFINE_BLEEDBLOCK(CXBlk_Bleed);

//============================================================================
//
//
//
//============================================================================

IMPLEMENT_BLEEDBLOCK(CXBlk_Bleed, "BB_Simple", "", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "Simple",  " ");

CXBlk_Bleed::CXBlk_Bleed(pTagObjClass pClass_, pchar Tag_, TaggedObject* pAttach, TagObjAttachment eAttach) :
CBleedBlock(pClass_, Tag_, pAttach, eAttach)
  {
  m_Dst       = PlantAreaTag;
  m_DstType   = DstType_Area;
  m_Type      = Type_FracBleed;
  m_Frac      = 0;
  m_QmBleed   = 0;
  m_QmProduct = 0;
  }

//--------------------------------------------------------------------------

CXBlk_Bleed::~CXBlk_Bleed()
  {
  }

//--------------------------------------------------------------------------

//static DDBValueLst DDBDestinations[] =
//  {
//    { Dest_Mix      , "Mixture"},
//    { Dest_Discard  , "Discard"},
//    { 0 }
//  };

void CXBlk_Bleed::BuildDataDefn(DataDefnBlk& DDB)
  {

  //if (DDB.BeginStruct(this, "XXX", NULL, DDB_NoPage))
  //  {
  static DDBValueLst DDBDstType[] =
    {
      {DstType_Area,      "Area"},
      {DstType_BleedSnk,  "BleedSnk"},
      {}
    };
  static DDBValueLst DDBType[] =
    {
      {Type_FracBleed,  "Frac"},
      {Type_QmBleed,    "QmBleed"},
      {Type_QmProduct,  "QmProduct"},
      {}
    };
  //DDB.Text(" ");
  DDB.Long  ("", "DestType",    DC_,     "",      (long*)&m_DstType,  this, isParm|SetOnChange, DDBDstType);
  DDB.String("", "Destination", DC_,     "",      &m_Dst,             this, isParm);
  DDB.Long  ("", "Type",        DC_,     "",      (long*)&m_Type,     this, isParm|SetOnChange, DDBType);
  DDB.Double("", "Fraction",    DC_Frac, "%",     &m_Frac,            this, isParm);
  DDB.Double("", "QmBleed",     DC_Qm,   "kg/s",  &m_QmBleed,         this, isParm);
  DDB.Double("", "QmProduct",   DC_Qm,   "kg/s",  &m_QmProduct,       this, isParm);
  //  }
  //DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag CXBlk_Bleed::DataXchg(DataChangeBlk & DCB)
  {
  //  switch (DCB.lHandle)
  //    {
  //    case xidCompCount:
  //      if (DCB.rL)
  //        {
  //        int Old=m_Components.GetSize();
  //        m_Components.SetSize(*DCB.rL);
  //        for (int i=Old; i<m_Components.GetSize(); i++)
  //          {
  //          m_Components[i].m_CIndex=-1;
  //          m_Components[i].m_Fraction=0.0;
  //          }
  //        }
  //      DCB.L=m_Components.GetSize();
  //      return 1;
  //    }
  return 0;
  }

//--------------------------------------------------------------------------

flag CXBlk_Bleed::ValidateData(ValidateDataBlk & VDB)
  {
  m_Frac=Range(0.0, m_Frac, 1.0);
  return CBleedBlock::ValidateData(VDB); 
  }

//--------------------------------------------------------------------------

void CXBlk_Bleed::EvalProducts(SpConduit &QPrd, double Po, double FinalTEst)
  {
  switch (m_Type)                     
    {
    case Type_QmBleed:
      {
      double Qm=QPrd.QMass();
      double QmB=Min(Qm, m_QmBleed);
      //m_pBleedBase->DiscardCd().QSetM(Qf, som_ALL, QmB, Qf.Press()); 
      SnkIO.Cd.QSetM(QPrd, som_ALL, QmB, QPrd.Press()); 
      QPrd.QAdjustQmTo(som_ALL, Qm-QmB); 
      break;
      }
    case Type_QmProduct:
      {
      double Qm=QPrd.QMass();
      double QmB=Min(Qm, GEZ(Qm-m_QmProduct));
      SnkIO.Cd.QSetM(QPrd, som_ALL, QmB, QPrd.Press()); 
      QPrd.QAdjustQmTo(som_ALL, Qm-QmB); 
      break;
      }
    case Type_FracBleed:
      {
      double Qm=QPrd.QMass();
      double QmB=Qm*m_Frac;
      SnkIO.SetM(QPrd, som_ALL, QmB); 
      QPrd.QAdjustQmTo(som_ALL, Qm-QmB); 
      break;
      }
    }
  //SpConduit & Discard = m_pBleedBase->DiscardCd();
  //Discard.QZero();
  //SpMArray OldVap;

  //Qf.SetPress(Po);
  //double H=Qf.totHf();
  //for (int i=0; i<m_Components.GetSize(); i++)
  //  {
  //  CAdjustComp  & EC = m_Components[i];
  //  CComponent & C  = gs_CDB[EC.m_CIndex];
  //  int iLiq=C.LiqPhInx();
  //  int iVap=C.VapPhInx();
  //  double Liq=Qf.VMass[iLiq];
  //  double Vap=Qf.VMass[iVap];
  //  double D=Liq*EC.m_Fraction;

  //  Qf.SetVValue(iLiq, Liq-D);
  //  Qf.SetVValue(iVap, Vap+D);

  //  Discard.SetVValue(iVap, D);
  //  OldVap.SetVValue(iVap, Vap);
  //  }
  //Qf.Set_totHf(H);

  //double T=Qf.Temp();
  //double P=Qf.Press();
  //for (int i=0; i<m_Components.GetSize(); i++)
  //  {
  //  CAdjustComp  & EC = m_Components[i];
  //  int iVap = gs_CDB[EC.m_CIndex].VapPhInx();
  //  switch (EC.m_Dest)
  //    {
  //    case Dest_Mix:
  //      // Qf is at correct conditions
  //      Discard.SetVValue(iVap, 0.0);
  //      break;
  //    case Dest_Discard:
  //      Qf.SetVValue(iVap, OldVap[iVap]); // reset to original vapour mass
  //      break;
  //    }
  //  }
  //Qf.SetTempPress(T, P);
  //Discard.SetTempPress(T, P);
  };

void CXBlk_Bleed::EvalProductsPipe(SpConduit & QPrd, double Len, double Diam, double Po, double FinalTEst)
  {
  EvalProducts(QPrd, Po, FinalTEst);
  };

//=========================================================================
//
//
//
//=========================================================================

#endif