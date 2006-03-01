//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include "sc_defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "drwhdr.h"
#include "debugnew.h"
#include "scd_text.h"
#include "scd_font.h"
#include "dxfstuff.h"

//#define PRINTFONTFILE   /* to enable the printing of file fontfile in the current directory */
#define dbgtextorg   0
#define dbgtext      0
#define dbgparse     0
#define MAX_FONTS   10
static acad_font *loaded_fonts[MAX_FONTS];
static int        n_loaded_fonts;

static int
gobble_acad_byte(FILE *fp,unsigned char *result)
{
   int  i,val;
   char buf[256],*p = buf;
   unsigned char *r = (unsigned char *)(&val);

  
   // skip brackets or commas or newline
   while((i=fgetc(fp)) != EOF && !(isxdigit(i)||(i=='-')||(i=='+'))){
     ;
   }
   ungetc(i,fp);
   while((i=fgetc(fp)) != EOF && (isxdigit(i)||(i=='-')||(i=='+'))){
     *p++ = /*(char)*/i;
   }     
   ungetc(i,fp);
   *p = '\0';


   if((buf[0] == '0' && buf[1] != '\0')){
     sscanf(buf+1,"%x",&val);
   }
   else if(buf[0] == '-' && buf[1] == '0' && buf[2] != '\0'){
     sscanf(buf+2,"%x",&val);
     val *= -1;
   }
   else{
     sscanf(buf  ,"%d",&val);
   }
   
    //if( buf[0] == '0' || (buf[0] == '-' && buf[1] == '0')){
    //   (void)sscanf(buf,"%i",&val);
    //}
    //else{
    //   val = atoi(buf);
    //}
   memcpy(result,r,1); // dubious

   #if dbgparse
   TRACE3(" SCANNED buf >%s< to val = >%d<  result >%x<\n",buf,val,*result);
   #endif
   return(0); 

}

acad_font* load_acad_font(char *fn)
  {
  acad_font *f =  (acad_font *)calloc((size_t)1,(size_t)sizeof(acad_font));
  FILE *fp;
  short nbyte,fontnum,i,tmp;
  char  fontname[1024],c,tmpnm[1024],bnm[1024],*pp;
  long  bufsize = sizeof(acad_font);

#ifdef PRINTFONTFILE
  FILE *fp_font = fopen("fontfile","w");
#endif

  // strip any leading or trailing path from the font
  if ((pp = strrchr(fn,'\\')))
    {
    pp++;
    strcpy(bnm,pp);
    }
  else
    {
    strcpy(bnm,fn);
    }
  // strip any leading or trailing path from the font
  if((pp = strchr(bnm,'.')))
    {
    *pp = '\0';
    }
 // Strng ss;
  //GrfFontFiles(ss);
//  sprintf(tmpnm, "%s%s.SHP", GrfFontFiles(), RootDirectory(), bnm);
  sprintf(tmpnm, "%s%s.SHP", BaseGrfFontFiles(), bnm);
  if (!(fp = fopen(tmpnm,"r")))
    {
    return((acad_font *)NULL);
    }
  #if dbgtext
  TRACE1(" Font file %s opened\n",tmpnm);
  #endif
  
   // scan the header line
   while( (i = fscanf(fp,"%c%d,%d,%[^\n]",&c,&fontnum,&nbyte,fontname)) == 4 ){
      if( nbyte < 1 || nbyte > 2000 )
         return((acad_font *)NULL);
      if( fontnum == 0 && ( nbyte != 4 || c != '*' )){
            return((acad_font *)NULL);
      }
      if( fontnum != 0 &&  c != '*' ){
         return((acad_font *)NULL);
      }
      //build a byte buffer
      f->instr[fontnum] = (unsigned char *)malloc((size_t)nbyte);
      bufsize += nbyte;

      (void)strcpy(f->name,bnm); 
      #if dbgtext  
      TRACE1(" ------------- Current font number %d",fontnum); 
    #endif
      for( i = 0; i < nbyte; i++){
         if((tmp = gobble_acad_byte(fp,&f->instr[fontnum][i])) == -1){
            return((acad_font *)NULL);
         }
#ifdef PRINTFONTFILE
         if( i == 0 ){
            (void)fprintf(fp_font,"static unsigned char instr%d[] = {0x%02x",fontnum,(int)(f->instr[fontnum][i]));
         }
         else{
            if( (i+1)%10 == 0 )
                (void)fprintf(fp_font,"\n                                   ");
            (void)fprintf(fp_font,",0x%02x",(int)(f->instr[fontnum][i]));
         }         
#endif
      }
#ifdef PRINTFONTFILE
      (void)fprintf(fp_font,"};\n",tmp);
#endif

      while( (i=fgetc(fp)) != EOF && i != '*'){;}
      ungetc(i,fp);
   }           
   f->bufsize = bufsize;
   f->above   = FONT_ABOVE(f);
   f->below   = FONT_BELOW(f);

#ifdef PRINTFONTFILE
   { char *comma;
   (void)fprintf(fp_font,"static acad_font default_font = {\n");
   (void)fprintf(fp_font,"              %d,\n",f->bufsize);
   (void)fprintf(fp_font,"              %d,\n",f->above);
   (void)fprintf(fp_font,"              %d,\n",f->below);
   (void)fprintf(fp_font,"              \"%s\",\n",f->name);
   (void)fprintf(fp_font,"              {\n");
   for( i = 0; i < FONT_BUF_SIZE; i++ ){
      if( f->instr[i] ){
         (void)fprintf(fp_font,"              instr%d,\n",i);
      }
      else{
         if( i != FONT_BUF_SIZE-1 )comma = ",";
         else comma = " ";
         (void)fprintf(fp_font,"              (unsigned char *)NULL%s\n",comma);
      }
   }
   (void)fprintf(fp_font,"              }};\n");
   (void)fclose(fp_font);
   }
#endif

   (void)fclose(fp);
   return(f);
}

