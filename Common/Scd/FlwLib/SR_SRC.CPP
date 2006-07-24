//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __SR_SRC_CPP
#include "sp_db.h"
#include "sp_model.h"
#include "sr_src.h"

#define dbgReaction  WITHDEBUG
#if dbgReaction
#include "dbgmngr.h"
static CDbgMngr dbgRctSrcSnks("RctEqns", "SrcSnks");
#endif

//===========================================================================
//
//                          Source Reaction
//
//===========================================================================

_FWDDEF(CR_SrcTerm);
_FWDDEF(CR_Src);

//===========================================================================


IMPLEMENT_REACTION(CR_Src, "SR_Src", "", TOC_SYSTEM, "Source", "Source of Material");

CR_Src::CR_Src(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
CReaction(pClass_, Tag_, pAttach, eAttach),
Spcs(*(new CR_SrcTermArray))
  {   
  Clear();
  //dbgpln("-------------------------------- CR_Src: Cons");
  }

//---------------------------------------------------------------------------

CR_Src::~CR_Src()
  {
  Clear();
  delete &Spcs;
  //dbgpln("-------------------------------- CR_Src: Dest");
  }

//---------------------------------------------------------------------------

void CR_Src::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this, NULL, NULL, DDB_OptPage))
    {
    Strng Es("Source:");
    for (int k = 0; k < NSpcs(); k++)
      {
      if (k>0) Es+=",";
      Es+=Spcs[k].SpcName();
      }
    if (Es.Length()>40)
      {
      Es.SetLength(37);
      Es+="...";
      }
    DDB.Text(Es());
    DDB.String("Reaction", "", DC_, "", xidRCTSrcReaction, this, isResult|InitHidden|noFileAtAll);

    for (k = 0; k < NSpcs(); k++)
      {
      Strng Spc(Spcs[k].SpcName());
      if (PrjFileVerNo()<22)
        Spc+=".Dmnd";
      DDB.Double(Spc(),   "",        DC_Qm,     "kg/s",  &Spcs[k].Demand, this, isResult|0);
      }

    DDB.Double("totEnthalpy", "HsGain",     DC_Pwr,     "kJ/s",     xidRCTSrcHs, this, isResult|0);
    DDB.Double("",            "HfGain",     DC_Pwr,     "kJ/s",     xidRCTSrcHf, this, isResult|0);
    }
  DDB.EndStruct();
  };

//---------------------------------------------------------------------------

flag CR_Src::DataXchg(DataChangeBlk & DCB)
  {
  int i;
  switch (DCB.lHandle)
    {
    case xidRCTSrcReaction:
      {
      m_DispEqnStr = "Source:";
      for (int k = 0; k < NSpcs(); k++)
        {
        if (k>0) m_DispEqnStr+=", ";
        m_DispEqnStr+=Spcs[k].SpcName();
        }
      DCB.pC = m_DispEqnStr();
      return 1;
      }
    case xidRCTSrcHf:
      DCB.D=0.0;
      for (i = 0; i < NSpcs(); i++) 
        {
        int SpcId=Spcs[i].SpcId;
        DCB.D+=Spcs[i].Demand*SDB[SpcId].msHf(RB.Fidelity(), RB.m_TempKFeed, RB.m_Press, RB.Ovr(), RB.MassArray());
        }
      return 1;
    case xidRCTSrcHs:
      DCB.D=0.0;
      for (i = 0; i < NSpcs(); i++) 
        {
        int SpcId=Spcs[i].SpcId;
        DCB.D+=Spcs[i].Demand*SDB[SpcId].msHs(RB.Fidelity(), RB.m_TempKFeed, RB.m_Press, RB.Ovr(), RB.MassArray());
        }
      return 1;
    }

  return 0;
  };

//---------------------------------------------------------------------------

flag CR_Src::ValidateData(ValidateDataBlk & VDB)
  {
  return True;
  };

//---------------------------------------------------------------------------

void CR_Src::Clear()
  {
  Spcs.SetSize(0);
  //  m_dHs=0.0;
  //  dHf=0;
  StructureChanged(this);
  }

//---------------------------------------------------------------------------

eScdReactionBasis CR_Src::Basis()
  {
  return RctSource;
  };

//---------------------------------------------------------------------------

void CR_Src::dbgDump(pchar pTitle)
  {
  dbgp("%s",pTitle);
  for (int k = 0; k < NSpcs(); k++)
    dbgp("%s %3.1f %s",k>0 ? "+" : " ", Spcs[k].Demand,Spcs[k].SpcName());
  dbgpln("");
  };

//---------------------------------------------------------------------------

