//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#include "stdafx.h"
#include "sc_defs.h"
#define  __SPDBE_IAF_CPP
#include "sp_db.h"
#include "surfaces.h"
//#include "optoff.h"

//#define dbgBuildSDB    0

#if dbgBuildSDB
#include "dbgmngr.h"
static CDbgMngr dbgBuild("Specie_DB", "BuildIAF"); 
#endif

// ==========================================================================
//
//
//
// ===========================================================================
/*
****************************************************************
*                                                              *
* C functions for calculating                                  *
* properties of water and steam                                *
*                                                              *
* Authored by Bernhard Spang, The Mining Company               *
* URL:        http://chemengineer.miningco.com                 *
* email:      chemengineer.guide@miningco.com                  *
*                                                              *
* Copyright (C) 1998 by Bernhard Spang. All rights reserved.   *
* May be used and modified for free. Redistribution is also    *
* allowed if full credit is given to the author and a link to  *
* http://chemengineer.miningco.com is included.                *
* Provided "as is" without warranty of any kind.               *
*                                                              *
* For documentation see                                        *
* http://chemengineer.miningco.com/library/weekly/aa081798.htm *
*                                                              *
* Reference:                                                   *
* Properties of Water and Steam in SI-Units,                   *
* 2nd Revised and Updated Printing, Springer 1979, pp. 175 ff. *
*                                                              *
* Revisions:                                                   *
* 04/19/99 Routine BOILP for saturation temperature added      *
*          Thanks to Ian Journeaux <IAJ1@mead.com>             *
*                                                              *
****************************************************************

*/

#include <math.h>

//double BL();
//double DBL();
double BL(double TR);
double DBL(double TR);

double VAPP(double TEMP)
/*
****************************************************************
*  Vapor pressure VAPP according to IFC 68 in bar              *
*  TEMP = temperature in K                                     *
*  VAPP = -1: given temperature outside of range               *
****************************************************************
*/
  {

  double T[12],U,SUM,TR,PR;//,TC,PC;
  int i;
  static const double AK[12]=
    {-4.0596821, 5.1322555, -1.1842407, 0.11779592, -0.005157642, 
    -0.0014689537, 0.00053622818, 0.00012455399, -0.000049154288, 
    0.000046302565, 0.000015301334, -0.00002095453,};
  static const double BK[4]=
    {2, 0.95, 1.45220717, -0.84878953,};
  static const double TC=647.3;
  static const double PC=221.2;

  TR = TEMP/TC;

  if (TEMP>714.0)
    {
    int x=444;
    }
  // CNM if ((TR < 0.421) || (TR > 1.658)) {return(-1);}
  //if ((TR < 0.421) || (TR > 1.658)) {return(-1);}
  //else {
  TR=Range(0.421, TR, 1.0/BK[1]);
  //TR < 1.0/BK[1] CNM
  U = (BK[0]*pow((1./TR-BK[1]),0.4)-BK[2])/BK[3];
  T[0] = 1.;
  T[1] = U;
  for (i = 2; i < 12; i++)
    T[i] = 2. * U * T[i-1] - T[i-2];

  SUM = 0.;
  for (i = 0; i < 12; i++)
    SUM = SUM + AK[i]*T[i];

  PR = exp(SUM);
  return(PR*PC);
  //}
  }
double BOILP(double PRES)
/*
****************************************************************
*  Saturation Temperature BOILP according to IFC 68 in K       *
*  PRES = pressure in bar                                      *
*  BOILP = -1: given pressure outside of range                 *
*  Written by Ian Journeaux from pascal version of water.c     *
****************************************************************
*/
  {

  double Tunt, Tob, Tm;
  double Punt, Pob, Pm;
  double DPunt, DPob, DPm;
  double BOILP;
  Tunt = 280.15;
  Tob = 646.85;
  Tm = 0.5 * (Tunt + Tob);
  Punt = VAPP(Tunt);
  Pob = VAPP(Tob);
  Pm = VAPP(Tm);
  DPunt = Punt - PRES;
  DPob = Pob - PRES;
  DPm = Pm - PRES;

  if ((DPunt*DPob)>0) {
    BOILP = -1;
    } else {
      do {
        if ((DPm * DPunt)<0) {
          Tob = Tm;
          Pob = Pm;
          DPob = DPm;
          } else {
            Tunt = Tm;
            Punt = Pm;
            DPunt = DPm;
            }
        Tm = 0.5 *(Tunt+Tob);
        BOILP = Tm;
        Pm = VAPP(Tm);
        DPm = Pm - PRES;
        } while ((Tm - Tunt) >= 0.00005);
      }
  return(BOILP);
  }

