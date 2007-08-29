//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __SPDBE_IAF_CPP
#include "sp_db.h"
#include "surfaces.h"
#include "math.h"

//#define dbgBuildSDB    0
// ==========================================================================
//
//
//
// ===========================================================================
#if WithIF97H2OMdl
// ==========================================================================
//
//
//
// ===========================================================================

#if dbgBuildSDB
#include "dbgmngr.h"
static CDbgMngr dbgBuild("Specie_DB", "BuildIF97");
#endif

#define WITHH2OTIMERS (0 && WITHTIMERS)
#if WITHH2OTIMERS
  #define H2OTIMERLAP(x) CStopWatchLap SW##__LINE__(x)
#else
  #define H2OTIMERLAP(x)
#endif

#if WITHH2OTIMERS
static CGlblStopWatch sv_swSWvp(gs_swFlwSolve, "H2O97", "SWvp");
static CGlblStopWatch sv_swSWvh(gs_swFlwSolve, "H2O97", "SWvh");
static CGlblStopWatch sv_swSWlh(gs_swFlwSolve, "H2O97", "SWlh");
static CGlblStopWatch sv_swSWvs(gs_swFlwSolve, "H2O97", "SWvs");
static CGlblStopWatch sv_swSWls(gs_swFlwSolve, "H2O97", "SWls");
static CGlblStopWatch sv_swSWvc(gs_swFlwSolve, "H2O97", "SWvc");
static CGlblStopWatch sv_swSWlc(gs_swFlwSolve, "H2O97", "SWlc");
static CGlblStopWatch sv_swSWvd(gs_swFlwSolve, "H2O97", "SWvd");
static CGlblStopWatch sv_swSWld(gs_swFlwSolve, "H2O97", "SWld");
static CGlblStopWatch sv_swSWet(gs_swFlwSolve, "H2O97", "SWet");
#endif

// ==========================================================================
//
//  Encapulated in a class to provide auto initialisation
//
// ===========================================================================


//  water97_v12: A collection of Visual Basic functions
//  for calculating properties of water and steam.
//
//  Source: IAPWS-IF97. For details see
//  http://chemengineer.about.com/library/weekly/aa071700a.htm
//  http://chemengineer.about.com/library/weekly/aa073100a.htm
//  http://chemengineer.about.com/library/weekly/aa081400a.htm
//
//  Version 1.2, 02/06/01:   starting value for iteration in densreg3 for
//                           supercritical temperatures changed from 500 to 600
//
//
//  Version 1.1, 01/29/01:   mistake in calculation of partial derivatives
//                           for thermal conductivity corrected
//
//  (c) B. Spang, About Guide to Chemical Engineering, Hamburg, 2000, 2001
//      E-Mail: chemengineer.guide@about.com
//
//

const double rgas_water = 461.526;  // //gas constant in J/(kg K)
const double tc_water = 647.096;    // //critical temperature in K
const double pc_water = 220.64;     // //critical pressure in bar
const double dc_water = 322.0;      //  //critical density in kg/m**3

const double H20MoleWt = 18.01528;

class CIF97Calcs
  {
  protected:
    int /*double*/    ireg1[34+1];
    int /*double*/    jreg1[34+1];
    double nreg1[34+1];
    int /*double*/    j0reg2[9+1];
    double n0reg2[9+1];
    int /*double*/    ireg2[43+1];
    int /*double*/    jreg2[43+1];
    double nreg2[43+1];
    int /*double*/    ireg3[40+1];
    int /*double*/    jreg3[40+1];
    double nreg3[40+1];
    double nreg4[10+1];
    double nbound[5+1];
    double n0visc[3+1];
    int /*double*/    ivisc[19+1];
    int /*double*/    jvisc[19+1];
    double nvisc[19+1];
    double n0thcon[3+1];
    double nthcon[4+1][5+1];

  public:
    CIF97Calcs();

    void CInitFieldsreg1();
    void CInitFieldsreg2();
    void CInitFieldsreg3();
    void CInitFieldsreg4();
    void CInitFieldsbound();
    void CInitFieldsvisc();
    void CInitFieldsthcon();
    void InitFieldsreg1() {};
    void InitFieldsreg2() {};
    void InitFieldsreg3() {};
    void InitFieldsreg4() {};
    void InitFieldsbound() {};
    void InitFieldsvisc() {};
    void InitFieldsthcon() {};
    double gammareg1(double tau, double pi);
    double gammapireg1(double tau, double pi);
    double gammapipireg1(double tau, double pi);
    double gammataureg1(double tau, double pi);
    double gammatautaureg1(double tau, double pi);
    double gammapitaureg1(double tau, double pi);
    double gamma0reg2(double tau, double pi);
    double gamma0pireg2(double tau, double pi);
    double gamma0pipireg2(double tau, double pi);
    double gamma0taureg2(double tau, double pi);
    double gamma0tautaureg2(double tau, double pi);
    double gamma0pitaureg2(double tau, double pi);
    double gammarreg2(double tau, double pi);
    double gammarpireg2(double tau, double pi);
    double gammarpipireg2(double tau, double pi);
    double gammartaureg2(double tau, double pi);
    double gammartautaureg2(double tau, double pi);
    double gammarpitaureg2(double tau, double pi);
    double fireg3(double tau, double delta);
    double fideltareg3(double tau, double delta);
    double fideltadeltareg3(double tau, double delta);
    double fitaureg3(double tau, double delta);
    double fitautaureg3(double tau, double delta);
    double fideltataureg3(double tau, double delta);
    double psivisc(double tau, double delta);
    double lambthcon(double temperature, double pressure, double tau, double delta);
    double pSatW(double temperature);
    double tSatW(double pressure);
    double pBound(double pressure);
    double tBound(double pressure);
    double volreg1(double temperature, double pressure);
    double energyreg1(double temperature, double pressure);
    double entropyreg1(double temperature, double pressure);
    double enthalpyreg1(double temperature, double pressure);
    double cpreg1(double temperature, double pressure);
    double cvreg1(double temperature, double pressure);
    double volreg2(double temperature, double pressure);
    double energyreg2(double temperature, double pressure);
    double entropyreg2(double temperature, double pressure);
    double enthalpyreg2(double temperature, double pressure);
    double cpreg2(double temperature, double pressure);
    double cvreg2(double temperature, double pressure);
    double pressreg3(double temperature, double density);
    double energyreg3(double temperature, double density);
    double entropyreg3(double temperature, double density);
    double enthalpyreg3(double temperature, double density);
    double cpreg3(double temperature, double density);
    double cvreg3(double temperature, double density);
    double densreg3(double temperature, double pressure);
    double densW(double temperature, double pressure);
    double energyW(double temperature, double pressure);
    double entropyW(double temperature, double pressure);
    double enthalpyW(double temperature, double pressure);
    double cpW(double temperature, double pressure);
    double cvW(double temperature, double pressure);
    double viscW(double temperature, double pressure);
    double thconW(double temperature, double pressure);
    double densSatLiqTW(double temperature);
    double densSatVapTW(double temperature);
    double densSatLiqPW(double pressure);
    double densSatVapPW(double pressure);
    double energySatLiqTW(double temperature);
    double energySatVapTW(double temperature);
    double energySatLiqPW(double pressure);
    double energySatVapPW(double pressure);
    double entropySatLiqTW(double temperature);
    double entropySatVapTW(double temperature);
    double entropySatLiqPW(double pressure);
    double entropySatVapPW(double pressure);
    double enthalpySatLiqTW(double temperature);
    double enthalpySatVapTW(double temperature);
    double enthalpySatLiqPW(double pressure);
    double enthalpySatVapPW(double pressure);
    double cpSatLiqTW(double temperature);
    double cpSatVapTW(double temperature);
    double cpSatLiqPW(double pressure);
    double cpSatVapPW(double pressure);
    double cvSatLiqTW(double temperature);
    double cvSatVapTW(double temperature);
    double cvSatLiqPW(double pressure);
    double cvSatVapPW(double pressure);
    double viscSatLiqTW(double temperature);
    double viscSatVapTW(double temperature);
    double viscSatLiqPW(double pressure);
    double viscSatVapPW(double pressure);
    double thconSatLiqTW(double temperature);
    double thconSatVapTW(double temperature);
    double thconSatLiqPW(double pressure);
    double thconSatVapPW(double pressure);
  };




CIF97Calcs::CIF97Calcs()
  {
  CInitFieldsreg1();
  CInitFieldsreg2();
  CInitFieldsreg3();
  CInitFieldsreg4();
  CInitFieldsbound();
  CInitFieldsvisc();
  CInitFieldsthcon();
  };



void CIF97Calcs::CInitFieldsreg1()
  {
  //
  //  Initialize coefficients and exponents for region 1
  //
  ireg1[1] = 0;
  ireg1[2] = 0;
  ireg1[3] = 0;
  ireg1[4] = 0;
  ireg1[5] = 0;
  ireg1[6] = 0;
  ireg1[7] = 0;
  ireg1[8] = 0;
  ireg1[9] = 1;
  ireg1[10] = 1;
  ireg1[11] = 1;
  ireg1[12] = 1;
  ireg1[13] = 1;
  ireg1[14] = 1;
  ireg1[15] = 2;
  ireg1[16] = 2;
  ireg1[17] = 2;
  ireg1[18] = 2;
  ireg1[19] = 2;
  ireg1[20] = 3;
  ireg1[21] = 3;
  ireg1[22] = 3;
  ireg1[23] = 4;
  ireg1[24] = 4;
  ireg1[25] = 4;
  ireg1[26] = 5;
  ireg1[27] = 8;
  ireg1[28] = 8;
  ireg1[29] = 21;
  ireg1[30] = 23;
  ireg1[31] = 29;
  ireg1[32] = 30;
  ireg1[33] = 31;
  ireg1[34] = 32;
  //
  jreg1[1] = -2;
  jreg1[2] = -1;
  jreg1[3] = 0;
  jreg1[4] = 1;
  jreg1[5] = 2;
  jreg1[6] = 3;
  jreg1[7] = 4;
  jreg1[8] = 5;
  jreg1[9] = -9;
  jreg1[10] = -7;
  jreg1[11] = -1;
  jreg1[12] = 0;
  jreg1[13] = 1;
  jreg1[14] = 3;
  jreg1[15] = -3;
  jreg1[16] = 0;
  jreg1[17] = 1;
  jreg1[18] = 3;
  jreg1[19] = 17;
  jreg1[20] = -4;
  jreg1[21] = 0;
  jreg1[22] = 6;
  jreg1[23] = -5;
  jreg1[24] = -2;
  jreg1[25] = 10;
  jreg1[26] = -8;
  jreg1[27] = -11;
  jreg1[28] = -6;
  jreg1[29] = -29;
  jreg1[30] = -31;
  jreg1[31] = -38;
  jreg1[32] = -39;
  jreg1[33] = -40;
  jreg1[34] = -41;
  //
  nreg1[1] = 0.14632971213167;
  nreg1[2] = -0.84548187169114;
  nreg1[3] = -3.756360367204;
  nreg1[4] = 3.3855169168385;
  nreg1[5] = -0.95791963387872;
  nreg1[6] = 0.15772038513228;
  nreg1[7] = -0.016616417199501;
  nreg1[8] = 8.1214629983568E-04;
  nreg1[9] = 2.8319080123804E-04;
  nreg1[10] = -6.0706301565874E-04;
  nreg1[11] = -0.018990068218419;
  nreg1[12] = -0.032529748770505;
  nreg1[13] = -0.021841717175414;
  nreg1[14] = -5.283835796993E-05;
  nreg1[15] = -4.7184321073267E-04;
  nreg1[16] = -3.0001780793026E-04;
  nreg1[17] = 4.7661393906987E-05;
  nreg1[18] = -4.4141845330846E-06;
  nreg1[19] = -7.2694996297594E-16;
  nreg1[20] = -3.1679644845054E-05;
  nreg1[21] = -2.8270797985312E-06;
  nreg1[22] = -8.5205128120103E-10;
  nreg1[23] = -2.2425281908E-06;
  nreg1[24] = -6.5171222895601E-07;
  nreg1[25] = -1.4341729937924E-13;
  nreg1[26] = -4.0516996860117E-07;
  nreg1[27] = -1.2734301741641E-09;
  nreg1[28] = -1.7424871230634E-10;
  nreg1[29] = -6.8762131295531E-19;
  nreg1[30] = 1.4478307828521E-20;
  nreg1[31] = 2.6335781662795E-23;
  nreg1[32] = -1.1947622640071E-23;
  nreg1[33] = 1.8228094581404E-24;
  nreg1[34] = -9.3537087292458E-26;
  //
  }
//
//
//
void CIF97Calcs::CInitFieldsreg2()
  {
  //
  //  Initialize coefficients and exponents for region 2
  //
  j0reg2[1] = 0;
  j0reg2[2] = 1;
  j0reg2[3] = -5;
  j0reg2[4] = -4;
  j0reg2[5] = -3;
  j0reg2[6] = -2;
  j0reg2[7] = -1;
  j0reg2[8] = 2;
  j0reg2[9] = 3;
  //
  n0reg2[1] = -9.6927686500217;
  n0reg2[2] = 10.086655968018;
  n0reg2[3] = -0.005608791128302;
  n0reg2[4] = 0.071452738081455;
  n0reg2[5] = -0.40710498223928;
  n0reg2[6] = 1.4240819171444;
  n0reg2[7] = -4.383951131945;
  n0reg2[8] = -0.28408632460772;
  n0reg2[9] = 0.021268463753307;
  //
  ireg2[1] = 1;
  ireg2[2] = 1;
  ireg2[3] = 1;
  ireg2[4] = 1;
  ireg2[5] = 1;
  ireg2[6] = 2;
  ireg2[7] = 2;
  ireg2[8] = 2;
  ireg2[9] = 2;
  ireg2[10] = 2;
  ireg2[11] = 3;
  ireg2[12] = 3;
  ireg2[13] = 3;
  ireg2[14] = 3;
  ireg2[15] = 3;
  ireg2[16] = 4;
  ireg2[17] = 4;
  ireg2[18] = 4;
  ireg2[19] = 5;
  ireg2[20] = 6;
  ireg2[21] = 6;
  ireg2[22] = 6;
  ireg2[23] = 7;
  ireg2[24] = 7;
  ireg2[25] = 7;
  ireg2[26] = 8;
  ireg2[27] = 8;
  ireg2[28] = 9;
  ireg2[29] = 10;
  ireg2[30] = 10;
  ireg2[31] = 10;
  ireg2[32] = 16;
  ireg2[33] = 16;
  ireg2[34] = 18;
  ireg2[35] = 20;
  ireg2[36] = 20;
  ireg2[37] = 20;
  ireg2[38] = 21;
  ireg2[39] = 22;
  ireg2[40] = 23;
  ireg2[41] = 24;
  ireg2[42] = 24;
  ireg2[43] = 24;
  //
  jreg2[1] = 0;
  jreg2[2] = 1;
  jreg2[3] = 2;
  jreg2[4] = 3;
  jreg2[5] = 6;
  jreg2[6] = 1;
  jreg2[7] = 2;
  jreg2[8] = 4;
  jreg2[9] = 7;
  jreg2[10] = 36;
  jreg2[11] = 0;
  jreg2[12] = 1;
  jreg2[13] = 3;
  jreg2[14] = 6;
  jreg2[15] = 35;
  jreg2[16] = 1;
  jreg2[17] = 2;
  jreg2[18] = 3;
  jreg2[19] = 7;
  jreg2[20] = 3;
  jreg2[21] = 16;
  jreg2[22] = 35;
  jreg2[23] = 0;
  jreg2[24] = 11;
  jreg2[25] = 25;
  jreg2[26] = 8;
  jreg2[27] = 36;
  jreg2[28] = 13;
  jreg2[29] = 4;
  jreg2[30] = 10;
  jreg2[31] = 14;
  jreg2[32] = 29;
  jreg2[33] = 50;
  jreg2[34] = 57;
  jreg2[35] = 20;
  jreg2[36] = 35;
  jreg2[37] = 48;
  jreg2[38] = 21;
  jreg2[39] = 53;
  jreg2[40] = 39;
  jreg2[41] = 26;
  jreg2[42] = 40;
  jreg2[43] = 58;
  //
  nreg2[1] = -1.7731742473213E-03;
  nreg2[2] = -0.017834862292358;
  nreg2[3] = -0.045996013696365;
  nreg2[4] = -0.057581259083432;
  nreg2[5] = -0.05032527872793;
  nreg2[6] = -3.3032641670203E-05;
  nreg2[7] = -1.8948987516315E-04;
  nreg2[8] = -3.9392777243355E-03;
  nreg2[9] = -0.043797295650573;
  nreg2[10] = -2.6674547914087E-05;
  nreg2[11] = 2.0481737692309E-08;
  nreg2[12] = 4.3870667284435E-07;
  nreg2[13] = -3.227767723857E-05;
  nreg2[14] = -1.5033924542148E-03;
  nreg2[15] = -0.040668253562649;
  nreg2[16] = -7.8847309559367E-10;
  nreg2[17] = 1.2790717852285E-08;
  nreg2[18] = 4.8225372718507E-07;
  nreg2[19] = 2.2922076337661E-06;
  nreg2[20] = -1.6714766451061E-11;
  nreg2[21] = -2.1171472321355E-03;
  nreg2[22] = -23.895741934104;
  nreg2[23] = -5.905956432427E-18;
  nreg2[24] = -1.2621808899101E-06;
  nreg2[25] = -0.038946842435739;
  nreg2[26] = 1.1256211360459E-11;
  nreg2[27] = -8.2311340897998;
  nreg2[28] = 1.9809712802088E-08;
  nreg2[29] = 1.0406965210174E-19;
  nreg2[30] = -1.0234747095929E-13;
  nreg2[31] = -1.0018179379511E-09;
  nreg2[32] = -8.0882908646985E-11;
  nreg2[33] = 0.10693031879409;
  nreg2[34] = -0.33662250574171;
  nreg2[35] = 8.9185845355421E-25;
  nreg2[36] = 3.0629316876232E-13;
  nreg2[37] = -4.2002467698208E-06;
  nreg2[38] = -5.9056029685639E-26;
  nreg2[39] = 3.7826947613457E-06;
  nreg2[40] = -1.2768608934681E-15;
  nreg2[41] = 7.3087610595061E-29;
  nreg2[42] = 5.5414715350778E-17;
  nreg2[43] = -9.436970724121E-07;
  //
  }
