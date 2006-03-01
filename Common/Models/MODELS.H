//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
//#ifdef MDLBASE

#pragma LIBCOMMENT("..\\..\\com\\ScdIF\\", "\\ScdIF" )
#pragma LIBCOMMENT("..\\..\\scd\\ScdLib\\", "\\ScdLib" )
#pragma LIBCOMMENT("..\\..\\scd\\kwdb\\", "\\kwdb" )
#pragma LIBCOMMENT("..\\..\\scd\\ScExec\\", "\\ScExec" )
#pragma LIBCOMMENT("..\\..\\scd\\XyLib\\", "\\XyLib" )
#pragma LIBCOMMENT("..\\..\\scd\\FlwLib\\", "\\FlwLib" )

#pragma LIBCOMMENT("..\\..\\Models\\MdlBase\\", "\\MdlBase" )
#include ".\MdlBase\CTRL_IO.H"
#include ".\MdlBase\FnBasic1.h"
#include ".\MdlBase\GSModel.h"
#include ".\MdlBase\MdlBase.h"
#include ".\MdlBase\M_HXBASE.H"
#include ".\MdlBase\M_BASE.H"
#include ".\MdlBase\M_BSTRES.H"
#include ".\MdlBase\M_LINK.H"
#include ".\MdlBase\M_SRCSNK.H"
#include ".\MdlBase\M_SURGE.H"
#include ".\MdlBase\M_XFER.H"
#include ".\MdlBase\mtrlogic.H"
#include ".\MdlBase\RL_BASIC.H"
#include ".\MdlBase\VLVLOGIC.H"
//#include ".\MdlBase\sp_queue.h"
#include "..\Scd\FlwLib\sp_react.h"
#include "..\Scd\FlwLib\sq_lock.h"
#include "..\Scd\FlwLib\SR_CTRL.H"
#include "..\Scd\FlwLib\sr_eqn.h"
#include "..\Scd\FlwLib\sr_vcs.h"
#include "..\Scd\FlwLib\sr_src.h"
//#include ".\MdlBase\vlebase.h"
#include ".\MdlBase\FlshTrn.h"
//#endif

#if defined(BASIC1) && !defined(BASIC1_BUSY)
  #define BASIC1_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\Basic1\\", "\\Basic1" )
  //#include ".\\Basic1\\ComSpMdl.h"
  #include ".\\Basic1\\Basic1.h"
  #include ".\\Basic1\\HeatX.h"
  #include ".\\Basic1\\OrfcEqns.h"
  #include ".\\Basic1\\Pipe.h"
  #include ".\\Basic1\\PipeEqns.h"
  #include ".\\Basic1\\PpFtEqns.h"
  #include ".\\Basic1\\Pump.h"
  #include ".\\Basic1\\PumpEqns.h"
  #include ".\\Basic1\\REDUCER.H"
  #include ".\\Basic1\\SrcSnk.h"
  #include ".\\Basic1\\Tank.h"
  #include ".\\Basic1\\Valve.h"
  #include ".\\Basic1\\ValvEqns.h"
  #include ".\\Basic1\\XFer.h"
  #undef BASIC1_BUSY
#endif

#if defined(CONTROL1) && !defined(CONTROL1_BUSY)
  #define CONTROL1_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\Control1\\", "\\Control1" )
  #include ".\Control1\Control1.h"
  #include ".\Control1\GENCON.H"
  #include ".\Control1\PIDCON.H"
  #undef CONTROL1_BUSY
#endif

#if defined(FLWEQN1) && !defined(FLWEQN1_BUSY)
  #define FLWEQN1_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\FlwEqn1\\", "\\FlwEqn1" )
  #include ".\FlwEqn1\FE_IMP.H"
  #include ".\FlwEqn1\FlwEqn1.h"
  #include ".\FlwEqn1\PC_CVOL.H"
  #include ".\FlwEqn1\PC_HQCRV.H"
  #include ".\FlwEqn1\PC_PMPCR.H"
  #include ".\FlwEqn1\PC_PSDSP.H"
  #include ".\FlwEqn1\VE_ROTAR.H"
  #undef FLWEQN1_BUSY
