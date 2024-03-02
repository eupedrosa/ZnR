
set(znr_SRC
    init.cpp
    node.cpp
    this_node.cpp
    publisher.cpp
    subscriber.cpp
)

add_library(znr STATIC ${znr_SRC})

target_include_directories(znr PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(znr PUBLIC cxx_std_20)
target_compile_options (znr PRIVATE -Wall -Wextra)

target_link_libraries(znr zenohcxx)
