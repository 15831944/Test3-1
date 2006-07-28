//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "sc_defs.h"
#define  __CYCLONE_CPP
#include "Cyclone.h"
#include "dbgmngr.h"
//#include "optoff.h"

#define dbgModels  0
#if dbgModels
static CDbgMngr dbgCyclone("Models", "Cyclone");
#endif

//==========================================================================
//
//                             Cyclone Define Method
//
//==========================================================================

class CMDefine : public CycloneMeth
  {
  protected:
    double         m_CutSize;
    double         m_UFSolidsFrac;
    double         m_dP, m_Q;

  public:

    CMDefine (Cyclone * pC) :
      CycloneMeth(pC)
      {
      m_CutSize = 0.00005;
      m_dP      = 10.0;
      m_Q       = 100.0;
      }

    char*  MethName() { return "Df"; };

    void   BuildDataDefn0(DataDefnBlk & DDB, dword Flags)
      {
      //DDB.Text("");
      DDB.Double("Cut_Size",         "CutSize",      DC_L,    "um",     &m_CutSize,      m_pCyc, Flags|isParm);
      DDB.Double("UnderFlowSolids",  "UFSolids",     DC_Frac, "%",      &m_UFSolidsFrac, m_pCyc, Flags|isParm);
      DDB.Visibility(SM_DynBoth|HM_All);
      DDB.Double("",                 "PrssDrop",     DC_DP,   "kPa",    &m_dP,           m_pCyc, Flags|isParm);
      DDB.Double("",                 "PrssDropQ",    DC_Qm,   "kg/s",   &m_Q,            m_pCyc, Flags|isParm);
      DDB.Visibility();
      }

    void   BuildDataDefn1(DataDefnBlk & DDB, dword Flags)
      {
      DDB.Text("Results");
      }

    flag   DataXchg(DataChangeBlk & DCB)
      {
      return 0;
      }

    flag   ValidateData(ValidateDataBlk & VDB)
      {
      m_d50=Range(1.0e-6, m_CutSize, 1.0e-2);
      return true;
      }

    int    NPasses(Cyclone * pC) { return 1; };
    flag   Converged(int Pass) { return true; };
    void   Init(double Q, SpConduit & Fd) { }
    void   Term(SpConduit & Fd, SpConduit & Os, SpConduit & Us) { }
    double DP(double Q, SpConduit & Fd) { return m_dP*fabs(Q)/m_Q; };
    double D50() { return m_d50; }
    double OSizeFrac(double Size, double UFLiq, double OFLiq, int Pass)
      {
      double X, FLiq, Rf;
      X    = Size/m_d50;
      FLiq = UFLiq + OFLiq;
      if (FLiq > 1e-6)
        Rf = UFLiq/FLiq;
      else
        Rf = 0.0;
      if (X <= 10.0)
        {
        double OSize = (exp(4.0 * X) - 1.0) / (exp(4.0 * X) + 52.598);
        if (Pass>0)
          {
          OSize = Range(0.0, OSize + Rf*(1.0 - OSize), 1.0);
          }
        return OSize;
        }
      return 1.0;
      }
    double GetLiquidSplit(int Pass, SpConduit & Fd, double UFSol, double OFSol, double &UFLiq, double &OFLiq)
      {
      double QmLIn = Fd.QMass(som_Liq);
      if (QmLIn > 1.0e-6)
        {
        UFLiq    = UFSol / GTZ(m_UFSolidsFrac) - UFSol;
        OFLiq    = QmLIn - UFLiq;
        // Convert to % of feed
        UFLiq = Range(0.0, UFLiq / QmLIn, 1.0);
        OFLiq = 1.0 - UFLiq;
        }
      else
        {
        UFLiq = 0.0;
        OFLiq = 0.0;
        }
      return OFLiq/GTZ(QmLIn);
      }
  };

//==========================================================================
//
//                             Cyclone Krebs Method
//
//==========================================================================

