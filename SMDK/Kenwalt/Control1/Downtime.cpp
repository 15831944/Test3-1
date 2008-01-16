//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __DOWNTIME_CPP
#include "Downtime.h"
#include <math.h>
//#pragma optimize("", off)

//====================================================================================

static double Drw_Downtime[] = 
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

#if defined(Control1)
DEFINE_CONTROL_UNIT_EX(Downtime, "Downtime", MDLLIBNAME)
#else
DEFINE_CONTROL_UNIT(Downtime, "TestDowntime", DLL_GroupName)
#endif

void Downtime_UnitDef::GetOptions()
{
	SetDefaultTag("DT");
	SetDrawing("Control", Drw_Downtime);
  #if defined(Control1)
	SetTreeDescription("Control:Downtime");
  #else
	SetTreeDescription("Demo:Downtime");
  #endif
	SetModelSolveMode(MSolveMode_DynamicFlow|MSolveMode_DynamicFull);
	SetModelGroup(MGroup_General);
  //SetModelLicense(MLicense_Standard);
};

//---------------------------------------------------------------------------

using namespace std;
const int maxElements = 20;

Downtime::Downtime(MUnitDefBase * pUnitDef, TaggedObject * pNd) : MBaseMethod(pUnitDef, pNd)
{
	//default values...
	bOn = true;
	dCurrentTime = 0.0;
	bForceIntegralPeriod = true;
	bForceIntegralDowntime = false;
}

//---------------------------------------------------------------------------

void Downtime::Init()
{
}

//---------------------------------------------------------------------------

bool Downtime::PreStartCheck()
{
	CheckTags(); 
	return true;
}

//---------------------------------------------------------------------------

void Downtime::EvalCtrlInitialise(eScdCtrlTasks Tasks)
  {
  Reset();
  if (bOn)
    {
    for (unsigned int i = 0; i < tasks.size(); i++)
      {
      if (tasks.at(i)->TagToSet.IsActive)
	      if (tasks.at(i)->eCurrentState == DTRS_Running || tasks.at(i)->eCurrentState == DTRS_Off)
		      tasks.at(i)->TagToSet.DoubleSI = tasks.at(i)->dOnValue;
	      else
		      tasks.at(i)->TagToSet.DoubleSI = tasks.at(i)->dOffValue;
      }
    }
  }

//---------------------------------------------------------------------------

void Downtime::Reset()
  {
  dCurrentTime = 0.0;
  for (unsigned int i = 0; i < tasks.size(); i++)
    {
    tasks.at(i)->Reset();
    }
  }

//---------------------------------------------------------------------------

const int idDX_Count = 1;
const int idDX_Reset = 2;
const int idDX_SetTag = 50;
const int idDX_Description = idDX_SetTag + maxElements;
const int idDX_OutputVal = idDX_Description + maxElements;
const int idDX_TestTag = idDX_OutputVal + maxElements;

//---------------------------------------------------------------------------

