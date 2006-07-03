//$T Serv.c GC 1.136 01/11/02 11:22:50
// Serv.f -- translated by f2c (version of 23 April 1993  18:34:30).
//   You must link the resulting object file with the libraries:
//	-lf2c -lm   (in that order)
//
#include "stdafx.h"
#include "sc_defs.h"
#include "flwnode.h"
#define  __SERV_CPP

#include "myf2c.h"
#include "PrecipPSD.h"

#define dbgModels 01
#if dbgModels
#include "dbgmngr.h"
static CDbgMngr dbgModel2State("QALPSD", "Model2State");
static CDbgMngr dbgState2Model("QALPSD", "State2Model");
static CDbgMngr dbgState2Conc ("QALPSD", "State2Conc");
#endif

static MInitialiseTest InitTest("QALPSDClass");

//Specie Access 
SPECIEBLK_V(InitTest, Steam,           "H2O(g)",                                   false);
SPECIEBLK_L(InitTest, Water,           "H2O(l)",                                   false);
SPECIEBLK_L(InitTest, NaAluminate,     "NaAl[OH]4(l)",                             false);
SPECIEBLK_L(InitTest, CausticSoda,     "NaOH(l)",                                  false);
SPECIEBLK_L(InitTest, NaCarbonate,     "Na2CO3(l)",                                false);
SPECIEBLK_L(InitTest, NaOxalate,       "Na2C2O4(l)",                               false);
SPECIEBLK_L(InitTest, NaChloride,      "NaCl(l)",                                  false);
SPECIEBLK_L(InitTest, NaSulphate,      "Na2SO4(l)",                                false);
SPECIEBLK_L(InitTest, OrgSoda,         "Na2C5.2O7.2(l)",                           false);
SPECIEBLK_L(InitTest, NaSilicate,      "Na2SiO3(l)",                               false);
                                                                    
SPECIEBLK_S(InitTest, THA,             "Al[OH]3(s)",                               false);
SPECIEBLK_S(InitTest, DSP1,            "Na2O.2SiO2.Al2O3.2H2O.1/3Na2CO3(s)",       false);
SPECIEBLK_S(InitTest, DSP2,            "Na2O.2SiO2.Al2O3.2H2O.2/3NaCl(s)",         false);
SPECIEBLK_S(InitTest, DSP3,            "Na2O.2SiO2.Al2O3.2H2O.2/3NaAl[OH]4(s)",    false);
SPECIEBLK_S(InitTest, InertAlumina,    "Al2O3(s)",                                 false);
SPECIEBLK_S(InitTest, BndSoda,         "NaOH*(s)",                                 false); //required by PSD
SPECIEBLK_S(InitTest, BndOrgSoda,      "Na2C5.2O7.2*(s)",                          false); //required by PSD


double NaAluminate2Alumina() { return ::InertAlumina.MW/(2.0*::NaAluminate.MW); }; //=101.96/(2*118.001)=0.43204
double NaAluminate2Caustic() { return (2.0*::CausticSoda.MW)/(2.0*::NaAluminate.MW); } //=2.0*39.997/(2*118.001)=0.33896

