##-----------------------------------------------------------------------------
##
## Copyright (C) 2015-2025 David Hill
##
## See COPYING for license information.
##
##-----------------------------------------------------------------------------
##
## CMake file for acsvm-exec.
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
## acsvm-exec
##
add_executable(acsvm-exec
   main_exec.cpp
)

target_link_libraries(acsvm-exec acsvm-util)

ACSVM_INSTALL_EXE(acsvm-exec)

##
## acsvm-execc
##
## Used to test the C API.
##
#if(EXISTS "${CMAKE_SOURCE_DIR}/CAPI")
#   add_executable(acsvm-execc
#      main_execc.c
#   )
#
#   target_link_libraries(acsvm-execc acsvm-capi)
#endif()

## EOF

