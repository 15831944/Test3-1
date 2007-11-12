//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#include "SchedEvents.h"
#include <math.h>
//#pragma optimize("", off)

#define ROUNDBY(a, b) a/b - (int)(a/b) < 0.5 ? b * (int)(a/b) : b * (int)(a/b + 1)

//====================================================================================

/*static double Drw_SchedEvents[] = 
  { MDrw_Poly,  -4.,4.,  4.,4.,  4.,-4., -4.,-4., -4.,4.,
    MDrw_Poly,  -4.,1., 1.,1., 1.,-2.2, 2.5,-2.2, 2.5,1., 4.,1.,
    MDrw_Poly,  2.5,3.6, 2.5,3.3,
    MDrw_Poly,  2.5,3.0, 2.5,2.7,
    MDrw_Poly,  2.5,2.4, 2.5,2.1,
    MDrw_Poly,  2.5,1.8, 2.5,1.5,
    MDrw_Poly,  2.5,-2.5, 2.5,-2.8,
    MDrw_Poly,  2.5,-3.1, 2.5,-3.4,
    MDrw_Poly,  2.5,-3.7, 2.5,-4.0,
    MDrw_End };*/
static double Drw_SchedEvents[] = 
  { MDrw_Poly,  -3.2,-3.2, -3.2,3.2, 3.2,3.2, 3.2,-3.2, -3.2,-3.2,
    MDrw_Poly,  -3.2,0.8, 0.8,0.8, 0.8,-1.76, 2.0,-1.76, 2.0,0.8, 3.2,0.8,
    MDrw_Poly,  2.0,2.88, 2.0,2.64,
    MDrw_Poly,  2.0,2.40, 2.0,2.16,
    MDrw_Poly,  2.0,1.92, 2.0,1.68,
    MDrw_Poly,  2.0,1.44, 2.0,1.20,
    MDrw_Poly,  2.0,-2.00, 2.0,-2.24,
    MDrw_Poly,  2.0,-2.48, 2.0,-2.72,
    MDrw_Poly,  2.0,-2.96, 2.0,-3.20,
    MDrw_TagPos, 0, -6.7,
    MDrw_End };

//---------------------------------------------------------------------------

#if defined(Control1)
DEFINE_CONTROL_UNIT_EX(ScheduledEvents, "ScheduledEvents", MDLLIBNAME)
#else
DEFINE_CONTROL_UNIT(ScheduledEvents, "TestScheduledEvents", DLL_GroupName)
#endif

