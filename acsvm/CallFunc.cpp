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
#include "Thread.hpp"

#include <cinttypes>


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
      String *s = thread->module->env->getString(argv[0]);
      thread->printBuf.reserve(s->len0);
      thread->printBuf.put(s->str, s->len0);
      return false;
   }

   //======================================================
   // String Functions
   //

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

