//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// List of all codes.
//
//-----------------------------------------------------------------------------


#ifdef ACSVM_CodeList

ACSVM_CodeList(Nop,          0)
ACSVM_CodeList(Kill,         2)

// Binary operator codes.
#define ACSVM_CodeList_BinaryOpSet(name) \
   ACSVM_CodeList(name,           0) \
   ACSVM_CodeList(name##_GblArr,  1) \
   ACSVM_CodeList(name##_GblReg,  1) \
   ACSVM_CodeList(name##_LocArr,  1) \
   ACSVM_CodeList(name##_LocReg,  1) \
   ACSVM_CodeList(name##_MapArr,  1) \
   ACSVM_CodeList(name##_MapReg,  1) \
   ACSVM_CodeList(name##_WldArr,  1) \
   ACSVM_CodeList(name##_WldReg,  1)
ACSVM_CodeList_BinaryOpSet(AddU)
ACSVM_CodeList_BinaryOpSet(AndU)
ACSVM_CodeList_BinaryOpSet(DivI)
ACSVM_CodeList_BinaryOpSet(ModI)
ACSVM_CodeList_BinaryOpSet(MulU)
ACSVM_CodeList_BinaryOpSet(OrIU)
ACSVM_CodeList_BinaryOpSet(OrXU)
ACSVM_CodeList_BinaryOpSet(ShLU)
ACSVM_CodeList_BinaryOpSet(ShRI)
ACSVM_CodeList_BinaryOpSet(SubU)
#undef ACSVM_CodeList_BinaryOpSet
ACSVM_CodeList(CmpI_GE,      0)
ACSVM_CodeList(CmpI_GT,      0)
ACSVM_CodeList(CmpI_LE,      0)
ACSVM_CodeList(CmpI_LT,      0)
ACSVM_CodeList(CmpU_EQ,      0)
ACSVM_CodeList(CmpU_NE,      0)
ACSVM_CodeList(DivX,         0)
ACSVM_CodeList(LAnd,         0)
ACSVM_CodeList(LOrI,         0)
ACSVM_CodeList(MulX,         0)

// Call codes.
ACSVM_CodeList(Call_Lit,     1)
ACSVM_CodeList(Call_Stk,     0)
ACSVM_CodeList(CallFunc,     2)
ACSVM_CodeList(CallFunc_Lit, 0)
ACSVM_CodeList(CallSpec,     2)
ACSVM_CodeList(CallSpec_Lit, 0)
ACSVM_CodeList(CallSpec_R1,  2)
ACSVM_CodeList(Retn,         0)

// Drop codes.
ACSVM_CodeList(Drop_GblArr,  1)
ACSVM_CodeList(Drop_GblReg,  1)
ACSVM_CodeList(Drop_LocArr,  1)
ACSVM_CodeList(Drop_LocReg,  1)
ACSVM_CodeList(Drop_MapArr,  1)
ACSVM_CodeList(Drop_MapReg,  1)
ACSVM_CodeList(Drop_Nul,     0)
ACSVM_CodeList(Drop_ScrRet,  0)
ACSVM_CodeList(Drop_WldArr,  1)
ACSVM_CodeList(Drop_WldReg,  1)

// Jump codes.
ACSVM_CodeList(Jcnd_Lit,     2)
ACSVM_CodeList(Jcnd_Nil,     1)
ACSVM_CodeList(Jcnd_Tab,     1)
ACSVM_CodeList(Jcnd_Tru,     1)
ACSVM_CodeList(Jump_Lit,     1)
ACSVM_CodeList(Jump_Stk,     0)

// Push codes.
ACSVM_CodeList(Pfun_Lit,     1)
ACSVM_CodeList(Pstr_Stk,     0)
ACSVM_CodeList(Push_GblArr,  1)
ACSVM_CodeList(Push_GblReg,  1)
ACSVM_CodeList(Push_Lit,     1)
ACSVM_CodeList(Push_LitArr,  0)
ACSVM_CodeList(Push_LocArr,  1)
ACSVM_CodeList(Push_LocReg,  1)
ACSVM_CodeList(Push_MapArr,  1)
ACSVM_CodeList(Push_MapReg,  1)
ACSVM_CodeList(Push_WldArr,  1)
ACSVM_CodeList(Push_WldReg,  1)

// Script control codes.
ACSVM_CodeList(ScrDelay,     0)
ACSVM_CodeList(ScrDelay_Lit, 1)
ACSVM_CodeList(ScrHalt,      0)
ACSVM_CodeList(ScrRestart,   0)
ACSVM_CodeList(ScrTerm,      0)
ACSVM_CodeList(ScrWaitI,     0)
ACSVM_CodeList(ScrWaitI_Lit, 1)
ACSVM_CodeList(ScrWaitS,     0)
ACSVM_CodeList(ScrWaitS_Lit, 1)

// Stack control codes.
ACSVM_CodeList(Copy,         0)
ACSVM_CodeList(Swap,         0)

// Unary operator codes.
#define ACSVM_CodeList_UnaryOpSet(name) \
   ACSVM_CodeList(name##_GblArr,  1) \
   ACSVM_CodeList(name##_GblReg,  1) \
   ACSVM_CodeList(name##_LocArr,  1) \
   ACSVM_CodeList(name##_LocReg,  1) \
   ACSVM_CodeList(name##_MapArr,  1) \
   ACSVM_CodeList(name##_MapReg,  1) \
   ACSVM_CodeList(name##_WldArr,  1) \
   ACSVM_CodeList(name##_WldReg,  1)
ACSVM_CodeList_UnaryOpSet(DecU)
ACSVM_CodeList_UnaryOpSet(IncU)
#undef ACSVM_CodeList_UnaryOpSet
ACSVM_CodeList(InvU,         0)
ACSVM_CodeList(NegI,         0)
ACSVM_CodeList(NotU,         0)

#undef ACSVM_CodeList
#endif


#ifdef ACSVM_CodeListACS0

ACSVM_CodeListACS0(Nop,            0, "",       Nop,          0, None)
ACSVM_CodeListACS0(ScrTerm,        1, "",       ScrTerm,      0, None)
ACSVM_CodeListACS0(ScrHalt,        2, "",       ScrHalt,      0, None)
ACSVM_CodeListACS0(Push_Lit,       3, "W",      Push_Lit,     0, None)
ACSVM_CodeListACS0(CallSpec_1,     4, "W",      CallSpec,     1, None)
ACSVM_CodeListACS0(CallSpec_2,     5, "W",      CallSpec,     2, None)
ACSVM_CodeListACS0(CallSpec_3,     6, "W",      CallSpec,     3, None)
ACSVM_CodeListACS0(CallSpec_4,     7, "W",      CallSpec,     4, None)
ACSVM_CodeListACS0(CallSpec_5,     8, "W",      CallSpec,     5, None)
ACSVM_CodeListACS0(CallSpec_1L,    9, "WW",     CallSpec_Lit, 0, None)
ACSVM_CodeListACS0(CallSpec_2L,   10, "WWW",    CallSpec_Lit, 0, None)
ACSVM_CodeListACS0(CallSpec_3L,   11, "WWWW",   CallSpec_Lit, 0, None)
ACSVM_CodeListACS0(CallSpec_4L,   12, "WWWWW",  CallSpec_Lit, 0, None)
ACSVM_CodeListACS0(CallSpec_5L,   13, "WWWWWW", CallSpec_Lit, 0, None)
ACSVM_CodeListACS0(AddU,          14, "",       AddU,         2, None)
ACSVM_CodeListACS0(SubU,          15, "",       SubU,         2, None)
ACSVM_CodeListACS0(MulU,          16, "",       MulU,         2, None)
ACSVM_CodeListACS0(DivI,          17, "",       DivI,         2, None)
ACSVM_CodeListACS0(ModI,          18, "",       ModI,         2, None)
ACSVM_CodeListACS0(CmpU_EQ,       19, "",       CmpU_EQ,      2, None)
ACSVM_CodeListACS0(CmpU_NE,       20, "",       CmpU_NE,      2, None)
ACSVM_CodeListACS0(CmpI_LT,       21, "",       CmpI_LT,      2, None)
ACSVM_CodeListACS0(CmpI_GT,       22, "",       CmpI_GT,      2, None)
ACSVM_CodeListACS0(CmpI_LE,       23, "",       CmpI_LE,      2, None)
ACSVM_CodeListACS0(CmpI_GE,       24, "",       CmpI_GE,      2, None)
ACSVM_CodeListACS0(Drop_LocReg,   25, "b",      Drop_LocReg,  1, None)
ACSVM_CodeListACS0(Drop_MapReg,   26, "b",      Drop_MapReg,  1, None)
ACSVM_CodeListACS0(Drop_WldReg,   27, "b",      Drop_WldReg,  1, None)
ACSVM_CodeListACS0(Push_LocReg,   28, "b",      Push_LocReg,  1, None)
ACSVM_CodeListACS0(Push_MapReg,   29, "b",      Push_MapReg,  1, None)
ACSVM_CodeListACS0(Push_WldReg,   30, "b",      Push_WldReg,  1, None)
ACSVM_CodeListACS0(AddU_LocReg,   31, "b",      AddU_LocReg,  1, None)
ACSVM_CodeListACS0(AddU_MapReg,   32, "b",      AddU_MapReg,  1, None)
ACSVM_CodeListACS0(AddU_WldReg,   33, "b",      AddU_WldReg,  1, None)
ACSVM_CodeListACS0(SubU_LocReg,   34, "b",      SubU_LocReg,  1, None)
ACSVM_CodeListACS0(SubU_MapReg,   35, "b",      SubU_MapReg,  1, None)
ACSVM_CodeListACS0(SubU_WldReg,   36, "b",      SubU_WldReg,  1, None)
ACSVM_CodeListACS0(MulU_LocReg,   37, "b",      MulU_LocReg,  1, None)
ACSVM_CodeListACS0(MulU_MapReg,   38, "b",      MulU_MapReg,  1, None)
ACSVM_CodeListACS0(MulU_WldReg,   39, "b",      MulU_WldReg,  1, None)
ACSVM_CodeListACS0(DivI_LocReg,   40, "b",      DivI_LocReg,  1, None)
ACSVM_CodeListACS0(DivI_MapReg,   41, "b",      DivI_MapReg,  1, None)
ACSVM_CodeListACS0(DivI_WldReg,   42, "b",      DivI_WldReg,  1, None)
ACSVM_CodeListACS0(ModI_LocReg,   43, "b",      ModI_LocReg,  1, None)
ACSVM_CodeListACS0(ModI_MapReg,   44, "b",      ModI_MapReg,  1, None)
ACSVM_CodeListACS0(ModI_WldReg,   45, "b",      ModI_WldReg,  1, None)
ACSVM_CodeListACS0(IncU_LocReg,   46, "b",      IncU_LocReg,  1, None)
ACSVM_CodeListACS0(IncU_MapReg,   47, "b",      IncU_MapReg,  1, None)
ACSVM_CodeListACS0(IncU_WldReg,   48, "b",      IncU_WldReg,  1, None)
ACSVM_CodeListACS0(DecU_LocReg,   49, "b",      DecU_LocReg,  1, None)
ACSVM_CodeListACS0(DecU_MapReg,   50, "b",      DecU_MapReg,  1, None)
ACSVM_CodeListACS0(DecU_WldReg,   51, "b",      DecU_WldReg,  1, None)
ACSVM_CodeListACS0(Jump_Lit,      52, "J",      Jump_Lit,     0, None)
ACSVM_CodeListACS0(Jcnd_Tru,      53, "J",      Jcnd_Tru,     1, None)
ACSVM_CodeListACS0(Drop_Nul,      54, "",       Drop_Nul,     1, None)
ACSVM_CodeListACS0(ScrDelay,      55, "",       ScrDelay,     1, None)
ACSVM_CodeListACS0(ScrDelay_Lit,  56, "W",      ScrDelay_Lit, 0, None)

ACSVM_CodeListACS0(ScrRestart,    69, "",       ScrRestart,   0, None)
ACSVM_CodeListACS0(LAnd,          70, "",       LAnd,         2, None)
ACSVM_CodeListACS0(LOrI,          71, "",       LOrI,         2, None)
ACSVM_CodeListACS0(AndU,          72, "",       AndU,         2, None)
ACSVM_CodeListACS0(OrIU,          73, "",       OrIU,         2, None)
ACSVM_CodeListACS0(OrXU,          74, "",       OrXU,         2, None)
ACSVM_CodeListACS0(NotU,          75, "",       NotU,         1, None)
ACSVM_CodeListACS0(ShLU,          76, "",       ShLU,         2, None)
ACSVM_CodeListACS0(ShRI,          77, "",       ShRI,         2, None)
ACSVM_CodeListACS0(NegI,          78, "",       NegI,         1, None)
ACSVM_CodeListACS0(Jcnd_Nil,      79, "J",      Jcnd_Nil,     1, None)

ACSVM_CodeListACS0(ScrWaitI,      81, "",       ScrWaitI,     1, None)
ACSVM_CodeListACS0(ScrWaitI_Lit,  82, "W",      ScrWaitI_Lit, 0, None)

ACSVM_CodeListACS0(Jcnd_Lit,      84, "WJ",     Jcnd_Lit,     1, None)
ACSVM_CodeListACS0(PrintPush,     85, "",       CallFunc,     0, PrintPush)

ACSVM_CodeListACS0(PrintString,   87, "",       CallFunc,     1, PrintString)
ACSVM_CodeListACS0(PrintIntD,     88, "",       CallFunc,     1, PrintIntD)
ACSVM_CodeListACS0(PrintChar,     89, "",       CallFunc,     1, PrintChar)

ACSVM_CodeListACS0(MulX,         136, "",       MulX,         2, None)
ACSVM_CodeListACS0(DivX,         137, "",       DivX,         2, None)

ACSVM_CodeListACS0(PrintFixD,    157, "",       CallFunc,     1, PrintFixD)

ACSVM_CodeListACS0(Push_LitB,    167, "B",      Push_Lit,     0, None)
ACSVM_CodeListACS0(CallSpec_1LB, 168, "WB",     CallSpec_Lit, 0, None)
ACSVM_CodeListACS0(CallSpec_2LB, 169, "WBB",    CallSpec_Lit, 0, None)
ACSVM_CodeListACS0(CallSpec_3LB, 170, "WBBB",   CallSpec_Lit, 0, None)
ACSVM_CodeListACS0(CallSpec_4LB, 171, "WBBBB",  CallSpec_Lit, 0, None)
ACSVM_CodeListACS0(CallSpec_5LB, 172, "WBBBBB", CallSpec_Lit, 0, None)
ACSVM_CodeListACS0(ScrDelay_LB,  173, "B",      ScrDelay_Lit, 0, None)
ACSVM_CodeListACS0(Push_LitArrB, 175, "",       Push_LitArr,  0, None)
ACSVM_CodeListACS0(Push_Lit2B,   176, "BB",     Push_LitArr,  0, None)
ACSVM_CodeListACS0(Push_Lit3B,   177, "BBB",    Push_LitArr,  0, None)
ACSVM_CodeListACS0(Push_Lit4B,   178, "BBBB",   Push_LitArr,  0, None)
ACSVM_CodeListACS0(Push_Lit5B,   179, "BBBBB",  Push_LitArr,  0, None)

ACSVM_CodeListACS0(Drop_GblReg,  181, "b",      Drop_GblReg,  1, None)
ACSVM_CodeListACS0(Push_GblReg,  182, "b",      Push_GblReg,  0, None)
ACSVM_CodeListACS0(AddU_GblReg,  183, "b",      AddU_GblReg,  1, None)
ACSVM_CodeListACS0(SubU_GblReg,  184, "b",      SubU_GblReg,  1, None)
ACSVM_CodeListACS0(MulU_GblReg,  185, "b",      MulU_GblReg,  1, None)
ACSVM_CodeListACS0(DivI_GblReg,  186, "b",      DivI_GblReg,  1, None)
ACSVM_CodeListACS0(ModI_GblReg,  187, "b",      ModI_GblReg,  1, None)
ACSVM_CodeListACS0(IncU_GblReg,  188, "b",      IncU_GblReg,  1, None)
ACSVM_CodeListACS0(DecU_GblReg,  189, "b",      DecU_GblReg,  1, None)

ACSVM_CodeListACS0(Call_Lit,     203, "b",      Call_Lit,     0, None)
ACSVM_CodeListACS0(Call_Nul,     204, "b",      Call_Lit,     0, None)
ACSVM_CodeListACS0(Retn_Nul,     205, "",       Retn,         0, None)
ACSVM_CodeListACS0(Retn_Stk,     206, "",       Retn,         0, None)
ACSVM_CodeListACS0(Push_MapArr,  207, "b",      Push_MapArr,  1, None)
ACSVM_CodeListACS0(Drop_MapArr,  208, "b",      Drop_MapArr,  2, None)
ACSVM_CodeListACS0(AddU_MapArr,  209, "b",      AddU_MapArr,  2, None)
ACSVM_CodeListACS0(SubU_MapArr,  210, "b",      SubU_MapArr,  2, None)
ACSVM_CodeListACS0(MulU_MapArr,  211, "b",      MulU_MapArr,  2, None)
ACSVM_CodeListACS0(DivI_MapArr,  212, "b",      DivI_MapArr,  2, None)
ACSVM_CodeListACS0(ModI_MapArr,  213, "b",      ModI_MapArr,  2, None)
ACSVM_CodeListACS0(IncU_MapArr,  214, "b",      IncU_MapArr,  2, None)
ACSVM_CodeListACS0(DecU_MapArr,  215, "b",      DecU_MapArr,  2, None)
ACSVM_CodeListACS0(Copy,         216, "",       Copy,         1, None)
ACSVM_CodeListACS0(Swap,         217, "",       Swap,         2, None)

ACSVM_CodeListACS0(Pstr_Stk,     225, "",       Pstr_Stk,     1, None)
ACSVM_CodeListACS0(Push_WldArr,  226, "b",      Push_WldArr,  1, None)
ACSVM_CodeListACS0(Drop_WldArr,  227, "b",      Drop_WldArr,  2, None)
ACSVM_CodeListACS0(AddU_WldArr,  228, "b",      AddU_WldArr,  2, None)
ACSVM_CodeListACS0(SubU_WldArr,  229, "b",      SubU_WldArr,  2, None)
ACSVM_CodeListACS0(MulU_WldArr,  230, "b",      MulU_WldArr,  2, None)
ACSVM_CodeListACS0(DivI_WldArr,  231, "b",      DivI_WldArr,  2, None)
ACSVM_CodeListACS0(ModI_WldArr,  232, "b",      ModI_WldArr,  2, None)
ACSVM_CodeListACS0(IncU_WldArr,  233, "b",      IncU_WldArr,  2, None)
ACSVM_CodeListACS0(DecU_WldArr,  234, "b",      DecU_WldArr,  2, None)
ACSVM_CodeListACS0(Push_GblArr,  235, "b",      Push_GblArr,  1, None)
ACSVM_CodeListACS0(Drop_GblArr,  236, "b",      Drop_GblArr,  2, None)
ACSVM_CodeListACS0(AddU_GblArr,  237, "b",      AddU_GblArr,  2, None)
ACSVM_CodeListACS0(SubU_GblArr,  238, "b",      SubU_GblArr,  2, None)
ACSVM_CodeListACS0(MulU_GblArr,  239, "b",      MulU_GblArr,  2, None)
ACSVM_CodeListACS0(DivI_GblArr,  240, "b",      DivI_GblArr,  2, None)
ACSVM_CodeListACS0(ModI_GblArr,  241, "b",      ModI_GblArr,  2, None)
ACSVM_CodeListACS0(IncU_GblArr,  242, "b",      IncU_GblArr,  2, None)
ACSVM_CodeListACS0(DecU_GblArr,  243, "b",      DecU_GblArr,  2, None)

ACSVM_CodeListACS0(StrLen,       253, "",       CallFunc,     1, StrLen)

ACSVM_CodeListACS0(Jcnd_Tab,     256, "",       Jcnd_Tab,     1, None)
ACSVM_CodeListACS0(Drop_ScrRet,  257, "",       Drop_ScrRet,  1, None)

ACSVM_CodeListACS0(CallSpec_5R1, 263, "W",      CallSpec_R1,  5, None)

ACSVM_CodeListACS0(AndU_LocReg,  291, "b",      AndU_LocReg,  1, None)
ACSVM_CodeListACS0(AndU_MapReg,  292, "b",      AndU_MapReg,  1, None)
ACSVM_CodeListACS0(AndU_WldReg,  293, "b",      AndU_WldReg,  1, None)
ACSVM_CodeListACS0(AndU_GblReg,  294, "b",      AndU_GblReg,  1, None)
ACSVM_CodeListACS0(AndU_MapArr,  295, "b",      AndU_MapArr,  2, None)
ACSVM_CodeListACS0(AndU_WldArr,  296, "b",      AndU_WldArr,  2, None)
ACSVM_CodeListACS0(AndU_GblArr,  297, "b",      AndU_GblArr,  2, None)
ACSVM_CodeListACS0(OrXU_LocReg,  298, "b",      OrXU_LocReg,  1, None)
ACSVM_CodeListACS0(OrXU_MapReg,  299, "b",      OrXU_MapReg,  1, None)
ACSVM_CodeListACS0(OrXU_WldReg,  300, "b",      OrXU_WldReg,  1, None)
ACSVM_CodeListACS0(OrXU_GblReg,  301, "b",      OrXU_GblReg,  1, None)
ACSVM_CodeListACS0(OrXU_MapArr,  302, "b",      OrXU_MapArr,  2, None)
ACSVM_CodeListACS0(OrXU_WldArr,  303, "b",      OrXU_WldArr,  2, None)
ACSVM_CodeListACS0(OrXU_GblArr,  304, "b",      OrXU_GblArr,  2, None)
ACSVM_CodeListACS0(OrIU_LocReg,  305, "b",      OrIU_LocReg,  1, None)
ACSVM_CodeListACS0(OrIU_MapReg,  306, "b",      OrIU_MapReg,  1, None)
ACSVM_CodeListACS0(OrIU_WldReg,  307, "b",      OrIU_WldReg,  1, None)
ACSVM_CodeListACS0(OrIU_GblReg,  308, "b",      OrIU_GblReg,  1, None)
ACSVM_CodeListACS0(OrIU_MapArr,  309, "b",      OrIU_MapArr,  2, None)
ACSVM_CodeListACS0(OrIU_WldArr,  310, "b",      OrIU_WldArr,  2, None)
ACSVM_CodeListACS0(OrIU_GblArr,  311, "b",      OrIU_GblArr,  2, None)
ACSVM_CodeListACS0(ShLU_LocReg,  312, "b",      ShLU_LocReg,  1, None)
ACSVM_CodeListACS0(ShLU_MapReg,  313, "b",      ShLU_MapReg,  1, None)
ACSVM_CodeListACS0(ShLU_WldReg,  314, "b",      ShLU_WldReg,  1, None)
ACSVM_CodeListACS0(ShLU_GblReg,  315, "b",      ShLU_GblReg,  1, None)
ACSVM_CodeListACS0(ShLU_MapArr,  316, "b",      ShLU_MapArr,  2, None)
ACSVM_CodeListACS0(ShLU_WldArr,  317, "b",      ShLU_WldArr,  2, None)
ACSVM_CodeListACS0(ShLU_GblArr,  318, "b",      ShLU_GblArr,  2, None)
ACSVM_CodeListACS0(ShRI_LocReg,  319, "b",      ShRI_LocReg,  1, None)
ACSVM_CodeListACS0(ShRI_MapReg,  320, "b",      ShRI_MapReg,  1, None)
ACSVM_CodeListACS0(ShRI_WldReg,  321, "b",      ShRI_WldReg,  1, None)
ACSVM_CodeListACS0(ShRI_GblReg,  322, "b",      ShRI_GblReg,  1, None)
ACSVM_CodeListACS0(ShRI_MapArr,  323, "b",      ShRI_MapArr,  2, None)
ACSVM_CodeListACS0(ShRI_WldArr,  324, "b",      ShRI_WldArr,  2, None)
ACSVM_CodeListACS0(ShRI_GblArr,  325, "b",      ShRI_GblArr,  2, None)

ACSVM_CodeListACS0(InvU,         330, "",       InvU,         1, None)

ACSVM_CodeListACS0(PrintIntB,    349, "",       CallFunc,     1, PrintIntB)
ACSVM_CodeListACS0(PrintIntX,    350, "",       CallFunc,     1, PrintIntX)
ACSVM_CodeListACS0(CallFunc,     351, "bh",     CallFunc,     0, None)

ACSVM_CodeListACS0(Pfun_Lit,     359, "b",      Pfun_Lit,     0, None)
ACSVM_CodeListACS0(Call_Stk,     360, "",       Call_Stk,     1, None)
ACSVM_CodeListACS0(ScrWaitS,     361, "",       ScrWaitS,     1, None)

ACSVM_CodeListACS0(Jump_Stk,     363, "",       Jump_Stk,     1, None)
ACSVM_CodeListACS0(Drop_LocArr,  364, "b",      Drop_LocArr,  2, None)
ACSVM_CodeListACS0(Push_LocArr,  365, "b",      Push_LocArr,  1, None)

#undef ACSVM_CodeListACS0
#endif


#ifdef ACSVM_FuncList

ACSVM_FuncList(Nop)

// Printing functions.
ACSVM_FuncList(PrintChar)
ACSVM_FuncList(PrintFixD)
ACSVM_FuncList(PrintIntB)
ACSVM_FuncList(PrintIntD)
ACSVM_FuncList(PrintIntX)
ACSVM_FuncList(PrintPush)
ACSVM_FuncList(PrintString)

// String functions.
ACSVM_FuncList(StrLen)

#undef ACSVM_FuncList
#endif

// EOF

