//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
// SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __MTX_MTRX_H
#define  __MTX_MTRX_H


/*----------------------------------------------------------------------------*/
/*     Static Storage                     : 60 Bytes (globals) + code         */
/*     Dynamic Storage  - Matrix          : 12*N + (R+12)*(ç + F)             */
/*                      - For the Unit    : 20*N + (R+12)*(ç + F) , where     */
/*     Dimention(Matrix) = N x N , ç = Number of non zero's in the matrix,    */
/*     and F = number of fill-ins after the analyse phase. Run the test       */
/*     program for an idea of the sizes of ç.                                 */
/*----------------------------------------------------------------------------*/


typedef class node *ptr;
 
class node
  {                                    /* Node for each non zero              */
  DEFINE_SPARES(node)
  public:
    //DBG_NEW(SP_NODE)
  double value;
  ptr right,
      down;
  dword col,
  row;
  };
 
class head
  {                                 /* Head of each list contained in an array*/
  DEFINE_SPARES(head)
  public:
    //DBG_NEW(SP_HEAD)
  dword nonzerocount;
  ptr next;
  };
 
//typedef class head *table_dynamicarray;/* Size dynamically allocated         */
///typedef dword * perm_dynamicarray;
typedef CArray <head, head&> table_dynamicarray;
typedef CArray <dword, dword> perm_dynamicarray;
 
class SpM_Mtrx : public SpM_Ordr
  {
  DEFINE_SPARES(SpM_Mtrx)
  public:
    //DBG_NEW(SP_MATRIX)
    
    SpM_Mtrx(dword n);
    ~SpM_Mtrx();
    
    void Clear(dword n);
    flag Singular (void);
    dword rownonzerocount (dword row);
    dword colnonzerocount (dword col);
    void createordering (void);
    void insertelement (dword rowpos, dword colpos, double value);
    double getelement (dword rowpos, dword colpos);
    void swoprow (dword fromrow, dword torow );
    void swopcol (dword fromcol, dword tocol );
    void addrows (dword x, dword y, double *mult );
    void searchrow (dword rownum, dword *colnum, dword *max_fill_ins, double *value );
    void searchcol (dword colnum, dword fromrow, dword *rownum, dword *max_fill_ins, double *value, flag *valid);
    void getfirstcol (dword rownum, dword *colnum, double *value);
    void getfirstrow (dword colnum, dword *rownum, double *value);
    void comparerow (dword rownum, dword colnum, double value, flag *valid);
    void getfirstactiverow (dword colnum, dword *rownum, flag *exists);
    void getnextactiverow (dword *rownum, flag *exists);
    void solve (CDArray & solution, CDArray & b);
    void report_stats (Mtx_Info *table);
    void rowprint (CDArray &  b, char Symbolic, int Wide, int Prec);
    void colprint (void);
    void coltable (void);
    short errors;

  private:
    CDArray        tempb,
                   tempsoln;
//    CArray <head, head&> rowarray,
//                         colarray;
    table_dynamicarray rowarray,
                       colarray;
    dword size,                     /* Size of dynamic array needed        */
    nonzeros,
    fill_ins;
    node    *from,
            *too,
            *rowplacer,
            *colplacer;       /* Temp nodes                          */
    perm_dynamicarray rowpermutations,
                      colpermutations,
                      inverserowperm,
                      inversecolperm;
    // void (* performsolve) (vector *, vector);
    // void (* performaddrows) (dword, dword, double *);
    // void (* performinsert) (dword, dword, double);
    char  firstsolve;
    char  IsSingular;

    void colinsert(dword colpos, dword rowpos, node *newnode);
    void colremove(dword colpos, dword rowpos);
    void rowremove(dword colpos, dword rowpos);
    void rowinsert(dword colpos, dword rowpos, node *newnode);

    void insertelement1(dword rowpos, dword colpos, double value);
    double getelement1(dword rowpos, dword colpos);
    void addrows1(dword x, dword y, double * mult);
    void solve1(CDArray &  solution, CDArray & b);

    void insertelement2(dword rowpos, dword colpos, double value);
    double getelement2(dword rowpos, dword colpos);
    void addrows2(dword x, dword y, double * mult);
    void solve2(CDArray &  solution, CDArray & b);
  };

#endif









