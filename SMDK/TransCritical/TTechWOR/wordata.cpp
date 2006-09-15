enum PrecipMethod {GRM_Fixed, GRM_White, GRM_CAR_QAL, GRM_TTTest};
enum CoolType{COOL_dQ, COOL_dT, COOL_Hx};
enum EvapType{EVAP_NONE, EVAP_FIXED, EVAP_dT};
enum ICool{COOL_NONE, COOL_INTERNAL, COOL_EXTERNAL};

enum ClassifierMethod {CM_Molerus, CM_Lynch, CM_Plitt };


enum ThermalHeatLossMethods {THL_None, 
			     THL_TempDrop, 
			     THL_FixedHeatFlow, 
			     THL_Ambient,
			     THL_FixedTemp};

void CPrecipitator::BuildDataFields()
{
  static MDDValueLst DDB1[]={
    {GRM_AAEq,   "A - Aeq" },
    {GRM_sigma,   "sigma"},
    {0}};

  static MDDValueLst DDB2[]={
    {0,   "Mass" },
    {1,   "Fraction"},
    {2,   "Number"},
    {0}};

  static MDDValueLst DDB3[]={
    {THL_None,          "None" },
    {THL_TempDrop,      "TempDrop"},
    {THL_FixedHeatFlow, "FixedLoss"},
    {THL_Ambient,  "Ambient"},
    {THL_FixedTemp, "FixedTemp"},
    {0}};

  static MDDValueLst DDB5[]={
    { COOL_dQ, "Fixed.dQ"},
    { COOL_dT, "Fixed.dT" },
    { COOL_Hx, "HeatExchange"},
    {0}};

  static MDDValueLst DDB6[]={
    { EVAP_NONE, "None"},
    { EVAP_FIXED, "Fixed"},
    { EVAP_dT, "Ambient" },
    {0}};

  static MDDValueLst DDB7[]={
    { COOL_NONE, "None"},
    { COOL_INTERNAL, "Internal"},
    { COOL_EXTERNAL, "External" },
    {0}};

  static MDDValueLst DDB8[] = {
    {0,   "Quasi Timestep" },
    {1,   "Direct"},
    {0}};

  static MDDValueLst DDB13[]={
    {0,   "A/C - A/Ceq" },
    {1,   "Growth Rate"},
    {0}};


  static MDDValueLst DDB14[]={
    {0,   "Growth" },
    {1,   "Aggl (N/s/m^3)"},
    {2,   "deltaN"},
    {3,   "oldN"},
    {4,   "newN"},
    {0}};
 
  static MDDValueLst DDB15[]={
    {CM_Molerus,   "Molerus" },
    {CM_Lynch,   "Lynch"},
    {CM_Plitt, "Plitt"},
    {0}};



  DD.Text  ("");

#ifdef TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
#endif
  DD.CheckBox("On", "",  &bOnLine, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Use.Saved.Tank", "",  &bTest, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Verbose", "",  &bVerbose, MF_PARAMETER|MF_SET_ON_CHANGE|MF_INIT_HIDDEN);

  DD.Text("");
  DD.CheckBox("Growth.On", "",  &bGrowthOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Agglom.On", "",  &bAgglomOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Nucleation.On", "",  &bNuclOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Classif.On", "",  &bClassOn, MF_PARAMETER|MF_SET_ON_CHANGE);

  DD.Text("");
  DD.Text  ("Requirements");
  DD.Double("TankVol", "",	        &dTankVol         ,MF_PARAMETER, MC_Vol("m^3"));
  DD.Double("UFValve", "",	        &m_dUFrac         ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("ShortCircuit", "",	        &m_dShort_Circ         ,MF_PARAMETER, MC_Frac("%"));

  DD.Long  ("ThermalLossMethod", "",&iThermalLossMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB3);
  DD.Show(iThermalLossMethod==THL_TempDrop);
  DD.Double("Temp_Drop", "",        &dTempDropRqd     ,MF_PARAMETER, MC_dT("C"));
  DD.Show(iThermalLossMethod==THL_FixedTemp);
  DD.Double("FixedTemp", "",        &dFixedTempRqd     ,MF_PARAMETER, MC_T("C"));
  DD.Show(iThermalLossMethod==THL_FixedHeatFlow);
  DD.Double("ThermalLossRqd", "",   &dThermalLossRqd  ,MF_PARAMETER, MC_Pwr("kW"));
  DD.Show(iThermalLossMethod==THL_Ambient);
  DD.Double("ThermalLossAmbient", "",  &dThermalLossAmbient ,MF_PARAMETER, MC_UA);
  DD.Show();
  DD.Long  ("Evaporation", "", &iEvapMethod , MF_PARAMETER|MF_SET_ON_CHANGE, DDB6);

  DD.Show(iEvapMethod!=EVAP_NONE);
  DD.Double("Evap.Rate", "",  &m_dEvapRate ,iEvapMethod==EVAP_FIXED?MF_PARAMETER:MF_RESULT, MC_Qm("kg/s"));
  DD.Show(iEvapMethod==EVAP_dT);
  DD.Double("Evap.Per.degK", "",            &m_dEvapRateK         ,MF_PARAMETER, MC_Qm("kg/s"));

  DD.Show();
  DD.Long  ("Cooling", "", &iCoolType, MF_PARAMETER|MF_SET_ON_CHANGE, DDB7);
  


  DD.Text("");

  DD.Text  ("");
  DD.Text  ("Results Tank");
  DD.Double("ResidenceTime", "",    &dResidenceTime ,MF_RESULT, MC_Time("h"));
  DD.Double("SuperSat", "", &m_dSSat, MF_RESULT, MC_);
  DD.Double("SSA",  "", &dSSA, MF_RESULT, MC_SurfAreaM);
  DD.Double("Yield", "",            &dYield         ,MF_RESULT, MC_Conc("kg/m^3"));
  DD.Double("THA.Precip", "",        &dTHAPrecip     ,MF_RESULT, MC_Qm("kg/s"));
  DD.Double("AluminaPrecip", "",    &dAlPrecip     ,MF_RESULT, MC_Qm("kg/s"));
  DD.Double("Solids.Precip", "",    &dSolPrecip     ,MF_RESULT, MC_Qm("kg/s"));
  DD.Double("Solids.Conc", "",    &dSolConc     ,MF_RESULT,  MC_Conc("kg/m^3"));

    
  DD.Text  ("Results");         
  DD.Show();                    
  DD.Double("Vol_FlowIn", "",       &dQvin          ,MF_RESULT, MC_Qv("L/s"));
  DD.Double("Vol_FlowOut", "",      &dQvout         ,MF_RESULT, MC_Qv("L/s"));

  DD.Double("MassFlowIn", "",       &dQmin          ,MF_RESULT, MC_Qm("t/d"));
  DD.Double("MassFlowOut", "",      &dQmout         ,MF_RESULT, MC_Qm("t/d"));
  
  DD.Double("ACin", "",             &dACin          ,MF_RESULT, MC_);
  DD.Double("ACout", "",            &dACout         ,MF_RESULT, MC_);
  DD.Double("ACequil", "",          &m_dACeq        ,MF_RESULT, MC_);


  DD.Double("TempIn", "",           &dTin           ,MF_RESULT, MC_T("C"));
  DD.Double("TempOut", "",          &dTout          ,MF_RESULT, MC_T("C"));
  DD.Double("GrowthRate", "",       &m_dGrowth          ,MF_RESULT, MC_Ldt("um/h"));
  DD.Double("NuclRate", "",         &m_dNucleation          ,MF_RESULT, MC_);
  DD.Double("NuclNum", "",          &m_dNuclN          ,MF_RESULT, MC_);
  DD.Double("AgglNum", "",          &m_dAgglomN          ,MF_RESULT, MC_);
  DD.Double("AgglMin", "",          &agmin,   MF_RESULT, MC_);
  DD.Double("BoundSoda",  "",       &dSoda,             MF_RESULT, MC_);
 

  DD.Text("");
  
  DD.Page("Precip");

  DD.Double("ConvergenceLimit", "",   &m_dConvergenceLimit, MF_PARAMETER|MF_INIT_HIDDEN, MC_);
  DD.Double("Acceleration", "",   &m_dAcc, MF_PARAMETER|MF_INIT_HIDDEN, MC_);
  DD.Double("AggMinLimit", "",   &dAggMinLim, MF_PARAMETER|MF_INIT_HIDDEN, MC_);

  DD.Double("ThermalDamping", "",   &m_dDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  DD.Double("MassDamping", "",   &m_dMassDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));

  DD.Long("Iterations", "", &m_lIterations, MF_RESULT|MF_NO_FILING);
  DD.Long("IterMax", "", &m_lItermax, MF_PARAMETER);
  


  DD.Text("");
  DD.Text("Plant Parameters");
  DD.Long("MaxAgglClass", "",   &m_lMaxAgglomClass, MF_PARAMETER);
  DD.Double("NuclPlantCorr", "", &m_dNuclPlantCorr, MF_PARAMETER, MC_);
  DD.Double("AgglomerisationConst", "", &m_dKAgglom, MF_PARAMETER, MC_);
  DD.Double("SurfaceActivity", "", &m_dSurfaceAct, MF_PARAMETER, MC_);
  DD.Double("SodaConst", "", &m_dSodaConst, MF_PARAMETER, MC_);
 
  

  DD.Text("");
  DD.Text("Linear Growth Rate Equation");
  DD.Double("GrowthConst", "",     &m_dK_G   	   ,MF_PARAMETER, MC_);
  DD.Double("GrowthActEnergy", "", &m_dActEGrowth  ,MF_PARAMETER, MC_T("K"));
  DD.Double("GrowthPwr", "",     &m_dgamma_g     ,MF_PARAMETER, MC_);
  DD.Long  ("GrowthTerm", "", &m_lGrowthTerm, MF_PARAMETER|MF_SET_ON_CHANGE, DDB1); 
  DD.Double("Sigma.Term", "",          &m_dsigma          ,MF_RESULT|MF_NO_FILING, MC_);
 

  DD.Text("");
  DD.Text("Nucleation Rate Equation");
  DD.Long  ("NucleationTerm", "", &m_lNuclTerm, MF_PARAMETER|MF_SET_ON_CHANGE, DDB1); 
  DD.Double("NucleationActEnergy", "", &m_dActENucleation       ,MF_PARAMETER, MC_T("K"));
  DD.Double("NucleationConst", "",    	       &m_dK_N	    ,MF_PARAMETER, MC_);
  DD.Double("NucleationPwr", "",    	       &m_dgamma_N 	    ,MF_PARAMETER, MC_);

  DD.Text("");
  DD.Text("Agglomeration Rate Equation");
  DD.Double("SolidDenPwr", "",    	       &m_dn_s  	    ,MF_PARAMETER, MC_);
  DD.Double("GrowthPwrAggl", "",    	       &m_dgamma_2	    ,MF_PARAMETER, MC_);

  DD.Text("");
  DD.Text("Soda Incorporation");
  DD.Long  ("SodaTerm", "", &m_lSodaTerm, MF_PARAMETER|MF_SET_ON_CHANGE, DDB13);
  DD.Double("SodaPwr", "", &m_dgamma_s, MF_PARAMETER, MC_);
  DD.Double("SodaActEnergy",     "", &m_dActESoda    ,MF_PARAMETER, MC_T("K"));


  DD.Page("PSD");
  DD.Double("Feed.N.Tot", "", &dFeedNTot, MF_RESULT, MC_);
  DD.Double("Prod.N.Tot", "", &dProdNTot, MF_RESULT, MC_);
    

  // Classification Stuff
  DD.Show(bClassOn);

  DD.Page("Classification");

  DD.Long  ("ClassMethod", "", &iClassMethod , MF_PARAMETER|MF_SET_ON_CHANGE, DDB15);
  DD.Double("ExcessBypassUFlow", "",   &m_dBypass         ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("MeasureSharpness",  "",   &m_dSharp_Index     ,MF_PARAMETER, MC_);
  DD.Double("CutSize",           "",   &m_dCut_Size        ,MF_PARAMETER, MC_);
  DD.Double("LiquorToUFlow",  "",      &m_dSlurry_split    ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("OverPassOFlow",  "",      &m_dOverpass       ,MF_PARAMETER, MC_Frac("%"));
  DD.Double("L_0",  "",                &m_dL0              ,MF_PARAMETER, MC_);
  DD.Text("Results");
  DD.Double("SolidsSplit",  "",  &m_dxS        ,MF_RESULT, MC_Frac("%"));
  DD.Double("Efficiency",  "",   &m_dEff        ,MF_RESULT, MC_);
	  
   

  DD.Show(iCoolType);
  DD.Page("Cooler");
  DD.Show(iCoolType==COOL_INTERNAL);
  DD.CheckBox("Cooler.On", "",  &m_bCoolerOn, MF_PARAMETER);
  DD.Long  ("Cooling.Type", "", (long*)&iCoolMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB5);

  DD.Show(iCoolType==COOL_INTERNAL && (iCoolMethod==COOL_dT));
  DD.Double("dT", "", &m_dCooldT, MF_PARAMETER, MC_dT("C")); 

  DD.Show(iCoolType==COOL_INTERNAL && (iCoolMethod==COOL_dQ));
  DD.Double("dQ",  "", &m_dCooldQ, MF_PARAMETER, MC_Pwr("kW")); 

  DD.Show(iCoolType==COOL_INTERNAL && (iCoolMethod==COOL_Hx));
  DD.Double("HX.Area",  "", &m_dCoolArea, MF_PARAMETER, MC_Area("m^2")); 
  DD.Double("HX.HTC",  "", &m_dCoolHTC, MF_PARAMETER, MC_HTC);
  DD.Show(iCoolType==COOL_EXTERNAL || (iCoolType==COOL_INTERNAL && (iCoolMethod==COOL_Hx)));
  DD.CheckBox("By.Vol.Flow", "",  &m_bByVolFlow, MF_PARAMETER|MF_SET_ON_CHANGE);

  DD.Show(iCoolType==COOL_INTERNAL && (iCoolMethod==COOL_Hx));
  DD.Double("Cooling.Flow",  "", &m_dCoolFlow,
	    m_bByVolFlow ? MF_RESULT : MF_PARAMETER, MC_Qm("kg/s")); // Internal cooling flow
  DD.Double("Int.Vol.Flow", "", &m_dIntCoolVolFlow, 
	    m_bByVolFlow ? MF_PARAMETER : MF_RESULT, MC_Qv("m^3/s")); // By Volume

  DD.Double("Hx.UA", "", &m_dUA, MF_RESULT, MC_UA);
  DD.Double("Hx.LMTD", "", &m_dLMTD, MF_RESULT, MC_dT);

  DD.Double("Water.Flow",  "", &m_dCoolWaterFlow, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("Water.Vol.Flow",  "", &m_dCoolWaterFlowVol, MF_RESULT, MC_Qv);

  DD.Double("Water.Tin",  "", &m_dCoolWaterTin, MF_RESULT, MC_T("C"));
  DD.Double("Water.Tout",  "", &m_dCoolWaterTout, MF_RESULT, MC_T("C"));
  DD.Double("Liquor.Tin",  "", &m_dLiquorTin, MF_RESULT, MC_T("C"));
  DD.Double("Liquor.Tout",  "", &m_dLiquorTout, MF_RESULT, MC_T("C"));
  DD.Show(iCoolType==COOL_EXTERNAL);
  DD.Double("Ext.Vol.Flow", "", &m_dExtCoolVolFlow, 
	    m_bByVolFlow ? MF_PARAMETER : MF_RESULT, MC_Qv("m^3/s")); // Ext Cooling.Flow
  DD.Double("Ext.Cooling.Flow", "", &m_dExtCoolFlow, 
	    m_bByVolFlow ? MF_RESULT : MF_PARAMETER, MC_Qm("kg/s")); // Ext Cooling.Flow

  DD.Double("Ext.Cooling.Temp", "", &m_dExtCoolTemp, MF_RESULT, MC_T("C")); // Ext Cooling.Temp
  DD.Double("Ext.Cooling.totHz", "", &m_dCoolOutTotHz , MF_RESULT, MC_Pwr("kW")); // Ext Cooling Rate
  DD.Show(iCoolType);
  DD.Double("Cooling.Rate", "", &m_dCoolRate, MF_RESULT, MC_Pwr("kW")); // Ext Cooling Rate


  DD.Show();
  DD.Page("Inlet");

  DD.Long("PSD.Display", "", &iPSD, MF_PARAMETER|MF_SET_ON_CHANGE, DDB2);
  const int NumSpecies = gs_MVDefn.Count();
  for (int i=0; i<=nClasses; i++) { 
    std::stringstream os;
    os << "Size " << std::setw(3) << i << std::setw(12) << L[i] << " " << Lav[i] ;
    DD.Double(os.str().c_str(), "", dd[2]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  

  /*******************  
  DD.Page("Size In");
  for (int i=0; i<=nClasses; i++) { 
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[3]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }************************/

  DD.Page("Size In");
  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[4]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Text("");
  DD.Double("Number", "", dd[4]+26, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Area", "", dd[4]+27, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Vol", "", dd[4]+28, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Mass", "", dd[4]+29, MF_RESULT|MF_NO_FILING, MC_None);

  DD.Page("Size Tank");
  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[5]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Text("");
  DD.Double("Number", "", dd[5]+26, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Area", "", dd[5]+27, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Vol", "", dd[5]+28, MF_RESULT|MF_NO_FILING, MC_None);
  DD.Double("Mass", "", dd[5]+29, MF_RESULT|MF_NO_FILING, MC_None);
  
  DD.Page("Numbers...");
  //  DD.Page(DDB14[iDType].m_pStr);
  DD.Long("DType", "", &iDType, MF_PARAMETER|MF_SET_ON_CHANGE, DDB14);  

  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[6]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Object(SavedTank, MDD_RqdPage);

    
#ifdef TTDEBUG
  DD.Page("Growth...");

  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[7]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Page("Aggl...");

  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[8]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Page("Nin...");

  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[9]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }

  DD.Page("Nout...");

  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[10]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }

#endif



}

//---------------------------------------------------------------------------

bool CPrecipitator::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
  }
