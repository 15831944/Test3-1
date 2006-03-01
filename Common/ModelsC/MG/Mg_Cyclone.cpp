//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define  __MG_CYC_CPP

#include "sc_defs.h"
#include "datacnvs.h"
#include "mg_cyclone.h"
#include "net_err.h"

#include "dbgmngr.h"

#define dbgHCyclone 1

#if dbgHCyclone
static CDbgMngr dbgCyc("Models", "MgCyclone");
#endif

#include "debugnew.h"

//===========================================================================
//
//                      Mossgas Hydrocyclone
//
//==========================================================================
/*#D:#T:Hydrocyclone
#X:#h<General Description>#nThe Hydrocyclone is used to seperate a feed liquid
into two product streams.  The overflow stream contains the hydrocarbon fraction
of the feed.  The underflow contains the MEG and water.  The separation is based
on a user defined separation fraction and the pressure drop across the unit.

#n#h<Variables to be supplied by the user>#n
#n#i<SeparationEff:> The user must specify the separation efficiency of the unit.
This will determine the ideal separation of MEG and Water from the hydrocarbons.

#n#h<Other>#n
Default model prefix:HCY#n
Short name:Hydrocyclone#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

SPECIEBLK_L(H2O,       "H2O"     , false);
SPECIEBLK_L(MEG,       "MEG"     , false);

long Drw_HCyclone[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5,
                     DD_End };

//===========================================================================

const byte idIn    = 0;
const byte idOver  = 1;
const byte idUnder = 2;

static IOAreaRec HCycloneIOAreaList[] =
  {{"Feed",       "Feed"          , idIn,    LIO_In ,  nc_MLnk, 1, 1, 0, 0.0f, ""},
   {"Overflow",   "Overflow"      , idOver,  LIO_Out,  nc_MLnk, 1, 1, 0, 0.0f, ""},
   {"Underflow",  "Underflow"     , idUnder, LIO_Out,  nc_MLnk, 1, 1, 0, 0.0f, ""},
   {NULL}};

IMPLEMENT_MODELUNIT(HCyclone, "HCyclone", "", Drw_HCyclone, "HCY", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "HydroCyclone",
                    "HydroCyclone")

HCyclone::HCyclone(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
   MN_Xfer(pClass_, TagIn, pAttach, eAttach)
  {
  AttachClassInfo(nc_Process, HCycloneIOAreaList);

  SepEff = 1.0;
  };

//-------------------------------------------------------------------------

void HCyclone::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text("");
  DDB.Double ("",          "Datum",      DC_L,      "m",      &dDatum,      this, isParm);
  DDB.Text("");
  DDB.Double ("",              "P",      DC_P,      "kPag",   xidPMean,     this, 0);
  DDB.Double ("",              "T",      DC_T,      "C",      xidTemp,      this, 0);
  DDB.Text("");
  DDB.Text("");
  DDB.Double ("",    "SeparationEff",    DC_Frac,   "%",      &SepEff,      this, isParm);

  DDB.EndStruct();
  };


//---------------------------------------------------------------------------

void HCyclone::ConfigureJoins()
  {
  Init_NJoins(1);
  int i;
  switch (SolveMode())
    {
    case PBMODE:
    case SSMODE:
      for (i=0; (i<NoFlwIOs()); i++)
        SetIO_Join(i, 0, 0);
      break;
    case DYNMODE:
      for (i=0; (i<NoFlwIOs()); i++)
          {
          SetIO_Join(i, 0, 0);
          //if (IOId_Self(i)==idOver)
          //  SetJoinQm_QmRatio (0, i);
          }

      break;
    }
  };

//--------------------------------------------------------------------------

void HCyclone::EvalJoinPressures()
  {
  for (int i = 0; i < NoFlwIOs(); i++)
    Set_IOP_Self(i,Std_P);
  };

//-------------------------------------------------------------------------

flag HCyclone::EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE)
  {
  double Qm=0.0;
  pFlwBlk pC = IOFB(IONo, FE);
  pC->SetQmFree();
  return True;
  };

//-------------------------------------------------------------------------
// the function of this is to set all output conduit compositions given all the estimated flows and
// input conduits.
void HCyclone::EvalProducts()
  {
  if (NJoins()>0)
    switch (SolveMode())
      {
      case PBMODE:
        DoBreak();
        break;
      case SSMODE:
        DoBreak();
        break;
      case DYNMODE:
        {
        int JoinId=0;
        double Press=Joins[JoinId].Pressure();

        int I[MaxIOList+1];
        FillIOIndexList(JoinId, I);

        if (SolveMode()==PBMODE)
          Press=GetPBInputPressure(JoinId);

        StkSpConduit SdLcl("SdLcl", chLINEID(), this);
        SpConduit &Sd = SdLcl();
        Sd.QZero();
        Sd.SetPress(Press);
        double  Qot=0.0;
        int NFeeds=0;
        for (int i, ic = 0; (i=I[ic])>=0; ic++)
          if (IO_In(i))
            {
            Sd.SelectModel(IOConduit(i), NFeeds==0);
            Sd.QAddF(*IOConduit(i), som_ALL, 1.0);
            NFeeds++;
            }
          else
            Qot += IOQmEst_Out(i);

          const int cI = IOWithId_Self(idIn );
          const int cO = IOWithId_Self(idOver);
          const int cB = IOWithId_Self(idUnder);

          if (cI >= 0 && cO >= 0 && cB >= 0 && !IO_Out(cI) && !IO_In(cO) && !IO_In(cB))
            {
            pSpConduit pI = IOConduit(cI);
            pSpConduit pO = IOConduit(cO);
            pSpConduit pB = IOConduit(cB);

            SepEff = Range(0.1, SepEff, 0.99);

            double TempIn = pI->Temp();
            pO->SetTemp(TempIn);
            pB->SetTemp(TempIn);

            double TotalOver=0.0, TotalUnder=0.0, Scale1, Scale2;
            double t1 = IOQmEst_Out(cO);
            double t2 = IOQmEst_Out(cB);

            if ((Cd.GetLen() != SDB.No()) || (Wt.GetLen() != SDB.No()))
              {
              Cd.SetSize(SDB.No());
              Wt.SetSize(SDB.No());
              }
            for (int i = 0; i < SDB.No() ; i++ )
              {
              if (i == H2O.li() || i == MEG.li())
                {
                Cd[i] = pI->Qm(i) * (1.0 - SepEff);
                Wt[i] = pI->Qm(i) - Cd[i];
                }
              else
                {
                Cd[i] = pI->Qm(i) * SepEff;
                Wt[i] = pI->Qm(i) - Cd[i];
                }
              TotalOver  += Cd[i];
              TotalUnder += Wt[i];
              }
            /* Add code to adjust the species
            based on the actual flows from the unit.*/

            if (TotalOver > 0.0)
              Scale1 = t1/TotalOver;
            else
              Scale1 = 0.0;

            if (TotalUnder > 0.0)
              Scale2 = t2/TotalUnder;
            else
              Scale2 = 0.0;

            for (i = 0; i < SDB.No() ; i++ )
              {
              if (Scale1 > Scale2)
                {
                double m1 = Wt[i] * Scale2;
                pB->SetQm(i, m1);
                double m2 = pI->Qm(i) - m1;
                pO->SetQm(i, m2);
                }
              else
                {
                double n1 = Cd[i] * Scale1;
                pO->SetQm(i, n1);
                double n2 = pI->Qm(i) - n1;
                pB->SetQm(i, n2);
                }
              }


            }
          else
            MN_Xfer::EvalProducts();
        }
        break;
      }

  };

//=========================================================================
