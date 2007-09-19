// ScdProject.cpp : Implementation of CScdProject
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdProject.h"
#include "scdcomcmds.h"
#include "licbase.h"
#include "scdproject.h"
#include "neutralmdl.h"
#include "cmd_mngr.h"

#include "project.h"
#include "tagvdoc.h"

/////////////////////////////////////////////////////////////////////////////
// CScdProject

HRESULT CScdProject::FinalConstruct()
  {
  FC_TESTLICENSE(gs_License.AllowCOMApp());
  FC_ROF(m_pGraphics  .CoCreateInstance(CLSID_ScdGraphics ));
  FC_ROF(m_pTrends    .CoCreateInstance(CLSID_ScdTrends   ));
  FC_ROF(m_pHistorian .CoCreateInstance(CLSID_ScdHistorian));
  FC_ROF(m_pAppTags   .CoCreateInstance(CLSID_ScdAppTags  ));
  FC_ROF(m_pSolver    .CoCreateInstance(CLSID_ScdSolver   ));
  FC_ROF(m_pReports   .CoCreateInstance(CLSID_ScdReports  ));
  //FC_ROF(m_pSnapshot  .CoCreateInstance(CLSID_ScdSnapshot ));
  FC_ROF(m_pReplay    .CoCreateInstance(CLSID_ScdReplay   ));
  FC_ROF(m_pOPCServer .CoCreateInstance(CLSID_ScdOPCServer));
  FC_ROF(m_pIOMarshal .CoCreateInstance(CLSID_ScdIOMarshal));
  FC_ROF(m_pDDEServer .CoCreateInstance(CLSID_ScdDDEServer));

  AddChildEvents(m_pGraphics.p );
  AddChildEvents(m_pTrends.p   );
  AddChildEvents(m_pHistorian.p);
  AddChildEvents(m_pAppTags.p  );
  AddChildEvents(m_pSolver.p   );
  AddChildEvents(m_pReports.p  );
  //AddChildEvents(m_pSnapshot.p );
  AddChildEvents(m_pReplay.p   );
  AddChildEvents(m_pOPCServer.p);
  AddChildEvents(m_pIOMarshal.p);
  AddChildEvents(m_pDDEServer.p);

  return S_OK; //CoCreateFreeThreadedMarshaler(GetControllingUnknown(), &m_pUnkMarshaler.p);
  }

void CScdProject::FinalRelease()
  {
  m_pGraphics  .Release();
  m_pTrends    .Release();
  m_pHistorian .Release();
  m_pAppTags   .Release();
  m_pSolver    .Release();
  m_pReports   .Release();
  //m_pSnapshot  .Release();
  m_pReplay    .Release();
  m_pOPCServer .Release();
  m_pIOMarshal .Release();
  m_pDDEServer .Release();
  //m_pUnkMarshaler.Release();
  }

STDMETHODIMP CScdProject::InterfaceSupportsErrorInfo(REFIID riid)
  {
  static const IID* arr[] =
    {
    &IID_IScdProject
    };
  for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
    {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
    }
  return S_FALSE;
  }

void CScdProject::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_SaveProject:
    case ComCmd_SaveProjectNV:
      //Fire_OnProjectSaved(l);
      break;
    case ComCmd_ExportNeutralDB:
    case ComCmd_ImportNeutralDB:
      //Fire_OnProjectSaved(l);
      break;
    };
  };

