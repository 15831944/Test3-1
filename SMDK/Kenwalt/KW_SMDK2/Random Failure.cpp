//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __RANDOM_FAILURE_CPP
#include "Random Failure.h"
#include <math.h>
#pragma optimize("", off)

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

RandomFailure::RandomFailure(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
	srand((unsigned)time(NULL));

	//default values...
	bOn = true;

	TagIO.Open(20);

	dCurrentTime = 0;
}

//---------------------------------------------------------------------------

void RandomFailure::Init()
{
}

//---------------------------------------------------------------------------

bool RandomFailure::PreStartCheck()
{
	return true;
}

//---------------------------------------------------------------------------

void RandomFailure::EvalCtrlInitialise(eScdCtrlTasks Tasks)
{
	Reset();
}

//---------------------------------------------------------------------------

void RandomFailure::Reset()
{
	dCurrentTime = 0;
	for (int i = 0; i < tasks.size(); i++)
	{
		tasks.at(i).bRunning = true;
		tasks.at(i).dTotalDowntime = 0;
		tasks.at(i).dBackedUpDowntime = 0;
		tasks.at(i).dNextFailure = -1;
		tasks.at(i).dRepairsDone = -1;
		tasks.at(i).lFailureCount = 0;
	}
}

//---------------------------------------------------------------------------

const int idDX_Description = 100;
const int idDX_Tag = 200;
const int idDX_Count = 1;
const int idDX_Reset = 2;

const int maxElements = 20;

//---------------------------------------------------------------------------

