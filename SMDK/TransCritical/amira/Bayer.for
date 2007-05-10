      SUBROUTINE BAYER(TempC,InUnits,Pressure,NInComp,InComp,
     $     DPDATA, 
     $     NOutComp, Comp_molkg, Comp_molL,
     $     Comp_molL25, Comp_mpercent, Comp_gL,
     $     NOC,OC,
     $     NGamma,Gamma,
     $     NSI,SI,
     $     NSol, SolML, Solmkg)


C
C      Original call...
C
C      SUBRO BAYER(TempC,InUnits,Pressure,NInComp,InComp,
C     >I_m,I_c,I_c25,P_sat,NOutComp,Comp_molkg,Comp_molL,
C     >Comp_molL25,Comp_mpercent,Comp_gL,Al2O3,TC,TA,NOC,OC,
C     >TempSat,BPE,Cp_Liq,Cp_H2O,Rho_Liq,Rho_H2O,Cp_phi,V_phi,Cp_LiqH2O,
C     >Phi,Aw,NGamma,Gamma,NSI,SI,NSol,SolML,
C     >Solmkg)
C     ******************************************************************
C   Purpose:  TO CALCULATE THE THERMODYNAMIC PROPERTIES OF BAYER LIQUORS
C             USING THE PITZER MODEL.  BAYER version 5.0 (06/11/06)
C   Import:   TempC - Temperature (deg C)
C             InUnits - Specifies concentration units of input components
C                       "0" for MOLALITY at saturation pressure     
C                       "1" for MOLALITY at specified pressure       
C                       "2" for MASS PER CENT at saturation pressure   
C                       "3" for MASS PER CENT at specified pressure  
C                       "4" for MOLARITY (25 C, 1 bar) at saturation pressure
C                       "5" for MOLARITY (25 C, 1 bar) at specified pressure
C                       "6" for g dm-3 (25 C, 1 bar) at saturation pressure
C                       "7" for g dm-3 (25 C, 1 bar) at specified pressure
C                       "8" for MOLARITY (t, p) at saturation pressure        
C                       "9" for MOLARITY (t, p) at specified pressure
C                       "10" for INDUSTRIAL UNITS (25 C, 1 bar) at saturation pressure
C                       "11" for INDUSTRIAL UNITS (25 C, 1 bar) at specified pressure
C                       Components:
C                            for InUnits = 0,1,4,5,8,9:
C                                Al(OH)3
C                                NaOH
C                                NaCl
C                                Na2CO3
C                                Na2SO4
C                                Na Acetate
C                                Na Formate
C                                NaF
C                            for InUnits = 2,3,6,7:
C                                Al2O3
C                                Na2O
C                                Al(OH)3
C                                NaOH
C                                NaCl
C                                Na2CO3
C                                Na2SO4
C                                Na Oxalate 
C                                Na Acetate
C                                Na Formate
C                                NaF
C                            for InUnits = 10,11:
C                                Al2O3
C                                TC
C                                NaCl
C                                TA
C                                Na2SO4
C                                Na Oxalate 
C                                OC(Na Acetate)
C                                OC(Na Formate)
C                                NaF
C             Pressure - Pressure (bar)
C             NInComp - # of input components: NInComp = 
C             InComp - Concentrations of input components 
C                      InComp(1) to InComp(9) for InUnits = 0,1,4,5,8,9,10,11
C                      InComp(1) to InComp(11) for InUnits = 2,3,6,7
C   Export:   I_m - Ionic strength (mol/kg)
C             I_c - Ionic strength (mol/L, at TempC)
C             I_c25 - Ionic strength (mol/L, at 25 C)
C             P_sat - Water vapour pressure of solution
C             NOutComp - # of output components: NOutComp = 9
C             OutCompName - Names of output components:
C                           OutCompName(1) = 'Al(OH)3'
C                           OutCompName(2) = 'NaOH'
C                           OutCompName(3) = 'NaCl'
C                           OutCompName(4) = 'Na2CO3'
C                           OutCompName(5) = 'Na2SO4'
C                           OutCompName(6) = 'Na Oxalate'
C                           OutCompName(7) = 'Na Acetate'
C                           OutCompName(8) = 'Na Formate'
C                           OutCompName(9) = 'NaF'
C             Comp_molkg - Concentrations of output components (mol/kg)
C             Comp_molL - Concentrations of output components (mol/L, at TempC)
C             Comp_molL25 - Concentrations of output components (mol/L, at 25 C)
C             Comp_mpercent - Concentrations of output components (mass percent)
C             Comp_gL - Concentrations of output components (g/L, at 25 C)
C             Al2O3 - g/L, at 25 C
C             TC - [NaOH], g/L Na2     $     NOC,OC,
C             TA - [NaOH] + [Na2CO3], g/L Na2CO3 eq., at 25 C
C             NOC - # of organic components
C             OCName - Names of organic components
C                      OCName(1) = 'Na Acetate' 
C                      OCName(2) = 'Na Formate' 
C             OC - Concentrations of orgamic components, g/L C eq., at 25 C
C             TempSat - Boiling point of solution
C             BPE - Boiling point elevation
C             Cp_Liq - Heat capacity of solution (J K-1 (g solution)-1)
C             Cp_H2O - Heat capacity of water (J K-1 g-1) 
C             Rho_Liq - Density of solution (g/mL)
C             Rho_H2O - Density of water (g/mL)  
C             Cp_phi - Apparent molar heat capacity of solution (J K-1 mol-1)  
C             V_phi - Apparent molar volume of solution (mL mol-1)
C             Cp_LiqH2O - Heat capacity of solution (J K-1 (g water)-1) 
C             Phi - Osmotic coefficient of solution
C             Aw - Water activity
C             NGamma - # of activity coefficients: NGamma = 12
C             GammaName - Names of activity coefficients:
C                         GammaName(2) = 'Na+'      
C                         GammaName(3) = 'OH-'      
C                         GammaName(4) = 'HCO3-'    
C                         GammaName(5) = 'CO3-2'    
C                         GammaName(6) = 'Cl-'      
C                         GammaName(7) = 'SO4-2'    
C                         GammaName(8) = 'Al(OH)4-' 
C                         GammaName(9) = 'C2O4-2'   
C                         GammaName(10) = 'CH3COO-' 
C                         GammaName(11) = 'HCOO-'   
C                         GammaName(12) = 'F-'      
C             Gamma - Conventional activity coefficients: 
C                     Gamma(2) to Gamma(12). (Gamma(1) = Aw)
C             NSI - # of saturation indices: NSI = 10
C             SIName - Names of solids for which saturation indices are calulated:
C                      SIName(1) = 'NaCl'
C                      SIName(2) = 'Na2SO4'
C                      SIName(3) = 'Na2SO4.10H2O'
C                      SIName(4) = 'Na2CO3.10H2O'
C                      SIName(5) = 'Na2CO3.7H2O'
C                      SIName(6) = 'Na2CO3.H2O'
C                      SIName(7) = 'Al(OH)3'
C                      SIName(8) = 'AlOOH'
C                      SIName(9) = 'NaF'
C                      SIName(10) ='Na3FSO4'
C             SI - Saturation indices: SI(1) to SI(10)
C             NSol - # of solids whose solubilities are calculated: NSol = 6
C             SolName - Names of solids whose solubilities are calculated: 
C                       SolName(1) = 'Al(OH)3'
C                       SolName(2) = 'AlOOH'
C                       SolName(3) = 'Na2SO4'
C                       SolName(4) = 'Na2C2O4'
C                       SolName(5) = 'NaF'
C                       SolName(6) = 'Na3FSO4'
C             SolML - Solubility in mol/L (25 C): SolML(1), SolML(2), SolML(4)
C             Solmkg - Solubility in mol/kg Solmkg(1) to Solmkg(6)

  
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)

C      DLL_EXPORT :: BAYER
     
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /ERRM/ IERR
      DOUBLE PRECISION InComp(NInComp+2), I_m, I_c, I_c25



      
      DIMENSION X(NGamma),GAMMA(NGamma),SI(NSI),SolML(NSol),Solmkg(NSol)
      DIMENSION W(NInComp+5),CI(NInComp),CWI(NInComp+2),CMXI(NInComp),
     >CMX25I(NInComp),CG25I(NInComp+2),CI25I(NInComp),C(NInComp)
      
      DIMENSION Comp_molkg(NOutComp),Comp_molL(NOutComp),
     >Comp_molL25(NOutComp),Comp_mpercent(NOutComp),
     >Comp_gL(NOutComp),OC(NOC),CMX(NOutComp)
     
C     Replaced the mishmash of variables in which data is returned by
C     a single array, which will contain all these. This is
C     EQUIVALENCED back to the original variables.
C     

      DIMENSION DPLDATA(19), DPDATA(19)
      EQUIVALENCE (DPLDATA( 1),   I_m      )
      EQUIVALENCE (DPLDATA( 2),   I_c      )
      EQUIVALENCE (DPLDATA( 3),   I_c25    )
      EQUIVALENCE (DPLDATA( 4),   P_Sat    )
      EQUIVALENCE (DPLDATA( 5),   Al2O3    )
      EQUIVALENCE (DPLDATA( 6),   TC       )
      EQUIVALENCE (DPLDATA( 7),   TA       )
      EQUIVALENCE (DPLDATA( 8),   TempSat  )
      EQUIVALENCE (DPLDATA( 9),   BPE      )
      EQUIVALENCE (DPLDATA(10),   Cp_Liq   )
      EQUIVALENCE (DPLDATA(11),   Cp_H2O   )
      EQUIVALENCE (DPLDATA(12),   Rho_Liq  )
      EQUIVALENCE (DPLDATA(13),   Rho_H2O  )
      EQUIVALENCE (DPLDATA(14),   Cp_phi   )
      EQUIVALENCE (DPLDATA(15),   V_phi    )
      EQUIVALENCE (DPLDATA(16),   Cp_LiqH2O)
      EQUIVALENCE (DPLDATA(17),   Phi      )
      EQUIVALENCE (DPLDATA(18),   Aw       )
      EQUIVALENCE (DPLDATA(19),   V25      )




C      CHARACTER*20 OutCompName(NOutComp),OCName(NOC),GammaName(NGamma),
C     >SIName(NSI),SolName(NSol)
      
      IDC = InUnits
      
      CALL INIT(NInComp,NOutComp,NOC,NGamma,NSI,NSol,W,TR,P0,IDC,
     >PLO,PHI,TLO,THI,TSAVE,PSAVE)


      CALL MODELS


  
      CALL FILERD
      
      CALL VOLH2O25(VH2O25,P0,TR)
      
      CALL SETEMP(TempC,T,TSAVE)


      
      IF(IDC.EQ.0.OR.IDC.EQ.2.OR.IDC.EQ.4.OR.IDC.EQ.6.OR.IDC.EQ.8.OR.IDC
     >                                                      .EQ.10) THEN
           CALL SETPST(T,P,PSAVE)
           Pressure = P
      ELSE
           P = Pressure
           PSAVE = P
      ENDIF




      
      CALL PRPH2O(NGamma,T,P,CpH2Okg,Cp_H2O,Rho_H2O,VH2O,BpH2O)
      
      IF (IERR.GT.0) THEN
         InUnits = IERR
         RETURN
      ENDIF
      
      CALL CompIn(NGAMMA,T,P,TR,P0,IDC,NInComp,InComp,W,VH2O25,VH2O,C)
            
      CALL CompOut(NGAMMA,T,P,TR,P0,IDC,NInComp,W,VH2O25,VH2O,C,
     >NOutComp,Comp_molkg,Comp_molL,Comp_molL25,Comp_gL,Comp_mpercent,
     >Al2O3,TC,TA,NOC,OC,I_m,I_c,I_c25,TM,WT,V25,X)

      CALL PRPSLN(NGAMMA,T,P,X,WT,TM,CpH2Okg,VH2O,BpH2O,Cp_Liq,
     >RHO_Liq,Cp_phi,V_phi,Cp_LiqH2O,TempSat,BPE,Gamma,Aw,Phi,NSI,SI,
     >P_sat)

      CALL SOLBY(NGAMMA,NInComp,InUnits,VH2O25,T,P,TR,P0,X,Comp_molkg,
     >Comp_molL25,V25,NSol,SolML,Solmkg)


C     COPY THE LOCAL EQUIVALENCED STUFF BACK TO THE DUMMY ARGUMENT
      DO 10, I=1,19
 10      DPDATA(I) = DPLDATA(I)



      RETURN
      END
      
      
      
      SUBROUTINE INIT(NInComp,NOutComp,NOC,NGamma,NSI,NSol,W,TR,P0,IDC,
     >PLO,PHI,TLO,THI,TSAVE,PSAVE)
 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION W(NInComp+5)
        
C      CHARACTER*20 OutCompName(NOutComp),OCName(NOC),GammaName(NGamma),
C     >SIName(NSI),SolName(NSol)
      
      TR = 2.9815D2
      P0 = 1.D0
      TSAVE = TR
      PSAVE = P0

      W(1) = 18.0153D0
      W(2) = 101.9613D0
      W(3) = 61.9789D0
      W(4) = 78.0036D0
      W(5) = 39.9971D0
      W(6) = 58.4427D0
      W(7) = 105.9887D0
      W(8) = 142.0432D0
      W(9) = 133.9985D0
      W(10) = 82.0344D0
      W(11) = 68.0075D0
      W(12) = 41.9882D0
      W(13) = 24.0220D0
      W(14) = 12.0110D0

C          Permitted P and T ranges

      PLO = 1.D-3 
      PHI = 3.D2
      TLO = -2.D1
      THI = 3.0D2



C      Deleted all this stuff to set component names...
C      OutCompName(1) = 'Al(OH)3'
C
      RETURN
      END
 
     
     
      SUBROUTINE VOLH2O25(VH2O25,P0,TR)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)

      VH2O25 = 1.D0/DNSH2O(P0,TR)
      
      RETURN
      END



      SUBROUTINE SETEMP(TC,T,TSAVE)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)

      T = TC + 273.15D0

C ...   Set T to T

      TSAVE = T
      
      RETURN
      END



      SUBROUTINE SETPST(T,P,PSAVE)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)

      P = PSAT(T)
      IF(P.GT.1.D0) P = PBOIL(T)

C ...  Set P to P

      PSAVE = P

      RETURN
      END



      SUBROUTINE PRPH2O(NGamma,T,P,CpH2Okg,CpH2O,RHOH2O,VH2O,BpH2O)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION X(NGamma)

      DO 1 I = 1, NGamma
         X(I) = 0.D0
1     CONTINUE

      CpH2Okg = CP(NGAMMA,P,T,X)
      CpH2O = 1.D-3*CpH2Okg
      RHOH2O = DNSH2O(P,T)
      VH2O = 1.D0/RHOH2O
      BpH2O = TBOIL(NGAMMA,P,X)

      RETURN
      END



      SUBROUTINE PRPSLN(NGAMMA,T,P,X,WT,TM,CpH2Okg,VH2O,BpH2O,CpT,RHO,
     >Cpphi,Vphi,Cp_LiqH2O,Bp,Bpel,Gamma,Aw,Phi,NSI,SI,P_Sat)

      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA),SI(NSI)

      Cpkg = CP(NGAMMA,P,T,X)
      VT = V(NGAMMA,P,T,X)
      RHO = 1.D-3*WT/VT
      CpT = Cpkg/WT
      Cp_LiqH2O = Cpkg/1.D3
      CALL AC(P,T,X,GAMMA,OSMO)
      IF (TM .GT. 0.D0) THEN
      	   Cpphi = (Cpkg - CpH2Okg)/TM
           Vphi = 1.D3*(VT - VH2O)/TM
           CALL BPSLN(NGAMMA,X,P,BpH2O,Bp,Bpel)
           AW = GAMMA(1)
           PHI = OSMO
           SI(1) = SINACL(NGAMMA,P,T,X)
           SI(2) = SITHEN(NGAMMA,P,T,X)
           SI(3) = SIMIRA(NGAMMA,P,T,X)
           SI(4) = SINATR(NGAMMA,P,T,X)
           SI(5) = SIHEPT(NGAMMA,P,T,X)
           SI(6) = SITHER(NGAMMA,P,T,X)
           SI(7) = SIGIB(NGAMMA,P,T,X)
           SI(8) = SIBHM(NGAMMA,P,T,X)
           SI(9) = SINAF(NGAMMA,P,T,X)
           SI(10) = SIKO(NGAMMA,P,T,X)
      ELSE 
           Bp = BpH2O - 273.15D0
           Bpel = 0.D0
      	   Cpphi = 0.D0
           Vphi = 0.D0
           PHI = 1.D0
           AW = 1.D0
           DO 1, I = 1,NSI
              SI(I) = 0.D0
1          CONTINUE
      ENDIF

      P_Sat = PBOIL(T)*AW

      RETURN
      END



      SUBROUTINE SOLBY(NGAMMA,NInComp,IDC,VH2O25,T,P,TR,P0,X,C,CM25,
     >V25,NSol,SolML,Solmkg)

      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),SolML(NSol),Solmkg(NSol),C(NInComp),
     >CSAVE(NInComp),CMX(NInComp),CM25(NInComp)

      DO 1, I = 1, NSol
         SolML(I) = 0.D0
         SOLmkg(I) = 0.D0
1     CONTINUE      

      DO 4034 I = 1,NInComp
         CSAVE(I) = C(I)
4034  CONTINUE

      TSAVE = T
      PSAVE = P
      SGI = SGIB(NGAMMA,P,T,X)
      C(1) = SGI
      CALL SETMOL(NGamma,NInComp,C,X)
      TSAVE = TR
      PSAVE = P0
      
      V25SGI = V(NGAMMA,P0,TR,X)
      SGIM = SGI/V25SGI
      IF (IDC .LT. 4) GOTO 4160

      DO 4035 I = 1,NInComp
         CMX(I) = CM25(I)
4035  CONTINUE
      CMX(1) = SGIM
      
      VX = VH2O25

4130  VXSAV = VX
      
      DO 4036 I = 1,NInComp
         C(I) = CMX(I)*VX
4036  CONTINUE

      CALL SETMOL(NGamma,NInComp,C,X)
      VX = V(NGAMMA,P0,TR,X)
 
      IF (ABS(VXSAV-VX) .LT. 1.D-7) GOTO 4140

      GOTO 4130
      
4140  SGIMSAV = SGIM

      TSAVE = T
      PSAVE = P
      SGII = SGIB(NGAMMA,P,T,X)
      SGIM = SGII/VX
      CMX(1) = SGIM
      
      IF (ABS(SGIMSAV-SGIM) .LT. 1.D-7) GOTO 4149
      
      VX = VH2O25


4150  VXSAV = VX
      
      DO 4056 I = 1,NInComp
         C(I) = CMX(I)*VX
4056  CONTINUE

      CALL SETMOL(NGamma,NInComp,C,X)
      VX = V(NGAMMA,P0,TR,X)
 
      IF (ABS(VXSAV-VX) .LT. 1.D-7) GOTO 4140

      GOTO 4150
      
4149  SGI = SGIM*VX
4160  CONTINUE

      DO 4141 I = 1,NInComp
         C(I) = CSAVE(I)
4141  CONTINUE
      TSAVE = T
      PSAVE = P
      CALL SETMOL(NGamma,NInComp,C,X)

      SBO = SBHM(NGAMMA,P,T,X)
      C(1) = SBO
      CALL SETMOL(NGamma,NInComp,C,X)
      TSAVE = TR
      PSAVE = P0
      V25SBO = V(NGAMMA,P0,TR,X)
      SBOM = SBO/V25SBO
      IF (IDC .LT. 4) GOTO 5160

      DO 5035 I = 1,NInComp
         CMX(I) = CM25(I)
5035  CONTINUE
      CMX(1) = SBOM
      
      VX = VH2O25

5130  VXSAV = VX
      
      DO 5036 I = 1,NInComp
         C(I) = CMX(I)*VX
5036  CONTINUE

      CALL SETMOL(NGamma,NInComp,C,X)
      VX = V(NGAMMA,P0,TR,X)
 
      IF (ABS(VXSAV-VX) .LT. 1.D-7) GOTO 5140

      GOTO 5130
      
5140  SBOMSAV = SBOM

      TSAVE = T
      PSAVE = P
      SBOI = SBHM(NGAMMA,P,T,X)
      SBOM = SBOI/VX
      CMX(1) = SBOM
      
      IF (ABS(SBOMSAV-SBOM) .LT. 1.D-7) GOTO 5149
      
      VX = VH2O25


5150  VXSAV = VX
      
      DO 5056 I = 1,NInComp
         C(I) = CMX(I)*VX
5056  CONTINUE

      CALL SETMOL(NGamma,NInComp,C,X)
      VX = V(NGAMMA,P0,TR,X)
 
      IF (ABS(VXSAV-VX) .LT. 1.D-7) GOTO 5140

      GOTO 5150
      
5149  SBO = SBOM*VX
5160  CONTINUE

      DO 5141 I = 1,NInComp
         C(I) = CSAVE(I)
