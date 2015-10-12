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


      explicit GlobalScope(Environment *env);
      ~GlobalScope();

      HubScope *getHubScope(std::size_t id);

      Environment *const env;

      Array arrV[ArrC];
      Word  regV[RegC];

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


      explicit HubScope(GlobalScope *global);
      ~HubScope();

      MapScope *getMapScope(std::size_t id);

      GlobalScope *const global;

      Array arrV[ArrC];
      Word  regV[RegC];

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
      explicit MapScope(HubScope *hub);
      ~MapScope();

      ModuleScope *getModuleScope(Module *module);

      HubScope *const hub;

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

