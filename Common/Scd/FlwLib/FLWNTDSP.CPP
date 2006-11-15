//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define  __FLWNETS_CPP
#include "sp_cont.h"
#include ".\sparseslv\sparseslv.h"
#include "flwnets.h"

#include "errorlog.h"
#include "dbgmngr.h"

//===========================================================================

void CSubNetBlk::ShowSubNetsL(CGrpFlwBlk * g, int dbgShowConns)
  {
  dbgp("%s%s ", g->m_iFlwPh_Act&FPh_Gasses ? "V":" ", g->m_iFlwPh_Act&FPh_Liquid? "L":" ");
  dbgp("%02i ", g->m_pSubNet->m_SubNetNo);
  dbgp(" %s", JoinTypeStr(g->m_LnkInx.Typ));

  int Fxd=g->m_Src.m_pFBJn->GetFixed();
  dbgp(" %s[%s]%s", (Fxd?"{":" "), g->m_Src.m_pNd->FullObjTag(),(Fxd?"}":" "));
  JoinIter L(g->m_Lnks);
  for (CJoinRec * p = L.First(); p; p = L.Next())
    {
    switch(p->Typ)
      {
      case JTyp_Link:
      case JTyp_Edge:
      case JTyp_FxdLnk:
      case JTyp_Lone:
        {
        dbgp("->");
        Fxd=0;
        for (int c=0; c<p->NConns(); c++)
          if (p->IOFB(c,0)->GetFixed())
            Fxd=1;
        dbgp("%s%s%s(%i)%s",(Fxd?"{":" "), SgnStr(p->Dirn), p->Nd()->FullObjTag(),p->iJoinArrayInx,(Fxd?"}":" "));
        dbgp("%02i ", p->m_pSubNet->m_SubNetNo);
        }
        break;
      default:
        dbgpln(" %s%s(%i)", SgnStr(p->Dirn), p->Nd()->FullObjTag(),p->iJoinArrayInx);
      }
    }
  Fxd=g->m_Dst.m_pFBJn->GetFixed();
  dbgp("->%s[%s]%s", (Fxd?"{":" "), g->m_Dst.m_pNd->FullObjTag(),(Fxd?"}":" "));
  dbgpln("");
  }

//===========================================================================

void CSubNetBlk::ShowSubNetsT(CJoinRec * p, int dbgShowConns)
  {
  dbgp("%s%s ", p->VFlowAllowed(m_bFullFlow) ? "V":" ", p->LFlowAllowed(m_bFullFlow) ? "L":" ");
  dbgp("%02i ", p->m_pSubNet->m_SubNetNo);
  dbgp(" %s", JoinTypeStr(p->Typ));
  dbgp(" %s",SgnStr(p->Dirn));
  dbgp(" %s%s%s%s %s",
    (p->fLonely ? "L" : p->fUnDefined ? "U" : " "),
    (p->fIsolatedNet ? (p->fIsoNetRef ? "I" : " ") : p->fIsolatedNode ? "i" : " "),
    (p->fIsFixedXfer ? "X" : (p->fInFixedNet ? "F" : " ")),
    (p->fIsFixedExtern ? "x" : " "), (p->fHasXBstHere? "B" : " "), (p->fUDFShut ? "S" : " "));
  switch(p->Typ)
    {
    case JTyp_Tie:
    case JTyp_ImpNd:
    case JTyp_AccLnk:
      dbgp(" P ");
      if (p->nProdFracs>=2)
        {
        dbgp(" pf[%i]",p->nProdFracs);
        for (int pf=0; pf<p->nProdFracs; pf++)
          dbgp(" [%s:%g]",p->Nd_Rmt(p->ProdFracs[pf].iConnNo)->FullObjTag(),p->ProdFracs[pf].dProdFrac);
        }
      break;
    default :
      dbgp(" ?              ");
      break;
    }
  dbgpln(" %s%s(%i)", SgnStr(p->Dirn), p->Nd()->FullObjTag(),p->iJoinArrayInx);
  }

