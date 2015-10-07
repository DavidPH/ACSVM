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

      codes{nullptr},
      codeNum{0},

      functions{nullptr},
      functionNum{0},

      jumps{nullptr},
      jumpNum{0},

      scripts{nullptr},
      scriptNum{0},

      strings{nullptr},
      stringNum{0},

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
   // Module::allocCodes
   //
   void Module::allocCodes()
   {
      if(codes) freeCodes();

      codes = new std::uint32_t[codeNum];
   }

   //
   // Module::allocScripts
   //
   void Module::allocScripts()
   {
      if(scripts) freeScripts();

      scripts = static_cast<Script *>(operator new(sizeof(Script) * scriptNum));

      for(Script *itr = scripts, *end = itr + scriptNum; itr != end; ++itr)
         new(itr) Script(this);
   }

   //
   // Module::allocStrings
   //
   void Module::allocStrings()
   {
      if(strings) freeStrings();

      strings = new String*[stringNum];
   }

   //
   // Module::freeCodes
   //
   void Module::freeCodes()
   {
      if(!codes) return;

      delete[] codes;
      codes = nullptr;
   }

   //
   // Module::freeScripts
   //
   void Module::freeScripts()
   {
      if(!scripts) return;

      for(Script *itr = scripts + scriptNum; itr != scripts;)
         (--itr)->~Script();

      operator delete(scripts);
      scripts = nullptr;
   }

   //
   // Module::freeStrings
   //
   void Module::freeStrings()
   {
      if(!strings) return;

      delete[] strings;
      strings = nullptr;
   }

   //
   // Module::reset
   //
   void Module::reset()
   {
      freeCodes();     codeNum     = 0;
    //freeFunctions(); functionNum = 0;
    //freeJumps();     jumpNum     = 0;
      freeScripts();   scriptNum   = 0;
      freeStrings();   stringNum   = 0;

      loaded = false;
   }
}


// EOF

