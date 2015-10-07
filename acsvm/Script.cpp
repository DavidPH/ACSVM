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

#include "Script.hpp"

#include "Environ.hpp"
#include "Module.hpp"


//----------------------------------------------------------------------------|
// Extern Fumnctions                                                          |
//

namespace ACSVM
{
   //
   // Script constructor
   //
   Script::Script() :
      module{nullptr},

      envNext{this},
      envPrev{this}
   {
   }

   //
   // Script constructor
   //
   Script::Script(Module *module_) :
      module{module_},

      envNext{module->env->getScriptHead()},
      envPrev{envNext->envPrev},
      nameStr{nullptr},
      argNum {0},
      arrNum {0},
      codeIdx{0},
      flags  {0},
      nameInt{0},
      regNum {module->env->scriptRegNum},
      type   {ScriptType::Closed}
   {
      envNext->envPrev = this;
      envPrev->envNext = this;
   }

   //
   // Script destructor
   //
   Script::~Script()
   {
      envNext->envPrev = envPrev;
      envPrev->envNext = envNext;
   }
}

// EOF

