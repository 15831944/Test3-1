//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#ifndef __SCD_TEXT_H
#define __SCD_TEXT_H

// Autocad shape codes
#define    ACAD_SHP_CODE(x)      (((*((unsigned char *)(x)))&0x0F))
#define   ACAD_SHP_DATA(x)     (((*((unsigned char *)(x)))&0xF0)>>4)
#define    ACAD_UCHAR(x)           (*((unsigned char *)(x)))
#define    ACAD_SCHAR(x)          (*((signed   char *)(x)))

#define   ACAD_SHP_END              0x00
#define   ACAD_SHP_PENDOWN      0x01
#define   ACAD_SHP_PENUP            0x02
#define   ACAD_SHP_DIVVEC           0x03
#define   ACAD_SHP_MULTVEC      0x04
#define   ACAD_SHP_PUSH         0x05
#define   ACAD_SHP_POP              0x06
#define   ACAD_SHP_SHP              0x07
#define   ACAD_SHP_XYOFF            0x08
#define   ACAD_SHP_NXYOFF           0x09
#define   ACAD_SHP_OCTARC           0x0a
#define   ACAD_SHP_FRACARC      0x0b
#define   ACAD_SHP_BULGEARC     0x0c
#define   ACAD_SHP_NBULGEARC        0x0d
#define   ACAD_SHP_VERTICAL     0x0e
      
//Sycad Font buffer
#define MAX_FONT_NAME 10  // dos file name - 10 should be ok
#define FONT_BUF_SIZE 256
#define FONT_ABOVE(f) ((short)((f)->instr[0][0]))
#define FONT_BELOW(f) ((short)((f)->instr[0][1]))
#define FONT_SCL(f)   (FONT_ABOVE(f) + FONT_BELOW(f))
#define FONT_MODE(f)  ((short)((f)->instr[0][2]))

typedef struct tacad_font {
                    long           bufsize;
                    int            above;
                    int            below;
                    char           name[MAX_FONT_NAME];
                    unsigned char *instr[FONT_BUF_SIZE];
               }acad_font;


typedef struct ttext_info {
                  char       font_name[MAX_FONT_NAME];
                  acad_font  *font;
                  REAL       *pos;
                  char       *text;
                  char       *style;
                  double     height;
                  double     oblique;
                  double     angle;
                  double     wfactor;
                  int        horiz_org;    // 0=left 1 = centre 2 = right
                  int        vert_org;
                  int        mirror_x;
                  int        mirror_y;
                  int        text_vertical;
                  int        flags;
                  int        on;

                  double     tantheta;
                  double     cosrot;
                  double     sinrot;
               } text_info;




extern void dxf_text_to_viewport(DXF_ENTITY e,DML_LIST stylelst,VP_VIEWPORT vp);
extern void dxf_attrib_to_viewport(DXF_ENTITY e,DML_LIST stylelst,VP_VIEWPORT vp);
extern void dxf_set_text_alignment_point(DXF_ENTITY e,DML_LIST stylelst);
extern void dxf_set_attrib_alignment_point(DXF_ENTITY e,DML_LIST stylelst);

extern void text_info_from_dxf_attrib(text_info *t,DML_LIST stylelst,DXF_ENTITY e);
extern void text_info_from_dxf_text(text_info *t,DML_LIST stylelst,DXF_ENTITY e);
extern void scd_acad_text_corners(text_info *ti,double *d1,double *d2,double *d3,double *d4);



#endif
