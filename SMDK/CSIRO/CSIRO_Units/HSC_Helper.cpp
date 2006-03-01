//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "hsc_helper.h"
#pragma optimize("", off)

#import "c:\windows\system32\gibbs5.dll" named_guids

#define TestLocation 0
#if TestLocation
const char* HSC_file_path = "C:\\113net\\SMDK\\CSIRO\\";
#else
const char* HSC_file_path = "C:\\SysCAD90\\SMDK\\CSIRO\\";
#endif

#define UseThread 0

//====================================================================================

CHSCHelper gs_HSC;
char* RqdGibbsSpecies[GibbsSpecieCount] = { "N2(g)", "Ca2+(aq)", "CN-(aq)", "Cu+(aq)", "Cu2+(aq)", "Cu[CN]2-(aq)", "Cu[CN]32-(aq)", "Cu[CN]43-(aq)", "Fe2+(aq)", "Fe3+(aq)", "Fe[CN]63-(aq)", "Fe[CN]64-(aq)", "H+(aq)", "HCN(aq)", "H2O(l)", "Mg2+(aq)", "OH-(aq)", "SO42-(aq)", "CuCN(s)", "Ca[OH]2(s)", "CaSO4*2H2O(s)", "Fe[OH]2(s)", "Fe[OH]3(s)", "Mg[OH]2(s)"  };

/*

Species order
 'N2(g)              
 'Ca(+2a)    
 'CN(-a)                 
 'Cu(+a)                
 'Cu(+2a)               
 'Cu(CN)2(-a)           
 'Cu(CN)3(-2a)          
 'Cu(CN)4(-3a)          
 'Fe(+2a)               
 'Fe(+3a)               
 'Fe(CN)6(-3a)          
 'Fe(CN)6(-4a)          
 'H(+a)                 
 'HCN(a)                
 'H2O                   
 'Mg(+2a)               
 'OH(-a)                
 'SO4(-2a)              
 'CuCN                  
 'Ca(OH)2               
 'CaSO4*2H2O            
 'Fe(OH)2               
 'Fe(OH)3               
 'Mg(OH)2        
*/

//---------------------------------------------------------------------------
#if UseThread
CRITICAL_SECTION HSC_Sect;
HANDLE HSC_Signal;  //handle for semaphore
#endif

CHSCHelper::CHSCHelper()
  {
#if UseThread
  InitializeCriticalSection(&HSC_Sect);
  HSC_Signal = CreateSemaphore((LPSECURITY_ATTRIBUTES)NULL, 0, 0x7fffffff, NULL);
#endif
  pGibbs = NULL;
  guessFlag = 0;
  fastFlag = 0;
  dampenFlag = 1;
  bDoneInit = false;
  bOK = false;
  }

//---------------------------------------------------------------------------

CHSCHelper::~CHSCHelper()
  {
#if UseThread
  CloseHandle(HSC_Signal);
  DeleteCriticalSection(&HSC_Sect);
#else
  if (pGibbs) 
    pGibbs.Release();
#endif
  }

//---------------------------------------------------------------------------

void CHSCHelper::Init()
  {
  if (1)
    {
    HRESULT hRes = pGibbs.CreateInstance(Gibbs5::CLSID_Gibbs);
    bOK = (SUCCEEDED(hRes));
    }
  else
    {
    bOK = true;
    }
  bDoneInit = true;
  }

