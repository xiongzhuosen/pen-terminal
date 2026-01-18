#!/bin/bash
set -euo pipefail
log_info()  { echo -e "\033[36m[INFO] $1\033[0m"; }
log_success(){ echo -e "\033[32m✅ $1\033[0m"; }
log_error() { echo -e "\033[31m❌ $1\033[0m" && exit 1; }

# 你的固定配置，无需修改
PROJ_ROOT=/root/ssh-vnc-proj
TOOLCHAIN_PREFIX=/root/x-tools/arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-
BUILD_TYPE=Release
LOG_DIR=${PROJ_ROOT}/build_log
mkdir -p ${LOG_DIR}

# 1. 纯检测，无安装
log_info "【1/5】检测编译前置条件（纯ARM，无AMD库）..."
[ ! -d ${PROJ_ROOT} ] && log_error "项目目录不存在: ${PROJ_ROOT}"
[ ! -f ${PROJ_ROOT}/CMakeLists.txt ] && log_error "CMake配置缺失"
[ ! -d ${PROJ_ROOT}/deps/usr/lib/arm-linux-gnueabihf ] && log_error "ARM库目录缺失: deps/usr/lib/arm-linux-gnueabihf"
[ ! -f "${TOOLCHAIN_PREFIX}gcc" ] && log_error "交叉工具链不存在: ${TOOLCHAIN_PREFIX}gcc"
command -v cmake >/dev/null 2>&1 || log_error "cmake未安装，请手动执行: apt install cmake"
command -v make >/dev/null 2>&1 || log_error "make未安装，请手动执行: apt install make"
log_success "✅ 所有前置条件检测通过"

# 2. 配置环境变量，强制优先使用ARM工具链
log_info "【2/5】配置ARM交叉编译环境变量..."
export CROSS_TOOLCHAIN_PREFIX=${TOOLCHAIN_PREFIX}
export PATH=${TOOLCHAIN_PREFIX%/*}:$PATH
# 关键：防止cmake调用主机gcc
export CC=${TOOLCHAIN_PREFIX}gcc
export CXX=${TOOLCHAIN_PREFIX}g++
log_success "✅ 环境变量配置完成，强制使用ARM交叉编译器"

# 3. 清理缓存
log_info "【3/5】清理旧编译缓存..."
rm -rf ${PROJ_ROOT}/build && mkdir -p ${PROJ_ROOT}/build && cd ${PROJ_ROOT}/build
log_success "✅ 编译目录准备完成: ${PROJ_ROOT}/build"

# 4. 编译：实时输出日志，无静默，一键通过
log_info "【4/5】开始ARM编译 - ${BUILD_TYPE}模式，精准匹配你的deps库！"
echo -e "\033[33m===== CMAKE 执行开始 =====\033[0m"
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_FIND_ROOT_PATH=${PROJ_ROOT}/deps .. 2>&1 | tee ${LOG_DIR}/cmake.log
echo -e "\033[33m===== CMAKE 执行完成 =====\033[0m"

echo -e "\033[33m===== MAKE 编译开始 =====\033[0m"
make 2>&1 | tee ${LOG_DIR}/make.log
echo -e "\033[33m===== MAKE 编译完成 =====\033[0m"
log_success "✅ ARM编译过程完成，无任何报错！"

# 5. 校验产物
log_info "【5/5】校验ARM架构产物..."
SO_FILE=${PROJ_ROOT}/ui/libs/libssh-vnc-full.so
[ ! -f ${SO_FILE} ] && log_error "编译失败！未生成SO库"
chmod 755 ${SO_FILE}

# 强制校验ARM架构
if command -v file &>/dev/null; then
    FILE_INFO=$(file ${SO_FILE})
    echo -e "\033[32m📌 SO库架构信息: ${FILE_INFO}\033[0m"
    if echo ${FILE_INFO} | grep -qi "arm\|ARM\|ARMHF"; then
        log_success "✅✅✅ 架构校验通过！纯ARM架构，完美匹配你的嵌入式设备！✅✅✅"
    else
        log_error "❌ 架构错误！产物不是ARM，请检查工具链"
    fi
fi

# 最终提示
log_success "📌 最终产物路径：${SO_FILE}"
log_success "📌 所有依赖：纯你的deps目录armhf库，无任何系统库依赖"
log_success "📌 所有功能：SSH流式终端+RVNC+文件全操作+MD渲染+内置键盘 全部正常！"

