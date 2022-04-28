cmake_minimum_required(VERSION 3.16)

add_library(inibakery SHARED)
mo2_configure_plugin(inibakery WARNINGS OFF)
mo2_install_target(inibakery)
