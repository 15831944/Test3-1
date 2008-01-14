//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __RANDOM_FAILURE_CPP
#include "Random Failure.h"
#include <math.h>
#pragma optimize("", off)

#pragma warning (disable: 4018 4244)

//====================================================================================

//TODO: A Proper icon
static double Drw_SchedMaint[] = { MDrw_Poly,  -2.,2.,  2.,2.,  2.,-2., -2.,-2., -2.,2., MDrw_End };

//---------------------------------------------------------------------------

DEFINE_CONTROL_UNIT(RandomFailure, "Random Failure", DLL_GroupName)

void RandomFailure_UnitDef::GetOptions()
{
	SetDefaultTag("RF");
	SetDrawing("Tank", Drw_SchedMaint);
	SetTreeDescription("Demo:Random Failure");
	SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
	SetModelGroup(MGroup_General);
};

//---------------------------------------------------------------------------
const int maxElements = 20;

RandomFailure::RandomFailure(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
	srand((unsigned)time(NULL));

	//default values...
	bOn = true;

	dCurrentTime = 0;
}

//---------------------------------------------------------------------------

void RandomFailure::Init()
{
}

//---------------------------------------------------------------------------

bool RandomFailure::PreStartCheck()
{
	CheckTags();
	return true;
}

//---------------------------------------------------------------------------

void RandomFailure::EvalCtrlInitialise(eScdCtrlTasks Tasks)
{
	Reset();
	if (bOn)
		for (int i = 0; i < tasks.size(); i++)
			if (tasks.at(i)->TagSubs.IsActive)
				if (tasks.at(i)->bRunning)
					tasks.at(i)->TagSubs.DoubleSI = tasks.at(i)->dOnValue;
				else
					tasks.at(i)->TagSubs.DoubleSI = tasks.at(i)->dOffValue;
}

//---------------------------------------------------------------------------

void RandomFailure::Reset()
{
	dCurrentTime = 0;
	for (int i = 0; i < tasks.size(); i++)
	{
		tasks.at(i)->bRunning = true;
		tasks.at(i)->dTotalDowntime = 0;
		tasks.at(i)->dBackedUpDowntime = 0;
		tasks.at(i)->dNextFailure = -1;
		tasks.at(i)->dRepairsDone = -1;
		tasks.at(i)->lFailureCount = 0;
	}
	bG_iset = 0;
}

//---------------------------------------------------------------------------

const int idDX_Description = 100;
const int idDX_Tag = 200;
const int idDX_Count = 1;
const int idDX_Reset = 2;

//---------------------------------------------------------------------------

