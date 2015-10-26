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

#include "BinaryIO.hpp"

#include <new>
#include <unordered_map>
#include <vector>


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

      std::vector<Word> tableStringIdx;

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
      StringData{data}, lckCount{0}, idx{idx_}, len0(std::strlen(str))
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
   // String::Read
   //
   String *String::Read(std::istream &in, Word idx)
   {
      std::size_t len = ReadVLN<std::size_t>(in);

      String *str = static_cast<String *>(operator new(sizeof(String) + len + 1));
      char   *buf = reinterpret_cast<char *>(str + 1);

      in.read(buf, len);
      buf[len] = '\0';

      return new(str) String{{buf, len, StrHash(buf, len)}, idx};
   }

   //
   // String::Write
   //
   void String::Write(std::ostream &out, String *in)
   {
      WriteVLN(out, in->len);
      out.write(in->str, in->len);
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
   // StringTable move constructor
   //
   StringTable::StringTable(StringTable &&table) :
      strV{table.strV},
      strC{table.strC},

      strNone{table.strNone},

      pd{table.pd}
   {
      table.strV = nullptr;
      table.strC = 0;

      table.strNone = nullptr;

      table.pd = nullptr;
   }

   //
   // StringTable destructor
   //
   StringTable::~StringTable()
   {
      if(!pd) return;

      for(auto &str : pd->tableString)
      {
         if(str != strNone)
            String::Delete(str);
      }

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

      Word idx;
      if(pd->tableStringIdx.empty())
      {
         // Index has to fit within Word size.
         // If size_t has an equal or lesser max, then the check is redundant,
         // and some compilers warn about that kind of tautological comparison.
         #if SIZE_MAX > UINT32_MAX
         if(pd->tableString.size() > UINT32_MAX)
            throw std::bad_alloc();
         #endif

         idx = pd->tableString.size();
         pd->tableString.emplace_back(strNone);
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
   // StringTable::loadState
   //
   void StringTable::loadState(std::istream &in)
   {
      if(pd)
      {
         pd->tableString.clear();
         pd->tableStringIdx.clear();
         pd->tableStringData.clear();
      }
      else
      {
         pd      = new PrivData;
         strNone = String::New({"", 0, 0}, 0);
      }

      auto count = ReadVLN<std::size_t>(in);

      pd->tableString.resize(count);
      strV = pd->tableString.data();
      strC = pd->tableString.size();

      for(std::size_t idx = 0; idx != count; ++idx)
      {
         if(in.get())
         {
            String *string = String::Read(in, idx);
            string->lckCount = ReadVLN<std::size_t>(in);
            pd->tableString[idx] = string;
            pd->tableStringData.emplace(string, string);
         }
         else
         {
            pd->tableString[idx] = strNone;
            pd->tableStringIdx.emplace_back(idx);
         }
      }
   }

   //
   // StringTable::saveState
   //
   void StringTable::saveState(std::ostream &out) const
   {
      WriteVLN(out, pd->tableString.size());

      for(String *&str : pd->tableString)
      {
         if(str != strNone)
         {
            out << '\1';

            String::Write(out, str);
            WriteVLN(out, str->lckCount);
         }
         else
            out << '\0';
      }
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