void RandomFailure::BuildDataFields()
{
	static MDDValueLst DDB2[]={ 
		{ PDFType_Constant, "Constant" },
		{ PDFType_Exponential, "Exponential" },
		{ PDFType_Normal, "Normal" }, 
		{ 0 } };

	DD.CheckBox("On", "", &bOn, MF_PARAMETER);
	DD.Long ("Count", "", idDX_Count, MF_PARAMETER | MF_SET_ON_CHANGE);
	DD.Text("");
	DD.Double("TotalTime", "T(tot)", &dCurrentTime, MF_RESULT, MC_Time(""));
	DD.Button("Reset All", "", idDX_Reset, MF_PARAMETER);
	DD.Text("");

	DD.ArrayBegin("Maintenance_Tasks", "Task", tasks.size());
	for (long i = 0; i < tasks.size(); i++)
	{
		if (i == 2 || (i-2)%3 == 0)
			DD.Page("Tasks");

		DD.ArrayElementStart(i);
		DD.String("Description", "Desc", idDX_Description + i, MF_PARAMETER);
		DD.Long ("Failure PDF", "FPDF", (long*)&tasks.at(i).eFailureType, MF_PARAMETER | MF_SET_ON_CHANGE, DDB2);
		DD.Long ("Repair PDF", "RPDF", (long*)&tasks.at(i).eRepairType, MF_PARAMETER | MF_SET_ON_CHANGE, DDB2);

		DD.Double ("Average Uptime", "Tu", &tasks.at(i).dAvgUptime, MF_PARAMETER, MC_Time(""));
		if (tasks.at(i).eFailureType != PDFType_Exponential)
			DD.Double ("Uptime StdDev", "Tus", &tasks.at(i).dUptimeStdDev, MF_PARAMETER, MC_Time(""));
		else
			DD.Double ("Uptime StdDev", "Tus", &tasks.at(i).dAvgUptime, MF_RESULT, MC_Time(""));

		DD.Double ("Average Downtime", "Td", &tasks.at(i).dAvgDowntime, MF_PARAMETER, MC_Time(""));
		if (tasks.at(i).eRepairType != PDFType_Exponential)
			DD.Double ("Downtime StdDev", "Tds", &tasks.at(i).dDowntimeStdDev, MF_PARAMETER, MC_Time(""));
		else
			DD.Double ("Downtime StdDev", "Tds", &tasks.at(i).dAvgDowntime, MF_RESULT, MC_Time(""));

		DD.Text("");
		DD.String("TagToSet", "TagToSet", idDX_Tag, MF_PARAMETER | MF_SET_ON_CHANGE);
		if (tasks.at(i).nTagID < 0)
			DD.Text("Tag Not Found");
		DD.Show(tasks.at(i).nTagID >= 0);
		DD.Double("OnValueToSet", "TagOnVal", &tasks.at(i).dOnValue, MF_PARAMETER);
		DD.Double("OffValueToSet", "TagOffVal", &tasks.at(i).dOffValue, MF_PARAMETER);
		DD.Show();

		DD.Text("");
		DD.Bool("Running", "", &tasks.at(i).bRunning, MF_RESULT);
		DD.Double("Total Downtime", "TDown", &tasks.at(i).dTotalDowntime, MF_RESULT, MC_Time(""));
		DD.Long("Failure Count", "Nf", &tasks.at(i).lFailureCount, MF_RESULT);
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
		  tasks.at(DX.Handle - idDX_Description).sDescription = DX.String;
		DX.String = tasks.at(DX.Handle - idDX_Description).sDescription;
		return true;
	}
	if (0 <= DX.Handle - idDX_Tag && DX.Handle - idDX_Tag < tasks.size())
	{
		int task = DX.Handle - idDX_Tag;
		if (DX.HasReqdValue)
		{
			tasks.at(task).sTag = DX.String;
			CString name;
			name.Format("TagToSet%x", task);

			int curTag = TagIO.FindTag(DX.String);
			if (curTag >= 0)
				tasks.at(task).nTagID = curTag;
			else
			{
				TagIO.Remove(TagIO.FindName(name));
				tasks.at(task).nTagID = TagIO.Set(-1, DX.String, name, MTagIO_Set);
			}

			if (tasks.at(task).nTagID >= 0)
			{
				tasks.at(task).dOffValue = 0;
				double dTemp;
				_asm int 3; // CNM fix next line 
				//TagIO.SetTag(DX.String, dTemp);
				tasks.at(task).dOnValue = dTemp;
			}
		}
		DX.String = tasks.at(task).sTag;
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
	for (int i = 0; i < tasks.size(); i++)
	{
		if (tasks.at(i).dAvgDowntime < 0)
			tasks.at(i).dAvgDowntime = 0;
		if (tasks.at(i).dAvgUptime < 0)
			tasks.at(i).dAvgUptime = 0;
		if (tasks.at(i).dDowntimeStdDev < 0)
			tasks.at(i).dDowntimeStdDev = 0;
		if (tasks.at(i).dUptimeStdDev < 0)
			tasks.at(i).dUptimeStdDev = 0;
		if (tasks.at(i).dAvgDowntime + tasks.at(i).dAvgUptime == 0)	//This will result in an infinite loop...
			return false;
	}
	return true;
}

void RandomFailure::RevalidateDataFields()
{
	ValidateDataFields();
	for (int i = 0; i < tasks.size(); i++)
	{
		if (tasks.at(i).dDowntimeStdDev > tasks.at(i).dAvgDowntime)
			tasks.at(i).dDowntimeStdDev = tasks.at(i).dAvgDowntime;
		if (tasks.at(i).dUptimeStdDev > tasks.at(i).dAvgUptime)
			tasks.at(i).dUptimeStdDev = tasks.at(i).dAvgUptime;
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
			tasks.at(i).bRunning = true;
		}
		else
			try
			{
				while (true)
				{
					if (tasks.at(i).dRepairsDone < 0)	//Item is running
					{
						if (tasks.at(i).dNextFailure < 0)	//Item has not been initialized
							RepairItem(&tasks.at(i));
						if (tasks.at(i).dNextFailure < dCurrentTime)	//Item will go down in this iteration
							FailItem(&tasks.at(i));
						else
							break;
					}
					else								//Item is being repaired
					{
						if (tasks.at(i).dRepairsDone < dCurrentTime)	//Item will be repaired in this iteration
							RepairItem(&tasks.at(i));
						else
							break;
					}
				}
				bool bNowRunning = tasks.at(i).dBackedUpDowntime < getDeltaTime();
				if (tasks.at(i).nTagID >= 0)
				{
					if (bNowRunning &! tasks.at(i).bRunning)			//Task is starting up again, set tag to OnValue
            TagIO[tasks.at(i).nTagID]->DoubleSI = tasks.at(i).dOnValue;
					if (!bNowRunning && tasks.at(i).bRunning)			//Task is shutting down, set tag to 0
            TagIO[tasks.at(i).nTagID]->DoubleSI = tasks.at(i).dOffValue;
				}
				tasks.at(i).bRunning = bNowRunning;
				if (!tasks.at(i).bRunning)
				{
					tasks.at(i).dTotalDowntime += getDeltaTime();
					tasks.at(i).dBackedUpDowntime -= getDeltaTime();
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

void RandomFailure::FailItem(FailureVariables* task)
{
	double dRepairPeriod = CalculateEvent(task->eRepairType, task->dAvgDowntime, task->dDowntimeStdDev);
	task->dRepairsDone = task->dNextFailure + dRepairPeriod;
	task->dBackedUpDowntime += dRepairPeriod;
	task->lFailureCount++;
	task->dNextFailure = -1;	//NextFailure = -1 implies the item is currently failed.
}

//---------------------------------------------------------------------------

void RandomFailure::RepairItem(FailureVariables* task)
{
	double dUpPeriod = CalculateEvent(task->eFailureType, task->dAvgUptime, task->dUptimeStdDev);
	if (task->dRepairsDone < 0)	//This should only be called on the first iteration
		task->dNextFailure = dCurrentTime - getDeltaTime() + dUpPeriod;
	else
		task->dNextFailure = task->dRepairsDone + dUpPeriod;
	task->dRepairsDone = -1;
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
	case PDFType_Normal:			//This is derived from the approximation found on Mathworld for 1/2 * Erf(x) = 0.1x(4.4-x). http://mathworld.wolfram.com/NormalDistributionFunction.html
		double dev;
		if (rnd > 0.5)				//Effectively, we're normalising this so -0.5 <= rnd <= 0.5.
			if (rnd <= 0.98)
				dev = 2.2 - sqrt(4.84-10*(rnd-0.5));
			else
				dev = 2.2 + 40 * (rnd - 0.98);
		else
			if (rnd <= 0.48)
				dev = -2.2 + sqrt(4.84-10*rnd);
			else
				dev = -2.2 - 40 * (rnd - 0.98);
		ret = dAverage + dStdDev * dev;
		break;
	default:
		ret = dAverage;				//If the PDFType is not supported.
	}
	return ret > 0 ? ret : 0;		//Simple failsafe for in case the function results in the event happening before the present.
}

//---------------------------------------------------------------------------

void RandomFailure::SetSize(long size)
{
	if (size > maxElements) size = maxElements;
	if (size < 0) size = 0;

	if (size > tasks.size()) //We want to add elements
		for (int i = tasks.size(); i < size; i++)
		{
			FailureVariables newTask;
			newTask.bRunning = true;
			newTask.dAvgDowntime =24 * 3600;
			newTask.dDowntimeStdDev = 24 * 3600;
			newTask.dAvgUptime = 30 * 24 * 3600;
			newTask.dUptimeStdDev = 10 * 24 * 3600;
			
			newTask.dNextFailure = -1;
			newTask.dRepairsDone = -1;

			newTask.dBackedUpDowntime = 0;
			newTask.dTotalDowntime = 0;
			newTask.lFailureCount = 0;
			newTask.eFailureType = PDFType_Constant;
			newTask.eRepairType = PDFType_Constant;

			newTask.nTagID = -1;
			newTask.sTag = "";
			tasks.push_back(newTask);
		}
	if (size < tasks.size())  //We want to remove elements
		for (int i = tasks.size() - 1; i >= size; i--)
		{
			tasks.pop_back();
		}
}



