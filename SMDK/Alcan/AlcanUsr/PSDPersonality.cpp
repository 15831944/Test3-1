//===========================================================================
//=== SysCAD SMDK - Hyprod PSD Definition 2006 : Alcan, Kenwalt           ===
//===========================================================================

#include "stdafx.h"

#include "HyprodCommon.h"
#include "PSDPersonality.h"

// --------------------------------------------------------------------------

PSDPersonality::PSDPersonality()
  {
  FracPass = NULL;
  Intervals = NULL;
  NIntervals = 0;

  m_eType = eAlcanPSD_Rosin;

  m_Rqd.m_dMedian  = 50*1e-6;  //dNAN;
  m_Rqd.m_dd10     = dNAN;
  m_Rqd.m_dd90     = dNAN;
  m_Rqd.m_dq1      = dNAN;
  m_Rqd.m_dq3      = dNAN;
  m_Rqd.m_dQS      = 20*1e-6;  //dNAN;
  m_Rqd.m_dCrit    = 1e-6;

  m_Rqd.m_dm149    = dNAN;
  m_Rqd.m_dm106    = dNAN;
  m_Rqd.m_dm74     = dNAN;
  m_Rqd.m_dm53     = dNAN;
  m_Rqd.m_dm45     = dNAN;
  m_Rqd.m_dm33     = dNAN;
  m_Rqd.m_dm20     = dNAN;
  }

// --------------------------------------------------------------------------

PSDPersonality::~PSDPersonality()
{
  if (FracPass != NULL)
    delete[] FracPass;
  if (Intervals != NULL)
    delete[] Intervals;
}
// --------------------------------------------------------------------------

void PSDPersonality::Init(MIPSD *PSD)
  {
  m_PSD = PSD;
  }

// --------------------------------------------------------------------------

void PSDPersonality::BuildDataDefn(MDataDefn & DD)
  {
  DD.ObjectBegin("ASz", "AlcanSz");

  static MDDValueLst DDBPSDTypes[]={
    {eAlcanPSD_Log   , "Logrithmic"},
    {eAlcanPSD_Rosin , "Rosin-Rammler"},
    {eAlcanPSD_Table , "Table"},
    //{eAlcanPSD_File  , "File"},
    {0}};

  DD.Text("");
  DD.Text("Rqd Characteristics");
  DD.Long  ("PSD_Type", "", (long*)&m_eType,  MF_PARAMETER, DDBPSDTypes);
  DD.StructBegin("Rqd");
  DD.Double("d10",      "", &m_Rqd.m_dd10, MF_PARAMETER|MF_NAN_OK, MC_L("um"));
  //DDB.Double("q1",       "", &m_Rqd.m_dq1,      isParm|NAN_OK); not used
  DD.Double("Median",   "", &m_Rqd.m_dMedian,  MF_PARAMETER|MF_NAN_OK, MC_L("um"));
  //DDB.Double("q3",       "", DC_L,    "um", &m_Rqd.m_dq3,  isParm|NAN_OK); not used
  DD.Double("d90",      "", &m_Rqd.m_dd90,     MF_PARAMETER|MF_NAN_OK, MC_L("um"));
  DD.Double("QS",       "", &m_Rqd.m_dQS,      MF_PARAMETER|MF_NAN_OK, MC_L("um"));
  DD.Double("DCrit",    "", &m_Rqd.m_dCrit,    MF_PARAMETER);
  DD.Text("");
  DD.Double("m20",      "", &m_Rqd.m_dm20,     MF_PARAMETER|MF_NAN_OK, MC_Frac("%"));
  DD.Double("m33",      "", &m_Rqd.m_dm33,     MF_PARAMETER|MF_NAN_OK, MC_Frac("%"));
  DD.Double("m45",      "", &m_Rqd.m_dm45,     MF_PARAMETER|MF_NAN_OK, MC_Frac("%"));
  DD.Double("m53",      "", &m_Rqd.m_dm53,     MF_PARAMETER|MF_NAN_OK, MC_Frac("%"));
  DD.Double("m74",      "", &m_Rqd.m_dm74,     MF_PARAMETER|MF_NAN_OK, MC_Frac("%"));
  DD.Double("m106",     "", &m_Rqd.m_dm106,    MF_PARAMETER|MF_NAN_OK, MC_Frac("%"));
  DD.Double("m149",     "", &m_Rqd.m_dm149,    MF_PARAMETER|MF_NAN_OK, MC_Frac("%"));
  DD.StructEnd();
  DD.Text("");

  //DD.Button("Set", "", idDX_Set, MF_PARAMETER);

  DD.ObjectEnd();
  };

