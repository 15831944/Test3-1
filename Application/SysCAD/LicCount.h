//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __LICCOUNT_H
#define  __LICCOUNT_H

/*#ifdef __LICCOUNT_CPP
  #define DllImportExport DllExport
#elif !defined(SCEXEC)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif*/
  #define DllImportExport

//===========================================================================

_FWDDEF(CLicenseCount)
class DllImportExport CLicenseCount
  {
  protected:
    flag    bLicBlock;   //true if licensing conditions are exceeded
    int     iNoOfUnits;  //no of units       
    int     iNoOfIllegalUnits;  //no of illegal units based on licensing


  public:
    CLicenseCount()
      {
      bLicBlock=0;
      iNoOfUnits=0;
      iNoOfIllegalUnits=0;
      }

    void    CalcUnits(flag ForceCnt = false);
    void    CalcLicUnits();
    void    CountUnits();
    flag    DynamicCntExceeded();
    flag    ProBalCntExceeded();

    flag    Blocked() { return bLicBlock; };   //true if licensing conditions are exceeded
    int     NoOfUnits() { return iNoOfUnits; };  //no of units       
    int     NoOfIllegalUnits() { return iNoOfIllegalUnits; };  //no of illegal units based on licensing
  };

extern DllImportExport CLicenseCount gs_LicenseCnt;

#undef DllImportExport 
//===========================================================================

#endif
