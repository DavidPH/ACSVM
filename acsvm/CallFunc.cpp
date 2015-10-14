//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Internal CallFunc functions.
//
//-----------------------------------------------------------------------------

#include "CallFunc.hpp"

#include "Environ.hpp"
#include "Module.hpp"
#include "Scope.hpp"
#include "Thread.hpp"

#include <cinttypes>


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace ACSVM
{
   //
   // PrintArray
   //
   static void PrintArray(Thread *thread, Word const *argv, Word argc, Array const &arr)
   {
      Word idx = argv[1] + (argc > 2 ? argv[2] : 0);
      Word len = argc > 3 ? argv[3] : -1;

      thread->env->printArray(thread->printBuf, arr, idx, len);
   }

   //
   // StrCpyArray
   //
   static bool StrCpyArray(Thread *thread, Word const *argv, Array &dst)
   {
      Word    dstOff = argv[1] + argv[2];
      Word    dstLen = argv[3];
      String *src = thread->env->getString(argv[4]);
      Word    srcIdx = argv[5];

      if(srcIdx > src->len) return false;

      for(Word dstIdx = dstOff;;)
      {
         if(dstIdx - dstOff == dstLen) return false;
         if(!(dst[dstIdx++] = src->str[srcIdx++])) return true;
      }
   }
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace ACSVM
{
   //
   // void Nop()
   //
   bool CallFunc_Func_Nop(Thread *, Word const *, Word)
   {
      return false;
   }

   //======================================================
   // Printing Functions
   //

   //
   // void PrintChar(char c)
   //
   bool CallFunc_Func_PrintChar(Thread *thread, Word const *argv, Word)
   {
      thread->printBuf.reserve(1);
      thread->printBuf.put(static_cast<char>(argv[0]));
      return false;
   }

   //
   // str PrintEndStr()
   //
   bool CallFunc_Func_PrintEndStr(Thread *thread, Word const *, Word)
   {
      char const *data = thread->printBuf.data();
      std::size_t size = thread->printBuf.size();
      String     *str  = thread->env->getString(data, size);
      thread->printBuf.drop();
      thread->dataStk.push(~str->idx);
      return false;
   }

   //
   // void PrintFixD(fixed d)
   //
   bool CallFunc_Func_PrintFixD(Thread *thread, Word const *argv, Word)
   {
      // %E worst case: -3.276800e+04 == 13
      // %F worst case: -32767.999985 == 13
      // %G worst case: -1.52588e-05  == 12
      // %G should be maximally P+6 + extra exponent digits.
      thread->printBuf.reserve(12);
      thread->printBuf.format("%G", static_cast<std::int32_t>(argv[0]) / 65536.0);
      return false;
   }

   //
   // void PrintGblArr(int arr, int idx, int off = 0, int len = -1)
   //
   bool CallFunc_Func_PrintGblArr(Thread *thread, Word const *argv, Word argc)
   {
      PrintArray(thread, argv, argc, thread->scopeGbl->arrV[argv[0]]);
      return false;
   }

   //
   // void PrintHubArr(int arr, int idx, int off = 0, int len = -1)
   //
   bool CallFunc_Func_PrintHubArr(Thread *thread, Word const *argv, Word argc)
   {
      PrintArray(thread, argv, argc, thread->scopeHub->arrV[argv[0]]);
      return false;
   }

   //
   // void PrintIntB(int b)
   //
   bool CallFunc_Func_PrintIntB(Thread *thread, Word const *argv, Word)
   {
      // %B worst case: 11111111111111111111111111111111 == 32
      char buf[32], *end = buf+32, *itr = end;
      for(Word b = argv[0]; b; b >>= 1) *--itr = '0' + (b & 1);
      thread->printBuf.reserve(end - itr);
      thread->printBuf.put(itr, end - itr);
      return false;
   }

   //
   // void PrintIntD(int d)
   //
   bool CallFunc_Func_PrintIntD(Thread *thread, Word const *argv, Word)
   {
      // %d worst case: -2147483648 == 11
      thread->printBuf.reserve(11);
      thread->printBuf.format("%" PRId32, static_cast<std::int32_t>(argv[0]));
      return false;
   }

   //
   // void PrintIntX(int x)
   //
   bool CallFunc_Func_PrintIntX(Thread *thread, Word const *argv, Word)
   {
      // %d worst case: FFFFFFFF == 8
      thread->printBuf.reserve(8);
      thread->printBuf.format("%" PRIX32, static_cast<std::uint32_t>(argv[0]));
      return false;
   }

   //
   // void PrintLocArr(int arr, int idx, int off = 0, int len = -1)
   //
   bool CallFunc_Func_PrintLocArr(Thread *thread, Word const *argv, Word argc)
   {
      PrintArray(thread, argv, argc, thread->localArr[argv[0]]);
      return false;
   }

   //
   // void PrintModArr(int arr, int idx, int off = 0, int len = -1)
   //
   bool CallFunc_Func_PrintModArr(Thread *thread, Word const *argv, Word argc)
   {
      PrintArray(thread, argv, argc, *thread->scopeMod->arrV[argv[0]]);
      return false;
   }

   //
   // void PrintPush()
   //
   bool CallFunc_Func_PrintPush(Thread *thread, Word const *, Word)
   {
      thread->printBuf.push();
      return false;
   }

   //
   // void PrintString(str s)
   //
   bool CallFunc_Func_PrintString(Thread *thread, Word const *argv, Word)
   {
      String *s = thread->env->getString(argv[0]);
      thread->printBuf.reserve(s->len0);
      thread->printBuf.put(s->str, s->len0);
      return false;
   }

   //======================================================
   // String Functions
   //

   //
   // int GetChar(str s, int i)
   //
   bool CallFunc_Func_GetChar(Thread *thread, Word const *argv, Word)
   {
      thread->dataStk.push(thread->module->env->getString(argv[0])->get(argv[1]));
      return false;
   }

   //
   // int StrCpyGblArr(int dst, int idx, int dstOff, int dstLen, str src, int srcOff)
   //
   bool CallFunc_Func_StrCpyGblArr(Thread *thread, Word const *argv, Word)
   {
      thread->dataStk.push(StrCpyArray(thread, argv, thread->scopeGbl->arrV[argv[0]]));
      return false;
   }

   //
   // int StrCpyHubArr(int dst, int idx, int dstOff, int dstLen, str src, int srcOff)
   //
   bool CallFunc_Func_StrCpyHubArr(Thread *thread, Word const *argv, Word)
   {
      thread->dataStk.push(StrCpyArray(thread, argv, thread->scopeHub->arrV[argv[0]]));
      return false;
   }

   //
   // int StrCpyLocArr(int dst, int idx, int dstOff, int dstLen, str src, int srcOff)
   //
   bool CallFunc_Func_StrCpyLocArr(Thread *thread, Word const *argv, Word)
   {
      thread->dataStk.push(StrCpyArray(thread, argv, thread->localArr[argv[0]]));
      return false;
   }

   //
   // int StrCpyModArr(int dst, int idx, int dstOff, int dstLen, str src, int srcOff)
   //
   bool CallFunc_Func_StrCpyModArr(Thread *thread, Word const *argv, Word)
   {
      thread->dataStk.push(StrCpyArray(thread, argv, *thread->scopeMod->arrV[argv[0]]));
      return false;
   }

   //
   // int StrLen(str s)
   //
   bool CallFunc_Func_StrLen(Thread *thread, Word const *argv, Word)
   {
      thread->dataStk.push(thread->module->env->getString(argv[0])->len0);
      return false;
   }
}

// EOF