void
draw_fract_arc(double &xc,double &yc,int soff,int eoff,int hrad,int lrad,int sgn,int soct,int noct,int dflag,VP_VIEWPORT vp)
{
   int i = soff;

   TRACE0(" Fractional arc never used\n");




} 

void
draw_bulge_curve(double &xc,double &yc,int xd,int yd,int b,int dflag,VP_VIEWPORT vp)
{
   C3_CURVE arc,lin;
   PT3 s,e,m,p,q,t1,t2,t3;
   int sgn;
   double D;
   double H;

   if( b == 0 ){
    c3v_set(xc         ,yc         ,0.0,s);
      c3v_set(xc+xd      ,yc+yd      ,0.0,e);
    if( dflag ){
         lin = c3d_line(s,e);      
         v3d_curve(lin,(PARM)0,(PARM)0,vp);
         c3d_free_curve(lin);
      }
   }
   else{

      sgn = b/abs(b);
      b   = abs(b);
      D = sqrt(double((xd*xd)+(yd*yd)));
      H = (double)b*D/(127.0*2.0);

      c3v_set(xc         ,yc         ,0.0,s);
      c3v_set(xc+xd      ,yc+yd      ,0.0,e);
      c3v_set(xc+(xd/2.0),yc+(yd/2.0),0.0,p);
      c3v_set(yd* -1.0 ,xd,0.0,m);
      c3v_normalize(m,m);
      c3v_scale(m,H,m);
      c3v_add(p,m,q);


      c3v_sub(e,s,t1);
      c3v_sub(q,s,t2);
    c3v_normalize(t1,t1);
    c3v_normalize(t2,t2);
    c3v_cross(t1,t2,t3);
     
    if( t3[2] < 0.0 && sgn == -1 ){
         c3v_sub(p,m,q);
    }
    if( t3[2] > 0.0 && sgn ==  1 ){
         c3v_sub(p,m,q);
    }

      if( dflag ){
         arc = c3d_arc_3pts(s,q,e);      
         v3d_curve(arc,(PARM)0,(PARM)0,vp);
         c3d_free_curve(arc);
      }
   } 
   xc = e[0];
   yc = e[1];
}

void
draw_octant_arc(double &xc,double &yc,int rad,int sgn,int soct,int noct,int dflag,VP_VIEWPORT vp)
{
                 
  C3_CURVE arc;
  PT3 spp,epp,txx,mpp;
  REAL as,ae,am;

  if( noct == 0 ){// the arc is a circle
     noct = 8;
  }

  c3v_set(0.0,0.0,0.0,spp);
  c3v_set(0.0,0.0,0.0,mpp);
  c3v_set(0.0,0.0,0.0,epp);
  c3v_set(0.0,0.0,0.0,txx);
  as = Degs2Rads( soct*45.0);
  am = Degs2Rads((soct+((double)(sgn*noct))/2.0)*45.0);
  ae = Degs2Rads((soct+(sgn*noct))*45.0);
  spp[0] = rad*cos(as);
  spp[1] = rad*sin(as);
  mpp[0] = rad*cos(am);
  mpp[1] = rad*sin(am);
  epp[0] = rad*cos(ae);
  epp[1] = rad*sin(ae);                
  txx[0] = xc - spp[0];
  txx[1] = yc - spp[1];
  c3v_add(spp,txx,spp);
  c3v_add(epp,txx,epp);
  c3v_add(mpp,txx,mpp);
  if( dflag && noct != 8 ){
     arc = c3d_arc_3pts(spp,mpp,epp);      
     v3d_curve(arc,(PARM)0,(PARM)0,vp);
     c3d_free_curve(arc);
  } 
  if( dflag && noct == 8 ){ 
     PT3 ctr,norm;
     c3v_set(0.0,0.0,1.0,norm);
     c3v_mid_pt(spp,mpp,ctr);
     arc = c3d_circle(ctr,(REAL)rad,norm);      
     v3d_curve(arc,(PARM)0,(PARM)0,vp);
     c3d_free_curve(arc);
  } 
  xc = epp[0];
  yc = epp[1];
}

