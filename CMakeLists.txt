cmake_minimum_required(VERSION 3.14)
project ("Xonix"
    VERSION 1.0.0
    DESCRIPTION "Remake for Xonix game"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)
# include_directories(${SFML_INCLUDE_DIR})
# message(STATUS "SFML INCLUDE DIR: " ${SFML_INCLUDE_DIR})

add_library (Game game.cpp)
target_include_directories (Game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "/usr/local/Cellar/sfml/2.5.1_1/include")

add_executable (main main.cpp)
target_include_directories (main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(main ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
target_link_libraries (main LINK_PUBLIC Game)

install(TARGETS main DESTINATION bin)