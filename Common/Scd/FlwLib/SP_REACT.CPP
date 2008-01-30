//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __SP_REACT_CPP
#include "sp_db.h"
#include "sp_model.h"
#include "sp_react.h"
#include "sr_eqn.h"
#include "sr_src.h"
#include "sr_vcs.h"
#include "sq_lock.h"
#include "scd_wm.h"
//#include "optoff.h"

_declspec(dllimport) int SpModelExDbgOn;

#define dbgReaction  WITHDEBUG
#if dbgReaction
#include "dbgmngr.h"
static CDbgMngr dbgRctLoad("Reactions", "Load");
static CDbgMngr dbgRctSolve("Reactions", "Solve");
#endif

//===========================================================================
//
//
//
//===========================================================================

const pchar CReaction::GroupName="Specie_Reaction";

//===========================================================================
//
//
//
//===========================================================================

flag CRCTTokenFile::TokenIs(const char * pRqdToken)
  {
  const char * p=NextToken();
  if (_stricmp(p, pRqdToken)==0)
    return True;
  m_ReturnSameToken= True;
  return False;
  }

//---------------------------------------------------------------------------

void CRCTTokenFile::CheckToken(const char * pRqdToken)
  {
  const char * p=NextToken();
  if (_stricmp(p, pRqdToken)!=0)
    CReactionBlock::Error(&RB, 'B', 1);
  }

//---------------------------------------------------------------------------

double CRCTTokenFile::DoubleToken()
  {
  const char * p=NextToken();
  if ((GetCurTokenType()!=TokNumeric) && (p[0]!='+') && (p[0]!='-'))
    CReactionBlock::Error(&RB, 'B', 2);
  double d=atof/*SafeAtoF*/(p);
  if (strchr(p, '%'))
    d*=0.01;
  return d;
  };

//---------------------------------------------------------------------------

int CRCTTokenFile::IntToken()
  {
  const char * p=NextToken();
  if (GetCurTokenType()!=TokNumeric)
    CReactionBlock::Error(&RB, 'B', 2);
  return atoi(p);
  };

//===========================================================================
//
//
//
//===========================================================================

CReaction::CReaction(TagObjClass * pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, Tag_, pAttach, eAttach),
  RB(*(CReactionBlock*)pAttach)
  {
  SeqNo=0;
  EqnIDNo=0;
  };

//---------------------------------------------------------------------------

CReaction::~CReaction()
  {
  };

//--------------------------------------------------------------------------

//flag CReaction::CIStrng(int No, pchar & pS)
//  {
//  // NB check CBCount is large enough.
//  switch (No-CBContext())
//    {
//    case  1: pS="W\t????????";          return 1;
//    default:
//      return TaggedObject::CIStrng(No, pS);
//    }
//  };

//===========================================================================
//
//
//
//===========================================================================
/*#D:#T:Reaction Block
#X:#h<General Description>#nGeneral purpose reaction model which can be used in
a tank, pipe, thickener etc.  The user must check the block, #i<Reactions>, within
the appropriate model for this page to become active.  The user is then required to
load a reaction file into the model.  For a full description of the syntax and method
of writing a reaction file, see Reactions in the main SysCAD help library.
#n#n
#h<Variables to be supplied by the user>#n
#i<RCT_Name> : The name of the reaction file that contains one or more reactions.
The file can be entered in any text editor, and must have the file extension 'rct'.
The file path does not need to be specified if the reaction file is in the
project directory or the data_lib directory for the current model configuration.
See documentation on reactions for more details.#n
#i<RefTemp> : The reference temperature for the heats of reaction.#n
There may be other parameters depending on the number and nature of the reactions.#n
#n#n
#h<Associated variables>#n
#i<State> : The state of the reaction file once a load has been attempted.#n
#i<Error> : The last error found (if any) when the reaction file was loaded.#n
#i<Temp_Start> : The temperature before the reactions occur.#n
#i<Temp_End> : The temperature after the reactions in the reaction block have been
processed.#n
#i<Duty> : The total heat transfer in the reaction block, equal to the sum of the
heats of reaction (tot_dH) and heat gain.#n
There may be other parameters depending on the number and nature of the reactions.#n
#n#n
#h<Buttons>#n
#i<Edit RCT> : This allows you to edit the specified RCT file.#n
#i<Reload RCT> : This reloads the RCT reaction file. Use this after changes to
the reaction file have been made.#n
#n
#n#h<Other>#n
Short name:ReactBlk#n
Model type:Block#n
#G:Common model blocks
*/

//IMPLEMENT_TAGOBJ(CReactionBlock, "ReactBlk", "ReactBlk", "", "RB", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "ReactionBlk", "Reaction Block");
IMPLEMENT_TAGOBJ(CReactionBlock, "SR_Cfg", "SR_Cfg", "", "", "RB", TOC_ALL|TOC_GRP_GENERAL|TOC_STD_KENWALT, "ReactionBlk", "Reaction Block");

CReactionBlock::CReactionBlock(TagObjClass * pClass_, pchar Tag_, pTaggedObject pAttach, TagObjAttachment eAttach) :
  TaggedObject(pClass_, Tag_, pAttach, eAttach),
  All(*(new CReactionArray)),
  Eqns(*(new CReactionArray)),
  Srcs(*(new CReactionArray)),
  Snks(*(new CReactionArray)),
  Pwrs(*(new CReactionArray)),
  HXs1(*(new CReactionArray)),
  HXs2(*(new CReactionArray)),
  Seq(*(new CReactionArray)),
  m_RctTol(TBF_Both, "CReactionBlock::RctTol", 1e-20, 1e-8, 80, TBF_LocalOnly), 
  m_RctTTol(TBF_Both, "CReactionBlock::RctTTol", 1e-20, 0.01, 100, TBF_LocalOnly)
  {

  fOn=True;
  nPermReacts=0;
  fFileLoadAllowed=True;
  pTkns=NULL;
  hEditProcess=NULL;
  EditTime.dwLowDateTime=0;
  EditTime.dwHighDateTime=0;
  //Reaction.SetSize=NULL;
  m_UseStdRefModel=false;
  m_pRefModel=NULL;
  m_pStdRefModel=NULL;
  m_pModelOwn=NULL;
  m_pCSB=NULL;

  Clear(true, true);

  bAutoReload=true;
  bReloadReqd=false;
  //dRctTol=1.0e-8;
  //iMaxIter=20;
  //dRctTTol=0.01;
  //iMaxTIter=100;

  m_dEstFinalT=Std_T;
  m_bFindFinalT=true;
  m_TimeIncAtSolve=1.0;

  m_ExtentType=(PrjFileVerNo()<73 ? RBXT_Target : RBXT_Strict);

  //dShortResTime=0.01;
  m_pSpMRT=NULL;

  //m_pNd=static_cast<FlwNode*>(pAttach);
  #if WithQAL
  iVersion=QALOptions() ? 2 : 3;
  #else
  iVersion=3;
  #endif
  }

//---------------------------------------------------------------------------

CReactionBlock::~CReactionBlock()
  {
  Clear(true, true);
  nPermReacts=0;

  delete pTkns;
  for (int i =0; i<NAll(); i++)
    delete All[i];
  delete &All;
  delete &Eqns;
  delete &Srcs;
  delete &Snks;
  delete &Pwrs;
  delete &HXs1;
  delete &HXs2;
  delete &Seq;
  }

//---------------------------------------------------------------------------

void CReactionBlock::Clear(flag fClearMsg, flag ClearAll)
  {
  for (int k = nPermReacts; k < NAll(); k++)
    {
    //All[k]->Clear();
    delete All[k]; //KGA 22/9/97 : Surely this should be deleted, not just cleared; otherwise causes memory leak
    }

  delete m_pStdRefModel;
  m_pStdRefModel=NULL;


  All.SetSize(nPermReacts);
  Eqns.SetSize(0);
  Srcs.SetSize(0);
  Snks.SetSize(0);
  Pwrs.SetSize(0);
  HXs1.SetSize(0);
  HXs2.SetSize(0);
  Seq.SetSize(0);

  //HeatSum              = 0.0;
  if (ClearAll)
    {
    m_dHfIn                = 0;
    m_dHfOut               = 0;

    m_dHfSumTotFdT         = 0.0;
    m_dHfSumTotPrT         = 0.0;
    m_dHfSumTotRefT        = 0.0;
    m_dHfSumTotZeroT       = 0.0;
    m_dHfSumTotZeroTAtmP   = 0.0;

    m_SrcB.Clear();
    m_SnkB.Clear();
    m_dHfErrFdT            = 0.0;
    m_dHfErrPrT            = 0.0;
    m_dHfErrZeroT          = 0.0;

    m_HfFdStd0           = 0;
    m_HfPrStd0           = 0;
    m_HfFdStdT           = 0;
    m_HfPrStdT           = 0;
    m_HfFdMdl0           = 0;
    m_HfPrMdl0           = 0;
    m_HfFdMdlT           = 0;
    m_HfPrMdlT           = 0;
    }

  NumSpecies           = 0;
  NumElemnts           = 0;

  if (ClearAll)
    {
    m_TempK              = Std_T;
    m_TempKFeed          = Std_T;
    m_TempKRct           = Std_T;
    m_TempKProd          = Std_T;
    }
  m_UsrDefT            = C_2_K(25.0);
  m_UsrDefP            = StdP;

  m_pModelOwn          = NULL;
  m_pOvr               = NULL;
  m_Press              = Std_P;
  m_InWhat             = eNull;    
  iReactionState       = RBS_Empty;

  m_DynRateScale       = 1.0;
  
  bReloadReqd          = false;

  for (int i=1; i<=MaxRctCIs; i++)
    ClrCI(i);

  if (fClearMsg)
    {
    m_LoadState=nPermReacts>0 ? "Only Permanent Reactions" : "No Reactions";
    m_LoadError="";
    }
  StructureChanged(this);
  }

//---------------------------------------------------------------------------

void CReactionBlock::GetSpecieIDs(CSVector &SpcNames, CLVector &SpcIDs)
  {
  int NSpc=SpcNames.GetLen();
  SpcIDs.SetSize(NSpc);
  for (int i=0; i<NSpc; i++)
    SpcIDs[i]=SDB.Find(SpcNames[i].Str());
  }

//---------------------------------------------------------------------------

void CReactionBlock::FindElements(Strng &ElemList)
  {
  Strng Elms;
  ElemList="";

  for (int i=0; i < SVSpcCount(); i++)
    {
    GetElements(CDB[SDB[i].iCId].m_sElDef, Elms);
    ElemList+=Elms;
    }
  }

//---------------------------------------------------------------------------

void CReactionBlock::FindElements(CLVector &SpcIDs, Strng &ElemList)
  {
  Strng Elms;
  ElemList="";

  for (int i= 0; i < SpcIDs.GetLen(); i++)
    {
    int SpcID = SpcIDs[i];
    if ((SpcID >= 0))
      {
      GetElements(CDB[SDB[SpcID].iCId].m_sElDef, Elms);
      ElemList+=Elms;
      }
    else
      return;
    }
  }

//---------------------------------------------------------------------------

void CReactionBlock::SortElements(Strng &ElemList, CLVector &ElemIDs, int &NumElems, int NumSpecies)
  {
  Strng SortedList;
  int i;
  Strng elments;

  NumElems = 0;

  for (i = 0; i < ElemList.Length(); i++)
    {
    if (ElemList[i] != Comma) // Separator
      elments += ElemList[i];
    else
      {
      //  check if element already in list
      elments += Comma;
      pchar p=SortedList.XStrStr(elments());
      // Check that this is the full name
      if (p && p!=SortedList() && (*(p-1)!=','))
        p=NULL;

      if (p==NULL)
        {
        SortedList+=elments;
//        SortedList+=Comma;
//        ElemIDs[NumElems]=EDB.Find(elments);
        NumElems++;
        }
      elments="";
      }
    }

  ElemList=SortedList();
  ElemIDs.SetSize(NumElems);

  int ElNo=0;
  for (i = 0; i < ElemList.Length(); i++)
    {
    if (ElemList[i] != Comma) // Separator
      elments += ElemList[i];
    else
      {
      ElemIDs[ElNo++]=EDB.Find(elments());
      elments="";
      }
    }
  }

//---------------------------------------------------------------------------

void CReactionBlock::ElementComp(CDMatrix &Bm, Strng &ElemList, int NumElemnts)
  {
  Strng elm;
  for (int i = 0; i < SVSpcCount(); i++)
    for (int j = 0; j < NumElemnts; j++)
      {
      elm=Element(j, ElemList);
      Bm(j,i) = GetElemAmt(CDB[SDB[i].iCId].m_sElDef,elm);
      }
  }

//---------------------------------------------------------------------------

void CReactionBlock::ElementComp(CDMatrix &Bm, CLVector &SpcIDs, Strng &ElemList, int NumElemnts)
  {
  Strng elm;
  for (int i = 0; i < SpcIDs.GetLen(); i++)
    {
    int SpcID = SpcIDs[i];
    for (int j = 0; j < NumElemnts; j++)
      {
      elm=Element(j, ElemList);
      Bm(j,i) = GetElemAmt(CDB[SDB[SpcID].iCId].m_sElDef,elm);
      }
    }
  }

//---------------------------------------------------------------------------

Strng CReactionBlock::Element(int j, Strng &ElemList)
  {
  //flag done;
  Strng Elem;

  for (int i=0, k=0; ElemList[i]!=0 ; i++)
    if (ElemList[i]==Comma)
      if (k == j)
        break;
      else
        k++;
  for (int s=i-1; s>0; s--)
    if (ElemList[s-1]==Comma || isupper(ElemList[s]))
      break;
  for ( ; s<i; s++)
    Elem+=ElemList[s];

  return Elem;
  }

//---------------------------------------------------------------------------

void CReactionBlock::GetMolMass(CSysVector &MolMass, CDMatrix &Bm, CLVector &ElemIDs)
  {
  int i,j;
  MolMass.Zero();
  for (i = 0; i < SVSpcCount(); i++)
    {
    for (j = 0; j < ElemIDs.GetLen(); j++)
      {
      MolMass.VValue[i] += Bm(j,i) * EDB[ElemIDs[j]].m_AtmWt;
      #ifdef dbgRCTUtils
      if (Bm(j,i)>1.0e-6)
        dbgpln("m: %2i %12.2f %12.2f %4.4s  %s", ElemIDs[j], EDB[ElemIDs[j]].AtmWt, MolMass[i], EDB[ElemIDs[j]].Name, SDB[i].Name());
      #endif
      }
    }
  }

//---------------------------------------------------------------------------