parse_acad_shape_file(double xi,double yi,text_info *ti,VP_VIEWPORT vp,double *xmin,double *ymin,double *xmax,double *ymax,int dflag)
{
    PT3 pp;
    char *c = ti->text;
    acad_font *font = ti->font;
    int vert_flag = 0;
    double h = ti->height;
    double a = ti->angle;
    unsigned char *cd;
    double xcur = xi,ycur = yi,xstk[10],ystk[10];
    int istk = -1,nstk = 0,pen;
    int xtmp,ytmp,skip = 0,xp,yp;
    int mval = 1,dval = 1,over_score = 0,under_score = 0;
    double sxcur,sycur;
    int charnum = 0;

    if( dflag )
       {
       pp[0] = 0.0; pp[1] = 0.0; pp[2] = 0.0;
       v3d_moveto(pp,vp);
       }

    if( *ymin > -1 * font->below)
        *ymin = -1 * font->below;
    if( *ymax <      font->above)
        *ymax = font->above;


    do{
       pen = 0;
       skip = 0;
       charnum = 0;
       while( *c && (*c == '%') && (*(c+1) == '%') ){
          if( *(c+2) == 'o' ){
             over_score = !over_score;
             c += 3;
          }
          else if( *(c+2) == 'u' ){
             under_score = !under_score;
             c += 3;
          }       
          else if( *(c+2) == 'd' ){
             c += 2;
             charnum = 127;
          }
          else if( *(c+2) == 'p' ){
             c += 2;
             charnum = 128;
          }
          else if( *(c+2) == 'c' ){
             c += 2;
             charnum = 129;
          }
          else if( *(c+2) == '%' ){
             c += 2;
          }
          else{
             c += 2;
          }
       }
       if( !(*c))break;
       if( !charnum )charnum = (int)(*c);
       cd = font->instr[charnum];
       if( !cd )
          return(-1); 

  #if dbgtext
  TRACE2("---------------- Current character=%c number=%d--------",*c,charnum);
    unsigned char *xx = cd;
  char bufa[1024],xxxxxx[100];
  char bufb[1024],yyyyyy[100];
  bufa[0] = '\0';
  bufb[0] = '\0';
  do{
     (void)sprintf(xxxxxx," %4x",*xx);
     strcat(bufa,xxxxxx);
     (void)sprintf(yyyyyy," %4d",(int)(*xx));
     strcat(bufb,yyyyyy);
  }while (*++xx);

    TRACE("hex: %s\n",bufa);
    TRACE("dec: %s\n",bufb);


  #endif

    if( *xmin > xcur) *xmin = xcur;
    if( *xmax < xcur) *xmax = xcur;
    sxcur = xcur;
    sycur = ycur;
    do{
       int xxx = *cd;
       int shp = ACAD_SHP_CODE(cd);
       int len = ACAD_SHP_DATA(cd);
       if( len == 0 ){
          switch( shp ){

            case ACAD_SHP_END:
               break;
            case ACAD_SHP_PENUP:
               if( !skip ){
            #if dbgtext
            TRACE0(" Pen Up");
          #endif
                  pen = 0;
               }
               else{
                  skip = 0;
            #if dbgtext
            TRACE0(" Skip Reset");
          #endif
               }
               break;
            case ACAD_SHP_PENDOWN:
               if( !skip ){
                  pen = 1;
          #if dbgtext
            TRACE0(" Pen Down");
          #endif
               }
               else{
                  skip = 0;
          #if dbgtext
            TRACE0(" Skip Reset");
          #endif
               }
               break;
            case ACAD_SHP_DIVVEC:
               cd++;
               if( !skip ){
                  dval *= *cd;
          #if dbgtext
            TRACE1(" Division value %d",dval);
          #endif
               }
               else{
                  skip = 0;
          #if dbgtext
            TRACE0(" Skip Reset");
          #endif
               }
               break;
            case ACAD_SHP_MULTVEC:
               cd++;
               if( !skip ){
                  mval *= *cd;
                  #if dbgtext 
            TRACE1(" Multiply value %d",dval);
          #endif
               }
               else{
            #if dbgtext
            TRACE0(" Skip Reset");
          #endif
                  skip = 0;
               }
               break;
            case ACAD_SHP_PUSH:
               if( !skip ){ 
                  if( nstk == 10 ){
             TRACE0(" Size of shape stack exceeded\n");
                     return(-1);
            }
                  xstk[nstk] = xcur;
                  ystk[nstk] = ycur;
          #if dbgtext
            TRACE2(" Loc on stack %.3f %.3f\n",xcur,ycur);
          #endif
                  nstk++;
               }
               else{
                  skip = 0;
          #if dbgtext
            TRACE0(" Skip Reset");
          #endif
               }
               break;
            case ACAD_SHP_POP:
               if( !skip ){
                  if( nstk == 0 )
                     return(-1);
                  xcur = xstk[nstk-1];
                  ycur = ystk[nstk-1];
          #if dbgtext
            TRACE2(" Loc off stack %.3f %.3f\n",xcur,ycur);
          #endif
                  nstk--;
               }
               else{
            #if dbgtext
            TRACE0(" Skip Reset");
          #endif
                  skip = 0;
               }
               break;
            case ACAD_SHP_SHP:
               cd++;
               if( !skip ){
                  char ss[2];
                  ss[0] = *cd;
                  ss[1] = '\0'; // will have to pass address of current
                  parse_acad_shape_file(xcur,ycur,ti,vp,xmin,xmax,ymin,ymax,dflag);
            #if dbgtext
            TRACE0(" Nested shape");
          #endif
               }
               else{
            #if dbgtext
            TRACE0(" Skip Reset");
          #endif
                  skip = 0;
               }
               break;
            case ACAD_SHP_XYOFF:
               if( !skip ){
            int xa,ya;
                  xa = (int)ACAD_SCHAR(++cd)*mval/dval;
                  ya = (int)ACAD_SCHAR(++cd)*mval/dval;
                  pp[0] = (xcur += xa);
                  pp[1] = (ycur += ya);
                  pp[2] = 0.0;
          #if dbgtext
            TRACE2(" Single XY offset  %d %d",xa,ya);
            TRACE2("           current=%.3f %.3f",xcur,ycur);
          #endif
                  if( dflag ){
                  if( pen )
                     v3d_lineto(pp,vp);
                  else
                     v3d_moveto(pp,vp);
                  }
                  //font_pen_rel(&xcur,&ycur,xtmp,ytmp,pen,mval,dval,font,h,vp);
               }
               else{
                  cd += 2;
                  skip = 0;   
          #if dbgtext
          TRACE0(" Skipping xy offset  - skip flag off");
          #endif
               }
               break;
            case ACAD_SHP_NXYOFF:
               if( skip ){
                  cd++;
                  while( *(cd+0) || *(cd+1))cd += 2;
                  skip = 0;
          #if dbgtext
            TRACE0(" Skip Reset");
          #endif
               }
               else{
                  
                  do{
                     xtmp = ACAD_SCHAR(++cd);
                     ytmp = ACAD_SCHAR(++cd);
                     pp[0] = (xcur += xtmp*mval/dval);
                     pp[1] = (ycur += ytmp*mval/dval);
                     pp[2] = 0.0;
                     #if dbgtext
               TRACE2(" Multiple XY offset  %d   %d",xtmp,ytmp);
               TRACE2("             current=%.3f,%.3f",xcur,ycur);
             #endif
                     if( dflag ){
                     if( pen )
                        v3d_lineto(pp,vp);
                     else
                        v3d_moveto(pp,vp);
                     }
                  }
                  while( xtmp || ytmp );
               }
               break;
            case ACAD_SHP_OCTARC:
               if( !skip ){
               
                  //PT3 cp1,cp2;
                  //double a1;
                  //C3_CURVE arc;
                  int val,sgn,rad,soct,noct,xxx;
                  rad = *++cd; //slow but seldom used
                  val = *++cd;          
            xxx = val;

                  sgn  = ((val&0x80) ? -1 : 1);
                  val = val*sgn;
                  soct = (val & 0x70)>>4;
                  noct = (val & 0x07);

          #if dbgtext
                TRACE1("OCTARC   rad=%d",rad);             
                TRACE1("         xxx=%d",xxx);             
                TRACE1("         sgn=%d",sgn);             
                TRACE1("        soct=%d",soct);             
                  TRACE1("        noct=%d",noct); 
                  #endif  
                            
                draw_octant_arc(xcur,ycur,rad,sgn,soct,noct,dflag,vp);
               }
               else{
             #if dbgtext
            TRACE0(" Skip Reset");
          #endif
                  skip = 0;
               }
               break;
            case ACAD_SHP_FRACARC:
               int val,sgn,soct,noct,st,en,high,low;
               st   = *++cd;
               en   = *++cd;
               high = *++cd;
               low  = *++cd;
               val  = *++cd;

               sgn  = ((val&0x80) ? -1 : 1);
               val = val*sgn;
               soct = (val & 0x70)>>4;
               noct = (val & 0x07);

               if( !skip ){
            #if dbgtext
                  TRACE1("FRACTIONAL ARC start offset = %d",st);             
                  TRACE1("                 end offset = %d",en);             
                  TRACE1("                high radius = %d",high);             
                  TRACE1("                 low radius = %d",low);             
                  TRACE1("                  direction = %d",sgn);             
                  TRACE1("                       soct = %d",soct);             
                  TRACE1("                       noct = %d",noct); 
          #endif
                  draw_fract_arc(xcur,ycur,st,en,high,low,sgn,soct,noct,dflag,vp); 
               }
               else{
            #if dbgtext
            TRACE0(" Skip Reset");
          #endif
                  skip =0;
               }
               break;
            case ACAD_SHP_BULGEARC:
         int xdis,ydis,bulg;
         xdis = *((char *)(++cd)); 
         ydis = *((char *)(++cd));
         bulg = *((char *)(++cd));
               //xdis = *++cd;
               //ydis = *++cd;
               //bulg = *++cd;
               if( !skip ){
            #if dbgtext
                  TRACE1("BULGE ARC xdis %d",xdis);             
                  TRACE1("          ydis %d",ydis);             
                  TRACE1("          bulg %d",bulg);
                  #endif             
          draw_bulge_curve(xcur,ycur,xdis,ydis,bulg,dflag,vp);
            #if dbgtext
          TRACE2(" AFTER BULGE %.3f %.3f",xcur,ycur);
                  #endif             
               }
               else{
            #if dbgtext
            TRACE0(" Skip Reset");
                  #endif             
                  skip = 0;
               }
               break;
            case ACAD_SHP_NBULGEARC:
         xdis = *((char *)(++cd)); 
         ydis = *((char *)(++cd));
         while( xdis || ydis ){
                  bulg = *((char *)(++cd));
          if( !skip ){
             #if dbgtext
                     TRACE1("NBULGE ARC xdis = %d",xdis);             
                     TRACE1("           ydis = %d",ydis);             
                     TRACE1("           bulg = %d",bulg);
                     #endif             
                     draw_bulge_curve(xcur,ycur,xdis,ydis,bulg,dflag,vp);
             //TRACE2(" AFTER NBULGE %.3f %.3f",xcur,ycur);

          }
                  else{
             #if dbgtext
               TRACE0(" Skip Reset");
                     #endif             
                     skip = 0;
                  }
          xdis = *((char *)(++cd)); 
            ydis = *((char *)(++cd));
            //xdis = *++cd;
            //ydis = *++cd;
         }
               break;
            case ACAD_SHP_VERTICAL:
               if( !skip ){
                   if( !vert_flag ){
                      skip = 1;
            #if dbgtext
                      TRACE0(" Skip flag on");
                      #endif             
           }
               }
               else{
                  skip = 0;
          #if dbgtext
                  TRACE0(" Skip flag off");  
                  #endif           
               }
               break;
            default:
               break;
          }
       }
       else{
          if( skip ){
             skip = 0;
       #if ( dbgtext )
             TRACE0(" Skip Reset");
       #endif
          }
          else{
       #if ( dbgtext )
       TRACE0(" Skip Reset");
       #endif
             skip = 0;
             len *= mval/dval;
             switch( shp ){
                case 0x0:
                   xp = len;       yp = 0;        
                    break;
                case 0x1:
                   xp = len;       yp = len/2;   
                   break;
                case 0x2:
                   xp = len;       yp = len;        
                   break;
                case 0x3:
                   xp = len/2;     yp = len;        
                   break;
                case 0x4:
                   xp = 0;         yp = len;        
                   break;
                case 0x5:
                   xp = -1*len/2;  yp = len;        
                   break;             
                case 0x6:
                   xp = -1*len;    yp = len;        
                   break;
                case 0x7:
                   xp = -1*len;    yp = len/2;      
                   break;
                case 0x8:
                   xp = -1*len;    yp = 0;          
                   break;                                     
                case 0x9:
                   xp = -1*len;    yp = -1*len/2;   
                   break;
                case 0xa:
                   xp = -1*len;    yp = -1*len;     
                   break;
                case 0xb:
                   xp = -1*len/2;  yp = -1*len;     
                   break;             
                case 0xc:
                   xp = 0;         yp = -1*len;     
                   break;
                case 0xd:
                   xp = len/2;     yp = -1*len;     
                   break;                          
                case 0xe:
                   xp = len;       yp = -1*len;     
                   break;
                case 0xf:
                   xp = len;       yp = -1*len/2;   
                   break;
                default:
                   return(-1);                 
                   break;               
                
            
             }
             pp[0] = (xcur += xp);
             pp[1] = (ycur += yp);
             pp[2] = 0.0;
       #if( dbgtext )
             TRACE2(" Vector generator vector  %d %d",xp,yp);
             TRACE2("                  current %.3f %.3f",xcur,ycur);
       #endif
             if( dflag ){
             if( pen )
                v3d_lineto(pp,vp);
             else
                v3d_moveto(pp,vp);
             }
             //font_pen_rel(&xcur,&ycur,xp,yp,pen,mval,dval,font,h,vp);
          }
       }

    }
    while( *++cd );
    // do the under/over score for the character
    if( over_score ){
       pp[0] = sxcur;
       pp[1] = FONT_ABOVE(font);
       pp[2] = 0.0;
       if( dflag )v3d_moveto(pp,vp);

       pp[0] = xcur;
       pp[1] = FONT_ABOVE(font);
       pp[2] = 0.0;
       if( dflag )v3d_lineto(pp,vp);

       //font_pen_abs(sxcur,FONT_ABOVE(font)   ,xcur,FONT_ABOVE(font),font,h,vp);
    }
    if( under_score ){
       pp[0] = sxcur;
       pp[1] = -1 * FONT_BELOW(font);
       pp[2] = 0.0;
       if( dflag )v3d_moveto(pp,vp);

       pp[0] = xcur;
       pp[1] = -1 * FONT_BELOW(font);
       pp[2] = 0.0;
       if( dflag )v3d_lineto(pp,vp);

       //font_pen_abs(sxcur,-1*FONT_BELOW(font),xcur,-1*FONT_BELOW(font),font,h,vp);
    }
    if( *xmin > xcur) *xmin = xcur;
    if( *xmax < xcur) *xmax = xcur;
    }
    while( *++c );


    //font_pen_dis(ti,vp);

    return(0);    
}

