//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "materialmerge.h"
#include <string>
#include <vector>
#pragma optimize("", off)

//====================================================================================

const long idFeed = 0;
const long idProd = 1;

static MInOutDefStruct s_IODefs[]=
{
  //  Desc;             Name;        PortId; Rqd; Max; CnId, FracHgt;  Options;
  { "Feed",           "Feed",      idFeed,   1,  10,    0,    1.0f,  MIO_In |MIO_Material },
  { "Product",        "Product",   idProd,   1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
  { NULL },
};

static double Drw_MaterialMerge[] = { MDrw_Poly, -8,4, -8,-4, 8,-4, 8,4, -8,4,
MDrw_Poly, -5,3, -3,0, -5,-3,
MDrw_Poly, 0,3, 2,0, 0,-3,
MDrw_Poly, 3,3, 5,0, 3,-3,
MDrw_End };

//---------------------------------------------------------------------------
DEFINE_TRANSFER_UNIT(MaterialMerge, "MaterialMerge", DLL_GroupName)

void MaterialMerge_UnitDef::GetOptions()
{
  SetDefaultTag("SR");
  SetDrawing("Tank", Drw_MaterialMerge);
  SetTreeDescription("Iluka:Material Merge");
  SetDescription("TODO: A description");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow);
  SetModelGroup(MGroup_General);
  SetModelLicense(MLicense_Standard);
};

//---------------------------------------------------------------------------

MaterialMerge::MaterialMerge(MUnitDefBase * pUnitDef, TaggedObject * pNd) : 
MBaseMethod(pUnitDef, pNd)
{
  //default values...
  m_sMaterialType = 0;
  allCount = 0;
  destCount = 0;
  phaseCount = 0;

  destIndex = NULL;
  destText = NULL;

  map = NULL;
  mapText = NULL;

  makeObject = NULL;
  enable = NULL;

  totals = NULL;

  targetTypeDropDown = NULL;
}

//---------------------------------------------------------------------------

MaterialMerge::~MaterialMerge()
{
  if (destIndex != NULL)
    delete[] destIndex;

  if (destText != NULL)
    delete[] destText;

  if (map != NULL)
  {
    for (int i=0; i<allCount; i++)
      if (map[i] != NULL)
        delete[] map[i];
    delete[] map;
  }

  if (mapText != NULL)
  {
    for (int i=0; i<allCount; i++)
      if (mapText[i] != NULL)
        delete[] mapText[i];
    delete[] mapText;
  }

  if (makeObject != NULL)
    delete[] makeObject;

  if (enable != NULL)
    delete[] enable;

  if (totals != NULL)
    delete[] totals;

  if (targetTypeDropDown != NULL)
    delete[] targetTypeDropDown;
}

//---------------------------------------------------------------------------

void MaterialMerge::Init()
{
  SetIODefinition(s_IODefs);
}

//---------------------------------------------------------------------------

const int idDX_MaterialType = 1;