void VSUB2(double TEMP, double PRES, double *VOL, double *ENTH)
/*
****************************************************************
*  Calculation of specific volume VOL in m**3/kg               *
*  and specific enthalphy ENTH in kJ/kg                        *
*  according to IFC 67 for sub-region 2                        *
*  temperature range: 273.16 K < T < 1073.15 K                 *
*  pressure range : 0 < P < vapor pressure                     *
*  Exception : for 623.15 < T < TC = 647.3 K                   *
*              only  P < BL * PC (see L-FUNCTION BL)           *
*  !!! must be taken care of in the calling program !!!        *
*		     	      						  *
*  Temperature TEMP in K                                       *
*  Pressure PRES in bar                                        *   
****************************************************************
*/
  {
  //H2OTIMERLAP(sv_swSWvh);
  double TR,PR,VR,HR,TC,PC,VC;
  double RI1,BKA,X,PRL,DPRL;
  double S1,S2,S2A,S3,S3A,S3B,S3ZAE,S3HI,S3NEN,S4,H1,H2,H3,H4,H3A,H4A,H4B;
  double H4C,HIL1,H5,H5A;
  int LAM,NAA,NAB,NAC,i,ia,ib,ic;

  static const int NA[8] = 
    {2, 3, 2, 2, 3, 2, 2, 2,};
  static const int NZ[8][3] =
    {
      {13,  3,  0,  },
      {18,  2,  1,  },
      {18,  10, 0,  },
      {25,  14, 0,  },
      {32,  28, 24, },
      {12,  11, 0,  },
      {24,  18, 0,  },
      {24,  14, 0,  },
    };
  static const int NL[3] =
    {1, 1, 2,};

  static const int NX[3][2]=
    {
      {14,  0,  },
      {19,  0,  },
      {54,  27, },
    };

  static const double BGA[6]=
    { 16.83599274, 28.56067796, -54.38923329, 0.4330662834, -0.6547711697, 0.08565182058,};

  static const double BG[8][3]=
    {
      {0.06670375918,  1.388983801,     0,              }, 
      {0.08390104328,  0.02614670893,   -0.03373439453, },
      {0.4520918904,   0.1069036614,    0,              },
      {-0.5975336707,  -0.08847535804,  0,              },
      {0.5958051609,   -0.5159303373,   0.2075021122,   },
      {0.1190610271,   -0.09867174132,  0,              },
      {0.1683998803,   -0.05809438001,  0,              },
      {0.006552390126, 0.0005710218649, 0,              },
    }; 

  static const double BK[3][2]=
    {
      {0.4006073948,  0,           },
      {0.08636081627, 0,           },
      {-0.8532322921, 0.3460208861,},
    };


  static const double B9[7] = 
    {193.6587558, -1388.522425, 4126.607219, -6508.211677, 
    5745.984054, -2693.088365, 523.5718623,};

  RI1 = 4.260321148;
  BKA = 7.633333333E-1;
  TC = 647.3;
  PC = 221.2;
  VC = 0.00317;

  TR = TEMP/TC;
  PR = PRES/PC;

  X = exp(BKA*(1.-TR));
  PRL = BL(TR);
  DPRL = DBL(TR);

  if (VOL || ENTH)
    {
    S1 = RI1*TR/PR;

    S2 = 0.;

    for (i = 0; i < 5; i++)
      {
      S2A = 0.;
      NAB = NA[i];
      for (ia = 0; ia < NAB; ia++) 
        S2A = S2A + BG[i][ia]*pow(X,NZ[i][ia]);
      S2 = S2 + (i+1)*pow(PR,i)*S2A;
      }

    S3 = 0.;
    for (i = 5; i < 8; i++)
      {
      LAM = i-5;
      S3A = 0.;
      NAB = NA[i];
      for (ia = 0; ia < NAB; ia++)
        S3A = S3A + BG[i][ia]*pow(X,NZ[i][ia]);
      S3ZAE = (i-1)*pow(PR,(-i))*S3A;
      S3B = 0.;
      NAC = NL[LAM];
      for (ib = 0; ib < NAC; ib++)
        S3B = S3B + BK[LAM][ib]*pow(X,NX[LAM][ib]);
      S3HI = pow(PR,(1-i)) + S3B;
      S3NEN = S3HI * S3HI;
      S3 = S3 + S3ZAE/S3NEN;
      }

    if (VOL)
      {
      S4 = 0.;
      for (i = 0; i < 7; i++)
        S4 = S4 + B9[i]*pow(X,i);

      S4 = 11.*pow((PR/PRL),10)*S4;

      VR = S1-S2-S3+S4;
      *VOL = VR*VC;
      }
    }

  if (ENTH)
    {
    H1 = BGA[0]*TR;
    H2 = 0.;
    for (i = 1; i < 6; i++)
      H2 = H2 + BGA[i]*(i-2)*pow(TR,(i-1));

    H3 = 0.;
    for (i = 0; i < 5; i++) 
      {
      H3A = 0.;
      NAA = NA[i];
      for(ia = 0;ia < NAA;ia++)
        H3A = H3A+(BG[i][ia]*(1.+NZ[i][ia]*BKA*TR))*pow(X,NZ[i][ia]);
      H3 = H3 + pow(PR,(i+1)) * H3A;
      }

    H4 = 0.;

    for (i = 5; i < 8; i++)
      {
      H4A = 0.;
      H4B = 0.;
      LAM = i-5;
      NAB = NL[LAM];
      for (ib = 0; ib < NAB; ib++)
        H4B = H4B + NX[LAM][ib]*BK[LAM][ib]*pow(X,NX[LAM][ib]);
      S3ZAE = (i-1)*pow(PR,(-i))*S3A;
      H4C = 0.;
      for (ic = 0; ic < NAB; ic++)
        H4C = H4C + BK[LAM][ic]*pow(X,NX[LAM][ic]);
      HIL1 = pow(PR,(1-i)) + H4C;
      NAC = NA[i];
      for (ia = 0; ia < NAC; ia++)
        H4A = H4A + BG[i][ia]*pow(X,NZ[i][ia])*(1.+NZ[i][ia]*BKA*TR - BKA*TR*H4B/HIL1);
      H4 = H4 + H4A/HIL1;
      }

    H5A = 0.;
    for (i = 0;i < 7;i++) 
      H5A = H5A + (1.+TR*(10.*DPRL/PRL+i*BKA))*B9[i]*pow(X,i);
    H5 = PR * pow((PR/PRL),10) * H5A;
    HR = H1-H2-H3-H4+H5;
    *ENTH = HR*70.1204;
    }
  /*      return(0);  */
  }