void CPrecipPSD::Model2State(SMBayerBase *pBm, CSD_SpDist *pDist, CPSDStates & State)
  {
  double MT=GTZ(pBm->Mass(som_SL));
  if (State.NSpecies()<0)
    {
    long N=0;
    for (long i=SDB.First(som_SL); i>=0 ;i=SDB.Next(i, som_SL))
      {
      if (i==Water        .LiqPhInx())  { Water       .alti() = N; continue; } // not a direct state
      if (i==NaAluminate  .LiqPhInx())  { NaAluminate .alti() = N; continue; } // not a direct state
      if (i==CausticSoda  .LiqPhInx())  { CausticSoda .alti() = N; continue; } // not a direct state
      if (i==NaCarbonate  .LiqPhInx())  { NaCarbonate .alti() = N; }
      if (i==NaOxalate    .LiqPhInx())  { NaOxalate   .alti() = N; }
      if (i==NaChloride   .LiqPhInx())  { NaChloride  .alti() = N; }
      if (i==NaSulphate   .LiqPhInx())  { NaSulphate  .alti() = N; }
      if (i==OrgSoda      .LiqPhInx())  { OrgSoda     .alti() = N; }
      if (i==NaSilicate   .LiqPhInx())  { NaSilicate  .alti() = N; }
                                                      
      if (i==THA          .SolPhInx())  { THA         .alti() = N; continue; } // not a direct state 
      if (i==BndSoda      .SolPhInx())  { BndSoda     .alti() = N; }
      if (i==BndOrgSoda   .SolPhInx())  { BndOrgSoda  .alti() = N; }
      N++;
      }
    State.SetNStates(N, m_GridPts.npts);
    }

  double rHyd         = pBm->m_M[THA.SolPhInx()]/MT;
  double rAlum        = pBm->m_M[NaAluminate.LiqPhInx()]/MT*NaAluminate2Alumina();
  double rSoda        = (pBm->m_M[CausticSoda.LiqPhInx()]+pBm->m_M[NaAluminate.LiqPhInx()]*NaAluminate2Caustic())/MT;
  double rOrgSoda     = pBm->m_M[OrgSoda.LiqPhInx()]/MT;
  double rBndSoda     = pBm->m_M[BndSoda.SolPhInx()]/MT;
  double rBndOrgSoda  = pBm->m_M[BndOrgSoda.SolPhInx()]/MT;

  State.Temp()        = pBm->Temp();   
  State.rXLiq()       = 1.0-rHyd-rAlum/m_ModelParms.m_Hyd2A;
  State.rSoda()       = rSoda/m_ModelParms.m_C2NaOH;
  State.rOrgSoda()    = rOrgSoda/m_ModelParms.m_C2OrgSoda;
  State.rBndSoda()    = rBndSoda/m_ModelParms.m_C2NaOH;
  State.rBndOrgSoda() = rBndOrgSoda/m_ModelParms.m_C2OrgSoda;
  
  long s=0;
  for (long i=SDB.First(som_SL); i>=0 ;i=SDB.Next(i, som_SL))
    {
    if (i==Water.LiqPhInx())
      {}
    else if (i==NaAluminate.LiqPhInx())
      {}
    else if (i==CausticSoda.LiqPhInx())
      {}
    else if(i==OrgSoda.LiqPhInx())
      {}
    else if(i==THA.SolPhInx())
      {}
    else if(i==BndSoda.SolPhInx())
      {}
    else if(i==BndOrgSoda.SolPhInx())
      {}
    else
      State.MFrac(s++)=pBm->m_M[i]/MT;
    }

  for (int x=0; x<m_GridPts.npts; x++)
    State.PSD(x)=pDist->FracPass[x];

  MassToNumber(State.PSD(), &m_GridPts.npts, m_GridPts.x);
  double Moment3 = Moment(State.PSD(), 3, &m_GridPts.npts, m_GridPts.x) * 1e-18;  // third moment of psd in m^3
  double const_ = 6.0 * (rHyd) / GTZ(m_ModelParms.m_RhoH * 3.141592653589793 * Moment3);  // Number particles /Kg slurry

  for(x = 0; x < m_GridPts.npts; ++x)
    State.PSD(x) = const_ * State.PSD(x);

#if 0&&dbgModels
  if (dbgModel2State())
    {
    dbgpln("Model2State");
    dbgpln("      %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s| psd...",
            "", "re", "", "rc", "rCO3", "rSO4", "rCl", "rOX", "rOC", "rh");
    dbgp("      %12.12s %11.4f%% %12.12s %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %12.4f|",
            "", State.re()*100, "", State.rc*100, State.rCO3*100, State.rSO4*100, 
            State.rCl*100, State.rOX*100, State.rOC*100, RC.m_rh);
    for(i = 0; i < m_GridPts.npts; ++i)
      dbgp(" %12.6f", psd[i]);
    dbgpln("");
    dbgp("      %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s|",
            "", "", "", "", "", "", "", "", "", "");
    for(i = 0; i < m_GridPts.npts; ++i)
      dbgp(" %12.1f", State.PSD(i));
    dbgpln("");
    }
#endif
  }


