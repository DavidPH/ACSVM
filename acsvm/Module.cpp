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
#include "Function.hpp"
#include "Jump.hpp"
#include "Script.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace ACSVM
{
   //
   // ModuleName copy constructor
   //
   ModuleName::ModuleName(ModuleName const &name) :
      s{StrDup(name.s.get())}, p{name.p}, i{name.i}
   {
   }

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
   // Module::reset
   //
   void Module::reset()
   {
      // Unload locally defined functions from env.
      for(Function *&func : functionV)
      {
         if(func && func->module == this)
            env->freeFunction(func);
      }

      codeV.free();
      funcNameV.free();
      functionV.free();
      jumpV.free();
      scrNameV.free();
      scriptV.free();
      stringV.free();

      loaded = false;
   }
}


// EOF

