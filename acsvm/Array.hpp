//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Array class.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__Array_H__
#define ACSVM__Array_H__

#include "Types.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // Array
   //
   class Array
   {
   public:
      Array() : data{nullptr} {}
      Array(Array const &) = delete;
      Array(Array &&array) : data{array.data} {array.data = nullptr;}
      ~Array() {FreeData(data);}

      Word &operator [] (Word idx);

      void clear();

      // If idx is allocated, returns that Word. Otherwise, returns a reference
      // to none set to 0.
      Word &find(Word idx);

      Word none;

   private:
      static constexpr std::size_t PageSize = 256;
      static constexpr std::size_t SegmSize = 256;
      static constexpr std::size_t BankSize = 256;
      static constexpr std::size_t DataSize = 256;

      using Page = Word [PageSize];
      using Segm = Page*[SegmSize];
      using Bank = Segm*[BankSize];
      using Data = Bank*[DataSize];

      Data *data;


      static void FreePage(Page *&data);
      static void FreeSegm(Segm *&data);
      static void FreeBank(Bank *&data);
      static void FreeData(Data *&data);
   };

   //
   // ArrayInit
   //
   class ArrayInit
   {
   public:
      ArrayInit();
      ~ArrayInit();

      void apply(Array &arr);

      void finish();

      Word get(Word idx);

      void reserve(Word count);

      void set(Word idx, Word value);

   private:
      struct PrivData;

      PrivData *pd;
   };
}

#endif//ACSVM__Array_H__

