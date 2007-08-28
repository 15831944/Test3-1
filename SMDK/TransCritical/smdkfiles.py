##   Time-stamp: <2007-05-31 02:14:32 Rod Stephenson Transcritical Pty Ltd>
##   smdkfiles.py - Templates for all the files in an SMDK project
##   Import from smdkgen.py


makeFile = r"""#   Generic makefile for compiling SMDK using GNU tools and free MS Compilers
#   Time-stamp: <2007-05-28 00:21:31 Rod Stephenson Transcritical Pty Ltd>
SYSCAD=91
SYSCADDIR = C:\SysCAD$(SYSCAD)\bin

COMMON = ../TTCommon/

SMDKDLL = ${dllGroupName}

CFLAGS = /GF /FD /EHsc /MD /GR /W3 /nologo /c /Zi /wd4800 /YX"stdafx.h"
DFLAGS =  /D "_RELEASE" /D "NDEBUG" /D "SMDKDemoU" /D "WIN32" \
          /D "_WINDOWS" /D "_AFXEXT" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS"
LFLAGS = /INCREMENTAL:NO /NOLOGO /DLL /DEBUG  $(COMMON)ttcommon.lib
LIBS = SMDK1.lib scdlib.lib

OBJS = 	${dllGroupName}.obj   \
	${dllGroupName}UM.obj


ifeq ($(SYSCAD),91)
 IDIR = "..\..\smdk\Include"
 LDIR = "..\..\smdk\bin\rls"
else
 IDIR = "..\..\include"
 LDIR = "..\..\bin\rls"
endif


%.obj : %.cpp
	 cl /I $(IDIR) $(DFLAGS) $(CFLAGS) $<




${dllGroupName}.dll: $(OBJS)
	mkdir -p Rls
	link /OUT:"c:\SysCAD$(SYSCAD)\Bin\$(SMDKDLL).dll" $(LFLAGS) /PDB:".\Rls/$(SMDKDLL).pdb" \
	/IMPLIB:".\Rls/$(SMDKDLL).lib" /MACHINE:X86 /LIBPATH:$(LDIR) $(OBJS) $(LIBS)
	../../smdk/devlic.exe /q /f:"c:\syscad$(SYSCAD)\bin\$(SMDKDLL).dll"

${dllGroupName}.obj : ${dllGroupName}.cpp ${dllGroupName}.h
${dllGroupName}UM.obj : ${dllGroupName}UM.cpp ${dllGroupName}UM.h


clean:
	rm -f $(OBJS) vc70.pdb vc70.idb
"""





stdafxhFile = r"""// stdafx.h : include file for standard system include files,
//   Time-stamp: <2007-05-26 06:43:25 Rod Stephenson Transcritical Pty Ltd>
//  or project specific include files that are used frequently, but
//      are changed infrequently
//--------------------------------------------------------------------------

//SMDK: NB you must define your SMDK user DLL group name here!
#define DLL_GroupName "${dllGroupName}"
//--------------------------------------------------------------------------

#if !defined(STDAFX_${udllGroupName}_H_)
#define STDAFX_${udllGroupName}_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxtempl.h>

#include "ScdIF.h"

//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(STDAFX_${udllGroupName}_H_)
"""



stdafxcppFile = """\
// stdafx.cpp : source file that includes just the standard
	</TR> includes
//   Time-stamp: <2006-11-01 11:56:06 Rod Stephenson Transcritical Pty Ltd>

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
"""


dllMainhFile = r"""//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//   Time-stamp: <2007-05-28 03:23:21 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#ifndef  __${udllGroupName}_H
#define  __${udllGroupName}_H

#ifdef __${udllGroupName}_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif

DllImportExport void ForceLoad_TTDynTest();

//===========================================================================

//.....

//---------------------------------------------------------------------------
#undef DllImportExport

#endif // #ifndef __${udllGroupName}_H
"""


dllMaincppFile = r"""//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//   Time-stamp: <2007-05-28 03:22:28 Rod Stephenson Transcritical Pty Ltd>
//===========================================================================

#include "stdafx.h"
#define __${udllGroupName}_CPP
#include "${udllGroupName}.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#include "scdmacros.h" 
#include "md_headers.h"

#pragma LIBCOMMENT("..\\..\\Bin\\", "\\SMDK1" )
#pragma LIBCOMMENT("..\\..\\Bin\\", "\\scdlib" )

//===========================================================================

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

//===========================================================================

void ForceLoad_${udllGroupName}()
  {
  // Dummy Function to allow other libraries to force load this one
  }

//===========================================================================

extern "C" __declspec(dllexport) BOOL IsSMDKLibDLL()
  {
  return TRUE;
  }


//===========================================================================
"""


##
## Unit Model Header File
##

umhFile = """\
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//    QAL Classifer Model. Transcritical Technologies Pty Ltd copyright 2004
//   Time-stamp: <2007-05-25 03:12:19 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
//===========================================================================

#ifndef  __${udllGroupName}UM_H
#define  __${udllGroupName}UM_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

#ifdef __${udllGroupName}UM_CPP
  #define DllImportExport __declspec(dllexport)
#elif !defined(SMDKDemoU)
  #define DllImportExport __declspec(dllimport)
#else
  #define DllImportExport
#endif


//---------------------------------------------------------------------------


class ${className} : public MBaseMethod
{
 public:


  ${className}(MUnitDefBase *pUnitDef,TaggedObject * pNd);
  virtual ~${className}(void);

  void Init();
  void BuildDataFields();
  bool ValidateDataFields();
  void EvalProducts();
  //void ClosureInfo(MClosureInfo & CI);

 protected:
  // Streams 

  MStream Feed;

  // Booleans
  bool bOnline;
  // Doubles


  double dTankVol;
  double dQvin;
  double dTout;
};

#endif __${udllGroupName}UM_H
"""


