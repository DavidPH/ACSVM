//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Deferred Action classes.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__Action_H__
#define ACSVM__Action_H__

#include "List.hpp"
#include "Script.hpp"
#include "Vector.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // ScopeID
   //
   class ScopeID
   {
   public:
      ScopeID(Word global_, Word hub_, Word map_) :
         global{global_}, hub{hub_}, map{map_} {}

      Word global;
      Word hub;
      Word map;
   };

   //
   // ScriptAction
   //
   // Represents a deferred Script action.
   //
   class ScriptAction
   {
   public:
      enum Action
      {
         Start,
         StartForced,
         Stop,
         Pause,
      };


      ScriptAction(ScriptAction &&action) = default;
      ScriptAction(ScopeID id, ScriptName name, Action action, Vector<Word> &&argV);
      ~ScriptAction();

      Action                 action;
      Vector<Word>           argV;
      ScopeID                id;
      ListLink<ScriptAction> link;
      ScriptName             name;
   };
}

#endif//ACSVM__Action_H__