static void
find_acad_font(text_info *ti)
{
   int i;
   // to lookup the font each time the text is drawn obviously needs review
   // always make font 0 the default font
   if( n_loaded_fonts == 0 ){
       n_loaded_fonts = 1;
       loaded_fonts[0] = &default_font;
       TRACE0("SETTING DEFAULT FONT FOR ACAD\n");
   }
 
   // find the appropriate font pointer to go with ti->font_name
   ti->font = (acad_font *)NULL;
   for(i = 0; i < n_loaded_fonts; i++ ){
      if( strcmp(ti->font_name,loaded_fonts[i]->name) == 0){
         ti->font = loaded_fonts[i];
         //TRACE1("FONT SELECTED %s\n",loaded_fonts[i]->name);
         return;
      }
   }
   if( !ti->font && (n_loaded_fonts < MAX_FONTS) && ti->font_name[0] != '\0'){
      ti->font = load_acad_font(ti->font_name);
      if( ti->font ){
         loaded_fonts[n_loaded_fonts++] = ti->font;
         return;
      }
      else{
         ti->font = loaded_fonts[0];
         //TRACE2("FONT %s REQUESTED NOT FOUND USING %s\n",ti->font_name,loaded_fonts[0]->name);
                    // mhm need to show a message about this
         return;
      }
   }
   ti->font = loaded_fonts[0];
   //TRACE2("FONT %s REQUESTED BUT FONT BUFFER FULL USING %s\n",ti->font_name,loaded_fonts[0]->name);
   //mhm need to say not enough space for this font
   return;
}

