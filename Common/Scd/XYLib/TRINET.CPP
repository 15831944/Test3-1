//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include <sc_defs.h>

//#include <memory.h>
#include <string.h>
//#include <malloc.h>
#include <math.h>
#define __TRINET_CPP
#include "trinet.h"

#define dbgLostTriangles 0

#if dbgLostTriangles
static flag dbgcnm=0, dbgit=0;
#endif

TriNet::TriNet(double x1,double y1,double x2,double y2)
  {
  t_l=NULL;
  e_l=NULL;
  p_l=NULL;
  InitTriNet(x1,y1,x2,y2);
  }


TriNet::~TriNet()
  {
  Clear();
  }

void TriNet::Clear()
  {
  if (t_l)
    delete []t_l;
  t_l=NULL;
  if (e_l)
    delete []e_l;
  e_l=NULL;
  while (p_l)
    {
    pnt *tmp = p_l;
    p_l = p_l->nxt;
    delete tmp;
    }
  }


int TriNet::first_triangle(double &x1,double &y1,double &z1,
                                     double &x2,double &y2,double &z2,
                                     double &x3,double &y3,double &z3)
  {
  c_tri = -1;
  return next_triangle(x1,y1,z1,x2,y2,z2,x3,y3,z3);
  }

int TriNet::next_triangle(double &x1,double &y1,double &z1,
                                     double &x2,double &y2,double &z2,
                                     double &x3,double &y3,double &z3)
  {
  c_tri += 1;

  if( c_tri >= n_tri )
     return 0;

  x1 = t_l[c_tri].p0->x;
  y1 = t_l[c_tri].p0->y;
  z1 = t_l[c_tri].p0->z;

  x2 = t_l[c_tri].p1->x;
  y2 = t_l[c_tri].p1->y;
  z2 = t_l[c_tri].p1->z;

  x3 = t_l[c_tri].p2->x;
  y3 = t_l[c_tri].p2->y;
  z3 = t_l[c_tri].p2->z;

//  x1 = xmin + x1*(xmax-xmin);
//  x2 = xmin + x2*(xmax-xmin);
//  x3 = xmin + x3*(xmax-xmin);

//  y1 = ymin + y1*(ymax-ymin);
//  y2 = ymin + y2*(ymax-ymin);
//  y3 = ymin + y3*(ymax-ymin);

  return 1;
  }

int TriNet::TriangleCount()
  {
  return n_tri - 1;
  }

int TriNet::Valid()
  {
  if( n_tri > 1 )
    return 1;
  return 0;
  }


int Side(double x1,double y1,double x2,double y2,double X,double Y)
  {

  double tmp = (X*(y1-y2)) + (Y*(x2-x1)) + (x1*y2-y1*x2);

  #if dbgLostTriangles
  if (dbgcnm && dbgit)
    dbgpln("        %14.6g %14.6g", tmp, Max(fabs(x2-x1), fabs(y2-y1)));
  //if( tmp <= 0.0 || fabs(tmp) < 1.0e-10)
  // CNM Count "on the line" as "on the right"
  #endif
  double Mx=Max(fabs(x2-x1), fabs(y2-y1));
  if (fabs(tmp) < 1.0e-3*Mx)
    return 0;
  else 
    return (tmp >= 0.0) ? 1 : 2;
  //if( tmp >= -1.0e-4*Max(fabs(x2-x1), fabs(y2-y1)))
  //  return 1;
  //return 0;
  }

int poly_test(double x,double y,pnt *p0,pnt *p1,pnt *p2)
  { 
  int Cnt[3];
  // Index 0 Undecided
  // Index 1 A Side
  // Index 2 Other Side
  Cnt[0]=0;
  Cnt[1]=0;
  Cnt[2]=0;
  
  #if dbgLostTriangles
  if (dbgcnm && dbgit)
    dbgpln("   Poly [%14.6g %14.6g] [%14.6g %14.6g] [%14.6g %14.6g] %8x %8x %8x", p0->x,p0->y,p1->x,p1->y,p2->x,p2->y,p0,p1,p2);
  #endif

  Cnt[Side(p0->x,p0->y,p1->x,p1->y,x,y)]++;
  Cnt[Side(p1->x,p1->y,p2->x,p2->y,x,y)]++;
  Cnt[Side(p2->x,p2->y,p0->x,p0->y,x,y)]++;
  #if dbgLostTriangles
  if (dbgcnm && dbgit)
    dbgpln("       :%2i %2i %2i", Cnt[0], Cnt[1], Cnt[2]);
//    dbgpln("   Poly %i %i [%14.6g %14.6g] [%14.6g %14.6g] [%14.6g %14.6g]", l,r, p0->x,p0->y,p1->x,p1->y,p2->x,p2->y);
  #endif
  if (((Cnt[1]+Cnt[0])==3) || ((Cnt[2]+Cnt[0])==3)) 
    return 1;
  return 0;
  }
    
