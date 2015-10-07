//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Store class.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__Store_H__
#define ACSVM__Store_H__

#include "Types.hpp"

#include <new>
#include <utility>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // Store
   //
   // Manages storage area for locals.
   //
   template<typename T>
   class Store
   {
   public:
      Store() : store{nullptr}, storeEnd{nullptr}, active{nullptr}, activeEnd{nullptr} {}
      ~Store() {clear(); ::operator delete(store);}

      // operator []
      T &operator [] (std::size_t idx) {return active[idx];}

      //
      // alloc
      //
      void alloc(std::size_t count)
      {
         // Possibly reallocate underlying storage.
         if(static_cast<std::size_t>(storeEnd - activeEnd) < count)
         {
            // Save pointers as indexes.
            std::size_t activeIdx    = active    - store;
            std::size_t activeEndIdx = activeEnd - store;
            std::size_t storeEndIdx  = storeEnd  - store;

            // Calculate new array size.
            if(SIZE_MAX / sizeof(T) - storeEndIdx < count * 2)
               throw std::bad_alloc();

            storeEndIdx += count * 2;

            // Allocate and initialize new array.
            T *storeNew = static_cast<T *>(::operator new(storeEndIdx * sizeof(T)));
            for(T *itrNew = storeNew, *itr = activeEnd, *end = store; itr != end;)
            {
               new(itrNew++) T(std::move(*--itr));
               itr->~T();
            }

            // Free old array.
            ::operator delete(store);

            // Restore pointers.
            store     = storeNew;
            active    = store + activeIdx;
            activeEnd = store + activeEndIdx;
            storeEnd  = store + storeEndIdx;
         }

         active = activeEnd;
         while(count--) new(activeEnd++) T{};
      }

      //
      // clear
      //
      void clear()
      {
         while(activeEnd != store)
           (--activeEnd)->~T();

         active = store;
      }

      //
      // free
      //
      // count must be the size (in elements)of the previous allocation.
      //
      void free(std::size_t count)
      {
        while(activeEnd != active)
           (--activeEnd)->~T();

        active -= count;
      }

      // size
      std::size_t size() const {return activeEnd - active;}

   private:
      T *store,  *storeEnd;
      T *active, *activeEnd;
   };
}

#endif//ACSVM__Store_H__