//===========================================================================

void CSubNetBlk::ShowSubNetsJ(CJoinRec * p, int dbgShowConns)
  {
  dbgp("%s%s ", p->VFlowAllowed(m_bFullFlow) ? "V":" ", p->LFlowAllowed(m_bFullFlow) ? "L":" ");
  dbgp("%02i ", p->m_pSubNet->m_SubNetNo);
  dbgp(" %s", JoinTypeStr(p->Typ));
//  dbgp("%s", p->fIsImpNd ? "I":" ");
//  dbgp("%s", p->fIsAccLnk? "A":" ");
  dbgp(" %s",SgnStr(p->Dirn));
  dbgp(" %s%s%s%s %s",
    (p->fLonely ? "L" : p->fUnDefined ? "U" : " "),
    (p->fIsolatedNet ? (p->fIsoNetRef ? "I" : " ") : p->fIsolatedNode ? "i" : " "),
    (p->fIsFixedXfer ? "X" : (p->fInFixedNet ? "F" : " ")),
    (p->fIsFixedExtern ? "x" : " "), (p->fHasXBstHere? "B" : " "), (p->fUDFShut ? "S" : " "));
  switch(p->Typ)
    {
    case JTyp_Tie:
    case JTyp_ImpNd:
    case JTyp_AccLnk:
      dbgp(" P ");
      if (p->nProdFracs>=2)
        {
        dbgp(" pf[%i]",p->nProdFracs);
        for (int pf=0; pf<p->nProdFracs; pf++)
          dbgp(" [%s:%g]",p->Nd_Rmt(p->ProdFracs[pf].iConnNo)->FullObjTag(),p->ProdFracs[pf].dProdFrac);
        }
      break;
    default :
      dbgp(" ?              ");
      break;
    }
  dbgpln(" %s%s(%i)", SgnStr(p->Dirn), p->Nd()->FullObjTag(),p->iJoinArrayInx);
  }

//-------------------------------------------------------------------------

void CSubNetBlk::ShowSubNet(int dbgShowConns)
  {
  dbgpln("--------------------------------");
  //dbgp("%i.%i - %s - ",pNet->NetNo, SubNetNo, SolnString(SolutionType()));
  dbgp("%i.%i - ",pNet->NetNo, m_SubNetNo);
  if (fLonely) dbgp("Lonely ");
  if (fUnDefined) dbgp("Undefined ");
  if (fIsolatedNet) dbgp("IsolatedNet ");
  if (fIsolatedNode) dbgp("IsolatedNode ");
  if (fFixed) dbgp("Fixed ");
  if (fNetWrk) dbgp("NetWrk ");
  dbgpln("");

  GrpFlwIter G(m_Grps);
  for (CGrpFlwBlk * g=G.First(); g; g=G.Next())
    ShowSubNetsL(g, dbgShowConns);

  dbgpln("...");
  JoinIter T(Ties);
  for (CJoinRec * p = T.First(); (p); p = T.Next())
    ShowSubNetsT(p, dbgShowConns);

//  dbgpln("...");
//  JoinIter J(Joins);
//  for (CJoinRec * j = J.First(); (j); j = J.Next())
//    ShowSubNetsJ(j, dbgShowConns);

  if (dbgShowConns) 
    {
    }
  }

//-------------------------------------------------------------------------

void CNetBlk::ShowSubNets(int N, int dbgShowConns)
  {
  int N1=(N>=0 ? N : 0);
  int N2=(N>=0 ? N+1 : NSubNets);

  for (int Sub = N1; Sub < N2; Sub++)
    SubNet[Sub]->ShowSubNet(dbgShowConns);

  Strng CS;
  int dun=0;
  JoinIter J(Joins);
  for (CJoinRec * p = J.First(); (p); p = J.Next())
    if (!p->m_pSubNet)
      {
      if (!dun)
        {
        dbgpln("--------------------------------");
        dbgpln("SubN PQIx - UnAssigned");              
        if (p->m_pSubNet)
          dbgpln("Sub ??? Unknown");
        }
      dbgpln("%4i %s %s(%i)", p->m_pSubNet ? p->m_pSubNet->m_SubNetNo : -1, p->Nd()->TypeString(CS), p->Nd()->FullObjTag(),p->iJoinArrayInx);
      dun=1;
      }
  }

