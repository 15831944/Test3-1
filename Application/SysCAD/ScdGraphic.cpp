// ScdGraphic.cpp : Implementation of CScdGraphic
#include "stdafx.h"
#include "SysCAD_i.h"
#include "ScdGraphic.h"

#include "SysCAD.h"
#include "grfdoc.h"
#include ".\scdgraphic.h"
//#include "..\..\..\com\scdslv\scdslvwrap.h"

/////////////////////////////////////////////////////////////////////////////
// CScdGraphic

static int CheckTagExists(LPCTSTR Tag)
  {
  if (gs_pSfeSrvr)
    {
    if (!(gs_pSfeSrvr->FE_TestModelTagUnique((char*)Tag)))
      return 0;
    else
      return -1;
    }
  return -2;
  }

STDMETHODIMP CScdGraphic::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IScdGraphic
	};
	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

void CScdGraphic::FireTheEvent(long Evt, long Data)
  {
  switch (Evt)
    {
    case ComCmd_NULL : /*Fire_On...*/ ; break;
    };
  };

CDocument* GetGrfDoc(int index, LPCTSTR name)
  {
  bool Done=false;
  CDocument* pDoc=NULL;
  int GrfDocCnt=-1;
  CDocTemplate &Templ = ScdApp()->GraphTemplate();
  POSITION PosD = Templ.GetFirstDocPosition();
  while (!Done && PosD)
    {
    pDoc = Templ.GetNextDoc(PosD);
    POSITION pos = pDoc->GetFirstViewPosition();
    if (pos)
      {
      CView* pFirstView = pDoc->GetNextView(pos);
      CWnd* w = pFirstView->GetParent();
//      if (pDoc->IsKindOf(RUNTIME_CLASS(CGrfDoc)))
      if (dynamic_cast<CGrfDoc*>(pDoc))
        {
        //ASSERT(dynamic_cast<CGrfFrameWnd*>(w));
        GrfDocCnt++;
        if (index<0 && pDoc->GetTitle().CompareNoCase(name)==0)
          Done = true;
        else if (GrfDocCnt==index)
          Done = true;
        }
      }
    }
  if (!Done)
    return NULL;
  return pDoc;
  }


STDMETHODIMP CScdGraphic::get_Name(BSTR *pVal)
	{
	//USES_CONVERSION;
  appSCD_COMENTRYGET(long, pVal)
    {
    _bstr_t t(m_sName);
    *pVal=t.copy();
    }		
	SCD_COMEXIT
	}

STDMETHODIMP CScdGraphic::put_Name(BSTR newVal)
	{
  appSCD_COMENTRY(long)
    {
    m_sName=(LPCTSTR)_bstr_t(newVal);
    }		
	SCD_COMEXIT
	}

STDMETHODIMP CScdGraphic::AddUnit(BSTR bsTag, double X, double Y, double Z, BSTR bsSymbol, double XScale, double YScale, double Rotation)
  {
  appSCD_COMENTRY(eScdGraphicCodes)
    {
    SCD_LclStr(Tag, bsTag);
    SCD_LclStr(Symbol, bsSymbol);

    bool DataErr = (CheckTagExists(Tag)!=0);
    if (DataErr)
      return Scd.Return(eScdGraphicCode_TagNotFound, "Tag not found : AddUnit '%s' failed!", Tag);

    CDocument* pDoc=NULL;
    if (!DataErr)
      {
      pDoc=GetGrfDoc(-1, m_sName);
      DataErr = (pDoc==NULL);
      }

    if (!DataErr)
      {
      RequestModelInfoRec MInfo;
      Strng ClassId;
      flag b = gs_pPrj->RequestModelClassId((char*)Tag, ClassId);
      ASSERT_RDB(b, "Class for tag should be found", __FILE__, __LINE__);
      b = gs_pPrj->RequestModelInfoByClassId(ClassId(), MInfo);
      ASSERT_RDB(b, "ModelInfo for Class should be found", __FILE__, __LINE__);

      CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
      CInsertBlk CB; 

      CB.ATag=Tag;
      CB.AClass=ClassId();
      CB.Pt.World.X=X;
      CB.Pt.World.Y=Y;
      CB.Pt.World.Z=Z;
      CB.NdScl.X=XScale;
      CB.NdScl.Y=YScale;
      CB.NdScl.Z=YScale;
      CB.Rotate=(float)Rotation;
      CB.ATagBase=MInfo.TagInitialID();
      if (Symbol && strchr(Symbol, ':'))
        CB.ASymbol=Symbol;
      else
        {
        CB.ASymbol=MInfo.DrwGroup();
        CB.ASymbol+=':';
        CB.ASymbol+=Symbol;
        }
      //CB.ASymbol+=".dxf";

      pGDoc->GCB.DoInsertNodeGrf(&CB);
      }

    if (DataErr)
      return Scd.Return(eScdGraphicCode_GrfNotCreated, "AddUnit '%s' failed!", Tag);
    return S_OK;
    }
	SCD_COMEXIT
  }
   