flag CR_Src::ParseTerm(CR_SrcTerm &Term)
  {
  CRCTTokenFile &TF=*RB.pTkns;
  Strng Tkn(TF.NextToken());
  Term.SpcName=Tkn();
  Term.SpcId=SDB.Find(Term.SpcName());
  if (Term.SpcId<0)
    CReactionBlock::Error(&RB, 'B', 3, Term.SpcName());
  return (Term.SpcId>=0);
  }

//---------------------------------------------------------------------------

void CR_Src::Parse(CRCTTokenFile &TF)
  {
  //dbgpln("-------------------------------- CR_Src: Parse");
  //CRCTTokenFile &TF=*RB.pTkns;
  Strng Tkn;
  CR_SrcTerm ATerm;
  Clear();

  //Reactants
  Spcs.SetSize(0, 5);
  ParseTerm(ATerm);
  Spcs.Add(ATerm);                                           
  while (TF.TokenIs(","))
    {
    ParseTerm(ATerm);
    Spcs.Add(ATerm);
    }
  StructureChanged(this);  
  }

//---------------------------------------------------------------------------

void CR_Src::SetUp()
  {
  //Control().SetUp();
  }

//----------------------------------------------------------------------------

dword CR_Src::Check()
  {
  dwState=0;
  return dwState;
  }

//----------------------------------------------------------------------------

void CR_Src::Zero()
  {
  for (int i=0; i<Spcs.GetSize(); i++)
    Spcs[i].Zero();
  };

//---------------------------------------------------------------------------

dword CR_Src::GetState()
  {
  return dwState;
  };

//---------------------------------------------------------------------------

LPCTSTR CR_Src::GetRctString(dword Options)
  {
  m_AsString="";
  if (Options & EAS_Rct)
    {
    for (int i=0; i<Spcs.GetSize(); i++)
      {
      if (i>0)
        m_AsString+=", ";
      m_AsString+= Spcs[i].SpcName();
      }
    }
  return m_AsString;
  };

//---------------------------------------------------------------------------

//double CR_Src::ReactingMoles()
//  {
//  return 0.0;
//  }


//---------------------------------------------------------------------------
//
//void CR_Src::StartStep()
//  {
//  }
//
////---------------------------------------------------------------------------

void CR_Src::SrcSnkStart()
  {
#if dbgReaction
  if (dbgRctSrcSnks())
    dbgpln("-------------------------------- CR_Src: Start");
#endif

  //  m_dHs=0.0;
  //  dHf=0.0;

  AddMoles=0.0;
  for (int s=0; s<SDB.Count(); s++)
    AddMoles=Max(AddMoles, RB.Moles[s]);

  AddMoles*=1000.0;
  for (int i = 0; i < NSpcs(); i++) 
    {
    int SpcId=Spcs[i].SpcId;
    RB.Moles.VValue[SpcId]+=AddMoles;
#if dbgReaction
    if (dbgRctSrcSnks())
      dbgpln("Src Start : %14.6f %14.6f %14s %s", AddMoles, RB.Moles.VValue[SpcId], "", SDB[SpcId].SymOrTag());
#endif
    }
#if dbgReaction
  if (dbgRctSrcSnks())
    dbgpln("--------------------------------");
#endif
  }

//---------------------------------------------------------------------------

void CR_Src::SrcSnkEnd(CRctSolveBlk & RSB)//SpMArray &dMass, double &dHeat)
  {
#if dbgReaction
  if (dbgRctSrcSnks())
    dbgpln("-------------------------------- CR_Src: End");
#endif
  for (int i = 0; i < NSpcs(); i++) 
    {
    int SpcId=Spcs[i].SpcId;
    double Remainder=RB.Moles[SpcId]-AddMoles;
    Spcs[i].Demand=(Remainder>=0.0) ? 0.0 : -Remainder;
    RB.Moles.VValue[SpcId]=Max(0.0, Remainder);
    Spcs[i].Demand*=SDB[SpcId].MoleWt();
    RSB.dMass.VValue[SpcId]+=Spcs[i].Demand;
#if dbgReaction
    if (dbgRctSrcSnks())
      dbgpln("Src End   : %14.6f %14.6f %14.6f %s", AddMoles, RB.Moles.VValue[SpcId], Spcs[i].Demand, SDB[SpcId].SymOrTag());
#endif
    }
  RSB.m_Hf_FdT=totDHf(RB.Model, RB.m_TempKFeed, RB.m_Press);
  RSB.m_Hf_PrT=totDHf(RB.Model, RB.m_TempKProd, RB.m_Press);
#if dbgReaction
  if (dbgRctSrcSnks())
    dbgpln("--------------------------------");
#endif
  }

//---------------------------------------------------------------------------