##
##  Unit Model Source File
##

umcppFile = """\
//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
//           QAL Classifier Model 2004 - Transcritical Technologies/ QAL 
//   Time-stamp: <2007-05-28 03:21:31 Rod Stephenson Transcritical Pty Ltd>
// Copyright (C) 2005 by Transcritical Technologies Pty Ltd and KWA
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __${usrcName}_CPP
#include "${srcName}.h"


static MInitialiseTest InitTest("DynTest");


static MSpeciePtr  spAlumina       (InitTest, "NaAl[OH]4(l)", false);

enum {idFeed, idUflow, idOflow};


static MInOutDefStruct s_IODefs[]=
  {
    { "Feed",      "FD",   idFeed,  1, 10,  0,  1.0f,   MIO_In |MIO_Material },
    { "Underflow", "UF",   idUflow,  1,  1,  0,  1.0f,  MIO_Out|MIO_Material },
    { "Overflow",  "OF",   idOflow, 0,  1,  1,  1.0f,   MIO_Out|MIO_Material },
    
    
    { NULL },
  };

double Drw_${className}[] = 
  { 
    MDrw_Poly, -5,10, -5,-10, 5,-10, 5,10,
    MDrw_End 
  };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(${className}, "${unitName}", DLL_GroupName)

void ${className}_UnitDef::GetOptions()
{
  SetDefaultTag("DPSD");
  SetDrawing("Tank", Drw_${className});
  SetTreeDescription("TT1:DynamicPSD");
  SetDescription("TODO: Dynamic PSD Model");
  SetModelSolveMode(MSolveMode_All);
  SetModelGroup(MGroup_PSD);
  SetModelLicense(MLicense_BlackBox);
};

//---------------------------------------------------------------------------


${className}::${className}(MUnitDefBase *pUnitDef, TaggedObject * pNd) :  
  MBaseMethod(pUnitDef, pNd)
{   //default values...

}


//---------------------------------------------------------------------------

${className}::~${className}()
{   // Destructor

}


bool ${className}::ValidateDataFields()
  {//ensure parameters are within expected ranges
  return true;
  }

//---------------------------------------------------------------------------

void ${className}::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------



void ${className}::EvalProducts()
  {
  try {
    FlwIOs.AddMixtureIn_Id(Feed, idFeed);
    MStream & UFlow = FlwIOs[FlwIOs.First[idUflow]].Stream; //Reference to the output stream
    MStream & OFlow = FlwIOs[FlwIOs.First[idOflow]].Stream; //Reference to the output stream
    
    UFlow = Feed;
    OFlow = Feed;
    OFlow.SetF(Feed, MP_All, 0.0);
    UFlow.SetF(Feed, MP_All, 1.0);

    bool streamOK=true;
    
    if (!bOnline) return;

    // Check for existence of Bayer Property... 
    MIBayer & FeedB=Feed.IF<MIBayer>(false);   // Do the checks up front

    Log.SetCondition(IsNothing(FeedB), 1, MMsg_Warning, "Bad Feed Stream - Not BATC Bayer Model");


    // Try PSD
    MIPSD & FeedPSD = Feed.IF<MIPSD>(false);
    


    if (IsNothing(FeedB) || IsNothing(FeedPSD)) streamOK = false;


    if (bOnline && streamOK) {  // Online and have Bayer and SSA properties...
    

      
      UFlow = Feed;

    } else  {   // Just tidy up and put some sensible stuff in the results...
       Log.Message(MMsg_Warning, "Stream Problem...");
    }
  }
  catch (MMdlException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MFPPException &e)
    {
    e.ClearFPP();
    Log.Message(MMsg_Error, e.Description);
    }
  catch (MSysException &e)
    {
    Log.Message(MMsg_Error, e.Description);
    }
  catch (...)
    {
      Log.Message(MMsg_Error, "Some Unknown Exception occured");
    }
  }

//--------------------------------------------------------------------------



void ${className}::BuildDataFields()
{

  DD.Text  ("");

#ifdef TTDEBUG
  DD.CheckBox("TTDBG", "",  &bTTDebug, MF_PARAMETER);
#endif

  DD.CheckBox("On", "",  &bOnline, MF_PARAMETER|MF_SET_ON_CHANGE);
  DD.Text("");
  DD.Text  ("Requirements");
  DD.Double("TankVol", "",	        &dTankVol         ,MF_PARAMETER, MC_Vol("m^3"));

    
  DD.Text  ("Results");         
  DD.Show();                    
  DD.Double("Vol.Flow.In", "",       &dQvin          ,MF_RESULT, MC_Qv("L/s"));
  DD.Double("TempOut", "",          &dTout          ,MF_RESULT, MC_T("C"));
  DD.Text("");
  
}
"""

