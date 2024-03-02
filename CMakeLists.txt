cmake_minimum_required(VERSION 3.10.2)
project(
    znr
    VERSION 0.0.1
    DESCRIPTION "ZnR not ROS"
)

add_subdirectory(vendor)
add_subdirectory(znr)
add_subdirectory(examples)
