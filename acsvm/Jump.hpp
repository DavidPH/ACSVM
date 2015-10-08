//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Jump class.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__Jump_H__
#define ACSVM__Jump_H__

#include "Types.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // Jump
   //
   // Dynamic jump target.
   //
   class Jump
   {
   public:
      Word codeIdx;
   };
}

#endif//ACSVM__Jump_H__

