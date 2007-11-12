//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __FLOTATION_CELL_CPP
#include "Flotation Cell.h"
//#pragma optimize("", off)

//====================================================================================

const int maxSecondaries = 10;

enum { idFeed = 0,
		idTail,
		idConc,
		idAir,
		idVent };

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;			Id;			Rqd;Max;CnId,FracHgt;Options;
  { "Feed",           "Feed",			idFeed,		1,	10,	0,	1.0f,	MIO_In |MIO_Material },
  { "Tailings",       "Tailings",		idTail,		1,	1,	0,	1.0f,	MIO_Out|MIO_Material },
  { "Concentrate",    "Concentrate",	idConc,		1,	1,	0,	1.0f,	MIO_Out|MIO_Material },
  { "Air",			"Air",			idAir,		0,	5,	0,	1.0f,	MIO_In |MIO_Material },
  { "Vent",			"Vent",			idVent,		0,	1,	0,	1.0f,	MIO_Out|MIO_Material },
  { NULL },
  };

static double Drw_FlotCell[] = { 
  MDrw_Poly,  8.0,4.0, 8.0,-6.0, -8.0,-6.0, -8.0,4.0, 8.0,4.0,
  MDrw_Poly,  7.0,4.0, 7.0,6.0, -7.0,6.0, -7.0,4.0,
  MDrw_Poly,  8.0,2.5, 0.0,-6.0, -8.0,2.5,
  MDrw_TagPos, 0, -7.5,
  MDrw_End };

//---------------------------------------------------------------------------

#if defined(Separ1)
DEFINE_TRANSFER_UNIT_EX(FlotationCell, "FlotationCell", MDLLIBNAME)
#else
DEFINE_TRANSFER_UNIT(FlotationCell, "TestFlotationCell", DLL_GroupName)
#endif

void FlotationCell_UnitDef::GetOptions()
  {
  SetDefaultTag("FC");
  SetDrawing("Tank", Drw_FlotCell);
  #if defined(Separ1)
  SetTreeDescription("Separation:Flotation Cell");
  #else
  SetTreeDescription("Demo:Flotation Cell");
  #endif
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow/*|MSolveMode_DynamicFull*/);
  SetModelGroup(MGroup_General);
  SetModelLicense(MLicense_Standard);
  };

//---------------------------------------------------------------------------

FlotationCell::FlotationCell(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
  //default values...
	eSpecType = FTST_ByCompound;
	nPrimary1 = nPrimary2 = -1;
	dPrimaryRecovery = dPrimaryGrade = 0.0;
	dReqPrimaryRecovery = dReqPrimaryGrade = 0.0;

	dReqWaterFrac = 0.0;
	dSfConcentrate = dSfTailings = 0.0;

	m_bOn = true;
}

//---------------------------------------------------------------------------

static	vector<MDDValueLst>	gs_vSolidElements;


void FlotationCell::Init()
{
	SetIODefinition(s_IODefs);

	if (gs_vSolidElements.size() == 0)
	{
		MDDValueLst blank = {-1, "-"};
		gs_vSolidElements.push_back(blank);
		vector<CString> temp;
		bool* elementSolid = new bool[gs_PeriodicTable.Count() + 1];
		memset(elementSolid, 0, sizeof(bool) * gs_PeriodicTable.Count() + 1);

		for (int i = 0; i < gs_MVDefn.Count(); i++)
			if (gs_MVDefn[i].IsSolid())
			{
				MSpecieElements curElements = gs_MVDefn[i].Elements();
				for (int j = 0; j < curElements.Count(); j++)
					elementSolid[curElements[j].Element().AtomicNumber()] = true;
			}
		for (int i = 0; i < gs_PeriodicTable.Count() + 1; i++)
			if (elementSolid[i])
			{
				int dstSize = strlen(gs_PeriodicTable[i - 1].Symbol()) + 1;
				char* nonConst = new char[dstSize]; //It'll be a memory leak on the order of 300 bytes per model, maximum...
				//strcpy_s(nonConst, dstSize, gs_PeriodicTable[i - 1].Symbol());
				strcpy(nonConst, gs_PeriodicTable[i - 1].Symbol());
				MDDValueLst cur = { i, nonConst };
				gs_vSolidElements.push_back(cur);
			}
		MDDValueLst term = {0};
		gs_vSolidElements.push_back(term);
	}
	vPrimaryIndices.resize(1, gs_MVDefn.DDSolSpList()[0].m_lVal);
}

