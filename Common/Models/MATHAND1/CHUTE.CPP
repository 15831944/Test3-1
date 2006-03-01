//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define  __CHUTE_CPP

#include "chute.h"

//==========================================================================
/*#D:#T:Chute                       
#X:#h<General Description>#nThe Chute is used to as a simple seperation 
unit to split solids and liquids in a user defined ration, or to remove certain species 
from the feed stream.
The user must supply a feed stream to the Chute, and overflow and underflow streams 
from the unit.  A wash stream to the unit is optional.  The unit will add the wash stream 
to the feed stream and then split the combined stream according to the user defined method.
The user must configure the two output streams from the Chute to have Qm_Rqd flows 
larger than any flow expected from the unit.
#n#n
#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
Feed         Input          1              20#n
Product      Output         1              5#n
#a#n
#h<Variables to be supplied by the user>#n
#i<Method> : The user is required to define the split method for the unit.  Two choices 
exist : 1. Specie - in which case the unit will remove a user defined fraction of the 
chosen specie(s) to the overflow stream. 2. Sol/Liq - the unit separates the feed solids 
and liquids in a set proportion.#n
#i<OSSol> : This is only required if the 'Sol/Liq' split method is chosen.  The user 
is required to input the percentage of the feed solid which reports to the overflow stream.#n
#i<OSLiq> : This is also only required if the 'Sol/Liq' split method is chosen.  The user 
is required to input the percentage of the feed liquids which report to the overflow stream.#n
#i<Specie> : This is required if the 'Specie' split method is chosen.  The user chooses 
which specie will be split out from the feed stream.#n
#i<Split> : This is required if the 'Specie' split method is chosen.  The user inputs the 
percentage of the specie(s) which are sent to the overflow stream.#n
#i<Z_Rqd> : The relative elevation of the Chute with respect to the common datum 
of the system.#n
#h<Associated variables>#n
#i<OSDens> : The density of the overflow stream.#n
#i<USDens> : The density of the underflow stream.#n
#n
#n#h<Other>#n
Default model prefix:CH#n
Short name:Chute#n
Model type:Unit#n
#G:Units
*/

//==========================================================================
const byte idFeed = 0;
const byte idProd = 1;

static IOAreaRec ChuteIOAreaList[] =
  {{"Feed ",         "Feed",       idFeed, LIO_In0 ,   nc_MLnk, 1, 20, IOSetXfer|IOGRP(0)},
   {"Product",       "Product",    idProd, LIO_Out0,   nc_MLnk, 1, 5,  IOSetXfer|IOGRP(0)},
   {NULL}};

static double Drw_Chute[] = { DD_Poly, -1,5, 1,5, 3,-5, -3,-5, -1,5,
                     DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Chute, "Chute", "1", Drw_Chute, "Piping", "CH", TOC_ALL|TOC_GRP_TRANSFER|TOC_STD_KENWALT,
                    "Material Transfer:Chute",
                    "General Purpose Chute") 

