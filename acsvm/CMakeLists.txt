##-----------------------------------------------------------------------------
##
## Copyright (C) 2015 David Hill
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for acsvm.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Environment Configuration                                                  |
##

include_directories(.)


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## acsvm
##
add_library(acsvm ${ACSVM_SHARED_DECL}
   Array.cpp
   CallFunc.cpp
   CodeData.cpp
   Environ.cpp
   Module.cpp
   Module/ReadBytecodeACS0.cpp
   Module/ReadBytecodeACSE.cpp
   PrintBuf.cpp
   Script.cpp
   String.cpp
   Thread.cpp
   ThreadExec.cpp
   Tracer.cpp
)

ACSVM_INSTALL_LIB(acsvm)

## EOF

