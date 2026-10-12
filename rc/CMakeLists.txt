# 单独构建遥控器（不想经根 CMakeLists 时用）
#
# ⚠ 路径必须走 ASCII 别名，原因见根 CMakeLists.txt 顶部说明。
#   一般直接用 build-gcc\build-gcc.ps1 即可。

cmake_minimum_required(VERSION 3.20)
project(rc LANGUAGES C ASM)

get_filename_component(GCC_DIR "${CMAKE_CURRENT_LIST_DIR}/../build-gcc" ABSOLUTE)
get_filename_component(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

include("${GCC_DIR}/cmake/common.cmake")

py32f0_firmware(
  TARGET_NAME rc
  PROJECT_KEY rc
  PROJ_DIR    "${CMAKE_CURRENT_LIST_DIR}"
  ROOT_DIR    "${ROOT_DIR}"
  GCC_DIR     "${GCC_DIR}"
)
