// SysCADConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define TESTORIGINAL 1
#define TESTGIBBS 0
#define TESTSHOW 0

#if TESTGIBBS
#import "c:\windows\system32\gibbs5.dll" named_guids
#else
#define CNMTESTS  1
#if CNMTESTS  
#import "\SysCAD\trunk\bin\ScdMdl.tlb" //named_guids 
#import "\SysCAD\trunk\bin\ScdSlv.tlb" //named_guids 
#import "\SysCAD\trunk\bin\SysCAD.tlb" named_guids//, no_namespace 
#else
#import "c:\116net\bin\ScdMdl.tlb" //named_guids 
#import "c:\116net\bin\ScdSlv.tlb" //named_guids
#import "c:\116net\bin\SysCAD.tlb" named_guids//, no_namespace 
#endif
#endif

//======================================================================
#if TESTORIGINAL

void ExecOriginal(int N, int RunTime, int NBrk)
  {
  HRESULT hRes;
  for (int iPass=0; iPass<N; iPass++)
    {
    ScdApp::IScdApplicationPtr  pSysCAD = NULL;
    ScdApp::IScdProjectPtr      pPrj = NULL;
    ScdApp::IScdHistorianPtr    pHst = NULL;
    ScdApp::IScdReportsPtr      pRpts = NULL;
    ScdApp::IScdAppTagsPtr      pTags = NULL;
    ScdApp::IScdOPCServerPtr    pOPC = NULL;
    ScdSlv::IScdSolverPtr       pSlv = NULL;

    try
      {
      if(pSysCAD == NULL)
        {
        printf("CreateInstance %i\n", iPass);
        hRes = pSysCAD.CreateInstance(ScdApp::CLSID_ScdApplication);
        }
      if (pSysCAD != NULL && SUCCEEDED(hRes))
        {
        bool bSetExcel = false;
        bool bGetExcel = false;
        bool bSetHistTagList = false;
        bool bGetHistData = false;
        bool bSetOPC_On = false;

#if CNMTESTS  
        _bstr_t bstrScenarionPath = L"D:\\SysCAD\\90\\Projects\\Iluka 110\\TstStkPile-05.spf\\";
#else
        _bstr_t bstrScenarionPath = L"C:\\SysCAD90\\Projects - Iluka 110\\Projects\\PPT_MW_0505-03.spf\\";
#endif
        _bstr_t bstrScenarionProject = bstrScenarionPath + L"project.spj";

        //Open project
        printf("Open project\n");
        pPrj = pSysCAD->OpenProject(bstrScenarionProject);
        if (pPrj!=NULL)
          {
          pSlv = pPrj->GetSolver();
          pHst = pPrj->GetHistorian();
          pRpts = pPrj->GetReports();
          pTags = pPrj->GetTags();

          if (bSetOPC_On)
            {
            //Set OPC Server ON
            pOPC = pPrj->GetOPCServer();
            pOPC->put_On(true);
            pOPC.Release();
            }

          //Set solver time
          printf("SetTime\n");
          _variant_t T;
          pSlv->get_Time(ScdSlv::eScdTimeFmt_Date1900, &T); // to test
          pSlv->get_Time(ScdSlv::eScdTimeFmt_Secs1970, &T); //  eScdTimeFmt_Secs1970 & eScdTimeFmt_Seconds are the same
          pSlv->get_Time(ScdSlv::eScdTimeFmt_Seconds, &T);  //
          T=38106.75; // 29 April 2004 - 1800hrs
          pSlv->put_Time(ScdSlv::eScdTimeFmt_Date1900, T);
          _variant_t StartTime;
          StartTime = T;

          if (bSetHistTagList)
            {
            //delete historian
            pHst->Delete();

            //Ask historian to record tags
            printf("Set Historian Record tags\n");
            _bstr_t bstrHistorianTags = bstrScenarionPath + L"CSVFiles\\HistoryTags\\HistoryTags.csv";
            pHst->RecordTags(bstrHistorianTags);
            }

          if (bSetExcel)
            {
            //Set tags from Excel to SysCAD
            printf("Excel Inputs\n");
            _bstr_t bstrExcelInput = bstrScenarionPath + L"inputs.xls";
            _bstr_t bstrDataSetName = "Scenario";
            pRpts->Process(ScdApp::eScdRT_SetValues, bstrExcelInput, bstrDataSetName, true);
            }

          //set other SysCAD tags
          printf("Set Tags\n");
          //pTags = pPrj->GetTags();
          double dStepSize = 4.0*3600; //seconds
          pTags->put_TagValue(_bstr_t("$Dyn.StepSize (s)"), _variant_t(dStepSize));
          pTags->put_TagValue(_bstr_t("$Dyn.TimeIncMax (s)"), _variant_t(dStepSize));
          pTags->put_TagValue(_bstr_t("$Dyn.TimeIncMin (s)"), _variant_t(dStepSize));

          printf("Starting...");
          HRESULT hr;
          hr = pSlv->GetDynamic()->Start();
          printf("Started\n");

          _variant_t Iterations;
          _variant_t StateDesc;
          VARIANT_BOOL Idling = false;
          VARIANT_BOOL Running = false;
          VARIANT_BOOL Stopped = false;
          //printf("Wait\n");
          for (int i=0;i<10;i++)
            {
            Sleep(RunTime*200);
            pTags->get_TagValue(_bstr_t("PlantModel.Stats.Iterations"), &Iterations);
            pTags->get_TagValue(_bstr_t("PlantModel.StateDesc"), &StateDesc);
            printf("Iteration:%d %s\n", Iterations.lVal, (LPCTSTR)(_bstr_t)(StateDesc));

            pSlv->GetDynamic()->get_IsIdling(&Idling);
            pSlv->GetDynamic()->get_IsRunning(&Running);
            pSlv->GetDynamic()->get_IsStopped(&Stopped);
            printf("%d ", i);
            if (Idling)
              printf("Idling ");
            if (Running)
              printf("Running ");
            if (Stopped)
              printf("Stopped ");
            printf("\n");
            }

          // force an exception if reqd
          if (iPass==NBrk)
            {
            *((double *)NULL)=0;
            }

          printf("Stopping...");
          hr = pSlv->GetDynamic()->Stop();
          printf("Stopped\n");

          if (bGetExcel)
            {
            //Get tags from SysCAD to Excel             
            printf("Excel Report\n");
            _bstr_t bstrExcelOutput = bstrScenarionPath + L"outputs.xls";
            _bstr_t bstrDataSetName1 = "Scenario";
            pRpts->Process(ScdApp::eScdRT_GetValues, bstrExcelOutput, bstrDataSetName1, true);
            }

          //Get other SysCAD tags
          printf("Get Tags\n");
          _variant_t SolveTimeDesc;
          _variant_t SimulationTimeDesc;
          pTags->get_TagValue(_bstr_t("PlantModel.Stats.SolveTimeDesc"), &SolveTimeDesc);
          pTags->get_TagValue(_bstr_t("PlantModel.Stats.SimulationTimeDesc"), &SimulationTimeDesc);
          printf("Solve time:%s\n", (LPCTSTR)(_bstr_t)SolveTimeDesc);
          printf("Simulation time:%s\n", (LPCTSTR)(_bstr_t)SimulationTimeDesc);

          if (bGetHistData)
            {
            //Retrieve tags from historian to csv
            _bstr_t bstrHistorianReport = bstrScenarionPath + L"HistReport";
            double dDuration = 365*24*3600; //seconds
            double dInterval = 1*24*3600; //seconds
            pHst->ExtractData(bstrHistorianReport, ScdSlv::eScdTimeFmt_Date1900, StartTime, dDuration, dInterval, ScdApp::eScdHDEO_CSV);
            //pHst->ExtractData(L"\\\\per1642\\xxxi\\HistReport", StartTime, dDuration, dInterval, ScdApp::eScdHDEO_CSV);
            }

          pTags.Release();
          pRpts.Release();
          pSlv.Release();
          pHst.Release();
          }

        printf("Close\n");
        pSysCAD->CloseProject(VARIANT_FALSE);
        pPrj.Release();
        }

      printf("Release\n");
      if(pSysCAD) 
        pSysCAD.Release();
      }
    catch(...)
      {
      // Manually release all interfaces
      pTags.Release();
      pRpts.Release();
      pHst.Release();
      pSlv.Release();
      pOPC.Release();
      pPrj.Release();
      printf("Release Final\n");
      if(pSysCAD) 
        pSysCAD.Release();
      break;
      }
    }
  printf("Done. Exit in ");
  for (int i=6; i; i--)
    {
    Sleep(1000);
    printf("%d", i);
    }
  printf("0\n", i);
  }

