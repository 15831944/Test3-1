#include "stdafx.h"
//#include <stdio.h>
//#include <stdlib.h>
extern "C" 
  {
  #include "glpk\\include\\glpk.h" 
  }
#include "sc.h"
//#include "optoff.h"
//#pragma comment(lib, "sc\\glpk.lib")

TankBlendOptimiser::TankBlendOptimiser()
  {
  tanks = 0;
  assays = 0;

  cols = 0;
  rows = 0;
  maxConstraints = 0;
  col = 0;
  row = 0;
  constraint = 0;

  c = 0;

  tank = NULL;
  assay = NULL;
  
  tankMax = NULL;
  tankLow = NULL;   
  tankHigh = NULL;  
  assayLow = NULL;  
  assayHigh = NULL; 
  
  tankLowPenalty = NULL;
  tankHighPenalty = NULL;
  assayLowPenalty = NULL;
  assayHighPenalty = NULL;

  assayConc = NULL;

  assayRatioLow = NULL;
  assayRatioHigh = NULL;
  assayRatioLowEnabled = NULL;
  assayRatioHighEnabled = NULL;

  assayRatioLowPenalty = NULL;
  assayRatioHighPenalty = NULL;

  lp = NULL;
  }

TankBlendOptimiser::~TankBlendOptimiser()
  {
  clean();
  }

void TankBlendOptimiser::SetSize(int newtanks, int newassays)
  {
  if (newtanks==0 || newassays==0)
    return;
  if (newtanks==tanks && newassays==assays)
    return;

  clean();

  tanks = newtanks;
  assays = newassays;

  cols = 3*tanks + 2*assays + 2*assays*assays;
  rows = 1 + 2*tanks + 2*assays + 2*assays*assays;
  maxConstraints = 1 + tanks + 2*tanks + 2*tanks + tanks*assays + assays + tanks*assays + assays + assays*assays*tanks + assays*assays + assays*assays*tanks + assays*assays;

  tank = new double[tanks];    for (int i=0; i<tanks; i++) tank[i] = 0.0;
  assay = new double[assays];  for (int i=0; i<assays; i++) assay[i] = 0.0;

  ia = new int[maxConstraints];
  ja = new int[maxConstraints];
  ar = new double[maxConstraints];

  tankMax = new double[tanks];    for (int i=0; i<tanks; i++) tankMax[i] = 1.0;
  tankLow = new double[tanks];    for (int i=0; i<tanks; i++) tankLow[i] = 0.0;
  tankHigh = new double[tanks];   for (int i=0; i<tanks; i++) tankHigh[i] = 1.0;
  assayLow = new double[assays];  for (int i=0; i<assays; i++) assayLow[i] = 0.0;
  assayHigh = new double[assays]; for (int i=0; i<assays; i++) assayHigh[i] = 1.0;

  tankLowPenalty = new double[tanks];    for (int i=0; i<tanks; i++) tankLowPenalty[i] = 1.0;
  tankHighPenalty = new double[tanks];   for (int i=0; i<tanks; i++) tankHighPenalty[i] = 1.0;
  assayLowPenalty = new double[assays];  for (int i=0; i<assays; i++) assayLowPenalty[i] = 1.0;
  assayHighPenalty = new double[assays]; for (int i=0; i<assays; i++) assayHighPenalty[i] = 1.0;

  assayConc = new double*[assays];
  for (int i=0; i<assays; i++)
    assayConc[i] = new double[tanks];
  for (int i=0; i<assays; i++)
    for (int j=0; j<tanks; j++)
      assayConc[i][j] = 0.0;

  assayRatioLow = new double*[assays];
  for (int i=0; i<assays; i++)
    assayRatioLow[i] = new double[assays];
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      assayRatioLow[i][j] = 0.0;

  assayRatioHigh = new double*[assays];
  for (int i=0; i<assays; i++)
    assayRatioHigh[i] = new double[assays];
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      assayRatioHigh[i][j] = 1.0;

  assayRatioLowEnabled = new bool*[assays];
  for (int i=0; i<assays; i++)
    assayRatioLowEnabled[i] = new bool[assays];
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      assayRatioLowEnabled[i][j] = false;

  assayRatioHighEnabled = new bool*[assays];
  for (int i=0; i<assays; i++)
    assayRatioHighEnabled[i] = new bool[assays];
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      assayRatioHighEnabled[i][j] = false;

  assayRatioLowPenalty = new double*[assays];
  for (int i=0; i<assays; i++)
    assayRatioLowPenalty[i] = new double[assays];
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      assayRatioLowPenalty[i][j] = 1.0;

  assayRatioHighPenalty = new double*[assays];
  for (int i=0; i<assays; i++)
    assayRatioHighPenalty[i] = new double[assays];
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      assayRatioHighPenalty[i][j] = 1.0;
}

