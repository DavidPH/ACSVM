##-----------------------------------------------------------------------------
##
## Copyright (C) 2015-2025 David Hill
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for acsvm-util.
##
##-----------------------------------------------------------------------------


##----------------------------------------------------------------------------|
## Environment Configuration                                                  |
##

include_directories(.)


##----------------------------------------------------------------------------|
## Variables                                                                  |
##

set(ACSVM_Util_H
   Floats.hpp
)


##----------------------------------------------------------------------------|
## Targets                                                                    |
##

##
## acsvm-capi
##
add_library(acsvm-util ${ACSVM_SHARED_DECL}
   ${ACSVM_Util_H}
   Floats.cpp
)

target_link_libraries(acsvm-util acsvm)

ACSVM_INSTALL_LIB(acsvm-util Util Util)

## EOF

