project(RESTRUCT CXX)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(Lua 5.3 REQUIRED)

add_library(RESTRUCT SHARED
  src/a.cpp
  )

target_include_directories(RESTRUCT
  PUBLIC inc
  PRIVATE
    src
    ${LUA_INCLUDE_DIR}
  )

target_link_libraries(RESTRUCT
  PRIVATE
    ${LUA_LIBRARIES}
    nlohmann_json::nlohmann_json
    m
  )

enable_testing()