//
//
//
void CIF97Calcs::CInitFieldsreg3()
  {
  //
  //  Initialize coefficients and exponents for region 3
  //
  ireg3[1] = 0;
  ireg3[2] = 0;
  ireg3[3] = 0;
  ireg3[4] = 0;
  ireg3[5] = 0;
  ireg3[6] = 0;
  ireg3[7] = 0;
  ireg3[8] = 0;
  ireg3[9] = 1;
  ireg3[10] = 1;
  ireg3[11] = 1;
  ireg3[12] = 1;
  ireg3[13] = 2;
  ireg3[14] = 2;
  ireg3[15] = 2;
  ireg3[16] = 2;
  ireg3[17] = 2;
  ireg3[18] = 2;
  ireg3[19] = 3;
  ireg3[20] = 3;
  ireg3[21] = 3;
  ireg3[22] = 3;
  ireg3[23] = 3;
  ireg3[24] = 4;
  ireg3[25] = 4;
  ireg3[26] = 4;
  ireg3[27] = 4;
  ireg3[28] = 5;
  ireg3[29] = 5;
  ireg3[30] = 5;
  ireg3[31] = 6;
  ireg3[32] = 6;
  ireg3[33] = 6;
  ireg3[34] = 7;
  ireg3[35] = 8;
  ireg3[36] = 9;
  ireg3[37] = 9;
  ireg3[38] = 10;
  ireg3[39] = 10;
  ireg3[40] = 11;
  //
  jreg3[1] = 0;
  jreg3[2] = 0;
  jreg3[3] = 1;
  jreg3[4] = 2;
  jreg3[5] = 7;
  jreg3[6] = 10;
  jreg3[7] = 12;
  jreg3[8] = 23;
  jreg3[9] = 2;
  jreg3[10] = 6;
  jreg3[11] = 15;
  jreg3[12] = 17;
  jreg3[13] = 0;
  jreg3[14] = 2;
  jreg3[15] = 6;
  jreg3[16] = 7;
  jreg3[17] = 22;
  jreg3[18] = 26;
  jreg3[19] = 0;
  jreg3[20] = 2;
  jreg3[21] = 4;
  jreg3[22] = 16;
  jreg3[23] = 26;
  jreg3[24] = 0;
  jreg3[25] = 2;
  jreg3[26] = 4;
  jreg3[27] = 26;
  jreg3[28] = 1;
  jreg3[29] = 3;
  jreg3[30] = 26;
  jreg3[31] = 0;
  jreg3[32] = 2;
  jreg3[33] = 26;
  jreg3[34] = 2;
  jreg3[35] = 26;
  jreg3[36] = 2;
  jreg3[37] = 26;
  jreg3[38] = 0;
  jreg3[39] = 1;
  jreg3[40] = 26;
  //
  nreg3[1] = 1.0658070028513;
  nreg3[2] = -15.732845290239;
  nreg3[3] = 20.944396974307;
  nreg3[4] = -7.6867707878716;
  nreg3[5] = 2.6185947787954;
  nreg3[6] = -2.808078114862;
  nreg3[7] = 1.2053369696517;
  nreg3[8] = -8.4566812812502E-03;
  nreg3[9] = -1.2654315477714;
  nreg3[10] = -1.1524407806681;
  nreg3[11] = 0.88521043984318;
  nreg3[12] = -0.64207765181607;
  nreg3[13] = 0.38493460186671;
  nreg3[14] = -0.85214708824206;
  nreg3[15] = 4.8972281541877;
  nreg3[16] = -3.0502617256965;
  nreg3[17] = 0.039420536879154;
  nreg3[18] = 0.12558408424308;
  nreg3[19] = -0.2799932969871;
  nreg3[20] = 1.389979956946;
  nreg3[21] = -2.018991502357;
  nreg3[22] = -8.2147637173963E-03;
  nreg3[23] = -0.47596035734923;
  nreg3[24] = 0.0439840744735;
  nreg3[25] = -0.44476435428739;
  nreg3[26] = 0.90572070719733;
  nreg3[27] = 0.70522450087967;
  nreg3[28] = 0.10770512626332;
  nreg3[29] = -0.32913623258954;
  nreg3[30] = -0.50871062041158;
  nreg3[31] = -0.022175400873096;
  nreg3[32] = 0.094260751665092;
  nreg3[33] = 0.16436278447961;
  nreg3[34] = -0.013503372241348;
  nreg3[35] = -0.014834345352472;
  nreg3[36] = 5.7922953628084E-04;
  nreg3[37] = 3.2308904703711E-03;
  nreg3[38] = 8.0964802996215E-05;
  nreg3[39] = -1.6557679795037E-04;
  nreg3[40] = -4.4923899061815E-05;
  //
  }
//
//
//
void CIF97Calcs::CInitFieldsreg4()
  {
  //
  //  Initialize coefficients for region 4
  //
  nreg4[1] = 1167.0521452767;
  nreg4[2] = -724213.16703206;
  nreg4[3] = -17.073846940092;
  nreg4[4] = 12020.82470247;
  nreg4[5] = -3232555.0322333;
  nreg4[6] = 14.91510861353;
  nreg4[7] = -4823.2657361591;
  nreg4[8] = 405113.40542057;
  nreg4[9] = -0.23855557567849;
  nreg4[10] = 650.17534844798;
  //
  }
//
//
//
void CIF97Calcs::CInitFieldsbound()
  {
  //
  //  Initialize coefficients for boundary equation
  //
  nbound[1] = 348.05185628969;
  nbound[2] = -1.1671859879975;
  nbound[3] = 1.0192970039326E-03;
  nbound[4] = 572.54459862746;
  nbound[5] = 13.91883977887;
  //
  }
//
//
//
void CIF97Calcs::CInitFieldsvisc()
  {
  //
  //  Initialize coefficients and exponents for viscosity
  //
  n0visc[0] = 1.0;
  n0visc[1] = 0.978197;
  n0visc[2] = 0.579829;
  n0visc[3] = -0.202354;
  //
  ivisc[1] = 0;
  ivisc[2] = 0;
  ivisc[3] = 0;
  ivisc[4] = 0;
  ivisc[5] = 1;
  ivisc[6] = 1;
  ivisc[7] = 1;
  ivisc[8] = 1;
  ivisc[9] = 2;
  ivisc[10] = 2;
  ivisc[11] = 2;
  ivisc[12] = 3;
  ivisc[13] = 3;
  ivisc[14] = 3;
  ivisc[15] = 3;
  ivisc[16] = 4;
  ivisc[17] = 4;
  ivisc[18] = 5;
  ivisc[19] = 6;
  //
  jvisc[1] = 0;
  jvisc[2] = 1;
  jvisc[3] = 4;
  jvisc[4] = 5;
  jvisc[5] = 0;
  jvisc[6] = 1;
  jvisc[7] = 2;
  jvisc[8] = 3;
  jvisc[9] = 0;
  jvisc[10] = 1;
  jvisc[11] = 2;
  jvisc[12] = 0;
  jvisc[13] = 1;
  jvisc[14] = 2;
  jvisc[15] = 3;
  jvisc[16] = 0;
  jvisc[17] = 3;
  jvisc[18] = 1;
  jvisc[19] = 3;
  //
  nvisc[1] = 0.5132047;
  nvisc[2] = 0.3205656;
  nvisc[3] = -0.7782567;
  nvisc[4] = 0.1885447;
  nvisc[5] = 0.2151778;
  nvisc[6] = 0.7317883;
  nvisc[7] = 1.241044;
  nvisc[8] = 1.476783;
  nvisc[9] = -0.2818107;
  nvisc[10] = -1.070786;
  nvisc[11] = -1.263184;
  nvisc[12] = 0.1778064;
  nvisc[13] = 0.460504;
  nvisc[14] = 0.2340379;
  nvisc[15] = -0.4924179;
  nvisc[16] = -0.0417661;
  nvisc[17] = 0.1600435;
  nvisc[18] = -0.01578386;
  nvisc[19] = -0.003629481;
  //
  }
//
//
//
void CIF97Calcs::CInitFieldsthcon()
  {
  //
  //  Initialize coefficients and exponents for thermal conductivity
  //
  n0thcon[0] = 1.0;
  n0thcon[1] = 6.978267;
  n0thcon[2] = 2.599096;
  n0thcon[3] = -0.998254;
  //
  nthcon[0][0] = 1.3293046;
  nthcon[0][1] = -0.40452437;
  nthcon[0][2] = 0.2440949;
  nthcon[0][3] = 0.018660751;
  nthcon[0][4] = -0.12961068;
  nthcon[0][5] = 0.044809953;
  nthcon[1][0] = 1.7018363;
  nthcon[1][1] = -2.2156845;
  nthcon[1][2] = 1.6511057;
  nthcon[1][3] = -0.76736002;
  nthcon[1][4] = 0.37283344;
  nthcon[1][5] = -0.1120316;
  nthcon[2][0] = 5.2246158;
  nthcon[2][1] = -10.124111;
  nthcon[2][2] = 4.9874687;
  nthcon[2][3] = -0.27297694;
  nthcon[2][4] = -0.43083393;
  nthcon[2][5] = 0.13333849;
  nthcon[3][0] = 8.7127675;
  nthcon[3][1] = -9.5000611;
  nthcon[3][2] = 4.3786606;
  nthcon[3][3] = -0.91783782;
  nthcon[3][4] = 0.0;
  nthcon[3][5] = 0.0;
  nthcon[4][0] = -1.8525999;
  nthcon[4][1] = 0.9340469;
  nthcon[4][2] = 0.0;
  nthcon[4][3] = 0.0;
  nthcon[4][4] = 0.0;
  nthcon[4][5] = 0.0;
  //
  }
//
//
// This is a workaround for some compiler problem when this function expands
static __declspec(noinline) double My_Pow_int(double _X, int _Y)
  {
  unsigned int _N;
  if (_Y >= 0)
    _N = _Y;
  else
    _N = -_Y;
  for (double _Z = double (1); ; _X *= _X)
    {
    if ((_N & 1) != 0)
      _Z *= _X;
    if ((_N >>= 1) == 0)
      return (_Y < 0 ? double(1) / _Z : _Z); 
    }
  }

double CIF97Calcs::gammareg1(double tau, double pi)
  {
  //
  // Fundamental equation for region 1
  //
  InitFieldsreg1();
  double val = 0;
  for (int i=1; i<=34; i++)
      val = val + nreg1[i] * My_Pow_int((7.1 - pi), ireg1[i]) * My_Pow_int((tau - 1.222), jreg1[i]);
  //
  return val;
  }
//
//
//
double CIF97Calcs::gammapireg1(double tau, double pi)
  {
  //
  // First derivative of fundamental equation in pi for region 1
  //
  InitFieldsreg1();
  double val = 0;
  for (int i=1; i<=34; i++)
      val = val - nreg1[i] * ireg1[i] * My_Pow_int((7.1 - pi) ,(ireg1[i] - 1)) * My_Pow_int((tau - 1.222) , jreg1[i]);
  //
  return val;
  }
//
//
//
double CIF97Calcs::gammapipireg1(double tau, double pi)
  {
  //
  // Second derivative of fundamental equation in pi for region 1
  //
  InitFieldsreg1();
  double val = 0;
  for ( int i=1; i<=34;  i++)
      val = val + nreg1[i] * ireg1[i] * (ireg1[i] - 1) * My_Pow_int((7.1 - pi) , (ireg1[i] - 2)) * My_Pow_int((tau - 1.222) , jreg1[i]);
  //
  return val;
  }
//
//
//
double CIF97Calcs::gammataureg1(double tau, double pi)
  {
  //
  // First derivative of fundamental equation in tau for region 1
  //
  InitFieldsreg1();
  double val = 0;
  for ( int i=1; i<=34;  i++)
      val = val + nreg1[i] * My_Pow_int((7.1 - pi) , ireg1[i]) * jreg1[i] * My_Pow_int((tau - 1.222) , (jreg1[i] - 1));
  //
  return val;
  }
//
//
//
double CIF97Calcs::gammatautaureg1(double tau, double pi)
  {
  //
  // Second derivative of fundamental equation in tau for region 1
  //
  InitFieldsreg1();
  double val = 0;
  for ( int i=1; i<=34;  i++)
      val = val + nreg1[i] * My_Pow_int((7.1 - pi) , ireg1[i]) * jreg1[i] * (jreg1[i] - 1) * My_Pow_int((tau - 1.222) , (jreg1[i] - 2));
  //
  return val;
  } //
//
//
//
double CIF97Calcs::gammapitaureg1(double tau, double pi)
  {
  //
  // Second derivative of fundamental equation in pi and tau for region 1
  //
  InitFieldsreg1();
  double val = 0;
  for ( int i=1; i<=34;  i++)
      val = val - nreg1[i] * ireg1[i] * My_Pow_int((7.1 - pi) , (ireg1[i] - 1)) * jreg1[i] * My_Pow_int((tau - 1.222) , (jreg1[i] - 1));
  //
  return val;
  }
//
//
//
double CIF97Calcs::gamma0reg2(double tau, double pi)
  {
  //
  // Ideal-gas part of fundamental equation for region 2
  //
  InitFieldsreg2();
  double val = Ln(pi);
  for (int i = 1; i <= 9; i++)
      val = val + n0reg2[i] * My_Pow_int(tau , j0reg2[i]);
  //
  return val;
  }
//
//
//
double CIF97Calcs::gamma0pireg2(double tau, double pi)
  {
  //
  // First derivative in pi of ideal-gas part of fundamental equation for region 2
  //
  double val = 1 / pi;
  //
  return val;
  }