void CReactionBlock::GetQtyElems(CDVector &QtyElemns, CSysVector &Moles, CDMatrix &Bm, int NumElemnts)
  {
  double MolSum;
  int i,j;

  QtyElemns.SetSize(NumElemnts);
  #ifdef dbgRCTUtils
  dbgpln("Bm : No of elements (Row) per specie (Col)");
  #endif
  for (j = 0; j < NumElemnts; j++)
    {
    MolSum = 0.0;
    for (i = 0; i < SVSpcCount(); i++)
      {
      #ifdef dbgRCTUtils
      dbgp(" %3.1f", Bm(j,i));
      #endif
      MolSum = MolSum + Moles[i] * Bm(j,i);
      }
    QtyElemns[j] = MolSum;
    #ifdef dbgRCTUtils
    dbgpln("");
    #endif
    }
  }

//---------------------------------------------------------------------------

Strng ScanComments(CRCTTokenFile &TF)
  {
  Strng X;
  while (!TF.AtEOF() && TF.TokenIs(";"))
    {
    if (X.GetLength()>0)
      X+= " ";
    Strng Y=TF.TokenLine();
    Y.LRTrim();
    X += Y;
    TF.GetNextLine();
    }
  return X;
  }

//---------------------------------------------------------------------------

void CReactionBlock::Parse(CRCTTokenFile &TF)
  {
  TagObjClass * pCFirst=TagObjClass::FindGroupId(CReaction::GroupName);
  TagObjClass * pC=NULL;//pCFirst;
  int SeqNo=0;
  flag UsrSeq=0;
  bool SeqReqd=false;
  bool FirstRct=true;
  Strng RTag;
  Strng BlkComment;
  Strng RctComment;
  char StringChar='\"';
  TF.SetStringChar(StringChar);
  TF.SetUseStringChar(False);
  TF.SetIgnoreComments(false);
  int SrcCount = 0;
  int SnkCount = 0;

  BlkComment=ScanComments(TF);

  while (!TF.AtEOF() && !TF.TokenIs("End"))
    {
    if (TF.TokenIs("EnergyBalance"))
      {
      // Parse Old RCT Files OK
      //if (fHtOfFormsUseFinalised)
      //  Error(this, 'B', 13);
      //else
      //  {
      flag fUseHtOfForms=0;
      TF.CheckToken(":");
      if (TF.TokenIs("HeatsOfFormation"))
        fUseHtOfForms=1;
      else
        {
        TF.CheckToken("HeatsOfReaction");
        fUseHtOfForms=0;
        }
      //  fHtOfFormsUseFinalised=true;
      //  }
      }
  
    BlkComment+=ScanComments(TF);

    int n=NAll();
    RTag.Set("R%i",n+1);
    TagObjClass * pCMem=pC;
    for (pC=pCFirst; pC; pC=pC->NextClassInGrp())
      if (TF.TokenIs(pC->ShortDesc()))
        {
        TF.CheckToken(":");
        break;
        }
    if (pC==NULL)
      {
      if (pCMem==NULL)
        Error(this, 'B', 1);
      pC=pCMem;
      }

    RctComment+=ScanComments(TF);

    TF.SetIgnoreComments(true);
    
    Strng Lbl(TF.NextToken());
    if (Lbl[0]=='\"')
      {
      while (Lbl[Lbl.Length()-1]!=StringChar)
        {
        Lbl+='_';
        Lbl+=TF.NextToken();
        }

      RTag=Lbl.Mid(1, Lbl.Length()-2);
      char *p;
      while (p=RTag.XStrChr(' '))
        *p='_';
      TF.CheckToken(":");
      }
    else
      TF.SetReturnSameToken();

    //ScanComments(TF); // throw away

    if (dynamic_cast<CR_SrcClassDef*>(pC) && SrcCount++>0)
      {
      Error(this, 'B', 18);
      }
    else if (dynamic_cast<CR_SnkClassDef*>(pC) && SnkCount++>0)
      {
      Error(this, 'B', 19);
      }
    else
      {
      CReaction* pRct=(CReaction*)pC->Construct(NULL, RTag(), this, TOA_Embedded);
      pRct->EqnIDNo=n;
      pRct->m_sComment=RctComment;
      TF.SetIgnoreComments(true);
      pRct->Parse(TF);

      flag IsSeq=TF.TokenIs("Sequence");

      if (dynamic_cast<CR_Eqn*>(pRct))
        {
        if (IsSeq && !FirstRct && !SeqReqd)
          Error(this, 'B', 17);
        }

      if (IsSeq && (SeqReqd || FirstRct))
        {
        SeqReqd = true;
        TF.CheckToken(":");
        SeqNo=abs(TF.IntToken());
        UsrSeq=1;
        }
      else 
        {
        // if following default sequence
        if (!UsrSeq)
          SeqNo--;
        }
      pRct->SeqNo=abs(SeqNo);

      All.Add(pRct);

      if (dynamic_cast<CR_Eqn*>(pRct))
        FirstRct=false;
      }

    TF.SetIgnoreComments(false);

    RctComment=ScanComments(TF);
    }
  m_sComment=BlkComment;
  };

//---------------------------------------------------------------------------

void CReactionBlock::SetUpReactions ()
  {
  NumSpecies = SVSpcCount();

  //NumSpecies = 0;
  //for (int i=0; i<SVSpcCount(); i++)
  //  if (CDB[SDB[i].iCId].ElDef.Length()>0)
  //    NumSpecies++;
  //
  //SpcIds.SetSize(NumSpecies);
  //SpcIds2Inx.SetSize(SVSpcCount(), -1);
  //int j=0;
  //for (i=0; i<SVSpcCount(); i++)
  //  if (CDB[SDB[i].iCId].ElDef.Length()>0)
  //    {
  //    SpcIds[j]=i;
  //    SpcIds2Inx[i] = j;
  //    j++;
  //    }

  int s=0,n=0;
  CLVector SeqI;
  SeqI.SetSize(NAll());

  while (n<NAll())
    {
    int neqns=0;
    for (int k = 0; k < NAll(); k++)
      if (All[k]->SeqNo==s)
        {
        SeqI[n++]=k;
        neqns++;
        }
    int ne=0;
    for (int k = 0; k < NAll(); k++)
      if (All[k]->SeqNo==s)
        All[k]->m_SeqStr.Set(neqns>1?"%i (%i of %i Sim)":"%i", s, ++ne, neqns);
    s++;
    ASSERT(n<=NAll());
    }

  for (int i = 0; i < NAll(); i++)
    {
    int k=SeqI[i];
    switch (All[k]->Basis())
      {
      case RctSource :
        Srcs.Add(All[k]);
        break;
      case RctEquilibrium :
      case RctRate :
      case RctExtent :
        Eqns.Add(All[k]);
        break;
      case RctSink:
        Snks.Add(All[k]);
        break;
      case RctPwr:
        Pwrs.Add(All[k]);
        break;
      case RctHX1:
        HXs1.Add(All[k]);
        break;
      case RctHX2:
        HXs2.Add(All[k]);
        break;
      }

    All[k]->SetUp();
    }

  for (i = 0; i < NSrcs(); i++)
    Seq.Add(Srcs[i]);

  for (i = 0; i < NEqns(); i++)
    Seq.Add(Eqns[i]);

  for (i = 0; i < NSnks(); i++)
    Seq.Add(Snks[i]);

  for (i = 0; i < NPwrs(); i++)
    Seq.Add(Pwrs[i]);

  for (i = 0; i < NHXs1(); i++)
    Seq.Add(HXs1[i]);

  for (i = 0; i < NHXs2(); i++)
    Seq.Add(HXs2[i]);

  ElemIds.SetSize(0);
  //MolMass.SetSize(NumSpecies, 0.0);
  //Moles.SetSize(NumSpecies, 0.0);
  //Moles_S.SetSize(NumSpecies, 0.0);
  //Moles_BS.SetSize(NumSpecies, 0.0);
  //Moles_DStrt.SetSize(NumSpecies, 0.0);
  //Lockup.SetSize(NumSpecies, 0.0);
  //dMoles.SetSize(NumSpecies, 0.0);
  //dMass.SetSize(NumSpecies, 0.0);
  //FdMoles.SetSize(NumSpecies, 0.0);
  //PrMoles.SetSize(NumSpecies, 0.0);
  //FdLockup.SetSize(NumSpecies, 0.0);
//  SpMRT.SetSize(NumSpecies, 0.0);
//  SpMRTInc.SetSize(NumSpecies, 0.0);
//  SpRT.SetSize(NumSpecies, 0.0);

  FindElements(ElemList);
  SortElements(ElemList, ElemIds, NumElemnts, NumSpecies);

  Bm.SetSize(NumElemnts, NumSpecies, 0.0);
  QtyElemns.SetSize(NumElemnts);

  ElementComp(Bm, ElemList, NumElemnts);
  GetMolMass(MolMass, Bm, ElemIds);

  }

//----------------------------------------------------------------------------

void CReactionBlock::dbgDump()
  {
  int i, k;
  //ex : array[0..0] of char;
  dbgpln(" **********************************************************");
  dbgpln(" List of component elements ");
  dbgpln("");

  dbgpln("  %s",ElemList());
  dbgp("  ");
  for (k = 0; k < NumElemnts; k++)
    dbgp("%i,",ElemIds[k]);
  dbgpln("");
  dbgpln("");

  dbgpln(" Table of specie elemental composition ");
  dbgpln("");
  for (i = 0; i < NumSpecies; i++)
    {
    dbgp("%s ",SDB[i].SymOrTag());
    for (k = 0; k < NumElemnts; k++)
      dbgp("%4.1f",Bm(k,i));
    dbgpln("");
    }

  dbgpln("");
  dbgpln(" ....... Input Reactions ........ ");
  dbgpln("");
  for (i = 0; i< NSeq(); i++)
    Seq[i]->dbgDump("");
  dbgpln(" **********************************************************");
  };

//---------------------------------------------------------------------------

void CReactionBlock::dbgDumpPostSpcData()
  {
  double sum=0.0;
  dbgpln("");
  for (int i = 0; i<NumSpecies; i++)
    {
    dbgpln("%-15.15s %12.2f %12.4g",SDB[i].SymOrTag(),Moles[i],dMoles[i]);
    sum+=Moles[i];
    }

  dbgpln("");
  dbgpln(" Pressure       %12.2f kPa",m_Press);
  dbgpln(" Temperature    %12.2f K",m_TempK);
  dbgpln(" Change in heat %12.2f  ",m_dHfSumTotFdT);
  dbgpln(" Hf In          %12.2f  ",m_dHfIn);
  dbgpln(" Hf Out         %12.2f  ",m_dHfOut);
  dbgpln("");
  };

//---------------------------------------------------------------------------

void CReactionBlock::dbgDumpPostSolveData()
  {
  double sum1=0.0,sum2=0.0;
  dbgpln("");
  for (int i = 0; i<NumSpecies; i++)
    {
    dbgpln("%-15.15s %14.6g -> %14.6g , %12.4g",
            SDB[i].SymOrTag(),Moles_S[i],Moles[i],
            dMoles[i]);
    sum1+=Moles_S[i];
    sum2+=Moles[i];
    }
  //dbgpln("%-15.15s %12.2f %12.2f","TOTAL",sum1,sum2);
  dbgpln("");
  dbgpln(" Temperature    %12.2f -> %12.2f K",m_TempKFeed,m_TempK);
  dbgpln(" Change in heat %12.2f  ",m_dHfSumTotFdT);
  dbgpln(" Enthalpy In    %12.2f  ",m_dHfIn);
  dbgpln(" Enthalpy Out   %12.2f  ",m_dHfOut);
  dbgpln("");
  };

//---------------------------------------------------------------------------

void CReactionBlock::dbgDumpPostCheckData()
  {
  dbgpln(" **********************************************************");
  dbgpln("");
  dbgpln(" ....... Balanced Reactions ........ ");
  dbgpln("");
  for (int i = 0; i< NSeq(); i++)
    Seq[i]->dbgDump("");
  dbgDumpPostSpcData();
  dbgpln(" **********************************************************");
  };

//---------------------------------------------------------------------------

void CReactionBlock::Error(pCReactionBlock pRBlk, char Code, int errNo, pchar s/*=NULL*/)
  {
  Strng errDesc;
  switch (errNo) // execute errors 0 to 100 ; load errors 100 to 200
    {
    case 0 : errDesc = "Specie Not Found";
      break;
    case 1 : errDesc = "Unexpected token ";
      break;
    case 2 : errDesc = "Number expected";
      break;
    case 3 : errDesc.Set("Specie %s unknown", s);
      break;
    case 4 : errDesc.Set("Specie %s not part of Reaction", s);
      break;
    case 5 : errDesc = "Unknown Extent type";
      break;
    case 6 : errDesc = "Specie Name Expected";
      break;
    case 7 : errDesc.Set("Specie %s not a product", s);
      break;
    case 8 : errDesc.Set("Specie %s not a reactant", s);
      break;
    case 9 : errDesc.Set("File load not permitted");
      break;
    case 10: errDesc.Set("No Catalytic Bed");
      break;
    case 11: errDesc.Set("Invalid Specie Id");
      break;
    case 12: errDesc.Set("Specie %s : Too few Moles", s);
      break;
    case 13: errDesc.Set("May not change Heat of Reaction basis after first Reaction");
      break;
    case 14: errDesc.Set("Heat of Reaction invalid : using Heats of Formation");
      break;
    case 15: errDesc.Set("Heat of Reaction Required");
      break;
    case 16: errDesc.Set("Specie %s : Heat of formation not Specified", s);
      break;
    case 17: errDesc.Set("First Reaction must have a Sequence number");
      break;
    case 18: errDesc.Set("Source may only be specified once. (Use ',' to separate species)");
      break;
    case 19: errDesc.Set("Sink may only be specified once. (Use ',' to separate species)");
      break;
    case 20: errDesc.Set("Specie %s : Cp not defined", s);
      break;
    case 21: errDesc.Set("Specie %s : Hf not defined", s);
      break;
    default: errDesc = ((s !=NULL) && (strlen(s)>0)) ? s : "Unknown Error Number";
    };

  char Buff0[1024], Buff1[1024];
  sprintf(Buff0, "%d: %s", errNo, errDesc);

  if (pRBlk!=NULL && pRBlk->pTkns!=NULL  && (Code == SPR_ErrBuild))// || Code == SPR_WarnBuild))
    sprintf(Buff1, "Line:%4d %s" , pRBlk->pTkns->GetLineNo(), pRBlk->pTkns->GetWholeLine());
  else
    sprintf(Buff1, "");

  dbgpln(" ======================= ERROR =========================");
  dbgpln("ERROR %c : %s", Code, Buff0);
#ifndef TESTING_GCONTROL
  if (Code != SPR_WarnBuild)
    LogStop(pRBlk ? pRBlk->FullObjTag() : "Reactions", 0, "Reactions:ERROR %c : %s", Code, Buff0);
#endif

  if (strlen(Buff1)>0)
    {
    dbgpln("%s", Buff1);
#ifndef TESTING_GCONTROL
    if (Code != SPR_WarnBuild)
      LogStop(pRBlk ? pRBlk->FullObjTag() : "Reactions", 0, "%s", Buff1);
#endif
    }
  dbgpln(" =======================================================");

  if (pRBlk)
    {
    pRBlk->m_LoadState=Buff0;
    pRBlk->m_LoadError=Buff1;
    }

#ifdef TESTING_GCONTROL
  AfxMessageBox(Buff0);
#endif

  switch (Code)
    {
    case SPR_WarnBuild:
    case SPR_ErrBuild:
      break;
    default:
      sprintf(Buff0, "%s\n%c: Run-time error!\n", Buff0, Code);
      AfxMessageBox(Buff0);
      break;
    };

  switch (Code)
    {
    case SPR_ErrMathRuntime: //KGA:TODO what to do for runtime errors ????
      break;
    case SPR_ErrOtherRuntime: //KGA:TODO what to do for runtime errors ????
      AfxMessageBox("Unpredictable results will occur!!\n System likely to CRASH !");
      break;
    case SPR_WarnBuild:
      break;
    case SPR_ErrBuild:
      {
      //RBException* theE = new RBException;
      RBException E(RBException::Build);// = new RBException;
      throw E;
      break;
      };
    default:
      ASSERT(FALSE);//should never get here
      break;
    };
  };

