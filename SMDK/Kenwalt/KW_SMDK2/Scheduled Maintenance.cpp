//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __SCHEDULED_MAINTENANCE_CPP
#include "scheduled maintenance.h"
#include <math.h>
#pragma optimize("", off)

#define ROUNDBY(a, b) a/b - (int)(a/b) < 0.5 ? b * (int)(a/b) : b * (int)(a/b + 1)

//====================================================================================

static double Drw_SchedMaint[] = { MDrw_Poly,  -2.,2.,  2.,2.,  2.,-2., -2.,-2., -2.,2., MDrw_End };

//---------------------------------------------------------------------------

DEFINE_TRANSFER_UNIT(ScheduledMaintenance, "Scheduled Maintenance", DLL_GroupName)
void ScheduledMaintenance_UnitDef::GetOptions()
{
	SetDefaultTag("SM");
	SetDrawing("Tank", Drw_SchedMaint);
	SetTreeDescription("Demo:Scheduled Maintenance");
	SetModelSolveMode(MSolveMode_Probal|MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
	SetModelGroup(MGroup_General);
};

//---------------------------------------------------------------------------

ScheduledMaintenance::ScheduledMaintenance(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
	//default values...
	bOn = true;

	dCurrentTime = 0;

	TagIO.Open(10);
}

//---------------------------------------------------------------------------

void ScheduledMaintenance::Init()
{
}

//---------------------------------------------------------------------------

bool ScheduledMaintenance::PreStartCheck()
{
	return true;
}

//---------------------------------------------------------------------------

void ScheduledMaintenance::EvalCtrlInitialise(eScdCtrlTasks Tasks)
{
	Reset();
}

//---------------------------------------------------------------------------

void ScheduledMaintenance::Reset()
{
	dCurrentTime = 0;
	for (int i = 0; i < tasks.size(); i++)
	{
		tasks.at(i).dBackedUpDowntime = 0;
		tasks.at(i).dNextShutdown = tasks.at(i).dOffset;
		tasks.at(i).bRunning = true;
		tasks.at(i).dTotalDowntime = 0;
	}
}

//---------------------------------------------------------------------------

const int idDX_Description = 100;
const int idDX_Tag = 200;
const int idDX_Count = 1;
const int idDX_Reset = 2;

const int maxElements = 20;

//---------------------------------------------------------------------------

void ScheduledMaintenance::BuildDataFields()
{
	DD.CheckBox("On", "", &bOn, MF_PARAMETER);
	DD.CheckBox("Force Integral Period", "FIP", &bForceIntegralPeriod, MF_PARAMETER);
	DD.CheckBox("Force Integral Downtime", "FID", &bForceIntegralDowntime, MF_PARAMETER);
	DD.Long ("Count", "", idDX_Count, MF_PARAM_STOPPED | MF_SET_ON_CHANGE);
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

		DD.Double ("Desired Period", "DT", &tasks.at(i).dDesiredPeriod, MF_PARAMETER, MC_Time(""));
		DD.Double ("Period", "T", &tasks.at(i).dPeriod, MF_RESULT, MC_Time(""));
		DD.Double ("Offset", "T0", &tasks.at(i).dOffset, MF_PARAM_STOPPED, MC_Time(""));
		DD.Double ("Desired Downtime", "DTd", &tasks.at(i).dDesiredDowntime, MF_PARAMETER, MC_Time(""));
		DD.Double ("Downtime", "Td", &tasks.at(i).dDowntime, MF_RESULT, MC_Time(""));

		DD.Text("");
		DD.String("TagToSet", "TagToSet", idDX_Tag + i, MF_PARAMETER | MF_SET_ON_CHANGE);
		if (tasks.at(i).nTagID < 0)
			DD.Text("Tag Not Found");
		DD.Show(tasks.at(i).nTagID >= 0);
		DD.Double("OnValueToSet", "TagOnVal", &tasks.at(i).dOnValue, MF_PARAMETER);
		DD.Double("OffValueToSet", "TagOffVal", &tasks.at(i).dOffValue, MF_PARAMETER);
		DD.Show();

		DD.Text("");
		DD.Bool("Running", "", &tasks.at(i).bRunning, MF_RESULT);
		DD.Double("Total Downtime", "TDown", &tasks.at(i).dTotalDowntime, MF_RESULT, MC_Time(""));
		DD.Double("Next Shutdown", "Tls", &tasks.at(i).dNextShutdown, MF_RESULT, MC_Time(""));
		DD.Text("");
		DD.ArrayElementEnd();
	}
	DD.ArrayEnd();
}