//---------------------------------------------------------------------------

enum {	idDX_PrimaryElement1 = 0,
		idDX_PrimaryElement2,
		idDX_PrimaryCompound,
		idDX_SecondaryCount,
		idDX_SpecType,
		idDX_PrimaryCompoundStr,
		idDX_PrimariesFound,
		idDX_Unspecified,};

const int idDX_SecondaryName = idDX_Unspecified + 1;
const int idDX_SecondaryIndex = 10 + maxSecondaries;
const int idDX_SecondaryReqRecovery = 10 + 2*maxSecondaries;
const int idDX_SecondaryRecovery = 10 + 3*maxSecondaries;

void FlotationCell::BuildDataFields()
{
	static MDDValueLst DDB1[] ={
		{ FTST_ByCompound, "Compound" },
		{ FTST_ByElement, "Element" },
		{ 0 }};

	DD.CheckBox("On", "", &m_bOn, MF_PARAMETER);

	DD.Text("Requirements:");
	DD.Text("Primary Compound:");
	DD.Long("Method", "", idDX_SpecType, MF_PARAM_STOPPED | MF_SET_ON_CHANGE, DDB1);
	switch (eSpecType)
	{
	case FTST_ByCompound:
		DD.Long("Primary_Compound", "", idDX_PrimaryCompound, MF_PARAM_STOPPED | MF_NO_FILING | MF_SET_ON_CHANGE, gs_MVDefn.DDSolSpList());
		DD.String("PrimaryStr", "", idDX_PrimaryCompoundStr, MF_PARAM_STOPPED | MF_NO_VIEW); //This allows filing, and automatic setting of this parameter.
		break;
	case FTST_ByElement:
		DD.Long("Primary_Element", "", idDX_PrimaryElement1, MF_PARAM_STOPPED, &gs_vSolidElements.at(0));
		DD.Long("Secondary_Element", "", idDX_PrimaryElement2, MF_PARAM_STOPPED, &gs_vSolidElements.at(0));
	}
	DD.Double("Primary_Recovery", "", &dReqPrimaryRecovery, MF_PARAMETER, MC_Frac("%"));
	DD.Double("Primary_Grade", "", &dReqPrimaryGrade, MF_PARAMETER, MC_Frac("%"));
	DD.Text("Liquid Split:");
	DD.Double("Liquid_to_Concentrate", "", &dReqWaterFrac, MF_PARAMETER, MC_Frac("%"));
	DD.Text("Additional Compounds to Concentrate:");
	DD.Long("Number_of_Compounds", "", idDX_SecondaryCount, MF_PARAM_STOPPED | MF_SET_ON_CHANGE);
	if (vSecondaryIndices.size() > 0)
	{
		DD.ArrayBegin("Additional_Compounds", "Additional_Compounds", vSecondaryIndices.size());
		for (unsigned int i = 0; i < vSecondaryIndices.size(); i++)
		{
			DD.ArrayElementStart(i);
			DD.Long("Compound", "", idDX_SecondaryIndex + i, MF_PARAM_STOPPED | MF_NO_FILING, gs_MVDefn.DDSolSpList());
			DD.String("CompoundStr", "", idDX_SecondaryName + i, MF_PARAM_STOPPED | MF_NO_VIEW);
			DD.Double("Recovery", "", idDX_SecondaryReqRecovery + i, MF_PARAMETER, MC_Frac("%"));
			DD.ArrayElementEnd();
		}
		DD.ArrayEnd();
	}
	DD.Text("");
	DD.Text("Results:");
	DD.String("Primaries_Found", "", idDX_PrimariesFound, MF_RESULT | MF_NO_FILING);
	DD.Double("Actual_Recovery", "", &dPrimaryRecovery, MF_RESULT, MC_Frac);
	DD.Double("Actual_Grade", "", &dPrimaryGrade, MF_RESULT, MC_Frac);
	DD.Text("");
	DD.String("Gangue_Compounds", "", idDX_Unspecified, MF_RESULT | MF_NO_FILING);
	DD.Text("");
	DD.Double("Sf_Concentrate", "", &dSfConcentrate, MF_RESULT, MC_Frac);
	DD.Double("Sf_Tailings", "", &dSfTailings, MF_RESULT, MC_Frac);
	if (m_sPrestartErrors.GetLength() > 0)
	{
		DD.Text("");
		DD.Text("Prestart Errors:");
		DD.Text(m_sPrestartErrors);
	}
}