//===========================================================================

void CFullNetBlk::DumpSolveStats(FILE *f)
  {
  /*
  int         Net, Sub, dif;
  pFlwNode      p, pWorstS, pWorstE;
  fprintf(f, "Solve Stats\n");
  for (int i=0;i<2;i++)
    {
    pWorstS=NULL;
    dif=0;
    for (Net = 0; (Net < NNets); Net++)
      for (Sub = 0; (Sub < NetInfo[Net].NSubNets); Sub++)
        {
        p = NetInfo[Net].SubNet[Sub].Start;
        while (p)
          {
          if (p->IsLnk)
            {
            pFlwNode pStart = p;
            while (!p->SubNet.GrpEnd) p = p->SubNxt();
            int d=pStart->pGFb->nTraverse-pStart->pGFb->nTraverseLast;
            if (d>dif)
              {
              dif=d;
              pWorstS=pStart;
              pWorstE=p;
              }
            }
          p = p->SubNxt();
          }
        }
    if (pWorstS)
      {
      fprintf(f, "%8li  %8li  %s -> %s\n", pWorstS->pGFb->nTraverse-pWorstS->pGFb->nTraverseLast,pWorstS->pGFb->nTraverse,pWorstS->Tag(), pWorstE->Tag());
      pWorstS->pGFb->nTraverseLast=pWorstS->pGFb->nTraverse;
      }
    }

  fprintf(f, "\n");

  for (Net = 0; (Net < NNets); Net++)
    for (Sub = 0; (Sub < NetInfo[Net].NSubNets); Sub++)
      {
      p = NetInfo[Net].SubNet[Sub].Start;
      while (p)
        {
        if (p->IsLnk)
          {
          pFlwNode pStart = p;
          while (!p->SubNet.GrpEnd) p = p->SubNxt();
          pStart->pGFb->nTraverseLast=pStart->pGFb->nTraverse;;
          }
        p = p->SubNxt();
        }
      }
  */
  }

//===========================================================================