#endif

#if defined(HEATXCH1) && !defined(HEATXCH1_BUSY)
  #define HEATXCH1_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\HeatXch1\\", "\\HeatXch1" )
  #include ".\HeatXch1\ContctHt.h"
  #include ".\HeatXch1\FlashTk.h"
  #include ".\HeatXch1\HeatXch1.h"
  #include ".\HeatXch1\ShlTube.H"
  #include ".\HeatXch1\HeatX1.H"
  #undef HEATXCH1_BUSY
#endif

#if defined(SEPAR1) && !defined(SEPAR1_BUSY)
  #define SEPAR1_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\Separ1\\", "\\Separ1" )
  #include ".\\Separ1\\BeltFilt.h"
  #include ".\\Separ1\\ccd.h"
  #include ".\\Separ1\\ccwash.h"
  #include ".\\Separ1\\classif.h"
  #include ".\\Separ1\\Cntrfg1.h"
  #include ".\\Separ1\\CntrfgMB.h"
  #include ".\\Separ1\\FiltPrss.H"
  #include ".\\Separ1\\Separ1.h"
  #include ".\\Separ1\\Splitter.h"
  #include ".\\Separ1\\Thicken.h"
  #include ".\\Separ1\\Washer.h"
  #undef SEPAR1_BUSY
#endif

#if defined(MATHAND1) && !defined(MATHAND1_BUSY)
  #define MATHAND1_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\MatHand1\\", "\\MatHand1" )
  #include ".\MatHand1\MatHand1.h"
  #include ".\MatHand1\beltcnv.h"
  #include ".\MatHand1\beltcnv2.h"
  #include ".\MatHand1\chute.h"
  #undef MATHAND1_BUSY
#endif

#if defined(SOLUBLE1) && !defined(SOLUBLE1_BUSY)
  #define SOLUBLE1_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\Soluble1\\", "\\Soluble1" )
  #include ".\Soluble1\Soluble1.h"
  #include ".\Soluble1\SqSolbl1.h"
  #undef SOLUBLE1_BUSY
#endif

#if defined(ALUMINA1) && !defined(ALUMINA1_BUSY)
  #define ALUMINA1_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\Alumina1\\", "\\Alumina1" )
  #include ".\\Alumina1\\Alumina1.h"
  #include ".\\Alumina1\\AlSpMdl.h"
  #undef ALUMINA1_BUSY
#endif

#if defined(BLACKBOX1) && !defined(BLACKBOX1_BUSY)
  #define BLACKBOX1_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\BlackBox1\\", "\\BlackBox1" )
  #include ".\BlackBox1\BlackBoxBase.h"
  #undef BLACKBOX1_BUSY
#endif

#if (defined(SIZEDST1) || defined(COMMN1) || defined(SEPAR2)) && !defined(SIZEDST1_BUSY)
  #define SIZEDST1_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\SizeDst1\\", "\\SizeDst1" )
  #include ".\SizeDst1\SqSSA.h"
  #include ".\SizeDst1\SizeDst1.h"
  #include ".\SizeDst1\SqSzDst1.h"
  #undef SIZEDST1_BUSY
#endif

#if defined(COMMN1) && !defined(COMMN1_BUSY)
  #define COMMN1_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\Commn1\\", "\\Commn1" )
  #include ".\Commn1\Commn1.h"
  #include ".\Commn1\Crush1.h"
  #include ".\Commn1\Mill1.h"
  #undef COMMN1_BUSY
#endif

#if defined(SEPAR2) && !defined(SEPAR2_BUSY)
  #define SEPAR2_BUSY
  #pragma LIBCOMMENT("..\\..\\Models\\Separ2\\", "\\Separ2" )
  #include ".\Separ2\Separ2.h"
  #include ".\Separ2\Screen.h"
  #undef SEPAR2_BUSY
#endif
