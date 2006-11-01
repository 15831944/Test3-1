//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992
 
#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#define  __POWER_CPP
#include "power.h"
#include "flwnode.h"
#include "elecbase.h"

//XID xidNomVolts       = PwrXID(1);
//XID xidNomCurrent     = PwrXID(2);
//XID xidNomPower       = PwrXID(3);
XID xidTSVolts        = PwrXID(4);
XID xidTSCurrent      = PwrXID(5);
XID xidTSPhase        = PwrXID(6);
XID xidPwrSupply      = PwrXID(7);
XID xidRemoteSrc      = PwrXID(8);
XID xidPwrAvail       = PwrXID(9);
XID xidOn             = PwrXID(10);

//===========================================================================
//
//
//
//===========================================================================

DDBValueLst DDBCTTypes[] = 
  {
  {eCT_1Ph2W, "SinglePhase"},
  {eCT_3Ph3W, "3Phase-3Wire"},
  {eCT_3Ph4W, "3Phase-4Wire"},
  {0}
  };

//===========================================================================
//
//
//
//===========================================================================

static CPwrSupply * CreatePwrSupply(CPwrTypes Type)
  {
  switch (Type)
    {
    case CPS_Elec: return new CElecSupply;
    case CPS_Air:  return new CAirSupply;
    case CPS_None: return new CPwrSupply;
    }
  return NULL;
  };

//---------------------------------------------------------------------------

static CPwrLoad * CreatePwrLoad(CPwrTypes Type)
  {
  switch (Type)
    {
    case CPS_Elec: return new CElecLoad;
    case CPS_Air:  return new CAirLoad;
    case CPS_None: return new CPwrLoad;
    }
  return NULL;
  };

//===========================================================================
//
//
//
//===========================================================================

CElecLoad::CElecLoad()
  {
  m_dShaftPower=0;
  m_dPhaseAngle=acos(0.8);
  };

//---------------------------------------------------------------------------

void CElecLoad::BuildDataDefn(DataDefnBlk & DDB, TaggedObject *pTagObj, LPCSTR Tag, bool AsParm)
  {
  if (DDB.BeginObject(pTagObj, (LPSTR)Tag, "E_Load", NULL, DDB_NoPage, -1, AsParm?0:MarkerClosed))
    {
    DDB.Double("PowerFactor", "", DC_CosPhi, "Cos(<)", &m_dPhaseAngle,  pTagObj, isParm);
    DDB.Double("ShaftPower",  "", DC_Pwr,    "kW",     &m_dShaftPower,  pTagObj, AsParm?isParm:isResult);
    }                               
  DDB.EndStruct();
  };

//---------------------------------------------------------------------------

void CElecSupply::BuildDataDefn(DataDefnBlk & DDB, TaggedObject *pTagObj, LPCSTR Tag, bool AsParm)
  {
  //if (DDB.BeginObject(pTagObj, (LPSTR)Tag, "ElecCd", NULL, DDB_NoPage, -1, AsParm?0:MarkerClosed))
  //  {
  //  }                               
  //DDB.EndStruct();
  };

//---------------------------------------------------------------------------

flag CElecSupply::DataXchg(DataChangeBlk & DCB, TaggedObject *pOwner) 
  { 
//  switch (DCB.lHandle)
//    {
//    }

  return 0;
  };

//---------------------------------------------------------------------------

flag CElecSupply::ValidateData(ValidateDataBlk & VDB, TaggedObject *pOwner) 
  { 
  return true; 
  };

//---------------------------------------------------------------------------

bool CElecSupply::Present()
  { 
  if (m_pTermStrip && m_pTermStrip->IsPowerStd())
    return m_pTermStrip->PowerPresent();

  return m_bOn; 
  };

//---------------------------------------------------------------------------

