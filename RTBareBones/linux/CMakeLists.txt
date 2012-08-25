cmake_minimum_required(VERSION 2.6)

project(rtbarebones)

include(../../shared/linux/Proton.cmake)

proton_include_components(FocusInputComponent)
proton_include_components(CustomInputComponent ArcadeInputComponent)

set(APP "../source")
proton_set_sources("${APP}/App.cpp")
