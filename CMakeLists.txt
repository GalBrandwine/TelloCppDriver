cmake_minimum_required(VERSION 3.14)
project(TelloDriver VERSION "0.0.26")

set(CMAKE_CXX_STANDARD 14)

set(OBSERVERS
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/OutGoingCommandsObservers/TelloAckLogHeaderIdSenderObserver.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/OutGoingCommandsObservers/TelloStickCommandsObserver.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/IncommingDataObservers/TelloLogHeaderMsgObserver.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/IncommingDataObservers/TelloLogDataMsgObserver.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/IncommingDataObservers/TelloConnAckMsgObserver.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/IncommingDataObservers/TelloWifiMsgObserver.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/IncommingDataObservers/TelloAltLimitMsgObserver.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/IncommingDataObservers/TelloAttLimitMsgObserver.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/IncommingDataObservers/TelloFlightDataMsgObserver.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/IncommingDataObservers/TelloLowBatThreshMsgObserver.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/IncommingDataObservers/TelloPowerOnTimerMsgObserver.cpp
)

set(TELLO_TELEMETRY_SOURES
  ${CMAKE_SOURCE_DIR}/src/packet_parser/protocol.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/flight_data/FlightData.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/log_data/LogData.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/log_data/LogNewMvoFeedback.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/log_data/LogNewImuAttiFeedback.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/TelloTelemetry.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/TelloCommander.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/TelloSocket.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/movement_commands/MovementCommandsManager.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/movement_commands/MovementsToPacketConverter.cpp

  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/data_manager/DataManager.cpp
  ${OBSERVERS}
)

include_directories(
  include
)

add_library(TelloDriver SHARED
  src/TelloDriver.cpp
  ${TELLO_TELEMETRY_SOURES}
)

target_include_directories(TelloDriver PUBLIC
  ${CMAKE_SOURCE_DIR}/src/packet_parser
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/IncommingDataObservers
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/OutGoingCommandsObservers
)

# Very cool way for Fetching gtest
include(FetchContent)
message("Fetching content: googletest")
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG origin/main
)

message("Fetching content: spdlog")
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1
)

FetchContent_MakeAvailable(spdlog googletest)

include(GoogleTest)
target_include_directories(TelloDriver PUBLIC ${spdlog_SOURCE_DIR}/include)

execute_process(
  COMMAND ${CMAKE_COMMAND} -E tar xzf ${PROJECT_SOURCE_DIR}/lib/asio-1.18.0.tar.bz2
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/
)

target_include_directories(TelloDriver PUBLIC IMPORTED ${PROJECT_SOURCE_DIR}/lib/asio-1.18.0/include)

# asio requires pthread
target_link_libraries(TelloDriver pthread)

# Add Cmake current version as header in the project.
configure_file(${PROJECT_SOURCE_DIR}/conf/TelloDriverConfig.hpp.in TelloDriverConfig.hpp)
target_include_directories(TelloDriver PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

include(CTest)
enable_testing()

add_subdirectory(examples)
# add_subdirectory(test)
