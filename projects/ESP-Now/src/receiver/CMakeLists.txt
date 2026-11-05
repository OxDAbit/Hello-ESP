# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.22)

# Configuración común de la placa ESP32-S3-N16R8 (src/sdkconfig.n16r8)
set(SDKCONFIG_DEFAULTS "${CMAKE_CURRENT_LIST_DIR}/../sdkconfig.n16r8")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(receiver)
