//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __SCHEDEVENTS_CPP
#include "SchedEvents.h"
#include <math.h>
//#pragma optimize("", off)

#define ROUNDBY(a, b) a/b - (int)(a/b) < 0.5 ? b * (int)(a/b) : b * (int)(a/b + 1)

//====================================================================================

static double Drw_SchedEvents[] = 
  { MDrw_Poly,  -4.,4.,  4.,4.,  4.,-4., -4.,-4., -4.,4.,
    MDrw_Poly,  -4.,1., 1.,1., 1.,-2.2, 2.5,-2.2, 2.5,1., 4.,1.,
    MDrw_Poly,  2.5,3.6, 2.5,3.3,
    MDrw_Poly,  2.5,3.0, 2.5,2.7,
    MDrw_Poly,  2.5,2.4, 2.5,2.1,
    MDrw_Poly,  2.5,1.8, 2.5,1.5,
    MDrw_Poly,  2.5,-2.5, 2.5,-2.8,
    MDrw_Poly,  2.5,-3.1, 2.5,-3.4,
    MDrw_Poly,  2.5,-3.7, 2.5,-4.0,
    MDrw_End };

//---------------------------------------------------------------------------

DEFINE_CONTROL_UNIT(ScheduledEvents, "Scheduled Events", DLL_GroupName)

