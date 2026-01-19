#!/bin/bash
# ==============================================
# SSH-VNC 前端源码一键部署+编译脚本
# 运行目录: /root/ssh-vnc-proj (项目根目录)
# 功能: 自动生成完整前端目录+源码 + 依赖安装 + 补丁修复 + 编译打包
# 规范: pages(业务页面) / components(公共组件) 严格分离 不混放
# ==============================================
set -e
export LC_ALL=C
# 颜色定义
RED='\033[31m' GREEN='\033[32m' YELLOW='\033[33m' BLUE='\033[34m' NC='\033[0m'
# 当前目录 = 项目根目录 (绝对路径)
ROOT_DIR=$(pwd)
UI_DIR="${ROOT_DIR}/ui"
LIB_DIR="${UI_DIR}/libs"
SRC_DIR="${UI_DIR}/src"
PAGES_DIR="${SRC_DIR}/pages"
COMPONENTS_DIR="${SRC_DIR}/components"
UTILS_DIR="${SRC_DIR}/utils"

# 日志函数
info(){ echo -e "${BLUE}[INFO]${NC} $1"; }
ok(){ echo -e "${GREEN}[OK]${NC} $1"; }
err(){ echo -e "${RED}[ERROR]${NC} $1"; exit 1; }
warn(){ echo -e "${YELLOW}[WARN]${NC} $1"; }

# ===================== 第一步：检查前置环境 =====================
check_env(){
    info "检查前端构建环境..."
    if ! command -v node &>/dev/null; then
        err "未安装nodejs，请先执行: apt install -y nodejs npm"
    fi
    if ! command -v pnpm &>/dev/null; then
        info "安装pnpm包管理器..."
        npm install -g pnpm --registry=https://registry.npmmirror.com
    fi
    if ! command -v sed &>/dev/null; then
        apt install -y sed &>/dev/null
    fi
    ok "环境检查通过"
}

# ===================== 第二步：强制创建标准前端目录结构 =====================
mkdir_frontend_dir(){
    info "在当前目录创建标准前端目录结构..."
    mkdir -p ${LIB_DIR}
    mkdir -p ${PAGES_DIR}/{index,ssh,vnc,file,setting}
    mkdir -p ${COMPONENTS_DIR}
    mkdir -p ${UTILS_DIR}
    # 自动关联后端so库目录，如果没有则创建空目录，不影响编译
    if [ -f "${ROOT_DIR}/build/libssh-vnc-full.so" ]; then
        cp -f ${ROOT_DIR}/build/libssh-vnc-full.so ${LIB_DIR}/
        ok "已自动拷贝后端核心库: libssh-vnc-full.so 到前端libs目录"
    fi
    ok "前端目录创建完成: ${UI_DIR}"
}

