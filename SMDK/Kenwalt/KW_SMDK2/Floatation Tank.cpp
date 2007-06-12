//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __FLOATATION_TANK_CPP
#include "Floatation Tank.h"
#pragma optimize("", off)

//====================================================================================

const int maxSecondaries = 10;

const long idFeed = 0;
const long idTail = 1;
const long idConc = 2;

static MInOutDefStruct s_IODefs[]=
  {
  //  Desc;             Name;			Id;    Rqd; Max; CnId, FracHgt;  Options;
    { "Feed",           "Feed",			idFeed,  1,  10,    0,    1.0f,  MIO_In |MIO_Material },
    { "Tailings",       "Tailings",		idTail,  1,   1,    0,    1.0f,  MIO_Out|MIO_Material },
    { "Concentrate",    "Concentrate",	idConc,  1,   4,    0,    1.0f,  MIO_Out|MIO_Material },
    { NULL },
  };

static double Drw_FloatTank[] = { MDrw_Poly,  -2.,2.,  2.,2.,  2.,-2., -2.,-2., -2.,2.,
MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(FloatationTank, "Floatation Tank", DLL_GroupName)
void FloatationTank_UnitDef::GetOptions()
  {
  SetDefaultTag("SP");
  SetDrawing("Tank", Drw_FloatTank);
  SetTreeDescription("Demo:Floatation Tank");
  SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
  SetModelGroup(MGroup_General);
  };

//---------------------------------------------------------------------------

FloatationTank::FloatationTank(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
  //default values...
	eSpecType = FTST_ByCompound;
	vPrimaryIndices.resize(1, 0);
	nPrimary1 = nPrimary2 = -1;
	dPrimaryRecovery = dPrimaryGrade = 0.0;
	dReqPrimaryRecovery = dReqPrimaryGrade = 0.0;

	dWaterFrac = dReqWaterFrac = 0.0;
}

//---------------------------------------------------------------------------

static	vector<MDDValueLst>	gs_vSolidElements;


void FloatationTank::Init()
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
}

//---------------------------------------------------------------------------

enum {	idDX_PrimaryElement1 = 0,
		idDX_PrimaryElement2,
		idDX_PrimaryCompound,
		idDX_SecondaryCount,
		idDX_SpecType,
		idDX_PrimaryCompoundStr,
		idDX_PrimariesFound,
		idDX_Unspecified };

const int idDX_SecondaryName = 10;
const int idDX_SecondaryIndex = 10 + maxSecondaries;
const int idDX_SecondaryReqRecovery = 10 + 2*maxSecondaries;
const int idDX_SecondaryRecovery = 10 + 3*maxSecondaries;