#endif
//======================================================================
//======================================================================
#if TESTSHOW

void ExecShow()
  {
  HRESULT hRes;
  ScdApp::IScdApplicationPtr  pSysCAD = NULL;
  ScdApp::IScdProjectPtr      pPrj = NULL;
  ScdApp::IScdHistorianPtr    pHst = NULL;
  ScdApp::IScdReportsPtr      pRpts = NULL;
  ScdApp::IScdAppTagsPtr      pTags = NULL;
  ScdApp::IScdOPCServerPtr    pOPC = NULL;

  ScdSlv::IScdSolverPtr       pSlv = NULL;

  try
    {
    printf("CreateInstance\n");
    hRes = pSysCAD.CreateInstance(ScdApp::CLSID_ScdApplication);

    if (pSysCAD != NULL && SUCCEEDED(hRes))
      {
      bool bSetExcel = false;
      bool bGetExcel = false;
      _bstr_t bstrScenarionPath = L"D:\\SysCAD\\90\\Projects\\Iluka 110\\TstStkPile-05.spf\\";


      printf("Release\n");
      if(pSysCAD) 
        pSysCAD.Release();
      }
    }
  catch(...)
    {
    // Manually release all interfaces
    pOPC.Release();
    pTags.Release();
    pRpts.Release();
    pHst.Release();
    pSlv.Release();
    pPrj.Release();
    printf("Release Final\n");
    if(pSysCAD) 
      pSysCAD.Release();
    }
  }