void CFullNetBlk::DumpFlows(FILE *f, double Time_, double dTime_, pchar Desc_)
  {
  /*
  int         Net, Sub;
  pFlwNode      p;
  flag DoDebugFlowsHdr=(strcmpi(Desc_, "strt")==0);

  if (DoDebugFlowsHdr)
    {
    fprintf(f, "qq%-4.4s:PTags,    Time,  dTime",Desc_);
    CFlwNodeIter Nds(NetNds);
    for (p = Nds.First(); (p); p = Nds.Next())
    //for (p = NetNds; (p); p = p->AllNetNxt())
      if (p->IsSurge && p->Debug)
        fprintf(f, ",%13.13s", p->Tag());
    fprintf(f, "\n");

    fprintf(f, "qq%-4.4s:FTags,    Time,  dTime",Desc_);
    for (Net = 0; (Net < NNets); Net++)
      for (Sub = 0; (Sub < NetInfo[Net].NSubNets); Sub++)
        {
        p = NetInfo[Net].SubNet[Sub].Start;
        while (p)
          {
          if (p->IsLnk)
            {
            pFlwNode pStart = p;
            if (pStart->Debug)
              fprintf(f, ",%13.13s", pStart->Tag());
            while (!p->SubNet.GrpEnd) p = p->SubNxt();
            }
          p = p->SubNxt();
          }
        }
    fprintf(f, "\n");
    }

  fprintf(f, "qq%-4.4s:Pres ,%8.2f,%7.3f",Desc_, Time_, dTime_);
  CFlwNodeIter Nds(NetNds);
  for (p = Nds.First(); (p); p = Nds.Next())
    if (p->IsSurge && p->Debug)
      fprintf(f, ",%13.5f", p->Pm);
  fprintf(f, "\n");

  fprintf(f, "qq%-4.4s:Flws ,%8.2f,%7.3f",Desc_, Time_, dTime_);
  for (Net = 0; (Net < NNets); Net++)
    for (Sub = 0; (Sub < NetInfo[Net].NSubNets); Sub++)
      {
      p = NetInfo[Net].SubNet[Sub].Start;
      while (p)
        {
        if (p->IsLnk)
          {
          pFlwNode pStart = p;
          if (pStart->Debug)
            fprintf(f, ",%13.7f", pStart->Qm);
          while (!p->SubNet.GrpEnd) p = p->SubNxt();
          }
        p = p->SubNxt();
        }
      }
  fprintf(f, "\n");

  fprintf(f, "qq%-4.4s:dP   ,%8.2f,%7.3f",Desc_, Time_, dTime_);
  for (Net = 0; (Net < NNets); Net++)
    for (Sub = 0; (Sub < NetInfo[Net].NSubNets); Sub++)
      {
      p = NetInfo[Net].SubNet[Sub].Start;
      while (p)
        {
        if (p->IsLnk)
          {
          pFlwNode pStart = p;
          while (!p->SubNet.GrpEnd) p = p->SubNxt();
          if (pStart->Debug)
            {
            int io = (pStart->SubNet.Dirn > 0 ? 0 : 1);
            double P = pStart->IOP_Rmt(io);
            io = (p->SubNet.Dirn > 0 ? 1 : 0);
            P -= p->IOP_Rmt(io);
            fprintf(f, ",%#13.7g", P);
            }
          }
        p = p->SubNxt();
        }
      }
  fprintf(f, "\n");

  fprintf(f, "qq%-4.4s:SrcP ,%8.2f,%7.3f",Desc_, Time_, dTime_);
  for (Net = 0; (Net < NNets); Net++)
    for (Sub = 0; (Sub < NetInfo[Net].NSubNets); Sub++)
      {
      p = NetInfo[Net].SubNet[Sub].Start;
      while (p)
        {
        if (p->IsLnk)
          {
          pFlwNode pStart = p;
          while (!p->SubNet.GrpEnd) p = p->SubNxt();
          if (pStart->Debug)
            {
            int io = (pStart->SubNet.Dirn > 0 ? 0 : 1);
            double P = pStart->IOP_Rmt(io);
            fprintf(f, ",%13.5f", P);
            }
          }
        p = p->SubNxt();
        }
      }
  fprintf(f, "\n");

  fprintf(f, "qq%-4.4s:DstP ,%8.2f,%7.3f",Desc_, Time_, dTime_);
  for (Net = 0; (Net < NNets); Net++)
    for (Sub = 0; (Sub < NetInfo[Net].NSubNets); Sub++)
      {
      p = NetInfo[Net].SubNet[Sub].Start;
      while (p)
        {
        if (p->IsLnk)
          {
          pFlwNode pStart = p;
          while (!p->SubNet.GrpEnd) p = p->SubNxt();
          if (pStart->Debug)
            {
            int io = (p->SubNet.Dirn > 0 ? 1 : 0);
            double P = p->IOP_Rmt(io);
            fprintf(f, ",%13.5f", P);
            }
          }
        p = p->SubNxt();
        }
      }
  fprintf(f, "\n");
  */
  }

//===========================================================================