tri* TriNet::FindTriangle(double x,double y)
  {
  //x = (x-xmin)/(xmax-xmin);
  //y = (y-ymin)/(ymax-ymin);

  // finds the appropriate triangle using x and y
  // search the radii
  
  #if dbgLostTriangles
  dbgit = ((x>=0.1) && (x < 0.8) && (y>40.0) && (y<70.0));
  if (dbgit)
    dbgp("FindTriangle: %14.6g %14.6g ",x,y);
  #endif
 
  for (int i=0; i<n_tri; i++ ) 
    {
    double d1val = t_l[i].x-x;
    d1val = d1val * d1val;
    double d2val = t_l[i].y-y;
    d2val = d2val * d2val;
    #if dbgLostTriangles
    if (dbgcnm && dbgit && (d1val + d2val <= t_l[i].r2 * 1.2))
      dbgpln("  D:%3i %14.6g %14.6g %14.6g %14.6g",i,d1val,d2val,t_l[i].r2, d1val+d2val-t_l[i].r2 * 1.000001);
    #endif
    //if (d1val + d2val <= t_l[i].r2 * 1.000001)
    if (d1val + d2val <= t_l[i].r2 * 1.1)
      {
      #if dbgLostTriangles
      if (dbgcnm && dbgit)
        dbgpln("                                            A:%i",((t_l[i].p0->z < 0.5e30) && (t_l[i].p1->z < 0.5e30) && (t_l[i].p2->z < 0.5e30)));
      #endif
      if ((t_l[i].p0->z < 0.5e30) && (t_l[i].p1->z < 0.5e30) && (t_l[i].p2->z < 0.5e30))
        {
        #if dbgLostTriangles
        if (dbgcnm && dbgit)
          dbgpln("                                            B:%i",poly_test(x,y,t_l[i].p0,t_l[i].p1,t_l[i].p2));
        #endif
        if (poly_test(x,y,t_l[i].p0,t_l[i].p1,t_l[i].p2))
          {
          #if dbgLostTriangles
          if (dbgcnm && dbgit)
            dbgpln(" FOUND ======= %3i [%g %g] [%g %g] [%g %g]", i, t_l[i].p0->x,t_l[i].p0->y,t_l[i].p1->x,t_l[i].p1->y,t_l[i].p2->x,t_l[i].p2->y);
          else if (dbgit)
            dbgpln(" FOUND =======");
          //dbgcnm=dbgmem;
          #endif
          return &t_l[i];
          }
        }
      }
    }
  #if dbgLostTriangles
  if (dbgit)
    dbgpln(" NOT FOUND");
  //dbgcnm=dbgmem;
  #endif
  return NULL;
  }


int fit_plane(double x1,double x2,double x3,double y1,double y2,double y3,double z1,double z2,double z3,double &A,double &B,double &C)
  {
  double a = DET(y2-y1,z2-z1,y3-y1,z3-z1);
  double b = DET(z2-z1,x2-x1,z3-z1,x3-x1);
  double c = DET(x2-x1,y2-y1,x3-x1,y3-y1);

  if (fabs(c) < 1.0e-30)
    return 0;
  //double tmp = a*x1 + b*y1 + c*z1;
  A = (a/c) * (-1.0);
  B = (b/c) * (-1.0);
  C = (a*x1 + b*y1 + c*z1)/c;
  return 1;
  }

int TriNet::XYZonplane(double x,double y,double &z)
  {
  #if dbgLostTriangles
  dbgcnm=0;
  #endif
  double A,B,C;
  tri *p = FindTriangle(x,y);
  if (!p)
    {
    #if dbgLostTriangles
    dbgcnm=1;
    FindTriangle(x,y);
    dbgcnm=0;
    #endif
    return 0;
    }

  // fit plane and eval
  //if( !fit_plane(p->p0->x,p->p0->y,p->p0->z,
  //          p->p1->x,p->p1->y,p->p1->z,
  //          p->p2->x,p->p2->y,p->p2->z,A,B,C))
  
  //x = (x-xmin)/(xmax-xmin);
  //y = (y-ymin)/(ymax-ymin);

  if (!fit_plane( p->p0->x,p->p1->x,p->p2->x,
                  p->p0->y,p->p1->y,p->p2->y,
                  p->p0->z,p->p1->z,p->p2->z,
                  A,B,C))
    return 0;

  z = A*x + B*y + C;
  return 1;
  }

