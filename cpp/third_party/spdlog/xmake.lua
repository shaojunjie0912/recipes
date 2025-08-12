set_project("TestSpdlog")

set_languages("c++20")
set_warnings("allextra")

add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.compile_commands.autoupdate")

add_requires("spdlog")

target("test1")
    set_kind("binary")
    add_files("src/test1.cpp")
    add_packages("spdlog")

target("test2")
    set_kind("binary")
    add_files("src/test2.cpp")
    add_packages("spdlog")

target("test3")
    set_kind("binary")
    add_files("src/test3.cpp")
    add_packages("spdlog")
    set_rundir("$(projectdir)")