//---------------------------------------------------------------------------

bool ScheduledMaintenance::ExchangeDataFields()
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
				tasks.at(task).nTagID = TagIO.Add(DX.String, name, TIO_Set);
			}

			if (tasks.at(task).nTagID >= 0)
			{
				tasks.at(task).dOffValue = 0;
				double dTemp;
				TagIO.Peek(DX.String, dTemp);
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

bool ScheduledMaintenance::ValidateDataFields()
{
	for (int i = 0; i < tasks.size(); i++)
	{
		if (tasks.at(i).dOffset < 0)
			tasks.at(i).dOffset = 0;

		if (bForceIntegralPeriod)
			tasks.at(i).dPeriod = ROUNDBY(tasks.at(i).dDesiredPeriod, getDeltaTime());
		else
			tasks.at(i).dPeriod = tasks.at(i).dDesiredPeriod;

		if (bForceIntegralDowntime)
			tasks.at(i).dDowntime = ROUNDBY(tasks.at(i).dDesiredDowntime, getDeltaTime());
		else
			tasks.at(i).dDowntime = tasks.at(i).dDesiredDowntime;
	}
	return true;
}

//---------------------------------------------------------------------------

void ScheduledMaintenance::EvalCtrlActions(eScdCtrlTasks Tasks)
{
	//TODO: Add support for periods other than Simple.
	RevalidateParameters();
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
				if (tasks.at(i).dNextShutdown < dCurrentTime)
				{
					tasks.at(i).dBackedUpDowntime += tasks.at(i).dDowntime;
					tasks.at(i).dNextShutdown += tasks.at(i).dPeriod;
				}
				bool bNowRunning = tasks.at(i).dBackedUpDowntime <= 0;
				if (tasks.at(i).nTagID >= 0)
				{
					if (bNowRunning &! tasks.at(i).bRunning)			//Task is starting up again, set tag to OnValue
						TagIO.DValue[tasks.at(i).nTagID] = tasks.at(i).dOnValue;
					if (!bNowRunning && tasks.at(i).bRunning)			//Task is shutting down, set tag to 0
						TagIO.DValue[tasks.at(i).nTagID] = tasks.at(i).dOffValue;
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

void ScheduledMaintenance::RevalidateParameters()
{
	for (int i = 0; i < tasks.size(); i++)
	{
		if (tasks.at(i).dDesiredDowntime > tasks.at(i).dDesiredPeriod)
			tasks.at(i).dDesiredDowntime = tasks.at(i).dDesiredPeriod;

		if (bForceIntegralDowntime)
			tasks.at(i).dDowntime = ROUNDBY(tasks.at(i).dDesiredDowntime, getDeltaTime());
		else
			tasks.at(i).dDowntime = tasks.at(i).dDesiredDowntime;

		if (bForceIntegralPeriod)
			tasks.at(i).dPeriod = ROUNDBY(tasks.at(i).dDesiredPeriod, getDeltaTime());
		else
			tasks.at(i).dPeriod = tasks.at(i).dDesiredPeriod;
	}
}

//---------------------------------------------------------------------------

void ScheduledMaintenance::SetSize(long size)
{
	if (size > maxElements) size = maxElements;
	if (size < 0) size = 0;

	if (size > tasks.size()) //We want to add elements
		for (int i = tasks.size(); i < size; i++)
		{
			MaintVariables newTask;
			newTask.bRunning = true;
			newTask.dDesiredDowntime = 3600;
			newTask.dOffset = 0;
			newTask.dDesiredPeriod = 24 * 3600;
			newTask.dTotalDowntime = 0;
			newTask.sTag = "";
			newTask.nTagID = -1;
			tasks.push_back(newTask);
		}
	if (size < tasks.size())  //We want to remove elements
		for (int i = tasks.size() - 1; i >= size; i--)
		{
			tasks.pop_back();
		}
}



