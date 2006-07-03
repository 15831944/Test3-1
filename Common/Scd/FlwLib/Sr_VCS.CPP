//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "sc_defs.h"

#define  __SR_VCS_CPP

#include "sp_db.h"
#include "sp_model.h"
#include "sr_vcs.h"
#include "errorlog.h"
//#include "optoff.h"

//const char * FemError[] = 
//  {
//    " No errors ",
//    " Error: No unique solution - No. components < No. of elements ",
//    " Error: Linear programming solution unbound ",
//    " Error: Lin. prog. problem has no feasible solution ",
//    " Error: Convergence criteria not reached ",
//    " Error: Specie not in data base ",
//    " Error: Cannot invert matrix"
//  };

//const State : array[0..2] of PChar = ("Solid","Gas","Liquid");

//===========================================================================
//
//                          Free Energy Minimisation
//
//===========================================================================

IMPLEMENT_REACTION(CR_VCS, "SR_VCS", "", TOC_SYSTEM, "FreeEnergyMin", "Free Energy Minimisation");

//CR_VCS::CR_VCS(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach):
//  CReaction(pClass_, Tag_, pAttach, eAttach)
//  {
//  };
//
////---------------------------------------------------------------------------
//
//CR_VCS::~CR_VCS()
//  {
//  };
//
////---------------------------------------------------------------------------
//
//void CR_VCS::Clear() 
//  {
//  };
//
////---------------------------------------------------------------------------
//
//void CR_VCS::BuildDataDefn(DataDefnBlk & DDB)
//  {
//  if (DDB.BeginStruct(this, NULL, NULL, DDB_OptPage))
//    {
//    DDB.Text  ("TEST");
//    }
//  DDB.EndStruct();
//  };
//
////---------------------------------------------------------------------------
//
//flag CR_VCS::DataXchg(DataChangeBlk & DCB)
//  {
////  switch (DCB.lHandle)
////    {
////    case xidRCTName:
//  return 0;
//  };
//
////---------------------------------------------------------------------------
//
//flag CR_VCS::ValidateData(ValidateDataBlk & VDB)
//  {
//  return True;
//  };
//
////---------------------------------------------------------------------------
//
//void CR_VCS::Parse(CRCTTokenFile &TF) 
//  {
//  };
//
////---------------------------------------------------------------------------
//
//void CR_VCS::SetUp() 
//  {
//  };
//
////---------------------------------------------------------------------------
//
//dword CR_VCS::Check() 
//  {
//  dwState=CSR_NotAvail;
//  return dwState;
//  };
//
////---------------------------------------------------------------------------
//
//dword CR_VCS::GetState()
//  {
//  return dwState;
//  };
//
////---------------------------------------------------------------------------
//
//void CR_VCS::dbgDump(pchar pTitle)
//  {
//  };
//
////---------------------------------------------------------------------------
//
//void CR_VCS::StartStep()
//  {
//  }
//
////---------------------------------------------------------------------------
//
//void CR_VCS::EvalProducts(CNodeEvalIndex & NEI)
//  {
//  };
//
////---------------------------------------------------------------------------
//
//void CR_VCS::EvalDerivs()
//  {
//  };
//
////---------------------------------------------------------------------------
//
//void CR_VCS::ConvergeStates(CConvergeStateBlk &CSB)
//  {
//  };
//
////---------------------------------------------------------------------------
//
//void CR_VCS::EvalDiscrete()
//  {
//  };
//
////===========================================================================

//IMPLEMENT_REACTION(CR_VCS, "SR_React", "", TOC_SYSTEM, "Reactions", "Stoichiometric Equations");