class CMKrebs : public CycloneMeth
  {
  protected:
    double         m_Diam, m_UFSolidsFrac;
    double         m_Factor;
    double         m_ICut, m_DP;
    double         m_OFLiqCorrFac;

    double PercSolids(SpConduit &QFd)
      {
      double SolMs = QFd.QMass(som_Sol);
      double LiqMs = QFd.QMass(som_Liq);
      if ((SolMs > 1e-6) && (LiqMs > 1e-6))
        {
        double SolVol = SolMs / QFd.Rho(som_Sol);
        double LiqVol = LiqMs / QFd.Rho(som_Liq);
        double V = SolVol / (LiqVol + SolVol);

        double Cr1 = pow((53.0 - V)/53.0, -1.43);
        return Cr1;
        }
      else
        return 1.0;
      }

    double SpecGravity(SpConduit &QFd)
      {
      double SolSG = QFd.Rho(som_Sol) / 1000.0;
      double LiqSG = QFd.Rho(som_Liq) / 1000.0;
      if (SolSG > LiqSG)
        {
        double Cr3 = sqrt(1.65 /(SolSG - LiqSG));
        return Cr3;
        }
      else
        return 1.0;
      }

    double Intersection(SpConduit &QUs, SpConduit &QOs)
      {
      SQSzDist1 &USz=*(SQSzDist1::Ptr(QUs.Model()));
      SQSzDist1 &OSz=*(SQSzDist1::Ptr(QOs.Model()));

      double MassRange = 0.0, Cut;
      for (int d=0; d<USz.NDistributions(); d++)
        if (USz.DistExists(d))
          {
          CSD_Distribution &D1 = USz.Dist(d);
          CSD_Distribution &D2 = OSz.Dist(d);
          CDArray Tons, CutPt;
          int n = D1.NPriIds();
          Tons.SetSize(n);
          CutPt.SetSize(n);
          for (int s=0; s<D1.NPriIds(); s++)
            {
            CDArray & Size1 = D1.PriSp[s]->FracPass;
            CDArray & Size2 = D2.PriSp[s]->FracPass;
            long len = Size1.GetSize();
            CDArray YTemp1, YTemp2, USize, OSize, ROSize;
            YTemp1.SetSize(len);
            YTemp2.SetSize(len);
            USize.SetSize(len);
            OSize.SetSize(len);
            ROSize.SetSize(len);

            if (len >0.0)
              {
              double Fr1 = 0.0, Fr2 = 0.0;
              int top;

              for(long i=0 ; i<len ; i++)
                {
                Fr1 += Size1[i];
                Fr2 += Size2[i];
                double s1 = Size1[i], s2 = Size2[i];
                }
              if (Fr1 <= 0.0)
                Fr1 = 1.0;
              if (Fr2 <= 0.0)
                Fr2 = 1.0;

              Tons[s] = 0.0;
              for (int s1=0; s1<D1.NSecIds(s); s1++)
                Tons[s]+= QOs.VMass[D1.SzId(s,s1)] + QUs.VMass[D1.SzId(s,s1)];

              for(i=0 ; i<len ; i++)
                {
                YTemp1[i] = Size1[i] / Fr1;
                YTemp2[i] = Size2[i] / Fr2;
                }

              USize[1] = YTemp1[0];
              OSize[1] = YTemp2[0];
              ROSize[1] = 1 - OSize[1];
              for (i=2; i<len; i++)
                {
                USize[i] = USize[i-1] + YTemp1[i-1];
                OSize[i] = OSize[i-1] + YTemp2[i-1];
                ROSize[i] = 1 - OSize[i];
                }

              for (i=2; i<len; i++)
                {
                double U1 = USize[i], O1 = ROSize[i];
                if (U1 >= O1)
                  {
                  top = i;
                  i = len;
                  }
                }

              if ((top >= 2) && (top < len))
                {
                // First determine linear equation for under size
                double x1, x2, y1, y2, Um, Uc, Om, Oc;
                x1 = D1.Intervals()[top-1];
                x2 = D1.Intervals()[top];
                y1 = USize[top-1];
                y2 = USize[top];

                Um = (y2 - y1)/GTZ(x2 - x1);
                Uc = y2 - Um * x2;

                // Linear equation for over size
                x1 = D2.Intervals()[top-1];
                x2 = D2.Intervals()[top];
                y1 = ROSize[top-1];
                y2 = ROSize[top];

                Om = (y2 - y1)/GTZ(x2 - x1);
                Oc = y2 - Om * x2;

                // Now find intersection of the lines
                Cut = (Oc - Uc)/GTZ(Um - Om);
                }
              else
                Cut = 0.0;
              CutPt[s] = Cut;
              }
            }
          // Calculate weighted avarage cut-point
          double TotalTons = 0.0;
          Cut = 0.0;
          for (s=0; s<D1.NPriIds(); s++)
            {
            Cut+= CutPt[s] * Tons[s];
            TotalTons+= Tons[s];
            }
          if (TotalTons > 1e-12)
            Cut = Cut/TotalTons;
          else
            Cut = 0.0;
          }
        return Cut;
      }

  public:

    CMKrebs(Cyclone * pC) :
      CycloneMeth(pC)
      {
      m_UFSolidsFrac = 0.5;
      m_Diam         = 0.1;
      m_Factor       = 1.0;
      m_ICut         = 0.00005;
      m_DP           = 100.0;
      m_OFLiqCorrFac = 1.0;
      };

    char*  MethName() { return "Kr"; };

    void   BuildDataDefn0(DataDefnBlk & DDB, dword Flags)
      {
      DDB.Double("CycloneDiam",      "Diam",         DC_L,    "mm",     &m_Diam,         m_pCyc, Flags|isParm);
      DDB.Double("CorrectionFactor", "Factor",       DC_,     "",       &m_Factor,       m_pCyc, Flags|isParm);
      DDB.Double("UnderFlowSolids",  "UFSolids",     DC_Frac, "%",      &m_UFSolidsFrac, m_pCyc, Flags|isParm);
      DDB.Double("OverFlowLiqCorrFac","OFLiqCorrFac",DC_,     "",       &m_OFLiqCorrFac, m_pCyc, Flags|isParm);
      }
    void   BuildDataDefn1(DataDefnBlk & DDB, dword Flags)
      {
      DDB.Text("Results");
      DDB.Double("",                 "Calc_d50",     DC_L,    "um",     &m_d50,          m_pCyc, Flags|isResult); //tag should be d50
      DDB.Double("",                 "InterSectPt",  DC_L,    "um",     &m_ICut,         m_pCyc, Flags|isResult);
      DDB.Double("Pressure_Drop",    "CalcDP",       DC_DP,   "kPa",    &m_DP,           m_pCyc, Flags|isResult); //tag should be dP
      }
    flag   DataXchg(DataChangeBlk & DCB)
      {
      return 0;
      };
    flag   ValidateData(ValidateDataBlk & VDB)
      {
      m_UFSolidsFrac = Range(0.001, m_UFSolidsFrac, 0.999);
      return true;
      };
    int    NPasses() { return 2; };
    flag   Converged(int Pass) { return Pass>0; };
    void   Init(double Q, SpConduit & Fd)
      {
      m_DP = DP(Fd.QMass(), Fd);

      double Diamcm = m_Diam * 100.0;               // Convert diam to cm
      double d50Base = m_Factor * (2.84 * pow(Diamcm,0.66)) * 1e-6;  // Convert to m

      double C1 = PercSolids(Fd);
      double C3 = SpecGravity(Fd);
      double C2 = 3.27 * pow(m_DP, -0.28);

      m_d50 = d50Base * C1 * C3 * C2;
      }
    void   Term(SpConduit & Fd, SpConduit & Os, SpConduit & Us)
      {
      m_ICut = Intersection(Us, Os);
      }

    double DP(double Qm, SpConduit & Fd)
    /* Use the Plitt method to calculate pressure drop, using 'Standard'
       cyclone dimensions.*/
      {
      double Qmc = Qm/Max(m_pCyc->dCycNo, 0.001);
      double Dc  = m_Diam * 100.0;
      double Di  = 0.2 * Dc;
      double Du  = 0.15 * Dc;
      double Do  = 0.3 * Dc;
      double h   = 1.5 * Dc;

      double Qv  = Qmc/Fd.Rho()*1000.0*60.0;
      double Phi = Fd.VolFrac(som_Sol, Std_T, Std_P);

      double N = 1.88*Pow(Qv, 1.78)*Exps(0.0055*Phi*100.0);
      double D = Pow(Dc, 0.37)*Pow(Di, 0.94)*Pow(h, 0.28)*Pow(Sqr(Du)+Sqr(Do), 0.87);
      return N/GTZ(D);
      };

    double D50() { return m_d50; };
    double OSizeFrac(double Size, double UFLiq, double OFLiq, int Pass)
      {
      double X, FLiq, Rf;
      X    = Size/m_d50;
      FLiq = UFLiq + OFLiq;
      if (FLiq > 1e-6)
        Rf = UFLiq/FLiq;
      else
        Rf = 0.0;
      if (X <= 10.0)
        {
        double OSize = (exp(4.0 * X) - 1.0) / (exp(4.0 * X) + 52.598);
        if (Pass>0)
          {
          OSize = Range(0.0, OSize + Rf*(1.0 - OSize), 1.0);
          }
        return OSize;
        }
      return 1.0;
      }
    double GetLiquidSplit(int Pass, SpConduit & Fd, double UFSol, double OFSol, double &UFLiq, double &OFLiq)
      {
      double QmLIn = Fd.QMass(som_Liq);
      if (QmLIn > 1.0e-6)
        {
        UFLiq    = UFSol / GTZ(m_UFSolidsFrac) - UFSol;
        OFLiq    = QmLIn - UFLiq;
        // Convert to % of feed
        UFLiq = Range(0.0, UFLiq / QmLIn, 1.0);
        OFLiq = 1.0 - UFLiq;
        OFLiq = Range(0.0, OFLiq*m_OFLiqCorrFac, 1.0);
        UFLiq = 1.0 - OFLiq;
        }
      else
        {
        UFLiq = 0.0;
        OFLiq = 0.0;
        }
      return OFLiq/GTZ(QmLIn);
      }

  };