void CR_Src::SrcSnkAdjust(SpMArray & M)//SpMArray &dMass, double &dHeat)
  {
#if dbgReaction
  if (dbgRctSrcSnks())
    dbgpln("-------------------------------- CR_Src: Adjust");
#endif
  for (int i = 0; i < NSpcs(); i++) 
    {
    int SpcId=Spcs[i].SpcId;
    double Remainder=M[SpcId]-AddMoles;
    //Spcs[i].Demand=(Remainder>=0.0) ? 0.0 : -Remainder;
    M.VValue[SpcId]=Max(0.0, Remainder);

    double Demand=(Remainder>=0.0) ? 0.0 : -Remainder;

#if dbgReaction
    if (dbgRctSrcSnks())
      dbgpln("Src Adjust: Add:%14.6f Spc:%3i M[Spc]:%14.6f Dmnd:%14.6f %s", AddMoles, SpcId, M.VValue[SpcId], /*Spcs[i].*/Demand, SDB[SpcId].SymOrTag());
#endif
    }
#if dbgReaction
  if (dbgRctSrcSnks())
    dbgpln("--------------------------------");
#endif
  }

//---------------------------------------------------------------------------
#if DOPBSMOOTHING  
void CR_Src::GetMsAGData(CMSJoin & AG)
  {
  };
#endif
//---------------------------------------------------------------------------

flag CR_Src::CpsOK()
  {
  for (int i = 0; i < NSpcs(); i++) 
    if (!SDB[Spcs[i].SpcId].CpOK())
      return false;
  return true;
  }

//---------------------------------------------------------------------------

flag CR_Src::HfsOK()
  {
  for (int i = 0; i < NSpcs(); i++) 
    if (!SDB[Spcs[i].SpcId].H25OK())
      return false;
  return true;
  };

//---------------------------------------------------------------------------

//double CR_Src::totDHs(double T)
//  {
//  return m_dHs;
//  };

//---------------------------------------------------------------------------

double CR_Src::totDHf(SpModel *Mdl, double T, double P)
  {
  double dHf=0;
  for (int i = 0; i < NSpcs(); i++) 
    {
    int SpcId=Spcs[i].SpcId;
    dHf+=Spcs[i].Demand*SDB[SpcId].msHf(RB.Fidelity(), T, P, RB.Ovr(), RB.MassArray());
    }
  return dHf;
  };

//===========================================================================
//
//                          Sink Reaction
//
//===========================================================================

_FWDDEF(CR_SnkTerm);
_FWDDEF(CR_Snk);

//===========================================================================

IMPLEMENT_REACTION(CR_Snk, "SR_Snk", "", TOC_SYSTEM, "Sink", "Sink of Material");

CR_Snk::CR_Snk(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
CReaction(pClass_, Tag_, pAttach, eAttach),
Spcs(*(new CR_SnkTermArray))
  {
  Clear();
  }

//---------------------------------------------------------------------------

CR_Snk::~CR_Snk()
  {
  Clear();
  delete &Spcs;
  }

//---------------------------------------------------------------------------

void CR_Snk::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this, NULL, NULL, DDB_OptPage))
    {
    Strng Es("Sink:");
    for (int k = 0; k < NSpcs(); k++)
      {
      if (k>0) Es+=",";
      Es+=Spcs[k].SpcName();
      }
    if (Es.Length()>40)
      {
      Es.SetLength(37);
      Es+="...";
      }
    DDB.Text(Es());
    DDB.String("Reaction", "", DC_, "", xidRCTSnkReaction, this, isResult|InitHidden|noFileAtAll);

    for (k = 0; k < NSpcs(); k++)
      {
      Strng Spc(Spcs[k].SpcName());
      if (PrjFileVerNo()<22)
        Spc+=".Excs";
      DDB.Double(Spc(),   "",        DC_Qm,     "kg/s",  &Spcs[k].Excess, this, isResult|0);
      }

    DDB.Double("totEnthalpy", "HsGain",     DC_Pwr,     "kJ/s",     xidRCTSnkHs, this, isResult|0);
    DDB.Double("",            "HfGain",     DC_Pwr,     "kJ/s",     xidRCTSnkHf, this, isResult|0);
    }
  DDB.EndStruct();
  };

//---------------------------------------------------------------------------