void MaterialMerge::BuildDataFields()
{
  if (targetTypeDropDown == NULL)
  {
    allCount = gs_MVDefn.Count();

    for (int i=0; i<allCount; i++)
    {
      std::string symbol = gs_MVDefn[i].Symbol();
      std::string phase = symbol.substr(symbol.find("(")+1, symbol.length()-symbol.find("(")-2); // leave off the brackets.

      if ((phase != "s") && (phase != "l") && (phase != "g")) // Don't want elements.
      {
        // All this just allows only the items that have more than one copy.
        // That means that elements don't get included.
        bool existstmp = false;
        for (std::vector<std::string>::size_type p=0; p<phasestmp.size(); p++)
        {
          if (phasestmp[p] == phase)
            existstmp = true;
        }

        if (!existstmp)
        {
          phasestmp.insert(phasestmp.end(), phase);
        }
        else
        {
          // If we're here then we have found a multiple 'phase' so it's to be included.
          bool exists = false;
          for (std::vector<std::string>::size_type p=0; p<phases.size(); p++)
          {
            if (phases[p] == phase)
              exists = true;
          }

          if (!exists)
          {
            phases.insert(phases.end(), phase);
            phaseCount++;
          }
        }
      }
    }

    targetTypeDropDown = new MDDValueLst[phaseCount+2];

    targetTypeDropDown[0].m_dwFlags = 0;
    targetTypeDropDown[0].m_lVal = 0;
    targetTypeDropDown[0].m_pStr = (LPTSTR)"Off";

    targetTypeDropDown[phaseCount+1].m_dwFlags = 0;
    targetTypeDropDown[phaseCount+1].m_lVal = 0;
    targetTypeDropDown[phaseCount+1].m_pStr = 0;

    for (int dd = 0; dd < phaseCount; dd++)
    {
      targetTypeDropDown[dd+1].m_dwFlags = 0;
      targetTypeDropDown[dd+1].m_lVal = dd+1;
      char *c = new char[phases[dd].length()+1];
      const LPCSTR s = strcpy(c, phases[dd].c_str());
      targetTypeDropDown[dd+1].m_pStr = (LPTSTR)s;          

      //const LPCSTR s = gs_MVDefn[i].Symbol();
      //m_DDSpeciesPartition[k].m_pStr = (LPTSTR)s;          
    }
  }

  DD.Text("");
  DD.Text("Target Type...");
  DD.Long("TargetType", "", idDX_MaterialType, MF_PARAM_STOPPED | MF_SET_ON_CHANGE, targetTypeDropDown);

  DD.Text("");
  DD.ObjectBegin("Mapping Enable", "MappingEnable");
  if (m_sMaterialType != 0)
  {
    if ((map != NULL)&&(mapText != NULL))
      for (int i=0; i<allCount; i++)
      {
        if (makeObject[i])
        {
          DD.CheckBox(gs_MVDefn[i].Symbol(), "", &enable[i], MF_PARAMETER); 
        }
      }
  }
  DD.ObjectEnd();

  DD.Page("Mappings");

  DD.Text("");

  if (m_sMaterialType != 0)
  {
    if ((map != NULL)&&(mapText != NULL))
      for (int i=0; i<allCount; i++)
      {
        if (makeObject[i])
        {
          DD.ObjectBegin("Mapping", gs_MVDefn[i].Symbol());
          if (enable[i])
          {
            for (int j=0; j<destCount; j++)
            {
              DD.Double(mapText[i][j].c_str(), "", &map[i][j], MF_PARAMETER, MC_Frac("%"));
            }
          }
          DD.ObjectEnd();
        }
      }
  }
}

//---------------------------------------------------------------------------

