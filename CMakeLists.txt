cmake_minimum_required(VERSION 3.5)

project(gurobi-template CXX)  # C → CXX 로 변경

set(CMAKE_CXX_STANDARD 17)  # 11 → 17로 최신화

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Visual Studio 컴파일러 설정 (선택적)
if(MSVC AND MT)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")
endif()

find_package(GUROBI REQUIRED)
include_directories(${GUROBI_INCLUDE_DIRS})

# 모든 .cpp 파일을 sources로 등록
set(sources
    main.cpp
    a.cpp
    assign_model.cpp
    genetic.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${sources})

target_link_libraries(${CMAKE_PROJECT_NAME}
    optimized ${GUROBI_CXX_LIBRARY}
    debug     ${GUROBI_CXX_DEBUG_LIBRARY}
    ${GUROBI_LIBRARY}
)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
  include(FeatureSummary)
  feature_summary(WHAT ALL)
endif()
