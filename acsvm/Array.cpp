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
}

// EOF