void RandomFailure::BuildDataFields()
{
	static MDDValueLst DDB2[]={ 
		{ PDFType_Constant, "Constant" },
		{ PDFType_Exponential, "Exponential" },
		{ PDFType_Normal, "Normal" }, 
		{ 0 } };

	static MDDValueLst DDBStates[] = { { 0, "0: Failure" }, { 1, "1: Running" } };

	DD.CheckBox("On", "", &bOn, MF_PARAMETER);
	DD.Long ("Count", "", idDX_Count, MF_PARAMETER | MF_SET_ON_CHANGE);
	DD.Text("");
	DD.Double("TotalTime", "", &dCurrentTime, MF_RESULT, MC_Time(""));
	DD.Button("Reset_All", "", idDX_Reset, MF_PARAMETER);
	DD.Text("");

	DD.ArrayBegin("Maintenance_Tasks", "Task", tasks.size());
	for (long i = 0; i < tasks.size(); i++)
	{
		if (i == 2 || (i-2)%3 == 0)
			DD.Page("Tasks");

		DD.ArrayElementStart(i);
		DD.String("Description", "", idDX_Description + i, MF_PARAMETER);
		DD.Long ("Failure_PDF", "", (long*)&tasks.at(i)->eFailureType, MF_PARAMETER | MF_SET_ON_CHANGE, DDB2);
		DD.Long ("Repair_PDF", "", (long*)&tasks.at(i)->eRepairType, MF_PARAMETER | MF_SET_ON_CHANGE, DDB2);

		DD.Double ("Average_Uptime", "", &tasks.at(i)->dAvgUptime, MF_PARAMETER, MC_Time(""));
		if (tasks.at(i)->eFailureType != PDFType_Exponential)
			DD.Double ("Uptime_StdDev", "", &tasks.at(i)->dUptimeStdDev, MF_PARAMETER, MC_Time(""));
		else
			DD.Double ("Uptime_StdDev", "", &tasks.at(i)->dAvgUptime, MF_RESULT, MC_Time(""));

		DD.Double ("Average_Downtime", "", &tasks.at(i)->dAvgDowntime, MF_PARAMETER, MC_Time(""));
		if (tasks.at(i)->eRepairType != PDFType_Exponential)
			DD.Double ("Downtime_StdDev", "", &tasks.at(i)->dDowntimeStdDev, MF_PARAMETER, MC_Time(""));
		else
			DD.Double ("Downtime_StdDev", "", &tasks.at(i)->dAvgDowntime, MF_RESULT, MC_Time(""));

		DD.Text("");
		DD.String("TagToSet", "", idDX_Tag + i, MF_PARAM_STOPPED | MF_SET_ON_CHANGE);
		MCnv TagCnv = (tasks.at(i)->TagSubs.Tag != "" && tasks.at(i)->TagSubs.IsActive) ? tasks.at(i)->TagSubs.Cnv : MC_;
		DD.Double("OnValueToSet", "", &tasks.at(i)->dOnValue, MF_PARAMETER, TagCnv);
		DD.Double("OffValueToSet", "", &tasks.at(i)->dOffValue, MF_PARAMETER, TagCnv);

		DD.Text("");
		DD.Bool("Running", "", &tasks.at(i)->bRunning, MF_RESULT, DDBStates);
		DD.Double("Total_Downtime", "", &tasks.at(i)->dTotalDowntime, MF_RESULT, MC_Time(""));
		DD.Long("Failure_Count", "", &tasks.at(i)->lFailureCount, MF_RESULT);
		DD.Text("");
		DD.ArrayElementEnd();
	}
	DD.ArrayEnd();
}

//---------------------------------------------------------------------------

bool RandomFailure::ExchangeDataFields()
{
	if (0 <= DX.Handle - idDX_Description && DX.Handle - idDX_Description < tasks.size())
	{
		if (DX.HasReqdValue)
		  tasks.at(DX.Handle - idDX_Description)->sDescription = DX.String;
		DX.String = tasks.at(DX.Handle - idDX_Description)->sDescription;
		return true;
	}
	if (0 <= DX.Handle - idDX_Tag && DX.Handle - idDX_Tag < tasks.size())
	{
		int task = DX.Handle - idDX_Tag;
		if (DX.HasReqdValue)
		{
			tasks.at(task)->TagSubs.Tag = DX.String;
		}
		DX.String = tasks.at(task)->TagSubs.Tag;
		return true;
	}
	if (DX.Handle == idDX_Count)
	{
		if (!DX.HasReqdValue)
		{
			DX.Long = tasks.size();
			return true;
		}
		SetSize(DX.Long);
		DX.Long = tasks.size();
		return true;
	}
	if (DX.Handle == idDX_Reset)
	{
		if (DX.HasReqdValue && DX.Bool)
			Reset();
		DX.Bool = false;
		return true;
	}
		
	return false;
}

//---------------------------------------------------------------------------

bool RandomFailure::ValidateDataFields()
{
	if (TagIO.ValidateReqd())
	{
		if (TagIO.StartValidateDataFields())
			for (int i = 0; i < tasks.size(); i++)
			{
				CString name;
				name.Format("Task%i", i);
				tasks.at(i)->TagSubs.Configure(i, NULL, name, MTagIO_Set);
			}
		TagIO.EndValidateDataFields();
	}
	CheckTags();
	for (int i = 0; i < tasks.size(); i++)
	{
		if (tasks.at(i)->dAvgDowntime < 0)
			tasks.at(i)->dAvgDowntime = 0;
		if (tasks.at(i)->dAvgUptime < 0)
			tasks.at(i)->dAvgUptime = 0;
		if (tasks.at(i)->dDowntimeStdDev < 0)
			tasks.at(i)->dDowntimeStdDev = 0;
		if (tasks.at(i)->dUptimeStdDev < 0)
			tasks.at(i)->dUptimeStdDev = 0;
		if (tasks.at(i)->dAvgDowntime + tasks.at(i)->dAvgUptime == 0)	//This will result in an infinite loop...
		{
			m_sErrorMsg.Format("Task %i has period of zero", i);
			return false;
		}
	}
	return true;
}

