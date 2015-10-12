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

#include "Module.hpp"

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
      delete pd;
   }

   //
   // MapScope::getModuleScope
   //
   ModuleScope *MapScope::getModuleScope(Module *module)
   {
      return &pd->moduleScopes.emplace(std::piecewise_construct,
         std::make_tuple(module), std::make_tuple(this, module)).first->second;
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
   // ModuleScope destructor
   //
   ModuleScope::~ModuleScope()
   {
   }
}

// EOF

