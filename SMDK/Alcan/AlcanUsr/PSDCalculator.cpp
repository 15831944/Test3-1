//===========================================================================
//=== SysCAD SMDK - Hyprod PSD Definition Model 2006 : Alcan, Kenwalt     ===
//===========================================================================

#include "stdafx.h"
#define  __PSDCALCULATOR_CPP
#include "PSDPersonality.h"
#include "psdcalculator.h"
//#include "math.h"
#include "HyprodCommon.h"

#if ForceOptimizeOff
#pragma optimize("", off)
#endif

//====================================================================================

const int idFeed = 0;
const int idOut  = 1;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;      Id;      Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",    idFeed,  1,    10,   0,    1.0f,  MIO_In |MIO_Material|MIO_Transfer},
    { "Out",            "Out",     idOut,   1,    1,    0,    1.0f,  MIO_Out|MIO_Material|MIO_Transfer},
    { NULL },
  };

static double Drw_PSDCalculator[] = {
    MDrw_Poly, -10, -10, 10,-10, 10,10, -10,10, -10, -10, 
    -10, -9, -8, -9,
    -8,  -7, -6, -7, 
    -6,  -6, -4, -6,
    -4,  -2, -2, -2,
    -2,   3,  0,  3,
     0,   5,  2,  5,
     2,   6,  4,  6,
     4,   4,  6, 4,
     6,  -3 ,  8, -3,
     8,  -8,  10, -8,
    MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(PSDCalculator, "PSDCalculator", DLL_GroupName)
void PSDCalculator_UnitDef::GetOptions()
  {
  SetDefaultTag("SC");
  SetDrawing("SizeSeparation", Drw_PSDCalculator);
  SetTreeDescription("Size Separation:Alcan PSD Calculator");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_PSD);
  SetModelLicense(MLicense_PSD);
  };

//---------------------------------------------------------------------------

PSDCalculator::PSDCalculator(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
  {
  personality = new PSDPersonality();
  bSetThePSD = false;
  }

//---------------------------------------------------------------------------

PSDCalculator::~PSDCalculator()
  {
  delete personality;
  }

//---------------------------------------------------------------------------

void PSDCalculator::Init()
  {
  SetIODefinition(s_IODefs);
  }

//---------------------------------------------------------------------------

void PSDCalculator::BuildDataFields()
  {
  DD.CheckBox("SetPSD", "", &bSetThePSD, MF_PARAMETER);
  DD.Text("");
  personality->BuildDataDefn(DD);
  }

//---------------------------------------------------------------------------

bool PSDCalculator::ExchangeDataFields()
  {
  switch (DX.Handle)
    {
    case idDX_Set: 
      if (DX.HasReqdValue)
        {
        SetPersonality();
        }
      DX.Bool = false;
      return true;
    }
  return false;
  }

//---------------------------------------------------------------------------

void PSDCalculator::SetPersonality()
  {
  try
    {
    MStreamI QI;
    FlwIOs.AddMixtureIn_Id(QI, idFeed);

    // Get References to our Output Streams using the FlwIOs helper class and IO ids
    MStream & QO = FlwIOs[FlwIOs.First[idOut]].Stream;

    // Always initialise the outputs as a copy of the inputs.
    // This ensures all "qualities" are copied and initialised (eg PSD, SpecieModel, etc)
    QO = QI;

    if (bSetThePSD)
      {
      // Determine if the input stream has size information present
      // NB: This will be successful if any of the species have PSD data
      // associated with them.
      MIPSD & PSD = *QI.FindIF<MIPSD>();
      MIPSD & PSDO = *QO.FindIF<MIPSD>();

      if ( IsNothing(PSD)==false )
        {
        personality->Init(&PSD);
        personality->SetDistribution();
        }
      }
    }
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
  }

//---------------------------------------------------------------------------

void PSDCalculator::EvalProducts()
  {
  try
    {
    MStreamI QI;
    FlwIOs.AddMixtureIn_Id(QI, idFeed);

    // Get References to our Output Streams using the FlwIOs helper class and IO ids
    MStream & QO = FlwIOs[FlwIOs.First[idOut]].Stream;

    // Always initialise the outputs as a copy of the inputs.
    // This ensures all "qualities" are copied and initialised (eg PSD, SpecieModel, etc)
    QO = QI;

    MIPSD & PSDO = *QO.FindIF<MIPSD>();

    personality->ReplaceDistribution(&PSDO);

    }

  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
    Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
  }

//====================================================================================