void Downtime::BuildDataFields()
{
	static MDDValueLst DD_PDF[]={ 
		{ PDFType_Constant, "Constant" },
		{ PDFType_Exponential, "Exponential" },
		{ PDFType_Normal, "Normal" }, 
		{ 0 } };

	static MDDValueLst DD_States[] = {
		{ DTRS_MinorMaintenance, "Minor Maintenance" },
		{ DTRS_MajorMaintenance, "Major Maintenance" },
		{ DTRS_MinorFailure, "Minor Failure" },
		{ DTRS_MajorFailure, "Major Failure" },
		{ DTRS_Running, "Running" },
		{ DTRS_Off, "Off" },
		{ 0 }};

		
	CString states;
	int j = 0;
	while (DD_States[j].m_pStr != 0)
	{
		CString temp;
		temp.Format("%i:%s, ", DD_States[j].m_lVal, DD_States[j].m_pStr);
		states.Append(temp);
		j++;
	}
	states.Trim(", ");

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
	for (unsigned long i = 0; i < tasks.size(); i++)
	{
		CString temp;
		temp.Format("Task_%i", i);
		DD.Page(temp);

		DD.ArrayElementStart(i);
		DD.Text("Requirements...");
		DD.String("Description", "Desc", idDX_Description + i, MF_PARAMETER);

		//Major Maintenance
		DD.Double ("RequiredMajorMaint_Period", "RqdMjrMntPeriod", &tasks.at(i)->MajorMaintenance.dDesiredPeriod, MF_PARAMETER, MC_Time("h"));
		DD.Double ("", "MajorMaint_Offset", &tasks.at(i)->MajorMaintenance.dOffset, MF_PARAM_STOPPED, MC_Time("h"));
		DD.Double ("", "RqdMajorMaintInactive_Period", &tasks.at(i)->MajorMaintenance.dDesiredDowntime, MF_PARAMETER, MC_Time("h"));

		//Minor Maintenance
		DD.Double ("RequiredMinorMaint_Period", "RqdMnrMntPeriod", &tasks.at(i)->MinorMaintenance.dDesiredPeriod, MF_PARAMETER, MC_Time("h"));
		DD.Double ("", "MinorMaint_Offset", &tasks.at(i)->MinorMaintenance.dOffset, MF_PARAM_STOPPED, MC_Time("h"));
		DD.Double ("", "RqdMinorMaint_InactivePeriod", &tasks.at(i)->MinorMaintenance.dDesiredDowntime, MF_PARAMETER, MC_Time("h"));

		DD.Text("");
		//Major Failure
		DD.Long ("MajorFailure_PDF", "", (long*)&tasks.at(i)->MajorFailure.eFailureType, MF_PARAM_STOPPED | MF_SET_ON_CHANGE, DD_PDF);
		DD.Long ("MajorRepair_PDF", "", (long*)&tasks.at(i)->MajorFailure.eRepairType, MF_PARAM_STOPPED | MF_SET_ON_CHANGE, DD_PDF);
		DD.Double ("MajorFailure_UptimeAverage", "", &tasks.at(i)->MajorFailure.dAvgUptime, MF_PARAMETER, MC_Time);
		DD.Double ("MajorFailure_UptimeDeviation", "", &tasks.at(i)->MajorFailure.dUptimeStdDev, 
			tasks.at(i)->MajorFailure.eFailureType==PDFType_Exponential ? MF_RESULT : MF_PARAMETER, MC_Time);
		DD.Double ("MajorFailure_DowntimeAverage", "", &tasks.at(i)->MajorFailure.dAvgDowntime, MF_PARAMETER, MC_Time);
		DD.Double ("MajorFailure_DowntimeDeviation", "", &tasks.at(i)->MajorFailure.dDowntimeStdDev,
			tasks.at(i)->MajorFailure.eRepairType==PDFType_Exponential ? MF_RESULT : MF_PARAMETER, MC_Time);

		//Minor Failure
		DD.Long ("MinorFailure_PDF", "", (long*)&tasks.at(i)->MinorFailure.eFailureType, MF_PARAM_STOPPED | MF_SET_ON_CHANGE, DD_PDF);
		DD.Long ("MinorRepair_PDF", "", (long*)&tasks.at(i)->MinorFailure.eRepairType, MF_PARAM_STOPPED | MF_SET_ON_CHANGE, DD_PDF);
		DD.Double ("MinorFailure_UptimeAverage", "", &tasks.at(i)->MinorFailure.dAvgUptime, MF_PARAMETER, MC_Time);
		DD.Double ("MinorFailure_UptimeDeviation", "", &tasks.at(i)->MinorFailure.dUptimeStdDev, 
			tasks.at(i)->MinorFailure.eFailureType==PDFType_Exponential ? MF_RESULT : MF_PARAMETER, MC_Time);
		DD.Double ("MinorFailure_DowntimeAverage", "", &tasks.at(i)->MinorFailure.dAvgDowntime, MF_PARAMETER, MC_Time);
		DD.Double ("MinorFailure_DowntimeDeviation", "", &tasks.at(i)->MinorFailure.dDowntimeStdDev,
			tasks.at(i)->MinorFailure.eRepairType==PDFType_Exponential ? MF_RESULT : MF_PARAMETER, MC_Time);

		DD.Text("");
		MCnv& SetTagCnv = tasks.at(i)->TagToSet.IsActive ? tasks.at(i)->TagToSet.Cnv : MC_;
		DD.Double ("ActiveValueToSet", "ActiveVal", &tasks.at(i)->dOnValue, MF_PARAMETER, SetTagCnv);
		DD.Double ("InactiveValueToSet", "InactiveVal", &tasks.at(i)->dOffValue, MF_PARAMETER, SetTagCnv);
		DD.String ("TagToSet", "TagToSet", idDX_SetTag + i, MF_PARAMETER | MF_SET_ON_CHANGE);
		DD.String ("TagToTest", "TagToTest", idDX_TestTag + i, MF_PARAMETER | MF_SET_ON_CHANGE);
		MCnv& TestTagCnv = tasks.at(i)->TagToTest.IsActive ? tasks.at(i)->TagToTest.Cnv : MC_;
		DD.Double ("On_Threshold", "", &tasks.at(i)->dOnThreshold, MF_PARAMETER, TestTagCnv);
		DD.Text("");
		DD.Text("Results...");
		DD.Double ("", "MajorMaint_Period", &tasks.at(i)->MajorMaintenance.dPeriod, MF_RESULT, MC_Time("h"));
		DD.Double ("", "MajorMaint_InactivePeriod", &tasks.at(i)->MajorMaintenance.dDowntime, MF_RESULT, MC_Time("h"));
		DD.Double ("", "MinorMaint_Period", &tasks.at(i)->MinorMaintenance.dPeriod, MF_RESULT, MC_Time("h"));
		DD.Double ("", "MinorMaint_InactivePeriod", &tasks.at(i)->MinorMaintenance.dDowntime, MF_RESULT, MC_Time("h"));
		if (!tasks.at(i)->TagToSet.IsActive)
			DD.Text("Set Tag Not Valid");
		if (!tasks.at(i)->TagToTest.IsActive)
			DD.Text("Test Tag Not Valid");
		DD.Long("State", "", (long*) &tasks.at(i)->eCurrentState, MF_RESULT, DD_States);
		DD.Text(states);
		DD.Double("OutputValue", "Output", idDX_OutputVal + i, MF_RESULT|MF_NO_FILING, SetTagCnv);
		DD.Text("");
		DD.Double("", "TtlInactiveTime", &tasks.at(i)->dTotalDowntime, MF_RESULT, MC_Time("h"));
		//DD.Double("", "TimeToNextInactivePeriod", &tasks.at(i)->dNextShutdown, MF_RESULT, MC_Time("h")); //Maybe to add in later, but it'll be a bit of a mess with four timers going simultaneously
		DD.ArrayElementEnd();
	}
	DD.ArrayEnd();
}