void ScheduledEvents_UnitDef::GetOptions()
{
	SetDefaultTag("SE");
	SetDrawing("Control", Drw_SchedEvents);
  #if defined(Control1)
	SetTreeDescription("Control:Scheduled Events");
  #else
 	SetTreeDescription("Demo:Scheduled Events");
  #endif
	SetModelSolveMode(MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
	SetModelGroup(MGroup_General);
  SetModelLicense(MLicense_Standard);
};

//---------------------------------------------------------------------------

const int maxElements = 20;

ScheduledEvents::ScheduledEvents(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
	//default values...
	bOn = true;
	dCurrentTime = 0.0;
	bForceIntegralPeriod = true;
	bForceIntegralDowntime = false;
}

//---------------------------------------------------------------------------

void ScheduledEvents::Init()
{
}

//---------------------------------------------------------------------------

bool ScheduledEvents::PreStartCheck()
{
	CheckTags(); 
	return true;
}

//---------------------------------------------------------------------------

void ScheduledEvents::EvalCtrlInitialise(eScdCtrlTasks Tasks)
{
	Reset();
	for (int i = 0; i < tasks.size(); i++)
		if (tasks.at(i)->tagSubs.IsActive)
			if (tasks.at(i)->bRunning)
				tasks.at(i)->tagSubs.DoubleSI = tasks.at(i)->dOnValue;
			else
				tasks.at(i)->tagSubs.DoubleSI = tasks.at(i)->dOffValue;
}

//---------------------------------------------------------------------------

void ScheduledEvents::Reset()
{
	dCurrentTime = 0.0;
	for (int i = 0; i < tasks.size(); i++)
	{
		tasks.at(i)->dBackedUpDowntime = 0.0;
		tasks.at(i)->dNextShutdown = tasks.at(i)->dOffset;
		tasks.at(i)->bRunning = true;
		tasks.at(i)->dTotalDowntime = 0.0;
	}
}

//---------------------------------------------------------------------------

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

		DD.Double ("RequiredPeriod", "RqdPeriod", &tasks.at(i)->dDesiredPeriod, MF_PARAMETER, MC_Time("h"));
		DD.Double ("", "Offset", &tasks.at(i)->dOffset, MF_PARAM_STOPPED, MC_Time("h"));
		DD.Double ("", "RqdInactivePeriod", &tasks.at(i)->dDesiredDowntime, MF_PARAMETER, MC_Time("h"));
    MCnv TagCnv = tasks.at(i)->tagSubs.IsActive ? tasks.at(i)->tagSubs.Cnv : MC_;
		DD.Double ("ActiveValueToSet", "ActiveVal", &tasks.at(i)->dOnValue, MF_PARAMETER, TagCnv);
		DD.Double ("InactiveValueToSet", "InactiveVal", &tasks.at(i)->dOffValue, MF_PARAMETER, TagCnv);
		DD.String ("TagToSet", "TagToSet", idDX_Tag + i, MF_PARAMETER | MF_SET_ON_CHANGE);
		DD.Text("");
		DD.Text("Results...");
		DD.Double ("", "Period", &tasks.at(i)->dPeriod, MF_RESULT, MC_Time("h"));
		DD.Double ("", "InactivePeriod", &tasks.at(i)->dDowntime, MF_RESULT, MC_Time("h"));
		if (!tasks.at(i)->tagSubs.IsActive)
			DD.Text("Tag Not Valid");
		DD.Bool("Active", "", &tasks.at(i)->bRunning, MF_RESULT);
		DD.Double("OutputValue", "Output", idDX_OutputVal + i, MF_RESULT|MF_NO_FILING, TagCnv);
		DD.Text("");
		DD.Double("", "TtlInactiveTime", &tasks.at(i)->dTotalDowntime, MF_RESULT, MC_Time("h"));
		DD.Double("", "TimeToNextInactivePeriod", &tasks.at(i)->dNextShutdown, MF_RESULT, MC_Time("h"));
		DD.Text("");
		DD.ArrayElementEnd();
	}
	DD.ArrayEnd();
}

//---------------------------------------------------------------------------

