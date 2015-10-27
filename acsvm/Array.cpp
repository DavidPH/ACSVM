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

#include "BinaryIO.hpp"


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace ACSVM
{
   //
   // FreeData (Word)
   //
   static void FreeData(Word &)
   {
   }

   //
   // FreeData
   //
   template<typename T>
   static void FreeData(T *&data)
   {
      if(!data) return;

      for(auto &itr : *data)
         FreeData(itr);

      delete[] data;
      data = nullptr;
   }

   //
   // ReadData (Word)
   //
   static void ReadData(std::istream &in, Word &out)
   {
      out = ReadVLN<Word>(in);
   }

   //
   // ReadData
   //
   template<typename T>
   static void ReadData(std::istream &in, T *&out)
   {
      if(in.get())
      {
         if(!out) out = new T[1]{};

         for(auto &itr : *out)
            ReadData(in, itr);
      }
      else
         FreeData(out);
   }

   //
   // WriteData (Word)
   //
   static void WriteData(std::ostream &out, Word const &in)
   {
      WriteVLN(out, in);
   }

   //
   // WriteData
   //
   template<typename T>
   static void WriteData(std::ostream &out, T *const &in)
   {
      if(in)
      {
         out.put('\1');

         for(auto &itr : *in)
            WriteData(out, itr);
      }
      else
         out.put('\0');
   }
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
   Word Array::find(Word idx) const
   {
      if(!data) return 0;
      Bank *&bank = (*data)[idx / (BankSize * SegmSize * PageSize)];

      if(!bank) return 0;
      Segm *&segm = (*bank)[idx / (SegmSize * PageSize) % BankSize];

      if(!segm) return 0;
      Page *&page = (*segm)[idx / PageSize % SegmSize];

      if(!page) return 0;
      return (*page)[idx % PageSize];
   }

   //
   // Array::clear
   //
   void Array::clear()
   {
      FreeData(data);
   }

   //
   // Array::loadState
   //
   void Array::loadState(std::istream &in)
   {
      ReadData(in, data);
   }

   //
   // Array::saveState
   //
   void Array::saveState(std::ostream &out) const
   {
      WriteData(out, data);
   }
}

// EOF

