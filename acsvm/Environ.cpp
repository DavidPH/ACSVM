//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Environment class.
//
//-----------------------------------------------------------------------------

#include "Environ.hpp"

#include "CallFunc.hpp"
#include "Code.hpp"
#include "CodeData.hpp"
#include "Function.hpp"
#include "Module.hpp"
#include "PrintBuf.hpp"
#include "Scope.hpp"
#include "Script.hpp"
#include "Thread.hpp"

#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // Environment::PrivData
   //
   struct Environment::PrivData
   {
      struct NameEqual
      {
         bool operator () (ModuleName const *l, ModuleName const *r) const
            {return *l == *r;}
      };

      struct NameHash
      {
         std::size_t operator () (ModuleName const *name) const
            {return name->hash();}
      };

      struct FuncPairHash
      {
         std::size_t operator () (std::pair<ModuleName, String *> const &pair) const
            {return pair.first.hash() + pair.second->hash;}
      };

      std::unordered_map<std::pair<ModuleName, String *>, Word, FuncPairHash> functionTab;

      // Reserve index 0 as no function.
      std::vector<Function *> functionVec{nullptr};

      std::unordered_map<std::size_t, GlobalScope> globalScopes;

      std::vector<CallFunc> tableCallFunc
      {
         #define ACSVM_FuncList(name) \
            CallFunc_Func_##name,
         #include "CodeList.hpp"

         CallFunc_Func_Nop
      };

      std::unordered_map<Word, CallSpec> tableCallSpec;

      std::unordered_map<Word, CodeDataACS0> tableCodeDataACS0
      {
         #define ACSVM_CodeListACS0(name, code, args, transCode, stackArgC, transFunc) \
            {code, {CodeACS0::name, args, Code::transCode, stackArgC, Func::transFunc}},
         #include "CodeList.hpp"
      };

      std::unordered_map<Word, FuncDataACS0> tableFuncDataACS0
      {
         #define ACSVM_FuncListACS0(name, func, transFunc, ...) \
            {func, {FuncACS0::name, Func::transFunc, __VA_ARGS__}},
         #include "CodeList.hpp"
      };

      std::unordered_map<ModuleName const *, std::unique_ptr<Module>,
         NameHash, NameEqual> tableModule;

      Script scriptHead;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace ACSVM
{
   //
   // Environment constructor
   //
   Environment::Environment() :
      defCallSpec  {CallSpecDefault},
      module0      {nullptr},
      scriptLocRegC{ScriptLocRegCDefault},

      tableCallFunc{nullptr},

      funcV{nullptr},
      funcC{0},

      pd{new PrivData}
   {
      tableCallFunc = pd->tableCallFunc.data();

      funcV = pd->functionVec.data();
      funcC = pd->functionVec.size();
   }

   //
   // Environment destructor
   //
   Environment::~Environment()
   {
      while(!pd->tableModule.empty())
         pd->tableModule.erase(pd->tableModule.begin());

      delete pd;

      // Deallocate threads. Do this after scopes have been destructed.
      while(threadFree.next->obj)
         delete threadFree.next->obj;
   }

   //
   // Environment::addCallFunc
   //
   Word Environment::addCallFunc(CallFunc func)
   {
      pd->tableCallFunc.push_back(func);
      tableCallFunc = pd->tableCallFunc.data();
      return pd->tableCallFunc.size() - 1;
   }

   //
   // Environment::addCallSpec
   //
   void Environment::addCallSpec(Word idx, CallSpec spec)
   {
      pd->tableCallSpec[idx] = spec;
   }

   //
   // Environment::addCodeDataACS0
   //
   void Environment::addCodeDataACS0(Word code, CodeDataACS0 &&data)
   {
      auto itr = pd->tableCodeDataACS0.find(code);
      if(itr == pd->tableCodeDataACS0.end())
         pd->tableCodeDataACS0.emplace(code, std::move(data));
      else
         itr->second = std::move(data);
   }

   //
   // Environment::addFuncDataACS0
   //
   void Environment::addFuncDataACS0(Word func, FuncDataACS0 &&data)
   {
      auto itr = pd->tableFuncDataACS0.find(func);
      if(itr == pd->tableFuncDataACS0.end())
         pd->tableFuncDataACS0.emplace(func, std::move(data));
      else
         itr->second = std::move(data);
   }

   //
   // Environment::allocThread
   //
   Thread *Environment::allocThread()
   {
      return new Thread;
   }

   //
   // Environment::checkTag
   //
   bool Environment::checkTag(Word, Word)
   {
      return false;
   }

   //
   // Environment::exec
   //
   void Environment::exec()
   {
      for(auto &itr : pd->globalScopes)
      {
         if(itr.second.active)
            itr.second.exec();
      }
   }

   //
   // Environment::findCodeDataACS0
   //
   CodeDataACS0 const *Environment::findCodeDataACS0(Word code)
   {
      auto itr = pd->tableCodeDataACS0.find(code);
      return itr == pd->tableCodeDataACS0.end() ? nullptr : &itr->second;
   }

   //
   // Environment::findFuncDataACS0
   //
   FuncDataACS0 const *Environment::findFuncDataACS0(Word func)
   {
      auto itr = pd->tableFuncDataACS0.find(func);
      return itr == pd->tableFuncDataACS0.end() ? nullptr : &itr->second;
   }

   //
   // Environment::freeFunction
   //
   void Environment::freeFunction(Function *func)
   {
      // Null every reference to this function in every Module.
      // O(N*M) is not very nice, but that can be fixed if/when it comes up.
      for(auto &moduleItr : pd->tableModule)
      {
         auto &module = moduleItr.second;

         for(Function *&funcItr : module->functionV)
         {
            if(funcItr == func)
               funcItr = nullptr;
         }
      }

      pd->functionVec[func->idx] = nullptr;
      delete func;
   }

   //
   // Environment::freeThread
   //
   void Environment::freeThread(Thread *thread)
   {
      thread->threadLink.relink(&threadFree);
   }

   //
   // Environment::getCallSpec
   //
   CallSpec Environment::getCallSpec(Word spec)
   {
      auto itr = pd->tableCallSpec.find(spec);
      return itr == pd->tableCallSpec.end() ? defCallSpec : itr->second;
   }

   //
   // Environment::getCodeData
   //
   CodeData const *Environment::getCodeData(Code code)
   {
      switch(code)
      {
         #define ACSVM_CodeList(name, argc) case Code::name: \
            {static CodeData const data{Code::name, argc}; return &data;}
         #include "CodeList.hpp"

      default:
      case Code::None:
         static CodeData const dataNone{Code::None, 0};
         return &dataNone;
      }
   }

   //
   // Environment::getFreeThread
   //
   Thread *Environment::getFreeThread()
   {
      if(threadFree.next->obj)
      {
         Thread *thread = threadFree.next->obj;
         thread->threadLink.unlink();
         return thread;
      }
      else
         return allocThread();
   }

   //
   // Environment::getFunction
   //
   Function *Environment::getFunction(Module *module, String *funcName)
   {
      if(funcName)
      {
         auto &idx = pd->functionTab[{module->name, funcName}];

         if(!idx)
         {
            idx = pd->functionVec.size();
            pd->functionVec.emplace_back();
            funcV = pd->functionVec.data();
            funcC = pd->functionVec.size();
         }

         auto &ptr = pd->functionVec[idx];

         if(!ptr)
            ptr = new Function{module, funcName, idx};

         return ptr;
      }
      else
         return new Function{module, nullptr, 0};
   }

   //
   // Environment::getGlobalScope
   //
   GlobalScope *Environment::getGlobalScope(std::size_t id)
   {
      return &pd->globalScopes.emplace(std::piecewise_construct,
         std::make_tuple(id), std::make_tuple(this)).first->second;
   }

   //
   // Environment::getModule
   //
   Module *Environment::getModule(ModuleName &&name)
   {
      auto itr = pd->tableModule.find(&name);

      if(itr == pd->tableModule.end())
      {
         std::unique_ptr<Module> module{new Module(this, std::move(name))};
         loadModule(module.get());
         ModuleName const *namePtr = &module->name;
         itr = pd->tableModule.emplace(namePtr, std::move(module)).first;
      }
      else
      {
         if(!itr->second->loaded)
            loadModule(itr->second.get());
      }

      return itr->second.get();
   }

   //
   // Environment::getModuleName
   //
   ModuleName Environment::getModuleName(char const *str)
   {
      return getModuleName(str, std::strlen(str));
   }

   //
   // Environment::getModuleName
   //
   ModuleName Environment::getModuleName(char const *str, std::size_t)
   {
      return {StrDup(str), nullptr, 0};
   }

   //
   // Environment::getScriptHead
   //
   Script *Environment::getScriptHead()
   {
      return &pd->scriptHead;
   }

   //
   // Environment::printArray
   //
   void Environment::printArray(PrintBuf &buf, Array const &array, Word index, Word limit)
   {
      // Calculate output length and end index.
      std::size_t len = 0;
      Word        end;
      for(Word &itr = end = index; itr - index != limit; ++itr)
      {
         Word c = array.find(itr);
         if(!c) break;
         if(c > 0x10FFFF) c = 0xFFFD;

              if(c <= 0x007F) len += 1;
         else if(c <= 0x07FF) len += 2;
         else if(c <= 0xFFFF) len += 3;
         else                 len += 4;
      }

      // Acquire output buffer.
      buf.reserve(len);
      char *s = buf.getBuf(len);

      // Convert UTF-32 sequence to UTF-8.
      for(Word itr = index; itr != end; ++itr)
      {
         Word c = array.find(itr);
         if(c > 0x10FFFF) c = 0xFFFD;

         if(c <= 0x7F)   {*s++ = 0x00 | (c >>  0); goto put0;}
         if(c <= 0x7FF)  {*s++ = 0xC0 | (c >>  6); goto put1;}
         if(c <= 0xFFFF) {*s++ = 0xE0 | (c >> 12); goto put2;}
                         {*s++ = 0xF0 | (c >> 18); goto put3;}

         put3: *s++ = 0x80 | ((c >> 12) & 0x3F);
         put2: *s++ = 0x80 | ((c >>  6) & 0x3F);
         put1: *s++ = 0x80 | ((c >>  0) & 0x3F);
         put0:;
      }
   }

   //
   // Environment::printKill
   //
   void Environment::printKill(Thread *thread, Word type, Word data)
   {
      std::cerr << "ACSVM ERROR: Kill " << type << ':' << data
         << " at " << (thread->codePtr - thread->module->codeV.data() - 3) << '\n';
   }

   //
   // Environment::CallSpecDefault
   //
   Word Environment::CallSpecDefault(Thread *, Word, Word const *, Word)
   {
      return 0;
   }
}

// EOF

