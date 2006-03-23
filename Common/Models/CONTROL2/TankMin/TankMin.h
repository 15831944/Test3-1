#include <math.h>

class TankMin
{
public:
  double* delta;

private:
  int tanks;
  int assays;

  //TODO: INSTANT C++ TODO TASK: C++ does not allow initialization of non-static fields in their declarations:
  double totalTank;
public:
  double* tank; // quantity from each tank -- totals to 1.0.
  double* dTank; // partial derivitave of tank.
  double* oldDTank;
  double* lowTank; // lowest preferred quantity from each tank.
  double* highTank; // higheset preferred quantity from each tank.
  double* weightLowTank;
  double* weightHighTank;
  double* maxTank; // maximum possible quantity from each tank.
  bool* emptyTank;
  double* weightMaxTank;
  double* tempTank;
  double* newTank;

  double** tankAssay; // amount of each compound in each tank.
  double* lowAssay; // lowest preferred quantity of each compound.
  double* highAssay; // highest preferred quantity of each compound.
  double* weightLowAssay;
  double* weightHighAssay;
  double* assay; // amount of each compound after mixing.

  bool** ratioBoolAssay;
  double** ratioAssay;
  double** lowRatioAssay;
  double** highRatioAssay;
  double** weightLowRatioAssay;
  double** weightHighRatioAssay;

  double* costTank; // cost function for each tank usage.
  double* costMaxTank; // cost function for each tank maximum.
  double* costAssay; // cost function for each compound.
  double** costRatioAssay;
  double costTankTtl, costMaxTankTtl, costAssayTtl, costRatioAssayTtl;

  double bestCost; // lowest cost so far.
  double* bestTank; // best solution so far.
  double* bestAssay; // best solution so far.

  TankMin();
  ~TankMin();
  void SetSize(int tanks, int assays);
  void Clear();
  void Update();
  double Cost();
  double Cost(double* tank);
  double CostTank();
  double CostMaxTank();
  double CostAssay();
  double CostRatioAssay();
  void InitSolution();
  double* Go();
  double* Go(double minDelta);
  double* Go(double minDelta, int maxSteps);
  };