int TriNet::GetZ(int TriNo,double x,double y,double &z)//bga
  {
  double A,B,C;
  if (TriNo>n_tri) 
    return 0;
  tri *p = &t_l[TriNo];
  if (!p)
    return 0;

  if (!fit_plane( p->p0->x,p->p1->x,p->p2->x,
                  p->p0->y,p->p1->y,p->p2->y,
                  p->p0->z,p->p1->z,p->p2->z,
                  A,B,C))
    return 0;

  z = A*x + B*y + C;
  return 1;
  }
    
int TriNet::Getr(int TriNo,double &r)//bga
  {
  if (TriNo>n_tri) 
    return 0;
  c_tri = TriNo;
  tri *p = &t_l[TriNo];
  if (!p)
    return 0;
  r=p->r;
  return 1;
  }

void TriNet::add_edg(tri *t)
  {
  register int i,j;
  int f[3];
  edg e[3];

  f[0]=f[1]=f[2]=1;
  if (t->p0 < t->p1)
    {
    e[0].p0 = t->p0;
    e[0].p1 = t->p1;
    }
  else
    {
    e[0].p0 = t->p1;
    e[0].p1 = t->p0;
    }
  if (t->p0 < t->p2)
    {
    e[1].p0 = t->p0;
    e[1].p1 = t->p2;
    }
  else
    {
    e[1].p0 = t->p2;
    e[1].p1 = t->p0;
    }
  if (t->p1 < t->p2)
    {
    e[2].p0 = t->p1;
    e[2].p1 = t->p2;
    }
  else
    {
    e[2].p0 = t->p2;
    e[2].p1 = t->p1;
    }                              

  for (i=0; i<n_edg; i++ )
    {
    for (j=0; j<3; j++)
      {
      //if( f[j] && !bcmp((char *)(e+j),(char *)(e_l+i),sizeof(edg))){
      if (f[j] && !memcmp((const void *)(e+j),(const void *)(e_l+i),(size_t)sizeof(edg)))
        {
        f[j] = 0;
        if (i != n_edg-1)
          {
          //bcopy((char *)(e_l+n_edg-1),(char *)(e_l+i),sizeof(edg));
          memcpy((void *)(e_l+i),(const void *)(e_l+n_edg-1),(size_t)sizeof(edg));
          }
        n_edg--;
        i--;
        }
      }
    }
  for (j=0; j<3; j++)
    {
    if (f[j])
      {
      if (n_edg==m_edg)
        {
        edg *tmp = new edg[m_edg + EDG_DELTA];
        memcpy((void *)tmp,(const void *)e_l,(size_t)sizeof(edg)*m_edg);
        delete []e_l;
        e_l = tmp;
        m_edg += EDG_DELTA;
        }

      e_l[n_edg].p0 = e[j].p0;
      e_l[n_edg].p1 = e[j].p1;
      n_edg++;
      }
    }
  }


pnt* TriNet::add_pnt(double x,double y,double z)
  {
  pnt *p = new pnt;//malloc((unsigned int)sizeof(pnt));
  p->x   = x;
  p->y   = y;
  p->z   = z;

  p->nxt = p_l;
  p_l    = p;
  return p;
/*   
   if( n_pnt == m_pnt )
     {
     pnt *tmp = new pnt[m_pnt + PNT_DELTA];
     memcpy((void *)tmp,(const void *)p_l,(size_t)sizeof(pnt)*m_pnt);
     delete []p_l;
     p_l = tmp;
     m_pnt += PNT_DELTA;
     }

   p_l[n_pnt].x = x;
   p_l[n_pnt].y = y;
   p_l[n_pnt].z = z;
   n_pnt++;

   return(&p_l[n_pnt-1]);*/
  }

 
int TriNet::calc_circ(tri *t)
  {
  /* find point sepeared on the x axis */
  if (fabs((t->p0->x - t->p1->x)) < 1.0e-10 )
    {
    pnt *tmp = t->p0;
    t->p0 = t->p2;
    t->p2 = tmp;
    }
  if (fabs((t->p0->x - t->p2->x)) < 1.0e-10 )
    {
    pnt *tmp = t->p0;
    t->p0 = t->p1;
    t->p1 = tmp;
    }
  double dx01 = t->p0->x - t->p1->x;
  double dx02 = t->p0->x - t->p2->x;
  double dy01 = t->p0->y - t->p1->y;
  double dy02 = t->p0->y - t->p2->y;
  if (fabs(dx01) < 1.0e-10 || fabs(dx02) < 1.0e-10)
    return -1;
  double ddd = (dy01/dx01)-(dy02/dx02);
  if (fabs(ddd) < 1.0e-10)
    return -1;

  double x02 = t->p0->x*t->p0->x;
  double y02 = t->p0->y*t->p0->y;
  double x12 = t->p1->x*t->p1->x;
  double y12 = t->p1->y*t->p1->y;
  double x22 = t->p2->x*t->p2->x;
  double y22 = t->p2->y*t->p2->y;
  double k1  = ((y02-y12)+(x02-x12))/(2.0*dx01);
  double k2  = ((y02-y22)+(x02-x22))/(2.0*dx02);
  double b   = (k1-k2)/ddd;
  double a   = -1.0*b*dy01/dx01 + k1;
  dx01 = t->p0->x-a;
  dy01 = t->p0->y-b;
  double r = sqrt(dx01*dx01 + dy01*dy01);
  t->x = a;
  t->y = b;
  t->r = r;
  t->r2 = r*r;
  return 0;
  }

 
