// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#define  __FILTER_CPP
#include "filter.h"

//==========================================================================
/*#D:#T:Filter
#X:Reference : Chemical Engineering Journal Vol ? 19??
Predict performance of belt filter washing.  A.Tomiak.
#X:#n#u<#1<#b<General Description>>>#N#NThis model is used to simulate a belt filter 
with counter current washing.
The main requirements for using this model are : a feed containing solids and liquids, 
a wash water stream, containing no liquid solvents, and data on filtration losses for 
a simple filtration wash.  If the unit does not require washing, the user should rather 
use the Splitter to simulate the process.The user must configure the two output streams 
from the filter to have Qm_Rqd flows larger than any flow expected from the unit.

The model will simulate any number of counter current washing steps, from 1 upwards.  The 
wash water is assumed to be added to the final washing stage, with the filtrate from each 
stage being added as a wash to the previous stage.  The model has only two outputs, the 
final filtrate and the washed cake.
The washing loss vs. wash ratio curve for simple filtration washing is used to determine 
the wash losses of each washing stage. For example, the wash loss for stage one, f1, is 
found from the curve using :
           Wash ratio (N) = Volume of wash water / Volume of solution left in cake
Losses on stage 2 (f2) are found from 2 * N, stage 3 from 3 * N, etc.
#n#n#u<#l<#b<Variables to be supplied by the user>>>#n
#n#b<Stages? :>#NThe user must insert the number of counter current washing stages required.
#n#b<Sol Moist :>#NThis is the percentage of liquids to solids in the final washed cake, 
the default is 10%.
#n#b<Datum Required :>#NThe datum of the filter relative to the common datum of the system.
#n#b<WashLoss curve :>#NThe user is required to supply the data points for the washing losses 
for simple filtration washing.  These can be read in from a spreadsheet file saved as a comma 
delimited file (CSV), or inputted directly into the model.

#n#n#u<#l<#b<Additional information available from the model>>>#n
#n#b<Tag :>#NThe unique name given to the unit to be able to refer to and identify the unit.  If the 
user does not specify a name when configuring the flowsheet, SysCAD will give the unit a name, 
starting with the prefix 'FLT_' followed by a unique number.
#n#b<Parent :>#NThis refers to the project in which the flowsheet is configured.
#n#b<Type :>#NThis defines the type of unit, in this case a Filter. 
#n
#n#h<Other>#n
Default model prefix:FT#n
Short name:Filter#n
Model type:Unit#n
#G:Units
*/

//==========================================================================

ACCESS_SPECIE(H2O, "H2O");

const byte ioidFd = 0;
const byte ioidWS = 1;
const byte ioidFt = 2;
const byte ioidSo = 3;

static IOAreaRec FilterIOAreaList[] =
  {{"Feed",                       "Feed",       ioidFd, LIO_In0 ,   nc_MLnk, 1, 2, dwIOIsBuffer|IOGRP(0)},
   {"Wash solution",              "WashSoln",   ioidWS, LIO_In1 ,   nc_MLnk, 0, 5, dwIOIsBuffer|IOGRP(0)},
   {"Liquid filtrate",            "Filtrate",   ioidFt, LIO_Out0,   nc_MLnk, 1, 1, dwIOIsBuffer|IOGRP(0)},
   {"Solid product",              "Solids",     ioidSo, LIO_Out1,   nc_MLnk, 1, 1, dwIOIsBuffer|IOGRP(0)},
   {NULL}};

long Drw_Filter[] = { DD_Poly, -2,2, 0,2, 0,1, 10,1, 10,0, -2,0, -2,2,
                    DD_Poly,   -2,0, 0,-2, 10,0,
                    DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Filter, "Filter", "1", Drw_Filter, "FT", TOC_ALL|TOC_GRP_SEPAR|TOC_STD_KENWALT,
                    "Filter",
                    "A Filter") 

Filter::Filter(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  //OnFilter("OnFilter", this, TOA_Embedded),
  //ThruFilter("ThruFilter", this, TOA_Embedded),
  QFd("QFd", this, TOA_Embedded),
  QWS("QWS", this, TOA_Embedded),
  QFt("QFt", this, TOA_Embedded),
  QSo("QSo", this, TOA_Embedded),
  WashLossCurve("DataOnly", "WashLoss", this, TOA_Embedded)
  {
  AttachIOAreas(FilterIOAreaList);
  Stage  = 1;
  N      = 1.0;
  Moist  = 0.1;
  }

//--------------------------------------------------------------------------

Filter::~Filter()
  {
  }

//--------------------------------------------------------------------------
/*This provides access to the variables of the model and calls the
BuildDataDefn for objects used by the model.*/
void Filter::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Long    ("No_Stages",      "Stages?",  DC_,     "",       &Stage,         this, isParm);
  DDB.Double  ("Solids_Moisture","SolMoist", DC_Frac, "%",      &Moist,         this, isParm);
  DDB.Text   ("Datum");
  DDB.Double ("Datum",        "Z",    DC_L    , "m",    &dDatum,        this, isResult|0      | DYNMODE);
  DDB.Double ("Datum_Rqd",    "Z_Rqd",DC_L    , "m",    &dDatum_Rqd,    this, isParmStopped | DYNMODE|NAN_OK);
  
  DDB.Object(&WashLossCurve, this, NULL, NULL, DDB_RqdPage);

  DDB.EndStruct();
  };