STDMETHODIMP CScdGraphic::Zoom(eScdZooms zoom)
  {
  appSCD_COMENTRY(eScdGraphicCodes)
    {
    CDocument* pDoc=GetGrfDoc(-1, m_sName);
    if (pDoc)
      {
      CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
      pGDoc->GCB.pDsp->Open();
      switch (zoom)
        {
        case eScdZoom_Full: 
          pGDoc->GCB.pDsp->SetZoom(Zoom_All); 
          break;
        }
      pGDoc->GCB.pDrw->EntityInvalidate();
      pGDoc->GCB.pWnd->Invalidate(True);
      pGDoc->GCB.pDsp->Close();
      }
    }
	SCD_COMEXIT
  }



STDMETHODIMP CScdGraphic::AddLinkA(BSTR bsTag, VARIANT Points3D)
  {
  appSCD_COMENTRY(long)
    {
    SCD_LclStr(Tag, bsTag);
    if (!((Points3D.vt&VT_R8) && (Points3D.vt&VT_ARRAY)))
      Scd.ReturnH(DISP_E_TYPEMISMATCH);
    else
      {    
      SAFEARRAY *pSA=Points3D.parray;
      //SAFEARRAY *pSA=pnewVal;

      long Cnt=pSA->rgsabound->cElements;
      long s=pSA->rgsabound->lLbound;
    
      if (pSA->cDims!=1)
        return Scd.ReturnH(E_INVALIDARG, "Points Must be 1 Dimensional");
      if (Cnt%3!=0)
        return Scd.ReturnH(E_INVALIDARG, "Points Must be in sets of 3(X,Y,Z)");

      //for (long i=0; i<Cnt; i++)
      //  m_Mdl.SetMass(i, pDble[s++]);

      bool DataErr = (CheckTagExists(Tag)!=0);
      if (DataErr)
        return Scd.Return(eScdGraphicCode_TagNotFound, "Tag not found : AddLink '%s' failed!", Tag);
      CDocument* pDoc=GetGrfDoc(-1, m_sName);

      if (pDoc)
        {
        RequestModelInfoRec MInfo;
        Strng ClassId;
        flag b = gs_pPrj->RequestModelClassId((char*)Tag, ClassId);
        ASSERT_RDB(b, "Class for tag should be found", __FILE__, __LINE__);
        b = gs_pPrj->RequestModelInfoByClassId(ClassId(), MInfo);
        ASSERT_RDB(b, "ModelInfo for Class should be found", __FILE__, __LINE__);
        RequestConnModelInfoRec MCInfo;
        b = gs_pPrj->RequestModelConnInfo((char*)Tag, MCInfo);
        ASSERT_RDB(b, "ModelConnInfo for Class should be found", __FILE__, __LINE__);

        CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
        CConnectBlk CB; 

        CB.ATag=Tag;
        CB.AClass=ClassId();
        CB.SrcTag=MCInfo.SrcTag();
        //CB.SrcIO=SrcIO;
        CB.DstTag=MCInfo.DstTag();
        //CB.DstIO=DstIO;
        CB.iLineMethod=0;
        CB.NVerts=Cnt/3;

        double HUGEP *pDble;
        // Get a pointer to the elements of the array.
        SCD_ROF(SafeArrayAccessData(pSA, (void HUGEP**)&pDble));

        for (int ii=0, i=0; ii<Cnt; ii+=3, i++)
          {
          CB.Verts[i].World.X=pDble[ii];
          CB.Verts[i].World.Y=pDble[ii+1];
          CB.Verts[i].World.Z=pDble[ii+2];
          }

        SCD_ROF(SafeArrayUnaccessData(pSA));
#pragma chCHECKIT(NBNBNB Why is this not called as per example COM&ATL3.0 pg465)
        // SafeArrayDestroy(pSA);

        CB.ATagBase=MInfo.TagInitialID();
        CB.ASymbol=MInfo.DrwGroup();
        CB.ASymbol+=":default";
  
        pGDoc->GCB.DoInsertLinkGrf(&CB);
        }
      else
        {
        Scd.Return(eScdGraphicCode_GrfNotCreated, "AddLinkA '%s' failed!", Tag);
        }
      }
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdGraphic::AddLinkM(BSTR bsTag, eScdLinkDrawMethods Method)
  {
  appSCD_COMENTRY(long)
    {
    SCD_LclStr(Tag, bsTag);

    bool DataErr = (CheckTagExists(Tag)!=0);
    if (DataErr)
      return Scd.Return(eScdGraphicCode_TagNotFound, "Tag not found : AddLink '%s' failed!", Tag);
    CDocument* pDoc=GetGrfDoc(-1, m_sName);

    if (pDoc)
      {
      RequestModelInfoRec MInfo;
      Strng ClassId;
      flag b = gs_pPrj->RequestModelClassId((char*)Tag, ClassId);
      ASSERT_RDB(b, "Class for tag should be found", __FILE__, __LINE__);
      b = gs_pPrj->RequestModelInfoByClassId(ClassId(), MInfo);
      ASSERT_RDB(b, "ModelInfo for Class should be found", __FILE__, __LINE__);
      RequestConnModelInfoRec MCInfo;
      b = gs_pPrj->RequestModelConnInfo((char*)Tag, MCInfo);
      ASSERT_RDB(b, "ModelConnInfo for Class should be found", __FILE__, __LINE__);

      CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
      CConnectBlk CB; 

      CB.ATag=Tag;
      CB.AClass=ClassId();
      CB.SrcTag=MCInfo.SrcTag();
      //CB.SrcIO=SrcIO;
      CB.DstTag=MCInfo.DstTag();
      //CB.DstIO=DstIO;
      CB.iLineMethod=Method;
      CB.NVerts=-1;

      CB.ATagBase=MInfo.TagInitialID();
      CB.ASymbol=MInfo.DrwGroup();
      CB.ASymbol+=".default";
    
      pGDoc->GCB.DoInsertLinkGrf(&CB);
      }
    else
      {
      Scd.Return(eScdGraphicCode_GrfNotCreated, "AddLinkM '%s' failed!", Tag);
      }
    }
  SCD_COMEXIT
  }

STDMETHODIMP CScdGraphic::InsertGroup(BSTR bsLibrary, BSTR bsGroup, DOUBLE X, DOUBLE Y, DOUBLE Z, DOUBLE XScale, DOUBLE YScale, DOUBLE ZScale, IScdTagFixup * TagFixups, eScdNDBOptions Options)
  {
  appSCD_COMENTRY(eScdGraphicCodes)
    {
    SCD_LclStr(Library, bsLibrary);
    SCD_LclStr(Group, bsGroup);

    bool DataErr = false;//(CheckTagExists(Tag)!=0);
    //if (DataErr)
    //  return Scd.Return(eScdGraphicCode_TagNotFound, "Tag not found : AddUnit '%s' failed!", Tag);

    CDocument* pDoc=NULL;
    if (!DataErr)
      {
      pDoc=GetGrfDoc(-1, m_sName);
      DataErr = (pDoc==NULL);
      }

    if (!DataErr)
      {
      CGrfDoc * pGDoc=(CGrfDoc*)pDoc;
      CInsertBlk CB; 

      CB.m_sGroupLibrary=Library;
      CB.m_sGroupName=Group;
      CB.Pt.World.X=X;
      CB.Pt.World.Y=Y;
      CB.Pt.World.Z=Z;
      CB.GrpScl.X=XScale;
      CB.GrpScl.Y=YScale;
      CB.GrpScl.Z=YScale;

      _bstr_t String1, String2;
      DataErr=FAILED(TagFixups->get_Rule((eScdTagFixupRules*)&CB.m_iFixupRule));
      DataErr=DataErr||FAILED(TagFixups->get_String1(String1.GetAddress()));
      DataErr=DataErr||FAILED(TagFixups->get_String2(String2.GetAddress()));
      CB.m_sString1=(LPCTSTR)String1;
      CB.m_sString2=(LPCTSTR)String2;


      pGDoc->GCB.DoInsertGroup(&CB);
      }

    if (DataErr)
      return Scd.Return(eScdGraphicCode_GrfNotCreated, "InsertGroup %s:%s failed!", Library, Group);
    }
	SCD_COMEXIT
  }
