cmake_minimum_required(VERSION 3.15)

project(footprint
  VERSION
    1.0
  DESCRIPTION
    "footprint"
  LANGUAGES
    CXX
)

include(FetchContent)
FetchContent_Declare(
  async-mqtt5
  GIT_REPOSITORY https://github.com/mireo/async-mqtt5.git
  GIT_TAG d52090f438906976c864698def2413b9d4ee097b
)
FetchContent_MakeAvailable(async-mqtt5)

add_executable(checker src/checker.cpp)
include_directories(${async-mqtt5_SOURCE_DIR}/include)
