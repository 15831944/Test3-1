// Generic Base Clase for Shell and tube heat exchanger - contains only geometric
// and material data




class CSTHx 
{
 public:
  CSTHx();
  virtual ~CSTHx();


 protected:

  void doGeometry();
  




  double m_dHTArea;        // Overall Area: clean, inside tube
  double m_dHTExtArea;     // Area based on OD
  double m_dTSFlowArea;    // Flow areas for Reynolds no calcs
  double m_dSSFlowArea;
  


    // ID and OD refer to inside and outside diameters of tubes, pipes etc
  double m_dTubeLength;     // Geometric and construction parameters for detailed
  double m_dTubeID;         // HX calculations
  double m_dTubeOD;
  double m_dTubePlateThk;   // Tube Plate Thickness
  double m_dShellID;        // 
  double m_dShellOTL;       // Shell outer tube limit - diameter of bundle.
  double m_dBaffleSpacing;  
  double m_dtPitch;         // Transverse and longitudinal tube pitches in pattern
  double m_dlPitch;
  double m_dBaffleCut;      // Typically 25% - how much shellside baffle removed
  double m_dTBClearance;    // Tube-Baffle Clearance
  double m_dBSClearance;    // Baffle-Shell Clearance
  long m_lNTubes;           // Number of tubes
  long m_lNPasses;          // Number of passes
  
  double m_dTSVel;
  double m_dThermalConductivity;
  double m_dTubeConductivity;
  double m_dScaleConductivity;

  bool m_bTSScaling;       // include tubeside scaling?
  bool m_bSSScaling;       // shellside scaling?
  
  double m_dScaleThickness;  // Tube scale thickness
  double m_dSScaleThickness; // Shell scale thickness
  // TS and SS refer to TubeSide and ShellSide paramters respectively


};