void VSUB1(double TEMP, double PRES, double *VOL, double *ENTH)

/*
****************************************************************
*  Calculation of specific volume VOL in m**3/kg               * 
*  and specific enthalphy ENTH in kJ/kg                        *
*  according to IFC 67 for sub-region 1                        *
*  temperature range: 273.16 K < T < 623.15 K                  *
*  pressure range : vapor pressure < P < 1000 bar              *
*  !!! must be taken care of in the calling program !!!        *
*										   *
*  Temperature TEMP in K                                       *
*  Pressure PRES in bar                                        *
**************************************************************** 
*/
  { 
  //H2OTIMERLAP(sv_swSWlh);

  double Y,Y1,Z,S1,S2,S3,S4,S5,S6;
  double H1,H2,H3,H4,H5,H6,H7,H8,TR,PR,VR,HR,TC,PC,VC;
  int i,i1;
  static const double AK[12] =
    {0.8438375405, 0.0005362162162, 1.72, 0.07342278489, 0.0497585887,
    0.65371543, 0.00000115, 0.000015108, 0.14188,  7.002753165,
    0.0002995284926, 0.204};

  static const double AG[12] =
    {7.982692717, -0.02616571843, 0.00152241179, 0.02284279054, 242.1647003,
    1.269716088E-10, 2.074838328E-07, 2.17402035E-08, 1.105710498E-09,
    12.93441934, 0.00001308119072, 6.047626338E-14};

  static const double AGA[11] =
    {6824.687741, -542.2063673, -20966.66205, 39412.86787, -67332.77739,
    99023.81028, -109391.1774, 85908.41667, -45111.68742, 14181.38926, -2017.271113};

  TC = 647.3;
  PC = 221.2;
  VC = 0.00317;

  TR = TEMP/TC;
  PR = PRES/PC;

  Y = 1.-AK[0]*TR*TR-AK[1]/pow(TR,6);
  Y1 = 6.*AK[1]/pow(TR,7)-2.*AK[0]*TR;
  Z = Y + sqrt(GEZ(AK[2]*Y*Y-2.*AK[3]*TR+2.*AK[4]*PR));
  //CNM      Z = Y + sqrt(AK[2]*Y*Y-2.*AK[3]*TR+2.*AK[4]*PR);

  if (VOL)
    {
    S1 = AG[0]*AK[4]/pow(Z,(5./17.));
    S2 = AG[1]+AG[2]*TR+AG[3]*TR*TR+AG[4]*pow((AK[5]-TR),10)+AG[5]/(AK[6]+pow(TR,19));
    S3 = (AG[6]+2.*AG[7]*PR+3.*AG[8]*PR*PR)/(AK[7]+pow(TR,11));
    S4 = AG[9]*pow(TR,18)*(AK[8]+TR*TR)*((-3.)/pow((AK[9]+PR),4)+AK[10]);
    S5 = 3.*AG[10]*(AK[11]-TR)*PR*PR;
    S6 = 4.*AG[11]*pow(PR,3)/pow(TR,20);

    VR = S1+S2-S3-S4+S5+S6;
    *VOL = VR*VC;
    }

  if (ENTH)
    {
    H1 = AGA[0]*TR;
    H2 = 0.;
    for (i = 0;i < 10;i++)
      {
      i1 = i+1;
      H2 = H2 + (i-1)*AGA[i1]*pow(TR,i);
      }
    H3 = AG[0]*(Z*(17.*(Z/29.-Y/12.)+5.*TR*Y1/12.)+AK[3]*TR -(AK[2]-1.)*TR*Y*Y1)/pow(Z,(5./17.));
    H4 = (AG[1]-AG[3]*TR*TR+AG[4]*(9.*TR+AK[5])*pow((AK[5]-TR),9) + AG[5]*(20.*pow(TR,19)+AK[6])/pow((AK[6]+pow(TR,19)),2))*PR;
    H5 = (12.*pow(TR,11)+AK[7])/pow((AK[7]+pow(TR,11)),2)*(AG[6]*PR+AG[7]*PR*PR+AG[8]*pow(PR,3));
    H6 = AG[9]*pow(TR,18)*(17.*AK[8]+19.*TR*TR)*(pow((AK[9]+PR),(-3))+AK[10]*PR);
    H7 = AG[10]*AK[11]*pow(PR,3);
    H8 = 21.*AG[11]*pow(PR,4)/pow(TR,20);
    HR = H1-H2+H3+H4-H5+H6+H7+H8;
    *ENTH = HR*70.1204;
    }
  /*      return(0); */
  }