//==========================================================================
//
//                             Cyclone Plitt Method
//
//==========================================================================

const byte SE_RosinRammler = 0;
const byte SE_Lynch        = 1;

class CMPlitt : public CycloneMeth
  {
  protected:
    byte           iSharpEqn;

    double         m_Di, m_SIDi;
    double         m_Do, m_SIDo;
    double         m_Du, m_SIDu;
    double         m_Dc, m_SIDc;
    double         m_h, m_SIh;
    double         m_Scale;
    double         m_Factor1_d50;
    double         m_Factor2_Sharp;
    double         m_Factor3_DP;
    double         m_Factor4_S;
    double         m_Factor5_OFLiq;
    bool           m_OldCalcs;
    double         m_Phi;
    double         m_Qvc;
    double         m_S;
    double         m_H;
    double         m_m;
    double         m_Alpha;
    double         m_DP;
    double         m_Rv;
    double         m_Rs;
    double         m_Rf, m_RfMem;   // This is a value which needs to be converged

  public:

    CMPlitt(Cyclone * pC) :
      CycloneMeth(pC)
      {
      iSharpEqn=SE_Lynch;
      m_SIDc=6.0   * 0.0254;
      m_SIDo=1.0   * 0.0254;
      m_SIDu=0.707 * 0.0254;
      m_SIDi=1.09  * 0.0254;
      m_SIh= 17.0  * 0.0254;
      m_OldCalcs = (PrjFileVerNo()<=82 ? true : 0);

      m_S = 0.0;
      m_H = 0.0;
      m_Alpha = 0.0;
      m_m = 0.0;
      m_Rv = 0.1;
      m_Rf = 0.1;
      m_RfMem = 0.0;
      m_Factor1_d50 = 1.0;
      m_Factor2_Sharp = 1.0;
      m_Factor3_DP = 1.0;
      m_Factor4_S = 1.0;
      m_Factor5_OFLiq = 1.0;

      m_Scale=dNAN;
      };

    char*  MethName() { return "Pl"; };

    void   BuildDataDefn0(DataDefnBlk & DDB, dword Flags)
      {
      DDB.Double("Cyclone_Diameter",   "Dc",           DC_L,    "mm",     &m_SIDc,   m_pCyc, Flags|isParm);
      DDB.Double("Inlet_Diameter",     "Di",           DC_L,    "mm",     &m_SIDi,   m_pCyc, Flags|isParm);
      DDB.Double("Vortex_Diameter",    "Do",           DC_L,    "mm",     &m_SIDo,   m_pCyc, Flags|isParm);
      DDB.Double("Spigot_Diameter",    "Du",           DC_L,    "mm",     &m_SIDu,   m_pCyc, Flags|isParm);
      DDB.Double("Height",             "h",            DC_L,    "mm",     &m_SIh,    m_pCyc, Flags|isParm);
      DDB.Double("Scale",              "",             DC_Frac, "",       &m_Scale,  m_pCyc, Flags|isParm|NAN_OK);
      DDB.CheckBoxBtn("OldCalcs",      "",             DC_,     "",       &m_OldCalcs, m_pCyc, Flags|isParm);
      DDB.Double("d50_Factor1",        "Factor",       DC_,     "",       &m_Factor1_d50, m_pCyc, Flags|isParm);
      DDB.Double("Sharp_Factor2",      "SharpFactor",  DC_,     "",       &m_Factor2_Sharp, m_pCyc, Flags|isParm);
      DDB.Double("Pressure_Factor3",   "PressFactor",  DC_,     "",       &m_Factor3_DP, m_pCyc, Flags|isParm);
      DDB.Double("S_Factor4",          "SFactor",      DC_,     "",       &m_Factor4_S, m_pCyc, Flags|isParm);
      DDB.Double("OFLiq_Factor5",      "OFLiqCorrFac", DC_,     "",       &m_Factor5_OFLiq, m_pCyc, Flags|isParm);
      static DDBValueLst DDB0[]={
        {SE_RosinRammler,  "Rosin-Rammler" },
        {SE_Lynch,         "Lynch", },
        {0}};
      DDB.Byte  ("SharpEqn",                 "",       DC_,     "",       &iSharpEqn,     m_pCyc, Flags|isParm, DDB0);
      }

    void   BuildDataDefn1(DataDefnBlk & DDB, dword Flags)
      {
      DDB.Text("Results");
      //DDB.Double("",    "RefTemp",      DC_T,       "C",     &gs_StdTemp, m_pCyc, Flags|isResult|noFileAtAll|InitHidden);
      //DDB.Double("",    "RefPress",     DC_P,       "kPa",   &gs_StdPress, m_pCyc, Flags|isResult|noFileAtAll|InitHidden);
      DDB.Double("",    "QvPerCyclone", DC_Qv,      "m^3/s", &m_Qvc,   m_pCyc, Flags|isResult);
      DDB.Double("",    "VolFrac",      DC_Frac,    "%",     &m_Phi,   m_pCyc, Flags|isResult|InitHidden);
      DDB.Double("",    "d50",          DC_L,       "um",    &m_d50,   m_pCyc, Flags|isResult);
      DDB.Double("",    "PressDrop",    DC_DP,      "kPa",   &m_DP,    m_pCyc, Flags|isResult);  //tag should be dP
      DDB.Double("",    "S",            DC_,        "",      &m_S,     m_pCyc, Flags|isResult);
      DDB.Double("",    "Rv",           DC_Frac,    "%",     &m_Rv,    m_pCyc, Flags|isResult);
      DDB.Double("Sharpness", "m",      DC_,        "",      &m_m,     m_pCyc, Flags|isResult);
      DDB.Double("",    "Alpha",        DC_,        "",      &m_Alpha, m_pCyc, Flags|isResult|InitHidden);
      DDB.Double("",    "Rs",           DC_Frac,    "%",     &m_Rs,    m_pCyc, Flags|isResult);
      DDB.Double("",    "Rf",           DC_Frac,    "%",     &m_Rf,    m_pCyc, Flags|isResult);
      }

    flag   DataXchg(DataChangeBlk & DCB)
      {
      return 0;
      };

    flag   ValidateData(ValidateDataBlk & VDB)
      {
      if (Valid(m_Scale))
        {
        m_SIDc*=m_Scale;
        m_SIDo*=m_Scale;
        m_SIDu*=m_Scale;
        m_SIDi*=m_Scale;
        m_SIh*=m_Scale;
        m_Scale=dNAN;
        }
      m_Dc= 100.0*m_SIDc;
      m_Do= 100.0*m_SIDo;
      m_Du= 100.0*m_SIDu;
      m_Di= 100.0*m_SIDi;
      m_h=  100.0*m_SIh;

      return true;
      };

    int    NPasses() { return 20; };
    flag   Converged(int Pass)
      {
      flag OK=fabs(m_Rf-m_RfMem)<1.0e-6;
      m_RfMem=m_Rf;
      return OK;
      };

    bool UseGeomMean() { return (!m_OldCalcs && iSharpEqn==SE_Lynch); };

    void   Init(double QmTtl, SpConduit & Fd)
      {
      const double FeedTemp = Fd.Temp();
      const double FeedPress = Fd.Press();
      const double RefTemp = (m_OldCalcs ? gs_StdTemp : FeedTemp); // Reference temperature
      const double RefPress = (m_OldCalcs ? gs_StdPress : FeedPress); // Reference pressure
      m_Phi = Fd.VolFrac(som_Sol, RefTemp, RefPress);  //kga 28/07/06: Changed to feed stream temperature & Pressure

      double Qv = GTZ(QmTtl)/GTZ(Fd.Rho());
      m_pCyc->dCycNo = Max(m_pCyc->dCycNo, 0.001);
      m_Qvc = Qv/m_pCyc->dCycNo;
      Qv = Qv*1000.0*60.0;     // convert to l/min
      const double Qvc = Qv/m_pCyc->dCycNo;  // l/min
      double RhoS=0.001*Fd.Rho(som_Sol);     // g/cc
      double RhoL=0.001*Fd.Rho(som_Liq);     // g/cc
      double d50N=50.5*Pow(m_Dc, 0.46)*Pow(m_Di, 0.6)*Pow(m_Do, 1.21)*Exps(0.063*m_Phi*100.0);
      double d50D;
      if (m_OldCalcs)
        d50D=Pow(m_Du, 0.71)*Pow(m_h, 0.38)*Pow(Qvc, 0.46)*Pow((RhoS-RhoL), 0.5);
      else
        d50D=Pow(m_Du, 0.71)*Pow(m_h, 0.38)*Pow(Qvc, 0.45)*Pow((RhoS-RhoL), 0.5); //kga 19/03/06: changed Qvc power to 0.45 from 0.46
      m_d50=m_Factor1_d50*1.0e-6*d50N/GTZ(d50D);

      m_DP=DP(Fd.QMass(), Fd);
      m_H=Press2Head(m_DP, Fd.Rho());

      double SN=1.9*Pow(m_Du/m_Do, 3.31)*Pow(m_h, 0.54)*Pow(Sqr(m_Du)+Sqr(m_Do), 0.36)*Exps(0.0054*m_Phi*100.0);
      double SD=Pow(m_H, 0.24)*Pow(m_Dc, 1.11);
      m_S=m_Factor4_S*SN/GTZ(SD);

      m_Rv=m_S/(m_S+1);
      if (m_OldCalcs)
        m_m=m_Factor2_Sharp*1.94*Exps(-1.58*m_Rv)*Pow(Sqr(m_Dc)*m_h/Qv, 0.15);
      else
        m_m=m_Factor2_Sharp*1.94*Exps((-1.58*m_Rv)*Pow(Sqr(m_Dc)*m_h/Qvc, 0.15)); //kga 19/03/06: Brackets for exp part was WRONG!!!; Also should have used Qvc!!!

      m_Alpha=1.54*m_m-0.47;
      //m_Rf=m_Rv; // Initial Guess
      }

    void   Term(SpConduit & Fd, SpConduit & Os, SpConduit & Us)
      {
      }

    double DP(double Qm, SpConduit & Fd)
      {
      const double Qv  = Qm/Fd.Rho()*1000.0*60.0; // l/min
      const double Qvc = Qv/m_pCyc->dCycNo; // l/min
      const double RefTemp = (m_OldCalcs ? gs_StdTemp : Fd.Temp());
      const double RefPress = (m_OldCalcs ? gs_StdPress : Fd.Press());
      const double Phi = Fd.VolFrac(som_Sol, RefTemp, RefPress);

      const double N = 1.88*Pow(Qvc, 1.78)*Exps(0.0055*Phi*100.0);
      const double D = Pow(m_Dc, 0.37)*Pow(m_Di, 0.94)*Pow(m_h, 0.28)*Pow(Sqr(m_Du)+Sqr(m_Do), 0.87);
      return m_Factor3_DP*N/GTZ(D);
      };

    double D50() { return m_d50; };

    double OSizeFrac(double Size, double UFLiq, double OFLiq, int Pass)
      {
      double YP;
      switch (iSharpEqn)
        {
        case SE_RosinRammler:
          {
          YP=1.0-Exps(-0.693*Pow(Size/m_d50, m_m));
          break;
          }
        case SE_Lynch:
          {
          const double A=Exps(m_Alpha*Size/m_d50);
          YP = (A-1.0)/(A+Exps(m_Alpha)-2.0);
          break;
          }
        default:
          {
          YP=1.0;
          break;
          }
        }
      const double Y = YP + m_Rf*(1.0-YP);
      return Y;
      }

    double GetLiquidSplit(int Pass, SpConduit & Fd, double UFSol, double OFSol, double &UFLiq, double &OFLiq)
      {
      if ((1.0-m_Phi) > 1e-6)
        {
        //double QmLIn = Fd.QMass(som_Liq);
        //m_Rv=m_S/(m_S+1.0);
        m_Rs=UFSol/GTZ(UFSol+OFSol);
        const double Damp=0.0;
        const double m_RfRqd=(m_Rv-m_Rs*m_Phi)/(1.0-m_Phi);
        m_Rf = Damp*m_Rf+(1.0-Damp)*Range(0.001, m_RfRqd, 0.999);
        UFLiq = m_Rf/(1.0+m_Rf);
        OFLiq = 1.0-UFLiq;
        OFLiq = Range(0.0, OFLiq*m_Factor5_OFLiq, 1.0);
        UFLiq = 1.0 - OFLiq;
        }
      else
        {
        UFLiq = 0.0;
        OFLiq = 0.0;
        m_Rf = 0.0;
        }
      return m_Rf;
      }
  };

