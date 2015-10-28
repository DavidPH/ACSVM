//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// HashMap class.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__HashMap_H__
#define ACSVM__HashMap_H__

#include "List.hpp"
#include "Types.hpp"
#include "Vector.hpp"

#include <functional>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // HashMapGetKey
   //
   // Helper class for HashMap.
   //
   template<typename Key, typename T, bool UseKeyMem>
   struct HashMapGetKey
   {
      template<Key T::*KeyMem>
      static Key const &Get(T *obj) {return obj->*KeyMem;}
   };

   //
   // HashMapGetKey<false>
   //
   template<typename Key, typename T>
   struct HashMapGetKey<Key, T, false>
   {
      template<Key T::*KeyMem>
      static Key const &Get(T *obj) {return *obj;}
   };

   //
   // HashMap
   //
   // Stores objects of type T that can be found by keys of type Key. If KeyMem
   // is non-null, it designates a member of T that stores its key. Otherwise,
   // T must be derived from Key.
   //
   // This class does not manage the lifetimes of the contained objects, and
   // objects must be unlinked by the unlink function before being destructed.
   // Although an exception is made to the latter if the clear function is
   // called before any other.
   //
   template<typename Key, typename T, ListLink<T> T::*LinkMem,
      Key T::*KeyMem = nullptr, typename Hash = std::hash<Key>,
      typename KeyEqual = std::equal_to<Key>>
   class HashMap
   {
   public:
      HashMap() : chainV{16}, objC{0}, growC{16} {}
      HashMap(std::size_t count, std::size_t growC_) :
         chainV{count}, objC{0}, growC{growC_} {}
      ~HashMap() {clear();}

      //
      // clear
      //
      void clear()
      {
         for(auto &chain : chainV)
         {
            while(auto obj = chain.next->obj)
               (obj->*LinkMem).unlink();
         }

         objC = 0;
      }

      //
      // find
      //
      T *find(Key const &key)
      {
         for(auto itr = chainV[hasher(key) % chainV.size()].next; itr->obj; itr = itr->next)
         {
            if(equal(key, GetKey(itr->obj)))
               return itr->obj;
         }

         return nullptr;
      }

      //
      // insert
      //
      void insert(T *obj)
      {
         if(objC >= chainV.size())
            resize(chainV.size() + chainV.size() / 2 + growC);

         ++objC;
         (obj->*LinkMem).insert(&chainV[hasher(GetKey(obj)) % chainV.size()]);
      }

      //
      // resize
      //
      void resize(std::size_t count)
      {
         auto oldChainV = std::move(chainV);
         chainV.alloc(count);

         for(auto &chain : oldChainV)
         {
            while(auto obj = chain.next->obj)
               (obj->*LinkMem).relink(&chainV[hasher(GetKey(obj)) % chainV.size()]);
         }
      }

      //
      // unlink
      //
      void unlink(T *obj)
      {
         --objC;
         (obj->*LinkMem).unlink();
      }

   private:
      Vector<ListLink<T>> chainV;
      Hash                hasher;
      KeyEqual            equal;

      std::size_t objC;
      std::size_t growC;


      //
      // GetKey
      //
      static Key const &GetKey(T *obj)
      {
         return HashMapGetKey<Key, T, !!KeyMem>::template Get<KeyMem>(obj);
      }
   };
}

#endif//ACSVM__HashMap_H__

