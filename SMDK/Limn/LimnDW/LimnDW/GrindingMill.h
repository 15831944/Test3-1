//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __GRINDINGMILL_H
#define  __GRINDINGMILL_H

//---------------------------------------------------------------------------

class CGrindingMill : public MBaseMethod
  {
  public:
    CGrindingMill(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
    virtual void    EvalProducts();
    virtual bool    GetModelAction(CMdlActionArray & Acts);
    virtual bool    SetModelAction(CMdlAction & Act);
    virtual bool    GetModelGraphic(CMdlGraphicArray & Grfs);
    virtual bool    OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf);

  protected:

    C_ModelParameters_DiamondWizard_GrindingMill m_DWParms;

    bool          m_LBtnDn;
    bool          m_RBtnDn;
    CPoint        m_MousePt;
  };



#endif