//===========================================================================
//
//                                 Cyclone
//
//==========================================================================
/*#D:#T:Cyclone
#X:#h<General Description>#nThe Cyclone is used to seperate solids
and liquids into two product streams.  The separation is based on size distribution
curves and the d50 of the Cyclone.
#nThe feed to the Cyclone must have a size distribution. The Cyclone will then
manipulate the curve to produce two products, an underflow fraction and an overflow fraction.

#n#nThree options exist for use with the Cyclone:
#na) Define the d50 of the Cyclone.
#nb)Calculate the d50 of the Cyclone based on the Krebs model.  The following equations are used
to calculate the d50 :
#n#nD50 = D50(base) #* C1 #* C2 #* C3 where :

#n#nD50(base) = 2.84 #* D^0.66 where
#nD50(base) = the micron  cut size that a "standard cyclone" can achieve
#nD = Cyclone diameter in cm

#n#nC1 = ((53 - V)/53)^-1.43 where
#nC1 = Correction for the influence of cyclone feed concentration
#nV  = Percent solids by volume in cyclone feed

#n#nC2 = 3.27 #* dP^-0.28 where
#nC2 = Correction for the influence of pressure drop
#ndP = Pressure drop across the cyclone in kPa

#n#nC3 = (1.65/(Gs - Gl))^0.5 where
#nC3 = Correction for the influence of SG
#nGs = SG of solids
#nGl = SG of liquids

#nUsing the d50 (either calculated or defined above), the amount of material in each size
fraction in the feed size distribution curve which will report to the underflow is
computed.  The difference reports to the overflow.  The percentage solids in the
underflow is defined by the user.  The model uses this number to calculate the amount
of liquid reporting to the underflow.  The balance of the feed liquid flows to the
overflow.
#nThe model also takes into account the amount of material which is entrained with the
liquid and is displaced to the cyclone underflow.  The following formula is used to
determine the offset from perfect separation :
#nU(x)  =  U'(x) + Lf #* Us(1 - U'(x))
#nwhere U(x) - percentage of particles of size x in the feed reporting to the underflow
#nU'(x) - percentage of particles reporting to under flow for a perfect separation
#nUs    - percentage of feed liquid reporting to the under flow
#nLf    - Liquid fraction reporting to the underflow.

#n#nThe alogorithms used in the Krebs model were obtained from the following paper :
#nThe Sizing and Selection of Hydrocyclones.  Richard A. Arterburn

#n#nc)Calculate the d50 based on the Plitt model.
#nThe alogorithms used in the Plitt model were obtained from the following paper :
#n3. L.R.Plitt., "A mathematical model of the hydrocyclone classifier", CIM Bulletin,
December 1976.#n

#n#h<Variables to be supplied by the user>#n
#n#i<Method:> The user must specify which method is to be used to define the cyclone.
Depending upon which method is used, the user may have to input the cyclone cutpoint, or
the cyclone dimensions.
#n'Krebs' which will calculate the d50 based on the Krebs model, or
#n'Plitt' which will calculate the d50 based on the Plitt model, or
#n'd50Reqd' in which case the user must insert the required d50.
#nIf the user selects either the Krebs or the Plitt model, the cyclone parameters need
to be supplied :
#n#i<Diam:> The diameter of the cyclone.
#n#i<Factor:> Krebs only.  The user may supply a factor to adjust the base d50 calculated
by the model.  This may be useful if the dimensions of the cyclone are different from a
'standard' cyclone.  The default value is 1.
#n#i<UFSolids:> This is required for the d50Reqd and the Krebs method.  The user must
specify the percentage solids in the underflow stream.
#nThe following parameters are required for the Plitt method:
#n#i<Di:> The inlet diameter of the cyclone.
#n#i<Do:> The overflow, or vortex, diameter of the cyclone.
#n#i<Du:> The underflow, or spigot, diameter of the cyclone.
#n#i<h:> The free vortex height of the cyclone.
#n#i<Datum required:> The relative elevation of the Cyclone with respect to
the common datum of the system.

#n#n#h<Variables calculated by the model>#n
#n#i<Datum:> The height of the Cyclone above the system datum.
#n#i<IntersectCut:> The particle size at which the cummulative percentage oversize
of the coarse product (the underflow) is equal to the cummulative percentage undersize
of the fine product (the overflow).
this figure, depending on the percent solids in the feed and the solids SG.
#n#i<OFDens:> The density of the material, solids and liquids, which reports to
the Cyclone overflow.
#n#i<UFDens:> The density of the material, solids and liquids, which reports to
the Cyclone underflow.

#n#h<Other>#n
Default model prefix:CYC#n
Short name:Cyclone#n
Model type:Unit#n
#G:Units
*/
//==========================================================================