//---------------------------------------------------------------------------

static DDBValueLst DDBVersion[] = {
  {2, "Version_2"},
  {3, "Version_3"},
  {0}};

static DDBValueLst DDBExtentTypes[] = {
  { RBXT_AllStrict,   "All_Strict"  },
  { RBXT_AllTarget,   "All_Target"  },   
  { RBXT_Strict,      "Strict"      },
  { RBXT_Target,      "Target"      },   
  {0}};

static DDBValueLst DDBExtraInfo[] = {
  { 0,                "Remove All"},
  { SRSX_UsrDefInfo,  "Include UsrDefInfo"},
  { SRSX_FeedInfo,    "Include FeedInfo"},
  { SRSX_ProdInfo,    "Include ProdInfo"},   
  { SRSX_All,         "Include All"},  
  {0}};

static DDBValueLst DDBChgVars[] = {
  { 0,                "Remove All"},
  { SRSC_Mass,        "Include Mass"},
  { SRSC_Molar,       "Include Molar"},   
  { SRSC_All,         "Include All"},  
  {0}};


// TO MOVE to *.H ?????????????????????????????????????????????????????????????
XID xidRCTSpRT = RctXID(11100); // Keep 256 Spares

byte CReactionBlock::sm_AllVars=0;
byte CReactionBlock::sm_ChgVars=0;//SRSC_Mass;

void CReactionBlock::BuildDataDefn(DataDefnBlk & DDB)//, pTaggedObject pOwnObj, pchar pName)
  {
  if (DDB.BeginStruct(this, NULL, NULL, DDB_RqdPage))
    {
    if (fFileLoadAllowed)
      {
      DDB.Text  ("");
      DDB.String("RCT_Name",        "",    DC_,     "",      xidRCTName,      this, isParm|SetOnChange);
      DDB.String("RCT_Path",        "",    DC_,     "",      xidRCTPath,      this, noFileAtAll);
      DDB.CheckBoxBtn("On",         "",    DC_,     "",      xidRCTOn,        this, isParm, DDBOnOff);
      DDB.CheckBoxBtn("AutoReload", "",    DC_,     "",      &bAutoReload,    this, isParm, DDBOnOff);
      DDB.String("LoadState",       "",    DC_,     "",      xidRCTLdState,   this, MultiLineStr|noFileAtAll);
      DDB.String("Error",           "",    DC_,     "",      xidRCTLdError,   this, MultiLineStr|noFileAtAll);
//      DDB.Text  ("");
      // NB These 2 entries should follow the name  (at least)
#if WITHRCTEDITDLG
      DDB.Button("Edit_RCT_Dlg",    "",    DC_,     "",      xidEditRCTBtnDlg,   this, isParm);
      DDB.Button("Edit_RCT_Txt",    "",    DC_,     "",      xidEditRCTBtnTxt,   this, isParm);
#else
      DDB.Button("Edit_RCT",        "",    DC_,     "",      xidEditRCTBtnTxt,   this, isParm);
#endif
      DDB.Button("Reload_RCT",      "",    DC_,     "",      xidReloadRCTBtn, this, isParm);
      DDB.Button("Browse_RCT",      "",    DC_,     "",      xidBrowseRCTBtn, this, isParm);
      DDB.Text  ("");
      DDB.Int  ("ExtentType",      "",    DC_,     "",      &m_ExtentType,   this, isParm|SetOnChange, DDBExtentTypes);
      DDB.Text  ("");
      }

    DDB.Visibility();
    DDB.String("RunState",          "",    DC_,     "",      xidRCTRunState,  this, MultiLineStr|noFileAtAll);
    DDB.CheckBoxBtn("StdRefModel",  "",    DC_,     "",      &m_UseStdRefModel, this, isParm);


    if (Nd()->SolveSurgeMethod()) 
      DDB.Double("DynRateScale",           "",    DC_Frac,    "%",     &m_DynRateScale,     this, isParm);

    DDB.Visibility(NSHM_All, sm_AllVars!=0);
    //DDB.Double("RefTemp",           "",    DC_T,    "C",     &m_RefTempK,     this, isParm);
    DDB.Double("UsrDefT",           "",    DC_T,    "C",     &m_UsrDefT,      this, isParm);
    DDB.Double("UsrDefP",           "",    DC_P,    "kPa",   &m_UsrDefP,      this, isParm);
    DDB.Visibility();
    DDB.Double("Press",             "",    DC_P,    "kPa",   &m_Press,        this, isResult);
    DDB.Double("TempFeed",    "Feed.T",    DC_T,    "C",     &m_TempKFeed,    this, isResult);
    DDB.Double("TempRct",           "",    DC_T,    "C",     &m_TempKRct,     this, isResult|InitHidden);
    DDB.Double("TempProd",    "Prod.T",    DC_T,    "C",     &m_TempKProd,    this, isResult);

    DDB.Byte("ExtraInfo",           "",    DC_,     "",      &sm_AllVars,     this, isParm|SetOnChange, DDBExtraInfo);
    DDB.Byte("ChangeVars",          "",    DC_,     "",      &sm_ChgVars,     this, isParm|SetOnChange, DDBChgVars);

    DDB.Text("");

    if (!HeatSkipMethod())
      {
      //DDB.Double("HsGain",            "",    DC_Pwr,  "kJ/s",  xidRCTPrDuty,    this, isResult|noFile|noSnap);
      //DDB.Double("HsLoss.Rct",        "",    DC_Pwr,  "kJ/s",  xidRCTPrDutyRct, this, isResult|noFile|noSnap);
      //DDB.TagComment("-ExoThermic");
      //DDB.Double("HsGain.HX",         "",    DC_Pwr,  "kJ/s",  xidRCTDutyHXPwr, this, isResult|noFile|noSnap|InitHidden);
      //DDB.Double("HfGain.Src",        "",    DC_Pwr,  "kJ/s",  xidRCTPrDutySrc, this, isResult|noFile|noSnap|InitHidden);
      //DDB.Double("HfLoss.Snk",        "",    DC_Pwr,  "kJ/s",  xidRCTPrDutySnk, this, isResult|noFile|noSnap|InitHidden);
      //DDB.Double("HfErr",             "",    DC_Pwr,  "kJ/s",  xidRCTPrHfErr,   this, isResult|noFile|noSnap|InitHidden);
      if (sm_AllVars & SRSX_UsrDefInfo)
        {
        if (DDB.BeginStruct(this, "dH@Ref", NULL, DDB_NoPage, 0, MarkerClosed))
          {
          DDB.Double("Nett",       "",    DC_Pwr,  "kJ/s",  xidRCTRefDuty,    this, 0/*|isResult|noFile|noSnap*/|InitHidden);
          DDB.Double("Rct",        "",    DC_Pwr,  "kJ/s",  xidRCTRefDutyRct, this, 0/*|isResult|noFile|noSnap*/|InitHidden);
          DDB.TagComment("-ExoThermic");
          }
        DDB.EndStruct();
        }
      if (DDB.BeginStruct(this, "dH@0", NULL, DDB_NoPage, 0, MarkerClosed))
        {
        DDB.Double("Nett",       "",    DC_Pwr,  "kJ/s",  xidRCTZeroDuty,    this, 0/*|isResult|noFile|noSnap*/|InitHidden);
        DDB.Double("Rct",        "",    DC_Pwr,  "kJ/s",  xidRCTZeroDutyRct, this, 0/*|isResult|noFile|noSnap*/|InitHidden);
        DDB.TagComment("-ExoThermic");
        }
      DDB.EndStruct();
      if (sm_AllVars & SRSX_FeedInfo)
        {
        if (DDB.BeginStruct(this, "dH@Fd", NULL, DDB_NoPage, 0, MarkerClosed))
          {
          DDB.Double("Nett",       "",    DC_Pwr,  "kJ/s",  xidRCTFdDuty,    this, 0/*|isResult|noFile|noSnap*/);
          DDB.Double("Rct",        "",    DC_Pwr,  "kJ/s",  xidRCTFdDutyRct, this, 0/*|isResult|noFile|noSnap*/);
          DDB.TagComment("-ExoThermic");
          DDB.Double("HX",         "",    DC_Pwr,  "kJ/s",  xidRCTDutyHXPwr, this, 0/*|isResult|noFile|noSnap*/|InitHidden);
          DDB.Double("Src",        "",    DC_Pwr,  "kJ/s",  xidRCTFdDutySrc, this, 0/*|isResult|noFile|noSnap*/|InitHidden);
          DDB.Double("Snk",        "",    DC_Pwr,  "kJ/s",  xidRCTFdDutySnk, this, 0/*|isResult|noFile|noSnap*/|InitHidden);
          DDB.Double("Err",        "",    DC_Pwr,  "kJ/s",  xidRCTFdHfErr,   this, 0/*|isResult|noFile|noSnap*/|InitHidden);
          }
        DDB.EndStruct();
        }
      if (sm_AllVars & SRSX_ProdInfo)
        {
        if (DDB.BeginStruct(this, "dH@Pr", NULL, DDB_NoPage, 0))
          {
          DDB.Double("Nett",       "",    DC_Pwr,  "kJ/s",  xidRCTPrDuty,    this, isResult/*|noFile|noSnap*/);
          DDB.Double("Rct",        "",    DC_Pwr,  "kJ/s",  xidRCTPrDutyRct, this, isResult/*|noFile|noSnap*/);
          DDB.TagComment("-ExoThermic");
          DDB.Double("HX",         "",    DC_Pwr,  "kJ/s",  xidRCTDutyHXPwr, this, 0/*|isResult|noFile|noSnap*/|InitHidden);
          DDB.Double("Src",        "",    DC_Pwr,  "kJ/s",  xidRCTPrDutySrc, this, 0/*|isResult|noFile|noSnap*/|InitHidden);
          DDB.Double("Snk",        "",    DC_Pwr,  "kJ/s",  xidRCTPrDutySnk, this, 0/*|isResult|noFile|noSnap*/|InitHidden);
          DDB.Double("Err",        "",    DC_Pwr,  "kJ/s",  xidRCTPrHfErr,   this, 0/*|isResult|noFile|noSnap*/|InitHidden);
          }
        DDB.EndStruct();
        }
      }

    //?? Must add the Abs Tol
    DDB.Double("CnvTolerance",      "",    DC_Frac, "%", xidRCTdRctTol,   this, isParm);
    DDB.Long  ("MaxIters",          "",    DC_,     "",  xidRCTiMaxIter,  this, isParm);
    DDB.Double("CnvTTolerance",     "",    DC_dT,   "C", xidRCTdRctTTol,  this, isParm);
    DDB.Long  ("MaxTIters",         "",    DC_,     "",  xidRCTiMaxTIter, this, isParm);
    DDB.Long  ("Version",           "",    DC_,     "",  &iVersion,       this, isParm|InitHidden, DDBVersion);
    DDB.Long  ("NoOfReactions",     "",    DC_,     "",  xidRCTNumOfRct,  this, isResult/*|noFile|noSnap*/|InitHidden);

    for (int i = 0; i< NSeq(); i++)
      DDB.Object(Seq[i], this, NULL/*Nm()*/, NULL, DDB_OptPage);

    DDB.Visibility(NM_Dynamic|SM_All|HM_All);
    Strng Sym, Tag;
    DDB.Page(".RTs");
//    if (PrjFileVerNo()<22)
//      {
//      for (int s=0; s<SpMRT.GetLen(); s++)
//        {
//        Tag.Set("%s.RT",SDB[s].Tag());
//        Sym.Set("%s.RT",SDB[s].Sym());
//        DDB.Double(Tag(),   Sym(),     DC_Time,    "s",     &SpMRT.m_d[s], this, 0);
//        }
//      }
//    else
//      {
//      if (DDB.BeginObject(this, "RTs", "Sp_RT",NULL,DDB_NoPage,0))
//        {
//        for (int s=0; s<SpMRT.GetLen(); s++)
//          DDB.Double(SDB[s].Tag(),  SDB[s].Sym(), DC_Time, "s", &SpMRT.m_d[s], this, 0);
//        }
//      DDB.EndObject();
//      }

    DDB.Visibility();

    DDB.Text("");
    }
  DDB.EndStruct();
  };

//---------------------------------------------------------------------------
const char* DefaultRCT1 =
"Reactions:\n"
"\n"
";reactions to be completed...\n"
"\n"
"\n"
"End\n"
";----------------------------------------------------------------------\n"
"\n";