//---------------------------------------------------------------------------
const char* Header = 
"§ Created by Visual C++ DLL, (C) CSIRO Minerals\n"
"  1\n"
"  10   8   6   1  17   1   1   1   1   1   1\n"
" 'N2(g)                   '  2.00  0.00  0.00  0.00  0.00  0.00  0.00  0.00  0.00  0.00\n"
" 'Ca(+2a)                 '  0.00  1.00 -2.00  0.00  0.00  0.00  0.00  0.00  0.00  0.00\n"
" 'CN(-a)                  '  1.00  0.00  1.00  1.00  0.00  0.00  0.00  0.00  0.00  0.00\n"
" 'Cu(+a)                  '  0.00  0.00 -1.00  0.00  1.00  0.00  0.00  0.00  0.00  0.00\n"
" 'Cu(+2a)                 '  0.00  0.00 -2.00  0.00  1.00  0.00  0.00  0.00  0.00  0.00\n"
" 'Cu(CN)2(-a)             '  2.00  0.00  1.00  2.00  1.00  0.00  0.00  0.00  0.00  0.00\n"
" 'Cu(CN)3(-2a)            '  3.00  0.00  2.00  3.00  1.00  0.00  0.00  0.00  0.00  0.00\n"
" 'Cu(CN)4(-3a)            '  4.00  0.00  3.00  4.00  1.00  0.00  0.00  0.00  0.00  0.00\n"
" 'Fe(+2a)                 '  0.00  0.00 -2.00  0.00  0.00  1.00  0.00  0.00  0.00  0.00\n"
" 'Fe(+3a)                 '  0.00  0.00 -3.00  0.00  0.00  1.00  0.00  0.00  0.00  0.00\n"
" 'Fe(CN)6(-3a)            '  6.00  0.00  3.00  6.00  0.00  1.00  0.00  0.00  0.00  0.00\n"
" 'Fe(CN)6(-4a)            '  6.00  0.00  4.00  6.00  0.00  1.00  0.00  0.00  0.00  0.00\n"
" 'H(+a)                   '  0.00  0.00 -1.00  0.00  0.00  0.00  1.00  0.00  0.00  0.00\n"
" 'HCN(a)                  '  1.00  0.00  0.00  1.00  0.00  0.00  1.00  0.00  0.00  0.00\n"
" 'H2O                     '  0.00  0.00  0.00  0.00  0.00  0.00  2.00  1.00  0.00  0.00\n"
" 'Mg(+2a)                 '  0.00  0.00 -2.00  0.00  0.00  0.00  0.00  0.00  1.00  0.00\n"
" 'OH(-a)                  '  0.00  0.00  1.00  0.00  0.00  0.00  1.00  1.00  0.00  0.00\n"
" 'SO4(-2a)                '  0.00  0.00  2.00  0.00  0.00  0.00  0.00  4.00  0.00  1.00\n"
" 'CuCN                    '  1.00  0.00  0.00  1.00  1.00  0.00  0.00  0.00  0.00  0.00\n"
" 'Ca(OH)2                 '  0.00  1.00  0.00  0.00  0.00  0.00  2.00  2.00  0.00  0.00\n"
" 'CaSO4*2H2O              '  0.00  1.00  0.00  0.00  0.00  0.00  4.00  6.00  0.00  1.00\n"
" 'Fe(OH)2                 '  0.00  0.00  0.00  0.00  0.00  1.00  2.00  2.00  0.00  0.00\n"
" 'Fe(OH)3                 '  0.00  0.00  0.00  0.00  0.00  1.00  3.00  3.00  0.00  0.00\n"
" 'Mg(OH)2                 '  0.00  0.00  0.00  0.00  0.00  0.00  2.00  2.00  1.00  0.00\n"
" 4\n";