double CP(double TEMP, double PRES, int IAG)
/*
****************************************************************
*  specific heat capacity CP IN KJ/(KG*K)                      *
*                                                              *
*  Numerical calculation from enthalpy                         *
*                                                              *
*  Temperature TEMP in K                                       *
*  Pressure PRES in bar                                        *  
*  IAG = 1: LIQUID;   IAG = 2: VAPOR                           *
****************************************************************
*/
  {
  double T1,T2,H1,H2,VDUM;
  T1 = TEMP - 0.50;
  T2 = TEMP + 0.50;
  if (IAG==2)
    {
    VSUB2(T1,PRES,&VDUM,&H1);
    VSUB2(T2,PRES,&VDUM,&H2);
    }
  else
    {
    VSUB1(T1,PRES,&VDUM,&H1);
    VSUB1(T2,PRES,&VDUM,&H2);
    }
  return(H2-H1);
  }

double ETAF(double T, double V)
/*
****************************************************************
*   Dynamic viscosity ETAF of water                            *
*   up to 800 °C and 1000 bar                                  *
*   See PROPERTIES OF WATER AND STEAM, p. 188                  *
*                                                              *
*   Temperature T   IN K                                       *
*   Specific volume V IN M**3/KG                               *
*   ETAF IN E-6KG/MS                                           *
****************************************************************
*/
  {
  double TUM,VUM,SUM,ETA0;
  int i,j;

  static const double AK[4] = 
    { 0.0181583, 0.0177624, 0.0105287, -0.0036744};
  static const double BK[6][5] =
    {     
      { 0.501938,  0.235622,  -0.274637, 0.145831,  -0.0270448, },
      { 0.162888,  0.789393,  -0.743539, 0.263129,  -0.0253093, },
      { -0.130356, 0.673665,  -0.959456, 0.347247,  -0.0267758, },
      { 0.907919,  1.207552,  -0.687343, 0.213486,  -0.0822904, },
      { -0.551119, 0.0670665, -0.497089, 0.100754,  0.0602253,  },
      { 0.146543,  -0.084337, 0.195286,  -0.032932, -0.0202595, },
    };    

  TUM = 647.27/T;
  VUM = 3.147E-3/V;

  SUM = 0.;
  for (i = 0;i < 4;i++) 
    SUM = SUM + AK[i]*pow(TUM,i);

  ETA0 = sqrt(1./TUM)/SUM;

  SUM = 0.;
  for (i = 0;i < 6;i++)
    {
    for (j = 0;j < 5;j++)
      SUM = SUM + BK[i][j]*pow((TUM-1.),i)*pow((VUM-1.),j);
    }

  return(ETA0 * exp(VUM*SUM));
  }