void TankBlendOptimiser::clean()
  { 
  if (lp)
    lpx_delete_prob(lp);
  lp = NULL;

  if (tanks>0)
    {
    delete []ia;
    delete []ja;
    delete []ar;

    delete []tankMax;
    delete []tankLow;
    delete []tankHigh;
    delete []assayLow;
    delete []assayHigh;

    delete []tankLowPenalty;
    delete []tankHighPenalty;
    delete []assayLowPenalty;
    delete []assayHighPenalty;

    for (int i=0; i<assays; i++)
      delete []assayConc[i];
    delete []assayConc;

    for (int i=0; i<assays; i++)
      delete []assayRatioLow[i];
    delete []assayRatioLow;

    for (int i=0; i<assays; i++)
      delete []assayRatioHigh[i];
    delete []assayRatioHigh;

    for (int i=0; i<assays; i++)
      delete []assayRatioLowEnabled[i];
    delete []assayRatioLowEnabled;

    for (int i=0; i<assays; i++)
      delete []assayRatioHighEnabled[i];
    delete []assayRatioHighEnabled;

    for (int i=0; i<assays; i++)
      delete []assayRatioLowPenalty[i];
    delete []assayRatioLowPenalty;

    for (int i=0; i<assays; i++)
      delete []assayRatioHighPenalty[i];
    delete []assayRatioHighPenalty;
    }
  tanks = 0;
  assays = 0;
  maxConstraints = 0;
  }

void TankBlendOptimiser::InitSolution()
  {
  col = 1;
  row = 1;
  constraint = 1;
  }