//
//
//
double CIF97Calcs::gamma0pipireg2(double tau, double pi)
  {
  //
  // Second derivative in pi of ideal-gas part of fundamental equation for region 2
  //
  double val = -1 / (pi * pi);
  //
  return val;
  }
//
//
//
double CIF97Calcs::gamma0taureg2(double tau, double pi)
  {
  //
  // First derivative in tau of ideal-gas part of fundamental equation for region 2
  //
  InitFieldsreg2();
  double val = 0;
  for (int i = 1; i <= 9; i++)
    val = val + n0reg2[i] * j0reg2[i] * My_Pow_int(tau , (j0reg2[i] - 1));
  //
  return val;
  }
//
//
//
double CIF97Calcs::gamma0tautaureg2(double tau, double pi)
  {
  //
  // Second derivative in tau of ideal-gas part of fundamental equation for region 2
  //
  InitFieldsreg2();
  double val = 0;
  for (int i = 1; i <= 9; i++)
    val = val + n0reg2[i] * j0reg2[i] * (j0reg2[i] - 1) * My_Pow_int(tau , (j0reg2[i] - 2));
  //
  return val;
  }
//
//
//
double CIF97Calcs::gamma0pitaureg2(double tau, double pi)
  {
  //
  // Second derivative in pi and tau of ideal-gas part of fundamental equation for region 2
  //
  InitFieldsreg2();
  double val = 0;
  //
  return val;
  }
//
//
//
double CIF97Calcs::gammarreg2(double tau, double pi)
  {
  //
  // Residual part of fundamental equation for region 2
  //
  InitFieldsreg2();
  double val = 0;
  for (int i = 1; i<=43; i++)
    val = val + nreg2[i] * My_Pow_int(pi , ireg2[i]) * My_Pow_int((tau - 0.5) , jreg2[i]);
  //
  return val;
  }
//
//
//
double CIF97Calcs::gammarpireg2(double tau, double pi)
  {
  //
  // First derivative in pi of residual part of fundamental equation for region 2
  //
  InitFieldsreg2();
  double val = 0;
  for (int i = 1; i<=43; i++)
    val = val + nreg2[i] * ireg2[i] * My_Pow_int(pi , (ireg2[i] - 1)) * My_Pow_int((tau - 0.5) , jreg2[i]);
  //
  return val;
  }
//
//
//
double CIF97Calcs::gammarpipireg2(double tau, double pi)
  {
  //
  // Second derivative in pi of residual part of fundamental equation for region 2
  //
  InitFieldsreg2();
  double val = 0;
  for (int i = 1; i<=43; i++)
    val = val + nreg2[i] * ireg2[i] * (ireg2[i] - 1) * My_Pow_int(pi , (ireg2[i] - 2)) * My_Pow_int((tau - 0.5) , jreg2[i]);
  //
  return val;
  }
//
//
//
double CIF97Calcs::gammartaureg2(double tau, double pi)
  {
  //
  // First derivative in tau of residual part of fundamental equation for region 2
  //
  InitFieldsreg2();
  double val = 0;
  for (int i = 1; i<=43; i++)
    val = val + nreg2[i] * My_Pow_int(pi , ireg2[i]) * jreg2[i] * My_Pow_int((tau - 0.5) , (jreg2[i] - 1));
  //
  return val;
  }
//
//
//
double CIF97Calcs::gammartautaureg2(double tau, double pi)
  {
  //
  // Second derivative in tau of residual part of fundamental equation for region 2
  //
  InitFieldsreg2();
  double val = 0;
  for (int i = 1; i<=43; i++)
    val = val + nreg2[i] * My_Pow_int(pi , ireg2[i]) * jreg2[i] * (jreg2[i] - 1) * My_Pow_int((tau - 0.5) , (jreg2[i] - 2));
  //
  return val;
  }
//
//
//
double CIF97Calcs::gammarpitaureg2(double tau, double pi)
  {
  //
  // Second derivative in pi and tau of residual part of fundamental equation for region 2
  //
  InitFieldsreg2();
  double val = 0;
  for (int i = 1; i<=43; i++)
    val = val + nreg2[i] * ireg2[i] * My_Pow_int(pi , (ireg2[i] - 1)) * jreg2[i] * My_Pow_int((tau - 0.5) , (jreg2[i] - 1));
  //
  return val;
  }
//
//
//
double CIF97Calcs::fireg3(double tau, double delta)
  {
  //
  // Fundamental equation for region 3
  //
  InitFieldsreg3();
  double val = nreg3[1] * Ln(delta);
  for (int i = 2; i<=40; i++)
    val = val + nreg3[i] * My_Pow_int(delta , ireg3[i]) * My_Pow_int(tau , jreg3[i]);
  //
  return val;
  }
//
//
//
double CIF97Calcs::fideltareg3(double tau, double delta)
  {
  //
  // First derivative in delta of fundamental equation for region 3
  //
  InitFieldsreg3();
  double val = nreg3[1] / delta;
  for (int i = 2; i<=40; i++)
    val = val + nreg3[i] * ireg3[i] * My_Pow_int(delta , (ireg3[i] - 1)) * My_Pow_int(tau , jreg3[i]);
  //
  return val;
  }
//
//
//
double CIF97Calcs::fideltadeltareg3(double tau, double delta)
  {
  //
  // Second derivative in delta of fundamental equation for region 3
  //
  InitFieldsreg3();
  double val = -nreg3[1] / (delta * delta);
  for (int i = 2; i<=40; i++)
    val = val + nreg3[i] * ireg3[i] * (ireg3[i] - 1) * My_Pow_int(delta , (ireg3[i] - 2)) * My_Pow_int(tau , jreg3[i]);
  //
  return val;
  }
//
//
//
double CIF97Calcs::fitaureg3(double tau, double delta)
  {
  //
  // First derivative in tau of fundamental equation for region 3
  //
  InitFieldsreg3();
  double val = 0;
  for (int i = 2; i<=40; i++)
    val = val + nreg3[i] * My_Pow_int(delta , ireg3[i]) * jreg3[i] * My_Pow_int(tau , (jreg3[i] - 1));
  //
  return val;
  }
//
//
//
double CIF97Calcs::fitautaureg3(double tau, double delta)
  {
  //
  // Second derivative in tau of fundamental equation for region 3
  //
  InitFieldsreg3();
  double val = 0;
  for (int i = 2; i<=40; i++)
    val = val + nreg3[i] * My_Pow_int(delta , ireg3[i]) * jreg3[i] * (jreg3[i] - 1) * My_Pow_int(tau , (jreg3[i] - 2));
  //
  return val;
  }
//
//
//
double CIF97Calcs::fideltataureg3(double tau, double delta)
  {
  //
  // Second derivative in delta and tau of fundamental equation for region 3
  //
  InitFieldsreg3();
  double val = 0;
  for (int i = 2; i<=40; i++)
    val = val + nreg3[i] * ireg3[i] * My_Pow_int(delta , (ireg3[i] - 1)) * jreg3[i] * My_Pow_int(tau , (jreg3[i] - 1));
  //
  return val;
  }
//
//
//
double CIF97Calcs::psivisc(double tau, double delta)
  {
  //
  // Reduced dynamic viscosity
  //
  InitFieldsvisc();
  double psi0 = 0;
  double psi1 = 0;
  for (int i = 0;i<=3; i++)
    psi0 = psi0 + n0visc[i] * My_Pow_int(tau , i);
  psi0 = 1 / (pow(tau , 0.5) * psi0);
  for (i = 1; i<=19; i++)
    psi1 = psi1 + nvisc[i] * My_Pow_int((delta - 1.0) , ivisc[i]) * My_Pow_int((tau - 1.0) , jvisc[i]);
  psi1 = exp(delta * psi1);
  double val = psi0 * psi1;
  //
  return val;
  }
//
//
//
double CIF97Calcs::lambthcon(double temperature, double pressure, double tau, double delta)
  {
  //
  // Reduced thermal conductivity
  //
  InitFieldsthcon();
  double lamb0 = 0;
  double lamb1 = 0;
  for (int i=0; i<=3; i++)
  lamb0 = lamb0 + n0thcon[i] * My_Pow_int(tau , i);
  lamb0 = 1 / (pow(tau , 0.5) * lamb0);
  for (i = 0; i<=4; i++)
  for (int j=0; j<=5; j++)
    lamb1 = lamb1 + nthcon[i][j] * My_Pow_int((tau - 1.0) , i) * My_Pow_int((delta - 1.0) , j);
  lamb1 = exp(delta * lamb1);
  //
  // v1.1: calculation of lamb2 corrected
  //
  double taus;
  double pis;
  double deltas;
  double dpidtau;
  double ddeltadpi;
  if (temperature >= 273.15 && temperature <= 623.15 && pressure >= pSatW(temperature) && pressure <= 1000.0)
    {
    //  region 1
    taus = 1386.0 / temperature;
    pis = pressure / 165.3;
    dpidtau = (647.226 * 165.3 * (gammapitaureg1(taus, pis) * 1386.0 - gammapireg1(taus, pis) * temperature)) / (221.15 * temperature *temperature * gammapipireg1(taus, pis));
    ddeltadpi = -(22115000.0 * gammapipireg1(taus, pis)) / (317.763 * rgas_water * temperature * My_Pow_int(gammapireg1(taus, pis) , 2));
    }
  else if ( (temperature >= 273.15 && temperature <= 623.15 && pressure > 0 && pressure <= pSatW(temperature)) || (temperature >= 623.15 && temperature <= 863.15 && pressure > 0 && pressure <= pBound(temperature)) || (temperature >= 863.15 && temperature <= 1073.15 && pressure > 0 && pressure <= 1000.0) )
    {
    //  region 2
    taus = 540.0 / temperature;
    pis = pressure / 10.0;
    dpidtau = (647.226 * 10.0 * ((gamma0pitaureg2(taus, pis) + gammarpitaureg2(taus, pis)) * 540.0 - (gamma0pireg2(taus, pis) + gammarpireg2(taus, pis)) * temperature)) / (221.15 * temperature * temperature * (gamma0pipireg2(taus, pis) + gammarpipireg2(taus, pis)));
    ddeltadpi = -(22115000.0 * (gamma0pipireg2(taus, pis) + gammarpipireg2(taus, pis))) / (317.763 * rgas_water * temperature * My_Pow_int((gamma0pireg2(taus, pis) + gammarpireg2(taus, pis)) , 2));
    }
  else if ( temperature >= 623.15 && temperature <= tBound(pressure) && pressure >= pBound(temperature) && pressure <= 1000.0 )
    {
    //  region 3
    taus = 647.096 / temperature;
    deltas = delta * 317.763 / 322.0;
    dpidtau = (647.226 * rgas_water * Sqr(delta * 317.763) * (fideltareg3(taus, deltas) - (647.096 / temperature) * fideltataureg3(taus, deltas))) / (22115000.0 * 322.0);
    ddeltadpi = (22115000.0 * 322.0) / (317.763 * delta * 317.763 * rgas_water * temperature * (2 * fideltareg3(taus, deltas) + (delta * 317.763 / 322.0) * fideltadeltareg3(taus, deltas)));
    }
  else
    {
    //  outside range
    dpidtau = 0;
    ddeltadpi = 0;
    }
  double lamb2 = 0.0013848 / psivisc(tau, delta) * My_Pow_int((tau * delta) , (-2)) * Sqr(dpidtau) * pow((delta * ddeltadpi) , 0.4678) * pow(delta , 0.5) * exp(-18.66 * Sqr(1 / tau - 1) - My_Pow_int((delta - 1) , 4));
  double val = lamb0 * lamb1 + lamb2;
  //
  return val;
  }