const char* Middle1 = 
"  0.000000E+00  1.916105E+02  9\n"
"  2.929831E+01 -1.567199E-03 -7.232298E+02  3.418894E-06  0.000000E+00  0.000000E+00\n"
"   350.000      0.000000E+00\n"
"  2.775317E+01  6.047626E-04  7.283212E+04  4.959830E-06  0.000000E+00  0.000000E+00\n"
"   700.000      0.000000E+00\n"
"  2.352867E+01  1.211645E-02  1.210385E+05 -3.076219E-06  0.000000E+00  0.000000E+00\n"
"  1500.000      0.000000E+00\n"
"  3.536803E+01  1.041014E-03 -4.146351E+06 -1.111572E-07  0.000000E+00  0.000000E+00\n"
"  3400.000      0.000000E+00\n"
"  4.149648E+01 -1.249780E-03 -1.801499E+07  1.361319E-07  0.000000E+00  0.000000E+00\n"
"  6000.000      0.000000E+00\n"
"  7.238607E+01 -8.392248E-03 -1.904021E+08  6.018330E-07  0.000000E+00  0.000000E+00\n"
" 10000.000      0.000000E+00\n"
" -2.146581E+02  2.957306E-02  4.720399E+09 -8.154477E-07  0.000000E+00  0.000000E+00\n"
" 13000.000      0.000000E+00\n"
" -1.768727E+02  2.763313E-02  2.583514E+09 -8.150473E-07  0.000000E+00  0.000000E+00\n"
" 16500.000      0.000000E+00\n"
"  4.197048E+02 -2.128860E-02 -2.382031E+10  3.148703E-07  0.000000E+00  0.000000E+00\n"
" -5.430832E+05 -8.987592E+01  2\n"
"  1.242970E+04 -5.130678E+01 -2.168100E+08  5.934561E-02  0.000000E+00  0.000000E+00\n"
"   333.150      0.000000E+00\n"
" -1.831311E+03  7.765371E+00  3.046790E+07 -9.551703E-03  0.000000E+00  0.000000E+00\n"
"  1.506240E+05  6.074808E+01  2\n"
"  2.683157E+04 -1.109675E+02 -4.701921E+08  1.290272E-01  0.000000E+00  0.000000E+00\n"
"   333.150      0.000000E+00\n"
" -1.523787E+03  6.926776E+00  1.757967E+07 -8.970651E-03  0.000000E+00  0.000000E+00\n"
"  7.168029E+04  7.192879E+00  2\n"
"  2.118213E+03 -8.458134E+00 -3.555778E+07  9.685775E-03  0.000000E+00  0.000000E+00\n"
"   333.150      0.000000E+00\n"
" -5.936023E+02  2.710936E+00  1.202810E+07 -3.269245E-03  0.000000E+00  0.000000E+00\n"
"  6.568880E+04 -1.304607E+02  2\n"
"  1.965291E+04 -8.102065E+01 -3.428102E+08  9.394645E-02  0.000000E+00  0.000000E+00\n"
"   333.150      0.000000E+00\n"
" -2.011878E+03  8.870886E+00  3.150666E+07 -1.106635E-02  0.000000E+00  0.000000E+00\n"
"  2.740000E+05  1.716081E+02  1\n"
"  0.000000E+00 -2.797103E-01  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
"  3.340000E+05  2.096081E+02  1\n"
"  0.000000E+00  6.526606E-02  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
"  4.378000E+05  1.756081E+02  1\n"
"  0.000000E+00 -1.131713E-02  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
" -9.225720E+04 -1.392471E+02  2\n"
"  2.039299E+04 -8.418110E+01 -3.562709E+08  9.765793E-02  0.000000E+00  0.000000E+00\n"
"   333.150      0.000000E+00\n"
" -1.943789E+03  8.530467E+00  2.937468E+07 -1.068499E-02  0.000000E+00  0.000000E+00\n"
" -4.958040E+04 -3.107911E+02  2\n"
"  3.134064E+04 -1.294335E+02 -5.474486E+08  1.499751E-01  0.000000E+00  0.000000E+00\n"
"   333.150      0.000000E+00\n"
" -3.668348E+03  1.575243E+01  5.807319E+07 -1.955126E-02  0.000000E+00  0.000000E+00\n"
"  5.619530E+05  2.369363E+02  1\n"
"  0.000000E+00  3.477597E-01  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
"  4.556376E+05  6.162672E+01  1\n"
"  0.000000E+00 -5.561915E-01  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
"  0.000000E+00 -3.339192E+01  2\n"
"  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
"   333.150      0.000000E+00\n"
"  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
"  1.071104E+05  9.129128E+01  2\n"
" -1.839542E+04  7.667705E+01  3.256124E+08 -8.953529E-02  0.000000E+00  0.000000E+00\n"
"   333.150      0.000000E+00\n"
" -2.044123E+02  7.736513E-01  1.507514E+07 -3.865749E-04  0.000000E+00  0.000000E+00\n"
" -2.858300E+05  6.995000E+01  6\n"
" -2.623368E-01  1.405184E-01  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
"   273.151      6.006969E+03\n"
"  1.868842E+02 -4.642470E-01 -1.956497E+06  5.486312E-04  0.000000E+00  0.000000E+00\n"
"   495.000      0.000000E+00\n"
"  1.493077E+04 -3.949327E+01 -6.258060E+08  2.961320E-02  0.000000E+00  0.000000E+00\n"
"   515.000      0.000000E+00\n"
" -9.022881E+02  2.043884E+00  5.574553E+07 -1.033564E-03  0.000000E+00  0.000000E+00\n"
"   540.000      0.000000E+00\n"
"  2.260505E+04 -5.499107E+01 -1.127182E+09  3.788351E-02  0.000000E+00  0.000000E+00\n"
"   600.000      0.000000E+00\n"
"  1.174059E+02  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
" -4.659595E+05 -1.714639E+02  2\n"
"  2.503097E+04 -1.032821E+02 -4.373187E+08  1.199274E-01  0.000000E+00  0.000000E+00\n"
"   333.150      0.000000E+00\n"
" -1.965478E+03  8.845748E+00  2.810659E+07 -1.119037E-02  0.000000E+00  0.000000E+00\n"
" -2.300238E+05 -4.410296E+01  2\n"
"  4.092852E+04 -1.694011E+02 -7.180118E+08  1.970896E-01  0.000000E+00  0.000000E+00\n"
"   333.150      0.000000E+00\n"
" -1.918448E+03  8.830259E+00  1.829773E+07 -1.162882E-02  0.000000E+00  0.000000E+00\n"
" -9.096016E+05 -1.456392E+01  2\n"
"  7.160955E+04 -2.964848E+02 -1.256543E+09  3.448830E-01  0.000000E+00  0.000000E+00\n"
"   333.150      0.000000E+00\n"
" -3.610891E+03  1.636638E+01  3.648847E+07 -2.143251E-02  0.000000E+00  0.000000E+00\n"
"  9.497680E+04  8.999784E+01  2\n"
"  6.256754E+01  1.991166E-02 -6.267630E+05  9.372200E-07  0.000000E+00  0.000000E+00\n"
"   746.000      1.255200E+04\n"
"  8.368000E+01  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
" -9.859000E+05  8.340000E+01  2\n"
"  8.924762E+01  3.315045E-02 -1.034809E+06 -2.253128E-08  0.000000E+00  0.000000E+00\n"
"  1023.000      2.900000E+04\n"
"  1.530000E+02  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
" -2.022629E+06  1.940999E+02  1\n"
"  9.137856E+01  3.179840E-01  0.000000E+00  0.000000E+00  0.000000E+00  0.000000E+00\n"
" -5.720000E+05  9.300000E+01  1\n"
"  9.510672E+01  2.847733E-02 -5.864392E+05  1.116779E-09  0.000000E+00  0.000000E+00\n"
" -8.360000E+05  1.050000E+02  1\n"
"  1.624770E+02  1.191614E-02 -4.358058E+06 -3.481407E-08  0.000000E+00  0.000000E+00\n"
" -9.246640E+05  6.313656E+01  1\n"
"  1.000551E+02  1.833696E-02 -2.525456E+06 -1.668536E-08  0.000000E+00  0.000000E+00\n"
" 24  1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24\n";

