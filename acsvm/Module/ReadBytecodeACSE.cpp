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
#include "Function.hpp"
#include "Jump.hpp"
#include "Script.hpp"

#include <algorithm>
#include <vector>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace ACSVM
{
   //
   // Module::chunkIterACSE
   //
   bool Module::chunkIterACSE(Byte const *data, std::size_t size,
      bool (Module::*chunker)(Byte const *, std::size_t, Word))
   {
      std::size_t iter = 0;

      while(iter != size)
      {
         // Need space for header.
         if(size - iter < 8) throw ReadError();

         // Read header.
         Word chunkName = ReadLE4(data + iter + 0);
         Word chunkSize = ReadLE4(data + iter + 4);

         // Consume header.
         iter += 8;

         // Need space for payload.
         if(size - iter < chunkSize) throw ReadError();

         // Read payload.
         if((this->*chunker)(data + iter, chunkSize, chunkName))
            return true;

         // Consume payload.
         iter += chunkSize;
      }

      return false;
   }

   //
   // Module::chunkStrTabACSE
   //
   void Module::chunkStrTabACSE(Vector<String *> &strV,
      Byte const *data, std::size_t size, bool junk)
   {
      std::size_t iter = 0;

      if(junk)
      {
         if(size < 12) throw ReadError();

         /*junk   = ReadLE4(data + iter);*/ iter += 4;
         strV.alloc(ReadLE4(data + iter));  iter += 4;
         /*junk   = ReadLE4(data + iter);*/ iter += 4;
      }
      else
      {
         if(size < 4) throw ReadError();

         strV.alloc(ReadLE4(data + iter)); iter += 4;
      }

      if(size - iter < strV.size() * 4) throw ReadError();
      for(String *&str : strV)
      {
         str = readStringACS0(data, size, ReadLE4(data + iter)); iter += 4;
      }
   }

   //
   // Module::chunkerACSE_FARY
   //
   bool Module::chunkerACSE_FARY(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("FARY")) return false;

      if(size < 2 || (size - 2) % 4) throw ReadError();

      Word        idx  = ReadLE2(data);
      std::size_t arrC = (size - 2) / 4;

      if(idx < functionV.size() && functionV[idx])
         functionV[idx]->locArrC = arrC;

      return false;
   }

   //
   // Module::chunkerACSE_FNAM
   //
   bool Module::chunkerACSE_FNAM(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("FNAM")) return false;

      chunkStrTabACSE(funcNameV, data, size, false);

      return true;
   }

   //
   // Module::chunkerACSE_FUNC
   //
   bool Module::chunkerACSE_FUNC(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("FUNC")) return false;

      if(size % 8) throw ReadError();

      // Read functions.
      functionV.alloc(size / 8);

      std::size_t iter = 0;
      for(Function *&func : functionV)
      {
         Word idx     = iter / 8;
         Word argC    = ReadLE1(data + iter); iter += 1;
         Word locRegC = ReadLE1(data + iter); iter += 1;
         Word flags   = ReadLE2(data + iter); iter += 2;
         Word codeIdx = ReadLE4(data + iter); iter += 4;

         // Ignore undefined functions for now.
         if(!codeIdx) continue;

         String   *funcName = idx < funcNameV.size() ? funcNameV[idx] : nullptr;
         Function *function = env->getFunction(this, funcName);

         function->argC    = argC;
         function->locRegC = locRegC;
         function->flagRet = flags & 0x0001;
         function->codeIdx = codeIdx;

         func = function;
      }

      return true;
   }

   //
   // Module::chunkerACSE_JUMP
   //
   bool Module::chunkerACSE_JUMP(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("JUMP")) return false;

      if(size % 4) throw ReadError();

      // Read jumps.
      jumpV.alloc(size / 4);

      std::size_t iter = 0;
      for(Jump &jump : jumpV)
      {
         jump.codeIdx = ReadLE4(data + iter); iter += 4;
      }

      return true;
   }

   //
   // Module::chunkerACSE_SARY
   //
   bool Module::chunkerACSE_SARY(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("SARY")) return false;

      if(size < 2 || (size - 2) % 4) throw ReadError();

      Word        nameInt = ReadLE2(data);
      std::size_t arrC    = (size - 2) / 4;

      if(nameInt & 0x8000) nameInt |= 0xFFFF0000;

      for(Script &scr : scriptV)
         if(scr.nameInt == nameInt) scr.locArrC = arrC;

      return false;
   }

   //
   // Module::chunkerACSE_SFLG
   //
   bool Module::chunkerACSE_SFLG(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("SFLG")) return false;

      if(size % 4) throw ReadError();

      for(std::size_t iter = 0; iter != size;)
      {
         Word nameInt = ReadLE2(data + iter); iter += 2;
         Word flags   = ReadLE2(data + iter); iter += 2;

         bool flagNet    = flags & 0x0001;
         bool flagClient = flags & 0x0002;

         if(nameInt & 0x8000) nameInt |= 0xFFFF0000;

         for(Script &scr : scriptV)
         {
            if(scr.nameInt == nameInt)
            {
               scr.flagClient = flagClient;
               scr.flagNet    = flagNet;
            }
         }
      }

      return false;
   }

   //
   // Module::chunkerACSE_SNAM
   //
   bool Module::chunkerACSE_SNAM(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("SNAM")) return false;

      chunkStrTabACSE(scrNameV, data, size, false);

      return true;
   }

   //
   // Module::chunkerACSE_SPTR8
   //
   // Reads 8-byte SPTR chunk.
   //
   bool Module::chunkerACSE_SPTR8(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("SPTR")) return false;

      if(size % 8) throw ReadError();

      // Read scripts.
      scriptV.alloc(size / 8, this);

      std::size_t iter = 0;
      for(Script &scr : scriptV)
      {
         Word nameInt = ReadLE2(data + iter); iter += 2;
         Word type    = ReadLE1(data + iter); iter += 1;
         scr.argC     = ReadLE1(data + iter); iter += 1;
         scr.codeIdx  = ReadLE4(data + iter); iter += 4;

         if(nameInt & 0x8000) nameInt |= 0xFFFF0000;
         setScriptNameTypeACSE(&scr, nameInt, type);
      }

      return true;
   }

   //
   // Module::chunkerACSE_SPTR12
   //
   // Reads 12-byte SPTR chunk.
   //
   bool Module::chunkerACSE_SPTR12(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("SPTR")) return false;

      if(size % 12) throw ReadError();

      // Read scripts.
      scriptV.alloc(size / 12, this);

      std::size_t iter = 0;
      for(Script &scr : scriptV)
      {
         Word nameInt = ReadLE2(data + iter); iter += 2;
         Word type    = ReadLE2(data + iter); iter += 2;
         scr.codeIdx  = ReadLE4(data + iter); iter += 4;
         scr.argC     = ReadLE4(data + iter); iter += 4;

         if(nameInt & 0x8000) nameInt |= 0xFFFF0000;
         setScriptNameTypeACSE(&scr, nameInt, type);
      }

      return true;
   }

   //
   // Module::chunkerACSE_STRE
   //
   bool Module::chunkerACSE_STRE(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("STRE")) return false;

      std::size_t iter = 0;

      if(size < 12) throw ReadError();

      /*junk      = ReadLE4(data + iter);*/ iter += 4;
      stringV.alloc(ReadLE4(data + iter));  iter += 4;
      /*junk      = ReadLE4(data + iter);*/ iter += 4;

      if(size - iter < stringV.size() * 4) throw ReadError();
      for(String *&str : stringV)
      {
         std::size_t offset = ReadLE4(data + iter); iter += 4;

         // Decrypt string.
         std::unique_ptr<Byte[]> buf;
         std::size_t             len;
         std::tie(buf, len) = DecryptStringACSE(data, size, offset);

         // Scan string.
         Byte const *bufEnd;
         std::tie(std::ignore, bufEnd, len) = ScanStringACS0(buf.get(), len, 0);

         // Parse string.
         str = env->getString(ParseStringACS0(buf.get(), bufEnd, len).get(), len);
      }

      return true;
   }

   //
   // Module::chunkerACSE_STRL
   //
   bool Module::chunkerACSE_STRL(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("STRL")) return false;

      chunkStrTabACSE(stringV, data, size, true);

      return true;
   }

   //
   // Module::chunkerACSE_SVCT
   //
   bool Module::chunkerACSE_SVCT(Byte const *data, std::size_t size, Word chunkName)
   {
      if(chunkName != ChunkID("SVCT")) return false;

      if(size % 4) throw ReadError();

      for(std::size_t iter = 0; iter != size;)
      {
         Word nameInt = ReadLE2(data + iter); iter += 2;
         Word regC    = ReadLE2(data + iter); iter += 2;

         if(nameInt & 0x8000) nameInt |= 0xFFFF0000;

         for(Script &scr : scriptV)
         {
            if(scr.nameInt == nameInt)
               scr.locRegC = regC;
         }
      }

      return false;
   }

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
   void Module::readChunksACSE(Byte const *data, std::size_t size, bool fakeACS0)
   {
      // AINI - Map Array Init
      // TODO

      // ARAY - Map Arrays
      // TODO

      // ASTR - Map Array Strings
      // TODO

      // FNAM - Function Names
      chunkIterACSE(data, size, &Module::chunkerACSE_FNAM);

      // FUNC - Functions
      chunkIterACSE(data, size, &Module::chunkerACSE_FUNC);

      // FARY - Function Arrays
      chunkIterACSE(data, size, &Module::chunkerACSE_FARY);

      // JUMP - Dynamic Jump Targets
      chunkIterACSE(data, size, &Module::chunkerACSE_JUMP);

      // MEXP - Map Variable/Array Export
      // TODO

      // MINI - Map Variable Init
      // TODO

      // MSTR - Map Variable Strings
      // TODO

      // SNAM - Script Names
      chunkIterACSE(data, size, &Module::chunkerACSE_SNAM);

      // SPTR - Script Pointers
      if(fakeACS0)
         chunkIterACSE(data, size, &Module::chunkerACSE_SPTR8);
      else
         chunkIterACSE(data, size, &Module::chunkerACSE_SPTR12);

      // SARY - Script Arrays
      chunkIterACSE(data, size, &Module::chunkerACSE_SARY);

      // SFLG - Script Flags
      chunkIterACSE(data, size, &Module::chunkerACSE_SFLG);

      // SVCT - Script Variable Count
      chunkIterACSE(data, size, &Module::chunkerACSE_SVCT);

      // STRE - Encrypted String Literals
      if(!chunkIterACSE(data, size, &Module::chunkerACSE_STRE))
      {
         // STRL - String Literals
         chunkIterACSE(data, size, &Module::chunkerACSE_STRL);
      }

      // Process exports.
      // TODO

      // LOAD - Library Loading
      // TODO

      // Process imports.
      // TODO

      // Process function imports.
      // TODO

      // AIMP - Map Array Import
      // TODO

      // MIMP - Map Variable Import
      // TODO

      // ATAG - Map Array Tagging
      // TODO
   }

   //
   // Module::setScriptNameTypeACSE
   //
   void Module::setScriptNameTypeACSE(Script *scr, Word nameInt, Word type)
   {
      // If high bit is set, script is named.
      if((scr->nameInt = nameInt) & 0x80000000)
      {
         // Fetch name.
         Word nameIdx = ~scr->nameInt;
         if(nameIdx < scrNameV.size())
            scr->nameStr = scrNameV[nameIdx];
      }

      switch(type)
      {
      default:
      case  0: scr->type = ScriptType::Closed; break;
      case  1: scr->type = ScriptType::Open;   break;
      }
   }

   //
   // Module::DecryptStringACSE
   //
   std::pair<
      std::unique_ptr<Byte[]> /*data*/,
      std::size_t             /*size*/>
   Module::DecryptStringACSE(Byte const *data, std::size_t size, std::size_t iter)
   {
      Word const key = iter * 157135;

      // Calculate length. Start at 1 for null terminator.
      std::size_t len = 1;
      for(std::size_t i = iter, n = 0;; ++i, ++n, ++len)
      {
         if(i == size) throw ReadError();

         Byte c = data[i] ^ (n / 2 + key);
         if(!c) break;
      }

      // Decrypt data.
      std::unique_ptr<Byte[]> buf{new Byte[len]};
      for(std::size_t i = iter, n = 0;; ++i, ++n)
      {
         Byte c = data[i] ^ (n / 2 + key);
         if(!(buf[n] = c)) break;
      }

      return {std::move(buf), len};
   }
}

// EOF

