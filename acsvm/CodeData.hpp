//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// CodeData classes.
//
//-----------------------------------------------------------------------------

#ifndef ACSVM__CodeData_H__
#define ACSVM__CodeData_H__

#include "Types.hpp"

#include <initializer_list>
#include <utility>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace ACSVM
{
   enum class Code;
   enum class CodeACS0;
   enum class Func;

   //
   // CodeData
   //
   // Internal code description.
   //
   class CodeData
   {
   public:
      Code code;

      Word argc;
   };

   //
   // CodeDataACS0
   //
   // ACS0 code description.
   //
   class CodeDataACS0
   {
   public:
      CodeDataACS0(char const *args, Code transCode, Word stackArgC,
         Word transFunc = 0);
      CodeDataACS0(CodeACS0 code, char const *args, Code transCode,
         Word stackArgC, Func transFunc);

      // Code index. If not an internally recognized code, is set to None.
      CodeACS0 code;

      // String describing the code's arguments.
      //    B - Single byte.
      //    b - Single byte if compressed, full word otherwise.
      //    H - Half word.
      //    h - Half word if compressed, full word otherwise.
      //    J - Full word as jump target.
      //    S - Full word as string index.
      //    W - Full word.
      char const *args;
      std::size_t argc;

      // Stack argument count.
      Word stackArgC;

      // Internal code to translate to.
      Code transCode;

      // CallFunc index to translate to.
      Word transFunc;
   };

   //
   // FuncDataACS0
   //
   // ACS0 CallFunc description.
   //
   class FuncDataACS0
   {
   public:
      FuncDataACS0(FuncDataACS0 const &) = delete;
      FuncDataACS0(FuncDataACS0 &&data);
      FuncDataACS0(Word transFunc);
      FuncDataACS0(Word transFunc, std::initializer_list<std::pair<Word, Code>> transCodes);
      ~FuncDataACS0();

      FuncDataACS0 &operator = (FuncDataACS0 &&data);

      // Internal code to translate to.
      Code getTransCode(Word argc) const;

      // CallFunc index to translate to.
      Word transFunc;

   private:
      std::pair<Word, Code> *transCodeV;
      std::size_t            transCodeC;
   };
}

#endif//ACSVM__CodeData_H__

