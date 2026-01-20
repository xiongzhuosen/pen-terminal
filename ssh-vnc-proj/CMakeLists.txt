cmake_minimum_required(VERSION 3.10)
project(ssh-vnc-full C CXX)

# ======================== 保留用户指定的编译规则 ========================
add_compile_options(-Wall -Werror=return-type -Wno-psabi)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Os)
else()
    add_compile_options(-g -O0)
    add_compile_options(-Wformat -Wformat-security -fstack-protector --param ssp-buffer-size=4)
endif()
# ======================================================================

# 核心变量定义（严格匹配你的命名）
set(LIB_NAME ssh-vnc-full)
set(SDK_LIB_NAME iot-miniapp-sdk-static)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 强制使用ARM交叉编译工具链（保留你的校验逻辑）
if(NOT DEFINED ENV{CROSS_TOOLCHAIN_PREFIX})
    message(FATAL_ERROR "CROSS_TOOLCHAIN_PREFIX environment variable not set!")
endif()
set(TOOLCHAIN_PREFIX $ENV{CROSS_TOOLCHAIN_PREFIX})
set(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc" CACHE STRING "ARM C Compiler" FORCE)
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++" CACHE STRING "ARM CXX Compiler" FORCE)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# 头文件路径：补齐 SDK 头文件 + 匹配 deps 目录结构
set(ARM_DEPS_ROOT ${CMAKE_SOURCE_DIR}/deps)
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/iot-miniapp-sdk/include
    ${ARM_DEPS_ROOT}/usr/include
    ${ARM_DEPS_ROOT}/usr/include/arm-linux-gnueabihf
)

# 库文件路径：仅搜索 deps 里的 ARM 库
link_directories(
    ${ARM_DEPS_ROOT}/usr/lib/arm-linux-gnueabihf
)

# ======================== 新增：编译 SDK 静态库（之前完全缺失） ========================
file(GLOB SDK_SRC ${CMAKE_SOURCE_DIR}/iot-miniapp-sdk/src/*.cpp)
if(NOT SDK_SRC)
    message(WARNING "SDK source files not found in ${CMAKE_SOURCE_DIR}/iot-miniapp-sdk/src")
endif()
add_library(${SDK_LIB_NAME} STATIC ${SDK_SRC})
target_compile_options(${SDK_LIB_NAME} PRIVATE -w)
set_target_properties(${SDK_LIB_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
# ======================================================================================

# 编译主 SO 库：修正源文件路径 + 依赖 SDK 静态库
file(GLOB SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
if(NOT SRC_FILES)
    message(FATAL_ERROR "Main source files not found in ${CMAKE_SOURCE_DIR}/src")
endif()
add_library(${LIB_NAME} SHARED ${SRC_FILES})
add_dependencies(${LIB_NAME} ${SDK_LIB_NAME})

# 链接库：完全替换为你指定的列表（补全 md4c-html/magic/jsoncpp 等刚需库）
target_link_libraries(${LIB_NAME} PRIVATE
    ${SDK_LIB_NAME}
    md4c-html
    ssh2 vncclient vncserver
    z magic
    crypto ssl jsoncpp
    pthread dl m util
    -Wl,-unresolved-symbols=ignore-all
)

# 输出路径：修正为你指定的绝对路径，确保SO库放入前端ui/libs
set_target_properties(${LIB_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/ui/libs
    # 版本号（可选，根据你的需求添加）
    VERSION 1.0.0
    SOVERSION 1
)

# 安装规则（可选：如需安装到系统目录）
install(TARGETS ${LIB_NAME}
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/ui/libs
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/ui/libs
)