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
// Static Functions                                                           |
//

namespace ACSVM
{
   //
   // FreeV
   //
   template<typename T>
   static void FreeV(T *&dataV, std::size_t &dataC)
   {
      if(!dataV) return;

      for(T *itr = dataV + dataC; itr != dataV;)
         (--itr)->~T();

      ::operator delete(dataV);
      dataV = nullptr;
      dataC = 0;
   }

   //
   // AllocV
   //
   template<typename T, typename... Args>
   static void AllocV(T *&dataV, std::size_t &dataC, std::size_t count, Args const &...args)
   {
      if(dataV) FreeV<T>(dataV, dataC);

      dataC = count;
      dataV = static_cast<T *>(::operator new(sizeof(T) * dataC));

      for(T *itr = dataV, *end = itr + dataC; itr != end; ++itr)
         new(itr) T{args...};
   }
}


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
   // Module::alloc*
   //

   void Module::allocCodeV(std::size_t n)     {AllocV(codeV,     codeC,     n);}
   void Module::allocFuncNameV(std::size_t n) {AllocV(funcNameV, funcNameC, n);}
   void Module::allocFunctionV(std::size_t n) {AllocV(functionV, functionC, n);}
   void Module::allocJumpV(std::size_t n)     {AllocV(jumpV,     jumpC,     n);}
   void Module::allocScrNameV(std::size_t n)  {AllocV(scrNameV,  scrNameC,  n);}
   void Module::allocScriptV(std::size_t n)   {AllocV(scriptV,   scriptC,   n, this);}
   void Module::allocStringV(std::size_t n)   {AllocV(stringV,   stringC,   n);}

   //
   // Module::free*
   //

   void Module::freeCodeV()     {FreeV(codeV,     codeC);}
   void Module::freeFuncNameV() {FreeV(funcNameV, funcNameC);}
   void Module::freeFunctionV() {FreeV(functionV, functionC);}
   void Module::freeJumpV()     {FreeV(jumpV,     jumpC);}
   void Module::freeScrNameV()  {FreeV(scrNameV,  scrNameC);}
   void Module::freeScriptV()   {FreeV(scriptV,   scriptC);}
   void Module::freeStringV()   {FreeV(stringV,   stringC);}

   //
   // Module::reset
   //
   void Module::reset()
   {
      // Unload locally defined functions from env.
      for(Function **func = functionV, **end = func + functionC; func != end; ++func)
      {
         if(*func && (*func)->module == this)
            env->freeFunction(*func);
      }

      freeCodeV();
      freeFuncNameV();
      freeFunctionV();
      freeJumpV();
      freeScrNameV();
      freeScriptV();
      freeStringV();

      loaded = false;
   }
}


// EOF

