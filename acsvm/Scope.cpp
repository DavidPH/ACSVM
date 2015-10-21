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
#include "Environ.hpp"
#include "Module.hpp"
#include "Script.hpp"
#include "Thread.hpp"

#include <unordered_map>
#include <unordered_set>


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
      std::unordered_map<std::size_t, HubScope> hubScopes;
   };

   //
   // HubScope::PrivData
   //
   struct HubScope::PrivData
   {
      std::unordered_map<std::size_t, MapScope> mapScopes;
   };

   //
   // MapScope::PrivData
   //
   struct MapScope::PrivData
   {
      std::unordered_map<Module *, ModuleScope> moduleScopes;

      std::unordered_set<Module *> modules;

      std::unordered_map<Word,     Script *> scriptInt;
      std::unordered_map<String *, Script *> scriptStr;

      std::unordered_map<Script *, Thread *> scriptThread;
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
   // HubScope constructor
   //
   HubScope::HubScope(GlobalScope *global_, Word id_) :
      global{global_},
      id    {id_},

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
   // MapScope constructor
   //
   MapScope::MapScope(HubScope *hub_, Word id_) :
      hub{hub_},
      id {id_},

      pd{new PrivData}
   {
   }

   //
   // MapScope destructor
   //
   MapScope::~MapScope()
   {
      // Stop any remaining threads and return them to free list.
      while(threadActive.next->obj)
      {
         threadActive.next->obj->stop();
         hub->global->env->freeThread(threadActive.next->obj);
      }

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

      for(auto &script : module->scriptV)
      {
         if(script.name.s)
            pd->scriptStr[script.name.s] = &script;
         else
            pd->scriptInt[script.name.i] = &script;
      }
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
      auto itr = pd->scriptStr.find(name);
      return itr != pd->scriptStr.end() ? itr->second : nullptr;
   }

   //
   // MapScope::findScript
   //
   Script *MapScope::findScript(Word name)
   {
      auto itr = pd->scriptInt.find(name);
      return itr != pd->scriptInt.end() ? itr->second : nullptr;
   }

   //
   // MapScope::freeThread
   //
   void MapScope::freeThread(Thread *thread)
   {
      auto itr = pd->scriptThread.find(thread->script);
      if(itr != pd->scriptThread.end() && itr->second == thread)
         itr->second = nullptr;

      hub->global->env->freeThread(thread);
   }

   //
   // MapScope::getModuleScope
   //
   ModuleScope *MapScope::getModuleScope(Module *module)
   {
      auto itr = pd->moduleScopes.find(module);
      if(itr == pd->moduleScopes.end())
      {
         itr = pd->moduleScopes.emplace(std::piecewise_construct,
            std::make_tuple(module), std::make_tuple(this, module)).first;

         itr->second.import();
      }

      return &itr->second;
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
      return itr != pd->scriptThread.end() && itr->second &&
         itr->second->state.state != ThreadState::Inactive;
   }

   //
   // MapScope::scriptPause
   //
   void MapScope::scriptPause(Script *script)
   {
      auto itr = pd->scriptThread.find(script);
      if(itr != pd->scriptThread.end() && itr->second)
         itr->second->state = ThreadState::Paused;
   }

   //
   // MapScope::scriptStart
   //
   void MapScope::scriptStart(Script *script, Word const *argV, Word argC)
   {
      Thread *&thread = pd->scriptThread[script];

      if(thread)
      {
         thread->state = ThreadState::Running;
      }
      else
      {
         thread = hub->global->env->getFreeThread();

         thread->start(script, this);
         std::copy(argV, argV + std::min<Word>(argC, script->argC), &thread->localReg[0]);
      }
   }

   //
   // MapScope::scriptStartForced
   //
   void MapScope::scriptStartForced(Script *script, Word const *argV, Word argC)
   {
      Thread *thread = hub->global->env->getFreeThread();

      thread->start(script, this);
      std::copy(argV, argV + std::min<Word>(argC, script->argC), &thread->localReg[0]);
   }

   //
   // MapScope::scriptStartResult
   //
   Word MapScope::scriptStartResult(Script *script, Word const *argV, Word argC)
   {
      Thread *thread = hub->global->env->getFreeThread();

      thread->start(script, this);
      std::copy(argV, argV + std::min<Word>(argC, script->argC), &thread->localReg[0]);
      thread->exec();

      Word result = thread->result;
      freeThread(thread);
      return result;
   }

   //
   // MapScope::scriptStop
   //
   void MapScope::scriptStop(Script *script)
   {
      auto itr = pd->scriptThread.find(script);
      if(itr != pd->scriptThread.end() && itr->second)
      {
         itr->second->state = ThreadState::Stopped;
         itr->second        = nullptr;
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
            module->arrInitV[i].apply(selfArrV[i]);
      }

      for(std::size_t i = 0; i != RegC; ++i)
      {
         if(i < module->regInitV.size())
            selfRegV[i] = module->regInitV[i];
      }
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
   // ModuleScope destructor
   //
   ModuleScope::~ModuleScope()
   {
   }
}

// EOF

