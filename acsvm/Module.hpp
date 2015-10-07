//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Module class.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__Module_H__
#define ACSVM__Module_H__

#include "Types.hpp"

#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // ModuleName
   //
   // Stores a Module's name. Name semantics are user-defined and must provide
   // a (user-defined) mapping from name to bytecode data. The names are used
   // internally only for determining if a specific module has already been
   // loaded. That is, two ModuleNames should compare equal if and only if they
   // designate the same bytecode data.
   //
   class ModuleName
   {
   public:
      ModuleName(ModuleName const &name);
      ModuleName(ModuleName &&name) = default;
      ModuleName(std::unique_ptr<char[]> &&s_, void *p_, std::size_t i_) :
         s{std::move(s_)}, p{p_}, i{i_} {}

      bool operator == (ModuleName const &name) const;

      std::size_t hash() const;

      // Null-terminated string. May be null.
      std::unique_ptr<char[]> s;

      // Arbitrary pointer value. Must be valid for equality compares.
      void *p;

      // Arbitrary integer value.
      std::size_t i;
   };

   //
   // Module
   //
   // Represents an ACS bytecode module.
   //
   class Module
   {
   public:
      Module(Environment *env, ModuleName &&name);
      ~Module();

      void readBytecode(Byte const *data, std::size_t size);

      void reset();

      Environment *env;
      ModuleName   name;

      Word       *codes;
      std::size_t codeNum;

      Function  **functions;
      std::size_t functionNum;

      Jump       *jumps;
      std::size_t jumpNum;

      Script     *scripts;
      std::size_t scriptNum;

      String    **strings;
      std::size_t stringNum;

      bool loaded;


      static constexpr std::uint32_t ChunkID(char c0, char c1, char c2, char c3)
      {
         return
            (static_cast<std::uint32_t>(c0) <<  0) |
            (static_cast<std::uint32_t>(c1) <<  8) |
            (static_cast<std::uint32_t>(c2) << 16) |
            (static_cast<std::uint32_t>(c3) << 24);
      }

      static constexpr std::uint32_t ChunkID(char const (&s)[5])
         {return ChunkID(s[0], s[1], s[2], s[3]);}

      static std::unique_ptr<char[]> ParseStringACS0(Byte const *first,
         Byte const *last, std::size_t len);

      static std::tuple<
         Byte const */*begin*/,
         Byte const */*end*/,
         std::size_t /*len*/>
      ScanStringACS0(Byte const *data, std::size_t size, std::size_t iter);

   private:
      void allocCodes();
      void allocScripts();
      void allocStrings();

      void freeCodes();
      void freeScripts();
      void freeStrings();

      void readBytecodeACS0(Byte const *data, std::size_t size);
      void readBytecodeACSE(Byte const *data, std::size_t size,
         bool compressed, std::size_t iter = 4);

      void readChunksACSE(Byte const *data, std::size_t size, bool fakeACS0);

      void readCodeACS0(Byte const *data, std::size_t size, bool compressed);

      String *readStringACS0(Byte const *data, std::size_t size, std::size_t iter);
   };
}

#endif//ACSVM__Module_H__

