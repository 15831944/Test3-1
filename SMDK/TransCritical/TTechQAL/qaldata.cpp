//   Time-stamp: <2006-04-20 17:01:19 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA


void CPrecipitator::BuildDataFields()
{
  static MDDValueLst DDB1[]={
    {GRM_Fixed,   "Fixed" },
    {GRM_White,   "White"},
    {GRM_CAR_QAL, "CAR-QAL"},
    {0}};

  static MDDValueLst DDB2[]={
    {0,   "Fraction" },
    {1,   "Number"},
    {2,   "F*"},
    {0}};


  static MDDValueLst DDB3[]={
    {THL_None,          "None" },
    {THL_TempDrop,      "TempDrop"},
    {THL_FixedHeatFlow, "FixedLoss"},
    {THL_Ambient,  "Ambient"},
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

  static MDDValueLst DDB14[]={
    {0,   "Growth" },
    {1,   "Aggl (N/s/m^3)"},
    {2,   "deltaN"},
    {3,   "oldN"},
    {4,   "newN"},
    {0}};


  DD.Text  ("");

#ifdef TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
#endif


  DD.CheckBox("On", "",  &bOnLine, MF_PARAMETER|MF_SET_ON_CHANGE);
  //  DD.CheckBox("Int.Cooling", "", &m_bInternalCool, MF_PARAMETER|MF_SET_ON_CHANGE);
  //DD.CheckBox("Ext.Cooling", "", &m_bExternalCool, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Text("");
  DD.CheckBox("Growth.On", "",  &bGrowthOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Agglom.On", "",  &bAgglomOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Nucleation.On", "",  &bNuclOn, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.CheckBox("Attrition.On", "",  &bAttritionOn, MF_PARAMETER|MF_SET_ON_CHANGE);


  DD.Long  ("Cooling", "", &iCoolType, MF_PARAMETER|MF_SET_ON_CHANGE, DDB7);

  //m_RB.OnOffCheckBox();

  DD.Long  ("Evaporation", "", &iEvapMethod , MF_PARAMETER|MF_SET_ON_CHANGE, DDB6);

  DD.Show(iEvapMethod!=EVAP_NONE);
  DD.Double("Evap.Rate", "",  &m_dEvapRate ,iEvapMethod==EVAP_FIXED?MF_PARAMETER:MF_RESULT, MC_Qm("kg/s"));
  DD.Show(iEvapMethod==EVAP_dT);
  DD.Double("Evap.Per.degK", "",            &m_dEvapRateK         ,MF_PARAMETER, MC_Qm("kg/s"));

  DD.Show();
  DD.Long  ("ThermalLossMethod", "",&iThermalLossMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB3);
  DD.Show(iThermalLossMethod==THL_TempDrop);
  DD.Double("Temp_Drop", "",        &dTempDropRqd     ,MF_PARAMETER, MC_dT("C"));
  DD.Show(iThermalLossMethod==THL_FixedHeatFlow);
  DD.Double("ThermalLossRqd", "",   &dThermalLossRqd  ,MF_PARAMETER, MC_Pwr("kW"));
  DD.Show(iThermalLossMethod==THL_Ambient);
  DD.Double("ThermalLossAmbient", "",  &dThermalLossAmbient ,MF_PARAMETER, MC_UA);
  DD.Show();
  DD.Text("");
  DD.Text  ("Requirements");
  DD.Double("TankVol", "",	        &dTankVol         ,MF_PARAMETER, MC_Vol("m^3"));
  DD.Text  ("");
  DD.Text  ("Results Tank");
  DD.Double("ResidenceTime", "",    &dResidenceTime ,MF_RESULT, MC_Time("h"));
  DD.Double("SuperSat", "", &m_dSSat, MF_RESULT, MC_);
  DD.Double("Yield", "",            &dYield         ,MF_RESULT, MC_Conc("kg/m^3"));
  DD.Double("SolidsIn",  "",    &m_dSolidsIn, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("SolidsOut",  "",    &m_dSolidsOut, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("NucleationYield", "", &dNuclYield, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("GrowthYield", "", &dGrowthYield, MF_RESULT, MC_Qm("kg/s"));

  DD.Double("THA.Precip", "",        &dTHAPrecip     ,MF_RESULT, MC_Qm("kg/s"));
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
  DD.Text("");
  
  DD.Page("Precip");
  DD.Long  ("GrowthMethod", "", (long*)&iGrowthRateMethod, MF_PARAMETER|MF_SET_ON_CHANGE, DDB1);
  DD.Double("Convergence.Limit", "",   &m_dConvergenceLimit, MF_PARAMETER|MF_INIT_HIDDEN, MC_);
  DD.Double("Acceleration", "", &m_ddeltaT, MF_PARAMETER|MF_INIT_HIDDEN, MC_);
  DD.Double("Thermal.Damping", "",   &m_dDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  DD.Double("Mass.Damping", "",   &m_dMassDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  DD.Long("Iterations", "", &m_lIterations, MF_RESULT|MF_NO_FILING);
  DD.Long("IterMax", "", &m_lItermax, MF_PARAMETER);

  // DD.Double("Vol.Damping", "",   &m_dVolDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  
  DD.Show (iGrowthRateMethod==GRM_Fixed);
  DD.Double("Precip.Rate", "",      &dGrowthRate    ,MF_PARAMETER, MC_Ldt("um/h"));
  DD.Show (iGrowthRateMethod==GRM_White);
  DD.Double("ER_White", "",         &ER_White        ,MF_PARAMETER, MC_T("K"));
  DD.Double("K_White", "",	    &K_White         ,MF_PARAMETER, MC_);
  DD.Double("gF_White", "",    	    &gF_White        ,MF_PARAMETER, MC_);


  DD.Show (iGrowthRateMethod==GRM_CAR_QAL);
  DD.Double("ActivationEnergy", "",    	    &m_dActivationEnergy    ,MF_PARAMETER, MC_T("K"));
  DD.Double("K0", "",    	    &m_dK0   		    ,MF_PARAMETER, MC_);
  //DD.Double("K1", "",    	    &m_dK1   		    ,MF_PARAMETER, MC_);
  DD.Double("k_TOC", "",    	    &m_dk_TOC		    ,MF_PARAMETER, MC_);
  DD.Double("n_s", "",    	    &m_dn_s  		    ,MF_PARAMETER, MC_);
  DD.Double("n_fc", "",    	    &m_dn_fc 		    ,MF_PARAMETER, MC_);
  DD.Double("n_eq", "",    	    &m_dn_eq 		    ,MF_PARAMETER, MC_);
  DD.Double("n_", "",    	    &m_dn_   		    ,MF_PARAMETER, MC_);
  DD.Double("n_sol", "",    	    &m_dn_sol		    ,MF_PARAMETER, MC_);
  DD.Double("n_ssa", "",    	    &m_dn_ssa		    ,MF_PARAMETER, MC_);

  
  DD.Show();
  DD.Text("Fudge Factors");
  DD.Double("Growth.Rate.Factor", "", &dGrowthRateFactor,   MF_PARAMETER, MC_);
  DD.Double("Agglom.Rate.Factor", "", &dAgglRateFactor, MF_PARAMETER, MC_);
  DD.Double("Nucl.Rate.Factor", "", &dNuclRateFactor, MF_PARAMETER, MC_);

  DD.Show();
  DD.Text("Results");
  DD.Double("Growth.Rate", "",       &dGrowthRate          ,MF_RESULT|MF_NO_FILING, MC_Ldt("um/h"));
  DD.Double("Agglom.Rate", "",       &dAgglomRate          ,MF_RESULT|MF_NO_FILING, MC_);
  DD.Double("Nucl.Rate", "",       &dNuclRate              ,MF_RESULT|MF_NO_FILING, MC_);

  DD.Double("SSAin", "",    	    &m_dSSAin		    ,MF_RESULT, MC_);//SurfAreaM);
  DD.Double("SSA", "",    	    &m_dSSA		    ,MF_RESULT, MC_);//SurfAreaM);
 

  DD.Text("Thermal and Mass Balance");
  DD.Double("Mass.Flow.In", "",       &dQmin          ,MF_RESULT|MF_NO_FILING, MC_Qm("t/d"));
  DD.Double("Mass.Flow.Out", "",      &dQmout         ,MF_RESULT|MF_NO_FILING, MC_Qm("t/d"));
  DD.Show(iEvapMethod);
  DD.Double("Evap.Mass.Loss", "", &m_dEvapRate, MF_RESULT, MC_Qm("kg/s"));
  DD.Double("Evap.Thermal.Loss", "",   &m_dEvapThermalLoss  ,MF_RESULT, MC_Pwr("kW"));
  DD.Show(iThermalLossMethod==THL_Ambient);
  DD.Double("Env.Thermal.Loss", "", &m_dEnvironmentLoss, MF_RESULT, MC_Pwr("kW"));  
  DD.Show(iCoolType);
  DD.Double("Cooler.Thermal.Loss", "", &m_dCoolRate, MF_RESULT, MC_Pwr("kW")); // Ext Cooling Rate
  DD.Double("ReactionHeat", "",     &dReactionHeat  ,MF_RESULT, MC_Pwr("kW"));
  DD.Double("Total.Thermal.Loss", "", &m_dTotThermalLoss, MF_RESULT, MC_Pwr("kW"));
  DD.Text("Stream.Enthalpy");
  DD.Double("HzIn", "", &m_dHIn, MF_RESULT, MC_Pwr);
  DD.Double("HzEvap", "", &m_dHEvap, MF_RESULT, MC_Pwr);
  DD.Double("HzOut", "", &m_dHOut, MF_RESULT, MC_Pwr);
  DD.Double("HzBal", "", &m_dHBal, MF_RESULT, MC_Pwr);


  DD.Show();
  DD.Page("Size Inlet");

  DD.Long("PSD.Display", "", &iPSD, MF_PARAMETER|MF_SET_ON_CHANGE, DDB2);
  const int NumSpecies = gs_MVDefn.Count();
  for (int i=0; i<nClasses; i++) { 
    std::stringstream os;
    os << "Size " << std::setw(3) << i << std::setw(12) << D[i] ;
    DD.Double(os.str().c_str(), "", dd[2]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  

//   DD.Page("Size In");
//   for (int i=0; i<nClasses; i++) {
//     std::stringstream os;
//     os << "Size " << std::setw(3) << i;
//     DD.Double(os.str().c_str(), "", dd[4]+i, MF_RESULT|MF_NO_FILING, MC_None);
//   }
//   DD.Text("");
//   DD.Double("Number", "", dd[4]+26, MF_RESULT|MF_NO_FILING, MC_None);
//   DD.Double("Area", "", dd[4]+27, MF_RESULT|MF_NO_FILING, MC_None);
//   DD.Double("Vol", "", dd[4]+28, MF_RESULT|MF_NO_FILING, MC_None);
//   DD.Double("Mass", "", dd[4]+29, MF_RESULT|MF_NO_FILING, MC_None);

  DD.Page("Size Tank");
  for (int i=0; i<nClasses; i++) {
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

  for (int i=0; i<nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[6]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }



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





  if (!m_bEvapConnected && iEvapMethod!=EVAP_NONE) //if optional Evap is NOT connected and evaporation functionality is required show the output stream
    DD.Object(Evap, MDD_RqdPage);

  //DD.Show(m_RB.Enabled());  
  //DD.Page("RB");
  //m_RB.BuildDataFields();
  //DD.Show();
}
