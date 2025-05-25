add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")

set_languages("c++20")

if is_plat("windows") then
    set_runtimes(is_mode("debug") and "MDd" or "MD")
end

add_requires("dpp")

add_requires("libogg")
add_requires("libopus")
add_requires("vcpkg::glfw3")

set_rundir(".")

target("LeBot")
    set_kind("binary")
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    
    add_includedirs("src")
    
    add_packages("dpp")
    add_packages("libogg")
    add_packages("libopus")
    add_packages("vcpkg::glfw3")
    
    before_build(function (target)
        os.mkdir("tools")

        local http    = import("net.http")
        local ffurl   = "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl-shared.zip"
        local ffzip   = path.join("tools", "ffmpeg.zip")
        local ffdir   = path.join("tools", "ffmpeg")
        local ytdlp   = path.join("tools", "yt-dlp.exe")

        -- only download yt-dlp if it’s missing
        if not os.isfile(ytdlp) then
            print("Downloading yt-dlp...")
            http.download(
                "https://github.com/yt-dlp/yt-dlp/releases/download/2025.05.22/yt-dlp.exe",
                ytdlp
            )
        end

        -- only fetch & unpack FFmpeg if the ffmpeg folder is missing
        if not os.isdir(ffdir) then
            print("Downloading ffmpeg...")
            -- download ffmpeg zip
            http.download(ffurl, ffzip)

            -- unzip
            if is_plat("windows") then
                os.exec(string.format(
                    [[powershell -noprofile -command "Expand-Archive -Path '%s' -DestinationPath '%s' -Force"]]
                  , ffzip, "tools"
                ))
            else
                os.execf("unzip -o %s -d %s", ffzip, "tools")
            end

            -- rename extracted folder to `tools/ffmpeg`
            os.mv(path.join("tools", "ffmpeg-master-latest-win64-gpl-shared"), ffdir)
            -- cleanup
            os.rm(ffzip)
        end
    end)
target_end()