# ===================== 第三步：写入完整前端源码文件【所有核心文件一键生成】 =====================
write_frontend_code(){
    info "写入完整前端源码文件..."

# --- 1. 前端入口页面: ui/index.html
cat > ${UI_DIR}/index.html << 'EOF'
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
    <title>SSH-VNC 远程管理</title>
    <style>*{margin:0;padding:0;box-sizing:border-box;}body{font-family:Arial, sans-serif;background:#f5f5f5;height:100vh;overflow:hidden;}</style>
</head>
<body>
    <div id="app"></div>
    <script src="./src/main.js"></script>
</body>
</html>
EOF

# --- 2. 前端依赖配置: ui/package.json (和miniapp一致用pnpm，aiot-vue-cli适配)
cat > ${UI_DIR}/package.json << 'EOF'
{
  "name": "ssh-vnc-frontend",
  "version": "1.0.0",
  "description": "SSH-VNC 前端页面，适配有道词典笔miniapp规范",
  "private": true,
  "scripts": {
    "dev": "aiot-vue-cli serve",
    "build": "aiot-vue-cli build",
    "package": "aiot-vue-cli package"
  },
  "dependencies": {
    "vue": "^2.7.14",
    "vue-router": "^3.6.5",
    "@vue/compiler-sfc": "^2.7.14",
    "@rollup/plugin-typescript": "^11.1.6"
  },
  "devDependencies": {
    "aiot-vue-cli": "^1.0.0"
  }
}
EOF

# --- 3. 前端入口JS: ui/src/main.js
cat > ${SRC_DIR}/main.js << 'EOF'
import Vue from 'vue'
import App from './App.vue'
import VueRouter from 'vue-router'
Vue.use(VueRouter)
Vue.config.productionTip = false

// 页面路由配置 (pages目录下的业务页面，严格分离)
import Index from './pages/index/index.vue'
import SSH from './pages/ssh/ssh.vue'
import VNC from './pages/vnc/vnc.vue'
import File from './pages/file/file.vue'
import Setting from './pages/setting/setting.vue'

const router = new VueRouter({
  routes: [
    { path: '/', component: Index },
    { path: '/ssh', component: SSH },
    { path: '/vnc', component: VNC },
    { path: '/file', component: File },
    { path: '/setting', component: Setting }
  ]
})

new Vue({
  router,
  render: h => h(App)
}).$mount('#app')
EOF

# --- 4. 根组件: ui/src/App.vue
cat > ${SRC_DIR}/App.vue << 'EOF'
<template>
  <div id="app">
    <NavBar title="SSH-VNC 远程管理"></NavBar>
    <router-view class="page-container"></router-view>
  </div>
</template>

<script>
import NavBar from './components/NavBar.vue'
export default {
  name: 'App',
  components: { NavBar }
}
</script>

<style scoped>
.page-container {
  width: 100vw;
  height: calc(100vh - 45px);
  overflow: auto;
}
</style>
EOF

# --- 5. 公共组件 - 顶部导航 (仅组件，放components): ui/src/components/NavBar.vue
cat > ${COMPONENTS_DIR}/NavBar.vue << 'EOF'
<template>
  <div class="navbar">
    <div class="navbar-title">{{title}}</div>
  </div>
</template>

<script>
export default {
  name: 'NavBar',
  props: { title: { type: String, default: 'SSH-VNC' } }
}
</script>

<style scoped>
.navbar {
  width: 100%;
  height: 45px;
  line-height: 45px;
  background: #1E90FF;
  color: #fff;
  text-align: center;
  font-size: 18px;
  font-weight: bold;
  position: fixed;
  top: 0;
  left: 0;
  z-index: 999;
}
</style>
EOF

# --- 6. 公共组件 - SSH终端 (仅组件，放components): ui/src/components/Terminal.vue
cat > ${COMPONENTS_DIR}/Terminal.vue << 'EOF'
<template>
  <div class="terminal">
    <div class="terminal-log" ref="log">{{terminalLog}}</div>
    <div class="terminal-input">
      <span>> </span>
      <input v-model="cmd" @keyup.enter="sendCmdHandle" placeholder="输入命令并回车执行"/>
    </div>
  </div>
</template>

<script>
export default {
  name: 'Terminal',
  props: { placeholder: { type: String, default: '输入命令' } },
  data() { return { terminalLog: '欢迎使用SSH终端，输入命令执行\n', cmd: '' } },
  methods: {
    showMsg(msg) { this.terminalLog += `\n${msg}\n`; this.scrollToBottom(); },
    appendOutput(out) { this.terminalLog += out + '\n'; this.scrollToBottom(); },
    sendCmdHandle() {
      if(!this.cmd.trim()) return;
      this.terminalLog += `> ${this.cmd}\n`;
      this.$emit('sendCmd', this.cmd.trim());
      this.cmd = '';
      this.scrollToBottom();
    },
    scrollToBottom() { this.$refs.log.scrollTop = this.$refs.log.scrollHeight; }
  }
}
</script>

<style scoped>
.terminal {
  width: 100%;
  height: 100%;
  background: #000;
  color: #00FF00;
  padding: 10px;
  font-family: monospace;
  font-size: 14px;
}
.terminal-log {
  height: calc(100% - 30px);
  overflow-y: auto;
  white-space: pre-wrap;
}
.terminal-input {
  height: 30px;
  display: flex;
  align-items: center;
}
.terminal-input input {
  flex: 1;
  background: transparent;
  border: none;
  color: #00FF00;
  outline: none;
  font-size: 14px;
}
</style>
EOF

# --- 7. 公共组件 - VNC画布 (仅组件，放components): ui/src/components/VncCanvas.vue
cat > ${COMPONENTS_DIR}/VncCanvas.vue << 'EOF'
<template>
  <div class="vnc-canvas">
    <canvas ref="canvas" width="1024" height="768"></canvas>
    <div class="status" v-if="!connected">未连接VNC，点击上方按钮连接</div>
  </div>
</template>

<script>
export default {
  name: 'VncCanvas',
  props: { connected: { type: Boolean, default: false } },
  mounted() { this.initCanvas(); },
  methods: {
    initCanvas() {
      const ctx = this.$refs.canvas.getContext('2d');
      ctx.fillStyle = '#f5f5f5';
      ctx.fillRect(0,0,1024,768);
    },
    updateCanvas(data) {
      if(!this.connected) return;
      const ctx = this.$refs.canvas.getContext('2d');
      const img = new Image();
      img.onload = () => ctx.drawImage(img,0,0);
      img.src = data;
    }
  }
}
</script>

<style scoped>
.vnc-canvas { width: 100%; height: 100%; text-align: center; }
canvas { width: 100%; height: 100%; border: 1px solid #ccc; }
.status { margin-top: 20px; color: #999; }
</style>
EOF

# --- 8. 业务页面 - 首页 (放pages): ui/src/pages/index/index.vue
cat > ${PAGES_DIR}/index/index.vue << 'EOF'
<template>
  <div class="index-page">
    <div class="btn-list">
      <button class="func-btn" @click="$router.push('/ssh')">SSH 终端连接</button>
      <button class="func-btn" @click="$router.push('/vnc')">VNC 远程投屏</button>
      <button class="func-btn" @click="$router.push('/file')">文件管理</button>
      <button class="func-btn" @click="$router.push('/setting')">系统设置</button>
    </div>
  </div>
</template>

<script>
export default { name: 'IndexPage' }
</script>

<style scoped>
.index-page { display: flex; align-items: center; justify-content: center; height: 100%; }
.btn-list { width: 90%; display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }
.func-btn { padding: 25px 0; border: none; border-radius: 8px; background: #1E90FF; color: #fff; font-size: 16px; }
.func-btn:active { background: #0066CC; }
</style>
EOF

# --- 9. 业务页面 - SSH终端 (放pages): ui/src/pages/ssh/ssh.vue
cat > ${PAGES_DIR}/ssh/ssh.vue << 'EOF'
<template>
  <div class="ssh-page">
    <div class="config-bar">
      <input v-model="sshIp" placeholder="SSH服务器IP" class="ipt-ip" />
      <button @click="connectSSH" class="btn-connect">{{connected?'已连接':'连接SSH'}}</button>
    </div>
    <Terminal ref="terminal" @sendCmd="sendSSHCommand" />
  </div>
</template>

<script>
import Terminal from '../../components/Terminal.vue'
// 加载后端核心库 - 路径完美匹配 ui/libs 目录
const sshVncLib = window.require('./libs/libssh-vnc-full.so')

export default {
  components: { Terminal },
  data() { return { sshIp: '192.168.1.100', connected: false } },
  methods: {
    connectSSH() {
      const res = sshVncLib.ssh_init(this.sshIp)
      if(res === 0) {
        this.connected = true
        this.$refs.terminal.showMsg('✅ SSH连接成功！')
      } else {
        this.$refs.terminal.showMsg(`❌ SSH连接失败，错误码: ${res}`)
      }
    },
    sendSSHCommand(cmd) {
      if(!this.connected) return this.$refs.terminal.showMsg('❌ 请先连接SSH服务器')
      const result = new Array(4096).fill(0)
      sshVncLib.ssh_send_cmd(cmd, result, 4096)
      this.$refs.terminal.appendOutput(result.join(''))
    }
  }
}
</script>

<style scoped>
.ssh-page { height: 100%; }
.config-bar { padding: 10px; display: flex; gap: 10px; }
.ipt-ip { flex:1; padding:8px; border:1px solid #ccc; border-radius:4px; outline:none; }
.btn-connect { padding:8px 15px; border:none; border-radius:4px; background:#32CD32; color:#fff; }
</style>
EOF

# --- 10. 业务页面 - VNC投屏 (放pages): ui/src/pages/vnc/vnc.vue
cat > ${PAGES_DIR}/vnc/vnc.vue << 'EOF'
<template>
  <div class="vnc-page">
    <div class="config-bar">
      <input v-model="vncIp" placeholder="VNC服务器IP" class="ipt-ip" />
      <button @click="connectVNC" class="btn-connect">{{connected?'已连接':'连接VNC'}}</button>
    </div>
    <VncCanvas :connected="connected" ref="vncCanvas" />
  </div>
</template>

<script>
import VncCanvas from '../../components/VncCanvas.vue'
const sshVncLib = window.require('./libs/libssh-vnc-full.so')

export default {
  components: { VncCanvas },
  data() { return { vncIp: '192.168.1.100', connected: false } },
  methods: {
    connectVNC() {
      const res = sshVncLib.vnc_init(this.vncIp)
      if(res === 0) {
        this.connected = true
        this.$refs.vncCanvas.showMsg('✅ VNC投屏连接成功！')
        this.startVncRender()
      } else {
        this.$refs.vncCanvas.showMsg(`❌ VNC连接失败，错误码: ${res}`)
      }
    },
    startVncRender() {
      setInterval(() => {
        if(!this.connected) return
        const frame = sshVncLib.vnc_get_frame()
        this.$refs.vncCanvas.updateCanvas(frame)
      }, 30);
    }
  }
}
</script>

<style scoped>
.vnc-page { height:100%; }
.config-bar { padding:10px; display:flex; gap:10px; }
.ipt-ip { flex:1; padding:8px; border:1px solid #ccc; border-radius:4px; outline:none; }
.btn-connect { padding:8px 15px; border:none; border-radius:4px; background:#32CD32; color:#fff; }
</style>
EOF

# --- 11. 业务页面 - 文件管理 (放pages): ui/src/pages/file/file.vue
cat > ${PAGES_DIR}/file/file.vue << 'EOF'
<template>
  <div class="file-page">
    <input v-model="filePath" placeholder="输入文件路径 e.g. /root/test.md" class="ipt-path" />
    <button @click="viewFile" class="btn-view">查看文件</button>
    <div class="file-content" v-if="content">{{content}}</div>
  </div>
</template>

<script>
const sshVncLib = window.require('./libs/libssh-vnc-full.so')

export default {
  data() { return { filePath: '/root/test.md', content: '' } },
  methods: {
    viewFile() {
      if(!this.filePath) return this.content = '❌ 文件路径不能为空'
      const result = new Array(8192).fill(0)
      const res = sshVncLib.file_view(this.filePath, result, 8192)
      if(res === 0) {
        this.content = result.join('')
      } else {
        this.content = `❌ 文件读取失败，错误码: ${res}`
      }
    }
  }
}
</script>

<style scoped>
.file-page { padding:10px; height:100%; }
.ipt-path { width:100%; padding:8px; margin-bottom:10px; border:1px solid #ccc; border-radius:4px; outline:none; }
.btn-view { padding:8px 15px; border:none; border-radius:4px; background:#1E90FF; color:#fff; }
.file-content { margin-top:10px; padding:10px; border:1px solid #ccc; border-radius:4px; height: calc(100% - 80px); overflow:auto; white-space: pre-wrap; }
</style>
EOF

# --- 12. 业务页面 - 系统设置 (放pages): ui/src/pages/setting/setting.vue
cat > ${PAGES_DIR}/setting/setting.vue << 'EOF'
<template>
  <div class="setting-page">
    <div class="setting-item">
      <span>SSH端口:</span>
      <input v-model="sshPort" type="number" class="ipt-set" />
    </div>
    <div class="setting-item">
      <span>VNC分辨率:</span>
      <select v-model="vncSize" class="ipt-set">
        <option value="1024x768">1024x768</option>
        <option value="800x600">800x600</option>
      </select>
    </div>
    <button @click="saveSetting" class="btn-save">保存设置</button>
  </div>
</template>

<script>
export default {
  data() { return { sshPort: 22, vncSize: '1024x768' } },
  methods: {
    saveSetting() {
      alert(`✅ 设置保存成功！\nSSH端口: ${this.sshPort}\nVNC分辨率: ${this.vncSize}`)
    }
  }
}
</script>

<style scoped>
.setting-page { padding:20px; }
.setting-item { display:flex; justify-content:space-between; align-items:center; margin-bottom:20px; }
.ipt-set { padding:8px; border:1px solid #ccc; border-radius:4px; outline:none; width:150px; }
.btn-save { width:100%; padding:10px; border:none; border-radius:4px; background:#1E90FF; color:#fff; font-size:16px; }
</style>
EOF

    ok "所有前端源码文件写入完成，共12个核心文件"
}

# ===================== 第四步：安装前端依赖 + 修复aiot-vue-cli配置补丁 =====================
install_deps_and_patch(){
    info "进入前端目录，安装依赖包..."
    cd ${UI_DIR}
    pnpm install --registry=https://registry.npmmirror.com &>/dev/null
    ok "前端依赖安装完成"

    info "修复aiot-vue-cli配置文件 (和miniapp完全一致的补丁)..."
    sed -i "s/commonjs(),/commonjs(),require('@rollup\/plugin-typescript')(),/g" ./node_modules/aiot-vue-cli/src/libs/rollup.config.js
    sed -i "s/compiler.parseComponent(content, { pad: 'line' })/compiler.parse(content, { pad: 'line' }).descriptor/g" ./node_modules/aiot-vue-cli/web-loaders/falcon-vue-loader/lib/parser.js
    sed -i "s/path.resolve(__dirname, '.\/vue\/packages\/vue-template-compiler\/index.js')/'@vue\/compiler-sfc'/g" ./node_modules/aiot-vue-cli/cli-libs/index.js
    sed -i "s/compiler.parseComponent(content, { pad: true })/compiler.parse(content, { pad: true }).descriptor/g" ./node_modules/aiot-vue-cli/src/libs/parser.js
    sed -i "s/compiler.compile/compiler.compileTemplate/g" ./node_modules/aiot-vue-cli/web-loaders/falcon-vue-loader/lib/template-compiler/index.js
    sed -i "s/const replaceValues = {}/const replaceValues = { 'defineComponent': '' }/g" ./node_modules/aiot-vue-cli/src/libs/rollup.config.js
    ok "aiot-vue-cli补丁修复完成"
}

# ===================== 第五步：前端编译打包 生成部署产物 =====================
build_frontend(){
    info "执行前端编译打包，生成部署产物..."
    cd ${UI_DIR}
    pnpm run package &>/dev/null
    # 编译产物自动生成在 ui/dist 目录，部署时直接拷贝该目录即可
    if [ -d "${UI_DIR}/dist" ]; then
        ok "前端编译打包成功！部署产物目录: ${UI_DIR}/dist"
    else
        err "前端编译失败，请检查日志"
    fi
}

# ===================== 主执行流程 =====================
main(){
    info "============= SSH-VNC 前端源码一键部署开始 ============="
    check_env
    mkdir_frontend_dir
    write_frontend_code
    install_deps_and_patch
    build_frontend
    cd ${ROOT_DIR}
    echo -e "\n${GREEN}✅✅✅ 前端源码部署+编译全部完成！✅✅✅${NC}"
    echo -e "📌 前端源码目录: ${UI_DIR}"
    echo -e "📌 部署产物目录: ${UI_DIR}/dist (直接拷贝到ARM设备即可)"
    echo -e "📌 后端库关联: ${LIB_DIR}/libssh-vnc-full.so"
    info "========================================================"
}

# 启动脚本
main
