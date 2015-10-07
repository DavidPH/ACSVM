//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Common typedefs and class forward declarations.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__Types_H__
#define ACSVM__Types_H__

#include <cstdint>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   // Host platform byte.
   // Should this be uint8_t? Short answer: No.
   // Long answer: char is the smallest addressable unit in the implementation.
   // That is, it is by definition the byte of the target platform. And it is
   // required to be at least 8 bits wide, which is the only real requirement
   // for loading ACS bytecode.
   // Furthermore, uint8_t is only defined if the implementation has a type
   // with exactly 8 data bits and no padding bits. So even if you wanted to
   // unilaterally declare bytes to be 8 bits, the only type that can possibly
   // satisfy uint8_t is unsigned char. If CHAR_BIT is not 8, then there can be
   // no uint8_t.
   using Byte = unsigned char;

   using DWord = std::uint64_t;
   using SDWord = std::int64_t;
   using SWord = std::int32_t;
   using Word = std::uint32_t;

   enum class Code;
   enum class Func;
   class Array;
   class CodeData;
   class CodeDataACS0;
   class Environment;
   class FuncDataACS0;
   class Function;
   class Jump;
   class Module;
   class ModuleName;
   class Script;
   class String;
   class Thread;
}

#endif//ACSVM__Types_H__

