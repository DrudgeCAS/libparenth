# Fetch Catch2 v3.11.0
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.11.0
)

FetchContent_MakeAvailable(Catch2)

# The main test driver.
add_executable(testmain
        matrixchain.cpp
        )

target_link_libraries(testmain PRIVATE Catch2::Catch2WithMain)
