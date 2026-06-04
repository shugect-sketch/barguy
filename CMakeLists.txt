cmake_minimum_required(VERSION 3.21)

set(CMAKE_TOOLCHAIN_FILE /usr/local/psn00bsdk/lib/libpsn00b/cmake/sdk.cmake)

project(barguy C)

include(/usr/local/psn00bsdk/lib/libpsn00b/cmake/libpsn00b.cmake)

add_executable(barguy
    src/main.c
    src/game.c
    src/menu.c
    src/save.c
    src/achievements.c
    src/graphics.c
    src/sprites.c
)

target_link_options(barguy PRIVATE
    -T/usr/local/psn00bsdk/lib/libpsn00b/ldscripts/exe.ld
)
target_include_directories(barguy PRIVATE
    /usr/local/psn00bsdk/include
    src/
)
target_link_options(barguy PRIVATE
    -T/usr/local/psn00bsdk/lib/libpsn00b/ldscripts/exe.ld
)
target_link_libraries(barguy PRIVATE
    /usr/local/psn00bsdk/lib/libpsn00b/release/libpsxgpu_exe_gprel.a
    /usr/local/psn00bsdk/lib/libpsn00b/release/libpsxetc_exe_gprel.a
    /usr/local/psn00bsdk/lib/libpsn00b/release/libpsxapi_exe_gprel.a
    /usr/local/psn00bsdk/lib/libpsn00b/release/libc_exe_gprel.a
)
