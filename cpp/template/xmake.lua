set_project("Template")
set_xmakever("3.0.0")

set_languages("c++20")

add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.compile_commands.autoupdate")

set_warnings("allextra")

add_requires("spdlog")

target("template", function () 
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("include")
    add_packages("spdlog")
end)



