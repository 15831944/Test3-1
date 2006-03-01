//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define  __comp_CPP

#include "mg_comp.h"
#include "dbgmngr.h"

#define dbgCompressor  01

#if dbgCompressor
static CDbgMngr dbgComp("Models", "Compressor");
#endif

#define Bar2kPa(v) ((v)*100.0)
#define Bar2kPag(v) ((v)*100.0 - Std_p)
#define kPa2Bar(v) ((v)/100.0)

//#include "optoff.h"

//#define m3perhr2kgpers(v) ((v)/3600.0)
//#define kgpers2m3perhr(v) ((v)*3600.0)
#define m3perhr2kgpers(v) ((v)/((3600.0/0.9593) * 35.32 * 24.0/1e6))
#define kgpers2m3perhr(v) ((v) * (3600.0/0.9593) * 35.32 * 24.0/1e6)
// This converts from kg/s to MMSCFD and visa versa hss 9/3/98

//==========================================================================
/*#D:#T:Compressor
#X:#n#h<General Description>#N#NThe compressor model effectively
operates as a pressure regulator by setting the output pressure to a user
specified value for any flow rate requiring power comsumption less than
the maximum power specified. At any flow rate exceeding this the output
pressure of the unit will be regulated downward to a value which will
ensure the maximum power consumption is not exceeded.

#pThe power curve used assumes the relationship

 #p                        (n-1) =  (k-1)
 #n                        -----    -----
 #n                          n       k.Mu
#n
#n
 #n                   where

  #p                     Mu - is polytropic efficiency
  #n                     k    - is specific heat ratio                                                      n        n
  #n                     n    - is the exponent in the polytropic cycle relationship p1V1  = p2V2

  #p                                                                               (n-1)/n
  #n                  Power = w . Hp . R . T1 . Z . n . (r        - 1)
  #n                                ----------------
  #n                              33000.Mu.M  (n-1)

  #p                  where

 #n                      w   -  is mass flow
 #n                      Hp -  is polytropic head
 #n                      R   -  is the gas constant
 #n                      T1 -  gas inlet temperature
 #n                      Z   -  the average gas compressability factor
 #n                      r    -  compression ratio Pout/Pin
 #n                      M  -  gas molecular weight


 #p  (Refer to Gas Production Operations by H. Dale Beggs ( p145)
 for further information )

#n#n#h<Inputs and Outputs>#w
Label    Input/Output  Minimum number  Maximum number#n
In          Input          1              1#n
Out         Input          1              1#n
BlowOff     Output         0              1#n
#a

#n#n#h<Variables to be supplied by the user>
#n#i<PowerMaximum :>The maximum power value to be applied in limiting
the output pressure at high demand. Mechanical losses are excluded. The
default is 4000 kW
#n#i<Psetpoint :>The output pressure required from the compressor, in
kiloPascals (guage).  The default is 8500 kPag.
#n#i<PolytropicEfficiency :>The polytropic efficiency required for the
compressor, expressed  as a percentage. The default is 77 %
#n#i<Status :>This determines the operational status of the compressor
i.e. either on or off.  The default is On
#p#b<PQSurge :>This is the curve of inlet pressure in bar versus surge
limit in MMSCFD for the compressor. The only function of this curve is
to permit the lookup to the Qs value discussed below
#p#b<Spd-PQ :>This curve represents the manufacturer-specified surface
defining speed as a function of inlet pressure versus normalised volume
flow. This surface is used as a lookup to permit output of the current
operating point for the compressor.  Flow is in MMSCFD, Pressure in bar
and speed in rpm.
#p#b<Eff-PQ :>This curve represents the manufacturer-specified surface
defining efficiency as a function of inlet pressure versus normalised
volume flow. This surface is used as a lookup to permit output of the
current operating point for the compressor.  Flow is in MMSCFD, Pressure in bar
and speed in rpm.

#n#n#h<Variables calculated by the model>
#n#i<SpecificHeatRat :>The specific heat ratio for the gas.
#n#i<MolecularWeight :>The molecular weight of the gas.
#n#i<SpeedRatio :>The shaft speed, expressed as a percentage of the maximum
shaft speed.
#n#i<EfficiencyRatio :>The compressor efficiency, expressed as a percentage.
#n#i<Pin :>The current inlet pressure.
#n#i<Pout :>The current outlet pressure.
#n#i<Qin :>The inlet mass flow.
#n#i<Qout :>The outlet mass flow.
#n#i<Tin :>The current inlet temperature.
#n#i<Tout :>The current outlet temperature.
#n#i<Power :>The current power usage excluding mechanical losses.
#n#i<Qs :>The current surge limit of the compressor.

#n#n#h<Other>#n
Default model prefix:CP#n
Short name:Comp#n
Model type:Unit#n
#G:Units
*/
//==========================================================================
const int ioid_In=0;
const int ioid_Out=1;
const int ioid_Blow=2;