bool CElecSupply::SetPower(CPwrLoad * pLoad)
  {
  CElecLoad *pLd=m_bOn ? dynamic_cast<CElecLoad*>(pLoad) : NULL;

  m_dPower = pLd ? pLd->ShaftPower() : 0;
  m_dPhaseAngle = pLd ? pLd->m_dPhaseAngle : 0;

  if (m_pTermStrip)
    {
    m_pTermStrip->SetLoad(m_dPower, cos(m_dPhaseAngle));
    }

  return true;
  }

//===========================================================================
//
//
//
//===========================================================================

CAirLoad::CAirLoad()
  {
  m_dShaftPower=0;
  };

//---------------------------------------------------------------------------

void CAirLoad::BuildDataDefn(DataDefnBlk & DDB, TaggedObject *pTagObj, LPCSTR Tag, bool AsParm)//, bool WithOn)
  {
  if (DDB.BeginObject(pTagObj, (LPSTR)Tag, "AirLd", NULL, DDB_NoPage, -1, AsParm?0:MarkerClosed))
    {
    DDB.Text("CElecLoad");
    }                               
  DDB.EndStruct();
  };

//===========================================================================
//
//
//
//===========================================================================

CAirSupply::CAirSupply() 
  {
  m_dXXXX=0;
  };

//---------------------------------------------------------------------------

void CAirSupply::BuildDataDefn(DataDefnBlk & DDB, TaggedObject *pTagObj, LPCSTR Tag, bool AsParm)//, bool WithOn)
  {
  if (DDB.BeginObject(pTagObj, (LPSTR)Tag, "AirCd", NULL, DDB_NoPage, -1, AsParm?0:MarkerClosed))
    {
//    DDB.Double("",          "Phie", DC_CosPhi, "deg",  &m_Earth.m_dPhi,    pTagObj, (AsParm?isParm:0)|InitHidden);
    }                               
  DDB.EndStruct();
  };

//---------------------------------------------------------------------------

bool CAirSupply::SetPower(CPwrLoad * pLoad)
  {
  return false;
  }

//===========================================================================
//
//
//
//===========================================================================

//IMPLEMENT_TAGOBJ(Obj, Grp, ClassId, Version, DrwGroup, DefTag, Cat, ShortDesc, Desc)
IMPLEMENT_TAGOBJ(CPwrUser, "PwrUser", "PwrUser", "1", "Power", "P", 0, "PwrUser", "PwrUser");

CPwrUser::CPwrUser(TagObjClass *pClass_, char *TagIn, TaggedObject *pAttach, TagObjAttachment eAttach):
  TaggedObject(pClass_, TagIn, pAttach, eAttach)
  {
  //m_pNd=pFlwNd;
  m_pNd=dynamic_cast<FlwNode*>(pAttach);
  m_sPwrSupply="525v_A/C";
  m_iInx=-1;
  m_dPowerDraw=0;
  m_pSupply=NULL;
  m_eConnectRqd=CPU_ByName;
  m_eConnectAct=CPU_None;
  m_pLoad=NULL;
  m_pSocket=NULL;
  m_pTermStrip=NULL;

  if (m_pNd)
    m_pNd->AddPwrUser(this);

  Load(); // Initialise a Load
  SetConnect(CPU_None, NULL);
  }

//---------------------------------------------------------------------------

CPwrUser::CPwrUser(char *TagIn, TaggedObject *pAttach, TagObjAttachment eAttach):
  TaggedObject(&CPwrUserClass, TagIn, pAttach, eAttach)
  {
  //m_pNd=pFlwNd;
  //m_pNd=0;
  m_pNd=dynamic_cast<FlwNode*>(pAttach);
  m_sPwrSupply="525v_A/C";
  m_iInx=-1;
  m_dPowerDraw=0;
  m_pSupply=NULL;
  m_eConnectRqd=CPU_ByName;
  m_eConnectAct=CPU_None;
  m_pLoad=NULL;
  m_pSocket=NULL;
  m_pTermStrip=NULL;
  //m_nType=eCT_3Ph4W;
  //m_dVolts=220;
  if (m_pNd)
    m_pNd->AddPwrUser(this);

  Load(); // Initialise a Load

  SetConnect(CPU_None, NULL);
  }