flag CR_Snk::DataXchg(DataChangeBlk & DCB)
  {
  int i;
  switch (DCB.lHandle)
    {
    case xidRCTSnkReaction:
      {
      m_DispEqnStr = "Sink:";
      for (int k = 0; k < NSpcs(); k++)
        {
        if (k>0) m_DispEqnStr+=", ";
        m_DispEqnStr+=Spcs[k].SpcName();
        }
      DCB.pC = m_DispEqnStr();
      return 1;
      }
    case xidRCTSnkHf:
      //DCB.D=totDHf(RB.TempKFeed);
      DCB.D=0.0;
      for (i = 0; i < NSpcs(); i++) 
        {
        int SpcId=Spcs[i].SpcId;
        DCB.D+=Spcs[i].Excess*SDB[SpcId].msHf(RB.Fidelity(), RB.m_TempKFeed, RB.m_Press, RB.Ovr(), RB.MassArray());
        }
      return 1;
    case xidRCTSnkHs:
      DCB.D=0.0;
      for (i = 0; i < NSpcs(); i++) 
        {
        int SpcId=Spcs[i].SpcId;
        DCB.D+=Spcs[i].Excess*SDB[SpcId].msHs(RB.Fidelity(), RB.m_TempKFeed, RB.m_Press, RB.Ovr(), RB.MassArray());
        }
      return 1;
    }
  return 0;
  };

//---------------------------------------------------------------------------

flag CR_Snk::ValidateData(ValidateDataBlk & VDB)
  {
  return True;
  };

//---------------------------------------------------------------------------

void CR_Snk::Clear()
  {
  Spcs.SetSize(0);
  //  m_dHs=0.0;
  //  dHf=0.0;
  StructureChanged(this);
  }

//---------------------------------------------------------------------------

eScdReactionBasis CR_Snk::Basis()
  {
  return RctSink;
  };

//---------------------------------------------------------------------------

void CR_Snk::dbgDump(pchar pTitle)
  {
  dbgp("%s",pTitle);
  for (int k = 0; k < NSpcs(); k++)
    dbgp("%s %3.1f %s",k>0 ? "+" : " ", Spcs[k].Excess,Spcs[k].SpcName());
  dbgpln("");
  };

//---------------------------------------------------------------------------

flag CR_Snk::ParseTerm(CR_SnkTerm &Term)
  {
  CRCTTokenFile &TF=*RB.pTkns;
  Strng Tkn(TF.NextToken());
  int i=0;
  while (isdigit(Tkn[i]) || Tkn[i]=='.') i++;
  if (i>0)
    {
    if (i==Tkn.Length())
      {
      //Term.Value=atof(Tkn());
      Term.SpcName=TF.NextToken();
      }
    else
      {
      //Term.Value=atof(Tkn.Left(i));
      Term.SpcName=Tkn.Right(Tkn.Length()-i);
      }
    }
  else
    {
    //Term.Value=1.0;
    Term.SpcName=Tkn();
    }
  Term.SpcId=SDB.Find(Term.SpcName());
  if (Term.SpcId<0)
    CReactionBlock::Error(&RB, 'B', 3, Term.SpcName());

  return (Term.SpcId>=0);
  }

//---------------------------------------------------------------------------

void CR_Snk::Parse(CRCTTokenFile &TF)
  {
  Strng Tkn;
  CR_SnkTerm ATerm;
  Clear();

  //Reactants
  Spcs.SetSize(0, 5);
  ParseTerm(ATerm);
  Spcs.Add(ATerm);                                           
  while (TF.TokenIs(","))
    {
    ParseTerm(ATerm);
    Spcs.Add(ATerm);
    }
  StructureChanged(this);  
  }

//---------------------------------------------------------------------------

void CR_Snk::SetUp()
  {
  }

//----------------------------------------------------------------------------

dword CR_Snk::Check()
  {
  dwState=0;
  return dwState;
  }

//----------------------------------------------------------------------------

void CR_Snk::Zero()
  {
  for (int i=0; i<Spcs.GetSize(); i++)
    Spcs[i].Zero();
  };

//---------------------------------------------------------------------------

dword CR_Snk::GetState()
  {
  return dwState;
  };

//---------------------------------------------------------------------------

LPCTSTR CR_Snk::GetRctString(dword Options)
  {
  m_AsString="";
  if (Options & EAS_Rct)
    {
    for (int i=0; i<Spcs.GetSize(); i++)
      {
      if (i>0)
        m_AsString+=", ";
      m_AsString+= Spcs[i].SpcName();
      }
    }
  return m_AsString;
  };

//---------------------------------------------------------------------------

//double CR_Snk::ReactingMoles()
//  {
//  return 0.0;
//  }


//---------------------------------------------------------------------------
//
//void CR_Snk::StartStep()
//  {
//  }
//
////---------------------------------------------------------------------------

void CR_Snk::SrcSnkStart()
  {
  }

//---------------------------------------------------------------------------

void CR_Snk::SrcSnkEnd(CRctSolveBlk & RSB)//SpMArray &dMass, double &dHeat)
  {
  for (int i = 0; i < NSpcs(); i++) 
    {
    int SpcId=Spcs[i].SpcId;
    Spcs[i].Excess=RB.Moles[SpcId];
    RB.Moles.VValue[SpcId]=0.0;
    Spcs[i].Excess*=SDB[SpcId].MoleWt();
    RSB.dMass.VValue[SpcId]+=Spcs[i].Excess;
    }
  RSB.m_Hf_FdT=totDHf(RB.Model, RB.m_TempKFeed, RB.m_Press);
  RSB.m_Hf_PrT=totDHf(RB.Model, RB.m_TempKProd, RB.m_Press);
  }