STDMETHODIMP CScdProject::get_Graphics(IScdGraphics **pGraphics)
  {
  appSCD_COMENTRYGET(long, pGraphics)
    {
    Scd.ReturnH(m_pGraphics.CopyTo(pGraphics));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdProject::get_Trends(IScdTrends **pTrends)
  {
  appSCD_COMENTRYGET(long, pTrends)
    {
    Scd.ReturnH(m_pTrends.CopyTo(pTrends));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdProject::get_Historian(IScdHistorian **pHistorian)
  {
  appSCD_COMENTRYGET(long, pHistorian)
    {
    Scd.ReturnH(m_pHistorian.CopyTo(pHistorian));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdProject::get_Reports(IScdReports **pReports)
  {
  appSCD_COMENTRYGET(long, pReports)
    {
    Scd.ReturnH(m_pReports.CopyTo(pReports));
    }
  SCD_COMEXIT
  }

//STDMETHODIMP CScdProject::get_Snapshot(IScdSnapshot **pSnapshot)
//  {
//  appSCD_COMENTRYGET(long, pSnapshot)
//    {
//    Scd.ReturnH(m_pSnapshot.CopyTo(pSnapshot));
//    }
//  SCD_COMEXIT
//  }

STDMETHODIMP CScdProject::get_Solver(IScdSolver **pSolver)
  {
  appSCD_COMENTRYGET(long, pSolver)
    {
    Scd.ReturnH(m_pSolver.CopyTo(pSolver));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdProject::get_SpecieDefns(IScdSpecieDefns **pSpecieDefns)
  {
  appSCD_COMENTRYGET(long, pSpecieDefns)
    {
    Scd.ReturnH(m_pSolver->get_SpecieDefns(pSpecieDefns));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdProject::get_Tags(IScdAppTags **pTags)
  {
  appSCD_COMENTRYGET(long, pTags)
    {
    Scd.ReturnH(m_pAppTags.CopyTo(pTags));
    }
  SCD_COMEXIT
  }


STDMETHODIMP CScdProject::Save(BSTR bsFilename, VARIANT_BOOL NewVersion)
  {
  appSCD_COMENTRY(long)
    {
    ScdApp()->m_HoldAddToRecentFileList++;

    SCD_LclStr(Filename, bsFilename);
    LPTSTR S=new char[strlen(Filename)+1];
    strcpy(S, Filename);
    switch(PostComCmd(NewVersion ? ComCmd_SaveProjectNV:ComCmd_SaveProject, (long)S))
      {
      case CScdCOCmdBase::SS_CallTimeout:
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:
      case CScdCOCmdBase::SS_CallASync:
        break;
      }
    ScdApp()->m_HoldAddToRecentFileList--;
    //    if (Scd.HResultOK())
    //      Scd.ReturnH(m_pProject.CopyTo(pProject));
    }
  SCD_COMEXIT
  }


STDMETHODIMP CScdProject::get_OPCServer(IScdOPCServer ** pOPCServer)
  {
  appSCD_COMENTRYGET(long, pOPCServer)
    {
    Scd.ReturnH(m_pOPCServer.CopyTo(pOPCServer));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdProject::get_IOMarshal(IScdIOMarshal ** pIOMarshal)
  {
  appSCD_COMENTRYGET(long, pIOMarshal)
    {
    Scd.ReturnH(m_pIOMarshal.CopyTo(pIOMarshal));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdProject::get_DDEServer(IScdDDEServer** pDDEServer)
  {
  appSCD_COMENTRYGET(long, pDDEServer)
    {
    Scd.ReturnH(m_pDDEServer.CopyTo(pDDEServer));
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdProject::get_CfgFolder(BSTR* pVal)
  {
  appSCD_COMENTRYGET(long, pVal)
    {
    SCD_Copy2BSTR(pVal, CfgFiles());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdProject::get_PrjFolder(BSTR* pVal)
  {
  appSCD_COMENTRYGET(long, pVal)
    {
    SCD_Copy2BSTR(pVal, PrjFiles());
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdProject::ExportNeutralDB(eScdNDBOptions Options, BSTR bsGraphicsDatabase, BSTR bsModelDatabase)
  {
  appSCD_COMENTRY(long)
    {
    SCD_LclStr(GraphicsDatabase, bsGraphicsDatabase);
    SCD_LclStr(ModelDatabase, bsModelDatabase);
    CScdCmdImportExport Cmd;
    Cmd.m_iOptions=Options;
    Cmd.m_sGraphicsDatabase=GraphicsDatabase;
    Cmd.m_sModelDatabase=ModelDatabase;

    switch(PostComCmd(ComCmd_ExportNeutralDB, (long)&Cmd))
      {
      case CScdCOCmdBase::SS_CallTimeout:
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:
      case CScdCOCmdBase::SS_CallASync:
        break;
      }
    //    if (Scd.HResultOK())
    //      Scd.ReturnH(m_pProject.CopyTo(pProject));
    }
  SCD_COMEXIT

  //appSCD_COMENTRY(eScdGraphicCodes)
  //  {
  //  SCD_LclStr(GraphicsDatabase, bsGraphicsDatabase);
  //  SCD_LclStr(ModelDatabase, bsModelDatabase);

  //  bool Failed = false;//(CheckTagExists(Tag)!=0);

  //  //eScdTagFixupRules iFixupRule;
  //  //Failed=FAILED(TagFixups->get_Rule(&iFixupRule));

  //  //_bstr_t Prefix, Suffix;
  //  //Failed=Failed||FAILED(TagFixups->get_Prefix(Prefix.GetAddress()));
  //  //Failed=Failed||FAILED(TagFixups->get_Suffix(Suffix.GetAddress()));

  //  CNeutralImportExport NExport;
  //  //NExport.SetImportTagFixups(iFixupRule, Prefix, Suffix, NULL, NULL);
  //  if (!Failed)
  //    Failed=!NExport.DoExport(Options, GraphicsDatabase, ModelDatabase);

  //  if (Failed)
  //    return Scd.Return(eScdGraphicCode_GrfNotCreated, "Neutral Database import %s:%s failed!", GraphicsDatabase, ModelDatabase);
  //  }
  //SCD_COMEXIT
  }

STDMETHODIMP CScdProject::ImportNeutralDB(eScdNDBOptions Options, BSTR bsGraphicsDatabase, BSTR bsModelDatabase, IScdTagFixup * TagFixups)
  {
  appSCD_COMENTRY(long)
    {
    SCD_LclStr(GraphicsDatabase, bsGraphicsDatabase);
    SCD_LclStr(ModelDatabase, bsModelDatabase);
    CScdCmdImportExport Cmd;
    Cmd.m_iOptions=Options;
    Cmd.m_sGraphicsDatabase=GraphicsDatabase;
    Cmd.m_sModelDatabase=ModelDatabase;

    bool Failed=false;
    if (TagFixups)
      {
      eScdTagFixupRules iFixupRule;
      _bstr_t String1, String2;
      Failed=FAILED(TagFixups->get_Rule(&iFixupRule));
      Failed=Failed||FAILED(TagFixups->get_String1(String1.GetAddress()));
      Failed=Failed||FAILED(TagFixups->get_String2(String2.GetAddress()));
      
      Cmd.m_iRule=iFixupRule;
      Cmd.m_sPrefix=(LPCTSTR)String1;
      Cmd.m_sSuffix=(LPCTSTR)String2;
      }

    if (Failed)
      return Scd.Return(eScdGraphicCode_GrfNotCreated, "Neutral Database import %s:%s failed!", GraphicsDatabase, ModelDatabase);

    switch(PostComCmd(ComCmd_ImportNeutralDB, (long)&Cmd))
      {
      case CScdCOCmdBase::SS_CallTimeout:
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:
      case CScdCOCmdBase::SS_CallASync:
        break;
      }
    //    if (Scd.HResultOK())
    //      Scd.ReturnH(m_pProject.CopyTo(pProject));
    }
  SCD_COMEXIT
  }


STDMETHODIMP CScdProject::SaveSnapshot(BSTR bsFileName, long SeqStart)
  {
  appSCD_COMENTRY(long)
    {
    SCD_LclStr(FileName, bsFileName);
    CScdCmdSnapshot Cmd;
    
    if (strlen(FileName)>0)
      {
      m_iSeqNo = SeqStart;
      if (m_iSeqNo>=0)
        m_sSnapName = FileName;
      else
        Cmd.m_sFileName = FileName;
      }
    if (m_iSeqNo>=0)
      {
      if (m_sSnapName.GetLength()==0)
        m_sSnapName="Snap";

      Cmd.m_sFileName.Set("%s%03i", m_sSnapName, m_iSeqNo++);
      }
  
    bool Failed=false;

    if (Failed)
      return Scd.Return(eScdGraphicCode_GrfNotCreated, "SaveSnapshot %s:%s failed!", Cmd.m_sFileName());

    switch(PostComCmd(ComCmd_SaveSnapshot, (long)&Cmd))
      {
      case CScdCOCmdBase::SS_CallTimeout:
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:
      case CScdCOCmdBase::SS_CallASync:
        break;
      }
    //    if (Scd.HResultOK())
    //      Scd.ReturnH(m_pProject.CopyTo(pProject));
    }
  SCD_COMEXIT
  };

STDMETHODIMP CScdProject::LoadSnapshot(BSTR bsFileName, long NoInSeq)
  {
  appSCD_COMENTRY(long)
    {
    SCD_LclStr(FileName, bsFileName);
    CScdCmdSnapshot Cmd;

    if (strlen(FileName)==0)
      Cmd.m_sFileName="Snap";
    else
      Cmd.m_sFileName=FileName;

    if (NoInSeq>=0)
      Cmd.m_sFileName.Set("%s%03i", FileName, NoInSeq);

    bool Failed=false;

    if (Failed)
      return Scd.Return(eScdGraphicCode_GrfNotCreated, "LoadSnapshot %s:%s failed!", Cmd.m_sFileName());

    switch(PostComCmd(ComCmd_LoadSnapshot, (long)&Cmd))
      {
      case CScdCOCmdBase::SS_CallTimeout:
        Scd.Return(HRESULT_ERR(2), "Async Call Timeout");
        break;
      case CScdCOCmdBase::SS_CallReturned:
      case CScdCOCmdBase::SS_CallASync:
        break;
      }
    //    if (Scd.HResultOK())
    //      Scd.ReturnH(m_pProject.CopyTo(pProject));
    }
  SCD_COMEXIT
  };
