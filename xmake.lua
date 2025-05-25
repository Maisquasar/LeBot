add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

set_languages("c++20")

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

add_requires("dpp")

target("LeBot")
    set_kind("binary")
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    
    add_includedirs("src")

    add_packages("dpp")
    
    before_build(function (target)
        
        os.mkdir("tools")
        
        local http = import("net.http")
        
        http.download(
            "https://github.com/yt-dlp/yt-dlp/releases/download/2025.05.22/yt-dlp.exe",
            path.join("tools", "yt-dlp.exe")
        )
    end)
