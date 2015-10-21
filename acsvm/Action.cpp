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

#include "Action.hpp"

#include <utility>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace ACSVM
{
   //
   // ScriptAction constructor
   //
   ScriptAction::ScriptAction(ScopeID id_, ScriptName name_, Action action_, Vector<Word> &&argV_) :
      action{action_},
      argV  {std::move(argV_)},
      id    {id_},
      link  {this},
      name  {name_}
   {
   }

   //
   // ScriptAction destructor
   //
   ScriptAction::~ScriptAction()
   {
   }
}

// EOF