void CFullNetBlk::ShowFlowTree1(int lvl, pFlwNode pSrc, pFlwNode p)
  {
  /*
  int j;
  int N = p->NoFlwIOs();

  p->Busy = 1;

  if (lvl < 0)
    dbgpln("   Press     Drop    Boost        Flow");
  lvl = Max(lvl, 0);

  for (j = 0; j < N; j++)
    {
    pFlwNode pR = p->Nd_Rmt(j);
    if (pR != pSrc)
      {
      pFlwNode    DstNd = NULL;
      pFlange   DstFlange = NULL, SrcFlange;
      double Sgn;
      if (N == 2)
        {
        int Sio = (j + 1) % 2;
        SrcFlange = p->IOFlange(Sio);
        DstFlange = p->IOFlange(j);
        DstNd = p->Nd_Rmt(j);
        Sgn = (j == 1 ? 1.0 : -1.0);

        if (fabs(SrcFlange->dPq) > 1e-6 ||
            fabs(SrcFlange->dPb+SrcFlange->dPbX) > 1e-6 ||
            fabs(SrcFlange->dPz) > 1e-6)
          {
          dbgp("%#8.2f %#8.2f %#8.2f %#13.7f",
            p->IOP_Rmt(Sio), - Sgn * SrcFlange->dPq, - Sgn * (SrcFlange->dPb+SrcFlange->dPbX+SrcFlange->dPz), Sgn * SrcFlange->Qm);
          dbgpln(" %-5.5s | %3i %s", "===", lvl + 1, p->FullObjTag());
          }

        dbgp("%#8.2f %#8.2f %#8.2f %#13.7f",
          p->IOP_Self(Sio), - Sgn * p->dPq, - Sgn * (p->dPb+p->dPbX+p->dPz), Sgn * p->Qm);
        dbgpln(" %-5.5s | %3i %s", p->ShortDesc(), lvl + 1, p->FullObjTag());

        if (!(!pR->IsSurge && ! pR->Busy))
          {
          int Dio = j;
          if (fabs(DstFlange->dPq) > 1e-6 ||
              fabs(DstFlange->dPb+DstFlange->dPbX) > 1e-6 ||
              fabs(DstFlange->dPz) > 1e-6)
            {
            dbgp("%#8.2f %#8.2f %#8.2f %#13.7f",
              p->IOP_Self(Dio), - Sgn * DstFlange->dPq, - Sgn * (DstFlange->dPb+DstFlange->dPbX+DstFlange->dPz), Sgn * DstFlange->Qm);
            dbgpln(" %-5.5s | %3i %s", "===", lvl + 1, p->FullObjTag());
            }
          }

        }
      else
        {
        dbgp("%#8.2f                              ", p->Pm);
        dbgpln(" %-5.5s | %3i %s", p->ShortDesc(), lvl + 1, p->FullObjTag());
        }

      if (!pR->IsSurge && ! pR->Busy)
        {
        int l = lvl;
        if (p->IsTie || pR->IsTie) l++;
        ShowFlowTree1(l, p, pR);
        }
      else if (DstNd)
        {
        dbgpln("%#8.2f                               %5.5s |     %s",
          DstFlange->Pm, DstNd->ShortDesc(), DstNd->FullObjTag());
        //dbgpln("---------------------------------------------------");
        dbgpln("-- Press --- Drop -- Boost ------ Flow ------------");
        }
      }
    }
  */
  }

//-------------------------------------------------------------------------

void CFullNetBlk::ShowFlowTree(pFlwNode Root, pchar StartTag)
  {
  /**
  pFlwNode p;
  char doterm = 0;
  int Net, Sub;

  for (Net = 0; Net < NNets; Net++)
    {
    CFlwNodeIter NNds(NetInfo[Net].Nds);
    for (p = NNds.First(); (p); p = NNds.Next())
      p->Busy = 0;
    }
  pFlwNode pStart = (pFlwNode)Root->FindObj(StartTag);

  if (pStart)
    ShowFlowTree1(-1, NULL, pStart);
  else
    for (Net = 0; Net < NNets; Net++)
      {
      pStart = NULL;
      double BigPres = 0.0;
      for (Sub = 0; Sub < NetInfo[Net].NSubNets; Sub++)
        {
        for (p = NetInfo[Net].SubNet[Sub].Start; (p); p = p->SubNxt())
          if (p->Typ == NTyp_Edge)
            {
            double P = 0.0;
            for (int i = 0; i < 2; i++)
              if (p->Nd_Rmt(i)->IsSurge)
                P = Max(p->Nd_Rmt(i)->Pm, P);
            if (P > BigPres)
              {
              pStart = p;
              BigPres = P;
              }
            }
        if (pStart)
          {
          dbgpln("----------------------- FlowTree %3i %3i -----------------------", Net, Sub);
          ShowFlowTree1(-1, NULL, pStart);
          doterm = 1;
          }
        }
      }
  if (doterm)
    dbgpln("--------------------------------------------------------------------------");
  */
  };

