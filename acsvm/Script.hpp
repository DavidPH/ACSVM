//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Script class.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__Script_H__
#define ACSVM__Script_H__

#include "Types.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // ScriptType
   //
   enum class ScriptType
   {
      Closed,
      Open,
   };

   //
   // Script
   //
   class Script
   {
   public:
      explicit Script(Module *module);
      ~Script();

      Module *const module;

      Script    *envNext;
      Script    *envPrev;
      String    *nameStr;
      Word       argC;
      Word       codeIdx;
      Word       flags;
      Word       locArrC;
      Word       locRegC;
      Word       nameInt;
      ScriptType type;

      bool flagClient : 1;
      bool flagNet    : 1;


      friend class Environment;

   private:
      Script();
   };
}

#endif//ACSVM__Script_H__