5141  CONTINUE
      TSAVE = T
      PSAVE = P
      CALL SETMOL(NGamma,NInComp,C,X)

      Solmkg(1) = SGI
      SolML(1) = SGIM

      Solmkg(2) = SBO
      SolML(2) = SBOM

      Solmkg(3) = STHEN(NGAMMA,P,T,X)
      Solmkg(4) = SNAOX(NGAMMA,P,T,X)
      SolML(4) = Solmkg(4)/V25
      Solmkg(5) = SNAF(NGAMMA,P,T,X)
      Solmkg(6) = SKO(NGAMMA,P,T,X)

      RETURN
      END


      SUBROUTINE BPSLN(NGAMMA,X,P,BpH2O,Bp,Bpel)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION X(NGAMMA)
      
      Bp = TBOIL(NGAMMA,P,X)
      Bpel = Bp - BpH2O
      Bp = Bp - 273.15D0

      RETURN
      END



      SUBROUTINE SETMOL(NGamma,NInComp,C,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION X(NGamma),C(NInComp)

C ...  Set molalities

      X(2) = C(2)+C(3)+C(7)+C(8)+C(9)+2.D0*(C(4)+C(5)+C(6))
      X(3) = C(2) - C(1)
      X(4) = 0.D0
      X(5) = C(4)
      X(6) = C(3)
      X(7) = C(5)
      X(8) = C(1)
      X(9) = C(6)
      X(10) = C(7)
      X(11) = C(8)
      X(12) = C(9)

      RETURN
      END



      SUBROUTINE CompIn(NGAMMA,T,P,TR,P0,IDC,NInComp,InComp,W,VH2O25,
     >VH2O,C)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DOUBLE PRECISION InComp(NInComp+2)
      
      DIMENSION W(NInComp+5),C(NInComp),X(NGAMMA)
      
      DIMENSION CW(NInComp+2),CMX(NInComp),CG25(NInComp+2),CI25(NInComp)

      IF (IDC .EQ. 0 .OR. IDC .EQ. 1) THEN
      
           DO 1014 I = 1,NInComp
              C(I) = InComp(I)
1014       CONTINUE               
     	   
      ELSEIF (IDC .EQ. 2 .OR. IDC .EQ. 3) THEN

           DO 1018 I = 1,NInComp+2
              CW(I) = InComp(I)
1018       CONTINUE      

           CORR = (CW(2)/W(3) + 3.D0*CW(1)/W(2))*W(1)
           FAKTOR = 1.D2 - CORR
           DO 1019 I = 1,NInComp+2
              FAKTOR = FAKTOR - CW(I)
1019       CONTINUE              
           
      	   FAKTOR = 1.D3/FAKTOR
      	   
      	   C(1) = (CW(3)/W(4) + 2.D0*CW(1)/W(2))*FAKTOR
      	   C(2) = (CW(4)/W(5) + 2.D0*CW(2)/W(3))*FAKTOR
           C(3) = CW(5)/W(6)*FAKTOR
      	   C(4) = CW(6)/W(7)*FAKTOR
      	   C(5) = CW(7)/W(8)*FAKTOR
           C(6) = CW(8)/W(9)*FAKTOR
      	   C(7) = CW(9)/W(10)*FAKTOR
      	   C(8) = CW(10)/W(11)*FAKTOR
      	   C(9) = CW(11)/W(12)*FAKTOR
      	   
      ELSEIF (IDC .EQ. 4 .OR. IDC .EQ. 5) THEN

           DO 1023 I = 1,NInComp
              CMX(I) = InComp(I)
              C(I) = InComp(I)
1023       CONTINUE      

      ELSEIF (IDC .EQ. 6 .OR. IDC .EQ. 7) THEN

           DO 1025 I = 1,NInComp+2
              CG25(I) = InComp(I)
1025       CONTINUE      

      	   C(1) = (CG25(3)/W(4) + 2.D0*CG25(1)/W(2))
           C(2) = (CG25(4)/W(5) + 2.D0*CG25(2)/W(3))
           C(3) = CG25(5)/W(6)
      	   C(4) = CG25(6)/W(7)
      	   C(5) = CG25(7)/W(8)
           C(6) = CG25(8)/W(9)
      	   C(7) = CG25(9)/W(10)
      	   C(8) = CG25(10)/W(11)
      	   C(9) = CG25(11)/W(12)
      	   
           DO 1027 I = 1,NInComp
              CMX(I) = C(I)
1027       CONTINUE      

      ELSEIF (IDC .EQ. 8 .OR. IDC .EQ. 9) THEN

           DO 1031 I = 1,NInComp
              C(I) = InComp(I)
              CMX(I) = InComp(I)
1031       CONTINUE      
      	   
      ELSEIF (IDC .EQ. 10 .OR. IDC .EQ. 11) THEN

           DO 1033 I = 1,NInComp
              CI25(I) = InComp(I)
1033       CONTINUE      

      	   C(1) = 2.D0*CI25(1)/W(2)
           C(2) = 2.D0*CI25(2)/W(7) 
      	   C(3) = CI25(3)/W(6)
      	   C(4) = (CI25(4) - CI25(2))/W(7)
      	   C(5) = CI25(5)/W(8)
           C(6) = CI25(6)/W(9)
      	   C(7) = CI25(7)/W(13)
      	   C(8) = CI25(8)/W(14)
      	   C(9) = CI25(9)/W(12)

           DO 1035 I = 1,NInComp
              CMX(I) = C(I)
1035       CONTINUE      

      ENDIF

      CALL SETMOL(NGamma,NInComp,C,X)

      NIT = 1
      IF (IDC .GE. 4) THEN
           IF(IDC.GE.8.AND.IDC.LE.9) THEN
                VX = VH2O
                TX = T
                PX = P
           ELSE
                VX = VH2O25
                TX = TR
                PX = P0
           ENDIF
          
C ...  Set P to PX and T to TX

           PSAVE = PX
           TSAVE = TX

130        VXSAV = VX

C ...  Calculate VX

           VX = V(NGAMMA,PX,TX,X)

           IF (ABS(VXSAV-VX) .LT. 1.D-7) GOTO 140
           
           DO 1036 I = 1,NInComp
              C(I) = CMX(I)*VX
1036       CONTINUE

           CALL SETMOL(NGamma,NInComp,C,X)
                         
      	   NIT = NIT + 1
           GOTO 130
      ENDIF
      
140   RETURN
      END



      SUBROUTINE CompOut(NGAMMA,T,P,TR,P0,IDC,NInComp,W,VH2O25,VH2O,C,
     >NOutComp,Comp_molkg,Comp_molL,Comp_molL25,Comp_gL,Comp_mpercent,
     >Al2O3,TC,TA,NOC,OC,I_m,I_c,I_c25,TM,WT,V25,X)
      
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DOUBLE PRECISION I_m,I_c,I_c25

      DIMENSION C(NInComp),Comp_molkg(NOutComp),Comp_molL(NOutComp),
     >Comp_molL25(NOutComp),Comp_mpercent(NOutComp),X(NGAMMA),
     >Comp_gL(NOutComp),OC(NOC),CMX(NOutComp),W(NInComp+5)
     
      TM = C(2) + C(3) + C(4) + C(5) + C(6) + C(7) + C(8) + C(9)
      I_m = C(2) + C(3) + C(7) + C(8) + C(9)
     >+ 3.D0*(C(4) + C(5) + C(6))

      WT = 1.D3 
      DO 1038 I = 1, NInComp
         WT = WT + C(I)*W(I+3)
1038  CONTINUE
      DO 1039 I = 1, NOutComp
         Comp_mpercent(I) = 1.D2*C(I)*W(I+3)/WT
1039  CONTINUE

      CALL SETMOL(NGamma,NInComp,C,X)
      PSAVE = P0
      TSAVE = TR
      V25 = V(NGAMMA,P0,TR,X)
      PSAVE = P
      TSAVE = T
      VT = V(NGAMMA,P,T,X)

      DO 1040 I = 1,NOutComp
         Comp_molkg(I) = C(I)
         Comp_molL25(I) = C(I)/V25
         Comp_molL(I) = C(I)/VT
         Comp_gL(I) = Comp_molL25(I)*W(I+3)
1040  CONTINUE

      I_c = I_m/VT
      I_c25 = I_m/V25
 
      Al2O3 = Comp_molL25(1)*W(2)/2.D0
      TC = Comp_molL25(2)*W(7)/2.D0
      TA = TC + Comp_gL(4)
      OC(1) = Comp_molL25(7)*W(13)
      OC(2) = Comp_molL25(8)*W(14)
      
      RETURN
      END
      
      
      BLOCK DATA THINIT
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /THDTA/CH,TERM,GS,VS,NC
      DIMENSION CH(12),TERM(12,12,0:12,6),GS(20,6),VS(20)
 
      DATA TERM/11232*0.D0/

      DATA (CH(I),I=2,12)/1.0D0,-1.0D0,-1.0D0,-2.0D0,-1.0D0,-2.0D0,
     >-1.0D0,-2.0D0,-1.0D0,-1.0D0,-1.0D0/

      DATA ((GS(I,J),J=1,6),I=1,20)/  -948934.81D0,   101.57972D0,
     >87.900000D0,  0.00000000D0,     0.00000000D0,  0.00000000D0,
     >-4080068.1D0,      560.98089D0,
     >550.31000D0,  0.00000000D0,     0.00000000D0,  0.00000000D0,    
     >-3199133.3D0,      423.19989D0,                             
     >420.00000D0,  0.00000000D0,     0.00000000D0,  0.00000000D0,    
     >-1428240.7D0,      173.66163D0,
     >145.60000D0,  0.00000000D0,     0.00000000D0,  0.00000000D0,    
     >-1130258.2D0,      133.80843D0,                             
     >112.30000D0,  0.00000000D0,     0.00000000D0,  0.00000000D0,    
     >-2681110.0D0,      305.13000D0,                             
     >270.00000D0,  0.00000000D0,     0.00000000D0,  0.00000000D0,    
     >-3982030.0D0,      435.33000D0,                             
     >371.00000D0,  0.00000000D0,     0.00000000D0,  0.00000000D0,    
     >-292816.00D0,      44.529000D0,                             
     >36.246000D0,  0.00000000D0,     0.00000000D0,  0.00000000D0,    
     >-411220.00D0,      71.970000D0,                             
     >45.151000D0,  0.17974000D-01,   0.00000000D0,  0.00000000D0,    
     >-997690.00D0,      162.51000D0,
     >137.00000D0,  0.00000000D0,     0.00000000D0,  0.00000000D0,    
     >-4327350.0D0,      594.90000D0,                             
     >88.550000D0,   1.6296000D0,     0.00000000D0,  0.00000000D0,    
     >-1388000.0D0,      149.58000D0,                             
     >109.42000D0,  0.11391000D0,     0.00000000D0,  -1385200.0D0,    
     >-1381200.0D0,      160.13000D0,                             
     >139.05000D0,  0.61641000D-01,   0.00000000D0,  0.00000000D0,    
     >-1293130.0D0,      68.440000D0,                             
     >54.697400D0,  0.17027210D0,     0.00000000D0,  -1221728.0D0,    
     >-1404721.5D0,      135.55965D0,                             
     >51.637533D0,  0.00000000D0,     0.00000000D0,  0.00000000D0,    
     >-708811.44D0,      123.0096D0,                              
     >79.9144D0,    0.00000000D0,     0.00000000D0,  0.00000000D0,    
     >-1603308.8D0,      242.672D0,                               
     >0.00000000D0, 0.00000000D0,     0.00000000D0,  0.00000000D0,    
     >-995500.00D0,      37.1900D0,                                              
     >28.0701120D0, 0.14318980D0,    -8.8155695D-5,  -781106.77D0,
     >-576370.00D0,      51.4632D0,    
     >43.749695D0,  0.22617781D-01,  -.13342855D-05, -298255.11D0,    
     >-1969750.0D0,      205.900D0,    
     >100.49280D0,  0.27871763D0,     0.00000000D0,  -340493.09D0/    

      DATA VS/0.100000D-09,0.100000D-09,0.100000D-09,0.100000D-09,                                          
     >0.100000D-09,0.100000D-09,0.100000D-09,19.6500D0,27.0150D0,
     >58.0000D0,220.080D0,53.0000D0,53.0000D0,31.9560D0,0.100000D-09,                                          
     >0.100000D-09,0.100000D-09,19.90D0,15.00000D0,68.6700D0/
     
      END


      SUBROUTINE FILERD
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /THDTA/CH,TERM,GS,VS,NC
      DIMENSION CH(12),TERM(12,12,0:12,6),GS(20,6),
     >VS(20)

      TERM(3,6,0,1) = -123.94856D0
      TERM(4,6,0,1) =  88.995065D0 
      TERM(5,6,0,1) = -131.38547D0
      TERM(2,5,6,1) =  92.956222D0
      TERM(2,5,6,2) = -.19539100D0
      TERM(2,3,6,1) = -111.31422D0
      TERM(2,3,6,2) = 0.42649219D0
      TERM(2,3,6,4) = -.34551971D-3
      TERM(2,4,6,1) = -35.449288D0  
      TERM(3,8,0,1) =  34.705600D0
      TERM(2,3,8,1) = -11.899060D0  
      TERM(6,8,0,1) = -136.34000D0
      TERM(6,7,0,1) =  74.369000D0  
      TERM(3,7,0,1) = -32.227000D0  
      TERM(2,3,7,1) = -97.196600D0  
      TERM(2,3,7,2) = 0.25110000D0  
      TERM(6,9,0,1) =   123.9000D0  

      RETURN
      END
      
      
      
      DOUBLE PRECISION FUNCTION PARVAL(ITX1,ITX2,ITX3,T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /THDTA/CH,TERM,GS,VS,NC
      DIMENSION CH(12),TERM(12,12,0:12,6),GS(20,6),
     >VS(20)
      PARVAL1 = TERM(ITX1,ITX2,ITX3,1) +
     >          TERM(ITX1,ITX2,ItX3,2)*T +
     >          TERM(ITX1,ITX2,ITX3,3)*T*LOG(T) 
      PARVAL2 = TERM(ITX1,ITX2,ITX3,4)*T*T +
     >          TERM(ITX1,ITX2,ITX3,5)*T*T*T +
     >          TERM(ITX1,ITX2,ITX3,6)/T
      PARVAL = (PARVAL1 + PARVAL2)/(8.31451D0*T)
      RETURN
      END
      
      
      
      SUBROUTINE AC(P,T,X,GAMMA,OSMO)

C ...  GAMMA(1) = AW

      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /THDTA/CH,TERM,GS,VS,NC
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION CH(12),TERM(12,12,0:12,6),X(12),
     >A1(12,12),GS(20,6),A2(12,12),
     >WTX(12,12,0:12),GSUM(12),GAMMA(12),VS(20)
      CHARACTER*1, EFLG

      NION = 11
      TC = T - 2.7315D2
      NC = NION + 1
      DO 10010 I = 1, NC
          GSUM(I) = 0.D0
          GAMMA(I) = 0.D0
10010 CONTINUE
      
      TM = 0.D0               
      SI = 0.D0
      DO 10020 I = 2, NC
         TM = TM + X(I)
         SI = SI + 0.5D0*CH(I)*CH(I)*X(I)
10020 CONTINUE
      SQSI = SQRT(SI)

      EFLG = 'Y'
      DO 10050 I = 1,NC
         DO 10040 J = 1,NC
            A1(I,J) = 0.D0
            A2(I,J) = 0.D0
            DO 10030 K = 0,NC
               WTX(I,J,K) = 0.D0
10030       CONTINUE
10040    CONTINUE
10050 CONTINUE

      APHI = SIPDHL(P,T)
      CALL PTZPAR(NION,SQSI,P,TC,A1,A2,WTX)
      CALL PTZGXS(NION,X,APHI,WTX,EFLG,A1,A2,GSUM,GAMMA,OSMO)
c      print*,osmo
c      pause
      GAMMA(1) = EXP(-OSMO*TM/5.550837D1)

      RETURN
      END
      
      
      
      DOUBLE PRECISION FUNCTION GAQ(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      GNaOH = 0.D0
      GNaCl = 0.D0
      GNaAl = 0.D0
      GNaAc = 0.D0
      GNaForm = 0.D0
      GNaF = 0.D0
      GNaSO4 = 0.D0
      GNaCO3 = 0.D0 
      RT = 8.31451D0*T
      CALL AC(P,T,X,GAMMA,OSMO)
      
      IF(GAMMA(1).GT.0.D0) GH2O = RT*(G0H2O(P,T) + LOG(GAMMA(1)))
      IF(X(2).GT.0.D0) THEN
      IF(X(3).GT.0.D0) GNaOH = RT*(G0NaOH(P,T) + LOG(X(3)*GAMMA(3)) 
     > + LOG(X(2)*GAMMA(2)))
      IF(X(6).GT.0.D0) GNaCl = RT*(G0NaCl(P,T) + LOG(X(6)*GAMMA(6))
     > + LOG(X(2)*GAMMA(2)))
      IF(X(8).GT.0.D0) GNaAl = RT*(G0NaAlOH4(P,T) + LOG(X(8)*GAMMA(8))
     > + LOG(X(2)*GAMMA(2)))
      IF(X(10).GT.0.D0) GNaAc = RT*(G0NaAc(P,T) + LOG(X(10)*GAMMA(10))
     > + LOG(X(2)*GAMMA(2)))
      IF(X(11).GT.0.D0) GNaForm=RT*(G0NaForm(P,T)+LOG(X(11)*GAMMA(11))
     > + LOG(X(2)*GAMMA(2)))
      IF(X(12).GT.0.D0) GNaF = RT*(G0NaF(P,T)+LOG(X(12)*GAMMA(12))
     > + LOG(X(2)*GAMMA(2)))
      IF(X(7).GT.0.D0) GNaSO4 = RT*(G0Na2SO4(P,T) + LOG(X(7)*GAMMA(7))
     > + 2.D0*LOG(X(2)*GAMMA(2)))
      IF(X(5).GT.0.D0) GNaCO3 = RT*(G0Na2CO3(P,T) + LOG(X(5)*GAMMA(5))
     > + 2.D0*LOG(X(2)*GAMMA(2)))
      ENDIF
      
      GAQ1 = (1.D3/18.0153D0)*GH2O+X(3)*GNaOH+X(5)*GNaCO3+X(6)*GNaCl 
      GAQ2 = X(7)*GNaSO4+X(8)*GNaAl+X(10)*GNaAc+X(11)*GNaForm
      GAQ = GAQ1 + GAQ2 + X(12)*GNaF
     
      RETURN
      END
  


      DOUBLE PRECISION FUNCTION GSOL(K,P,T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /THDTA/CH,TERM,GS,VS,NC
      DIMENSION CH(12),TERM(12,12,0:12,6),GS(20,6),
     >VS(20),CP(14)
      RT = 8.31451D0*T
      DO 10010 I = 1, 14
         CP(I) = 0.D0
10010 CONTINUE      
      H = GS(K,1)
      S = GS(K,2)
      CP(1) = GS(K,3)
      CP(4) = GS(K,4)
      CP(5) = GS(K,5)
      CP(10) = GS(K,6)
      GSOL = HSCPXX(P,T,H,S,CP) + 1.D-1*(P-1.D0)*VS(K)/RT
      RETURN
      END
      
       
      
      DOUBLE PRECISION FUNCTION STHEN(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /ERRM/ IERR

      DIMENSION X(NGAMMA)
      
      PARAMETER (S1 = 1.D-8)
      PARAMETER (S2 = 5.D0)
      PARAMETER (JMAX = 100)
      PARAMETER (SACC = 1.D-6)
      
      XNASAV = X(2)
      XSO4SAV = X(7)
      
      X(2) = XNASAV - 2.D0*(XSO4SAV - S1)
      X(7) = S1
      FMID = SITHEN(NGAMMA,P,T,X) - 1.D0
      X(2) = XNASAV - 2.D0*(XSO4SAV - S2)
      X(7) = S2
      F = SITHEN(NGAMMA,P,T,X) - 1.D0
C      IF (F*FMID.GE.0) STOP 'Problem in STHEN'
      IF (F*FMID.GE.0) THEN
           STHEN = 1.D10
           X(2) = XNASAV
           X(7) = XSO4SAV
           RETURN
      ENDIF
      IF (F.LT.0.D0) THEN
           SBIS = S1
           DS = S2 - S1
      ELSE
           SBIS = S2
           DS = S1 - S2
      ENDIF
      DO 10010 J = 1, JMAX
         DS = DS/2.D0
         SMID = SBIS + DS
         X(2) = XNASAV - 2.D0*(XSO4SAV - SMID)
         X(7) = SMID
         FMID = SITHEN(NGAMMA,P,T,X) - 1.D0
         IF (FMID.GT.0.D0) SBIS = SMID
         IF (ABS(DS/SBIS).LT.SACC .OR. SMID.EQ.0.D0) THEN
              STHEN = SBIS
              X(2) = XNASAV
              X(7) = XSO4SAV
              RETURN
         ENDIF
10010 CONTINUE
      STOP 'STHEN DID NOT CONVERGE'
      

      END               
         
      

      DOUBLE PRECISION FUNCTION SKO(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /ERRM/ IERR

      DIMENSION X(NGAMMA)
      
      PARAMETER (S1 = 1.D-8)
      PARAMETER (S2 = 3.D0)
      PARAMETER (JMAX = 100)
      PARAMETER (SACC = 1.D-6)
      
      XNASAV = X(2)
      XSO4SAV = X(7)
      XFSAV = X(12)
      
      IF(XFSAV.LT.XSO4SAV) THEN
           X(2) = XNASAV + 3.D0*(S1 - XFSAV)
           X(7) = XSO4SAV - XFSAV + S1
           X(12) = S1
      ELSE
           X(2) = XNASAV + 3.D0*(S1 - XSO4SAV)
           X(7) = S1
           X(12) = XFSAV - XSO4SAV + S1
      ENDIF
      FMID = SIKO(NGAMMA,P,T,X) - 1.D0
      IF(XFSAV.LT.XSO4SAV) THEN
           X(2) = XNASAV + 3.D0*(S2 - XFSAV)
           X(7) = XSO4SAV - XFSAV + S2
           X(12) = S2
      ELSE
           X(2) = XNASAV + 3.D0*(S2 - XSO4SAV)
           X(7) = S2
           X(12) = XFSAV - XSO4SAV + S2
      ENDIF
      F = SIKO(NGAMMA,P,T,X) - 1.D0
C      IF (F*FMID.GE.0) STOP 'Problem in SKO'
      IF (F*FMID.GE.0) THEN
           SKO = 1.D10
           X(2) = XNASAV
           X(7) = XSO4SAV
           X(12) = XFSAV
           RETURN
      ENDIF
      IF (F.LT.0.D0) THEN
           SBIS = S1
           DS = S2 - S1
      ELSE
           SBIS = S2
           DS = S1 - S2
      ENDIF
      DO 10010 J = 1, JMAX
         DS = DS/2.D0
         SMID = SBIS + DS
         IF(XFSAV.LT.XSO4SAV) THEN
              X(2) = XNASAV + 3.D0*(SMID - XFSAV)
              X(7) = XSO4SAV - XFSAV + SMID
              X(12) = SMID
         ELSE
              X(2) = XNASAV + 3.D0*(SMID - XSO4SAV)
              X(7) = SMID
              X(12) = XFSAV - XSO4SAV + SMID
         ENDIF
         FMID = SIKO(NGAMMA,P,T,X) - 1.D0
         IF (FMID.GT.0.D0) SBIS = SMID
         IF (ABS(DS/SBIS).LT.SACC .OR. SMID.EQ.0.D0) THEN
              SKO = SBIS
              X(2) = XNASAV
              X(7) = XSO4SAV
              X(12) = XFSAV
              RETURN
         ENDIF
10010 CONTINUE
      STOP 'SKO DID NOT CONVERGE'
      END               
         
      

      DOUBLE PRECISION FUNCTION SNAF(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /ERRM/ IERR
      DIMENSION X(NGAMMA)
      
      PARAMETER (S1 = 1.D-8)
      PARAMETER (S2 = 3.0D0)
      PARAMETER (JMAX = 100)
      PARAMETER (SACC = 1.D-6)
      
      XNASAV = X(2)
      XFSAV = X(12)
      
      X(2) = XNASAV - (XFSAV - S1)
      X(12) = S1
      FMID = SINAF(NGAMMA,P,T,X) - 1.D0
      X(2) = XNASAV - (XFSAV - S2)
      X(12) = S2
      F = SINAF(NGAMMA,P,T,X) - 1.D0
C      IF (F*FMID.GE.0) STOP 'Problem in SNAF'
      IF (F*FMID.GE.0) THEN
           SNAF = 1.D10
           X(2) = XNASAV
           X(12) = XFSAV
           RETURN
      ENDIF
      IF (F.LT.0.D0) THEN
           SBIS = S1
           DS = S2 - S1
      ELSE
           SBIS = S2
           DS = S1 - S2
      ENDIF
      DO 10010 J = 1, JMAX
         DS = DS/2.D0
         SMID = SBIS + DS
         X(2) = XNASAV - (XFSAV - SMID)
         X(12) = SMID
         FMID = SINAF(NGAMMA,P,T,X) - 1.D0
         IF (FMID.GT.0.D0) SBIS = SMID
         IF (ABS(DS/SBIS).LT.SACC .OR. SMID.EQ.0.D0) THEN
              SNAF = SBIS
              X(2) = XNASAV
              X(12) = XFSAV
              RETURN
         ENDIF
10010 CONTINUE
      STOP 'SNAF DID NOT CONVERGE'
      END               
         
      

      DOUBLE PRECISION FUNCTION SNAOX(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /ERRM/ IERR
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      PARAMETER (S1 = 1.D-8)
      PARAMETER (S2 = 3.D0)
      PARAMETER (JMAX = 100)
      PARAMETER (SACC = 1.D-6)
      
      XNASAV = X(2)
      
      X(2) = XNASAV + 2.D0*S1
      X(9) = S1
      CALL AC(P,T,X,GAMMA,OSMO)
      FMID = G0Na2SO4(P,T)+LOG(X(9)*GAMMA(9))+2.D0*LOG(X(2)*GAMMA(2)) -
     >  GSOL(15,P,T)
      X(2) = XNASAV + 2.D0*S2
      X(9) = S2
      CALL AC(P,T,X,GAMMA,OSMO)
      F = G0Na2SO4(P,T)+LOG(X(9)*GAMMA(9))+2.D0*LOG(X(2)*GAMMA(2)) -
     >  GSOL(15,P,T)
C      IF (F*FMID.GE.0) STOP 'Problem in SNAOX'
      IF (F*FMID.GE.0) THEN
           SNAOX = 1.D10
           X(2) = XNASAV
           X(9) = 0.D0
           RETURN
      ENDIF
      IF (F.LT.0.D0) THEN
           SBIS = S1
           DS = S2 - S1
      ELSE
           SBIS = S2
           DS = S1 - S2
      ENDIF
      DO 10010 J = 1, JMAX
         DS = DS/2.D0
         SMID = SBIS + DS
         X(2) = XNASAV + 2.D0*SMID
         X(9) = SMID
         CALL AC(P,T,X,GAMMA,OSMO)
         FMID = G0Na2SO4(P,T)+LOG(X(9)*GAMMA(9))+2.D0*LOG(X(2)*GAMMA(2))
     >   - GSOL(15,P,T)
         IF (FMID.GT.0.D0) SBIS = SMID
         IF (ABS(DS/SBIS).LT.SACC .OR. SMID.EQ.0.D0) THEN
              SNAOX = SBIS
              X(2) = XNASAV
              X(9) = 0.D0
              RETURN
         ENDIF
10010 CONTINUE
      STOP 'SNAOX DID NOT CONVERGE'
      END               
          
      

      DOUBLE PRECISION FUNCTION SGIB(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /ERRM/ IERR
      DIMENSION X(NGAMMA)
      
      PARAMETER (S1 = 1.D-8)
      PARAMETER (JMAX = 100)
      PARAMETER (SACC = 1.D-7)
      
      IF (X(3).LE.1.D-8 .AND. X(8).LE.1.D-8) THEN
           SGIB = 0.D0
           RETURN
      ENDIF
      
      XOHSAV = X(3)
      XAlOH4SAV = X(8)
      S2 = XOHSAV + XALOH4SAV - 1.D-8
      
      X(3) = XOHSAV + XAlOH4SAV - S1
      X(8) = S1
      FMID = SIGIB(NGAMMA,P,T,X) - 1.D0
      X(3) = XOHSAV + XAlOH4SAV - S2
      X(8) = S2
      F = SIGIB(NGAMMA,P,T,X) - 1.D0
C      IF (F*FMID.GE.0) STOP 'Problem in SGIB'
      IF (F*FMID.GE.0) THEN
           SGIB = 1.D10
           X(3) = XOHSAV
           X(8) = XAlOH4SAV
           RETURN
      ENDIF
      IF (F.LT.0.D0) THEN
           SBIS = S1
           DS = S2 - S1
      ELSE
           SBIS = S2
           DS = S1 - S2
      ENDIF
      DO 10010 J = 1, JMAX
         DS = DS/2.D0
         SMID = SBIS + DS
         X(3) = XOHSAV + XAlOH4SAV - SMID
         X(8) = SMID
         FMID = SIGIB(NGAMMA,P,T,X) - 1.D0
         IF (FMID.GT.0.D0) SBIS = SMID
         IF (ABS(DS/SBIS).LT.SACC .OR. SMID.EQ.0.D0) THEN
              SGIB = SBIS
              X(3) = XOHSAV
              X(8) = XAlOH4SAV
              RETURN
         ENDIF
10010 CONTINUE
      STOP 'SGIB DID NOT CONVERGE'
      END               
         
      

      DOUBLE PRECISION FUNCTION SBHM(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /ERRM/ IERR
      DIMENSION X(NGAMMA)
      
      PARAMETER (S1 = 1.D-8)
      PARAMETER (JMAX = 100)
      PARAMETER (SACC = 1.D-7)
      
      IF (X(3).LE.1.D-8 .AND. X(8).LE.1.D-8) THEN
           SBHM = 0.D0
           RETURN
      ENDIF
      
      XOHSAV = X(3)
      XAlOH4SAV = X(8)
      S2 = XOHSAV + XALOH4SAV - 1.D-8
      
      X(3) = XOHSAV + XAlOH4SAV - S1
      X(8) = S1
      FMID = SIBHM(NGAMMA,P,T,X) - 1.D0
      X(3) = XOHSAV + XAlOH4SAV - S2
      X(8) = S2
      F = SIBHM(NGAMMA,P,T,X) - 1.D0
C      IF (F*FMID.GE.0) STOP 'Problem in SBHM'
      IF (F*FMID.GE.0) THEN
           SBHM = 1.D10
           X(3) = XOHSAV
           X(8) = XAlOH4SAV
           RETURN
      ENDIF
      IF (F.LT.0.D0) THEN
           SBIS = S1
           DS = S2 - S1
      ELSE
           SBIS = S2
           DS = S1 - S2
      ENDIF
      DO 10010 J = 1, JMAX
         DS = DS/2.D0
         SMID = SBIS + DS
         X(3) = XOHSAV + XAlOH4SAV - SMID
         X(8) = SMID
         FMID = SIBHM(NGAMMA,P,T,X) - 1.D0
         IF (FMID.GT.0.D0) SBIS = SMID
         IF (ABS(DS/SBIS).LT.SACC .OR. SMID.EQ.0.D0) THEN
              SBHM = SBIS
              X(3) = XOHSAV
              X(8) = XAlOH4SAV
              RETURN
         ENDIF
10010 CONTINUE
      STOP 'SBHM DID NOT CONVERGE'
      END               
         
      

      DOUBLE PRECISION FUNCTION SITHEN(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      CALL AC(P,T,X,GAMMA,OSMO)
      IF(X(2).GT.0.D0.AND.X(7).GT.0.D0) THEN
           GTHEN = G0Na2SO4(P,T) + LOG(X(7)*GAMMA(7))
     >                          + 2.D0*LOG(X(2)*GAMMA(2)) - GSOL(12,P,T)
           SITHEN = EXP(GTHEN)
      ELSE
           SITHEN = 0.D0
      ENDIF
      RETURN
      END

      
      
      DOUBLE PRECISION FUNCTION SINACL(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      CALL AC(P,T,X,GAMMA,OSMO)
      IF(X(2).GT.0.D0.AND.X(6).GT.0.D0) THEN
           GNACL = G0NaCl(P,T) + LOG(X(6)*GAMMA(6))
     >                          + LOG(X(2)*GAMMA(2)) - GSOL(9,P,T)
           SINACL = EXP(GNACL)
      ELSE
           SINACL = 0.D0
      ENDIF
      RETURN
      END

      
      
      DOUBLE PRECISION FUNCTION SINAF(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      CALL AC(P,T,X,GAMMA,OSMO)
      IF(X(2).GT.0.D0.AND.X(12).GT.0.D0) THEN
           GNAF = G0NaF(P,T) + LOG(X(12)*GAMMA(12))
     >                          + LOG(X(2)*GAMMA(2)) - GSOL(19,P,T)
           SINAF = EXP(GNAF)
      ELSE
           SINAF = 0.D0
      ENDIF
      RETURN
      END

      
      
      DOUBLE PRECISION FUNCTION SIKO(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      CALL AC(P,T,X,GAMMA,OSMO)
      IF(X(2).GT.0.D0.AND.X(12).GT.0.D0.AND.X(7).GT.0.D0) THEN
           GKO = G0NaF(P,T) + G0Na2SO4(P,T) + LOG(X(12)*GAMMA(12))
     >     +LOG(X(7)*GAMMA(7))+3.D0*LOG(X(2)*GAMMA(2))-GSOL(20,P,T)
           SIKO = EXP(GKO)
      ELSE
           SIKO = 0.D0
      ENDIF
      RETURN
      END

      
      
      DOUBLE PRECISION FUNCTION SIGIB(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      IF(X(3).EQ.0.D0) X(3) = 1.D-8
      CALL AC(P,T,X,GAMMA,OSMO)
      IF(X(8).GT.0.D0) THEN
           GGIB = G0NaAlOH4(P,T) - G0NaOH(P,T) + LOG(X(8)*GAMMA(8))
     >                          - LOG(X(3)*GAMMA(3)) - GSOL(14,P,T)
           SIGIB = EXP(GGIB)
      ELSE
           SIGIB = 0.D0
      ENDIF
      RETURN
      END

      
      
      DOUBLE PRECISION FUNCTION SIBHM(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      IF(X(3).EQ.0.D0) X(3) = 1.D-8
      CALL AC(P,T,X,GAMMA,OSMO)
      IF(X(8).GT.0.D0) THEN
           GBHM = G0NaAlOH4(P,T) - G0NaOH(P,T) - G0H2O(P,T)  
     >     - LOG(GAMMA(1)) + LOG(X(8)*GAMMA(8)) - LOG(X(3)*GAMMA(3))
     >     - GSOL(18,P,T)
           SIBHM = EXP(GBHM)
      ELSE
           SIBHM = 0.D0
      ENDIF
      RETURN
      END

      
      
      DOUBLE PRECISION FUNCTION SIMIRA(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      CALL AC(P,T,X,GAMMA,OSMO)
      IF(X(2).GT.0.D0.AND.X(7).GT.0.D0) THEN
           GMIRA = G0Na2SO4(P,T) + 1.D1*G0H2O(P,T) + LOG(X(7)*GAMMA(7))  
     >     + 1.D1*LOG(GAMMA(1)) + 2.D0*LOG(X(2)*GAMMA(2)) - GSOL(11,P,T)
           SIMIRA = EXP(GMIRA)
      ELSE
           SIMIRA = 0.D0
      ENDIF
      RETURN
      END

      
      
      DOUBLE PRECISION FUNCTION SINATR(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      CALL AC(P,T,X,GAMMA,OSMO)
      IF(X(2).GT.0.D0.AND.X(5).GT.0.D0) THEN
           GNATR = G0Na2CO3(P,T) + 1.D1*G0H2O(P,T) + LOG(X(5)*GAMMA(5))  
     >     + 1.D1*LOG(GAMMA(1)) + 2.D0*LOG(X(2)*GAMMA(2)) - GSOL(2,P,T)
           SINATR = EXP(GNATR)
      ELSE
           SINATR = 0.D0
      ENDIF
      RETURN
      END

      
      
      DOUBLE PRECISION FUNCTION SIHEPT(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      CALL AC(P,T,X,GAMMA,OSMO)
      IF(X(2).GT.0.D0.AND.X(5).GT.0.D0) THEN
           GHEPT = G0Na2CO3(P,T) + 7.D0*G0H2O(P,T) + LOG(X(5)*GAMMA(5))  
     >     + 7.D0*LOG(GAMMA(1)) + 2.D0*LOG(X(2)*GAMMA(2)) - GSOL(3,P,T)
           SIHEPT = EXP(GHEPT)
      ELSE
           SIHEPT = 0.D0
      ENDIF
      RETURN
      END

      
      
      DOUBLE PRECISION FUNCTION SITHER(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      CALL AC(P,T,X,GAMMA,OSMO)
      IF(X(2).GT.0.D0.AND.X(5).GT.0.D0) THEN
           GTHER = G0Na2CO3(P,T) + G0H2O(P,T) + LOG(X(5)*GAMMA(5))  
     >     + LOG(GAMMA(1)) + 2.D0*LOG(X(2)*GAMMA(2)) - GSOL(4,P,T)
           SITHER = EXP(GTHER)
      ELSE
           SITHER = 0.D0
      ENDIF
      RETURN
      END

      
      
      DOUBLE PRECISION FUNCTION V(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION X(NGAMMA)

C      unit: dm3
      
      DP = 1.D-3*P
      PH = P + DP
      PL = P - DP
      V = (GAQ(NGAMMA,PH,T,X)-GAQ(NGAMMA,PL,T,X))/(2.D2*DP)

      RETURN
      END
      
            
            
      DOUBLE PRECISION FUNCTION VGAS(P,T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      
C      units: m3/mol
      
      DP = 1.D-4*P
      PH = P + DP
      PL = P - DP
      VGAS = 8.31451D0*T*(GH2OVAP(PH,T)-GH2OVAP(PL,T))/(2.D5*DP)

      RETURN
      END
      
            
            
      DOUBLE PRECISION FUNCTION SGAS(P,T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      
C      units: J/(K mol)
      
      TH = T + 1.D-3
      TL = T - 1.D-3
      SGAS = -8.31451D0*(TH*GH2OVAP(P,TH)-TL*GH2OVAP(P,TL))/(2.D-3)

      RETURN
      END
      
            
            
      DOUBLE PRECISION FUNCTION CP(NGAMMA,P,T,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION X(NGAMMA)
      
      CP = 0.D0            
      TSAVE = T
      TSAV1 = 1.D0/TSAVE   
      DO 10010 K = -2, 2, 4
         TDIFF = 0.1D0*DBLE(K)
         TDIFF1 = 1.D0/TDIFF
         T = TSAVE + 0.5D0*TDIFF
         G = GAQ(NGAMMA,P,T,X)
         TEMP = T*TDIFF1
         CP = CP + TEMP*TSAV1*G/T    
         T = TSAVE + TDIFF
         G = GAQ(NGAMMA,P,T,X)
         CP = CP - TEMP*G*(TSAV1 + TDIFF1)/T
10010 CONTINUE
      T = TSAVE
      G = GAQ(NGAMMA,P,T,X)
      CP = (CP + 50.D0*G)*T
      
      RETURN
      END            
      
      
      
      DOUBLE PRECISION FUNCTION TBOIL(NGAMMA,P,X)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /ERRM/ IERR
      DIMENSION X(NGAMMA),GAMMA(NGAMMA)
      
      PARAMETER (JMAX = 100)
      PARAMETER (TACC = 1.D-5)
      
      T1 = TSAT(P)
      CALL AC(P,T1,X,GAMMA,OSMO)
      F1 = GH2OVAP(P,T1) 
      F2 = G0H2O(P,T1) + LOG(GAMMA(1))
      F = F1 - F2
10000 IF(F.LE.0.D0) THEN
           T2 = T1 - 2.D0
           TBIS = T1
           DT = T2 - T1
      ELSE
           T2 = T1 + 2.D0
           TBIS = T2
           DT = T1 - T2
      ENDIF
      CALL AC(P,T2,X,GAMMA,OSMO)
      FMID = GH2OVAP(P,T2) - G0H2O(P,T2) - LOG(GAMMA(1))
      IF(F*FMID.GE.0.D0) THEN
           T1 = T1 + 2.D0
           GOTO 10000
      ENDIF
      DO 10010 J = 1, JMAX
         DT = DT/2.D0
         TMID = TBIS + DT
         CALL AC(P,TMID,X,GAMMA,OSMO)
         FMID1 = GH2OVAP(P,TMID) 
         FMID2 = G0H2O(P,TMID) + LOG(GAMMA(1))
         FMID = FMID1 - FMID2
         IF(FMID.LE.0.D0) TBIS = TMID
         IF(ABS(DT).LT.TACC .OR. FMID.EQ.0.D0) THEN
              TBOIL = TMID
              RETURN
         ENDIF
10010 CONTINUE
      STOP 'TBOIL DID NOT CONVERGE'
      END         
      
      
      
      DOUBLE PRECISION FUNCTION PBOIL(T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /ERRM/ IERR

C ...  Calculates saturated vapour pressure of pure H2O from Gibbs energies
      
      PARAMETER (JMAX = 100)
      PARAMETER (PACC = 1.D-6)
      
      P1 = PSAT1(T)
      F1 = GH2OVAP(P1,T) 
      F2 = G0H2O(P1,T)
      F = F1 - F2
      IF(F.LE.0.D0) THEN
           P2 = P1*1.01D0
           PBIS = P2
           DP = P1 - P2
      ELSE
           P2 = P1*0.99D0
           PBIS = P1
           DP = P2 - P1
      ENDIF
      FMID = GH2OVAP(P2,T) - G0H2O(P2,T) 
      IF(F*FMID.GE.0.D0) STOP 'ERROR IN PBOIL'
      DO 10010 J = 1, JMAX
         DP = DP/2.D0
         PMID = PBIS + DP
         FMID1 = GH2OVAP(PMID,T) 
         FMID2 = G0H2O(PMID,T)
         FMID = FMID1 - FMID2
         IF(FMID.GE.0.D0) PBIS = PMID
         IF(ABS(DP/PMID).LT.PACC .OR. FMID.EQ.0.D0) THEN
              PBOIL = PMID
              RETURN
         ENDIF
10010 CONTINUE
      STOP 'PBOIL DID NOT CONVERGE'
      END 
      
      
      DOUBLE PRECISION FUNCTION PSAT1(T)
      IMPLICIT DOUBLE PRECISION(A-H,N-Z)            
C      
C     IAPWS-IF97
C      
      DIMENSION N(10)
      DATA N /0.11670521452767D4,-0.72421316703206D6,
     >       -0.17073846940092D2, 0.12020824702470D5,
     >       -0.32325550322333D7, 0.14915108613530D2,
     >       -0.48232657361591D4, 0.40511340542057D6,
     >       -0.23855557567849D0, 0.65017534844798D3/

      TH = T + N(9)/(T-N(10))
      A = TH*(TH + N(1)) + N(2)
      B = TH*(TH*N(3) + N(4)) + N(5)
      C = TH*(TH*N(6) + N(7)) + N(8)
      PS = (2.D0*C/(SQRT(B*B-4.D0*A*C)-B))**4.D0
      PSAT1 = 1.D1*PS

      RETURN
      END


                    


      SUBROUTINE PTZFNC(NT,IT1,IT2,IT3,WT,EFLG,ALPHA1,
     +                     ALPHA2,X,CH,NC,APHI,SION,TMI,GINT,G,OSMO)
C     **************************************************************

C   Purpose:  TO PROVIDE CONTRIBUTIONS FROM A GIVEN INTERACTION TO
C             THE EXCESS PARTIAL GIBBS ENERGY OF ALL PHASE CONSTITUENTS,
C             USING THE PITZER MODEL
C   Import:   NT - ORDER OF THE GIVEN INTERACTION
C             IT1 - INDEX NUMBER OF AN INTERACTING CONSTITUENT
C             IT2 - INDEX NUMBER OF AN INTERACTING CONSTITUENT
C             IT3 - INDEX NUMBER OF AN INTERACTING CONSTITUENT
C                   (= CLASSIFICATION NUMBER FOR BINARY INTERACTIONS)
C             WT - DIMENSIONLESS EXCESS GIBBS ENERGY PARAMETER OF THE GI
C                  INTERACTION
C             X - MOLALITY OF ALL SOLUTES
C             CH - CHARGE OF ALL SOLUTES
C             NC - NUMBER OF PHASE CONSTITUENTS FOR THE PHASE
C             APHI - PARAMETER A(PHI) OF THE DEBYE-HUECKEL LIMITING LAW
C             SION - IONIC STRENGTH
C             TMI - SUM OF SOLUTE MOLALITIES
C   Export:   (DEFAULT VALUE = 0)
C             GINT - CONTRIBUTION TO THE EXCESS PARTIAL GIBBS ENERGY FRO
C                    DEBYE-HUECKEL TERM AFTER THIS HAS BEEN DIVIDED BY A
C                    CHARGE SQUARED
C             G - CONTRIBUTION TO THE EXCESS PARTIAL GIBBS ENERGY OF ALL
C                 PHASE CONSTITUENTS FROM THE GIVEN INTERACTION
C             OSMO - CONTRIBUTION TO THE OSMOTIC COEFFICIENT FROM THE DE
C                    HUECKEL TERM (NT = 0) OR FROM THE GIVEN INTERACTION
C                    (NT > 0)
C   Created:  EK/PM/KM (30/8/01) Copyright reserved
C   Modified: Version 7 (18/9/01)

      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      CHARACTER*(*) EFLG
      DIMENSION CH(*), G(*), X(*)
      DIMENSION AI(0:20), AII(0:20), BP(0:20), CHP(3), DP(0:20), XX(3)

      SAVE SION2,SQSION,ZUSAZ

      DATA AI            /1.925154014814667D0,-.060076477753119D0,
     >-.029779077456514D0,-.007299499690937D0,0.000388260636404D0,
     >0.000636874599598D0,0.000036583601823D0,-.000045036975204D0,
     >-.000004537895710D0,0.000002937706971D0,0.000000396566462D0,
     >-.000000202099617D0,-.000000025267769D0,0.000000013522610D0,
     >0.000000001229405D0,-.000000000821969D0,-.000000000050847D0,
     >0.000000000046333D0,0.000000000001943D0,-.000000000002563D0,
     >-.000000000010991D0/
      DATA AII           /0.628023320520852D0,0.462762985338493D0,
     >0.150044637187895D0,-.028796057604906D0,-.036552745910311D0,
     >-.001668087945272D0,0.006519840398744D0,0.001130378079086D0,
     >-.000887171310131D0,-.000242107641309D0,0.000087294451594D0,
     >0.000034682122751D0,-.000004583768938D0,-.000003548684306D0,
     >-.000000250453880D0,0.000000216991779D0,0.000000080779570D0,
     >0.000000004558555D0,-.000000006944757D0,-.000000002849257D0,
     >0.000000000237816D0/

Code ...

      IF (NT .EQ. 0) THEN

C ...    The section below is for initialization

      IF (SION .EQ. 0.D0) RETURN

      SION2 = SION + SION
      SQSION = SQRT(SION)
      TEMP = -APHI*SION2
      TEMP1 = 1.2D0*SQSION + 1.D0
      GINT = 2.D0*TEMP*LOG(TEMP1)/1.2D0
      OSMO = TEMP*SQSION/TEMP1
      SION2 = 1.D0/SION2
      ZUSAZ = 0.D0
      DO 10010 I = 2, NC
         ZUSAZ = ZUSAZ + X(I)*ABS(CH(I))
10010 CONTINUE

      IF (NC .GT. 2 .AND. EFLG .EQ. 'Y') THEN

      DP(20) = 0.D0
      TEMP1 = 6.D0*APHI*SQSION
      TEMP2 = 1.D0/9.D0
      DO 10200 I = 2, NC-1
         IF (CH(I) .NE. 0.D0 .AND. X(I) .GT. 0.D0) THEN
              XX(2) = TEMP1*CH(I)*CH(I)
              DO 10110 K = I+1, NC
                 IF (CH(K) .EQ. 0.D0 .OR. X(K) .EQ. 0.D0) THEN
                 ELSEIF (SIGN(CH(K),CH(I)) .EQ. CH(K)) THEN
                      CHP(2) = -0.25D0*SION2*CH(I)*CH(K)*X(I)*X(K)
                      CHP(1) = -CHP(2) - CHP(2)
                      CHP(3) = CHP(2)
                      TEMP = TEMP1*CH(K)
                      XX(1) = TEMP*CH(I)
                      XX(3) = TEMP*CH(K)
                      FIJ = 0.D0
                      DO 10070 J = 1, 3
                         IF (XX(J) .GT. 1.D0) THEN
                              XLN = -0.1D0*LOG(XX(J))
                              ZPITZ = TEMP2*(40.D0*EXP(XLN) - 22.D0)
                              DZDX = -TEMP2*EXP(11.D0*XLN)
                              BP(20) = AII(20)
                              BP(19) = ZPITZ*AII(20) + AII(19)
                              DP(19) = AII(20)
                              DO 10030 L = 18, 0, -1
                                 L1 = L + 1
                                 L2 = L + 2
                                 BP(L) = ZPITZ*BP(L1) - BP(L2) + AII(L)
                                 DP(L) = BP(L1) + ZPITZ*DP(L1) - DP(L2)
10030                         CONTINUE
                         ELSE
                              XLN = 0.2D0*LOG(XX(J))
                              ZPITZ = 4.D0*EXP(XLN) - 2.D0
                              DZDX = 0.2D0*EXP(-4.D0*XLN)
                              BP(20) = AI(20)
                              BP(19) = ZPITZ*AI(20) + AI(19)
                              DP(19) = AI(20)
                              DO 10050 L = 18, 0, -1
                                 L1 = L + 1
                                 L2 = L + 2
                                 BP(L) = ZPITZ*BP(L1) - BP(L2) + AI(L)
                                 DP(L) = BP(L1) + ZPITZ*DP(L1) - DP(L2)
10050                         CONTINUE
                          ENDIF
                          FIJ = FIJ - CHP(J)*(BP(0) - BP(2) +
     +                                              0.5D0*XX(J) - 2.D0)
                          OSMO = OSMO + CHP(J)*XX(J)*(2.D0*DZDX*(DP(0)
     +                                               - DP(2)) + 0.25D0)
10070                  CONTINUE
                       GINT = GINT + FIJ
                       G(I) = G(I) - FIJ/X(I)
                       G(K) = G(K) - FIJ/X(K)
                 ENDIF
10110         CONTINUE
         ENDIF
10200 CONTINUE

      ENDIF

      GINT = SION2*(GINT + OSMO)
      IF (TMI .GT. 0.D0) OSMO = OSMO/TMI

      ELSE

C        The section below is called for each interaction term

      IF (SION .EQ. 0.D0) RETURN
      WTI = X(IT1)*WT
      WTJ = X(IT2)*WT
      WTIJ = WTI*X(IT2)

      IF (NT .EQ. 3) THEN
           OSMO = X(IT3)*(WTIJ + WTIJ)
           G(IT1) = WTJ*X(IT3)
           G(IT2) = WTI*X(IT3)
           G(IT3) = WTIJ

      ELSEIF (IT3 .EQ. 0) THEN
           OSMO = WTIJ + WTIJ
           G(IT1) = WTJ + WTJ
           G(IT2) = WTI + WTI

      ELSEIF (IT3 .LT. 3) THEN
           IF (IT3. EQ. 1 .AND. ALPHA1 .NE. 0.D0) THEN
                GF = ALPHA1*SQSION
           ELSEIF (IT3. EQ. 2 .AND. ALPHA2 .NE. 0.D0) THEN
                GF = ALPHA2*SQSION
           ELSE
                K = 2*IT3
                IF (ABS(CH(IT1)).NE.2.D0.OR.ABS(CH(IT2)).NE.2.D0) K=K-1
                IF (K .EQ. 1) THEN
                     GF = 2.D0*SQSION
                ELSEIF (K .EQ. 2) THEN
                     GF = 1.4D0*SQSION
                ELSEIF (K .EQ. 3) THEN
                     GF = 50.D0*SQSION
                ELSE
                     GF = 12.D0*SQSION
                ENDIF
           ENDIF
           GFE = 2.D0*EXP(-GF)
           GX = 2.D0*(2.D0 - GF*GFE - GFE)/(GF*GF)
           TEMP = SION2*WTIJ*(GFE - GX)
           OSMO = WTIJ*GFE
           G(IT1) = WTJ*GX
           G(IT2) = WTI*GX
           DO 10250 I = 2, NC
              G(I) = G(I) + TEMP*CH(I)*CH(I)
10250      CONTINUE

      ELSEIF(IT3 .EQ. 3) THEN
           TEMP = 1.D0/SQRT(-CH(IT1)*CH(IT2))
           TEMP1 = TEMP*ZUSAZ
           OSMO = TEMP1*WTIJ
           TEMP1 = 0.5D0*TEMP1
           G(IT1) = TEMP1*WTJ
           G(IT2) = TEMP1*WTI
           TEMP = 0.5D0*TEMP*WTIJ
           DO 10270 I = 2, NC
              G(I) = G(I) + TEMP*ABS(CH(I))
10270      CONTINUE
      ENDIF
      IF (TMI .GT. 0.D0) OSMO = OSMO/TMI

      ENDIF

      RETURN
      END




      SUBROUTINE PTZGXS(NION,X,APHI,WT,EFLG,A1,A2,GSUM,GAMMA,OSMOXX)
C     **************************************************************

C   Purpose:  Calculates osmotic and activity coefficients
C             from Pitzer model at given T and P
C   Import:   NION - number of ions, CH(2...12) their charges,
C             X(2...12) their molalities, APHI - DH constant,
C             WT(I,J,K) interaction parameters
C   Export:   GSUM(2...12) - ln gamma_i, GAMMA(2...12) - gamma_i,
C             OSMOXX - osmotic coefficient
C   Created:  EK/PM/KM (17/9/01) Copyright reserved
C   Modified: Version 7 (9/10/01)

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      COMMON /THDTA/CH,TERM,GS,VS,NC
      DIMENSION TERM(12,12,0:12,6),
     >WT(12,12,0:12),CH(12),GS(20,6),VS(20),
     >A1(12,12),A2(12,12),X(12),G(12),
     >GSUM(12),GAMMA(12)
      CHARACTER EFLG*1

Code ...

      NC = NION + 1

      SION = 0.D0
      TMI = 0.D0
      DO 10020 I = 2, NC
         SION = SION + 0.5D0*CH(I)*CH(I)*X(I)
         TMI = TMI + X(I)
         G(I) = 0.D0
10020 CONTINUE

      NT = 0
      IT1 = 0
      IT2 = 0
      IT3 = 0
      GINT = 0.D0
      OSMO = 0.D0
      CALL PTZFNC(NT,IT1,IT2,IT3,0.D0,EFLG,A1(1,1),A2(1,1),X,CH,NC,APHI,
     +                                             SION,TMI,GINT,G,OSMO)
      OSMOXX = OSMO
      DO 10060 I = 2, NC
         GSUM(I) = G(I) + CH(I)**2.D0*GINT
10060 CONTINUE

      NT = 2

      DO 10150 IT1 = 2, NION
         DO 10150 IT2 = IT1+1, NION+1
            DO 10150 IT3 = 0,3
               IF (CH(IT1)*CH(IT2) .GT. 0.D0) GOTO 10150
               DO 10080 I = 2, NC
                  G(I) = 0.D0
10080          CONTINUE
               OSMO = 0.D0
               CALL PTZFNC(NT,IT1,IT2,IT3,WT(IT1,IT2,IT3),EFLG,A1(IT1,
     +               IT2),A2(IT1,IT2),X,CH,NC,APHI,SION,TMI,GINT,G,OSMO)
               OSMOXX = OSMOXX + OSMO
               DO 10100 I = 2, NC
                  GSUM(I) = GSUM(I) + G(I)
10100          CONTINUE
10150 CONTINUE

      IT3 = 0

      DO 10180 IT1 = 2, NION
         DO 10180 IT2 = IT1+1, NION+1
            IF (CH(IT1)*CH(IT2) .LT. 0.D0) GOTO 10180
            DO 10160 I = 2, NC
               G(I) = 0.D0
10160       CONTINUE
            OSMO = 0.D0
            CALL PTZFNC(NT,IT1,IT2,IT3,WT(IT1,IT2,IT3),EFLG,A1(IT1,
     +            IT2),A2(IT1,IT2),X,CH,NC,APHI,SION,TMI,GINT,G,OSMO)
            OSMOXX = OSMOXX + OSMO
            DO 10170 I = 2, NC
               GSUM(I) = GSUM(I) + G(I)
10170       CONTINUE
10180 CONTINUE

      NT = 3

      DO 10220 IT1 = 2, NC-2
         DO 10220 IT2 = IT1+1, NC-1
            DO 10220 IT3 = IT2+1, NC

               IF (CH(IT1).GT.0.D0.AND.CH(IT2).GT.0.D0.AND.CH(IT3).GT.
     +             0.D0 .OR. CH(IT1).LT.0.D0.AND.CH(IT2).LT.0.D0.AND.
     +             CH(IT3).LT.0.D0)GOTO 10220

               DO 10190 I = 2, NC
                  G(I) = 0.D0
10190          CONTINUE
               OSMO = 0.D0
               CALL PTZFNC(NT,IT1,IT2,IT3,WT(IT1,IT2,IT3),EFLG,A1(IT1,
     +               IT2),A2(IT1,IT2),X,CH,NC,APHI,SION,TMI,GINT,G,OSMO)
               OSMOXX = OSMOXX + OSMO
               DO 10200 I = 2, NC
                  GSUM(I) = GSUM(I) + G(I)
10200          CONTINUE
10220 CONTINUE

      OSMOXX = OSMOXX + 1.D0
      DO 10400 I = 2, NC
         GAMMA(I) = EXP(GSUM(I))
10400 CONTINUE

      RETURN
      END




      SUBROUTINE PTZPAR(NION,SQSI,PRE,TC,A1,A2,WT)
C     ********************************************

C   Purpose:  Evaluates and stores Pitzer parameters for given P and T
C   Import:   NC - number of constituents, CH(2...12) - their
C             charges, PRE - pressure [bar]
C             TC - temperature [C]
C   Export:   WT(I,J,K) interaction parameters

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      COMMON /THDTA/CH,TERM,GS,VS,NC
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION TERM(12,12,0:12,6),
     >WT(12,12,0:12),CH(12),
     >A1(12,12),A2(12,12),GS(20,6),VS(20)

      NC = NION + 1
      TK = TC + 2.7315D2
      TXX = TK
      PXX = PRE
      
C ...  binary interactions      
      
      WT(2,3,0)= BETA0NAOH(PXX,TXX)
      WT(2,3,1)= BETA1NAOH(PXX,TXX)
      WT(2,3,2)= BETA2NAOH(PXX,TXX)
      A2(2,3) = 8.D0
      WT(2,3,3)= CPHINAOH(PXX,TXX)
      WT(2,4,0)= BETA0NAHCO3(PXX,TXX)
      WT(2,4,1)= BETA1NAHCO3(PXX,TXX)
      WT(2,4,3)= CPHINAHCO3(PXX,TXX)
      WT(2,5,0)= BETA0NA2CO3(PXX,TXX)
      WT(2,5,1)= BETA1NA2CO3(PXX,TXX)
      WT(2,5,3)= CPHINA2CO3(PXX,TXX)
      IF (TSAVE .LT. 338.15D0) THEN
           WT(2,6,0)= BETA0NACLlow(PXX,TXX)
           WT(2,6,3)= CPHINACLlow(PXX,TXX)
      ELSE
           WT(2,6,0)= BETA0NACL(PXX,TXX)
           WT(2,6,3)= CPHINACL(PXX,TXX)
      ENDIF
      WT(2,6,1)= BETA1NACL(TXX)
      WT(2,7,0)= BETA0NA2SO4(PXX,TXX)
      WT(2,7,1)= BETA1NA2SO4(PXX,TXX,SQSI)
      A1(2,7) = 1.4D0
      WT(2,7,3)= CPHINA2SO4(PXX,TXX)
      WT(2,8,0)= BETA0NAALOH4(PXX,TXX)
      WT(2,8,1)= BETA1NAALOH4(PXX,TXX)
      WT(2,8,2)= BETA2NAALOH4(PXX,TXX)
      A2(2,8) = 8.D0
      WT(2,8,3)= CPHINAALOH4(PXX,TXX)
      WT(2,9,0)= WT(2,7,0)
      WT(2,9,1)= WT(2,7,1)
      A1(2,9) = 1.4D0
      WT(2,9,3)= WT(2,7,3)
      WT(2,10,0)= BETA0NAAC(PXX,TXX)
      WT(2,10,1)= BETA1NAAC(PXX,TXX)
      WT(2,10,3)= CPHINAAC(PXX,TXX)
      WT(2,11,0) = BETA0NAFORM(PXX,TXX)
      WT(2,11,1) = BETA1NAFORM(PXX,TXX)
      WT(2,11,3) = CPHINAFORM(PXX,TXX) 
      WT(2,12,0) = BETA0NAF(PXX,TXX)
      WT(2,12,1) = BETA1NAF(PXX,TXX)
      
      NT = 2
      DO 10150 IT1 = 2, NC-1
         DO 10140 IT2 = IT1+1, NC
            IF (CH(IT1)*CH(IT2) .LT. 0.D0) GO TO 10140
            WT(IT1,IT2,0) = PARVAL(IT1,IT2,0,TXX)
10140    CONTINUE
10150 CONTINUE

      NT = 3
      DO 10200, IT1 = 2, NC-2
         DO 10190, IT2 = IT1+1, NC-1
            DO 10180, IT3 = IT2+1, NC

               IF (CH(IT1).GT.0.D0.AND.CH(IT2).GT.0.D0.AND.CH(IT3).GT.
     +             0.D0 .OR. CH(IT1).LT.0.D0.AND.CH(IT2).LT.0.D0.AND.
     +             CH(IT3).LT.0.D0) GOTO 10180

               WT(IT1,IT2,IT3) = PARVAL(IT1,IT2,IT3,TXX)
               
10180       CONTINUE
10190    CONTINUE
10200 CONTINUE

      RETURN
      END




      DOUBLE PRECISION FUNCTION SIPDHL(P,T)
C     *************************************

C   Purpose:  CALCULATES THE PARAMETER A(PHI) OF THE DEBYE-HUECKEL 
C             LIMITING LAW USING THE BRADLEY-PITZER EQUATIONS
C   Import:   P [bar], T [K}
C   Created:  EK/PM/KM (17/10/01) Copyright reserved
C   Modified: Version 7 (17/10/01)

      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION AA(24), U(9)
      PARAMETER(EP3=1.D3,RN0=0.D0,RN1=1.D0,RN2=2.D0,RN3=3.D0,RN4=4.D0,
     >RN5=5.D0,RN17=1.7D1,Q=4.42907596D4,PCH2O=2.2064D2,TCH2O=6.47073D2,
     >VCH2O=3.17D-3)
      DATA AA/8.438375405D-1,5.362162162D-4,1.72D0,7.342278489D-2,
     >4.975858870D-2,6.537154300D-1,1.15D-6,1.510D-5,1.4188D-1,
     >7.002753165D0,2.995284926D-4,2.04D-1,7.982692717D0,
     >-2.616571843D-2,1.522411790D-3,2.284279054D-2,2.421647003D2,
     >1.269716088D-10,2.074838328D-7,2.174020350D-8,1.105710498D-9,
     >1.293441934D1,1.308119072D-5,6.047626338D-14/
      DATA U/3.4279D2,-5.0866D-3,9.4690D-7,-2.0525D0,3.1159D3,-1.8289D2,
     >-8.0325D3,4.2142D6,2.1417D0/

Code ...

      PR = P/PCH2O
      PR2 = PR*PR
      TR = T/TCH2O
      TR2 = TR*TR
      TR6 = TR2*TR2*TR2
      TR12 = TR6*TR6
      TR18 = TR6*TR12
      TR19 = TR*TR18 + AA(7)
      TEMP = RN1 - TR2*AA(1) - AA(2)/TR6
      TEMP=ABS(SQRT(ABS(TEMP*TEMP*AA(3)-RN2*(TR*AA(4)-PR*AA(5))))+TEMP)
      TEMP = AA(5)*AA(13)*EXP(-RN5/RN17*LOG(TEMP)) + AA(14)
      TEMP1=TR*AA(15)+TR2*AA(16)+ABS(AA(6)-TR)**10*AA(17)+AA(18)/TR19
      TEMP2=-(AA(19)+RN2*PR*AA(20)+RN3*PR2*AA(21))/(TR12/TR+AA(8))
      TEMP3 = -AA(22)*TR18*(AA(9) + TR2)*(AA(11) - RN3/(AA(10) + PR)**4)
      TEMP4 = PR2*(RN3*AA(23)*(AA(12) - TR) + RN4*PR*AA(24)/(TR2*TR18))
      SPVH2O = VCH2O*(TEMP + TEMP1 + TEMP2 + TEMP3 + TEMP4)
      TEMP = U(7) + U(8)/T + U(9)*T
      TEMP = (TEMP + P)/(TEMP + EP3)
      TEMP1 = U(1)*EXP(T*(U(2) + T*U(3)))
      IF (TEMP.GT.RN0) TEMP1=TEMP1+(U(4)+U(5)/(U(6)+T))*LOG(TEMP)
      TEMP = T*ABS(TEMP1)
      SIPDHL = Q/SQRT(ABS(TEMP*TEMP*TEMP*SPVH2O))
      RETURN
      END
      
      
      SUBROUTINE MODELS
      
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /NAOH/ V0NAOH, MGC, MVC, MDLGC, MDLVC
      COMMON /NA2SO4/ V0NA2SO4, MGT, MVT, MDLGT, MDLVT
      COMMON /NAHCO3/ V0NAHCO3, MGN, MVN, MDLGN, MDLVN
      COMMON /NA2CO3/ V0NA2CO3, MGS, MVS, MDLGS, MDLVS
      COMMON /NAALOH4/ V0NAALOH4, MGA, MVA, MDLGA, MDLVA 
      CHARACTER*65 MDLGC(10), MDLGT(10), MDLGN(10), MDLGS(10), MDLGA(10)
      CHARACTER*65 MDLVC(10), MDLVT(10), MDLVN(10), MDLVS(10), MDLVA(10)

C
C     Model for NaOH
C      
C     MGC - Gibbs energy model: 
      MDLGC(1) = 'Simonson et al. (1989)'
      MDLGC(2) = 'const. Cp model (Koenigsberger, 2001)'
      MDLGC(3) = 'const. Cp model, Cp fitted to Murdoch data'
      MDLGC(4) = 'New model, 100 bar'
C     
C     MVC - Volumetric model:
      MDLVC(1) = 'Simonson et al. (1989)'
      MDLVC(2) = 'Pitzer fit to 25 C density data'
      MDLVC(3) = 'Pitzer fit to 100 bar density data'
      MDLVC(4) = 'Pabalan & Pitzer (1987)'
      MDLVC(5) = 'Corti et al. (1990)'
      MDLVC(6) = 'Pitzer fit to 100 bar density data excl. 275 C data'
C
      MGC = 4
      MVC = 3
C
C     Model for Na2SO4
C      
C     MGT - Gibbs energy model: 
      MDLGT(1) = 'Holmes and Mesmer (1986)'
      MDLGT(2) = 'Pabalan and Pitzer (1988) saturation pressure'
      MDLGT(3) = 'Pabalan and Pitzer (1988) 200 bar'
      MDLGT(4) = 'const. Cp model, XG, XL from Pitzer (1991), XJ from C&
     >M (1999)'
      MDLGT(5) = 'const. Cp model, XG, XL: Pitzer (1991), XJ fitted to M
     >urdoch data'
      MDLGT(6) = 'new model'
C     
C     MVT - Volumetric model:
      MDLVT(1) = 'Monnin (1990)'
      MDLVT(2) = '2 times NaCl (for MGT = 3 only)'
      MDLVT(3) = 'Krumgalz et al. (2000)'
      MDLVT(4) = 'Monnin (1990) (for MGT = 3 only)'
C
      MGT = 1
      MVT = 1
C
C     Model for Na2CO3
C      
C     MGS - Gibbs energy model: 
      MDLGS(1) = 'const. Cp model (Koenigsberger, 2001)'
      MDLGS(2) = 'Polya (2001), U2, U8 adj. to XL and XJ at 25 C of mode
     >l 4'
      MDLGS(3) = 'Polya et al. (2001)'
      MDLGS(4) = 'const. Cp model, with Cphi_J adj. to fit Murdoch data
     >'
      MDLGS(5) = 'XG, XL from P&P, XJ(298) for UNHYDROLYSED Na2CO3 from 
     >C&M'
C                     i.e. HCO3- must be suppressed in CS (automatically done in BAYER)
      MDLGS(6) = 'HKF model'
C     
C     MVS - Volumetric model:
      MDLVS(1) = 'Krumgalz et al. (2000), original'
      MDLVS(2) = 'Krumgalz et al. (2000), modified'
      MDLVS(3) = 'Polya et al. (2001) 25 C only!'
      MDLVS(4) = 'New model'
      MDLVS(5) = 'HKF model'
C
      MGS = 6
      MVS = 5
C
C     Model for NaHCO3
C      
C     MGN - Gibbs energy model: 
      MDLGN(1) = 'const. Cp model (Koenigsberger, 2001)'
      MDLGN(2) = 'Polya et al. (2001)'
      MDLGN(3) = 'HKF model'
C     
C     MVN - Volumetric model:
      MDLVN(1) = 'Krumgalz et al. (2000)'
      MDLVN(2) = 'Polya et al. (2001) 25 C only!'
      MDLVN(3) = 'New model'
      MDLVN(4) = 'HKF model'
C
      MGN = 3
      MVN = 4
C
C     Model for NaAl(OH)4
C      
C     MGA - Gibbs energy model: 
      MDLGA(1) = 'Wesolowski (1992)'
      MDLGA(2) = 'Caiani et al. (1989) for XJ, XL, Wesolowski for XG'
      MDLGA(3) = 'Park & Englezos (1999)'
      MDLGA(4) = 'New model, 100 bar'
C     
C     MVA - Volumetric model:
      MDLVA(1) = 'V0 only'
      MDLVA(2) = 'Pitzer fit'
      MDLVA(3) = 'Pitzer fit to 100 bar density data'
C
      MGA = 4
      MVA = 3
C      
      RETURN
      END


      DOUBLE PRECISION FUNCTION CP0NAOH(P,T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      
      R = 8.31451D0      
      CP = 0.D0            
      TSAVE = T
      TSAV1 = 1.D0/TSAVE   
      DO 10010 K = -2, 2, 4
         TDIFF = 0.1D0*DBLE(K)
         TDIFF1 = 1.D0/TDIFF
         T = TSAVE + 0.5D0*TDIFF
         G = G0NAOH(P,T)*R*T
         TEMP = T*TDIFF1
         CP = CP + TEMP*TSAV1*G/T    
         T = TSAVE + TDIFF
         G = G0NAOH(P,T)*R*T
         CP = CP - TEMP*G*(TSAV1 + TDIFF1)/T
10010 CONTINUE
      T = TSAVE
      G = G0NAOH(P,T)*R*T
      CP0NAOH = (CP + 50.D0*G)*T
      RETURN
      END            
      
      
      DOUBLE PRECISION FUNCTION CP0NAALOH4(P,T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      
      R = 8.31451D0      
      CP = 0.D0            
      TSAVE = T
      TSAV1 = 1.D0/TSAVE   
      DO 10010 K = -2, 2, 4
         TDIFF = 0.1D0*DBLE(K)
         TDIFF1 = 1.D0/TDIFF
         T = TSAVE + 0.5D0*TDIFF
         G = G0NAALOH4(P,T)*R*T
         TEMP = T*TDIFF1
         CP = CP + TEMP*TSAV1*G/T    
         T = TSAVE + TDIFF
         G = G0NAALOH4(P,T)*R*T
         CP = CP - TEMP*G*(TSAV1 + TDIFF1)/T
10010 CONTINUE
      T = TSAVE
      G = G0NAALOH4(P,T)*R*T
      CP0NAALOH4 = (CP + 50.D0*G)*T
      RETURN
      END            
      
      
      DOUBLE PRECISION FUNCTION CP0NACL(P,T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      
      R = 8.31451D0      
      CP = 0.D0            
      TSAVE = T
      TSAV1 = 1.D0/TSAVE   
      DO 10010 K = -2, 2, 4
         TDIFF = 0.1D0*DBLE(K)
         TDIFF1 = 1.D0/TDIFF
         T = TSAVE + 0.5D0*TDIFF
         G = G0NACL(P,T)*R*T
         TEMP = T*TDIFF1
         CP = CP + TEMP*TSAV1*G/T    
         T = TSAVE + TDIFF
         G = G0NACL(P,T)*R*T
         CP = CP - TEMP*G*(TSAV1 + TDIFF1)/T
10010 CONTINUE
      T = TSAVE
      G = G0NACL(P,T)*R*T
      CP0NACL = (CP + 50.D0*G)*T
      RETURN
      END            
      
      
      DOUBLE PRECISION FUNCTION CP0NAAC(P,T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      
      R = 8.31451D0      
      CP = 0.D0            
      TSAVE = T
      TSAV1 = 1.D0/TSAVE   
      DO 10010 K = -2, 2, 4
         TDIFF = 0.1D0*DBLE(K)
         TDIFF1 = 1.D0/TDIFF
         T = TSAVE + 0.5D0*TDIFF
         G = G0NAAC(P,T)*R*T
         TEMP = T*TDIFF1
         CP = CP + TEMP*TSAV1*G/T    
         T = TSAVE + TDIFF
         G = G0NAAC(P,T)*R*T
         CP = CP - TEMP*G*(TSAV1 + TDIFF1)/T
10010 CONTINUE
      T = TSAVE
      G = G0NAAC(P,T)*R*T
      CP0NAAC = (CP + 50.D0*G)*T
      RETURN
      END            
      
      
      DOUBLE PRECISION FUNCTION CP0NAFORM(P,T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      
      R = 8.31451D0      
      CP = 0.D0            
      TSAVE = T
      TSAV1 = 1.D0/TSAVE   
      DO 10010 K = -2, 2, 4
         TDIFF = 0.1D0*DBLE(K)
         TDIFF1 = 1.D0/TDIFF
         T = TSAVE + 0.5D0*TDIFF
         G = G0NAFORM(P,T)*R*T
         TEMP = T*TDIFF1
         CP = CP + TEMP*TSAV1*G/T    
         T = TSAVE + TDIFF
         G = G0NAFORM(P,T)*R*T
         CP = CP - TEMP*G*(TSAV1 + TDIFF1)/T
10010 CONTINUE
      T = TSAVE
      G = G0NAFORM(P,T)*R*T
      CP0NAFORM = (CP + 50.D0*G)*T
      RETURN
      END            
      
      
      DOUBLE PRECISION FUNCTION CP0NA2CO3(P,T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      
      R = 8.31451D0      
      CP = 0.D0            
      TSAVE = T
      TSAV1 = 1.D0/TSAVE   
      DO 10010 K = -2, 2, 4
         TDIFF = 0.1D0*DBLE(K)
         TDIFF1 = 1.D0/TDIFF
         T = TSAVE + 0.5D0*TDIFF
         G = G0NA2CO3(P,T)*R*T
         TEMP = T*TDIFF1
         CP = CP + TEMP*TSAV1*G/T    
         T = TSAVE + TDIFF
         G = G0NA2CO3(P,T)*R*T
         CP = CP - TEMP*G*(TSAV1 + TDIFF1)/T
10010 CONTINUE
      T = TSAVE
      G = G0NA2CO3(P,T)*R*T
      CP0NA2CO3 = (CP + 50.D0*G)*T
      RETURN
      END            
      
      
      DOUBLE PRECISION FUNCTION CP0NA2SO4(P,T)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      
      R = 8.31451D0      
      CP = 0.D0            
      TSAVE = T
      TSAV1 = 1.D0/TSAVE   
      DO 10010 K = -2, 2, 4
         TDIFF = 0.1D0*DBLE(K)
         TDIFF1 = 1.D0/TDIFF
         T = TSAVE + 0.5D0*TDIFF
         G = G0NA2SO4(P,T)*R*T
         TEMP = T*TDIFF1
         CP = CP + TEMP*TSAV1*G/T    
         T = TSAVE + TDIFF
         G = G0NA2SO4(P,T)*R*T
         CP = CP - TEMP*G*(TSAV1 + TDIFF1)/T
10010 CONTINUE
      T = TSAVE
      G = G0NA2SO4(P,T)*R*T
      CP0NA2SO4 = (CP + 50.D0*G)*T
      RETURN
      END            
      
      
      DOUBLE PRECISION FUNCTION VXP(P1,T1,VX,VP,PR,P0)
      
C     Calculates V0 and dV0/dP contribution to G0 
C     Integrate (V0(P0) + (P - P0)V0')dP from Pr to P --->
C     (P - Pr)[V0(P0) + {0.5(P + Pr) - P0}V0']
C     P in MPa --> factor 0.1
C
C     V0 = VX(1) + VX(2)/T + VX(3)*LOG(T) + VX(4)*T +
C          VX(5)*T*T + VX(6)/(T-VX(7)) + VX(8)/(VX(9)-T) + 
C          VX(10)/(T*T) + VX(11)*T/(T-VX(12))**3 +
C          VX(13)*T/(VX(14)-T)**3 +
C     V0'= VP(1) + VP(2)/T + VP(3)*LOG(T) + VP(4)*T +
C          VP(5)*T*T + VP(6)/(T-VX(7)) + VP(8)/(VP(9)-T) + 
C          VP(10)/(T*T) + VP(11)*T/(T-VP(12))**3 +
C          VP(13)*T/(VP(14)-T)**3
C
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION VX(14), VP(14)
      VXX1 = VX(1) + VX(2)/T1 + VX(3)*LOG(T1) + VX(4)*T1 
      VXX2 = VX(5)*T1*T1 + VX(6)/(T1-VX(7)) + VX(8)/(VX(9)-T1) 
      VXX3 = VX(10)/(T1*T1) 
      VXX4 = VX(11)*T1/((T1-VX(12))*(T1-VX(12))*(T1-VX(12))) 
      VXX5 = VX(13)*T1/((VX(14)-T1)*(VX(14)-T1)*(VX(14)-T1))
      VXP1 = VP(1) + VP(2)/T1 + VP(3)*LOG(T1) + VP(4)*T1 
      VXP2 = VP(5)*T1*T1 + VP(6)/(T1-VP(7)) + VP(8)/(VP(9)-T1) 
      VXP3 = VP(10)/(T1*T1) 
      VXP4 = VP(11)*T1/((T1-VP(12))*(T1-VP(12))*(T1-VP(12))) 
      VXP5 = VP(13)*T1/((VP(14)-T1)*(VP(14)-T1)*(VP(14)-T1))
      VXX = VXX1 + VXX2 + VXX3 + VXX4 + VXX5 
      VXP = VXX + 1.D-1*(0.5D0*(P1+PR)-P0)*(VXP1+VXP2+VXP3+VXP4+VXP5)
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION V0XP(P1,T1,VX,VP,PR)
C     V0 = VX(1) + VX(2)/T + VX(3)*LOG(T) + VX(4)*T +
C          VX(5)*T*T + VX(6)/(T-VX(7)) + VX(8)/(VX(9)-T) + 
C          VX(10)/(T*T) + VX(11)*T/(T-VX(12))**3 +
C          VX(13)*T/(VX(14)-T)**3 +
C     1.D-1*(P-PR)*VP(1) + VP(2)/T + VP(3)*LOG(T) + VP(4)*T +
C          VP(5)*T*T + VP(6)/(T-VX(7)) + VP(8)/(VP(9)-T) + 
C          VP(10)/(T*T) + VP(11)*T/(T-VP(12))**3 +
C          VP(13)*T/(VP(14)-T)**3
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION VX(14), VP(14)
      VXX1 = VX(1) + VX(2)/T1 + VX(3)*LOG(T1) + VX(4)*T1
      VXX2 = VX(5)*T1*T1 + VX(6)/(T1-VX(7)) + VX(8)/(VX(9)-T1) 
      VXX3 = VX(10)/(T1*T1) 
      VXX4 = VX(11)*T1/((T1-VX(12))*(T1-VX(12))*(T1-VX(12))) 
      VXX5 = VX(13)*T1/((VX(14)-T1)*(VX(14)-T1)*(VX(14)-T1))
      VXP1 = VP(1) + VP(2)/T1 + VP(3)*LOG(T1) + VP(4)*T1 
      VXP2 = VP(5)*T1*T1 + VP(6)/(T1-VP(7)) + VP(8)/(VP(9)-T1) 
      VXP3 = VP(10)/(T1*T1) 
      VXP4 = VP(11)*T1/((T1-VP(12))*(T1-VP(12))*(T1-VP(12))) 
      VXP5 = VP(13)*T1/((VP(14)-T1)*(VP(14)-T1)*(VP(14)-T1))
      VXX = VXX1 + VXX2 + VXX3 + VXX4 + VXX5
      VXP = VXP1 + VXP2 + VXP3 + VXP4 + VXP5
C     P in MPa --> factor 0.1
      V0XP = VXX + 1.D-1*(P1 - PR)*VXP
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CPXP(P1,T1,CpX)
C     Cp0 = CpX(1) + CpX(2)/T + CpX(3)*LOG(T) + CpX(4)*T +
C           CpX(5)*T*T + CpX(6)/(T-CpX(7)) + CpX(8)/(CpX(9)-T) + 
C           CpX(10)/(T*T) + CpX(11)*T/(T-CpX(12))**3 +
C           CpX(13)*T/(CpX(14)-T)**3
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION CpX(14)
      CpXX1 = CpX(1) + CpX(2)/T1 + CpX(3)*LOG(T1) + CpX(4)*T1 
      CpXX2 = CpX(5)*T1*T1 + CpX(6)/(T1-CpX(7)) + CpX(8)/(CpX(9)-T1) 
      CpXX3 = CpX(10)/(T1*T1) 
      CpXX4 = CpX(11)*T1/((T1-CpX(12))*(T1-CpX(12))*(T1-CpX(12)))
      CpXX5 = CpX(13)*T1/((CpX(14)-T1)*(CpX(14)-T1)*(CpX(14)-T1))
      CPXP = CpXX1 + CpXX2 + CpXX3 + CpXX4 + CpXX5
      RETURN
      END
C      
      DOUBLE PRECISION FUNCTION HSCPXX(P1,T1,HXX,SXX,CPXX)
C     Cp0 = CPXX(1) + CPXX(2)/T + CPXX(3)*LOG(T) + CPXX(4)*T +
C           CPXX(5)*T*T + CPXX(6)/(T-CPXX(7)) + CPXX(8)/(CPXX(9)-T) + 
C           CPXX(10)/(T*T) + CPXX(11)*T/(T-CPXX(12))**3 +
C           CPXX(13)*T/(CPXX(14)-T)**3
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION CPXX(14)
      TR = 2.9815D2
      HTXX = HXX + CPHXX(P1,T1,CPXX) - CPHXX(P1,TR,CPXX)
      STXX = SXX + CPSXX(P1,T1,CPXX) - CPSXX(P1,TR,CPXX)
      HSCPXX = (HTXX - T1*STXX)/(8.31451D0*T1)
      RETURN
      END
C      
      DOUBLE PRECISION FUNCTION HSCPXX2(P1,T1,HXX,SXX,CPXX1,CPXX2,TR2)
C     2 T ranges      
C     Cp0 = CPXX(1) + CPXX(2)/T + CPXX(3)*LOG(T) + CPXX(4)*T +
C           CPXX(5)*T*T + CPXX(6)/(T-CPXX(7)) + CPXX(8)/(CPXX(9)-T) + 
C           CPXX(10)/(T*T) + CPXX(11)*T/(T-CPXX(12))**3 +
C           CPXX(13)*T/(CPXX(14)-T)**3
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION CPXX1(14), CPXX2(14)
      TR1 = 2.9815D2
      HTXX = HXX + CPHXX(P1,TR2,CPXX1) - CPHXX(P1,TR1,CPXX1)
     >           + CPHXX(P1,T1,CPXX2) - CPHXX(P1,TR2,CPXX2)
      STXX = SXX + CPSXX(P1,TR2,CPXX1) - CPSXX(P1,TR1,CPXX1)
     >           + CPSXX(P1,T1,CPXX2) - CPSXX(P1,TR2,CPXX2)
      HSCPXX2 = (HTXX - T1*STXX)/(8.31451D0*T1)
      RETURN
      END
C      
      DOUBLE PRECISION FUNCTION CPHXX(P1,T1,CPXX)
C     Integrate Cp0 dT --> Contribution to H
C     Cp0 = CPXX(1) + CPXX(2)/T + CPXX(3)*LOG(T) + CPXX(4)*T +
C           CPXX(5)*T*T + CPXX(6)/(T-CPXX(7)) + CPXX(8)/(CPXX(9)-T) + 
C           CPXX(10)/(T*T) + CPXX(11)*T/(T-CPXX(12))**3 +
C           CPXX(13)*T/(CPXX(14)-T)**3
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION CPXX(14)
      CPH1 = CPXX(1)*T1 + (CPXX(2) + CPXX(3)*T1)*LOG(T1) - CPXX(3)*T1 
      CPH2 = 5.D-1*CPXX(4)*T1*T1 + CPXX(5)*T1*T1*T1/3.D0  
      CPH3 = CPXX(6)*LOG(ABS(T1-CPXX(7))) - CPXX(8)*LOG(ABS(CPXX(9)-T1))
      CPH4 = -CPXX(10)/T1 + CPXX(11)*(CPXX(12)-2.D0*T1)/(2.D0*(CPXX(12)
     > -T1)*(CPXX(12)-T1)) 
      CPH5 = CPXX(13)*(2.D0*T1-CPXX(14))/(2.D0*(CPXX(14)-
     > T1)*(CPXX(14)-T1))
      CPHXX = CPH1 + CPH2 + CPH3 + CPH4 + CPH5
      RETURN
      END
C      
      DOUBLE PRECISION FUNCTION CPSXX(P1,T1,CPXX)
C     Integrate Cp0/T dT --> Contribution to S
C     Cp0 = CPXX(1) + CPXX(2)/T + CPXX(3)*LOG(T) + CPXX(4)*T +
C           CPXX(5)*T*T + CPXX(6)/(T-CPXX(7)) + CPXX(8)/(CPXX(9)-T) + 
C           CPXX(10)/(T*T) + CPXX(11)*T/(T-CPXX(12))**3 +
C           CPXX(13)*T/(CPXX(14)-T)**3
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION CPXX(14)
      IF(CPXX(7) .EQ. 0.D0 .AND. CPXX(9) .EQ. 0.D0) THEN
      CPS1 = CPXX(1)*LOG(T1)
      CPS1a = -CPXX(2)/T1 + 5.D-1*CPXX(3)*LOG(T1)*LOG(T1) + CPXX(4)*T1
      ELSEIF(CPXX(7) .EQ. 0.D0) THEN
      CPS1 = (CPXX(1) + CPXX(8)/CPXX(9))*LOG(T1)
      CPS1a = -CPXX(2)/T1 + 5.D-1*CPXX(3)*LOG(T1)*LOG(T1) + CPXX(4)*T1 
      ELSEIF(CPXX(9) .EQ. 0.D0) THEN
      CPS1 = (CPXX(1) - CPXX(6)/CPXX(7))*LOG(T1)
      CPS1a = -CPXX(2)/T1 + 5.D-1*CPXX(3)*LOG(T1)*LOG(T1) + CPXX(4)*T1 
      ELSE 
      CPS1 = (CPXX(1) - CPXX(6)/CPXX(7) + CPXX(8)/CPXX(9))*LOG(T1)
      CPS1a = -CPXX(2)/T1+5.D-1*CPXX(3)*LOG(T1)*LOG(T1)+CPXX(4)*T1
      ENDIF
      IF(CPXX(7) .EQ. 0.D0 .AND. CPXX(9) .EQ. 0.D0) THEN
      CPS2 = 5.D-1*CPXX(5)*T1*T1 - 5.D-1*CPXX(10)/(T1*T1)
      CPS2a = 0.D0
      ELSEIF(CPXX(7) .EQ. 0.D0) THEN
      CPS2 = 5.D-1*CPXX(5)*T1*T1 
      CPS2a=-CPXX(8)*LOG(ABS(CPXX(9)-T1))/CPXX(9)-5.D-1*CPXX(10)/(T1*T1)
      ELSEIF(CPXX(9) .EQ. 0.D0) THEN
      CPS2 = 5.D-1*CPXX(5)*T1*T1 + CPXX(6)*LOG(ABS(T1-CPXX(7)))/CPXX(7)
      CPS2a = -5.D-1*CPXX(10)/(T1*T1)
      ELSE
      CPS2 = 5.D-1*CPXX(5)*T1*T1 + CPXX(6)*LOG(ABS(T1-CPXX(7)))/CPXX(7)
      CPS2a=-CPXX(8)*LOG(ABS(CPXX(9)-T1))/CPXX(9)-5.D-1*CPXX(10)/(T1*T1)
      ENDIF
      CPS3 = (-5.D-1)*CPXX(11)/((CPXX(12) - T1)*(CPXX(12) - T1))
      CPS4 = 5.D-1*CPXX(13)/((CPXX(14) - T1)*(CPXX(14) - T1))
      CPSXX = CPS1 + CPS1a + CPS2 + CPS2a + CPS3 + CPS4
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION XHGCP(P1,T1,XG,XL,XCP,TR)
C
C     Calculates excess parameters as a function of T using as input:
C     XG = excess Gibbs energy parameter at TR
C     XL = excess enthalpy parameter at TR
C     XCP = excess heat capacity function calculated from excess
C           heat capacity parameter function XJ via XCP = -T*T*XJ.
C     Note: that in this way T-independent XJ parameters lead to 
C           T-dependent excess heat capacity functions!  In order 
C           to make Cp^xs T-independent, the XCP(1) parameter has     
C           to be used, e.g., if XJ is valid for 298.15 K, 
C           then XCP(1) = -(298.15)**2*XJ
C
C     XCP = XCP(1) + XCP(2)/T + XCP(3)*LOG(T) + XCP(4)*T +
C           XCP(5)*T*T + XCP(6)/(T-XCP(7)) + XCP(8)/(XCP(9)-T) + 
C           XCP(10)/(T*T) + XCP(11)*T/(T-XCP(12))**3 +
C           XCP(13)*T/(XCP(14)-T)**3 + XCP(15)*T*T*T +
C           XCP(16)*T*T*T*T + XCP(17)*T*T*T*T*T
C
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      C = 0.D0
      DO I = 1,17
      C = C + XCP(I)
      ENDDO
      IF (XL .EQ. 0.D0 .AND. C .EQ. 0.D0) THEN
      XHGCP = XG*TR/T1
      ELSE
      XHGCP = XG + TR*(1.D0 - TR/T1)*XL + 
     >        (XCPH(P1,T1,XCP) - XCPH(P1,TR,XCP))/T1 -
     >        (XCPS(P1,T1,XCP) - XCPS(P1,TR,XCP))
      ENDIF
      RETURN
      END
C      
      DOUBLE PRECISION FUNCTION XCPH(P1,T1,XCP)
C     Integrate XCP dT --> Contribution to H
C     XCP = XCP(1) + XCP(2)/T + XCP(3)*LOG(T) + XCP(4)*T +
C           XCP(5)*T*T + XCP(6)/(T-XCP(7)) + XCP(8)/(XCP(9)-T) + 
C           XCP(10)/(T*T) + XCP(11)*T/(T-XCP(12))**3 +
C           XCP(13)*T/(XCP(14)-T)**3 + XCP(15)*T*T*T +
C           XCP(16)*T*T*T*T + XCP(17)*T*T*T*T*T
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      CPH1 = XCP(1)*T1 + (XCP(2) + XCP(3)*T1)*LOG(T1) - XCP(3)*T1 +
     > 5.D-1*XCP(4)*T1*T1 + XCP(5)*T1*T1*T1/3.D0  
      CPH2 = XCP(6)*LOG(T1-XCP(7)) - XCP(8)*LOG(ABS(XCP(9)-T1))
     > - XCP(10)/T1 + XCP(11)*(XCP(12) - 2.D0*T1)/(2.D0*(XCP(12) -
     > T1)*(XCP(12) - T1)) 
      CPH3 = XCP(13)*(2.D0*T1 - XCP(14))/(2.D0*(XCP(14) -
     > T1)*(XCP(14) - T1)) + XCP(15)*T1*T1*T1*T1/4.D0 +
     > XCP(16)*T1*T1*T1*T1*T1/5.D0 + XCP(17)*T1*T1*T1*T1*T1*T1/6.D0
      XCPH = CPH1 + CPH2 + CPH3
      RETURN
      END
C      
      DOUBLE PRECISION FUNCTION XCPS(P1,T1,XCP)
C     Integrate XCP/T dT --> Contribution to S
C     XCP = XCP(1) + XCP(2)/T + XCP(3)*LOG(T) + XCP(4)*T +
C           XCP(5)*T*T + XCP(6)/(T-XCP(7)) + XCP(8)/(XCP(9)-T) + 
C           XCP(10)/(T*T) + XCP(11)*T/(T-XCP(12))**3 +
C           XCP(13)*T/(XCP(14)-T)**3 + XCP(15)*T*T*T +
C           XCP(16)*T*T*T*T + XCP(17)*T*T*T*T*T
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      IF(XCP(7) .EQ. 0.D0 .AND. XCP(9) .EQ. 0.D0) THEN
      CPS1 = XCP(1)*LOG(T1)
     > - XCP(2)/T1 + 5.D-1*XCP(3)*LOG(T1)*LOG(T1) + XCP(4)*T1
      ELSEIF(XCP(7) .EQ. 0.D0) THEN
      CPS1 = (XCP(1) + XCP(8)/XCP(9))*LOG(T1)
     > - XCP(2)/T1 + 5.D-1*XCP(3)*LOG(T1)*LOG(T1) + XCP(4)*T1 
      ELSEIF(XCP(9) .EQ. 0.D0) THEN
      CPS1 = (XCP(1) - XCP(6)/XCP(7))*LOG(T1)
     > - XCP(2)/T1 + 5.D-1*XCP(3)*LOG(T1)*LOG(T1) + XCP(4)*T1 
      ELSE 
      CPS1 = (XCP(1) - XCP(6)/XCP(7) + XCP(8)/XCP(9))*LOG(T1)
     > - XCP(2)/T1 + 5.D-1*XCP(3)*LOG(T1)*LOG(T1) + XCP(4)*T1 
      ENDIF
      IF(XCP(7) .EQ. 0.D0 .AND. XCP(9) .EQ. 0.D0) THEN
      CPS2 = 5.D-1*XCP(5)*T1*T1 - 5.D-1*XCP(10)/(T1*T1)
      ELSEIF(XCP(7) .EQ. 0.D0) THEN
      CPS2 = 5.D-1*XCP(5)*T1*T1 
     > - XCP(8)*LOG(XCP(9)-T1)/XCP(9) - 5.D-1*XCP(10)/(T1*T1)
      ELSEIF(XCP(9) .EQ. 0.D0) THEN
      CPS2 = 5.D-1*XCP(5)*T1*T1 + XCP(6)*LOG(T1-XCP(7))/XCP(7)
     > - 5.D-1*XCP(10)/(T1*T1)
      ELSE
      CPS2 = 5.D-1*XCP(5)*T1*T1 + XCP(6)*LOG(T1-XCP(7))/XCP(7)
     > - XCP(8)*LOG(XCP(9)-T1)/XCP(9) - 5.D-1*XCP(10)/(T1*T1)
      ENDIF
      CPS3 = (-5.D-1)*XCP(11)/((XCP(12) - T1)*(XCP(12) - T1)) +
     > 5.D-1*XCP(13)/((XCP(14) - T1)*(XCP(14) - T1))
      CPS4 = XCP(15)*T1*T1*T1/3.D0 + XCP(16)*T1*T1*T1*T1/4.D0 +
     > XCP(17)*T1*T1*T1*T1*T1/5.D0
      XCPS = CPS1 + CPS2 + CPS3 + CPS4
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION G0H2O(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-J,N-Z)            
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
C      
C     IAPWS-IF97
C      
      DIMENSION I(34), J(34), N(34)
      DATA I /0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,1.D0,1.D0,1.D0,
     >        1.D0,1.D0,1.D0,2.D0,2.D0,2.D0,2.D0,2.D0,3.D0,3.D0,3.D0,
     >        4.D0,4.D0,4.D0,5.D0,8.D0,8.D0,21.D0,23.D0,29.D0,30.D0,
     >        31.D0,32.D0/
      DATA J /-2.D0,-1.D0,0.D0,1.D0,2.D0,3.D0,4.D0,5.D0,-9.D0,-7.D0,
     >        -1.D0,0.D0,1.D0,3.D0,-3.D0,0.D0,1.D0,3.D0,17.D0,-4.D0,
     >        0.D0,6.D0,-5.D0,-2.D0,10.D0,-8.D0,-11.D0,-6.D0,-29.D0,
     >        -31.D0,-38.D0,-39.D0,-40.D0,-41.D0/
      DATA N /0.14632971213167D0,-0.84548187169114D0,
     >        -0.37563603672040D1,0.33855169168385D1,
     >        -0.95791963387872D0,0.15772038513228D0,
     >        -0.16616417199501D-1,0.81214629983568D-3,
     >        0.28319080123804D-3,-0.60706301565874D-3,
     >        -0.18990068218419D-1,-0.32529748770505D-1,
     >        -0.21841717175414D-1,-0.52838357969930D-4,
     >        -0.47184321073267D-3,-0.30001780793026D-3,
     >        0.47661393906987D-4,-0.44141845330846D-5,
     >        -0.72694996297594D-15,-0.31679644845054D-4,
     >        -0.28270797985312D-5,-0.85205128120103D-9,
     >        -0.22425281908000D-5,-0.65171222895601D-6,
     >        -0.14341729937924D-12,-0.40516996860117D-6,
     >        -0.12734301741641D-8,-0.17424871230634D-9,
     >        -0.68762131295531D-18,0.14478307828521D-19,
     >        0.26335781662795D-22,-0.11947622640071D-22,
     >        0.18228094581404D-23,-0.93537087292458D-25/
      P = PXX/1.653D2
      T = 1.386D3/TXX
      G = 0.D0
      DO K = 1,34
      G = G + N(K)*(7.1D0 - P)**I(K)*(T - 1.222D0)**J(K)
      ENDDO
C     Change to Standard Element Reference State and adjustment of R
      RCORR = 1.0000024D0
      G0H2O = G*RCORR-(287720.31D0+63.33423D0*TXX)/(8.31451D0*TXX)
      RETURN
      END
C

      DOUBLE PRECISION FUNCTION DNSH2O(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-J,N-Z)            
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
C      
C     IAPWS-IF97
C      
      DIMENSION I(34), J(34), N(34)
      DATA I /0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,1.D0,1.D0,1.D0,
     >        1.D0,1.D0,1.D0,2.D0,2.D0,2.D0,2.D0,2.D0,3.D0,3.D0,3.D0,
     >        4.D0,4.D0,4.D0,5.D0,8.D0,8.D0,21.D0,23.D0,29.D0,30.D0,
     >        31.D0,32.D0/
      DATA J /-2.D0,-1.D0,0.D0,1.D0,2.D0,3.D0,4.D0,5.D0,-9.D0,-7.D0,
     >        -1.D0,0.D0,1.D0,3.D0,-3.D0,0.D0,1.D0,3.D0,17.D0,-4.D0,
     >        0.D0,6.D0,-5.D0,-2.D0,10.D0,-8.D0,-11.D0,-6.D0,-29.D0,
     >        -31.D0,-38.D0,-39.D0,-40.D0,-41.D0/
      DATA N /0.14632971213167D0,-0.84548187169114D0,
     >        -0.37563603672040D1,0.33855169168385D1,
     >        -0.95791963387872D0,0.15772038513228D0,
     >        -0.16616417199501D-1,0.81214629983568D-3,
     >        0.28319080123804D-3,-0.60706301565874D-3,
     >        -0.18990068218419D-1,-0.32529748770505D-1,
     >        -0.21841717175414D-1,-0.52838357969930D-4,
     >        -0.47184321073267D-3,-0.30001780793026D-3,
     >        0.47661393906987D-4,-0.44141845330846D-5,
     >        -0.72694996297594D-15,-0.31679644845054D-4,
     >        -0.28270797985312D-5,-0.85205128120103D-9,
     >        -0.22425281908000D-5,-0.65171222895601D-6,
     >        -0.14341729937924D-12,-0.40516996860117D-6,
     >        -0.12734301741641D-8,-0.17424871230634D-9,
     >        -0.68762131295531D-18,0.14478307828521D-19,
     >        0.26335781662795D-22,-0.11947622640071D-22,
     >        0.18228094581404D-23,-0.93537087292458D-25/
      P = PSAVE/1.653D2
      T = 1.386D3/TSAVE
      G = 0.D0
      DO K = 1,34
      G = G - I(K)*N(K)*(7.1D0 - P)**(I(K)-1.D0)*(T - 1.222D0)**J(K)
      ENDDO
      R = 0.461526D3
      PPA = 1.D5*PSAVE
      V = G*P*TSAVE*R/PPA
      DNSH2O = 1.D-3/V
      RETURN
      END
C

      DOUBLE PRECISION FUNCTION GH2OVAP(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-J,N-Z)            
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
C      
C     IAPWS-IF97
C      
      DIMENSION I(43), J(43), N(43), J0(9), N0(9)
      DATA J0 /0.D0,1.D0,-5.D0,-4.D0,-3.D0,-2.D0,-1.D0,2.D0,3.D0/
      DATA N0 /-0.96927686500217D1,0.10086655968018D2,
     >         -0.56087911283020D-2,0.71452738081455D-1,
     >         -0.40710498223928D0,0.14240819171444D1,
     >         -0.43839511319450D1,-0.28408632460772D0,
     >          0.21268463753307D-1/
      DATA I /1.D0,1.D0,1.D0,1.D0,1.D0,2.D0,2.D0,2.D0,2.D0,2.D0,3.D0,
     >        3.D0,3.D0,3.D0,3.D0,4.D0,4.D0,4.D0,5.D0,6.D0,6.D0,6.D0,
     >        7.D0,7.D0,7.D0,8.D0,8.D0,9.D0,10.D0,10.D0,10.D0,16.D0,
     >        16.D0,18.D0,20.D0,20.D0,20.D0,21.D0,22.D0,23.D0,24.D0,
     >        24.D0,24.D0/
      DATA J /0.D0,1.D0,2.D0,3.D0,6.D0,1.D0,2.D0,4.D0,7.D0,36.D0,0.D0,
     >        1.D0,3.D0,6.D0,35.D0,1.D0,2.D0,3.D0,7.D0,3.D0,16.D0,
     >        35.D0,0.D0,11.D0,25.D0,8.D0,36.D0,13.D0,4.D0,10.D0,
     >        14.D0,29.D0,50.D0,57.D0,20.D0,35D0,48.D0,21.D0,53.D0,
     >        39.D0,26.D0,40.D0,58.D0/
      DATA N /-0.17731742473213D-2,-0.17834862292358D-1,
     >        -0.45996013696365D-1,-0.57581259083432D-1,
     >        -0.50325278727930D-1,-0.33032641670203D-4,
     >        -0.18948987516315D-3,-0.39392777243355D-2,
     >        -0.43797295650573D-1,-0.26674547914087D-4,
     >         0.20481737692309D-7, 0.43870667284435D-6,
     >        -0.32277677238570D-4,-0.15033924542148D-2,
     >        -0.40668253562649D-1,-0.78847309559367D-9,
     >         0.12790717852285D-7, 0.48225372718507D-6,
     >         0.22922076337661D-5,-0.16714766451061D-10,
     >        -0.21171472321355D-2,-0.23895741934104D2,
     >        -0.59059564324270D-17,-0.12621808899101D-5,
     >        -0.38946842435739D-1, 0.11256211360459D-10,
     >        -0.82311340897998D1,  0.19809712802088D-7,
     >         0.10406965210174D-18,-0.10234747095929D-12,
     >        -0.10018179379511D-8,-0.80882908646985D-10,
     >         0.10693031879409D0, -0.33662250574171D0,
     >         0.89185845355421D-24,0.30629316876232D-12,
     >        -0.42002467698208D-5,-0.59056029685639D-25,
     >         0.37826947613457D-5,-0.12768608934681D-14,
     >         0.73087610595061D-28,0.55414715350778D-16,
     >        -0.94369707241210D-6/

      P = PXX/1.D1
      T = 5.4D2/TXX
      G = LOG(P)
      DO K = 1,9
      G = G + N0(K)*T**J0(K)
      ENDDO
      DO K = 1,43
      G = G + N(K)*P**I(K)*(T-0.5D0)**J(K)
      ENDDO
C     Change to Standard Element Reference State and adjustment of R
      RCORR = 1.0000024D0
      GH2OVAP = G*RCORR-(287720.31D0+63.33423D0*TXX)/(8.31451D0*TXX)
      RETURN
      END
C

      DOUBLE PRECISION FUNCTION PSAT(T)
      IMPLICIT DOUBLE PRECISION(A-H,N-Z)            
C      
C     IAPWS-IF97
C      
      DIMENSION N(10)
      DATA N /0.11670521452767D4,-0.72421316703206D6,
     >       -0.17073846940092D2, 0.12020824702470D5,
     >       -0.32325550322333D7, 0.14915108613530D2,
     >       -0.48232657361591D4, 0.40511340542057D6,
     >       -0.23855557567849D0, 0.65017534844798D3/

      IF (T .GT. 372.756D0) THEN

      TH = T + N(9)/(T-N(10))
      A = TH*(TH + N(1)) + N(2)
      B = TH*(TH*N(3) + N(4)) + N(5)
      C = TH*(TH*N(6) + N(7)) + N(8)
      PS = (2.D0*C/(SQRT(B*B-4.D0*A*C)-B))**4.D0
      PSAT = 1.D1*PS

      ELSE 
      PSAT = 1.D0
      ENDIF

      RETURN
      END


      DOUBLE PRECISION FUNCTION TSAT(P)
      IMPLICIT DOUBLE PRECISION(A-H,N-Z)            
C      
C     IAPWS-IF97
C      
      DIMENSION N(10)
      DATA N /0.11670521452767D4,-0.72421316703206D6,
     >       -0.17073846940092D2, 0.12020824702470D5,
     >       -0.32325550322333D7, 0.14915108613530D2,
     >       -0.48232657361591D4, 0.40511340542057D6,
     >       -0.23855557567849D0, 0.65017534844798D3/

      B = (P/1.D1)**0.25D0
      E = B*(B + N(3)) + N(6)
      F = B*(B*N(1) + N(4)) + N(7)
      G = B*(B*N(2) + N(5)) + N(8)
      D = 2.D0*G/(-SQRT(F*F-4.D0*E*G)-F)
      TSAT = (N(10)+D-SQRT((N(10)+D)**2.D0-4.D0*(N(9)+N(10)*D)))/2.D0

      RETURN
      END
C
      DOUBLE PRECISION FUNCTION G0NACL(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)            
      COMMON /NACL/ V0NACL
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION CPXX(14), CPXX1(14), CPXX2(14), VX(14), VP(14)
      P1 = PXX
      T1 = TXX
      TR2 = 3.8315D2
      HXX = -4.0742D5
      SXX = 1.1505D2
      IF (TSAVE. LT. TR2) THEN
C     0 - 110 C, 1 bar, Pitzer et al (1984), Table A 5 --> V3_NaCl.dat Curve expert
      VX(1) = 131663.93D0
      VX(2) = -2572975.5D0
      VX(3) = -25343.032D0
      VX(4) = 85.236575D0
      VX(5) = -0.05137802D0
      VX(6) = -45.906818D0
      VX(7) = 2.27D2
      VX(8) = 185397.18D0
      VX(9) = 6.47D2
C     0 - 110 C, 1 bar, Pitzer et al (1984), Table A 7 --> B1_NaCl.dat Curve expert     
      VP(1) = -75330.066D-2
      VP(2) = 1601231.D-2
      VP(3) = 14111.469D-2
      VP(4) = -41.058297D-2
      VP(5) = 0.019182144D-2
      VP(6) = 80.641291D-2
      VP(7) = 2.27D2
      VP(8) = 34057.316D-2
      VP(9) = 6.47D2
      DO I = 10,14
      VX(I) = 0.D0
      VP(I) = 0.D0
      ENDDO
C     
C     0 - 110 C, 1 bar, Pitzer et al (1984), Table A 4, --> Cp3_NaCl.dat Curve expert
      CPXX(1) = -335730.47D0
      CPXX(2) = 15095950.D0
      CPXX(3) = 45226.456D0
      CPXX(4) = 144.2046D0
      CPXX(5) = -0.27564185D0
      CPXX(6) = -61117.116D0
      CPXX(7) = 2.27D2
      CPXX(8) = 3383837.4D0
      CPXX(9) = 6.47D2
      CPXX(10) = 0.D0
      CPXX(11) = 0.D0
      CPXX(12) = 0.D0
      CPXX(13) = 0.D0
      CPXX(14) = 0.D0
C
      PR = 1.D0
      P0 = 1.D0
      VXXX = 1.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
      G0NACL = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX
      
      V0NACL = V0XP(P1, T1, VX, VP, P0)
C
      ELSE
C
C     0 - 110 C, 1 bar, Pitzer et al (1984), Table A 4, --> Cp3_NaCl.dat Curve expert
      CPXX1(1) = -335730.47D0
      CPXX1(2) = 15095950.D0
      CPXX1(3) = 45226.456D0
      CPXX1(4) = 144.2046D0
      CPXX1(5) = -0.27564185D0
      CPXX1(6) = -61117.116D0
      CPXX1(7) = 2.27D2
      CPXX1(8) = 3383837.4D0
      CPXX1(9) = 6.47D2
      CPXX1(10) = 0.D0
      CPXX1(11) = 0.D0
      CPXX1(12) = 0.D0
      CPXX1(13) = 0.D0
      CPXX1(14) = 0.D0
C
C     100 - 290 C, saturation p, Pitzer et al (1984), Table A 4, CurveExp. Cp4_NaCl
      CPXX2(1) = -81471.376D0
      CPXX2(2) = -16991055.D0
      CPXX2(3) = 37697.201D0
      CPXX2(4) = -359.19584D0
      CPXX2(5) = 0.24025356D0
      CPXX2(6) = 836017.29D0
      CPXX2(7) = 2.27D2
      CPXX2(8) = -397486.78D0
      CPXX2(9) = 6.47D2
      CPXX2(10) = 0.D0
      CPXX2(11) = 0.D0
      CPXX2(12) = 0.D0
      CPXX2(13) = 0.D0
      CPXX2(14) = 0.D0
C
C     90 - 300 C, sat. press., Pitzer et al (1984), Table A 5
      VX(1) = -96815.627D0
      VX(2) = 2602848.3D0
      VX(3) = 17352.751D0
      VX(4) = -40.189747D0
      VX(5) = 0.016063196D0
      VX(6) = -11881.634D0
      VX(7) = 2.27D2
      VX(8) = -17645.104D0
      VX(9) = 6.47D2
C     100 - 300 C, sat. press., Pitzer et al (1984), Table A 7
      VP(1) = 272837.36D-2
      VP(2) = -3101579.6D-2
      VP(3) = -53916.149D-2
      VP(4) = 184.09396D-2
      VP(5) = -0.094512561D-2
      VP(6) = -161863.84D-2
      VP(7) = 2.27D2
      VP(8) = 93469.169D-2
      VP(9) = 6.47D2
      DO I = 10,14
      VX(I) = 0.D0
      VP(I) = 0.D0
      ENDDO
C     
C     Correction for Psat/bar at T
C
      PR = PSAT(TSAVE)
      P0 = PSAT(TSAVE)
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
      P0 = 1.D0
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TSAVE,VX,VP,PR,P0)/
     >(8.31451D0*TSAVE)
      G0NACL = HSCPXX2(P1,T1,HXX,SXX,CPXX1,CPXX2,TR2) + VXXX + VXXC
      
      V0NACL = V0XP(P1, T1, VX, VP, P0)

      ENDIF
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA0NACL(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      P1 = PXX
      P2 = P1*P1
      P3 = P1*P2
      T1 = TXX
      T2 = T1*T1
      TA = T1 - 2.27D2
      TB = 6.8D2 - T1
      BETA0NACL1 = -656.81518D0/T1 + 24.869130D0 + 5.3812753D-5*P1 
     > -5.5887470D-8*P2 + 6.5893263D-12*P3 - 4.4640952D0*LOG(T1) 
     > + (1.1109914D-2 - 2.6573399D-7*P1 + 1.7460070D-10*P2 
     > + 1.0462619D-14*P3)*T1
      BETA0NACL2 =
     > + (-5.3070129D-6 + 8.6340233D-10*P1 - 4.1785962D-13*P2)*T2
     > + (-1.5793660D0 + 2.2022821D-3*P1 - 1.3105503D-7*P2 
     > - 6.3813683D-11*P3)/TA
      BETA0NACL3 =
     > + (9.7065780D0 - 2.6860396D-2*P1 + 1.5344744D-5*P2 
     > - 3.2153983D-9*P3)/TB 
      BETA0NACL = BETA0NACL1 + BETA0NACL2 + BETA0NACL3
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA1NACL(TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      T1 = TXX
      TA = T1 - 2.27D2
      BETA1NACL = 119.31966D0/T1 - 0.48309327D0 + 1.4068095D-3*T1 
     > - 4.2345814D0/TA
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CPHINACL(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      P1 = PXX
      T1 = TXX
      T2 = T1*T1
      TA = T1 - 2.27D2
      TB = 6.8D2 - T1
      CPHINACL = -6.1084589D0/T1 + 0.40217793D0 + 2.2902837D-5*P1 
     > - 0.75354649D-1*LOG(T1) + (1.5317673D-4 - 9.0550901D-8*P1)*T1 
     > + (-1.5386008D-8 + 8.6926600D-11*P1)*T2 + (0.35310414D0 
     > - 4.3314252D-4*P1)/TA + (-9.1871455D-2 + 5.1904777D-4*P1)/TB
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA0NACLlow(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      P1 = PXX
      P2 = P1*P1
      P3 = P1*P2
      T1 = TXX
      T2 = T1*T1
      TA = T1 - 2.27D2
      TB = 6.8D2 - T1
      BETA0NACL1 = -656.81518D0/T1 + 24.8791830D0 - 2.1552731D-5*P1 
     > + 5.0166855D-8*P2 - 4.4640952D0*LOG(T1) 
     > + (1.1087099D-2 - 6.4479761D-8*P1 - 2.3234032D-10*P2)*T1
      BETA0NACL2 =
     > + (-5.2194871D-6 + 2.4445210D-10*P1 + 2.8527066D-13*P2)*T2
     > + (-1.5696231D0 + 2.2337864D-3*P1 - 6.3933891D-7*P2 
     > + 4.5270573D-11*P3)/TA
      BETA0NACL3 = 5.4151933D0/TB 
      BETA0NACLlow = BETA0NACL1 + BETA0NACL2 + BETA0NACL3
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CPHINACLlow(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      P1 = PXX
      T1 = TXX
      T2 = T1*T1
      TA = T1 - 2.27D2
      TB = 6.8D2 - T1
      CPHINACLlow = -6.1084589D0/T1 + 0.40743803D0 - 6.8152430D-6*P1 
     > - 0.75354649D-1*LOG(T1) + (1.2609014D-4 + 6.2480692D-8*P1)*T1 
     > + (1.8994373D-8 - 1.0731284D-10*P1)*T2 + (0.32136572D0 
     > - 2.5382945D-4*P1)/TA 
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION G0NAOH(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)            
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAOH/ V0NAOH, MGC, MVC, MDLGC, MDLVC
      CHARACTER*65 MDLGC(10), MDLVC(10)
      DIMENSION CPXX(14),VX(14),VP(14),VX0(14),VP0(14)
      P1 = PXX
      T1 = TXX
C
      IF (MVC .EQ. 1) THEN
C
C     Simonson et al (1989), Table 4
C
      IF(TSAVE .LE. 348.36D0) THEN
      VX(1) = -47.2184D0
      VX(2) = 0.D0
      VX(3) = 0.D0
      VX(4) = 0.314497D0
      VX(5) = -5.16093D-4
      VX(6) = -445.889D0
      VX(7) = 2.27D2
      VX(8) = 0.D0
      VX(9) = 0.D0
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0
      IF(TSAVE .LE. 323.36D0) THEN
      VP(1) = 1.38604D0
      VP(2) = 0.D0
      VP(3) = 0.D0
      VP(4) = -8.16550D-3
      VP(5) = 1.25354D-5
      VP(6) = 0.939619D0
      VP(7) = 2.27D2
      VP(8) = 0.D0
      VP(9) = 0.D0
      VP(10) = 0.D0
      VP(11) = 0.D0
      VP(12) = 0.D0
      VP(13) = 0.D0
      VP(14) = 0.D0
      ELSE
      DO I = 1,14
      VP(I) = 0.D0
      ENDDO
      ENDIF
      ELSE
      VX(1) = -41.065892D0
      VX(2) = 0.D0
      VX(3) = 0.D0
      VX(4) = 0.35165694D0
      VX(5) = -0.00039289427D0
      VX(6) = 0.D0
      VX(7) = 0.D0
      VX(8) = -11272.344D0
      VX(9) = 6.47D2
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0
      DO I = 1,14
      VP(I) = 0.D0
      ENDDO
      ENDIF
C
      ELSEIF (MVC .EQ. 2) THEN
C
C     Fit to 25 C density data
C
      DO I = 1,14
      VX(I) = 0.D0
      VP(I) = 0.D0
      ENDDO
      VX(1) = -5.29060D0
C
      ELSEIF (MVC .EQ. 3) THEN
C
C     89SiR (0 - 50 C) and 06CoS (100 - 300 C)
C
      VP(1) = 1677.2839D0
      VP(2) = -37008.039D0
      VP(3) = -312.872D0
      VP(4) = 0.89310395D0
      VP(5) = -0.00043830501D0
      VP(6) = 24.506482D0
      VP(7) = 2.27D2
      VP(8) = 655.46529D0
      VP(9) = 6.47D2
      VP(10) = 0.D0
      VP(11) = 0.D0
      VP(12) = 0.D0
      VP(13) = 0.D0
      VP(14) = 0.D0
C      
C     V^0 at 298.15 K (89SiR)
C
      VX0(1) = -47.2184D0
      VX0(2) = 0.D0
      VX0(3) = 0.D0
      VX0(4) = 0.314497D0
      VX0(5) = -5.16093D-4
      VX0(6) = -445.889D0
      VX0(7) = 2.27D2
      VX0(8) = 0.D0
      VX0(9) = 0.D0
      VX0(10) = 0.D0
      VX0(11) = 0.D0
      VX0(12) = 0.D0
      VX0(13) = 0.D0
      VX0(14) = 0.D0
C
      VP0(1) = 1.38604D0
      VP0(2) = 0.D0
      VP0(3) = 0.D0
      VP0(4) = -8.16550D-3
      VP0(5) = 1.25354D-5
      VP0(6) = 0.939619D0
      VP0(7) = 2.27D2
      VP0(8) = 0.D0
      VP0(9) = 0.D0
      VP0(10) = 0.D0
      VP0(11) = 0.D0
      VP0(12) = 0.D0
      VP0(13) = 0.D0
      VP0(14) = 0.D0
C
      IF(TSAVE .LE. 350.36D0) THEN
C
C     [89SMR]
C
      VX(1) = -47.2184D0 + 1.520096D0
      VX(2) = 0.D0
      VX(3) = 0.D0
      VX(4) = 0.314497D0 - 0.005769D0
      VX(5) = -5.16093D-4
      VX(6) = -445.889D0
      VX(7) = 2.27D2
      VX(8) = 0.D0
      VX(9) = 0.D0
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0

      ELSE
C
C     Fit to 100 bar density data incl 275 C data
C
      VX(1) = -3512.8854D0
      VX(2) = 0.D0
      VX(3) = 793.94174D0
      VX(4) = -4.066471D0
      VX(5) = 0.0028271164D0
      VX(6) = 0.D0
      VX(7) = 0.D0
      VX(8) = -19135.677D0
      VX(9) = 6.47D2
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0

      ENDIF
      
      ELSEIF (MVC .EQ. 4) THEN
C
C     Pabalan & Pitzer (1987)
C
      VX(1) = 6.1602358D2
      VX(2) = -7.8003405D4
      VX(3) = 0.D0
      VX(4) = -1.4663461D0
      VX(5) = 1.3512006D-3
      VX(6) = 0.D0
      VX(7) = 0.D0
      VX(8) = -1.4796803D4
      VX(9) = 6.47D2
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0
      DO I = 1,14
      VP(I) = 0.D0
      ENDDO
C      
C     V^0 at 298.15 K (89SiR)
C
      VX0(1) = -47.2184D0
      VX0(2) = 0.D0
      VX0(3) = 0.D0
      VX0(4) = 0.314497D0
      VX0(5) = -5.16093D-4
      VX0(6) = -445.889D0
      VX0(7) = 2.27D2
      VX0(8) = 0.D0
      VX0(9) = 0.D0
      VX0(10) = 0.D0
      VX0(11) = 0.D0
      VX0(12) = 0.D0
      VX0(13) = 0.D0
      VX0(14) = 0.D0
C
      VP0(1) = 1.38604D0
      VP0(2) = 0.D0
      VP0(3) = 0.D0
      VP0(4) = -8.16550D-3
      VP0(5) = 1.25354D-5
      VP0(6) = 0.939619D0
      VP0(7) = 2.27D2
      VP0(8) = 0.D0
      VP0(9) = 0.D0
      VP0(10) = 0.D0
      VP0(11) = 0.D0
      VP0(12) = 0.D0
      VP0(13) = 0.D0
      VP0(14) = 0.D0
C
      ELSEIF (MVC .EQ. 5) THEN
C
C     Corti et al (1990)
C
      VX(1) = 105.32576D0
      VX(2) = 0.D0
      VX(3) = 0.D0
      VX(4) = -0.35672734D0
      VX(5) = 0.00069782588D0
      VX(6) = 0.D0
      VX(7) = 0.D0
      VX(8) = -20532.964D0
      VX(9) = 6.47D2
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0
      DO I = 1,14
      VP(I) = 0.D0
      ENDDO
C      
C     V^0 at 298.15 K (89SiR)
C
      VX0(1) = -47.2184D0
      VX0(2) = 0.D0
      VX0(3) = 0.D0
      VX0(4) = 0.314497D0
      VX0(5) = -5.16093D-4
      VX0(6) = -445.889D0
      VX0(7) = 2.27D2
      VX0(8) = 0.D0
      VX0(9) = 0.D0
      VX0(10) = 0.D0
      VX0(11) = 0.D0
      VX0(12) = 0.D0
      VX0(13) = 0.D0
      VX0(14) = 0.D0
C
      VP0(1) = 1.38604D0
      VP0(2) = 0.D0
      VP0(3) = 0.D0
      VP0(4) = -8.16550D-3
      VP0(5) = 1.25354D-5
      VP0(6) = 0.939619D0
      VP0(7) = 2.27D2
      VP0(8) = 0.D0
      VP0(9) = 0.D0
      VP0(10) = 0.D0
      VP0(11) = 0.D0
      VP0(12) = 0.D0
      VP0(13) = 0.D0
      VP0(14) = 0.D0
C
      ELSEIF (MVC .EQ. 6) THEN
C
C     89SiR (0 - 50 C) and 06CoS (100 - 300 C)
C
      VP(1) = 1677.2839D0
      VP(2) = -37008.039D0
      VP(3) = -312.872D0
      VP(4) = 0.89310395D0      
      VP(5) = -0.00043830501D0
      VP(6) = 24.506482D0
      VP(7) = 2.27D2
      VP(8) = 655.46529D0
      VP(9) = 6.47D2
      VP(10) = 0.D0
      VP(11) = 0.D0
      VP(12) = 0.D0
      VP(13) = 0.D0
      VP(14) = 0.D0
C      
C     V^0 at 298.15 K (89SiR)
C
      VX0(1) = -47.2184D0
      VX0(2) = 0.D0
      VX0(3) = 0.D0
      VX0(4) = 0.314497D0
      VX0(5) = -5.16093D-4
      VX0(6) = -445.889D0
      VX0(7) = 2.27D2
      VX0(8) = 0.D0
      VX0(9) = 0.D0
      VX0(10) = 0.D0
      VX0(11) = 0.D0
      VX0(12) = 0.D0
      VX0(13) = 0.D0
      VX0(14) = 0.D0
C
      VP0(1) = 1.38604D0
      VP0(2) = 0.D0
      VP0(3) = 0.D0
      VP0(4) = -8.16550D-3
      VP0(5) = 1.25354D-5
      VP0(6) = 0.939619D0
      VP0(7) = 2.27D2
      VP0(8) = 0.D0
      VP0(9) = 0.D0
      VP0(10) = 0.D0
      VP0(11) = 0.D0
      VP0(12) = 0.D0
      VP0(13) = 0.D0
      VP0(14) = 0.D0

      IF(TSAVE .LE. 350.36D0) THEN
C
C     [89SMR]
C
      VX(1) = -47.2184D0 + 1.520096D0
      VX(2) = 0.D0
      VX(3) = 0.D0
      VX(4) = 0.314497D0 - 0.005769D0
      VX(5) = -5.16093D-4
      VX(6) = -445.889D0
      VX(7) = 2.27D2
      VX(8) = 0.D0
      VX(9) = 0.D0
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0

      ELSE
C
C     Fit to 100 bar density data excl 275 C data
C
      VX(1) = -2322.0951D0
      VX(2) = 0.D0
      VX(3) = 525.83317D0
      VX(4) = -2.6695409D0
      VX(5) = 0.0019070114D0
      VX(6) = 0.D0
      VX(7) = 0.D0
      VX(8) = -18138.779D0
      VX(9) = 6.47D2
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0

      ENDIF
C
      ENDIF      
C     
      IF (MGC .EQ. 1) THEN
C
C     p = 7 MPa, Simonson et al (1989), Table 3
C
      CPXX(1) = -325.646D0
      CPXX(2) = 0.D0
      CPXX(3) = 0.D0
      CPXX(4) = 2.26011D0
      CPXX(5) = -3.76119D-3
      CPXX(6) = 0.D0
      CPXX(7) = 0.D0
      CPXX(8) = 0.D0
      CPXX(9) = 0.D0
      CPXX(10) = 0.D0
      CPXX(11) = -2.28323D5
      CPXX(12) = 210.D0
      CPXX(13) = -1.19721D6
      CPXX(14) = 647.D0

      PR = 70.D0
C
      IF (MVC .EQ. 1) THEN
C
      P0 = 1.D0
      TR = 2.9815D2
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX,VP,PR,P0)/(8.31451D0*TR)
      P0 = PSAT(TSAVE)
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
C
      ELSEIF (MVC .EQ. 3) THEN
C
      P0 = 1.D0
      TR = 2.9815D2
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX0,VP0,PR,P0)/(8.31451D0*TR)
      P0 = 100.D0
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
c      print*, vxxx, vxxc
c      pause
C
      ELSEIF (MVC .EQ. 4) THEN
C
      P0 = 1.D0
      TR = 2.9815D2
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX0,VP0,PR,P0)/(8.31451D0*TR)
      P0 = PSAT(TSAVE)
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
C
      ELSEIF (MVC .EQ. 5) THEN
C
      P0 = 1.D0
      TR = 2.9815D2
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX0,VP0,PR,P0)/(8.31451D0*TR)
      P0 = PSAT(TSAVE)
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
C
      ELSEIF (MVC .EQ. 6) THEN
C
      P0 = 1.D0
      TR = 2.9815D2
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX0,VP0,PR,P0)/(8.31451D0*TR)
      P0 = 100.D0
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
C
      ENDIF
C
      ELSEIF (MGC .EQ. 2) THEN
C
      CPXX(1) = -97.79D0
      DO I = 2,14
      CPXX(I) = 0.D0
      ENDDO
      PR = 1.D0
      P0 = 1.D0
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
      VXXC = 0.D0
C      
      ELSEIF (MGC .EQ. 3) THEN
C
      CPXX(1) = -101.44D0
      DO I = 2,14
      CPXX(I) = 0.D0
      ENDDO
      PR = 1.D0
      P0 = 1.D0
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
      VXXC = 0.D0
C
      ELSEIF (MGC .EQ. 4) THEN

      IF (TSAVE .LE. 350.36D0) THEN
C
C     p = 7 MPa, Simonson et al (1989), Table 3
C
      CPXX(1) = -325.646D0 + 23.34615D0
      CPXX(2) = 0.D0
      CPXX(3) = 0.D0
      CPXX(4) = 2.26011D0 - 0.07692D0
      CPXX(5) = -3.76119D-3
      CPXX(6) = 0.D0
      CPXX(7) = 0.D0
      CPXX(8) = 0.D0
      CPXX(9) = 0.D0
      CPXX(10) = 0.D0
      CPXX(11) = -2.28323D5
      CPXX(12) = 210.D0
      CPXX(13) = -1.19721D6
      CPXX(14) = 647.D0

      PR = 70.D0
c      PR = 100.D0

      ELSE
C
C     p = 10 MPa, New model
C
c      Simon's extrapolation
c
c      CPXX(1) = -1667482.2D0
c      CPXX(2) = 39186092.D0
c      CPXX(3) = 307026.04D0
c      CPXX(4) = -821.01375D0
c      CPXX(5) = 0.37708936D0
c      CPXX(6) = -66105.784D0
c      CPXX(7) = 227.D0      
c      CPXX(8) = -403625.D0  
c      CPXX(9) = 647.D0      
c      CPXX(10) = 0.D0
c      CPXX(11) = 0.D0
c      CPXX(12) = 0.D0
c      CPXX(13) = 0.D0
c      CPXX(14) = 0.D0
C
C      CPXX(1) = -1563263.3D0

      CPXX(1) = -1563253.3D0
      CPXX(2) = 36566523.D0
      CPXX(3) = 288163.14D0
      CPXX(4) = -775.23835D0
      CPXX(5) = 0.35895126D0
      CPXX(6) = -61733.97D0
      CPXX(7) = 227.D0      
      CPXX(8) = -426848.18D0  
      CPXX(9) = 647.D0      
      CPXX(10) = 0.D0
      CPXX(11) = 0.D0
      CPXX(12) = 0.D0
      CPXX(13) = 0.D0
      CPXX(14) = 0.D0

      PR = 100.D0
      
      ENDIF
C
      IF (MVC .EQ. 1) THEN
C
      P0 = 1.D0
      TR = 2.9815D2
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX,VP,PR,P0)/(8.31451D0*TR)
      P0 = PSAT(TSAVE)
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
c      print*, vxxx, vxxc
C
      ELSEIF (MVC .EQ. 3) THEN
C
      P0 = 1.D0
      TR = 2.9815D2
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX0,VP0,PR,P0)/(8.31451D0*TR)
      
c      print*,p0,pr,vxxc
c      pause

      IF (TSAVE .LE. 350.36D0) THEN
      P0 = 1.D0
c      P0 = 70.D0
c      P0 = 100.D0
      ELSE
      P0 = 100.D0
      ENDIF
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1) 

c     >+ (LOG(DNSH2O1(P1,T1)/DNSH2O1(P0,T1))-(P1-P0)*CAPPA(P0,T1))*
c     >(-10536.19D0-702413.9D0/(T1-227.D0)-65.64918D0*T1)/(8.31451D0*T1)
c      print*, vxxx, vxxc
c      pause
C
      ELSEIF (MVC .EQ. 4) THEN
C
      P0 = 1.D0
      TR = 2.9815D2
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX0,VP0,PR,P0)/(8.31451D0*TR)
      P0 = PSAT(TSAVE)
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
c      print*, vxxx, vxxc
C
      ELSEIF (MVC .EQ. 5) THEN
C
      P0 = 1.D0
      TR = 2.9815D2
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX0,VP0,PR,P0)/(8.31451D0*TR)
      P0 = PSAT(TSAVE)
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
c      print*, vxxx, vxxc
C
      ELSEIF (MVC .EQ. 6) THEN
C
      P0 = 1.D0
      TR = 2.9815D2
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX0,VP0,PR,P0)/(8.31451D0*TR)

      IF (TSAVE .LE. 350.36D0) THEN
      P0 = 1.D0
c      P0 = 70.D0
c      P0 = 100.D0
      ELSE
      P0 = 100.D0
      ENDIF
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
c      print*, vxxx, vxxc
C
      ENDIF
C
      ENDIF
C
C     CODATA, corr. for pressure (1 --> 70 (or 100) bar) by VXXC term    
C
      HXX = -470355.D0
      SXX = 47.55D0
      G0NAOH = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX + VXXC
      
c      print*,p0,pr,vxxx,vxxc
c      pause
      
      V0NAOH = V0XP(PSAVE, TSAVE, VX, VP, P0) 

c     >+1.D1*(CAPPA(P1,T1)-CAPPA(P0,T1))*(-10536.19D0-702413.9D0/
c     >(T1-227.D0)-65.64918D0*T1)

      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CAPPA(PRESS,TEMK)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DP = PRESS*.001D0
      PU = PRESS + DP
      PL = PRESS - DP
      RHOU = DNSH2O1(PU,TEMK)
      RHOL = DNSH2O1(PL,TEMK)
      RHO = DNSH2O1(PRESS,TEMK)
      drdp = (RHOU-RHOL)/(2.D0*DP)
      cappa = drdp/rho
      end

      DOUBLE PRECISION FUNCTION DNSH2O1(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-J,N-Z)            
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
C      
C     IAPWS-IF97
C      
      DIMENSION I(34), J(34), N(34)
      DATA I /0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,1.D0,1.D0,1.D0,
     >        1.D0,1.D0,1.D0,2.D0,2.D0,2.D0,2.D0,2.D0,3.D0,3.D0,3.D0,
     >        4.D0,4.D0,4.D0,5.D0,8.D0,8.D0,21.D0,23.D0,29.D0,30.D0,
     >        31.D0,32.D0/
      DATA J /-2.D0,-1.D0,0.D0,1.D0,2.D0,3.D0,4.D0,5.D0,-9.D0,-7.D0,
     >        -1.D0,0.D0,1.D0,3.D0,-3.D0,0.D0,1.D0,3.D0,17.D0,-4.D0,
     >        0.D0,6.D0,-5.D0,-2.D0,10.D0,-8.D0,-11.D0,-6.D0,-29.D0,
     >        -31.D0,-38.D0,-39.D0,-40.D0,-41.D0/
      DATA N /0.14632971213167D0,-0.84548187169114D0,
     >        -0.37563603672040D1,0.33855169168385D1,
     >        -0.95791963387872D0,0.15772038513228D0,
     >        -0.16616417199501D-1,0.81214629983568D-3,
     >        0.28319080123804D-3,-0.60706301565874D-3,
     >        -0.18990068218419D-1,-0.32529748770505D-1,
     >        -0.21841717175414D-1,-0.52838357969930D-4,
     >        -0.47184321073267D-3,-0.30001780793026D-3,
     >        0.47661393906987D-4,-0.44141845330846D-5,
     >        -0.72694996297594D-15,-0.31679644845054D-4,
     >        -0.28270797985312D-5,-0.85205128120103D-9,
     >        -0.22425281908000D-5,-0.65171222895601D-6,
     >        -0.14341729937924D-12,-0.40516996860117D-6,
     >        -0.12734301741641D-8,-0.17424871230634D-9,
     >        -0.68762131295531D-18,0.14478307828521D-19,
     >        0.26335781662795D-22,-0.11947622640071D-22,
     >        0.18228094581404D-23,-0.93537087292458D-25/
      P = PXX/1.653D2
      T = 1.386D3/TXX
      G = 0.D0
      DO K = 1,34
      G = G - I(K)*N(K)*(7.1D0 - P)**(I(K)-1.D0)*(T - 1.222D0)**J(K)
      ENDDO
      R = 0.461526D3
      PPA = 1.D5*PXX
      V = G*P*TXX*R/PPA
      DNSH2O1 = 1.D-3/V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA0NAOH(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAOH/ V0NAOH, MGC, MVC, MDLGC, MDLVC
      CHARACTER*65 MDLGC(10), MDLVC(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
C      
      IF (MGC .EQ. 1) THEN
C
      PR = 7.D1
      T2 = T1*T1
      TA = T1 - 2.1D2
      TB = 6.47D2 - T1
      BETA0 = 12.6677D0 - 389.77D0/T1 - 2.13287*LOG(T1) + 3.42184D-3*T1
     > - 11.661D-7*T2 - 4.16833D0/TA + 1.68567D0/TB
C
      ELSEIF (MGC .EQ. 2) THEN
C     
      TR = 2.9815D2 
      PR = 1.D0
      XG = 0.0864D0
      XL = 0.5307D-3
      XJ = -1.41D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGC .EQ. 3) THEN
C     
      TR = 2.9815D2 
      PR = 1.D0
      XG = 0.0864D0
      XL = 0.5307D-3
      XJ = -1.7744D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGC .EQ. 4) THEN
C
      P0 = 1.D2
      PR = 1.D2
      T2 = T1*T1
      TA = T1 - 2.1D2
      TB = 6.47D2 - T1
      BETA0 = 12.6677D0 - 389.77D0/T1 - 2.13287*LOG(T1) + 3.42184D-3*T1
     > - 11.661D-7*T2 - 4.16833D0/TA + 1.68567D0/TB
C
      ENDIF
C
      BETA0NAOH = BETA0 + 1.D-1*(P1 - PR)*BETA0NAOHVK(PXX,TXX)
C
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA0NAOHVK(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAOH/ V0NAOH, MGC, MVC, MDLGC, MDLVC
      CHARACTER*65 MDLGC(10), MDLVC(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
C      
      IF (MVC .EQ. 1) THEN
C
      PR = 7.D1
      TC = T1 - 2.27D2
      IF (TSAVE .LE. 323.36D0) THEN
      BETA0K = 1.01784D-6 - 2.20494D-4/TC
      ELSE
      BETA0K = 0.D0
      ENDIF
      IF (TSAVE .LE. 348.36D0) THEN
      BETA0V = -3.24287D-5 + 1.83077D-2/TC 
      ELSE
      TB = 6.47D2 - T1
      BETA0V = 5.02915D-3 - 3.68325D-2/TB - 1.03663D-3*LOG(T1) +
     > 3.82255D-6*T1
      ENDIF
C
      ELSEIF (MVC .EQ. 2) THEN
C
C     Fit to 25 C density data
C
      PR = 1.D0
      BETA0K = 0.D0
      BETA0V = 1.D1*0.63283594D-01/(8.31451D0*TSAVE)
C
      ELSEIF (MVC .EQ. 3) THEN

      IF (TSAVE .LE. 350.36D0) THEN
      PR = 1.D2
      P0 = 1.D2
c      P0 = 1.D0
c      PR = 7.D1
      TC = T1 - 2.27D2
      BETA0K = 1.01784D-6 - 2.20494D-4/TC
      BETA0V = -3.24287D-5 + 1.83077D-2/TC 
      ELSE
C
C     Fit to 100 bar density data
C
      PR = 1.D2
      P0 = 1.D2
      TB = 6.47D2 - T1
      BETA0K = -1.725603D-11*T1*T1 + 7.181401D-9*T1
C
C     excl 275 C data
C
c      BETA0V = 0.0034055747D0 - 3.5872013D-6*T1 + 7.3984325D-9*T1*T1 - 
c     > 0.074058039D0/TB - 0.00045385769D0*LOG(T1)
C
C     incl 275 C data
C
c      BETA0V = 0.0061210626D0 - 4.0168362D-7*T1 + 5.3002843D-9*T1*T1 - 
c     > 0.071784958D0/TB - 0.0010652528D0*LOG(T1)
C
C     exact V^0 function incl 275 C data
C
      BETA0V = -0.0014447796D0 - 9.1640097D-6*T1 + 1.1001668D-8*T1*T1 - 
     > 0.077528072D0/TB + 0.00063412614D0*LOG(T1)
c      print*,'0 ', beta0v
c      pause
C
C     exact V^0 function excl 275 C data
C
c      BETA0V = 0.0019407043D0 - 5.2562126D-6*T1 + 8.4627532D-9*T1*T1 - 
c     > 0.074932365D0/TB - 0.0001257795D0*LOG(T1)
C
      ENDIF
      ELSEIF (MVC .EQ. 4) THEN
C
C     Pabalan & Pitzer (1987)
C
      PR = PSAT(TSAVE)
      TB = 6.47D2 - T1
      BETA0K = 0.D0
C
      BETA0V = 1.D1*(-2.8131778D-3 + 3.7012540D-1/T1 + 7.1788733D-6*T1 
     > - 5.8847404D-9*T1*T1 - 4.8217410D-3/TB)
C
      ELSEIF (MVC .EQ. 5) THEN
C
C     Corti et al (1990)
C
      PR = PSAT(TSAVE)
      TB = 6.47D2 - T1
      BETA0K = 0.D0
C
      BETA0V = 1.D1*(-0.00073017818D0 + 3.4780351D-6*T1 
     > - 5.2035942D-9*T1*T1 + 0.046802156D0/TB)
C
      ELSEIF (MVC .EQ. 6) THEN
c
      IF (TSAVE .LE. 350.36D0) THEN
      PR = 1.D2
      P0 = 1.D2
c      P0 = 1.D0
c      PR = 7.D1
      TC = T1 - 2.27D2
      BETA0K = 1.01784D-6 - 2.20494D-4/TC
      BETA0V = -3.24287D-5 + 1.83077D-2/TC 
      ELSE
C
C     Fit to 100 bar density data
C
      PR = 1.D2
      P0 = 1.D2
      TB = 6.47D2 - T1
      BETA0K = -1.725603D-11*T1*T1 + 7.181401D-9*T1
C
C     excl 275 C data
C
c      BETA0V = 0.0034055747D0 - 3.5872013D-6*T1 + 7.3984325D-9*T1*T1 - 
c     > 0.074058039D0/TB - 0.00045385769D0*LOG(T1)
C
C     incl 275 C data
C
c      BETA0V = 0.0061210626D0 - 4.0168362D-7*T1 + 5.3002843D-9*T1*T1 - 
c     > 0.071784958D0/TB - 0.0010652528D0*LOG(T1)
C
C     exact V^0 function incl 275 C data
C
c      BETA0V = -0.0014447796D0 - 9.1640097D-6*T1 + 1.1001668D-8*T1*T1 - 
c     > 0.077528072D0/TB + 0.00063412614D0*LOG(T1)
c      print*,'0 ', beta0v
c      pause
C
C     exact V^0 function excl 275 C data
C
      BETA0V = 0.0019407043D0 - 5.2562126D-6*T1 + 8.4627532D-9*T1*T1 - 
     > 0.074932365D0/TB - 0.0001257795D0*LOG(T1)
C
      ENDIF
      ENDIF
C      
C     Integrate (V(P0) + (P - P0)V')dP from Pr to P --->
C     (P - Pr)[V(P0) + {0.5(P + Pr) - P0}V']
C     For MGC = 1, P0 = 0.1 MPa,  Pr = 7 MPa  --> factor 0.1
C     For MGC = 2, P0 = 0.1 MPa,  Pr = 0.1 MPa  --> factor 0.1
C
      BETA0NAOHVK = BETA0V + 1.D-1*(0.5D0*(P1 + PR) - P0)*BETA0K
C
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA1NAOH(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAOH/ V0NAOH, MGC, MVC, MDLGC, MDLVC
      CHARACTER*65 MDLGC(10), MDLVC(10)
      P1 = PXX
      T1 = TXX
C      
      IF (MGC .EQ. 1) THEN
C
      PR = 7.D1
      T2 = T1*T1
      TA = T1 - 2.1D2
      TB = 6.47D2 - T1
      BETA1 = -60.547D0 + 2708.27D0/T1 + 9.31351*LOG(T1) 
     > - 94.2226D-7*T2 - 43.0272D0/TA + 11.961D0/TB
C
      ELSEIF (MGC .EQ. 2) THEN
C     
      TR = 2.9815D2 
      PR = 1.D0
      XG = 0.253D0
      XL = 0.7495D-3
      XJ = -3.76D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGC .EQ. 3) THEN
C     
      TR = 2.9815D2 
      PR = 1.D0
      XG = 0.253D0
      XL = 0.7495D-3
      XJ = -3.6431D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGC .EQ. 4) THEN
C
      P0 = 1.D2
      PR = 1.D2
      T2 = T1*T1
      TA = T1 - 2.1D2
      TB = 6.47D2 - T1
      BETA1 = -60.547D0 + 2708.27D0/T1 + 9.31351*LOG(T1) 
     > - 94.2226D-7*T2 - 43.0272D0/TA + 11.961D0/TB
C
      ENDIF
C     
      BETA1NAOH = BETA1 + 1.D-1*(P1 - PR)*BETA1NAOHV(PXX,TXX)
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA1NAOHV(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAOH/ V0NAOH, MGC, MVC, MDLGC, MDLVC
      CHARACTER*65 MDLGC(10), MDLVC(10)
      P1 = PXX
      T1 = TXX
C      
      IF (MVC .EQ. 1) THEN
C
      IF(TSAVE .LE. 348.36D0) THEN
      BETA1NAOHV = 3.79568D-3 - 1.04928D-5*T1
      ELSE
      BETA1NAOHV = 0.D0
      ENDIF
C
      ELSEIF (MVC .EQ. 2) THEN
C
C     Fit to 25 C density data
C
      BETA1NAOHV = 1.D1*0.99006953D-1/(8.31451D0*TSAVE)
C
      ELSEIF (MVC .EQ. 3) THEN
      P0 = 1.D2

      IF(TSAVE .LE. 350.36D0) THEN
      BETA1NAOHV = 3.79568D-3 - 1.04928D-5*T1
      ELSE
C
C     Fit to 100 bar density data
C
      TB = 6.47D2 - T1
C
C     excl 275 C data
C
c      BETA1NAOHV = -0.056313707D0 - 8.7593121D-5*T1 + 
c     >6.5950557D-8*T1*T1 + 0.018147493D0/TB + 0.013486833D0*LOG(T1)
C
C     incl 275 C data
C
c      BETA1NAOHV = -0.024049004D0 - 4.9743064D-5*T1 + 
c     >4.1020186D-8*T1*T1 + 0.045158417D0/TB + 0.0062223784D0*LOG(T1)
C
C     exact V^0 function incl 275 C data
C
      BETA1NAOHV = 0.019245357D0 - 2.4897101D-6*T1 + 
     >1.2014476D-8*T1*T1 + 0.063902573D0/TB - 0.0033975961D0*LOG(T1)
c      print*,'1 ',BETA1NAOHV
c      pause
C
C     exact V^0 function excl 275 C data
C
c      BETA1NAOHV = -0.03843121D0 - 6.7336952D-5*T1 + 
c     >5.3086995D-8*T1*T1 + 0.028359466D0/TB + 0.0094863891D0*LOG(T1)
C
      ENDIF
      ELSEIF (MVC .EQ. 4) THEN
C
C     Pabalan & Pitzer (1987)
C
      BETA1NAOHV = 0.D0
C
      ELSEIF (MVC .EQ. 5) THEN
C
C     Corti et al (1990)
C
      BETA1NAOHV = 0.D0
C
      ELSEIF (MVC .EQ. 6) THEN

      IF(TSAVE .LE. 350.36D0) THEN
      BETA1NAOHV = 3.79568D-3 - 1.04928D-5*T1
      ELSE
C
C     Fit to 100 bar density data
C
      TB = 6.47D2 - T1
C
C     excl 275 C data
C
c      BETA1NAOHV = -0.056313707D0 - 8.7593121D-5*T1 + 
c     >6.5950557D-8*T1*T1 + 0.018147493D0/TB + 0.013486833D0*LOG(T1)
C
C     incl 275 C data
C
c      BETA1NAOHV = -0.024049004D0 - 4.9743064D-5*T1 + 
c     >4.1020186D-8*T1*T1 + 0.045158417D0/TB + 0.0062223784D0*LOG(T1)
C
C     exact V^0 function incl 275 C data
C
c      BETA1NAOHV = 0.019245357D0 - 2.4897101D-6*T1 + 
c     >1.2014476D-8*T1*T1 + 0.063902573D0/TB - 0.0033975961D0*LOG(T1)
c      print*,'1 ',BETA1NAOHV
c      pause
C
C     exact V^0 function excl 275 C data
C
      BETA1NAOHV = -0.03843121D0 - 6.7336952D-5*T1 + 
     >5.3086995D-8*T1*T1 + 0.028359466D0/TB + 0.0094863891D0*LOG(T1)
C
      ENDIF
      ENDIF
C      
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA2NAOH(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /NAOH/ V0NAOH, MGC, MVC, MDLGC, MDLVC
      CHARACTER*65 MDLGC(10), MDLVC(10)
      T1 = TXX
C
      IF (MGC .EQ. 1 .OR. MGC .EQ. 4) THEN
C
      TB = 6.47D2 - T1
      BETA2NAOH = -340.559D0*(-0.19554D0 - 7.4581D-5*T1 + 4.534D0/T1 +
     > 3.505D-2*LOG(T1) + 1.D0/TB)
C
      ELSE
C
      BETA2NAOH = 0.D0
C
      ENDIF
C            
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CPHINAOH(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAOH/ V0NAOH, MGC, MVC, MDLGC, MDLVC
      CHARACTER*65 MDLGC(10), MDLVC(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
C      
      IF (MGC .EQ. 1) THEN
C
      PR = 7.D1
      T2 = T1*T1
      TA = T1 - 2.1D2
C
      CPHI = -0.01687D0 + 2.23824D0/T1 + 0.31094D-7*T2 + 1.01063D0/TA 
C
      ELSEIF (MGC .EQ. 2) THEN
C     
      TR = 2.9815D2 
      PR = 1.D0
      XG = 0.0044D0
      XL = -0.1355D-3
      XJ = 0.134D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGC .EQ. 3) THEN
C     
      TR = 2.9815D2 
      PR = 1.D0
      XG = 0.0044D0
      XL = -0.1355D-3
      XJ = 0.23387D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGC .EQ. 4) THEN
C
      P0 = 1.D2
      PR = 1.D2
      T2 = T1*T1
      T3 = T1*T1*T1
      TA = T1 - 2.1D2
      TB = 6.47D2 - T1

C     [89SMR]      
C
C      CPHI = -0.01687D0 + 2.23824D0/T1 + 0.31094D-7*T2 + 1.01063D0/TA 
C
C     changed to fit cp at 291.8 C
C
      CPHI = -0.01687D0 + 2.23824D0/T1 + 0.31094D-7*T2 + 1.01063D0/TA  
     >- 0.8D-10*T3/TB
C
      ENDIF
C
      CPHINAOH = CPHI + 1.D-1*(P1 - PR)*CPHINAOHVK(PXX,TXX)
C
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CPHINAOHVK(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAOH/ V0NAOH, MGC, MVC, MDLGC, MDLVC
      CHARACTER*65 MDLGC(10), MDLVC(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
C      
      IF (MVC .EQ. 1) THEN
C
      PR = 7.D1
      IF (TSAVE .LE. 323.36D0) THEN
      CPHIK = 1.90754D-6 - 5.5092D-9*T1
      ELSE
      CPHIK = 0.D0
      ENDIF
      IF (TSAVE .LE. 348.36D0) THEN
      CPHIV = -1.16142D-4 + 3.45044D-7*T1 
      ELSE
      CPHIV = 4.18652D-5 - 1.41605D-7*T1
      ENDIF
C
      ELSEIF (MVC .EQ. 2) THEN
C
C     Fit to 25 C density data
C
      PR = 1.D0
      CPHIK = 0.D0
      CPHIV = -1.D1*0.43578214D-02/(8.31451D0*TSAVE)
C
      ELSEIF (MVC .EQ. 3) THEN

      IF (TSAVE .LE. 350.36D0) THEN
      P0 = 1.D2
      PR = 1.D2
c      P0 = 1.D0
c      PR = 7.D1
      CPHIK = 1.90754D-6 - 5.5092D-9*T1
      CPHIV = -1.16142D-4 + 3.45044D-7*T1 
      ELSE
C
C     Fit to 100 bar density data
C
      P0 = 1.D2
      PR = 1.D2
      TB = 6.47D2 - T1
      CPHIK = 0.D0
C
C     excl 275 C data
C
c      CPHIV = -0.00029121719D0 + 1.9318234D-7*T1 - 4.8037D-10*T1*T1 + 
c     > 0.0039899813D0/TB + 4.4700988D-5*LOG(T1)
C
C     incl 275 C data
C
c      CPHIV = -0.00079895059D0 - 4.0249951D-7*T1 - 8.7984951D-11*T1*T1 + 
c     > 0.0035646638D0/TB + 0.00015902002D0*LOG(T1)
C
C     exact V^0 function incl 275 C data
C
      CPHIV = 0.0002617618D0 + 8.2758696D-7*T1 - 8.8917226D-10*T1*T1 + 
     > 0.0043732013D0/TB - 7.9287977D-5*LOG(T1)
c      print*,'C ', cphiv
c      pause
C
C     exact V^0 function excl 275 C data
C
c      CPHIV = -8.0859864D-5 + 4.3262059D-7*T1 - 6.3289639D-10*T1*T1 + 
c     > 0.0041136196D0/TB - 2.4025458D-6*LOG(T1)
C
      ENDIF

      ELSEIF (MVC .EQ. 4) THEN
C
C     Pabalan & Pitzer (1987)
C
      PR = PSAT(TSAVE)
      CPHIK = 0.D0
C
      CPHIV = 1.D1*(4.0534778D-4 - 5.1714017D-2/T1 - 1.0553037D-6*T1 +
     > 8.9893405D-10*T1*T1)
C
      ELSEIF (MVC .EQ. 5) THEN
C
C     Corti et al (1990)
C
      PR = PSAT(TSAVE)
      TB = 6.47D2 - T1
      CPHIK = 0.D0
C
      CPHIV = 1.D1*(0.0009359541D0 - 4.3411648D-6*T1 + 
     > 5.9915349D-9*T1*T1 - 0.039306672D0/TB)
C
      ELSEIF (MVC .EQ. 6) THEN

      IF (TSAVE .LE. 350.36D0) THEN
      P0 = 1.D2
      PR = 1.D2
c      P0 = 1.D0
c      PR = 7.D1
      CPHIK = 1.90754D-6 - 5.5092D-9*T1
      CPHIV = -1.16142D-4 + 3.45044D-7*T1 
      ELSE
C
C     Fit to 100 bar density data
C
      P0 = 1.D2
      PR = 1.D2
      TB = 6.47D2 - T1
      CPHIK = 0.D0
C
C     excl 275 C data
C
c      CPHIV = -0.00029121719D0 + 1.9318234D-7*T1 - 4.8037D-10*T1*T1 + 
c     > 0.0039899813D0/TB + 4.4700988D-5*LOG(T1)
C
C     incl 275 C data
C
c      CPHIV = -0.00079895059D0 - 4.0249951D-7*T1 - 8.7984951D-11*T1*T1 + 
c     > 0.0035646638D0/TB + 0.00015902002D0*LOG(T1)
C
C     exact V^0 function incl 275 C data
C
c      CPHIV = 0.0002617618D0 + 8.2758696D-7*T1 - 8.8917226D-10*T1*T1 + 
c     > 0.0043732013D0/TB - 7.9287977D-5*LOG(T1)
c      print*,'C ', cphiv
c      pause
C
C     exact V^0 function excl 275 C data
C
      CPHIV = -8.0859864D-5 + 4.3262059D-7*T1 - 6.3289639D-10*T1*T1 + 
     > 0.0041136196D0/TB - 2.4025458D-6*LOG(T1)
C
      ENDIF
      ENDIF
C      
C     Integrate (V(P0) + (P - P0)V')dP from Pr to P --->
C     (P - Pr)[V(P0) + {0.5(P + Pr) - P0}V']
C     For MGC = 1, P0 = 0.1 MPa,  Pr = 7 MPa  --> factor 0.1
C     For MGC = 2, P0 = 0.1 MPa,  Pr = 0.1 MPa  --> factor 0.1
C
      CPHINAOHVK = CPHIV + 1.D-1*(0.5D0*(P1 + PR) - P0)*CPHIK
C
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION G0NA2SO4(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)            
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NA2SO4/ V0NA2SO4, MGT, MVT, MDLGT, MDLVT
      CHARACTER*65 MDLGT(10), MDLVT(10)
      DIMENSION CPXX(14),VX(14),VP(14)
      P1 = PXX
      T1 = TXX
      TR = 2.9815D2
C
C     CODATA with 6USERXX.FOR
C     HXX = -1.39002D6 
C     SXX = 1.354D2 
C     Pabalan & Pitzer (1988) SXX, HXX adj.  with 5USERXX.DAT
      HXX = -1.389538D6
      SXX = 1.3812D2 
C
      IF (MGT .EQ. 6) THEN
C
C     0-175 C: Holmes and Mesmer, 200-300 C: SUPCRT92, CurveExpt Na2SO4C0.DAT
C
      CPXX(1) = -1244200.9D0
      CPXX(2) = 33167766.D0
      CPXX(3) = 223615.93D0
      CPXX(4) = -528.96497D0
      CPXX(5) = 0.22293607D0
      CPXX(6) = -147143.87D0
      CPXX(7) = 2.27D2
      CPXX(8) = -471593.77D0
      CPXX(9) = 6.47D2
      CPXX(10) = 0.D0
      CPXX(11) = 0.D0
      CPXX(12) = 0.D0
      CPXX(13) = 0.D0
      CPXX(14) = 0.D0
C
      ELSEIF (MGT .EQ. 5) THEN
C
      CPXX(1) = -198.35D0
      DO I = 2,14
      CPXX(I) = 0.D0
      ENDDO
C      
      ELSEIF (MGT .EQ. 4) THEN
C
      CPXX(1) = -195.57D0
      DO I = 2,14
      CPXX(I) = 0.D0
      ENDDO
C      
      ELSEIF (MGT .EQ. 3) THEN
C
C     25 - 300 C, 200 bar, Pabalan & Pitzer (1988), Table 3
C
      CPXX(1) = 1.7278418D6
      CPXX(2) = -4.6433635D7
      CPXX(3) = -3.0786428D5
      CPXX(4) = 6.8028314D2
      CPXX(5) = -2.4386675D-1
      CPXX(6) = 1.0033867D5
      CPXX(7) = 2.27D2
      CPXX(8) = -2.5274859D5
      CPXX(9) = 6.47D2
      CPXX(10) = 0.D0
      CPXX(11) = 0.D0
      CPXX(12) = 0.D0
      CPXX(13) = 0.D0
      CPXX(14) = 0.D0
C
      ELSEIF (MGT .EQ. 2) THEN
C
C     25 - 300 C, Pabalan & Pitzer (1988), Table 5, fitted to Psat data
C
      CPXX(1) = -939548.27D0
      CPXX(2) = 20107007.D0
      CPXX(3) = 177101.97D0
      CPXX(4) = -535.32292D0
      CPXX(5) = 0.28140946D0
      CPXX(6) = -49766.289D0
      CPXX(7) = 2.27D2
      CPXX(8) = -643162.24D0
      CPXX(9) = 6.47D2
      CPXX(10) = 0.D0
      CPXX(11) = 0.D0
      CPXX(12) = 0.D0
      CPXX(13) = 0.D0
      CPXX(14) = 0.D0
C
      ELSEIF (MGT .EQ. 1) THEN
C
C     Holmes and Mesmer (1986)
C
      CPXX(1) = -1206.2D0 
      CPXX(2) = 0.D0   
      CPXX(3) = 0.D0  
      CPXX(4) = 7.6405D0 
      CPXX(5) = -1.23672D-2 
      CPXX(6) = -6045.D0 
      CPXX(7) = 2.63D2       
      CPXX(8) = 0.D0 
      CPXX(9) = 0.D0       
      CPXX(10) = 0.D0        
      CPXX(11) = 0.D0        
      CPXX(12) = 0.D0        
      CPXX(13) = 0.D0        
      CPXX(14) = 0.D0        
C
      ENDIF      
C
      IF (MVT .EQ. 2) THEN
C
C     Same as for NaCl
C
      IF (TSAVE. LT. 3.7315D2) THEN
C     0 - 110 C, 1 bar, Pitzer et al (1984), Table A 5 --> V3_NaCl.dat Curve expert
      VX(1) = 131663.93D0
      VX(2) = -2572975.5D0
      VX(3) = -25343.032D0
      VX(4) = 85.236575D0
      VX(5) = -0.05137802D0
      VX(6) = -45.906818D0
      VX(7) = 2.27D2
      VX(8) = 185397.18D0
      VX(9) = 6.47D2
C     0 - 110 C, 1 bar, Pitzer et al (1984), Table A 7 --> B1_NaCl.dat Curve expert     
      VP(1) = -75330.066D-2
      VP(2) = 1601231.D-2
      VP(3) = 14111.469D-2
      VP(4) = -41.058297D-2
      VP(5) = 0.019182144D-2
      VP(6) = 80.641291D-2
      VP(7) = 2.27D2
      VP(8) = 34057.316D-2
      VP(9) = 6.47D2
      DO I = 10,14
      VX(I) = 0.D0
      VP(I) = 0.D0
      ENDDO
C     
      ELSE
C      
C     90 - 300 C, sat. press., Pitzer et al (1984), Table A 5
      VX(1) = -96815.627D0
      VX(2) = 2602848.3D0
      VX(3) = 17352.751D0
      VX(4) = -40.189747D0
      VX(5) = 0.016063196D0
      VX(6) = -11881.634D0
      VX(7) = 2.27D2
      VX(8) = -17645.104D0
      VX(9) = 6.47D2
C     100 - 300 C, sat. press., Pitzer et al (1984), Table A 7
      VP(1) = 272837.36D-2
      VP(2) = -3101579.6D-2
      VP(3) = -53916.149D-2
      VP(4) = 184.09396D-2
      VP(5) = -0.094512561D-2
      VP(6) = -161863.84D-2
      VP(7) = 2.27D2
      VP(8) = 93469.169D-2
      VP(9) = 6.47D2
      DO I = 10,14
      VX(I) = 0.D0
      VP(I) = 0.D0
      ENDDO
C
      ENDIF
C 
C     Twice as for NaCl    
C
      P0 = PSAT(TSAVE)
      PR = 2.D2
      VXXX = 2.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
      P0 = 1.D0
      VXXC = 2.D-1*(PR - P0)*VXP(PR,TR,VX,VP,PR,P0)/(8.31451D0*TR)
C      
      V0NA2SO4 = V0XP(P1, T1, VX, VP, P0)
C
      ELSEIF (MVT .EQ. 1) THEN      
C
C     Monnin 1990
C
      VX(1) = 1.281259D3
      VX(2) = -1.231424D5
      VX(3) = 0.D0
      VX(4) = -3.292342D0
      VX(5) = 4.267199D-3
      VX(6) = -1.067946D3
      VX(7) = 2.27D2
      VX(8) = -9.132116D4
      VX(9) = 6.8D2
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0
C     Factor 10 for bar, minus for definition of compressibility
      VP(1) = 1.D1*(2.33D-2 - 7.2D-4*1.281259D3)
      VP(2) = 1.D1*7.2D-4*1.231424D5
      VP(3) = 0.D0
      VP(4) = 1.D1*7.2D-4*3.292342D0
      VP(5) = -1.D1*7.2D-4*4.267199D-3
      VP(6) = 1.D1*7.2D-4*1.067946D3
      VP(7) = 2.27D2
      VP(8) = 1.D1*7.2D-4*9.132116D4
      VP(9) = 6.8D2
      VP(10) = 0.D0
      VP(11) = 0.D0
      VP(12) = 0.D0
      VP(13) = 0.D0
      VP(14) = 0.D0
C      
C     Correction for Psat/bar at T
C      
      PR = PSAT(TSAVE)
      P0 = PSAT(TSAVE)
      VXXX = 1.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
      P0 = 1.D0
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TSAVE,VX,VP,PR,P0)/
     >(8.31451D0*TSAVE)
C      
      V0NA2SO4 = V0XP(P1, T1, VX, VP, P0)
C
      ELSEIF (MVT .EQ. 4) THEN      
C
C     Monnin 1990, Pr = 200 bar
C
      VX(1) = 1.281259D3
      VX(2) = -1.231424D5
      VX(3) = 0.D0
      VX(4) = -3.292342D0
      VX(5) = 4.267199D-3
      VX(6) = -1.067946D3
      VX(7) = 2.27D2
      VX(8) = -9.132116D4
      VX(9) = 6.8D2
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0
C     Factor 10 for bar, minus for definition of compressibility
      VP(1) = 1.D1*(2.33D-2 - 7.2D-4*1.281259D3)
      VP(2) = 1.D1*7.2D-4*1.231424D5
      VP(3) = 0.D0
      VP(4) = 1.D1*7.2D-4*3.292342D0
      VP(5) = -1.D1*7.2D-4*4.267199D-3
      VP(6) = 1.D1*7.2D-4*1.067946D3
      VP(7) = 2.27D2
      VP(8) = 1.D1*7.2D-4*9.132116D4
      VP(9) = 6.8D2
      VP(10) = 0.D0
      VP(11) = 0.D0
      VP(12) = 0.D0
      VP(13) = 0.D0
      VP(14) = 0.D0
C      
C     Correction for Psat/bar at T
C      
      PR = 2.D2
      P0 = PSAT(TSAVE)
      VXXX = 1.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
      P0 = 1.D0
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TSAVE,VX,VP,PR,P0)/
     >(8.31451D0*TSAVE)
C      
      V0NA2SO4 = V0XP(P1, T1, VX, VP, P0)
C
      ELSEIF (MVT .EQ. 3) THEN
C
C     Krumgalz et al. (2000)      
C     
      TX = T1 - TR
      P0 = 1.D0
      V0NA2SO4 = (11.776D0 + TX*(22.858D-2 + TX*(-4.2216D-3 
     > + 1.7764D-5*TX)))/(8.31451D0*T1)
      VXXX = 1.D-1*(P1 - P0)*V0NA2SO4
      VXXC = 0.D0
C
      ENDIF
C      
      G0NA2SO4 = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX + VXXC
      
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA0NA2SO4(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NA2SO4/ V0NA2SO4, MGT, MVT, MDLGT, MDLVT
      CHARACTER*65 MDLGT(10), MDLVT(10)
      DIMENSION Q(7),P(11)
      P1 = PXX
      T1 = TXX
      T2 = T1*T1
      TR = 2.9815D2
      RNA = 2.27D2
      TA = T1 - RNA
C      
      IF (MGT .EQ. 2 .OR. MGT .EQ. 3) THEN
C
      IF (MGT .EQ. 2) THEN
C
C     Pabalan & Pitzer:  Parameters for Psat only for phi and gamma, not for derivatives!!!
C      
      P(1) = 2.1549644D-2   
      P(2) = -7.6918219D-1  
      P(3) = -3.5486084D-3  
      P(4) = 4.0811837D-6   
      P(5) = 0.D0           
      P(6) = 0.D0           
      P(7) = 0.D0           
      P(8) = 3915.434531D0  
      P(9) = 1.738512D-3    
      P(10) = 55.769488D0   
      P(11) = -1.255087D-2  
C
      ELSEIF (MGT .EQ. 3) THEN
C
C     Pabalan & Pitzer:  Parameters for 200 bar
C
      P(1) = 6.0955633D-1      
      P(2) = -1.6090797D1       
      P(3) = -1.0932828D-1     
      P(4) = 2.5321479D-4      
      P(5) = -9.9384034D-8     
      P(6) = 4.0107638D-2      
      P(7) = 2.1711348D-2      
      P(8) = 92308.895357D0    
      P(9) = 1.722469D-3        
      P(10) = 963.974106D0     
      P(11) = -1.04936D-2     
C
      ENDIF
C
      T3 = T1*T2
      T4 = T1*T3
      RNB = 6.47D2
      TB = ABS(RNB - T1)
      BETA0NA2SO41 = P(1)*T2/6.D0 + P(2)*T1/2.D0 + P(3)*T2*(LOG(T1) -
     > 5.D0/6.D0)/6.D0 + P(4)*T3/1.2D1 + P(5)*T4/2.D1 + P(6)*(T1/2.D0 +
     > 3.D0*RNA*RNA/(2.D0*T1) + RNA*TA*LOG(TA)/T1)
      BETA0NA2SO42 = -P(7)*(T1/2.D0 + 3.D0*RNB*RNB/(2.D0*T1) - RNB*TB  
     >*LOG(TB)/T1) - P(8)/T1 - P(9)*TR*TR/T1 + P(10) + P(11)
      BETA0 = BETA0NA2SO41 + BETA0NA2SO42 
C
      ELSEIF (MGT .EQ. 1) THEN
C
C     Holmes and Mesmer (1986)
C
      Q(1) = -1.727D-2
      Q(2) = 1.7828D-3
      Q(3) = 9.133D-6
      Q(4) = 0.D0
      Q(5) = -6.552D0
      Q(6) = 0.D0
      Q(7) = -96.9D0
      RNC = 2.63D2
      RND = 6.8D2
      TR2 = TR*TR
      TR3 = TR2*TR
      BETA0NA2SO41 = Q(1) + Q(2)*(TR - TR2/T1) + Q(3)*(T2 + 2.D0*TR3/T1
     > - 3.D0*TR2) + Q(4)*(T1 + TR2/T1 - 2.D0*TR) + Q(5)*(LOG(T1/TR) +
     > TR/T1 - 1.D0)
      BETA0NA2SO42 = Q(6)*(1.D0/(T1 - RNC) + (RNC*T1 - TR2)/(T1*(TR -
     > RNC)*(TR - RNC))) + Q(7)*(1.D0/(RND - T1) + (TR2 - RND*T1)/
     > (T1*(RND - TR)*(RND - TR)))
      BETA0 = BETA0NA2SO41 + BETA0NA2SO42 
C      write(33,*)'beta0   ',beta0
C
      ELSEIF (MGT .EQ. 4) THEN
C
      XG = 0.02492D0*0.75D0
      XL = 2.367D-3
      XJ = -5.8094D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGT .EQ. 5) THEN
C
      XG = 0.02492D0*0.75D0
      XL = 2.367D-3
      XJ = -4.2281D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGT .EQ. 6) THEN
C
      BETA0 = -9.6959137D0 + T1*(0.10583252D0 + T1*(-0.00046172551D0 +
     > T1*(1.0087615D-06 + T1*(-1.1001743D-09 + T1*4.7830523D-13))))
C
      ENDIF     
C      
      IF (MVT .EQ. 1) THEN
C      
C     Monnin 1990
C
      BETA0VNA2SO4 = 5.3088D-5 + 4.33707D-6*T1 - 3.262413D-9*T2 - 
     >2.70953D-4*LOG(T1) + 1.42266D-1/T1 + 4.51986D-3/TA
      BETA0KNA2SO4 = 2.4D-8 - 1.14D-3*BETA0VNA2SO4
C
C     Integrate (V(P0) + (P - P0)V')dP from Pr to P --->
C     (P - Pr)[V(P0) + {0.5(P + Pr) - P0}V']
C     
C     Monnin, Holmes and Mesmer:  PR = PSAT, P0 = PSAT  -  TEST  !!!!!!!!!
C     !!!!!!!! was P0 = 1 in BAYER 1.0 to 1.2 !!!!!!!!
C
      PR = PSAT(TSAVE)
      P0 = PSAT(TSAVE)
C            
      BETA0VKNA2SO4 = BETA0VNA2SO4 + (0.5D0*(P1+PR)-P0)*BETA0KNA2SO4
      BETA0NA2SO4 = BETA0 + (P1 - PR)*BETA0VKNA2SO4
C
      ELSEIF (MVT .EQ. 4) THEN
C      
C     Monnin 1990, Pr = 200 bar
C
      BETA0VNA2SO4 = 5.3088D-5 + 4.33707D-6*T1 - 3.262413D-9*T2 - 
     >2.70953D-4*LOG(T1) + 1.42266D-1/T1 + 4.51986D-3/TA
      BETA0KNA2SO4 = 2.4D-8 - 1.14D-3*BETA0VNA2SO4
C
C     Integrate (V(P0) + (P - P0)V')dP from Pr to P --->
C     (P - Pr)[V(P0) + {0.5(P + Pr) - P0}V']
C     
      PR = 2.D2
      P0 = 1.D0
C            
      BETA0VKNA2SO4 = BETA0VNA2SO4 + (0.5D0*(P1+PR)-P0)*BETA0KNA2SO4
      BETA0NA2SO4 = BETA0 + (P1 - PR)*BETA0VKNA2SO4
C
      ELSEIF (MVT .EQ. 2) THEN
C
      P1 = PXX
      P2 = P1*P1
      P3 = P1*P2
      PR = 2.D2
      PR2 = PR*PR
      PR3 = PR*PR2
      TB = ABS(6.8D2 - T1)
      BETA0NACL1P = 5.3812753D-5*(P1-PR)
     > -5.5887470D-8*(P2-PR2) + 6.5893263D-12*(P3-PR3) 
     > + (-2.6573399D-7*(P1-PR) + 1.7460070D-10*(P2-PR2) 
     > + 1.0462619D-14*(P3-PR3))*T1
      BETA0NACL2P =
     > (8.6340233D-10*(P1-PR) - 4.1785962D-13*(P2-PR2))*T2
     > + (2.2022821D-3*(P1-PR) - 1.3105503D-7*(P2-PR2) 
     > - 6.3813683D-11*(P3-PR3))/TA
      BETA0NACL3P =
     >(-2.6860396D-2*(P1-PR) + 1.5344744D-5*(P2-PR2) 
     > - 3.2153983D-9*(P3-PR3))/TB 
      BETA0NACLP = BETA0NACL1P + BETA0NACL2P + BETA0NACL3P
      BETA0NA2SO4 = BETA0 + 2.D0*BETA0NACLP
C
      ELSEIF (MVT .EQ. 3) THEN
C     
C     Krumgalz et al. (2000)
C     
      TX = T1 - TR
      PR = 1.D0
      BETA0V = 5.3250D-5 + TX*(-10.93D-7 + TX*(55.828D-9 
     > - 4.6248D-10*TX))
      BETA0NA2SO4 = BETA0 + (P1 - PR)*BETA0V
C
      ENDIF
C
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA1NA2SO4(PXX,TXX,SQSION)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NA2SO4/ V0NA2SO4, MGT, MVT, MDLGT, MDLVT
      CHARACTER*65 MDLGT(10), MDLVT(10)
      DIMENSION Q(7), P(11)
      P1 = PXX
      T1 = TXX
      T2 = T1*T1
      TR = 2.9815D2
      RNA = 2.27D2
      TA = T1 - RNA
C      
      IF (MGT .EQ. 2 .OR. MGT .EQ. 3) THEN
C
      IF (MGT .EQ. 2) THEN      
C
C     Pabalan & Pitzer:  Parameters for Psat only for phi and gamma, not for derivatives!!!
C
      P(1) = 3.6439508D0
      P(2) = -9.1962646D1
      P(3) = -6.5961772D-1
      P(4) = 1.6043868D-3
      P(5) = -6.5972836D-7
      P(6) = 1.6491982D-1
      P(7) = 2.2057312D-1
      P(8) = 708447.986899D0
      P(9) = 5.820066D-3
      P(10) = 5768.102375D0
      P(11) = 7.03766D-1
C
      ELSEIF (MGT .EQ. 3) THEN
C
C     Pabalan & Pitzer:  Parameters for 200 bar
C
      P(1) = 1.1040235D0
      P(2) = -2.5758534D1
      P(3) = -2.0290775D-1
      P(4) = 5.3309441D-4
      P(5) = -2.3576724D-7
      P(6) = 0.D0
      P(7) = 1.4455381D-1
      P(8) = 363078.716679D0
      P(9) = 5.512612D-3 
      P(10) = 1926.602872D0
      P(11) = 6.90077D-1
C
      ENDIF
C
      RNB = 6.47D2
      T3 = T1*T2
      T4 = T1*T3
      TB = ABS(RNB - T1)
      BETA1NA2SO41 = P(1)*T2/6.D0 + P(2)*T1/2.D0 + P(3)*T2*(LOG(T1) -
     > 5.D0/6.D0)/6.D0 + P(4)*T3/1.2D1 + P(5)*T4/2.D1 + P(6)*(T1/2.D0 +
     > 3.D0*RNA*RNA/(2.D0*T1) + RNA*TA*LOG(TA)/T1)
      BETA1NA2SO42 = -P(7)*(T1/2.D0 + 3.D0*RNB*RNB/(2.D0*T1) - RNB*TB  
     >*LOG(TB)/T1) - P(8)/T1 - P(9)*TR*TR/T1 + P(10) + P(11)
      BETA1 = BETA1NA2SO41 + BETA1NA2SO42 
C
      ELSEIF (MGT .EQ. 1) THEN
C
C     Holmes and Mesmer (1986)
C
      Q(1) = 0.7534D0
      Q(2) = 5.61D-3
      Q(3) = -5.7513D-4
      Q(4) = 1.11068D0
      Q(5) = -378.82D0
      Q(6) = 0.D0
      Q(7) = 1861.3D0
      RNA = 2.27D2
      RNC = 2.63D2
      RND = 6.8D2
      P1 = PXX
      T1 = TXX
      T2 = T1*T1
      TR = 2.9815D2
      TR2 = TR*TR
      TR3 = TR2*TR
      TA = T1 - RNA
      BETA1NA2SO41 = Q(1) + Q(2)*(TR - TR2/T1) + Q(3)*(T2 + 2.D0*TR3/T1
     > - 3.D0*TR2) + Q(4)*(T1 + TR2/T1 - 2.D0*TR) + Q(5)*(LOG(T1/TR) +
     > TR/T1 - 1.D0)
      BETA1NA2SO42 = Q(6)*(1.D0/(T1 - RNC) + (RNC*T1 - TR2)/(T1*(TR -
     > RNC)*(TR - RNC))) + Q(7)*(1.D0/(RND - T1) + (TR2 - RND*T1)/
     > (T1*(RND - TR)*(RND - TR)))
      BETA1 = BETA1NA2SO41 + BETA1NA2SO42 
C      write(33,*)'beta1   ',beta1
C
      ELSEIF (MGT .EQ. 4) THEN
C
      XG = 1.466D0*0.75D0
      XL = 5.63D-3
      XJ = -5.1549D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
C     Change of BETA1 from alpha = 2 to alpha = 1.4
      CONFAC = 1.D0
      IF(SQSION.GT.0.D0) THEN
      GF = 1.4D0*SQSION
      GFE = 2.D0*EXP(-GF)
      GX1 = (2.D0 - GF*GFE - GFE)/(GF*GF)
      GF = 2.D0*SQSION
      GFE = 2.D0*EXP(-GF)
      GX2 = (2.D0 - GF*GFE - GFE)/(GF*GF)
      IF(GX1.NE.0.D0) CONFAC = GX2/GX1
      ENDIF
      BETA1 = CONFAC*XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGT .EQ. 5) THEN
C
      XG = 1.466D0*0.75D0
      XL = 5.63D-3
      XJ = -12.063D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
C     Change of BETA1 from alpha = 2 to alpha = 1.4
      CONFAC = 1.D0
      IF(SQSION.GT.0.D0) THEN
      GF = 1.4D0*SQSION
      GFE = 2.D0*EXP(-GF)
      GX1 = (2.D0 - GF*GFE - GFE)/(GF*GF)
      GF = 2.D0*SQSION
      GFE = 2.D0*EXP(-GF)
      GX2 = (2.D0 - GF*GFE - GFE)/(GF*GF)
      IF(GX1.NE.0.D0) CONFAC = GX2/GX1
      ENDIF
      BETA1 = CONFAC*XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGT .EQ. 6) THEN
C
      BETA1 = -40.873695D0 + T1*(0.47867333D0 + T1*(-0.0021984072D0 +
     > T1*(5.0467572D-06 + T1*(-5.7831414D-09 + T1*2.6624651D-12))))
C
      ENDIF     
C      
      IF (MVT .EQ. 1) THEN
C      
C     Monnin 1990
C
      PR = PSAT(TSAVE)
      BETA1VNA2SO4 = 3.200188D-3 - 1.092875D-6*T1 - 7.101661D-9*T2 - 
     >8.373935D-1/T1 + 4.901041D-2/TA
C     Change of BETA1VNA2SO4 from alpha = 2 to alpha = 1.4
      CONFAC = 1.D0
      IF(SQSION.GT.0.D0) THEN
      GF = 1.4D0*SQSION
      GFE = 2.D0*EXP(-GF)
      GX1 = (2.D0 - GF*GFE - GFE)/(GF*GF)
      GF = 2.D0*SQSION
      GFE = 2.D0*EXP(-GF)
      GX2 = (2.D0 - GF*GFE - GFE)/(GF*GF)
      IF(GX1.NE.0.D0) CONFAC = GX2/GX1
      ENDIF
      BETA1NA2SO4 = BETA1 + CONFAC*(P1 - PR)*BETA1VNA2SO4
C
      ELSEIF (MVT .EQ. 4) THEN
C      
C     Monnin 1990, Pr = 200 bar
C
      PR = 2.D2
      BETA1VNA2SO4 = 3.200188D-3 - 1.092875D-6*T1 - 7.101661D-9*T2 - 
     >8.373935D-1/T1 + 4.901041D-2/TA
C     Change of BETA1VNA2SO4 from alpha = 2 to alpha = 1.4
      CONFAC = 1.D0
      IF(SQSION.GT.0.D0) THEN
      GF = 1.4D0*SQSION
      GFE = 2.D0*EXP(-GF)
      GX1 = (2.D0 - GF*GFE - GFE)/(GF*GF)
      GF = 2.D0*SQSION
      GFE = 2.D0*EXP(-GF)
      GX2 = (2.D0 - GF*GFE - GFE)/(GF*GF)
      IF(GX1.NE.0.D0) CONFAC = GX2/GX1
      ENDIF
      BETA1NA2SO4 = BETA1 + CONFAC*(P1 - PR)*BETA1VNA2SO4
C
      ELSEIF (MVT .EQ. 2) THEN
C
      BETA1NA2SO4 = BETA1 
C
      ELSEIF (MVT .EQ. 3) THEN
C     
C     Krumgalz et al. (2000)
C     
      TX = T1 - TR
      PR = 1.D0
      BETA1V = 12.9320D-5 + TX*(5.1406D-6 + TX*(-1.3311D-7 
     > + 0.1552D-8*TX))
C     Change of BETA1VNA2SO4 from alpha = 2 to alpha = 1.4
      CONFAC = 1.D0
      IF(SQSION.GT.0.D0) THEN
      GF = 1.4D0*SQSION
      GFE = 2.D0*EXP(-GF)
      GX1 = (2.D0 - GF*GFE - GFE)/(GF*GF)
      GF = 2.D0*SQSION
      GFE = 2.D0*EXP(-GF)
      GX2 = (2.D0 - GF*GFE - GFE)/(GF*GF)
      IF(GX1.NE.0.D0) CONFAC = GX2/GX1
      ENDIF
      BETA1NA2SO4 = BETA1 + CONFAC*(P1 - PR)*BETA1V
C
      ENDIF
C
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CPHINA2SO4(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NA2SO4/ V0NA2SO4, MGT, MVT, MDLGT, MDLVT
      CHARACTER*65 MDLGT(10), MDLVT(10)
      DIMENSION Q(7), P(11)
      P1 = PXX
      T1 = TXX
      T2 = T1*T1
      TR = 2.9815D2
      RNA = 2.27D2
      TA = T1 - RNA
C      
      IF (MGT .EQ. 2 .OR. MGT .EQ. 3) THEN
C
      IF (MGT .EQ. 2) THEN
C
C     Pabalan & Pitzer:  Parameters for Psat only for phi and gamma, not for derivatives!!!
C
      P(1) = -4.659076D-2
      P(2) = 1.1711403D0
      P(3) = 8.4319701D-3
      P(4) = -2.043955D-5
      P(5) = 8.3348147D-9
      P(6) = -1.3147008D-3
      P(7) = -1.6797063D-3
      P(8) = -6717.929066D0
      P(9) = -1.117462D-4
      P(10) = -68.202263D0
      P(11) = 3.80855D-3
C
      ELSEIF (MGT .EQ. 3) THEN
C
C     Pabalan & Pitzer:  Parameters for 200 bar
C
      P(1) = -1.0300087D-1
      P(2) = 2.6412809D0
      P(3) = 1.8580153D-2
      P(4) = -4.4349928D-5
      P(5) = 1.7883333D-8
      P(6) = -5.1836594D-3
      P(7) = -3.8805720D-3
      P(8) = -15447.360134D0
      P(9) = -1.152988D-4 
      P(10) = -155.941254D0
      P(11) = 3.74436D-3
C
      ENDIF
C
      RNB = 6.47D2
      T3 = T1*T2
      T4 = T1*T3
      TB = ABS(RNB - T1)
      CPHINA2SO41 = (P(1)*T2/6.D0 + P(2)*T1/2.D0 + P(3)*T2*(LOG(T1) -
     > 5.D0/6.D0)/6.D0 + P(4)*T3/1.2D1 + P(5)*T4/2.D1 + P(6)*(T1/2.D0 +
     > 3.D0*RNA*RNA/(2.D0*T1) + RNA*TA*LOG(TA)/T1))*(2.D0**1.5D0)
      CPHINA2SO42 = (-P(7)*(T1/2.D0 + 3.D0*RNB*RNB/(2.D0*T1) - RNB*TB  
     >*LOG(TB)/T1) - P(8)/T1-P(9)*TR*TR/T1+P(10)+P(11))*(2.D0**1.5D0)
      CPHI = CPHINA2SO41 + CPHINA2SO42 
C
      ELSEIF (MGT .EQ. 1) THEN
C
C     Holmes and Mesmer (1986)
C
      Q(1) = 1.1745D-2
      Q(2) = -3.3038D-4
      Q(3) = 1.85794D-5
      Q(4) = -3.92D-2
      Q(5) = 14.213D0
      Q(6) = 0.D0
      Q(7) = -24.95D0
      RNC = 2.63D2
      RND = 6.8D2
      TR2 = TR*TR
      TR3 = TR2*TR
      CPHINA2SO41 = Q(1) + Q(2)*(TR - TR2/T1) + Q(3)*(T2 + 2.D0*TR3/T1
     > - 3.D0*TR2) + Q(4)*(T1 + TR2/T1 - 2.D0*TR) + Q(5)*(LOG(T1/TR) +
     > TR/T1 - 1.D0)
      CPHINA2SO42 = Q(6)*(1.D0/(T1 - RNC) + (RNC*T1 - TR2)/(T1*(TR -
     > RNC)*(TR - RNC))) + Q(7)*(1.D0/(RND - T1) + (TR2 - RND*T1)/
     > (T1*(RND - TR)*(RND - TR)))
      CPHI = CPHINA2SO41 + CPHINA2SO42 
C      write(33,*)'cphi   ',cphi
C
      ELSEIF (MGT .EQ. 4) THEN
C
      XG = 0.010463D0/(2.D0**2.5D0/3.D0)
      XL = -1.725D-4*2.D0**1.5D0
      XJ = 70.981D-7*2.D0**1.5D0
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGT .EQ. 5) THEN
C
      XG = 0.010463D0/(2.D0**2.5D0/3.D0)
      XL = -1.725D-4*2.D0**1.5D0
      XJ = 1.02222D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGT .EQ. 6) THEN
C
      CPHI = 5.5167126D0 + T1*(-0.074935096D0 + T1*(0.00042366491D0 +
     > T1*(-1.2710689D-06 + T1*(2.1282773D-09 + T1*(-1.8834711D-12 +
     > T1*6.8830253D-16)))))
C
      ENDIF     
C      
      IF (MVT .EQ. 1) THEN
C      
C     Monnin 1990
C
      PR = PSAT(TSAVE)
      BETA0VNA2SO4 = 5.3088D-5 + 4.33707D-6*T1 - 3.262413D-9*T2 - 
     >2.70953D-4*LOG(T1) + 1.42266D-1/T1 + 4.51986D-3/TA
      CPHINA2SO4 = CPHI + (2.D0**1.5D0)*
     >(P1 - PR)*(1.752D-6 - 8.55D-2*BETA0VNA2SO4)
C
      ELSEIF (MVT .EQ. 4) THEN
C      
C     Monnin 1990, Pr = 200 bar
C
      PR = 2.D2
      BETA0VNA2SO4 = 5.3088D-5 + 4.33707D-6*T1 - 3.262413D-9*T2 - 
     >2.70953D-4*LOG(T1) + 1.42266D-1/T1 + 4.51986D-3/TA
      CPHINA2SO4 = CPHI + (2.D0**1.5D0)*
     >(P1 - PR)*(1.752D-6 - 8.55D-2*BETA0VNA2SO4)
C
      ELSEIF (MVT .EQ. 2) THEN
C
      P1 = PXX
      PR = 2.D2
      TB = ABS(6.8D2 - T1)
      CPHINACLP = 2.2902837D-5*(P1-PR) - 9.0550901D-8*(P1-PR)*T1 
     >+ 8.6926600D-11*(P1-PR)*T2 - 4.3314252D-4*(P1-PR)/TA 
     >+ 5.1904777D-4*(P1-PR)/TB
      CPHINA2SO4 = CPHI + 2.D0*CPHINACLP*(2.D0**0.5D0)
C
      ELSEIF (MVT .EQ. 3) THEN
C     
C     Krumgalz et al. (2000)
C     
      TX = T1 - TR
      PR = 1.D0
      CPHIV = 2.D0**1.5D0*(-2.9140D-6 + TX*(10.798D-8 + TX*(-9.5282D-9 
     > + 0.7375D-10*TX)))
      CPHINA2SO4 = CPHI + (P1 - PR)*CPHIV
C
      ENDIF
C
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION G0NAHCO3(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)            
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAHCO3/ V0NAHCO3, MGN, MVN, MDLGN, MDLVN
      CHARACTER*65 MDLGN(10), MDLVN(10)
      DIMENSION CPXX(14), VX(14), VP(14)
      P1 = PXX
      T1 = TXX
C
C     CODATA 
C
      HXX = -9.3027D5
      SXX = 1.5685D2 

      IF(MGN .EQ. 3 .OR. MGN .EQ. 2) THEN
      
C     HKF, 25 C adj., NaHCO3CH.dat     
      
      CPXX(1) = -72723.757D0
      CPXX(2) = 957189.14D0
      CPXX(3) = 14993.317D0
      CPXX(4) = -63.469963D0
      CPXX(5) = 0.043477417D0
      CPXX(6) = -10780.078D0
      CPXX(7) = 227.D0
      CPXX(8) = -248172.17D0
      CPXX(9) = 647.D0
      DO I = 10,14
      CPXX(I) = 0.D0
      ENDDO

      ELSE

      CPXX(1) = -1.122D1
      DO I = 2,14
      CPXX(I) = 0.D0
      ENDDO

      ENDIF

      IF (MVN .EQ. 1) THEN
C
C     Krumgalz et al. (2000)
C
      P0 = 1.D0
      TA = T1 - 2.9815D2
      VXXX = 1.D-1*(P1 - P0)*(23.181D0 + TA*(0.11309D0 + TA*(-5.5413D-3
     >+ TA*2.0591D-5)))/(8.31451D0*T1)
C      
      ELSEIF (MVN .EQ. 2 .OR. MVN .EQ. 3) THEN
C
C     Hershey et al. (1983) and Sharygin and Wood (1998) at 100, 175 and 250 C
C
      VX(1) = 80160.923D0
      VX(2) = -1690770.4D0
      VX(3) = -15081.299D0
      VX(4) = 45.166814D0
      VX(5) = -0.022956297D0
      VX(6) = -444.2219D0
      VX(7) = 2.27D2
      VX(8) = 14198.148D0
      VX(9) = 6.47D2
C     0-250 C, 1 bar, Hershey et al.(1983) 0-50 C, Sharygin and Wood (1998),100,175 and 250 C    
      VP(1) = 0.18373421D0
      VP(2) = 0.D0
      VP(3) = 0.D0
      VP(4) = -0.0020018847D0
      VP(5) = -3.4555831D-7
      VP(6) = 0.D0
      VP(7) = 0.D0
      VP(8) = 179.31887D0
      VP(9) = 6.47D2
      DO I = 10,14
      VX(I) = 0.D0
      VP(I) = 0.D0
      ENDDO
      PR = 1.D0
      P0 = 1.D0
      VXXX = 1.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
C
      ELSEIF (MVN .EQ. 4) THEN
C
C     Hershey et al. (1983) and HKF (supcrt92) from 50 to 300 C
C     NaHCO3VH.dat
C
      VX(1) = -4424.3146D0
      VX(2) = 95435.441D0
      VX(3) = 850.56391D0
      VX(4) = -2.7057536D0
      VX(5) = 0.0016714919D0
      VX(6) = -965.4543D0
      VX(7) = 2.27D2
      VX(8) = -16430.588D0
      VX(9) = 6.47D2
C     0-250 C, 1 bar, Hershey et al.(1983) 0-50 C, Sharygin and Wood (1998),100,175 and 250 C    
      VP(1) = 0.18373421D0
      VP(2) = 0.D0
      VP(3) = 0.D0
      VP(4) = -0.0020018847D0
      VP(5) = -3.4555831D-7
      VP(6) = 0.D0
      VP(7) = 0.D0
      VP(8) = 179.31887D0
      VP(9) = 6.47D2
      DO I = 10,14
      VX(I) = 0.D0
      VP(I) = 0.D0
      ENDDO
      PR = Psat(T1)
      P0 = 1.D0
      VXXX = 1.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
C
      ENDIF
     
      G0NAHCO3 = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX 
     
      V0NAHCO3 = V0XP(P1, T1, VX, VP, P0)

      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA0NAHCO3(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAHCO3/ V0NAHCO3, MGN, MVN, MDLGN, MDLVN
      CHARACTER*65 MDLGN(10), MDLVN(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 2.9815D2
      TA = T1 - TR
C
      IF (MGN .EQ. 1) THEN
C
      XG = 0.028D0
      XL = 1.D-3
      XJ = -1.929D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGN .EQ. 2) THEN
C     
      TB = T1 - 2.1D2
      TC = 6.47D2 - T1
      TD = T1 - 4.43D2
      U0 = 0.661D-1
      U1 = 0.D0
      U2 = 0.D0
      U3 = 0.D0
      U4 = 0.D0
      U5 = 3.75951D-8
      BETA0 = U0 + U1*T1 + U2/T1 + U3/TB + U4/TC + U5*TD*TD*TD/3.D0    
C
      ELSEIF (MGN .EQ. 3) THEN
C
      XG = 0.028D0
      XL = 1.D-3
      XJ = -1.929D-5
      X25 = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      XCP(1) = X25*(1.D0-28.D0/35.5D0)
      XCP(6) = X25*28.D0
      XCP(7) = 263.D0
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ENDIF
C
      IF (MVN .EQ. 1) THEN
C     
      BETA0V = -1.162D-5 + TA*(-28.646D-7 + 512.84D-9*TA)
C
      ELSEIF (MVN .EQ. 2) THEN
C     
      U6 = 0.D0
      U7 = 0.D0
      BETA0V = U6 + U7*(P1 - P0)/2.D0
C
      ELSEIF (MVN .EQ. 3 .OR. MVN .EQ. 4) THEN
C     
      BETA0V = -1.162D-5*TR/T1
C
      ENDIF
C 
      BETA0NAHCO3 = BETA0 + (P1 - P0)*BETA0V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA1NAHCO3(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAHCO3/ V0NAHCO3, MGN, MVN, MDLGN, MDLVN
      CHARACTER*65 MDLGN(10), MDLVN(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 2.9815D2
      TA = T1 - TR
C
      IF (MGN .EQ. 1) THEN
C
      XG = 0.044D0
      XL = 1.1D-3
      XJ = -3.562D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGN .EQ. 2) THEN
C     
      TB = T1 - 2.1D2
      TC = 6.47D2 - T1
      TD = T1 - 4.43D2
      U0 = -41.16D-1
      U1 = 63.09D-4
      U2 = 924.D0
      U3 = -52.02D0
      U4 = -80.26D0
      U5 = 0.D0
      BETA1 = U0 + U1*T1 + U2/T1 + U3/TB + U4/TC + U5*TD*TD*TD/3.D0    
C
      ELSEIF (MGN .EQ. 3) THEN
C     
      XG = 0.044D0
      XL = 1.1D-3
      XJ = -3.562D-5
      X25 = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      XCP(1) = X25*(1.D0-14.D0/35.5D0)
      XCP(6) = X25*14.D0
      XCP(7) = 263.D0
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR)
      ENDIF
C
      IF (MVN .EQ. 1) THEN
C     
      BETA1V = 17.8D-5 + TA*(4.4518D-6 + TA*13.897D-7)
C
      ELSEIF (MVN .EQ. 2) THEN
C     
      U6 = 16.34D-5
      U7 = -13.9D-8
      BETA1V = U6 + U7*(P1 - P0)/2.D0
C
      ELSEIF (MVN .EQ. 3 .OR. MVN .EQ. 4) THEN
C     
      BETA1V = 17.8D-5*TR/T1
C
      ENDIF
C
      BETA1NAHCO3 = BETA1 + (P1 - P0)*BETA1V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CPHINAHCO3(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAHCO3/ V0NAHCO3, MGN, MVN, MDLGN, MDLVN
      CHARACTER*65 MDLGN(10), MDLVN(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 2.9815D2
      TA = T1 - TR
C
      IF (MGN .EQ. 1 .OR. MGN .EQ. 3) THEN
C
      XG = 0.D0
      XL = 0.D0
      XCP(1) = 0.D0
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGN .EQ. 2) THEN
C     
      TB = T1 - 2.1D2
      TC = 6.47D2 - T1
      TD = T1 - 4.43D2
      U0 = 0.D0
      U1 = 0.D0
      U2 = 0.D0
      U3 = 0.D0
      U4 = 0.D0
      U5 = 0.D0
      CPHI = U0 + U1*T1 + U2/T1 + U3/TB + U4/TC + U5*TD*TD*TD/3.D0    
C
      ENDIF
C
      IF (MVN .EQ. 1) THEN
C     
      CPHIV = 2.D0*(13.741D-6 + TA*(73.175D-8 + TA*149.71D-9))
C
      ELSEIF (MVN .EQ. 2) THEN
C     
      U6 = 0.D0
      U7 = 0.D0
      CPHIV = U6 + U7*(P1 - P0)/2.D0
C
      ELSEIF (MVN .EQ. 3 .OR. MVN .EQ. 4) THEN
C     
      CPHIV = 2.D0*13.741D-6*TR/T1
C
      ENDIF
C
      CPHINAHCO3 = CPHI + (P1 - P0)*CPHIV
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION G0NA2CO3(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)            
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NA2CO3/ V0NA2CO3, MGS, MVS, MDLGS, MDLVS
      CHARACTER*65 MDLGS(10), MDLVS(10)
      DIMENSION CPXX(14), VX(14), VP(14)
      P1 = PXX
      T1 = TXX
C
C     CODATA 
C
      HXX = -1.15591D6
      SXX = 6.69D1 
C
      IF (MGS.EQ.1.OR.MGS.EQ.4) THEN
C
      CPXX(1) = -2.2848D2
      DO I = 2,14
      CPXX(I) = 0.D0
      ENDDO
C
c      ELSEIF () THEN
C
C     T dependence as Na2SO4 (Holmes and Mesmer, 1986), CPXX(1) adj. acc. to model 1
C
c      CPXX(1) = -1235.173D0 
c      CPXX(2) = 0.D0   
c      CPXX(3) = 0.D0  
c      CPXX(4) = 7.6405D0 
c      CPXX(5) = -1.23672D-2 
c      CPXX(6) = -6045.D0 
c      CPXX(7) = 2.63D2       
c      CPXX(8) = 0.D0 
c      CPXX(9) = 0.D0       
c      CPXX(10) = 0.D0        
c      CPXX(11) = 0.D0        
c      CPXX(12) = 0.D0        
c      CPXX(13) = 0.D0        
c      CPXX(14) = 0.D0        
C
      ELSEIF (MGS .EQ. 5) THEN
C
      CPXX(1) = -185.96D0
      DO I = 2,14
      CPXX(I) = 0.D0
      ENDDO
C
      ELSEIF (MGS.EQ.6.OR.MGS.EQ.3.OR.MGS.EQ.2) THEN
C
C     HKF, 25 C adj.  Na2CO3CH.dat
C
      CPXX(1) = 1001370.2D0 
      CPXX(2) = -23675475.D0   
      CPXX(3) = -182880.7D0  
      CPXX(4) = 462.52238D0 
      CPXX(5) = -0.18822832D0
      CPXX(6) = -28810.509D0 
      CPXX(7) = 2.27D2       
      CPXX(8) = -339594.61D0 
      CPXX(9) = 647.D0       
      CPXX(10) = 0.D0        
      CPXX(11) = 0.D0        
      CPXX(12) = 0.D0        
      CPXX(13) = 0.D0        
      CPXX(14) = 0.D0        
C   mod. HKF  ???
c      CPXX(1) = 1172450.7D0 
c      CPXX(2) = -32224588.D0   
c      CPXX(3) = -207452.95D0  
c      CPXX(4) = 439.79401D0 
c      CPXX(5) = -0.14764466D0
c      CPXX(6) = 34248.985D0 
c      CPXX(7) = 2.27D2       
c      CPXX(8) = -383306.D0 
c      CPXX(9) = 647.D0       
c      CPXX(10) = 0.D0        
c      CPXX(11) = 0.D0        
c      CPXX(12) = 0.D0        
c      CPXX(13) = 0.D0        
c      CPXX(14) = 0.D0        

C   mod. HKF, Na2CO3CX.dat

c      CPXX(1) = 1198241.3D0 
c      CPXX(2) = -34020561.D0   
c      CPXX(3) = -210380.67D0  
c      CPXX(4) = 424.27723D0 
c      CPXX(5) = -0.13248063D0
c      CPXX(6) = 50681.325D0 
c      CPXX(7) = 2.27D2       
c      CPXX(8) = -398015.61D0 
c      CPXX(9) = 647.D0       
c      CPXX(10) = 0.D0        
c      CPXX(11) = 0.D0        
c      CPXX(12) = 0.D0        
c      CPXX(13) = 0.D0        
c      CPXX(14) = 0.D0        

      ENDIF      
C      
      IF (MVS .EQ. 1) THEN
C
C     Krumgalz et al. (2000)
C
      P0 = 1.D0
      TA = T1 - 2.9815D2
      V0 = (-6.48D0 + TA*(0.25109D0 + TA*(-6.8782D-3 + TA*5.4186D-5))) 
      VXXX = 1.D-1*(P1 - P0)*V0/(8.31451D0*T1)
C
      ELSEIF (MVS .EQ. 2) THEN
C
C     Krumgalz et al. (2000) V0 data (Table 3) fitted to Holmes & Mesmer eqn.
C
      P0 = 1.D0
      TA = T1 - 2.63D2
      V0 = -228.5329D0 + 1.4600145D0*T1 - 0.0023306034D0*T1*T1
     > - 211.56993D0/TA
      VXXX = 1.D-1*(P1 - P0)*V0/(8.31451D0*T1)
C
      ELSEIF (MVS .EQ. 4) THEN
C
C     Krumgalz et al. (2000) and Sharygin and Wood (1998) at 100, 175 and 250 C
C
c      VX(1) = 11963.974D0
c      VX(2) = -523116.63D0
c      VX(3) = -1712.244D0
c      VX(4) = -2.5826213D0
c      VX(5) = 0.0054551378D0
c      VX(6) = 365.98155D0
c      VX(7) = 2.27D2
c      VX(8) = -62872.167D0
c      VX(9) = 6.47D2

c      Na2CO3V0.dat

      VX(1) = 143427.38D0
      VX(2) = -3163761.4D0
      VX(3) = -26742.774D0
      VX(4) = 76.629908D0
      VX(5) = -0.037337186D0
      VX(6) = 440.19027D0
      VX(7) = 2.27D2
      VX(8) = 4424.7264D0
      VX(9) = 6.47D2

C     0-250 C, 1 bar, Hershey et al.(1983) 0-50 C, Sharygin and Wood (1998),100,175 and 250 C    
C     Na2CO3K0.dat
    
c      VP(1) = 0.72831393D0
c      VP(2) = 0.D0
c      VP(3) = 0.D0
c      VP(4) = -0.0068167548D0
c      VP(5) = 2.3873105D-6
c      VP(6) = 0.D0
c      VP(7) = 0.D0
c      VP(8) = 434.00245D0
c      VP(9) = 6.47D2
c      DO I = 10,14
c      VX(I) = 0.D0
c      VP(I) = 0.D0
c      ENDDO

c      Na2CO3V1.dat

c      VX(1) = 45561.466D0
c      VX(2) = -1039239.7D0
c      VX(3) = -8475.782D0
c      VX(4) = 24.806797D0
c      VX(5) = -0.013048575D0
c      VX(6) = -43.85806D0
c      VX(7) = 2.27D2
c      VX(8) = -9164.8915D0
c      VX(9) = 6.47D2

C     0-250 C, 1 bar, Hershey et al.(1983) 0-50 C, Sharygin and Wood (1998),100,175 and 250 C
C     Na2CO3K1.dat
    
      VP(1) = -0.26079715D0
      VP(2) = 0.D0
      VP(3) = 0.D0
      VP(4) = -0.0020556136D0
      VP(5) = -4.5538706D-6
      VP(6) = 0.D0
      VP(7) = 0.D0
      VP(8) = 483.32731D0
      VP(9) = 6.47D2
      DO I = 10,14
      VX(I) = 0.D0
      VP(I) = 0.D0
      ENDDO
C
      PR = 1.D0
      P0 = 1.D0
      VXXX = 1.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
C
      ELSEIF (MVS .EQ. 5 .OR. MVS .EQ. 3) THEN

c     HFK > 200 C, Na2CO3V0.dat
      VX(1) = 42108.546D0
      VX(2) = -896109.53D0
      VX(3) = -7911.9534D0
      VX(4) = 23.852278D0
      VX(5) = -0.012127799D0
      VX(6) = -939.68778D0
      VX(7) = 2.27D2
      VX(8) = -17672.703D0
      VX(9) = 6.47D2

C     0-250 C, 1 bar, Hershey et al.(1983) 0-50 C, Sharygin and Wood (1998),100,175 and 250 C    
C     Na2CO3K2.dat
      VP(1) = 0.34237337D0
      VP(2) = 0.D0
      VP(3) = 0.D0
      VP(4) = -0.0049277108D0
      VP(5) = 5.4207019D-7
      VP(6) = 0.D0
      VP(7) = 0.D0
      VP(8) = 421.98191D0
      VP(9) = 6.47D2
      DO I = 10,14
      VX(I) = 0.D0
      VP(I) = 0.D0
      ENDDO
C
      PR = PSAT(T1)
      P0 = 1.D0
      VXXX = 1.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1)
C
      ENDIF
C
      G0NA2CO3 = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX 
      
      V0NA2CO3 = V0XP(P1, T1, VX, VP, P0)

      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA0NA2CO3(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NA2CO3/ V0NA2CO3, MGS, MVS, MDLGS, MDLVS
      CHARACTER*65 MDLGS(10), MDLVS(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 2.9815D2
      TA = T1 - TR
C
      IF (MGS .EQ. 1) THEN
C
      XG = 0.0362D0
      XL = 2.126891305D-3
      XJ = -3.019262D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGS .EQ. 2) THEN
C     
      TB = T1 - 2.1D2
      TC = 6.47D2 - T1
      TD = T1 - 4.43D2
      U0 = -15.837178D0
      U1 = -5.991D-4
      U2 = 771.329688D0
      U3 = -25.81D0
      U4 = -2.659D0
      U5 = 0.D0
      U8 = 2.415995D0
      BETA0 = U0 + U1*T1 + U2/T1 + U3/TB + U4/TC + U5*TD*TD*TD/3.D0 
     > + U8*LOG(T1)   
C
      ELSEIF (MGS .EQ. 3) THEN
C     
      TB = T1 - 2.1D2
      TC = 6.47D2 - T1
      TD = T1 - 4.43D2
      U0 = 5.153D-1
      U1 = -5.991D-4
      U2 = 0.D0
      U3 = -25.81D0
      U4 = -2.659D0
      U5 = 0.D0
      BETA0 = U0 + U1*T1 + U2/T1 + U3/TB + U4/TC + U5*TD*TD*TD/3.D0    
C
      ELSEIF (MGS .EQ. 4) THEN
C
      XG = 0.0362D0
      XL = 2.126891305D-3
      XJ = -3.019262D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGS .EQ. 5) THEN
C
      XG = 0.0362D0
      XL = 1.79D-3
      XJ = -3.1096D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGS .EQ. 6) THEN
C
      XG = 0.0362D0
      XL = 2.126891305D-3
      XJ = -3.019262D-5
      X25 = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      XCP(1) = X25*(1.D0-28.D0/35.5D0)
      XCP(6) = X25*28.D0
      XCP(7) = 263.D0
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ENDIF
C 
      IF (MVS .EQ. 1 .OR. MVS .EQ. 2) THEN
C     
      BETA0V = 5.98D-5 + TA*(-6.9018D-7 + TA*2.112D-9)
C
      ELSEIF (MVS .EQ. 3) THEN
C     
      U6 = 8.75D-5
      U7 = -2.66D-8
      BETA0V = U6 + U7*(P1 - P0)/2.D0
C
      ELSEIF (MVS .EQ. 4 .OR. MVS .EQ. 5) THEN
C
c      IF (TSAVE .LT. 333.15D0) THEN
c      BETA0V = 5.98D-5 + TA*(-6.9018D-7 + TA*2.112D-9)
c      ELSE
c      BETA0V = 4.D-5
c      ENDIF
C
      BETA0V = 5.98D-5 + TA*(-6.9018D-7 + TA*2.112D-9)

      ENDIF
C 
C      IF (MVS .EQ. 3) P0 = 7.D1
      BETA0NA2CO3 = BETA0 + (P1 - P0)*BETA0V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA1NA2CO3(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NA2CO3/ V0NA2CO3, MGS, MVS, MDLGS, MDLVS
      CHARACTER*65 MDLGS(10), MDLVS(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 2.9815D2
      TA = T1 - TR
C
      IF (MGS .EQ. 1) THEN
C
      XG = 1.51D0
      XL = 5.0592226326D-3
      XJ = -15.42485D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGS .EQ. 2) THEN
C     
      TB = T1 - 2.1D2
      TC = 6.47D2 - T1
      TD = T1 - 4.43D2
      U0 = 77.074917D0
      U1 = -43.03D-4
      U2 = -3575.534705D0
      U3 = -25.45D0
      U4 = 361.8D0
      U5 = 0.D0
      U8 = -11.063961D0
      BETA1 = U0 + U1*T1 + U2/T1 + U3/TB + U4/TC + U5*TD*TD*TD/3.D0    
     > + U8*LOG(T1)
C
      ELSEIF (MGS .EQ. 3) THEN
C     
      TB = T1 - 2.1D2
      TC = 6.47D2 - T1
      TD = T1 - 4.43D2
      U0 = 20.44D-1
      U1 = -43.03D-4
      U2 = 0.D0
      U3 = -25.45D0
      U4 = 361.8D0
      U5 = 0.D0
      BETA1 = U0 + U1*T1 + U2/T1 + U3/TB + U4/TC + U5*TD*TD*TD/3.D0    
C
      ELSEIF (MGS .EQ. 4) THEN
C
      XG = 1.51D0
      XL = 5.0592226326D-3
      XJ = -15.42485D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGS .EQ. 5) THEN
C
      XG = 1.51D0
      XL = 2.05D-3
      XJ = -6.7714D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGS .EQ. 6) THEN
C
      XG = 1.51D0
      XL = 5.0592226326D-3
      XJ = -15.42485D-5
      X25 = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      XCP(1) = X25*(1.D0-14.D0/35.5D0)
      XCP(6) = X25*14.D0
      XCP(7) = 263.D0
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ENDIF
C 
      IF (MVS .EQ. 1 .OR. MVS .EQ. 2) THEN
C     
      BETA1V = 8.16D-5 + TA*(-7.4232D-6 + TA*1.5774D-7)
C
      ELSEIF (MVS .EQ. 3) THEN
C     
      BETA1V = 0.D0
C
      ELSEIF (MVS .EQ. 4 .OR. MVS .EQ. 5) THEN
C      
C      BETA1V = 248.36203D-5 + T1*(-1.1817211D-5 + 0.0012787083D-5*T1)
C
c      IF (TSAVE .LT. 333.15D0) THEN
c      BETA1V = 8.16D-5 + TA*(-7.4232D-6 + TA*1.5774D-7)
c      ELSE
c      BETA1V = 0.D0
c      ENDIF
C      
      TC1 = T1 - 273.15D0
      BETA1V = -18.687529D-5 + 1390.5256D-5/(TC1+25.D0)
c      BETA1V=8.16D-5*(1.D0-87.516246D0/35.15D0+87.516246D0
c     > /(TC1+10.15D0))
      ENDIF
C 
      BETA1NA2CO3 = BETA1 + (P1 - P0)*BETA1V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CPHINA2CO3(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NA2CO3/ V0NA2CO3, MGS, MVS, MDLGS, MDLVS
      CHARACTER*65 MDLGS(10), MDLVS(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 2.9815D2
      TA = T1 - TR
C
      IF (MGS .EQ. 1) THEN
C
      XG = 0.0052D0
      XL = -0.37649173885D-3
      XJ = 0.46D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGS .EQ. 2) THEN
C     
      TB = T1 - 2.1D2
      TC = 6.47D2 - T1
      TD = T1 - 4.43D2
c      U0 = 5.7132704D0
c      U0 = 6.2703972D0
      U0 = 6.3681800D0
      U1 = 0.D0
c      U2 = -288.011245D0
      U2 = -317.165187D0
      U3 = 6.482D0
      U4 = 8.048D0
      U5 = 0.D0
c      U8 = -0.8492493D0
      U8 = -0.9470321D0
      CPHI = U0 + U1*T1 + U2/T1 + U3/TB + U4/TC + U5*TD*TD*TD/3.D0    
     > + U8*LOG(T1)
C
      ELSEIF (MGS .EQ. 3) THEN
C     
      TB = T1 - 2.1D2
      TC = 6.47D2 - T1
      TD = T1 - 4.43D2
      U0 = -0.914D-1
      U1 = 0.D0
      U2 = 0.D0
      U3 = 6.482D0
      U4 = 8.048D0
      U5 = 0.D0
      CPHI = U0 + U1*T1 + U2/T1 + U3/TB + U4/TC + U5*TD*TD*TD/3.D0    
C
      ELSEIF (MGS .EQ. 4) THEN
C
      XG = 0.0052D0
      XL = -0.37649173885D-3
      XJ = 0.35D-5
      XCP(1) = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGS .EQ. 5) THEN
C
      XG = 0.0052D0
      XL = 0.D0
      XCP(1) = 0.D0
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGS .EQ. 6) THEN
C
      XG = 0.0052D0
      XL = -0.37649173885D-3
      XJ = 0.35D-5
      X25 = -TR*TR*XJ
      DO I = 2,17
      XCP(I) = 0.D0
      ENDDO
      XCP(1) = X25*(1.D0-28.D0/35.5D0)
      XCP(6) = X25*28.D0
      XCP(7) = 263.D0
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ENDIF
C 
      IF (MVS .EQ. 1 .OR. MVS .EQ. 2) THEN
C     
      CPHIV = 2.D0**1.5D0*(-3.25D-6 + TA*(1.6502D-8 +TA*0.6157D-9))
C
      ELSEIF (MVS .EQ. 3) THEN
C     
      U6 = -2.89D-5
      U7 = 0.D0
      CPHIV = U6 + U7*(P1 - P0)/2.D0
C
      ELSEIF (MVS .EQ. 4 .OR. MVS .EQ. 5) THEN
C
C     CPHIV = -(2.D0**1.5D0)*3.25D-6 
C
C     CPHIV = -(2.D0**1.5D0)*3.25D-6*(TR/T1)**2.D0
C 
C     CPHIV = 2.D0**1.5D0*(-15.110451D-6 + T1*0.039111615D-6)
C
C     CPHIV = 2.D0**1.5D0*(-3.45D-6 + (T1-TR)*0.039111615D-6)

      CPHIV = 2.D0**1.5D0*(-3.D-6 + (T1-TR)*0.04D-6)

      ENDIF
C 
C      IF (MVS .EQ. 3) P0 = 7.D1
      CPHINA2CO3 = CPHI + (P1 - P0)*CPHIV
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION G0NAALOH4(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)            
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAALOH4/ V0NAALOH4, MGA, MVA, MDLGA, MDLVA 
      CHARACTER*65 MDLGA(10), MDLVA(10)
      DIMENSION CPXX(14),VX(14),VP(14),VX0(14),VP0(14)
      P1 = PXX
      T1 = TXX
C
      IF (MGA .EQ. 1 .OR. MGA .EQ. 3 .OR. MGA .EQ. 4) THEN
C
C     Wesolowski (1992) 
C
      HXX = -1.740985D6
      SXX = 1.6969D2 
C
C     Zeng et. al. (1994) ??, SXX est.
C      
C      HXX = -1.74395D6
C      SXX = 1.61D2 
C
C     Chen & Zeng (1994)
C      
C      HXX = -1.74399D6
C      SXX = 1.5796D2 
C
C     H of Chen et. al. (1991), S est.
C      
C      HXX = -1.74311D6
C      SXX = 1.6356D2 
C
      ELSEIF (MGA .EQ. 2) THEN
C
C     Chen et. al. (1991)
C      
      HXX = -1.74311D6
      SXX = 1.5994D2 
C
      ENDIF
C      
C     Chen et al. (1991) 
C
C     DO I = 2,14
C     CPXX(I) = 0.D0
C     ENDDO
C     CPXX(1) = -1.451041D3
C     CPXX(4) = 8.993519D0
C     CPXX(5) = -1.235584D-2
C     CPXX(6) = -7.758518D0
C     CPXX(7) = 4.7D2
C
C     CurveExpert fit to Hovey et al. (1988) and Caiani et al. (1989)
C
C     CPXX(1) = 636440.96D0
C     CPXX(2) = -13877718.D0
C     CPXX(3) = -118389.62D0
C     CPXX(4) = 331.35745D0
C     CPXX(5) = -0.15361365D0
C     CPXX(6) = -20677.96D0
C     CPXX(7) = 2.27D2
C     CPXX(8) = -23053.602D0
C     CPXX(9) = 6.47D2
C     CPXX(10) = 0.D0
C     CPXX(11) = 0.D0
C     CPXX(12) = 0.D0
C     CPXX(13) = 0.D0
C     CPXX(14) = 0.D0
C
      IF (MGA .EQ. 1 .OR. MGA .EQ. 2 .OR. MGA .EQ. 3) THEN
C
C     CurveExpert fit to Hovey et al. (1988) and adj. values of Caiani et al. (1989)
C
      CPXX(1) = 65671.169D0
      CPXX(2) = -550158.92D0
      CPXX(3) = -13464.009D0
      CPXX(4) = 55.509011D0
      CPXX(5) = -0.03292435D0
      CPXX(6) = -31384.1D0
      CPXX(7) = 2.27D2
      CPXX(8) = -54151.1512D0
      CPXX(9) = 6.47D2
      CPXX(10) = 0.D0
      CPXX(11) = 0.D0
      CPXX(12) = 0.D0
      CPXX(13) = 0.D0
      CPXX(14) = 0.D0
C
      P0 = 1.D0
C
      ELSEIF (MGA .EQ. 4) THEN

      IF(TSAVE .LE. 350.36D0) THEN
C
C     CurveExpert fit to Hovey et al. (1988) and adj. values of Caiani et al. (1989)
C
      CPXX(1) = 65671.169D0 + 29.401923D0
      CPXX(2) = -550158.92D0
      CPXX(3) = -13464.009D0
      CPXX(4) = 55.509011D0 - 0.115385D0
      CPXX(5) = -0.03292435D0
      CPXX(6) = -31384.1D0
      CPXX(7) = 2.27D2
      CPXX(8) = -54151.1512D0
      CPXX(9) = 6.47D2
      CPXX(10) = 0.D0
      CPXX(11) = 0.D0
      CPXX(12) = 0.D0
      CPXX(13) = 0.D0
      CPXX(14) = 0.D0
C
      P0 = 1.D0

      ELSE
C
C     p = 10 MPa, New model
C
      CPXX(1) = -1563253.3D0 + 266.93553D0 
      CPXX(2) = 36566523.D0
      CPXX(3) = 288163.14D0
      CPXX(4) = -775.23835D0 - 0.13497D0
      CPXX(5) = 0.35895126D0
      CPXX(6) = -61733.97D0
      CPXX(7) = 227.D0      
      CPXX(8) = -426848.18D0  
      CPXX(9) = 647.D0      
      CPXX(10) = 0.D0
      CPXX(11) = 0.D0
      CPXX(12) = 0.D0
      CPXX(13) = 0.D0
      CPXX(14) = 0.D0
C
      P0 = 1.D2
C
      ENDIF
      ENDIF
C
C     Hovey et al. (1988) + 200 C value estimated from Pokrovskii & Helgeson (1995)
C      
      IF (MVA .EQ. 1 .OR. MVA .EQ. 2) THEN

      V0 = -7.542734D0 + T1*(0.35765143D0 - 0.00056047206D0*T1) -
     > 148.47227D0/(T1 - 263.D0)

      VXXX = 1.D-1*(P1 - P0)*V0/(8.31451D0*T1)
      
      VXXC = 0.D0

      V0NAALOH4 = V0

      ELSEIF (MVA .EQ. 3) THEN
C
C     89SiR (0 - 50 C) and 06CoS (100 - 300 C)
C
      VP(1) = 1677.2839D0
      VP(2) = -37008.039D0
      VP(3) = -312.872D0
      VP(4) = 0.89310395D0
      VP(5) = -0.00043830501D0
      VP(6) = 24.506482D0
      VP(7) = 2.27D2
      VP(8) = 655.46529D0
      VP(9) = 6.47D2
      VP(10) = 0.D0
      VP(11) = 0.D0
      VP(12) = 0.D0
      VP(13) = 0.D0
      VP(14) = 0.D0

      IF(TSAVE .LE. 350.36D0) THEN
C
C     Hovey et al. (1988) + 200 C value estimated from Pokrovskii & Helgeson (1995)
C      
      VX(1) = -7.542734D0
      VX(2) = 0.D0
      VX(3) = 0.D0
      VX(4) = 0.35765143D0
      VX(5) = -0.00056047206D0
      VX(6) = -148.47227D0
      VX(7) = 2.63D2
      VX(8) = 0.D0
      VX(9) = 0.D0
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0

      P0 = 1.D0
      PR = 1.D0
      VXXC = 0.D0
C      
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1) 

      ELSE
C
C     NaOH incl 275 C data + lin f(T)
C
      VX(1) = -3512.8854D0 + 37.68D0
      VX(2) = 0.D0
      VX(3) = 793.94174D0 
      VX(4) = -4.066471D0 + 0.028D0
      VX(5) = 0.0028271164D0
      VX(6) = 0.D0
      VX(7) = 0.D0
      VX(8) = -19135.677D0
      VX(9) = 6.47D2
      VX(10) = 0.D0
      VX(11) = 0.D0
      VX(12) = 0.D0
      VX(13) = 0.D0
      VX(14) = 0.D0
C
C     V^0 at 298.15 K (89SiR)
C
      VX0(1) = -47.2184D0
      VX0(2) = 0.D0
      VX0(3) = 0.D0
      VX0(4) = 0.314497D0
      VX0(5) = -5.16093D-4
      VX0(6) = -445.889D0
      VX0(7) = 2.27D2
      VX0(8) = 0.D0
      VX0(9) = 0.D0
      VX0(10) = 0.D0
      VX0(11) = 0.D0
      VX0(12) = 0.D0
      VX0(13) = 0.D0
      VX0(14) = 0.D0
C
      VP0(1) = 1.38604D0
      VP0(2) = 0.D0
      VP0(3) = 0.D0
      VP0(4) = -8.16550D-3
      VP0(5) = 1.25354D-5
      VP0(6) = 0.939619D0
      VP0(7) = 2.27D2
      VP0(8) = 0.D0
      VP0(9) = 0.D0
      VP0(10) = 0.D0
      VP0(11) = 0.D0
      VP0(12) = 0.D0
      VP0(13) = 0.D0
      VP0(14) = 0.D0
C
      P0 = 1.D0
      TR = 2.9815D2
      PR = 100.D0
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX0,VP0,PR,P0)/(8.31451D0*TR)
C      
      P0 = 100.D0
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1) 

      ENDIF

      V0NAALOH4 = V0XP(PSAVE, TSAVE, VX, VP, P0) 

      ENDIF
C      
      G0NAALOH4 = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX + VXXC
      
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA0NAALOH4(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAALOH4/ V0NAALOH4, MGA, MVA, MDLGA, MDLVA 
      CHARACTER*65 MDLGA(10), MDLVA(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
C
      IF (MGA .EQ. 1 .OR. MGA .EQ. 4) THEN
      BETA0 = BETA0NAOH(P1,T1) - 0.0356D0
C
      ELSEIF (MGA .EQ. 2) THEN
      TR = 3.7215D2
      XG = BETA0NAOH(P0,TR) - 0.0356D0
      XL = 1.76315D-3
      DO I = 1,17
      XCP(I) = 0.D0
      ENDDO
      XCP(5) = -9.93607D-4
      XCP(15) = 4.81382D-6
      XCP(16) = -5.87747D-9
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGA .EQ. 3) THEN
      BETA0 = BETA0NAOH(P1,T1) - 0.0947D0
C
      ENDIF
C      
      IF (MVA .EQ. 1) THEN
      BETA0V = 0.D0

      ELSEIF (MVA .EQ. 2) THEN

C      BETA0V = 0.0684446D0/(8.31451D0*TSAVE)
c      BETA0V = (7.0722903D0 + T1*(-0.04460589D0 + 7.0964638D-05*T1))
c     >/(8.31451D0*TSAVE) - 1.D-1*BETA0NAOHVK(PXX,TXX)
c      print*,'0   ',beta0v,BETA0NAOHVK(PXX,TXX)

      BETA0V = (4.8684772D0 + T1*(-0.030302689D0 + 4.778889D-05*T1))
     >/(8.31451D0*T1) - 1.D-1*BETA0NAOHVK(PXX,TXX)

      ELSEIF (MVA .EQ. 3) THEN
      
      IF(TSAVE .LE. 350.36D0) THEN

      BETA0V = (4.8684772D0 + T1*(-0.030302689D0 + 4.778889D-05*T1))
     >/(8.31451D0*T1) - 1.D-1*BETA0NAOHVK(PXX,TXX)

      ELSE

      BETA0V = 0.D0
      
      ENDIF
      ENDIF
C
      BETA0NAALOH4 = BETA0 + (P1 - P0)*BETA0V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA1NAALOH4(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAALOH4/ V0NAALOH4, MGA, MVA, MDLGA, MDLVA 
      CHARACTER*65 MDLGA(10), MDLVA(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
C
      IF (MGA .EQ. 1 .OR. MGA .EQ. 4) THEN
      BETA1 = BETA1NAOH(P1,T1)
C
      ELSEIF (MGA .EQ. 2) THEN
      TR = 3.7215D2
      XG = BETA1NAOH(P0,TR) 
      XL = -5.22260D-3
      DO I = 1,17
      XCP(I) = 0.D0
      ENDDO
      XCP(5) = 1.21394D-2
      XCP(15) = -7.88087D-5
      XCP(16) = 1.67861D-7
      XCP(17) = -1.16436D-10
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGA .EQ. 3) THEN
      BETA1 = BETA1NAOH(P1,T1) + 0.0832D0
C
      ENDIF
C      
      IF (MVA .EQ. 1) THEN

      BETA1V = 0.D0

      ELSEIF (MVA .EQ. 2) THEN

C      BETA1V = -0.543729D0/(8.31451D0*TSAVE)
c      BETA1V = (-21.5798D0+T1*(0.14256985D0 - 0.00023974291D0*T1))
c     >/(8.31451D0*TSAVE) - 1.D-1*BETA1NAOHV(PXX,TXX)
c      print*,'1   ',beta1v,BETA1NAOHV(PXX,TXX)

      BETA1V = (-19.229914D0+T1*(0.12571011D0 - 0.00020973762D0*T1))
     >/(8.31451D0*T1) - 1.D-1*BETA1NAOHV(PXX,TXX)

      ELSEIF (MVA .EQ. 3) THEN

      IF(TSAVE .LE. 350.36D0) THEN

      BETA1V = (-19.229914D0+T1*(0.12571011D0 - 0.00020973762D0*T1))
     >/(8.31451D0*T1) - 1.D-1*BETA1NAOHV(PXX,TXX)

      ELSE

      BETA1V = 0.D0

      ENDIF
      ENDIF
C
      BETA1NAALOH4 = BETA1 + (P1 - P0)*BETA1V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA2NAALOH4(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAALOH4/ V0NAALOH4, MGA, MVA, MDLGA, MDLVA 
      CHARACTER*65 MDLGA(10), MDLVA(10)
      P1 = PXX
      T1 = TXX
C
      IF (MGA .EQ. 4) THEN
      BETA2NAALOH4 = BETA2NAOH(P1,T1)
      ELSE
      BETA2NAALOH4 = 0.D0
      ENDIF
C
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CPHINAALOH4(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      COMMON /NAALOH4/ V0NAALOH4, MGA, MVA, MDLGA, MDLVA 
      CHARACTER*65 MDLGA(10), MDLVA(10)
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
C
      IF (MGA .EQ. 1 .OR. MGA .EQ. 4) THEN
      CPHI = CPHINAOH(P1,T1) - 0.00526D0
C
      ELSEIF (MGA .EQ. 2) THEN
      TR = 3.7215D2
      XG = CPHINAOH(P0,TR) - 0.00526D0
      XL = -1.08641D-3
      DO I = 1,17
      XCP(I) = 0.D0
      ENDDO
      XCP(5) = 4.97929D-4
      XCP(15) = -2.41445D-6
      XCP(16) = 2.94923D-9
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      ELSEIF (MGA .EQ. 3) THEN
c      CPHI = CPHINAOH(P1,T1) + 0.00537D0
      CPHI = CPHINAOH(P1,T1) 
C
      ENDIF
C      
      IF (MVA .EQ. 1) THEN

      CPHIV = 0.D0

      ELSEIF (MVA .EQ. 2) THEN

C      CPHIV = -0.00701156D0/(8.31451D0*TSAVE)
c      CPHIV = (-1.0438714D0 + T1*(0.0066255095D0 - 1.057375D-05*T1))
c     >/(8.31451D0*TSAVE) - 1.D-1*CPHINAOHVK(PXX,TXX)
c      print*,'C   ',cphiv,CPHINAOHVK(PXX,TXX)

      CPHIV = (-0.68101262D0 + T1*(0.0042572924D0 - 6.7144494D-06*T1))
     >/(8.31451D0*T1) - 1.D-1*CPHINAOHVK(PXX,TXX)

      ELSEIF (MVA .EQ. 3) THEN

      IF(TSAVE .LE. 350.36D0) THEN

      CPHIV = (-0.68101262D0 + T1*(0.0042572924D0 - 6.7144494D-06*T1))
     >/(8.31451D0*T1) - 1.D-1*CPHINAOHVK(PXX,TXX)

      ELSE

      CPHIV = 0.D0

      ENDIF
      ENDIF
C
      CPHINAALOH4 = CPHI + (P1 - P0)*CPHIV
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION G0NAAC(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)            
      COMMON /NAAC/ V0NAAC
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION CPXX(14)
      P1 = PXX
      T1 = TXX
C
C     NBS (Technical Note 270-8)
C
      HXX = -726133.2D0
      SXX = 1.45603D2 
C      
C     99BFC (Woolley), 3.4 added to CPXX(1) to agree with 96CrM at 25 C
C
      CPXX(1) = 16821.3D0
      CPXX(2) = -833575.D0
      CPXX(3) = -2448.02D0
      CPXX(4) = 0.D0
      CPXX(5) = 0.D0
      CPXX(6) = -293.18D0
      CPXX(7) = 2.7D2
      CPXX(8) = 0.D0
      CPXX(9) = 0.D0
      CPXX(10) = 0.D0
      CPXX(11) = 0.D0
      CPXX(12) = 0.D0
      CPXX(13) = 0.D0
      CPXX(14) = 0.D0
C
      P0 = 1.D0
C
C     99BFC (Woolley) 
C      
      V0NAAC = 1101.52D0 - 0.458D0 - 155.671D0*LOG(T1) - 52155.8D0/T1
      VXXX = 1.D-1*(P1 - P0)*V0NAAC/(8.31451D0*T1)
C      
      G0NAAC = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX 
      
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA0NAAC(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 298.15D0
C
      XG = 359.92769D0/(TR*8.31451D0)
      XL = 32.360421D0/(TR*TR*8.31451D0)
      XJ = -1.39D-5
      DO I = 1,17
      XCP(I) = 0.D0
      ENDDO
      XCP(1) = -TR*TR*XJ
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      BETA0V = 0.283672362D-1/(TR*8.31451D0)
      BETA0NAAC = BETA0 + (P1 - P0)*BETA0V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA1NAAC(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 298.15D0
C
      XG = 724.87425D0/(TR*8.31451D0)
      XL = -346.51144D0/(TR*TR*8.31451D0)
      XJ = -6.88D-7
      DO I = 1,17
      XCP(I) = 0.D0
      ENDDO
      XCP(1) = -TR*TR*XJ
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      BETA1V = 0.484297763D-1/(TR*8.31451D0)
      BETA1NAAC = BETA1 + (P1 - P0)*BETA1V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CPHINAAC(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 298.15D0
C
      XG = -15.320618D0/(TR*8.31451D0)
      XL = -47.804676D0/(TR*TR*8.31451D0)
      XJ = 2.22D-6
      DO I = 1,17
      XCP(I) = 0.D0
      ENDDO
      XCP(1) = -TR*TR*XJ
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      CPHIV = -0.228797217D-2/(TR*8.31451D0)
      CPHINAAC = CPHI + (P1 - P0)*CPHIV
      RETURN
      END


      DOUBLE PRECISION FUNCTION G0NAFORM(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)            
      COMMON /NAFORM/ V0NAFORM
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION CPXX(14)
      P1 = PXX
      T1 = TXX
C
C     NBS (Technical Note 270-8)
C
      HXX = 0.D0
      SXX = 0.D0 
C      
C     NaAc - 80
C
      CPXX(1) = 16741.3D0
      CPXX(2) = -833575.D0
      CPXX(3) = -2448.02D0
      CPXX(4) = 0.D0
      CPXX(5) = 0.D0
      CPXX(6) = -293.18D0
      CPXX(7) = 2.7D2
      CPXX(8) = 0.D0
      CPXX(9) = 0.D0
      CPXX(10) = 0.D0
      CPXX(11) = 0.D0
      CPXX(12) = 0.D0
      CPXX(13) = 0.D0
      CPXX(14) = 0.D0
C
      P0 = 1.D0
C
C      
C      
      V0NAFORM = 1101.52D0 - 14.582D0 - 155.671D0*LOG(T1) - 52155.8D0/T1
      VXXX = 1.D-1*(P1 - P0)*V0NAFORM/(8.31451D0*T1)
C      
      G0NAFORM = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX 
      
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA0NAFORM(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 298.15D0
C
      XG = 180.981940D0/(TR*8.31451D0)
      XL = -24.01845D0/(TR*TR*8.31451D0)
      XJ = -1.39D-5
      DO I = 1,17
      XCP(I) = 0.D0
      ENDDO
      XCP(1) = -TR*TR*XJ
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      BETA0V = 0.183276906D-1/(TR*8.31451D0)
      BETA0NAFORM = BETA0 + (P1 - P0)*BETA0V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA1NAFORM(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 298.15D0
C
      XG = 864.0310D0/(TR*8.31451D0)
      XL = 660.990D0/(TR*TR*8.31451D0)
      XJ = -6.88D-7
      DO I = 1,17
      XCP(I) = 0.D0
      ENDDO
      XCP(1) = -TR*TR*XJ
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      BETA1V = 0.419029834D-1/(TR*8.31451D0)
      BETA1NAFORM = BETA1 + (P1 - P0)*BETA1V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION CPHINAFORM(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      P1 = PXX
      T1 = TXX
      P0 = 1.D0
      TR = 298.15D0
C
      XG = -8.134755D0/(TR*8.31451D0)
      XL = -27.0291D0/(TR*TR*8.31451D0)
      XJ = 2.22D-6
      DO I = 1,17
      XCP(I) = 0.D0
      ENDDO
      XCP(1) = -TR*TR*XJ
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      CPHIV = -0.387290559D-3/(TR*8.31451D0)
      CPHINAFORM = CPHI + (P1 - P0)*CPHIV
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION G0NAF(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)            
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      DIMENSION CPXX(14)
      P1 = PXX
      T1 = TXX
C
C     CODATA
C
      HXX = -575690.D0
      SXX = 44.65D0 
C      
C     Saluja et al. 1992
C
      CPXX(1) = 4.60951D3
      CPXX(2) = -7.70467D5
      CPXX(3) = 0.D0
      CPXX(4) = -7.03589D0
      CPXX(5) = 0.D0
      CPXX(6) = 0.D0
      CPXX(7) = 0.D0
      CPXX(8) = 0.D0
      CPXX(9) = 0.D0
      CPXX(10) = 0.D0
      CPXX(11) = 0.D0
      CPXX(12) = 0.D0
      CPXX(13) = 0.D0
      CPXX(14) = 0.D0
C
      P0 = 1.D0
C
C     Saluja et al. 1992
C      
      V0NAF = -0.955949D5/T1+0.727528D3-1.75804*T1+1.29150D-3*T1*T1
      VXXX = 1.D-1*(P1 - P0)*V0NAF/(8.31451D0*T1)
C      
      G0NAF = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX 
      
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA0NAF(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      P1 = PXX
      T1 = TXX
      P0 = 6.D0
      TR = 298.15D0
C
      XG = 0.019106
      XL = 4.8911D-4
      DO I = 1,17
      XCP(I) = 0.D0
      ENDDO
      XCP(4) = 3.05959D-1
      XCP(5) = -16.4442D-4
      XCP(15) = 2.15244D-6
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      BETA0V = 3.85545D0/T1 - 2.25639D-2 + 3.31212D-5*T1
C	
C     BETA0V in MPa-1 --> factor 0.1	
C      
      BETA0NAF = BETA0 + 1.D-1*(P1 - P0)*BETA0V
      RETURN
      END
C
      DOUBLE PRECISION FUNCTION BETA1NAF(PXX,TXX)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DIMENSION XCP(17)
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE
      P1 = PXX
      T1 = TXX
      P0 = 6.D0
      TR = 298.15D0
C
      XG = 0.22511
      XL = 2.9372D-4
      DO I = 1,17
      XCP(I) = 0.D0
      ENDDO
      XCP(4) = -1.39083D-1
      XCP(5) = 4.54741D-4
      BETA1NAF = XHGCP(P1,T1,XG,XL,XCP,TR)
C
      RETURN
      END

