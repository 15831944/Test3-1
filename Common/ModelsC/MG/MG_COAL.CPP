//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define  __MG_COALES_CPP

#include "sc_defs.h"
#include "datacnvs.h"
#include "mg_coal.h"
#include "net_err.h"

#include "dbgmngr.h"

#define dbgCoalescer 1

#if dbgCoalescer
static CDbgMngr dbgCoals("Models", "Coalescer");
#endif

#include "debugnew.h"

SPECIEBLK_L(H2O,       "H2O"     , false);

long Drw_Coalescer[] = { DD_Poly, -5,5, -5,-5, 5,-5, 5,5, -5,5,
                     DD_End };

//===========================================================================

const byte idIn    = 0;
const byte idOut   = 1;
const byte idBoot  = 2;

static IOAreaRec CoalescerIOAreaList[] =
  {{"Feed",       "Feed"          , idIn,   LIO_In ,  nc_MLnk, 1, 1, 0, 0.0f, ""},
   {"Outlet",     "Outlet"        , idOut,  LIO_Out,  nc_MLnk, 1, 1, 0, 0.0f, ""},
   {"BootOutlet", "BootOutlet"    , idBoot, LIO_Out,  nc_MLnk, 1, 1, 0, 0.0f, ""},
   {NULL}};

IMPLEMENT_MODELUNIT(Coalescer, "Coalscr", "", Drw_Coalescer, "CL", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "Coalescer",
                    "Coalescer")

Coalescer::Coalescer(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
   MN_Xfer(pClass_, TagIn, pAttach, eAttach),
   BootC("Contents",     this, TOA_Embedded),
   QBootIn("BootInFlow", this, TOA_Embedded)
  {
  AttachClassInfo(nc_Process, CoalescerIOAreaList);
  BootC.Enable(IE_Integrate);
  BootC.SetVolume(2.0);
  BootV      = 2.0;
  BootLvl    = 0.0;
  Split      = 0.0;
  CondW      = 0.01;
  WaterC     = 0.05;
  DoneWarnVap = false;
  DoneWarnSol = false;
  };

//-------------------------------------------------------------------------

void Coalescer::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text("");
  DDB.Double ("",          "Datum",      DC_L,      "m",      &dDatum,           this  ,isParm);
  DDB.Text("");
  DDB.Double ("",              "P",      DC_P,      "kPag",   xidPMean,     this, 0);
  DDB.Double ("",              "T",      DC_T,      "C",      xidTemp,      this, 0);
  DDB.Text("");
  DDB.Text("");
  DDB.Double ("",     "BootVolume",      DC_Vol,    "m^3",    &BootV,       this, isParm);
  DDB.Double ("",        "BootLvl",      DC_Frac,   "%",      &BootLvl,     this, 0);
  DDB.Double ("",    "CondinWater",      DC_Frac,   "%",      &CondW,       this, isParm);
  DDB.Double ("",    "WatertoCond",      DC_Frac,   "%",      &WaterC,      this, isParm);

  DDB.Object(&BootC, this, NULL, NULL, DDB_RqdPage);

  DDB.Object(&QBootIn,    this, NULL, NULL, DDB_RqdPage);

  DDB.EndStruct();
  };

//-------------------------------------------------------------------------

flag Coalescer::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Xfer::DataXchg(DCB))
    return 1;

  switch (DCB.lHandle)
    {
    case xidTemp:         DCB.D = BootC.Temp();             return 1;
    }
  return 0;
  }
//-------------------------------------------------------------------------

//flag Coalescer::EvalFlowEquations(CSpPropInfo *pProps, int iJoinId, int FE)
//  {
//  return False;
//  }

//-------------------------------------------------------------------------

flag Coalescer::EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE)
  {
  double Qm=0.0;
  pFlwBlk pC = IOFB(IONo, FE);
  pC->SetQmFree();
  return True;
  };

//-------------------------------------------------------------------------
// the function of this is to set all output conduit compositions given all the estimated flows and
// input conduits.
void Coalescer::EvalProducts()
  {
  const int cI = IOWithId_Self(idIn );
  const int cO = IOWithId_Self(idOut);
  const int cB = IOWithId_Self(idBoot);

  BootC.SetVolume(BootV);
  BootC.ZeroDeriv();

  if (cI >= 0 && cO >= 0 && cB >= 0 && !IO_Out(cI) && !IO_In(cO) && !IO_In(cB))
    {
    pSpConduit pI = IOConduit(cI);
    pSpConduit pO = IOConduit(cO);
    pSpConduit pB = IOConduit(cB);

    CondW  = Range(0.0, CondW, 0.98);
    WaterC = Range(0.0, WaterC, 0.98);

    double TempIn = pI->Temp();
    pO->SetTemp(TempIn);
    QBootIn.SetTemp(TempIn);

    if ((Cd.GetLen() != SDB.No()) || (Wt.GetLen() != SDB.No()))
      {
      Cd.SetSize(SDB.No());
      Wt.SetSize(SDB.No());
      }
    for (int i = 0; i < SDB.No() ; i++ )
      {
      if (i == H2O.li())
        {
        Cd[i] = pI->Qm(i) * WaterC;
        Wt[i] = pI->Qm(i) - Cd[i];
        pO->SetQm(i, Cd[i]);
        QBootIn.SetQm(i, Wt[i]);
        }
      else
        {
        Wt[i] = pI->Qm(i) * CondW;
        Cd[i] = pI->Qm(i) - Wt[i];
        pO->SetQm(i, Cd[i]);
        QBootIn.SetQm(i, Wt[i]);
        }
      }
    // Make the boot outlet flange flow from BootC
    pB->QSetM(BootC, som_ALL, IOQmEst_Out(cB), IOP_Self(cB));
    BootLvl = BootC.Level();

    }
  else
    {
    MN_Xfer::EvalProducts();
    }
  }

//-------------------------------------------------------------------------
// called after eval products.
void Coalescer::EvalDerivs()
  {
  BootC.ZeroDeriv();
  BootC.AddDeriv(QBootIn, 1.0);
  BootC.AddDeriv(*IOConduit(idBoot), -1.0);

    for (int i=0; i<NoFlwIOs(); i++)
      {
      if ( IO_In(i) && ( IOConduit(i)->MassFrac(som_Vap) > 0.001)  )
        {
        if (!DoneWarnVap)
          LogWarning("Coalescer", 0," Input contains Gas");
        DoneWarnVap=true;
        }
      else if ( IO_In(i) && ( IOConduit(i)->MassFrac(som_Sol) > 0.001)  )
        {
        if (!DoneWarnSol)
          LogWarning("Coalescer", 0," Input contains Solids");
        DoneWarnSol=true;
        }
      else
        {
        //FirstWarn=1;
        }
      }
  }

// --------------------------------------------------------------------------

void Coalescer::ApplyDerivs(double dTime, flag DoDbg)
  {
  BootC.ApplyDerivs(dTime, true);
  }

//=========================================================================