CR_VCS::CR_VCS(pTagObjClass pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  CReaction(pClass_, Tag_, pAttach, eAttach),
  m_Species(*(new CR_VCSSpcArray))
  {
  //pControl=NULL;
  //pHeat=NULL;
//  pBlk=NULL;
  Clear();
  }

//---------------------------------------------------------------------------

CR_VCS::~CR_VCS()
  {
  Clear();
  delete &m_Species;
  }

//---------------------------------------------------------------------------

void CR_VCS::BuildDataDefn(DataDefnBlk & DDB)
  {
  if (DDB.BeginStruct(this, NULL, NULL, DDB_OptPage))
    {
    Strng Es;
    GetEqnAsString(Es, EAS_Rct);
    if (Es.Length()>40)
      {
      Es.SetLength(37);
      Es+="...";
      }
    DDB.Text(Es());
    //DDB.String("Reaction", "", DC_, "", xid_Reaction, this, 0|InitHidden|noFileAtAll);
    //DDB.String("State",    "", DC_, "", xid_RctState, this, 0|noFileAtAll);
    //
    //if (HasControl())
    //  Control().BuildDataDefn(DDB);
    //
    //int iRNo=0;
    //if (pHeat)
    //  iRNo=Heat().iReactantNo;
    //int iRctSpc=Reactant[iRNo].SpcId;
    //
    //DDB.PushUserInfo(iRNo);
    //CCnvIndex Ldc, Mdc;
    //pchar pLCnvTxt, pMCnvTxt;
    //SDB.AddSpCnv(DC_QKgMl, SDB[iRctSpc].SymOrTag(), "", Ldc, pLCnvTxt);
    //SDB.AddSpCnv(DC_Qm,    SDB[iRctSpc].SymOrTag(), "", Mdc, pMCnvTxt);
    //DDB.Double("ReactingMoles", "mlXfer", Ldc, pLCnvTxt, xid_dMlXfer, this, 0|InitHidden);
    //DDB.Double("ReactingMass",  "msXfer", Mdc, pMCnvTxt, xid_dMsXfer, this, 0);
    //DDB.PopUserInfo();
    //if (pHeat)
    //  Heat().BuildDataDefn(DDB);
    }
  DDB.EndStruct();
  };

//---------------------------------------------------------------------------

flag CR_VCS::DataXchg(DataChangeBlk & DCB)
  {
  /*
  switch (DCB.lHandle)
    {
    case xid_Reaction:
      {
      //if(DCB.rpC != NULL)
      //  {
      //  }
      GetEqnAsString(EqnStr, EAS_Rct);
      DCB.pC = EqnStr();
      return 1;
      }
    case xid_RctState:
      {
      sState="";
      GetState();
      if ((dwState & (CRS_BadCps|CRS_BadHfs))==(CRS_BadCps|CRS_BadHfs))
        sState+="Bad Hf's & Cp's";
      else if (dwState & CRS_BadCps)
        sState+="Bad Cp's";
      else if (dwState & CRS_BadHfs)
        sState+="Bad Hf's";
      if (sState.Length() > 0)
        DCB.dwRetFlags=DDEF_INERROR;
      else
        sState="OK";
      DCB.pC=sState();
      return 1;
      }
    case xid_dMlXfer:
      {
      CR_VCSTerm &R=Reactant[DCB.dwUserInfo];
      DCB.D=dMlXfer*R.NoOfMoles/ICGetTimeInc();
      return 1;
      }
    case xid_dMsXfer:
      {
      CR_VCSTerm &R=Reactant[DCB.dwUserInfo];
      DCB.D=dMlXfer*R.NoOfMoles*SDB[R.SpcId].MoleWt()/ICGetTimeInc();
      return 1;
      }
    }
  if (HasControl())
    if (Control().DataXchg(DCB))
      return 1;
  if (pHeat)
    if (Heat().DataXchg(DCB))
      return 1;
  */
  return 0;
  };

//---------------------------------------------------------------------------

flag CR_VCS::ValidateData(ValidateDataBlk & VDB)
  {
  return True;
  };

//---------------------------------------------------------------------------

void CR_VCS::Clear()
  {
  ClrCI(1);
  ClrCI(2);
  //iDirn=0;

  m_Species.SetSize(0);
//  Product.SetSize(0);

//  delete pBlk;
  //delete pControl;
  //delete pHeat;
  //pControl=NULL;
  //pHeat=NULL;
//  pBlk=NULL;

//  dRctScl=1.0;
//  
//	//fFwdRct=0;
//	//fRevRct=0;
//	fAutoBalance=0;
	fDone=0;
	//iLimitSpc=-1;
//	dMlXfer=0.0;
//	dRctScl=1.0;
//	dEqK0=0.0;
//	dK0=1.0;
//	dExtent=1.0;

  sState="";
  dwState=0;
//  fAutoBalance=0;
  StructureChanged(this);
  }

//---------------------------------------------------------------------------

eScdReactionBasis CR_VCS::Basis()
  {
//  if (HasControl())
//    return Control().Basis();
  return RctEquilibrium;
  };

//---------------------------------------------------------------------------

void CR_VCS::GetEqnAsString(Strng & Es, dword Options)
  {
  char Buff[4096];
  char *p=Buff;
  for (int k = 0; k < NSpecies(); k++)
    {
    p+=sprintf(p,"%s",m_Species[k].SpcName());
    if (k < NSpecies()-1)
      p+=sprintf(p, ", ");
    }
  //if (FwdRct() && RevRct())
  //  p+=sprintf(p,"<=> ");
  //else if (FwdRct())
  //  p+=sprintf(p,"=> ");
  //else if (RevRct())
  //  p+=sprintf(p,"<= ");
//  p+=sprintf(p," = ");
//  for (k = 0; k < NProducts(); k++)
//    {
//    if (fabs(Product[k].NoOfMoles-1.0) > 1.0e-3)
//      p+=sprintf(p,"%g ",Product[k].NoOfMoles);
//    p+=sprintf(p,"%s ",Product[k].SpcName());
//    if (k < NProducts()-1)
//      p+=sprintf(p, "+ ");
//    }
//
  Es=Buff;
  };

//---------------------------------------------------------------------------

void CR_VCS::dbgDump(pchar pTitle)
  {
  dbgp("%s",pTitle);
//  for (int k = 0; k < NReactants(); k++)
//    dbgp("%s %3.1f %s",k>0 ? "+" : " ", Reactant[k].NoOfMoles,Reactant[k].SpcName());
//  dbgp(" = ");
//  for (k = 0; k < NProducts(); k++)
//    dbgp("%s %3.1f %s",k>0 ? "+" : " ", Product[k].NoOfMoles,Product[k].SpcName());
  dbgpln("");
  };

//---------------------------------------------------------------------------

flag CR_VCS::ParseSpc(CR_VCSSpc &Spc)
  {
  CRCTTokenFile &TF=*RB.pTkns;
  Strng Tkn(TF.NextToken());
  
  Spc.SpcId=SDB.Find(Tkn());

  //if (Spc.SpcId<0) // Tkn does not contain a valid Specie Name
  //  {
  //  int i=0;
  //  while (i<Tkn.Length() && (isdigit(Tkn[i]) || Tkn[i]=='.' || Tkn[i]=='/')) i++;
  //  if (i>0)
  //    {
  //    if (i==Tkn.Length())
  //      {
  //      Spc.NoOfMoles=FractionToDouble(Tkn());
  //      Spc.SpcName=TF.NextToken();
  //      }
  //    else
  //      {
  //      Strng sss;
  //      sss = Tkn.Left(i);
  //      Spc.NoOfMoles=FractionToDouble(sss());
  //      Spc.SpcName=Tkn.Right(Tkn.Length()-i);
  //      }
  //    Spc.SpcId=SDB.Find(Spc.SpcName());
  //    }
  //  else
  //    Term.SpcName=Tkn();
  //  }
  //else
  //  {
  //  Spc.NoOfMoles=1.0;
    Spc.SpcName=Tkn();
  //  }
  
  if (Spc.SpcId<0)
    CReactionBlock::Error(&RB, 'B', 3, Spc.SpcName());
 // if (Spc.NoOfMoles<1.0e-6)
 //   CReactionBlock::Error(&RB, 'B', 12, Spc.SpcName());

  return (Spc.SpcId>=0);
  }

//---------------------------------------------------------------------------

void CR_VCS::Parse(CRCTTokenFile &TF)
  {
  Strng Tkn;
  CR_VCSSpc ASpc;
  Clear();

  flag fFwdRctDef=True;
  flag fRevRctDef=True;

  //Reactants
  m_Species.SetSize(0, 5);
  ParseSpc(ASpc);
  m_Species.Add(ASpc);                                           
  while (TF.TokenIs(","))
    {
    ParseSpc(ASpc);
    m_Species.Add(ASpc);
    }

//  if (TF.TokenIs("<->"))
//    {
//    fFwdRctDef=true;
//    fRevRctDef=true;
//    }
//  else if (TF.TokenIs("<-"))
//    {
//    fFwdRctDef=false;
//    fRevRctDef=true;
//    }
//  else if(TF.TokenIs("->"))
//    {
//    fFwdRctDef=true;
//    fRevRctDef=false;
//    }
//  else
//    TF.CheckToken("=");
//
//  //Products
//  Product.SetSize(0, 5);
//  ParseTerm(ATerm);
//  Product.Add(ATerm);
//  while (TF.TokenIs("+"))
//    {
//    ParseTerm(ATerm);
//    Product.Add(ATerm);
//    }
//
  for (flag Done=0; !Done; )
    {
    //if (TF.TokenIs("Extent"))
    //  {
    //  fFwdRct=fFwdRctDef;
    //  fRevRct=fRevRctDef;
    //  if (TF.TokenIs("Fwd"))
    //    {
    //    fFwdRct=True;
    //    fRevRct=TF.TokenIs("Rev");
    //    }
    //  else if (TF.TokenIs("Rev"))
    //    {
    //    fFwdRct=True;
    //    fRevRct=False;
    //    }
    //
    //  TF.CheckToken(":");
    //
    //  Strng EqnName(TF.NextToken());
    //  pControl=CR_VCSControlFactory::Construct(EqnName(), this);
    //  if (pControl)
    //    Control().Parse(TF);
    //  else
    //    CReactionBlock::Error(&RB, 'B', 5);
    //  
    //  }
    //else if (TF.TokenIs("HeatOfReaction"))
    //  {
    //  TF.CheckToken(":");
    //  Strng EqnName(TF.NextToken());
    //  pHeat=CR_VCSHeatFactory::Construct(EqnName(), this);
    //  if (pHeat)
    //    Heat().Parse(TF);
    //  else
    //    CReactionBlock::Error(&RB, 'B', 5);
    //  }
    //
    //else
      Done=1;
    }


//  if (RB.fHtOfFormsUseFinalised)
//    {
////    if (RB.fUseHtOfForms && (pHeat!=NULL))
////      CReactionBlock::Error(&RB, 'B', 14);
////    else if (!RB.fUseHtOfForms && (pHeat==NULL))
////      CReactionBlock::Error(&RB, 'B', 15);
//    }
//  else
//    {
//    //RB.fUseHtOfForms=(pHeat==NULL);
//    //RB.fHtOfFormsUseFinalised=true;
//    }
//  if (pHeat==NULL)
//    pHeat=new CR_VCSHeat(this);

//  if (RB.fUseHtOfForms)
//    {
//    //for (int k = 0; k < NReactants(); k++)
//    //  {
//    //  if (!SDB[Reactant[k].SpcId].HfOK())
//    //    CReactionBlock::Error(&RB, 'B', 16, Reactant[k].SpcName());
//    //  }
//    //for (k = 0; k < NProducts(); k++)
//    //  {
//    //  if (!SDB[Product[k].SpcId].HfOK())
//    //    CReactionBlock::Error(&RB, 'B', 16, Product[k].SpcName());
//    //  }
//    }
//  if (NReactants()>0 && NProducts()>0)
//    {
//    }
  StructureChanged(this);
  }

//---------------------------------------------------------------------------

int CR_VCS::SpecieIndex(Strng &SpcName)
  {
  int i;
  for (i=NSpecies()-1; i>=0; i--)
    if (m_Species[i].SpcName.XStrCmp(SpcName())==0)
      break;
  return i;
  }

//---------------------------------------------------------------------------

//int CR_VCS::SpecieProductIndex(Strng &SpcName)
//  {
//  int i;
//  for (i=NProducts()-1; i>=0; i--)
//    if (Product[i].SpcName.XStrCmp(SpcName())==0)
//      break;
//  return i;
//  }

//---------------------------------------------------------------------------

inline int Find(CIndexXlate & A, int What)
  {
  for (int i=0; i<A.GetSize(); i++)
    if (A[i]==What)
      return i;
  return -1;
  }

void CR_VCS::SetUp()
  {
  CIndexXlate SpId2L;
  CIndexXlate L2SpId;
  CIndexXlate ElId2L;
  CIndexXlate L2ElId;

  L2SpId.SetSize(0,10);
  L2SpId.Add(-1);// Indices for VCS got from 1... (ie.e FORTRAN Heritage);
  L2ElId.SetSize(0,10);
  L2ElId.Add(-1);// Indices for VCS got from 1... (ie.e FORTRAN Heritage);

  SpId2L.SetSize(SDB.Count());
  ElId2L.SetSize(EDB.Count());

  int EUsed[250];
  memset(EUsed, 0, sizeof(EUsed));
  int NEle=0;
  // Scan for # Elements used
  for (int l=0; l<NSpecies(); l++)
    {
    int s=m_Species[l].SpcId;
    ElemCompArray &ElComp=CDB[SDB[s].iCId].ElComp;
    for (int j=0; j<ElComp.GetSize(); j++)
      {
      int e=ElComp[j].iENo;
      if (!EUsed[e])
        {
        EUsed[e]=1;
        NEle++;
        }
      }
    }

  CDMatrix SpEl;
  SpEl.SetSize(NSpecies()+1, NEle+1);
  CLVector SpPh;
  SpPh.SetSize(NSpecies()+1);
  
  for (l=0; l<NSpecies(); l++)
    {
    int s=m_Species[l].SpcId;
    int SpInx=L2SpId.Add(s);
    SpId2L[s]=SpInx;
    if (SDB[s].IsGas())
      SpPh[SpInx]=CVCSSolver::GasPhase;
    else if (SDB[s].IsLiq())
      SpPh[SpInx]=CVCSSolver::LiquidPhase;
    else 
      SpPh[SpInx]=CVCSSolver::SolidPhase;
    // Add element stuff
    ElemCompArray &ElComp=CDB[SDB[s].iCId].ElComp;
    for (int j=0; j<ElComp.GetSize(); j++)
      {
      int e=ElComp[j].iENo;
      if (Find(L2ElId, e)<0)
        ElId2L[e]=L2ElId.Add(e);
      int ElInx=ElId2L[e];
      SpEl[SpInx][ElInx]=ElComp[j].dMoles;
      }
    }
  int NSpcs=L2SpId.GetSize()-1;
  int NElems=L2ElId.GetSize()-1;

  m_VCS.Initialise(SpEl, SpPh, SpId2L, L2SpId, ElId2L, L2ElId);

  int xxx=0;
  }

//---------------------------------------------------------------------------

dword CR_VCS::Check()
  {
//  BalanceData BD(this);
//
//  BD.NumSpc = NReactants() + NProducts();
//  BD.SpcName.SetSize(BD.NumSpc);
//  BD.Moles.SetSize(BD.NumSpc);
//
//  if (NReactants()>0)
//    {
//    // Normalize no of moles (stoichiometric) to 1 for the first reactant
//    double Scl=1.0/Reactant[0].NoOfMoles;
//
//    int j = 0;
//    for (int k = 0; k < NReactants(); k++)
//      {
//      BD.SpcName[j]=Reactant[k].SpcName();
//      BD.Moles[j] = Reactant[k].NoOfMoles*Scl;
//      j++;
//      }
//    for (k = 0; k < NProducts(); k++)
//      {
//      BD.SpcName[j]=Product[k].SpcName();
//      BD.Moles[j] = Product[k].NoOfMoles*Scl;
//      j++;
//      }
//    }
// // end copy specie names over
//
  dwState=0;
//  if (!pControl)
//    dwState|=CRS_NoExtent;
//  if (!BD.Check())
//    dwState|=CRS_NoBalance;

//  if (!pControl)
//    SetCI(1, "Reaction has no extent");
//  else
//    ClrCI(1);
//

  return dwState;//BD.Check() && pControl;
  }

//---------------------------------------------------------------------------

void CR_VCS::Zero()
  {
  for (int i=0; i<m_Species.GetSize(); i++)
    {
    m_Species[i].Zero();
    }
  };

//---------------------------------------------------------------------------

dword  CR_VCS::GetState()
  {
//  if (!CpsOK())
//    dwState|=CRS_BadCps;
//  else
//    dwState=dwState&~CRS_BadCps;
//  if (!HfsOK())
//    dwState|=CRS_BadHfs;
//  else
//    dwState=dwState&~CRS_BadHfs;

  return dwState;
  };

//---------------------------------------------------------------------------

LPCTSTR CR_VCS::GetRctString(dword Options=EAS_All)
  {
  m_AsString="CR_VCS::GetRctString";
  return m_AsString;
  };

//---------------------------------------------------------------------------

flag CR_VCS::CpsOK()
  {
  for (int k = 0; k < NSpecies(); k++)
    if (!SDB[m_Species[k].SpcId].CpOK())
      return false;
//  for (k = 0; k < NProducts(); k++)
//    if (!SDB[Product[k].SpcId].CpOK())
//      return false;
  return true;
  }

//---------------------------------------------------------------------------

flag CR_VCS::HfsOK()
  {
  for (int k = 0; k < NSpecies(); k++)
    if (!SDB[m_Species[k].SpcId].H25OK())
      return false;
//  for (k = 0; k < NProducts(); k++)
//    if (!SDB[Product[k].SpcId].HfOK())
//      return false;
  return true;
  };

//---------------------------------------------------------------------------

//double CR_VCS::totdHf(double T)
//  {
//  //dwState=dwState&~(CRS_BadCps|CRS_BadHfs)
//  double Ht=0.0;
//  for (int l=0; l<NReactants(); l++)
//    {
//    CR_VCSTerm &R=Reactant[l];
//    double H=SDB[R.SpcId].mlHf(T, RB.Press, RB.Ovr(), RB.MassArray());
//    //if (!SDB[R.SpcId].CpOK())
//    //  dwState|=CRS_BadCps;
//    //if (!SDB[R.SpcId].HfOK())
//    //  dwState|=CRS_BadHfs;
//    Ht+=R.dMlXfer*H;
//    }
//  for (int r=0; r<NProducts(); r++)
//    {
//    CR_VCSTerm &R=Product[r];
//    double H=SDB[R.SpcId].mlHf(T, RB.Press, RB.Ovr(), RB.MassArray());
//    //if (!SDB[R.SpcId].CpOK())
//    //  dwState|=CRS_BadCps;
//    //if (!SDB[R.SpcId].HfOK())
//    //  dwState|=CRS_BadHfs;
//    Ht+=R.dMlXfer*H;
//    }
//
//  return Ht;
//  };

//===========================================================================
// Performs the reaction, diminishing reactants and making products,    
// as well as doing a tally of the overall Heat of Reaction.            
//
// deltaHeat := ProductHeat - ReactantHeat;                              
// The normal convention requires that if the result of this subtraction 
// is negative then the reaction is exothermic, if positive then         
// endothermic. In our case we will reverse the convention i.e. subtract 
// ProductHeat from ReactantHeat and have a positive value equal to      
// an increase of Heat released (exothermic) and vice versa              
//===========================================================================

//---------------------------------------------------------------------------

long CR_VCS::SolveBlock(int Version, CReactionBlock &RB, CReaction **Eqns, int NEqns, CRctSolveBlk & RSB)
  {
  double HfFd=RB.Model->totHf();

  CR_VCS *p=(CR_VCS*)Eqns[0];
  p->m_VCS.Solve(RB.Model, RB.Moles, RB.m_TempKFeed, RB.EstFinalT(), RB.m_Press, RB.m_bFindFinalT);

  for (int i=0; i<SDB.Count(); i++)
    {
//    double dm=p->m_VCS.DeltaMass(i);
    RSB.dMass.VValue[i]=p->m_VCS.DeltaMoles(i)*SDB[i].MoleWt();
    RB.Moles.VValue[i]+=p->m_VCS.DeltaMoles(i);//dm*SDB[i].MoleWt();
    };

  RSB.m_Hf_FdT=p->m_VCS.DHfFdT();
  RSB.m_Hf_PrT=p->m_VCS.DHfPrT();
//        RSBLcl.dHfRefT+=E.totDHf(RB.RefTempK);
//        #if UseRBZeroK
//        RSBLcl.dHfZeroT+=E.totDHf(ZeroCinK);
//        RSBLcl.dHfZeroTAtmP+=E.totDHf_atP(ZeroCinK, Std_P);
//        #endif
//        RSBLcl.fCpsOK&=E.CpsOK();
//        RSBLcl.fHfsOK&=E.HfsOK();
//  #if UseRBZeroK
//  RSB.dHfZeroT=0;
//  RSB.dHfZeroTAtmP=0;
//  #endif

//  m_VCS.
//  switch (Version)
//    {
//    case 1:
//      //return SolveBlock1(RB, (CR_VCS**)Eqns, NEqns, RSB);
//    case 2:
//      return SolveBlock2(RB, (CR_VCS**)Eqns, NEqns, RSB);
//    case 3:
//      return SolveBlock3(RB, (CR_VCS**)Eqns, NEqns, RSB);
//    }

  return 0;
  };

//--------------------------------------------------------------------------

flag CR_VCS::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="E\tReaction : No Extent control";                return 1;
    case  2: pS="E\tNo Balance";                                  return 1;
    case  3: pS="E\tReaction : Direction Limited Reaction";       return 1;
    case  4: pS="E\tReaction : Unbounded Problem";                return 1;
    case  5: pS="E\tReaction : No Solution found";                return 1;
    case  6: pS="E\tReaction : Not converged";                    return 1;
    case  7: pS="E\tReaction : Specie Overflow Moles > 1.0e6/s";  return 1;
    default:                                               
      return CReaction::CIStrng(No, pS);
    }
  };

//===========================================================================
//
//
//
//===========================================================================
