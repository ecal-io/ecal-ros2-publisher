cmake_minimum_required(VERSION 3.5)
project(pub_navsatfix_ecal)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(/W4)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

  # CMake >= 3.15 will erroneously define PROTOBUF_USE_DLLS otherwise
  set (Protobuf_USE_STATIC_LIBS TRUE) 
endif()

find_package(eCAL REQUIRED)
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

create_targets_protobuf()

set(source_files
  src/main.cpp
)

set(proto_files
  ${CMAKE_CURRENT_SOURCE_DIR}/pb/builtin_interfaces/time.proto
  ${CMAKE_CURRENT_SOURCE_DIR}/pb/std_msgs/header.proto
  ${CMAKE_CURRENT_SOURCE_DIR}/pb/sensor_msgs/navsatstatus.proto
  ${CMAKE_CURRENT_SOURCE_DIR}/pb/sensor_msgs/navsatfix.proto
)

add_executable(${PROJECT_NAME} ${source_files} ${PROTO_SRCS} ${PROTO_HDRS})

PROTOBUF_TARGET_CPP(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/pb ${proto_files})

target_link_libraries(${PROJECT_NAME} 
  eCAL::core 
)

install(TARGETS
  ${PROJECT_NAME}
  DESTINATION lib/${PROJECT_NAME}
)
