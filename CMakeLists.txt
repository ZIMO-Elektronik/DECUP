cmake_minimum_required(VERSION 3.25 FATAL_ERROR)
include(FetchContent)

if(ESP_PLATFORM)
  file(GLOB_RECURSE SRC src/*.c)
  idf_component_register(SRCS ${SRC} INCLUDE_DIRS include REQUIRES driver)
  # Change OUTPUT_NAME from DECUP to lib__idf_DECUP to avoid conflicts
  set_target_properties(${COMPONENT_LIB} PROPERTIES PREFIX "")
  set_target_properties(
    ${COMPONENT_LIB} PROPERTIES OUTPUT_NAME
                                ${CMAKE_STATIC_LIBRARY_PREFIX}${COMPONENT_LIB})
  target_link_libraries(${COMPONENT_LIB} PUBLIC DECUP)
endif()

FetchContent_Declare(
  CMakeModules
  GIT_REPOSITORY "https://github.com/ZIMO-Elektronik/CMakeModules"
  GIT_TAG v0.9.3
  SOURCE_DIR ${CMAKE_BINARY_DIR}/CMakeModules)
FetchContent_MakeAvailable(CMakeModules)

version_from_git()
project(
  DECUP
  VERSION ${VERSION_FROM_GIT}
  LANGUAGES ASM C CXX)

set(DECUP_MAX_PACKET_SIZE
    261u
    CACHE STRING "Maximum size of a packet in bytes")
set(DECUP_TX_BIT_TIMING
    25u
    CACHE STRING "Duration of a bit of command station")
set(DECUP_TX_ZPP_STOPBIT_TIMING
    50u
    CACHE STRING "Duration of a ZPP stopbit of command station")
set(DECUP_TX_ZSU_STOPBIT_TIMING
    200u
    CACHE STRING "Duration of a ZSU stopbit of command station")

add_library(DECUP INTERFACE)
add_library(DECUP::DECUP ALIAS DECUP)

target_compile_features(DECUP INTERFACE cxx_std_23)

target_compile_definitions(
  DECUP INTERFACE DECUP_MAX_PACKET_SIZE=${DECUP_MAX_PACKET_SIZE}
                  DECUP_TX_BIT_TIMING=${DECUP_TX_BIT_TIMING})

if(PROJECT_IS_TOP_LEVEL)
  target_include_directories(DECUP INTERFACE include)
else()
  target_include_directories(DECUP SYSTEM INTERFACE include)
endif()

target_common_warnings(DECUP INTERFACE)

if(NOT TARGET ZTL::ZTL)
  cpmaddpackage("gh:ZIMO-Elektronik/ZTL@0.19.1")
endif()

target_link_libraries(DECUP INTERFACE ZTL::ZTL)

if(PROJECT_IS_TOP_LEVEL)
  include(CTest)
  add_subdirectory(examples)
  file(
    DOWNLOAD
    "https://github.com/ZIMO-Elektronik/.github/raw/master/data/.clang-format"
    ${CMAKE_CURRENT_LIST_DIR}/.clang-format)
  file(GLOB_RECURSE SRC examples/*.[ch]pp include/*.[ch]pp src/*.[ch]pp
       tests/*.[ch]pp)
  add_clang_format_target(DECUPFormat OPTIONS -i FILES ${SRC})
endif()

if(BUILD_TESTING
   AND PROJECT_IS_TOP_LEVEL
   AND CMAKE_SYSTEM_NAME STREQUAL CMAKE_HOST_SYSTEM_NAME)
  add_subdirectory(tests)
endif()
