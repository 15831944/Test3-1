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





  DD.Text  ("");

#ifdef TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
#endif
  DD.CheckBox("On", "",  &bOnLine, MF_PARAMETER|MF_SET_ON_CHANGE);
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


  DD.Text  ("");
  DD.Text  ("Results Tank");
  DD.Double("ResidenceTime", "",    &dResidenceTime ,MF_RESULT, MC_Time("h"));
  DD.Double("SuperSat", "", &m_dSSat, MF_RESULT, MC_);
  DD.Double("Yield", "",            &dYield         ,MF_RESULT, MC_Conc("kg/m^3"));
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






  DD.Double("Convergence.Limit", "",   &m_dConvergenceLimit, MF_PARAMETER|MF_INIT_HIDDEN, MC_);
  DD.Double("Thermal.Damping", "",   &m_dDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  DD.Double("Mass.Damping", "",   &m_dMassDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  DD.Double("Vol.Damping", "",   &m_dVolDamping, MF_PARAMETER|MF_INIT_HIDDEN, MC_Frac("%"));
  DD.Text("Plant Parameters");
  
  DD.Long("Max.Aggl.Class", "",   &m_lMaxAgglomClass, MF_PARAMETER);
  DD.Double("Nucl.Plant.Corr", "", &m_dNuclPlantCorr, MF_PARAMETER, MC_);
  DD.Double("Agglomerisation.Const", "", &m_dKAgglom, MF_PARAMETER, MC_);
  DD.Double("Surface.Activity", "", &m_dSurfaceAct, MF_PARAMETER, MC_);
  DD.Double("SodaConst", "", &m_dSodaConst, MF_PARAMETER, MC_);
  


  DD.Text("Linear Growth Rate Equation");
  DD.Double("GrowthActEnergy",     "", &m_dActEGrowth    ,MF_PARAMETER, MC_T("K"));
  DD.Double("GrowthConst", "",    	       &m_dK_G   	    ,MF_PARAMETER, MC_);
  DD.Double("GrowthPower", "",    	       &m_dgamma_g          ,MF_PARAMETER, MC_);


  DD.Text("Nucleation Rate Equation");
  DD.Long  ("NucleationTerm", "", &m_lNuclTerm, MF_PARAMETER|MF_SET_ON_CHANGE, DDB1);
  
  DD.Double("NucleationActEnergy", "", &m_dActENucleation       ,MF_PARAMETER, MC_T("K"));
  DD.Double("NucleationConstant", "",    	       &m_dK_N	    ,MF_PARAMETER, MC_);
  DD.Double("NucleationPower", "",    	       &m_dgamma_N 	    ,MF_PARAMETER, MC_);

  DD.Text("Agglomeration Rate Equation");


  DD.Double("Solid.Den.Pwr", "",    	       &m_dn_s  	    ,MF_PARAMETER, MC_);
  DD.Double("Growth.Pwr", "",    	       &m_dgamma_2	    ,MF_PARAMETER, MC_);
  //  DD.Double("K_A", "",    	       &m_dK1   	    ,MF_PARAMETER, MC_);

  DD.Page("Classification");
  DD.Double("ExcessBypassUFlow", "",   &m_dBy_Pass         ,MF_PARAMETER, MC_);
  DD.Double("MeasureSharpness",  "",   &m_dSharp_Index     ,MF_PARAMETER, MC_);
  DD.Double("CutSize",           "",   &m_dCut_Size        ,MF_PARAMETER, MC_);
  DD.Double("LiquorToUFlow",  "",      &m_dSlurry_split    ,MF_PARAMETER, MC_);
  DD.Double("OverPassOFlow",  "",      &m_dOver_pass       ,MF_PARAMETER, MC_);
  DD.Double("L_0",  "",                &m_dL0              ,MF_PARAMETER, MC_);
	  
   



#ifdef TTDEBUG
  //std::string s;
  DD.Show(bTTDebug);
  DD.Page("Inlet");
  DD.Long("PSD.Display", "", &iPSD, MF_PARAMETER|MF_SET_ON_CHANGE, DDB2);
  const int NumSpecies = gs_MVDefn.Count();
  for (int i=0; i<=nClasses; i++) { 
    std::stringstream os;
    os << "Size " << std::setw(3) << i << std::setw(12) << L[i] << " " << Lav[i] ;
    // s=os.str();
    DD.Double(os.str().c_str(), "", dd[2]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Page("Size In");
  for (int i=0; i<=nClasses; i++) { 
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    // s=os.str();
    DD.Double(os.str().c_str(), "", dd[3]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Page("Size OF");
  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[4]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Page("Size UF");
  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[5]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
  DD.Page("Split.uf.of");
  for (int i=0; i<=nClasses; i++) {
    std::stringstream os;
    os << "Size " << std::setw(3) << i;
    DD.Double(os.str().c_str(), "", dd[6]+i, MF_RESULT|MF_NO_FILING, MC_None);
  }
    
#endif




  


  if (!m_bEvapConnected && iEvapMethod!=EVAP_NONE) //if optional Evap is NOT connected and evaporation functionality is required show the output stream
    DD.Object(Evap, MDD_RqdPage);

  DD.Show(m_RB.Enabled());  
  DD.Page("RB");
  m_RB.BuildDataFields();
  DD.Show();
}

//---------------------------------------------------------------------------

bool CPrecipitator::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
  }
