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

#include "Array.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // ArrayInit::PrivData
   //
   struct ArrayInit::PrivData
   {
      std::vector<Word> dataV;
   };
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace ACSVM
{
   //
   // Array::operator [Word]
   //
   Word &Array::operator [] (Word idx)
   {
      if(!data) data = new Data[1]{};
      Bank *&bank = (*data)[idx / (BankSize * SegmSize * PageSize)];

      if(!bank) bank = new Bank[1]{};
      Segm *&segm = (*bank)[idx / (SegmSize * PageSize) % BankSize];

      if(!segm) segm = new Segm[1]{};
      Page *&page = (*segm)[idx / PageSize % SegmSize];

      if(!page) page = new Page[1]{};
      return (*page)[idx % PageSize];
   }

   //
   // Array::find
   //
   Word &Array::find(Word idx)
   {
      if(!data) return none = 0;
      Bank *&bank = (*data)[idx / (BankSize * SegmSize * PageSize)];

      if(!bank) return none = 0;
      Segm *&segm = (*bank)[idx / (SegmSize * PageSize) % BankSize];

      if(!segm) return none = 0;
      Page *&page = (*segm)[idx / PageSize % SegmSize];

      if(!page) return none = 0;
      return (*page)[idx % PageSize];
   }

   //
   // Array::FreePage
   //
   void Array::FreePage(Page *&page)
   {
      if(!page) return;

      delete[] page;
      page = nullptr;
   }

   //
   // Array::FreeSegm
   //
   void Array::FreeSegm(Segm *&segm)
   {
      if(!segm) return;

      for(Page *&page : *segm)
         FreePage(page);

      delete[] segm;
      segm = nullptr;
   }

   //
   // Array::FreeBank
   //
   void Array::FreeBank(Bank *&bank)
   {
      if(!bank) return;

      for(Segm *&segm : *bank)
         FreeSegm(segm);

      delete[] bank;
      bank = nullptr;
   }

   //
   // Array::FreeData
   //
   void Array::FreeData(Data *&data)
   {
      if(!data) return;

      for(Bank *&bank : *data)
         FreeBank(bank);

      delete[] data;
      data = nullptr;
   }

   //
   // ArrayInit constructor
   //
   ArrayInit::ArrayInit() :
      pd{new PrivData}
   {
   }

   //
   // ArrayInit destructor
   //
   ArrayInit::~ArrayInit()
   {
      delete pd;
   }

   //
   // ArrayInit::apply
   //
   void ArrayInit::apply(Array &arr)
   {
      Word idx = 0;
      for(Word value : pd->dataV)
      {
         if(value) arr[idx] = value;
         ++idx;
      }
   }

   //
   // ArrayInit::finish
   //
   void ArrayInit::finish()
   {
      // Clear out trailing zeroes.
      while(!pd->dataV.empty() && ! pd->dataV.back())
         pd->dataV.pop_back();

      // Shrink vector.
      pd->dataV.shrink_to_fit();

      // TODO: Break up initialization data into nonzero ranges.
   }

   //
   // ArrayInit::get
   //
   Word ArrayInit::get(Word idx)
   {
      return idx < pd->dataV.size() ? pd->dataV[idx] : 0;
   }

   //
   // ArrayInit::reserve
   //
   void ArrayInit::reserve(Word count)
   {
      pd->dataV.resize(count, 0);
   }

   //
   // ArrayInit::set
   //
   void ArrayInit::set(Word idx, Word value)
   {
      if(idx <= pd->dataV.size())
         pd->dataV.resize(idx + 1, 0);

      pd->dataV[idx] = value;
   }
}

// EOF

