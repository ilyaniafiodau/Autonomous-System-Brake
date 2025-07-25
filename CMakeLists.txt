cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()

# Default to C+20
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

add_definitions(
  -DHARDWARE_VERSION_MAJOR=1
  -DHARDWARE_VERSION_MINOR=0
  -DSOFTWARE_VERSION_MAJOR=1
  -DSOFTWARE_VERSION_MINOR=0
  -DVCS_REVISION_ID=0x${GIT_HASH}
)

set(APP_SOURCES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/src/fsm/fsm_task.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/fsm/fsm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/init/init_task.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rte/generated/rte.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/scheduler/scheduler_task.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/acs/acs_task.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/monitoring/monitoring_task.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/supervisor/supervisor_task.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/brake_warning_light/brake_warning_light_task.cpp
)

set(TASKS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/
  ${CMAKE_CURRENT_SOURCE_DIR}/src/fsm
  ${CMAKE_CURRENT_SOURCE_DIR}/src/init
  ${CMAKE_CURRENT_SOURCE_DIR}/src/scheduler
  ${CMAKE_CURRENT_SOURCE_DIR}/src/acs
  ${CMAKE_CURRENT_SOURCE_DIR}/src/monitoring
  ${CMAKE_CURRENT_SOURCE_DIR}/src/supervisor
   ${CMAKE_CURRENT_SOURCE_DIR}/src/brake_warning_light
)

include_directories(${PROJECT_NAME} 
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/submodules/tinyfsm/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include 
  ${CMAKE_CURRENT_SOURCE_DIR}/platform
  ${TASKS}
)

set(IS_NUCLEO OFF)
set(IS_LINUX  OFF)

if(DEFINED ENV{GLOBAL_TARGET_PLATFORM})
  if ($ENV{GLOBAL_TARGET_PLATFORM} STREQUAL "NUCLEO")
    set(IS_NUCLEO ON)
  endif()
  if($ENV{GLOBAL_TARGET_PLATFORM} STREQUAL "LINUX")
    set(IS_LINUX ON)
  endif()
else()
  message("ERROR: Target platform is not set")
endif()

########## LINUX BUILDING BEGIN ##########
if(IS_LINUX)
  project(asb_linux)
  set(platform ${CMAKE_CURRENT_SOURCE_DIR}/platform/specific/linux)
  set(DRIVER_SOURCES
    ${platform}/canbus.cpp
    ${platform}/io.cpp
    ${platform}/socketcan/socketcan.c
    ${platform}/time.cpp
  )
  add_executable(${PROJECT_NAME} ${APP_SOURCES} ${DRIVER_SOURCES})
endif()
########## LINUX BUILDING END   ##########

########## MBED  BUILDING BEGIN ##########
if(IS_NUCLEO)
  set(platform ${CMAKE_CURRENT_SOURCE_DIR}/platform/specific/nucleo-f446re)
  set(MBED_PATH ${CMAKE_CURRENT_SOURCE_DIR}/mbed-os CACHE INTERNAL "")
  set(MBED_CONFIG_PATH ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "")
  set(DRIVER_SOURCES
    ${platform}/canbus.cpp
    ${platform}/io.cpp
    ${platform}/time.cpp
  )
  include(${MBED_PATH}/tools/cmake/app.cmake)
  project(asb_nucleo)
  add_subdirectory(${MBED_PATH})
  target_compile_options(mbed-core INTERFACE "$<$<COMPILE_LANGUAGE:C>:-Wno-pedantic;-Wno-write-strings>")
  target_compile_options(mbed-core INTERFACE "$<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile;-Wno-register;-Wno-write-strings>")
  add_executable(${PROJECT_NAME} ${APP_SOURCES} ${DRIVER_SOURCES})
  mbed_configure_app_target(${PROJECT_NAME})
  target_compile_options(${PROJECT_NAME} PUBLIC -Wall -Wextra -Wpedantic -O2)
  target_link_libraries(${PROJECT_NAME}
    PRIVATE
    mbed-os
    mbed-events
  )
  mbed_set_post_build(${PROJECT_NAME})

  option(VERBOSE_BUILD "Have a verbose build process")
  if(VERBOSE_BUILD)
      set(CMAKE_VERBOSE_MAKEFILE ON)
  endif()
endif()
########## MBED  BUILDING END   ##########
