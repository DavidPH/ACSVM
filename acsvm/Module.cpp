//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Module class.
//
//-----------------------------------------------------------------------------

#include "Module.hpp"

#include "Environ.hpp"
#include "Script.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace ACSVM
{
   //
   // ModuleName::operator == ModuleName
   //
   bool ModuleName::operator == (ModuleName const &name) const
   {
      if(i != name.i || p != name.p) return false;

      if(!s) return !name.s;
      if(!name.s) return false;
      return !std::strcmp(s.get(), name.s.get());
   }

   //
   // ModuleName::hash
   //
   std::size_t ModuleName::hash() const
   {
      return StrHash(s.get()) + std::hash<void*>()(p) + i;
   }

   //
   // Module constructor
   //
   Module::Module(Environment *env_, ModuleName &&name_) :
      env{env_},
      name{std::move(name_)},

      codeV    {nullptr}, codeC    {0},
      funcNameV{nullptr}, funcNameC{0},
      functionV{nullptr}, functionC{0},
      jumpV    {nullptr}, jumpC    {0},
      scrNameV {nullptr}, scrNameC {0},
      scriptV  {nullptr}, scriptC  {0},
      stringV  {nullptr}, stringC  {0},

      loaded{false}
   {
   }

   //
   // Module destructor
   //
   Module::~Module()
   {
      if(env->module0 == this)
         env->module0 = nullptr;

      reset();
   }

   //
   // Module::allocCodeV
   //
   void Module::allocCodeV(std::size_t count)
   {
      if(codeV) freeCodeV();

      codeC = count;
      codeV = new std::uint32_t[codeC];
   }

   //
   // Module::allocScrNameV
   //
   void Module::allocScrNameV(std::size_t count)
   {
      if(scrNameV) freeScrNameV();

      scrNameC = count;
      scrNameV = new String*[scrNameC]{};
   }

   //
   // Module::allocScriptV
   //
   void Module::allocScriptV(std::size_t count)
   {
      if(scriptV) freeScriptV();

      scriptC = count;
      scriptV = static_cast<Script *>(operator new(sizeof(Script) * scriptC));

      for(Script *itr = scriptV, *end = itr + scriptC; itr != end; ++itr)
         new(itr) Script(this);
   }

   //
   // Module::allocStringV
   //
   void Module::allocStringV(std::size_t count)
   {
      if(stringV) freeStringV();

      stringC = count;
      stringV = new String*[stringC];
   }

   //
   // Module::freeCodeV
   //
   void Module::freeCodeV()
   {
      if(!codeV) return;

      delete[] codeV;
      codeV = nullptr;
      codeC = 0;
   }

   //
   // Module::freeScrNameV
   //
   void Module::freeScrNameV()
   {
      if(!scrNameV) return;

      delete[] scrNameV;
      scrNameV = nullptr;
      scrNameC = 0;
   }

   //
   // Module::freeScriptV
   //
   void Module::freeScriptV()
   {
      if(!scriptV) return;

      for(Script *itr = scriptV + scriptC; itr != scriptV;)
         (--itr)->~Script();

      operator delete(scriptV);
      scriptV = nullptr;
      scriptC = 0;
   }

   //
   // Module::freeStringV
   //
   void Module::freeStringV()
   {
      if(!stringV) return;

      delete[] stringV;
      stringV = nullptr;
      stringC = 0;
   }

   //
   // Module::reset
   //
   void Module::reset()
   {
      freeCodeV();
    //freeFunctionV();
    //freeJumpV();
      freeScrNameV();
      freeScriptV();
      freeStringV();

      loaded = false;
   }
}


// EOF