bool MaterialMerge::ExchangeDataFields()
{
  switch (DX.Handle)
  {
  case idDX_MaterialType: 
    if ((DX.HasReqdValue)&&(m_sMaterialType!=DX.Long))
    {
      m_sMaterialType=DX.Long;

      if (m_sMaterialType != 0)
      {
        // Deallocate old mapping and associated.
        if (destIndex != NULL)
          delete[] destIndex;

        if (destText != NULL)
          delete[] destText;

        if (map != NULL)
        {
          for (int i=0; i<allCount; i++)
            if (map[i] != NULL)
              delete[] map[i];
          delete[] map;
        }

        if (mapText != NULL)
        {
          for (int i=0; i<allCount; i++)
            if (mapText[i] != NULL)
              delete[] mapText[i];
          delete[] mapText;
        }

        if (makeObject != NULL)
          delete[] makeObject;

        if (enable != NULL)
          delete[] enable;

        if (totals != NULL)
          delete[] totals;

        // Allocate new mapping & associated.
        destCount = 0;
        for (int i = 0; i < allCount; i++)
          if (std::string(gs_MVDefn[i].Symbol()).find(("("+phases[m_sMaterialType-1]+")").c_str()) != std::string::npos)
            destCount++;

        destIndex = new int[destCount];
        destText = new std::string[destCount];

        {
          int dI = 0;
          for (int i = 0; i < allCount; i++)
            if (std::string(gs_MVDefn[i].Symbol()).find(("("+phases[m_sMaterialType-1]+")").c_str()) != std::string::npos)
            { 
              destIndex[dI] = i;
              destText[dI] = gs_MVDefn[i].Symbol();
              dI++;
            }
        }

        map = new double*[allCount];
        mapText = new std::string*[allCount];

        for (int i = 0; i < allCount; i++)
        {
          map[i] = new double[destCount];
          mapText[i] = new std::string[destCount];

          for (int j = 0; j < destCount; j++)
          {
            map[i][j] = 0.0;
            mapText[i][j] = "";
          }
        }

        makeObject = new bool[allCount];
        for (int i = 0; i < allCount; i++)
          makeObject[i] = false;

        enable = new bool[allCount];
        for (int i = 0; i < allCount; i++)
          enable[i] = false;

        totals = new double[allCount];
        for (int i = 0; i < allCount; i++)
          totals[i] = 0.0;


        // Fill mapping & associated.
        int dI = 0;
        for (int i=0; i<allCount; i++)
        {
          std::string symbol = gs_MVDefn[i].Symbol();

          if (gs_MVDefn[i].IsGas()) // is a gas, map to other -- will be handled specially in eval.
          {
            map[i][destCount-1] = 1.0;
          }

          else if (symbol.find(("("+phases[m_sMaterialType-1]+")").c_str()) != std::string::npos) // Is (IL1), null-mapping.
          {
            map[i][dI] = 1.0;
            dI++;
          }

          else // something else...
          {
            bool exactMatch = false;
            std::string assay = symbol.substr(0, symbol.find("("));
            for (int j=0; j<destCount; j++)
            {
              if (destText[j] == std::string(assay + ("("+phases[m_sMaterialType-1]+")").c_str())) // Exact assay match, set up mapping.
              {
                map[i][j] = 1.0;
                exactMatch = true;
              }
            }
            if (!exactMatch) // No exact match, jam into last entry (usually 'Other')...
            {
              map[i][destCount-1] = 1.0;
              makeObject[i] = true;
              for (int j=0; j<destCount; j++)
                mapText[i][j] = destText[j];
            }
          }
        }
      }
    }

    DX.Long=m_sMaterialType;

    return true;
  }
  return false;
}

