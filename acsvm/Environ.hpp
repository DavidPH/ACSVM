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

#ifndef ACSVM__Environ_H__
#define ACSVM__Environ_H__

#include "List.hpp"
#include "String.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   using CallFunc = bool (*)(Thread *thread, Word const *argv, Word argc);
   using CallSpec = Word (*)(Thread *thread, Word spec, Word const *argv, Word argc);

   //
   // Environment
   //
   // Represents an entire ACS environment.
   //
   class Environment
   {
   public:
      Environment();
      virtual ~Environment();

      Word addCallFunc(CallFunc func);
      void addCallSpec(Word idx, CallSpec spec);

      void addCodeDataACS0(Word code, CodeDataACS0 &&data);
      void addFuncDataACS0(Word func, FuncDataACS0 &&data);

      // Function to check tags. Must return true to indicate script should
      // continue. Default behavior is to always return false.
      virtual bool checkTag(Word type, Word tag);

      void exec();

      CodeDataACS0 const *findCodeDataACS0(Word code);
      FuncDataACS0 const *findFuncDataACS0(Word func);

      // Used by Module when unloading.
      void freeFunction(Function *func);

      void freeThread(Thread *thread);

      CallFunc getCallFunc(Word func) {return tableCallFunc[func];}
      CallSpec getCallSpec(Word spec);

      CodeData const *getCodeData(Code code);

      Thread *getFreeThread();

      Function *getFunction(Word idx) {return idx < funcC ? funcV[idx] : nullptr;}

      Function *getFunction(Module *module, String *name);

      GlobalScope *getGlobalScope(std::size_t id);

      // Gets the named module, loading it if needed.
      Module *getModule(ModuleName const &name);
      Module *getModule(ModuleName &&name);

      ModuleName getModuleName(char const *str);
      virtual ModuleName getModuleName(char const *str, std::size_t len);

      Script *getScriptHead();

      String *getString(Word idx)
      {
         if(idx & 0x80000000)
            return &stringTable[~idx];
         else
            return &stringTable[idx]; // TODO: Read module 0.
      }

      String *getString(char const *first, char const *last)
         {return &stringTable[{first, last}];}

      String *getString(char const *str, std::size_t len)
         {return &stringTable[{str, len}];}

      // Prints an array to a print buffer. Default behavior is to convert the
      // sequence as UTF-32 to UTF-8.
      virtual void printArray(PrintBuf &buf, Array const &array, Word index, Word limit);

      // Function to print Kill instructions. Default behavior is to print
      // message to stderr.
      virtual void printKill(Thread *thread, Word type, Word data);

      StringTable stringTable;

      // Function to return from getCallSpec for unknown specials.
      CallSpec defCallSpec;

      // First loaded module.
      Module *module0;

      // Default number of script variables. Default is 20.
      Word scriptLocRegC;


      static Word CallSpecDefault(Thread *thread, Word spec, Word const *argv, Word argc);

      static constexpr Word ScriptLocRegCDefault = 20;

   protected:
      virtual Thread *allocThread();

      virtual void loadModule(Module *module) = 0;

      ListLink<Thread> threadFree;

      CallFunc *tableCallFunc;

      Function  **funcV;
      std::size_t funcC;

   private:
      struct PrivData;

      PrivData *pd;
   };
}

#endif//ACSVM__Environ_H__

