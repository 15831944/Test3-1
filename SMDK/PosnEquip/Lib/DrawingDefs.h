#pragma once

typedef enum {
	LINES,
	LINE_STRIP,
	LINE_LOOP,
	TRIANGLES,
	QUADS,
	LINE_BLOCK,
	SOLID_BLOCK,
	COMBO_BLOCK,
	UNIT_CUBE,
	QUAD_STRIP,
	POLYGON,
	POLY_LINE_BLOCK,
	POLY_SOLID_BLOCK,
	POLY_COMBO_BLOCK
} DRAWING_MODE;

int Mode2Int(DRAWING_MODE dm);
DRAWING_MODE Int2Mode(int nMode);

extern int g_nNumDefs;