//---------------------------------------------------------------------------
/*
;------------------------------------------------------
; Reusable Class logic for Feeder Units
; 1st Draft by I.Gough August 2004
;------------------------------------------------------

;================================================================================

Class Feeder
Str Unit
Str Content
Str In

Function InitTag(Str UnitTag)
Unit = UnitTag
Content = StrCat(Unit, ".Content.QM.")
Return 0
EndFunct

Function Ru_Zr_To_IL1(str Stream)

In = StrCat(Stream, ".Qi.")

SetDynTag(StrCat(Content, "TiO2(IL1)"),		GetDynTag(StrCat(In, "TiO2(Ru)")) + 	GetDynTag(StrCat(In, "TiO2(Zr)")))
SetDynTag(StrCat(Content, "Fe2O3(IL1)"),	GetDynTag(StrCat(In, "Fe2O3(Ru)")) + 	GetDynTag(StrCat(In, "Fe2O3(Zr)")))
SetDynTag(StrCat(Content, "ZrO2(IL1)"), 	GetDynTag(StrCat(In, "ZrO2(Ru)")) + 	GetDynTag(StrCat(In, "ZrO2(Zr)")))
SetDynTag(StrCat(Content, "S(IL1)"), 		GetDynTag(StrCat(In, "S(Ru)")) + 		GetDynTag(StrCat(In, "SO3(Zr)")) / 2.5)
SetDynTag(StrCat(Content, "Th(IL1)"), 		GetDynTag(StrCat(In, "Th(Ru)")) + 		GetDynTag(StrCat(In, "Th(Zr)")))
SetDynTag(StrCat(Content, "U(IL1)"), 		GetDynTag(StrCat(In, "U(Ru)")) + 		GetDynTag(StrCat(In, "U(Zr)")))
SetDynTag(StrCat(Content, "SiO2(IL1)"), 	GetDynTag(StrCat(In, "Other(Zr)"))+ 	GetDynTag(StrCat(In, "Quartz(Qu)")))
SetDynTag(StrCat(Content, "P2O5(IL1)"), 	GetDynTag(StrCat(In, "P2O5(Zr)")))
SetDynTag(StrCat(Content, "Al2O3(IL1)"),	GetDynTag(StrCat(In, "Al2O3(Zr)")))
SetDynTag(StrCat(Content, "Other(IL1)"), 	GetDynTag(StrCat(In, "Other(Ru)")) + 	GetDynTag(StrCat(In, "BaO(Zr)"))+	GetDynTag(StrCat(In, "SnO2(Ru)")))

Return 0
EndFunct

EndClass

Feeder	SR1_Ru_Zr
SR1_Ru_Zr.InitTag("SR1_Ru_Zr")
;--This is to manually convert all other minerals to Ilmenite1 (Monazite, etc with assay)
SetDynTag("SR1_MONAZITE.Qm_Rqd", GetDynTag("S30P031.Qi.Monazite(Mo)"))
SetDynTag("SR1_KYANITE.Qm_Rqd", GetDynTag("S30P031.Qi.Kyanite(Ky)"))
SetDynTag("SR1_STAUROLITE.Qm_Rqd", GetDynTag("S30P031.Qi.Staurolite(St)"))
SetDynTag("SR1_GARNET.Qm_Rqd", GetDynTag("S30P031.Qi.Garnet(Ga)"))
SetDynTag("SR1_SPARE1.Qm_Rqd", GetDynTag("S30P031.Qi.Spare1(s1)"))
SetDynTag("SR1_SPARE2.Qm_Rqd", GetDynTag("S30P031.Qi.Spare2(s2)"))
SetDynTag("SR1_SPARE3.Qm_Rqd", GetDynTag("S30P031.Qi.Spare3(s3)"))

;-- now set all the rutile, zircon, mag leucoxene, non-mag leucoxene as Ilmenite 1,
SR1_Ru_Zr.Ru_Zr_To_IL1("S30P031")

;NOTE: All the extra mass flow added above is corrected by stream going to SR2_MINERAL_BAL.
*/


//---------------------------------------------------------------------------

bool MaterialMerge::ValidateDataFields()
{//ensure parameters are within expected ranges
  if (m_sMaterialType != 0)
  {
    for (int i=0; i<allCount; i++)
    {
      totals[i] = 0.0;
      for (int j=0; j<destCount; j++)
        totals[i] += map[i][j];
    }

    for (int i=0; i<allCount; i++)
    {
      int dI = destCount-1;
      while ((totals[i]>1.0)&&(dI>=0))
      {
        if (map[i][dI]>0.0)
        {
          if (map[i][dI]>=totals[i]-1.0)
          {
            map[i][dI] -= totals[i]-1.0;
            totals[i] = 1.0;
          }
          else
          {
            totals[i] -= map[i][dI];
            map[i][dI] = 0.0;
          }
        }
        dI--;
      }

      if (totals[i]<1.0)
      {
        map[i][destCount-1] += 1.0 - totals[i];
      }
    }
  }

  return true; // true = ok.
}

//---------------------------------------------------------------------------

void MaterialMerge::EvalProducts()
{
  try
  {
    MStream QI; //initialise local empty copy of a stream
    FlwIOs.AddMixtureIn_Id(QI, idFeed); //sum of all feed streams
    const long index = FlwIOs.First[idProd];
    MStream & QO = FlwIOs[index].Stream; //get reference to the actual output stream

    QO = QI; //set output = input (copies all qualities, etc)

    if ((map != NULL)&&(mapText != NULL))
    {
      for (int i=0; i<allCount; i++)
      {
        if ((enable[i])&&(!gs_MVDefn[i].IsGas())) // isn't a gas, clear.
          QO.M[i] = 0.0;
      }

      for (int i=0; i<allCount; i++)
      {
        if ((enable[i])&&(!gs_MVDefn[i].IsGas())) // isn't a gas
        {
          for (int j=0; j<destCount; j++)
            if (map[i][j]>0.0)
              QO.M[destIndex[j]] += QI.M[i]*map[i][j];
        }
      }
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

//====================================================================================
