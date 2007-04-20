
      SUBROUTINE USERGP 
     $    (NTXX,ITXX1,ITXX2,ITXX3,ITXX4,WTXX,PXX,TXX,G0XX,V0XX,XXX,CHXX, 
     $     MDLXX,NCXX,APHI,SION,TMI,GINT,GXX,OSMO) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
C     
C     SUBROUTINE TO PROVIDE CONTRIBUTIONS FROM A GIVEN INTERACTION TO 
C     THE EXCESS PARTIAL GIBBS ENERGY OF ALL PHASE CONSTITUENTS 
C     
C     THIS SUBROUTINE WILL NOT BE CALLED FOR THE GASEOUS PHASE 
C     
C     THE SUPPLIED MODELS MIGHT HAVE NAMES 'USP?' OR 'US?Z' ('Z' AS 
C     4TH CHARACTER IS RESERVED FOR AQUEOUS PHASES ONLY) 
C     
C     CHEMSAGE WILL ADD ON (G0XX + LOG(XXX)) TO THE VALUE OF GXX 
C     PROVIDED WITH THIS SUBROUTINE TO CALCULATE CHEMICAL POTENTIALS 
C     (THE VARIABLES G0XX, XXX, AND GXX ARE DESCRIBED BELOW) 
C     
C     INPUT QUANTITIES: 
C     NTXX = ORDER OF THE GIVEN INTERACTION 
C     ITXX1 = INDEX NUMBER OF AN INTERACTING CONSTITUENT 
C     ITXX2 = INDEX NUMBER OF AN INTERACTING CONSTITUENT 
C     ITXX3 = INDEX NUMBER OF AN INTERACTING CONSTITUENT 
C     (= CLASSIFICATION NUMBER FOR BINARY INTERACTIONS) 
C     ITXX4 = INDEX NUMBER OF AN INTERACTING CONSTITUENT (= 0 FOR BINARY 
C     AND TERNARY INTERACTIONS OR FOR AQUEOUS PHASES) 
C     WTXX = DIMENSIONLESS EXCESS GIBBS ENERGY PARAMETER OF THE GIVEN 
C     INTERACTION 
C     PXX = TOTAL PRESSURE/bar 
C     TXX = TEMPERATURE/K 
      
      
C     G0XX = DIMENSIONLESS STANDARD GIBBS ENERGY AT PXX BAR AND TXX K 
C     OF ALL PHASE CONSTITUENTS 
C     V0XX = MOLAR VOLUME/CM3 AT 1 BAR AND 298.15 K OF ALL PHASE 
C     CONSTITUENTS 
C     XXX = MOLE FRACTION OF ALL PHASE CONSTITUENTS OR MOLALITY OF ALL 
C     SOLUTES (AQUEOUS PHASES ONLY) 
C     CHXX = CHARGE OF ALL SOLUTES (AQUEOUS PHASES ONLY) 
C     MDLXX = MODEL NAME FOR THE PHASE 
C     NCXX = NUMBER OF PHASE CONSTITUENTS FOR THE PHASE 
C     APHI = PARAMETER A(PHI) OF THE DEBYE-HUECKEL LIMITING LAW 
C     (AQUEOUS PHASES ONLY) 
C     SION = IONIC STRENGTH (AQUEOUS PHASES ONLY) 
C     TMI = SUM OF SOLUTE MOLALITIES (AQUEOUS PHASES ONLY) 
C     
C     OUTPUT QUANTITIES (DEFAULT VALUE = 0): 
C     GINT = CONTRIBUTION TO THE EXCESS PARTIAL GIBBS ENERGY FROM THE 
C     DEBYE-HUECKEL TERM AFTER THIS HAS BEEN DIVIDED BY A SOLUTE 
C     CHARGE SQUARED (AQUEOUS PHASES ONLY) 
C     GXX = CONTRIBUTION TO THE EXCESS PARTIAL GIBBS ENERGY OF ALL 
C     PHASE CONSTITUENTS FROM THE GIVEN INTERACTION 
C     OSMO = CONTRIBUTION TO THE OSMOTIC COEFFICIENT FROM THE DEBYE- 
C     HUECKEL TERM (NTXX = 0) OR FROM THE GIVEN INTERACTION 
C     (NTXX > 0) (AQUEOUS PHASES ONLY) 
C     
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAOH/ MGC, MVC, MDLGC, MDLVC 
      COMMON /NA2SO4/ MGT, MVT, MDLGT, MDLVT 
      COMMON /NAHCO3/ MGN, MVN, MDLGN, MDLVN 
      COMMON /NA2CO3/ MGS, MVS, MDLGS, MDLVS 
      COMMON /NAALOH4/ MGA, MVA, MDLGA, MDLVA 
      CHARACTER*65 MDLGC(10), MDLGT(10), MDLGN(10), MDLGS(10), MDLGA(10) 
      CHARACTER*65 MDLVC(10), MDLVT(10), MDLVN(10), MDLVS(10), MDLVA(10) 
      CHARACTER MDLXX*5 
      DIMENSION CHXX(*), GXX(*), G0XX(*), V0XX(*), XXX(*) 
C     
      DIMENSION AI(0:20), AII(0:20), BP(0:20), CHP(3), DP(0:20), XX(3) 
      DATA AI /1.925154014814667D0,-.060076477753119D0, 
     $     -.029779077456514D0,-.007299499690937D0,0.000388260636404D0, 
     $     0.000636874599598D0,0.000036583601823D0,-.000045036975204D0, 
     $     -.000004537895710D0,0.000002937706971D0,0.000000396566462D0, 
     $     -.000000202099617D0,-.000000025267769D0,0.000000013522610D0, 
     $     0.000000001229405D0,-.000000000821969D0,-.000000000050847D0, 
     $     0.000000000046333D0,0.000000000001943D0,-.000000000002563D0, 
     $     -.000000000010991D0/ 
      DATA AII /0.628023320520852D0,0.462762985338493D0, 
     $     0.150044637187895D0,-.028796057604906D0,-.036552745910311D0, 
     $     -.001668087945272D0,0.006519840398744D0,0.001130378079086D0, 
     $     -.000887171310131D0,-.000242107641309D0,0.000087294451594D0, 
     $     0.000034682122751D0,-.000004583768938D0,-.000003548684306D0, 
     $     -.000000250453880D0,0.000000216991779D0,0.000000080779570D0, 
     $     0.000000004558555D0,-.000000006944757D0,-.000000002849257D0, 
     $     0.000000000237816D0/ 
      IF (MDLXX .EQ. 'USAZ') THEN 
C     
C     Model for NaOH 
C     
C     MGC - Gibbs energy model: 
         MDLGC(1) = 'Simonson et al. (1989)' 
         MDLGC(2) = 'const. Cp model (Koenigsberger, 2001)' 
         MDLGC(3) = 'const. Cp model, Cp fitted to Murdoch data' 
C     
C     MVC - Volumetric model: 
         
         
         MDLVC(1) = 'Simonson et al. (1989)' 
         MDLVC(2) = 'Pitzer fit to 25 C density data' 
C     
         MGC=1 
         MVC=1 
C     
C     Model for Na2SO4 
C     
C     MGT - Gibbs energy model: 
         MDLGT(1) = 'Holmes and Mesmer (1986)' 
         MDLGT(2) = 'Pabalan and Pitzer (1988) saturation pressure' 
         MDLGT(3) = 'Pabalan and Pitzer (1988) 200 bar' 
         MDLGT(4) = 'const. Cp model, XG, XL from Pitzer (1991), XJ from C& 
     $        M (1999)' 
         MDLGT(5) = 'const. Cp model, XG, XL: Pitzer (1991), XJ fitted to M 
     $        urdoch data' 
C     
C     MVT - Volumetric model: 
         MDLVT(1) = 'Monnin (1990)' 
         MDLVT(2) = '2 times NaCl (for MGT = 3 only)' 
         MDLVT(3) = 'Krumgalz et al. (2000)' 
C     
         MGT=1 
         MVT=1 
C     
C     Model for Na2CO3 
C     
C     MGS - Gibbs energy model: 
         MDLGS(1) = 'const. Cp model (Koenigsberger, 2001)' 
         MDLGS(2) = 'Polya (2001), U2, U8 adj. to XL and XJ at 25 C of mode 
     $        l 1' 
         MDLGS(3) = 'Polya et al. (2001)' 
         MDLGS(4) = 'const. Cp model, with Cphi_J adj. to fit Murdoch data 
     $        ' 
         MDLGS(5) = 'XG, XL from P&P, XJ(298) for UNHYDROLYSED Na2CO3 from 
     $        C&M' 
C     i.e. HCO3- must be suppressed in CS (automatically 
C     done in BAYER) 
C     
C     MVS - Volumetric model: 
         MDLVS(1) = 'Krumgalz et al. (2000)' 
         MDLVS(2) = 'Polya et al. (2001) 25 C only!' 
C     
         MGS=4 
         MVS=1 
C     
C     Model for NaHCO3 
C     
C     MGN - Gibbs energy model: 
         MDLGN(1) = 'const. Cp model (Koenigsberger, 2001)' 
         MDLGN(2) = 'Polya et al. (2001)' 
C     
C     MVN - Volumetric model: 
         MDLVN(1) = 'Krumgalz et al. (2000)' 
         MDLVN(2) = 'Polya et al. (2001) 25 C only!' 
C     
         MGN=1 
         MVN=1 
C     
C     Model for NaAl(OH)4 
C     
C     MGA - Gibbs energy model: 
         
         
         MDLGA(1) = 'Wesolowski (1992)' 
         MDLGA(2) = 'Caiani et al. (1989) for XJ, XL, Wesolowski for XG' 
         MDLGA(3) = 'Park & Englezos (1999)' 
C     
C     MVA - Volumetric model: 
         MDLVA(1) = 'V0 only' 
C     
         MGA=1 
         MVA=1 
C     
         IF(NTXX.EQ.0)THEN 
C     
C     The section below is for initialization 
C     
C     Standard Gibbs energies 
C     
            G0XX(1) = G0H2O(PXX,TXX) 
            G0XX(2) = 0.D0 
            G0XX(3) = G0NAOH(PXX,TXX) 
            G0XX(4) = G0NAHCO3(PXX,TXX) 
            G0XX(5) = G0NA2CO3(PXX,TXX) 
            G0XX(6) = G0NACL(PXX,TXX) 
            G0XX(7) = G0NA2SO4(PXX,TXX) 
            G0XX(8) = G0NAALOH4(PXX,TXX) 
            G0XX(9) = G0NA2SO4(PXX,TXX) 
C     
            IF (SION .EQ. 0.D0) RETURN 
            SION2 = SION + SION 
            SQSION = SQRT(SION) 
            TEMP = -APHI*SION2 
            TEMP1 = 1.2D0*SQSION + 1.D0 
            GINT = 2.D0*TEMP*LOG(TEMP1)/1.2D0 
            OSMO = TEMP*SQSION/TEMP1 
            SION2 = 1.D0/SION2 
            ZUSAZ = 0.D0 
            DOI=2,NCXX 
            ZUSAZ = ZUSAZ + XXX(I)*ABS(CHXX(I)) 
            GXX(I) = 0.D0 
         ENDDO 
         IF(NCXX.GT.2)THEN 
            DP(20) = 0.D0 
            TEMP1 = 6.D0*APHI*SQSION 
            TEMP2 = 1.D0/9.D0 
            DOI=2,NCXX-1 
            IF (CHXX(I) .NE. 0.D0 .AND. XXX(I) .GT. 0.D0) THEN 
               XX(2) = TEMP1*CHXX(I)*CHXX(I) 
               DOK=I+1,NCXX 
               IF (CHXX(K) .EQ. 0.D0 .OR. XXX(K) .EQ. 0.D0) THEN 
               ELSEIF (SIGN(CHXX(K),CHXX(I)) .EQ. CHXX(K)) THEN 
                  CHP(2) = -0.25D0*SION2*CHXX(I)*CHXX(K)*XXX(I)*XXX(K) 
                  CHP(1) = -CHP(2) - CHP(2) 
                  CHP(3) = CHP(2) 
                  TEMP = TEMP1*CHXX(K) 
                  XX(1) = TEMP*CHXX(I) 
                  XX(3) = TEMP*CHXX(K) 
                  FIJ=0.D0 
                  DOJ=1,3 
                  IF (XX(J) .GT. 1.D0) THEN 
                     XLN = -0.1D0*LOG(XX(J)) 
                     ZPITZ = TEMP2*(40.D0*EXP(XLN) - 22.D0) 
                     DZDX = -TEMP2*EXP(11.D0*XLN) 
                     BP(20) = AII(20) 
                     
                     
                     BP(19) = ZPITZ*AII(20) + AII(19) 
                     DP(19) = AII(20) 
                     DOL=18,0,-1 
                     L1=L+1 
                     L2=L+2 
                     BP(L) = ZPITZ*BP(L1) - BP(L2) + AII(L) 
                     DP(L) = BP(L1) + ZPITZ*DP(L1) - DP(L2) 
                  ENDDO 
               ELSE 
                  XLN = 0.2D0*LOG(XX(J)) 
                  ZPITZ = 4.D0*EXP(XLN) - 2.D0 
                  DZDX = 0.2D0*EXP(-4.D0*XLN) 
                  BP(20) = AI(20) 
                  BP(19) = ZPITZ*AI(20) + AI(19) 
                  DP(19) = AI(20) 
                  DOL=18,0,-1 
                  L1=L+1 
                  L2=L+2 
                  BP(L) = ZPITZ*BP(L1) - BP(L2) + AI(L) 
                  DP(L) = BP(L1) + ZPITZ*DP(L1) - DP(L2) 
               ENDDO 
            ENDIF 
            FIJ = FIJ - CHP(J)*(BP(0) - BP(2) + 0.5D0*XX(J) - 2.D0) 
            OSMO = OSMO + CHP(J)*XX(J)*(2.D0*DZDX*(DP(0) - DP(2)) + 0.25
     $           D0)
         ENDDO 
         GINT = GINT + FIJ 
         GXX(I) = GXX(I) - FIJ/XXX(I) 
         GXX(K) = GXX(K) - FIJ/XXX(K) 
      ENDIF 
      ENDDO 
      ENDIF 
      ENDDO 
      ENDIF 
      GINT = SION2*(GINT + OSMO) 
      IF (TMI .GT. 0.D0) OSMO = OSMO/TMI 
      ELSE 