const char* DefaultRCT2 =
"\n"
";+++++++++++++++++++++\n"
";++ Optional Fields ++\n"
";+++++++++++++++++++++\n"
"\n"
"  Source: HCl(aq)\n"
"  HeatOfReaction : msFixed = 10 / MgCO3(s)\n"
"  HeatOfReaction : msFixed = 10 / MgCO3(s) At 25\n"
"  HeatOfReaction : msFixed = 10 / MgCO3(s) At FeedTemp\n"
"  HeatOfReaction : msFixed = 10 / MgCO3(s) At ProdTemp\n"
"  HeatOfReaction: mlFixed = 12 / Na2CO3(s)\n"
"  HeatExchange: FinalT = 60\n"
"  HeatExchange: Power = 80\n"
"  Sink: CO2(g)\n"
"\n"
";+++++++++++++++++++++\n"
";++ Extent Examples ++\n"
";+++++++++++++++++++++\n"
"\n"
"Extent: Fraction Na2CO3(s) = 0.50\n"
"Extent: FinalConc H2SO4(aq) = 5.5\n"
"Extent: FinalConc H2SO4(aq) = 5.5 At 25\n"
"Extent: mlFinalFraction MgCO3(s) = 0.35\n"
"Extent: msFinalFraction MgCO3(s) = 0.35\n"
"Extent: msFinalFraction Total MgCO3(s) = 0.2\n"
"Extent: Ratio Na2CO3(s):Na2SO4(s) = 1:500\n"
"Extent: Equilibrium 2.5\n"
"\n"
"----------------------------------\n"
"--- Example of a reaction file ---\n"
"----------------------------------\n"
"Reactions:\n"
"\n"
";Reaction 1 - Specifies the reaction extent of a reactant in mass fractions\n"
"  MgCO3(s) + 2HCl(aq) = MgCl2(aq) + H2O(l) + CO2(g)\n"
"   Extent : Fraction  MgCO3(s) = 0.6\n"
"   Sequence : 1\n"
"\n"
";Reaction 2 - Specifies the molar ratio of two components at the end of reaction\n"
"  CaCO3(s) + 2HCl(aq) = CaCl2(aq) + H2O(l) + CO2(g)\n"
"   Extent : Ratio CaCO3(s):CaCl2(aq) = 1:10\n"
"   Sequence : 1\n"
"\n"
";Reaction 3 - Specifies the reaction's equilibrium constant\n"
"  Na2CO3(s) + 2HCl(aq) = 2NaCl(aq) + H2O(l) + CO2(g)\n"
"   Extent : Equilibrium 2.5\n"
"   Sequence : 2\n"
"\n"
"End\n"
"\n"
"-------------------------\n"
"--- Example reactions ---\n"
"-------------------------\n"
"  H2O(g) -> H2O(l)\n"
"   Extent : Fraction  H2O(g) = 1.0\n"
"\n"
"  H2O(l) -> H2O(g)\n"
"   Extent : Fraction  H2O(l) = 0.01\n"
"\n"
"  CaS(s) + 2O2(g) = CaSO4(aq)\n"
"   Extent: mlFinalFraction CaS(s) = 0.1\n"
"\n"
"  Na2CO3(s) + H2SO4(l) -> Na2SO4(s) + CO2(g) + H2O(l)\n"
"   Extent : Fraction Na2CO3(s) = 0.50\n"
"   HeatOfReaction : msFixed = 10 / Na2CO3(s)\n"
"\n"
";+++++++++++\n"
";++ NOTES ++\n"
";+++++++++++\n"
"1) The tokens 'Reactions:' and 'End' are compulsory as they mark the start and end of the reactions block.\n"
"2) Everything after the word 'End' is ignored.\n"
"3) If source is used to add in reactants, it needs to be placed before the keyword 'Reactions:'\n"
"4) For HeatOfReaction, if a reference temperature is not specified, then FeedTemp is assumed.'\n"
"5) For FinalConc extent option, if a reference temperature is not specified, then product temperature is assumed.'\n"
"6) For more information on the Reaction file format, please refer to SysCAD 9 Models Help - Sub Models - RB Section.\n"
"\n";
const int DefaultRCT1Len = strlen(DefaultRCT1);
const int DefaultRCT2Len = strlen(DefaultRCT2);

flag CReactionBlock::DataXchg(DataChangeBlk & DCB)//, pTaggedObject pOwnObj)
  {
  switch (DCB.lHandle)
    {
    case xidRCTLdState:
      DCB.pC=m_LoadState();
      return 1;
    case xidRCTLdError:
      DCB.pC=m_LoadError();
      if (DCB.pC==NULL)
        DCB.pC=" ";
      DCB.dwRetFlags=DDEF_INERROR;
      return 1;
    case xidRCTRunState:
      {
      SetTheCIs();

      m_RunState="";
      Strng_List L;
      GetAllCIs(L);
      int i=0;
      for (pStrng p= L.First();p; p=L.Next())
        {
        if (i++>0)
          m_RunState+="\n";
        char *ps=p->Str();
        char *pt=strchr(ps, '\t');
        m_RunState+=pt ? (pt+1) : ps;
        }
      if (i>0)
        DCB.dwRetFlags=DDEF_INERROR;
      else
        m_RunState="OK";
      DCB.pC=m_RunState();
      return 1;
      }
    case xidRCTRefDuty:
      if (DCB.rD)
        m_dHfSumTotRefT = *DCB.rD*TimeIncAtSolve();
      DCB.D=m_dHfSumTotRefT/TimeIncAtSolve();
      return 1;
    case xidRCTRefDutyRct:
      if (DCB.rD)
        m_RctB.m_Hf_RefT = *DCB.rD*TimeIncAtSolve();
      DCB.D=m_RctB.m_Hf_RefT/TimeIncAtSolve();
      return 1;
    case xidRCTZeroDuty:
      if (DCB.rD)
        m_dHfSumTotZeroTAtmP = *DCB.rD*TimeIncAtSolve();
      DCB.D=m_dHfSumTotZeroTAtmP/TimeIncAtSolve();
      return 1;
    case xidRCTZeroDutyRct:
      if (DCB.rD)
        m_RctB.m_dHfZeroTAtmP = *DCB.rD*TimeIncAtSolve();
      DCB.D=m_RctB.m_dHfZeroTAtmP/TimeIncAtSolve();
      return 1;
    case xidRCTFdDuty:
      if (DCB.rD)
        m_dHfSumTotFdT = *DCB.rD*TimeIncAtSolve();
      DCB.D=m_dHfSumTotFdT/TimeIncAtSolve();
      return 1;
    case xidRCTPrDuty:
      if (DCB.rD)
        m_dHfSumTotPrT = *DCB.rD*TimeIncAtSolve();
      DCB.D=m_dHfSumTotPrT/TimeIncAtSolve();
      return 1;
    case xidRCTFdDutyRct:
      if (DCB.rD)
        m_RctB.m_Hf_FdT= *DCB.rD*TimeIncAtSolve();
      DCB.D=m_RctB.m_Hf_FdT/TimeIncAtSolve();
      return 1;
    case xidRCTPrDutyRct:
      if (DCB.rD)
        m_RctB.m_Hf_PrT= *DCB.rD*TimeIncAtSolve();
      DCB.D=m_RctB.m_Hf_PrT/TimeIncAtSolve();
      return 1;
    case xidRCTDutyHXPwr:
      DCB.D=HsSumHXPwr()/TimeIncAtSolve();
      return 1;
    case xidRCTDutyHX:
      if (DCB.rD)
        m_dHsSumHX = *DCB.rD*TimeIncAtSolve();
      DCB.D=m_dHsSumHX/TimeIncAtSolve();
      return 1;
    case xidRCTDutyPwr:
      if (DCB.rD)
        m_dHsSumPwr = *DCB.rD*TimeIncAtSolve();
      DCB.D=m_dHsSumPwr/TimeIncAtSolve();
      return 1;
    case xidRCTFdDutySrc:
      if (DCB.rD)
        m_SrcB.m_Hf_FdT = *DCB.rD*TimeIncAtSolve();
      DCB.D = m_SrcB.m_Hf_FdT/TimeIncAtSolve();
      return 1;
    case xidRCTPrDutySrc:
      if (DCB.rD)
        m_SrcB.m_Hf_PrT = *DCB.rD*TimeIncAtSolve();
      DCB.D = m_SrcB.m_Hf_PrT/TimeIncAtSolve();
      return 1;
    case xidRCTFdDutySnk:
      if (DCB.rD)
        m_SnkB.m_Hf_FdT = *DCB.rD*TimeIncAtSolve();
      DCB.D = m_SnkB.m_Hf_FdT/TimeIncAtSolve();
      return 1;
    case xidRCTPrDutySnk:
      if (DCB.rD)
        m_SnkB.m_Hf_PrT = *DCB.rD*TimeIncAtSolve();
      DCB.D = m_SnkB.m_Hf_PrT/TimeIncAtSolve();
      return 1;
    case xidRCTFdDutySrcSnk:
      DCB.D=HfSumSrcSnk(false)/TimeIncAtSolve();
      return 1;
    case xidRCTPrDutySrcSnk:
      DCB.D=HfSumSrcSnk(true)/TimeIncAtSolve();
      return 1;
    case xidRCTFdHfErr:
      if (DCB.rD)
        m_dHfErrFdT = *DCB.rD*TimeIncAtSolve();
      DCB.D = m_dHfErrFdT/TimeIncAtSolve();
    case xidRCTPrHfErr:
      if (DCB.rD)
        m_dHfErrPrT = *DCB.rD*TimeIncAtSolve();
      DCB.D = m_dHfErrPrT/TimeIncAtSolve();
      return 1;
    case xidRCTName:
      if (DCB.rpC!=NULL)
        {
        // Dont Set if coming from snapshot
        Strng File0, Path0;
        File0.FnNameExt(DCB.rpC);
        Path0.FnDrivePath(DCB.rpC);

        Strng File1, Path1;
        File1.FnNameExt(sRCTName());
        Path1.FnDrivePath(sRCTName());

        Strng S;
        S=Path0.Length()>0 ? Path0() : Path1();
        S+=File0.Length()>0 ? File0() : File1();

        Load(LoadForDynamic(), S(), false);//DCB.ForView());
        hEditProcess = NULL; // Kill this Filename Changed
        }
      if (DCB.ForView())
        {
        sRCTFile.FnNameExt(sRCTName());
        DCB.pC= sRCTFile();
        }
      else
        DCB.pC= sRCTName();
      return 1;
    case xidRCTPath:
      sRCTPath.FnDrivePath(sRCTName());
      if (SymbolicPaths())
        sRCTPath.FnContract();
      else
        sRCTPath.FnExpand();
      DCB.pC= sRCTPath();
      return 1;
    case xidBrowseRCTBtn:
      if (DCB.rB && (*DCB.rB!=0))
        {
        Strng Ext("*.rct");
        Strng Dir=FnDefaultReactionsFolder();
        CSCDFileDialog Dlg(true, NULL, Ext(), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR | OFN_SHAREAWARE, "RCT Files (*.RCT)|*.RCT||");
        Dlg.m_ofn.lpstrInitialDir = Dir();
        Dlg.m_ofn.lpstrTitle = "Reaction File";
        HWND H=::GetActiveWindow();
        if (Dlg.DoModal()==IDOK)
          sRCTName=Dlg.GetPathName();
        ::SetActiveWindow(H);
        DCB.B=0;
        sRCTName.FnContract();

        bReloadReqd=true;
        if (bAutoReload)
          Load(LoadForDynamic(), sRCTName(), false);//DCB.ForView());
//        ValidateData(ValidateDataBlk & VDB);
        }
      return 1;
    case xidRCTOn:
      if (DCB.rB)
        {
        fOn=*DCB.rB;
        Load(LoadForDynamic(), sRCTName(), DCB.ForView());
        pAttachedTo->StructureChanged(pAttachedTo);
        }
      DCB.B=fOn;
      return 1;
    case xidReloadRCTBtn:
      if (DCB.rB && (*DCB.rB!=0))
        Load(LoadForDynamic(), sRCTName(), DCB.ForView());
      DCB.B=0;
      return 1;
    case xidEditRCTBtnTxt:
      if (DCB.rB && (*DCB.rB!=0))
        {
        EditTime.dwLowDateTime=0;
        EditTime.dwHighDateTime=0; 
        if (sRCTName.Length()==0)
          {                          
          sRCTName=FnDefaultReactionsFolder();
          sRCTName+=FullObjTag();
          if (sRCTName.Len()>3)
            sRCTName=sRCTName.Left(sRCTName.Len()-3); //strip .RB from end
          sRCTName+=".rct";
          }
        Strng FileName(sRCTName());
        Strng FullFilename;
        int RetCode = DCB.DoEditBtn(hEditProcess, dwEditProcessId, "", FileName(), "rct", &FullFilename, true, FnDefaultReactionsFolder()());
        if (RetCode==-1)
          {
          Strng RctCfg(CfgFiles(), "Template.rct");
          Strng RctBase(BaseCfgFiles(), "Template.rct");
          if (FileExists(RctCfg()))
            {
            CopyFile(RctCfg(), FullFilename(), true);
            }
          else if (FileExists(RctBase()))
            {
            CopyFile(RctBase(), FullFilename(), true);
            }
          else
            {
            FILE* f = fopen(FullFilename(), "wt");
            if (f)
              {
              fwrite(DefaultRCT1, sizeof(char), DefaultRCT1Len, f);
              char Buff[256];
              for (int i=0; i<SVCfgCount(); i++)
                {
                CSysVecItem &I=*SVI.Cfg(i);
                if (I.Type()==SVT_Specie)
                  {
                  const int iSp=I.SDBIndex();
                  sprintf(Buff, "%s\n", SDB[iSp].SymOrTag());
                  fwrite(Buff, sizeof(char), strlen(Buff), f);
                  }
                }
              fwrite(DefaultRCT2, sizeof(char), DefaultRCT2Len, f);
              fclose(f);
              }
            }
          sRCTName = FullFilename();
          sRCTName.FnContract();
          FileName = sRCTName();
          RetCode = DCB.DoEditBtn(hEditProcess, dwEditProcessId, "", FileName(), "rct");
          }
        if (RetCode==0)
          {
          SYSTEMTIME ST;
          GetSystemTime(&ST);
          SystemTimeToFileTime(&ST, &EditTime);
          }
        }
      DCB.B=0;
      return 1;
    case xidEditRCTBtnDlg:
      if (DCB.rB && (*DCB.rB!=0))
        {
        EditTime.dwLowDateTime=0;
        EditTime.dwHighDateTime=0; 
        if (sRCTName.Length()==0)
          {                          
          sRCTName=FullObjTag();
          if (sRCTName.Len()>3)
            sRCTName=sRCTName.Left(sRCTName.Len()-3); //strip .RB from end
          sRCTName+=".rct";
          }
        //Strng FileName(sRCTName());
        //FileName.FnExpand();
        //Strng FullFilename;
       
        char* pTag = new char[strlen(Nd()->FullObjTag())+1];
        strcpy(pTag, Nd()->FullObjTag());
        char* pFileName = new char[strlen(sRCTName())+1];
        strcpy(pFileName, sRCTName());
        ScdMainWnd()->PostMessage(WMU_EDITRCTDLG, (WPARAM)pTag, (LPARAM)pFileName);

        //int RetCode = DCB.DoEditBtn(hEditProcess, dwEditProcessId, "", FileName(), "rct", &FullFilename, true);
        //if (RetCode==-1)
        //  {
        //  FILE* f = fopen(FullFilename(), "wt");
        //  if (f)
        //    {
        //    fwrite(DefaultRCT1, sizeof(char), DefaultRCT1Len, f);
        //    char Buff[256];
        //    for (int i=0; i<SVCfgCount(); i++)
        //      {
        //      CSysVecItem &I=*SVI.Cfg(i);
        //      if (I.Type()==SVT_Specie)
        //        {
        //        const int iSp=I.SDBIndex();
        //        sprintf(Buff, "%s\n", SDB[iSp].SymOrTag());
        //        fwrite(Buff, sizeof(char), strlen(Buff), f);
        //        }
        //      }
        //    fwrite(DefaultRCT2, sizeof(char), DefaultRCT2Len, f);
        //    fclose(f);
        //    }
        //  RetCode = DCB.DoEditBtn(hEditProcess, dwEditProcessId, "", FileName(), "rct");
        //  }
        //if (RetCode==0)
        //  {
        //  SYSTEMTIME ST;
        //  GetSystemTime(&ST);
        //  SystemTimeToFileTime(&ST, &EditTime);
        //  }
        }
      DCB.B=0;
      return 1;
    case xidRCTdRctTol:
      if (DCB.rD)
        m_RctTol.SetRel(*DCB.rD);
      DCB.D=m_RctTol.GetRel();
      return 1;
    case xidRCTiMaxIter:
      if (DCB.rL)
        m_RctTol.SetMaxIters(*DCB.rL);
      DCB.L=m_RctTol.GetMaxIters();
      return 1;
    case xidRCTdRctTTol:
      if (DCB.rD)
        m_RctTTol.SetRel(*DCB.rD);
      DCB.D=m_RctTTol.GetRel();
      return 1;
    case xidRCTiMaxTIter:
      if (DCB.rL)
        m_RctTTol.SetMaxIters(*DCB.rL);
      DCB.L=m_RctTTol.GetMaxIters();
      return 1;
    case xidRCTNumOfRct:
      DCB.L=NSrcs() + NEqns() + NSnks();
      return 1;
      
    default :
      {
//      if (DCB.lHandle>= xidRCTSpRT && DCB.lHandle<xidRCTSpRT+MaxSpecies)
//        {
//        int i=DCB.lHandle-xidRCTSpRT;
//        SpMRT.SetSize(Max((int)SpMRT.GetLen(), i+1));
//        if (DCB.rD)
//          SpMRT[i]=*DCB.rD;
//        DCB.D=SpMRT[i];
//        return 1;
//        }
      break;
      }
    }
  
    if (TaggedObject::DataXchg(DCB))
      return 1;
    return 0;
  };