void CPrecipPSD::State2Model(CPSDStates & State, SMBayerBase *pBm, CSD_SpDist *pDist)
  {
  if (pBm!=NULL)
    {
#if 0&&dbgModels
    if (dbgModel2State())
      {
      dbgpln("State2Model");
      dbgpln("      %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s| psd...",
              "", "re", "ra", "rc", "rCO3", "rSO4", "rCl", "rOX", "rOC","rh");
      dbgp("      %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%% %11.4f%%|",
              -1.0, State.re()*100, -1., State.rc*100, State.rCO3*100, State.rSO4*100, 
              State.rCl*100, State.rOX*100, State.rOC*100, rh*100);
      for(int i = 0; i < m_GridPts.npts; ++i)
        dbgp(" %12.1f", State.PSD(i));
      dbgpln("");
      }
#endif

    double rHyd         = m_ModelParms.m_RhoH * 3.141592653589793 * Moment(State.PSD(), 3, &m_GridPts.npts, m_GridPts.x) / 6e18;
    double rAlum        = (1-rHyd-State.rXLiq())*m_ModelParms.m_Hyd2A;
    double rSoda        = State.rSoda()*m_ModelParms.m_C2NaOH;
    double rOrgSoda     = State.rOrgSoda()*m_ModelParms.m_C2OrgSoda;
    double rBndSoda     = State.rBndSoda()*m_ModelParms.m_C2NaOH;
    double rBndOrgSoda  = State.rBndOrgSoda()*m_ModelParms.m_C2OrgSoda;


    double MT=GTZ(pBm->Mass(som_SL));
    double SodiumAluminateMass = GEZ(MT*rAlum/NaAluminate2Alumina());
    double CausticSodaMass     = GEZ(MT*rSoda - SodiumAluminateMass*NaAluminate2Caustic());
    double THAMass             = GEZ(MT*rHyd);
    double OrgSodaMass         = GEZ(MT*rOrgSoda);
    double BndSodaMass         = GEZ(MT*rBndSoda);
    double BndOrgSodaMass      = GEZ(MT*rBndOrgSoda);

    long s=0;
    double TotMassNoH2O=0;
    for (long i=SDB.First(som_SL); i>=0 ;i=SDB.Next(i, som_SL))
      {
      if (i==Water.LiqPhInx())
        {}
      else 
        {
        if (i==NaAluminate.LiqPhInx())
          pBm->SetVMass(i, *pBm, SodiumAluminateMass);
        else if (i==CausticSoda.LiqPhInx())
          pBm->SetVMass(i, *pBm, CausticSodaMass);
        else if (i==OrgSoda.LiqPhInx())
          pBm->SetVMass(i, *pBm, OrgSodaMass);
        else if(i==THA.SolPhInx())
          pBm->SetVMass(i, *pBm, THAMass);
        else if(i==BndSoda.SolPhInx())
          pBm->SetVMass(i, *pBm, BndSodaMass);
        else if(i==BndOrgSoda.SolPhInx())
          pBm->SetVMass(i, *pBm, BndOrgSodaMass);
        else
          pBm->SetVMass(i, *pBm, GEZ(MT*State.MFrac(s++)));
        TotMassNoH2O+=pBm->m_M[i];
        }
      }
    // Water
    pBm->SetVMass(Water.LiqPhInx(), *pBm, MT-TotMassNoH2O);
    pBm->SetTemp(State.Temp());   // temperature K

    double  psd[MaxPSDDivs];
    double  tot=0.0;
    for (i=0; i<m_GridPts.npts; i++)
      {
      psd[i]=GEZ(State.PSD(i));
      tot+=psd[i];
      }
    // fix any minor errors;
    double scl=1.0/tot;
    for (i=0; i<m_GridPts.npts; i++)
      psd[i]*=scl;
    NumberToMass(psd, &m_GridPts.npts, m_GridPts.x);
    for (i=0; i<m_GridPts.npts; i++)
      pDist->FracPass[i]=psd[i];

    pBm->EquilibrateAll();

#if dbgModels
    if (dbgState2Model())
      {
      dbgp("      %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s %12.12s|",
              "", "", "", "", "", "", "", "", "", "");
      for(i = 0; i < m_GridPts.npts; ++i)
        dbgp(" %12.6f", psd[i]);
      dbgpln("");
      
      double t=0;
      for (i=0; i<m_GridPts.npts; i++)
        t+=pDist->FracPass[i];
      dbgpln("TotPSDMass=%12.6f", t);
      }
#endif
    }
  }


// ***********************************************************************
//       Plant : State2Conc: subroutine to calculate liquor aluminate concentration,
//       caustic concentration from the basic variables
// ***********************************************************************


//const double H2OTestFrac = 0.98;
const double MW_C        = 12.011;

