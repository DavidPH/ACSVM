//----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//----------------------------------------------------------------------------
//
// Program entry point.
//
//----------------------------------------------------------------------------

#include "acsvm/Environ.hpp"

#include "acsvm/Code.hpp"
#include "acsvm/CodeData.hpp"
#include "acsvm/Error.hpp"
#include "acsvm/Module.hpp"
#include "acsvm/Script.hpp"
#include "acsvm/Thread.hpp"

#include <fstream>
#include <iostream>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Environment
//
class Environment : public ACSVM::Environment
{
protected:
   virtual void loadModule(ACSVM::Module *module);
};


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// EndPrint
//
static bool EndPrint(ACSVM::Thread *thread, ACSVM::Word const *, ACSVM::Word)
{
   std::cout << thread->printBuf.data() << '\n';
   thread->printBuf.drop();
   return false;
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

//
// Environment::loadModule
//
void Environment::loadModule(ACSVM::Module *module)
{
   std::ifstream in{module->name.s.get(), std::ios_base::out | std::ios_base::binary};

   if(!in) throw ACSVM::ReadError();

   std::vector<ACSVM::Byte> data;

   for(int c; c = in.get(), in;)
      data.push_back(c);

   module->readBytecode(data.data(), data.size());
}

//
// main
//
int main(int argc, char *argv[])
{
   Environment env;

   ACSVM::Word funcEndPrint = env.addCallFunc(EndPrint);

   env.addCodeDataACS0( 86, {"", ACSVM::Code::CallFunc, 0, funcEndPrint});
   env.addCodeDataACS0(270, {"", ACSVM::Code::CallFunc, 0, funcEndPrint});

   try
   {
      for(int i = 1; i < argc; ++i)
         env.getModule(env.getModuleName(argv[i]));
   }
   catch(ACSVM::ReadError &e)
   {
      std::cerr << "Error loading modules: " << e.what() << std::endl;
      return EXIT_FAILURE;
   }

   for(ACSVM::Script *head = env.getScriptHead(), *scr = head->envNext; scr != head; scr = scr->envNext)
   {
      if(scr->type == ACSVM::ScriptType::Open)
         env.getFreeThread()->start(scr);
   }

   env.exec();
}

// EOF

