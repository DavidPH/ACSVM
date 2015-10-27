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

#include "Scope.hpp"

#include "Action.hpp"
#include "BinaryIO.hpp"
#include "Environ.hpp"
#include "HashMapFixed.hpp"
#include "Init.hpp"
#include "Module.hpp"
#include "Script.hpp"
#include "Thread.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // GlobalScope::PrivData
   //
   struct GlobalScope::PrivData
   {
      std::unordered_map<Word, HubScope> hubScopes;
   };

   //
   // HubScope::PrivData
   //
   struct HubScope::PrivData
   {
      std::unordered_map<Word, MapScope> mapScopes;
   };

   //
   // MapScope::PrivData
   //
   struct MapScope::PrivData
   {
      HashMapFixed<Module *, ModuleScope> moduleScopes;

      std::unordered_set<Module *> modules;

      HashMapFixed<Word,     Script *> scriptInt;
      HashMapFixed<String *, Script *> scriptStr;

      HashMapFixed<Script *, Thread *> scriptThread;
   };
}


//----------------------------------------------------------------------------|
// Extern Objects                                                             |
//

namespace ACSVM
{
   constexpr std::size_t ModuleScope::ArrC;
   constexpr std::size_t ModuleScope::RegC;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace ACSVM
{
   //
   // GlobalScope constructor
   //
   GlobalScope::GlobalScope(Environment *env_, Word id_) :
      env{env_},
      id {id_},

      arrV{},
      regV{},

      active{false},

      pd{new PrivData}
   {
   }

   //
   // GlobalScope destructor
   //
   GlobalScope::~GlobalScope()
   {
      delete pd;
   }

   //
   // GlobalScope::exec
   //
   void GlobalScope::exec()
   {
      // Delegate deferred script actions.
      for(auto itr = scriptAction.next, next = itr->next; itr->obj; itr = next, next = itr->next)
      {
         auto lookup = pd->hubScopes.find(itr->obj->id.global);
         if(lookup != pd->hubScopes.end() && lookup->second.active)
            itr->relink(&lookup->second.scriptAction);
      }

      for(auto &itr : pd->hubScopes)
      {
         if(itr.second.active)
            itr.second.exec();
      }
   }

   //
   // GlobalScope::getHubScope
   //
   HubScope *GlobalScope::getHubScope(Word hubID)
   {
      return &pd->hubScopes.emplace(std::piecewise_construct,
         std::make_tuple(hubID), std::make_tuple(this, hubID)).first->second;
   }

   //
   // GlobalScope::hasActiveThread
   //
   bool GlobalScope::hasActiveThread()
   {
      for(auto &itr : pd->hubScopes)
      {
         if(itr.second.active && itr.second.hasActiveThread())
            return true;
      }

      return false;
   }

   //
   // GlobalScope::loadState
   //
   void GlobalScope::loadState(std::istream &in)
   {
      reset();

      for(auto &arr : arrV)
         arr.loadState(in);

      for(auto &reg : regV)
         reg = ReadVLN<Word>(in);

      env->readScriptActions(in, scriptAction);

      active = in.get();

      for(auto n = ReadVLN<std::size_t>(in); n--;)
         getHubScope(ReadVLN<Word>(in))->loadState(in);
   }

   //
   // GlobalScope::reset
   //
   void GlobalScope::reset()
   {
      while(scriptAction.next->obj)
         delete scriptAction.next->obj;

      pd->hubScopes.clear();
   }

   //
   // GlobalScope::saveState
   //
   void GlobalScope::saveState(std::ostream &out) const
   {
      for(auto &arr : arrV)
         arr.saveState(out);

      for(auto &reg : regV)
         WriteVLN(out, reg);

      env->writeScriptActions(out, scriptAction);

      out.put(active ? '\1' : '\0');

      WriteVLN(out, pd->hubScopes.size());
      for(auto &itr : pd->hubScopes)
      {
         WriteVLN(out, itr.first);
         itr.second.saveState(out);
      }
   }

   //
   // HubScope constructor
   //
   HubScope::HubScope(GlobalScope *global_, Word id_) :
      global{global_},
      id    {id_},

      arrV{},
      regV{},

      active{false},

      pd{new PrivData}
   {
   }

   //
   // HubScope destructor
   //
   HubScope::~HubScope()
   {
      delete pd;
   }

   //
   // HubScope::exec
   //
   void HubScope::exec()
   {
      // Delegate deferred script actions.
      for(auto itr = scriptAction.next, next = itr->next; itr->obj; itr = next, next = itr->next)
      {
         auto lookup = pd->mapScopes.find(itr->obj->id.global);
         if(lookup != pd->mapScopes.end() && lookup->second.active)
            itr->relink(&lookup->second.scriptAction);
      }

      for(auto &itr : pd->mapScopes)
      {
         if(itr.second.active)
            itr.second.exec();
      }
   }

   //
   // HubScope::getMapScope
   //
   MapScope *HubScope::getMapScope(Word mapID)
   {
      return &pd->mapScopes.emplace(std::piecewise_construct,
         std::make_tuple(mapID), std::make_tuple(this, mapID)).first->second;
   }

   //
   // HubScope::hasActiveThread
   //
   bool HubScope::hasActiveThread()
   {
      for(auto &itr : pd->mapScopes)
      {
         if(itr.second.active && itr.second.hasActiveThread())
            return true;
      }

      return false;
   }

   //
   // HubScope::loadState
   //
   void HubScope::loadState(std::istream &in)
   {
      reset();

      for(auto &arr : arrV)
         arr.loadState(in);

      for(auto &reg : regV)
         reg = ReadVLN<Word>(in);

      global->env->readScriptActions(in, scriptAction);

      active = in.get();

      for(auto n = ReadVLN<std::size_t>(in); n--;)
         getMapScope(ReadVLN<Word>(in))->loadState(in);
   }

   //
   // HubScope::reset
   //
   void HubScope::reset()
   {
      while(scriptAction.next->obj)
         delete scriptAction.next->obj;

      pd->mapScopes.clear();
   }

   //
   // HubScope::saveState
   //
   void HubScope::saveState(std::ostream &out) const
   {
      for(auto &arr : arrV)
         arr.saveState(out);

      for(auto &reg : regV)
         WriteVLN(out, reg);

      global->env->writeScriptActions(out, scriptAction);

      out.put(active ? '\1' : '\0');

      WriteVLN(out, pd->mapScopes.size());
      for(auto &itr : pd->mapScopes)
      {
         WriteVLN(out, itr.first);
         itr.second.saveState(out);
      }
   }

   //
   // MapScope constructor
   //
   MapScope::MapScope(HubScope *hub_, Word id_) :
      env{hub_->global->env},
      hub{hub_},
      id {id_},

      active{false},

      pd{new PrivData}
   {
   }

   //
   // MapScope destructor
   //
   MapScope::~MapScope()
   {
      reset();
      delete pd;
   }

   //
   // MapScope::addModule
   //
   void MapScope::addModule(Module *module)
   {
      if(pd->modules.count(module)) return;

      pd->modules.insert(module);

      for(auto &import : module->importV)
         addModule(import);
   }

   //
   // MapScope::addModuleFinish
   //
   void MapScope::addModuleFinish()
   {
      std::size_t scriptThrC = 0;
      std::size_t scriptIntC = 0;
      std::size_t scriptStrC = 0;

      // Count scripts.
      for(auto &module : pd->modules)
      {
         for(auto &script : module->scriptV)
         {
            ++scriptThrC;
            if(script.name.s)
               ++scriptStrC;
            else
               ++scriptIntC;
         }
      }

      // Create lookup tables.

      pd->moduleScopes.alloc(pd->modules.size());
      pd->scriptInt.alloc(scriptIntC);
      pd->scriptStr.alloc(scriptStrC);
      pd->scriptThread.alloc(scriptThrC);

      auto moduleItr    = pd->moduleScopes.begin();
      auto scriptIntItr = pd->scriptInt.begin();
      auto scriptStrItr = pd->scriptStr.begin();
      auto scriptThrItr = pd->scriptThread.begin();

      for(auto &module : pd->modules)
      {
         using ElemMod = HashMapFixed<Module *, ModuleScope>::Elem;

         new(moduleItr++) ElemMod{module, {this, module}, nullptr};

         for(auto &script : module->scriptV)
         {
            using ElemInt = HashMapFixed<Word,     Script *>::Elem;
            using ElemStr = HashMapFixed<String *, Script *>::Elem;
            using ElemThr = HashMapFixed<Script *, Thread *>::Elem;

            new(scriptThrItr++) ElemThr{&script, nullptr, nullptr};

            if(script.name.s)
               new(scriptStrItr++) ElemStr{script.name.s, &script, nullptr};
            else
               new(scriptIntItr++) ElemInt{script.name.i, &script, nullptr};
         }
      }

      pd->moduleScopes.build();
      pd->scriptInt.build();
      pd->scriptStr.build();
      pd->scriptThread.build();

      for(auto &moduleScope : pd->moduleScopes)
         moduleScope.val.import();
   }

   //
   // MapScope::exec
   //
   void MapScope::exec()
   {
      // Execute deferred script actions.
      while(scriptAction.next->obj)
      {
         ScriptAction *action = scriptAction.next->obj;
         Script       *script = findScript(action->name);

         if(script) switch(action->action)
         {
         case ScriptAction::Start:
            scriptStart(script, action->argV.data(), action->argV.size());
            break;

         case ScriptAction::StartForced:
            scriptStartForced(script, action->argV.data(), action->argV.size());
            break;

         case ScriptAction::Stop:
            scriptStop(script);
            break;

         case ScriptAction::Pause:
            scriptPause(script);
            break;
         }

         delete action;
      }

      // Execute running threads.
      for(ListLink<Thread> *itr = threadActive.next; itr->obj;)
      {
         itr->obj->exec();
         itr = itr->next;
         if(itr->prev->obj->state.state == ThreadState::Inactive)
            freeThread(itr->prev->obj);
      }
   }

   //
   // MapScope::findScript
   //
   Script *MapScope::findScript(ScriptName name)
   {
      return name.s ? findScript(name.s) : findScript(name.i);
   }

   //
   // MapScope::findScript
   //
   Script *MapScope::findScript(String *name)
   {
      if(Script **script = pd->scriptStr.find(name))
         return *script;
      else
         return nullptr;
   }

   //
   // MapScope::findScript
   //
   Script *MapScope::findScript(Word name)
   {
      if(Script **script = pd->scriptInt.find(name))
         return *script;
      else
         return nullptr;
   }

   //
   // MapScope::freeThread
   //
   void MapScope::freeThread(Thread *thread)
   {
      auto itr = pd->scriptThread.find(thread->script);
      if(itr  && *itr == thread)
         *itr = nullptr;

      env->freeThread(thread);
   }

   //
   // MapScope::getModuleScope
   //
   ModuleScope *MapScope::getModuleScope(Module *module)
   {
      return pd->moduleScopes.find(module);
   }

   //
   // MapScope::hasActiveThread
   //
   bool MapScope::hasActiveThread()
   {
      for(ListLink<Thread> *itr = threadActive.next; itr->obj; itr = itr->next)
      {
         if(itr->obj->state.state != ThreadState::Inactive)
            return true;
      }

      return false;
   }

   //
   // MapScope::isScriptActive
   //
   bool MapScope::isScriptActive(Script *script)
   {
      auto itr = pd->scriptThread.find(script);
      return itr && *itr && (*itr)->state.state != ThreadState::Inactive;
   }

   //
   // MapScope::loadModules
   //
   void MapScope::loadModules(std::istream &in)
   {
      auto count = ReadVLN<std::size_t>(in);
      std::vector<Module *> modules;
      modules.reserve(count);

      for(auto n = count; n--;)
         modules.emplace_back(env->getModule(env->readModuleName(in)));

      for(auto &module : modules)
         addModule(module);
      addModuleFinish();

      for(auto &module : modules)
         pd->moduleScopes.find(module)->loadState(in);
   }

   //
   // MapScope::loadState
   //
   void MapScope::loadState(std::istream &in)
   {
      reset();

      env->readScriptActions(in, scriptAction);
      active = in.get();
      loadModules(in);
      loadThreads(in);
   }

   //
   // MapScope::loadThreads
   //
   void MapScope::loadThreads(std::istream &in)
   {
      for(auto n = ReadVLN<std::size_t>(in); n--;)
      {
         Thread *thread = env->getFreeThread();
         thread->link.insert(&threadActive);
         thread->loadState(in);

         if(in.get())
         {
            auto scrThread = pd->scriptThread.find(thread->script);
            if(scrThread)
               *scrThread = thread;
         }
      }
   }

   //
   // MapScope::reset
   //
   void MapScope::reset()
   {
      // Stop any remaining threads and return them to free list.
      while(threadActive.next->obj)
      {
         threadActive.next->obj->stop();
         env->freeThread(threadActive.next->obj);
      }

      while(scriptAction.next->obj)
         delete scriptAction.next->obj;

      active = false;

      pd->moduleScopes.free();

      pd->modules.clear();

      pd->scriptInt.free();
      pd->scriptStr.free();
      pd->scriptThread.free();
   }

   //
   // MapScope::saveModules
   //
   void MapScope::saveModules(std::ostream &out) const
   {
      WriteVLN(out, pd->moduleScopes.size());

      for(auto &scope : pd->moduleScopes)
         env->writeModuleName(out, scope.key->name);

      for(auto &scope : pd->moduleScopes)
         scope.val.saveState(out);
   }

   //
   // MapScope::saveState
   //
   void MapScope::saveState(std::ostream &out) const
   {
      env->writeScriptActions(out, scriptAction);
      out.put(active ? '\1' : '\0');
      saveModules(out);
      saveThreads(out);
   }

   //
   // MapScope::saveThreads
   //
   void MapScope::saveThreads(std::ostream &out) const
   {
      std::size_t count = 0;
      for(auto thread = threadActive.next; thread->obj; thread = thread->next)
         ++count;

      WriteVLN(out, count);
      for(auto thread = threadActive.next; thread->obj; thread = thread->next)
      {
         thread->obj->saveState(out);

         auto scrThread = pd->scriptThread.find(thread->obj->script);
         out.put(scrThread && *scrThread == thread->obj ? '\1' : '\0');
      }
   }

   //
   // MapScope::scriptPause
   //
   void MapScope::scriptPause(Script *script)
   {
      auto itr = pd->scriptThread.find(script);
      if(itr && *itr)
         (*itr)->state = ThreadState::Paused;
   }

   //
   // MapScope::scriptStart
   //
   void MapScope::scriptStart(Script *script, Word const *argV, Word argC)
   {
      auto itr = pd->scriptThread.find(script);
      if(!itr) return;

      if(Thread *&thread = *itr)
      {
         thread->state = ThreadState::Running;
      }
      else
      {
         thread = env->getFreeThread();

         thread->start(script, this);
         std::copy(argV, argV + std::min<Word>(argC, script->argC), &thread->localReg[0]);
      }
   }

   //
   // MapScope::scriptStartForced
   //
   void MapScope::scriptStartForced(Script *script, Word const *argV, Word argC)
   {
      Thread *thread = env->getFreeThread();

      thread->start(script, this);
      std::copy(argV, argV + std::min<Word>(argC, script->argC), &thread->localReg[0]);
   }

   //
   // MapScope::scriptStartResult
   //
   Word MapScope::scriptStartResult(Script *script, Word const *argV, Word argC)
   {
      Thread *thread = env->getFreeThread();

      thread->start(script, this);
      std::copy(argV, argV + std::min<Word>(argC, script->argC), &thread->localReg[0]);
      thread->exec();

      Word result = thread->result;
      if(thread->state.state == ThreadState::Inactive)
         freeThread(thread);
      return result;
   }

   //
   // MapScope::scriptStop
   //
   void MapScope::scriptStop(Script *script)
   {
      auto itr = pd->scriptThread.find(script);
      if(itr && *itr)
      {
         (*itr)->state = ThreadState::Stopped;
         (*itr)        = nullptr;
      }
   }

   //
   // ModuleScope constructor
   //
   ModuleScope::ModuleScope(MapScope *map_, Module *module_) :
      map   {map_},
      module{module_},

      selfArrV{},
      selfRegV{}
   {
      // Set arrays and registers to refer to this scope's by default.
      for(std::size_t i = 0; i != ArrC; ++i) arrV[i] = &selfArrV[i];
      for(std::size_t i = 0; i != RegC; ++i) regV[i] = &selfRegV[i];

      // Apply initialization data from module.

      for(std::size_t i = 0; i != ArrC; ++i)
      {
         if(i < module->arrInitV.size())
            module->arrInitV[i].apply(selfArrV[i], module);
      }

      for(std::size_t i = 0; i != RegC; ++i)
      {
         if(i < module->regInitV.size())
            selfRegV[i] = module->regInitV[i].getValue(module);
      }
   }

   //
   // ModuleScope destructor
   //
   ModuleScope::~ModuleScope()
   {
   }

   //
   // ModuleScope::import
   //
   void ModuleScope::import()
   {
      for(std::size_t i = 0, e = std::min<std::size_t>(ArrC, module->arrImpV.size()); i != e; ++i)
      {
         String *arrName = module->arrImpV[i];
         if(!arrName) continue;

         for(auto &imp : module->importV)
         {
            for(auto &impName : imp->arrNameV)
            {
               if(impName == arrName)
               {
                  std::size_t impIdx = &impName - imp->arrNameV.data();
                  if(impIdx >= ArrC) continue;
                  arrV[i] = &map->getModuleScope(imp)->selfArrV[impIdx];
                  goto arr_found;
               }
            }
         }

      arr_found:;
      }

      for(std::size_t i = 0, e = std::min<std::size_t>(RegC, module->regImpV.size()); i != e; ++i)
      {
         String *regName = module->regImpV[i];
         if(!regName) continue;

         for(auto &imp : module->importV)
         {
            for(auto &impName : imp->regNameV)
            {
               if(impName == regName)
               {
                  std::size_t impIdx = &impName - imp->regNameV.data();
                  if(impIdx >= RegC) continue;
                  regV[i] = &map->getModuleScope(imp)->selfRegV[impIdx];
                  goto reg_found;
               }
            }
         }

      reg_found:;
      }
   }

   //
   // ModuleScope::loadState
   //
   void ModuleScope::loadState(std::istream &in)
   {
      for(auto &arr : selfArrV)
         arr.loadState(in);

      for(auto &reg : selfRegV)
         reg = ReadVLN<Word>(in);
   }

   //
   // ModuleScope::saveState
   //
   void ModuleScope::saveState(std::ostream &out) const
   {
      for(auto &arr : selfArrV)
         arr.saveState(out);

      for(auto &reg : selfRegV)
         WriteVLN(out, reg);
   }
}

// EOF