//
//
//
double CIF97Calcs::pSatW(double temperature)
  {
  //
  // saturation pressure of water
  // pSatW in bar
  // temperature in K
  //
  // pSatW = -1: temperature outside range
  //
  //
  double val;
  if ( temperature < 273.15 || temperature > 647.096 )
    {
    val = -1.0;
    }
  else
    {
    InitFieldsreg4();
    double del = temperature + nreg4[9] / (temperature - nreg4[10]);
    double aco = Sqr(del) + nreg4[1] * del + nreg4[2];
    double bco = nreg4[3] * Sqr(del) + nreg4[4] * del + nreg4[5];
    double cco = nreg4[6] * Sqr(del) + nreg4[7] * del + nreg4[8];
    val = My_Pow_int((2 * cco / (-bco + pow((Sqr(bco) - 4 * aco * cco) , 0.5))) , 4) * 10;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::tSatW(double pressure)
  {
  //
  // saturation temperature of water
  // tSatW in K
  // pressure in bar
  //
  // tSatW = -1: pressure outside range
  //
  //
  double val;
  if ( pressure < 0.00611213 || pressure > 220.64 )
    {
    val = -1.0;
    }
  else
    {
    InitFieldsreg4();
    double bet = pow((0.1 * pressure) , 0.25);
    double eco = Sqr(bet ) + nreg4[3] * bet + nreg4[6];
    double fco = nreg4[1] * Sqr(bet) + nreg4[4] * bet + nreg4[7];
    double gco = nreg4[2] * Sqr(bet) + nreg4[5] * bet + nreg4[8];
    double dco = 2 * gco / (-fco - pow((Sqr(fco) - 4 * eco * gco) , 0.5));
    val = 0.5 * (nreg4[10] + dco - pow((Sqr(nreg4[10] + dco) - 4 * (nreg4[9] + nreg4[10] * dco)) , 0.5));
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::pBound(double temperature)
  {
  //
  // boundary pressure between regions 2 and 3
  // pBound in bar
  // temperature in K
  //
  // pBound = -1: temperature outside range
  //
  //
  double val;
  if ( temperature < 623.15 || temperature > 863.15 )
    {
    val = -1.0;
    }
  else
    {
    InitFieldsbound();
    val = (nbound[1] + nbound[2] * temperature + nbound[3] * Sqr(temperature)) * 10.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::tBound(double pressure)
  {
  //
  // boundary temperature between regions 2 and 3
  // tBound in K
  // pressure in bar
  //
  // tBound = -1: pressure outside range
  //
  //
  double val;
  if ( pressure < 165.292 || pressure > 1000.0 )
    {
    val = -1.0;
    }
  else
    {
    InitFieldsbound();
    val = nbound[4] + Sqrt((0.1 * pressure - nbound[5]) / nbound[3]);
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::volreg1(double temperature, double pressure)
  {
  //
  // specific volume in region 1
  // volreg1 in m^3/kg
  // temperature in K
  // pressure in bar
  //
  double tau = 1386.0 / temperature;
  double pi = 0.1 * pressure / 16.53;
  double val = rgas_water * temperature * pi * gammapireg1(tau, pi) / (pressure * 100000.0);
  //
  return val;
  }
//
//
//
double CIF97Calcs::energyreg1(double temperature, double pressure)
  {
  //
  // specific internal energy in region 1
  // energyreg1 in kJ/kg
  // temperature in K
  // pressure in bar
  //
  double tau = 1386.0 / temperature;
  double pi = 0.1 * pressure / 16.53;
  double val = 0.001 * rgas_water * temperature * (tau * gammataureg1(tau, pi) - pi * gammapireg1(tau, pi));
  //
  return val;
  }
//
//
//
double CIF97Calcs::entropyreg1(double temperature, double pressure)
  {
  //
  // specific entropy in region 1
  // entropyreg1 in kJ/(kg K)
  // temperature in K
  // pressure in bar
  //
  double tau = 1386.0 / temperature;
  double pi = 0.1 * pressure / 16.53;
  double val = 0.001 * rgas_water * (tau * gammataureg1(tau, pi) - gammareg1(tau, pi));
  //
  return val;
  }
//
//
//
double CIF97Calcs::enthalpyreg1(double temperature, double pressure)
  {
  //
  // specific enthalpy in region 1
  // enthalpyreg1 in kJ/kg
  // temperature in K
  // pressure in bar
  //
  double tau = 1386.0 / temperature;
  double pi = 0.1 * pressure / 16.53;
  double val = 0.001 * rgas_water * temperature * tau * gammataureg1(tau, pi);
  //
  return val;
  }
//
//
//
double CIF97Calcs::cpreg1(double temperature, double pressure)
  {
  //
  // specific isobaric heat capacity in region 1
  // cpreg1 in kJ/(kg K)
  // temperature in K
  // pressure in bar
  //
  double tau = 1386.0 / temperature;
  double pi = 0.1 * pressure / 16.53;
  double val = -0.001 * rgas_water * Sqr(tau) * gammatautaureg1(tau, pi);
  //
  return val;
  }
//
//
//
double CIF97Calcs::cvreg1(double temperature, double pressure)
  {
  //
  // specific isochoric heat capacity in region 1
  // cvreg1 in kJ/(kg K)
  // temperature in K
  // pressure in bar
  //
  double tau = 1386.0 / temperature;
  double pi = 0.1 * pressure / 16.53;
  double val = 0.001 * rgas_water * (-Sqr(tau) * gammatautaureg1(tau, pi) + Sqr(gammapireg1(tau, pi) - tau * gammapitaureg1(tau, pi)) / gammapipireg1(tau, pi));
  //
  return val;
  }
//
//
//
// double spsoundreg1(double temperature, double pressure)
//
// speed of sound in region 1
// spsoundreg1 in m/s
// temperature in K
// pressure in bar
//
//    double tau = 540.0 / temperature
//    double pi = 0.1 * pressure / 16.53
//    double spsoundreg1 = Sqrt(rgas_water * temperature * (Sqr(gammapireg1(tau, pi)) / (Sqr(gammapireg1(tau, pi) - tau * gammapitaureg1(tau, pi)) / (Sqr(tau) * gammatautaureg1(tau, pi)) - gammapipireg1(tau, pi))))
//
//  return val;
//}
//
//
//
double CIF97Calcs::volreg2(double temperature, double pressure)
  {
  //
  // specific volume in region 2
  // volreg2 in m^3/kg
  // temperature in K
  // pressure in bar
  //
  double tau = 540.0 / temperature;
  double pi = 0.1 * pressure;
  double val = rgas_water * temperature * pi * (gamma0pireg2(tau, pi) + gammarpireg2(tau, pi)) / (pressure * 100000.0);
  //
  return val;
  }
//
//
//
double CIF97Calcs::energyreg2(double temperature, double pressure)
  {
  //
  // specific internal energy in region 2
  // energyreg2 in kJ/kg
  // temperature in K
  // pressure in bar
  //
  double tau = 540.0 / temperature;
  double pi = 0.1 * pressure;
  double val = 0.001 * rgas_water * temperature * (tau * (gamma0taureg2(tau, pi) + gammartaureg2(tau, pi)) - pi * (gamma0pireg2(tau, pi) + gammarpireg2(tau, pi)));
  //
  return val;
  }
//
//
//
double CIF97Calcs::entropyreg2(double temperature, double pressure)
  {
  //
  // specific entropy in region 2
  // entropyreg2 in kJ/(kg K)
  // temperature in K
  // pressure in bar
  //
  double tau = 540.0 / temperature;
  double pi = 0.1 * pressure;
  double val = 0.001 * rgas_water * (tau * (gamma0taureg2(tau, pi) + gammartaureg2(tau, pi)) - (gamma0reg2(tau, pi) + gammarreg2(tau, pi)));
  //
  return val;
  }
//
//
//
double CIF97Calcs::enthalpyreg2(double temperature, double pressure)
  {
  //
  // specific enthalpy in region 2
  // enthalpyreg2 in kJ/kg
  // temperature in K
  // pressure in bar
  //
  double tau = 540.0 / temperature;
  double pi = 0.1 * pressure;
  double val = 0.001 * rgas_water * temperature * tau * (gamma0taureg2(tau, pi) + gammartaureg2(tau, pi));
  //
  return val;
  }
//
//
//
double CIF97Calcs::cpreg2(double temperature, double pressure)
  {
  //
  // specific isobaric heat capacity in region 2
  // cpreg2 in kJ/(kg K)
  // temperature in K
  // pressure in bar
  //
  double tau = 540.0 / temperature;
  double pi = 0.1 * pressure;
  double val = -0.001 * rgas_water * Sqr(tau) * (gamma0tautaureg2(tau, pi) + gammartautaureg2(tau, pi));
  //
  return val;
  }
//
//
//
double CIF97Calcs::cvreg2(double temperature, double pressure)
  {
  //
  // specific isochoric heat capacity in region 2
  // cvreg2 in kJ/(kg K)
  // temperature in K
  // pressure in bar
  //
  double tau = 540.0 / temperature;
  double pi = 0.1 * pressure;
  double val = 0.001 * rgas_water * (-Sqr(tau ) * (gamma0tautaureg2(tau, pi) + gammartautaureg2(tau, pi)) - Sqr(1 + pi * gammarpireg2(tau, pi) - tau * pi * gammarpitaureg2(tau, pi)) / (1 - Sqr(pi) * gammarpipireg2(tau, pi)));
  //
  return val;
  }
//
//
//
// double spsoundreg2(double temperature, double pressure)
//
// speed of sound in region 2
// spsoundreg2 in m/s
// temperature in K
// pressure in bar
//
//    double tau = 540.0 / temperature
//    double pi = 0.1 * pressure
//    double spsoundreg2 = (rgas_water * temperature * (1 + 2 * pi * gammarpireg2(tau, pi) + pi ^ 2 * gammarpireg2(tau, pi) ^ 2) / ((1 - pi ^ 2 * gammarpipireg2(tau, pi)) + (1 + pi * gammarpireg2(tau, pi) - tau * pi * gammarpitaureg2(tau, pi)) ^ 2 / (tau ^ 2 * (gamma0tautaureg2(tau, pi) + gammartautaureg2(tau, pi))))) ^ 0.5
//
//  return val;
//}
//
//
//
double CIF97Calcs::pressreg3(double temperature, double density)
  {
  //
  // pressure in region 3
  // pressreg3 in bar
  // temperature in K
  // density in kg/m^3
  //
  double tau = tc_water / temperature;
  double delta = density / dc_water;
  double val = density * rgas_water * temperature * delta * fideltareg3(tau, delta) / 100000.0;
  //
  return val;
  }
//
//
//
double CIF97Calcs::energyreg3(double temperature, double density)
  {
  //
  // specific internal energy in region 3
  // energyreg3 in kJ/kg
  // temperature in K
  // density in kg/m^3
  //
  double tau = tc_water / temperature;
  double delta = density / dc_water;
  double val = 0.001 * rgas_water * temperature * tau * fitaureg3(tau, delta);
  //
  return val;
  }
//
//
//
double CIF97Calcs::entropyreg3(double temperature, double density)
  {
  //
  // specific entropy in region 3
  // entropyreg3 in kJ/(kg K)
  // temperature in K
  // density in kg/m^3
  //
  double tau = tc_water / temperature;
  double delta = density / dc_water;
  double val = 0.001 * rgas_water * (tau * fitaureg3(tau, delta) - fireg3(tau, delta));
  //
  return val;
  }
//
//
//
double CIF97Calcs::enthalpyreg3(double temperature, double density)
  {
  //
  // specific enthalpy in region 3
  // enthalpyreg3 in kJ/kg
  // temperature in K
  // density in kg/m^3
  //
  double tau = tc_water / temperature;
  double delta = density / dc_water;
  double val = 0.001 * rgas_water * temperature * (tau * fitaureg3(tau, delta) + delta * fideltareg3(tau, delta));
  //
  return val;
  }
//
//
//
double CIF97Calcs::cpreg3(double temperature, double density)
  {
  //
  // specific isobaric heat capacity in region 3
  // cpreg3 in kJ/(kg K)
  // temperature in K
  // density in kg/m^3
  //
  double tau = tc_water / temperature;
  double delta = density / dc_water;
  double val = 0.001 * rgas_water * (-Sqr(tau) * fitautaureg3(tau, delta) + Sqr(delta * fideltareg3(tau, delta) - delta * tau * fideltataureg3(tau, delta)) / (2 * delta * fideltareg3(tau, delta) + Sqr(delta) * fideltadeltareg3(tau, delta)));
  //
  return val;
  }
//
//
//
double CIF97Calcs::cvreg3(double temperature, double density)
  {
  //
  // specific isochoric heat capacity in region 3
  // cvreg3 in kJ/(kg K)
  // temperature in K
  // density in kg/m^3
  //
  double tau = tc_water / temperature;
  double delta = density / dc_water;
  double val = 0.001 * rgas_water * (-Sqr(tau) * fitautaureg3(tau, delta));
  //
  return val;
  }
//
//
//
// double spsoundreg3(double temperature, double density)
//
// speed of sound in region 3
// spsoundreg3 in m/s
// temperature in K
// density in kg/m^3
//
//    double tau = tc_water / temperature
//    double delta = density / dc_water
//    double spsoundreg3 = (rgas_water * temperature * (2 * delta * fideltareg3(tau, delta) + delta ^ 2 * fideltadeltareg3(tau, delta) - (delta * fideltareg3(tau, delta) - delta * tau * fideltataureg3(tau, delta)) ^ 2 / (tau ^ 2 * fitautaureg3(tau, delta)))) ^ 0.5
//
//  return val;
//}
//
//
//
double CIF97Calcs::densreg3(double temperature, double pressure)
  {
  //
  // Determine density in region 3 iteratively using Newton method
  // densreg3 in kg/m^3
  // temperature in K
  // pressure in bar
  //
  // densreg3 = -2: not converged
  //
  double densold;
  if ( (temperature < tc_water && pressure < pSatW(temperature)) )
    {
    densold = 100.0;
    }
  else
    {
    densold = 600.0;
    }
  double tau = tc_water / temperature;
  //
  double val;
  double damp=1;
  for (int j = 1; j<1000; j++)
    {
    double delta = densold / dc_water;
    double fd1 = fideltareg3(tau, delta);
    double fd2 = fideltadeltareg3(tau, delta);
    double derivprho = rgas_water * temperature / dc_water * (2.0 * densold * fd1 + Sqr(densold) / dc_water * fd2);
    double densnew = densold + damp*(pressure * 100000.0 - rgas_water * temperature * Sqr(densold) / dc_water * fd1) / derivprho;
    //double derivprho = rgas_water * temperature / dc_water * (2 * densold * fideltareg3(tau, delta) + Sqr(densold) / dc_water * fideltadeltareg3(tau, delta));
    //double densnew = densold + (pressure * 100000.0 - rgas_water * temperature * Sqr(densold) / dc_water * fideltareg3(tau, delta)) / derivprho;
    double diffdens = fabs(densnew - densold);
    if ( densnew < 0.0)
      {
      densnew = 400;
      damp = damp * 0.9;
	  }
	else if ( diffdens < 0.000005 )
      {
      return densnew;
      }
    densold = densnew;
    }
  val = -2.0;

  //
  return val;
  }
//
//
//
double CIF97Calcs::densW(double temperature, double pressure)
  {
  //
  // density of water or steam
  // densW in kg/m^3
  // temperature in K
  // pressure in bar
  //
  // densW = -1: temperature and/or pressure outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 && pressure >= pSatW(temperature) && pressure <= 1000.0 )
    {
    //  region 1
    val = 1 / volreg1(temperature, pressure);
    }
  else if ( (temperature >= 273.15 && temperature <= 623.15 && pressure > 0 && pressure <= pSatW(temperature)) || (temperature >= 623.15 && temperature <= 863.15 && pressure > 0 && pressure <= pBound(temperature)) || (temperature >= 863.15 && temperature <= 1073.15 && pressure > 0 && pressure <= 1000.0) )
    {
    //  region 2
    val = 1 / volreg2(temperature, pressure);
    }
  else if ( temperature >= 623.15 && temperature <= tBound(pressure) && pressure >= pBound(temperature) && pressure <= 1000.0 )
    {
    //  region 3
    val = densreg3(temperature, pressure);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::energyW(double temperature, double pressure)
  {
  //
  // specific internal energy of water or steam
  // energyW in kJ/kg
  // temperature in K
  // pressure in bar
  //
  // energyW = -1: temperature and/or pressure outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 && pressure >= pSatW(temperature) && pressure <= 1000.0 )
    {
    //  region 1
    val = energyreg1(temperature, pressure);
    }
  else if ( (temperature >= 273.15 && temperature <= 623.15 && pressure > 0 && pressure <= pSatW(temperature)) || (temperature >= 623.15 && temperature <= 863.15 && pressure > 0 && pressure <= pBound(temperature)) || (temperature >= 863.15 && temperature <= 1073.15 && pressure > 0 && pressure <= 1000.0) )
    {
    //  region 2
    val = energyreg2(temperature, pressure);
    }
  else if ( temperature >= 623.15 && temperature <= tBound(pressure) && pressure >= pBound(temperature) && pressure <= 1000.0 )
    {
    //  region 3
    double density = densreg3(temperature, pressure);
    val = energyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::entropyW(double temperature, double pressure)
  {
  //
  // specific entropy of water or steam
  // entropyW in kJ/(kg K)
  // temperature in K
  // pressure in bar
  //
  // entropyW = -1: temperature and/or pressure outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 && pressure >= pSatW(temperature) && pressure <= 1000.0 )
    {
    //  region 1
    val = entropyreg1(temperature, pressure);
    }
  else if ( (temperature >= 273.15 && temperature <= 623.15 && pressure > 0 && pressure <= pSatW(temperature)) || (temperature >= 623.15 && temperature <= 863.15 && pressure > 0 && pressure <= pBound(temperature)) || (temperature >= 863.15 && temperature <= 1073.15 && pressure > 0 && pressure <= 1000.0) )
    {
    //  region 2
    val = entropyreg2(temperature, pressure);
    }
  else if ( temperature >= 623.15 && temperature <= tBound(pressure) && pressure >= pBound(temperature) && pressure <= 1000.0 )
    {
    //  region 3
    double density = densreg3(temperature, pressure);
    val = entropyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::enthalpyW(double temperature, double pressure)
  {
  //
  // specific enthalpy of water or steam
  // enthalpyW in kJ/kg
  // temperature in K
  // pressure in bar
  //
  // enthalpyW = -1: temperature and/or pressure outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 && pressure >= pSatW(temperature) && pressure <= 1000.0 )
    {
    //  region 1
    val = enthalpyreg1(temperature, pressure);
    }
  else if ( (temperature >= 273.15 && temperature <= 623.15 && pressure > 0 && pressure <= pSatW(temperature)) || (temperature >= 623.15 && temperature <= 863.15 && pressure > 0 && pressure <= pBound(temperature)) || (temperature >= 863.15 && temperature <= 1073.15 && pressure > 0 && pressure <= 1000.0) )
    {
    //  region 2
    val = enthalpyreg2(temperature, pressure);
    }
  else if ( temperature >= 623.15 && temperature <= tBound(pressure) && pressure >= pBound(temperature) && pressure <= 1000.0 )
    {
    //  region 3
    double density = densreg3(temperature, pressure);
    val = enthalpyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::cpW(double temperature, double pressure)
  {
  //
  // specific isobaric heat capacity of water or steam
  // cpW in kJ/(kg K)
  // temperature in K
  // pressure in bar
  //
  // cpW = -1: temperature and/or pressure outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 && pressure >= pSatW(temperature) && pressure <= 1000.0 )
    {
    //  region 1
    val = cpreg1(temperature, pressure);
    }
  else if ( (temperature >= 273.15 && temperature <= 623.15 && pressure > 0 && pressure <= pSatW(temperature)) || (temperature >= 623.15 && temperature <= 863.15 && pressure > 0 && pressure <= pBound(temperature)) || (temperature >= 863.15 && temperature <= 1073.15 && pressure > 0 && pressure <= 1000.0) )
    {
    //  region 2
    val = cpreg2(temperature, pressure);
    }
  else if ( temperature >= 623.15 && temperature <= tBound(pressure) && pressure >= pBound(temperature) && pressure <= 1000.0 )
    {
    //  region 3
    double density = densreg3(temperature, pressure);
    val = cpreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::cvW(double temperature, double pressure)
  {
  //
  // specific isochoric heat capacity of water or steam
  // cvW in kJ/(kg K)
  // temperature in K
  // pressure in bar
  //
  // cvW = -1: temperature and/or pressure outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 && pressure >= pSatW(temperature) && pressure <= 1000.0 )
    {
    //  region 1
    val = cvreg1(temperature, pressure);
    }
  else if ( (temperature >= 273.15 && temperature <= 623.15 && pressure > 0 && pressure <= pSatW(temperature)) || (temperature >= 623.15 && temperature <= 863.15 && pressure > 0 && pressure <= pBound(temperature)) || (temperature >= 863.15 && temperature <= 1073.15 && pressure > 0 && pressure <= 1000.0) )
    {
    //  region 2
    val = cvreg2(temperature, pressure);
    }
  else if ( temperature >= 623.15 && temperature <= tBound(pressure) && pressure >= pBound(temperature) && pressure <= 1000.0 )
    {
    //  region 3
    double density = densreg3(temperature, pressure);
    val = cvreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
// double spsoundW(double temperature, double pressure)
//
// speed of sound in water or steam
// spsoundW in m/s
// temperature in K
// pressure in bar
//
// spsoundW = -1: temperature and/or pressure outside range
//
    double val;
//    if ( temperature >= 273.15 && temperature <= 623.15 && pressure >= pSatW(temperature) && pressure <= 1000.0 )
//    {
//  region 1
//        spsoundW = spsoundreg1(temperature, pressure)
//    }
//    else if ( (temperature >= 273.15 && temperature <= 623.15 && pressure > 0 && pressure <= pSatW(temperature)) || (temperature >= 623.15 && temperature <= 863.15 && pressure > 0 && pressure <= pBound(temperature)) || (temperature >= 863.15 && temperature <= 1073.15 && pressure > 0 && pressure <= 1000.0) )
//    {
//  region 2
//        spsoundW = spsoundreg2(temperature, pressure)
//    }
//    else if ( temperature >= 623.15 && temperature <= tBound(pressure) && pressure >= pBound(temperature) && pressure <= 1000.0 )
//    {
//  region 3
//        density = densreg3(temperature, pressure)
//        spsoundW = spsoundreg3(temperature, density)
//    }
//    else
//  {
//  outside range
//        spsoundW = -1.0
//      }
//
//  return val;
//}
//
//
//
double CIF97Calcs::viscW(double temperature, double pressure)
  {
  //
  // dynamic viscosity of water or steam
  // viscW in Pa s
  // temperature in K
  // pressure in bar
  //
  // viscW = -1: temperature and/or pressure outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 1073.15 && pressure > 0 && pressure <= 1000.0 )
    {
    double density = densW(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.000055071 * psivisc(tau, delta);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::thconW(double temperature, double pressure)
  {
  //
  // thermal conductivity of water or steam
  // thconW in W/(m K)
  // temperature in K
  // pressure in bar
  //
  // thconW = -1: temperature and/or pressure outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 1073.15 && pressure > 0 && pressure <= 1000.0 )
    {
    double density = densW(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.4945 * lambthcon(temperature, pressure, tau, delta);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::densSatLiqTW(double temperature)
  {
  //
  // density of saturated liquid water as a function of temperature
  // densSatLiqTW in kg/m^3
  // temperature in K
  //
  // densSatLiqTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 1
    double pressure = pSatW(temperature);
    val = 1 / volreg1(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature);
    val = densreg3(temperature, pressure);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::densSatVapTW(double temperature)
  {
  //
  // density of saturated steam as a function of temperature
  // densSatVapTW in kg/m^3
  // temperature in K
  //
  // densSatVapTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 2
    double pressure = pSatW(temperature);
    val = 1 / volreg2(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature) - 0.00001;
    val = densreg3(temperature, pressure);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::densSatLiqPW(double pressure)
  {
  //
  // density of saturated liquid water as a function of pressure
  // densSatLiqPW in kg/m^3
  // pressure in bar
  //
  // densSatLiqPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 1
    double temperature = tSatW(pressure);
    val = 1 / volreg1(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure + 0.00001;
    val = densreg3(temperature, pressure);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::densSatVapPW(double pressure)
  {
  //
  // density of saturated steam as a function of pressure
  // densSatVapPW in kg/m^3
  // pressure in bar
  //
  // densSatVapPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 2
    double temperature = tSatW(pressure);
    val = 1 / volreg2(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure - 0.00001;
    val = densreg3(temperature, pressure);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::energySatLiqTW(double temperature)
  {
  //
  // specific internal energy of saturated liquid water as a function of temperature
  // energySatLiqTW in kJ/kg
  // temperature in K
  //
  // energySatLiqTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 1
    double pressure = pSatW(temperature);
    val = energyreg1(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature);
    double density = densreg3(temperature, pressure);
    val = energyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::energySatVapTW(double temperature)
  {
  //
  // specific internal energy of saturated steam as a function of temperature
  // energySatVapTW in kJ/kg
  // temperature in K
  //
  // energySatVapTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 2
    double pressure = pSatW(temperature);
    val = energyreg2(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature) - 0.00001;
    double density = densreg3(temperature, pressure);
    val = energyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::energySatLiqPW(double pressure)
  {
  //
  // specific internal energy of saturated liquid water as a function of pressure
  // energySatLiqPW in kJ/kg
  // pressure in bar
  //
  // energySatLiqPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 1
    double temperature = tSatW(pressure);
    val = energyreg1(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure + 0.00001;
    double density = densreg3(temperature, pressure);
    val = energyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::energySatVapPW(double pressure)
  {
  //
  // specific internal energy of saturated steam as a function of pressure
  // energySatVapPW in kJ/kg
  // pressure in bar
  //
  // energySatVapPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 2
    double temperature = tSatW(pressure);
    val = energyreg2(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure - 0.00001;
    double density = densreg3(temperature, pressure);
    val = energyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::entropySatLiqTW(double temperature)
  {
  //
  // specific entropy of saturated liquid water as a function of temperature
  // entropySatLiqTW in kJ/(kg K)
  // temperature in K
  //
  // entropySatLiqTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 1
    double pressure = pSatW(temperature);
    val = entropyreg1(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature);
    double density = densreg3(temperature, pressure);
    val = entropyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::entropySatVapTW(double temperature)
  {
  //
  // specific entropy of saturated steam as a function of temperature
  // entropySatVapTW in kJ/(kg K)
  // temperature in K
  //
  // entropySatVapTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 2
    double pressure = pSatW(temperature);
    val = entropyreg2(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature) - 0.00001;
    double density = densreg3(temperature, pressure);
    val = entropyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::entropySatLiqPW(double pressure)
  {
  //
  // specific entropy of saturated liquid water as a function of pressure
  // entropySatLiqPW in kJ/(kg K)
  // pressure in bar
  //
  // entropySatLiqPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 1
    double temperature = tSatW(pressure);
    val = entropyreg1(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure + 0.00001;
    double density = densreg3(temperature, pressure);
    val = entropyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::entropySatVapPW(double pressure)
  {
  //
  // specific entropy of saturated steam as a function of pressure
  // entropySatVapPW in kJ/(kg K)
  // pressure in bar
  //
  // entropySatVapPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 2
    double temperature = tSatW(pressure);
    val = entropyreg2(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure - 0.00001;
    double density = densreg3(temperature, pressure);
    val = entropyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::enthalpySatLiqTW(double temperature)
  {
  //
  // specific enthalpy of saturated liquid water as a function of temperature
  // enthalpySatLiqTW in kJ/kg
  // temperature in K
  //
  // enthalpySatLiqTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 1
    double pressure = pSatW(temperature);
    val = enthalpyreg1(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature);
    double density = densreg3(temperature, pressure);
    val = enthalpyreg3(temperature, density);
    }
  else
    {
    //  outside range
    double val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::enthalpySatVapTW(double temperature)
  {
  //
  // specific enthalpy of saturated steam as a function of temperature
  // enthalpySatVapTW in kJ/kg
  // temperature in K
  //
  // enthalpySatVapTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 2
    double pressure = pSatW(temperature);
    val = enthalpyreg2(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature) - 0.00001;
    double density = densreg3(temperature, pressure);
    val = enthalpyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::enthalpySatLiqPW(double pressure)
  {
  //
  // specific enthalpy of saturated liquid water as a function of pressure
  // enthalpySatLiqPW in kJ/kg
  // pressure in bar
  //
  // enthalpySatLiqPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 1
    double temperature = tSatW(pressure);
    val = enthalpyreg1(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure + 0.00001;
    double density = densreg3(temperature, pressure);
    val = enthalpyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::enthalpySatVapPW(double pressure)
  {
  //
  // specific enthalpy of saturated steam as a function of pressure
  // enthalpySatVapPW in kJ/kg
  // pressure in bar
  //
  // enthalpySatVapPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 2
    double temperature = tSatW(pressure);
    val = enthalpyreg2(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure - 0.00001;
    double density = densreg3(temperature, pressure);
    val = enthalpyreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::cpSatLiqTW(double temperature)
  {
  //
  // specific isobaric heat capacity of saturated liquid water as a function of temperature
  // cpSatLiqTW in kJ/(kg K)
  // temperature in K
  //
  // cpSatLiqTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 1
    double pressure = pSatW(temperature);
    val = cpreg1(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature);
    double density = densreg3(temperature, pressure);
    val = cpreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::cpSatVapTW(double temperature)
  {
  //
  // specific isobaric heat capacity of saturated steam as a function of temperature
  // cpSatVapTW in kJ/(kg K)
  // temperature in K
  //
  // cpSatVapTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 2
    double pressure = pSatW(temperature);
    val = cpreg2(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature) - 0.00001;
    double density = densreg3(temperature, pressure);
    val = cpreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::cpSatLiqPW(double pressure)
  {
  //
  // specific isobaric heat capacity of saturated liquid water as a function of pressure
  // cpSatLiqPW in kJ/(kg K)
  // pressure in bar
  //
  // cpSatLiqPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 1
    double temperature = tSatW(pressure);
    val = cpreg1(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure + 0.00001;
    double density = densreg3(temperature, pressure);
    val = cpreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
    //
  return val;
  }
//
//
//
double CIF97Calcs::cpSatVapPW(double pressure)
  {
  //
  // specific isobaric heat capacity of saturated steam as a function of pressure
  // cpSatVapPW in kJ/(kg K)
  // pressure in bar
  //
  // cpSatVapPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 2
    double temperature = tSatW(pressure);
    val = cpreg2(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure - 0.00001;
    double density = densreg3(temperature, pressure);
    val = cpreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::cvSatLiqTW(double temperature)
  {
  //
  // specific isochoric heat capacity of saturated liquid water as a function of temperature
  // cvSatLiqTW in kJ/(kg K)
  // temperature in K
  //
  // cvSatLiqTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 1
    double pressure = pSatW(temperature);
    val = cvreg1(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature);
    double density = densreg3(temperature, pressure);
    val = cvreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::cvSatVapTW(double temperature)
  {
  //
  // specific isochoric heat capacity of saturated steam as a function of temperature
  // cvSatVapTW in kJ/(kg K)
  // temperature in K
  //
  // cvSatVapTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 2
    double pressure = pSatW(temperature);
    val = cvreg2(temperature, pressure);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature) - 0.00001;
    double  density = densreg3(temperature, pressure);
    val = cvreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::cvSatLiqPW(double pressure)
  {
  //
  // specific isochoric heat capacity of saturated liquid water as a function of pressure
  // cvSatLiqPW in kJ/(kg K)
  // pressure in bar
  //
  // cvSatLiqPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 1
    double temperature = tSatW(pressure);
    val = cvreg1(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure + 0.00001;
    double density = densreg3(temperature, pressure);
    val = cvreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::cvSatVapPW(double pressure)
  {
  //
  // specific isochoric heat capacity of saturated steam as a function of pressure
  // cvSatVapPW in kJ/(kg K)
  // pressure in bar
  //
  // cvSatVapPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 2
    double temperature = tSatW(pressure);
    val = cvreg2(temperature, pressure);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure - 0.00001;
    double density = densreg3(temperature, pressure);
    val = cvreg3(temperature, density);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
    //
  return val;
  }
//
//
//
// double spsoundSatLiqTW(double temperature)
//
// speed of sound in saturated liquid water as a function of temperature
// spsoundSatLiqTW in m/s
// temperature in K
//
// spsoundSatLiqTW = -1: temperature outside range
//
//    if ( temperature >= 273.15 && temperature <= 623.15 )
//    {
//  region 1
//        pressure = pSatW(temperature)
//        spsoundSatLiqTW = spsoundreg1(temperature, pressure)
//    }
//    else if ( temperature > 623.15 && temperature <= tc_water )
//    {
//  region 3
//        pressure = pSatW(temperature)
//        density = densreg3(temperature, pressure)
//        spsoundSatLiqTW = spsoundreg3(temperature, density)
//    }
//    else
//  {
//  outside range
//        spsoundSatLiqTW = -1.0
//      }
//
//  return val;
//}
//
//
//
// double spsoundSatVapTW(double temperature)
//
// speed of sound in saturated steam as a function of temperature
// spsoundSatVapTW in m/s
// temperature in K
//
// spsoundSatVapTW = -1: temperature outside range
//
//    if ( temperature >= 273.15 && temperature <= 623.15 )
//  {
//  region 2
//        pressure = pSatW(temperature)
//        spsoundSatVapTW = spsoundreg2(temperature, pressure)
//  }
//    else if ( temperature > 623.15 && temperature <= tc_water )
//  {
//  region 3
//        pressure = pSatW(temperature) - 0.00001
//        density = densreg3(temperature, pressure)
//        spsoundSatVapTW = spsoundreg3(temperature, density)
//    }
//  else
//    {
//  outside range
//        spsoundSatVapTW = -1.0
//      }
//
//  return val;
//}
//
//
//
// double spsoundSatLiqPW(double pressure)
//
// speed of sound in saturated liquid water as a function of pressure
// spsoundSatLiqPW in m/s
// pressure in bar
//
// spsoundSatLiqPW = -1: pressure outside range
//
//    if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
//    {
//  region 1
//        temperature = tSatW(pressure)
//        spsoundSatLiqPW = spsoundreg1(temperature, pressure)
//    }
//    else if ( pressure > pSatW(623.15) && pressure <= pc_water )
//    {
//  region 3
//        temperature = tSatW(pressure)
//        pressure = pressure + 0.00001
//        density = densreg3(temperature, pressure)
//        spsoundSatLiqPW = spsoundreg3(temperature, density)
//    }
//    else
//      {
//  outside range
//        spsoundSatLiqPW = -1.0
//      }
//
//  return val;
//}
//
//
//
// double spsoundSatVapPW(double pressure)
//
// speed of sound in saturated steam as a function of pressure
// spsoundSatVapPW in m/s
// pressure in bar
//
// spsoundSatVapPW = -1: pressure outside range
//
//    if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
//      {
//  region 2
//        temperature = tSatW(pressure)
//        spsoundSatVapPW = spsoundreg2(temperature, pressure)
//      }
//    else if ( pressure > pSatW(623.15) && pressure <= pc_water )
//      {
//  region 3
//        temperature = tSatW(pressure)
//        pressure = pressure - 0.00001
//        density = densreg3(temperature, pressure)
//        spsoundSatVapPW = spsoundreg3(temperature, density)
//    }
//      else
//        {
//  outside range
//        spsoundSatVapPW = -1.0
//      }
//
//  return val;
//}
//
//
//
double CIF97Calcs::viscSatLiqTW(double temperature)
  {
  //
  // dynamic viscosity of saturated liquid water as a function of temperature
  // viscSatLiqTW in Pa s
  // temperature in K
  //
  // viscSatLiqTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 1
    double pressure = pSatW(temperature);
    double density = 1 / volreg1(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.000055071 * psivisc(tau, delta);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature);
    double density = densreg3(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.000055071 * psivisc(tau, delta);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::viscSatVapTW(double temperature)
  {
  //
  // dynamic viscosity of saturated steam as a function of temperature
  // viscSatVapTW in Pa s
  // temperature in K
  //
  // viscSatVapTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 2
    double pressure = pSatW(temperature);
    double density = 1 / volreg2(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.000055071 * psivisc(tau, delta);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature) - 0.00001;
    double density = densreg3(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.000055071 * psivisc(tau, delta);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::viscSatLiqPW(double pressure)
  {
  //
  // dynamic viscosity of saturated liquid water as a function of pressure
  // viscSatLiqPW in Pa s
  // pressure in bar
  //
  // viscSatLiqPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 1
    double temperature = tSatW(pressure);
    double density = 1 / volreg1(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.000055071 * psivisc(tau, delta);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure + 0.00001;
    double density = densreg3(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.000055071 * psivisc(tau, delta);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::viscSatVapPW(double pressure)
  {
  //
  // dynamic viscosity of saturated steam as a function of pressure
  // viscSatVapPW in Pa s
  // pressure in bar
  //
  // viscSatVapPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 2
    double temperature = tSatW(pressure);
    double density = 1 / volreg2(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.000055071 * psivisc(tau, delta);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure - 0.00001;
    double density = densreg3(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.000055071 * psivisc(tau, delta);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::thconSatLiqTW(double temperature)
  {
  //
  // thermal conductivity of saturated liquid water as a function of temperature
  // thconSatLiqTW in W /(m K)
  // temperature in K
  //
  // thconSatLiqTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 1
    double pressure = pSatW(temperature);
    double density = 1 / volreg1(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.4945 * lambthcon(temperature, pressure, tau, delta);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature);
    double density = densreg3(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.4945 * lambthcon(temperature, pressure, tau, delta);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
    //
  return val;
  }
//
//
//
double CIF97Calcs::thconSatVapTW(double temperature)
  {
  //
  // thermal conductivity of saturated steam as a function of temperature
  // thconSatVapTW in W /(m K)
  // temperature in K
  //
  // thconSatVapTW = -1: temperature outside range
  //
  double val;
  if ( temperature >= 273.15 && temperature <= 623.15 )
    {
    //  region 2
    double pressure = pSatW(temperature);
    double density = 1 / volreg2(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    pressure = pressure - 0.0001 * pressure;
    val = 0.4945 * lambthcon(temperature, pressure, tau, delta);
    }
  else if ( temperature > 623.15 && temperature <= tc_water )
    {
    //  region 3
    double pressure = pSatW(temperature) - 0.00001;
    double density = densreg3(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.4945 * lambthcon(temperature, pressure, tau, delta);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }
//
//
//
double CIF97Calcs::thconSatLiqPW(double pressure)
  {
  //
  // thermal conductivity of saturated liquid water as a function of pressure
  // thconSatLiqPW in W /(m K)
  // pressure in bar
  //
  // thconSatLiqPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 1
    double temperature = tSatW(pressure);
    double density = 1 / volreg1(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.4945 * lambthcon(temperature, pressure, tau, delta);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure + 0.00001;
    double density = densreg3(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.4945 * lambthcon(temperature, pressure, tau, delta);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
    //
  return val;
  }
//
//
//
double CIF97Calcs::thconSatVapPW(double pressure)
  {
  //
  // thermal conductivity of saturated steam as a function of pressure
  // thconSatVapPW in W /(m K)
  // pressure in bar
  //
  // thconSatVapPW = -1: pressure outside range
  //
  double val;
  if ( pressure >= pSatW(273.15) && pressure <= pSatW(623.15) )
    {
    //  region 2
    double temperature = tSatW(pressure);
    double density = 1 / volreg2(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    pressure = pressure - 0.0001 * pressure;
    val = 0.4945 * lambthcon(temperature, pressure, tau, delta);
    }
  else if ( pressure > pSatW(623.15) && pressure <= pc_water )
    {
    //  region 3
    double temperature = tSatW(pressure);
    pressure = pressure - 0.00001;
    double density = densreg3(temperature, pressure);
    double delta = density / 317.763;
    double tau = 647.226 / temperature;
    val = 0.4945 * lambthcon(temperature, pressure, tau, delta);
    }
  else
    {
    //  outside range
    val = -1.0;
    }
  //
  return val;
  }


static CIF97Calcs IF97Calcs;
bool DBDataRaw=false;

// ==========================================================================
//
//
//
// ===========================================================================

const double VapTempBand=0.1;
static bool dbgit=false;

static bool IsLiq(double T, double P)
  {
  return (T>= 273.15 && T <= tc_water && P>= IF97Calcs.pSatW(T));
  }
 
static double WtrH(double T, double P)
  {
  if (!DBDataRaw)
    {
    double HiTempCorr=0;
    if (T > 1073.15)
      {
      HiTempCorr = IF97Calcs.cpW(1073.15, P) * (T - 1073.15);
      T = 1073.15;
      }
    double TIn = T;
    T = Max(T, 273.15);

    if (!IsLiq(T, P) && P < pc_water)// && T < tc_water)
      {
      //' assume in steam region
      double HSatLiq = IF97Calcs.enthalpySatLiqPW(P);
      double CpSatLiq = IF97Calcs.cpSatLiqPW(P);
      double HSteam = IF97Calcs.enthalpyW(T, P);
//'      if TIn < T 
//'        HSteam = enthalpySatVapPW(P) + cpW(273.15, P) * (TIn - 273.15)
      
      return Min(HSatLiq + CpSatLiq * (T - IF97Calcs.tSatW(P)), HSteam) + HiTempCorr;
      }
    else
      {
      double LoTFix = (TIn < T) ? LoTFix = IF97Calcs.cpW(T, P) * (TIn - T) : 0;
      return IF97Calcs.enthalpyW(T, P) + HiTempCorr + LoTFix;
      }



//    double HiTempCorr=0;
//    if (T > 1073.15)
//      {
//      HiTempCorr = IF97Calcs.cpW(1073.15, P) * (T - 1073.15);
//      T = 1073.15;
//      }
//    if (!IsLiq(T, P) && P < pc_water && T < tc_water )
//      {
////    ' assume in steam region
//      double HSatLiq = IF97Calcs.enthalpySatLiqPW(P);
//      double CpSatLiq = IF97Calcs.cpSatLiqPW(P);
//      return Min(HSatLiq + CpSatLiq * (T - IF97Calcs.tSatW(P)), IF97Calcs.enthalpyW(T, P)) + HiTempCorr;
//      }
//    else
//      return IF97Calcs.enthalpyW(T, P) + HiTempCorr ;

    //if (dbgit)
    //  {
    //  double H=IF97Calcs.enthalpyW(T, P);
    //  dbgpln("WtrH1 == (%8.3f,%8.3f) %12.3f : %12.3f %12.3f %12.3f",T,P, H - Lat + HiTempCorr, H , Lat , HiTempCorr);
    //  }
    //return IF97Calcs.enthalpyW(T, P) - Lat + HiTempCorr;
    }
  return IF97Calcs.enthalpyW(T,P);
  }

static double StmCpRgn5_KW(double T, double P)
  {
  // Cp Eqn - Rod Stephenson
  double Cp = 1.47951179E-18 * Pow(T, 6) - 
              9.42330592E-15 * Pow(T, 5) + 
              2.47275340E-11 * Pow(T, 4) - 
              3.42612967E-08 * Pow(T, 3) + 
              2.65138336E-05 * Pow(T, 2) - 
              1.02119293E-02 * T + 
              3.46562048E+00; 
  return Cp;            
  }
static double StmHRgn5_KW(double T, double P)
  {
  // Cp Eqn - Rod Stephenson
  double H = IF97Calcs.enthalpyW(1073.15, P) +
    
       1.47951179E-18 * (Pow(T, 7) - Pow(1073.15, 7))/7.0 - 
       9.42330592E-15 * (Pow(T, 6) - Pow(1073.15, 6))/6.0 + 
       2.47275340E-11 * (Pow(T, 5) - Pow(1073.15, 5))/5.0 - 
       3.42612967E-08 * (Pow(T, 4) - Pow(1073.15, 4))/4.0 + 
       2.65138336E-05 * (Pow(T, 3) - Pow(1073.15, 3))/3.0 - 
       1.02119293E-02 * (Pow(T, 2) - Pow(1073.15, 2))/2.0 + 
       3.46562048E+00 * (T-1073.15);

  return H;                                                          
  }                                                                  
static double StmSRgn5_KW(double T, double P)                        
  {                                                                  
  // Cp Eqn - Rod Stephenson                                         
  double S = IF97Calcs.entropyW(1073.15, P) +
    
       1.47951179E-18 * (Pow(T, 6) - Pow(1073.15, 6)) - 
       9.42330592E-15 * (Pow(T, 5) - Pow(1073.15, 5)) + 
       2.47275340E-11 * (Pow(T, 4) - Pow(1073.15, 4)) - 
       3.42612967E-08 * (Pow(T, 3) - Pow(1073.15, 3)) + 
       2.65138336E-05 * (Pow(T, 2) - Pow(1073.15, 2)) - 
       1.02119293E-02 * (T - 1073.15) + 
       3.46562048E+00 * log(T/1073.15);

  return S;
  }
static double StmDensRgn5_KW(double T, double P)
  {
  // 100 is to compensate for *0.01 in IF97_SteamDensity::AtTP
  double Dens = (100.0*P)*H20MoleWt/GTZ(8.314*T);
  //const double Dens = (100.0*P)*H20MoleWt/GTZ(R_c*T);
  return Dens;
  }

static double StmH(double T, double P)
  {
//  dbgpln("StmH: %16.8f %16.8f", T, P);
  
  if (!DBDataRaw)
    {
//    if (T > 1073.15)
//      {
//      if (dbgit)
//        {
//        double H=IF97Calcs.enthalpyW(1073.15, P);
//        double X=IF97Calcs.cpW(1073.15, P) * (T - 1073.15);
//
//        dbgpln("StmH1    (%8.3f,%8.3f) %12.3f : %12.3f %12.3f",T,P, H +X, H,X);
//        }
//      return IF97Calcs.enthalpyW(1073.15, P) + IF97Calcs.cpW(1073.15, P) * (T - 1073.15);
//      }
//    else if (IsLiq(T, P) && P < pc_water && T < tc_water )
//      {
//      //' assume in water region
//      double HSatVap = IF97Calcs.enthalpySatVapPW(P);
//      double CpSatVap = IF97Calcs.cpSatVapPW(P);
//      return Max(HSatVap + CpSatVap * (T - IF97Calcs.tSatW(P)), IF97Calcs.enthalpyW(T, P));
//      }
//    else
//      return IF97Calcs.enthalpyW(T, P);

    if (T > 1073.15)
      return StmHRgn5_KW(T,P);
      //return IF97Calcs.enthalpyW(1073.15, P) + IF97Calcs.cpW(1073.15, P) * (T - 1073.15);
    else
      {
      double TIn = T;
      T = Max(T, 273.15);
    
      if (IsLiq(T, P) && P < pc_water && T < tc_water)
        {// assume in water region
        double HSatVap = IF97Calcs.enthalpySatVapPW(P);
        double CpSatVap = IF97Calcs.cpSatVapPW(P);
        double HWater = IF97Calcs.enthalpyW(T, P);
        if (TIn < T)
          HWater += IF97Calcs.cpW(273.15, P) * (TIn - 273.15);
        return Max(HSatVap + CpSatVap * (Min(T, TIn) - IF97Calcs.tSatW(P)), HWater);
        }
      else
        {
        double LoTFix = (TIn < T) ? IF97Calcs.cpW(T, P) * (TIn - T) : 0;
        return IF97Calcs.enthalpyW(T, P) + LoTFix;
        }
      }


//      double Lat = 0;
//      double TSat = IF97Calcs.tSatW(P);
//      if (IsLiq(T,P))//T <= TSat || TSat<0) // assume in water region
//        {
//        Lat = IF97Calcs.enthalpySatVapTW(T) - IF97Calcs.enthalpySatLiqTW(T);
//        if (dbgit)
//          {
//          double H=IF97Calcs.enthalpyW(T, P);
//          dbgpln("StmH2    (%8.3f,%8.3f) %12.3f : %12.3f %12.3f",T,P, H +Lat, H,Lat);
//          }
//        return IF97Calcs.enthalpyW(T, P) + Lat;
//        }
//      else
//        {
//        if (dbgit)
//          {
//          double H=IF97Calcs.enthalpyW(T, P);
//          dbgpln("StmH3    (%8.3f,%8.3f) %12.3f",T,P, H);
//          }
//        return IF97Calcs.enthalpyW(T, P);
//        }
//      }
    }
  return IF97Calcs.enthalpyW(T,P);
  }

static double StmCp(double T, double P)
  {
  if (!DBDataRaw)
    {
    return StmH(T+0.5,P)-StmH(T-0.5,P);
    }
  return IF97Calcs.cpW(T,P);
  }
static double WtrCp(double T, double P)
  {
  if (!DBDataRaw)
    {
    return WtrH(T+0.5,P)-WtrH(T-0.5,P);
    }
  return IF97Calcs.cpW(T,P);
  }

static double WtrS(double T, double P)
  {
  if (!DBDataRaw)
    {
    double HiTempCorr=0;
    if (T > 1073.15)
      {
      HiTempCorr = (IF97Calcs.cpW(1073.15, P) / T) * (T - 1073.15);
      T = 1073.15;
      }

//    double Lat=0, TSat=0;
//    TSat = IF97Calcs.tSatW(P);
//    if (T > TSat || TSat < 0) // ' assume in steam region
//      Lat = IF97Calcs.entropySatVapPW(P) - IF97Calcs.entropySatLiqPW(P);
//    return IF97Calcs.entropyW(T, P) - Lat + HiTempCorr;

    double TIn = T;
    T = Max(T, 273.15);

    if (!IsLiq(T, P) && P < pc_water)// && T < tc_water)
      {//  assume in steam region
      double TSat = IF97Calcs.tSatW(P);
      double HSatLiq = IF97Calcs.entropySatLiqPW(P);
      double CpSatLiq = IF97Calcs.cpSatLiqPW(P) / TSat;
      double SSteam = IF97Calcs.entropyW(T, P);
      return Min(HSatLiq + CpSatLiq * (T - TSat), SSteam) + HiTempCorr;
      }
    else
      {
      double LoTFix = (TIn < T) ? IF97Calcs.cpW(T, P) / T * (TIn - T) : 0;
      return IF97Calcs.entropyW(T, P) + HiTempCorr + LoTFix;
      }

    }
  return IF97Calcs.entropyW(T,P);
  }
static double StmS(double T, double P)
  {
  if (!DBDataRaw)
    {
    if (T > 1073.15)
      return StmSRgn5_KW(T, P);
      //return IF97Calcs.entropyW(1073.15, P) + (IF97Calcs.cpW(1073.15, P)/1073.16)*(T - 1073.15);
    else
      {
      double TIn=T;
      double LoTFix = 0;
      if (T < 273.15)
        {
        T=273.15;
        LoTFix = IF97Calcs.cpW(T, P) / T * (TIn - T);
        }
    
      if (IsLiq(T, P) &&  P < pc_water && T < tc_water )
        {//  ' assume in water region
        double HSatVap = IF97Calcs.entropySatVapPW(P);
        double CpSatVap = IF97Calcs.cpSatVapPW(P);
        double TSat = IF97Calcs.tSatW(P);
        double HWater = IF97Calcs.entropyW(T, P) + LoTFix;
        return Max(HSatVap + CpSatVap / TSat * (Min(T, TIn) - TSat), HWater);
        }
      else
        return IF97Calcs.entropyW(T, P) + LoTFix;
      }
    }
  return IF97Calcs.entropyW(T,P);
  }

static double WtrDens(double T, double P)
  {
  if (!DBDataRaw)
    {
    //
    // density of water or steam
    // densW in kg/m^3
    // temperature in K
    // pressure in bar
    //
    // densW = -1: temperature and/or pressure outside range
    //
    double val,val1, Ti=T, Pi=P, PSat=IF97Calcs.pSatW(T);
    P = Min(P, 1000.0);
    if ( T >= 273.15 && T <= 623.15 && P >= PSat && P <= 1000.0 )
      {
      //  region 1
      val = 1 / IF97Calcs.volreg1(T, P);
      }
    else if ( T < 273.15)
      {
      T=273.15;
      val = 1 / IF97Calcs.volreg1(T, P);
      val1 = 1 / IF97Calcs.volreg1(T+1, P);
      val -=(val-val1)*(Ti-T);
      }
    else if (T <= 623.15)// && P > 0 && P <= IF97Calcs.pSatW(T))
      {
      P=Min(P, PSat);
      val = 1 / IF97Calcs.volreg1(T, P);
      val1 = 1 / IF97Calcs.volreg1(T-1, P);
      val += (val-val1)*(Ti-T);
      }   
     
    else 
      {
      T=Min(T, 623.15);
      val = 1 / IF97Calcs.volreg1(T, P);
      //val1 = 1 / IF97Calcs.volreg1(T-1, P);
      //val += (val-val1)*(Ti-T);
      val1 = 0.1*val;
      val  += (val1-val)*(Ti-T)/(10000-T);
      }
    return Max(1.0,val);
/*
    P=Min(P, 1000.0);
    if (T > 1073.15)
      return IF97Calcs.densW(1073.15, P);
    else
      {
      T=Max(T, 273.15);
      //P=Max(P, IF97Calcs.pSatW(T));
      P=Range(0.00611213, P, 220.64);
      
      if (!IsLiq(T, P) && P < pc_water)// && T < tc_water)
        {
        //' assume in steam region
        P=Range(IF97Calcs.pSatW(T), P, 1000.0);
        double TSat = IF97Calcs.tSatW(P);
        double RhoSatLiq = IF97Calcs.densSatLiqPW(P);
        double RhoSatLiq1 = IF97Calcs.densW(TSat - 0.01, P);
        return Max(RhoSatLiq + 100 * (RhoSatLiq - RhoSatLiq1) * (T - TSat), IF97Calcs.densW(T, P)); 
        }
      else
        return IF97Calcs.densW(T, P);
      }
*/
    }
  return IF97Calcs.densW(T,P);
  }
static double StmDens(double T, double P)
  {
  if (!DBDataRaw)
    {
    //
    // density of water or steam
    // densW in kg/m^3
    // T in K
    // P in bar
    //
    // densW = -1: T and/or P outside range
    //
    double val, val1, Ti=T, Pi=P;
    double PSat=IF97Calcs.pSatW(T);
    double PBnd=IF97Calcs.pBound(T);

    if (T>1073.15)
      val=StmDensRgn5_KW(T,P);
    else
      {
      P = Min(P, 1000.0);
      T = Min(T, 1073.15);
      //if ( T >= 273.15 && T <= 623.15 && P >= pSatW(T) && P <= 1000.0 )
      //  {
      //  //  region 1
      //  val = 1 / volreg1(T, P);
      //  }
      //else 
      if ( (T >= 273.15 && T <= 623.15 && P > 0 && P <= PSat) || 
        (T >= 623.15 && T <= 863.15 && P > 0 && P <= PBnd) || 
        (T >= 863.15 && T <= 1073.15 && P > 0 && P <= 1000.0) )
        {
        //  region 2
        val = 1 / IF97Calcs.volreg2(T, P);
        if (Ti>1073.15)
          {
          val1 = 0.1*val;
          val  += (val1-val)*(Ti-T)/(10000-T);
          //        val1 = 1 / IF97Calcs.volreg2(T-1, P);
          //        val += (val-val1)*(Ti-T);
          }
        }
      else if ( T >= 623.15 && T <= IF97Calcs.tBound(P) && P >= PBnd && P <= 1000.0 )
        {
        //  region 3
        val = IF97Calcs.densreg3(T, P);
        }
      else if ( P >= IF97Calcs.pSatW(623.15)) 
        {
        //  adjacent to region 3
        val = IF97Calcs.densreg3(623.15, P);
        val1 = IF97Calcs.densreg3(623.15+1, P);
        val += (val1-val)*(Ti-623.15);
        }
      else //if ( P >= IF97Calcs.pSatW(623.15)) 
        {
        //  adjacent to region 2
        T=IF97Calcs.tSatW(P);
        if (T>0.0)
          {
          val = 1 / IF97Calcs.volreg2(T, P);
          val1 = 1 / IF97Calcs.volreg2(T+1, P);
          val += (val1-val)*(Ti-T);
          }
        else
          val = -1.0;//  outside range
        }
      //    else
      //      {
      //      //  outside range
      //      val = -1.0;
      //
      //      }
      //
      }
    return Min(2000.0, val);
/*
    P=Min(P, 1000.0);
    if (T >= 1073.15 )
      return IF97Calcs.densW(1073.15, P);
    else
      {
      T=Max(T,273.15);
      //P=Min(P, IF97Calcs.pSatW(T));
      P=Range(IF97Calcs.pSatW(T), P, 1000.0);
      if (IsLiq(T, P) && P <= pc_water)// && T <= tc_water)
        {
        //' assume in water region
        double TSat = IF97Calcs.tSatW(P);
        double RhoSatVap = IF97Calcs.densW(TSat + 0.01, P);
        double RhoSatVap1 = IF97Calcs.densSatVapPW(P);
        return Min(RhoSatVap1 + 100 * (RhoSatVap - RhoSatVap1) * (T - TSat), IF97Calcs.densW(T, P)); //' + HiTempCorr
        }
      else
        return IF97Calcs.densW(T, P);
      }
*/
    }
  return IF97Calcs.densW(T,P);
  }

static double VapP(double T)
  {
  if (T<273.15)
    return T/273.15*IF97Calcs.pSatW(273.15);
  
  T=Min(T, 647.096);
  return IF97Calcs.pSatW(T);
  }

static double VapT(double P)
  {
//  T=Range(273.15, T, 647.096);
  return IF97Calcs.tSatW(P);
  }

static CSurface3DBoundary SatLine(273.16, tc_water, VapP,
                                  0.001, pc_water, VapT);

static CSurface2D SfVapP("WaterVapourPressure",
                         273.16,  623.0, 1000, NULL, NULL,
                         VapP, SfXFLogIn, SfXFLogOut);

const double MinStmTemp = 10;
const double MaxStmTemp = 3000;
const double MinWtrTemp = 10;
const double MaxWtrTemp = 3000;
                         
static CSurface3D SfStmCp("SteamCp",
                         MinStmTemp , MaxStmTemp, 20, NULL, NULL,
                         0.01, 221.0, 20, SfXFLogIn, SfXFLogOut,
                         StmCp, &SatLine);//, NULL, NULL);
static CSurface3D SfStmH("SteamEnthalpy",
                         MinStmTemp , MaxStmTemp, 20, NULL, NULL,
                         0.01, 221.0, 20, SfXFLogIn, SfXFLogOut,
                         StmH, &SatLine);//, NULL, NULL);
static CSurface3D SfStmS("SteamEntropy",
                         MinStmTemp , MaxStmTemp, 20, NULL, NULL,
                         0.01, 221.0, 20, SfXFLogIn, SfXFLogOut,
                         StmS, &SatLine);//, NULL, NULL);
static CSurface3D SfStmDens("SteamDensity",
                         MinStmTemp , MaxStmTemp, 20, NULL, NULL,
                         0.01, 221.0, 20, SfXFLogIn, SfXFLogOut,
                         StmDens, &SatLine);//, NULL, NULL);
static CSurface3D SfWtrCp("WaterCp",
                         MinWtrTemp, MaxWtrTemp, 20, NULL, NULL,
                         0.01, 1000.0, 20, SfXFLogIn, SfXFLogOut,
                         WtrCp, &SatLine);//, NULL, NULL);
static CSurface3D SfWtrH("WaterEnthalpy",
                         MinWtrTemp, MaxWtrTemp, 20, NULL, NULL,
                         0.01, 1000.0, 20, SfXFLogIn, SfXFLogOut,
                         WtrH, &SatLine);//, NULL, NULL);
static CSurface3D SfWtrS("WaterEntropy",
                         MinWtrTemp, MaxWtrTemp, 20, NULL, NULL,
                         0.01, 1000.0, 20, SfXFLogIn, SfXFLogOut,
                         WtrS, &SatLine);//, NULL, NULL);
static CSurface3D SfWtrDens("WaterDensity",
                         MinWtrTemp, MaxWtrTemp, 20, NULL, NULL,
                         0.01, 1000.0, 20, SfXFLogIn, SfXFLogOut,
                         WtrDens, &SatLine);//, NULL, NULL);
                         /*
static CSurface3D SfStmCp("SteamCp",
                         273.16, 1073.15, 200, NULL, NULL,
                         0.01, 221.0, 200, SfXFLogIn, SfXFLogOut,
                         StmCp, NULL, NULL);
static CSurface3D SfStmH("SteamEnthalpy",
                         273.16, 1073.15, 200, NULL, NULL,
                         0.01, 221.0, 200, SfXFLogIn, SfXFLogOut,
                         StmH, NULL, NULL);
static CSurface3D SfStmS("SteamEntropy",
                         273.16, 1073.15, 200, NULL, NULL,
                         0.01, 221.0, 200, SfXFLogIn, SfXFLogOut,
                         StmS, NULL, NULL);
static CSurface3D SfStmDens("SteamDensity",
                         273.16, 1073.15, 200, NULL, NULL,
                         0.01, 221.0, 200, SfXFLogIn, SfXFLogOut,
                         StmDens, NULL, NULL);
static CSurface3D SfWtrCp("WaterCp",
                         273.16, 623.15, 200, NULL, NULL,
                         0.01, 1000.0, 200, SfXFLogIn, SfXFLogOut,
                         WtrCp, NULL, NULL);
static CSurface3D SfWtrH("WaterEnthalpy",
                         273.16, 623.15, 200, NULL, NULL,
                         0.01, 1000.0, 200, SfXFLogIn, SfXFLogOut,
                         WtrH, NULL, NULL);
static CSurface3D SfWtrS("WaterEntropy",
                         273.16, 623.15, 200, NULL, NULL,
                         0.01, 1000.0, 200, SfXFLogIn, SfXFLogOut,
                         WtrS, NULL, NULL);
static CSurface3D SfWtrDens("WaterDensity",
                         273.16, 623.15, 200, NULL, NULL,
                         0.01, 1000.0, 200, SfXFLogIn, SfXFLogOut,
                         WtrDens, NULL, NULL);
*/
// ==========================================================================
//
//
//
// ==========================================================================
#define DoRangePress 1
#define DoRangeTemp 1
const double IF97MaxPress = 1.0e10; //perhaps reduce this from 1.0e12?
const double IF97MaxTemp = 1.1e4;

H2OEQNBEGIN(IF97_WaterHeatCapacity, "IF97WaterCp")
virtual double AtTP(double Temp, double Press)
  {
  H2OTIMERLAP(sv_swSWlc);
  #ifndef _RELEASE
  if (CSpecieDataBase::gs_nDoTestEstTP && Press*0.01>IF97MaxPress)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Pressure %10.2f", Press);
    }
  if (CSpecieDataBase::gs_nDoTestEstTP && Temp>IF97MaxTemp)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Temperature %10.2f", Temp);
    }
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Press*0.01<IF97MaxPress, "Excessive pressure in IF97!!!", __FILE__, __LINE__);
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Temp<IF97MaxTemp, "Excessive Temperature in IF97!!!", __FILE__, __LINE__);
  #endif
#if DoRangePress
  return H20MoleWt*SfWtrCp(Temp, Min(IF97MaxPress, Press*0.01));
#else
  return H20MoleWt*SfWtrCp(Temp, Press*0.01);
#endif
  };
H2OEQNEND

H2OEQNBEGIN(IF97_SteamHeatCapacity, "IF97SteamCp")
virtual double AtTP(double Temp, double Press)
  {
  H2OTIMERLAP(sv_swSWvc);
  #ifndef _RELEASE
  if (CSpecieDataBase::gs_nDoTestEstTP && Press*0.01>IF97MaxPress)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Pressure %10.2f", Press);
    }
  if (CSpecieDataBase::gs_nDoTestEstTP && Temp>IF97MaxTemp)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Temperature %10.2f", Temp);
    }
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Press*0.01<IF97MaxPress, "Excessive pressure in IF97!!!", __FILE__, __LINE__);
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Temp<IF97MaxTemp, "Excessive Temperature in IF97!!!", __FILE__, __LINE__);
  #endif
#if DoRangePress
  return H20MoleWt*SfStmCp(Temp, Min(IF97MaxPress, Press*0.01));
#else
  return H20MoleWt*SfStmCp(Temp, Press*0.01);
#endif
  }
H2OEQNEND

H2OEQNBEGIN(IF97_WaterEnthalpy, "IF97WaterH")
virtual double AtTP(double Temp, double Press)
  {
  H2OTIMERLAP(sv_swSWlh);
  #ifndef _RELEASE
  if (CSpecieDataBase::gs_nDoTestEstTP && Press*0.01>IF97MaxPress)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Pressure %10.2f", Press);
    }
  if (CSpecieDataBase::gs_nDoTestEstTP && Temp>IF97MaxTemp)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Temperature %10.2f", Temp);
    }
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Press*0.01<IF97MaxPress, "Excessive pressure in IF97!!!", __FILE__, __LINE__);
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Temp<IF97MaxTemp, "Excessive Temperature in IF97!!!", __FILE__, __LINE__);
  #endif
#if DoRangePress
  return H20MoleWt*SfWtrH(Temp, Min(IF97MaxPress, Press*0.01));
#else
  return H20MoleWt*SfWtrH(Temp, Press*0.01);
#endif
  }
H2OEQNEND

H2OEQNBEGIN(IF97_SteamEnthalpy, "IF97SteamH")
virtual double AtTP(double Temp, double Press)
  {
  H2OTIMERLAP(sv_swSWvh);
  #ifndef _RELEASE
  if (CSpecieDataBase::gs_nDoTestEstTP && Press*0.01>IF97MaxPress)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Pressure %10.2f", Press);
    }
  if (CSpecieDataBase::gs_nDoTestEstTP && Temp>IF97MaxTemp)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Temperature %10.2f", Temp);
    }
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Press*0.01<IF97MaxPress, "Excessive pressure in IF97!!!", __FILE__, __LINE__);
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Temp<IF97MaxTemp, "Excessive Temperature in IF97!!!", __FILE__, __LINE__);
  #endif
#if DoRangePress
  return H20MoleWt*SfStmH(Temp, Min(IF97MaxPress, Press*0.01));
#else
  return H20MoleWt*SfStmH(Temp, Press*0.01);
#endif
  }
H2OEQNEND

H2OEQNBEGIN(IF97_WaterEntropy, "IF97WaterS")
virtual double AtTP(double Temp, double Press)
  {
  H2OTIMERLAP(sv_swSWls);
  #ifndef _RELEASE
  if (CSpecieDataBase::gs_nDoTestEstTP && Press*0.01>IF97MaxPress)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Pressure %10.2f", Press);
    }
  if (CSpecieDataBase::gs_nDoTestEstTP && Temp>IF97MaxTemp)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Temperature %10.2f", Temp);
    }
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Press*0.01<IF97MaxPress, "Excessive pressure in IF97!!!", __FILE__, __LINE__);
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Temp<IF97MaxTemp, "Excessive Temperature in IF97!!!", __FILE__, __LINE__);
  #endif
#if DoRangePress
  return H20MoleWt*SfWtrS(Temp, Min(IF97MaxPress, Press*0.01));
#else
  return H20MoleWt*SfWtrS(Temp, Press*0.01);
#endif
  }
H2OEQNEND

H2OEQNBEGIN(IF97_SteamEntropy, "IF97SteamS")
virtual double AtTP(double Temp, double Press)
  {
  H2OTIMERLAP(sv_swSWvs);
  #ifndef _RELEASE
  if (CSpecieDataBase::gs_nDoTestEstTP && Press*0.01>IF97MaxPress)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Pressure %10.2f", Press);
    }
  if (CSpecieDataBase::gs_nDoTestEstTP && Temp>IF97MaxTemp)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Temperature %10.2f", Temp);
    }
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Press*0.01<IF97MaxPress, "Excessive pressure in IF97!!!", __FILE__, __LINE__);
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Temp<IF97MaxTemp, "Excessive Temperature in IF97!!!", __FILE__, __LINE__);
  #endif
#if DoRangePress
  return H20MoleWt*SfStmS(Temp, Min(IF97MaxPress, Press*0.01));
#else
  return H20MoleWt*SfStmS(Temp, Press*0.01);
#endif
  }
H2OEQNEND

H2OEQNBEGIN(IF97_WaterDensity, "IF97WaterRho")
virtual double AtTP(double Temp, double Press)
  {
  H2OTIMERLAP(sv_swSWld);
  #ifndef _RELEASE
  if (CSpecieDataBase::gs_nDoTestEstTP && Press*0.01>IF97MaxPress)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Pressure %10.2f", Press);
    }
  if (CSpecieDataBase::gs_nDoTestEstTP && Temp>IF97MaxTemp)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Temperature %10.2f", Temp);
    }
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Press*0.01<IF97MaxPress, "Excessive pressure in IF97!!!", __FILE__, __LINE__);
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Temp<IF97MaxTemp, "Excessive Temperature in IF97!!!", __FILE__, __LINE__);
  #endif
#if DoRangePress
  return SfWtrDens(Temp, Min(IF97MaxPress, Press*0.01));
#else
  return SfWtrDens(Temp, Press*0.01);
#endif
  }
H2OEQNEND

H2OEQNBEGIN(IF97_SteamDensity, "IF97SteamRho")
virtual double AtTP(double Temp, double Press)
  {
  H2OTIMERLAP(sv_swSWvd);
  #ifndef _RELEASE
  if (CSpecieDataBase::gs_nDoTestEstTP && Press*0.01>IF97MaxPress)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Pressure %10.2f", Press);
    }
  if (CSpecieDataBase::gs_nDoTestEstTP && Temp>IF97MaxTemp)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Temperature %10.2f", Temp);
    }
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Press*0.01<IF97MaxPress, "Excessive pressure in IF97!!!", __FILE__, __LINE__);
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Temp<IF97MaxTemp, "Excessive Temperature in IF97!!!", __FILE__, __LINE__);
  #endif
#if DoRangePress
  return SfStmDens(Temp, Min(IF97MaxPress, Press*0.01));
#else
  return SfStmDens(Temp, Press*0.01);
#endif
  }
H2OEQNEND

H2OEQNBEGIN(IF97_WaterVapourPress, "IF97WaterVp")
virtual double AtT(double Temp)
  {
  H2OTIMERLAP(sv_swSWvp);
  #ifndef _RELEASE
  if (CSpecieDataBase::gs_nDoTestEstTP && Temp>IF97MaxTemp)
    {
    CSpecieDataBase::gs_nDoTestEstTP--;
    LogError("IF97", 0, "Excessive Temperature %10.2f", Temp);
    }
  //ASSERT_ALWAYS(!CSpecieDataBase::gs_nDoTestEstTP || Temp<IF97MaxTemp, "Excessive Temperature in IF97!!!", __FILE__, __LINE__);
  #endif
#if DoRangeTemp
  return 100.0*SfVapP(Min(IF97MaxTemp, Temp));
#else
  return 100.0*SfVapP(Temp);
#endif
  }
H2OEQNEND

// ==========================================================================
//
//
//
// ==========================================================================

bool TryConstructIF97(CSpConstData * pConst, CSpFidelData * pSpD, LPCTSTR Str, LPCTSTR &pn, CSpDBEqn* & p)
  {
  if (_stricmp(Str, IF97_WaterDensity::Name())==0)
    {
    pn=IF97_WaterDensity::Name();
    p=new IF97_WaterDensity(pConst, pSpD);
    }
  else if (_stricmp(Str, IF97_SteamDensity::Name())==0)
    {
    pn=IF97_SteamDensity::Name();
    p=new IF97_SteamDensity(pConst, pSpD);
    }
  else if (_stricmp(Str, IF97_WaterHeatCapacity::Name())==0)
    {
    pn=IF97_WaterHeatCapacity::Name();
    p=new IF97_WaterHeatCapacity(pConst, pSpD);
    }
  else if (_stricmp(Str, IF97_SteamHeatCapacity::Name())==0)
    {
    pn=IF97_SteamHeatCapacity::Name();
    p=new IF97_SteamHeatCapacity(pConst, pSpD);
    }
  else if (_stricmp(Str, IF97_WaterEnthalpy::Name())==0)
    {
    pn=IF97_WaterEnthalpy::Name();
    p=new IF97_WaterEnthalpy(pConst, pSpD);
    }
  else if (_stricmp(Str, IF97_SteamEnthalpy::Name())==0)
    {
    pn=IF97_SteamEnthalpy::Name();
    p=new IF97_SteamEnthalpy(pConst, pSpD);
    }
  else if (_stricmp(Str, IF97_WaterEntropy::Name())==0)
    {
    pn=IF97_WaterEntropy::Name();
    p=new IF97_WaterEntropy(pConst, pSpD);
    }
  else if (_stricmp(Str, IF97_SteamEntropy::Name())==0)
    {
    pn=IF97_SteamEntropy::Name();
    p=new IF97_SteamEntropy(pConst, pSpD);
    }
  else if (_stricmp(Str, IF97_WaterVapourPress::Name())==0)
    {
    pn=IF97_WaterVapourPress::Name();
    p=new IF97_WaterVapourPress(pConst, pSpD);
    }
  else
    return false;

  return true;
  }

// ==========================================================================
//
//
//
// ===========================================================================


// ==========================================================================
//
//
//
// ===========================================================================


#define DllImportExport

//--------------------------------------------------------------------------


DEFINE_TAGOBJ(IF97H2OProps);
class IF97H2OProps : public H2OPropertiesBlock
  {
  public:

    IF97H2OProps(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~IF97H2OProps();
    virtual void   BuildDataDefn(DataDefnBlk & DDB)     { };
    virtual flag   DataXchg(DataChangeBlk & DCB)        { return 0; };
    virtual flag   ValidateData(ValidateDataBlk & VDB)  { return 0; };

  };

//static dword * dbgptr; 
//--------------------------------------------------------------------------

IMPLEMENT_TAGOBJ(IF97H2OProps, H2OPropsGroup, IF97H2OName, "", "", "", TOC_SYSTEM,
                 "IAPWS-IF97 Props",
                 "IAPWS-IF97 Props for Water & Steam");

IF97H2OProps::IF97H2OProps(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
  H2OPropertiesBlock(pClass_, pTag, pAttach, eAttach)
  {
  if (1)
    {
    m_Liq[0].m_dTRef        = C_2_K(25);
    m_Liq[0].m_dTs[0]       = C_2_K(0);
    m_Liq[0].m_dTe[0]       = 500.0;
    m_Liq[0].m_pRho[0]      = CSpDBEqn::Construct(NULL, NULL, "LiqH2ORho()", NULL);
    m_Liq[0].m_pCp[0]       = CSpDBEqn::Construct(NULL, NULL, "HSC_Cp(16.749,62.120,32.798,90.391)", NULL);
    m_Liq[0].m_pH[0]        = NULL;
    m_Liq[0].m_pS[0]        = NULL;
    m_Liq[0].m_dH25[0]      = -285829.970;
    m_Liq[0].m_dS25[0]      = 69.948;
    m_Liq[0].m_bH25OK       = true;
    m_Liq[0].m_bS25OK       = true;

    m_Vap[0].m_dTRef        = C_2_K(25);
    m_Vap[0].m_dTs[0]       = 298.15;                                                                      
    m_Vap[0].m_dTe[0]       = 500.0;

    m_Vap[0].m_pRho[0]      = CSpDBEqn::Construct(NULL, NULL, "IdealGasDensity(18.01528)", NULL);//CSpDBEqn::Construct(NULL, NULL, "0.8037", NULL);
    m_Vap[0].m_pCp[0]       = CSpDBEqn::Construct(NULL, NULL, "CRC_Cp(-38.14955, 463.4602, 6.24E-05, -762.3604)", NULL);       
    m_Vap[0].m_pH[0]        = NULL;                                                                        
    m_Vap[0].m_pS[0]        = NULL;                                                                        
    m_Vap[0].m_dH25[0]      = -241826.833;                                                                 
    m_Vap[0].m_dS25[0]      = 188.832;                                                                      
    m_Vap[0].m_bH25OK       = true;
    m_Vap[0].m_bS25OK       = true;
    
    m_Liq[1].m_dTs[0]       = 273.16;
    m_Liq[1].m_dTe[0]       = 623.16;
    m_Liq[1].m_pRho[0]      = new IF97_WaterDensity(NULL, NULL);
    m_Liq[1].m_pCp[0]       = new IF97_WaterHeatCapacity(NULL, NULL);
    m_Liq[1].m_pH[0]        = new IF97_WaterEnthalpy(NULL, NULL);
    m_Liq[1].m_pS[0]        = new IF97_WaterEntropy(NULL, NULL);
    m_Liq[1].m_dH25[0]      = -285829.970;
    m_Liq[1].m_dS25[0]      = 69.948;
    m_Liq[1].m_bH25OK       = true;
    m_Liq[1].m_bS25OK       = true;

    m_Vap[1].m_dTs[0]       = 273.16;
    m_Vap[1].m_dTe[0]       = 1073.16;
    m_Vap[1].m_pRho[0]      = new IF97_SteamDensity(NULL, NULL);
    m_Vap[1].m_pCp[0]       = new IF97_SteamHeatCapacity(NULL, NULL);
    m_Vap[1].m_pH[0]        = new IF97_SteamEnthalpy(NULL, NULL);
    m_Vap[1].m_pS[0]        = new IF97_SteamEntropy(NULL, NULL);
    m_Vap[1].m_dTRef        = C_2_K(25);
    m_Vap[1].m_dH25[0]      = -241826.833;
    m_Vap[1].m_dS25[0]      = 188.832;
    m_Vap[1].m_bH25OK       = true;
    m_Vap[1].m_bS25OK       = true;

    m_Common[0].m_bVpOK     = true;
    m_Common[0].m_pVp       = CSpDBEqn::Construct(NULL, NULL, "Vp(-3433.74,-12.0063,0.004782,41.1767)", NULL);
    m_Common[1].m_bVpOK     = true;
    m_Common[1].m_pVp       = new IF97_WaterVapourPress(NULL, NULL);
    }
  else
    {
    m_Liq[0].m_dTRef        = C_2_K(25);
    m_Liq[0].m_dTs[0]       = 298.15;
    m_Liq[0].m_dTe[0]       = 372.78;
    m_Liq[0].m_pRho[0]      = CSpDBEqn::Construct(NULL, NULL, "LiqH2ORho()", NULL);
    m_Liq[0].m_pCp[0]       = CSpDBEqn::Construct(NULL, NULL, "CRC1_Cp(53.927,31.712,31.365,8.167)", NULL);
    m_Liq[0].m_pH[0]        = NULL;
    m_Liq[0].m_pS[0]        = NULL;
    m_Liq[0].m_dH25[0]      = -285829.970;
    m_Liq[0].m_dS25[0]      = 69.948;

    m_Liq[0].m_dTs[1]       = 372.78;
    m_Liq[0].m_dTe[1]       = 500;
    m_Liq[0].m_pRho[1]      = CSpDBEqn::Construct(NULL, NULL, "LiqH2ORho()", NULL);
    m_Liq[0].m_pCp[1]       = CSpDBEqn::Construct(NULL, NULL, "HSC_Cp(16.749,62.120,32.798,90.391)", NULL);
    m_Liq[0].m_pH[1]        = NULL;
    m_Liq[0].m_pS[1]        = NULL;
    m_Liq[0].m_dH25[1]      = -285965.245;
    m_Liq[0].m_dS25[1]      = 0;
    m_Liq[0].m_bH25OK       = true;
    m_Liq[0].m_bS25OK       = true;


    m_Vap[0].m_dTRef        = C_2_K(25);
    m_Vap[0].m_dTs[0]       = 298.15;                                                                      
    m_Vap[0].m_dTe[0]       = 513;                                                                      
    m_Vap[0].m_pRho[0]      = CSpDBEqn::Construct(NULL, NULL, "0.8037", NULL);                               
    m_Vap[0].m_pCp[0]       = CSpDBEqn::Construct(NULL, NULL, "CRC_Cp(-38.14955, 463.4602, 6.24E-05, -762.3604)", NULL);       
    m_Vap[0].m_pH[0]        = NULL;                                                                        
    m_Vap[0].m_pS[0]        = NULL;                                                                        
    m_Vap[0].m_dH25[0]      = -241826.833;                                                                 
    m_Vap[0].m_dS25[0]      = 188.832;                                                                      
    m_Vap[0].m_bH25OK       = true;
    m_Vap[0].m_bS25OK       = true;
    
    m_Liq[1].m_dTs[0]       = 273.16;
    m_Liq[1].m_dTe[0]       = 623.16;
    m_Liq[1].m_pRho[0]      = new IF97_WaterDensity(NULL, NULL);
    m_Liq[1].m_pCp[0]       = new IF97_WaterHeatCapacity(NULL, NULL);
    m_Liq[1].m_pH[0]        = new IF97_WaterEnthalpy(NULL, NULL);
    m_Liq[1].m_pS[0]        = new IF97_WaterEntropy(NULL, NULL);
    m_Liq[1].m_dH25[0]      = -285829.970;
    m_Liq[1].m_dS25[0]      = 69.948;
    m_Liq[1].m_bH25OK       = true;
    m_Liq[1].m_bS25OK       = true;

    m_Vap[1].m_dTs[0]       = 273.16;
    m_Vap[1].m_dTe[0]       = 1073.16;
    m_Vap[1].m_pRho[0]      = new IF97_SteamDensity(NULL, NULL);
    m_Vap[1].m_pCp[0]       = new IF97_SteamHeatCapacity(NULL, NULL);
    m_Vap[1].m_pH[0]        = new IF97_SteamEnthalpy(NULL, NULL);
    m_Vap[1].m_pS[0]        = new IF97_SteamEntropy(NULL, NULL);
    m_Vap[1].m_dTRef        = C_2_K(25);
    m_Vap[1].m_dH25[0]      = -241826.833;
    m_Vap[1].m_dS25[0]      = 188.832;
    m_Vap[1].m_bH25OK       = true;
    m_Vap[1].m_bS25OK       = true;

    m_Common[0].m_bVpOK     = true;
    m_Common[0].m_pVp       = CSpDBEqn::Construct(NULL, NULL, "Vp(-3433.74,-12.0063,0.004782,41.1767)", NULL);
    m_Common[1].m_bVpOK     = true;
    m_Common[1].m_pVp       = new IF97_WaterVapourPress(NULL, NULL);
    }
  }

//--------------------------------------------------------------------------

IF97H2OProps::~IF97H2OProps()
  {
  for (int i=0; i<MaxFidelity; i++)
    {
    for (int e=0; e<SpDBEqnCnt; e++)
      {
      for (int j=0; j<MaxTPoints; j++)
        {
        delete m_Liq[i].m_pEqns[e][j];
        delete m_Vap[i].m_pEqns[e][j];

        m_Liq[i].m_pEqns[e][j]=NULL;
        m_Vap[i].m_pEqns[e][j]=NULL;
        }
      }
    }
  }

//--------------------------------------------------------------------------

// ==========================================================================
//
//
//
// ===========================================================================
#endif
// ==========================================================================
//
//
//
// ===========================================================================
