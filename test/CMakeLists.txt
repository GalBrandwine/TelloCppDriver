set(BINARY ${CMAKE_PROJECT_NAME}_test)


set(TEST_SOURCES
    TelloDriverTest.cpp
  ${CMAKE_SOURCE_DIR}/src/packet_parser/test/packet_test.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/test/flight_data_test.cpp

  ${TELLO_TELEMETRY_SOURES}
)

set(SOURCES ${TEST_SOURCES})

add_executable(${BINARY} ${TEST_SOURCES})

# Not neccesery
add_test(NAME ${BINARY} COMMAND ${BINARY})

# Link againts our gtest submodule, not the one installed in '/usr/local'
# Gtest require pthread, so we need to link against this one also.
target_link_libraries (${BINARY} PUBLIC ${CMAKE_PROJECT_NAME} ${PROJECT_BINARY_DIR}/lib/libgtestd.a pthread)

target_include_directories(${BINARY} PRIVATE
  ${CMAKE_SOURCE_DIR}/src/packet_parser
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry
)


## "wet" tests require connection to the drone
set(WET_TEST_SOURCES
    TelloDriverTest_wet.cpp
    ${CMAKE_SOURCE_DIR}/src/tello_telemetry/test/commands_test.cpp
  ${TELLO_TELEMETRY_SOURES}
)

add_executable(${BINARY}_wet ${WET_TEST_SOURCES})
target_link_libraries (${BINARY}_wet PUBLIC ${CMAKE_PROJECT_NAME} ${PROJECT_BINARY_DIR}/lib/libgtestd.a pthread)

add_executable(MovementCommandsManagerTest
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/movement_commands/test/movement_commands_manager_test.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/movement_commands/test/MovementCommandsManagerTest.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/movement_commands/MovementCommandsManager.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/movement_commands/MovementsToPacketConverter.cpp
 )
 target_include_directories(MovementCommandsManagerTest PUBLIC
 ${CMAKE_SOURCE_DIR}/lib/spdlog/include
 ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/movement_commands/test/
 ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/movement_commands/
 ${CMAKE_SOURCE_DIR}src/packet_parser/
 )
target_link_libraries (MovementCommandsManagerTest PUBLIC ${CMAKE_PROJECT_NAME} ${PROJECT_BINARY_DIR}/lib/libgtestd.a pthread)


################################ TelloVisTest #################################
find_library(AVUTIL DOC "Using AV util library to parse packages from the drone" avutil)
find_library(AVCODEC DOC "Using AV codec library to registero our decoder" avcodec)
find_library(SWSCALE DOC "Using AV codec library to registero our decoder" swscale)

# set( OpenCV_DIR "/usr/share/OpenCV" )
# set( OpenCV_DIR "/usr/share/OpenCV" )
# set( PKG_CONFIG_PATH "/usr/lib/pkgconfig" )
find_package(OpenCV REQUIRED)

add_executable(TelloVisTest
  ${CMAKE_SOURCE_DIR}/test/TelloVisSocketTest.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_vis/test/basic_vis_tests.cpp
  ${CMAKE_SOURCE_DIR}/src/tello_vis/utils/TelloVisSocket.cpp

  ${CMAKE_SOURCE_DIR}/src/tello_vis/lib/h264decoder/h264decoder.cpp
)

target_include_directories(TelloVisTest PUBLIC
  ${CMAKE_SOURCE_DIR}/lib/spdlog/include
  ${CMAKE_SOURCE_DIR}/src/tello_vis/utils
  ${CMAKE_SOURCE_DIR}/src/tello_vis/lib

  ${OpenCV_INCLUDE_DIRS}
#  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/movement_commands/test/
#  ${CMAKE_SOURCE_DIR}/src/tello_telemetry/utils/movement_commands/
#  ${CMAKE_SOURCE_DIR}src/packet_parser/
 )
target_link_libraries (TelloVisTest PUBLIC 
  ${CMAKE_PROJECT_NAME} 
  ${AVUTIL}
  ${AVCODEC}
  ${SWSCALE}
  
  ${OpenCV_LIBS}
  ${PROJECT_BINARY_DIR}/lib/libgtestd.a 
  pthread
)