int TriNet::delete_tri(int i)
  {
  int l = n_tri-1;
  if (i != l)
    {
    //bcopy((char *)(t_l+l),(char *)(t_l+i),sizeof(tri));
    memcpy((void *)(t_l+i),(const void *)(t_l+l),(size_t)sizeof(tri));
    }
  n_tri--;
  n_delete++;
  return 0;
  }

int TriNet::add_tri(pnt *p0,pnt *p1,pnt *p2)
  {
  if (n_tri==m_tri)
    {
    tri *tmp = new tri[m_tri + TRI_DELTA];
    memcpy((void *)tmp,(const void *)t_l,(size_t)sizeof(tri)*m_tri);
    delete []t_l;
    t_l = tmp;
    m_tri += TRI_DELTA;
    }

  t_l[n_tri].p0 = p0;
  t_l[n_tri].p1 = p1;
  t_l[n_tri].p2 = p2;
  if (calc_circ(&t_l[n_tri]))
    return -1;
  n_tri++;
  return 0;
  }

void TriNet::InitTriNet(double x1,double y1,double x2,double y2)
  {
  Clear();

  m_tri = TRI_DELTA;
  n_tri = 0;
  t_l   = new tri[TRI_DELTA];
   
  m_edg = EDG_DELTA;
  n_edg = 0;
  e_l   = new edg[EDG_DELTA];

  //m_pnt = PNT_DELTA;
  //n_pnt = 0;
  p_l   = 0;//new pnt[PNT_DELTA];

  n_delete = 0;

  xmin = x1;
  xmax = x2;
  ymin = y1;
  ymax = y2;

  //add a triangle that will contain all other points
  double dx = (xmax-xmin) * 0.1;
  double dy = (ymax-ymin) * 0.1;
  pnt *a = add_pnt(xmin-dx, ymin-dy, 1.0e30);
  pnt *b = add_pnt(xmin+(2.1*(xmax-xmin+dx)), ymin-dy, 1.0e30);
  pnt *c = add_pnt(xmin-dx, ymin+(2.1*(ymax-ymin+dy)), 1.0e30);
   
  (void)add_tri(a,b,c);
  }


int TriNet::mk_new_tri(double x,double y,double z)
  {
  register int i; 
  pnt *p = add_pnt(x,y,z);

  for (i=0; i<n_edg; i++ )
    {
    if (add_tri(p, e_l[i].p0, e_l[i].p1))
      return -1;
    }
  n_edg = 0;
  return 0;
  }

//int TriNet::flush_tri(int i)
//{
//   move(t_l[i].p0->x,t_l[i].p0->y);
//   draw(t_l[i].p1->x,t_l[i].p1->y);
//   draw(t_l[i].p2->x,t_l[i].p2->y);
//   draw(t_l[i].p0->x,t_l[i].p0->y);
//}


int TriNet::AddVertex(double x,double y,double z,int fast)
  {
  register int i;
  //x = (x-xmin)/(xmax-xmin);
  //y = (y-ymin)/(ymax-ymin);
  if (fast)
    {
    for (i=0; i<n_tri; i++)
      {
      if (t_l[i].x+t_l[i].r < x)
        {
        //flush_tri(i); 
        delete_tri(i);
        } 
      }
    }

  /* search the list delete and add edges */
  for (i=0; i<n_tri; i++)
    {
    double d1val  = t_l[i].x-x;
    d1val = d1val * d1val;
    double d2val = t_l[i].y-y;
    d2val = d2val * d2val;
    if (d1val + d2val <= t_l[i].r2/* * 1.000001*/)
      {
      add_edg(t_l+i);
      delete_tri(i);
      i--;
      }
    }

  if (mk_new_tri(x,y,z))
    return -1;
  return 0;
  }

//int TriNet::flush_delnet()
//{
//   register int i;
//   for( i = 0; i < n_tri; i++ ){
//       flush_tri(i);
//   }
//   (void)printf(" Ndelete = %d\n",n_delete);
//}