double TCON(double T, double V)
/*
****************************************************************
*   Thermal conductivity TCON of water                         *
*                                                              *
*   up to 1500 °C and 3000 BAR                                 *
*                                                              *
*   PROPERTIES OF WATER AND STEAM, p. 188/189                  *
*   equation of MINAMIYAMA                                     *
*                                                              *
*   Temperature T   IN K                                       * 
*   Specific volume V IN M**3/KG                               *
*   TCON IN W/K*M                                              *
****************************************************************
*/
  {
  double RHO,TR,RHOR;
  double SUM,HI,ALAM0,ALAMQ,DTR,Q,R,DL1,DL2,DL3,DL4,DL5,DLAM,S,DL41;
  int i;
  static const double AK[4] =
    { 0.0102811, 0.0299621, 0.0156146, -0.00422464};
  static const double BK[3] =
    { -0.39707, 0.400302, 1.06};
  static const double BG[2] =
    { -0.171587, 2.39219};
  static const double DK[4] =
    { 0.0701309, 0.011852, 0.00169937, -1.02};
  static const double CG[6] =
    { 0.642857, -4.11717, -6.17937, 0.00308976, 0.0822994, 10.0932};

  RHO = 1./V;
  TR = T/647.3;
  RHOR = RHO/317.7;

  SUM = 0.;
  for (i = 0;i < 4;i++) 
    SUM = SUM + AK[i]*pow(TR,i);

  ALAM0 = sqrt(TR) * SUM;

  HI = RHOR + BG[1];
  ALAMQ = BK[0]+BK[1]*RHOR+BK[2]*exp(BG[0]*HI*HI);

  DTR = fabs(TR-1.) + CG[3];
  Q = 2.+CG[4]*pow(DTR,(-0.6));
  R = Q*TR+1.;
  if (TR < 1.)
    S = CG[5]*pow(DTR,(-0.6));
  else
    S = 1./DTR;

  DL1 = (DK[0]*pow((1./TR),10) + DK[1])*pow(RHOR,1.8);
  DL2 = CG[0]*(1.-pow(RHOR,2.8));
  DL3 = DK[2]*S*pow(RHOR,Q);
  DL4 = Q/R * (1.-pow(RHOR,R));
  DL41= CG[1]*pow(TR,1.5)+CG[2]*pow((1./RHOR),5);
  if (fabs(DL41)<= 700)    DL5 = DK[3]*exp(DL41);
  else DL5 = 0.;
  DLAM = DL1*exp(DL2)+DL3*exp(DL4)+DL5;

  return(ALAM0 + ALAMQ + DLAM);
  }

double ATEMF(double TCON, double CPW, double VOL)
/*
****************************************************************
*   thermal diffusivity ATEMF in E-6*M**2/S                    *
*                                                              *
*   INPUT: thermal conductivity TCON in W/(K*M)                *
*          heat capacity        CPW  in KJ/(KG*K)              *
*          specific volume      VOL  in m**3/kg                *
****************************************************************
*/
  {
  return(TCON * VOL * 1.E+3/CPW);
  }

double PRANF(double ETA, double CPW, double TCON)
/*
****************************************************************
*   Prandtl number PRANF                                       *
*                                                              *
*   INPUT: dynamic viscosity    ETA in E-6*KG/(M*S)            *
*          heat capacity        CPW in KJ/(KG*K)               *
*          thermal conductivity TCON in W/(K*M)                *
****************************************************************
*/
  {
  return(ETA * CPW * 1.E-3/TCON);
  }

double DHVF(double H1, double H2)
/*
****************************************************************
*   enthalpy of vaporization DHV in KJ/KG                      *
*   H1 = enthalpy of liquid phase in KJ/kg                     *
*   H2 = enthalpy of vapor phase in KJ/kg                      *
****************************************************************
*/
  {
  return(H2-H1);
  }

double ANUEF(double ETA, double V)
/*
****************************************************************
*  Kinematic viscosity ANUE in E-6*M**2/S                      *
*  ETA  ........................ in E-6*KG/(M*S)               *
*  V ........................... in M**3/KG                    *
****************************************************************
*/
  {
  return(ETA*V);
  }