// --------------------------------------------------------------------------

void PSDPersonality::ReplaceDistribution(MIPSD *PSDO)
  {
  if ( IsNothing(FracPass)==false )
    PSDO->ReplaceFracVector(FracPass, 0);
  }

// --------------------------------------------------------------------------

bool PSDPersonality::SetDistribution()
  {
  double D25, D75, b, QS1;
  long Count;
  double Diam[MaxHPSizeClasses+1];  //Regression vector
  double wPrct[MaxHPSizeClasses+1]; //Regression vector
  double RegRslt[4];                //Regression result
  //DiamWeightData : variant ;        //Diam et % < que D : used to sort data
  //SortKey : variant ;
  //s1,s2 : string;

  Count = 0 ;
  //Verifie si toutes les donnees PSD sont definie (Voir deuxieme tableau Dlg)
  if (0)//not allPSDDefine 
    {
    //Lecture et verification de la m_dMedian
    if ((m_Rqd.m_dMedian <= 0) && ((m_Rqd.m_dm45 <= 0) || (m_Rqd.m_dm74 <= 0))) 
      {
      //if MessageDlg (hyprod.GetMessage(45), mtInformation,[mbOK], 0) = mrOK 
      //  {
      //    F1Book1.free;
      //    raise EPsdInvalid.Create('');
      //    Exit
      //  };
      //SetCI();
      return false;
      }
    };
  
  //Verification de la médiane
  if (Valid(m_Rqd.m_dMedian)) 
    {
    Diam[Count]  = m_Rqd.m_dMedian;
    wPrct[Count] = 0.50;
    Count++;
    };

  //Verification m_dm45
  if (Valid(m_Rqd.m_dm45) && (m_Rqd.m_dm45 > 0) && (m_Rqd.m_dm45 < 0.9999)) 
    {
    Diam[Count]  = 45*1e-6;
    wPrct[Count] = m_Rqd.m_dm45;
    Count++;
    };
  //Verification m_dm74
  if (Valid(m_Rqd.m_dm74) && (m_Rqd.m_dm74 > 0) && (m_Rqd.m_dm74 < 0.9999)) 
    {
    Diam[Count]  = 74*1e-6;
    wPrct[Count] = m_Rqd.m_dm74;
    Count++;
    };
  //Verification d10
  if (Valid(m_Rqd.m_dd10) && (m_Rqd.m_dd10 > 0)) 
    {
    Diam[Count]  = m_Rqd.m_dd10;
    wPrct[Count] = 0.10;
    Count++;
    };
  //Verification d90
  if (Valid(m_Rqd.m_dd90) && (m_Rqd.m_dd90 > 0)) 
    {
    Diam[Count]  = m_Rqd.m_dd90;
    wPrct[Count] = 0.90;
    Count++;
    };
  /*if (Valid(m_Rqd.m_dq1) && Valid(m_Rqd.m_dq3))
    {
    m_Rqd.m_dQS = m_Rqd.m_dq3 - m_Rqd.m_dq1;
    }*/
  if (Valid(m_Rqd.m_dQS) && Valid(m_Rqd.m_dMedian)) 
    {
    // Verification du QS et determine Q1 Q3
    if (m_eType == eAlcanPSD_Log) 
      {                               // Dist. Log-Normale
      // D75 = ( qs + sqrt( qs*qs + 4*d50*d50)) / 2.0 et D25 = (D75 - qs)
      D75 = (m_Rqd.m_dQS + Sqrt(m_Rqd.m_dQS*m_Rqd.m_dQS + 4*m_Rqd.m_dMedian*m_Rqd.m_dMedian))/ 2.;
      D25 = D75 - m_Rqd.m_dQS;
      Diam[Count]  = D25 ;
      wPrct[Count] = 0.25;
      Count++;
      Diam[Count]  = D75 ;
      wPrct[Count] = 0.75;
      Count++;
      }
    else
      {
      //construire //DiamWeightData pour QS > 0, treatment=1}
      D75 = m_Rqd.m_dMedian + m_Rqd.m_dQS / 2.0;               // initialisation
      do
        {
        b   = 0.69314718 / log(D75/m_Rqd.m_dMedian);
        D25 = exp (log(m_Rqd.m_dMedian) - 0.879386403/b);
        QS1 = D75 - D25 ;
        D75 = D75 - (QS1-m_Rqd.m_dQS) / 2.0;
        }
      while ((fabs((QS1-m_Rqd.m_dQS) / m_Rqd.m_dQS)) > 0.000001);

      Diam[Count]  = D25 ;
      wPrct[Count] = 0.25;
      Count++;
      Diam[Count]  = D75 ;
      wPrct[Count] = 0.75;
      Count++;
      //LogNote("Set PSD", 0, "Diam[0]:%.12g  Diam[1]:%.12g  Diam[2]:%.12g", Diam[0], Diam[1], Diam[2]);      
      }
    };

  // Seulement si toutes les donnees du PSD sont definies
  //(Voir Deuxieme tableau Dlg)
  if (1)//AllPSDDefine 
    {
    if (Valid(m_Rqd.m_dm149) && (m_Rqd.m_dm149 > 0) && (m_Rqd.m_dm149 < 0.9999))
      {
      Diam[Count]  = 149*1e-6;
      wPrct[Count] = m_Rqd.m_dm149;
      Count++;
      };
    if (Valid(m_Rqd.m_dm106) && (m_Rqd.m_dm106 > 0) && (m_Rqd.m_dm106 < 0.9999))
      {
      Diam[Count]  = 106*1e-6;
      wPrct[Count] = m_Rqd.m_dm106;
      Count++;
      };
    if (Valid(m_Rqd.m_dm53) && (m_Rqd.m_dm53 > 0)&& (m_Rqd.m_dm53 < 0.9999))
      {
      Diam[Count]  = 53*1e-6;
      wPrct[Count] = m_Rqd.m_dm53;
      Count++;
      };
    if (Valid(m_Rqd.m_dm33) && (m_Rqd.m_dm33 > 0) && (m_Rqd.m_dm33 <0.9999))
      {
      Diam[Count]  = 33*1e-6;
      wPrct[Count] = m_Rqd.m_dm33;
      Count++;
      };
    if (Valid(m_Rqd.m_dm20) && (m_Rqd.m_dm20 > 0) && (m_Rqd.m_dm20 < 0.9999))
      {
      Diam[Count]  = 20*1e-6;
      wPrct[Count] = m_Rqd.m_dm20;
      Count++;
      };
    };
  if (Count>=2)
    {
    if (Intervals == NULL)
    {
      NIntervals = m_PSD->getSizeCount();
      Intervals = new double[NIntervals];
      m_PSD->ExtractSizes(Intervals);
    }
    if (FracPass == NULL)
      FracPass = new double[NIntervals];
    if (NIntervals != m_PSD->getSizeCount())
      {//number of intervals has changed
      if (Intervals != NULL)
        delete[] Intervals;
      if (FracPass != NULL)
        delete[] FracPass;
      Intervals = new double[NIntervals];
      m_PSD->ExtractSizes(Intervals);
      FracPass = new double[NIntervals];
      }

    switch (m_eType)
      {
      case eAlcanPSD_Log:
        {
        RegressLN (Count, Diam, wPrct, RegRslt);
        HyprodDataFromLN (RegRslt ) ;
        //TableRsltFromLN (RegRslt) ;
        break;
        };
      case eAlcanPSD_Rosin:
        {
        RegressRR(Count, Diam, wPrct, RegRslt);
        HyprodDataFromRR (RegRslt) ;
        //TableRsltFromRR (RegRslt) ;
        break;
        };
      case eAlcanPSD_Table:
        {
        HyprodDataFromTable(Count, Diam, wPrct, RegRslt);
        break;
        }
      }
    }
  else
    {
    //todo LogWarning("Set PSD", 0, "Insufficient data to set PSD");
    }
  return true;
  };

