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

#include "Environ.hpp"
#include "Module.hpp"
#include "Thread.hpp"

#include <unordered_map>


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
   GlobalScope::GlobalScope(Environment *env_) :
      env{env_},

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
      for(auto &itr : pd->hubScopes)
      {
         if(itr.second.active)
            itr.second.exec();
      }
   }

   //
   // GlobalScope::getHubScope
   //
   HubScope *GlobalScope::getHubScope(std::size_t id)
   {
      return &pd->hubScopes.emplace(std::piecewise_construct,
         std::make_tuple(id), std::make_tuple(this)).first->second;
   }

   //
   // HubScope constructor
   //
   HubScope::HubScope(GlobalScope *global_) :
      global{global_},

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
      for(auto &itr : pd->mapScopes)
      {
         if(itr.second.active)
            itr.second.exec();
      }
   }

   //
   // HubScope::getMapScope
   //
   MapScope *HubScope::getMapScope(std::size_t id)
   {
      return &pd->mapScopes.emplace(std::piecewise_construct,
         std::make_tuple(id), std::make_tuple(this)).first->second;
   }

   //
   // MapScope constructor
   //
   MapScope::MapScope(HubScope *hub_) :
      hub{hub_},

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
   // MapScope::exec
   //
   void MapScope::exec()
   {
      for(ListLink<Thread> *itr = threadActive.next; itr->obj;)
      {
         itr->obj->exec();
         itr = itr->next;
         if(itr->prev->obj->state.state == ThreadState::Inactive)
            hub->global->env->freeThread(itr->prev->obj);
      }
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