double BL(double TR)
/*   ----------------------------------------------------------------
*  Calculation of L-FUNCTION according to IFC 67 section 8.2   *
----------------------------------------------------------------  */
  {
  double L[3];
  L[0] = 1.574373327E+1;
  L[1] = -3.417061978E+1;
  L[2] = 1.931380707E+1;

  return(L[0] + L[1]*TR + L[2]*TR*TR);
  }

double DBL(double TR)
/*    ----------------------------------------------------------------
* Calculation of first derivative of L-FUNCTION                *
* according to IFC 67 section 8.2                              *
----------------------------------------------------------------   */
  {
  static const double L[2]=
    {-34.17061978, 19.31380707, };
  //double L[2];
  //L[0] = -3.417061978E+1;
  //L[1] = 1.931380707E+1;

  return(L[0] + 2.*L[1]*TR);
  }

// ==========================================================================
//
//
//
// ===========================================================================

static double StmH(double T, double P)
  {
  //     *  temperature range: 273.16 K < T < 1073.15 K                 *
  //     *  pressure range : 0 < P < vapor pressure  bar                *
  //     *  Exception : for 623.15 < T < TC = 647.3 K                   *
  //     *              only  P < BL * PC (see L-FUNCTION BL)           *
  double ENTH;
  VSUB2(T, P, NULL, &ENTH); 
  return ENTH;
  }  
static double StmV(double T, double P)
  {
  //     *  temperature range: 273.16 K < T < 1073.15 K                 *
  //     *  pressure range : 0 < P < vapor pressure  bar                *
  //     *  Exception : for 623.15 < T < TC = 647.3 K                   *
  //     *              only  P < BL * PC (see L-FUNCTION BL)           *
  double VOL;
  VSUB2(T, P, &VOL, NULL); 
  return VOL;
  }  
static double WtrH(double T, double P)
  {
  //     *  temperature range: 273.16 K < T < 623.15 K                  *
  //     *  pressure range : vapor pressure < P < 1000 bar              *
  double ENTH;
  VSUB1(T, P, NULL, &ENTH); 
  return ENTH;
  }  
static double WtrV(double T, double P)
  {
  //     *  temperature range: 273.16 K < T < 623.15 K                  *
  //     *  pressure range : vapor pressure < P < 1000 bar              *
  double VOL;
  VSUB1(T, P, &VOL, NULL); 
  return VOL;
  }  

static CSurface2D SfVAPP("WaterVapourPressure", 
                         273.16,  623.0, 1000, NULL, NULL, 
                         VAPP, SfXFLogIn, SfXFLogOut);
static CSurface3D SfStmH("SteamEnthalpy", 
                         273.16, 1073.15, 200, NULL, NULL,
                         0.01, 221.0, 200, SfXFLogIn, SfXFLogOut, 
                         StmH, NULL);//, NULL, NULL);
static CSurface3D SfStmV("SteamVolume", 
                         273.16, 1073.15, 200, NULL, NULL,
                         0.01, 221.0, 200, SfXFLogIn, SfXFLogOut, 
                         StmV, NULL);//, NULL, NULL);
static CSurface3D SfWtrH("WaterEnthalpy", 
                         273.16, 623.15, 200, NULL, NULL,
                         0.01, 1000.0, 200, SfXFLogIn, SfXFLogOut, 
                         WtrH, NULL);//, NULL, NULL);
static CSurface3D SfWtrV("WaterVolume", 
                         273.16, 623.15, 200, NULL, NULL,
                         0.01, 1000.0, 200, SfXFLogIn, SfXFLogOut, 
                         WtrV, NULL);//, NULL, NULL);
static double SfCP(double TEMP, double PRES, int IAG)
  {
  double T1,T2,H1,H2;
  T1 = TEMP - 0.50;
  T2 = TEMP + 0.50;
  if (IAG==2)
    {
    H1=SfStmH(T1,PRES);
    H2=SfStmH(T2,PRES);
    }
  else
    {
    H1=SfWtrH(T1,PRES);
    H2=SfWtrH(T2,PRES);
    }
  return(H2-H1);
  }

// ==========================================================================
//
//
//
// ===========================================================================

H2OEQNBEGIN(IAF67_WaterHeatCapacity, "IAF67WaterCp")
virtual double AtTP(double Temp, double Press) 
  { 
  Temp=Range(273.16+1, Temp, 623.15-1);
  //if (ForceSatP) Press=100*VAPP(Temp);
  return 18.01528*SfCP(Temp, Range(0.01, Press*0.01, 1000.0), 1);
  };
H2OEQNEND

