# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.22.1)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(greentea-client-populate NONE)


# Pass through things we've already detected in the main project to avoid
# paying the cost of redetecting them again in ExternalProject_Add()
set(GIT_EXECUTABLE [==[/usr/bin/git]==])
set(GIT_VERSION_STRING [==[2.34.1]==])
set_property(GLOBAL PROPERTY _CMAKE_FindGit_GIT_EXECUTABLE_VERSION
  [==[/usr/bin/git;2.34.1]==]
)


include(ExternalProject)
ExternalProject_Add(greentea-client-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/ARMmbed/greentea-client.git" "GIT_TAG" "472aad2327fbfde827852fc44775904706847a3a"
                    SOURCE_DIR          "/workspaces/asb/cmake_build/NUCLEO_F446RE/develop/GCC_ARM/_deps/greentea-client-src"
                    BINARY_DIR          "/workspaces/asb/cmake_build/NUCLEO_F446RE/develop/GCC_ARM/_deps/greentea-client-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)


