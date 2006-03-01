
#ifndef  __OREPROPS_H
#define  __OREPROPS_H

//---------------------------------------------------------------------------

class RT_OreProp
  {
  friend class RT_OreProps;
  friend class OrePropDataBase;
  public:
    RT_OreProp()
      { bOK = false; 
        sMaterial[0]=0; sDescription[0]=0;// sGroup[0]=0;
        //dLump=dDensity=dDI=dK1C=dA=db=dAb=dRockA=dUTS=dCritUTS=dPPV=db80=dUCS=0.0;
        pOtherProps=NULL;
      };
    ~RT_OreProp()
      { delete []pOtherProps; };

  protected:
    bool          bOK; //valid ore property

    char          sMaterial[32];
    char          sDescription[64];
    /*char          sGroup[32]   ;
    double        dLump        ;
    double        dDensity     ;
    double        dDI          ;
    double        dK1C         ;
    double        dA           ;
    double        db           ;
    double        dAb          ;
    double        dRockA       ;
    double        dUTS         ;
    double        dCritUTS     ;
    double        dPPV         ;
    double        db80         ;
    double        dUCS         ;*/
    double*       pOtherProps;
     
    void SetOtherPropsCount(int Count)
      {
      if (pOtherProps)
        delete []pOtherProps;
      if (Count>0)
        pOtherProps = new double[Count];
      else
        pOtherProps=NULL;
      }

  public:
    bool          OK() { return bOK; };
    char*         Material    () { return sMaterial    ; };
    char*         Description () { return sDescription ; };
/*    char*         Group       () { return sGroup       ; };
    double        Lump        () { return dLump        ; };
    double        Density     () { return dDensity     ; };
    double        DI          () { return dDI          ; };
    double        K1C         () { return dK1C         ; };
    double        A           () { return dA           ; };
    double        b           () { return db           ; };
    double        Ab          () { return dAb          ; };
    double        RockA       () { return dRockA       ; };
    double        UTS         () { return dUTS         ; };
    double        CritUTS     () { return dCritUTS     ; };
    double        PPV         () { return dPPV         ; };
    double        b80         () { return db80         ; };
    double        UCS         () { return dUCS         ; };*/
    
    double        OtherProp(int index) { return pOtherProps[index]; };

    //calculated...
    //double        T10(double Ecs) { return dA * (1.0 - exp(-1.0*db*Ecs)); };
  };

typedef RT_OreProp* pRT_OreProp;


class RT_OtherProp
  {
  friend class RT_OreProps;
  public:
    RT_OtherProp()
      { sPropName[0]=0; };
  protected:
    char          sPropName[50];
  public:
    char*         PropName() { return sPropName; };
  };

typedef RT_OtherProp* pRT_OtherProp;

//---------------------------------------------------------------------------

class RT_OreProps
  {
  friend class OrePropDataBase;
  public:
    RT_OreProps() 
      { iCount=0; iOtherCount=0; pOreProps=NULL; pOtherPropNames=NULL; };
    ~RT_OreProps() 
      { delete []pOreProps; delete []pOtherPropNames; };
    
    int Count() { return iCount; };
    RT_OreProp* GetOreProps() { return pOreProps; };
    RT_OreProp & OrePropByIndex(int index)
      { return pOreProps[index]; };
    int FindByMaterialName(char* pName)
      { for (int i=0; i<iCount; i++)
          if (stricmp(pOreProps[i].sMaterial, pName)==0)
            return i; 
        return -1;
      };

    int OtherPropCount() { return iOtherCount; };
    int FindOtherProp(char* pName)
      { for (int i=0; i<iOtherCount; i++)
          if (stricmp(pOtherPropNames[i].PropName(), pName)==0)
            return i; 
        return -1;
      };
  char* OtherPropName(int index)
    { return pOtherPropNames[index].PropName(); };

  protected:
    int iOtherCount;
    int iCount;
    RT_OtherProp* pOtherPropNames;
    RT_OreProp* pOreProps;

    void SetOtherPropsCount(int Cnt)
      { delete []pOtherPropNames;
        pOtherPropNames = NULL;
        iOtherCount = Cnt;
        if (iOtherCount>0)
          pOtherPropNames = new RT_OtherProp[iOtherCount];
        for (int i=0; i<iCount; i++)
          pOreProps[i].SetOtherPropsCount(iOtherCount);
      };
    void SetOtherPropName(int index, char* pName)
      {
      strcpy(pOtherPropNames[index].sPropName, pName);
      };
    void SetOrePropsCount(int Cnt)
      { delete []pOreProps;
        iCount = Cnt;
        pOreProps = new RT_OreProp[iCount];
      };
  };

//---------------------------------------------------------------------------

#endif
