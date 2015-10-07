//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Error classes.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__Error_H__
#define ACSVM__Error_H__

#include <stdexcept>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // ReadError
   //
   // Generic exception class for errors occurring during bytecode reading.
   //
   class ReadError : public std::exception
   {
   public:
      virtual char const *what() const noexcept {return "ACSVM::ReadError";}
   };
}

#endif//ACSVM__Error_H__