bool ScheduledEvents::ExchangeDataFields()
{

  if (DX.Handle >= idDX_Description && DX.Handle < idDX_Description + maxElements)
	{
    const int task = DX.Handle - idDX_Description;
		if (DX.HasReqdValue)
		  tasks.at(task)->sDescription = DX.String;
		DX.String = tasks.at(task)->sDescription;
		return true;
	}
	if (DX.Handle >= idDX_Tag && DX.Handle < idDX_Tag + maxElements)
	{
		const int task = DX.Handle - idDX_Tag;
		if (DX.HasReqdValue)
		{
			tasks.at(task)->tagSubs.Tag = DX.String;
		}
		DX.String = tasks.at(task)->tagSubs.Tag;
		return true;
	}
	if (DX.Handle >= idDX_OutputVal && DX.Handle < idDX_OutputVal + maxElements)
	{
		const int task = DX.Handle - idDX_OutputVal;
		DX.Double = tasks.at(task)->bRunning ? tasks.at(task)->dOnValue : tasks.at(task)->dOffValue;
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
	if (TagIO.ValidateReqd())
	{
		if (TagIO.StartValidateDataFields())
			for (int i = 0; i < tasks.size(); i++)
			{
				CString name;
				name.Format("Task%i", i);
				tasks.at(i)->tagSubs.Configure(i, NULL, name, MTagIO_Set);
			}
		TagIO.EndValidateDataFields();
	}
	for (int i = 0; i < tasks.size(); i++)
	{
		if (tasks.at(i)->dOffset < 0)
			tasks.at(i)->dOffset = 0.0;

		if (bForceIntegralPeriod)
			tasks.at(i)->dPeriod = ROUNDBY(tasks.at(i)->dDesiredPeriod, getDeltaTime());
		else
			tasks.at(i)->dPeriod = tasks.at(i)->dDesiredPeriod;

		if (bForceIntegralDowntime)
			tasks.at(i)->dDowntime = ROUNDBY(tasks.at(i)->dDesiredDowntime, getDeltaTime());
		else
			tasks.at(i)->dDowntime = tasks.at(i)->dDesiredDowntime;
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
			tasks.at(i)->bRunning = true;
		}
		else
			try
			{
				if (tasks.at(i)->dNextShutdown < dCurrentTime)
				{
					tasks.at(i)->dBackedUpDowntime += tasks.at(i)->dDowntime;
					tasks.at(i)->dNextShutdown += tasks.at(i)->dPeriod;
				}
				bool bNowRunning = tasks.at(i)->dBackedUpDowntime <= 0;
				if (tasks.at(i)->tagSubs.IsActive)
				{
					if (bNowRunning &! tasks.at(i)->bRunning)			//Task is starting up again, set tag to OnValue
						tasks.at(i)->tagSubs.DoubleSI = tasks.at(i)->dOnValue;
					if (!bNowRunning && tasks.at(i)->bRunning)			//Task is shutting down, set tag to 0
						tasks.at(i)->tagSubs.DoubleSI = tasks.at(i)->dOffValue;
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

void ScheduledEvents::RevalidateParameters()
{
	for (int i = 0; i < tasks.size(); i++)
	{
		if (tasks.at(i)->dDesiredDowntime > tasks.at(i)->dDesiredPeriod)
			tasks.at(i)->dDesiredDowntime = tasks.at(i)->dDesiredPeriod;

		if (bForceIntegralDowntime)
			tasks.at(i)->dDowntime = ROUNDBY(tasks.at(i)->dDesiredDowntime, getDeltaTime());
		else
			tasks.at(i)->dDowntime = tasks.at(i)->dDesiredDowntime;

		if (bForceIntegralPeriod)
			tasks.at(i)->dPeriod = ROUNDBY(tasks.at(i)->dDesiredPeriod, getDeltaTime());
		else
			tasks.at(i)->dPeriod = tasks.at(i)->dDesiredPeriod;
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
			MaintVariables* newTask = new MaintVariables(TagIO);
			tasks.push_back(newTask);
		}
	if (size < tasks.size())  //We want to remove elements
		for (int i = tasks.size() - 1; i >= size; i--)
		{
			delete tasks.back();
			tasks.pop_back();
		}
}

//---------------------------------------------------------------------------

void ScheduledEvents::SetState(MStatesToSet SS)
  {
  MBaseMethod::SetState(SS);
  if (SS==MSS_DynStatsRunInit)
    Reset();
  }

//---------------------------------------------------------------------------

bool ScheduledEvents::CheckTags()
{
	bool ret = true;
	for (int i = 0; i < tasks.size(); i++)
	{
		if (tasks.at(i)->tagSubs.Tag != "" && !tasks.at(i)->tagSubs.IsActive)
		{
			CString warning;
			warning.Format("Task %i does not have a valid tag", i);
			Log.Message(MMsg_Warning, warning);
			ret = false;
		}
	}
	return ret;
}

MaintVariables::MaintVariables(MTagIO & TagIO) : tagSubs(TagIO)
{
	//tagItem = new MTagIOSubscription(TagIO);
	bRunning = true;
	dDesiredDowntime = 3600;
	dOffset = 0.0;
	dDesiredPeriod = 24 * 3600;
	dTotalDowntime = 0.0;
	dOnValue = 1.0;
	dOffValue = 0.0;
	dDowntime = dDesiredDowntime;
	dPeriod = dDesiredDowntime;
	dNextShutdown = dOffset;
	dBackedUpDowntime = 0.0;
}