// --------------------------------------------------------------------------

double PSDPersonality::NormInv(double x)
  {
  //Cette fonction retourne l'écart type qui sera dépassé avec une
  //probabilité "x" (>0, <1) lorsque la distribution est normale.
  const double c0 = 2.515517;
  const double c1 = 0.802853;
  const double c2 = 0.010328;
  const double d1 = 1.432788;
  const double d2 = 0.189269;
  const double d3 = 0.001308;

  double t, Y;
  if ((x <= 0 ) || (x >= 1.0))// error 
    {
    //MessageDlg(Hyprod.GetMessage(50), mtError, [mbOK], 0);
    //raise EPsdInvalid.Create('');
    return 0;
    };
  if ((x > 0.0) && (x <= 0.5))
    {
    t = Sqrt(log(1/x/x));
    Y = t - ((c2*t + c1)*t + c0)/(((d3*t + d2)*t +d1)*t +1);
    return -Y;
    }
  else //  x > 0.5 and < 1.0  
    {
    t = Sqrt(log(1/((1-x)*(1-x))));
    Y = t - ((c2*t + c1)*t + c0)/(((d3*t + d2)*t +d1)*t +1);
    return Y;
    }
  return 0;
  }

// --------------------------------------------------------------------------

double PSDPersonality::NormDist(double x)
  {
  //Cette fonction retourne la probabilité P (comprise entre 0 et 1) qu'une variable
  //distribuée normalement soit inférieure à la valeur x (exprimée en écarts types).

  const double r = 0.2316419;
  const double b1 = 0.319381530;
  const double b2 = -0.356563782;
  const double b3 = 1.781477937;
  const double b4 = -1.821255978;
  const double b5 = 1.330274429;

  double t, Y;
  
  t = 1 / (1 + r * fabs(x));
  Y = NormDens(x)*((((b5*t + b4) * t + b3) * t + b2) * t + b1 )*t ;
  return ( x >= 0) ? 1 - Y : Y;
  };

