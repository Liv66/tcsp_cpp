cmake_minimum_required(VERSION 3.5)
project(TCSP_CPP)

option(CXX "enable C++ compilation" ON)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard is ${CMAKE_CXX_STANDARD}")

# Visual Studio에서 정적 라이브러리 사용
if(MSVC AND MT)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
endif()

# Gurobi 설정
find_package(GUROBI REQUIRED)
include_directories(${GUROBI_INCLUDE_DIRS})

# matplotlib-cpp + Python/Numpy 설정
include_directories(
  ${PROJECT_SOURCE_DIR}/external
  "C:/Users/User/AppData/Local/Programs/Python/Python312/include"
  "C:/Users/User/AppData/Local/Programs/Python/Python312/Lib/site-packages/numpy/core/include"
)
link_directories(
  "C:/Users/User/AppData/Local/Programs/Python/Python312/libs"
)

# 소스 목록
set(sources
  main.cpp
  a.cpp
  assign_model.cpp
  genetic.cpp
)

add_executable(${PROJECT_NAME} ${sources})

# 모든 라이브러리 통합 링크
target_link_libraries(${PROJECT_NAME}
  optimized ${GUROBI_CXX_LIBRARY}
  debug ${GUROBI_CXX_DEBUG_LIBRARY}
  ${GUROBI_LIBRARY}
  python312
)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  include(FeatureSummary)
  feature_summary(WHAT ALL)
endif()