C     
C     The section below is called for each interaction term 
C     
         IF (SION .EQ. 0.D0) RETURN 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
C     
         IF(NTXX.EQ.3)THEN 
            OSMO = XXX(ITXX3)*(WTIJ + WTIJ) 
            GXX(ITXX1) = WTJ*XXX(ITXX3) 
            GXX(ITXX2) = WTI*XXX(ITXX3) 
            GXX(ITXX3) = WTIJ 
C     
         ELSEIF (ITXX3 .EQ. 0) THEN 
            OSMO = WTIJ + WTIJ 
            GXX(ITXX1) = WTJ + WTJ 
            GXX(ITXX2) = WTI + WTI 
C     
         ELSEIF (ITXX3 .LT. 3) THEN 
            K = 2*ITXX3 
            IF (ABS(CHXX(ITXX1)).NE.2.D0.OR.ABS(CHXX(ITXX2)).NE.2.D0) 
     $           K=K-1 
            IF (K .EQ. 1) THEN 
               GF = 2.D0*SQSION 
            ELSEIF (K .EQ. 2) THEN 
               GF = 1.4D0*SQSION 
               
               
            ELSEIF (K .EQ. 3) THEN 
               GF = 50.D0*SQSION 
            ELSE 
               GF = 12.D0*SQSION 
            ENDIF 
            GFE = 2.D0*EXP(-GF) 
            GX = 2.D0*(2.D0 - GF*GFE - GFE)/(GF*GF) 
            TEMP = SION2*WTIJ*(GFE - GX) 
            OSMO = WTIJ*GFE 
            GXX(ITXX1) = WTJ*GX 
            GXX(ITXX2) = WTI*GX 
            DOI=2,NCXX 
            GXX(I) = GXX(I) + TEMP*CHXX(I)*CHXX(I) 
         ENDDO 
C     
      ELSEIF(ITXX3 .EQ. 3) THEN 
         TEMP = 1.D0/SQRT(-CHXX(ITXX1)*CHXX(ITXX2)) 
         TEMP1 = TEMP*ZUSAZ 
         OSMO = TEMP1*WTIJ 
         TEMP1 = 0.5D0*TEMP1 
         GXX(ITXX1) = TEMP1*WTJ 
         GXX(ITXX2) = TEMP1*WTI 
         TEMP = 0.5D0*TEMP*WTIJ 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*ABS(CHXX(I)) 
      ENDDO 
C     
      ELSEIF (ITXX3 .EQ. 4) THEN 
         IF (TSAVE .LT. 338.15D0) THEN 
            WTXX = BETA0NACLlow(PXX,TXX) 
         ELSE 
            WTXX = BETA0NACL(PXX,TXX) 
         ENDIF 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         OSMO = WTIJ + WTIJ 
         GXX(ITXX1) = WTJ + WTJ 
         GXX(ITXX2) = WTI + WTI 