//--------------------------------------------------------------------------
void Filter::filt(CDMatrix& w, CDVector& d, int j, long max)
    {
    max = max + 1;
    double den = N - d[1];
    w[j][2] = (N * w[j][1] - d[1]) / den;
    for (int i=3; i<max; i++)
      {
      w[j][i] = (N - d[1] + d[2]) * w[j][i-1] - d[i-1];
      for (int k=3; k<i; k++)
        w[j][i] = w[j][i] - (d[k-1] - d[k]) * w[j][i-k+1];
      w[j][i] = w[j][i] / den;
      }
    };
//--------------------------------------------------------------------------
void Filter::washcon(CDMatrix& w, CDMatrix& c, double Cin, int j, long temp)
  {
  c[j][1] = 1.0 - N * (w[j][1] - w[j][2]);
  for (int i = 2; i<temp; i++)
     c[j][i] = c[j][i-1] - N * (w[j][i] - w[j][i+1]);
  for (i = 1; i<temp; i++)
     {
     c[j][i] = c[j][i] * Cin;
     }
  }

//--------------------------------------------------------------------------

void Filter::Conc(CDVector& d, long temp, double CLiq, double FLiq)
 {
 int water = H2O.li(), max = SDB.No();
 double Liq = QFd.QMass(som_Liq);
 CDMatrix w(max,temp+1),
          c(max,temp);
 for (int i=0; i<max; i++)
    {
    if ((SDB[i].IsLiq()) && (i != water))
      {
      double Cin, c0 = 1.0, w0 = 0.0;   
      Cin = QFd.Qm(i) / Liq;
      if (Cin > 1e-3)
        {
        w[i][1] = 0.9;
        filt(w, d, i, temp);
        w[i][1] = ((w[i][1] - w[i][temp])/(c0 - w[i][temp]));
        filt(w, d, i, temp);
        washcon(w, c, Cin, i, temp);
        double wash, loss = c[i][temp-1] * CLiq;
        if (loss < 0.0)
          loss = 0.0;
        wash = QFd.Qm(i) - loss;
        QSo.SetQm(i, loss);
        QFt.SetQm(i, wash);
        }
      }
    }
 };
 
//--------------------------------------------------------------------------
/*This determines what material should leave through each outlet,
and rate of change of the contents resulting from the flow, of material, out of each outlet
of the surge unit.
*/
void Filter::EvalProducts()
  {
  SpConduit & Qs=*IOConduit(IOWithId_Self(ioidSo));
  SpConduit & Qf=*IOConduit(IOWithId_Self(ioidFt));
 
  SpConduit * pw=IOConduit(IOWithId_Self(ioidWS));
  SpConduit * ff=IOConduit(IOWithId_Self(ioidFd));
  double SolIn = ff->QMass(som_Sol);
  double QmWIn = pw->QMass(som_Liq);
  SigmaQInPMin(QFd, som_ALL, Id_2_Mask(ioidFd));
  SpConduit Sd("Sd", this, TOA_Free);
  for (int i = 0; i < NoFlwIOs(); i++)
    {
    if (IO_In(i))
      Sd.QAddF(*IOConduit(i), som_ALL, 1.0);
    }
  double QmSIn = Sd.QMass(som_Sol);
  double QmLIn = Sd.QMass(som_Liq);
  N = QmWIn / (Moist * QmSIn);

  double SoLiq, FtLiq;
  if (QmLIn > 0.0)
    {
    SoLiq = Range(0.0,(QmSIn * Moist), QmLIn);
    FtLiq = QmLIn - SoLiq;
    }
  else
    {
    SoLiq = 0.0;
    FtLiq = 0.0;
    } 
  if (QmLIn <1e-5)
     QmLIn = 1.0;
  QSo.QSetF(Sd, SetMass_Frac, 1.0, SoLiq/QmLIn, 0.0, Std_P);
  QFt.QSetF(Sd, SetMass_Frac, 0.0, FtLiq/QmLIn, 0.0, Std_P);

  if ((Stage > 0.0) && (QmSIn > 1e-3))
    {
    long temp = Stage + 1;
    CDVector  d(temp),
              f(temp);
    f[0] = 1.0;
    for(long i=1 ; i<temp ; i++)
       {
       f[i] = WashLossCurve.Yx(N * i);
       d[i] = f[i-1] - f[i];
       }
    Conc(d, temp, SoLiq, FtLiq);
    }
  };

//--------------------------------------------------------------------------
/*This determines what material is arriving through each inlet,
and rate of change of the contents resulting from the flow, of material, into each inlet
of the surge unit. It also calculates the mass and volume accumulation of the surge unit,
whether the surge unit is to overflow and, if so, the quantity of material that is to
overflow.*/
void Filter::EvalDerivs()
  {
  };

// =========================================================================
