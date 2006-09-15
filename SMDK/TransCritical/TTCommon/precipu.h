//------------------------------------------------------------------------
//   New Precipitation model Transcritical Technologies Pty Ltd Feb 05
//   Time-stamp: <2005-02-18 09:50:27 Rod Stephenson Transcritical Pty Ltd>


//-------------------------------------------------------------------------
// FUNCTION Gibbsite Enthalpy of Dissolution (per kJ/kg-Gibbsite).
// Correlation taken from 1995 QAL Tech report by JS McFeaters and converted to 
// MJ per unit of Gibbsite from kJ per unit of Alumina.
//-------------------------------------------------------------------------
double GibbsiteHOR(double t) { return  (412.82 + (t * 0.91166)); }
//-------------------------------------------------------------------------
// FUNCTION Boehmite Enthalpy of Dissoslution (per kJ/kg-Boehmite).
// Correlation taken from 1995 QAL Tech report by JS McFeaters and converted to 
// MJ per unit of Boemite from kJ per unit of Alumina.
//-------------------------------------------------------------------------
double  BoemiteHOR(double t) {  return (144.5 + (t * 0.868)); }
