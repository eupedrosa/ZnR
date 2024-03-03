
add_executable(talker EXCLUDE_FROM_ALL talker.cpp)
target_link_libraries(talker znr)

add_executable(listener EXCLUDE_FROM_ALL listener.cpp)
target_link_libraries(listener znr)

add_executable(resources EXCLUDE_FROM_ALL resources.cpp)
target_link_libraries(resources znr)

add_custom_target(examples)
add_dependencies(examples talker listener resources)
