
class TankBlendOptimiser
{
public:
  int tanks;
  int assays;
  int maxConstraints;

  int cols;
  int rows;
  int col;
  int row;
  int constraint;

  char c;

  double *tank;
  double *assay;

  double *tankMax;
  double *tankLow;   
  double *tankHigh;  
  double *assayLow;  
  double *assayHigh; 

  double *tankLowPenalty;
  double *tankHighPenalty;
  double *assayLowPenalty;
  double *assayHighPenalty;

  double **assayConc;

  double **assayRatioLow;
  double **assayRatioHigh;
  bool   **assayRatioLowEnabled;
  bool   **assayRatioHighEnabled;

  double **assayRatioLowPenalty;
  double **assayRatioHighPenalty;

  LPX *lp;
  int *ia;
  int *ja;
  double *ar;

  TankBlendOptimiser();
  ~TankBlendOptimiser();
  void SetSize(int tanks, int assays);
  void clean();
  void InitSolution();
  int go();
  void show();
};

