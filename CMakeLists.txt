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

add_executable(check_async-mqtt5 src/check_async-mqtt5.cpp)
include_directories(${async-mqtt5_SOURCE_DIR}/include)


include(FetchContent)
FetchContent_Declare(
  async_mqtt
  GIT_REPOSITORY https://github.com/redboltz/async_mqtt.git
  GIT_TAG 7dfa232becd4ead69ab5ff00bf63a24c66c483be
)
FetchContent_MakeAvailable(async_mqtt)

add_executable(check_async_mqtt src/check_async_mqtt.cpp)
include_directories(${async_mqtt_SOURCE_DIR}/include)

add_library(async_mqtt src/async_mqtt.cpp)
target_compile_definitions(async_mqtt PRIVATE ASYNC_MQTT_SEPARATE_COMPILATION)

add_executable(check_async_mqtt_separate src/check_async_mqtt_separate.cpp)
include_directories(${async_mqtt_SOURCE_DIR}/include)
target_compile_definitions(check_async_mqtt_separate PRIVATE ASYNC_MQTT_SEPARATE_COMPILATION)
target_link_libraries(check_async_mqtt_separate PRIVATE async_mqtt)