const char* Middle2 = 
 " 2\n"
 " 1\n"
 " 2 2 3 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2\n";

const char* Footer = " 10\n";

const int HeaderLen = strlen(Header);
const int Middle1Len = strlen(Middle1);
const int Middle2Len = strlen(Middle2);
const int FooterLen = strlen(Footer);

const double MoleAmountScaleFactor = 1000.0;

bool CHSCHelper::CreateInputFile(CGibbsHelper & GH)
  {
  CString Fn = HSC_file_path;
  Fn += GH.sUnitTag;
  Fn += ".igi";
  FILE* f=fopen((const char*)Fn, "wt");
  if (!f)
    return false;

//  strcpy(Buff, ";--- SysCAD General Controller (PGM) program logic file ---\n\n;--- variable declarations ---\n\n\nif (OnInitialise)\n  ;--- place logic here for execution before first iteration --- (use SetDynTag in OnInitialise)\n  \nelseif (OnTerminate)\n");
  char Buff[2048];
  fwrite(Header, sizeof(char), HeaderLen, f);

  sprintf(Buff, " %.2f\n", GH.T);
  fwrite(Buff, sizeof(char), strlen(Buff), f);

  fwrite(Middle1, sizeof(char), Middle1Len, f);

  Buff[0]=0;
  for (int i=0; i<GibbsSpecieCount; i++)
    {
    sprintf(Buff, "%s  %7.3f  ", Buff, GH.T);
    }
  strcat(Buff, "\n");
  fwrite(Buff, sizeof(char), strlen(Buff), f);

  fwrite(Middle2, sizeof(char), Middle2Len, f);

  char *p1, *p2; //added for exponent digit removal code added by SM
  for (int i=0; i<GibbsSpecieCount; i++)
    {
    sprintf(Buff, " %.7E\n", MoleAmountScaleFactor*GH.Ml[i]);
    //Removes leading zero from VC++ 3-digit exponent - HSC needs 2-digit exponent	
    p1 = p2 = strchr(Buff, 'E') + 2;
    *p2 = *++p1;
	  p2++;
	  *p2 = *++p1;
	  *++p2 = 0;

  	//HSC requires step amount for CN- only, or terminate line
  	if (i==2)
	  	strcat(Buff, " 1.0000000E-02");
		strcat(Buff, "\n");
	
    fwrite(Buff, sizeof(char), strlen(Buff), f);
    }
	
	fwrite(Footer, sizeof(char), FooterLen, f);
    
  fclose(f);

  if (1)
    {//empty output file...
    Fn = HSC_file_path;
    Fn += GH.sUnitTag;
    Fn += ".ogi";
    f=fopen((const char*)Fn, "wt");
    if (f)
      {
      fclose(f);
      }
    }
  return true;
  }

