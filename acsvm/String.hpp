//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// String classes.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__String_H__
#define ACSVM__String_H__

#include "Types.hpp"

#include <cstring>
#include <memory>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   std::size_t StrHash(char const *str, std::size_t len);

   //
   // StringData
   //
   // Stores basic string information. Does not manage the storage for the
   // string data.
   //
   class StringData
   {
   public:
      StringData(char const *first, char const *last) :
         str{first}, len(last - first), hash{StrHash(str, len)} {}
      StringData(char const *str_, std::size_t len_) :
         str{str_}, len{len_}, hash{StrHash(str, len)} {}
      StringData(char const *str_, std::size_t len_, std::size_t hash_) :
         str{str_}, len{len_}, hash{hash_} {}

      bool operator == (StringData const &r) const
         {return hash == r.hash && len == r.len && !std::memcmp(str, r.str, len);}

      char const *const str;
      std::size_t const len;
      std::size_t const hash;
   };

   //
   // String
   //
   // Indexed string data.
   //
   class String : public StringData
   {
   public:
      std::size_t refCount;
      std::size_t lckCount;

      Word const idx;  // Index into table.
      Word const len0; // Null-terminated length.

      char get(std::size_t i) const {return i < len ? str[i] : '\0';}


      static void Delete(String *str);

      static String *New(StringData const &data, Word idx);

   private:
      String(StringData const &data, Word idx);
      ~String();
   };

   //
   // StringTable
   //
   class StringTable
   {
   public:
      StringTable();
      ~StringTable();

      String &operator [] (Word idx) {return idx < strC ? *strV[idx] : *strNone;}
      String &operator [] (StringData const &data);

   private:
      struct PrivData;

      String    **strV;
      std::size_t strC;

      String *strNone;

      PrivData *pd;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace ACSVM
{
   std::unique_ptr<char[]> StrDup(char const *str);
   std::unique_ptr<char[]> StrDup(char const *str, std::size_t len);

   std::size_t StrHash(char const *str);
   std::size_t StrHash(char const *str, std::size_t len);
}

#endif//ACSVM__String_H__

