cmake_minimum_required(VERSION 3.10)
project(ssh-vnc-full C CXX)

# ======================== 编译规则（保留+新增-fPIC） ========================
add_compile_options(-Wall -Werror=return-type -Wno-psabi -fPIC)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Os)
else()
    add_compile_options(-g -O0)
    add_compile_options(-Wformat -Wformat-security -fstack-protector --param ssp-buffer-size=4)
endif()
# ==========================================================================

# ✅ 强制全局位置无关代码（静态库链接动态库必需）
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ✅ 明确目标名（避免变量歧义）
set(SDK_TARGET "iot-miniapp-sdk-static")
set(MAIN_TARGET "ssh-vnc-full")

# ======================== 交叉编译工具链配置 ========================
if(NOT DEFINED ENV{CROSS_TOOLCHAIN_PREFIX})
    message(FATAL_ERROR "CROSS_TOOLCHAIN_PREFIX environment variable not set!")
endif()
set(TOOLCHAIN $ENV{CROSS_TOOLCHAIN_PREFIX})
set(CMAKE_C_COMPILER "${TOOLCHAIN}gcc" CACHE STRING "ARM C Compiler" FORCE)
set(CMAKE_CXX_COMPILER "${TOOLCHAIN}g++" CACHE STRING "ARM CXX Compiler" FORCE)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
# ==========================================================================

# ======================== 头文件+库文件路径 ========================
set(DEPS_ROOT ${CMAKE_SOURCE_DIR}/deps)
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/iot-miniapp-sdk/include
    ${DEPS_ROOT}/usr/include
    ${DEPS_ROOT}/usr/include/arm-linux-gnueabihf
)
link_directories(
    ${DEPS_ROOT}/usr/lib/arm-linux-gnueabihf
)
# ==========================================================================

# ======================== 编译SDK静态库 ========================
file(GLOB SDK_SRC ${CMAKE_SOURCE_DIR}/iot-miniapp-sdk/src/*.cpp)
if(NOT SDK_SRC)
    message(WARNING "No SDK source files found!")
endif()
add_library(${SDK_TARGET} STATIC ${SDK_SRC})
# ✅ 正确语法：target_compile_options(目标 作用域 选项)
target_compile_options(${SDK_TARGET} PRIVATE -w)
set_target_properties(${SDK_TARGET} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
# ==========================================================================

# ======================== 编译主SO库 ========================
file(GLOB MAIN_SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)
if(NOT MAIN_SRC)
    message(FATAL_ERROR "No main source files found!")
endif()
add_library(${MAIN_TARGET} SHARED ${MAIN_SRC})
# ✅ 正确语法：add_dependencies(主目标 依赖目标)
add_dependencies(${MAIN_TARGET} ${SDK_TARGET})
# ==========================================================================

# ======================== 链接库（关键修正） ========================
# ✅ 正确语法：target_link_libraries(目标 作用域 库列表)
target_link_libraries(${MAIN_TARGET} PRIVATE
    ${SDK_TARGET}
    md4c-html ssh2 vncclient vncserver z magic
    crypto ssl jsoncpp
    pthread dl m util
    -Wl,-unresolved-symbols=ignore-all
)
# ==========================================================================

# ======================== 输出配置 ========================
set_target_properties(${MAIN_TARGET} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/ui/libs
    VERSION 1.0.0
    SOVERSION 1
)
# ==========================================================================