//---------------------------------------------------------------------------

flag CReactionBlock::UsingStateSemantics() { return m_RctBase->m_bUsingStateSemantics!=0; };

//static int LoadAttempted=0;

flag CReactionBlock::ValidateData(ValidateDataBlk & VDB)
  {
  flag RetOK=true;
  if (fOn && m_RctBase->fEnabled)
    {
    if (LoadForDynamic() != UsingStateSemantics())
      {
      Load(LoadForDynamic(), sRCTName(), true);
      }

    SetCI(2, bReloadReqd);

    if (NSeq())
      {
      iReactionState=RBS_OK;

      dword Flags=SetTheCIs();
      if (Flags & CRS_StopExec)
        iReactionState=RBS_Error;
      }

    if (sRCTName.Length()==0)
      iReactionState=RBS_Empty;
    if (fOn && (iReactionState==RBS_Error))
      {
      if (gs_Exec.Running())// CNM || gs_Exec.SolvingPB())
        LogWarning(/*BasteTag()*/FullObjTag(), 0, "Stopping : Invalid Reaction Data");
      FlwNode::ExecObj()->XStop();
      RetOK=false;
      }
    }
  else
    {
    m_LoadState="Off";
    m_LoadError="";
    iReactionState=RBS_OK;
    for (int i=0; i<16; i++)
      ClrCI(i);
    }

  m_DynRateScale       = Range(0.001, m_DynRateScale, 1000.0);

  return RetOK;
  };

dword CReactionBlock::SetTheCIs()
  {
  dword Flags=0;
  for (int k = 0; k < NSeq(); k++)
    Flags|=Seq[k]->Check();
  SetCI( 8, (Flags & CRS_NoBalance)!=0);
  SetCI( 9, (Flags & CRS_BadCps)!=0);
  SetCI(10, (Flags & CRS_BadHfs)!=0);
  SetCI(11, (Flags & CRS_NoExtent)!=0);
  SetCI(12, (Flags & CSR_NotAvail)!=0);
  return Flags;
  };

//---------------------------------------------------------------------------

const byte SrOD_ID   = 0x31;
const byte SrOD_End  = 0x34;

flag CReactionBlock::GetOtherData(FilingControlBlock &FCB)
  {
  if (FCB.SaveAs() && hEditProcess)
    {//during a save-as we must reset the link to the "old" edit files...
    //LogNote(FullObjTag(), 0, "Remove link to currently open rct file. (File should be closed)");
    hEditProcess = NULL; //this is probably not needed for "far-away" files where location does not change
    }
  return True;
  };

// --------------------------------------------------------------------------

flag CReactionBlock::PutOtherData(FilingControlBlock &FCB)
  {
  return True;
  };

//---------------------------------------------------------------------------

void CReactionBlock::OnAppActivate(BOOL bActive)
  {
  if (fOn && bActive && fFileLoadAllowed)
    {
    if (1)//iReactionState==RBS_OK || iReactionState==RBS_Error || iReactionState==RBS_Empty)
      {
      Strng sFn;

      bool Reload=false;
      if (iReactionState==RBS_Empty && sRCTName())
        {
        Reload=true;
        }

      if (!Reload && sRCTName() && (EditTime.dwLowDateTime || EditTime.dwHighDateTime))
        {
        if (sFn.FnExpandQueryReload(sRCTName(), EditTime))
          {
          Reload=true;
          EditTime.dwLowDateTime=0;
          EditTime.dwHighDateTime=0;
          }
        }

      if (!Reload && pTkns)
        {
        for (int i=0; i<pTkns->GetFilesUsedCount(); i++)
          if (sFn.FnExpandQueryReload((char*)pTkns->GetIncFileName(i), pTkns->GetIncFileTime(i)))
            Reload=true;
        }

      if (Reload)
        {
        bReloadReqd=true;
        if (bAutoReload)
          {
          Load(LoadForDynamic(), sRCTName(), true);
          char* pTag = new char[strlen(BaseTag())+1];
          strcpy(pTag, BaseTag());
          ScdMainWnd()->PostMessage(WMU_TAGACTION, SUB_TAG_ACCREFRESHSAVE, (LPARAM)pTag);
          }
        ValidateDataBlk VDB;
        ValidateData(VDB);
        }
      }
    }
  }

//---------------------------------------------------------------------------

int CReactionBlock::FilesUsed(CFilesUsedArray & Files)
  {
  if (sRCTName.Len()>0)
    {
    Strng ss;
    for (int i=0; i<pTkns->GetFilesUsedCount(); i++)
      Files.AddFile(FnExpand(ss, (char*)pTkns->GetIncFileName(i)), FU_CopyFile|FU_EditTxt);
    return pTkns->GetFilesUsedCount();

    return 1;
    }
  return 0;
  };

//---------------------------------------------------------------------------

flag CReactionBlock::SetRCTName(pchar pRCTName)
  {
  if (pRCTName)
    sRCTName=pRCTName;
  sRCTName.LRTrim();
  return true;
  };

//---------------------------------------------------------------------------

flag CReactionBlock::Load(flag UsingStateSemantics, pchar pRCTName, flag KeepState)
  {
  //if (LoadAttempted)
  //  return False;
  //LoadAttempted=true;
  
  m_RctBase->m_bUsingStateSemantics=UsingStateSemantics!=0;
  //m_RctBase->m_bHasStateSemantics=UsingStateSemantics!=0;
  //m_bIs//m_RctBase->m_bCanBeSurge=bIsSurgeType;

  // Save the current values
  CXM_ObjectData CurrentData(0);
  if (KeepState)
    GetDefinedData(Tag(), CurrentData.List, NSHM_All|TABOpt_ForFile|TABOpt_AllInfo, NULL);

  MyTagsHaveChanged();

  Clear(true, false);
  iReactionState=RBS_Error;

  if (pRCTName)
    sRCTName=pRCTName;
  sRCTName.LRTrim();

  if (!fFileLoadAllowed)
    Error(this, 'B', 9);

  if (sRCTName()==NULL)
    {
    iReactionState=RBS_Empty;
    bReloadReqd=false;
    return False;
    }
  //dbgpln("------ %s", sRCTName());

  sRCTName.FnCheckExtension("rct");
  sRCTName.FnSearchExpand(NULL, AF_All|AF_BackupFiles);

  //dbgpln("       %s", sRCTName());
  flag FileFound = FnModifyTime(sRCTName(), RCTTime);

  if (!FileFound && sRCTName.Len()>0)
    {
    //let's see if the reaction file has been moved to the "Reactions" sub folder...
    Strng Tmp(sRCTName);
    Tmp.FnNameExt();
    Strng Tmp2 = FnDefaultReactionsFolder();
    Tmp2 += Tmp;
    FileFound = FnModifyTime(Tmp2(), RCTTime);
    if (FileFound)
      {
      sRCTName = Tmp2();
      LogNote(FullObjTag(), 0, "Reaction file (%s) folder location changed to '\\Reactions'.", Tmp());
      }
    }

  delete pTkns;
  pTkns = new CRCTTokenFile(this, AF_All|AF_BackupFiles, sRCTName());
  pTkns->SetSeperators(";,=: \t\v\f"); //"+;,=: "); taking out the + and - means that +/- may be included in Specie Names
  pTkns->SetWhiteSpace(" \t\v\f"); //WhiteSpace characters MUST also be in the seperators list!!!

  //dbgpln("       %s", sRCTName());
  if (pTkns->Open())
    {
    sRCTName.FnContract();
    //dbgpln("       %s", sRCTName());
    try
      {
      m_LoadState="OK";
      iReactionState=RBS_OK;

      pTkns->SetSeperators(";,=: \t\v\f");
      pTkns->SetWhiteSpace(" \t\v\f");
      Parse(*pTkns);
      SetUpReactions();
      #if dbgReaction
      if (dbgRctLoad())
        {
        dbgDump();
        dbgDumpPostCheckData();
        }
      #endif
      //CheckReactions();
      bReloadReqd=false;
      }
    catch (RBException thePGMException)
      {
      Clear(false, true);
      iReactionState=RBS_Error;
      dbgpln("** File Reset Exeption During Compile ** %i", thePGMException.m_Reason);
      }
    pTkns->Close();

    if (KeepState)
      {
      // Reload the values before load
      CPkDataItem * pItem=CurrentData.FirstItem();
      ASSERT(pItem && pItem->Type()==tt_Object);
      CPkDataIter Iter;
      // removing TABOpt_ForFile creates restore issues (Messages)
      DWORD dwOptions=TABOpt_ForFile|TABOpt_AllInfo;
      PutDefinedData(pItem->SymOrTag(), CurrentData.List, Iter, dwOptions, NULL);
      }

    StructureChanged(this);
    return True;
    }
  else
    {
    m_LoadState=nPermReacts>0 ? "Only Permanent Reactions" : "No Reactions";
    //m_State.Set("NO REACTIONS");
    if (sRCTName.Len()>0)
      {
      Strng Tmp(sRCTName);
      Tmp.FnNameExt();
      /*if (SymbolicPaths())
        Tmp.FnContract();
      else
        Tmp.FnExpand();*/
      LogStop(FullObjTag(), 0, "Reaction:%s File not opened!!!", Tmp());
      m_LoadError.Set("%s File not opened!!!", Tmp());
      }
    else
      {
      bReloadReqd=false;
      iReactionState=RBS_OK;
      }
    }

//  delete pTkns;
//  pTkns=NULL;

  StructureChanged(this);
  //if (gs_Exec.Running() || gs_Exec.SolvingPB())
  //  if (iReactionState==RBS_Error)
  //    LogStop(FullObjTag(), 0, "Error in Reactions");

  return False;
  };

//---------------------------------------------------------------------------

CReaction *CReactionBlock::AddReaction(TagObjClass * pEqnClass)
  {
  CReaction* pRct=(CReaction*)pEqnClass->Construct(NULL, "Rxx", this, TOA_Embedded);
  return AddReaction(pRct);
  }

//---------------------------------------------------------------------------

CReaction *CReactionBlock::AddReaction(CReaction *pRct)
  {
  Strng RTag;
  ASSERT_ALWAYS(nPermReacts==NAll(), "Bad Permanent Reactions", __FILE__, __LINE__);

  RTag.Set("R%i",nPermReacts+1);
  pRct->SetTag(RTag());
  pRct->EqnIDNo=nPermReacts;

  All.Add(pRct);
  nPermReacts++;

  StructureChanged(this);
  return pRct;
  }

//---------------------------------------------------------------------------

void CReactionBlock::Set_SpcMoles(SpModel & Mdl, CSysVector & M, CSysVector & Moles, CSysVector & LockUp)
  {
  SQLockup *pLock=SQLockup::Ptr(&Mdl, False);
  for (int s = 0; s < SVSpcCount(); s++)
    {
    Moles.VValue[s]  = M[s]/ MolMass[s];
    Lockup.VValue[s] = (pLock) ? Moles[s]*pLock->Lockup[s] : 0.0;
    Moles.VValue[s] -= Lockup[s];
    #ifdef dbgRCTUtils
    dbgpln("M: %12.2f %12.2f %12.2f %s", M[s], MolMass[s], Moles[s], SDB[s].Name());
    #endif
    }
  }

//---------------------------------------------------------------------------

void CReactionBlock::Get_SpcMoles(SpConduit & C, CSysVector & Moles, CSysVector & Lockup)
  {
  SQLockup *pLock=SQLockup::Ptr(C.Model(), False);
  // SaveProducts
  for (int s = 0; s < SVSpcCount(); s++)
    if (pLock)
      {
      Moles.VValue[s]+=Lockup[s];
      pLock->Lockup[s] = Lockup[s] / GTZ(Moles[s]);
      C.Model()->m_M.VValue[s]=Moles[s]*MolMass[s];
      }
    else
      C.Model()->m_M.VValue[s]=Moles[s]*MolMass[s];

  C.Model()->ClrStatesOK();
  }

//---------------------------------------------------------------------------

void CReactionBlock::Get_SpcMoles(SpContainer & M, CSysVector & Moles, CSysVector & Lockup)
  {
  SQLockup *pLock=SQLockup::Ptr(M.Model(), False);
  // SaveProducts
  for (int s = 0; s < SVSpcCount(); s++)
    if (pLock)
      {
      Moles.VValue[s]+=Lockup[s];
      pLock->Lockup[s] = Lockup[s] / GTZ(Moles[s]);
      M.VValue[s]=Moles[s]*MolMass[s];
      }
    else
      M.VValue[s]=Moles[s]*MolMass[s];
  }

