
# $ cmake CMakeLists.txt
# $ make

cmake_minimum_required(VERSION 3.16)

project (chess_engine VERSION 0.1 LANGUAGES CXX)



if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_BUILD_TYPE Debug)

  # Create a compile_commands.json file
  set(CMAKE_EXPORT_COMPILE_COMMANDS True)

  set(CMAKE_CXX_STANDARD 20)

  # Let's ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Place the complied executable in the bin directory
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Testing only available if this is the main app
  # Note this needs to be done in the main CMakeLists
  # since it calls enable_testing, which must be in the
  # main CMakeLists.
  include(CTest)

endif()

include(FetchContent)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    add_subdirectory(tests)
endif()


add_subdirectory(src)
add_subdirectory(app)

include_directories(include)

#target_link_libraries(chess_engine PRIVATE chess_engine_lib)

add_test(NAME TestName COMMAND TargetName)