// --------------------------------------------------------------------------

double PSDPersonality::NormDens(double x)
  {
  //Cette fonction calcule la densité f(x) d'une distribution normale N(0,1), c'est-à-dire
  //centrée à t := 0 et d'écart type s := 1. La fonction est évaluée à x écart type de la
  //moyenne.
  return exp(-x*x/2)/Sqrt(2*PI);
  }

// --------------------------------------------------------------------------

void PSDPersonality::RegressXY (const long Count, const double x[], const double y[], double RegRslt[])
  {
  // Retourne les coefficients (a, b) et les statistiques (StdErr, R2) de la
  //regression linéaire y = a + b*x dans le vecteur "RegRslt".
  double SS, Reg, SumX, SumY, SumXY, SumX2, SumY2;
  long i, N;

  SumX = 0.0;
  SumY = 0.0;
  SumXY = 0.0;
  SumX2 = 0.0;
  SumY2 = 0.0;
  N = Count ;
  for (i = 0; i<Count; i++)
    {
    SumX  += x[i];
    SumY  += y[i];
    SumXY += x[i]*y[i];
    SumX2 += x[i]*x[i];
    SumY2 += y[i]*y[i];
    };
  RegRslt[1]  = (SumXY - SumX*SumY/N) / (SumX2 - SumX*SumX/N) ;       // coeff. b
  RegRslt[0]  = (SumY - RegRslt[1]*SumX) / N ;                        // coeff. a
  SS          = SumY2 - SumY*SumY/N ;
  Reg         = (SumXY - SumX*SumY/N)*(SumXY - SumX*SumY/N) / (SumX2 - SumX*SumX/N) ;
  RegRslt[2]  = (N > 3) ? Sqrt( (SS - Reg) / (N - 2) ) : 0;
  RegRslt[3]  = Reg / SS ;                             // R square
  }

// --------------------------------------------------------------------------

void PSDPersonality::RegressLN (const long Count, const double Diam[], const double WPrct[], double RegRslt[])
  {
  double x[MaxHPSizeClasses+1];
  double y[MaxHPSizeClasses+1];
  long   i;

  for (i=0; i<Count; i++)
    {
    x[i] = log10(Diam[i]);
    y[i] = NormInv(WPrct[i]);
    }
  RegressXY (Count, x, y, RegRslt);
  };