//---------------------------------------------------------------------------

void CR_Snk::SrcSnkAdjust(SpMArray & M)//SpMArray &dMass, double &dHeat)
  {
  for (int i = 0; i < NSpcs(); i++) 
    {
    int SpcId=Spcs[i].SpcId;
    M.VValue[SpcId]=0.0;
    }
  }

//---------------------------------------------------------------------------
#if DOPBSMOOTHING  
void CR_Snk::GetMsAGData(CMSJoin & AG)
  {
  };
#endif
//---------------------------------------------------------------------------

flag CR_Snk::CpsOK()
  {
  for (int i = 0; i < NSpcs(); i++) 
    if (!SDB[Spcs[i].SpcId].CpOK())
      return false;
  return true;
  }

//---------------------------------------------------------------------------

flag CR_Snk::HfsOK()
  {
  for (int i = 0; i < NSpcs(); i++) 
    if (!SDB[Spcs[i].SpcId].H25OK())
      return false;
  return true;
  };

//---------------------------------------------------------------------------

//double CR_Snk::totDHs(double T)
//  {
//  return m_dHs;
//  };

//---------------------------------------------------------------------------

double CR_Snk::totDHf(SpModel *Mdl,double T, double P)
  {
  double dHf=0;
  for (int i=0; i < NSpcs(); i++) 
    {
    int SpcId=Spcs[i].SpcId;
    dHf+=Spcs[i].Excess*SDB[SpcId].msHf(RB.Fidelity(), T, P, RB.Ovr(), RB.MassArray());
    }
  return dHf;
  };

//===========================================================================
//
//                          HeatExchange Reaction
//
//===========================================================================

_FWDDEF(CR_HXTerm);
_FWDDEF(CR_HX);

//===========================================================================

IMPLEMENT_REACTION(CR_HX, "SR_HX", "", TOC_SYSTEM, "HeatExchange", "Heat Exchange");

CR_HX::CR_HX(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
CReaction(pClass_, Tag_, pAttach, eAttach)
  {
  Clear();
  }

//---------------------------------------------------------------------------

CR_HX::~CR_HX()
  {
  Clear();
  }

//---------------------------------------------------------------------------

void CR_HX::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this, NULL, NULL, DDB_OptPage))
    {
    bool UseOldVer=PrjFileVerNo()<=79;
    switch (m_iWhat)
      {
      case CR_HX_ApproachAmb:
        if (m_bLocalAmbT)
          m_ddAmbTemp.BuildDataDefn(DDB, "Ambient",     "",   DC_T,    "C",    xidRCTHXAmbTemp,   this, "Cfg", "Rqd", isParm, 0, UseOldVer);
        m_ddTFrac.BuildDataDefn(DDB, "ApproachAmbient", "",   DC_Frac, "%",    xidRCTHXTFrac,     this, "Cfg", "Rqd", isParm, 0, UseOldVer);
        break;
      case CR_HX_ApproachT:
        m_ddTargetT.BuildDataDefn(DDB, "TargetT",      "",    DC_T,    "C",    xidRCTHXTargetT,   this, "Cfg", "Rqd", isParm, 0, UseOldVer);
        m_ddTFrac.BuildDataDefn(DDB, "ApproachT",      "",    DC_Frac, "%",    xidRCTHXTFrac,     this, "Cfg", "Rqd", isParm, 0, UseOldVer);
        break;
      case CR_HX_Pwr:
        m_ddPwr.BuildDataDefn(DDB, "PowerInput",       "",    DC_Pwr,  "kJ/s", xidRCTHXPwr,       this, "Cfg", "Rqd", isParm, 0, UseOldVer);
        break;
      case CR_HX_FinalT:
        m_ddRqdFinalT.BuildDataDefn(DDB, "FinalTRqd",  "",    DC_T,    "C",    xidRCTHXRqdFinalT, this, "Cfg", "Rqd", 0, m_bMaintFeedT ? isResult : isParm, UseOldVer);
        break;
      case CR_HX_Electrolysis:
        m_ddElectEff.BuildDataDefn(DDB, "ElectEff",    "",    DC_Frac, "%",    xidRCTHXElectEff,  this, "Cfg", "Rqd", isParm, 0, UseOldVer);
        break;
      default:;
      }
    DDB.Double("HeatGain",   "HsGain",       DC_Pwr,  "kJ/s", &m_dHs, this, isResult|0);
    }
  DDB.EndStruct();
  };