const byte CM_Define = 0;
const byte CM_Krebs  = 1;
const byte CM_Plitt  = 2;

const byte idFeed  = 0;
const byte idOF    = 1;
const byte idUF    = 2;

static IOAreaRec CycloneIOAreaList[] =
  {{"Cyclone Feed",      "Feed"      , idFeed,  LIO_In0 ,  nc_MLnk, 1, 1,            0, 0.0F},
   {"Cyclone Overflow",  "OverFlow"  , idOF,    LIO_Out0,  nc_MLnk, 1, 1,            0, 0.0F},
   {"Cyclone Underflow", "UnderFlow" , idUF,    LIO_Out1,  nc_MLnk, 1, 1,            0, 0.0F}, //Out.Out"},
   {NULL}};

static double Drw_Cyclone[] = { DD_Poly, -8,4, -8,12, 8,12, 8,4, 4,-12, -4,-12, -8,4, 8,4,
                           DD_Poly, BOX(-4,12, 8,4),
                           DD_End };

//--------------------------------------------------------------------------

IMPLEMENT_MODELUNIT(Cyclone, "Cyclone", "1", Drw_Cyclone, "SizeSeparation", "CYC", TOC_ALL|TOC_GRP_SIZEDST|TOC_SIZEDIST,
                    "Size Separation:Cyclone", "")