void
c3t_identity(C3_TRANSFORM t)
{
    memset(t,0,sizeof(C3_TRANSFORM));
    t[0][0] = 1.0;
    t[1][1] = 1.0;
    t[2][2] = 1.0;

}

void
construct_text_transform(text_info *ti,C3_TRANSFORM txfrm)
{

   // mirror && oblique
   c3t_identity(txfrm);
   if( ti->oblique > 0.00001 ){
       C3_TRANSFORM tmp;
       c3t_identity(tmp);
       tmp[0][1] = tan(ti->oblique);
       c3t_mult(txfrm,tmp,txfrm);
   }
   if( ti->flags & 2 ){
       C3_TRANSFORM tmp;
       REAL dist=0;
       PT3 norm;
       norm[0] = 0.0; norm[1] = 1.0; norm[2] = 0.0;
       c3t_mirror(norm,dist,tmp);
       c3t_mult(txfrm,tmp,txfrm);
   }
   if( ti->flags & 4 ){
       C3_TRANSFORM tmp;
       REAL dist=0;
       PT3 norm;
       norm[0] = 1.0; norm[1] = 0.0; norm[2] = 0.0;
       c3t_mirror(norm,dist,tmp);
       c3t_mult(txfrm,tmp,txfrm);
   }
   //scale
   {
       C3_TRANSFORM tmp;
       REAL yscl = ti->height/ti->font->above;
       REAL xscl = ti->wfactor*ti->height/ti->font->above;
       PT3  org;
       org[0] = 0.0; org[1] = 0.0; org[2] = 0.0;
       c3t_scale(org,xscl,yscl,yscl,tmp);
       c3t_mult(txfrm,tmp,txfrm);
   }
   // rotate
   if( ti->angle > 0.000001 ){
       C3_TRANSFORM tmp;
       PT3 axis;
       REAL yscl = ti->height/(double)ti->font->above;
       REAL xscl = ti->wfactor*ti->height/(double)ti->font->above;
       PT3  org;
       org[0] = 0.0; org[1] = 0.0; org[2] = 0.0;
       axis[0] = 0.0; axis[1] = 0.0; axis[2] = 1.0;
       c3t_rotate(org,axis,ti->angle,tmp);
       c3t_mult(txfrm,tmp,txfrm);
   }
   //translate
   {
       C3_TRANSFORM tmp;
       PT3  org;
       org[0] = ti->pos[0]; org[1] = ti->pos[1]; org[2] = 0.0;
       c3t_translate(org,tmp);
       c3t_mult(txfrm,tmp,txfrm);
   }
}

