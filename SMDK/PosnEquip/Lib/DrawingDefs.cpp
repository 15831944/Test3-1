#include "StdAfx.h"
#include <drawingdefs.h>

int g_nNumDefs = 14;

int Mode2Int(DRAWING_MODE dm)
{
	switch(dm)
	{
	case LINES : return 0;
	case LINE_STRIP : return 1;
	case LINE_LOOP : return 2;
	case TRIANGLES : return 3;
	case QUADS : return 4;
	case LINE_BLOCK : return 5;
	case SOLID_BLOCK : return 6;
	case COMBO_BLOCK : return 7;
	case UNIT_CUBE : return 8;
	case QUAD_STRIP : return 9;
	case POLYGON : return 10;
	case POLY_LINE_BLOCK : return 11;
	case POLY_SOLID_BLOCK : return 12;
	case POLY_COMBO_BLOCK : return 13;
	default :  return -1;
	};
}

DRAWING_MODE Int2Mode(int nMode)
{
	switch(nMode)
	{
	case 0 : return LINES;
	case 1 : return LINE_STRIP; 
	case 2 : return LINE_LOOP; 
	case 3 : return TRIANGLES; 
	case 4 : return QUADS;
	case 5 : return LINE_BLOCK;
	case 6 : return SOLID_BLOCK;
	case 7 : return COMBO_BLOCK;
	case 8 : return UNIT_CUBE;
	case 9 : return QUAD_STRIP;
	case 10 : return POLYGON;
	case 11 : return POLY_LINE_BLOCK;
	case 12 : return POLY_SOLID_BLOCK;
	case 13 : return POLY_COMBO_BLOCK;
	default : return LINES;
	};
}