Cyclone::Cyclone(pTagObjClass pClass_, pchar TagIn, pTaggedObject pAttach, TagObjAttachment eAttach) :
  MdlNode(pClass_, TagIn, pAttach, eAttach),
  QFd("QFd",     this, TOA_Embedded),
  QTemp("QTemp", this, TOA_Embedded),
  QFine("QFine", this, TOA_Embedded),
  PartCrv("PartCrv", this, TOA_Embedded)
  {
  AttachClassInfo(nc_Process, CycloneIOAreaList);
  iMethod  = CM_Plitt;
  memset(Meth, 0, sizeof(Meth));

  OFDens = UFDens = UFSol = UFLiq = OFSol = OFLiq = 0.0;

  Meth[CM_Define] = new CMDefine(this);
  Meth[CM_Krebs]  = new CMKrebs(this);
  Meth[CM_Plitt]  = new CMPlitt(this);

  dCycNo = 1.0;

  PartCrv.SetNCurves(Max(1, SD_Defn.NPriIds(0)));
  for (int c=0; c<PartCrv.NCurves(); c++)
    PartCrv.SetEditable(c, false);
  PartCrv.SetLength(3);
  PartCrv.SetModeVisibility(true, "Passing", "Retained");
  PartCrv.SetStateVisibility(false, "", "");
  };

//--------------------------------------------------------------------------

Cyclone::~Cyclone ()
  {
  for (int i=0; i<MaxCycMeths; i++)
    {
    delete Meth[i];
    Meth[i]=NULL;
    }
  };

//--------------------------------------------------------------------------

void Cyclone::BuildDataDefn(DataDefnBlk & DDB)
  {
  dword VwDefine = (iMethod==CM_Define) ? 0 : noView;
  dword VwKrebs  = (iMethod==CM_Krebs)  ? 0 : noView;
  dword VwPlitt  = (iMethod==CM_Plitt)  ? 0 : noView;

  DDB.BeginStruct(this);
  DDB.Text    ("Requirements");
  DDB.Visibility(SM_DynBoth|HM_All);
  BuildDataDefnElevation(DDB);
  DDB.Visibility();

  static DDBValueLst DDB0[]={
    {CM_Define,  "DefineCutSize" },
    {CM_Krebs,   "CalcCutSize", MDD_Hidden},
    {CM_Krebs,   "Krebs", },
    {CM_Plitt,   "Plitt" },
    {0}};
  DDB.Byte  ("SplitMethod",      "Method",       DC_,     "",       &iMethod,  this, isParm, DDB0);

  DDB.Text("");
  //DDB.Long  ("No_of_Cyclones",   "NumCyclones",  DC_,     "",       &CycNo,    this, isParm);
  DDB.Double("No_of_Cyclones",   "NumCyclones",  DC_,     "",       &dCycNo,    this, isParm);

  for (int i=0; i<MaxCycMeths; i++)
    if (Meth[i])
      {
      dword Flags=(i==iMethod ? 0 : noView);
      if (PrjFileVerNo()<22)
        {
        DDB.PushUserInfo(i+1);
        Meth[i]->BuildDataDefn0(DDB, Flags);
        Meth[i]->BuildDataDefn1(DDB, Flags);
        DDB.PopUserInfo();
        }
      else
        {
        DDB.BeginStruct(this, Meth[i]->MethName(), NULL, DDB_NoPage, i+1);
        Meth[i]->BuildDataDefn0(DDB, Flags);
        Meth[i]->BuildDataDefn1(DDB, Flags);
        DDB.EndStruct();
        }
      }

  // Results
  DDB.Text("");
  DDB.Double("OF_Liq_Split",     "OFLiqSplit",   DC_Frac, "%",      &OFLiq,    this, isResult);
  DDB.Double("UF_Liq_Split",     "UFLiqSplit",   DC_Frac, "%",      &UFLiq,    this, isResult|InitHidden);
  DDB.Double("OF_Density",       "OFDens",       DC_Rho,  "kg/m^3", &OFDens,   this, isResult);
  DDB.Double("UF_Density",       "UFDens",       DC_Rho,  "kg/m^3", &UFDens,   this, isResult);

  //for (i=0; i<MaxCycMeths; i++)
  //  {
  //  if (Meth[i])
  //    {
  //    if (PrjFileVerNo()<22)
  //      {
  //      DDB.PushUserInfo(i+1);
  //      Meth[i]->BuildDataDefn1(DDB, i==iMethod ? 0 : noView);
  //      DDB.PopUserInfo();
  //      }
  //    else
  //      {
  //      Strng Tg(Meth[i]->MethName());
  //      Tg+="D";
  //      DDB.BeginStruct(this, Tg(), NULL, DDB_NoPage, i+1);
  //      Meth[i]->BuildDataDefn1(DDB, i==iMethod ? 0 : noView);
  //      DDB.EndStruct();
  //      }
  //    }
  //  }

  BuildDataDefnShowIOs(DDB);

  DDB.Object(&PartCrv, this, NULL, NULL, DDB_RqdPage);

  DDB.EndStruct();
  };

// --------------------------------------------------------------------------

flag Cyclone::DataXchg(DataChangeBlk & DCB)
  {
  if (MdlNode::DataXchg(DCB))
    return 1;
  if (Meth[iMethod]->DataXchg(DCB))
    return 1;
  return 0;
  }

// --------------------------------------------------------------------------

flag Cyclone::ValidateData(ValidateDataBlk & VDB)
  {
//TODO TOGO
  if (Meth[iMethod]==NULL)
    iMethod=0;

  flag OK=MdlNode::ValidateData(VDB);
  OK = OK && Meth[iMethod]->ValidateData(VDB);
  return OK;
  }

