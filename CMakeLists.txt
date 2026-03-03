cmake_minimum_required(VERSION 3.20)
project(stackgame3_pr1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(stackgame3_pr1
  src/main.cpp
  src/core/GameApp.cpp
  src/core/InputRouter.cpp
  src/core/SceneManager.cpp
  src/core/RenderLayout.cpp
)

target_include_directories(stackgame3_pr1 PRIVATE src)
