//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "syscad.h"
#include "resource.h"
#include "PrjDCmp.h"
#include "tagdb.h"
#include "dlgbusy.h"
#include "dbhelper.h"

//#include "optoff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------
// CPrjDataCompare dialog

CPrjDataCompare::CPrjDataCompare(CWnd* pParent /*=NULL*/)
	: CDialog(CPrjDataCompare::IDD, pParent)
  {
	//{{AFX_DATA_INIT(CPrjDataCompare)
	m_Prj1 = _T("");
	m_Prj2 = _T("");
	m_Tolerance = 0.1;
	m_Report1 = _T("");
	m_FirstInClass = TRUE;
	m_MissingTags = TRUE;
	m_MissingValues = TRUE;
	m_ValuesDifferent = FALSE;
	//}}AFX_DATA_INIT

	m_Prj1 = ScdPFUser.RdStr("ProjectCompare", "Project1", StartupDirectory());
	m_Prj2 = ScdPFUser.RdStr("ProjectCompare", "Project2", StartupDirectory());
  m_Tolerance=ScdPFUser.RdDouble("ProjectCompare", "Tolerance", m_Tolerance);
  Strng S=TemporaryFiles();
  S+="PrjDiffs.csv";
  m_Report1=ScdPFUser.RdStr("ProjectCompare", "ReportFile", S());
  m_FirstInClass=ScdPFUser.RdInt("ProjectCompare", "FirstInClass", m_FirstInClass);
	m_MissingTags=ScdPFUser.RdInt("ProjectCompare", "MissingTags", m_MissingTags);
	m_MissingValues=ScdPFUser.RdInt("ProjectCompare", "MissingValues", m_MissingValues);
	m_ValuesDifferent=ScdPFUser.RdInt("ProjectCompare", "ValuesDifferent", m_ValuesDifferent);

  hRpt=NULL;
  }

//------------------------------------------------------------------------

void CPrjDataCompare::DoDataExchange(CDataExchange* pDX)
  {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrjDataCompare)
	DDX_Text(pDX, IDC_PRJ1, m_Prj1);
	DDX_Text(pDX, IDC_PRJ2, m_Prj2);
	DDX_Text(pDX, IDC_TOLERANCE, m_Tolerance);
	DDX_Text(pDX, IDC_REPORT1, m_Report1);
	DDX_Check(pDX, IDC_FIRSTINCLASS, m_FirstInClass);
	DDX_Check(pDX, IDC_MISSINGTAGS, m_MissingTags);
	DDX_Check(pDX, IDC_MISSINGVALUES, m_MissingValues);
	DDX_Check(pDX, IDC_VALUESDIFF, m_ValuesDifferent);
	//}}AFX_DATA_MAP
  }

