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
#include <sstream>
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
public:
   Environment();

protected:
   virtual void loadModule(ACSVM::Module *module);
};


//----------------------------------------------------------------------------|
// Static Objects                                                             |
//

static bool NeedTestSaveEnv = false;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// CF_EndPrint
//
static bool CF_EndPrint(ACSVM::Thread *thread, ACSVM::Word const *, ACSVM::Word)
{
   std::cout << thread->printBuf.data() << '\n';
   thread->printBuf.drop();
   return false;
}

//
// CF_TestSave
//
static bool CF_TestSave(ACSVM::Thread *, ACSVM::Word const *, ACSVM::Word)
{
   NeedTestSaveEnv = true;
   return false;
}

//
// LoadModules
//
static void LoadModules(Environment &env, char const *const *argv, std::size_t argc)
{
   // Load modules.
   std::vector<ACSVM::Module *> modules;
   for(std::size_t i = 1; i < argc; ++i)
      modules.push_back(env.getModule(env.getModuleName(argv[i])));

   // Create and activate scopes.
   ACSVM::GlobalScope *global = env.getGlobalScope(0);  global->active = true;
   ACSVM::HubScope    *hub    = global->getHubScope(0); hub   ->active = true;
   ACSVM::MapScope    *map    = hub->getMapScope(0);    map   ->active = true;

   // Register modules with map scope.
   for(auto &module : modules)
      map->addModule(module);
   map->addModuleFinish();

   // Start Open scripts.
   map->scriptStartType(ACSVM::ScriptType::Open, nullptr, 0);
}


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

//
// Environment constructor
//
Environment::Environment()
{
   ACSVM::Word funcEndPrint = addCallFunc(CF_EndPrint);
   ACSVM::Word funcTestSave = addCallFunc(CF_TestSave);

   addCodeDataACS0( 86, {"", ACSVM::Code::CallFunc, 0, funcEndPrint});
   addCodeDataACS0(270, {"", ACSVM::Code::CallFunc, 0, funcEndPrint});

   addFuncDataACS0(0x10000, funcTestSave);
}

//
// Environment::loadModule
//
void Environment::loadModule(ACSVM::Module *module)
{
   std::ifstream in{module->name.s->str, std::ios_base::in | std::ios_base::binary};

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

   // Load modules.
   try
   {
      LoadModules(env, argv, argc);
   }
   catch(ACSVM::ReadError &e)
   {
      std::cerr << "Error loading modules: " << e.what() << std::endl;
      return EXIT_FAILURE;
   }

   // Execute until all threads terminate.
   while(env.hasActiveThread())
   {
      std::chrono::duration<double> rate{1.0 / 35};
      auto time = std::chrono::steady_clock::now() + rate;

      env.exec();

      if(NeedTestSaveEnv)
      {
         std::stringstream buf;
         env.saveState(buf);
         env.loadState(buf);

         NeedTestSaveEnv = false;
      }

      std::this_thread::sleep_until(time);
   }
}

// EOF

