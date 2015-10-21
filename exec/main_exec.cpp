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
#include "acsvm/Scope.hpp"
#include "acsvm/Script.hpp"
#include "acsvm/Thread.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
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
   std::ifstream in{module->name.s.get(), std::ios_base::in | std::ios_base::binary};

   if(!in) throw ACSVM::ReadError("file open failure");

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

   // Load modules.
   std::vector<ACSVM::Module *> modules;
   try
   {
      for(int i = 1; i < argc; ++i)
         modules.push_back(env.getModule(env.getModuleName(argv[i])));
   }
   catch(ACSVM::ReadError &e)
   {
      std::cerr << "Error loading modules: " << e.what() << std::endl;
      return EXIT_FAILURE;
   }

   ACSVM::GlobalScope *global = env.getGlobalScope(0);  global->active = true;
   ACSVM::HubScope    *hub    = global->getHubScope(0); hub   ->active = true;
   ACSVM::MapScope    *map    = hub->getMapScope(0);    map   ->active = true;

   // Register modules with map scope.
   for(auto &module : modules)
      map->addModule(module);
   map->addModuleFinish();

   // Start Open scripts.
   for(ACSVM::Script *head = env.getScriptHead(), *scr = head->envNext; scr != head; scr = scr->envNext)
   {
      if(scr->type == ACSVM::ScriptType::Open)
         map->scriptStartForced(scr, nullptr, 0);
   }

   while(env.hasActiveThread())
   {
      std::chrono::duration<double> rate{1.0 / 35};
      auto time = std::chrono::steady_clock::now() + rate;

      env.exec();

      std::this_thread::sleep_until(time);
   }
}

// EOF

