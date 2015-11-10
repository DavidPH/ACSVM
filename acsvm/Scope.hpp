//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Scope classes.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__Scope_H__
#define ACSVM__Scope_H__

#include "Array.hpp"
#include "List.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // GlobalScope
   //
   class GlobalScope
   {
   public:
      static constexpr std::size_t ArrC = 256;
      static constexpr std::size_t RegC = 256;


      GlobalScope(GlobalScope const &) = delete;
      GlobalScope(Environment *env, Word id);
      ~GlobalScope();

      void exec();

      HubScope *getHubScope(Word id);

      bool hasActiveThread();

      void lockStrings() const;

      void loadState(std::istream &in);

      void refStrings() const;

      void reset();

      void saveState(std::ostream &out) const;

      void unlockStrings() const;

      Environment *const env;
      Word         const id;

      Array arrV[ArrC];
      Word  regV[RegC];

      ListLink<GlobalScope>  hashLink;
      ListLink<ScriptAction> scriptAction;

      bool active;

   private:
      struct PrivData;

      PrivData *pd;
   };

   //
   // HubScope
   //
   class HubScope
   {
   public:
      static constexpr std::size_t ArrC = 256;
      static constexpr std::size_t RegC = 256;


      HubScope(HubScope const &) = delete;
      HubScope(GlobalScope *global, Word id);
      ~HubScope();

      void exec();

      MapScope *getMapScope(Word id);

      bool hasActiveThread();

      void lockStrings() const;

      void loadState(std::istream &in);

      void refStrings() const;

      void reset();

      void saveState(std::ostream &out) const;

      void unlockStrings() const;

      Environment *const env;
      GlobalScope *const global;
      Word         const id;

      Array arrV[ArrC];
      Word  regV[RegC];

      ListLink<HubScope>     hashLink;
      ListLink<ScriptAction> scriptAction;

      bool active;

   private:
      struct PrivData;

      PrivData *pd;
   };

   //
   // MapScope
   //
   class MapScope
   {
   public:
      using ScriptStartFunc = void (*)(Thread *);

      //
      // ScriptStartInfo
      //
      class ScriptStartInfo
      {
      public:
         ScriptStartInfo() : argV{nullptr}, func{nullptr}, info{nullptr}, argC{0} {}
         ScriptStartInfo(Word const *argV_, std::size_t argC_,
            ThreadInfo const *info_ = nullptr, ScriptStartFunc func_ = nullptr) :
            argV{argV_}, func{func_}, info{info_}, argC{argC_} {}

         Word       const *argV;
         ScriptStartFunc   func;
         ThreadInfo const *info;
         std::size_t       argC;
      };


      MapScope(MapScope const &) = delete;
      MapScope(HubScope *hub, Word id);
      ~MapScope();

      void addModule(Module *module);

      // This must be called after all modules have been added.
      void addModuleFinish();

      void exec();

      Script *findScript(ScriptName name);
      Script *findScript(String *name);
      Script *findScript(Word name);

      ModuleScope *getModuleScope(Module *module);

      String *getString(Word idx) const;

      bool hasActiveThread();

      bool isScriptActive(Script *script);

      void loadState(std::istream &in);

      void lockStrings() const;

      void refStrings() const;

      void reset();

      void saveState(std::ostream &out) const;

      bool scriptPause(Script *script);
      bool scriptPause(ScriptName name, ScopeID scope);
      bool scriptStart(Script *script, ScriptStartInfo const &info);
      bool scriptStart(ScriptName name, ScopeID scope, ScriptStartInfo const &info);
      bool scriptStartForced(Script *script, ScriptStartInfo const &info);
      bool scriptStartForced(ScriptName name, ScopeID scope, ScriptStartInfo const &info);
      Word scriptStartResult(Script *script, ScriptStartInfo const &info);
      Word scriptStartResult(ScriptName name, ScriptStartInfo const &info);
      Word scriptStartType(ScriptType type, ScriptStartInfo const &info);
      bool scriptStop(Script *script);
      bool scriptStop(ScriptName name, ScopeID scope);

      void unlockStrings() const;

      Environment *const env;
      HubScope    *const hub;
      Word         const id;

      ListLink<MapScope>     hashLink;
      ListLink<ScriptAction> scriptAction;
      ListLink<Thread>       threadActive;

      // Used for untagged string lookup.
      Module *module0;

      bool active;
      bool clampCallSpec;

   protected:
      void freeThread(Thread *thread);

   private:
      struct PrivData;

      void loadModules(std::istream &in);
      void loadThreads(std::istream &in);

      void saveModules(std::ostream &out) const;
      void saveThreads(std::ostream &out) const;

      PrivData *pd;
   };

   //
   // ModuleScope
   //
   class ModuleScope
   {
   public:
      static constexpr std::size_t ArrC = 256;
      static constexpr std::size_t RegC = 256;


      ModuleScope(ModuleScope const &) = delete;
      ModuleScope(MapScope *map, Module *module);
      ~ModuleScope();

      void import();

      void loadState(std::istream &in);

      void lockStrings() const;

      void refStrings() const;

      void saveState(std::ostream &out) const;

      void unlockStrings() const;

      Environment *const env;
      MapScope    *const map;
      Module      *const module;

      Array *arrV[ArrC];
      Word  *regV[RegC];

   private:
      Array selfArrV[ArrC];
      Word  selfRegV[RegC];
   };
}

#endif//ACSVM__Scope_H__