void
scd_acad_text_corners(text_info *ti,double *x1,double *y1,double *x2,double *y2)
{
   double xpos = 0.0,ypos = 0.0;
   PT3 pp1,pp2,pp3,pp4,ppp1,ppp2,ppp3,ppp4;
   C3_TRANSFORM txfrm;
   int vflag = 0;
   double xmin = 1.0e30,ymin = 1.0e30,xmax = -1.0e30,ymax = -1.0e30;
   static acad_font *font;


   find_acad_font(ti);
   c3t_identity(txfrm);
   if( ti->oblique > 0.00001 ){
       C3_TRANSFORM tmp;
       c3t_identity(tmp);
       tmp[0][1] = tan(ti->oblique);
       c3t_mult(txfrm,tmp,txfrm);
   }
   if( ti->flags & 2 ){
       C3_TRANSFORM tmp;
       REAL dist=0;
       PT3 norm;
       norm[0] = 0.0; norm[1] = 1.0; norm[2] = 0.0;
       c3t_mirror(norm,dist,tmp);
       c3t_mult(txfrm,tmp,txfrm);
   }
   if( ti->flags & 4 ){
       C3_TRANSFORM tmp;
       REAL dist=0;
       PT3 norm;
       norm[0] = 1.0; norm[1] = 0.0; norm[2] = 0.0;
       c3t_mirror(norm,dist,tmp);
       c3t_mult(txfrm,tmp,txfrm);
   }
   //scale
   {
       C3_TRANSFORM tmp;
       REAL yscl = ti->height/ti->font->above;
       REAL xscl = ti->wfactor*ti->height/ti->font->above;
       PT3  org;
       org[0] = 0.0; org[1] = 0.0; org[2] = 0.0;
       c3t_scale(org,xscl,yscl,yscl,tmp);
       c3t_mult(txfrm,tmp,txfrm);
   }

   parse_acad_shape_file(xpos,ypos,ti,0,&xmin,&ymin,&xmax,&ymax,0);

   // transform the corners to get the shape
   pp1[0] = xmin;
   pp1[1] = ymin;
   pp1[2] = 0.0;
   c3t_eval_pt(pp1,txfrm,ppp1);
   pp2[0] = xmin;
   pp2[1] = ymax;
   pp2[2] = 0.0;
   c3t_eval_pt(pp2,txfrm,ppp2);
   pp3[0] = xmax;
   pp3[1] = ymin;
   pp3[2] = 0.0;
   c3t_eval_pt(pp3,txfrm,ppp3);
   pp4[0] = xmax;
   pp4[1] = ymax;
   pp4[2] = 0.0;
   c3t_eval_pt(pp4,txfrm,ppp4);

   // these corners are chosen to prevent obliqueing from having an effect
   // on the sizes returnd
   *x1 = ppp1[0];
   *x2 = ppp3[0];
   *y1 = ppp1[1];
   *y2 = ppp2[1];

}