//---------------------------------------------------------------------------

void CReactionBlock::Get_SpcMoles(SpModel & Mdl, CSysVector & Moles, CSysVector & Lockup)
  {
  SQLockup *pLock=SQLockup::Ptr(&Mdl, False);
  // SaveProducts
  for (int s = 0; s < SVSpcCount(); s++)
    if (pLock)
      {
      Moles.VValue[s]+=Lockup[s];
      pLock->Lockup[s] = Lockup[s] / GTZ(Moles[s]);
      Mdl.VValue[s]=Moles[s]*MolMass[s];
      }
    else
      Mdl.VValue[s]=Moles[s]*MolMass[s];
  }

//---------------------------------------------------------------------------

double CReactionBlock::VolWithLockup(PhMask PhaseM, CSysVector & Moles, double T, double P)
  {
  SpMArray M;
  for (int s = 0; s < SVSpcCount(); s++)
    M.VValue[s]=(Moles[s]+Lockup[s])*SDB[s].MoleWt();
  //return M.Mass(PhaseM)/M.Rho(PhaseM, T, P);
  //
  return m_pModelOwn->pModel->Volume(PhaseM, T, P, &M);
  }

//---------------------------------------------------------------------------

double CReactionBlock::ConvergeHXs(CReactionArray &HXs, SpModel * M, double TotHs, double DHfTot, double TStart)//, bool DoTConverge)
  {
  double Sum=0;
  M->SetBadTempOK(true);
  for (int k = 0; k < HXs.GetSize(); k++)
    Sum+=HXs[k]->HeatExchange(M, DHfTot, TStart, M->Temp(), false, true);

  if (fabs(Sum)>1.0e-6)
    {
    double XH=Sum;
    int Loop=100;
    do
      {
      Sum=XH;
      M->Set_totHs(TotHs+Sum, dNAN, Model);
      XH=0.0;
      for (k = 0; k < HXs.GetSize(); k++)
        XH+=HXs[k]->HeatExchange(M, DHfTot, TStart, M->Temp(), false, false);
      }
    while (fabs(XH-Sum)>1.0e-6*fabs(Sum) && --Loop>0);
    SetCI(3, Loop<=0);
    }
  M->SetBadTempOK(false);
  return Sum;
  }

//---------------------------------------------------------------------------

void CReactionBlock::AdjustForSrcSnks(SpMArray & M)
  {
  for (int k = 0; k < NSrcs(); k++)
    Srcs[k]->SrcSnkAdjust(M);
  for (int k = 0; k < NSnks(); k++)
    Snks[k]->SrcSnkAdjust(M);
  }

//---------------------------------------------------------------------------

//SpModel * CReactionBlock::RefModel()
//  {
//  _asm int 3;
//  //if (m_UseStdRefMdl && !m_StdRefMdl)
//  //  {
//  //  m_StdRefMdl=SpModelClass.Construct();
//  //  XX().SetModel(&SpModelClass);
//  //  m_pRefModel=XX().Model();
//  //  }
//  return NULL;
//  }

class CAttachSpModel
  {
  public:
    CAttachSpModel(CReactionBlock * pRB, SpModelOwner & ModelOwn)
      {
      m_pRB=pRB;
      m_pRB->AttachSpModel(ModelOwn);
      }
    ~CAttachSpModel()
      {
      m_pRB->DetachSpModel();
      };
  protected:
    CReactionBlock * m_pRB;
  };

void CReactionBlock::AttachSpModel(SpModelOwner & ModelOwn)
  {
  m_pModelOwn=&ModelOwn;
  m_pOvr=&ModelOwn.Model()->m_Ovr;

  if (m_UseStdRefModel && (ModelOwn.Model()->Class()!=&SpModelClass))
    {
    SpModel * Mdl=ModelOwn.Model();
    if (m_pStdRefModel==NULL)
      m_pStdRefModel=new SpModel(&SpModelClass, "RefMdl", NULL, TOA_Free);
    m_pRefModel=m_pStdRefModel;
    
    // Copy to Ref Mdl
    m_pRefModel->SetMassF(Mdl, som_ALL, 1.0, Mdl->Press());
    m_pRefModel->SetTempPress(Mdl->Temp(), Mdl->Press());
    //ModelOwn.Model();
    }
  else
    m_pRefModel=ModelOwn.Model();
  };

void CReactionBlock::DetachSpModel()
  {
  if (m_UseStdRefModel && (m_pModelOwn->Model()->Class()!=&SpModelClass))
    {
    // Copy Back
    SpModel & Mdl=*m_pModelOwn->Model();
    Mdl.SetMassF(m_pRefModel, som_ALL, 1.0, m_pRefModel->Press());
    Mdl.SetTempPress(m_pRefModel->Temp(), m_pRefModel->Press());
    }

  m_pRefModel=NULL;
  m_pOvr=NULL;
  m_pModelOwn=NULL;
  };

void CReactionBlock::EvalProducts(SpConduit & Cd, double EstFinalT/*, bool FindFinalT*/)
  {
  m_InWhat = eEvalProducts;    
  m_pCSB=NULL;

  m_TimeIncAtSolve=1.0;//ICGetTimeInc(); these are all in terms of flow already

  CAttachSpModel ASM(this, Cd);

  m_dHfSumTotFdT=0.0;
  m_dHfSumTotPrT=0.0;
  m_dHfSumTotRefT=0.0;
  m_dHfSumTotZeroT=0.0;
  m_dHfSumTotZeroTAtmP=0.0;
  m_dHfErrFdT=0.0;
  m_dHfErrPrT=0.0;
  m_dHfErrZeroT=0.0;
  m_dHsSumPwr=0.0;
  m_dHsSumHX=0.0;
  m_dHfIn=Model->totHf();
  m_dHfOut=m_dHfIn;

  m_HfFdStd0           = 0;
  m_HfPrStd0           = 0;
  m_HfFdStdT           = 0;
  m_HfPrStdT           = 0;
  m_HfFdMdl0           = 0;
  m_HfPrMdl0           = 0;
  m_HfFdMdlT           = 0;
  m_HfPrMdlT           = 0;

  m_Press=Model->Press();
  m_TempK=Model->Temp();
  m_TempKFeed=m_TempK;
  m_TempKRct=m_TempK;
  m_TempKProd=m_TempK;
  m_bFindFinalT=true;//FindFinalT;

  if (!fOn || iReactionState!=RBS_OK)
    return;

  if (Model->Mass(som_ALL)<UsableMass)
    {
    Set_SpcMoles(*Model, Model->MArray(), Moles, Lockup);
    m_TempKProd=m_dEstFinalT; // CNM to Fix Final Product dHf etc - assure that calcs ITO TempKProd are correct
    for (int k = 0; k < NAll(); k++)
      All[k]->Zero();
    Moles_S = Moles;
    DMoles.Zero();
    ClrCI(1);
    ClrCI(2);
    ClrCI(3);
    ClrCI(13);
    ClrCI(14);
    return; //if no flow do nothing!
    }
  #if dbgReaction
  if (dbgRctSolve())
    dbgpln("RctSolve ===========================================================================================");
  #endif

  SetCI(2, bReloadReqd);

  //SpModelExDbgOn++;
  // Load Reaction Conditions
  double MsIn  = Model->Mass();
  double HfIn  = Model->totHf(som_ALL, m_TempKFeed, m_Press, &Model->MArray());
  double HsIn  = Model->totHs(som_ALL, m_TempKFeed, m_Press, &Model->MArray());

  if (Valid(EstFinalT) && !HeatSkipMethod())
    m_dEstFinalT=EstFinalT;
  else
    m_dEstFinalT=m_TempKFeed;

  //double TProdPrev=TempK;
  SpMArray CS(Model->MArray());
  Set_SpcMoles(*Model, Model->MArray(), Moles, Lockup);
  for (int iTIter=0; ; iTIter++)
    {
    m_TempKProd=m_dEstFinalT; // CNM to Fix Final Product dHf etc - assure that calcs ITO TempKProd are correct

    m_RctB.Clear();
    m_SrcB.Clear();
    m_SnkB.Clear();
    m_dHsSumPwr=0.0;
    m_dHsSumHX=0.0;

    if (iTIter==0)
      Moles_S = Moles;
    else
      Moles = Moles_S;
    DMoles.Zero();


    for (int k = 0; k < NSrcs(); k++)
      Srcs[k]->SrcSnkStart();

    for (k = 0; k < NEqns(); )
      {
      int n=1;
      while (k+n<NEqns() && Eqns[k+n]->SeqNo==Eqns[k]->SeqNo)
        n++;

      if(iVersion==3)
        Moles_BS = Moles;
      Eqns[k]->SolveBlock(iVersion, *this, &Eqns[k], n, m_RctB);
      k+=n;
      }

    for (k = 0; k < NSrcs(); k++)
      Srcs[k]->SrcSnkEnd(m_SrcB);
    for (k = 0; k < NSnks(); k++)
      Snks[k]->SrcSnkEnd(m_SnkB);

    Get_SpcMoles(*Model, Moles, Lockup);

    // Set Temp to that at start & add heats of reaction and HeatExchange
    Model->SetTempPress(m_TempKFeed, m_Press);
    if (!HeatSkipMethod())
      {
      // Calculate Differences
      double HsXXX    = Model->totHs(som_ALL, m_TempKFeed, m_Press, &Model->MArray()); 

      double HfFdT    = Model->totHf(som_ALL, m_TempKFeed, m_Press, &Model->MArray()); 
      double HfPrT    = Model->totHf(som_ALL, m_TempKProd, m_Press, &Model->MArray()); 
      double HfInPrT  = Model->totHf(som_ALL, m_TempKProd, m_Press, &CS);
      double Hf0T     = Model->totHf(som_ALL, ZeroCinK,    m_Press, &Model->MArray());
      double HfIn0T   = Model->totHf(som_ALL, ZeroCinK,    m_Press, &CS);

      m_dHfErrFdT=(HfFdT)-(HfIn+m_SrcB.m_Hf_FdT-m_SnkB.m_Hf_FdT)-m_RctB.m_Hf_FdT;
      m_dHfErrPrT=(HfPrT)-(HfInPrT+m_SrcB.m_Hf_PrT-m_SnkB.m_Hf_PrT)-m_RctB.m_Hf_PrT;
      m_dHfErrZeroT=(Hf0T)-(HfIn0T+m_SrcB.m_Hf_ZeroT-m_SnkB.m_Hf_ZeroT)-m_RctB.m_Hf_ZeroT;

      // Calculate Final Temp
      double TotHs = HsIn - m_RctB.m_HOR; // this HOR is HOR at 0C and StdP

      for (k = 0; k < NPwrs(); k++)
        m_dHsSumPwr+=Pwrs[k]->HeatExchange(Model, m_RctB.m_Hf_FdT, Model->Temp(), Model->Temp(), true, true);

      m_dHsSumHX+=ConvergeHXs(HXs1, Model, TotHs, m_RctB.m_Hf_FdT, m_TempKFeed);
      TotHs+=m_dHsSumPwr+m_dHsSumHX;
      Model->Set_totHs(TotHs);

      m_TempKRct=Model->Temp();
      SetCI(14, m_TempKRct<50.0);

      //double x1=C.totCp();
      //double x2=C.msCp();

      m_dHsSumHX+=ConvergeHXs(HXs2, Model, TotHs, m_RctB.m_Hf_FdT, m_TempKRct);

      //if (!FindFinalT)
      if (!m_bFindFinalT && Valid(EstFinalT)) //kga 7/1/2002
        {
        double H0=Model->totHf();
        Model->SetTemp(EstFinalT);
        m_dHsSumHX+=H0-Model->totHf();
        }
      m_TempKProd=Model->Temp();
      m_dHfOut=Model->totHf();

      #if dbgReaction
      if (dbgRctSolve())
      dbgpln(">>>>>>>>>>>>>> %3i T:f:%10.2f p:%10.2f  e:%10.2f Hf:%12.3f %12.3f",
        iTIter, K2C(m_TempKFeed), K2C(m_TempKProd), K2C(m_dEstFinalT), m_dHfIn, m_dHfOut);
      #endif
      
      // if the final ORV clause is used then Info @ ProdTemp is incorrect
      if (fabs(m_dEstFinalT-m_TempKProd)<m_RctTTol.GetRel())// || !m_bFindFinalT)
        break;
      m_dEstFinalT=m_TempKProd;
      if (iTIter>=m_RctTTol.GetMaxIters())// || !!DoTConverge)
        {
        #if dbgReaction
        if (dbgRctSolve())
        dbgpln(">>>>>>>>>>>>>> MAX ITERATIONS");
        #endif
        break;
        }
      }
    else
      break;
    }
  SetCI(13, iTIter>=m_RctTTol.GetMaxIters());

  //SpModelExDbgOn--;

  for (int k = 0; k < NAll(); k++)
    All[k]->Finalise(*this);

  m_dHfSumTotFdT=m_dHsSumHX+m_dHsSumPwr-m_RctB.m_Hf_FdT;
  m_dHfSumTotPrT=m_dHsSumHX+m_dHsSumPwr-m_RctB.m_Hf_PrT;
  m_dHfSumTotRefT=m_dHsSumHX+m_dHsSumPwr-m_RctB.m_Hf_RefT;
  m_dHfSumTotZeroTAtmP=m_dHsSumHX+m_dHsSumPwr-m_RctB.m_dHfZeroTAtmP;

  StkSpConduit ACd("Std", chLINEID(), dynamic_cast<FlwNode*>(pAttachedTo));

  ACd().ChangeModel("Standard", false); // Standard SysCAD
  m_HfFdStd0=ACd().totHf(som_ALL, C2K(0), Model->Press(), &CS);
  m_HfPrStd0=ACd().totHf(som_ALL, C2K(0), Model->Press(), *Model);
  m_HfFdStdT=ACd().totHf(som_ALL, m_TempKFeed, Model->Press(), &CS);
  m_HfPrStdT=ACd().totHf(som_ALL, m_TempKFeed, Model->Press(), *Model);

  if (Model->Class()!=ACd().Model()->Class())
    {
    ACd().ChangeModel(Model->Class()); 
    m_HfFdMdl0=ACd().totHf(som_ALL, C2K(0), Model->Press(), &CS);
    m_HfPrMdl0=ACd().totHf(som_ALL, C2K(0), Model->Press(), *Model);
    m_HfFdMdlT=ACd().totHf(som_ALL, m_TempKFeed, Model->Press(), &CS);
    m_HfPrMdlT=ACd().totHf(som_ALL, m_TempKFeed, Model->Press(), *Model);
    }
  else
    {
    m_HfFdMdl0=m_HfFdStd0;
    m_HfPrMdl0=m_HfPrStd0;
    m_HfFdMdlT=m_HfFdStdT;
    m_HfPrMdlT=m_HfPrStdT;
    }

  SetTheCIs();

  #if dbgReaction
  if (dbgRctSolve())
    {
    double sum1=0.0,sum2=0.0,sum3=0.0,sum4=0.0;
    dbgpln("----------------------------------------------------------------------------------------------------");
    dbgpln("%-15.15s %14s    %14s   %14s    %14s   %14s : %s","EvalProducts","Moles","Moles","kg","kg","Lockup(Moles)", pAttachedTo->Tag());
    for (int i = 0; i<NumSpecies; i++)
      {
      if (fabs(Moles_S[i]-Moles[i])>1.0e-6*Max(Moles_S[i], Moles[i]))
        dbgpln("%-15.15s %14.6g -> %14.6g  [%14.6g -> %14.6g] %14.6g",
                SDB[i].SymOrTag(),Moles_S[i],Moles[i],
                Moles_S[i]*SDB[i].MoleWt(),
                Moles[i]*SDB[i].MoleWt(),
                Lockup[i]);
      sum1+=Moles_S[i];
      sum2+=Moles[i];
      sum3+=Moles_S[i]*SDB[i].MoleWt();
      sum4+=Moles[i]*SDB[i].MoleWt();
      }
    dbgpln("%-15.15s %14.6g    %14.6g  [%14.6g    %14.6g]","TOTAL",sum1,sum2,sum3,sum4);
    dbgpln(" Temperature    %12.2f -> %12.2f K",m_TempKFeed,Model->Temp());
    dbgpln(" Change in heat %12.2f = %12.2f+%12.2f+%12.2f+%12.2f+%12.2f",
      m_dHfSumTotFdT, m_dHsSumHX, m_dHsSumPwr, m_SrcB.m_Hf_FdT, m_SnkB.m_Hf_FdT, m_RctB.m_Hf_FdT);
    dbgpln(" Enthalpy In    %12.2f  ",m_dHfIn);
    dbgpln(" Enthalpy Out   %12.2f  ",m_dHfOut);
    dbgpln(" Enthalpy Chg   %12.2f  ",m_dHfOut-m_dHfIn);
    //dbgpln(" H1f-HfIn        %12.2f  ",H1f-HfIn);
    //dbgpln(" H1s-HsIn        %12.2f  ",H1s-HsIn);
    dbgpln(" Mass In        %12.2f  ",MsIn);
    dbgpln(" Mass Out       %12.2f  ",Model->Mass());
    dbgpln("====================================================================================================");
    //dbgpln("");
    }
  #endif

  flag SpOK=True;
  for (int s=0; s<NumSpecies; s++)
    if (Moles[s]<0.0)
      SpOK=False;
  SetCI(1, !SpOK);

  };