//===========================================================================

static int CntTagTree1(Strng_List &OldList,
                        int Cnt,
                        pFlwNode p, flag ThruProcess)
  {
  int RetCnt = Cnt;
  /*

  if (!p->Busy && !p->Done)
    {
    p->Busy = 1;

    if (OldList.Find(p->FullObjTag()))
      Cnt++;

    int j, N = p->NoFlwIOs();
    for (j = 0; j < N; j++)
      {
      pFlwNode pR = p->Nd_Rmt(j);
      if (!pR->IsSurge||ThruProcess)
        RetCnt = Max(RetCnt, CntTagTree1(OldList, Cnt, pR, ThruProcess));
      }
    p->Busy = 0;
    }
  */
  return RetCnt;
  }

//-------------------------------------------------------------------------

static pchar PadStr = "-------------------------------------------------------------";

static void SortTagTree1(Strng_List &OldList, Strng_List &NewList, Strng_List &GrfList,
                         int &NdIdNo, int SrcNdIdNo, int &DstNdIdNo, int lvl,
                         pFlwNode p, flag ThruProcess)
  {
  /*
  const int BigDstNdIdNo = 32767;
  DstNdIdNo = BigDstNdIdNo;
  if (!p->Busy && !p->Done)
    {
    p->Busy = 1;
    p->Done = 1;
    int MyNdIdNo=-1,SrcNdIdNoNxt;

    pStrng gS = NULL, pS = OldList.Find(p->FullObjTag());
    if (pS)
      {
      OldList.Remove(pS);
      NewList.Append(pS);
      gS = new Strng("?");
      GrfList.Append(gS);
      MyNdIdNo = NdIdNo++;
      SrcNdIdNoNxt = MyNdIdNo;
      DstNdIdNo = MyNdIdNo;
      }
    else
      SrcNdIdNoNxt = SrcNdIdNo;

    int N = p->NoFlwIOs(), NOut=0;
    lvl = Max(lvl, 0);

    pFlwNode DoNd;
    do
      {
      int MinCnt = 1024;
      DoNd = NULL;
      for (int k = 0; k < N; k++)
        {
        pFlwNode pR = p->Nd_Rmt(k);
        if (!pR->IsSurge||ThruProcess)
          {
          int Cnt = CntTagTree1(OldList, 0, pR, ThruProcess);
          if ((Cnt > 0) && (Cnt < MinCnt))
            {
            DoNd = pR;
            MinCnt = Cnt;
            }
          }
        }

      if (DoNd)
        {
        NOut++;
        int l = lvl, MyDstNdIdNo;
        if (p->IsTie || DoNd->IsTie) l++;
        SortTagTree1(OldList, NewList, GrfList, NdIdNo, SrcNdIdNoNxt, MyDstNdIdNo, l, DoNd, ThruProcess);
        DstNdIdNo = Min(DstNdIdNo, MyDstNdIdNo);
        if (MyDstNdIdNo < BigDstNdIdNo && MyDstNdIdNo > MyNdIdNo + 1)
          {
          int l=0,n;
          pStrng p;
          for (n = 1, p = gS->Next() ; n < (MyDstNdIdNo-MyNdIdNo); p = p->Next(), n++)
            l = Max(l, p->Length());

          int PadLen = l+1-gS->Length();
          gS->Append("%*.*s+", PadLen, PadLen,PadStr);
          for (n = 1, p = gS->Next() ; n < (MyDstNdIdNo-MyNdIdNo); p = p->Next(), n++)
            p->Append("%*s|", l+1-p->Length(),"");
          PadLen = l+1-p->Length();
          p->Append("%*.*s+", PadLen, PadLen, PadStr);

          }
        }
      }
    while (DoNd);
    if (gS)
      {
      char c;
      switch(NOut)
        {
        case 0  :
          c = '-';
          break;
        case 1  :
          if (SrcNdIdNo==0)
            c = '-';
          else if (MyNdIdNo == (SrcNdIdNo+1))
            c = '|';
          else
            c = '+';
          break;
        default :
          c = '*';
          break;
        }
      gS->Str()[0] = c;
      dbgpln("%c %4i %4i %4i %3i %s", c, MyNdIdNo, SrcNdIdNo, DstNdIdNo, NOut, p->FullObjTag());
      //if (c=='*' && MyNdIdNo > SrcNdIdNo+1)
      //  {
      //  int l=0,n;
      //  pStrng p;
      //  for (n = 1, p = gS->Prv ; n < (MyNdIdNo-SrcNdIdNo); p = p->Prv, n++)
      //    l = Max(l, p->Length());
      //
      //  gS->Append("%*s*", l+1-gS->Length(),"");
      //  for (n = 1, p = gS->Prv ; n < (MyNdIdNo-SrcNdIdNo); p = p->Prv, n++)
      //    p->Append("%*s|", l+1-p->Length(),"");
      //  p->Append("%*s*", l+1-p->Length(),"");
      //
      //  }
      }
    p->Busy = 0;
    }
  */
  }

