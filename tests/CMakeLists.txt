cmake_minimum_required(VERSION 3.17)

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

project(asb_test)

set(asb_core "${CMAKE_CURRENT_SOURCE_DIR}/../")
set(submodules "${CMAKE_CURRENT_SOURCE_DIR}/../src/submodules")

set(SOURCES 
  test_fsm.cpp
  test_init.cpp
  test_main.cpp

  ${asb_core}/src/fsm/fsm_task.cpp
  ${asb_core}/src/fsm/fsm.cpp
  ${asb_core}/src/init/init_task.cpp
  ${asb_core}/src/rte/generated/rte.cpp
  ${asb_core}/src/scheduler/scheduler_task.cpp

  ${asb_core}/platform/specific/linux/canbus.cpp
  ${asb_core}/platform/specific/linux/socketcan/socketcan.c
  ${asb_core}/platform/specific/linux/io.cpp
  ${asb_core}/platform/specific/linux/time.cpp
)

set(TASKS
  ${asb_core}/src/
)

add_executable(${PROJECT_NAME} ${SOURCES})

include(FetchContent)
FetchContent_Declare(
  gtest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(gtest)

target_include_directories(${PROJECT_NAME} 
  PRIVATE
  ${TASKS}
  ${asb_core}/include 
  ${asb_core}/platform 
  ${common}
  ${common_virtual_environment_dirs}
  ${submodules}/tinyfsm/include
)

target_link_libraries(${PROJECT_NAME} 
  gmock
  gtest
)

target_compile_options(${PROJECT_NAME} PRIVATE -O0)