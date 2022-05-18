include_directories(include)
message(STATUS "Compiling examples: ${CMAKE_PROJECT_NAME}")

add_executable(simple_connection
  simple_connection.cpp
)
add_dependencies(simple_connection ${CMAKE_PROJECT_NAME})
target_link_libraries(simple_connection PUBLIC  ${CMAKE_PROJECT_NAME})


add_executable(simple_takeoff
simple_takeoff.cpp
)
add_dependencies(simple_takeoff ${CMAKE_PROJECT_NAME})
target_link_libraries(simple_takeoff PUBLIC  ${CMAKE_PROJECT_NAME})


add_executable(simple_land
simple_land.cpp
)
add_dependencies(simple_land ${CMAKE_PROJECT_NAME})
target_link_libraries(simple_land PUBLIC  ${CMAKE_PROJECT_NAME})

add_executable(manual_takeoff
manual_takeoff.cpp
)
add_dependencies(manual_takeoff ${CMAKE_PROJECT_NAME})
target_link_libraries(manual_takeoff PUBLIC  ${CMAKE_PROJECT_NAME})

add_executable(manual_manouvers
manual_manouvers.cpp
)
add_dependencies(manual_manouvers ${CMAKE_PROJECT_NAME})
target_link_libraries(manual_manouvers PUBLIC  ${CMAKE_PROJECT_NAME})