//-------------------------------------------------------------------------

void CFullNetBlk::SortTagTree(Strng_List &OldList, Strng_List &NewList, Strng_List &GrfList, pFlwNode pStart)
  {
  /*
  pFlwNode p;
  int Net, Sub;

  for (Net = 0; Net < NNets; Net++)
    {
    double BigPres = 0.0;
    for (Sub = 0; Sub < NetInfo[Net].NSubNets; Sub++)
      {
      for (p = NetInfo[Net].SubNet[Sub].Start; (p); p = p->SubNxt())
        if ( p->Typ == NTyp_Edge)
          {
          double P = 0.0;
          for (int i = 0; i < 2; i++)
            if (p->Nd_Rmt(i)->IsSurge)
              P = Max(p->Nd_Rmt(i)->Pm, P);
          if (P > BigPres)
            {
            pStart = p;
            BigPres = P;
            }
          }
      if (pStart)
        {
        int NdIdNo = 0, DstNdIdNo;
        SortTagTree1(OldList, NewList, GrfList, NdIdNo, NdIdNo, DstNdIdNo, -1, pStart, False);
        }
      }
    }
  */
  };

//-------------------------------------------------------------------------

void CFullNetBlk::SortTagList(pFlwNode Root, Strng_List &TagList, Strng_List &GrfList, int Ord, pchar StartTag)
  {
  /*
  if ((TagList.Length() == 0) || (Ord == 0))
    return;

  pFlwNode p = NULL;
  while (Root->GetObj_All(p))
    p->Busy = p->Done = 0;

  pFlwNode pStart = (pFlwNode)Root->FindObj(StartTag);
  Strng_List NewList;
  if (pStart)
    {
    int NdIdNo = 0, DstNdIdNo;
    SortTagTree1(TagList, NewList, GrfList, NdIdNo, NdIdNo, DstNdIdNo, -1, pStart, True);
    }
  else
    {
    // SortTagTree(TagList, NewList, GrfList);
    }

  p = NULL;
  while (Root->GetObj_All(p))
    p->Done = 0;

  TagList.Clear();
  TagList.Append(&NewList);

  int l = 0;
  pStrng s = GrfList.First();
  while (s)
    {
    l = Max(l, s->Length());
    s = s->Next();
    }

  s = GrfList.First();
  while (s)
    {
    int PadLen = l-s->Length();
    s->Append("%*s", PadLen, "");
    strrev(s->Str());
    s = s->Next();
    }
  */
  }

//===========================================================================