//---------------------------------------------------------------------------

flag CR_HX::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidRCTHXAmbTemp   :  return m_ddAmbTemp  .DataXchg(DCB, xidRCTHXAmbTemp  , this);
    case xidRCTHXTFrac     :  return m_ddTFrac    .DataXchg(DCB, xidRCTHXTFrac    , this);
    case xidRCTHXTargetT   :  return m_ddTargetT  .DataXchg(DCB, xidRCTHXTargetT  , this);
    case xidRCTHXPwr       :  return m_ddPwr      .DataXchg(DCB, xidRCTHXPwr      , this);
    case xidRCTHXRqdFinalT :  return m_ddRqdFinalT.DataXchg(DCB, xidRCTHXRqdFinalT, this);
    case xidRCTHXElectEff  :  return m_ddElectEff .DataXchg(DCB, xidRCTHXElectEff , this);
    }
  return 0;
  };

//---------------------------------------------------------------------------

flag CR_HX::ValidateData(ValidateDataBlk & VDB)
  {
  return True;
  };

//---------------------------------------------------------------------------

void CR_HX::Clear()
  {
  m_iWhat=0;
  m_bMaintFeedT=0;
  m_dHs=0.0;
  m_ddHtXfer=0.0;
  m_bLocalAmbT=0;
  m_ddAmbTemp=C2K(25.0);
  m_ddUA=0.0;
  m_ddTFrac=0.0;
  m_ddPwr=0.0;
  m_ddRqdFinalT=C2K(25.0);
  m_ddTargetT=AmbientTemp();
  m_ddElectEff=1.0;

  m_theBasis=RctHX2;

  StructureChanged(this);
  }

//---------------------------------------------------------------------------

eScdReactionBasis CR_HX::Basis()
  {
  return m_theBasis;//RctHX2;//Control().Basis();
  };

//---------------------------------------------------------------------------

void CR_HX::dbgDump(pchar pTitle)
  {
  dbgp("%s",pTitle);
  //for (int k = 0; k < NSpcs(); k++)
  //  dbgp("%s %3.1f %s",k>0 ? "+" : " ", Spcs[k].Excess,Spcs[k].SpcName());
  //dbgp(" = ");
  //for (k = 0; k < NProducts(); k++)
  //  dbgp("%s %3.1f %s",k>0 ? "+" : " ", Product[k].NoOfMoles,Product[k].SpcName());
  dbgpln("");
  };

//---------------------------------------------------------------------------

