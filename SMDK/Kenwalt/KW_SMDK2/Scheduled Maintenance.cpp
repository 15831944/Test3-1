//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#include "stdafx.h"
#define  __SCHEDULED_MAINTENANCE_CPP
#include "scheduled maintenance.h"
#pragma optimize("", off)

//====================================================================================

static double Drw_SchedMaint[] = { MDrw_Poly,  -2.,2.,  2.,2.,  2.,-2., -2.,-2., -2.,2.,
MDrw_End };

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
    MaintVariables* defaultSet = new MaintVariables();
    bOn = false;
    defaultSet->dDownTime = 3600;
    defaultSet->dPeriod = 24 * 3600;
    defaultSet->dOffset = 0;
    
    eType = Type_Simple;
    eStrategy = Strat_Warning;
  }

//---------------------------------------------------------------------------

void ScheduledMaintenance::Init()
  {
  }

void ScheduledMaintenance::EvalCtrlInitialise(eScdCtrlTasks Tasks)
  {
    dCurrentTime = 0;
    for (int i = 0; i < tasks.size(); i++)
      {
      tasks.at(i).bRunning = true;
      tasks.at(i).dTotalDowntime = 0;
      }
  }

//---------------------------------------------------------------------------

const int idDX_Description = 100;
const int idDX_Count = 1;

const int maxElements = 20;

void ScheduledMaintenance::BuildDataFields()
{
  static MDDValueLst DDB1[]={
    {Strat_Error,     "Error"},
    {Strat_Warning,   "Warning"},
    {Strat_Carry,     "Carry"},
    {0}};

  static MDDValueLst DDB2[]={ 
    { Type_Simple, "Simple" },
    {0} };

  DD.CheckBox("On", "", &bOn, MF_PARAMETER);
  DD.Long ("Strategy", "Strat", (long*)&eStrategy, MF_PARAMETER, DDB1);
  DD.Long ("Count", "", idDX_Count, MF_PARAMETER | MF_SET_ON_CHANGE);
  
  DD.ArrayBegin("Maintenance_Tasks", "Task", tasks.size());
  for (long i = 0; i < tasks.size(); i++)
    {
    DD.ArrayElementStart(i);
    DD.String("Description", "Desc", idDX_Description + i, MF_PARAMETER);
    DD.Long ("Type", "", (long*)&tasks.at(i).eType, MF_PARAMETER | MF_SET_ON_CHANGE, DDB2);

    DD.Double ("Period", "T", &tasks.at(i).dPeriod, MF_PARAMETER, MC_Time(""));
    DD.Double ("Offset", "T0", &tasks.at(i).dOffset, MF_PARAMETER, MC_Time(""));
    DD.Double ("Downtime", "Td", &tasks.at(i).dDownTime, MF_PARAMETER, MC_Time(""));
    
    DD.Text("");
    DD.Bool("Running", "", &tasks.at(i).bRunning, MF_RESULT);
    DD.Text("");
    DD.ArrayElementEnd();
    }
  DD.ArrayEnd();
}

bool ScheduledMaintenance::ExchangeDataFields()
  {
  if (0 <= DX.Handle - idDX_Description && DX.Handle - idDX_Description < tasks.size())
    {
    if (DX.HasReqdValue)
      tasks.at(DX.Handle - idDX_Description).sDescription = DX.String;
    DX.String = tasks.at(DX.Handle - idDX_Description).sDescription;
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
  return false;
  }
//---------------------------------------------------------------------------

void ScheduledMaintenance::EvalCtrlActions(eScdCtrlTasks Tasks)
  {
  //TODO: Add support for periods other than Simple.
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
        if (tasks.at(i).eType == Type_Simple && dCurrentTime < tasks.at(i).dOffset) return;  //Assume no maintenance before the specifed offset;
        int cycles;

        if (tasks.at(i).eType == Type_Simple)
          cycles = (int)((dCurrentTime - tasks.at(i).dOffset) / tasks.at(i).dPeriod);

        double dTimeInCycle = dCurrentTime - tasks.at(i).dOffset - tasks.at(i).dPeriod * cycles;
        double expectedDownTime = 0;
        switch (eStrategy)
          {
          case(Strat_Warning):
            tasks.at(i).bRunning = dTimeInCycle >= tasks.at(i).dDownTime;
            break;
          case (Strat_Carry):
            tasks.at(i).bRunning = cycles * tasks.at(i).dDownTime < tasks.at(i).dTotalDowntime;
          }

          if (!tasks.at(i).bRunning)
            tasks.at(i).dTotalDowntime += getDeltaTime();
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

void ScheduledMaintenance::SetSize(long size)
  {
  if (size > maxElements) size = maxElements;
  if (size < 0) size = 0;
    
    if (size > tasks.size()) //We want to add elements
      for (int i = tasks.size(); i < size; i++)
        {
        MaintVariables newTask;
        newTask.bRunning = true;
        newTask.dDownTime = 3600;
        newTask.dOffset = 0;
        newTask.dPeriod = 24 * 3600;
        newTask.dTotalDowntime = 0;
        newTask.eType = Type_Simple;
        tasks.push_back(newTask);
        }
    if (size < tasks.size())  //We want to remove elements
      for (int i = tasks.size() - 1; i >= size; i--)
        {
        tasks.pop_back();
        }
  }