//---------------------------------------------------------------------------

bool FlotationCell::ExchangeDataFields()
{
	switch (DX.Handle)
	{
	case idDX_SpecType:
		if (DX.HasReqdValue)
		{
			eSpecType = (FT_SpecType) DX.Long;
			switch (eSpecType)
			{
			case FTST_ByCompound:
				vPrimaryIndices.resize(1,gs_MVDefn.DDSolSpList()[0].m_lVal);
				break;
			case FTST_ByElement:
				UpdatePrimaryIndices();
			}
		}
		DX.Long = eSpecType;
		return true;
	case idDX_PrimaryElement1:
		if (DX.HasReqdValue)
		{
			nPrimary1 = DX.Long;
			UpdatePrimaryIndices();
		}
		DX.Long = nPrimary1;
		return true;
	case idDX_PrimaryElement2:
		if (DX.HasReqdValue)
		{
			nPrimary2 = DX.Long;
			UpdatePrimaryIndices();
		}
		DX.Long = nPrimary2;
		return true;
	case idDX_PrimaryCompoundStr:
		if (DX.HasReqdValue)
		{
			const int tmp = gs_MVDefn.LookupNX(DX.String);
			if (tmp != -1 && gs_MVDefn[tmp].IsSolid())
			{
				vPrimaryIndices.at(0) = tmp;
				UpdatePrimaryIndices();
			}
			else
				Log.Message(MMsg_Warning, "Primary Compound (%s) not found in specie database or not solid", DX.String);  
		}
		DX.String = gs_MVDefn[vPrimaryIndices.at(0)].Symbol();
		return true;
	case idDX_PrimaryCompound:
		if (DX.HasReqdValue)
		{
			vPrimaryIndices.at(0) = DX.Long;
			UpdatePrimaryIndices();
		}
		DX.Long = vPrimaryIndices.at(0);
		return true;
	case idDX_SecondaryCount:
		if (DX.HasReqdValue)
			SetSecondaryCount(DX.Long);
		DX.Long = vSecondaryIndices.size();
		return true;
	case idDX_PrimariesFound:
		sPrimariesFound.Empty();
		for (unsigned int i = 0; i < vPrimaryIndices.size(); i++)
			sPrimariesFound += gs_MVDefn[vPrimaryIndices.at(i)].Symbol() + (CString) ",";
		DX.String = sPrimariesFound;
		return true;
	case idDX_Unspecified:
		sOthersFound.Empty();
		for (unsigned int i = 0; i < vOtherIndices.size(); i++)
			sOthersFound += gs_MVDefn[vOtherIndices.at(i)].Symbol() + (CString) ",";
		DX.String = sOthersFound;
		return true; 
			
	}
	if (idDX_SecondaryName <= DX.Handle && DX.Handle < idDX_SecondaryName + vSecondaryIndices.size())
	{
		const int index = DX.Handle - idDX_SecondaryName;
		if (DX.HasReqdValue)
		{
			const int tmp = gs_MVDefn.LookupNX(DX.String);
			if (tmp != -1 && gs_MVDefn[tmp].IsSolid())
			{
				vSecondaryIndices.at(index) = tmp;
				UpdateOtherIndices();
				//UpdateMDDLists();
			}
			else
				Log.Message(MMsg_Warning, "Secondary Compound (%s) not found in specie database or not solid", DX.String);  
		}
		DX.String = gs_MVDefn[vSecondaryIndices.at(index)].Symbol();
		return true;
	}
	if (idDX_SecondaryIndex <= DX.Handle && DX.Handle < idDX_SecondaryIndex + vSecondaryIndices.size())
	{
		const int index = DX.Handle - idDX_SecondaryIndex;
		if (DX.HasReqdValue)
		{
			vSecondaryIndices.at(index) = DX.Long;
			UpdateOtherIndices();
			//UpdateMDDLists();
		}
		DX.Long = vSecondaryIndices.at(index);
		return true;
	}
	if (idDX_SecondaryReqRecovery <= DX.Handle && DX.Handle <idDX_SecondaryReqRecovery + vSecondaryIndices.size())
	{
		const int index = DX.Handle - idDX_SecondaryReqRecovery;
		if (DX.HasReqdValue)
			vReqSecondaryRecoveries.at(index) = DX.Double;
		DX.Double = vReqSecondaryRecoveries.at(index);
		return true;
	}
	if (idDX_SecondaryRecovery <= DX.Handle && DX.Handle < idDX_SecondaryRecovery + vSecondaryIndices.size())
	{
		const int index = DX.Handle - idDX_SecondaryRecovery;
		if (DX.HasReqdValue)
			vSecondaryRecoveries.at(index) = DX.Double;
		DX.Double = vSecondaryRecoveries.at(index);
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------

bool FlotationCell::PreStartCheck()
{
	bool ret = true;
	m_sPrestartErrors = "";
	if (!FlwIOs.getCount(idFeed) || !FlwIOs.getCount(idTail) || !FlwIOs.getCount(idConc))
	{
		m_sPrestartErrors.Append("Flows not connected; ");
		ret = false;
	}
	if (vPrimaryIndices.size() == 0)
	{
		m_sPrestartErrors.Append("No primary compounds; ");
		ret = false;
	}
	for (unsigned int i = 0; i < vPrimaryIndices.size(); i++)
		for (unsigned int j = 0; j < vSecondaryIndices.size(); j++)
			if (vPrimaryIndices.at(i) == vSecondaryIndices.at(j))
			{
				m_sPrestartErrors.Append("Additional specie is also primary specie; ");
				ret = false;
				break;
			}
	for (unsigned int i = 0; i < vSecondaryIndices.size(); i++)
		for (unsigned int j = 0; j < vSecondaryIndices.size(); j++)
			if (i != j && vSecondaryIndices.at(i) == vSecondaryIndices.at(j))
			{
				m_sPrestartErrors.Append("Additional specie listed twice; ");
				ret = false;
				break;
			}
	return ret;
}

//---------------------------------------------------------------------------

bool FlotationCell::ValidateDataFields()
{
	dReqPrimaryRecovery = Range(0.0, dReqPrimaryRecovery, 1.);
	dReqPrimaryGrade = Range(ZeroLimit, dReqPrimaryGrade, 1.);
	for (unsigned int i = 0; i < vSecondaryIndices.size(); i++)
		vReqSecondaryRecoveries.at(i) = Range(0., vReqSecondaryRecoveries.at(i), 1.);
	dReqWaterFrac = Range(0., dReqWaterFrac, 1.);
	return true;
}

//---------------------------------------------------------------------------

enum {LC_BadInput, LC_BadAir, LC_LittleAir, LC_NoPrimaries, LC_NoGrade, LC_Vent};

void FlotationCell::EvalProducts()
{
	try
	{
		dPrimaryRecovery = dReqPrimaryRecovery;
		dPrimaryGrade = dReqPrimaryGrade;
		for (unsigned int i = 0; i < vSecondaryRecoveries.size(); i++)
			vSecondaryRecoveries.at(i) = vReqSecondaryRecoveries.at(i);

		//sum all input streams into a working copy
		MStreamI QI;
		FlwIOs.AddMixtureIn_Id(QI, idFeed);
		MStreamI QAir;
		FlwIOs.AddMixtureIn_Id(QAir, idAir);
		bool bAirOn = FlwIOs.Count[idAir] > 0;

		//get handles to output streams...
		MStream & QOT = FlwIOs[FlwIOs.First[idTail]].Stream;
		MStream & QOC = FlwIOs[FlwIOs.First[idConc]].Stream;
		MStream & QOV = FlwIOs[FlwIOs.First[idVent]].Stream;

		QOT = QI;
		QOT.ZeroMass();
		QOC = QI;
		QOC.ZeroMass();

		if (!m_bOn)
		{
			QOT = QI;
			QOV = QAir;
			dPrimaryRecovery = dPrimaryGrade = 0;
			for (vector<double>::iterator it = vSecondaryRecoveries.begin(); it != vSecondaryRecoveries.end(); it++)
				*it = 0.0;
			return;
		}
		if (QI.Mass(MP_Sol) > QI.Mass(MP_Liq))
			Log.SetCondition(true, LC_BadInput, MMsg_Warning, "Input stream contains more solid than liquid.");

		if (bAirOn && (QAir.Mass(MP_Sol) > UsableMass || QAir.Mass(MP_Liq) > UsableMass))
			Log.SetCondition(true, LC_BadAir, MMsg_Warning, "Air stream contains liquids or solids");
		if (bAirOn && QAir.Volume(MP_Gas) < 2 * QI.Volume(MP_Sol))
			Log.SetCondition(true, LC_LittleAir, MMsg_Warning, "Air stream contains very little gas");

		QI.AddF(QAir, MP_All, 1);

		//do the work...
		const int NumSpecies = gs_MVDefn.Count();

		//Primaries:
		double dPrimaryMassConc = 0.0;
		double dPrimaryElementConc = 0.0;
		for (unsigned int i = 0; i < vPrimaryIndices.size(); i++)
		{
			double temp = QI.M[vPrimaryIndices.at(i)];
			dPrimaryMassConc += temp * dReqPrimaryRecovery;
			if (eSpecType == FTST_ByElement)
				dPrimaryElementConc += temp * dPrimaryRecovery * ElementMassFrac(vPrimaryIndices.at(i), nPrimary1);
			else
				dPrimaryElementConc += temp * dPrimaryRecovery;

			QOT.M[vPrimaryIndices.at(i)] = temp * (1 - dReqPrimaryRecovery);
			QOC.M[vPrimaryIndices.at(i)] = temp * dReqPrimaryRecovery;
		}

		if (dPrimaryElementConc == 0)
		{
			dPrimaryRecovery = dNAN;
			Log.SetCondition(true, LC_NoPrimaries, MMsg_Warning, "No primary compounds found in feed.");
		}

		//Secondaries:
		double dSecondaryMassConc = 0.0;
		for (unsigned int i = 0; i < vSecondaryIndices.size(); i++)
		{
			double temp = QI.M[vSecondaryIndices.at(i)];
			dSecondaryMassConc += temp * vReqSecondaryRecoveries.at(i);
			QOT.M[vSecondaryIndices.at(i)] = temp * (1 - vReqSecondaryRecoveries.at(i));
			QOC.M[vSecondaryIndices.at(i)] = temp * vReqSecondaryRecoveries.at(i);
			if (eSpecType == FTST_ByElement)
				dPrimaryElementConc += temp * vReqSecondaryRecoveries.at(i) * ElementMassFrac(vSecondaryIndices.at(i), nPrimary1);
		}

		//Those not included in either primaries or secondaries.
		double dPrimaryElementInGangue = 0;
		double dOtherMassIn = 0.0;
		for (unsigned int i = 0; i < vOtherIndices.size(); i++)
		{
			dOtherMassIn += QI.M[vOtherIndices.at(i)];
			if (eSpecType == FTST_ByElement)
				dPrimaryElementInGangue += QI.M[vOtherIndices.at(i)] * ElementMassFrac(vOtherIndices.at(i), nPrimary1);
		}
		double dPrimaryGangueFrac = dPrimaryElementInGangue / NZ(dOtherMassIn);

		double dReqOtherMassOut = (dPrimaryElementConc - dReqPrimaryGrade * (dPrimaryMassConc + dSecondaryMassConc)) / NZ(dReqPrimaryGrade - dPrimaryGangueFrac);
		if (dReqOtherMassOut < 0) //If this is the case, we will put in no more impurities.
		{
			if (dPrimaryElementConc / QOC.Mass(MP_Sol) < dReqPrimaryGrade)
			{
				Log.SetCondition(true, LC_NoGrade, MMsg_Warning, "Secondary products prevent required primary grade");
				dReqOtherMassOut = 0;
			}
			else
			{
				Log.SetCondition(true, LC_NoGrade, MMsg_Warning, "Grade of gangue to concentrate higher than required grade");
				if (dPrimaryElementConc / QOC.Mass(MP_Sol) > dPrimaryGangueFrac) //We will get as close as possible...
					dReqOtherMassOut = dOtherMassIn;
			}
		}
		if (dReqOtherMassOut > dOtherMassIn)
		{
			Log.SetCondition(true, LC_NoGrade, MMsg_Warning, "Not enough unspecified input mass to achieve required primary grade");
			dReqOtherMassOut = dOtherMassIn;
		}
		for (unsigned int i = 0; i < vOtherIndices.size(); i++)
		{
			QOC.M[vOtherIndices.at(i)] = QI.M[vOtherIndices.at(i)] * dReqOtherMassOut / NZ(dOtherMassIn);
			QOT.M[vOtherIndices.at(i)] = QI.M[vOtherIndices.at(i)] * (1 - dReqOtherMassOut / NZ(dOtherMassIn));
		}
		dPrimaryElementConc += dReqOtherMassOut * dPrimaryGangueFrac;

		//Finally: We deal with non-solid species:
		/*for (int i = 0; i < gs_MVDefn.Count(); i++)
		{
			if (gs_MVDefn[i].IsSolid())
				continue;
			QOC.M[i] = QI.M[i] * dReqWaterFrac;
			QOT.M[i] = QI.M[i] * (1 - dReqWaterFrac);
		}*/
		QOC.AddM(QI, MP_Liq, QI.Mass(MP_Liq) * dReqWaterFrac);
		QOT.AddM(QI, MP_Liq, QI.Mass(MP_Liq) * (1 - dReqWaterFrac));
		if (FlwIOs.Count[idVent] > 0)
		{
			QOV = QI;
			QOV.ZeroMass();
			QOV.AddM(QI, MP_Gas, QI.Mass(MP_Gas));
		}
		else
		{
			QOC.AddM(QI, MP_Gas, QI.Mass(MP_Gas));
			if (FlwIOs.Count[idAir] > 0)
				Log.SetCondition(true, LC_Vent, MMsg_Warning, "Stream connected to air, but no stream connected to vent.");
		}

		if (QOC.Mass(MP_Sol) > 0)
			dPrimaryGrade = dPrimaryElementConc / NZ(QOC.Mass(MP_Sol));
		else
			dPrimaryGrade = dNAN;
		dSfConcentrate = QOC.MassFrac(MP_Sol);
		dSfTailings = QOT.MassFrac(MP_Sol);
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

//This will recalculate the primary indices if the mode is set to ByElement. It will then call UpdateMDDLists.
void FlotationCell::UpdatePrimaryIndices()
{
	if (eSpecType == FTST_ByElement)
	{
		vPrimaryIndices.clear();
		int numSpecies = gs_MVDefn.Count();
		if (nPrimary1 >= 0) //If nPrimary1 is set to '-', we have no primaries.
			for (int i = 0; i < numSpecies; i++)
				if (gs_MVDefn[i].IsSolid())
				{
					MSpecieElements elems = gs_MVDefn[i].Elements();
					bool bContainsP1 = nPrimary1 < 0;
					bool bContainsP2 = nPrimary2 < 0;	//If the user has selected "-", we discount it

					for (int j = 0; j < elems.Count(); j++)
					{
						if (elems[j].Element().AtomicNumber() == nPrimary1)
							bContainsP1 = true;
						if (elems[j].Element().AtomicNumber() == nPrimary2)
							bContainsP2 = true;
					}
					if (bContainsP1 && bContainsP2)
						vPrimaryIndices.push_back(i);
				}
	}
	//UpdateMDDLists();
	UpdateOtherIndices();
}

//====================================================================================

//This will update the MDD list for each secondary.
//The rules are fairly simple - it's a list of all solids, but some will be unselectable:
//If they are a primary, or if they are a different secondary.
//The MDD lists will be created in SetSecondaryCount, so we will merely set flags at this point.
//This is somewhat inefficent - the number of iterations is n^2 * c, where n is the secondary count and c is the element count.
/*void FlotationCell::UpdateMDDLists()
{
	MDDValueLst Zero = {0};
	int i = 0;
	for (vector<vector<MDDValueLst>>::iterator lst = vSecondaryMDDLists.begin();
		lst != vSecondaryMDDLists.end(); lst++)
	{
		lst->clear();
		for (MDDValueLst* p = gs_MVDefn.DDSolSpList(); p->m_pStr != 0; p++)
			lst->push_back(*p);
		lst->push_back(Zero);

		for (vector<MDDValueLst>::iterator it = lst->begin(); it->m_pStr != 0/; it++)
		{
			bool removed = false;
			for (unsigned int k = 0; k < vPrimaryIndices.size(); k++) //Check against primaries.
				if (it->m_lVal == vPrimaryIndices.at(k))
				{
					removed = true;
					lst->erase(it);
					break;
				}
			if (!removed)
				for (unsigned int k = 0; k < vSecondaryIndices.size(); k++) //Check against secondaries
				{
					if (i = k) continue; //Don't check it against itself...
					if (it->m_lVal == vSecondaryIndices.at(k))
					{
						lst->erase(it);
						break;
					}
				}
		}
		i++;
	}
}*/

//---------------------------------------------------------------------------

void FlotationCell::SetSecondaryCount(int newSize)
{
	if (newSize < 0) newSize = 0;
	if (newSize > maxSecondaries) newSize = maxSecondaries;
	
	if (newSize <= vSecondaryIndices.size()) //Easy - just delete things. 'cept we need to do memory management...
	{
		vSecondaryIndices.resize(newSize);
		//vSecondaryMDDLists.resize(newSize);
		vSecondaryRecoveries.resize(newSize);
		vReqSecondaryRecoveries.resize(newSize);
	}
	else
	{
		int i;
		for (i = 0; i < gs_MVDefn.Count() && !gs_MVDefn[i].IsSolid(); i++);
		if (i == gs_MVDefn.Count())
			i = 0;
		vSecondaryIndices.resize(newSize, i);
		/*vector<MDDValueLst> MDDVector;
		if (vSecondaryMDDLists.size() > 0)
			MDDVector = vSecondaryMDDLists.at(0);
		else
		{
			int i = 0;
			do
			{
				MDDVector.push_back(gs_MVDefn.DDSolSpList()[i]);
			}
			while (gs_MVDefn.DDSolSpList()[i++].m_pStr != 0);
		}
		vSecondaryMDDLists.resize(newSize, MDDVector);*/
		vSecondaryRecoveries.resize(newSize, 0.0); //Maybe this shouldn't be zero. Something to think about...
		vReqSecondaryRecoveries.resize(newSize, 0.0);
	}
	//UpdateMDDLists();
	UpdateOtherIndices();
}

//---------------------------------------------------------------------------

void FlotationCell::UpdateOtherIndices()
{
	vOtherIndices.clear();
	for (int i = 0; i < gs_MVDefn.Count(); i++)
		if (gs_MVDefn[i].IsSolid())
		{
			bool other = true;
			for (unsigned int j = 0; j < vPrimaryIndices.size() && other; j++)
				if (vPrimaryIndices.at(j) == i)
					other = false;
			for (unsigned int j = 0; j < vSecondaryIndices.size() && other; j++)
				if (vSecondaryIndices.at(j) == i)
					other = false;
			if (other)
				vOtherIndices.push_back(i);
		}
}

//---------------------------------------------------------------------------

double FlotationCell::ElementMassFrac(int compound, int element)
{
	double dElementMass = gs_PeriodicTable.ByAtmNo(element).AtomicWt();
	double molWt = gs_MVDefn[compound].MolecularWt();
	double ret = 0;
	MSpecieElements curElements = gs_MVDefn[compound].Elements();
	for (int curElement = 0; curElement < curElements.Count(); curElement++)
		if (curElements[curElement].Element().AtomicNumber() == element)
		{
			double amount = curElements[curElement].Amount();
			ret += amount * dElementMass / molWt;
		}
	return ret;
}

//---------------------------------------------------------------------------