void
gen_viewport_text(text_info *ti,VP_VIEWPORT vp)
{
   double xpos = 0.0,ypos = 0.0;
   C3_TRANSFORM txfrm;
   VP_TRANSFORM vpt;
   int vflag = 0;
   double xmin = 20000,ymin = 20000,xmax = -20000,ymax = -20000;
   static acad_font *font;

#ifdef PRINTFONTFILE
   load_acad_font("ROMANS");
#endif

   // choose a font pointer into the ti struct - cannot fail
   find_acad_font(ti);
   // text is ready for generation
   construct_text_transform(ti,txfrm);
   vpi_get_transform(vp, vpt);
   vp3_append_transform(vp, txfrm);
   parse_acad_shape_file(xpos,ypos,ti,vp,&xmin,&ymin,&xmax,&ymax,1);
   vpi_set_transform(vp, vpt);
}
                

char *
trim_path_and_suffix(char *out,char *in)
{
   char nm[1024],*pp;
   
   strcpy(nm,in);
   if((pp = strrchr(in,'\\'))){    // dos
      pp++;
      (void)strcpy(nm,pp);
   }
   else if((pp = strrchr(in,'/'))){  // unix
      pp++;
      (void)strcpy(nm,pp);
   }
   else{
    (void)strcpy(nm,in);
   }
   if((pp = strchr(nm,'.'))){
      *pp = '\0';
   }


   if( strlen(nm) > MAX_FONT_NAME-1 )
     {
      AfxMessageBox("Max font name exceeded");
     (void)strcpy(out,"junk");

     }
   else
     {
     (void)strcpy(out,nm);
     }
   return(out);
}



void
text_info_from_dxf_text(text_info *ti,DML_LIST stylelst,DXF_ENTITY e)
{ 
   DML_ITEM item;
   ti->on        = 1;
   ti->pos       = DXF_TEXT_INS_PT(e);
   ti->text      = DXF_TEXT_VALUE_GET(e);
   ti->style     = DXF_TEXT_STYLE_GET(e);
   ti->height    = DXF_TEXT_HEIGHT(e);
   ti->oblique   = Degs2Rads(DXF_TEXT_OBL_ANG(e));
   ti->angle     = Degs2Rads(DXF_TEXT_ROT_ANG(e));
   ti->wfactor   = DXF_TEXT_X_SCALE(e);
   ti->flags     = DXF_TEXT_FLAGS(e); 
   ti->horiz_org = DXF_TEXT_HOR_JUST(e); //mhm not documented in book - also required for ATTRIB
   ti->vert_org  = DXF_TEXT_VERT_JUST(e);//mhm not documented in book - also required for ATTRIB


   #if( dbgtextorg )
       TRACE1(" Text entity ------------- %s",ti->text);
       TRACE1("                     flags %d",ti->flags);
       TRACE1("                     hor_j %d",ti->horiz_org);
       TRACE1("                     ver_j %d",ti->vert_org);
   #endif




   if( !ti->style )ti->style = "STANDARD";
   ti->font_name[0] = '\0';
   DML_WALK_LIST(stylelst,item){

      char *style_name = DXF_STYLE_NAME_GET((DXF_TABLE_ENTRY)DML_RECORD(item));
      char *font_name  = DXF_STYLE_FONT_GET((DXF_TABLE_ENTRY)DML_RECORD(item));

      if( strcmp(style_name,ti->style) == 0){
         (void)trim_path_and_suffix(ti->font_name,font_name);
         break;
      }
   }



} 

