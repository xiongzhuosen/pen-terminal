#!/bin/bash
set -euo pipefail
log_info()  { echo -e "\033[36m[INFO] $1\033[0m"; }
log_success(){ echo -e "\033[32m✅ $1\033[0m"; }
log_error() { echo -e "\033[31m❌ $1\033[0m" && exit 1; }

# 配置
PROJ_ROOT=$(cd $(dirname $0); pwd)
TOOLCHAIN_PREFIX="/root/x-tools/arm-none-linux-gnueabihf/bin/arm-none-linux-gnueabihf-"
BUILD_TYPE=Release
LOG_DIR=${PROJ_ROOT}/build_log
mkdir -p ${LOG_DIR}

# 前置检测
log_info "检测编译前置条件..."
[ ! -d ${PROJ_ROOT}/deps/usr/lib/arm-linux-gnueabihf ] && log_error "deps目录缺失ARM库"
[ ! -f "${TOOLCHAIN_PREFIX}gcc" ] && log_error "交叉工具链不存在"
command -v cmake >/dev/null 2>&1 || log_error "请安装cmake: apt install cmake"

# 配置环境变量
export CROSS_TOOLCHAIN_PREFIX=${TOOLCHAIN_PREFIX}
export PATH=${TOOLCHAIN_PREFIX%/*}:$PATH
export CC=${TOOLCHAIN_PREFIX}gcc
export CXX=${TOOLCHAIN_PREFIX}g++

# 清理缓存
log_info "清理旧编译缓存..."
rm -rf ${PROJ_ROOT}/build && mkdir -p ${PROJ_ROOT}/build && cd ${PROJ_ROOT}/build

# 编译
log_info "开始ARM编译..."
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} .. 2>&1 | tee ${LOG_DIR}/cmake.log
make 2>&1 | tee ${LOG_DIR}/make.log

# 产物校验
SO_FILE=${PROJ_ROOT}/ui/libs/libssh-vnc-full.so
[ ! -f ${SO_FILE} ] && log_error "编译失败，未生成SO库"
chmod 755 ${SO_FILE}

# 架构校验
if command -v file &>/dev/null; then
    FILE_INFO=$(file ${SO_FILE})
    echo -e "\033[32mSO库架构: ${FILE_INFO}\033[0m"
    echo ${FILE_INFO} | grep -qi "arm\|ARM" || log_error "产物不是ARM架构"
fi

log_success "最终版SO库编译成功！路径: ${SO_FILE}"