// --------------------------------------------------------------------------

void PSDPersonality::RegressRR (const long Count, const double Diam[], const double WPrct[], double RegRslt[])
  {
  double x[MaxHPSizeClasses+1];
  double y[MaxHPSizeClasses+1];
  long   i;

  for (i=0; i<Count; i++)
    {
    x[i] = log10(Diam[i]) ;
    y[i] = log10(log10(1.0/(1.0 - WPrct[i])));
    }
  RegressXY (Count, x, y, RegRslt );
  };

// --------------------------------------------------------------------------

void PSDPersonality::HyprodDataFromLN (double RegRslt[])
  {
  long i;
  double WCumul[MaxHPSizeClasses+1];

  m_PSD->ExtractFracVector(FracPass, 0);

  for (i = 0; i<NIntervals; i++)
    {
    const double D = Intervals[i];
    WCumul[i] = NormDist(RegRslt[0] + RegRslt[1]*log10(D)) ;
    WCumul[i] = Range(0.0, WCumul[i], 1.0);
    FracPass[i] = WCumul[i];
    if (i>0)
      FracPass[i] -= WCumul[i-1];
    }

  }; 

// --------------------------------------------------------------------------

void PSDPersonality::HyprodDataFromRR(double RegRslt[])
  {
  long i;
  double WCumul[MaxHPSizeClasses+1];

  m_PSD->ExtractFracVector(FracPass, 0);

  double Sum = 0.0;
  for (i=0; i<NIntervals; i++)
    {
    //const double DMean = (i==0) ? (Intervals[i] / 2.0) : ((Intervals[i] + Intervals[i-1]) / 2.0);
    //WCumul[i] = (1.0 - 1.0 / (pow(10.0, pow(10.0, (RegRslt[0] + RegRslt[1]*log10(DMean))))));
    const double D = Intervals[i];
    const double a = (RegRslt[0] + RegRslt[1]*log10(D));
    if (a>1.2)
      {//near limit
      WCumul[i] = 1.0;
      }
    else
      {
      WCumul[i] = 1.0 - (1.0 / (pow(10.0, pow(10.0, a))));
      WCumul[i] = Range(0.0, WCumul[i], 1.0);
      }
    if (i==0)
      {//adjust for special case for first size fraction!!! (adjust based on count)
      const double DMean_a = D/2.0;
      const double DMean_b = (D+m_Rqd.m_dCrit)/2.0;
      FracPass[i] = WCumul[0] * pow(DMean_a, 3) / pow(DMean_b, 3);
      }
    else
      {
      FracPass[i] = WCumul[i] - WCumul[i-1];
      }
    Sum += FracPass[i];
    }

  //adjust to ensure sum is exactly 1.0
  double Sum2 = 0.0;
  for (i=NIntervals-1; i>0; i--)
    {
    FracPass[i] += (FracPass[i] * (1.0-Sum));
    if (FracPass[i]<1.0e-10)
      FracPass[i] = 0.0;
    Sum2 += FracPass[i];
    }
  FracPass[0] = Max(0.0, 1.0-Sum2);

  m_PSD->ReplaceFracVector(FracPass, 0);

  };

// --------------------------------------------------------------------------