//---------------------------------------------------------------------------

int CHSCHelper::ExtractOutputFile(CGibbsHelper & GH)
  {
  CString Fn = HSC_file_path;
  Fn += GH.sUnitTag;
  Fn += ".ogi";
  FILE* f=fopen((const char*)Fn, "rt");
  if (!f)
    return -1;

  char readline[1024];
  if (fgets(readline, sizeof (readline), f)==NULL)
    {
    fclose(f);
    return -2;//empty file
    }

  char Value1[32],Value2[32],Value3[32];
  double amount1,amount2;
  int i = 0;

  Value1[10] = 0;
  Value2[11] = 0;
  int len = 0;
  while (fgets(readline, sizeof (readline), f)!=NULL)	//Read until end of file
    {
    len = strlen(readline);
    if ((len>11) && strncmp(readline," Iteration ", 11)==0)
      {
      strncpy(Value3, &readline[10], 3);
      Value3[3] = 0;
      GH.Iterations = atoi(Value3);
      strncpy(Value3, &readline[28], 17);
      Value3[17] = 0;
      GH.gmin = atof(Value3);
      }
    if ((len>6) && strncmp(readline," PHASE", 6)==0)
      {
      fgets(readline, sizeof(readline), f);
      len = strlen(readline);
      bool FoundTotal = false;
      while (!FoundTotal)
		    {
        amount1=amount2=0.0;
        char* p = &readline[28];
        strncpy(Value1, &readline[28], 10);
        strncpy(Value2, &readline[40], 11);
        amount1 = atof(Value1);
        amount2 = atof(Value2);
        if (i<GibbsSpecieCount)
          {
          GH.Ml[i] = amount2/MoleAmountScaleFactor; //assume index is same as input!
          i++;
          }
        else
          {//Error: Extra species!!!
          }
		    if (fgets(readline, sizeof(readline), f)==NULL)
          {
          FoundTotal = true;
          }
        else
          {
          len = strlen(readline);
          if (len>6 && strncmp(readline," Total", 6)==0)
            FoundTotal = true;
          }
		    }
	    }
    }
  fclose(f);
  return true;
  }

//---------------------------------------------------------------------------