void
text_info_from_dxf_attrib(text_info *ti,DML_LIST stylelst,DXF_ENTITY e)
{  
   DML_ITEM item;

   ti->on        = ((DXF_ATTRIB_AFLAGS(e) & DXF_ATTRIB_INVIS) == 0);
   ti->pos       = DXF_ATTRIB_PT(e);
   ti->text      = DXF_ATTRIB_VALUE_GET(e);
   ti->style     = DXF_ATTRIB_STYLE_GET(e);
   ti->height    = DXF_ATTRIB_HEIGHT(e);
   ti->oblique   = Degs2Rads(DXF_ATTRIB_OBL_ANG(e));
   ti->angle     = Degs2Rads(DXF_ATTRIB_ROT_ANG(e));
   ti->wfactor   = DXF_ATTRIB_X_SCALE(e);
   ti->flags     = DXF_ATTRIB_GFLAGS(e); 
   ti->horiz_org = 0; //mhm not clear where to fetch  - see above fix later
   ti->vert_org  = 0; //mhm not clear where to fetch  - see above fix later

   #if( dbgtextorg )
       TRACE1(" Attrib entity -------------  %s",ti->text);
       TRACE1("                       gflags %d",DXF_ATTRIB_GFLAGS(e));
       TRACE1("                       aflags %d",DXF_ATTRIB_AFLAGS(e));
       TRACE1("                         just %d",DXF_ATTRIB_JUST(e));
   #endif






   if( !ti->style )ti->style = "STANDARD";
   ti->font_name[0] = '\0';
   DML_WALK_LIST(stylelst,item){
      char *style_name = DXF_STYLE_NAME_GET((DXF_TABLE_ENTRY)DML_RECORD(item));
      char *font_name  = DXF_STYLE_FONT_GET((DXF_TABLE_ENTRY)DML_RECORD(item));
      if( strcmp(style_name,ti->style) == 0){
          (void)trim_path_and_suffix(ti->font_name,font_name);
          break;
      }
   }
}  

void
dxf_set_text_alignment_point(DXF_ENTITY e,DML_LIST stylelst)
{
   double x1,y1,x2,y2;
   
   text_info ti;
   text_info_from_dxf_text(&ti,stylelst,e);
   scd_acad_text_corners(&ti,&x1,&y1,&x2,&y2);

   if( ti.horiz_org || ti.vert_org ){  // CNM
      DXF_TEXT_AL_PT(e)[0] = DXF_TEXT_INS_PT(e)[0] + (x2-x1) * (ti.horiz_org * 0.5);
      DXF_TEXT_AL_PT(e)[1] = DXF_TEXT_INS_PT(e)[1] + (y2-y1) * (Max(0,ti.vert_org-1) * 0.5 + (ti.vert_org==0 ? 0.1 : 0));
      DXF_TEXT_AL_PT(e)[2] = DXF_TEXT_INS_PT(e)[2];
//      DXF_ATTRIB_AL_PT(e)[0] = 1.0;
//      DXF_ATTRIB_AL_PT(e)[1] = 1.0;
//      DXF_ATTRIB_AL_PT(e)[2] = 1.0;
   }
   else{
      DXF_TEXT_AL_PT(e)[0] = 0.0;
      DXF_TEXT_AL_PT(e)[1] = 0.0;
      DXF_TEXT_AL_PT(e)[2] = 0.0;
   }

}

void
dxf_set_attrib_alignment_point(DXF_ENTITY e,DML_LIST stylelst)
{
   double x1,y1,x2,y2;

   text_info ti;
   text_info_from_dxf_attrib(&ti,stylelst,e);
   scd_acad_text_corners(&ti,&x1,&y1,&x2,&y2);

   if( ti.horiz_org || ti.vert_org ){ // CNM
      DXF_TEXT_AL_PT(e)[0] = DXF_TEXT_INS_PT(e)[0] + (x2-x1) * (ti.horiz_org * 0.5);
      DXF_TEXT_AL_PT(e)[1] = DXF_TEXT_INS_PT(e)[1] + (y2-y1) * (Max(0,ti.vert_org-1) * 0.5 + (ti.vert_org==0 ? 0.1 : 0));
      DXF_TEXT_AL_PT(e)[2] = DXF_TEXT_INS_PT(e)[2];
//      DXF_ATTRIB_AL_PT(e)[0] = 1.0;
//      DXF_ATTRIB_AL_PT(e)[1] = 1.0;
//      DXF_ATTRIB_AL_PT(e)[2] = 1.0;
   }
   else{
      DXF_ATTRIB_AL_PT(e)[0] = 0.0;
      DXF_ATTRIB_AL_PT(e)[1] = 0.0;
      DXF_ATTRIB_AL_PT(e)[2] = 0.0;
   }
}


void 
dxf_text_to_viewport(DXF_ENTITY e,DML_LIST stylelst,VP_VIEWPORT vp)
{
   text_info ti;

   if( DXF_TEXT_VALUE_GET(e) && strlen( DXF_TEXT_VALUE_GET(e)) )
     {
     text_info_from_dxf_text(&ti,stylelst,e);
     gen_viewport_text(&ti,vp);
     }

}

void 
dxf_attrib_to_viewport(DXF_ENTITY e,DML_LIST stylelst,VP_VIEWPORT vp)
{
   text_info ti;

   if( DXF_ATTRIB_VALUE_GET(e) && strlen( DXF_ATTRIB_VALUE_GET(e)) )
     {
     text_info_from_dxf_attrib(&ti,stylelst,e);
     gen_viewport_text(&ti,vp);
     }

}
