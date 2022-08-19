cmake_minimum_required (VERSION 2.6) 

project(Bollean-Schemes)

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -std=gnu++17 -O3")

add_executable(main main.cpp advanced_bs.cpp base_bs.cpp my_exeption.cpp simple_solver.cpp)
