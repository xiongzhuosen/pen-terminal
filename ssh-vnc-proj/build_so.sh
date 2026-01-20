#!/bin/bash
# ==============================================
# SSH-VNC 全功能库一键交叉编译脚本
# 依赖：已配置 CROSS_TOOLCHAIN_PREFIX 环境变量
# 功能：自动创建build目录 -> cmake配置 -> make编译 -> 产物检查
# 支持模式：Release(默认) / Debug
# 运行目录：项目根目录（与 src/include/iot-miniapp-sdk 同级）
# ==============================================
set -e

# 颜色定义
RED='\033[31m'
GREEN='\033[32m'
BLUE='\033[34m'
YELLOW='\033[33m'
NC='\033[0m'

# 日志函数
info(){ echo -e "${BLUE}[INFO]${NC} $1"; }
ok(){ echo -e "${GREEN}[OK]${NC} $1"; }
warn(){ echo -e "${YELLOW}[WARN]${NC} $1"; }
err(){ echo -e "${RED}[ERROR]${NC} $1"; exit 1; }

# ===================== 1. 环境与目录检查 =====================
check_env(){
    # 检查交叉编译工具链环境变量
    if [ -z "$CROSS_TOOLCHAIN_PREFIX" ]; then
        err "未设置 CROSS_TOOLCHAIN_PREFIX 环境变量！\n示例: export CROSS_TOOLCHAIN_PREFIX=arm-linux-gnueabihf-"
    fi
    info "交叉编译工具链前缀: $CROSS_TOOLCHAIN_PREFIX"

    # 检查项目核心目录
    local REQUIRED_DIRS=("src" "include" "iot-miniapp-sdk" "deps" "ui")
    for dir in "${REQUIRED_DIRS[@]}"; do
        if [ ! -d "$dir" ]; then
            err "核心目录缺失: $dir，请检查项目结构"
        fi
    done
    ok "项目目录结构检查通过"

    # 检查 SDK 源码是否存在
    if [ -z "$(ls iot-miniapp-sdk/src/*.cpp 2>/dev/null)" ]; then
        warn "iot-miniapp-sdk/src 下无 cpp 文件，可能导致编译失败"
    fi

    # 检查主项目源码是否存在
    if [ -z "$(ls src/*.cpp 2>/dev/null)" ]; then
        err "src 目录下无 cpp 文件，无法编译"
    fi
}

# ===================== 2. 解析编译模式 =====================
parse_mode(){
    if [ "$1" = "debug" ]; then
        BUILD_TYPE="Debug"
        info "编译模式: Debug（带调试信息）"
    else
        BUILD_TYPE="Release"
        info "编译模式: Release（优化编译）"
    fi
}

# ===================== 3. 执行编译 =====================
run_compile(){
    local BUILD_DIR="build"
    # 创建并进入build目录
    if [ -d "$BUILD_DIR" ]; then
        info "清理旧build目录..."
        rm -rf "$BUILD_DIR"
    fi
    mkdir -p "$BUILD_DIR" && cd "$BUILD_DIR"

    # 执行cmake配置
    info "执行 CMake 配置..."
    cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..
    if [ $? -ne 0 ]; then
        err "CMake 配置失败，请检查CMakeLists.txt或依赖库"
    fi

    # 执行make编译
    info "开始编译（使用多核加速）..."
    make -j$(nproc)
    if [ $? -ne 0 ]; then
        err "Make 编译失败，请检查源码或依赖"
    fi

    # 返回项目根目录
    cd ..
}

# ===================== 4. 产物检查 =====================
check_output(){
    local SO_PATH="ui/libs/libssh-vnc-full.so"
    if [ -f "$SO_PATH" ]; then
        ok "编译成功！产物路径: $SO_PATH"
        info "SO库大小: $(du -h $SO_PATH | awk '{print $1}')"
    else
        err "编译产物缺失！请检查编译日志"
    fi
}

# ===================== 主流程 =====================
main(){
    info "============= SSH-VNC 全功能库编译开始 ============="
    parse_mode "$1"
    check_env
    run_compile
    check_output
    echo -e "\n${GREEN}✅✅✅ 编译流程全部完成！✅✅✅${NC}"
    info "下一步: 将 ui/libs 目录复制到ARM设备即可运行"
}

# 启动脚本
main "$1"
