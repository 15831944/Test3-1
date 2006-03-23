// TankMinC++.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tankmin.h"
//#include <iostream>
//#include "optoff.h"

  TankMin::TankMin()
  {
    tanks = 0;
    assays = 0;
  }

  TankMin::~TankMin()
  {
    Clear();
  }

  void TankMin::SetSize(int newtanks, int newassays)
  {
    if (newtanks==0 || newassays==0)
      return;
    if (newtanks==tanks && newassays==assays)
      return;

    if (tanks>0)
    {
      Clear();
    }

    tanks = newtanks;
    assays = newassays;

    tankAssay = new double*[assays];
    for (int i=0; i<assays; i++)
      tankAssay[i] = new double[tanks];

    lowTank = new double[tanks];
    weightLowTank = new double[tanks];
    highTank = new double[tanks];
    weightHighTank = new double[tanks];
    maxTank = new double[tanks];
    weightMaxTank = new double[tanks];
    tempTank = new double[tanks];
    newTank = new double[tanks];
    emptyTank = new bool[tanks];

    lowAssay = new double[assays];
    weightLowAssay = new double[assays];
    highAssay = new double[assays];
    weightHighAssay = new double[assays];

    ratioBoolAssay = new bool*[assays];
    lowRatioAssay = new double*[assays];
    highRatioAssay = new double*[assays];
    weightLowRatioAssay = new double*[assays];
    weightHighRatioAssay = new double*[assays];
    for (int i=0; i<assays; i++)
    {
      ratioBoolAssay[i] = new bool[assays];
      lowRatioAssay[i] = new double[assays];
      highRatioAssay[i] = new double[assays];
      weightLowRatioAssay[i] = new double[assays];
      weightHighRatioAssay[i] = new double[assays];
    }

    costTank = new double[tanks];
    costMaxTank = new double[tanks];
    costAssay = new double[assays];

    costRatioAssay = new double*[assays];
    for (int i=0; i<assays; i++)
      costRatioAssay[i] = new double[assays];

    bestTank = new double[tanks];
    bestAssay = new double[assays];

    assay = new double[assays];

    ratioAssay = new double*[assays];
    for (int i=0; i<assays; i++)
      ratioAssay[i] = new double[assays];

    delta = new double[tanks];
    oldDTank = new double[tanks];
    tank = new double[tanks];

    dTank = new double[tanks];

    InitSolution();
  }

  void TankMin::Clear()
    {
    for (int i=0; i<assays; i++)
      delete []tankAssay[i];
    delete []tankAssay;

    delete []lowTank;
    delete []weightLowTank;
    delete []highTank;
    delete []weightHighTank;
    delete []maxTank;
    delete []weightMaxTank;
    delete []emptyTank;
    delete []tempTank;
    delete []newTank;

    delete []lowAssay;
    delete []weightLowAssay;
    delete []highAssay;
    delete []weightHighAssay;

    for (int i=0; i<assays; i++)
    {
      delete []ratioBoolAssay[i];
      delete []lowRatioAssay[i];
      delete []highRatioAssay[i];
      delete []weightLowRatioAssay[i];
      delete []weightHighRatioAssay[i];
    }
    delete []ratioBoolAssay;
    delete []lowRatioAssay;
    delete []highRatioAssay;
    delete []weightLowRatioAssay;
    delete []weightHighRatioAssay;

    delete []costTank;
    delete []costMaxTank;
    delete []costAssay;

    for (int i=0; i<assays; i++)
      delete []costRatioAssay[i];
    delete []costRatioAssay;

    delete []assay;

    for (int i=0; i<assays; i++)
      delete []ratioAssay[i];
    delete []ratioAssay;


    delete []delta;
    delete []oldDTank;
    delete []tank;

    delete []dTank;
    tanks = 0;
    assays = 0;
    }

  void TankMin::Update()
  {
    double cost = Cost(tank);
    for (int dir=0; dir<tanks; dir++)
    {
      for (int i=0; i<tanks; i++) // calculate dTank
      {
        if (dir==i)
        {
          tempTank[i] = tank[i]+delta[i];
        }
        else
        {
          tempTank[i] = tank[i];
        }
      }
      dTank[dir] = emptyTank[dir] ? 0.0 : cost-Cost(tempTank);
    }

    // Normalize dTank.
    totalTank = 0.0;
    for (int i=0; i<tanks; i++)
      totalTank += abs(dTank[i]);
    if (totalTank>0.0f)
      for (int i=0; i<tanks; i++)
        dTank[i] /= totalTank;

    // Adjust delta's.
    for (int i=0; i<tanks; i++)
    {
      if (dTank[i]*oldDTank[i]>0)
        delta[i] *= 1.1;
      else
        delta[i] *= 0.9;
    }

    // Update oldDTank's
    for (int i=0; i<tanks; i++)
    {
      oldDTank[i] = dTank[i];
    }

    // Make jump.
    for (int i=0; i<tanks; i++)
    {
      newTank[i] = emptyTank[i] ? 0.0 : tank[i] + dTank[i]*delta[i];
      //newTank[i] = tank[i] + dTank[i]*delta[i];
      if (newTank[i]<0.0)
      {
        newTank[i] = 0.0; // can't have value < 0.0...
        delta[i] *= 0.5; // we've overjumped, reduce delta substantially.
      }
      if (newTank[i]>1.0)
      {
        newTank[i] = 1.0; // can't have value > 1.0...
        delta[i] *= 0.5; // we've overjumped, reduce delta substantially.
      }
    }

    // Normalize newTank.
    double total = 0.0;
    for (int i=0; i<tanks; i++)
      total += newTank[i];

    for (int i=0; i<tanks; i++)
      newTank[i] /= total;

    // If better][ update][ else reduce delta's
    cost = Cost(newTank);
    if (cost < bestCost)
    {
      bestCost = cost;
      for (int i=0; i<tanks; i++)
      {
        tank[i] = newTank[i];
        bestTank[i] = tank[i];
      }
      for (int i=0; i<assays; i++)
        bestAssay[i] = assay[i];
    }
    else
    {
      for (int i=0; i<tanks; i++)
      {
        delta[i] *= emptyTank[i] ? 0.0 : 0.5;
      }
    }
  }

  double TankMin::Cost()
  {
    return Cost(this->tank);
  }

  double TankMin::Cost(double* tank)
  {
    // Calculate the cost from the tank usage requirements.
    costTankTtl = 0.0;
    for (int i=0; i<tanks; i++)
    {
      costTank[i] = 0.0;
      if (!emptyTank[i])
        {
        if (tank[i]<lowTank[i])
          costTank[i] += (lowTank[i]-tank[i])*weightLowTank[i];
        if (tank[i]>highTank[i])
          costTank[i] += (tank[i]-highTank[i])*weightHighTank[i];
        if (tank[i]>maxTank[i])
          costTank[i] += (tank[i]-maxTank[i])*weightMaxTank[i];
        costTankTtl += costTank[i];
        }
    }

    costMaxTankTtl = 0.0;
    for (int i=0; i<tanks; i++)
    {
      costMaxTank[i] = 0.0;
      if (!emptyTank[i])
      {
        if (tank[i]>maxTank[i])
        {
          costMaxTank[i] += (tank[i]-maxTank[i])*weightMaxTank[i];
          costMaxTankTtl += costMaxTank[i];
        }
      }
    }

    // Calculate the assay quantities in the result.
    for (int i=0; i<assays; i++)
    {
      assay[i] = 0.0;
      for (int j=0; j<tanks; j++)
      {
        assay[i] += tankAssay[i][j]*tank[j];
      }
    }

    // Calculate the cost from the assay quantity requirements.
    costAssayTtl = 0.0;
    for (int i=0; i<assays; i++)
    {
      costAssay[i] = 0.0;
      if (assay[i]<lowAssay[i])
        costAssay[i] += (lowAssay[i]-assay[i])*weightLowAssay[i];
      if (assay[i]>highAssay[i])
        costAssay[i] += (assay[i]-highAssay[i])*weightHighAssay[i];
      costAssayTtl += costAssay[i];
    }

    // Calculate the cost from the assay ratio requirements.
    costRatioAssayTtl = 0.0;
    for (int i=0; i<assays; i++)
    {
      for (int j=0; j<assays; j++)
      {
        if (ratioBoolAssay[i][j])
        {
          costRatioAssay[i][j] = 0.0;
          ratioAssay[i][j] = assay[i]/assay[j];
          if (ratioAssay[i][j]<lowRatioAssay[i][j])
            costRatioAssay[i][j] += (lowRatioAssay[i][j]-ratioAssay[i][j])*weightLowRatioAssay[i][j];
          if (ratioAssay[i][j]>highRatioAssay[i][j])
            costRatioAssay[i][j] += (ratioAssay[i][j]-highRatioAssay[i][j])*weightHighRatioAssay[i][j];
          costRatioAssayTtl += costRatioAssay[i][j];
        }
      }
    }

    // Sum all the costs.
    double cost = costTankTtl + costMaxTankTtl + costAssayTtl + costRatioAssayTtl;
  
    /*double cost = 0.0;
    for (int i=0; i<tanks; i++)
      cost += costTank[i];
    for (int i=0; i<tanks; i++)
      cost += costMaxTank[i];
    for (int i=0; i<assays; i++)
      cost += costAssay[i];
    for (int i=0; i<assays; i++)
      for (int j=0; j<assays; j++)
        if (ratioBoolAssay[i][j])
          cost += costRatioAssay[i][j];*/

    return cost;
  }

  double TankMin::CostTank()
  {
    double cost = 0.0;
    for (int i=0; i<tanks; i++)
      cost += costTank[i];
    return cost;
  }

  double TankMin::CostMaxTank()
  {
    double cost = 0.0;
    for (int i=0; i<tanks; i++)
      cost += costMaxTank[i];
    return cost;
  }

  double TankMin::CostAssay()
  {
    double cost = 0.0;
    for (int i=0; i<assays; i++)
      cost += costAssay[i];
    return cost;
  }

  double TankMin::CostRatioAssay()
  {
    double cost = 0.0;
    for (int i=0; i<assays; i++)
      for (int j=0; j<assays; j++)
        if (ratioBoolAssay[i][j])
          cost += costRatioAssay[i][j];
    return cost;
  }

  void TankMin::InitSolution()
  {
    for (int i=0; i<tanks; i++)
    {
      delta[i]=0.1;
      oldDTank[i]=0.0;
      tank[i]=0.5;
      bestTank[i]=0.5;
    }
    totalTank = 1.0;
    bestCost = 100000000000000000000000000000000000.0;
  }

  double* TankMin::Go()
  {
    return Go(0.00000000001);
  }

  double* TankMin::Go(double minDelta)
  {
    return Go(minDelta, 10000);
  }

