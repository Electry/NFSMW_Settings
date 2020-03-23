cmake_minimum_required(VERSION 2.8)

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VITASDK})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
  else()
    message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
  endif()
endif()

project(NFSMW_Settings)
include("${VITASDK}/share/vita.cmake" REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q -Wall -O3 -std=gnu99 -Wno-strict-aliasing")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions")

add_executable(NFSMW_Settings
  src/main.c
  src/patch.c
  src/config.c
  src/encode.c
)

target_link_libraries(NFSMW_Settings
  taihen_stub
  SceAppMgr_stub
  SceDisplay_stub
  SceCtrl_stub
  SceKernelModulemgr_stub
  SceLibKernel_stub
  SceIofilemgr_stub
  SceLibc_stub
  gcc
)

set_target_properties(NFSMW_Settings
  PROPERTIES LINK_FLAGS "-nostdlib"
)

vita_create_self(nfsmw_settings.suprx NFSMW_Settings
  CONFIG ${CMAKE_SOURCE_DIR}/NFSMW_Settings.yml
)