H2OEQNBEGIN(IAF67_SteamHeatCapacity, "IAF67SteamCp")
virtual double AtTP(double Temp, double Press)
  { 
  Press*=0.01; 
  double Tkr=Range(273.16, Temp, 1073.15);
  double Vp=SfVAPP(Tkr); 
  double Pr=Range(0.01, Press, Vp); 
  //if (ForceSatP) Pr=VAPP(Tkr);
  //Temp=Range(273.16+1, Temp, 1073.15-1);
  //if (ForceSatP) Press=100*VAPP(Temp);

  return 18.01528*SfCP(Tkr, Pr, 2);
  }
H2OEQNEND

H2OEQNBEGIN(IAF67_WaterEnthalpy, "IAF67WaterH")
virtual double AtTP(double Temp, double Press)
  {
  Press*=0.01; 
  double Tkr=Range(273.16, Temp, 623.15);
  double Vp=SfVAPP(Tkr);
  double Pr=Range(Vp, Press, 1000.0); 
  //if (ForceSatP) Pr=VAPP(Tkr);
  //  double ENTH;
  //  VSUB1(Tkr, Pr, NULL, &ENTH); 
  //  double HLiq = ENTH;
  double HLiq = SfWtrH(Tkr, Pr);
  if (Tkr!=Temp)
    HLiq += SfCP(Tkr, Pr, 1)*(Temp-Tkr);
  if (Pr!=Press)
    {
    double DP=(Pr>(Vp+1.0) ? -0.1 : 0.1);
    DP=Range(-Pr*0.01, DP, Pr*0.01);
    HLiq += (SfCP(Tkr, Pr+DP, 1)-SfCP(Tkr, Pr, 1))/DP*(Press-Pr);
    }
  return 18.01528*HLiq;
  }
H2OEQNEND

H2OEQNBEGIN(IAF67_SteamEnthalpy, "IAF67SteamH")
virtual double AtTP(double Temp, double Press)
  {
  Press*=0.01; 

  double Tkr=Range(273.16, Temp, 1073.15);
  double Vp=SfVAPP(Tkr); 
  double Pr=Range(0.01, Press, Vp); 
  //if (ForceSatP) Pr=VAPP(Tkr);
  //  double ENTH;
  //  VSUB2(Tkr, Pr, NULL, &ENTH); 
  //  double HVap = ENTH;
  double HVap=SfStmH(Tkr, Pr);
  if (Tkr!=Temp)
    HVap += SfCP(Tkr, Pr, 2)*(Temp-Tkr);
  if (Pr!=Press)
    {
    double DP=(Pr<(Vp-1.0) ? 0.1 : -0.1);
    DP=Range(-Pr*0.01, DP, Pr*0.01);
    double cp1=SfCP(Tkr, Pr+DP, 2);
    double cp2=SfCP(Tkr, Pr, 2);
    HVap += (cp1-cp2)/DP*(Press-Pr);
    }
  return 18.01528*HVap;
  }
H2OEQNEND

H2OEQNBEGIN(IAF67_WaterDensity, "IAF67WaterRho")
virtual double AtTP(double Temp, double Press)
  {
  Temp=Range(273.16, Temp, 623.15);
  //if (ForceSatP) Press=100*VAPP(Temp);
  //  double VOL;
  //  VSUB1(Temp, Range(VAPP(Temp), Press*0.01, 1000.0), &VOL, NULL);
  double VOL=SfWtrV(Temp, Range(SfVAPP(Temp), Press*0.01, 1000.0));
  return 1.0/VOL;
  }
H2OEQNEND

H2OEQNBEGIN(IAF67_SteamDensity, "IAF67SteamRho")
virtual double AtTP(double Temp, double Press)
  {
  Press*=0.01;
  double T=Range(273.16, Temp, 1073.15);
  double P=Range(0.01, Press, SfVAPP(T));
  //double VOL;
  //VSUB2(T, P, &VOL, NULL);
  double VOL=SfStmV(T,P);
  VOL*=(Temp/T)*(P/Press);
  return 1.0/VOL;
  }
H2OEQNEND

H2OEQNBEGIN(IAF67_WaterVapourPress, "IAF67WaterVp")
virtual double AtT(double Temp) 
  {
  return 100*SfVAPP(Range(273.16, Temp, 623.0));
  }
H2OEQNEND

// ==========================================================================
//
//
//
// ==========================================================================

