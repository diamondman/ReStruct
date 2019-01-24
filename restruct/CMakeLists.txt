project(RESTRUCTCLI CXX)

add_executable(RESTRUCTCLI
  main.cpp
  )

target_include_directories(RESTRUCTCLI
  PRIVATE .
  )

target_link_libraries(RESTRUCTCLI
  PRIVATE
    RESTRUCT
  )