double* TankMin::Go(double minDelta, int maxSteps)
  {
    int step = 0;
    while (totalTank>minDelta)
    {
      Update();
      if (++step>maxSteps)
        break;
    }

    return tank;
  }

/*
void BlendTest()
{
  TankMin* tankMin;

  //Int64 start = DateTime::Now::Ticks;
  int trials=1000;

  for (int trial=0; trial<trials; trial++)
  {
    tankMin = new TankMin(5, 3);

    tankMin->tankAssay[0][0] = 0.5;
    tankMin->tankAssay[0][1] = 0.0;
    tankMin->tankAssay[0][2] = 0.0;
    tankMin->tankAssay[0][3] = 0.0;
    tankMin->tankAssay[0][4] = 0.0;

    tankMin->tankAssay[1][0] = 0.0;
    tankMin->tankAssay[1][1] = 0.7;
    tankMin->tankAssay[1][2] = 0.0;
    tankMin->tankAssay[1][3] = 0.0;
    tankMin->tankAssay[1][4] = 0.0;

    tankMin->tankAssay[2][0] = 0.0;
    tankMin->tankAssay[2][1] = 0.0;
    tankMin->tankAssay[2][2] = 0.9;
    tankMin->tankAssay[2][3] = 0.0;
    tankMin->tankAssay[2][4] = 0.0;

    tankMin->lowTank[0] = 0.0;
    tankMin->lowTank[1] = 0.0;
    tankMin->lowTank[2] = 0.0;
    tankMin->lowTank[3] = 0.0;
    tankMin->lowTank[4] = 0.0;

    tankMin->weightLowTank[0] = 0.0;
    tankMin->weightLowTank[1] = 0.0;
    tankMin->weightLowTank[2] = 0.0;
    tankMin->weightLowTank[3] = 0.0;
    tankMin->weightLowTank[4] = 0.0;

    tankMin->highTank[0] = 1.0;
    tankMin->highTank[1] = 1.0;
    tankMin->highTank[2] = 1.0;
    tankMin->highTank[3] = 1.0;
    tankMin->highTank[4] = 1.0;

    tankMin->weightHighTank[0] = 1.0;
    tankMin->weightHighTank[1] = 1.0;
    tankMin->weightHighTank[2] = 1.0;
    tankMin->weightHighTank[3] = 1.0;
    tankMin->weightHighTank[4] = 1.0;

    tankMin->maxTank[0] = 603.04/200.0;
    tankMin->maxTank[1] = 2614.97/200.0;
    tankMin->maxTank[2] = 200.0/200.0;
    tankMin->maxTank[3] = 201.66/200.0;
    tankMin->maxTank[4] = 231.66/200.0;

    tankMin->weightMaxTank[0] = 10000000.0;
    tankMin->weightMaxTank[1] = 10000000.0;
    tankMin->weightMaxTank[2] = 10000000.0;
    tankMin->weightMaxTank[3] = 10000000.0;
    tankMin->weightMaxTank[4] = 10000000.0;

    tankMin->lowAssay[0] = 0.0;
    tankMin->lowAssay[1] = 0.1;
    tankMin->lowAssay[2] = 0.1;

    tankMin->weightLowAssay[0] = 1.0;
    tankMin->weightLowAssay[1] = 1.0;
    tankMin->weightLowAssay[2] = 1.0;

    tankMin->highAssay[0] = 1.0;
    tankMin->highAssay[1] = 0.1;
    tankMin->highAssay[2] = 0.1;

    tankMin->weightHighAssay[0] = 1.0;
    tankMin->weightHighAssay[1] = 1.0;
    tankMin->weightHighAssay[2] = 1.0;

    tankMin->ratioBoolAssay[0][0] = false;
    tankMin->ratioBoolAssay[0][1] = true;
    tankMin->ratioBoolAssay[0][2] = false;
    tankMin->ratioBoolAssay[1][0] = false;
    tankMin->ratioBoolAssay[1][1] = false;
    tankMin->ratioBoolAssay[1][2] = false;
    tankMin->ratioBoolAssay[2][0] = false;
    tankMin->ratioBoolAssay[2][1] = false;
    tankMin->ratioBoolAssay[2][2] = false;

    tankMin->lowRatioAssay[0][0] = 0.0;
    tankMin->lowRatioAssay[0][1] = 0.3;
    tankMin->lowRatioAssay[0][2] = 0.0;
    tankMin->lowRatioAssay[1][0] = 0.0;
    tankMin->lowRatioAssay[1][1] = 0.0;
    tankMin->lowRatioAssay[1][2] = 0.4;
    tankMin->lowRatioAssay[2][0] = 0.0;
    tankMin->lowRatioAssay[2][1] = 0.0;
    tankMin->lowRatioAssay[2][2] = 0.0;

    tankMin->highRatioAssay[0][0] = 0.0;
    tankMin->highRatioAssay[0][1] = 0.3;
    tankMin->highRatioAssay[0][2] = 0.0;
    tankMin->highRatioAssay[1][0] = 0.0;
    tankMin->highRatioAssay[1][1] = 0.0;
    tankMin->highRatioAssay[1][2] = 0.4;
    tankMin->highRatioAssay[2][0] = 0.0;
    tankMin->highRatioAssay[2][1] = 0.0;
    tankMin->highRatioAssay[2][2] = 0.0;

    tankMin->weightLowRatioAssay[0][0] = 1.0;
    tankMin->weightLowRatioAssay[0][1] = 1.0;
    tankMin->weightLowRatioAssay[0][2] = 1.0;
    tankMin->weightLowRatioAssay[1][0] = 1.0;
    tankMin->weightLowRatioAssay[1][1] = 1.0;
    tankMin->weightLowRatioAssay[1][2] = 1.0;
    tankMin->weightLowRatioAssay[2][0] = 1.0;
    tankMin->weightLowRatioAssay[2][1] = 1.0;
    tankMin->weightLowRatioAssay[2][2] = 1.0;

    tankMin->weightHighRatioAssay[0][0] = 1.0;
    tankMin->weightHighRatioAssay[0][1] = 1.0;
    tankMin->weightHighRatioAssay[0][2] = 1.0;
    tankMin->weightHighRatioAssay[1][0] = 1.0;
    tankMin->weightHighRatioAssay[1][1] = 1.0;
    tankMin->weightHighRatioAssay[1][2] = 1.0;
    tankMin->weightHighRatioAssay[2][0] = 1.0;
    tankMin->weightHighRatioAssay[2][1] = 1.0;
    tankMin->weightHighRatioAssay[2][2] = 1.0;

    tankMin->Go();
  }

//  Int64 stop = DateTime::Now::Ticks;

  std::cout << tankMin->Cost() << std::endl;
  std::cout << tankMin->CostTank() << std::endl;
  std::cout << tankMin->CostMaxTank() << std::endl;
  std::cout << tankMin->CostAssay() << std::endl;
  std::cout << tankMin->CostRatioAssay() << std::endl;
  std::cout << tankMin->delta[0] << "," << tankMin->delta[1] << "," << tankMin->delta[2] << "," << tankMin->delta[3] << "," << tankMin->delta[4] << std::endl;
  //System::Console::WriteLine("[{0,10}, {1,10}, {2,10}, {3,10}, {4,10}]", tankMin->bestTank[0], tankMin->bestTank[1], tankMin->bestTank[2], tankMin->bestTank[3], tankMin->bestTank[4]);
  //System::Console::WriteLine("[{0,10}, {1,10}, {2,10}]", tankMin->bestAssay[0], tankMin->bestAssay[1], tankMin->bestAssay[2]);
  //System::Console::WriteLine("[{0,10}]", tankMin->ratioAssay[0,1]);
  //System::Console::WriteLine();
  //System::Console::WriteLine("{0}", TimeSpan((stop-start)/trials)->ToString());
  //System::Console::WriteLine("{0}", TimeSpan(24*365*3[stop-start)/trials)->ToString()); // every hour for three years.

  //System::Console::ReadLine();

  char a;
  std::cin >> a;
}

int _tmain(int argc, _TCHAR* argv[])
{
  BlendTest();
  return 0;
}
*/
