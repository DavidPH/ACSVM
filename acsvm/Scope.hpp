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


      GlobalScope(Environment *env, Word id);
      ~GlobalScope();

      void exec();

      HubScope *getHubScope(Word id);

      bool hasActiveThread();

      Environment *const env;
      Word         const id;

      Array arrV[ArrC];
      Word  regV[RegC];

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


      HubScope(GlobalScope *global, Word id);
      ~HubScope();

      void exec();

      MapScope *getMapScope(Word id);

      bool hasActiveThread();

      GlobalScope *const global;
      Word         const id;

      Array arrV[ArrC];
      Word  regV[RegC];

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
      MapScope(HubScope *hub, Word id);
      ~MapScope();

      void addModule(Module *module);

      void exec();

      Script *findScript(ScriptName name);
      Script *findScript(String *name);
      Script *findScript(Word name);

      ModuleScope *getModuleScope(Module *module);

      bool hasActiveThread();

      bool isScriptActive(Script *script);

      void scriptPause(Script *script);
      void scriptStart(Script *script, Word const *argV, Word argC);
      void scriptStartForced(Script *script, Word const *argV, Word argC);
      Word scriptStartResult(Script *script, Word const *argV, Word argC);
      void scriptStop(Script *script);

      HubScope *const hub;
      Word      const id;

      ListLink<ScriptAction> scriptAction;
      ListLink<Thread>       threadActive;

      bool active;

   protected:
      void freeThread(Thread *thread);

   private:
      struct PrivData;

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


      ModuleScope(MapScope *map, Module *module);
      ~ModuleScope();

      void import();

      MapScope *const map;
      Module   *const module;

      Array *arrV[ArrC];
      Word  *regV[RegC];

   private:
      Array selfArrV[ArrC];
      Word  selfRegV[RegC];
   };
}

#endif//ACSVM__Scope_H__