void CR_HX::Parse(CRCTTokenFile &TF)
  {
  Clear();
  flag Ok=0;
  m_bLocalAmbT=false;
  while (1)
    {
    if (TF.TokenIs("Ambient"))
      {
      if (m_iWhat!=CR_HX_ApproachAmb)
        m_ddTFrac=1.0; //set default
      m_iWhat=CR_HX_ApproachAmb;
      m_ddAmbTemp=AmbientTemp();
      if (TF.TokenIs("="))
        {
        m_bLocalAmbT=true;
        m_ddAmbTemp=C2K(TF.DoubleToken());
        }
      m_theBasis=RctHX2;
      Ok=1;
      }
    else if (TF.TokenIs("ApproachAmbient"))
      {
      if (m_iWhat!=CR_HX_ApproachAmb)
        {
        m_bLocalAmbT=false;
        m_ddAmbTemp=AmbientTemp(); //set default
        }
      m_iWhat=CR_HX_ApproachAmb;
      m_ddTFrac=1.0;
      if (TF.TokenIs("="))
        {
        if (OrigPrjFileVerNo()>=80)//79)
          {
          m_ddTFrac=TF.DoubleToken();
          if (TF.TokenIs("%"))
            m_ddTFrac=m_ddTFrac*0.01;
          }
        else
          m_ddTFrac=TF.DoubleToken()*0.01;
        }
      m_theBasis=RctHX2;
      Ok=1;
      }
    else if (TF.TokenIs("Power"))
      {
      m_iWhat=CR_HX_Pwr;
      m_ddPwr=0.0;
      if (TF.TokenIs("="))
        m_ddPwr=TF.DoubleToken();
      m_theBasis=RctPwr;
      Ok=1;
      }
    else if (TF.TokenIs("PwrPerT"))//not used???
      {
      TF.CheckToken("=");
      m_ddHtXfer=TF.DoubleToken();
      m_theBasis=RctPwr;
      Ok=1;
      }
    else if (TF.TokenIs("UA"))//not used???
      {
      TF.CheckToken("=");
      m_ddUA=TF.DoubleToken();
      m_theBasis=RctHX2;
      Ok=1;
      }
    // Done before T Estimated.-------------
    else if (TF.TokenIs("FinalT"))
      {
      m_iWhat=CR_HX_FinalT;
      m_ddRqdFinalT=AmbientTemp();
      m_bMaintFeedT=0;
      if (TF.TokenIs("="))
        {
        m_bLocalAmbT=true;
        if (TF.TokenIs("FeedT"))
          m_bMaintFeedT=1;
        else
          m_ddRqdFinalT=C2K(TF.DoubleToken());
        }
      m_theBasis=RctHX1;
      Ok=1;
      }
    else if (TF.TokenIs("TargetT")) // approach between FeedT and Target(default=AtmosT)
      {
      if (m_iWhat!=CR_HX_ApproachT)
        m_ddTFrac=1.0; //set default
      m_iWhat=CR_HX_ApproachT;
      m_ddTargetT=AmbientTemp();
      m_bMaintFeedT=0;
      if (TF.TokenIs("="))
        {
        if (TF.TokenIs("FeedT"))
          m_bMaintFeedT=1;
        else
          {
          m_bLocalAmbT=true;
          m_ddTargetT=C2K(TF.DoubleToken());
          }
        }
      m_theBasis=RctHX1;
      Ok=1;
      }
    else if (TF.TokenIs("ApproachT")) // approach between FeedT and Target(default=AtmosT)
      {
      if (m_iWhat!=CR_HX_ApproachT)
        {
        m_ddTargetT=AmbientTemp(); //set default
        m_bMaintFeedT=0;
        }
      m_iWhat=CR_HX_ApproachT;
      m_ddTFrac=1.0;
      if (TF.TokenIs("="))
        {
        if (OrigPrjFileVerNo()>=80)//79)
          {
          m_ddTFrac=TF.DoubleToken();
          if (TF.TokenIs("%"))
            m_ddTFrac=m_ddTFrac*0.01;
          }
        else
          m_ddTFrac=TF.DoubleToken()*0.01;
        }
      m_theBasis=RctHX1;
      Ok=1;
      }
    else if (TF.TokenIs("Electrolysis"))
      {
      m_iWhat=CR_HX_Electrolysis;
      m_ddElectEff=1.0;
      if (TF.TokenIs("="))
        {
        if (OrigPrjFileVerNo()>=80)//79)
          {
          m_ddElectEff=TF.DoubleToken();
          if (TF.TokenIs("%"))
            m_ddElectEff=m_ddElectEff*0.01;
          }
        else
          m_ddElectEff=TF.DoubleToken()*0.01;
        }
      m_theBasis=RctHX1;
      Ok=1;
      }
    // -------------------------------------
    else if (TF.TokenIs(","))
      {
      Ok=0;
      }
    else
      break;
    }
  if (!Ok)
    RB.Error(&RB, 'B', 1); 
  }

//---------------------------------------------------------------------------

void CR_HX::SetUp()
  {
  //Control().SetUp();
  }

//----------------------------------------------------------------------------

dword CR_HX::Check()
  {
  dwState=0;
  return dwState;
  }

//----------------------------------------------------------------------------

void CR_HX::Zero()
  {
  m_ddHtXfer=0.0;
  m_ddUA=0.0;
  m_dHs=0.0;
  m_ddPwr=0.0;
  };

//---------------------------------------------------------------------------

dword CR_HX::GetState()
  {
  return dwState;
  };

//---------------------------------------------------------------------------

LPCTSTR CR_HX::GetRctString(dword Options)
  {
  m_AsString="";
  CString S;
  switch (m_iWhat)
    {
    case CR_HX_ApproachAmb:
      if (m_bLocalAmbT)
        m_AsString.Format("Ambient = %s, ", RctStrippedDouble(m_ddAmbTemp));
      S.Format("ApproachAmbient = %s ", RctStrippedDouble(m_ddTFrac*100));
      m_AsString+=S;
    case CR_HX_Pwr:
      m_AsString.Format("Power = %s", RctStrippedDouble(m_ddPwr));
      break;
    case CR_HX_FinalT:
      m_AsString.Format(m_bLocalAmbT ? "%s" : "%s = %s", "FinalT", m_bMaintFeedT ? "FeedT" : RctStrippedDouble(m_ddRqdFinalT));
      break;
    case CR_HX_ApproachT:
      if (m_bLocalAmbT)
        m_AsString.Format("TargetT = %s, ", RctStrippedDouble(m_ddAmbTemp));
      S.Format("ApproachT = %s %%", RctStrippedDouble(m_ddTFrac*100));
      m_AsString+=S;
    case CR_HX_Electrolysis:
      m_AsString.Format(fabs(m_ddElectEff-1.0)>1e-10 ? "%s = %s":"%s", "Electrolysis", RctStrippedDouble(m_ddElectEff*100));
      break;
    }

  return m_AsString;
  };

//---------------------------------------------------------------------------