// -------------------------------------------------------------------------

flag Cyclone::ChangeMode(dword NewMode, dword OldMode)
  {
  if (NewMode==SM_Direct)
    GSM.Open();

  return True;
  };

//---------------------------------------------------------------------------

void Cyclone::ConfigureJoins()
  {
  Init_NoJoins(1);
  int i;
  switch (NetMethod())
    {
    case NM_Probal:
      for (i=0; (i<NoProcessIOs()); i++)
        SetIO_Join(i, 0);
      break;
    case NM_Dynamic:
      for (i=0; (i<NoProcessIOs()); i++)
          SetIO_Join(i, 0);
      break;
    }
  };

//--------------------------------------------------------------------------

void Cyclone::EvalJoinPressures(long JoinMask)
  {
  for (int i = 0; i < NoFlwIOs(); i++)
    Set_IOP_Self(i,Std_P);
  };

//--------------------------------------------------------------------------

flag Cyclone::EvalFlowEquations(eScdFlwEqnTasks Task, CSpPropInfo *pProps, int IONo, int FE, int LnkNo)
  {
  switch (Task)
    {
    case FET_GetMode:
      break;
    case FET_SetQm:
      IOFB(IONo, FE)->SetDPb(0.0, 0.0);
      IOFB(IONo, FE)->SetDPbX(0.0);
      IOFB(IONo, FE)->SetDPq(0.0, 0.0);
      break;
    case FET_CalcDP:
      {
      if (IOId_Self(IONo)==idFeed)
        {
        FlwBlk *pFB = IOFB(IONo, FE);
        pFB->SetQmFree();
        pFB->ClrDerivsBad();
        pFB->SetDPb(0.0, 0.0); // In is Positive

        SpConduit & QFd = *IOConduit(IONo);

        double Q=GEZ(pFB->GetQm());
        double Q2=GEZ(QFd.QMass());
        //RDBASSERT(fabs(Q-Q2)<1.0e-9, "Shouldn't these be equal?");
        //kga:8/10/02 what is difference between Q and Q2 should they have equal/similar values???

        if (Q>UsableMass && Q2>UsableMass)//kga:8/10/02: need to test Q2 to prevent div by zero!
          {
          double DQ=Max(1.0e-3, 1.0e-2*Q);

          Meth[iMethod]->Init(Q, QFd);

          double FlowLoss0=Meth[iMethod]->DP(Q, QFd);
          double FlowLoss1=Meth[iMethod]->DP(Q+DQ, QFd);
          double DFlowLoss=(FlowLoss1-FlowLoss0)/DQ;

          pFB->SetDPq(FlowLoss0, DFlowLoss);
          }
        else
          pFB->SetDPq(0.0, 0.0);
        return true;
        }
      break;
      }
    }
  return false;
  };

//--------------------------------------------------------------------------