//------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CPrjDataCompare, CDialog)
	//{{AFX_MSG_MAP(CPrjDataCompare)
	ON_BN_CLICKED(ID_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(ID_BROWSE2, OnBrowse2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------

void CPrjDataCompare::OnOK() 
  {

	CDialog::OnOK();

  ScdPFUser.WrStr("ProjectCompare", "Project1", m_Prj1);
  ScdPFUser.WrStr("ProjectCompare", "Project2", m_Prj2);
  ScdPFUser.WrDouble("ProjectCompare", "Tolerance", m_Tolerance);
  ScdPFUser.WrStr("ProjectCompare", "ReportFile", m_Report1);
  ScdPFUser.WrInt("ProjectCompare", "FirstInClass", m_FirstInClass);
	ScdPFUser.WrInt("ProjectCompare", "MissingTags", m_MissingTags);
	ScdPFUser.WrInt("ProjectCompare", "MissingValues", m_MissingValues);
	ScdPFUser.WrInt("ProjectCompare", "ValuesDifferent", m_ValuesDifferent);
  }

//------------------------------------------------------------------------

void CPrjDataCompare::DoBrowse(CString & Prj) 
  {
  Strng X;
  X.Set("(*.MDB)|*.MDB|SysCAD Databases (*.MDB)|*.MDB||");

  Strng Init;
  Init.FnDrivePath((char*)(LPCTSTR)Prj);

  CSCDFileDialog Dlg(true, "*.MDB", "", OFN_HIDEREADONLY | OFN_SHAREAWARE, X());
  Dlg.m_ofn.lpstrInitialDir = Init();
  Dlg.m_ofn.lpstrTitle = "Browse";

  if (Dlg.DoModal()==IDOK)
    {
    Prj=Dlg.GetPathName();
    }
  }

//------------------------------------------------------------------------

void CPrjDataCompare::OnBrowse1() 
  {
  DoBrowse(m_Prj1);	
  UpdateData(FALSE);
  }

//------------------------------------------------------------------------

void CPrjDataCompare::OnBrowse2() 
  {
  DoBrowse(m_Prj2);	
  UpdateData(FALSE);
	
  }

//------------------------------------------------------------------------

flag CPrjDataCompare::DoCompare() 
  {
  CDlgBusy::Open("", true);
  CWaitCursor Wait;

  CTagDBase TagDB[2];
  int db, db1, DBFmt[2];
  
  Strng Prj[2];
  Prj[0].FnDrivePath((char*)(LPCTSTR)m_Prj1);
  Prj[1].FnDrivePath((char*)(LPCTSTR)m_Prj2);

  ClsTagMap.InitHashTable(FindNextPrimeNumber(4096));

  NdMaps[0].InitHashTable(FindNextPrimeNumber(4096));
  NdMaps[1].InitHashTable(FindNextPrimeNumber(4096));

  CDlgBusy::SetLine(1, "Open : %s", Prj[0]());
  DBFmt[0]=TagDB[0].Open(DBConnect_Find, DBO_ForRead, Prj[0](), NULL, NULL);
  if (DBFmt[0]==0)
    {
    LogError("Compare Project", LF_Exclamation, "Cannot Open Database %s", (LPCTSTR)m_Prj1);
    goto Close0;
    }

  CDlgBusy::SetLine(2, "Open : %s", Prj[1]());
  DBFmt[1]=TagDB[1].Open(DBConnect_Find, DBO_ForRead, Prj[1](), NULL, NULL);
  if (DBFmt[1]==0)
    {
    LogError("Compare Project", LF_Exclamation, "Cannot Open Database %s", (LPCTSTR)m_Prj2);
    goto Close1;
    }

  hRpt=fopen((LPCTSTR)m_Report1, "wt");
  if (hRpt==NULL)
    {
    LogError("Compare Project", LF_Exclamation, "Cannot Open Report %s", (LPCTSTR)m_Report1);
    goto Close2;
    }
  else
    {
    fprintf(hRpt, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", "Tag", "ClassTag", "Cnvs", "Value1", "Value2", "Difference");
    fprintf(hRpt, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", "$PrjName", "", "", (LPCTSTR)m_Prj1, (LPCTSTR)m_Prj2, "");
    }

  CDlgBusy::SetLine(1, "Comparing : %s", Prj[0]());
  CDlgBusy::SetLine(2, "to : %s", Prj[1]());
//
LogNote("Compare Project", 0, "Project 1: %s", (LPCTSTR)m_Prj1);
LogNote("Compare Project", 0, "Project 2: %s", (LPCTSTR)m_Prj2);
LogNote("Compare Project", 0, "Report   : %s", (LPCTSTR)m_Report1);

  TagDB[0].FindRevisionNo(DBRev_MostRecent);
  TagDB[1].FindRevisionNo(DBRev_MostRecent);


  dbgpln("================================ Compare Data Start");
  if (1)
    {
    //CXMsgLst XM;
    CXM_ObjectData OD1(0);
    CXM_ObjectData OD2(0);
    CStopWatch Elapsed;//, LOSW, TLOSW;
    flag AllLoaded=true;
    flag Cancelled=false;
    int n=0;
    Elapsed.Start();

    for (db=0; db<2; db++)
      {
      OD1.List.Clear();
      //OD1.SetSize();
      n=0;
      TagDB[db].StartLoadSequence("Node");
      while (!Cancelled && TagDB[db].LoadObject(&OD1, true)>=0)
        {
        //LOSW.Stop();
        n++;
        if ((n%Min(n,10))==0)
          CDlgBusy::SetLine(3, "Identifying Unit :%i/%i", db+1 ,n);
        Cancelled=CDlgBusy::IsCancelled();
        //nObjs++;
        //LOSW.Start();
        Strng Tag;
     
        CPkDataItem * pItem=OD1.FirstItem();

        if (pItem->Type()==tt_Object)
          {
          CRootObj RObj;
          RObj.Tag=pItem->TagStr();
          RObj.Class=pItem->Class();
          Nds[db].AddTail(RObj);
          CRootObj* pRObj=&Nds[db].GetTail();
          NdMaps[db].SetAt(pRObj->Tag(), pRObj);
          }
        else
          LogError("Compare Project", LF_Exclamation, "Unknown Object Type");


        //dbgpln("%*s %5i: %s", db*20,"", n, Tag());
        }
      CDlgBusy::SetLine(3, "%d) Identifying Unit : %i/%i", db+1, n);
      }
    
    // Check for existance
    for (db=0; db<2; db++)
      {
      CDlgBusy::SetLine(3, "Check for existance : %i", db+1);
      OD2.List.Clear();
      //OD2.SetSize();
      db1=(db+1)%2;
      POSITION Pos=Nds[db].GetHeadPosition();
      while (Pos)
        {
        CRootObj & S1=Nds[db].GetNext(Pos);
        CRootObj *pS2;
        if (!NdMaps[db1].Lookup(S1.Tag(), pS2) && m_MissingTags)
          {
          fprintf(hRpt, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", S1.Tag(), S1.Class(),"", db==0 ? S1.Tag() : "", db==1 ? S1.Tag() : "", db==0 ? "Tag Missing from 2":"Tag Missing from 1");
          }
        }
      }

	  if (m_MissingValues || m_ValuesDifferent)
    if (1)
      {
      LogError("Compare Project", LF_Exclamation, "Object to object comparison not implemented!");
      }
    else
      {
      // Compare Data
      OD2.List.Clear();
      //OD2.SetSize();
      db=0;
      db1=1;
      n=0;
      POSITION Pos=Nds[db].GetHeadPosition();
      while (Pos)
        {
        n++;
        if ((n%Min(n,10))==0)
          CDlgBusy::SetLine(3, "Comparing Unit :%i", n);
        CRootObj & S1=Nds[db].GetNext(Pos);
        CRootObj *pS2;
        if (NdMaps[db1].Lookup(S1.Tag(), pS2))
          {
          int ret0=TagDB[0].LoadObject(S1.Tag(), &OD1);
          int ret1=TagDB[1].LoadObject(S1.Tag(), &OD2);
          if (ret0)
            LogError("Compare Project", LF_Exclamation, "Object %s not loaded from Project1", S1.Tag());
          else if (ret1)
            LogError("Compare Project", LF_Exclamation, "Object %s not loaded form Project2", S1.Tag());
          else
            {
            CompareObjects(&OD1, &OD2);
            }
          }
        }
      }
    /*
    //TLOSW.Start();
    //LOSW.Start();
    OD1.List.Clear();
    OD1.SetSize();
    TagDB1.StartLoadSequence("Node");
    while (!Cancelled && TagDB1.LoadObject(&OD1, false)>=0)
      {
      //LOSW.Stop();
      n++;
      if ((n%Min(n,10))==0)
        CDlgBusy::SetLine(3, "Comparing Unit :%i" ,n);
      Cancelled=CDlgBusy::IsCancelled();
      nObjs++;
      //LOSW.Start();
      }
    //TLOSW.Stop();
    //LOSW.Stop();
    CDlgBusy::SetLine(3,"Comparing Unit :%i",n);
    */
    Elapsed.ReStart();
    /*
    if (AllLoaded)
      {
      n=0;
      //TLOSW.ReStart();
      //LOSW.ReStart();
      OD1.List.Clear();
      OD1.SetSize();
      TagDB1.StartLoadSequence("Conn");
      while (!Cancelled && TagDB1.LoadObject(&OD1, false)>=0)
        {
        //LOSW.Stop();
        n++;
        if ((n%Min(n,10))==0)
          CDlgBusy::SetLine(3,"Comparing Connection :%i",n);
        Cancelled=CDlgBusy::IsCancelled();
        //LOSW.Start();
        }
      //TLOSW.Stop();
      //LOSW.Stop();
      CDlgBusy::SetLine(3, "Comparing Connection :%i", n);

      Elapsed.ReStart();
      }
    */

    //if (AllLoaded)
    //  {
    //  TagDB1.StartLoadSequence("Node");
    //  for (OInx=0; !Cancelled && OInx<Objs_Reg.GetSize(); OInx++)
    //    {
    //    CExecObj *r=Objs_Reg[OInx];
    //    if (LoadIt[OInx])//r->EO_BeginLoad(FCB))
    //      {
    //      n=0;
    //      TLOSW.ReStart();
  ////           TLOSW.Stop();
    //      LOSW.ReStart();
    //      OD1.List.Clear();
    //      OD1.SetSize();
    //      while (!Cancelled && TagDB1.LoadObject(&OD1, false)>=0)
    //        {
    //        LOSW.Stop();
    //        XM.Clear();
    //        if (XM.PackMsg(OD1))
    //          {
    //          n++;
    //          if ((n%Min(n,10))==0)
    //            CDlgBusy::SetLine(3, "Loading Object :%i of %i", n, nObjs);
    //          Cancelled=CDlgBusy::IsCancelled();
    //          #if dbgExecutive
    //          if (dbgDBMsgs())
    //            XM.dbgDump((flag)dbgFullDBMsgs(), r->sName(), "Load");
    //          #endif
    //          TLOSW.Start();
    //          r->EO_LoadDefinedData(FCB, XM);
    //          TLOSW.Stop();
    //          #if dbgStats
    //          if ((n%10)==0)
    //            dbgpln("Load TmElapsed %i, %10.5f/Obj", n, Elapsed.Secs()/Max(1,n));
    //          #endif
    //          }
    //        LOSW.Start();
    //        }
    //      TLOSW.Stop();
    //      LOSW.Stop();
    //      //TLOSW.Stop();
    //      CDlgBusy::SetLine(3, "Loading Object :%i of %i", n, nObjs);
    //
    //      dbgpln("EO_LoadDB %-15s Load      %8.3g = %8.5g/Obj [LO:%8.3f][TLO:%8.3f]",r->sName(), Elapsed.Secs(), Elapsed.Secs()/Max(1,n), LOSW.Secs(), TLOSW.Secs());
    //      }
    //    }
    //  }
    }

  goto CloseRpt;

CloseRpt:
  fclose(hRpt);
Close2:
  TagDB[1].Close();
Close1:
  TagDB[0].Close();
Close0:
  CDlgBusy::Close();

  return true;
  }

//------------------------------------------------------------------------
  
static flag ExpandObject(CXM_ObjectData *pObjData, CObjTagList& List, CObjTagMap & Map)
  {
  int dodbg=1;
  CXM_ObjectData &OD=*pObjData;

  Strng TagStack[30];
  Strng ClsStack[30];
  int iTag=-1;
  Map.InitHashTable(FindNextPrimeNumber(256));
  CPkDataIter Iter;
  for (CPkDataItem* pPItem=OD.FirstItem(Iter); pPItem; pPItem=OD.List.NextItem(Iter))
    {
    CObjTag Tag, *pTag;
    Tag.Str=pPItem->SymOrTag();
    if (!IsLayout(pPItem->Type()) && Tag.Str.Length()>0)
      {
      if (IsStructStart(pPItem->Type()))
        {
        iTag++;
        if (iTag>0)
          {
          TagStack[iTag]=TagStack[iTag-1];
          TagStack[iTag]+=".";
          }
        TagStack[iTag]+=Tag.Str();
        if (pPItem->Class() && _stricmp(pPItem->Class(), "*")!=0)
          ClsStack[iTag]=pPItem->Class();
        else
          {
          ClsStack[iTag]=ClsStack[iTag-1];
          ClsStack[iTag]+=".";
          ClsStack[iTag]+=Tag.Str();
          }
        if (dodbg)
          dbgpln("  %-8.8s: %s",tt_TypeString(pPItem->Type()), ">>", TagStack[iTag]());
        }
      else if (IsStructEnd(pPItem->Type()))
        {
        if (dodbg)
          dbgpln("  %-8.8s: %s",tt_TypeString(pPItem->Type()), "<<", TagStack[iTag]());
        iTag--;
        }
      else
        {
        Tag.Str=TagStack[iTag];
        Tag.Str+=".";
        Tag.Str+=pPItem->SymOrTag();
        Tag.pPItem=pPItem;
        Tag.ClsTag=ClsStack[iTag];
        Tag.ClsTag+=".";
        Tag.ClsTag+=pPItem->SymOrTag();
        List.AddTail(Tag);
        pTag=&List.GetTail();
        Map.SetAt(pTag->Str(), pTag);
        if (dodbg)
          dbgpln("  %-8.8s: %s = %s",tt_TypeString(pTag->Item().Type()), pTag->Str(), pTag->Item().Value()->String());
        }
      }
    else
      {
      if (dodbg)
        dbgpln("  %-8.8s: %s %s", tt_TypeString(pPItem->Type()), pPItem->SymOrTag(), "-------------- SKIP");
      }
    }
  return true;
  }

//------------------------------------------------------------------------

static int GetType(char * pStr)
  {
  int Typ=tt_Strng;
  while (*pStr==' ') pStr++;
  if (*pStr=='+' || *pStr=='-' || isdigit(*pStr))
    {
    Typ=tt_Long;
    pStr++;
    while (isdigit(*pStr))
      pStr++;
    if (*pStr=='.') 
      {
      Typ=tt_Double; 
      pStr++;
      while (isdigit(*pStr))
        pStr++;

      if (*pStr=='E' || *pStr=='e') 
        {
        pStr++;
        if (*pStr=='+' || *pStr=='-')
          pStr++;
        while (isdigit(*pStr))
          pStr++;
        }
      }
    while (*pStr && *pStr==' ')
      pStr++;
    if (*pStr && *pStr!=' ')
      Typ=tt_Strng;
    }
  else
    Typ=tt_Strng;
  return Typ;
  }

//------------------------------------------------------------------------

flag CPrjDataCompare::DoMessage(char * ClsTag0)
  {
  Strng *pClsTag;
  if (!m_FirstInClass || !ClsTagMap.Lookup(ClsTag0, pClsTag))
    {
    if (m_FirstInClass)
      {
      Strng ClsTag(ClsTag0);
      ClsTagLst.AddTail(ClsTag);
      pClsTag=&ClsTagLst.GetTail();
      ClsTagMap.SetAt(pClsTag->Str(), pClsTag);
      }
    return true;
    }
  return false;
  }

//------------------------------------------------------------------------

flag CPrjDataCompare::CompareObject(CObjTagList& List0, CObjTagMap & Map0, 
                          CObjTagList& List1, CObjTagMap & Map1, 
                          FILE* hRpt, double Tol)
  {
  Strng sVal0, sVal1;
  long lVal0, lVal1;
  double  dVal0, dVal1;
  flag OK=true;
  CObjTagListIter Lst0(List0);
  for (CObjTag * pObj0=Lst0.First(); (pObj0); pObj0=Lst0.Next())
    {
    CObjTag * pObj1;
    char * ClsTag0 =pObj0->ClsTag();
    if (Map1.Lookup(pObj0->Str(), pObj1))
      {
      if (m_ValuesDifferent)
        {
        pObj0->Found=true;
        pObj1->Found=true;
        PkDataUnion * pVal0=pObj0->Item().Value();
        PkDataUnion * pVal1=pObj1->Item().Value();
        //char * Class1 =pObj1->Class();
        char * Cnv0 =pObj0->Item().CnvFam();
        char * Cnv1 =pObj1->Item().CnvFam();
        int Typ0=pVal0->Type();
        int Typ1=pVal1->Type();
        char * pStr0=pVal0->String();
        char * pStr1=pVal1->String();
        if (Typ0==tt_Generic)
          Typ0=GetType(pStr0);
        if (Typ1==tt_Generic)
          Typ1=GetType(pStr1);
        flag Done=false;
        if (Typ0!=Typ1)
          {
          if (IsNumData(Typ0) && IsNumData(Typ1))
            {
            Typ0=tt_Double;
            Typ1=tt_Double;
            }
          else if ((IsNumData(Typ0) && _stricmp(pStr1, "*")==0) || 
                   (IsNumData(Typ1) && _stricmp(pStr0, "*")==0))
            {
            if (DoMessage(ClsTag0))
              fprintf(hRpt, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%i\"\n", pObj0->Str(), ClsTag0, "", pStr0, pStr1, "Disabled");
            Done=true;
            }
          }
        if (Typ0==Typ1 && !Done)
          {
          if (IsIntData(Typ0))
            {
            lVal0=atol(pStr0);
            lVal1=atol(pStr1);
            if (lVal0!=lVal1 && DoMessage(ClsTag0))
              fprintf(hRpt, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%i\"\n", pObj0->Str(), ClsTag0, "", pStr0, pStr1, lVal1-lVal0);
            }
          else if (IsFloatData(Typ0))
            {
            dVal0=atof(pStr0);
            dVal1=atof(pStr1);
            double Err=0;
            if (!ConvergedVV(dVal0, dVal1, 1.0e-12, Tol, Err) && DoMessage(ClsTag0))
              fprintf(hRpt, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%g\"\n", pObj0->Str(), ClsTag0, "", pStr0, pStr1, dVal1-dVal0);
            }
          else //if (IsFloatData(Typ0))
            {
            if (_stricmp(pStr0, pStr1)!=0 && DoMessage(ClsTag0))
              fprintf(hRpt, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", pObj0->Str(), ClsTag0, "", pStr0, pStr1, "");
            }
          }
        else
          {
          if (DoMessage(ClsTag0))
            fprintf(hRpt, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", pObj0->Str(), ClsTag0, "", pStr0, pStr1, "Types Different");
          }
        }      
      }
    else if (m_MissingValues)
      {
      if (DoMessage(ClsTag0))
        fprintf(hRpt, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", pObj0->Str(), ClsTag0, "", pObj0->Item().Value()->String(), "", "Missing from 2");
      }
    }

  if (m_MissingValues)
    {
    CObjTagListIter Lst1(List1);
    for (CObjTag * pObj1=Lst1.First(); (pObj1); pObj1=Lst1.Next())
      {
      char * ClsTag1 =pObj1->ClsTag();
      if (!pObj1->Found)
        {
        if (DoMessage(ClsTag1))
          fprintf(hRpt, "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", pObj1->Str(), ClsTag1, "", "", pObj1->Item().Value()->String(), "Missing from 1");
        }
      }
    }
  return OK;
  }

//------------------------------------------------------------------------

flag CPrjDataCompare::CompareObjects(CXM_ObjectData *pObjData0, CXM_ObjectData *pObjData1)
  {
  CObjTagMap Map0, Map1;
  CObjTagList Lst0, Lst1;
  ExpandObject(pObjData0, Lst0, Map0);
  ExpandObject(pObjData1, Lst1, Map1);

  return CompareObject(Lst0, Map0, Lst1, Map1, hRpt, m_Tolerance);
  }

//------------------------------------------------------------------------