//double CR_HX::ReactingMoles()
//  {
//  return 0.0;//Control().ReactingMoles();
//  }

//---------------------------------------------------------------------------

double LMTDCalc (double In1, double Out1, double In2, double Out2)
  {
  // LMTD return a negative value if duty > maxduty or there is a crossover

  double LSign = (In2 > In1) ? 1.0 : -1.0;    // hot side is 2 / 1 
  double Diff1 = LSign * (In2 - Out1);
  double Diff2 = LSign * (Out2 - In1);
  if (fabs(Diff2) > ZeroLimit)
    {
    double TmpDiff = (Diff1 / Diff2);
    if (TmpDiff > ZeroLimit)
      {
      if (fabs(1.0 - TmpDiff) > 0.01)
        return (Diff1 - Diff2) / Ln(TmpDiff);
      else
        return (Diff1 + Diff2) / 2.0;
      }
    else if (TmpDiff < ZeroLimit)
      return -1.0;         // crossover error
    };
  return ZeroLimit;
  };

//---------------------------------------------------------------------------

void CR_HX::StartStep()
  {
  }

//---------------------------------------------------------------------------

double CR_HX::HeatExchange(pSpModel M, double DHfTot, double TStart, double TEst, flag CnvrgStart)
  {
  if (CnvrgStart)
    m_dHs=0.0; // Assume No Change
  if (M->Moles() > 1.0e-10)
    {
    //double EstEndT=Temp+HeatOfReact/GTZ(Cp);
    switch(m_iWhat)
      {
      case CR_HX_Pwr:
        if (CnvrgStart)
          {
          if (RB.InEvalProducts() || RB.InEvalDerivs())
            m_dHs+=m_ddPwr;
          else
            m_dHs+=m_ddPwr*ICGetTimeInc();
          }
        break;
      case CR_HX_ApproachAmb:
        {
        // HeatSum (-) is Exothermic ie Hotter
        //double HeatLoss=m_ddTFrac*((HtI-HeatSum)-HtA); // (+) = Loss to ambient
        // Must Converge
        //double t=M->Temp();
        //double RqdT=m_ddTFrac*(m_dAmbTemp-TEst)+TEst; // (+) = Loss to ambient
        double AmbT = m_bLocalAmbT ? m_ddAmbTemp : AmbientTemp();
        if (fabs(AmbT-TEst)<1.0e-5) //what is a decent tolerance test?!!?!????
          {
          //do nothing because final T = Ambient T
          }
        else
          {
          double RqdT=m_ddTFrac*(AmbT-TStart)+TStart; // (+) = Loss to ambient
          double HtE=M->totHf(som_ALL, TEst, M->Press(), M->pMArray());        
          double HtR=M->totHf(som_ALL, RqdT, M->Press(), M->pMArray());        
          double HeatLoss=(HtR-HtE); // (+) = Loss to ambient
          m_dHs+=HeatLoss;
          }
        break;
        }
      case CR_HX_ApproachT:
        {
        if (m_bMaintFeedT || fabs(m_ddTargetT-TStart)<1.0e-8)  //what is a decent tolerance test?!!?!????
          {
          if (m_bMaintFeedT)
            m_ddTargetT=TStart;
          if (CnvrgStart)
            m_dHs += DHfTot;
          }
        else
          {
          // Must Converge
          double RqdT=m_ddTFrac*(m_ddTargetT-TStart)+TStart; // (+) = Loss to ambient
          double HtE=M->totHf(som_ALL, TEst, M->Press(), M->pMArray());        
          double HtR=M->totHf(som_ALL, RqdT, M->Press(), M->pMArray());        
          double HeatLoss=(HtR-HtE); // (+) = Loss to ambient
          m_dHs+=HeatLoss;
          }
        break;
        }
      case CR_HX_FinalT:
        {
        if (m_bMaintFeedT || fabs(m_ddRqdFinalT-TStart)<1.0e-8)  //what is a decent tolerance test?!!?!????
          {
          if (m_bMaintFeedT)
            m_ddRqdFinalT=TStart;
          if (CnvrgStart)
            m_dHs += DHfTot;
          }
        else
          {
          double HtE=M->totHf(som_ALL, TEst, M->Press(), M->pMArray());        
          double HtR=M->totHf(som_ALL, m_ddRqdFinalT, M->Press(), M->pMArray());        
          double HeatLoss=(HtR-HtE); // (+) = Loss
          m_dHs+=HeatLoss;
          }
        break;
        }
      case CR_HX_Electrolysis:
        {
        if (CnvrgStart)
          m_dHs += (DHfTot / m_ddElectEff);
        break;
        }
      }
    }
  return m_dHs;
  }

//===========================================================================
//
//
//
//===========================================================================