void Cyclone::EvalProducts(CNodeEvalIndex & NEI)
  {
  if (GSM.Enabled())
    {
    if (NoProcessJoins()>0)
      switch (SolveMethod())
        {
        case SM_Direct:
          if (NoProcessJoins()>=1)
            Xfer_EvalProducts(0, Joins[0].Pressure(), NULL, NULL, NULL, GSM(), NULL);
          break;
        case SM_Inline:
        case SM_Buffered:
          for (int j=0; j<NoProcessJoins(); j++)
            Xfer_EvalProducts(j, Joins[j].Pressure(), NULL, NULL, NULL, GSM(), NULL);
          break;
        }
    }
  else if (IO_In(idFeed) && IO_Out(idUF) && IO_Out(idOF))
    {
    SigmaQInPMin(QFd, som_ALL, Id_2_Mask(idFeed));

    const int iUF = IOWithId_Self(idUF);
    const int iOF = IOWithId_Self(idOF);
    rSpConduit QOs =*IOConduit(iUF);
    rSpConduit QUs =*IOConduit(iOF);

    if (1)
      {//check for valid feed...
      const double QmFd = QFd.QMass();
      if (SQSzDist1::Ptr(QFd.Model(), false)==NULL)
        {
        QUs.QZero(); //first do QZero to ensure existing SzQual deleted
        QUs.QCopy(QFd);
        QOs.QZero();
        if (QmFd>UsableMass)
          SetCI(1);
        else
          ClrCI(1);
        return;
        }
      ClrCI(1);

      if (QmFd <= UsableMass)
        {
        QUs.QCopy(QFd);
        QOs.QZero();
        return;
        }
      }

    SQSzDist1 &Sz   =*SQSzDist1::Ptr(QFd.Model());
    SQSzDist1 &SzOs =*SQSzDist1::Ptr(QOs.Model());
    SQSzDist1 &SzUs =*SQSzDist1::Ptr(QUs.Model());
    SQSzDist1 &SzTemp =*SQSzDist1::Ptr(QTemp.Model());
    SQSzDist1 &SzFine =*SQSzDist1::Ptr(QFine.Model());

    const double TonsLimit = 1e-8;
    double TonsTotal = 0.0;
    for (int d = 0; d<Sz.NDistributions(); d++)
      {
      if (Sz.DistExists(d))
        {
        CSD_Distribution &D = Sz.Dist(d);
        for (int s = 0; s<D.NPriIds(); s++)
          for (int s1 = 0; s1<D.NSecIds(s); s1++)
            TonsTotal += QFd.VMass[D.SzId(s,s1)];
        }
      }

    if (TonsTotal <= TonsLimit)
      {
      QUs.QCopy(QFd);
      QOs.QZero();
      SetCI(2);
      return;
      }
    ClrCI(2);

    Meth[iMethod]->Init(GEZ(IOQm_In(idFeed)), QFd);

    //===========================================================================
    // Transfer All Qualities / Solids to US and Liquids 50/50
    QOs.QSetF(QFd, SetMass_Frac, 0.0, 0.5, 0.0, Std_P);
    QUs.QSetF(QFd, SetMass_Frac, 1.0, 0.5, 0.0, Std_P);

    for (int Pass=0; Pass<Meth[iMethod]->NPasses(); Pass++)
      {
      UFSol = 0.0;
      OFSol = 0.0;
      for (d=0; d<Sz.NDistributions(); d++)
        if (Sz.DistExists(d))
          {
          SzUs.AllocDist(d);
          SzOs.AllocDist(d);
          CSD_Distribution &D=Sz.Dist(d);
          CSD_Distribution &DO=SzOs.Dist(d);
          CSD_Distribution &DU=SzUs.Dist(d);
          for (int s=0; s<D.NPriIds(); s++)
            {
            CDArray & Size = D.PriSp[s]->FracPass;

            int len=Size.GetSize();
            if (len>0)
              {
              //NoSize=False;
              CDArray & USize=DU.PriSp[s]->FracPass;
              CDArray & OSize=DO.PriSp[s]->FracPass;
              OTemp.SetSize(len);
              Feed.SetSize(len);
              UTemp.SetSize(len);
              YTemp.SetSize(len);

              double Fractions, x1, y1, tmpF;
              Fractions = 0.0;
              for(long i=0 ; i<len ; i++)
                Fractions += Size[i];
              if (Fractions <= 0.0)
                Fractions = 1.0;

              for(i=0 ; i<len ; i++)
                 YTemp[i] = Size[i] / Fractions;

              double TonsS = 0.0;
              for (int s1=0; s1<D.NSecIds(s); s1++)
                TonsS+=QFd.VMass[D.SzId(s,s1)];
              for(i=0 ; i<len ; i++)
                {
                x1      = D.Intervals()[i];
                y1      = YTemp[i];
                tmpF    = y1 * TonsS;
                Feed[i] = tmpF;
                }

              double SpcOSzSol = 0.0;
              double SpcUSzSol = 0.0;
              for (i=0; i<len; i++)
                {
                //kga: shouldn't we always use geometrical mean!!!
                if (Meth[iMethod]->UseGeomMean())
                  OSize[i]=(i==0 ? 0.0 : Meth[iMethod]->OSizeFrac(D.GeometricMean(i), UFLiq, OFLiq, Pass));
                else
                  OSize[i]=Meth[iMethod]->OSizeFrac(D.Intervals()[i], UFLiq, OFLiq, Pass);
                OTemp[i] = OSize[i] * Feed[i];
                SpcOSzSol+= OTemp[i];
                UTemp[i] = Feed[i] - OTemp[i];
                SpcUSzSol+= UTemp[i];
                }

              if ((SpcOSzSol < TonsLimit) && (SpcUSzSol < TonsLimit))
                {
                for(i=0 ; i<len ; i++)
                  {
                  OSize[i] = 0.0;
                  USize[i] = 0.0;
                  }
                }
              else
                {
                if (SpcOSzSol < TonsLimit)
                  {
                  OSize[0] = 0.0;
                  USize[0] = UTemp[0] / SpcUSzSol;
                  for(i=1 ; i<len ; i++)
                    {
                    OSize[i] = 0.0;
                    USize[i] = (UTemp[i] / SpcUSzSol) + USize[i-1];
                    }
                  }
                if (SpcUSzSol < TonsLimit)
                  {
                  OSize[0] = OTemp[0] / SpcOSzSol;
                  USize[0] = 0.0;
                  for(i=1 ; i<len ; i++)
                    {
                    USize[i] = 0.0;
                    OSize[i] = (OTemp[i] / SpcOSzSol) + OSize[i-1];
                    }
                  }
                if ((SpcOSzSol > TonsLimit) && (SpcUSzSol > TonsLimit))
                  {
                  for(i=0 ; i<len ; i++)
                    {
                    OSize[i] = (OTemp[i] / SpcOSzSol);
                    USize[i] = (UTemp[i] / SpcUSzSol);
                    }
                  }
                }

              double TotSol    = GTZ(SpcOSzSol+SpcUSzSol);
              double OSzSolFrac = SpcOSzSol/TotSol;
              double USzSolFrac = SpcUSzSol/TotSol;

              double intervals = D.NSecIds(s);
              for (s1=0; s1<D.NSecIds(s); s1++)
                {
                int Id = D.SzId(s,s1);
                QOs.SetVMass(Id, QFd, QFd.VMass[Id]*OSzSolFrac);
                QUs.SetVMass(Id, QFd, QFd.VMass[Id]*USzSolFrac);
                }

              UFSol += SpcOSzSol;
              OFSol += SpcUSzSol;

              // Show the partition curve of the cyclone
              CDArray& Crv = PartCrv.Curve(s);
              for (i=0; i<len; i++)
                {
                if (Feed[i]<1.0e-18)
                  Crv[i] = (i==0 ? 0.0 : Crv[i-1]);
                else
                  Crv[i] = OTemp[i] / Max(Feed[i], 1e-18);
                }
              if (PartCrv.CurveMode(s) == PC_Frac2Fine)
                {
                for (i=0; i<len; i++)
                  Crv[i] = 1.0 - Crv[i];
                }

              }
            }
          Meth[iMethod]->GetLiquidSplit(Pass, QFd, UFSol, OFSol, UFLiq, OFLiq);

          // Convert to % of feed
          UFSol = UFSol / TonsTotal;
          OFSol = OFSol / TonsTotal;

          for (s=0; s<SDB.Count(); s++)
            if (SDB[s].IsLiq())
              {
              QOs.SetVMass(s, QFd, QFd.VMass[s]*UFLiq);
              QUs.SetVMass(s, QFd, QFd.VMass[s]*OFLiq);
              }

          QOs.SetTemp(QFd.Temp());
          QUs.SetTemp(QFd.Temp());

          }
      if (Meth[iMethod]->Converged(Pass))
        {
        ClrCI(3);
        goto Done;
        }
      }
    SetCI(3);

Done:
    //===========================================================================
    // Calculate under and over flow densities and m_d50 (Cleveland way)

    OFDens = QUs.Rho();
    UFDens = QOs.Rho();

    Meth[iMethod]->Term(QFd, QOs, QUs);
    }
  else
    {
    Xfer_EvalProducts(0, Joins[0].Pressure(), &QFd, NULL, NULL, NULL, NULL);
    }
  }

//--------------------------------------------------------------------------

dword Cyclone::ModelStatus()
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
    }
  return Status;
  }

//--------------------------------------------------------------------------

flag Cyclone::CIStrng(int No, pchar & pS)
  {
  // NB check CBCount is large enough.
  switch (No-CBContext())
    {
    case  1: pS="W\tNo feed Size distributions defined"; return 1;
    case  2: pS="N\tInsufficient Solids in Feed";    return 1;
    case  3: pS="W\tCyclone not converged";          return 1;
    default:
      return MdlNode::CIStrng(No, pS);
    }
  };

//--------------------------------------------------------------------------
//===========================================================================