/*#G:This lists the areas of the model where links can be attached.*/
static IOAreaRec X_CompIOAreaList[] =
  {{"Input      ",       "In"  , ioid_In,   LIO_In,    nc_MLnk, 1, 1},
   {"Output     ",      "Out"  , ioid_Out,  LIO_Out,   nc_MLnk, 1, 1},
   {"Blowoff    ",  "Blowoff"  , ioid_Blow, LIO_Out,   nc_MLnk, 0, 1},
   {NULL}};


/*#G:This provides the default drawing of the X_Comp.*/
long Drw_X_Comp[] = { DD_Poly, -4, 6, 4, 2, 4, -2, -4, -6, -4, 6,
                      DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(X_Comp, "Comp", "", Drw_X_Comp, "CP", TOC_ALL|TOC_GRP_ENERGY|TOC_USER,
                    "Compressor",
                    "Compressor for Mossgas Project")

X_Comp::X_Comp(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment
eAttach) :
  MN_Xfer(pClass_, TagIn, pAttach, eAttach),
  SurgeCondition("DataOnly", "PQsurge", this, TOA_Embedded),
  Efficiency("DataOnly", "Eff-PQ", this, TOA_Embedded),
  Speed("DataOnly", "Spd-PQ", this, TOA_Embedded)
  {
  AttachIOAreas(X_CompIOAreaList);

  SurgeCondition.SetLength(2);
  SurgeCondition.SetPt(0,0.0,0.0);
  SurgeCondition.SetPt(1,1.0,1000.0);

  P_MaxPower          = 4000.0;    //Default number for Mossgas
  P_PolytropicEff     = 0.77;      //Default number for Mossgas
  P_SetpointPressure  = 8500.0;    //Set as a gauge pressure
  P_SpecificHeatRatio = 1.3;       //for now until this comes from the flashing
  P_MolecularWeight   = 20.96;     // ditto
  P_Compressability   = 0.96;      // ditto
  P_Status            = true;

  SpeedRatio          = 0.0;       // Speed % for a given QV point
  EfficiencyRatio     = 0.0;       // Eff % for a given QV point
  Pin                 = 101.0;     // Pressure on inlet flange
  Pout                = 101.0;     // Pressure on outlet flange
  QinFlange           = 0.0;       // MassFlow on inlet flange
  QoutFlange          = 0.0;       // MassFlow on outlet flange
  Tin                 = 293.0;     // Temperature on inlet flange
  Tout                = 293.0;     // Temperature on outlet flange

  Control=0;
  MinSpeed=10000/60.0;
  MaxSpeed=16000/60.0;
  SpdSpt=0;
  SpdFbk=0;
  SpdTau=10.0;
  ActSpeed=0;
  TheBoost=0;

  X_SetpointPressure  = 0.0;
  Qprv = 0.0;
  CurrentQOut = 0.0;
  CurrentPin  = 0.0;
  CurrentPout = 0.0;
  CurrentPow  = 0.0;

  Rdelta = 0.0;
  Boost  = 0.0;
  Resist = 0.0;
  Iter   = 0;
  MFlow  = 100.0;
  };

// -------------------------------------------------------------------------
/*#F:This provides access to the variables of the model.*/
void X_Comp::BuildDataDefn(DataDefnBlk & DDB)
  {
  DDB.BeginStruct(this);

  DDB.Text("");
  DDB.Double ("","PowerMaximum",         DC_Pwr,    "kW", &P_MaxPower,          this  ,isParm);
  DDB.Double ("","Psetpoint",            DC_P,    "kPag", &P_SetpointPressure,  this  ,isParm);
  DDB.Double ("","Polytropic_Eff",       DC_Frac,    "%", &P_PolytropicEff,     this  ,isParm);
  DDB.Double ("","Compressability",      DC_,         "", &P_Compressability,   this  ,isParm);
  DDB.Text("");

  DDB.Byte   ("","Status",               DC_,         "", &P_Status,            this  ,isParm, DDBOnOff);

  DDB.Text("");
  static DDBValueLst DDB0[] = {
    {0,    "Pressure"},
    {1,    "Speed"},
    {0}};
  DDB.Byte   ("","Control",              DC_,     "",     &Control,             this  ,isParm|SetOnChange|AffectsStruct, DDB0);
  dword IsPress=Control==0;
  dword IsSpeed=Control==1;
  DDB.Double ("","MinSpeed",             DC_pS,   "rpm",  &MinSpeed,            this  ,(IsSpeed?0:noView)|isParm);
  DDB.Double ("","MaxSpeed",             DC_pS,   "rpm",  &MaxSpeed,            this  ,(IsSpeed?0:noView)|isParm);
  DDB.Double ("","SpdTC",                DC_Time, "s",    &SpdTau,              this  ,(IsSpeed?0:noView)|isParm);
  DDB.Double ("","SpdSpt",               DC_Frac, "%",    &SpdSpt,              this  ,(IsSpeed?0:noView)|isParm);
  DDB.Double ("","SpdFbk",               DC_Frac, "%",    &SpdFbk,              this  ,(IsSpeed?0:noView));
  DDB.Double ("","Speed",                DC_pS,   "rpm",  &ActSpeed,            this  ,(IsSpeed?0:noView));
  DDB.Double ("","NetBoost",             DC_DP,   "kPa",  &TheBoost,            this  ,(IsSpeed?0:noView)|InitHidden);

  DDB.Double ("","SpeedRatio",           DC_Frac, "%",    &SpeedRatio,          this  ,(IsPress?0:noView)|isParm|NAN_OK); // Why? - CNM
  DDB.Text("");

  DDB.Text("");
  DDB.Double ("","SpecificHheaRat",      DC_Frac, "%",    &P_SpecificHeatRatio, this  ,0);
  DDB.Double ("","MolecularWeight",      DC_,     "",     &P_MolecularWeight,   this  ,0);
  DDB.Double ("","MMSCFD",               DC_,     "",     &MFlow,               this  ,0);
  DDB.Double ("","Pin",                  DC_P,    "kPag", &Pin,                 this  ,0);
  DDB.Double ("","Pout",                 DC_P,    "kPag", &Pout,                this  ,0);
  DDB.Double ("","Qin",                  DC_Qm,   "kg/s", &QinFlange,           this  ,0);
  DDB.Double ("","Qout",                 DC_Qm,   "kg/s", &QoutFlange,          this  ,0);
  DDB.Double ("","QvSrg",                DC_Qv,   "m^3/s",&QvSrg,               this  ,0);
  DDB.Double ("","QvIn",                 DC_Qv,   "m^3/s",&QvIn,                this  ,0);
  DDB.Double ("","Tin",                  DC_T,    "C",    &Tin,                 this  ,0);
  DDB.Double ("","Tout",                 DC_T,    "C",    &Tout,                this  ,0);
  DDB.Double ("","PowerUsage",           DC_Pwr,  "kW",   &CurrentPow,          this  ,0);
  DDB.Text("");

  DDB.Object(&SurgeCondition, this, NULL, "TestComment 1",DDB_RqdPage);
  DDB.Object(&Efficiency,     this, NULL, "TestComment 2",DDB_RqdPage);
  DDB.Object(&Speed,          this, NULL, "TestComment 3",DDB_RqdPage);

  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag X_Comp::DataXchg(DataChangeBlk & DCB)
  {
  if (MN_Xfer::DataXchg(DCB))
    return 1;
  return 0;
  }

// -------------------------------------------------------------------------

void X_Comp::PostConnect(int IONo)
  {
  MN_Xfer::PostConnect(IONo);
  }

// -------------------------------------------------------------------------

void X_Comp::PreDisConnect(int IONo)
  {
  MN_Xfer::PreDisConnect(IONo);
  }

static double TheBoostDmp=0.4;
void X_Comp::EvalDiscrete()
  {
  if (Control==CTSpeed)
    {
    SpConduit &C=*IOConduit(IOWithId_Self(ioid_In));
    //double Cflow = GEZ(IOFB(IOWithId_Self(ioid_In), 0)->GetQm());
    //double VFlow = GEZ(IOFB(IOWithId_Self(ioid_In), 0)->GetQv());
    double Qv=C.QVolume();
    double Qm=C.QMass();

    MaxSpeed=Max(100.0, MaxSpeed);
    MinSpeed=Range(0.0, MinSpeed, MaxSpeed);
    double RqdS=(P_Status ? Range(MinSpeed/MaxSpeed, SpdSpt,1.0) : 0);
    SpdFbk+=(RqdS-SpdFbk)*ICGetTimeInc()/Max(1.0, SpdTau);
    ActSpeed=SpdFbk*MaxSpeed;
    SpeedRatio=SpdFbk;

    double Hd=-166.466+1.581213*ActSpeed+(-17.5093-0.15871*ActSpeed)*Qv+(0.0003296+0.014441*ActSpeed)*Qv*Qv;
//    double SrgVolFlw=-0.96597+0.010344*ActSpeed;
    double SrgSpd=Range(166.0, ActSpeed, 275.0);
    QvSrg=7E-05*SrgSpd*SrgSpd - 0.0199*SrgSpd + 2.2831;
    QvIn=C.QVolume();

    //Qs=Range(0.0, Qm*SrgVolFlw/GTZ(Qm), 150.0);

    Pin  = Max(100.0,IOP_Rmt (IOWithId_Self(ioid_In)));
//    double PCalc=(7990.392-21.47525359*SpdFbk*266.67+26.75703018*Range(0.0,Cflow,100.0));
//    PCalc=Max(1000.0, PCalc);
//    double PRatio=7990/PCalc;

    //PressIn=GTZ(PressIn);
//    if ((Qv>1.0e-6) && (C.QMass()>1.0e-6))
//      { // Only Change if Direction is Forward
    double GammaIn=1.262;//Range(1.2, C.CpCv(), 1.5);
    double MoleWtFIn=C.MoleWt();//*Rho;
    double TempIn=C.Temp();
//      }


    double R=8.314/MoleWtFIn;
    double X=GammaIn/(GammaIn-1.0);
    // PHT_Isentropic:

    // Units of Hd Expected to be kNm/kg == kJ/kg;
    // See Perry P6-17 & WMC-KNS Acid Plant Curves.
    double dEfficiency=0.9;
    double P2=Pin*Pow(GEZ(Hd*dEfficiency/GTZ(X*R*TempIn)+1.0), X);
    double Bst=GEZ(P2-Pin);

    double TheBoostDmp=1.0-(1.0/Max(ICGetTimeInc(), SpdTau));
    TheBoost=TheBoostDmp*TheBoost+(1-TheBoostDmp)*GEZ(Bst);
    //TheBoost=TheBoostDmp*TheBoost+(1-TheBoostDmp)*GEZ(Pin*PRatio-Pin);

    }
  else
    {
    double Q     = IOQm_In(IOWithId_Self(ioid_In));
    double P1    = IOP_Rmt(IOWithId_Self(ioid_In));
    double P2    = IOP_Self(IOWithId_Self(ioid_In));
    double pwr   = Pwr_Curve(Q,P1,P2);
    double goodp = P2_Curve(Q,P1,P_MaxPower);
    double Qc    = Flw_Curve(P_MaxPower,P1,P_SetpointPressure);

    if( fabs(Q - Qprv)/Max(1.0e-60,Qprv) < 0.001 && ((P2 < P1) || (Q < 0.001)) )
      {
      X_SetpointPressure = 0.0;
      }
    else if( Q > 1.0e-50 && fabs(Q - Qprv)/Max(1.0e-60,Qprv) < 0.001 && P1 < P2 )
      {
      if( Q > Qc )
        {
        double newval = (P_SetpointPressure - goodp);
        double test   = (X_SetpointPressure*0.5) + (newval*0.5);
        if( P2-test < P1 )
          {// This will fail at unrealistically high flow
          X_SetpointPressure = (X_SetpointPressure*0.5) + 0.5*(P2-P1);
          }
        else
          {
          X_SetpointPressure = test;
          }
        //TRACE("EVAL DISCREET CONVERGED %f %f %f %f\n\n",Q,pwr,goodp,X_SetpointPressure);
        }
      else
        {
        X_SetpointPressure /= 2.0;
        }
      }
    Qprv = Q;
    }
  }

// -------------------------------------------------------------------------

void X_Comp::EvalDerivs()
  {
  MN_Xfer::EvalDerivs();
  }

// -------------------------------------------------------------------------

double X_Comp::Afac()
  {
  pSpConduit pI = IOConduit(IOWithId_Self(ioid_In));
  P_SpecificHeatRatio = Max(1.2, pI->CpCv());
  P_MolecularWeight   = Max(10.0, pI->MoleWt());

  // Given (n-1)/n = (k-1)/(k*mu);
  double nm1on = Max(1.0E-10,(P_SpecificHeatRatio-1.0)/(P_PolytropicEff*P_SpecificHeatRatio));
  // Use pg 145 of Gas Production Operations by Beggs
  // Will work in book units and convert result
  double Tabsin = Tin*1.8;                     // inlet temperature
  double R      = 1592.0;                      // gas constant
  double M      = P_MolecularWeight * 2.203;   // Molecular weight comeback should be from specie database
  double Zbar   = P_Compressability;           // Compressability comeback should be from specie database
  double Mply   = (0.745 * R * Tabsin * Zbar ) /
                      (33000.0 * P_PolytropicEff * M *nm1on * P_PolytropicEff);
  return Max(1.0e-20,Mply);
  }

double X_Comp::Bfac()
  {
  double nm1on = (P_SpecificHeatRatio-1.0)/(P_PolytropicEff*P_SpecificHeatRatio);
  return nm1on;
  }

double X_Comp::Pwr_Curve(double Q,double P1,double P2)
  {
  P1 = Max(1.0, P1);
  P2 = Max(1.0, P2);
  double Prat = P2/P1;
  return Afac()*(Q*60.0/0.454)*(pow(Prat,Bfac())-1);
  }

double X_Comp::Flw_Curve(double Pwr,double P1,double P2)
  {
  // Afac() contains a conversion from HP to kilowatts
  P1 = Max(1.0, P1);
  P2 = Max(1.0, P2);
  double Prat = P2/P1;
  Pin  = P1;
  Pout = P2;  // hss 18/8/96 ???
  double x = (Pwr/(Afac() * (pow(Prat,Bfac())-1)));
  return x*0.454/60.0;
  }

double X_Comp::P2_Curve(double Q,double P1,double Pwr)
  {
   return P1 * pow(((Pwr/(Afac()*(GTZ(Q)*60.0/0.454)))+1),1.0/Bfac());
  }

// -------------------------------------------------------------------------

flag X_Comp::EvalFlowEquations(CSpPropInfo *pProps, int IONo, int FE)
  {
  Pin  = Max(100.0,IOP_Rmt (IOWithId_Self(ioid_In)));
  Pout = Max(101.0,IOP_Self(IOWithId_Self(ioid_In)));

  FlwBlk &FBI = *IOFB(IOWithId_Self(ioid_In), FE);
  FlwBlk &FB  = *IOFB(IONo, FE);
  double Cflow = fabs(FBI.GetQm());

//  double PRatio=0;
//  if (SetSpeed)
//    {
////    double PCalc=(7990.392-21.47525359*SpeedRatio*266.67+26.75703018*Range(0.0,Cflow,100.0));
////    PCalc=Max(1000.0, PCalc);
////    PRatio=7990/PCalc;
//    }
//  else
//    {
//    //EfficiencyRatio = Efficiency.Zxy(kgpers2m3perhr(pI->GetQm()),kPa2Bar(Pin))/100.0; // hss 10/3/98
//
    SpeedRatio = Speed.Zxy(Cflow,kPa2Bar(Pin))/60.0;// hss 10/3/98 /100.0;
//    }
  MFlow      = kgpers2m3perhr(Cflow);
  CurrentPow = Pwr_Curve(Cflow,Pin,Pout);  // hss 20/6/96 ???
  //double xx= Speed.Yxz(Cflow,12000.0/60.0)/60.0; // CNM

  if( ! P_Status )
    {
    FB.SetQmReqd(0.0);
    FB.SetDPb(0.0, 0.0);
    FB.SetDPq(0.0, 0.0);
    }
  else
    {
    double QIn=FB.QmMeas();
    switch (IOId_Self(IONo))
      {
      // Set dP and dPdQ on this flange according Qin. The boost is determined from the
      // curve formed by the setpoint,maxpower and surge value
      case ioid_In:
        FB.SetQmFree();
        QinFlange = GTZ(QIn);
        if (Control==CTSpeed)
          {
          FB.SetDPb(GEZ(TheBoost), 0.0);
          }
        else
          {
          TheBoost=P_SetpointPressure - X_SetpointPressure - Pin;
          FB.SetDPb(TheBoost, 0.0);
          }
//        if (SetSpeed)
//          {
//          FB.SetDPb(GEZ(TheBoost), 0.0);
//          }
//        else
//          {
//          TheBoost=P_SetpointPressure - X_SetpointPressure - Pin;
//          FB.SetDPb(TheBoost, 0.0);
//          }
        FB.SetDPq(0.0, 0.0);
        #if dbgCompressor
        if (dbgComp())
          dbgpln("Comp In   QIn:%14.6g Pin:%14.6g Pout:%14.6g PSet:%14.6g XSet:%14.6g %s",
                  QIn,Pin,Pout,P_SetpointPressure, X_SetpointPressure, FullObjTag());
        #endif
        break;
      case ioid_Blow:
        {
        //double SurgeMassFlow = SurgeCondition.Xy(kPa2Bar(Pin));
        //double RqdBlowQmOut  = Max(1.0e-5, SurgeMassFlow - QinFlange );
        //Qs = SurgeMassFlow;
        FB.SetQmReqd(0.0);
        FB.SetDPb(0.0, 0.0);
        FB.SetDPq(0.0, 0.0);
        //#if dbgCompressor
        //if (dbgComp())
        //  dbgpln("Comp Blow Srg:%14.6g Rqd:%14.6g      %14s      %14s      %14s %s",
        //          SurgeMassFlow,RqdBlowQmOut,"","","",FullObjTag());
        //#endif
        }
        break;
      case ioid_Out:
        QoutFlange = LTZ(-QIn);
        CurrentQOut = ((0.5*CurrentQOut) + (0.5*(-QIn))); // 1st order filter
        FB.SetQmFree();
        FB.SetDPb(0.0, 0.0);
        FB.SetDPq(0.0, 0.0);
        break;
      }
    }

  return true;
  }

// --------------------------------------------------------------------------

void X_Comp::EvalProductsInit(EvalProductsInitTasks Task)
  {
  EvalProductsInit_Node(Task, 0x0ffffff);
  }

// --------------------------------------------------------------------------

void X_Comp::EvalProducts()
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

        flag dbg=0;//((Debug|dbgDerivs) & DBG_Derivs);

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

        if (dbg)
          for (ic = 0; (i=I[ic])>=0; ic++)
            if (IO_In(i))
              {
              char t[128];
              sprintf(t, "%s.%s", FullObjTag(), IODesc_Self(i)->pName);
              dbgpln("i-Xfr %-12.12s : %14.6g, %14.6g | %14.6g",t,
                     IOConduit(i)->QMass(som_SL), IOConduit(i)->QMass(som_Vap), K_2_C(IOConduit(i)->Temp()));
              }

        double denom;
        double  P1 = IOP_Rmt (IOWithId_Self(ioid_In));
        double  P2 = IOP_Self(IOWithId_Self(ioid_In));

        // hss 9/3/98 - Use data base for specific heat ratio.
        P_SpecificHeatRatio = Max(1.1, Sd.CpCv());

        if( Sd.Temp() > 0.01 )
          Tin = Sd.Temp();

        // hss Calc Polytropic Efficiency
        /*double FlowMin  = pI->QVolume() * 60.0;
        double rpm      = Max(SpeedRatio, 10000.0);
        double EffTemp  = Efficiency.Zxy(FlowMin,rpm);
        if (EffTemp >= 0.6)
          P_PolytropicEff = EffTemp;
        else
          P_PolytropicEff = 0.6;*/

        denom = P_SpecificHeatRatio * P_PolytropicEff;
        if( fabs(denom) < 1.0e-30)
          denom = 1.0e-30;
        // hss Try a fix to prevent crash when P1 is negative
        if ((P1 > 0.0) && (P2 > P1))
          Tout = Tin*pow(P2/P1,(P_SpecificHeatRatio - 1.0)/denom);
        else
          Tout = Tin;      // end of kluge

        Sd.SetTemp(Tout);

        double Qin=Sd.QMass(som_ALL);
        // What Comes in must go out
        double Scl=Range(0.0, Qin/GTZ(Qot), 1000.0);
        for (ic = 0; (i=I[ic])>=0; ic++)
          if (IO_Out(i))
            IOConduit(i)->QSetM(Sd, som_ALL, IOQmEst_Out(i)*Scl, Press);//Joins[JoinId].Pressure());//PMax);
        }
        break;
      }
  };

//=========================================================================