void CPrecipPSD::State2Conc(CPSDStates & State, CPSDConc & Conc, SMBayerBase*pBm, CPSDConc* pConcOvr, bool withdbg)
  {
  double    rhol, wtoc;
  long      ittol;
  long      numit;
  double    wa, wc, wf, vh, vl, wl, wfe, wcl, woc, vln, wsi, wox, wco3, wso4;

  double RelErr       = m_GlobalVars.relerr;
  long   MaxIt        = m_GlobalVars.maxit;
  double Moment3      = Moment(State.PSD(), 3, &m_GridPts.npts, m_GridPts.x);  // 3rd moment
  double rHyd         = m_ModelParms.m_RhoH * 3.141592653589793 * Moment3 / 6e18;
  double rBndSoda     = State.rBndSoda()*m_ModelParms.m_C2NaOH+State.rBndOrgSoda()*m_ModelParms.m_C2OrgSoda;           // mass Bnd soda/mass slurry ratio
  double rBndSodaOrg  = State.rBndOrgSoda()*m_ModelParms.m_C2OrgSoda / 
                        GTZ(State.rBndOrgSoda()*m_ModelParms.m_C2OrgSoda+
                            State.rBndSoda()*m_ModelParms.m_C2NaOH);  // mass organic bound soda/mass bound soda

  if (1)
    {
    if (1.0-rHyd<State.rXLiq())
      {
      //TROUBLE!!!!
      int xxx=0;
      }

    //     ***** initialisation *****
    ittol     = TRUE_;                        // ensure at least one iteration
    numit     = 0;                            // initialize counter
    wl        = 1.0 - rHyd;                   // mass of liquor
    wa        = wl - State.rXLiq();           // mass of aluminate as hydrate
    wc        = State.rSoda();                // mass of caustic as Na2O
    //???
    wcl       = State.MFrac(NaChloride.alti());                         // mass of NaCl
    wf        = 0;//State.MFrac(NaFluoride.alti());                     // mass of NaF
    wco3      = State.MFrac(NaCarbonate.alti());                        // mass of Na2CO3
    wso4      = State.MFrac(NaSulphate.alti());                         // mass of Na2SO4
    wsi       = State.MFrac(NaSilicate.alti());                         // mass of SiO2
    wfe       = 0;//XXrFe;                                              // mass of Fe2O3
    wox       = State.MFrac(NaOxalate.alti())/(NaOxalate.MW/(2*MW_C));  // mass of Na2C2O4
    woc       = State.rOrgSoda()/(OrgSoda.MW/(5.2*MW_C));               // mass of OC
    wtoc      = wox + woc;                    // mass of TOC

    rhol      = 1250.0;                       // typical liquor density
    vl        = wl / rhol;                    // initial volume
    Conc.a    = m_ModelParms.m_Hyd2A * wa / vl; // initial alumina concentration
    Conc.c    = wc / vl;                      // initial caustic concentration
    Conc.cl   = wcl / vl;                     // initial NaCl concentration
    Conc.f    = wf / vl;                      // initial NaF concentration
    Conc.co3  = wco3 / vl;                    // initial Na2CO3 concentration
    Conc.so4  = wso4 / vl;                    // initial Na2SO4 concentration
    Conc.si   = wsi / vl;                     // initial SiO2 concentration
    Conc.fe   = wfe / vl;                     // initial Fe2O3 concentration
    Conc.ox   = wox / vl;                     // initial Na2C2O4 concentration
    Conc.toc  = wtoc / vl;                    // initial TOC concentration

    //     ***** start of iterative loop *****
    while(ittol)
      {
      vln       = vl;                              // save for testing convergence

      //     Update liquor density
      rhol      = DenLiquor(Conc, m_ModelParms.m_Temp0);

      vl        = wl / rhol;                        // update volume
      Conc.a    = m_ModelParms.m_Hyd2A * wa / vl; // update alumina concentration
      Conc.c    = wc / vl;                      // update caustic concentration
      Conc.cl   = wcl / vl;                    // update NaCl concentration
      Conc.f    = wf / vl;                      // update NaF concentration
      Conc.co3  = wco3 / vl;                  // update Na2CO3 concentration
      Conc.so4  = wso4 / vl;                  // update Na2SO4 concentration
      Conc.si   = wsi / vl;                    // update SiO2 concentration
      Conc.fe   = wfe / vl;                    // update Fe2O3 concentration
      Conc.ox   = wox / vl;                    // update Na2C2O4 concentration
      Conc.toc  = wtoc / vl;                  // update TOC concentration
    
      //       determine if iteration has converged
      ittol                      = (fabs(vln - vl) > RelErr * vl);  // test convergence of volume
      ++numit;                               // update iteration counter

      if(ittol && numit >= MaxIt)
        {
        ittol = FALSE_;
        LogWarning("PSD", 0, "Subroutine State2Conc has failed to converge!");
        }
      }
    // **** end iterative loop ****

    vh                  = rHyd / m_ModelParms.m_RhoH;
    Conc.gpl            = rHyd / (vh + vl);
    Conc.m_BndSoda      = rBndSoda / (rHyd * m_ModelParms.m_Hyd2A);
    Conc.m_BndSodaAsOrg = rBndSodaOrg;
    Conc.cs             = Conc.c / m_ModelParms.m_Cs2C;  // caustic as Na2CO3
    Conc.acs            = Conc.a / Conc.cs; //kga 19/6/02 added this line. Why wasn't acs calculated?
    
    if (Conc.a<0.0 || Conc.c<0.0 || Conc.cs<0.0 || Conc.toc<0.0 || Conc.gpl<0.0)
      {
      int xxx=0;
      }
    }

  Conc.m_Moment0 = Moment(State.PSD(), 0, &m_GridPts.npts, m_GridPts.x);  // 0th moment
  Conc.m_Moment1 = Moment(State.PSD(), 1, &m_GridPts.npts, m_GridPts.x);  // 1st moment
  Conc.m_Moment2 = Moment(State.PSD(), 2, &m_GridPts.npts, m_GridPts.x);  // 2nd moment
  Conc.m_Moment3 = Moment(State.PSD(), 3, &m_GridPts.npts, m_GridPts.x);  // 3rd moment
  Conc.m_L10 = (fabs(Conc.m_Moment0)>0.0) ? Conc.m_Moment1/Conc.m_Moment0 : 0.0;
  Conc.m_L21 = (fabs(Conc.m_Moment1)>0.0) ? Conc.m_Moment2/Conc.m_Moment1 : 0.0;
  Conc.m_L32 = (fabs(Conc.m_Moment2)>0.0) ? Conc.m_Moment3/Conc.m_Moment2 : 0.0;
  if (pBm)
    {
    Conc.m_Dens = pBm->Rho();
    Conc.m_SAM = ((MISSA*)pBm)->SpecificSurfaceAreaMass();
    }
#if dbgModels
  if (dbgState2Conc() && withdbg)
    {
    dbgpln("State2Conc");
    dbgpln("      %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s %12s ",
      "", "Temp", "acs", "cs", "co3", "so4", "cl", "ox", "toc", "gpl", "m_Soda");
    dbgpln("      %12s %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f %12.6f ",
      "", State.Temp()/*Conc.m_Temp*/, Conc.acs, Conc.cs, Conc.co3, Conc.so4, Conc.cl, Conc.ox, Conc.toc, Conc.gpl, Conc.m_BndSoda);
    }
#endif
  }