void PSDPersonality::HyprodDataFromTable(long Count, double Diam[], double WPrct[], double RegRslt[])
  {
  long   i;
  double WCumul[MaxHPSizeClasses+1];

  m_PSD->ExtractFracVector(FracPass, 0);

  bool Flag = true;
  while (Flag)     // sorting of the entries in ascending order of size
    {
    Flag = false;
    for (i=0; i<Count-1; i++)
      {
      if (Diam[i] > Diam[i+1]) 
        {
        double HoldSize =Diam[i];
        double HoldWPrct=WPrct[i];
        Diam[i]=Diam[i+1];
        WPrct[i]=WPrct[i+1];
        Diam[i+1]=HoldSize;
        WPrct[i+1]=HoldWPrct;
        Flag=true;
        }
      }
    }
  double Diamr[MaxHPSizeClasses+1];
  for (i=0; i<Count; i++)
    Diamr[i] = Diam[i]*1.0e6; //convert from m to um

  for (i=Count; i>0; i--)
    {
    Diamr[i] = Diamr[i-1]; // moving the data one position up ...
    WPrct[i] = WPrct[i-1]; 
    }
  Diamr[0]  = 0.000001;	   //  ... in order to insert the point ( 0 ,0)
  WPrct[0]  = 0.0;
  Count++;

  // interpolation of the cumulative distribution...
  i=0; 
  double x1;
  double x2=log10(Diamr[i+1]);
  double y1;
  double y2=log10(log10(1.0000001/(1.0-WPrct[i+1])));
  double nn=2.6; // force the slope  for the first part otherwise give a strange curve aspect 
  double bb=y2 - nn*x2; // ... then calculate the intercept
  double Sum = 0.0;
  for (int mm=0; mm<NIntervals; mm++)
	  {
    double xx = (mm+1)*HPDSize;
    while (xx > Diamr[i+1]) //MAKE SURE THAT WE INTERPOLATE AROUND THE XX POINT 
			{
      i++;
      if ( (Diamr[i+1] - Diamr[i]) < HPDSize )
        i++;
      x1=x2;
      y1=y2;
      if (i < Count-1)
  			{
        x2=log10(Diamr[i+1]);
        y2=log10(log10(1.0000001/(1.000001-WPrct[i+1])));
        if (fabs(x1-x2)<1.0e-18)
			    {
				  //	 Error_Message(22,'WARNING -- ERROR -- WARNING');
				  break;
			    }
        nn=(y1-y2)/(x1-x2);
        bb=y2 - nn*x2;
			  }
      else
        {
        nn=3.0; // fix the slope for the coarser END
        bb=y1 - nn*x1;
        }
			} 
    
    const double d1 = bb + nn*log10(xx);
    WCumul[mm] = 1.0-1.0/pow(10.0, pow(10.0, d1));
    WCumul[mm] = Range(0.0, WCumul[mm], 1.0);
	  if (mm==0)
		  {//adjust for special case for first size fraction!!! (adjust based on count)
		  FracPass[mm] = WCumul[0];
		  }
		else
		  {
		  FracPass[mm] = WCumul[mm] - WCumul[mm-1];
		  }
    if (FracPass[mm]<1.0e-12)
      FracPass[mm] = 0.0;
	  Sum += FracPass[mm];
	  }

  //adjust to ensure sum is exactly 1.0
  double Sum2 = 0.0;
  for (i=NIntervals-1; i>0; i--)
    {
    FracPass[i] += (FracPass[i] * (1.0-Sum));
    Sum2 += FracPass[i];
    }
  FracPass[0] = Max(0.0, 1.0-Sum2);
 };

// --------------------------------------------------------------------------

void PSDPersonality::HyprodDist(long NIntervals, double SizeClass[], double WCumul[])
  {
  const double WFact  = PI*2.42/6*1e-12 ; //      { =  Pi/6 * 2.42 g/cm3 * 10^-12 cm3/µm3}
  const double SFact  = PI*1e-8;          //      { = Pi * 10^-8 cm2/µm2 }
  const double WSFact = WFact/SFact;      //      { = WFact / SFact }

  double PS[MaxHPSizeClasses+1];//, PCumul[MaxHPSizeClasses+1];
  double SCumul, PCumul;
  double SS, WS, DMean, SSurf;

  PCumul = 0.0;
  SCumul = 0.0;
  //WCumul[0] = 0.0 ;
  for (long i=0; i<NIntervals; i++)
    {
    DMean = (i==0) ? (SizeClass[i] / 2.0) : ((SizeClass[i] + SizeClass[i-1]) / 2.0);
    DMean *= 1e6; // to microns
    WS = WCumul[i];
    PS[i] = WS / pow(DMean, 3) / WFact;
    SS = WS / DMean / WSFact;
    PCumul += PS[i];
    SCumul += SS;                     // surface en cm2/g

    }
  //ppg := PCumul[50] ;
  SSurf = SCumul/10000.0;
  } //  {end procedure HyprodDist}

// --------------------------------------------------------------------------
