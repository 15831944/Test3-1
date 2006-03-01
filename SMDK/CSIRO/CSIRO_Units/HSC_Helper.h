#pragma once  
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#import "c:\windows\system32\gibbs5.dll" named_guids

const long GibbsSpecieCount = 24;
extern char* RqdGibbsSpecies[GibbsSpecieCount];

extern const char* HSC_file_path;

class CGibbsHelper
  {
  public:
    double Ml[GibbsSpecieCount]; //molar flow [kmol/s]
    double T; //temperature [K]
    long Iterations;
    double gmin;
    double CalcTime;
    CString sUnitTag;
  };

class CHSCHelper
  {
  public:
    CHSCHelper();
    ~CHSCHelper();

    int ExecuteGibbs(CGibbsHelper & GH);

    short guessFlag;
    short fastFlag;
    short dampenFlag;
    double time;

  private:
    bool bDoneInit;
    bool bOK;
    
    void Init();
    bool CreateInputFile(CGibbsHelper & GH);
    int ExtractOutputFile(CGibbsHelper & GH);

  protected:
    Gibbs5::_GibbsPtr pGibbs;
  };

extern CHSCHelper gs_HSC; //global static pointer to the one and only one interface to the HSC gibbs5 DLL

//===========================================================================