//---------------------------------------------------------------------------

void CReactionBlock::EvalDerivs(SpContainer & Cn, SpConduit & Fd, SpConduit & Pr)
  {
  m_InWhat = eEvalDerivs;      
  //m_pRefModel=M.Model();
  m_pCSB=NULL;

  m_TimeIncAtSolve=ICGetTimeInc();

  CAttachSpModel ASM(this, Cn);

  if (GetActiveHold() || !fOn || iReactionState!=RBS_OK)
    return;

  double H0=Model->totHf();
  m_TempK=Model->Temp();
  m_TempKFeed=m_TempK;
  m_Press=Model->Press();

  Set_SpcMoles(*Model, Model->MArray(), Moles, Lockup);
  Moles_S = Moles;
  Moles_BS = Moles;
  DMoles.Zero();

  Get_SpcMoles(*Model, Moles, Lockup);

  Model->Set_totHf(H0);//+dHfSumTot);
  };

//---------------------------------------------------------------------------

void CReactionBlock::ConvergeStates(CConvergeStateBlk &CSB, SpModelOwner /*Container*/ & Cn, SpVector * MeanResTimes, /*SpConduit & Fd, SpConduit & Pr,*/ double EstFinalT/*=dNAN*/)
  {
  //static int ForceProblemValues=0;
  //static int OnceOff = 1;
  //switch (ForceProblemValues) // MosRef crash values
  //  {
  //  case 1:
  //    Cn.VValue[0]=264.40405659734;
  //    Cn.VValue[1]=19.108073222274;
  //    Cn.VValue[2]=50.189010490158;
  //    Cn.VValue[3]=48.399918594287;
  //    Cn.VValue[4]=0.00060631039167093;
  //    Cn.VValue[5]=0.0023403580112748;
  //    Cn.VValue[6]=4.3002712309348e-007;
  //    Cn.VValue[7]=0.00021849285752143;
  //    Cn.VValue[8]=1.4820807521319e-008;
  //    Cn.VValue[9]=6.5821494135221;
  //    Cn.VValue[10]=1.5576441443300e-006;
  //    Cn.VValue[11]=8.0146658072286e-005;
  //    Cn.VValue[12]=1603.1990651912;
  //    Cn.VValue[13]=45.360651603192;
  //    Cn.pModel->ClrStatesOK();
  //    Cn.SetTempPress(893.14957804925, 2031.3642011385);
  //    break;
  //  case 2:
  //    Cn.VValue[0] =0.0067635214785174;
  //    Cn.VValue[1] =0.00015639024386595;
  //    Cn.VValue[2] =0.00091120779244015;
  //    Cn.VValue[3] =8.8041197585011e-007;
  //    Cn.VValue[4] =0.00000000000000;
  //    Cn.VValue[5] =0.00000000000000;
  //    Cn.VValue[6] =0.00000000000000;
  //    Cn.VValue[7] =0.00000000000000;
  //    Cn.VValue[8] =0.00000000000000;
  //    Cn.VValue[9] =0.00018771531123123;
  //    Cn.VValue[10]=4.6045594839881e-005;
  //    Cn.VValue[11]=1.5310138648903e-006;
  //    Cn.VValue[12]=2774.1820173737;
  //    Cn.VValue[13]=46.149296324042;
  //    Cn.pModel->ClrStatesOK();
  //    Cn.SetTempPress(745.38433780908, 2081.9871796650);
  //    break;
  //  case 3:
  //    Cn.VValue[0] = 0.0019888187933594;
  //    Cn.VValue[1] = 0.00000000000000  ;
  //    Cn.VValue[2] = 0.00026927085636111;
  //    Cn.VValue[3] = 0.00000000000000;
  //    Cn.VValue[4] = 0.00000000000000;
  //    Cn.VValue[5] = 0.00000000000000;
  //    Cn.VValue[6] = 0.00000000000000;
  //    Cn.VValue[7] = 0.00000000000000;
  //    Cn.VValue[8] = 0.00000000000000;
  //    Cn.VValue[9] = 5.6069623102990e-005;
  //    Cn.VValue[10]= 1.3990472763924e-005;
  //    Cn.VValue[11]= 2.6146802779345e-005;
  //    Cn.VValue[12]= 2840.0472552751;
  //    Cn.VValue[13]= 0.83555882415012;
  //    Cn.pModel->ClrStatesOK();
  //    Cn.SetTempPress(727.29310672262, 2075.4234753198);
  //    break;
  //  }
  //if (OnceOff)
  //  ForceProblemValues=0;

  CAttachSpModel ASM(this, Cn);

  m_dHfSumTotFdT=0.0;
  m_dHfSumTotPrT=0.0;
  m_dHfSumTotRefT=0.0;
  m_dHfSumTotZeroT=0.0;
  m_dHfSumTotZeroTAtmP=0.0;
  m_dHfErrFdT=0.0;
  m_dHfErrPrT=0.0;
  m_dHfErrZeroT=0.0;
  m_dHsSumPwr=0.0;
  m_dHsSumHX=0.0;
  m_RctB.Clear();
  m_SrcB.Clear();
  m_SnkB.Clear();
  m_dHfIn=Model->totHf();
  m_dHfOut=Model->totHf();

  m_HfFdStd0           = 0;
  m_HfPrStd0           = 0;
  m_HfFdStdT           = 0;
  m_HfPrStdT           = 0;
  m_HfFdMdl0           = 0;
  m_HfPrMdl0           = 0;
  m_HfFdMdlT           = 0;
  m_HfPrMdlT           = 0;

  if (!fOn || iReactionState!=RBS_OK)
    return;

  #if dbgReaction
  if (dbgRctSolve())
    dbgpln("Converge States ---------------------- %s", FullObjTag());
  #endif

  m_TimeIncAtSolve=ICGetTimeInc();

  m_InWhat = eConvergeStates;      
  //m_pRefModel=M.Model();
  m_pCSB=&CSB;

  // Load Reaction Conditions
  double FH0f  = Model->totHf();
  double FH0s  = Model->totHs();

  m_TempK=Model->Temp();
  m_TempKFeed=m_TempK;
  m_TempKRct=m_TempK;
  m_TempKProd=m_TempK;
  m_Press=Model->Press();

  //#pragma chFIXIT(restore this estimate)
  //m_dEstFinalT=TempK;//EstFinalT;
  m_bFindFinalT=true;
  if (Valid(EstFinalT))
    m_dEstFinalT=EstFinalT;
  else
    m_dEstFinalT=m_TempKFeed;

  //Cn.SetMeanResTimeCalcsReqd(true);
  m_pSpMRT=MeanResTimes;//=Cn.MeanResTimes();


  SpMArray MS(Model->MArray());
  Set_SpcMoles(*Model, Model->MArray(), Moles, Lockup);
  Moles_S = Moles;
  DMoles.Zero();
  for (int k = 0; k < NSrcs(); k++)
    Srcs[k]->SrcSnkStart();

  for (k = 0; k < NEqns(); )
    {
    int n=1;
    while (k+n<NEqns() && Eqns[k+n]->SeqNo==Eqns[k]->SeqNo)
      n++;

    if(iVersion==3)
      Moles_BS = Moles;
    Eqns[k]->SolveBlock(iVersion, *this, &Eqns[k], n, m_RctB);

    k+=n;
    }

  for (k = 0; k < NSrcs(); k++)
    Srcs[k]->SrcSnkEnd(m_SrcB);
  for (k = 0; k < NSnks(); k++)
    Snks[k]->SrcSnkEnd(m_SnkB);

  //if (ICStepPreStart())
  //  Moles_DStrt = Moles;

  Get_SpcMoles(*Model, Moles, Lockup);

  // Set Temp to that at start & add heats of reaction and HeatExchange
  Model->SetTempPress(m_TempKFeed, m_Press);
  double HfFdT;
  double FH1z;
  double HfPrT;
  double HfInPrT;

  if (!HeatSkipMethod())
    {
    HfFdT  = Model->totHf(som_ALL, m_TempKFeed, Model->Press());
    FH1z  = Model->totHz(som_ALL, m_TempKFeed, Model->Press());
    HfPrT  = Model->totHf(som_ALL, m_TempKProd, Model->Press());
    HfInPrT  = MS.totHf(Model->Fidelity(), som_ALL, m_TempKProd, Model->Press(), m_pOvr);

    double TotHz = FH1z - m_RctB.m_Hf_FdT;

    m_dHfErrFdT=(HfFdT)-(FH0f+m_SrcB.m_Hf_FdT-m_SnkB.m_Hf_FdT)-m_RctB.m_Hf_FdT;
    m_dHfErrPrT=(HfPrT)-(HfInPrT+m_SrcB.m_Hf_PrT-m_SnkB.m_Hf_PrT)-m_RctB.m_Hf_PrT;
    m_dHfErrZeroT=dNAN;//(HfPrT)-(HfInPrT+m_SrcB.m_Hf_PrT-m_SnkB.m_Hf_PrT)-m_RctB.m_Hf_PrT;

    for (k = 0; k < NPwrs(); k++)
      m_dHsSumPwr+=Pwrs[k]->HeatExchange(Model, m_RctB.m_Hf_FdT, Model->Temp(), Model->Temp(), CSB.Cmd()==eStateAdvance, true);

    m_dHsSumHX+=ConvergeHXs(HXs1, Model, TotHz, m_RctB.m_Hf_FdT, m_TempKFeed);

    TotHz+=m_dHsSumPwr+m_dHsSumHX;

    Model->Set_totHz(TotHz);
    m_TempKRct=Model->Temp();
    SetCI(14, m_TempKRct<50.0);

    m_dHsSumHX=ConvergeHXs(HXs2, Model, TotHz, m_RctB.m_Hf_FdT, m_TempKRct);

    m_dHfSumTotFdT=m_dHsSumHX+m_dHsSumPwr-m_RctB.m_Hf_FdT;
    m_dHfSumTotPrT=m_dHsSumHX+m_dHsSumPwr-m_RctB.m_Hf_PrT;
    m_dHfSumTotRefT=m_dHsSumHX+m_dHsSumPwr-m_RctB.m_Hf_RefT;
    m_dHfSumTotZeroTAtmP=m_dHsSumHX+m_dHsSumPwr-m_RctB.m_dHfZeroTAtmP;

    StkSpConduit ACd("Std", chLINEID(), dynamic_cast<FlwNode*>(pAttachedTo));
    ACd().ChangeModel("Standard", false); // Standard SysCAD
    m_HfFdStd0=ACd().totHf(som_ALL, C2K(0), Model->Press(), &MS);
    m_HfPrStd0=ACd().totHf(som_ALL, C2K(0), Model->Press(), *Model);
    m_HfFdStdT=ACd().totHf(som_ALL, m_TempKFeed, Model->Press(), &MS);
    m_HfPrStdT=ACd().totHf(som_ALL, m_TempKFeed, Model->Press(), *Model);

    if (Model->Class()!=ACd().Model()->Class())
      {
      ACd().ChangeModel(Model->Class()); 
      m_HfFdMdl0=ACd().totHf(som_ALL, C2K(0), Model->Press(), &MS);
      m_HfPrMdl0=ACd().totHf(som_ALL, C2K(0), Model->Press(), *Model);
      m_HfFdMdlT=ACd().totHf(som_ALL, m_TempKFeed, Model->Press(), &MS);
      m_HfPrMdlT=ACd().totHf(som_ALL, m_TempKFeed, Model->Press(), *Model);
      }
    else
      {
      m_HfFdMdl0=m_HfFdStd0;
      m_HfPrMdl0=m_HfPrStd0;
      m_HfFdMdlT=m_HfFdStdT;
      m_HfPrMdlT=m_HfPrStdT;
      }
    
    m_TempKProd=Model->Temp();
    m_dHfOut=Model->totHf();
    }

  SetTheCIs();

  #if dbgReaction
  if (dbgRctSolve())
    {
    double sum1=0.0,sum2=0.0,sum3=0.0,sum4=0.0;
    dbgpln("----------------------------------------------------------------------------------------------------");
    dbgpln("%-15.15s %14s    %14s   %14s    %14s   %14s : %s","ConvergeStates","Moles","Moles","kg","kg","Lockup(Moles)", pAttachedTo->Tag());
    for (int i = 0; i<NumSpecies; i++)
      {
      if (fabs(Moles_S[i]-Moles[i])>1.0e-6*Max(Moles_S[i], Moles[i]))
        dbgpln("%-15.15s %14.6g -> %14.6g  [%14.6g -> %14.6g] %14.6g",
                SDB[i].SymOrTag(),Moles_S[i],Moles[i],
                Moles_S[i]*SDB[i].MoleWt(),
                Moles[i]*SDB[i].MoleWt(),
                Lockup[i]);
      sum1+=Moles_S[i];
      sum2+=Moles[i];
      sum3+=Moles_S[i]*SDB[i].MoleWt();
      sum4+=Moles[i]*SDB[i].MoleWt();
      }
    dbgpln("%-15.15s %14.6g    %14.6g  [%14.6g    %14.6g]","TOTAL",sum1,sum2,sum3,sum4);
    dbgpln(" Temperature    %12.2f -> %12.2f K",m_TempKFeed,Model->Temp());
    dbgpln(" Pressure       %12.2f -> %12.2f K",m_Press,Model->Press());
    if (!HeatSkipMethod())
      {
      dbgpln(" Change in heat %12.2f = %12.2f+%12.2f+%12.2f+%12.2f+%12.2f",
        m_dHfSumTotFdT, m_dHsSumHX, m_dHsSumPwr, m_SrcB.m_Hf_FdT, m_SnkB.m_Hf_FdT, m_RctB.m_Hf_FdT);
      dbgpln(" Enthalpy In    %12.2f  ",m_dHfIn);
      dbgpln(" Enthalpy Out   %12.2f  ",m_dHfOut);
      dbgpln(" Enthalpy Chg   %12.2f  ",m_dHfOut-m_dHfIn);
      dbgpln(" H1f-HfIn        %12.2f  ",HfFdT-FH0f);
      //dbgpln(" H1s-HsIn        %12.2f  ",FH1s-FH0s);
      }
    dbgpln("");
    }
  #endif

  flag SpOK=True;
  for (int s=0; s<NumSpecies; s++)
    if (Moles[s]<0.0)
      SpOK=False;

  m_pCSB=&CSB;
  m_pSpMRT=NULL;
  SetCI(1, !SpOK);
  };

