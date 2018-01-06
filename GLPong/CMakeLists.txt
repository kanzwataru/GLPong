cmake_minimum_required(VERSION 3.0)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
project(GLPong)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(OPENGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

include_directories(src/
                    src/game/
                    src/renderer/
                    src/tests/
                    libraries/)

file(GLOB PROJECT_SOURCES src/*.c
                          src/game/*.c
                          src/renderer/*.c
                          src/tests/*.c)

file(GLOB PROJECT_HEADERS src/*.h
                          src/game/*.h
                          src/renderer/*.h
                          src/tests/*.h)

file(GLOB VENDORS_SOURCES libraries/*.c)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${VENDORS_SOURCES})

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} 
                                      ${OPENGL_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Package)

set(SHADER_SRC "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_DEST "${CMAKE_CURRENT_BINARY_DIR}/Package/shaders")
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink ${SHADER_SRC} ${SHADER_DEST}
  DEPENDS ${SHADER_DEST}
  COMMENT "Symlink ${SHADER_SRC} => ${SHADER_DEST}"
)