cmake_minimum_required(VERSION 3.11)
project(SystemMonitor)

# 1. Download and include FTXUI
include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

add_compile_options(-Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror)

# 2. Define our source files
add_executable(monitor
  src/main.cpp
  src/SystemInfo.cpp
  src/SystemInfo.hpp
  src/MonitorUI.cpp
  src/MonitorUI.hpp
)

# 3. Link dependencies
target_include_directories(monitor PRIVATE src)
target_link_libraries(monitor PRIVATE ftxui::screen ftxui::dom ftxui::component)