#if UseThread
UINT HSCThread( LPVOID pParam )
  {
  EnterCriticalSection(&HSC_Sect);
  CGibbsHelper* pGH = (CGibbsHelper*)pParam;
  Gibbs5::_GibbsPtr pGibbs_;
  HRESULT hRes = pGibbs_.CreateInstance(Gibbs5::CLSID_Gibbs);
  bool OK = (SUCCEEDED(hRes));
  if (OK)
    {
    pGH->CalcTime = 0.0;
    CString Fn1 = HSC_file_path;
    Fn1 += pGH->sUnitTag;
    CString Fn2 = Fn1;
    Fn1 += ".igi";
    Fn2 += ".ogi";
    BSTR infile = Fn1.AllocSysString();
    BSTR outfile = Fn2.AllocSysString();
    hRes = pGibbs_->raw_CalculateIgi(&infile, &outfile, &gs_HSC.guessFlag, &gs_HSC.fastFlag, &gs_HSC.dampenFlag, &(pGH->CalcTime));
    if (SUCCEEDED(hRes))
      {
      int xxx=0;
      }

    pGibbs_.Release();
    }
  LeaveCriticalSection(&HSC_Sect);
  ReleaseSemaphore(HSC_Signal, 1, NULL);
  return 0; // thread completed successfully
  }
#endif

//Sub CalculateIgi(igi_file As String, ogi_file As String, guessFlag As Integer, fastFlag As Integer, dampenFlag As Integer, time As Double)
//Creates the output file ogi_file$ using the specified flags (true or false) and returns the calculation time in time#.

int CHSCHelper::ExecuteGibbs(CGibbsHelper & GH)
  {
  if (!bDoneInit)
    Init();
  if (!bOK)
    return -1;

  if (!CreateInputFile(GH))
    return -2;

  //true/false flags MUST be passed as 0 or -1
  if (guessFlag!=0)
    guessFlag=-1;
  if (fastFlag!=0)
    fastFlag=-1;
  if (dampenFlag!=0)
    dampenFlag=-1;

  GH.CalcTime = 0.0;
  HRESULT hRes = 0;
#if UseThread
    //_bstr_t infile = L"C:\\113net\\SMDK\\CSIRO\\SysCAD.igi";
    //_bstr_t outfile = L"C:\\113net\\SMDK\\CSIRO\\SysCAD.ogi";
    //hRes = pGibbs->raw_CalculateIgi(&(infile.GetBSTR()), &(outfile.GetBSTR()), &guessFlag, &fastFlag, &dampenFlag, &GH.CalcTime);
    CWinThread* pt = AfxBeginThread(HSCThread, (LPVOID)&GH, THREAD_PRIORITY_NORMAL, 65536);
    Sleep(20); //give HSC thread some time to execute
    DWORD dwWaitResult; 
    //Try to enter the semaphore gate.
    dwWaitResult = WaitForSingleObject(HSC_Signal, 1000L);//INFINITE);
    if (dwWaitResult==WAIT_OBJECT_0)
      {// The semaphore object was signaled.
      int xxx=0;
      }
    else if (dwWaitResult==WAIT_TIMEOUT)
      {// Semaphore was nonsignaled, so a time-out occurred.
      int xxx=0;
      ReleaseSemaphore(HSC_Signal, 1, NULL);
      }
    if (0)
      {
      Sleep(200);
      CString Fn = HSC_file_path;
      Fn += GH.sUnitTag;
      Fn += ".ogi";
      CFileStatus State;
      int Cnt=50;
      if (CFile::GetStatus((LPCSTR)Fn, State))
        {
        while (Cnt-- && State.m_size==0)
          {
          Sleep(100);
          CFile::GetStatus((LPCSTR)Fn, State);
          }
        }
      if (Cnt==0)
        {//give up!!!
        Sleep(200);
        }
      }
#else
    CString Fn1 = HSC_file_path;
    Fn1 += GH.sUnitTag;
    CString Fn2 = Fn1;
    Fn1 += ".igi";
    Fn2 += ".ogi";
    BSTR infile = Fn1.AllocSysString();
    BSTR outfile = Fn2.AllocSysString();
    hRes = pGibbs->raw_CalculateIgi(&infile, &outfile, &guessFlag, &fastFlag, &dampenFlag, &GH.CalcTime);
    //SysFreeString(???Fn1);
#endif

  if (!SUCCEEDED(hRes))
    return -3;

  switch (ExtractOutputFile(GH))
    {
    case -1: return -4;
    case -2: return -5;
    }
  return 0;
  }

//---------------------------------------------------------------------------

