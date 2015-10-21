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

      name{},

      envNext{module->env->getScriptHead()},
      envPrev{envNext->envPrev},
      argC   {0},
      codeIdx{0},
      flags  {0},
      locArrC{0},
      locRegC{module->env->scriptLocRegC},
      type   {ScriptType::Closed},

      flagClient{false},
      flagNet   {false}
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

