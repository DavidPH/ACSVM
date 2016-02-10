//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Scripts.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__CAPI__Script_H__
#define ACSVM__CAPI__Script_H__

#include "Types.h"

#ifdef __cplusplus
#include "ACSVM/Script.hpp"
#endif


#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// ACSVM_ScriptType
//
typedef enum ACSVM_ScriptType
{
   ACSVM_ScriptType_Closed,
   ACSVM_ScriptType_BlueReturn,
   ACSVM_ScriptType_Death,
   ACSVM_ScriptType_Disconnect,
   ACSVM_ScriptType_Enter,
   ACSVM_ScriptType_Event,
   ACSVM_ScriptType_Lightning,
   ACSVM_ScriptType_Open,
   ACSVM_ScriptType_Pickup,
   ACSVM_ScriptType_RedReturn,
   ACSVM_ScriptType_Respawn,
   ACSVM_ScriptType_Return,
   ACSVM_ScriptType_Unloading,
   ACSVM_ScriptType_WhiteReturn,
} ACSVM_ScriptType;

//
// ACSVM_ScriptName
//
// ACSVM::ScriptName mirror.
//
struct ACSVM_ScriptName
{
   ACSVM_String *s;
   ACSVM_Word    i;
};


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

#ifdef __cplusplus
}
#endif

#endif//ACSVM__CAPI__Script_H__