C     
      ELSEIF (ITXX3 .EQ. 5) THEN 
         WTXX = BETA1NACL(TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         GF = 2.D0*SQSION 
         GFE = 2.D0*EXP(-GF) 
         GX = 2.D0*(2.D0 - GF*GFE - GFE)/(GF*GF) 
         TEMP = SION2*WTIJ*(GFE - GX) 
         OSMO = WTIJ*GFE 
         GXX(ITXX1) = WTJ*GX 
         GXX(ITXX2) = WTI*GX 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*CHXX(I)*CHXX(I) 
      ENDDO 
C     
      ELSEIF(ITXX3 .EQ. 6) THEN 
         IF (TSAVE .LT. 338.15D0) THEN 
            WTXX = CPHINACLlow(PXX,TXX) 
         ELSE 
            WTXX = CPHINACL(PXX,TXX) 
         ENDIF 
         
         
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         TEMP = 1.D0/SQRT(-CHXX(ITXX1)*CHXX(ITXX2)) 
         TEMP1 = TEMP*ZUSAZ 
         OSMO = TEMP1*WTIJ 
         TEMP1 = 0.5D0*TEMP1 
         GXX(ITXX1) = TEMP1*WTJ 
         GXX(ITXX2) = TEMP1*WTI 
         TEMP = 0.5D0*TEMP*WTIJ 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*ABS(CHXX(I)) 
      ENDDO 
C     
      ELSEIF (ITXX3 .EQ. 7) THEN 
         WTXX = BETA0NAOH(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         OSMO = WTIJ + WTIJ 
         GXX(ITXX1) = WTJ + WTJ 
         GXX(ITXX2) = WTI + WTI 
C     
      ELSEIF (ITXX3 .EQ. 8) THEN 
         WTXX = BETA1NAOH(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         GF = 2.D0*SQSION 
         GFE = 2.D0*EXP(-GF) 
         GX = 2.D0*(2.D0 - GF*GFE - GFE)/(GF*GF) 
         TEMP = SION2*WTIJ*(GFE - GX) 
         OSMO = WTIJ*GFE 
         GXX(ITXX1) = WTJ*GX 
         GXX(ITXX2) = WTI*GX 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*CHXX(I)*CHXX(I) 
      ENDDO 
C     
      ELSEIF (ITXX3 .EQ. 9) THEN 
         WTXX = BETA2NAOH(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         GF = 8.D0*SQSION 
         GFE = 2.D0*EXP(-GF) 
         GX = 2.D0*(2.D0 - GF*GFE - GFE)/(GF*GF) 
         TEMP = SION2*WTIJ*(GFE - GX) 
         OSMO = WTIJ*GFE 
         GXX(ITXX1) = WTJ*GX 
         GXX(ITXX2) = WTI*GX 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*CHXX(I)*CHXX(I) 
      ENDDO 
C     
      ELSEIF(ITXX3 .EQ. 10) THEN 
         WTXX = CPHINAOH(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         TEMP = 1.D0/SQRT(-CHXX(ITXX1)*CHXX(ITXX2)) 
         TEMP1 = TEMP*ZUSAZ 
         
         
         OSMO = TEMP1*WTIJ 
         TEMP1 = 0.5D0*TEMP1 
         GXX(ITXX1) = TEMP1*WTJ 
         GXX(ITXX2) = TEMP1*WTI 
         TEMP = 0.5D0*TEMP*WTIJ 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*ABS(CHXX(I)) 
      ENDDO 
C     
      ELSEIF (ITXX3 .EQ. 11) THEN 
         WTXX = BETA0NA2SO4(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         OSMO = WTIJ + WTIJ 
         GXX(ITXX1) = WTJ + WTJ 
         GXX(ITXX2) = WTI + WTI 
C     
      ELSEIF (ITXX3 .EQ. 12) THEN 
         WTXX = BETA1NA2SO4(PXX,TXX,SQSION) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         GF = 1.4D0*SQSION 
         GFE = 2.D0*EXP(-GF) 
         GX = 2.D0*(2.D0 - GF*GFE - GFE)/(GF*GF) 
         TEMP = SION2*WTIJ*(GFE - GX) 
         OSMO = WTIJ*GFE 
         GXX(ITXX1) = WTJ*GX 
         GXX(ITXX2) = WTI*GX 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*CHXX(I)*CHXX(I) 
      ENDDO 
C     
      ELSEIF(ITXX3 .EQ. 13) THEN 
         WTXX = CPHINA2SO4(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         TEMP = 1.D0/SQRT(-CHXX(ITXX1)*CHXX(ITXX2)) 
         TEMP1 = TEMP*ZUSAZ 
         OSMO = TEMP1*WTIJ 
         TEMP1 = 0.5D0*TEMP1 
         GXX(ITXX1) = TEMP1*WTJ 
         GXX(ITXX2) = TEMP1*WTI 
         TEMP = 0.5D0*TEMP*WTIJ 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*ABS(CHXX(I)) 
      ENDDO 
C     
      ELSEIF (ITXX3 .EQ. 14) THEN 
         WTXX = BETA0NAHCO3(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         OSMO = WTIJ + WTIJ 
         GXX(ITXX1) = WTJ + WTJ 
         GXX(ITXX2) = WTI + WTI 
C     
      ELSEIF (ITXX3 .EQ. 15) THEN 
         WTXX = BETA1NAHCO3(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         
         
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         GF = 2.D0*SQSION 
         GFE = 2.D0*EXP(-GF) 
         GX = 2.D0*(2.D0 - GF*GFE - GFE)/(GF*GF) 
         TEMP = SION2*WTIJ*(GFE - GX) 
         OSMO = WTIJ*GFE 
         GXX(ITXX1) = WTJ*GX 
         GXX(ITXX2) = WTI*GX 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*CHXX(I)*CHXX(I) 
      ENDDO 
C     
      ELSEIF(ITXX3 .EQ. 16) THEN 
         WTXX = CPHINAHCO3(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         TEMP = 1.D0/SQRT(-CHXX(ITXX1)*CHXX(ITXX2)) 
         TEMP1 = TEMP*ZUSAZ 
         OSMO = TEMP1*WTIJ 
         TEMP1 = 0.5D0*TEMP1 
         GXX(ITXX1) = TEMP1*WTJ 
         GXX(ITXX2) = TEMP1*WTI 
         TEMP = 0.5D0*TEMP*WTIJ 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*ABS(CHXX(I)) 
      ENDDO 
C     
      ELSEIF (ITXX3 .EQ. 17) THEN 
         WTXX = BETA0NA2CO3(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         OSMO = WTIJ + WTIJ 
         GXX(ITXX1) = WTJ + WTJ 
         GXX(ITXX2) = WTI + WTI 
C     
      ELSEIF (ITXX3 .EQ. 18) THEN 
         WTXX = BETA1NA2CO3(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         GF = 2.D0*SQSION 
         GFE = 2.D0*EXP(-GF) 
         GX = 2.D0*(2.D0 - GF*GFE - GFE)/(GF*GF) 
         TEMP = SION2*WTIJ*(GFE - GX) 
         OSMO = WTIJ*GFE 
         GXX(ITXX1) = WTJ*GX 
         GXX(ITXX2) = WTI*GX 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*CHXX(I)*CHXX(I) 
      ENDDO 
C     
      ELSEIF(ITXX3 .EQ. 19) THEN 
         WTXX = CPHINA2CO3(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         TEMP = 1.D0/SQRT(-CHXX(ITXX1)*CHXX(ITXX2)) 
         TEMP1 = TEMP*ZUSAZ 
         OSMO = TEMP1*WTIJ 
         
         
         TEMP1 = 0.5D0*TEMP1 
         GXX(ITXX1) = TEMP1*WTJ 
         GXX(ITXX2) = TEMP1*WTI 
         TEMP = 0.5D0*TEMP*WTIJ 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*ABS(CHXX(I)) 
      ENDDO 
C     
      ELSEIF (ITXX3 .EQ. 20) THEN 
         WTXX = BETA0NAALOH4(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         OSMO = WTIJ + WTIJ 
         GXX(ITXX1) = WTJ + WTJ 
         GXX(ITXX2) = WTI + WTI 
C     
      ELSEIF (ITXX3 .EQ. 21) THEN 
         WTXX = BETA1NAALOH4(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         GF = 2.D0*SQSION 
         GFE = 2.D0*EXP(-GF) 
         GX = 2.D0*(2.D0 - GF*GFE - GFE)/(GF*GF) 
         TEMP = SION2*WTIJ*(GFE - GX) 
         OSMO = WTIJ*GFE 
         GXX(ITXX1) = WTJ*GX 
         GXX(ITXX2) = WTI*GX 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*CHXX(I)*CHXX(I) 
      ENDDO 
C     
      ELSEIF(ITXX3 .EQ. 22) THEN 
         WTXX = CPHINAALOH4(PXX,TXX) 
         WTI = XXX(ITXX1)*WTXX 
         WTJ = XXX(ITXX2)*WTXX 
         WTIJ = WTI*XXX(ITXX2) 
         TEMP = 1.D0/SQRT(-CHXX(ITXX1)*CHXX(ITXX2)) 
         TEMP1 = TEMP*ZUSAZ 
         OSMO = TEMP1*WTIJ 
         TEMP1 = 0.5D0*TEMP1 
         GXX(ITXX1) = TEMP1*WTJ 
         GXX(ITXX2) = TEMP1*WTI 
         TEMP = 0.5D0*TEMP*WTIJ 
         DOI=2,NCXX 
         GXX(I) = GXX(I) + TEMP*ABS(CHXX(I)) 
      ENDDO 
C     
      ENDIF 
      IF (TMI .GT. 0.D0) OSMO = OSMO/TMI 
      ENDIF 
      ENDIF 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION PSAT(T) 
      
      
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      IF (T .GT. 373.15D0) THEN 
         PSAT = 105.6184D0 + T*(-1.4523486D0 + T*(0.0073825404D0 + T* 
     $        (-1.6596034D-05 + T*1.4012495D-08))) 
      ELSE 
         PSAT = 1.D0 
      ENDIF 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION VXP(P1,T1,VX,VP,PR,P0) 
C     
C     Calculates V0 and dV0/dP contribution to G0 
C     Integrate (V0(P0) + (P - P0)V0')dP from Pr to P ---> 
C     (P - Pr)[V0(P0) + {0.5(P + Pr) - P0}V0'] 
C     P in MPa --> factor 0.1 
C     
C     V0 = VX(1) + VX(2)/T + VX(3)*LOG(T) + VX(4)*T + 
C     VX(5)*T*T + VX(6)/(T-VX(7)) + VX(8)/(VX(9)-T) + 
C     VX(10)/(T*T) + VX(11)*T/(T-VX(12))**3 + 
C     VX(13)*T/(VX(14)-T)**3 + 
C     V0'= VP(1) + VP(2)/T + VP(3)*LOG(T) + VP(4)*T + 
C     VP(5)*T*T + VP(6)/(T-VX(7)) + VP(8)/(VP(9)-T) + 
C     VP(10)/(T*T) + VP(11)*T/(T-VP(12))**3 + 
C     VP(13)*T/(VP(14)-T)**3 
C     
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION VX(14), VP(14) 
      VXX1 = VX(1) + VX(2)/T1 + VX(3)*LOG(T1) + VX(4)*T1 + 
     $     VX(5)*T1*T1 + VX(6)/(T1-VX(7)) + VX(8)/(VX(9)-T1) 
      VXX2 = VX(10)/(T1*T1) + 
     $     VX(11)*T1/((T1-VX(12))*(T1-VX(12))*(T1-VX(12))) + 
     $     VX(13)*T1/((VX(14)-T1)*(VX(14)-T1)*(VX(14)-T1)) 
      VXP1 = VP(1) + VP(2)/T1 + VP(3)*LOG(T1) + VP(4)*T1 + 
     $     VP(5)*T1*T1 + VP(6)/(T1-VP(7)) + VP(8)/(VP(9)-T1) 
      VXP2 = VP(10)/(T1*T1) + 
     $     VP(11)*T1/((T1-VP(12))*(T1-VP(12))*(T1-VP(12))) + 
     $     VP(13)*T1/((VP(14)-T1)*(VP(14)-T1)*(VP(14)-T1)) 
      VXP = VXX1 + VXX2 + 1.D-1*(0.5D0*(P1 + PR) - P0)*(VXP1 + VXP2) 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION V0XP(P1,T1,VX,VP,P0) 
C     V0 = VX(1) + VX(2)/T + VX(3)*LOG(T) + VX(4)*T + 
C     VX(5)*T*T + VX(6)/(T-VX(7)) + VX(8)/(VX(9)-T) + 
C     VX(10)/(T*T) + VX(11)*T/(T-VX(12))**3 + 
C     VX(13)*T/(VX(14)-T)**3 + 
C     1.D-1*(P-P0)*VP(1) + VP(2)/T + VP(3)*LOG(T) + VP(4)*T + 
C     VP(5)*T*T + VP(6)/(T-VX(7)) + VP(8)/(VP(9)-T) + 
C     VP(10)/(T*T) + VP(11)*T/(T-VP(12))**3 + 
C     VP(13)*T/(VP(14)-T)**3 
C     P in MPa --> factor 0.1 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      
      
      DIMENSION VX(14), VP(14) 
      VXX1 = VX(1) + VX(2)/T1 + VX(3)*LOG(T1) + VX(4)*T1 + 
     $     VX(5)*T1*T1 + VX(6)/(T1-VX(7)) + VX(8)/(VX(9)-T1) 
      VXX2 = VX(10)/(T1*T1) + 
     $     VX(11)*T1/((T1-VX(12))*(T1-VX(12))*(T1-VX(12))) + 
     $     VX(13)*T1/((VX(14)-T1)*(VX(14)-T1)*(VX(14)-T1)) 
      VXP1 = VP(1) + VP(2)/T1 + VP(3)*LOG(T1) + VP(4)*T1 + 
     $     VP(5)*T1*T1 + VP(6)/(T1-VP(7)) + VP(8)/(VP(9)-T1) 
      VXP2 = VP(10)/(T1*T1) + 
     $     VP(11)*T1/((T1-VP(12))*(T1-VP(12))*(T1-VP(12))) + 
     $     VP(13)*T1/((VP(14)-T1)*(VP(14)-T1)*(VP(14)-T1)) 
      V0XP=VXX1+VXX2+1.D-1*(P1 - P0)*(VXP1+VXP2) 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION CPXP(P1,T1,CpX) 
C     Cp0 = CpX(1) + CpX(2)/T + CpX(3)*LOG(T) + CpX(4)*T + 
C     CpX(5)*T*T + CpX(6)/(T-CpX(7)) + CpX(8)/(CpX(9)-T) + 
C     CpX(10)/(T*T) + CpX(11)*T/(T-CpX(12))**3 + 
C     CpX(13)*T/(CpX(14)-T)**3 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION CpX(14) 
      CpXX1 = CpX(1) + CpX(2)/T1 + CpX(3)*LOG(T1) + CpX(4)*T1 + 
     $     CpX(5)*T1*T1 + CpX(6)/(T1-CpX(7)) + CpX(8)/(CpX(9)-T1) 
      CpXX2 = CpX(10)/(T1*T1) + 
     $     CpX(11)*T1/((T1-CpX(12))*(T1-CpX(12))*(T1-CpX(12))) + 
     $     CpX(13)*T1/((CpX(14)-T1)*(CpX(14)-T1)*(CpX(14)-T1)) 
      CPXP = CpXX1 + CpXX2 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION HSCPXX(P1,T1,HXX,SXX,CPXX) 
C     Cp0 = CPXX(1) + CPXX(2)/T + CPXX(3)*LOG(T) + CPXX(4)*T + 
C     CPXX(5)*T*T + CPXX(6)/(T-CPXX(7)) + CPXX(8)/(CPXX(9)-T) + 
C     CPXX(10)/(T*T) + CPXX(11)*T/(T-CPXX(12))**3 + 
C     CPXX(13)*T/(CPXX(14)-T)**3 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION CPXX(14) 
      TR = 2.9815D2 
      HTXX = HXX + CPHXX(P1,T1,CPXX) - CPHXX(P1,TR,CPXX) 
      STXX = SXX + CPSXX(P1,T1,CPXX) - CPSXX(P1,TR,CPXX) 
      HSCPXX = (HTXX - T1*STXX)/(8.31451D0*T1) 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION HSCPXX2(P1,T1,HXX,SXX,CPXX1,CPXX2,TR2) 
C     2 T ranges 
C     Cp0 = CPXX(1) + CPXX(2)/T + CPXX(3)*LOG(T) + CPXX(4)*T + 
      
      
C     CPXX(5)*T*T + CPXX(6)/(T-CPXX(7)) + CPXX(8)/(CPXX(9)-T) + 
C     CPXX(10)/(T*T) + CPXX(11)*T/(T-CPXX(12))**3 + 
C     CPXX(13)*T/(CPXX(14)-T)**3 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION CPXX1(14), CPXX2(14) 
      TR1 = 2.9815D2 
      HTXX = HXX + CPHXX(P1,TR2,CPXX1) - CPHXX(P1,TR1,CPXX1) 
     $     + CPHXX(P1,T1,CPXX2) - CPHXX(P1,TR2,CPXX2) 
      STXX = SXX + CPSXX(P1,TR2,CPXX1) - CPSXX(P1,TR1,CPXX1) 
     $     + CPSXX(P1,T1,CPXX2) - CPSXX(P1,TR2,CPXX2) 
      HSCPXX2 = (HTXX - T1*STXX)/(8.31451D0*T1) 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION CPHXX(P1,T1,CPXX) 
C     Integrate Cp0 dT --> Contribution to H 
C     Cp0 = CPXX(1) + CPXX(2)/T + CPXX(3)*LOG(T) + CPXX(4)*T + 
C     CPXX(5)*T*T + CPXX(6)/(T-CPXX(7)) + CPXX(8)/(CPXX(9)-T) + 
C     CPXX(10)/(T*T) + CPXX(11)*T/(T-CPXX(12))**3 + 
C     CPXX(13)*T/(CPXX(14)-T)**3 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION CPXX(14) 
      CPH1 = CPXX(1)*T1 + (CPXX(2) + CPXX(3)*T1)*LOG(T1) - CPXX(3)*T1 + 
     $     5.D-1*CPXX(4)*T1*T1 + CPXX(5)*T1*T1*T1/3.D0 
      CPH2 = CPXX(6)*LOG(ABS(T1-CPXX(7))) - CPXX(8)*LOG(ABS(CPXX(9)-T1)) 
     $     - CPXX(10)/T1 + CPXX(11)*(CPXX(12) - 2.D0*T1)/(2.D0*(CPXX(12) 
     $     - T1)*(CPXX(12) - T1)) 
      CPH3 = CPXX(13)*(2.D0*T1 - CPXX(14))/(2.D0*(CPXX(14) - 
     $     T1)*(CPXX(14) - T1)) 
      CPHXX = CPH1 + CPH2 + CPH3 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION CPSXX(P1,T1,CPXX) 
C     Integrate Cp0/T dT --> Contribution to S 
C     Cp0 = CPXX(1) + CPXX(2)/T + CPXX(3)*LOG(T) + CPXX(4)*T + 
C     CPXX(5)*T*T + CPXX(6)/(T-CPXX(7)) + CPXX(8)/(CPXX(9)-T) + 
C     CPXX(10)/(T*T) + CPXX(11)*T/(T-CPXX(12))**3 + 
C     CPXX(13)*T/(CPXX(14)-T)**3 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION CPXX(14) 
      IF(CPXX(7) .EQ. 0.D0 .AND. CPXX(9) .EQ. 0.D0) THEN 
         CPS1 = CPXX(1)*LOG(T1) 
     $        - CPXX(2)/T1 + 5.D-1*CPXX(3)*LOG(T1)*LOG(T1) + CPXX(4)*T1 
      ELSEIF(CPXX(7) .EQ. 0.D0) THEN 
         CPS1 = (CPXX(1) + CPXX(8)/CPXX(9))*LOG(T1) 
     $        - CPXX(2)/T1 + 5.D-1*CPXX(3)*LOG(T1)*LOG(T1) + CPXX(4)*T1 
      ELSEIF(CPXX(9) .EQ. 0.D0) THEN 
         CPS1 = (CPXX(1) - CPXX(6)/CPXX(7))*LOG(T1) 
     $        - CPXX(2)/T1 + 5.D-1*CPXX(3)*LOG(T1)*LOG(T1) + CPXX(4)*T1 
      ELSE 
         CPS1 = (CPXX(1) - CPXX(6)/CPXX(7) + CPXX(8)/CPXX(9))*LOG(T1) 
     $        - CPXX(2)/T1 + 5.D-1*CPXX(3)*LOG(T1)*LOG(T1) + CPXX(4)*T1 
         
         
      ENDIF 
      IF(CPXX(7) .EQ. 0.D0 .AND. CPXX(9) .EQ. 0.D0) THEN 
         CPS2 = 5.D-1*CPXX(5)*T1*T1 - 5.D-1*CPXX(10)/(T1*T1) 
      ELSEIF(CPXX(7) .EQ. 0.D0) THEN 
         CPS2 = 5.D-1*CPXX(5)*T1*T1 
     $ - CPXX(8)*LOG(ABS(CPXX(9)-T1))/CPXX(9) - 5.D-1*CPXX(10)/(T1*T1) 
      ELSEIF(CPXX(9) .EQ. 0.D0) THEN 
       CPS2 = 5.D-1*CPXX(5)*T1*T1 + CPXX(6)*LOG(ABS(T1-CPXX(7)))/CPXX(7) 
     $        - 5.D-1*CPXX(10)/(T1*T1) 
      ELSE 
       CPS2 = 5.D-1*CPXX(5)*T1*T1 + CPXX(6)*LOG(ABS(T1-CPXX(7)))/CPXX(7) 
     $   - CPXX(8)*LOG(ABS(CPXX(9)-T1))/CPXX(9) - 5.D-1*CPXX(10)/(T1*T1) 
      ENDIF 
      CPS3 = (-5.D-1)*CPXX(11)/((CPXX(12) - T1)*(CPXX(12) - T1)) 
      CPS4 = 5.D-1*CPXX(13)/((CPXX(14) - T1)*(CPXX(14) - T1)) 
      CPSXX=CPS1+CPS2+CPS3+CPS4 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
C     Calculates excess parameters as a function of T using as input: 
C     XG = excess Gibbs energy parameter at TR 
C     XL = excess enthalpy parameter at TR 
C     XCP = excess heat capacity function calculated from excess 
C     heat capacity parameter function XJ via XCP = -T*T*XJ. 
C     Note: that in this way T-independent XJ parameters lead to 
C     T-dependent excess heat capacity functions! In order 
C     to make Cp^xs T-independent, the XCP(1) parameter has 
C     to be used, e.g., if XJ is valid for 298.15 K, 
C     then XCP(1) = -(298.15)**2*XJ 
C     
C     XCP = XCP(1) + XCP(2)/T + XCP(3)*LOG(T) + XCP(4)*T + 
C     XCP(5)*T*T + XCP(6)/(T-XCP(7)) + XCP(8)/(XCP(9)-T) + 
C     XCP(10)/(T*T) + XCP(11)*T/(T-XCP(12))**3 + 
C     XCP(13)*T/(XCP(14)-T)**3 + XCP(15)*T*T*T + 
C     XCP(16)*T*T*T*T + XCP(17)*T*T*T*T*T 
C     
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      C=0.D0 
      DOI=1,17 
      C=C+XCP(I) 
      ENDDO 
      IF (XL .EQ. 0.D0 .AND. C .EQ. 0.D0) THEN 
         XHGCP = XG*TR/T1 
      ELSE 
         XHGCP = XG + TR*(1.D0 - TR/T1)*XL + 
     $        (XCPH(P1,T1,XCP) - XCPH(P1,TR,XCP))/T1 - 
     $        (XCPS(P1,T1,XCP) - XCPS(P1,TR,XCP)) 
      ENDIF 
      RETURN 
      END 
      
      

      DOUBLE PRECISION FUNCTION XCPH(P1,T1,XCP) 
C     Integrate XCP dT --> Contribution to H 
C     XCP = XCP(1) + XCP(2)/T + XCP(3)*LOG(T) + XCP(4)*T + 
C     XCP(5)*T*T + XCP(6)/(T-XCP(7)) + XCP(8)/(XCP(9)-T) + 
C     XCP(10)/(T*T) + XCP(11)*T/(T-XCP(12))**3 + 
C     XCP(13)*T/(XCP(14)-T)**3 + XCP(15)*T*T*T + 
C     XCP(16)*T*T*T*T + XCP(17)*T*T*T*T*T 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      CPH1 = XCP(1)*T1 + (XCP(2) + XCP(3)*T1)*LOG(T1) - XCP(3)*T1 + 
     $     5.D-1*XCP(4)*T1*T1 + XCP(5)*T1*T1*T1/3.D0 
      CPH2 = XCP(6)*LOG(T1-XCP(7)) - XCP(8)*LOG(ABS(XCP(9)-T1)) 
     $     - XCP(10)/T1 + XCP(11)*(XCP(12) - 2.D0*T1)/(2.D0*(XCP(12) - 
     $     T1)*(XCP(12) - T1)) 
      CPH3 = XCP(13)*(2.D0*T1 - XCP(14))/(2.D0*(XCP(14) - 
     $     T1)*(XCP(14) - T1)) + XCP(15)*T1*T1*T1*T1/4.D0 + 
     $     XCP(16)*T1*T1*T1*T1*T1/5.D0 + XCP(17)*T1*T1*T1*T1*T1*T1/6.D0 
      XCPH=CPH1+CPH2+CPH3 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION XCPS(P1,T1,XCP) 
C     Integrate XCP/T dT --> Contribution to S 
C     XCP = XCP(1) + XCP(2)/T + XCP(3)*LOG(T) + XCP(4)*T + 
C     XCP(5)*T*T + XCP(6)/(T-XCP(7)) + XCP(8)/(XCP(9)-T) + 
C     XCP(10)/(T*T) + XCP(11)*T/(T-XCP(12))**3 + 
C     XCP(13)*T/(XCP(14)-T)**3 + XCP(15)*T*T*T + 
C     XCP(16)*T*T*T*T + XCP(17)*T*T*T*T*T 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      IF(XCP(7) .EQ. 0.D0 .AND. XCP(9) .EQ. 0.D0) THEN 
         CPS1 = XCP(1)*LOG(T1) 
     $        - XCP(2)/T1 + 5.D-1*XCP(3)*LOG(T1)*LOG(T1) + XCP(4)*T1 
      ELSEIF(XCP(7) .EQ. 0.D0) THEN 
         CPS1 = (XCP(1) + XCP(8)/XCP(9))*LOG(T1) 
     $        - XCP(2)/T1 + 5.D-1*XCP(3)*LOG(T1)*LOG(T1) + XCP(4)*T1 
      ELSEIF(XCP(9) .EQ. 0.D0) THEN 
         CPS1 = (XCP(1) - XCP(6)/XCP(7))*LOG(T1) 
     $        - XCP(2)/T1 + 5.D-1*XCP(3)*LOG(T1)*LOG(T1) + XCP(4)*T1 
      ELSE 
         CPS1 = (XCP(1) - XCP(6)/XCP(7) + XCP(8)/XCP(9))*LOG(T1) 
     $        - XCP(2)/T1 + 5.D-1*XCP(3)*LOG(T1)*LOG(T1) + XCP(4)*T1 
      ENDIF 
      IF(XCP(7) .EQ. 0.D0 .AND. XCP(9) .EQ. 0.D0) THEN 
         CPS2 = 5.D-1*XCP(5)*T1*T1 - 5.D-1*XCP(10)/(T1*T1) 
      ELSEIF(XCP(7) .EQ. 0.D0) THEN 
         CPS2 = 5.D-1*XCP(5)*T1*T1 
     $        - XCP(8)*LOG(XCP(9)-T1)/XCP(9) - 5.D-1*XCP(10)/(T1*T1) 
      ELSEIF(XCP(9) .EQ. 0.D0) THEN 
         CPS2 = 5.D-1*XCP(5)*T1*T1 + XCP(6)*LOG(T1-XCP(7))/XCP(7) 
     $        - 5.D-1*XCP(10)/(T1*T1) 
      ELSE 
         CPS2 = 5.D-1*XCP(5)*T1*T1 + XCP(6)*LOG(T1-XCP(7))/XCP(7) 
     $        - XCP(8)*LOG(XCP(9)-T1)/XCP(9) - 5.D-1*XCP(10)/(T1*T1) 
      ENDIF 
      
      
      CPS3 = (-5.D-1)*XCP(11)/((XCP(12) - T1)*(XCP(12) - T1)) + 
     $     5.D-1*XCP(13)/((XCP(14) - T1)*(XCP(14) - T1)) 
      CPS4 = XCP(15)*T1*T1*T1/3.D0 + XCP(16)*T1*T1*T1*T1/4.D0 + 
     $     XCP(17)*T1*T1*T1*T1*T1/5.D0 
      XCPS=CPS1+CPS2+CPS3+CPS4 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION G0H2O(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-J,N-Z) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
C     
C     IAPWS-IF97 
C     
      DIMENSION I(34), J(34), N(34) 
      DATA I /0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,1.D0,1.D0,1.D0, 
     $     1.D0,1.D0,1.D0,2.D0,2.D0,2.D0,2.D0,2.D0,3.D0,3.D0,3.D0, 
     $     4.D0,4.D0,4.D0,5.D0,8.D0,8.D0,21.D0,23.D0,29.D0,30.D0, 
     $     31.D0,32.D0/ 
      DATA J /-2.D0,-1.D0,0.D0,1.D0,2.D0,3.D0,4.D0,5.D0,-9.D0,-7.D0, 
     $     -1.D0,0.D0,1.D0,3.D0,-3.D0,0.D0,1.D0,3.D0,17.D0,-4.D0, 
     $     0.D0,6.D0,-5.D0,-2.D0,10.D0,-8.D0,-11.D0,-6.D0,-29.D0, 
     $     -31.D0,-38.D0,-39.D0,-40.D0,-41.D0/ 
      DATA N /0.14632971213167D0,-0.84548187169114D0, 
     $     -0.37563603672040D1,0.33855169168385D1, 
     $     -0.95791963387872D0,0.15772038513228D0, 
     $     -0.16616417199501D-1,0.81214629983568D-3, 
     $     0.28319080123804D-3,-0.60706301565874D-3, 
     $     -0.18990068218419D-1,-0.32529748770505D-1, 
     $     -0.21841717175414D-1,-0.52838357969930D-4, 
     $     -0.47184321073267D-3,-0.30001780793026D-3, 
     $     0.47661393906987D-4,-0.44141845330846D-5, 
     $     -0.72694996297594D-15,-0.31679644845054D-4, 
     $     -0.28270797985312D-5,-0.85205128120103D-9, 
     $     -0.22425281908000D-5,-0.65171222895601D-6, 
     $     -0.14341729937924D-12,-0.40516996860117D-6, 
     $     -0.12734301741641D-8,-0.17424871230634D-9, 
     $     -0.68762131295531D-18,0.14478307828521D-19, 
     $     0.26335781662795D-22,-0.11947622640071D-22, 
     $     0.18228094581404D-23,-0.93537087292458D-25/ 
      P = PXX/1.653D2 
      T = 1.386D3/TXX 
      G=0.D0 
      DOK=1,34 
      G=G+N(K)*(7.1D0-P)**I(K)*(T-1.222D0)**J(K) 
      ENDDO 
C     Change to Standard Element Reference State and adjustment of R 
      RCORR = 1.0000024D0 
      G0H2O = G*RCORR-(287720.31D0+63.33423D0*TXX)/(8.31451D0*TXX) 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION DNSH2O(PXX,TXX) 
      
      
      IMPLICIT DOUBLE PRECISION(A-J,N-Z) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
C     
C     IAPWS-IF97 
C     
      DIMENSION I(34), J(34), N(34) 
      DATA I /0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,0.D0,1.D0,1.D0,1.D0, 
     $     1.D0,1.D0,1.D0,2.D0,2.D0,2.D0,2.D0,2.D0,3.D0,3.D0,3.D0, 
     $     4.D0,4.D0,4.D0,5.D0,8.D0,8.D0,21.D0,23.D0,29.D0,30.D0, 
     $     31.D0,32.D0/ 
      DATA J /-2.D0,-1.D0,0.D0,1.D0,2.D0,3.D0,4.D0,5.D0,-9.D0,-7.D0, 
     $     -1.D0,0.D0,1.D0,3.D0,-3.D0,0.D0,1.D0,3.D0,17.D0,-4.D0, 
     $     0.D0,6.D0,-5.D0,-2.D0,10.D0,-8.D0,-11.D0,-6.D0,-29.D0, 
     $     -31.D0,-38.D0,-39.D0,-40.D0,-41.D0/ 
      DATA N /0.14632971213167D0,-0.84548187169114D0, 
     $     -0.37563603672040D1,0.33855169168385D1, 
     $     -0.95791963387872D0,0.15772038513228D0, 
     $     -0.16616417199501D-1,0.81214629983568D-3, 
     $     0.28319080123804D-3,-0.60706301565874D-3, 
     $     -0.18990068218419D-1,-0.32529748770505D-1, 
     $     -0.21841717175414D-1,-0.52838357969930D-4, 
     $     -0.47184321073267D-3,-0.30001780793026D-3, 
     $     0.47661393906987D-4,-0.44141845330846D-5, 
     $     -0.72694996297594D-15,-0.31679644845054D-4, 
     $     -0.28270797985312D-5,-0.85205128120103D-9, 
     $     -0.22425281908000D-5,-0.65171222895601D-6, 
     $     -0.14341729937924D-12,-0.40516996860117D-6, 
     $     -0.12734301741641D-8,-0.17424871230634D-9, 
     $     -0.68762131295531D-18,0.14478307828521D-19, 
     $     0.26335781662795D-22,-0.11947622640071D-22, 
     $     0.18228094581404D-23,-0.93537087292458D-25/ 
      P = PSAVE/1.653D2 
      T = 1.386D3/TSAVE 
      G=0.D0 
      DOK=1,34 
      G=G-I(K)*N(K)*(7.1D0-P)**(I(K)-1.D0)*(T-1.222D0)**J(K) 
      ENDDO 
      R = 0.461526D3 
      PPA = 1.D5*PSAVE 
      V = G*P*TSAVE*R/PPA 
      DNSH2O = 1.D-3/V 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION G0NACL(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      DIMENSION CPXX(14), CPXX1(14), CPXX2(14), VX(14), VP(14) 
      P1=PXX 
      T1=TXX 
      TR2 = 3.8315D2 
      HXX = -4.0742D5 
      SXX = 1.1505D2 
      IF (TSAVE. LT. TR2) THEN 
C     0-110C,1bar,Pitzeretal(1984),TableA5-->V3_NaCl.dat 
C     Curve expert 
         VX(1) = 131663.93D0 
         
         
         VX(2) = -2572975.5D0 
         VX(3) = -25343.032D0 
         VX(4) = 85.236575D0 
         VX(5) = -0.05137802D0 
         VX(6) = -45.906818D0 
         VX(7) = 2.27D2 
         VX(8) = 185397.18D0 
         VX(9) = 6.47D2 
C     0-110C,1bar,Pitzeretal(1984),TableA7-->B1_NaCl.dat 
C     Curve expert 
         VP(1) = -75330.066D-2 
         VP(2) = 1601231.D-2 
         VP(3) = 14111.469D-2 
         VP(4) = -41.058297D-2 
         VP(5) = 0.019182144D-2 
         VP(6) = 80.641291D-2 
         VP(7) = 2.27D2 
         VP(8) = 34057.316D-2 
         VP(9) = 6.47D2 
         DOI=10,14 
         VX(I) = 0.D0 
         VP(I) = 0.D0 
      ENDDO 
C     
C     0 - 110 C, 1 bar, Pitzer et al (1984), Table A 4, --> Cp3_NaCl.dat 
C     Curve expert 
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
      PR=1.D0 
      P0=1.D0 
      VXXX = 1.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1) 
      G0NACL = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX 
C     
      ELSE 
C     
C     0 - 110 C, 1 bar, Pitzer et al (1984), Table A 4, --> Cp3_NaCl.dat 
C     Curve expert 
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
C     100 - 290 C, saturation p, Pitzer et al (1984), Table A 4, Curve 
C     expert Cp4_NaCl 
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
         DOI=10,14 
         VX(I) = 0.D0 
         VP(I) = 0.D0 
      ENDDO 
C     
C     Correction for Psat/bar at T 
C     
      PR = PSAT(TSAVE) 
      P0 = PSAT(TSAVE) 
      VXXX = 1.D-1*(P1 - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1) 
      P0=1.D0 
      VXXC = 1.D-1*(PR - P0)*VXP(PR,TSAVE,VX,VP,PR,P0)/ 
     $     (8.31451D0*TSAVE) 
      G0NACL = HSCPXX2(P1,T1,HXX,SXX,CPXX1,CPXX2,TR2) + VXXX + VXXC 
      ENDIF 
      RETURN 
      END 
      
      

      DOUBLE PRECISION FUNCTION BETA0NACL(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      P1=PXX 
      P2 = P1*P1 
      P3 = P1*P2 
      T1=TXX 
      T2 = T1*T1 
      TA=T1-2.27D2 
      TB=6.8D2-T1 
      BETA0NACL1 = -656.81518D0/T1 + 24.869130D0 + 5.3812753D-5*P1 
     $     -5.5887470D-8*P2 + 6.5893263D-12*P3 - 4.4640952D0*LOG(T1) 
     $     + (1.1109914D-2 - 2.6573399D-7*P1 + 1.7460070D-10*P2 
     $     + 1.0462619D-14*P3)*T1 
      BETA0NACL2 = 
     $     + (-5.3070129D-6 + 8.6340233D-10*P1 - 4.1785962D-13*P2)*T2 
     $     + (-1.5793660D0 + 2.2022821D-3*P1 - 1.3105503D-7*P2 
     $     - 6.3813683D-11*P3)/TA 
      BETA0NACL3 = 
     $     + (9.7065780D0 - 2.6860396D-2*P1 + 1.5344744D-5*P2 
     $     - 3.2153983D-9*P3)/TB 
      BETA0NACL = BETA0NACL1 + BETA0NACL2 + BETA0NACL3 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION BETA1NACL(TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      T1=TXX 
      TA=T1-2.27D2 
      BETA1NACL = 119.31966D0/T1 - 0.48309327D0 + 1.4068095D-3*T1 
     $     - 4.2345814D0/TA 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION CPHINACL(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      P1=PXX 
      T1=TXX 
      T2 = T1*T1 
      TA=T1-2.27D2 
      TB=6.8D2-T1 
      CPHINACL = -6.1084589D0/T1 + 0.40217793D0 + 2.2902837D-5*P1 
     $     - 0.75354649D-1*LOG(T1) + (1.5317673D-4 - 9.0550901D-8*P1)*T1 
     $     + (-1.5386008D-8 + 8.6926600D-11*P1)*T2 + (0.35310414D0 
     $     - 4.3314252D-4*P1)/TA + (-9.1871455D-2 + 5.1904777D-4*P1)/TB 
      RETURN 
      END 
      
      

      DOUBLE PRECISION FUNCTION BETA0NACLlow(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      P1=PXX 
      P2 = P1*P1 
      P3 = P1*P2 
      T1=TXX 
      T2 = T1*T1 
      TA=T1-2.27D2 
      TB=6.8D2-T1 
      BETA0NACL1 = -656.81518D0/T1 + 24.8791830D0 - 2.1552731D-5*P1 
     $     + 5.0166855D-8*P2 - 4.4640952D0*LOG(T1) 
     $     + (1.1087099D-2 - 6.4479761D-8*P1 - 2.3234032D-10*P2)*T1 
      BETA0NACL2 = 
     $     + (-5.2194871D-6 + 2.4445210D-10*P1 + 2.8527066D-13*P2)*T2 
     $     + (-1.5696231D0 + 2.2337864D-3*P1 - 6.3933891D-7*P2 
     $     + 4.5270573D-11*P3)/TA 
      BETA0NACL3 = 5.4151933D0/TB 
      BETA0NACLlow = BETA0NACL1 + BETA0NACL2 + BETA0NACL3 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION CPHINACLlow(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      P1=PXX 
      T1=TXX 
      T2 = T1*T1 
      TA=T1-2.27D2 
      TB=6.8D2-T1 
      CPHINACLlow = -6.1084589D0/T1 + 0.40743803D0 - 6.8152430D-6*P1 
     $     - 0.75354649D-1*LOG(T1) + (1.2609014D-4 + 6.2480692D-8*P1)*T1 
     $     + (1.8994373D-8 - 1.0731284D-10*P1)*T2 + (0.32136572D0 
     $     - 2.5382945D-4*P1)/TA 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION G0NAOH(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAOH/ MGC, MVC, MDLGC, MDLVC 
      CHARACTER*65 MDLGC(10), MDLVC(10) 
      DIMENSION CPXX(14),VX(14),VP(14) 
      P1=PXX 
      T1=TXX 
C     
      IF (MVC .EQ. 1) THEN 
C     
C     Simonson et al (1989), Table 4 
C     
         IF(TSAVE .LE. 348.16D0) THEN 
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
            IF(TSAVE .LE. 323.16D0) THEN 
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
               DOI=1,14 
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
         DOI=1,14 
         VP(I) = 0.D0 
      ENDDO 
      ENDIF 
C     
      ELSEIF (MVC .EQ. 2) THEN 
C     
C     Fit to 25 C density data 
C     
         DOI=1,14 
         VX(I) = 0.D0 
         VP(I) = 0.D0 
      ENDDO 
      VX(1) = -5.29060D0 
      
      
C     
      ENDIF 
C     
      IF (MGC .EQ. 1) THEN 
C     
C     p=7MPa,Simonsonetal(1989),Table3 
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
C     
         P0 = PSAT(TSAVE) 
         PR = 70.D0 
         VXXX = 1.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1) 
         P0=1.D0 
         TR = 2.9815D2 
         VXXC = 1.D-1*(PR - P0)*VXP(PR,TR,VX,VP,PR,P0)/(8.31451D0*TR) 
C     
      ELSEIF (MGC .EQ. 2) THEN 
C     
         CPXX(1) = -97.79D0 
         DOI=2,14 
         CPXX(I) = 0.D0 
      ENDDO 
      PR=1.D0 
      P0=1.D0 
      VXXX = 1.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1) 
      VXXC = 0.D0 
C     
      ELSEIF (MGC .EQ. 3) THEN 
C     
         CPXX(1) = -101.44D0 
         DOI=2,14 
         CPXX(I) = 0.D0 
      ENDDO 
      PR=1.D0 
      P0=1.D0 
      VXXX = 1.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1) 
      VXXC = 0.D0 
C     
      ENDIF 
C     
C     CODATA, corr. for pressure 
C     
      HXX = -470355.D0 
      SXX = 47.55D0 
      G0NAOH = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX + VXXC 
      RETURN 
      END 
      
      

      DOUBLE PRECISION FUNCTION BETA0NAOH(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAOH/ MGC, MVC, MDLGC, MDLVC 
      CHARACTER*65 MDLGC(10), MDLVC(10) 
      P1=PXX 
      T1=TXX 
      P0=1.D0 
C     
      IF (MVC .EQ. 1) THEN 
C     
         TC=T1-2.27D2 
         IF (TSAVE .LE. 323.16D0) THEN 
            BETA0K = 1.01784D-6 - 2.20494D-4/TC 
         ELSE 
            BETA0K = 0.D0 
         ENDIF 
         IF (TSAVE .LE. 348.16D0) THEN 
            BETA0V = -3.24287D-5 + 1.83077D-2/TC 
         ELSE 
            TB=6.47D2-T1 
            BETA0V = 5.02915D-3 - 3.68325D-2/TB - 1.03663D-3*LOG(T1) + 
     $           3.82255D-6*T1 
         ENDIF 
C     
      ELSEIF (MVC .EQ. 2) THEN 
C     
C     Fit to 25 C density data 
C     
         BETA0K = 0.D0 
         BETA0V = 1.D1*0.63283594D-01/RTSAVE 
C     
      ENDIF 
C     
      IF (MGC .EQ. 1) THEN 
C     
         PR=7.D1 
         T2 = T1*T1 
         TA=T1-2.1D2 
         TB=6.47D2-T1 
         BETA0 = 12.6677D0 - 389.77D0/T1 - 2.13287*LOG(T1) + 3.42184D-3* 
     $      T1  - 11.661D-7*T2 - 4.16833D0/TA + 1.68567D0/TB 
C     
      ELSEIF (MGC .EQ. 2) THEN 
C     
         TR = 2.9815D2 
         PR=1.D0 
         XG = 0.0864D0 
         XL = 0.5307D-3 
         XJ = -1.41D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGC .EQ. 3) THEN 
C     
         
         
         TR = 2.9815D2 
         PR=1.D0 
         XG = 0.0864D0 
         XL = 0.5307D-3 
         XJ = -1.7744D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ENDIF 
C     
C     Integrate (V(P0) + (P - P0)V')dP from Pr to P ---> 
C     (P-Pr)[V(P0)+{0.5(P+Pr)-P0}V'] 
C     ForMGC=1,P0=0.1MPa, Pr=7MPa -->factor0.1 
C     For MGC = 2, P0 = 0.1 MPa, Pr = 0.1 MPa --> factor 0.1 
C     
      BETA0VK = BETA0V + 1.D-1*(0.5D0*(P1 + PR) - P0)*BETA0K 
      BETA0NAOH = BETA0 + 1.D-1*(P1 - PR)*BETA0VK 
C     
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION BETA1NAOH(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAOH/ MGC, MVC, MDLGC, MDLVC 
      CHARACTER*65 MDLGC(10), MDLVC(10) 
      P1=PXX 
      T1=TXX 
C     
      IF (MVC .EQ. 1) THEN 
C     
         IF(TSAVE .LE. 348.16D0) THEN 
            BETA1V = 3.79568D-3 - 1.04928D-5*T1 
         ELSE 
            BETA1V = 0.D0 
         ENDIF 
C     
      ELSEIF (MVC .EQ. 2) THEN 
C     
C     Fit to 25 C density data 
C     
         BETA1V = 1.D1*0.99006953D-1/RTSAVE 
C     
      ENDIF 
C     
      IF (MGC .EQ. 1) THEN 
C     
         PR=7.D1 
         T2 = T1*T1 
         TA=T1-2.1D2 
         TB=6.47D2-T1 
         BETA1 = -60.547D0 + 2708.27D0/T1 + 9.31351*LOG(T1) 
     $        - 94.2226D-7*T2 - 43.0272D0/TA + 11.961D0/TB 
         
         
C     
      ELSEIF (MGC .EQ. 2) THEN 
C     
         TR = 2.9815D2 
         PR=1.D0 
         XG = 0.253D0 
         XL = 0.7495D-3 
         XJ = -3.76D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGC .EQ. 3) THEN 
C     
         TR = 2.9815D2 
         PR=1.D0 
         XG = 0.253D0 
         XL = 0.7495D-3 
         XJ = -3.6431D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ENDIF 
C     
      BETA1NAOH = BETA1 + 1.D-1*(P1 - PR)*BETA1V 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION BETA2NAOH(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      COMMON /NAOH/ MGC, MVC, MDLGC, MDLVC 
      CHARACTER*65 MDLGC(10), MDLVC(10) 
      T1=TXX 
C     
      IF (MGC .EQ. 1) THEN 
C     
         TB=6.47D2-T1 
         BETA2NAOH = -340.559D0*(-0.19554D0-7.4581D-5*T1 + 4.534D0/T1 + 
     $        3.505D-2*LOG(T1) + 1.D0/TB) 
C     
      ELSE 
C     
         BETA2NAOH = 0.D0 
C     
      ENDIF 
C     
      RETURN 
      END 
      
      

      DOUBLE PRECISION FUNCTION CPHINAOH(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAOH/ MGC, MVC, MDLGC, MDLVC 
      CHARACTER*65 MDLGC(10), MDLVC(10) 
      P1=PXX 
      T1=TXX 
      P0=1.D0 
C     
      IF (MVC .EQ. 1) THEN 
C     
         IF (TSAVE .LE. 323.16D0) THEN 
            CPHIK = 1.90754D-6 - 5.5092D-9*T1 
         ELSE 
            CPHIK = 0.D0 
         ENDIF 
         IF (TSAVE .LE. 348.16D0) THEN 
            CPHIV = -1.16142D-4 + 3.45044D-7*T1 
         ELSE 
            CPHIV = 4.18652D-5 - 1.41605D-7*T1 
         ENDIF 
C     
      ELSEIF (MVC .EQ. 2) THEN 
C     
C     Fit to 25 C density data 
C     
         CPHIK = 0.D0 
         CPHIV = -1.D1*0.43578214D-02/RTSAVE 
C     
      ENDIF 
C     
      IF (MGC .EQ. 1) THEN 
C     
         PR=7.D1 
         T2 = T1*T1 
         TA=T1-2.1D2 
         CPHI = -0.01687D0 + 2.23824D0/T1 + 0.31094D-7*T2 + 1.01063D0/TA 
C     
      ELSEIF (MGC .EQ. 2) THEN 
C     
         TR = 2.9815D2 
         PR=1.D0 
         XG = 0.0044D0 
         XL = -0.1355D-3 
         XJ = 0.134D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGC .EQ. 3) THEN 
C     
         TR = 2.9815D2 
         PR=1.D0 
         XG = 0.0044D0 
         XL = -0.1355D-3 
         XJ = 0.23387D-5 
         
         
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ENDIF 
C     
C     Integrate (V(P0) + (P - P0)V')dP from Pr to P ---> 
C     (P-Pr)[V(P0)+{0.5(P+Pr)-P0}V'] 
C     ForMGC=1,P0=0.1MPa, Pr=7MPa -->factor0.1 
C     For MGC = 2, P0 = 0.1 MPa, Pr = 0.1 MPa --> factor 0.1 
C     
      CPHIVK = CPHIV + 1.D-1*(0.5D0*(P1 + PR) - P0)*CPHIK 
      CPHINAOH = CPHI + 1.D-1*(P1 - PR)*CPHIVK 
C     
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION G0NA2SO4(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NA2SO4/ MGT, MVT, MDLGT, MDLVT 
      CHARACTER*65 MDLGT(10), MDLVT(10) 
      DIMENSION CPXX(14),VX(14),VP(14) 
      P1=PXX 
      T1=TXX 
      TR = 2.9815D2 
C     
C     CODATA with 6USERXX.FOR 
C     HXX = -1.39002D6 
C     SXX = 1.354D2 
C     Pabalan & Pitzer (1988) SXX, HXX adj. with 5USERXX.DAT 
      HXX = -1.389538D6 
      SXX = 1.3812D2 
C     
      IF (MGT .EQ. 5) THEN 
C     
         CPXX(1) = -198.35D0 
         DOI=2,14 
         CPXX(I) = 0.D0 
      ENDDO 
C     
      ELSEIF (MGT .EQ. 4) THEN 
C     
         CPXX(1) = -195.57D0 
         DOI=2,14 
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
C     0-110C,1bar,Pitzeretal(1984),TableA5-->V3_NaCl.dat 
C     Curve expert 
            VX(1) = 131663.93D0 
            VX(2) = -2572975.5D0 
            VX(3) = -25343.032D0 
            
            
            VX(4) = 85.236575D0 
            VX(5) = -0.05137802D0 
            VX(6) = -45.906818D0 
            VX(7) = 2.27D2 
            VX(8) = 185397.18D0 
            VX(9) = 6.47D2 
C     0-110C,1bar,Pitzeretal(1984),TableA7-->B1_NaCl.dat 
C     Curve expert 
            VP(1) = -75330.066D-2 
            VP(2) = 1601231.D-2 
            VP(3) = 14111.469D-2 
            VP(4) = -41.058297D-2 
            VP(5) = 0.019182144D-2 
            VP(6) = 80.641291D-2 
            VP(7) = 2.27D2 
            VP(8) = 34057.316D-2 
            VP(9) = 6.47D2 
            DOI=10,14 
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
         DOI=10,14 
         VX(I) = 0.D0 
         VP(I) = 0.D0 
      ENDDO 
C     
      ENDIF 
C     
C     Twice as for NaCl 
C     
      P0 = PSAT(TSAVE) 
      PR=2.D2 
      VXXX = 2.D-1*(PXX - PR)*VXP(P1,T1,VX,VP,PR,P0)/(8.31451D0*T1) 
      P0=1.D0 
      VXXC = 2.D-1*(PR - P0)*VXP(PR,TR,VX,VP,PR,P0)/(8.31451D0*TR) 
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
         P0=1.D0 
         VXXC = 1.D-1*(PR - P0)*VXP(PR,TSAVE,VX,VP,PR,P0)/ 
     $        (8.31451D0*TSAVE) 
C     
      ELSEIF (MVT .EQ. 3) THEN 
C     
         TX=T1-TR 
         P0=1.D0 
         VXXX = 1.D-1*(P1 - P0)*(11.776D0 + TX*(22.858D-2+TX*(-4.2216D-3 
     $        + 1.7764D-5*TX)))/(8.31451D0*T1) 
         VXXC = 0.D0 
C     
      ENDIF 
C     
      G0NA2SO4 = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX + VXXC 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION BETA0NA2SO4(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      
      
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NA2SO4/ MGT, MVT, MDLGT, MDLVT 
      CHARACTER*65 MDLGT(10), MDLVT(10) 
      DIMENSION Q(7),P(11) 
      P1=PXX 
      T1=TXX 
      T2 = T1*T1 
      TR = 2.9815D2 
      RNA = 2.27D2 
      TA=T1-RNA 
C     
      IF (MGT .EQ. 2 .OR. MGT .EQ. 3) THEN 
C     
         IF (MGT .EQ. 2) THEN 
C     
C     Pabalan & Pitzer: Parameters for Psat only for phi and gamma, not 
C     for derivatives!!! 
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
C     Pabalan & Pitzer: Parameters for 200 bar 
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
         TB=RNB-T1 
         BETA0NA2SO41 = P(1)*T2/6.D0 + P(2)*T1/2.D0 + P(3)*T2*(LOG(T1) - 
     $    5.D0/6.D0)/6.D0 +P(4)*T3/1.2D1+ P(5)*T4/2.D1 + P(6)*(T1/2.D0 + 
     $        3.D0*RNA*RNA/(2.D0*T1) + RNA*TA*LOG(TA)/T1) 
         BETA0NA2SO42 = -P(7)*(T1/2.D0 + 3.D0*RNB*RNB/(2.D0*T1) - RNB*TB 
     $        *LOG(TB)/T1) - P(8)/T1 - P(9)*TR*TR/T1 + P(10) + P(11) 
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
         BETA0NA2SO41 = Q(1) + Q(2)*(TR-TR2/T1) + Q(3)*(T2 + 2.D0*TR3/T1 
     $      - 3.D0*TR2) + Q(4)*(T1 +TR2/T1-2.D0*TR) + Q(5)*(LOG(T1/TR) + 
     $        TR/T1 - 1.D0) 
         BETA0NA2SO42 = Q(6)*(1.D0/(T1 - RNC) + (RNC*T1 - TR2)/(T1*(TR - 
     $       RNC)*(TR - RNC))) + Q(7)*(1.D0/(RND - T1) + (TR2 - RND*T1)/ 
     $        (T1*(RND - TR)*(RND - TR))) 
         BETA0 = BETA0NA2SO41 + BETA0NA2SO42 
C     
      ELSEIF (MGT .EQ. 4) THEN 
C     
         XG = 0.02492D0*0.75D0 
         XL = 2.367D-3 
         XJ = -5.8094D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
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
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ENDIF 
C     
      IF (MVT .EQ. 1) THEN 
C     
C     Monnin 1990 
C     
         BETA0VNA2SO4 = 5.3088D-5 + 4.33707D-6*T1 - 3.262413D-9*T2 - 
     $        2.70953D-4*LOG(T1) + 1.42266D-1/T1 + 4.51986D-3/TA 
         BETA0KNA2SO4 = 2.4D-8 - 1.14D-3*BETA0VNA2SO4 
C     
C     Integrate (V(P0) + (P - P0)V')dP from Pr to P ---> 
C     (P-Pr)[V(P0)+{0.5(P+Pr)-P0}V'] 
C     
C     Monnin, Holmes and Mesmer: PR = PSAT, P0 = 1 
C     
         PR = PSAT(TSAVE) 
         P0=1.D0 
C     
         BETA0VKNA2SO4 = BETA0VNA2SO4 + (0.5D0*(P1+PR)-P0)*BETA0KNA2SO4 
         BETA0NA2SO4 = BETA0 + (P1 - PR)*BETA0VKNA2SO4 
         
         
C     
      ELSEIF (MVT .EQ. 2) THEN 
C     
         P1=PXX 
         P2 = P1*P1 
         P3 = P1*P2 
         PR=2.D2 
         PR2 = PR*PR 
         PR3 = PR*PR2 
         TB=6.8D2-T1 
         BETA0NACL1P = 5.3812753D-5*(P1-PR) 
     $        -5.5887470D-8*(P2-PR2) + 6.5893263D-12*(P3-PR3) 
     $        + (-2.6573399D-7*(P1-PR) + 1.7460070D-10*(P2-PR2) 
     $        + 1.0462619D-14*(P3-PR3))*T1 
         BETA0NACL2P = 
     $        (8.6340233D-10*(P1-PR) - 4.1785962D-13*(P2-PR2))*T2 
     $        + (2.2022821D-3*(P1-PR) - 1.3105503D-7*(P2-PR2) 
     $        - 6.3813683D-11*(P3-PR3))/TA 
         BETA0NACL3P = 
     $        (-2.6860396D-2*(P1-PR) + 1.5344744D-5*(P2-PR2) 
     $        - 3.2153983D-9*(P3-PR3))/TB 
         BETA0NACLP = BETA0NACL1P + BETA0NACL2P + BETA0NACL3P 
         BETA0NA2SO4 = BETA0 + 2.D0*BETA0NACLP 
C     
      ELSEIF (MVT .EQ. 3) THEN 
C     
         TX=T1-TR 
         PR=1.D0 
         BETA0V = 5.3250D-5 + TX*(-10.93D-7 + TX*(55.828D-9 
     $        - 4.6248D-10*TX)) 
         BETA0NA2SO4 = BETA0 + (P1 - PR)*BETA0V 
C     
      ENDIF 
C     
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION BETA1NA2SO4(PXX,TXX,SQSION) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NA2SO4/ MGT, MVT, MDLGT, MDLVT 
      CHARACTER*65 MDLGT(10), MDLVT(10) 
      DIMENSION Q(7), P(11) 
      P1=PXX 
      T1=TXX 
      T2 = T1*T1 
      TR = 2.9815D2 
      RNA = 2.27D2 
      TA=T1-RNA 
C     
      IF (MGT .EQ. 2 .OR. MGT .EQ. 3) THEN 
C     
         IF (MGT .EQ. 2) THEN 
C     
C     Pabalan & Pitzer: Parameters for Psat only for phi and gamma, not 
C     for derivatives!!! 
            
            
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
C     Pabalan & Pitzer: Parameters for 200 bar 
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
         TB=RNB-T1 
         BETA1NA2SO41 = P(1)*T2/6.D0 + P(2)*T1/2.D0 + P(3)*T2*(LOG(T1) - 
     $        5.D0/6.D0)/6.D0 + P(4)*T3/1.2D1 + P(5)*T4/2.D1 + P(6)*(T1/2.D0 + 
     $        3.D0*RNA*RNA/(2.D0*T1) + RNA*TA*LOG(TA)/T1) 
         BETA1NA2SO42 = -P(7)*(T1/2.D0 + 3.D0*RNB*RNB/(2.D0*T1) - RNB*TB 
     $        *LOG(TB)/T1) - P(8)/T1 - P(9)*TR*TR/T1 + P(10) + P(11) 
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
         P1=PXX 
         T1=TXX 
         T2 = T1*T1 
         TR = 2.9815D2 
         TR2 = TR*TR 
         TR3 = TR2*TR 
         
         
         TA=T1-RNA 
         BETA1NA2SO41=Q(1)+Q(2)*(TR - TR2/T1) + Q(3)*(T2 + 2.D0*TR3/T1 
     $        - 3.D0*TR2)+Q(4)*(T1+TR2/T1-2.D0*TR)+Q(5)*(LOG(T1/TR) + 
     $        TR/T1 - 1.D0) 
         BETA1NA2SO42 = Q(6)*(1.D0/(T1 - RNC) + (RNC*T1 - TR2)/(T1*(TR - 
     $        RNC)*(TR - RNC))) + Q(7)*(1.D0/(RND- T1) + (TR2 - RND*T1)/ 
     $        (T1*(RND - TR)*(RND - TR))) 
         BETA1 = BETA1NA2SO41 + BETA1NA2SO42 
C     
      ELSEIF (MGT .EQ. 4) THEN 
C     
         XG = 1.466D0*0.75D0 
         XL = 5.63D-3 
         XJ = -5.1549D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
C     ChangeofBETA1fromalpha=2toalpha=1.4 
      GF = 1.4D0*SQSION 
      GFE = 2.D0*EXP(-GF) 
      GX1 = (2.D0 - GF*GFE - GFE)/(GF*GF) 
      GF = 2.D0*SQSION 
      GFE = 2.D0*EXP(-GF) 
      GX2 = (2.D0 - GF*GFE - GFE)/(GF*GF) 
      IF (GX1 .EQ. 0.D0) THEN 
         CONFAC = 1.D0 
      ELSE 
         CONFAC = GX2/GX1 
      ENDIF 
      BETA1 = CONFAC*XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGT .EQ. 5) THEN 
C     
         XG = 1.466D0*0.75D0 
         XL = 5.63D-3 
         XJ = -12.063D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
C     ChangeofBETA1fromalpha=2toalpha=1.4 
      GF = 1.4D0*SQSION 
      GFE = 2.D0*EXP(-GF) 
      GX1 = (2.D0 - GF*GFE - GFE)/(GF*GF) 
      GF = 2.D0*SQSION 
      GFE = 2.D0*EXP(-GF) 
      GX2 = (2.D0 - GF*GFE - GFE)/(GF*GF) 
      IF (GX1 .EQ. 0.D0) THEN 
         CONFAC = 1.D0 
      ELSE 
         CONFAC = GX2/GX1 
      ENDIF 
      BETA1 = CONFAC*XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ENDIF 
C     
      IF (MVT .EQ. 1) THEN 
C     
C     Monnin 1990 
C     
         PR = PSAT(TSAVE) 
         
         
         BETA1VNA2SO4 = 3.200188D-3 - 1.092875D-6*T1 - 7.101661D-9*T2 - 
     $        8.373935D-1/T1 + 4.901041D-2/TA 
C     Change of BETA1VNA2SO4 from alpha = 2 to alpha = 1.4 
         GF = 1.4D0*SQSION 
         GFE = 2.D0*EXP(-GF) 
         GX1 = (2.D0 - GF*GFE - GFE)/(GF*GF) 
         GF = 2.D0*SQSION 
         GFE = 2.D0*EXP(-GF) 
         GX2 = (2.D0 - GF*GFE - GFE)/(GF*GF) 
         IF (GX1 .EQ. 0.D0) THEN 
            CONFAC = 1.D0 
         ELSE 
            CONFAC = GX2/GX1 
         ENDIF 
         BETA1NA2SO4 = BETA1 + CONFAC*(P1 - PR)*BETA1VNA2SO4 
C     
      ELSEIF (MVT .EQ. 2) THEN 
C     
         BETA1NA2SO4 = BETA1 
C     
      ELSEIF (MVT .EQ. 3) THEN 
C     
         TX=T1-TR 
         PR=1.D0 
         BETA1V = 12.9320D-5 + TX*(5.1406D-6 + TX*(-1.3311D-7 
     $        + 0.1552D-8*TX)) 
C     Change of BETA1VNA2SO4 from alpha = 2 to alpha = 1.4 
         GF = 1.4D0*SQSION 
         GFE = 2.D0*EXP(-GF) 
         GX1 = (2.D0 - GF*GFE - GFE)/(GF*GF) 
         GF = 2.D0*SQSION 
         GFE = 2.D0*EXP(-GF) 
         GX2 = (2.D0 - GF*GFE - GFE)/(GF*GF) 
         IF (GX1 .EQ. 0.D0) THEN 
            CONFAC = 1.D0 
         ELSE 
            CONFAC = GX2/GX1 
         ENDIF 
         BETA1NA2SO4 = BETA1 + CONFAC*(P1 - PR)*BETA1V 
C     
      ENDIF 
C     
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION CPHINA2SO4(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NA2SO4/ MGT, MVT, MDLGT, MDLVT 
      CHARACTER*65 MDLGT(10), MDLVT(10) 
      DIMENSION Q(7), P(11) 
      P1=PXX 
      T1=TXX 
      T2 = T1*T1 
      TR = 2.9815D2 
      RNA = 2.27D2 
      
      
      TA=T1-RNA 
C     
      IF (MGT .EQ. 2 .OR. MGT .EQ. 3) THEN 
C     
         IF (MGT .EQ. 2) THEN 
C     
C     Pabalan & Pitzer: Parameters for Psat only for phi and gamma, not 
C     for derivatives!!! 
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
C     Pabalan & Pitzer: Parameters for 200 bar 
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
         TB=RNB-T1 
         CPHINA2SO41 = (P(1)*T2/6.D0 + P(2)*T1/2.D0 + P(3)*T2*(LOG(T1) - 
     $    5.D0/6.D0)/6.D0 + P(4)*T3/1.2D1 +P(5)*T4/2.D1+ P(6)*(T1/2.D0 + 
     $        3.D0*RNA*RNA/(2.D0*T1) + RNA*TA*LOG(TA)/T1))*(2.D0**1.5D0) 
         CPHINA2SO42 = (-P(7)*(T1/2.D0 + 3.D0*RNB*RNB/(2.D0*T1) - RNB*TB 
     $   *LOG(TB)/T1) - P(8)/T1-P(9)*TR*TR/T1+P(10)+P(11))*(2.D0**1.5D0) 
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
         CPHINA2SO41 = Q(1)+ Q(2)*(TR - TR2/T1) + Q(3)*(T2 + 2.D0*TR3/T1 
     $    - 3.D0*TR2)+ Q(4)*(T1 + TR2/T1 - 2.D0*TR) + Q(5)*(LOG(T1/TR) + 
     $        TR/T1 - 1.D0) 
         CPHINA2SO42 = Q(6)*(1.D0/(T1 - RNC) + (RNC*T1 - TR2)/(T1*(TR - 
     $      RNC)*(TR - RNC))) + Q(7)*(1.D0/(RND - T1) + (TR2 - RND*T1)/ 
     $        (T1*(RND - TR)*(RND - TR))) 
         CPHI = CPHINA2SO41 + CPHINA2SO42 
C     
      ELSEIF (MGT .EQ. 4) THEN 
C     
         XG = 0.010463D0/(2.D0**2.5D0/3.D0) 
         XL = -1.725D-4*2.D0**1.5D0 
         XJ = 70.981D-7*2.D0**1.5D0 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
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
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ENDIF 
C     
      IF (MVT .EQ. 1) THEN 
C     
C     Monnin 1990 
C     
         PR = PSAT(TSAVE) 
         BETA0VNA2SO4 = 5.3088D-5 + 4.33707D-6*T1 - 3.262413D-9*T2 - 
     $        2.70953D-4*LOG(T1) + 1.42266D-1/T1 + 4.51986D-3/TA 
         CPHINA2SO4 = CPHI + (2.D0**1.5D0)* 
     $        (P1 - PR)*(1.752D-6 - 8.55D-2*BETA0VNA2SO4) 
C     
      ELSEIF (MVT .EQ. 2) THEN 
C     
         P1=PXX 
         PR=2.D2 
         TB=6.8D2-T1 
         CPHINACLP = 2.2902837D-5*(P1-PR) - 9.0550901D-8*(P1-PR)*T1 
     $        + 8.6926600D-11*(P1-PR)*T2 - 4.3314252D-4*(P1-PR)/TA 
     $        + 5.1904777D-4*(P1-PR)/TB 
         CPHINA2SO4 = CPHI + 2.D0*CPHINACLP*(2.D0**0.5D0) 
C     
      ELSEIF (MVT .EQ. 3) THEN 
C     
         TX=T1-TR 
         PR=1.D0 
         CPHIV=2.D0**1.5D0*(-2.9140D-6 + TX*(10.798D-8 + TX*(-9.5282D-9 
     $        + 0.7375D-10*TX))) 
         CPHINA2SO4 = CPHI + (P1 - PR)*CPHIV 
         
         
C     
      ENDIF 
C     
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION G0NAHCO3(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAHCO3/ MGN, MVN, MDLGN, MDLVN 
      CHARACTER*65 MDLGN(10), MDLVN(10) 
      DIMENSION CPXX(14) 
      P1=PXX 
      T1=TXX 
C     
C     CODATA 
C     
      HXX = -9.3027D5 
      SXX = 1.5685D2 
      CPXX(1) = -1.122D1 
      DOI=2,14 
      CPXX(I) = 0.D0 
      ENDDO 
C     
C     Krumgalz et al. (2000) 
C     
      P0=1.D0 
      TA = T1 - 2.9815D2 
      VXXX = 1.D-1*(P1 - P0)*(23.181D0 + TA*(0.11309D0 + TA*(-5.5413D-3 
     $     + TA*2.0591D-5)))/(8.31451D0*T1) 
C     
      G0NAHCO3 = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION BETA0NAHCO3(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAHCO3/ MGN, MVN, MDLGN, MDLVN 
      CHARACTER*65 MDLGN(10), MDLVN(10) 
      P1=PXX 
      T1=TXX 
      P0=1.D0 
      TR = 2.9815D2 
      TA=T1-TR 
C     
      IF (MGN .EQ. 1) THEN 
C     
         XG = 0.028D0 
         XL = 1.D-3 
         XJ = -1.929D-5 
         XCP(1) = -TR*TR*XJ 
         
         
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGN .EQ. 2) THEN 
C     
         TB=T1-2.1D2 
         TC=6.47D2-T1 
         TD=T1-4.43D2 
         U0 = 0.661D-1 
         U1=0.D0 
         U2=0.D0 
         U3=0.D0 
         U4=0.D0 
         U5 = 3.75951D-8 
         BETA0=U0+U1*T1+U2/T1+U3/TB+U4/TC+U5*TD*TD*TD/3.D0 
C     
      ENDIF 
C     
      IF (MVN .EQ. 1) THEN 
C     
         BETA0V = -1.162D-5 + TA*(-28.646D-7 + 512.84D-9*TA) 
C     
      ELSEIF (MVN .EQ. 2) THEN 
C     
         U6=0.D0 
         U7=0.D0 
         BETA0V = U6 + U7*(P1 - P0)/2.D0 
C     
      ENDIF 
C     
      BETA0NAHCO3 = BETA0 + (P1 - P0)*BETA0V 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION BETA1NAHCO3(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAHCO3/ MGN, MVN, MDLGN, MDLVN 
      CHARACTER*65 MDLGN(10), MDLVN(10) 
      P1=PXX 
      T1=TXX 
      P0=1.D0 
      TR = 2.9815D2 
      TA=T1-TR 
C     
      IF (MGN .EQ. 1) THEN 
C     
         XG = 0.044D0 
         XL = 1.1D-3 
         XJ = -3.562D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      
      
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGN .EQ. 2) THEN 
C     
         TB=T1-2.1D2 
         TC=6.47D2-T1 
         TD=T1-4.43D2 
         U0 = -41.16D-1 
         U1 = 63.09D-4 
         U2 = 924.D0 
         U3 = -52.02D0 
         U4 = -80.26D0 
         U5=0.D0 
         BETA1=U0+U1*T1+U2/T1+U3/TB+U4/TC+U5*TD*TD*TD/3.D0 
C     
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
      ENDIF 
C     
      BETA1NAHCO3 = BETA1 + (P1 - P0)*BETA1V 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION CPHINAHCO3(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAHCO3/ MGN, MVN, MDLGN, MDLVN 
      CHARACTER*65 MDLGN(10), MDLVN(10) 
      P1=PXX 
      T1=TXX 
      P0=1.D0 
      TR = 2.9815D2 
      TA=T1-TR 
C     
      IF (MGN .EQ. 1) THEN 
C     
         XG=0.D0 
         XL=0.D0 
         XCP(1) = 0.D0 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGN .EQ. 2) THEN 
C     
         
         
         TB=T1-2.1D2 
         TC=6.47D2-T1 
         TD=T1-4.43D2 
         U0=0.D0 
         U1=0.D0 
         U2=0.D0 
         U3=0.D0 
         U4=0.D0 
         U5=0.D0 
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
         U6=0.D0 
         U7=0.D0 
         CPHIV = U6 + U7*(P1 - P0)/2.D0 
C     
      ENDIF 
C     
      CPHINAHCO3 = CPHI + (P1 - P0)*CPHIV 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION G0NA2CO3(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NA2CO3/ MGS, MVS, MDLGS, MDLVS 
      CHARACTER*65 MDLGS(10), MDLVS(10) 
      DIMENSION CPXX(14) 
      P1=PXX 
      T1=TXX 
C     
C     CODATA 
C     
      HXX = -1.15591D6 
      SXX = 6.69D1 
C     
      IF (MGS .EQ. 1 .OR. MGS .EQ. 4) THEN 
C     
         CPXX(1) = -2.2848D2 
         DOI=2,14 
         CPXX(I) = 0.D0 
      ENDDO 
C     
      ELSEIF (MGS .EQ. 2 .OR. MGS .EQ. 3) THEN 
C     
C     T dependence as Na2SO4 (Holmes and Mesmer, 1986), CPXX(1) adj. acc. 
C     to model 1 
C     
         CPXX(1) = -1235.173D0 
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
      ELSEIF (MGS .EQ. 5) THEN 
C     
         CPXX(1) = -185.96D0 
         DOI=2,14 
         CPXX(I) = 0.D0 
      ENDDO 
C     
      ENDIF 
C     
C     Krumgalz et al. (2000) 
C     
C     P0=1.D0 
C     TA=T1-2.9815D2 
C     V0 = (-6.48D0 + TA*(0.25109D0 + TA*(-6.8782D-3 + TA*5.4186D-5))) 
C     VXXX = 1.D-1*(P1 - P0)*V0/(8.31451D0*T1) 
C     
C     Krumgalz et al. (2000) V0 data (Table 3) fitted to Holmes & Mesmer 
C     equation 
C     
      P0=1.D0 
      TA=T1-2.63D2 
      V0 = -228.5329D0 + 1.4600145D0*T1 - 0.0023306034D0*T1*T1 
     $     - 211.56993D0/TA 
      VXXX = 1.D-1*(P1 - P0)*V0/(8.31451D0*T1) 
      G0NA2CO3 = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION BETA0NA2CO3(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NA2CO3/ MGS, MVS, MDLGS, MDLVS 
      CHARACTER*65 MDLGS(10), MDLVS(10) 
      P1=PXX 
      T1=TXX 
      P0=1.D0 
      TR = 2.9815D2 
      TA=T1-TR 
C     
      IF (MGS .EQ. 1) THEN 
C     
         XG = 0.0362D0 
         XL = 2.126891305D-3 
         
         
         XJ = -3.019262D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGS .EQ. 2) THEN 
C     
         TB=T1-2.1D2 
         TC=6.47D2-T1 
         TD=T1-4.43D2 
         U0 = -15.837178D0 
         U1 = -5.991D-4 
         U2 = 771.329688D0 
         U3 = -25.81D0 
         U4 = -2.659D0 
         U5=0.D0 
         U8 = 2.415995D0 
         BETA0=U0+U1*T1+U2/T1+U3/TB+U4/TC+U5*TD*TD*TD/3.D0 
     $        + U8*LOG(T1) 
C     
      ELSEIF (MGS .EQ. 3) THEN 
C     
         TB=T1-2.1D2 
         TC=6.47D2-T1 
         TD=T1-4.43D2 
         U0 = 5.153D-1 
         U1 = -5.991D-4 
         U2=0.D0 
         U3 = -25.81D0 
         U4 = -2.659D0 
         U5=0.D0 
         BETA0=U0+U1*T1+U2/T1+U3/TB+U4/TC+U5*TD*TD*TD/3.D0 
C     
      ELSEIF (MGS .EQ. 4) THEN 
C     
         XG = 0.0362D0 
         XL = 2.126891305D-3 
         XJ = -3.019262D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
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
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      BETA0 = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ENDIF 
C     
      IF (MVS .EQ. 1) THEN 
C     
         BETA0V = 5.98D-5 + TA*(-6.9018D-7 + TA*2.112D-9) 
         
         
C     
      ELSEIF (MVS .EQ. 2) THEN 
C     
         U6 = 8.75D-5 
         U7 = -2.66D-8 
         BETA0V = U6 + U7*(P1 - P0)/2.D0 
C     
      ENDIF 
C     
      BETA0NA2CO3 = BETA0 + (P1 - P0)*BETA0V 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION BETA1NA2CO3(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NA2CO3/ MGS, MVS, MDLGS, MDLVS 
      CHARACTER*65 MDLGS(10), MDLVS(10) 
      P1=PXX 
      T1=TXX 
      P0=1.D0 
      TR = 2.9815D2 
      TA=T1-TR 
C     
      IF (MGS .EQ. 1) THEN 
C     
         XG = 1.51D0 
         XL = 5.0592226326D-3 
         XJ = -15.42485D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGS .EQ. 2) THEN 
C     
         TB=T1-2.1D2 
         TC=6.47D2-T1 
         TD=T1-4.43D2 
         U0 = 77.074917D0 
         U1 = -43.03D-4 
         U2 = -3575.534705D0 
         U3 = -25.45D0 
         U4 = 361.8D0 
         U5=0.D0 
         U8 = -11.063961D0 
         BETA1=U0+U1*T1+U2/T1+U3/TB+U4/TC+U5*TD*TD*TD/3.D0 
     $        + U8*LOG(T1) 
C     
      ELSEIF (MGS .EQ. 3) THEN 
C     
         TB=T1-2.1D2 
         TC=6.47D2-T1 
         TD=T1-4.43D2 
         U0 = 20.44D-1 
         
         
         U1 = -43.03D-4 
         U2=0.D0 
         U3 = -25.45D0 
         U4 = 361.8D0 
         U5=0.D0 
         BETA1=U0+U1*T1+U2/T1+U3/TB+U4/TC+U5*TD*TD*TD/3.D0 
C     
      ELSEIF (MGS .EQ. 4) THEN 
C     
         XG = 1.51D0 
         XL = 5.0592226326D-3 
         XJ = -15.42485D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
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
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      BETA1 = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ENDIF 
C     
      IF (MVS .EQ. 1) THEN 
C     
         BETA1V = 8.16D-5 + TA*(-7.4232D-6 + TA*1.5774D-7) 
C     
      ELSEIF (MVS .EQ. 2) THEN 
C     
         BETA1V = 0.D0 
C     
      ENDIF 
C     
      BETA1NA2CO3 = BETA1 + (P1 - P0)*BETA1V 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION CPHINA2CO3(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NA2CO3/ MGS, MVS, MDLGS, MDLVS 
      CHARACTER*65 MDLGS(10), MDLVS(10) 
      P1=PXX 
      T1=TXX 
      P0=1.D0 
      TR = 2.9815D2 
      TA=T1-TR 
C     
      
      
      IF (MGS .EQ. 1) THEN 
C     
         XG = 0.0052D0 
         XL = -0.37649173885D-3 
         XJ = 0.46D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGS .EQ. 2) THEN 
C     
         TB=T1-2.1D2 
         TC=6.47D2-T1 
         TD=T1-4.43D2 
         U0 = 5.7132704D0 
         U1=0.D0 
         U2 = -288.011245D0 
         U3 = 6.482D0 
         U4 = 8.048D0 
         U5=0.D0 
         U8 = -0.8492493D0 
         CPHI = U0 + U1*T1 + U2/T1 + U3/TB + U4/TC + U5*TD*TD*TD/3.D0 
     $        + U8*LOG(T1) 
C     
      ELSEIF (MGS .EQ. 3) THEN 
C     
         TB=T1-2.1D2 
         TC=6.47D2-T1 
         TD=T1-4.43D2 
         U0 = -0.914D-1 
         U1=0.D0 
         U2=0.D0 
         U3 = 6.482D0 
         U4 = 8.048D0 
         U5=0.D0 
         CPHI = U0 + U1*T1 + U2/T1 + U3/TB + U4/TC + U5*TD*TD*TD/3.D0 
C     
      ELSEIF (MGS .EQ. 4) THEN 
C     
         XG = 0.0052D0 
         XL = -0.37649173885D-3 
         XJ = 0.35D-5 
         XCP(1) = -TR*TR*XJ 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGS .EQ. 5) THEN 
C     
         XG = 0.0052D0 
         XL=0.D0 
         XCP(1) = 0.D0 
         DOI=2,17 
         XCP(I) = 0.D0 
      ENDDO 
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ENDIF 
C     
      
      
      IF (MVS .EQ. 1) THEN 
C     
         CPHIV = 2.D0**1.5D0*(-3.25D-6 + TA*(1.6502D-8 +TA*0.6157D-9)) 
C     
      ELSEIF (MVS .EQ. 2) THEN 
C     
         U6 = -2.89D-5 
         U7=0.D0 
         CPHIV = U6 + U7*(P1 - P0)/2.D0 
C     
      ENDIF 
C     
      CPHINA2CO3 = CPHI + (P1 - P0)*CPHIV 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION G0NAALOH4(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAALOH4/ MGA, MVA, MDLGA, MDLVA 
      CHARACTER*65 MDLGA(10), MDLVA(10) 
      DIMENSION CPXX(14) 
      P1=PXX 
      T1=TXX 
C     
      IF (MGA .EQ. 1 .OR. MGA .EQ. 3) THEN 
C     
C     Wesolowski (1992) 
C     
         HXX = -1.740985D6 
         SXX = 1.6969D2 
C     
C     Zeng et. al. (1994), SXX est. 
C     
C     HXX = -1.74395D6 
C     SXX = 1.61D2 
C     
C     Chen et. al. (1991) 
C     
C     HXX = -1.74311D6 
C     SXX = 1.5994D2 
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
C     DOI=2,14 
C     CPXX(I) = 0.D0 
C     ENDDO 
C     DOI=2,14 
      
      
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
      CPXX(1) = 636440.96D0 
      CPXX(2) = -13877718.D0 
      CPXX(3) = -118389.62D0 
      CPXX(4) = 331.35745D0 
      CPXX(5) = -0.15361365D0 
      CPXX(6) = -20677.96D0 
      CPXX(7) = 2.27D2 
      CPXX(8) = -23053.602D0 
      CPXX(9) = 6.47D2 
      CPXX(10) = 0.D0 
      CPXX(11) = 0.D0 
      CPXX(12) = 0.D0 
      CPXX(13) = 0.D0 
      CPXX(14) = 0.D0 
C     
      P0=1.D0 
C     
C     Hovey et al. (1988) + 200 C value estimated from Pokrovskii & 
C     Helgeson (1995) 
C     
      V0 = -7.542734D0 + T1*(0.35765143D0 - 0.00056047206D0*T1) - 
     $     148.47227D0/(T1 - 263.D0) 
      VXXX = 1.D-1*(P1 - P0)*V0/(8.31451D0*T1) 
C     
      G0NAALOH4 = HSCPXX(P1,T1,HXX,SXX,CPXX) + VXXX 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION BETA0NAALOH4(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAALOH4/ MGA, MVA, MDLGA, MDLVA 
      CHARACTER*65 MDLGA(10), MDLVA(10) 
      P1=PXX 
      T1=TXX 
      P0=1.D0 
C     
      IF (MGA .EQ. 1) THEN 
         BETA0 = BETA0NAOH(P1,T1) - 0.0356D0 
C     
      ELSEIF (MGA .EQ. 2) THEN 
         TR = 3.7215D2 
         XG = BETA0NAOH(P0,TR) - 0.0356D0 
         XL = 1.76315D-3 
         DOI=1,17 
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
      BETA0V = 0.D0 
      BETA0NAALOH4 = BETA0 + (P1 - P0)*BETA0V 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION BETA1NAALOH4(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAALOH4/ MGA, MVA, MDLGA, MDLVA 
      CHARACTER*65 MDLGA(10), MDLVA(10) 
      P1=PXX 
      T1=TXX 
      P0=1.D0 
C     
      IF (MGA .EQ. 1) THEN 
         BETA1 = BETA1NAOH(P1,T1) 
C     
      ELSEIF (MGA .EQ. 2) THEN 
         TR = 3.7215D2 
         XG = BETA1NAOH(P0,TR) 
         XL = -5.22260D-3 
         DOI=1,17 
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
      BETA1V = 0.D0 
      BETA1NAALOH4 = BETA1 + (P1 - P0)*BETA1V 
      RETURN 
      END 

      DOUBLE PRECISION FUNCTION CPHINAALOH4(PXX,TXX) 
      IMPLICIT DOUBLE PRECISION(A-H,O-Z) 
      DIMENSION XCP(17) 
      
      
      COMMON /CSAV/ PSAVE, RTSAVE, TSAVE 
      COMMON /NAALOH4/ MGA, MVA, MDLGA, MDLVA 
      CHARACTER*65 MDLGA(10), MDLVA(10) 
      P1=PXX 
      T1=TXX 
      P0=1.D0 
C     
      IF (MGA .EQ. 1) THEN 
         CPHI = CPHINAOH(P1,T1) - 0.00526D0 
C     
      ELSEIF (MGA .EQ. 2) THEN 
         TR = 3.7215D2 
         XG = CPHINAOH(P0,TR) - 0.00526D0 
         XL = -1.08641D-3 
         DOI=1,17 
         XCP(I) = 0.D0 
      ENDDO 
      XCP(5) = 4.97929D-4 
      XCP(15) = -2.41445D-6 
      XCP(16) = 2.94923D-9 
      CPHI = XHGCP(P1,T1,XG,XL,XCP,TR) 
C     
      ELSEIF (MGA .EQ. 3) THEN 
         CPHI = CPHINAOH(P1,T1) + 0.00537D0 
C     
      ENDIF 
      CPHIV = 0.D0 
      CPHINAALOH4 = CPHI + (P1 - P0)*CPHIV 
      RETURN 
      END 
      
      
