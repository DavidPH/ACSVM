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

#ifndef ACSVM__Environment_H__
#define ACSVM__Environment_H__

#include "List.hpp"
#include "String.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
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

      void addCodeDataACS0(Word code, CodeDataACS0 &&data);
      void addFuncDataACS0(Word func, FuncDataACS0 &&data);

      virtual bool callFunc(Thread *thread, Word func, Word const *argV, Word argC);
      Word callSpec(Thread *thread, Word spec, Word const *argV, Word argC);

      // Function to check if a lock can be opened. Default behavior is to
      // always return false.
      virtual bool checkLock(Thread *thread, Word lock, bool door);

      // Function to check tags. Must return true to indicate script should
      // continue. Default behavior is to always return false.
      virtual bool checkTag(Word type, Word tag);

      void collectStrings();

      void deferAction(ScriptAction &&action);

      virtual void exec();

      CodeDataACS0 const *findCodeDataACS0(Word code);
      FuncDataACS0 const *findFuncDataACS0(Word func);

      Module *findModule(ModuleName const &name) const;

      // Used by Module when unloading.
      void freeFunction(Function *func);

      void freeGlobalScope(GlobalScope *scope);

      void freeModule(Module *module);

      void freeThread(Thread *thread);

      CodeData const *getCodeData(Code code);

      Thread *getFreeThread();

      Function *getFunction(Word idx) {return idx < funcC ? funcV[idx] : nullptr;}

      Function *getFunction(Module *module, String *name);

      GlobalScope *getGlobalScope(Word id);

      // Gets the named module, loading it if needed.
      Module *getModule(ModuleName const &name);
      Module *getModule(ModuleName &&name);

      ModuleName getModuleName(char const *str);
      virtual ModuleName getModuleName(char const *str, std::size_t len);

      String *getString(Word idx) {return &stringTable[~idx];}

      String *getString(char const *first, char const *last)
         {return &stringTable[{first, last}];}

      String *getString(char const *str, std::size_t len)
         {return &stringTable[{str, len}];}

      String *getString(StringData const *data)
         {return data ? &stringTable[*data] : nullptr;}

      // Returns true if any contained scope is active and has an active thread.
      bool hasActiveThread() const;

      virtual void loadState(std::istream &in);

      // Prints an array to a print buffer. Default behavior is PrintArrayChar.
      virtual void printArray(PrintBuf &buf, Array const &array, Word index, Word limit);

      // Function to print Kill instructions. Default behavior is to print
      // message to stderr.
      virtual void printKill(Thread *thread, Word type, Word data);

      // Deserializes a ModuleName. Default behavior is to load s and i.
      virtual ModuleName readModuleName(std::istream &in) const;

      Script *readScript(std::istream &in) const;
      ScriptAction *readScriptAction(std::istream &in) const;
      void readScriptActions(std::istream &in, ListLink<ScriptAction> &out) const;
      ScriptName readScriptName(std::istream &in) const;
      String *readString(std::istream &in) const;

      virtual void refStrings();

      virtual void resetStrings();

      virtual void saveState(std::ostream &out) const;

      // Serializes a ModuleName. Default behavior is to save s and i.
      virtual void writeModuleName(std::ostream &out, ModuleName const &name) const;

      void writeScript(std::ostream &out, Script *in) const;
      void writeScriptAction(std::ostream &out, ScriptAction const *in) const;
      void writeScriptActions(std::ostream &out, ListLink<ScriptAction> const &in) const;
      void writeScriptName(std::ostream &out, ScriptName const &in) const;
      void writeString(std::ostream &out, String const *in) const;

      StringTable stringTable;

      // Number of branches allowed per call to Thread::exec. Default of 0
      // means no limit.
      Word branchLimit;

      // Default number of script variables. Default is 20.
      Word scriptLocRegC;


      // Prints an array to a print buffer, truncating elements of the array to
      // fit char.
      static void PrintArrayChar(PrintBuf &buf, Array const &array, Word index, Word limit);

      // Prints an array to a print buffer, converting the array as a UTF-32
      // sequence into a UTF-8 sequence.
      static void PrintArrayUTF8(PrintBuf &buf, Array const &array, Word index, Word limit);

      static constexpr Word ScriptLocRegCDefault = 20;

   protected:
      virtual Thread *allocThread();

      // Called by callSpec after processing arguments. Default behavior is to
      // do nothing and return 0.
      virtual Word callSpecImpl(Thread *thread, Word spec, Word const *argV, Word argC);

      virtual void loadModule(Module *module) = 0;

      ListLink<ScriptAction> scriptAction;
      ListLink<Thread>       threadFree;

      Function  **funcV;
      std::size_t funcC;

   private:
      struct PrivData;

      void loadFunctions(std::istream &in);
      void loadGlobalScopes(std::istream &in);
      void loadScriptActions(std::istream &in);
      void loadStringTable(std::istream &in);

      void saveFunctions(std::ostream &out) const;
      void saveGlobalScopes(std::ostream &out) const;
      void saveScriptActions(std::ostream &out) const;
      void saveStringTable(std::ostream &out) const;

      PrivData *pd;
   };
}

#endif//ACSVM__Environment_H__

