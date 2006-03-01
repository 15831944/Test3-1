//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
 // SysCAD Copyright Kenwalt (Pty) Ltd 1992

#ifndef  __TRINET_H
#define  __TRINET_H

//#include <stdlib.h>
//#include <assert.h>         operator
#ifndef __SC_DEFS_H
  #include "sc_defs.h"
#endif  

#undef DllImportExport /* already defined */
#ifdef __TRINET_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


class pnt 
  {
  public:
    double     x;
    double     y;
    double     z;
    pnt       *nxt;
  };

class tri
  {
  public:
    pnt        *p0;
    pnt        *p1;
    pnt        *p2;
    double       x;
    double       y;
    double       r;
    double       r2;
  };

class edg
  {
  public:
    pnt   *p0;
    pnt   *p1;
  };

#undef DllImportExport
#ifdef __TRINET_CPP
  #define DllImportExport DllExport
#elif !defined(XYLIB)
  #define DllImportExport DllImport
#else
  #define DllImportExport
#endif


#define  DET(a11,a12,a21,a22)   (((a11) * (a22)) - ((a21) * (a12)))


//============================================================================

#define TRI_DELTA   20
#define EDG_DELTA   20 

//#define PNT_DELTA   4


class DllImportExport TriNet {
                protected:
                  tri *t_l;   // The triangle list
                  int  n_tri;
                  int  m_tri;
                  int  c_tri;

                  pnt *p_l;   // The point list
                  //int  n_pnt;
                  //int  m_pnt;


                  edg *e_l;   // The edge list
                  int  n_edg;
                  int  m_edg;
                  
                  int  n_delete;
                  double xmin,xmax,ymin,ymax;
                
                
                  void  add_edg(tri *t);
                  pnt * add_pnt(double x,double y,double z);
                  int   calc_circ(tri *t);
                  int   delete_tri(int i);
                  int   add_tri(pnt *p0,pnt *p1,pnt *p2);
                  tri * FindTriangle(double x,double y);
                  int   mk_new_tri(double x,double y,double z);
                  //int TriNet::flush_tri(int i);
                  //int TriNet::flush_delnet();

                public:

                  TriNet(double x1 = -10000.0,double y1 = -10000.0,double x2 = 10000.0,double y2 = 10000.0);
                  ~TriNet();
                  void Clear();

                  // Not appropriate since the pnt list is referenced by the tri list and
                  // a pure copy is no good.
                  //TriNet& operator=(const TriNet &v);
                  int   Valid();
                  int   AddVertex(double x,double y,double z,int fast);
                  void  InitTriNet(double x1 = -10000.0,double y1 = -10000.0,double x2 = 10000.0,double y2 = 10000.0);
                  int   TriangleCount();
                  int   GetZ(int TriNo,double x,double y,double &z);//bga
                  int   Getr(int TriNo,double &r);//bga
                  int   XYZonplane(double x,double y,double &z);

                  int   first_triangle(double &x1,double &y1,double &z1,// never tested
                                     double &x2,double &y2,double &z2,
                                     double &x3,double &y3,double &z3);

                  int   next_triangle(double &x1,double &y1,double &z1, // never tested
                                     double &x2,double &y2,double &z2,
                                     double &x3,double &y3,double &z3);

      
              };

            

#endif


