//---------------------------------------------------------------------------

CPwrUser::~CPwrUser()
  {
  if (m_pNd)
    m_pNd->RemovePwrUser(this);
  m_pSocket=NULL;
  //delete m_pSupply;
  delete m_pLoad;
  }

//---------------------------------------------------------------------------

void CPwrUser::Initialise(LPCTSTR SupplyName)
  {
  m_sPwrSupply=SupplyName;
  m_eConnectRqd=CPU_ByName;
  Load();
  }

//---------------------------------------------------------------------------

void CPwrUser::BuildDataDefn(DataDefnBlk & DDB)
  { 
  BuildDataDefn(DDB, NULL, NULL, -1);
  };

//---------------------------------------------------------------------------

void CPwrUser::BuildDataDefn(DataDefnBlk & DDB, TaggedObject * pOwnObj, LPTSTR pName, dword dwUserInfo)//, TaggedObject * pTagObj)
  { 
  bool CfgSupply = m_sRemoteSrc.GetLength()==0;//
  //(m_eConnect!=CPU_Direct && m_eConnect!=CPU_Socket);//m_pSupply!=NULL);
  CPwrSupply * pCnd=Supply();

  if (DDB.BeginStruct(this, pName?pName:"PowerSupply", NULL, DDB_NoPage, dwUserInfo))
    {
    if (1)//CfgSupply || DDB.ForFileSnpScn())
      {
      DDBValueLstMem DDBV;
      DDBV.Empty();
      DDBV.Add(-1, "None");
      DDBV.Add(-2, "Socket");
      DDBV.Add(-3, "Wired");
      for (int i=0; i<gs_PwrSupplies.NSupplies(); i++)
        DDBV.Add(i, (char*)gs_PwrSupplies.GetTag(i));
      DDB.String("",  "Source",   DC_,  "", xidPwrSupply,  this, isParmStopped|SetOnChange, &DDBV);

      }
    //else
    //  {
    //  Strng S;
    //  switch (m_eConnect)
    //    {
    //    case CPU_Wired : S="AreaIO";        break;
    //    case CPU_Socket: S="Socket";        break;
    //    default:  S.Set("'%s' missing", m_sRemoteSrc()); break;
    //    }
    //  DDB.String("",  "Source",   DC_,  "", &S,  this, 0);
    //  }
    if (1)//CfgSupply)
      DDB.TagComment((char*)State());
    //if (m_sPwrSupply.XStrICmp("Socket")==0)
    DDB.Bool("",    "Available",    DC_,        "",      xidPwrAvail, this, 0);//|InitHidden);

    if (m_eConnectRqd==CPU_Socket || DDB.ForFileSnpScn())
      DDB.String("",    "Socket",    DC_,        "",      xidRemoteSrc, this, m_eConnectRqd==CPU_Socket?isParmStopped:0);
    if (m_eConnectRqd==CPU_Wired || DDB.ForFileSnpScn())
      {
      //DDB.Long       ("TermCfg",    "", DC_,   "",  (long*)&m_nType, this, isParm, DDBCTTypes);
      //DDB.Double     ("NomPhVolts", "", DC_V,  "V", &m_dVolts,       this, isParm);
      }
    }
  DDB.EndStruct();

  if (Supply())
    Supply()->BuildDataDefn(DDB, this, "Supply", CfgSupply);
  Load()->BuildDataDefn(DDB, this, "Load", true);
  };

//---------------------------------------------------------------------------