void FloatationTank::BuildDataFields()
{
	static MDDValueLst DDB1[] ={
		{ FTST_ByCompound, "Compound" },
		{ FTST_ByElement, "Element" },
		{ 0 }};

	DD.Text("Requirements...");
	DD.Long("Specify_By", "", idDX_SpecType, MF_PARAM_STOPPED | MF_SET_ON_CHANGE, DDB1);
	switch (eSpecType)
	{
	case FTST_ByCompound:
		DD.Long("Primary", "", idDX_PrimaryCompound, MF_PARAM_STOPPED | MF_NO_FILING | MF_SET_ON_CHANGE, gs_MVDefn.DDSolSpList());
		DD.String("PrimaryStr", "", idDX_PrimaryCompoundStr, MF_PARAM_STOPPED | MF_NO_VIEW); //This allows filing, and automatic setting of this parameter.
		break;
	case FTST_ByElement:
		DD.Long("Primary1", "", idDX_PrimaryElement1, MF_PARAM_STOPPED, &gs_vSolidElements.at(0));
		DD.Long("Primary2", "", idDX_PrimaryElement2, MF_PARAM_STOPPED, &gs_vSolidElements.at(0));
	}
	DD.Double("Primary_Recovery", "", &dReqPrimaryRecovery, MF_PARAMETER, MC_Frac("%"));
	DD.Double("Primary_Grade", "", &dReqPrimaryGrade, MF_PARAMETER, MC_Frac("%"));
	DD.Double("Water_To_Concentrate", "", &dReqWaterFrac, MF_PARAMETER, MC_Frac("%"));
	DD.Long("Secondary_Count", "", idDX_SecondaryCount, MF_PARAM_STOPPED | MF_SET_ON_CHANGE);
	if (vSecondaryIndices.size() > 0)
	{
		DD.ArrayBegin("Secondaries", "Secondaries", vSecondaryIndices.size());
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
	DD.Text("Results...");
	DD.String("Primaries_Found", "", idDX_PrimariesFound, MF_RESULT | MF_NO_FILING);
	DD.String("Unspecified_Compounds", "", idDX_Unspecified, MF_RESULT | MF_NO_FILING);
	DD.Double("Actual_Grade", "", &dPrimaryGrade, MF_RESULT, MC_Frac);
}

//---------------------------------------------------------------------------

bool FloatationTank::ExchangeDataFields()
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
				vPrimaryIndices.resize(1, 0);
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

bool FloatationTank::PreStartCheck()
{
	if (!FlwIOs.getCount(idFeed) || !FlwIOs.getCount(idTail) || !FlwIOs.getCount(idConc))
		return false;
	if (vPrimaryIndices.size() == 0)
		return false;
	for (unsigned int i = 0; i < vPrimaryIndices.size(); i++)
		for (unsigned int j = 0; j < vSecondaryIndices.size(); j++)
			if (vPrimaryIndices.at(i) == vSecondaryIndices.at(j))
				return false;
	for (unsigned int i = 0; i < vSecondaryIndices.size(); i++)
		for (unsigned int j = 0; j < vSecondaryIndices.size(); j++)
			if (i != j && vSecondaryIndices.at(i) == vSecondaryIndices.at(j))
				return false;
	return true;
}

//---------------------------------------------------------------------------

bool FloatationTank::ValidateDataFields()
{
	dReqPrimaryRecovery = Range(0.0, dReqPrimaryRecovery, 1.);
	dReqPrimaryGrade = Range(ZeroLimit, dReqPrimaryGrade, 1.);
	for (unsigned int i = 0; i < vSecondaryIndices.size(); i++)
		vReqSecondaryRecoveries.at(i) = Range(0., vReqSecondaryRecoveries.at(i), 1.);
	dReqWaterFrac = Range(0., dReqWaterFrac, 1.);
	return true;
}

//---------------------------------------------------------------------------

void FloatationTank::EvalProducts()
{
	try
	{
		dPrimaryRecovery = dReqPrimaryRecovery;
		dPrimaryGrade = dReqPrimaryGrade;
		dWaterFrac = dReqWaterFrac;
		for (unsigned int i = 0; i < vSecondaryRecoveries.size(); i++)
			vSecondaryRecoveries.at(i) = vReqSecondaryRecoveries.at(i);

		//sum all input streams into a working copy
		MStream QI;
		FlwIOs.AddMixtureIn_Id(QI, idFeed);

		//get handles to input and output streams...
		MStream & QOT = FlwIOs[FlwIOs.First[idTail]].Stream;
		MStream & QOC = FlwIOs[FlwIOs.First[idConc]].Stream;

		// Always initialise the outputs as a copy of the inputs. This ensures all "qualities" are copied.
		QOT = QI;
		QOT.ZeroMass();
		QOC = QI;
		QOC.ZeroMass();

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
			{
				MSpecieElements curElements = gs_MVDefn[vPrimaryIndices.at(i)].Elements();
				int primaryElement = 0;
				while (primaryElement < curElements.Count() && curElements[primaryElement].Element().AtomicNumber() != nPrimary1)
				{
					int debug1 = curElements[primaryElement].Element().AtomicNumber();
					LPCSTR debug2 = curElements[primaryElement].Element().Symbol();
					primaryElement++;
					
				}
				if (primaryElement < curElements.Count())
					dPrimaryElementConc = temp * dReqPrimaryRecovery * curElements[primaryElement].Amount() * gs_PeriodicTable[nPrimary1 - 1].AtomicWt() / gs_MVDefn[i].MolecularWt();
			}
			else
				dPrimaryElementConc += temp * dPrimaryRecovery;

			QOT.M[vPrimaryIndices.at(i)] = temp * (1 - dReqPrimaryRecovery);
			QOC.M[vPrimaryIndices.at(i)] = temp * dReqPrimaryRecovery;
		}

		//Secondaries:
		double dSecondaryMassConc = 0.0;
		for (unsigned int i = 0; i < vSecondaryIndices.size(); i++)
		{
			double temp = QI.M[vSecondaryIndices.at(i)];
			dSecondaryMassConc += temp * vReqSecondaryRecoveries.at(i);
			QOT.M[vSecondaryIndices.at(i)] = temp * (1 - vReqSecondaryRecoveries.at(i));
			QOC.M[vSecondaryIndices.at(i)] = temp * vReqSecondaryRecoveries.at(i);
		}

		//Those not included in either primaries or secondaries.
		double dOtherMassIn = 0.0;
		for (unsigned int i = 0; i < vOtherIndices.size(); i++)
			dOtherMassIn += QI.M[vOtherIndices.at(i)];

		int liNoGrade = 0;
		double dTotalReqImpurities = dPrimaryElementConc * 1.0 / dReqPrimaryGrade - dPrimaryMassConc;
		if (dTotalReqImpurities < dSecondaryMassConc) //If this is the case, we will put in no more impurities.
		{
			Log.SetCondition(true, liNoGrade, MMsg_Warning, "Secondary products prevent required primary grade");
			dPrimaryGrade = dPrimaryMassConc / (dPrimaryMassConc + dSecondaryMassConc);
		}
		else
		{
			double dReqOtherMassOut = dTotalReqImpurities - dSecondaryMassConc;
			if (dReqOtherMassOut > dOtherMassIn)
			{
				Log.SetCondition(true, liNoGrade, MMsg_Warning, "Not enough unspecified input mass to achieve required primary grade");
				dReqOtherMassOut = dOtherMassIn;
			}
			for (unsigned int i = 0; i < vOtherIndices.size(); i++)
			{
				QOC.M[vOtherIndices.at(i)] = QI.M[vOtherIndices.at(i)] * dReqOtherMassOut / dOtherMassIn;
				QOT.M[vOtherIndices.at(i)] = QI.M[vOtherIndices.at(i)] * (1 - dReqOtherMassOut / dOtherMassIn);
			}
		}

		//Finally: We deal with non-solid species:
		for (int i = 0; i < gs_MVDefn.Count(); i++)
		{
			if (gs_MVDefn[i].IsSolid())
				continue;
			QOC.M[i] = QI.M[i] * dReqWaterFrac;
			QOT.M[i] = QI.M[i] * (1 - dReqWaterFrac);
		}

		dPrimaryGrade = dPrimaryElementConc / QOC.Mass(MP_Sol);
		dWaterFrac = QOC.Mass(MP_Liq) / QOC.Mass(MP_All);
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
void FloatationTank::UpdatePrimaryIndices()
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
/*void FloatationTank::UpdateMDDLists()
{
	for (unsigned int i = 0; i < vSecondaryIndices.size(); i++)
		for (unsigned int j = 0; j < vSecondaryMDDLists.at(i).size(); j++)
		{
			vSecondaryMDDLists.at(i).at(j).m_dwFlags = 0;
			for (unsigned int k = 0; k < vPrimaryIndices.size(); k++) //Check against primaries.
				if (vSecondaryMDDLists.at(i).at(j).m_lVal == vPrimaryIndices.at(k))
				{
					vSecondaryMDDLists.at(i).at(j).m_dwFlags = MDD_NoSelect;
					break;
				}
			for (unsigned int k = 0; k < vSecondaryIndices.size(); k++) //Check against secondaries
			{
				if (i = k) continue; //Don't check it against itself...
				if (vSecondaryMDDLists.at(i).at(j).m_lVal == vSecondaryIndices.at(k))
				{
					vSecondaryMDDLists.at(i).at(j).m_dwFlags = MDD_NoSelect;
					break;
				}
			}
		}
}*/

void FloatationTank::SetSecondaryCount(int newSize)
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
		int firstSolid = 0;
		int i;
		for (i = 0; i < gs_MVDefn.Count() && !gs_MVDefn[i].IsSolid(); i++);
		if (i == gs_MVDefn.Count())
			i = 0;
		vSecondaryIndices.resize(newSize, firstSolid);
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

void FloatationTank::UpdateOtherIndices()
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