//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Thread classes.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__Thread_H__
#define ACSVM__Thread_H__

#include "PrintBuf.hpp"
#include "Stack.hpp"
#include "Store.hpp"
#include "Types.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   //
   // CallFrame
   //
   // Stores a call frame for execution.
   //
   class CallFrame
   {
   public:
      Word        *codePtr;
      Module      *module;
      ModuleScope *scopeMod;
      std::size_t  locArrC;
      std::size_t  locRegC;
   };

   //
   // ThreadState
   //
   class ThreadState
   {
   public:
      enum State
      {
         Inactive, // Inactive thread.
         Running,  // Running.
         Stopped,  // Will go inactive on next exec.
         Paused,   // Paused by instruction.
         WaitScrI, // Waiting on a numbered script.
         WaitScrS, // Waiting on a named script.
         WaitTag,  // Waiting on tagged object.
      };


      ThreadState() : state{Inactive}, data{0}, type{0} {}
      ThreadState(State state_) :
         state{state_}, data{0}, type{0} {}
      ThreadState(State state_, Word data_) :
         state{state_}, data{data_}, type{0} {}
      ThreadState(State state_, Word data_, Word type_) :
         state{state_}, data{data_}, type{type_} {}

      State state;

      // Extra state data. Used by:
      //    WaitScrI - Script number.
      //    WaitScrS - Script name index.
      //    WaitTag  - Tag number.
      Word data;

      // Extra state data. Used by:
      //    WaitTag - Tag type.
      Word type;
   };

   //
   // Thread
   //
   class Thread
   {
   public:
      Thread();
      virtual ~Thread();

      void exec();

      void start(Script *script, MapScope *map);

      Stack<CallFrame> callStk;
      Stack<Word>      dataStk;
      Store<Array>     localArr;
      Store<Word>      localReg;
      PrintBuf         printBuf;
      ThreadState      state;

      Word        *codePtr; // Instruction pointer.
      Module      *module;  // Current execution Module.
      GlobalScope *scopeGbl;
      HubScope    *scopeHub;
      MapScope    *scopeMap;
      ModuleScope *scopeMod;
      Script      *script;  // Current execution Script.
      Word         delay;   // Execution delay tics.
      Word         result;  // Code-defined thread result.


      static constexpr std::size_t CallStkSize =   8;
      static constexpr std::size_t DataStkSize = 256;

   private:
      void stop();
   };
}

#endif//ACSVM__Thread_H__