flag CPwrUser::DataXchg(DataChangeBlk & DCB)//, TaggedObject * pTagObj)
  {
  if (Supply() && Supply()->DataXchg(DCB, this))
    return 1;
  if (Load()->DataXchg(DCB, this))
    return 1;
  
  switch (DCB.lHandle)
    {
    case xidPwrSupply:
      {
      if (DCB.rpC)
        {       
        SetConnectRqd(DCB.rpC);
        }

      switch (m_eConnectRqd)
        {
        case CPU_Socket:  DCB.pC="Socket";              return 1;
        case CPU_Wired:   DCB.pC="Wired";               return 1;
        case CPU_None:    DCB.pC="None";                return 1;
        case CPU_ByName:  DCB.pC=m_sPwrSupply();        return 1;
        }
      return 0;
      }
    case xidRemoteSrc:
      if (DCB.rpC)
        {
        m_sRemoteSrc=DCB.rpC;
        m_sRemoteSrc.LRTrim();
        }
      DCB.pC=m_sRemoteSrc();
      return 1;
    case xidPwrAvail:
      DCB.B= (m_pSupply ? m_pSupply->Present() : false);
      return 1;
    }      
  return 0;
  };

//---------------------------------------------------------------------------

flag CPwrUser::ValidateData(ValidateDataBlk & VDB)//, TaggedObject * pTagObj)
  { 
  //switch (m_eConnectRqd)
  //  {
  //  case CPU_ByName:
  //    {
  //    for (int i=0; i<gs_PwrSupplies.NSupplies(); i++)
  //      if (m_sPwrSupply.XStrICmp((LPCSTR)gs_PwrSupplies.GetTag(i))==0)
  //        break;
  //    if (i>=gs_PwrSupplies.NSupplies())
  //      m_sPwrSupply="525v_A/C";
  //    break;
  //    } 
  //  case CPU_Wired:
  //    SetConnect(CPU_Wired, NULL);
  //    break;
  //  }
  return true; 
  };

//---------------------------------------------------------------------------

CPwrUser::CPwrUserConns CPwrUser::SetConnectRqd(LPCTSTR pName)
  {
  m_sPwrSupply=pName;
  if (_stricmp(pName, "Socket")==0)
    m_eConnectRqd=CPU_Socket;
  else if (_stricmp(pName, "Wired")==0)
    m_eConnectRqd=CPU_Wired;
  else if (_stricmp(pName, "None")==0)
    m_eConnectRqd=CPU_None;
  else 
    m_eConnectRqd=CPU_ByName;
  // Update 
  switch (m_eConnectRqd)
    {
    case CPU_ByName:
      {
      for (int i=0; i<gs_PwrSupplies.NSupplies(); i++)
        if (m_sPwrSupply.XStrICmp((LPCSTR)gs_PwrSupplies.GetTag(i))==0)
          break;
      if (i>=gs_PwrSupplies.NSupplies())
        m_sPwrSupply="525v_A/C";
      break;
      } 
    case CPU_Wired:
      SetConnect(CPU_Wired, NULL);
      break;
    }
  return m_eConnectRqd;
  }

//---------------------------------------------------------------------------

