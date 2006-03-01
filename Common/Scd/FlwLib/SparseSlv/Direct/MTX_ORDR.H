//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MTX_ORDR_H
#define  __MTX_ORDR_H

 
_FWDDEF (SpM_Node); //  *pSpM_Node;
 
class SpM_Node
  {
  DEFINE_SPARES(SpM_Node)
  public:
    //DBG_NEW(SPM_NODE)
    dword           value;
    pSpM_Node      next, prev;
  };
 
//typedef pSpM_Node dynamicarray;              /* Size dynamically allocated          */
typedef CArray <SpM_Node*, SpM_Node*> dynamicarray;              /* Size dynamically allocated          */

class SpM_Ordr 
  {
  DEFINE_SPARES(SpM_Ordr)
  public:
    //DBG_NEW(SPM_ORDER)

    SpM_Ordr(dword n);
    ~SpM_Ordr();
    void Clear(dword n);
    void rowinsert (dword rownum, dword numnonzeros);
    void colinsert (dword colnum, dword numnonzeros);
    void getfirstrow (dword *rownum, dword numnonzeros, flag *exists);
    void getfirstcol (dword *colnum, dword numnonzeros, flag *exists);
    void getnextrow (dword *rownum, flag *exists);
    void getnextcol (dword *colnum, flag *exists);
    void rowchangeordering (dword rownum, dword oldnonzeros, dword newnonzeros);
    void colchangeordering (dword colnum, dword oldnonzeros, dword newnonzeros);
    void rowremove (dword rownum, dword numnonzeros);
    void colremove (dword colnum, dword numnonzeros);
    void rowprint (void);
    void colprint (void);
    dword colfound (dword colnum, dword nonzeros);
  private:  
//    dynamicarray * rows,
//                 * cols,
//                 * rowordering,
//                 * colordering;
    dynamicarray  rows,
                  cols,
                  rowordering,
                  colordering;
    SpM_Node     * rowplacer,
                 * colplacer,
                 * freelist;
    dword size;  /* Size of the Dynamic array needed.   */
  };

#endif
