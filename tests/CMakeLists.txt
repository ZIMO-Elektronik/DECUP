include(GoogleTest)

file(GLOB_RECURSE SRC *.cpp)
add_executable(DECUPTests ${SRC})

sanitize(address,undefined)

target_common_warnings(DECUPTests PRIVATE)

cpmaddpackage(
  NAME
  googletest
  GITHUB_REPOSITORY
  "google/googletest"
  GIT_TAG
  main
  SYSTEM
  YES
  OPTIONS
  "INSTALL_GTEST OFF")

target_link_libraries(DECUPTests PRIVATE DECUP::DECUP GTest::gtest_main
                                         GTest::gmock)

gtest_discover_tests(DECUPTests)