int TankBlendOptimiser::go()
{
  lp = lpx_create_prob();

  lpx_set_int_parm(lp, LPX_K_MSGLEV, 0); // 0 = no output
  lpx_set_int_parm(lp, LPX_K_SCALE, 3); // 3 = geometric mean scaling, then equilibration scaling
  lpx_set_int_parm(lp, LPX_K_DUAL, 1); // 1 = if initial basic solution is dual feasible, use the dual simplex
  lpx_set_int_parm(lp, LPX_K_ROUND, 1); // 1 = replace tiny primal and dual values by exact zero

  lpx_set_int_parm(lp, LPX_K_PRESOL, 1); // 1 = use the built-in presolver.

  lpx_set_prob_name(lp, "Blend Optimiser");
  lpx_set_obj_dir(lp, LPX_MIN);

  // Columns...
  
  lpx_add_cols(lp, cols);

  for (int i=0; i<tanks; i++) // 0.0 < x < tankMax
  {
    if (tankMax[i]>0.0)
      lpx_set_col_bnds(lp, col, LPX_DB, 0.0, tankMax[i]);
    else
      lpx_set_col_bnds(lp, col, LPX_FX, 0.0, 0.0);
    col++;
  }

  for (int i=0; i<tanks; i++) // 0.0 < slackTankLow
  {
    lpx_set_col_bnds(lp,  col, LPX_LO, 0.0, 0.0);
    lpx_set_obj_coef(lp,  col, tankLowPenalty[i]); // penalty weight.
    col++;
  }

  for (int i=0; i<tanks; i++) // 0.0 < slackTankHigh
  {
    lpx_set_col_bnds(lp,  col, LPX_LO, 0.0, 0.0);
    lpx_set_obj_coef(lp,  col, tankHighPenalty[i]); // penalty weight.
    col++;
  }

  for (int i=0; i<assays; i++) // 0.0 < slackAssayLow
  {
    lpx_set_col_bnds(lp,  col, LPX_LO, 0.0, 0.0);
    lpx_set_obj_coef(lp,  col, assayLowPenalty[i]); // penalty weight.
    col++;
  }

  for (int i=0; i<assays; i++) // 0.0 < slackAssayHigh
  {
    lpx_set_col_bnds(lp,  col, LPX_LO, 0.0, 0.0);
    lpx_set_obj_coef(lp,  col, assayHighPenalty[i]); // penalty weight.
    col++;
  }

  for (int i=0; i<assays; i++) // 0.0 < slackAssayRatioLow
    for (int j=0; j<assays; j++)
    {
      lpx_set_col_bnds(lp,  col, LPX_LO, 0.0, 0.0);
      lpx_set_obj_coef(lp,  col, assayRatioLowPenalty[i][j]); // penalty weight.
      col++;
    }

  for (int i=0; i<assays; i++) // 0.0 < slackAssayRatioHigh
    for (int j=0; j<assays; j++)
    {
      lpx_set_col_bnds(lp,  col, LPX_LO, 0.0, 0.0);
      lpx_set_obj_coef(lp,  col, assayRatioHighPenalty[i][j]); // penalty weight.
      col++;
    }

  // Rows...

  lpx_add_rows(lp, rows);

  
  { // x1 + ... + xn = 1.0
  lpx_set_row_bnds(lp, row, LPX_FX, 1.0, 1.0);
  for (int i=0; i<tanks; i++)
    ia[constraint] = row, ja[constraint] = 1+i, ar[constraint++] =  1.0;
  row++;
  }

  for (int i=0; i<tanks; i++) // tankLow < tank + slackTankLow
  {
    lpx_set_row_bnds(lp, row, LPX_LO, tankLow[i], 0.0);
    ia[constraint] = row, ja[constraint] = 1+i, ar[constraint++] = 1.0;
    ia[constraint] = row, ja[constraint] = 1+i+tanks, ar[constraint++] = 1.0;
    row++;
  }  

  for (int i=0; i<tanks; i++) // tank - slackTankHigh < tankHigh
  {
    lpx_set_row_bnds(lp, row, LPX_UP, 0.0, tankHigh[i]);
    ia[constraint] = row, ja[constraint] = 1+i, ar[constraint++] = 1.0;
    ia[constraint] = row, ja[constraint] = 1+i+2*tanks, ar[constraint++] = -1.0;
    row++;
  }  

  for (int i=0; i<assays; i++) // assayLow < assay + slackAssayLow
  {
    lpx_set_row_bnds(lp, row, LPX_LO, assayLow[i], 0.0);
    for (int j=0; j<tanks; j++)
    {
      if (assayConc[i][j]>0.0)
        ia[constraint] = row, ja[constraint] = 1+j, ar[constraint++] = assayConc[i][j];
    }
    ia[constraint] = row, ja[constraint] = 1+i+3*tanks, ar[constraint++] = 1.0;
    row++;
  }  

  for (int i=0; i<assays; i++) // assay - slackAssayHigh < assayHigh
  {
    lpx_set_row_bnds(lp, row, LPX_UP, 0.0, assayHigh[i]);
    for (int j=0; j<tanks; j++)
    {
      if (assayConc[i][j]>0.0)
        ia[constraint] = row, ja[constraint] = 1+j, ar[constraint++] = assayConc[i][j];
    }
    ia[constraint] = row, ja[constraint] = 1+i+3*tanks+assays, ar[constraint++] = -1.0;
    row++;
  }  

  for (int i=0; i<assays; i++) // 0 < assayNum - assayRatioLow*assayDen + slackAssayLow
    for (int j=0; j<assays; j++)
    {
      if (assayRatioLowEnabled[i][j])
        lpx_set_row_bnds(lp, row, LPX_LO, 0.0, 0.0);
      else
        lpx_set_row_bnds(lp, row, LPX_FR, 0.0, 0.0);
      for (int k=0; k<tanks; k++)
      {
        if (assayConc[i][k] - assayRatioLow[i][j]*assayConc[j][k]!=0.0)
          ia[constraint] = row, ja[constraint] = 1+k, ar[constraint++] = assayConc[i][k] - assayRatioLow[i][j]*assayConc[j][k];
      }
      ia[constraint] = row, ja[constraint] = 1+i*assays+j+3*tanks+2*assays, ar[constraint++] = 1.0;
      row++;
    }  

  for (int i=0; i<assays; i++) // assayNum - assayRatioHigh*assayDen - slackAssayHigh < 0
    for (int j=0; j<assays; j++)
    {
      if (assayRatioHighEnabled[i][j])
        lpx_set_row_bnds(lp, row, LPX_UP, 0.0, 0.0);
      else
        lpx_set_row_bnds(lp, row, LPX_FR, 0.0, 0.0);
      for (int k=0; k<tanks; k++)
      {
        if (assayConc[i][k] - assayRatioHigh[i][j]*assayConc[j][k]!=0.0)
          ia[constraint] = row, ja[constraint] = 1+k, ar[constraint++] = assayConc[i][k] - assayRatioHigh[i][j]*assayConc[j][k];
      }
      ia[constraint] = row, ja[constraint] = 1+i*assays+j+3*tanks+2*assays+assays*assays, ar[constraint++] = -1.0;
      row++;
    }  

  lpx_load_matrix(lp, constraint-1, ia, ja, ar);
  int exitCode = lpx_simplex(lp);

  for (int i=0; i<tanks; i++)    
    tank[i] = lpx_get_col_prim(lp, 1+i);

  for (int i=0; i<assays; i++)
  {
    assay[i] = 0.0;
    for (int j=0; j<tanks; j++)
      assay[i] += lpx_get_col_prim(lp, 1+j)*assayConc[i][j];
  }

  return exitCode;

  // LPX_E_OK        200   /* success */
  // LPX_E_FAULT     204   /* unable to start the search */
  // LPX_E_ITLIM     207   /* iterations limit exhausted */
  // LPX_E_TMLIM     208   /* time limit exhausted */
  // LPX_E_SING      211   /* problems with basis matrix */
  // LPX_E_NOPFS     213   /* no primal feas. sol. (LP presolver) */
  // LPX_E_NODFS     214   /* no dual feas. sol. (LP presolver) */

  // Usually:
  // LPX_E_OK = Solution found.
  // LPX_E_NOPFS = Sum-to-1.0 or tank-max constraints not met.
  // Others = Some major fault has occurred.
}