void ScheduledEvents_UnitDef::GetOptions()
{
	SetDefaultTag("SE");
	SetDrawing("Control", Drw_SchedEvents);
	SetTreeDescription("Control:Scheduled Events");
	SetModelSolveMode(MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
	SetModelGroup(MGroup_General);
  //SetModelLicense(MLicense_Standard);
};

//---------------------------------------------------------------------------

ScheduledEvents::ScheduledEvents(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
	//default values...
	bOn = true;
	dCurrentTime = 0.0;
  bForceIntegralPeriod = true;
  bForceIntegralDowntime = false;
	TagIO.Open(10);
}

//---------------------------------------------------------------------------

void ScheduledEvents::Init()
{
}

//---------------------------------------------------------------------------

bool ScheduledEvents::PreStartCheck()
{
	return true;
}

//---------------------------------------------------------------------------

void ScheduledEvents::EvalCtrlInitialise(eScdCtrlTasks Tasks)
{
	Reset();
}

//---------------------------------------------------------------------------

void ScheduledEvents::Reset()
{
	dCurrentTime = 0.0;
	for (int i = 0; i < tasks.size(); i++)
	{
		tasks.at(i).dBackedUpDowntime = 0.0;
		tasks.at(i).dNextShutdown = tasks.at(i).dOffset;
		tasks.at(i).bRunning = true;
		tasks.at(i).dTotalDowntime = 0.0;
	}
}

//---------------------------------------------------------------------------

const int maxElements = 20;

const int idDX_Count = 1;
const int idDX_Reset = 2;
const int idDX_Tag = 50;
const int idDX_Description = idDX_Tag+maxElements;
const int idDX_OutputVal = idDX_Description+maxElements;

//---------------------------------------------------------------------------

void ScheduledEvents::BuildDataFields()
{
	DD.CheckBox("On", "", &bOn, MF_PARAMETER);
	DD.CheckBox("IntegralPeriod", "", &bForceIntegralPeriod, MF_PARAMETER);
	DD.CheckBox("IntegralInactivePeriod", "", &bForceIntegralDowntime, MF_PARAMETER);
	DD.Long ("Count", "", idDX_Count, MF_PARAM_STOPPED | MF_SET_ON_CHANGE);
	DD.Text("");
	DD.Double("TotalTime", "", &dCurrentTime, MF_RESULT | MF_INIT_HIDDEN, MC_Time("h"));
	DD.Button("Reset_All", "", idDX_Reset, MF_PARAMETER);
	DD.Text("");
	DD.Text("");

	DD.ArrayBegin("Scheduled_Events", "SE", tasks.size());
	for (long i = 0; i < tasks.size(); i++)
	{
		if (i == 2 || (i-2)%3 == 0)
			DD.Page("Tasks");

		DD.ArrayElementStart(i);
		DD.Text("------------------------------------------------------------");
		DD.Text("Requirements...");
		DD.String("Description", "Desc", idDX_Description + i, MF_PARAMETER);

		DD.Double ("RequiredPeriod", "RqdPeriod", &tasks.at(i).dDesiredPeriod, MF_PARAMETER, MC_Time("h"));
		DD.Double ("", "Offset", &tasks.at(i).dOffset, MF_PARAM_STOPPED, MC_Time("h"));
		DD.Double ("", "RqdInactivePeriod", &tasks.at(i).dDesiredDowntime, MF_PARAMETER, MC_Time("h"));
		DD.Double ("ActiveValueToSet", "ActiveVal", &tasks.at(i).dOnValue, MF_PARAMETER);
		DD.Double ("InactiveValueToSet", "InactiveVal", &tasks.at(i).dOffValue, MF_PARAMETER);
		DD.String ("TagToSet", "TagToSet", idDX_Tag + i, MF_PARAMETER | MF_SET_ON_CHANGE);
		DD.Text("");
		DD.Text("Results...");
		DD.Double ("", "Period", &tasks.at(i).dPeriod, MF_RESULT, MC_Time("h"));
		DD.Double ("", "InactivePeriod", &tasks.at(i).dDowntime, MF_RESULT, MC_Time("h"));
		if (tasks.at(i).nTagID < 0)
			DD.Text("Tag Not Found");
		DD.Bool("Active", "", &tasks.at(i).bRunning, MF_RESULT);
		DD.Double("OutputValue", "Output", idDX_OutputVal + i, MF_RESULT|MF_NO_FILING);
		DD.Text("");
		DD.Double("", "TtlInactiveTime", &tasks.at(i).dTotalDowntime, MF_RESULT, MC_Time("h"));
		DD.Double("", "TimeToNextInactivePeriod", &tasks.at(i).dNextShutdown, MF_RESULT, MC_Time("h"));
		DD.Text("");
		DD.ArrayElementEnd();
	}
	DD.ArrayEnd();
  DD.Text("------------------------------------------------------------");
}

//---------------------------------------------------------------------------

bool ScheduledEvents::ExchangeDataFields()
{
	if (DX.Handle >= idDX_Description && DX.Handle < idDX_Description + maxElements)
	{
    const int task = DX.Handle - idDX_Description;
		if (DX.HasReqdValue)
		  tasks.at(task).sDescription = DX.String;
		DX.String = tasks.at(task).sDescription;
		return true;
	}
	if (DX.Handle >= idDX_Tag && DX.Handle < idDX_Tag + maxElements)
	{
		const int task = DX.Handle - idDX_Tag;
		if (DX.HasReqdValue)
		{
      //todo if tag is different to previous tag, then format and check if tag is valid
      //todo should probably only check tags and add them to tagIO later or at startup!!!!!
			tasks.at(task).sTag = DX.String;
			CString name;
			name.Format("TagToSet%x", task);

			int curTag = TagIO.FindTag(DX.String);
			if (curTag >= 0)
				tasks.at(task).nTagID = curTag;
			else
			{
        curTag = TagIO.FindName(name);
  			if (curTag >= 0)
				  TagIO.Remove(curTag);
				tasks.at(task).nTagID = TagIO.Add(DX.String, name, MTIO_Set);
			}

		}
		DX.String = tasks.at(task).sTag;
		return true;
	}
	if (DX.Handle >= idDX_OutputVal && DX.Handle < idDX_OutputVal + maxElements)
	{
		const int task = DX.Handle - idDX_OutputVal;
    DX.Double = tasks.at(task).bRunning ? tasks.at(task).dOnValue : tasks.at(task).dOffValue;
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

bool ScheduledEvents::ValidateDataFields()
{
	for (int i = 0; i < tasks.size(); i++)
	{
		if (tasks.at(i).dOffset < 0)
			tasks.at(i).dOffset = 0.0;

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

void ScheduledEvents::EvalCtrlActions(eScdCtrlTasks Tasks)
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

void ScheduledEvents::RevalidateParameters()
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

void ScheduledEvents::SetSize(long size)
{
	if (size > maxElements) size = maxElements;
	if (size < 0) size = 0;

	if (size > tasks.size()) //We want to add elements
		for (int i = tasks.size(); i < size; i++)
		{
			MaintVariables newTask;
			newTask.bRunning = true;
			newTask.dDesiredDowntime = 3600;
			newTask.dOffset = 0.0;
			newTask.dDesiredPeriod = 24 * 3600;
			newTask.dTotalDowntime = 0.0;
			newTask.sTag = "";
			newTask.nTagID = -1;
      newTask.dOnValue = 1.0;
      newTask.dOffValue = 0.0;
      newTask.dDowntime = newTask.dDesiredDowntime;
      newTask.dPeriod = newTask.dDesiredDowntime;
      newTask.dNextShutdown = newTask.dOffset;
      newTask.dBackedUpDowntime = 0.0;

			tasks.push_back(newTask);
		}
	if (size < tasks.size())  //We want to remove elements
		for (int i = tasks.size() - 1; i >= size; i--)
		{
			tasks.pop_back();
		}
}

//---------------------------------------------------------------------------