void RandomFailure::RevalidateDataFields()
{
	for (int i = 0; i < tasks.size(); i++)
	{
		if (tasks.at(i)->dDowntimeStdDev > tasks.at(i)->dAvgDowntime)
			tasks.at(i)->dDowntimeStdDev = tasks.at(i)->dAvgDowntime;
		if (tasks.at(i)->dUptimeStdDev > tasks.at(i)->dAvgUptime)
			tasks.at(i)->dUptimeStdDev = tasks.at(i)->dAvgUptime;
	}
}

//---------------------------------------------------------------------------

void RandomFailure::EvalCtrlActions(eScdCtrlTasks Tasks)
{
	RevalidateDataFields();
	dCurrentTime += getDeltaTime();
	for (int i = 0; i < tasks.size(); i++)
	{
		if (!bOn)
		{
			tasks.at(i)->bRunning = true;
		}
		else
			try
			{
				while (true)
				{
					if (tasks.at(i)->dRepairsDone < 0)	//Item is running
					{
						if (tasks.at(i)->dNextFailure < 0)	//Item has not been initialized
							RepairItem(*tasks.at(i));
						if (tasks.at(i)->dNextFailure < dCurrentTime)	//Item will go down in this iteration
							FailItem(*tasks.at(i));
						else
							break;
					}
					else								//Item is being repaired
					{
						if (tasks.at(i)->dRepairsDone < dCurrentTime)	//Item will be repaired in this iteration
							RepairItem(*tasks.at(i));
						else
							break;
					}
				}
				bool bNowRunning = tasks.at(i)->dBackedUpDowntime < getDeltaTime();
				if (tasks.at(i)->TagSubs.IsActive)
				{
					if (bNowRunning &! tasks.at(i)->bRunning)			//Task is starting up again, set tag to OnValue
						tasks.at(i)->TagSubs.DoubleSI = tasks.at(i)->dOnValue;
					if (!bNowRunning && tasks.at(i)->bRunning)			//Task is shutting down, set tag to 0
						tasks.at(i)->TagSubs.DoubleSI  = tasks.at(i)->dOffValue;
				}
				tasks.at(i)->bRunning = bNowRunning;
				if (!tasks.at(i)->bRunning)
				{
					tasks.at(i)->dTotalDowntime += getDeltaTime();
					tasks.at(i)->dBackedUpDowntime -= getDeltaTime();
				}
			}
			catch (MMdlException &ex)
			{
				Log.Message(MMsg_Error, ex.Description);
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
}

//---------------------------------------------------------------------------

void RandomFailure::FailItem(FailureVariables& task)
{
	double dRepairPeriod = CalculateEvent(task.eRepairType, task.dAvgDowntime, task.dDowntimeStdDev);
	task.dRepairsDone = task.dNextFailure + dRepairPeriod;
	task.dBackedUpDowntime += dRepairPeriod;
	task.lFailureCount++;
	task.dNextFailure = -1;	//NextFailure = -1 implies the item is currently failed.
}

//---------------------------------------------------------------------------

void RandomFailure::RepairItem(FailureVariables& task)
{
	double dUpPeriod = CalculateEvent(task.eFailureType, task.dAvgUptime, task.dUptimeStdDev);
	if (task.dRepairsDone < 0)	//This should only be called on the first iteration
		task.dNextFailure = dCurrentTime - getDeltaTime() + dUpPeriod;
	else
		task.dNextFailure = task.dRepairsDone + dUpPeriod;
	task.dRepairsDone = -1;
}

//---------------------------------------------------------------------------

double RandomFailure::CalculateEvent(PDFType ePDF, double dAverage, double dStdDev)
{
	double rnd = (double) rand() / (double) RAND_MAX;
	double ret;
	if (rnd == 0) rnd = (double) 1 / (double) RAND_MAX;	//To avoid log(0) errors.
	if (rnd == 1) rnd = (double) (RAND_MAX - 1) / (double) RAND_MAX;	//To avoid any errors caused by rnd = 1 (Possible future errors).
	switch (ePDF)
	{
	case PDFType_Constant:
		ret = dAverage + 2 * dStdDev * (rnd - 0.5);
		break;
	case PDFType_Exponential:
		ret = -log(rnd) * dAverage;	//TODO: Check that this is valid (Initial testing indicates that it is.)
		break;
	case PDFType_Normal:
		//Old approximate method:
		//This is derived from the approximation found on Mathworld for 1/2 * Erf(x) = 0.1x(4.4-x). http://mathworld.wolfram.com/NormalDistributionFunction.html
		/*double dev;
		if (rnd > 0.5)				//Effectively, we're normalising this so -0.5 <= rnd <= 0.5.
			if (rnd <= 0.98)
				dev = 2.2 - sqrt(4.84-10*(rnd-0.5));
			else
				dev = 2.2 + 40 * (rnd - 0.98);
		else
			if (rnd <= 0.48)
				dev = -2.2 + sqrt(4.84-10*rnd);
			else
				dev = -2.2 - 40 * (rnd - 0.98);*/

		ret = dAverage + dStdDev * RndGaus();
		break;
	default:
		ret = dAverage;				//If the PDFType is not supported.
	}
	SystemRnd();
	return ret > 0 ? ret : 0;		//Simple failsafe for in case the function results in the event happening before the present.
}

//---------------------------------------------------------------------------
 
// -- Lifted straight from Noise.CPP --
double RandomFailure::RndGaus()
  {//see Numerical Recipes for c (2nd Edition) - function: gasdev(long *idum)
  double fac,rsq,v1,v2;

  if (bG_iset == 0) 
    {
    do
      {
      v1 = 2.0*SystemRnd()-1.0;
      v2 = 2.0*SystemRnd()-1.0;
      rsq = v1*v1 + v2*v2;
      } while (rsq >= 1.0 || rsq == 0.0);
    fac = sqrt(-2.0 * log(rsq) / rsq);
    dG_gset = v1*fac;
    bG_iset = 1;
    return v2*fac;
    }
  else 
    {
    bG_iset = 0;
    return dG_gset;
    }
  }
// -- Lifted straight from Noise.CPP --

void RandomFailure::SetSize(long size)
{
	if (size > maxElements) size = maxElements;
	if (size < 0) size = 0;

	if (size > tasks.size()) //We want to add elements
		for (int i = tasks.size(); i < size; i++)
		{
			FailureVariables* newTask = new FailureVariables(TagIO);
			tasks.push_back(newTask);
		}
	if (size < tasks.size())  //We want to remove elements
		for (int i = tasks.size() - 1; i >= size; i--)
		{
			delete tasks.back();
			tasks.pop_back();
			tasks.at(i)->TagSubs.Tag = "";
		}
}

bool RandomFailure::CheckTags()
{
	bool ret = true;
	for (int i = 0; i < tasks.size(); i++)
	{
		if (((CString)tasks.at(i)->TagSubs.Tag).Trim() != "" && !tasks.at(i)->TagSubs.IsActive)
		{
			CString warning;
			warning.Format("Task %i does not have a valid tag", i);
			Log.SetCondition(bOn, i, MMsg_Warning, warning);
			ret = false;
		}
		else
			Log.ClearCondition(i);
	}
	return ret;
}

FailureVariables::FailureVariables(MTagIO & TagIO) : TagSubs(TagIO)
{
	TagSubs.Tag = "";
	bRunning = true;
	dAvgDowntime =24 * 3600;
	dDowntimeStdDev = 24 * 3600;
	dAvgUptime = 30 * 24 * 3600;
	dUptimeStdDev = 10 * 24 * 3600;
	
	dNextFailure = -1;
	dRepairsDone = -1;

	dBackedUpDowntime = 0;
	dTotalDowntime = 0;
	lFailureCount = 0;
	eFailureType = PDFType_Constant;
	eRepairType = PDFType_Constant;

	dOnValue = 1; dOffValue = 0;
}