//---------------------------------------------------------------------------

void CReactionBlock::EvalDiscrete(SpContainer & M, SpConduit & Fd, SpConduit & Pr)
  {
  m_InWhat = eDiscrete;    
  if (!fOn || iReactionState!=RBS_OK)
    return;

  // Do Nothing

  }

//--------------------------------------------------------------------------
#if DOPBSMOOTHING
void CReactionBlock::GetMsAGData(CMSJoin & AG)
  {
  AG.m_ConstBG.Clear();
  AG.m_ConstAG.Clear();
  AG.m_PropBG.Clear();
  AG.m_PropAG.Clear();
  if (!fOn || iReactionState!=RBS_OK)
    {
    AG.m_fDataIsValid=false;
    AG.m_Gamma.Clear();
    return;
    }
//  AG.m_PropBG.Init(false);
  AG.m_Gamma.Init(false);

//  m_pModelOwn=&C;//.Model();

//  Moles_S = Moles;

  for (int k = 0; k < NSrcs(); k++)
    Srcs[k]->GetMsAGData(AG);

  for (k = 0; k < NEqns(); k++)
    Eqns[k]->GetMsAGData(AG);

  //int p,r;
  //for (p=0; p<SVSpcCount(); p++)
  //  for (r=0; r<SVSpcCount(); r++)

//dbgpln("------- React .. Prod  Gamma, In, Src = Gamma [T]");
  for (int r=0; r<SVSpcCount(); r++)
    {
    double T=0;
    for (int p=0; p<SVSpcCount(); p++)
      {
      double Src=m_SrcB.dMass[r];
      double In=Moles_S[r]*SDB[r].MoleWt();
      //AG.m_PropBG[p][r]=AG.m_Gamma[p][r]*Src/GTZ(In+Src);
//if(AG.m_Gamma[p][r]>0.0)
//  dbgp(" .. %-20.20s %-20.20s %16.8f %16.8f %16.8f = ",
//       SDB[r].SymOrTag(), SDB[p].SymOrTag(), AG.m_Gamma[p][r], In, Src);
      AG.m_Gamma[p][r]/=GTZ(In+Src);
      if (p!=r)
        T+=AG.m_Gamma[p][r];
//if(AG.m_Gamma[p][r]>0.0)
//  dbgpln(" %16.8f [%16.8f] ", AG.m_Gamma[p][r], T);
      }
    T=GEZ(1.0-T);
    AG.m_Gamma[r][r]=T;
//dbgp(" %16.8f [%16.8f]", AG.m_Gamma[p][r], T);
    if (!InRange(0.0, T, 1.0))
      { int xxx=0; }
    }

  AG.m_ConstBG.Copy(*MsSumSrc(), 1);
  AG.m_ConstAG.Copy(*MsSumSnk(), -1);
//  for (k = 0; k < NSrcs(); k++)
//    {
//    Srcs[k]->SrcSnkEnd(dMass, dHeat);
//    }
//  for (k = 0; k < NSnks(); k++)
//    {
//    Snks[k]->SrcSnkEnd(dMass, dHeat);
//    }


//  #if dbgReaction
//  if (dbgRctSolve())
//    {
//    double sum1=0.0,sum2=0.0,sum3=0.0,sum4=0.0;
//    dbgpln("");
//    dbgpln("%-15.15s %14s    %14s   %14s    %14s   %14s","EvalProducts","Moles","Moles","kg","kg","Lockup(Moles)");
//    for (int i = 0; i<NumSpecies; i++)
//      {
//      if (fabs(Moles_S[i]-Moles[i])>1.0e-6*Max(Moles_S[i], Moles[i]))
//        dbgpln("%-15.15s %14.6g -> %14.6g  [%14.6g -> %14.6g] %14.6g",
//                SDB[i].SymOrTag(),Moles_S[i],Moles[i],
//                Moles_S[i]*SDB[i].MoleWt(),
//                Moles[i]*SDB[i].MoleWt(),
//                Lockup[i]);
//      sum1+=Moles_S[i];
//      sum2+=Moles[i];
//      sum3+=Moles_S[i]*SDB[i].MoleWt();
//      sum4+=Moles[i]*SDB[i].MoleWt();
//      }
//    dbgpln("%-15.15s %14.6g    %14.6g  [%14.6g    %14.6g]","TOTAL",sum1,sum2,sum3,sum4);
//    dbgpln(" Temperature    %12.2f -> %12.2f K",TempKFeed,C.Temp());
//    dbgpln(" Change in heat %12.2f  ",dHfSumTot);
//    dbgpln("");
//    }
//  #endif

  //AG.m_Gamma.Init(true);

  AG.m_fDataIsValid=true;
  }
#endif
//--------------------------------------------------------------------------

flag CReactionBlock::CpsOK()
  {
  for (int e=0; e<NAll(); e++)
    if (!All[e]->CpsOK())
      return false;
  return true;
  };

//--------------------------------------------------------------------------

flag CReactionBlock::HfsOK()
  {
  for (int e=0; e<NAll(); e++)
    if (!All[e]->HfsOK())
      return false;
  return true;
  };

void CReactionBlock::GetClosureInfo(CClosureInfo & CI)
  {
  CI.m_RCTMassGain    += MsSumSrc()->Mass()-MsSumSnk()->Mass();
  CI.m_RCTHfGain      += HfSumSrc()-HfSumSnk();
  CI.m_RCTHsGain      += HsSumSrc()-HsSumSnk();
  //CI.m_RCTPowerIn     += HsSumHX() + HsSumPwr();
  const double h1 = HsSumHX();
  if (Valid(h1))
    CI.m_RCTPowerIn   += h1;
#ifndef _RELEASE
  else
    {
    LogError(FullObjTag(), 0, "Invalid reaction HsSumHX()"); //error only occurs sometimes after first project load??? Why???
    ASSERT_ALWAYS(true, "Invalid reaction HsSumHX()!!!???", __FILE__, __LINE__);
    }
#endif
  const double h2 = HsSumPwr();
  CI.m_RCTPowerIn     += h2;
  CI.m_RCTHOR_Std0    += m_HfPrStd0-m_HfFdStd0;
  CI.m_RCTHOR_Mdl0    += m_HfPrMdl0-m_HfFdMdl0;
  CI.m_RCTHOR_Used0   += HfSumRctZero();
  CI.m_RCTHOR_Diff0   = CI.m_RCTHOR_Used0 - CI.m_RCTHOR_Mdl0;// HfErrZero();
  }

//--------------------------------------------------------------------------

flag CReactionBlock::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tNegative Product";                return 1;
    case  2: pS="W\tRCT reload required";             return 1;
    case  3: pS="W\tFinal Temperature not converged"; return 1;

    case  8: pS="E\tNot Balanced (see messages)";     return 1;
    case  9: pS="W\tUser defined heat of reaction not specified; or Cp not defined in specie database."; return 1; //pS="W\tBad Cp's";
    case 10: pS="W\tUser defined heat of reaction not specified; or Hf not defined in specie database."; return 1; //pS="W\tBad Hf's";
    case 11: pS="E\tNo Extent";                       return 1;
    case 12: pS="E\tNot Avail";                       return 1;

    case 13: pS="W\tFinal Temperature not converged"; return 1;
    case 14: pS="E\tLow intermediate temperature. (Reaction product temperature likely incorrect)"; return 1;

    default:
      return TaggedObject::CIStrng(No, pS);
    }
  };

//============================================================================
//
//
//
//============================================================================

CList<CReactionBase*, CReactionBase*> CReactionBase::sm_AllRBs;

flag CReactionBase::Open()
  {
  //CBlockEvalBase::Open();
  fEnabled=True;
  if (pRB==NULL)
    {
    pRB=(CReactionBlock*)CReactionBlockClass.Construct(NULL, "RB", m_pNd, TOA_Embedded);
    pRB->m_RctBase=this;
    }
  bOn=1;
  m_pNd->StructureChanged(NULL);
  m_pNd->fHasFiles=1;
  fEnabled=(pRB!=NULL);
  if (fEnabled)
    pRB->Load(pRB->LoadForDynamic(), NULL, false);

  return fEnabled;
  };

//--------------------------------------------------------------------------

void CReactionBase::Close()
  {
  if (pRB==NULL)
    return;

  pRB->Clear(true, true);

  if (pRB->nPermReacts>0)
    {
    //pRB->Clear(True);
    pRB->SetUpReactions();
    m_pNd->StructureChanged(m_pNd);
    m_pNd->fHasFiles=0;
    return;
    }

  m_pNd->StructureChanged(m_pNd);
  m_pNd->fHasFiles=0;

  fEnabled=False;
  };

//--------------------------------------------------------------------------

void CReactionBase::Add_OnOff(DataDefnBlk &DDB, dword Flags)
  {
  DDB.Byte("",     "Reactions",  DC_,    "",     xidRCTEnable,  m_pNd, Flags, GetOnOffValLst());  // TEMPORARY
  }

//--------------------------------------------------------------------------

void CReactionBase::BuildDataDefn(DataDefnBlk &DDB, pchar pTag_, pchar pTagComment, DDBPages PageIs, dword UserInfo)
  {
  DDEF_Flags Old=DDB.GetVisibility();
  DDB.Visibility(NSHM_All, fEnabled);

  if (0 || PrjFileVerNo() < 22)
    {
    if (pRB)//Enabled())
      pRB->BuildDataDefn(DDB);
    }
  else
    {
    //if (pTag_==NULL)
    //  pTag_="RB";
    if (pTag_)
      m_sName = pTag_;

    if (DDB.BeginObject(m_pNd, Name(), "SR_Cfg", pTagComment, PageIs))
      {
      if (pRB)
        {
        DDB.SuppressNextBlock();
        pRB->BuildDataDefn(DDB);
        }
      }
    DDB.EndObject();
    }

  DDB.SetVisibility(Old);
  }

//--------------------------------------------------------------------------

flag CReactionBase::DataXchg(DataChangeBlk & DCB)
  {
  switch (DCB.lHandle)
    {
    case xidRCTEnable:
      if (DCB.rB)
        {
        if (*DCB.rB)
          {
          if (PrjFileVerNo()<115 && *DCB.rB>=BlkEval_On && StateSemanticsOn())//m_bDefinesStateSemantics && m_bAllowStateSemantics)
            *DCB.rB = BlkEval_State + (*DCB.rB-BlkEval_On);
          Open(*DCB.rB);
          }
        else
          Close();
        }
      DCB.B=OpenStatus();// (Enabled());
      return 1;
    }
  return 0;
  }

//--------------------------------------------------------------------------

int CReactionBase::GetRctCount()
  {                       
  if (pRB)
    {
    return pRB->NAll();
    }
  return -1;
  };                            

//--------------------------------------------------------------------------

eScdReactionBasis CReactionBase::GetRctBasis(int iRct)
  {
  if (pRB && iRct>=0 && iRct<pRB->NAll())
    {
    return pRB->All[iRct]->Basis();
    }
  return RctNull;
  };

    
//--------------------------------------------------------------------------

LPCTSTR CReactionBase::GetRctTag(int iRct)
  {
  if (pRB && iRct>=0 && iRct<pRB->NAll())
    {
    return pRB->All[iRct]->Tag();
    }
  return NULL;
  };

//--------------------------------------------------------------------------

LPCTSTR CReactionBase::GetRctComment(int iRct)
  {
  if (pRB && iRct>=0 && iRct<pRB->NAll())
    {
    return pRB->All[iRct]->m_sComment();
    }
  return NULL;
  };

//--------------------------------------------------------------------------

LPCTSTR CReactionBase::GetRctString(int iRct)
  {
  if (pRB && iRct>=0 && iRct<pRB->NAll())
    {
    return pRB->All[iRct]->GetRctString(EAS_All);
    }
  return NULL;
  };

//--------------------------------------------------------------------------

long CReactionBase::GetRctSequence(int iRct)
  {
  if (pRB && iRct>=0 && iRct<pRB->NAll())
    {
    return pRB->All[iRct]->SeqNo;
    }
  return NULL;
  };

//===========================================================================
//
//
//
//===========================================================================

LPCTSTR RctStrippedDouble(double D)
  {
  static CString SA[10];
  static int iS=-1;
  CString &S=SA[(iS+1)%10];
  S.Format("%.6f", D);
  //S.TrimLeft(S, " 0"); 
  S.TrimRight(" 0"); 
  if (S[S.GetLength()-1]=='.')
    S+="0";
  return S;
  }

//===========================================================================
//
//
//
//===========================================================================
