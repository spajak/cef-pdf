# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

cmake_minimum_required(VERSION 2.8.12.1)
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0077 NEW)

if(APPLE)
  set(PROJECT_ARCH x86_64)
endif()

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Project name.
project(cef-pdf)

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)

#
# CEF_ROOT setup.
# This variable must be set to locate the binary distribution.
#

set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/cef" CACHE STRING "Directory where CEF release is located")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

#
# Load the CEF configuration.
#

set(USE_SANDBOX OFF)
set(USE_ATL OFF)

# Execute FindCEF.cmake which must exist in CMAKE_MODULE_PATH.
find_package(CEF REQUIRED)

#
# Define CEF-based targets.
#

# Include the libcef_dll_wrapper target.
# Comes from the libcef_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)

# Include application targets.
# Comes from the <target>/CMakeLists.txt file in the current directory.
add_subdirectory(src)

# Display configuration settings.
PRINT_CEF_CONFIG()
