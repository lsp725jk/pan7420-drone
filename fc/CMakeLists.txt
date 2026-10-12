# 单独构建飞控（不想经根 CMakeLists 时用）
#
# ⚠ 路径必须走 ASCII 别名，原因见根 CMakeLists.txt 顶部说明：
#   arm-none-eabi-gcc 在 Windows 上处理不了命令行里带中文的 -o / -I / -c 路径。
#
#   cmake -S C:\py32-fc\fc -B C:\py32-fc\build-gcc\build\fc-only \
#         -G Ninja -DCMAKE_TOOLCHAIN_FILE=C:/py32-fc/build-gcc/cmake/arm-none-eabi.cmake
#   cmake --build C:\py32-fc\build-gcc\build\fc-only
#
# 一般不必单独构建，用 build-gcc\build-gcc.ps1 更省事。

cmake_minimum_required(VERSION 3.20)
project(fc LANGUAGES C ASM)

get_filename_component(GCC_DIR "${CMAKE_CURRENT_LIST_DIR}/../build-gcc" ABSOLUTE)
get_filename_component(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

include("${GCC_DIR}/cmake/common.cmake")

py32f0_firmware(
  TARGET_NAME fc
  PROJECT_KEY fc
  PROJ_DIR    "${CMAKE_CURRENT_LIST_DIR}"
  ROOT_DIR    "${ROOT_DIR}"
  GCC_DIR     "${GCC_DIR}"
)
