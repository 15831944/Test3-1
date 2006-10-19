//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
#pragma once

#ifndef  __MD_HEADERS_H
#define  __MD_HEADERS_H

#if _MSC_VER < 1300
#error "Incorrect verion of compiler being used"
#endif

#pragma warning (disable : 4355) // this used in base member initializer list 

//Include all header files for SMDK
#include <typeinfo.h>
#include "md_share1.h"
#include "md_share2.h"
#include "md_share3.h"
#include "md_share4.h"
#include "md_data.h"
#include "md_defn.h"
#include "md_alumina.h"
#include "md_spmodel.h"
#include "md_PSD.h"
#include "md_vector.h"
#include "md_method.h"

#endif
