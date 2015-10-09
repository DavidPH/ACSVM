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

      Word       *codeV;
      std::size_t codeC;

      String    **funcNameV;
      std::size_t funcNameC;

      Function  **functionV;
      std::size_t functionC;

      Jump       *jumpV;
      std::size_t jumpC;

      String    **scrNameV;
      std::size_t scrNameC;

      Script     *scriptV;
      std::size_t scriptC;

      String    **stringV;
      std::size_t stringC;

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

      static std::pair<
         std::unique_ptr<Byte[]> /*data*/,
         std::size_t             /*size*/>
      DecryptStringACSE(Byte const *data, std::size_t size, std::size_t iter);

      static std::unique_ptr<char[]> ParseStringACS0(Byte const *first,
         Byte const *last, std::size_t len);

      static std::tuple<
         Byte const */*begin*/,
         Byte const */*end*/,
         std::size_t /*len*/>
      ScanStringACS0(Byte const *data, std::size_t size, std::size_t iter);

   private:
      void allocCodeV(std::size_t count);
      void allocFuncNameV(std::size_t count);
      void allocFunctionV(std::size_t count);
      void allocJumpV(std::size_t count);
      void allocScrNameV(std::size_t count);
      void allocScriptV(std::size_t count);
      void allocStringV(std::size_t count);

      bool chunkIterACSE(Byte const *data, std::size_t size,
         bool (Module::*chunker)(Byte const *, std::size_t, Word));

      void chunkStrTabACSE(String **&strV, std::size_t &strC,
         Byte const *data, std::size_t size, bool junk,
         void (Module::*alloc)(std::size_t));

      bool chunkerACSE_FARY(Byte const *data, std::size_t size, Word chunkName);
      bool chunkerACSE_FNAM(Byte const *data, std::size_t size, Word chunkName);
      bool chunkerACSE_FUNC(Byte const *data, std::size_t size, Word chunkName);
      bool chunkerACSE_JUMP(Byte const *data, std::size_t size, Word chunkName);
      bool chunkerACSE_SARY(Byte const *data, std::size_t size, Word chunkName);
      bool chunkerACSE_SFLG(Byte const *data, std::size_t size, Word chunkName);
      bool chunkerACSE_SNAM(Byte const *data, std::size_t size, Word chunkName);
      bool chunkerACSE_SPTR8(Byte const *data, std::size_t size, Word chunkName);
      bool chunkerACSE_SPTR12(Byte const *data, std::size_t size, Word chunkName);
      bool chunkerACSE_STRE(Byte const *data, std::size_t size, Word chunkName);
      bool chunkerACSE_STRL(Byte const *data, std::size_t size, Word chunkName);
      bool chunkerACSE_SVCT(Byte const *data, std::size_t size, Word chunkName);

      void freeCodeV();
      void freeFuncNameV();
      void freeFunctionV();
      void freeJumpV();
      void freeScrNameV();
      void freeScriptV();
      void freeStringV();

      void readBytecodeACS0(Byte const *data, std::size_t size);
      void readBytecodeACSE(Byte const *data, std::size_t size,
         bool compressed, std::size_t iter = 4);

      void readChunksACSE(Byte const *data, std::size_t size, bool fakeACS0);

      void readCodeACS0(Byte const *data, std::size_t size, bool compressed);

      String *readStringACS0(Byte const *data, std::size_t size, std::size_t iter);

      void setScriptNameTypeACSE(Script *scr, Word nameInt, Word type);
   };
}

#endif//ACSVM__Module_H__

