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

#include "String.hpp"

#include <new>
#include <unordered_map>
#include <vector>
#include <iostream>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // StringTable::PrivData
   //
   struct StringTable::PrivData
   {
      struct Equal
      {
         bool operator () (StringData const *l, StringData const *r) const
            {return *l == *r;}
      };

      struct Hash
      {
         std::size_t operator () (StringData const *s) const {return s->hash;}
      };

      std::vector<String *> tableString;

      std::vector<std::vector<String *>::size_type> tableStringIdx;

      std::unordered_map<StringData const *, String *, Hash, Equal> tableStringData;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace ACSVM
{
   //
   // String constructor
   //
   String::String(StringData const &data, Word idx_) :
      StringData{data}, idx{idx_}, len0(std::strlen(str))
   {
   }

   //
   // String destructor
   //
   String::~String()
   {
   }

   //
   // String::Delete
   //
   void String::Delete(String *str)
   {
      str->~String();
      operator delete(str);
   }

   //
   // String::New
   //
   String *String::New(StringData const &data, Word idx)
   {
      String *str = static_cast<String *>(operator new(sizeof(String) + data.len + 1));
      char   *buf = reinterpret_cast<char *>(str + 1);

      memcpy(buf, data.str, data.len);
      buf[data.len] = '\0';

      return new(str) String{{buf, data.len, data.hash}, idx};
   }

   //
   // StringTable constructor
   //
   StringTable::StringTable() :
      strV{nullptr},
      strC{0},

      strNone{String::New({"", 0, 0}, 0)},

      pd{new PrivData}
   {
   }

   //
   // StringTable destructor
   //
   StringTable::~StringTable()
   {
      delete pd;

      String::Delete(strNone);
   }

   //
   // StringTable::operator [StringData]
   //
   String &StringTable::operator [] (StringData const &data)
   {
      auto itr = pd->tableStringData.find(&data);
      if(itr != pd->tableStringData.end())
         return *itr->second;

      std::vector<String *>::size_type idx;
      if(pd->tableStringIdx.empty())
      {
         idx = pd->tableString.size();
         pd->tableString.emplace_back();
         strV = pd->tableString.data();
         strC = pd->tableString.size();
      }
      else
      {
         idx = pd->tableStringIdx.back();
         pd->tableStringIdx.pop_back();
      }

      String *str = String::New(data, idx);
      pd->tableString[idx] = str;
      pd->tableStringData.emplace(str, str);
      return *str;
   }

   //
   // StrDup
   //
   std::unique_ptr<char[]> StrDup(char const *str)
   {
      return StrDup(str, std::strlen(str));
   }

   //
   // StrDup
   //
   std::unique_ptr<char[]> StrDup(char const *str, std::size_t len)
   {
      std::unique_ptr<char[]> dup{new char[len + 1]};
      std::memcpy(dup.get(), str, len);
      dup[len] = '\0';

      return dup;
   }

   //
   // StrHash
   //
   std::size_t StrHash(char const *str)
   {
      std::size_t hash = 0;

      if(str) while(*str)
         hash = hash * 5 + static_cast<unsigned char>(*str++);

      return hash;
   }

   //
   // StrHash
   //
   std::size_t StrHash(char const *str, std::size_t len)
   {
      std::size_t hash = 0;

      while(len--)
         hash = hash * 5 + static_cast<unsigned char>(*str++);

      return hash;
   }
}

// EOF