// ***********************************************************************
//     DenSlurry : Function to calculate slurry density correlation.
//     NOTE: this expression uses caustic as Na2O
// ***********************************************************************
double CPrecipPSD::DenSlurry( CPSDConc & Conc, double tempk)
  {
  double        rhol, rhol0, vh, vl, wl, vs, vl0;
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //     rhol          liquor density
  //     rhol0         liquor density at standard temperature
  //     temp          liquor temperature                             deg C
  //     vh            volume of hydrate
  //     vl            volume of liquor
  //     vl0           volume of liquor at T0
  //     vs            volume of slurry
  //     wl            mass of liquor
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  
  rhol = DenLiquor(Conc, tempk);
  rhol0 = DenLiquor(Conc, m_ModelParms.m_Temp0);
  vh = Conc.gpl / m_ModelParms.m_RhoH;
  vl0 = 1.0 - vh;
  wl = vl0 * rhol0;
  vl = wl / rhol;  // temperature correction
  vs = vh + vl;
  double ret_val = (Conc.gpl + wl) / vs;

  return ret_val;
  } // DenSlurry


// ***********************************************************************
//       Temp_Corr : CorrectSlurryVol25 : subroutine to calculate eqivalent slurry
//       volume at 25 deg C
// ***********************************************************************
double CPrecipPSD::CorrectSlurryVol25(double vs, CPSDConc & Conc, double tempk)
  {
  double        rhol, rhol0, vh, vl, wl;

  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  //     rhol          liquor density at temperature temp               gpl
  //     rhol0         liquor density at standard temperature           gpl
  //     temp          liquor temperature                             deg C
  //     vs            volume of slurry                                 m^3
  //     vl            volume of liquor/volume slurry ratio
  //     vh            volume of Al(OH)3/volume slurry ratio
  //     wl            mass of liquor/volume slurry ratio
  // ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
  
  Conc.a = Conc.acs * Conc.cs;             // alumina concentration
  Conc.c = Conc.cs * m_ModelParms.m_Cs2C;  // caustic concentration
  
  //     Liquor Densities
  rhol = DenLiquor(Conc, tempk);
  rhol0 = DenLiquor(Conc, m_ModelParms.m_Temp0);

  vh = Conc.gpl / m_ModelParms.m_RhoH;  // volume hydrate at temp/volume slurry
  vl = 1.0 - vh;    // volume liquor at temp/volume slurry
  wl = vl * rhol;   // mass liquor/volume slurry
  vl = wl / rhol0;  // volume of liquor at temp0/volume slurry
  return (vs * (vh + vl));  // temperature corrected volume of slurry
  } // SlurryVol25

