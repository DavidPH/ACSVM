//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// PrintBuf class.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__PrintBuf_H__
#define ACSVM__PrintBuf_H__

#include "Types.hpp"

#include <cstdarg>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // PrintBuf
   //
   class PrintBuf
   {
   public:
      PrintBuf();
      ~PrintBuf();

      void clear() {bufBeg = bufPtr = buffer;}

      char const *data() const {return *bufPtr = '\0', bufBeg;}

      void drop();

      // Formats using sprintf. Must already have enough space reserved.
      void format(char const *fmt, ...);
      void formatv(char const *fmt, std::va_list arg);

      // Returns a pointer to count chars to write into. A null is added to one
      // past the end of the buffer, and the entire buffer must be written to
      // by the caller.
      char *getBuf(std::size_t count);

      void push();

      void put(char c) {*bufPtr++ = c;}
      void put(char const *s) {while(*s) *bufPtr++ = *s++;}
      void put(char const *s, std::size_t n) {while(n--) *bufPtr++ = *s++;}

      void reserve(std::size_t count);

      std::size_t size() const {return bufPtr - bufBeg;}

   private:
      char *buffer, *bufEnd, *bufBeg, *bufPtr;
   };
}

#endif//ACSVM__PrintBuf_H__