#endif
//======================================================================

//======================================================================
#if TESTGIBBS

void ExecGibbs()
  {
  Gibbs5::_GibbsPtr pGibbs = NULL;
  try
    {
    if(pGibbs == NULL)
      {
      printf("CreateInstance\n");
      hRes = pGibbs.CreateInstance(Gibbs5::CLSID_Gibbs);
      if (SUCCEEDED(hRes))
        {
        //_bstr_t infile("C:\\LimeCN\\SysCAD.igi");
        //_bstr_t outfile("C:\\LimeCN\\SysCAD.ogi");
        _bstr_t infile = L"C:\\113net\\SMDK\\CSIRO\\SysCAD.igi";
        _bstr_t outfile = L"C:\\113net\\SMDK\\CSIRO\\SysCAD.ogi";
        short gf = 0;
        short ff = 0;
        short df = 1;
        double Time = 0.0;
        //printf("Out1:%s\n", outfile.GetAddress());
        printf("Out2:%s\n", (char*)outfile);
	      hRes = pGibbs->raw_CalculateIgi(&(infile.GetBSTR()), &(outfile.GetBSTR()), &gf, &ff, &df, &Time);
	      //hRes = pGibbs->raw_CalculateIgi(infile.GetAddress(), outfile.GetAddress(), &gf, &ff, &df, &Time);
        printf("Out3:%s\n", outfile.GetAddress());
        printf("Out4:%s\n", (char*)outfile);
	      //hRes = Gibbs5::_Gibbs::raw_CalculateIgi(infile.GetAddress(), outfile.GetAddress(), &gf, &ff, &df, &Time);
        //Call Gibbs.CalculateIgi("C:\LimeCN\SysCAD.igi", "C:\LimeCN\SysCAD.ogi", False, False, True, Time)
        }
      }
    printf("Release\n");
    if(pGibbs) 
      pGibbs.Release();
    }
  catch(...)
    {
    if(pGibbs) 
      pGibbs.Release();
    }
  }

#endif
//======================================================================

int _tmain(int argc, _TCHAR* argv[])
  {
  HRESULT hRes = CoInitialize(NULL);
 
  #if TESTSHOW
    ExecShow();
  #else
  #if TESTGIBBS
    ExecGibbs();
  #else
  #if TESTORIGINAL
    int N=1, RunTime=1, NBrk=-1;
    if (argc>=2)                    
      RunTime=atoi(argv[1]);    // Run Time
    if (argc>=3)                    
      N=atoi(argv[2]);          // Number of loops
    if (argc>=4)                    
      NBrk=atoi(argv[3]);       // Loop number for Exception
    ExecOriginal(N, RunTime, NBrk);
  #endif
  #endif
  #endif

  CoUninitialize();
  return 0;
  }

//======================================================================
