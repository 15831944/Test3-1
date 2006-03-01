//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================

#define  SEG_LRFILL  1                         // fill from left to right
#define  SEG_RLFILL  2                         // fill from right to left
#define  SEG_TBFILL  4                         // fill from top to bottom
#define  SEG_BTFILL  8                         // fill from bottom to top
#define  SEG_LR      16                        // left to right slider
#define  SEG_HIGH    32                        // segment is highlighted
#define  SEG_TOGGLE  64                        // fill is an on off state
#define  SEG_BILEVEL 128                       // fill has two levels 

#define  SEG_FILL    SEG_LRFILL|SEG_RLFILL|SEG_BTFILL|SEG_TBFILL


#define  SEG_IS_LRFILL(v)    ((v) & SEG_LRFILL)    
#define  SEG_IS_RLFILL(v)    ((v) & SEG_RLFILL)    
#define  SEG_IS_TBFILL(v)    ((v) & SEG_TBFILL)    
#define  SEG_IS_BTFILL(v)    ((v) & SEG_BTFILL)    
#define  SEG_IS_TOGGLE(v)    ((v) & SEG_TOGGLE)    
#define  SEG_IS_BILEVEL(v)   ((v) & SEG_BILEVEL)    
#define  SEG_IS_LR(v)        ((v) & SEG_LR    )    
#define  SEG_IS_HIGH(v)      ((v) & SEG_HIGH  )    
#define  SEG_IS_FILL(v)      ((v) & SEG_FILL  )    

#define DEFAULTFILLCOLOUR RGB(0,255,255)/*CYAN*/
#define DEFAULTBARSCOLOUR RGB(255,0,0)
#define DEFAULTTOGGCOLOUR RGB(255,0,0)




                                            