flag CPwrUser::GlobalConnect(FlwNode * pFlwNd, CPwrSupply * pSupply)
  {
  m_pNd=pFlwNd;
  m_pSocket=NULL;
  //if (pSupply)
  //  {
  //  SetConnect(CPU_Direct, pSupply);
  //  return true;
  //  }
  
  switch (m_eConnectRqd)
    {
    case CPU_Wired:
      SetConnect(CPU_Wired, NULL);
      break;
    case CPU_Socket:
      if (m_sRemoteSrc.GetLength()>0)
        {  
        TaggedObject *pRoot = (TaggedObject*)m_pNd->InsertRoot(NULL);
        char TheTag[1024];
        strcpy(TheTag, m_sRemoteSrc());
        int ObjTagLen=0;
        TaggedObject *p=(TaggedObject *)pRoot->FindObjTag(TheTag, true, ObjTagLen, 0);
        if (p)
          {
          m_pSocket=dynamic_cast<CPwrUserSocket*>(p);
          if (m_pSocket==NULL)
            {
            pTaggedObject p1=p->pAttachments;
            while (p1 && (_stricmp(p1->Tag(), &TheTag[ObjTagLen+1])!=0))
              p1=p1->pNxtAttachment;
            if (p1)
              p=p1;
            m_pSocket=dynamic_cast<CPwrUserSocket*>(p);
            }
          if (m_pSocket)
            {
            SetConnect(CPU_Socket, m_pSocket->Supply());//&m_Value);
            m_pSocket->ConnectUser(this);
            return true;
            }
          }
        LogWarning(m_pNd->FullObjTag(), 0, "Remote Tag %s not Found",m_sRemoteSrc());
        }
      else
        LogWarning(m_pNd->FullObjTag(), 0, "No Remote Tag specified");
      break;
    case CPU_ByName:
      {
      CPwrSupply * pCd=gs_PwrSupplies.Supply(*this);
      if (pCd)
        {
        SetConnect(CPU_ByName, pCd);
        return true;
        }
      else
        LogWarning(m_pNd->FullObjTag(), 0, "Power supply not found");
      break;
      }
    case CPU_None:
      SetConnect(CPU_None, NULL);
      break;
    }
  return true;
  }

//---------------------------------------------------------------------------

flag CPwrUser::GlobalDisConnect()
  {
  if (m_pSocket)
    {
    m_pSocket->DisConnectUser();
    SetConnect(CPU_None, NULL);
    }
  return true;
  }

//---------------------------------------------------------------------------

bool CPwrUser::ChangeLoadToMatch(CPwrLoad ** pLoad, CPwrSupply *pSupply)
  {
  CPwrLoad * pOld=*pLoad;
  if (pSupply)
    {
    if (dynamic_cast<CElecSupply*>(pSupply))
      {
      if (dynamic_cast<CElecLoad*>(*pLoad)==NULL)
        {
        delete *pLoad;
        *pLoad=new CElecLoad;
        }
      }
    else if (dynamic_cast<CAirSupply*>(pSupply))
      {
      if (dynamic_cast<CAirLoad*>(*pLoad)==NULL)
        {
        delete *pLoad;
        *pLoad=new CAirLoad;
        }
      }
    else
      {
      if (dynamic_cast<CPwrLoad*>(*pLoad)==NULL)
        {
        delete *pLoad;
        *pLoad=new CPwrLoad;
        }
      }
    }
  else // pSupply==NULL 
    {
    // Leave Load alone
    //delete m_pLoad;
    //m_pLoad=new CPwrLoad;
    }
  return pOld!=*pLoad;
  }

//---------------------------------------------------------------------------

void CPwrUser::SetConnect(CPwrUserConns Connect, CPwrSupply *pSupply)
  {
  if (Connect==CPU_Wired)
    {
    if (!m_pTermStrip)
      {
      m_pTermStrip=new CETermStrip(m_pNd, Tag(), eCT_3Ph4W);
      m_pSupply=new CElecSupply(m_pTermStrip);
      }
    pSupply=m_pSupply; // Pick up orig
    }
  else
    {
    if (m_pTermStrip)
      {
      delete m_pSupply;
      delete m_pTermStrip;
      m_pTermStrip=NULL;      
      m_pSupply=NULL;
      }
    }
  //if (!m_pSupply && pSupply)
  //  {
  //  //Connect=CPU_None;
  //  if (!NetProbalMethod())
  //    LogWarning(m_pNd?m_pNd->FullObjTag():"PwrUser:SetConnect",0,"Bad Supply");
  //  }
  m_eConnectAct=Connect;
  m_pSupply=pSupply;
  ChangeLoadToMatch(&m_pLoad, pSupply);
  }

//---------------------------------------------------------------------------

CPwrSupply * CPwrUser::Supply()
  { 
  //if (m_pSupply)
  return m_pSupply;
  //return gs_PwrSupplies.Supply(*this);
  };

