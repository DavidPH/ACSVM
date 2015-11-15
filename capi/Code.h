//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Codes.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__CAPI__Code_H__
#define ACSVM__CAPI__Code_H__

#include "Types.h"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ACSVM_Code
//
// ACSVM::Code mirror.
//
typedef enum ACSVM_Code
{
   #define ACSVM_CodeList(name, ...) ACSVM_Code_##name,
   #include "../acsvm/CodeList.hpp"

   ACSVM_Code_None
} ACSVM_Code;

//
// ACSVM_Func
//
// ACSVM::Func mirror.
//
typedef enum ACSVM_Func
{
   #define ACSVM_FuncList(name) ACSVM_Func_##name,
   #include "../acsvm/CodeList.hpp"

   ACSVM_Func_None
} ACSVM_Func;

#endif//ACSVM__CAPI__Code_H__