//---------------------------------------------------------------------------

bool Downtime::ExchangeDataFields()
{

  if (DX.Handle >= idDX_Description && DX.Handle < idDX_Description + maxElements)
	{
    const int task = DX.Handle - idDX_Description;
		if (DX.HasReqdValue)
		  tasks.at(task)->sDescription = DX.String;
		DX.String = tasks.at(task)->sDescription;
		return true;
	}
	if (DX.Handle >= idDX_SetTag && DX.Handle < idDX_SetTag + maxElements)
	{
		const int task = DX.Handle - idDX_SetTag;
		if (DX.HasReqdValue)
		{
			tasks.at(task)->TagToSet.Tag = DX.String;
		}
		DX.String = tasks.at(task)->TagToSet.Tag;
		return true;
	}
	if (DX.Handle >= idDX_TestTag && DX.Handle < idDX_TestTag + maxElements)
	{
		const int task = DX.Handle - idDX_TestTag;
		if (DX.HasReqdValue)
		{
			tasks.at(task)->TagToTest.Tag = DX.String;
		}
		DX.String = tasks.at(task)->TagToSet.Tag;
		return true;
	}
	if (DX.Handle >= idDX_OutputVal && DX.Handle < idDX_OutputVal + maxElements)
	{
		const int task = DX.Handle - idDX_OutputVal;
		DX.Double = tasks.at(task)->eCurrentState == DTRS_Running || tasks.at(task)->eCurrentState == DTRS_Off ? tasks.at(task)->dOnValue : tasks.at(task)->dOffValue;
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

bool Downtime::ValidateDataFields()
{
	if (TagIO.ValidateReqd())
	{
		if (TagIO.StartValidateDataFields())
			for (unsigned int i = 0; i < tasks.size(); i++)
			{
				CString name;
				name.Format("Task%iSet", i);
				tasks.at(i)->TagToSet.Configure(i, NULL, name, MTagIO_Set);
				name.Format("Task%iTest", i);
				tasks.at(i)->TagToTest.Configure(i, NULL, name, MTagIO_Get);
			}
		TagIO.EndValidateDataFields();
	}
	for (unsigned int i = 0; i < tasks.size(); i++)
	{
		if (!tasks.at(i)->ValidateDataFields(getDeltaTime(), bForceIntegralPeriod, bForceIntegralDowntime))
			return false;
		/*if (tasks.at(i)->dOffset < 0)
			tasks.at(i)->dOffset = 0.0;
		

		if (bForceIntegralPeriod)
		{
			tasks.at(i)->dMajorMaintPeriod = ROUNDBY(tasks.at(i)->dDesiredPeriod, getDeltaTime());
			if (tasks.at(i)->dMinorFailBackedUp
		else
			tasks.at(i)->dPeriod = tasks.at(i)->dDesiredPeriod;

		if (bForceIntegralDowntime)
			tasks.at(i)->dDowntime = ROUNDBY(tasks.at(i)->dDesiredDowntime, getDeltaTime());
		else
			tasks.at(i)->dDowntime = tasks.at(i)->dDesiredDowntime;*/
	}
	return true;
}

//---------------------------------------------------------------------------

void Downtime::EvalCtrlActions(eScdCtrlTasks Tasks)
{
	try
	{
		//TODO: Add support for periods other than Simple.
		if (!bOn)
			return; //Note: Switching the control off will now not affect the tags to set.
		dCurrentTime += getDeltaTime();
		for (vector<DowntimeVariables*>::iterator it = tasks.begin(); it != tasks.end(); it++)
		{
			(*it)->RevalidateParameters(getDeltaTime(), bForceIntegralPeriod, bForceIntegralDowntime);
			DT_RunningStates ePreviousState = (*it)->eCurrentState;
			(*it)->MajorMaintenance.Update(getDeltaTime()); //Major maintenance ALWAYS updates.
			//Update things as nescessary:
			switch ((*it)->eCurrentState)
			{
			case DTRS_Running:
				(*it)->MinorMaintenance.Update(getDeltaTime());
				(*it)->MajorFailure.Update(getDeltaTime());
				(*it)->MinorFailure.Update(getDeltaTime());
				break;
			case DTRS_MajorMaintenance:
				break;
			case DTRS_MinorMaintenance:
				(*it)->MinorMaintenance.Update(getDeltaTime());
				break;
			case DTRS_MajorFailure:
				(*it)->MajorFailure.Update(getDeltaTime());
				(*it)->MinorMaintenance.Update(getDeltaTime());
				break;
			case DTRS_MinorFailure:
				(*it)->MinorFailure.Update(getDeltaTime());
				(*it)->MinorMaintenance.Update(getDeltaTime());
				break;
			case DTRS_Off:
				break;
			}
			//Then set the current state:
			if		((*it)->MajorMaintenance.dBackedUpDowntime > 0){
				(*it)->eCurrentState = DTRS_MajorMaintenance;
				(*it)->MajorMaintenance.dBackedUpDowntime -= getDeltaTime();
			}
			else if ((*it)->MinorMaintenance.dBackedUpDowntime > 0){
				(*it)->eCurrentState = DTRS_MinorMaintenance;
				(*it)->MinorMaintenance.dBackedUpDowntime -= getDeltaTime();
			}
			else if ((*it)->MajorFailure.dBackedUpDowntime > 0){
				(*it)->eCurrentState = DTRS_MajorFailure;
				(*it)->MajorFailure.dBackedUpDowntime -= getDeltaTime();
			}
			else if ((*it)->MinorFailure.dBackedUpDowntime > 0){
				(*it)->eCurrentState = DTRS_MinorFailure;
				(*it)->MinorFailure.dBackedUpDowntime -= getDeltaTime();
			}
			else if (!(*it)->TagToTest.IsActive || (*it)->TagToTest.DoubleSI > (*it)->dOnThreshold)
				(*it)->eCurrentState = DTRS_Running;
			else
				(*it)->eCurrentState = DTRS_Off;
			//In certain conditions, we will want to reset some timers:
			if (ePreviousState == DTRS_MajorMaintenance && (*it)->eCurrentState != DTRS_MajorMaintenance)
			{
				(*it)->MinorFailure.Reset();
				(*it)->MinorMaintenance.Reset();
			}
			if (ePreviousState == DTRS_MajorFailure && (*it)->eCurrentState != DTRS_MajorFailure)
				(*it)->MinorFailure.Reset();
			//Finally, change the tag if needed:
			if ((*it)->TagToSet.IsActive)
			{
				if ((ePreviousState == DTRS_Running || ePreviousState == DTRS_Off) &&
					!((*it)->eCurrentState == DTRS_Running || (*it)->eCurrentState == DTRS_Off))
					(*it)->TagToSet.DoubleSI = (*it)->dOffValue; //Turning off
				if (!(ePreviousState == DTRS_Running || ePreviousState == DTRS_Off) &&
					((*it)->eCurrentState == DTRS_Running || (*it)->eCurrentState == DTRS_Off))
					(*it)->TagToSet.DoubleSI = (*it)->dOnValue; //Turning on
			}
			//Don't forget to increment the total downtime:
			if (!((*it)->eCurrentState == DTRS_Running || (*it)->eCurrentState == DTRS_Off))
				(*it)->dTotalDowntime += getDeltaTime();
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

//---------------------------------------------------------------------------

/*void Downtime::RevalidateParameters()
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
}*/

//---------------------------------------------------------------------------

void Downtime::SetSize(long size)
{
	if (size > maxElements) size = maxElements;
	if (size < 0) size = 0;

	if (size > tasks.size()) //We want to add elements
		for (int i = tasks.size(); i < size; i++)
		{
			DowntimeVariables* newTask = new DowntimeVariables(TagIO);
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

void Downtime::SetState(MStatesToSet SS)
  {
  MBaseMethod::SetState(SS);
  if (SS==MSS_DynStatsRunInit)
    Reset();
  }

//---------------------------------------------------------------------------

bool Downtime::CheckTags()
{
	bool ret = true;
	for (int i = 0; i < tasks.size(); i++)
	{
		if ((tasks.at(i)->TagToSet.Tag != "" && !tasks.at(i)->TagToSet.IsActive) ||
			(tasks.at(i)->TagToTest.Tag != "" && !tasks.at(i)->TagToTest.IsActive))
		{
			CString warning;
			warning.Format("Task %i contains an invalid tag.", i);
			Log.Message(MMsg_Warning, warning);
			ret = false;
		}
	}
	return ret;
}

DowntimeVariables::DowntimeVariables(MTagIO & TagIO) : TagToSet(TagIO), TagToTest(TagIO)
{
	eCurrentState = DTRS_Running;
	dTotalDowntime = 0.0;
	dOnValue = 1.0;
	dOffValue = 0.0;

	Reset();
}

void DowntimeVariables::Reset()
{
	MajorMaintenance.Reset();
	MinorMaintenance.Reset();
	MajorFailure.Reset();
	MinorFailure.Reset();
}

bool DowntimeVariables::ValidateDataFields(double deltaT, bool bForceIntegralPeriod, bool bForceIntegralDowntime)
{
	return MajorMaintenance.ValidateDataFields(deltaT, bForceIntegralPeriod, bForceIntegralDowntime) &&
		MinorMaintenance.ValidateDataFields(deltaT, bForceIntegralPeriod, bForceIntegralDowntime) &&
		MajorFailure.ValidateDataFields() &&
		MinorFailure.ValidateDataFields();
}

void DowntimeVariables::RevalidateParameters(double deltaT, bool bForceIntegralPeriod, bool bForceIntegralDowntime)
{
	MajorMaintenance.RevalidateParameters(deltaT, bForceIntegralPeriod, bForceIntegralDowntime);
	MinorMaintenance.RevalidateParameters(deltaT, bForceIntegralPeriod, bForceIntegralDowntime);
	MajorFailure.RevalidateParameters();
	MinorFailure.RevalidateParameters();
}