//---------------------------------------------------------------------------

LPCTSTR CPwrUser::State()
  {
  //if (m_pSupply)
  return m_pSupply ? m_pSupply->Present() ? "On":"Off" : "?";
  //return gs_PwrSupplies.State(*this);
  };

//---------------------------------------------------------------------------

flag    CPwrUser::On()
  {
  //if (m_pSupply)
  return m_pSupply ? m_pSupply->Present() : false;
  //return gs_PwrSupplies.On(*this);
  };

//---------------------------------------------------------------------------

CPwrTypes CPwrUser::Type()
  {
  if (dynamic_cast<CElecSupply*>(m_pSupply))
    return CPS_Elec;
  if (dynamic_cast<CAirSupply*>(m_pSupply))
    return CPS_Air;
  return CPS_None;//;gs_PwrSupplies.Type(*this);
  };

//---------------------------------------------------------------------------

double CPwrUser::SetPower(double ShaftPower)
  {
  if (m_pNd==NULL)
    {
    INCOMPLETECODE();
    LogWarning("PwrUser", 0, "GlobalConnect Not Called");
    }
  if (m_pLoad) //kga 15/8/03 should this ever be NULL ?!!!
    {
    m_pLoad->SetShaftPower(ShaftPower);
    if (Supply())
      {
      if (!Supply()->Present())
        m_pLoad->SetShaftPower(0);
      return Supply()->SetPower(m_pLoad);
      }
    } 
  return ShaftPower;
  };

//---------------------------------------------------------------------------

void CPwrUser::ConnectPower()
  {
  if (m_pNd==NULL)
    {
    INCOMPLETECODE();
    LogWarning("PwrUser", 0, "GlobalConnect Not Called");
    }
  //m_pLoad->SetShaftPower(ShaftPower);
  if (Supply())
    {
    //if (!Supply()->Present())
    //  m_pLoad->SetShaftPower(0);
    Supply()->SetPower(m_pLoad);
    }
  };

//---------------------------------------------------------------------------

void CPwrUser::Zero()
  {
//  m_dPowerDraw=m_dCurrent=0;
  m_dPowerDraw=0;
  };

//---------------------------------------------------------------------------

CPwrLoad * CPwrUser::Load()   
  {
  if (!m_pLoad)
    m_pLoad=CreatePwrLoad(Type());
  return m_pLoad; 
  };

//---------------------------------------------------------------------------

CETermStrip *CPwrUser::TermStrip()
  {
  return m_pTermStrip;
  };

//===========================================================================
//
//
//
//===========================================================================

//IMPLEMENT_TAGOBJ(Obj, Grp, ClassId, Version, DrwGroup, DefTag, Cat, ShortDesc, Desc)
IMPLEMENT_TAGOBJ(CPwrUserSocket, "PwrUserSocket", "PwrUserSocket", "1", "Power", "P", 0, "PwrUserSocket", "PwrUserSocket");

CPwrUserSocket::CPwrUserSocket(TagObjClass *pClass_, char *TagIn, TaggedObject *pAttach, TagObjAttachment eAttach):
  TaggedObject(pClass_, TagIn, pAttach, eAttach)
  {
  m_pUser=NULL;
  m_pSupply=NULL;
  m_pLclLoad=NULL;
//  m_pRmtLoad=NULL;
  }

//---------------------------------------------------------------------------

CPwrUserSocket::CPwrUserSocket(char *TagIn, TaggedObject *pAttach, TagObjAttachment eAttach) : //, CPwrLoad * pLd, CPwrSupply * pCd) :
  TaggedObject(&CPwrUserSocketClass, TagIn, pAttach, eAttach)
  {
  //m_bOn=false;
  m_pUser=NULL;
  m_pSupply=NULL;
  m_pLclLoad=NULL;
//  m_pRmtLoad=NULL;
  CPwrUser::ChangeLoadToMatch(&m_pLclLoad, m_pSupply);
  };