Chute::Chute(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  QFd("QFd", this, TOA_Embedded),
  m_Pwr("Supply", this, TOA_Embedded)
  {
  AttachIOAreas(ChuteIOAreaList);
  };


// -------------------------------------------------------------------------

Chute::~Chute()
  {
  for (int i=0; i<VPB.GetSize();i++)
    if (VPB[i]!=NULL)
      delete VPB[i];
  }

// -------------------------------------------------------------------------
/*This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void Chute::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);
  DDB.Text    (" ");
 	Strng Tag;
  int j=0;
  for (int i=0; i<NoFlwIOs(); i++)
    if ((IOId_Self(i)==idProd) && (j<VPB.GetSize()))
      {
     	//Tag.Set("Pos_%i", j);
     	VPB[j]->BuildDataDefn(DDB, this, VPBTag[j].Str(), j, Nd_Rmt(i)->FullObjTag(), j);
	    j++;
      }

  j=0;
  for (i=0; i<NoFlwIOs(); i++)
    if (IOId_Self(i)==idProd)
      {
     	Tag.Set("Splt_%i", j);
     	DDB.Double(Tag(),     "",   DC_Frac ,     "%",    &Split[j],       this, 0);
	    DDB.TagComment(Nd_Rmt(i)->FullObjTag());
      j++;
      }

  DDB.EndStruct();
  };

//--------------------------------------------------------------------------

flag Chute::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;

  int j=0;
  for (int i=0; i<NoFlwIOs(); i++)
    if ((IOId_Self(i)==idProd) && (j<VPB.GetSize()))
      {
     	if (VPB[j]->DataXchg(DCB, j))
        return 1;
	    j++;
      }
  return 0;
  }

//--------------------------------------------------------------------------

flag Chute::ValidateData(ValidateDataBlk & VDB)
  {
  flag OK=MdlNode::ValidateData(VDB);
  return OK;
  }

//---------------------------------------------------------------------------

void Chute::ConfigureJoins()
  {
  for (int i=0; i<NoFlwIOs(); i++)
    SetIO_Open(i, 0, false, ESS_Denied);
  };

//---------------------------------------------------------------------------

void Chute::PostConnect(int IONo)
  {
  MdlNode::PostConnect(IONo);
  if (IOId_Self(IONo)==idProd)
    {
    int n=0;
    for (int i=0; i<NoFlwIOs(); i++)
      if (IOId_Self(i)==idProd)
        n++;
    
    Split.SetSize(n);

    n--;
    VPB.SetSize(Max(0, n));
    VPBTag.SetSize(Max(0, n));
    for (i=0; i<VPB.GetSize();i++)
      if (VPB[i]==NULL)
        {
        VPBTag[i].Set("Pos_%i", i);
        VPB[i] = new CIO_VlvPosBlk("VL_Basic", this, &m_Pwr);
        }
    StructureChanged(this);
    }
  };

//---------------------------------------------------------------------------

void Chute::PreDisConnect(int IONo)
  {
  if (IOId_Self(IONo)==idProd)
    {
    int n=0, j=0;
    for (int i=0; i<NoFlwIOs(); i++)
      if (IOId_Self(i)==idProd)
        {
        n++;
        if (i<IONo)
          j++;
        }
    for (  ; j<n-1; j++)
      Split[j]=Split[j+1];
    Split.SetSize(n);

    n--;
    for (i=Max(0,n); i<VPB.GetSize();i++)
      delete VPB[i];

    VPB.SetSize(Max(0, n));
    VPBTag.SetSize(Max(0, n));

    StructureChanged(this);
    }

  };

//--------------------------------------------------------------------------

void Chute::EvalJoinPressures(long JoinMask)
  {
  for (int i = 0; i < NoFlwIOs(); i++)
    Set_IOP_Self(i,AtmosPress(IODatum_Term(i)));
  };
 
//--------------------------------------------------------------------------
/*This determines what material should leave through each outlet,
and rate of change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.
*/
void Chute::EvalProducts(long JoinMask)
  {

  SigmaQInPMin(QFd, som_ALL, Id_2_Mask(idFeed));

  double T=QFd.Temp();
  
  double FracRemain=1.0;
  for (int i=0, j=0; i<NoFlwIOs(); i++)
    if (IOId_Self(i)==idProd)
      {
      if (j<VPB.GetSize())
        Split[j]=Range(0.0, VPB[j]->ActualPosition(this), FracRemain);
      else
        Split[j]=FracRemain;
      IOConduit(i)->QSetF(QFd, som_ALL, Split[j], AtmosPress(IODatum_Term(i)));//*QFd.QMass());
      FracRemain=Max(0.0, FracRemain-Split[j]);
      j++;
      }
  
  }

//---------------------------------------------------------------------------

void Chute::EvalCtrlActions()
  {
  for (int i=0; i<VPB.GetSize(); i++)
    VPB[i]->EvalCtrlActions(this);
  }

//---------------------------------------------------------------------------

dword Chute::ModelStatus()
  {
  dword Status=MdlNode::ModelStatus();
  if (NoFlwIOs())
    {
    int HasFlw=0;        
    for (int i=0; i<NoFlwIOs(); i++)
      {
      if (IOConduit(i)->QMass()>UsableMass)
        HasFlw=1;
      }
    Status |= (HasFlw ? FNS_UFlw : FNS_UNoFlw);
    //Status |= ((PSB.On() && PSB.Speed(this)>0.0) ? FNS_On : FNS_Off);
    }
  return Status;
  }

//==========================================================================
