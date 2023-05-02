cmake_minimum_required(VERSION 3.26)
project(game-of-life)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-Wall -Wextra -Wpedantic)

if (UNIX)
  set(LINK_LIBRARIES "GL" "X11" "glfw3" "m")
  set(BIN_PATH "${CMAKE_BINARY_DIR}/game-of-life")
endif (UNIX)


if (WIN32 AND MINGW)
  set(LINK_LIBRARIES "opengl32" "user32" "glfw3")
  set(BIN_PATH "${CMAKE_BINARY_DIR}/game-of-life.exe")
endif (WIN32 AND MINGW)


add_custom_target(run
  COMMAND ./game-of-life
  DEPENDS ${BIN_PATH}
  COMMENT "running game-of-life"
  )
FILE(GLOB MyCSources ./src/*.c)
add_executable(${PROJECT_NAME} ${MyCSources})
target_link_libraries(${PROJECT_NAME} ${LINK_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ./include/)