bool TryConstructIAF(CSpecieData * pSpD, LPCTSTR Str, LPCTSTR &pn, CSpDBEqn* & p)
  {
  if (stricmp(Str, IAF67_WaterDensity::Name())==0) 
    {
    pn=IAF67_WaterDensity::Name();
    p=new IAF67_WaterDensity(pSpD);
    }
  else if (stricmp(Str, IAF67_SteamDensity::Name())==0) 
    {
    pn=IAF67_SteamDensity::Name();
    p=new IAF67_SteamDensity(pSpD);
    }
  else if (stricmp(Str, IAF67_WaterHeatCapacity::Name())==0) 
    {
    pn=IAF67_WaterHeatCapacity::Name();
    p=new IAF67_WaterHeatCapacity(pSpD);
    }
  else if (stricmp(Str, IAF67_SteamHeatCapacity::Name())==0) 
    {
    pn=IAF67_SteamHeatCapacity::Name();
    p=new IAF67_SteamHeatCapacity(pSpD);
    }
  else if (stricmp(Str, IAF67_WaterEnthalpy::Name())==0) 
    {
    pn=IAF67_WaterEnthalpy::Name();
    p=new IAF67_WaterEnthalpy(pSpD);
    }
  else if (stricmp(Str, IAF67_SteamEnthalpy::Name())==0) 
    {
    pn=IAF67_SteamEnthalpy::Name();
    p=new IAF67_SteamEnthalpy(pSpD);
    }
  else if (stricmp(Str, IAF67_WaterVapourPress::Name())==0) 
    {
    pn=IAF67_WaterVapourPress::Name();
    p=new IAF67_WaterVapourPress(pSpD);
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


DEFINE_TAGOBJ(IAFH2OProps);
class IAFH2OProps : public H2OPropertiesBlock
  {
  public:

    IAFH2OProps(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach);

    virtual ~IAFH2OProps();
    virtual void   BuildDataDefn(DataDefnBlk & DDB)     { };
    virtual flag   DataXchg(DataChangeBlk & DCB)        { return 0; };
    virtual flag   ValidateData(ValidateDataBlk & VDB)  { return 0; };

  };

//--------------------------------------------------------------------------

IMPLEMENT_TAGOBJ(IAFH2OProps, H2OPropsGroup, IFC67H2OName, "", "", TOC_SYSTEM, 
                 "IAF Props",
                 "IAF Props for Water & Steam");

IAFH2OProps::IAFH2OProps(pTagObjClass pClass_, pchar pTag, pTaggedObject pAttach, TagObjAttachment eAttach) :
H2OPropertiesBlock(pClass_, pTag, pAttach, eAttach)
  {
  m_Liq[0].m_pRho[0] = new IAF67_WaterDensity(NULL);
  m_Liq[0].m_pCp[0]  = new IAF67_WaterHeatCapacity(NULL);
  m_Liq[0].m_pH[0]   = new IAF67_WaterEnthalpy(NULL);
  m_Liq[0].m_pVp[0]  = new IAF67_WaterVapourPress(NULL);
  m_Vap[0].m_pRho[0] = new IAF67_SteamDensity(NULL);
  m_Vap[0].m_pCp[0]  = new IAF67_SteamHeatCapacity(NULL);
  m_Vap[0].m_pH[0]   = new IAF67_SteamEnthalpy(NULL);
  m_Vap[0].m_pVp[0]  = new IAF67_WaterVapourPress(NULL);

  m_Liq[1].m_pRho[0] = new IAF67_WaterDensity(NULL);
  m_Liq[1].m_pCp[0]  = new IAF67_WaterHeatCapacity(NULL);
  m_Liq[1].m_pH[0]   = new IAF67_WaterEnthalpy(NULL);
  m_Liq[1].m_pVp[0]  = new IAF67_WaterVapourPress(NULL);
  m_Vap[1].m_pRho[0] = new IAF67_SteamDensity(NULL);
  m_Vap[1].m_pCp[0]  = new IAF67_SteamHeatCapacity(NULL);
  m_Vap[1].m_pH[0]   = new IAF67_SteamEnthalpy(NULL);
  m_Vap[1].m_pVp[0]  = new IAF67_WaterVapourPress(NULL);
  }  

//--------------------------------------------------------------------------

IAFH2OProps::~IAFH2OProps()
  {
  for (int i=0; i<MaxFidelity; i++)
    {
    for (int e=0; e<SpDBEqnCnt; e++)
      {
      for (int j=0; j<MaxTPoints; j++)
        {
        delete m_Liq[i].m_pEqns[0][j];
        delete m_Vap[i].m_pEqns[0][j];
        m_Liq[i].m_pEqns[0][j]=NULL;
        m_Vap[i].m_pEqns[0][j]=NULL;
        }
      }
    }
  }

//--------------------------------------------------------------------------

