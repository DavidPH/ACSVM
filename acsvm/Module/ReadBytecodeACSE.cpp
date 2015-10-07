//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Module class bytecode reading.
//
//-----------------------------------------------------------------------------

#include "Module.hpp"

#include "BinaryIO.hpp"
#include "Environ.hpp"
#include "Error.hpp"
#include "Script.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace ACSVM
{
   //
   // Module::readBytecodeACSE
   //
   void Module::readBytecodeACSE(Byte const *data, std::size_t size,
      bool compressed, std::size_t offset)
   {
      std::size_t iter = offset;

      // Find table start.
      if(iter > size || size - iter < 4) throw ReadError();
      iter = ReadLE4(data + iter);
      if(iter > size) throw ReadError();

      // Read chunks.
      if(offset == 4)
      {
         readChunksACSE(data + iter, size - iter, false);
      }
      else
      {
         if(iter <= offset)
            readChunksACSE(data + iter, offset - iter, true);
         else
            readChunksACSE(data + iter, size - iter, true);
      }

      // Read code.
      readCodeACS0(data, size, compressed);

      loaded = true;
   }

   //
   // Module::readChunksACSE
   //
   void Module::readChunksACSE(Byte const *, std::size_t, bool)
   {
      // TODO
      throw ReadError();
   }
}

// EOF

