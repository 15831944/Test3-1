//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#include "stdafx.h"
#include <math.h>
#include "sc_defs.h"
#include "drwhdr.h"
#include "dxfstuff.h"
#include "cmdmenu.h"


void
dump_entity(DXF_ENTITY e)
{
   DML_LIST l;
   DML_ITEM i;
   DXF_ENTITY en;
   int type     = DX_ENTITY_ID(e);
   pchar layer  = DXF_ENTITY_LAYER_GET(e);
   pchar handle = DXF_ENTITY_HANDLE_GET(e);
   pchar ltype  = DXF_ENTITY_HANDLE_GET(e);
   REAL  thk    = DXF_ENTITY_THICKNESS(e);
   int   color  = DXF_ENTITY_COLOR(e);
   REAL  *extr  = DXF_ENTITY_EXTRUSION(e);

   gs_pCmd->Print("\n\r");
   gs_pCmd->Print(" ---------------------------------------------------\n");
   if( layer  ) gs_pCmd->Print("      Layer: %-s\n",layer);
   if( handle ) gs_pCmd->Print("     Handle: %-s\n",handle);
   if( ltype  ) gs_pCmd->Print("      Ltype: %-s\n",ltype);
                gs_pCmd->Print("  Thickness: %.3f\n",thk);
                gs_pCmd->Print("      Color: %d\n",color);
   if( extr   ) gs_pCmd->Print("  Extrusion: %.3f,%.3f,%.3f\n",extr[0],extr[1],extr[2]);

   switch( type ){
    case DXF_POINT: 
       gs_pCmd->Print("DXF_ENTITY %x is a point\n",e);
       break;
    case DXF_LINE:
       gs_pCmd->Print("DXF_ENTITY %x is a line\n",e);
       break;
    case DXF_CIRCLE:
       gs_pCmd->Print("DXF_ENTITY %x is a circle\n",e);
       break;
    case DXF_ARC:
       gs_pCmd->Print("DXF_ENTITY %x is a arc\n",e);
       break;
    case DXF_TRACE:
       gs_pCmd->Print("DXF_ENTITY %x is a trace\n",e);
       break;
    case DXF_SOLID:
       gs_pCmd->Print("DXF_ENTITY %x is a solid\n",e);
       break;
    case DXF_POLYLINE:
       gs_pCmd->Print("DXF_ENTITY %x is a polyline\n",e);
       break;
    case DXF_LINE3D:
       gs_pCmd->Print("DXF_ENTITY %x is a line3d\n",e);
       break;
    case DXF_FACE3D:
       gs_pCmd->Print("DXF_ENTITY %x is a face3d\n",e);
       break;
    case DXF_TEXT:
       gs_pCmd->Print("DXF_ENTITY %x is a text with value %s\n",e,DXF_TEXT_VALUE_GET(e));
       gs_pCmd->Print("              Hgt %.3f\n",DXF_TEXT_HEIGHT(e));
       gs_pCmd->Print("              Scl %.3f\n",DXF_TEXT_X_SCALE(e));
       gs_pCmd->Print("              Rot %.3f\n",DXF_TEXT_ROT_ANG(e));
       break;
    case DXF_SHAPE:
       gs_pCmd->Print("DXF_ENTITY %x is a shape\n",e);
       break;
    case DXF_INSERT:
       gs_pCmd->Print("DXF_ENTITY %x is an insert with name %s\n",e,DXF_INSERT_BLOCK_GET(e));
       gs_pCmd->Print("              Flags %x\n",DXF_INSERT_FLAGS(e));
       gs_pCmd->Print("              Pos %.3f %.3f %.3f\n",DXF_INSERT_PT(e)[0],DXF_INSERT_PT(e)[1],DXF_INSERT_PT(e)[2]);
       gs_pCmd->Print("              Scl %.3f %.3f %.3f\n",DXF_INSERT_X_SCALE(e),DXF_INSERT_Y_SCALE(e),DXF_INSERT_Z_SCALE(e));
       gs_pCmd->Print("              Rot %.3f\n",DXF_INSERT_ROT_ANG(e));

       l = DXF_INSERT_ATTRS(e);
       if( l )
         {
         WALK_ENTITY_LIST(l,i,en)
           {
           gs_pCmd->Print("              Attributes %s:%s  layer:%s  col:%d\n",DXF_ATTRIB_TAG_GET(en), DXF_ATTRIB_VALUE_GET(en),DXF_ENTITY_LAYER_GET(en),DXF_ENTITY_COLOR(en));
           }
         }
       break;
    case DXF_ATTDEF:
       gs_pCmd->Print("DXF_ENTITY %x is a attdef\n",e);
       break;
    case DXF_DIM:
       gs_pCmd->Print("DXF_ENTITY %x is a dim\n",e);
       break;
    case DXF_CURVE:
       gs_pCmd->Print("DXF_ENTITY %x is a curve\n",e);
       break;
    default:
       gs_pCmd->Print("DXF_ENTITY %x is a unidentified\n",e);
       break;
   }
}

void
dump_curve(C3_CURVE c)
{
   if( C3_CURVE_IS_LINE(c)){
     gs_pCmd->Print(" Curve %x is a line\n",c);
   }
   else if( C3_CURVE_IS_ARC(c)){
     gs_pCmd->Print(" Curve %x is an arc\n",c);
   }
   else if( C3_CURVE_IS_PCURVE(c)){
     gs_pCmd->Print(" Curve %x is a pcurve\n",c);
   }
#ifdef  SPLINE
   else if( C3_CURVE_IS_ELLIPSE(c)){
     gs_pCmd->Print(" Curve %x is an ellipse\n",c);
   }
   else if( C3_CURVE_IS_SPLINE(c)){
     gs_pCmd->Print(" Curve %x is a spline\n",c);
   }
   else if( C3_CURVE_IS_BEZIER(c)){
     gs_pCmd->Print(" Curve %x is a bezier\n",c);
   }
#endif  /* SPLINE */
   else{
     gs_pCmd->Print(" Curve %x has unidentified curve type\n",c);
   }
}