//---------------------------------------------------------------------------

CPwrUserSocket::~CPwrUserSocket()
  {
  delete m_pLclLoad;
  }

//---------------------------------------------------------------------------

void CPwrUserSocket::BuildDataDefn(DataDefnBlk & DDB)//, TaggedObject * pTagObj)
  { 
  if (DDB.BeginStruct(this, "Socket", NULL, DDB_NoPage))
    {
    DDB.CheckBoxBtn("On", "", DC_, "", xidOn, this, isParm);
    if (Supply())
      Supply()->BuildDataDefn(DDB, this, "Supply", false);//!HasSupply);
    if (m_pUser)
      {
      if (m_pUser->m_pLoad)
        m_pUser->m_pLoad->BuildDataDefn(DDB, this, "Load", true);
      }
    if (!m_pUser || DDB.ForFileSnpScn())
      {
      if (m_pLclLoad)
        m_pLclLoad->BuildDataDefn(DDB, this, "LclLoad", !m_pUser);
      }
     //
    //Load()->BuildDataDefn(DDB, this, "Load", true);
    }
  DDB.EndStruct();
  };

//---------------------------------------------------------------------------

flag CPwrUserSocket::DataXchg(DataChangeBlk & DCB)//, TaggedObject * pTagObj)
  {
  switch (DCB.lHandle)
    {
    case xidOn:
      if (Supply())
        {
        if (DCB.rB)
          Supply()->SetOn(*DCB.rB!=0);
        DCB.B=Supply()->On();
        }
      else
        DCB.B=0;
      return 1;
    }
  if (Supply())
    Supply()->DataXchg(DCB, this);
  if (m_pUser)
    {
    if (m_pUser->m_pLoad && m_pUser->m_pLoad->DataXchg(DCB, this))
      return 1;
    }
  if (!m_pUser || DCB.ForFileSnpScn())
    {
    if (m_pLclLoad && m_pLclLoad->DataXchg(DCB, this))
      return 1;
    }
  return 0;
  };

//---------------------------------------------------------------------------

flag CPwrUserSocket::ValidateData(ValidateDataBlk & VDB)//, TaggedObject * pTagObj)
  { 
  return true; 
  };

//--------------------------------------------------------------------------

bool CPwrUserSocket::ConnectUser(CPwrUser *pUser)
  { 
  m_pUser=pUser;
  return true;
  };

//--------------------------------------------------------------------------

bool CPwrUserSocket::DisConnectUser()
  { 
  m_pUser=NULL;
  return true;
  };

//--------------------------------------------------------------------------

void CPwrUserSocket::SetSupply(CPwrSupply * pSupply)
  {
  m_pSupply=pSupply;
  CPwrUser::ChangeLoadToMatch(&m_pLclLoad, pSupply);
  }

//--------------------------------------------------------------------------

CPwrSupply * CPwrUserSocket::Supply()
  { 
  return m_pSupply;
  };

//--------------------------------------------------------------------------

void CPwrUserSocket::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
  };

//--------------------------------------------------------------------------

void CPwrUserSocket::EvalPowerRequired()
  {
  if (m_pSupply)
    {
    //if (m_bOn)
      m_pSupply->SetPower(m_pUser ? m_pUser->m_pLoad : m_pLclLoad);
    //else
    //  m_pSupply->SetPower(NULL);
    }
  };

//===========================================================================
//
//
//
//===========================================================================

CPwrSupplyItem::CPwrSupplyItem()
  {
  m_Tag="Supply";
  m_pSupply=new CElecSupply;
  };

//---------------------------------------------------------------------------

CPwrSupplyItem & CPwrSupplyItem::operator=(CPwrSupplyItem & I)
  {
  m_Tag=I.m_Tag;
  if (m_pSupply && (m_pSupply->Type()!=I.Type()))
    {
    delete m_pSupply;
    m_pSupply = CreatePwrSupply(I.Type());
    };
  *m_pSupply = *I.Supply();
  return *this;
  };

