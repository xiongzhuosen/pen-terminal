<template>
  <div class="page-root">
    <div class="wrapper">
      <!-- 连接配置 -->
      <div class="conn-config">
        <input class="input" v-model="conn.ip" placeholder="服务器IP" />
        <input class="input" v-model="conn.port" placeholder="端口(默认22)" />
        <input class="input" v-model="conn.user" placeholder="用户名" />
        <input class="input" v-model="conn.pass" placeholder="密码" type="password" />
        <text class="btn-primary" @click="selectKey">选择密钥文件</text>
        <text class="btn-success" @click="connectSSH">{{connected?'断开':'连接'}}</text>
      </div>
      <!-- 快捷命令块 -->
      <div class="quick-cmds">
        <text class="btn-warning" @click="runQuickCmd('ls -la')">ls -la</text>
        <text class="btn-warning" @click="runQuickCmd('top')">top</text>
        <text class="btn-warning" @click="runQuickCmd('vim test.txt')">vim test.txt</text>
        <text class="btn-warning" @click="addQuickCmd">添加命令</text>
      </div>
      <!-- 流式终端 -->
      <Terminal ref="terminal" @cmdInput="runCmd" />
      <!-- 内置键盘 -->
      <VirtualKeyboard @keyPress="handleKeyPress" />
    </div>
  </div>
</template>
<script>
import Terminal from '../../components/Terminal.vue';
import VirtualKeyboard from '../../components/VirtualKeyboard.vue';
const so = window.require('./libs/libssh-vnc-full.so');
export default {
  name: "ssh",
  components: { Terminal, VirtualKeyboard },
  data() {
    return {
      connected: false,
      conn: { ip: "192.168.1.100", port: "22", user: "root", pass: "", keyPath: "" },
      quickCmds: ["ls -la", "top", "vim test.txt"]
    };
  },
  methods: {
    onShow() { console.log("SSH页面显示"); },
    // 选择密钥文件
    selectKey() { this.$emit("openFileManager", { type: "key" }); },
    // 连接SSH
    connectSSH() {
      if (this.connected) { so.ssh_disconnect(); this.connected = false; return; }
      // 支持密码/密钥登录
      const res = this.conn.keyPath ? so.ssh_connect_with_key(this.conn.ip, this.conn.port, this.conn.user, this.conn.keyPath) : so.ssh_connect(this.conn.ip, this.conn.port, this.conn.user, this.conn.pass);
      if (res === 0) {
        this.connected = true;
        this.$refs.terminal.appendOutput("✅ SSH连接成功，支持vim/passwd等交互式命令");
        // 启动终端流监听
        this.startTerminalStream();
      } else this.$refs.terminal.appendOutput(`❌ 连接失败，错误码: ${res}`);
    },
    // 启动终端流监听
    startTerminalStream() {
      this.$page.setInterval(() => {
        const buf = new Array(1024).fill(0);
        const len = so.ssh_read_stream(buf, 1024);
        if (len > 0) this.$refs.terminal.appendOutput(buf.join(''));
      }, 50);
    },
    // 运行命令
    runCmd(cmd) {
      if (!this.connected) return;
      so.ssh_write_stream(cmd + "\n");
    },
    // 运行快捷命令
    runQuickCmd(cmd) { this.$refs.terminal.handleKey(cmd); this.runCmd(cmd); },
    // 添加快捷命令
    addQuickCmd() { const cmd = prompt("输入快捷命令"); if (cmd) this.quickCmds.push(cmd); },
    // 处理键盘输入
    handleKeyPress(key) { this.$refs.terminal.handleKey(key); }
  }
};
</script>
<style lang="less" scoped>
@import "../../styles/base.less";
.conn-config { display: flex; flex-direction: column; gap: @padding-sm; margin-bottom: @padding-md; }
.quick-cmds { display: flex; flex-wrap: wrap; gap: @padding-sm; margin-bottom: @padding-md; }
</style>
