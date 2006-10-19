//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#ifndef  __DUALEFFICIENCYCURVE_H
#define  __DUALEFFICIENCYCURVE_H

//---------------------------------------------------------------------------

class CDualEfficiencyCurve : public MBaseMethod
  {
  public:
    CDualEfficiencyCurve(MUnitDefBase * pUnitDef, TaggedObject * pNd);
    virtual void    Init();
    virtual void    BuildDataFields();
    virtual bool    ExchangeDataFields();
    virtual void    EvalProducts();
    virtual bool    GetModelAction(CMdlActionArray & Acts);
    virtual bool    SetModelAction(CMdlAction & Act);
    virtual bool    GetModelGraphic(CMdlGraphicArray & Grfs);
    virtual bool    OperateModelGraphic(CMdlGraphicWnd & Wnd, CMdlGraphic & Grf);

  protected:

    C_ModelParameters_DiamondWizard_DualEfficiencyCurve m_DWParms;

    bool          m_LBtnDn;
    bool          m_RBtnDn;
    CPoint        m_MousePt;
  };



#endif