//---------------------------------------------------------------------------

CPwrTypes CPwrSupplyItem::Type()
  {
  if (dynamic_cast<CElecSupply*>(m_pSupply))
    return CPS_Elec;
  if (dynamic_cast<CAirSupply*>(m_pSupply))
    return CPS_Air;
  return CPS_None;
  //gs_PwrSupplies.Type(*this);
  };

//===========================================================================
//
//
//
//===========================================================================

CPwrSupplies gs_PwrSupplies;

CPwrSupplies::CPwrSupplies()
  {
  m_Map.InitHashTable(23);
  };

//---------------------------------------------------------------------------

void CPwrSupplies::AddSupply(LPCTSTR Tag, CPwrTypes Type, double Voltage)//, eTSConfigurations Phases)
  {
  CPwrSupplyItem I;
  I.m_Tag=Tag;
  I.m_pSupply=CreatePwrSupply(Type);
  I.SetOn(true);
  
  if (Type==CPS_Elec)
    {
    CElecSupply*pE=(CElecSupply*)I.m_pSupply;
//ELEC     pE->m_TStrip.SetStdCable(Phases, Voltage);
    }

  char*p;
  while ((p=I.m_Tag.XStrChr(' '))!=NULL)
    *p='_';

  long i=m_Supplies.Add(I);
  FixMap();
  };

//---------------------------------------------------------------------------

void CPwrSupplies::RemSupply(LPCTSTR Tag)
  {
  long i;
  if (m_Map.Lookup(Tag, i))
    {
    m_Supplies.RemoveAt(i);
    FixMap();
    }
  };

//---------------------------------------------------------------------------

long CPwrSupplies::NSupplies() { return m_Supplies.GetSize(); };

void CPwrSupplies::SetNSupplies(long n)
  {
  m_Supplies.SetSize(n);
  FixMap();
  };

//---------------------------------------------------------------------------

LPCTSTR CPwrSupplies::GetTag(long n) { return m_Supplies[n].m_Tag(); };

void CPwrSupplies::SetTag(long n, LPCTSTR Tag)
  {
  if (Tag && strlen(Tag))
    {
    long i;
    if (!m_Map.Lookup(Tag, i))
      {
      m_Supplies[n].m_Tag=Tag;
      // ?? sort
      FixMap();
      }
    }
  };

//---------------------------------------------------------------------------

void CPwrSupplies::FixMap()
  {
  m_Map.RemoveAll();
  for (int i=0; i<m_Supplies.GetSize(); i++)
    if (m_Supplies[i].m_Tag.GetLength()>0)
      m_Map.SetAt(m_Supplies[i].m_Tag(), i);
  }

//---------------------------------------------------------------------------

void CPwrSupplies::Initialise(bool Full)
  {
  if (Full)
    {
//    if (OrigPrjFileVerNo()>=62)
//      AddSupply("220v_A/C", CPS_Electric, 220, 1);
    AddSupply("380v_A/C", CPS_Elec, 380);//, eCT_3Ph4W);
    AddSupply("525v_A/C", CPS_Elec, 525);//, eCT_3Ph4W);
    AddSupply("7bar_Air", CPS_Air,    0);//, eCT_Null);
    }
  };

//---------------------------------------------------------------------------

void CPwrSupplies::Terminate()
  {
  m_Supplies.SetSize(0);
  }          

//---------------------------------------------------------------------------

void CPwrSupplies::ZeroAll()
  {
  for (int i=0; i<m_Supplies.GetSize(); i++)
    {
    }
  }          

//---------------------------------------------------------------------------

CPwrSupply *CPwrSupplies::Supply(CPwrUser & User)
  {
  CPwrSupplyItem * Src=GetSupply(User);
  if (Src)
    return Src->m_pSupply;
  return NULL;
  };

//===========================================================================
//
//
//
//===========================================================================
