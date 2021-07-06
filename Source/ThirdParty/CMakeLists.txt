cmake_minimum_required(VERSION 3.19)

project(alimer_third_party LANGUAGES C CXX)

# spdlog
add_subdirectory(spdlog EXCLUDE_FROM_ALL)
set_property(TARGET spdlog PROPERTY FOLDER "ThirdParty")