void TankBlendOptimiser::show()
{
/*
  printf("Concentration Values\n");
  for (int i=0; i<assays; i++)
  {
    for (int j=0; j<tanks; j++)
      printf("%g; ", assayConc[i][j]);
    printf("\n");
  }
  printf("\n");


  printf("Tank Values\n");
  printf("Tank Max:  ");
  for (int i=0; i<tanks; i++)    printf("%g; ", tankMax[i]);
  printf("\n");
  printf("Tank High: ");
  for (int i=0; i<tanks; i++)    printf("%g; ", tankHigh[i]);
  printf("\n");
  printf("**** Tank: ");
  for (int i=0; i<tanks; i++)    printf("%g; ", tank[i]);
  printf("\n");
  printf("Tank Low:  ");
  for (int i=0; i<tanks; i++)    printf("%g; ", tankLow[i]);
  printf("\n\n");

  printf("Assay Values\n");
  printf("Assay High: ");
  for (int i=0; i<assays; i++)    printf("%g; ", assayHigh[i]);
  printf("\n");
  printf("**** Assay: ");
  for (int i=0; i<assays; i++)    printf("%g; ", assay[i]);
  printf("\n");
  printf("Assay Low:  ");
  for (int i=0; i<assays; i++)    printf("%g; ", assayLow[i]);
  printf("\n\n");

  printf("Assay Ratio Low Values\n");
  printf("Assay Ratio Num/Den: ");
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      if (assayRatioLowEnabled[i][j])
        printf("%d/%d; ", i, j);
  printf("\n");
  printf("**** Assay Ratio: ");
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      if (assayRatioLowEnabled[i][j])
      {
        double num = 0.0;
        double den = 0.0;
        for (int k=0; k<tanks; k++)
        {
          num += lpx_get_col_prim(lp, 1+k)*assayConc[i][k];
          den += lpx_get_col_prim(lp, 1+k)*assayConc[j][k];
        }
        printf("%g; ", num/den);
      }
  printf("\n");
  printf("Assay Ratio Low:  ");
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      if (assayRatioLowEnabled[i][j])
        printf("%g; ", assayRatioLow[i][j]);
  printf("\n\n");

  printf("Assay Ratio High Values\n");
  printf("Assay Ratio High Num/Den: ");
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      if (assayRatioHighEnabled[i][j])
        printf("%d/%d; ", i, j);
  printf("\n");
  printf("Assay Ratio High: ");
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      if (assayRatioHighEnabled[i][j])
        printf("%g; ", assayRatioHigh[i][j]);
  printf("\n");
  printf("**** Assay Ratio: ");
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      if (assayRatioHighEnabled[i][j])
      {
        double num = 0.0;
        double den = 0.0;
        for (int k=0; k<tanks; k++)
        {
          num += lpx_get_col_prim(lp, 1+k)*assayConc[i][k];
          den += lpx_get_col_prim(lp, 1+k)*assayConc[j][k];
        }
        printf("%g; ", num/den);
      }
  printf("\n\n");

  printf("Penalty Values\n");
  printf("Penalty Tank Low:   ");
  for (int i=0; i<tanks; i++)    printf("%g; ", lpx_get_col_prim(lp, 1+i+tanks));
  printf("\n");
  printf("Penalty Tank High:  ");
  for (int i=0; i<tanks; i++)    printf("%g; ", lpx_get_col_prim(lp, 1+i+2*tanks));
  printf("\n");
  printf("Penalty Assay Low:  ");
  for (int i=0; i<assays; i++)    printf("%g; ", lpx_get_col_prim(lp, 1+i+3*tanks));
  printf("\n");
  printf("Penalty Assay High: ");
  for (int i=0; i<assays; i++)    printf("%g; ", lpx_get_col_prim(lp, 1+i+3*tanks+assays));
  printf("\n");
  printf("Penalty Assay Ratio Low:  ");
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      if (assayRatioLowEnabled[i][j])
        printf("%g; ", lpx_get_col_prim(lp, 1+i*assays+j+3*tanks+2*assays));
  printf("\n");
  printf("Penalty Assay Ratio High: ");
  for (int i=0; i<assays; i++)
    for (int j=0; j<assays; j++)
      if (assayRatioHighEnabled[i][j])
        printf("%g; ", lpx_get_col_prim(lp, 1+i*assays+j+3*tanks+2*assays+assays*assays));
  printf("\n\n");

  printf("Weight Values\n");
  printf("Weight for tank[i]: ");
  for (int i=0; i<tanks; i++)
    printf("%g; ", lpx_get_obj_coef(lp,  1+i));
  printf(" (unused, should be zero)\n");

  printf("Weight Tank Low: ");
  for (int i=0; i<tanks; i++)
    printf("%g; ", lpx_get_obj_coef(lp,  1+i+tanks));
  printf("\n");

  printf("Weight Tank High: ");
  for (int i=0; i<tanks; i++)
    printf("%g; ", lpx_get_obj_coef(lp,  1+i+2*tanks));
  printf("\n");

  printf("Weight Assay Low: ");
  for (int i=0; i<assays; i++)
    printf("%g; ", lpx_get_obj_coef(lp,  1+i+3*tanks));
  printf("\n");

  printf("Weight Assay High: ");
  for (int i=0; i<assays; i++)
    printf("%g; ", lpx_get_obj_coef(lp,  1+i+3*tanks+assays));
  printf("\n");

  printf("Weight Assay Ratio Low:\n");
  for (int i=0; i<assays; i++)
  {
    printf("                        ");
    for (int j=0; j<assays; j++)
      printf("%g; ", lpx_get_obj_coef(lp, 1+i*assays+j+3*tanks+2*assays));
    printf("\n");
  }

  printf("Weight Assay Ratio High:\n");
  for (int i=0; i<assays; i++)
  {
    printf("                         ");
    for (int j=0; j<assays; j++)
      printf("%g; ", lpx_get_obj_coef(lp, 1+i*assays+j+3*tanks+2*assays+assays*assays));
    printf("\n");
  }
  printf("\n");

  printf("Total Weighted Penalty: %g\n", lpx_get_obj_val(lp));
  printf("\n================");

  scanf("%c", &c, 21);
  */
}

