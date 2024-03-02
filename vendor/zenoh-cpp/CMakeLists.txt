
find_package(zenohc REQUIRED)

add_library(zenohcxx INTERFACE)

target_include_directories(zenohcxx INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(zenohcxx INTERFACE zenohc::lib)
