<template>
  <view class="wrapper">
    <view class="tab-bar">
      <view class="tab-btn" @click="switchPage('index')">首页</view>
      <view class="tab-btn tab-btn-active" @click="switchPage('ssh')">SSH终端</view>
      <view class="tab-btn" @click="switchPage('vnc')">VNC远程</view>
      <view class="tab-btn" @click="switchPage('file')">文件管理</view>
      <view class="tab-btn" @click="switchPage('about')">关于</view>
    </view>
    <view class="page-container ssh-page" v-if="activePage === 'ssh'">
      <div class="ssh-section">
        <!-- 连接设置区 -->
        <div class="connect-bar">
          <input 
            class="input ip-input" 
            v-model="sshConfig.host" 
            placeholder="SSH 主机：192.168.1.100"
            @focus="setActiveInput('host')"
          >
          <div class="multi-input">
            <input 
              class="input" 
              v-model="sshConfig.port" 
              placeholder="端口：22" 
              style="flex: 1;"
              @focus="setActiveInput('port')"
            >
            <input 
              class="input" 
              v-model="sshConfig.user" 
              placeholder="用户：root" 
              style="flex: 1;"
              @focus="setActiveInput('user')"
            >
            <input 
              class="input" 
              v-model="sshConfig.pass" 
              placeholder="密码：******" 
              type="password" 
              style="flex: 1;"
              @focus="setActiveInput('pass')"
            >
          </div>
          <div class="connect-btn-group">
            <button class="btn-success" style="flex: 1;" @click="connectSSH" :disabled="isConnected">连接</button>
            <button class="btn-danger" style="flex: 1;" @click="disconnectSSH" :disabled="!isConnected">断开</button>
          </div>
        </div>
        <!-- 状态条 -->
        <div class="status-bar">
          <span class="status-text">● {{ isConnected ? '已连接' : '未连接' }}</span>
          <span class="status-subtext">编码：UTF-8 | 终端类型：xterm-256color | 流式传输：已启用</span>
        </div>
        <!-- 快捷命令面板 -->
        <quick-cmd-panel @exec-cmd="execQuickCmd" />
        <!-- 终端区 -->
        <div class="terminal" ref="terminal" @focus="setActiveInput('terminal')">
          <div v-for="(line, idx) in terminalLines" :key="idx" style="white-space: pre-wrap;">{{ line }}</div>
          <span class="terminal-cursor" v-if="isConnected"></span>
        </div>
        <!-- 内置键盘（默认显示） -->
        <virtual-keyboard 
          :show-keyboard="true"
          :active-input="activeInput"
          @key-input="handleKeyInput"
        />
      </div>
    </view>
  </view>
</template>
<script>
import QuickCmdPanel from '@/components/QuickCmdPanel.vue';
import VirtualKeyboard from '@/components/VirtualKeyboard.vue';
export default {
  name: "ssh",
  components: { QuickCmdPanel, VirtualKeyboard },
  data() {
    return {
      activePage: "ssh",
      sshConfig: { host: "", port: "22", user: "root", pass: "" },
      isConnected: false,
      sshConnId: "",
      terminalLines: [
        "[root@localhost ~]# ls -l",
        "total 48",
        "-rw-r--r-- 1 root root  128 Jan 21 10:00 test.txt",
        "-rwxr-xr-x 1 root root   45 Jan 20 15:30 deploy.sh",
        "drwxr-xr-x 2 root root 4096 Jan 19 09:15 docs",
        "[root@localhost ~]# cd docs",
        "[root@localhost docs]# ls",
        "manual.pdf  config.ini",
        "[root@localhost docs]# echo \"hello world\" > test.log",
        "[root@localhost docs]# chmod 755 test.log",
        "[root@localhost docs]# "
      ],
      currentCommand: "",
      activeInput: "terminal",
      maxTerminalLines: 50,
      streamTimer: null
    };
  },
  beforeDestroy() {
    this.disconnectSSH();
  },
  methods: {
    switchPage(page) {
      this.activePage = page;
      $falcon.navTo(page, { from: "ssh" });
    },
    setActiveInput(inputName) {
      this.activeInput = inputName;
    },
    handleKeyInput(key) {
      switch(this.activeInput) {
        case 'host':
          this.sshConfig.host += key.replace(/\n|\t|\b/g, '');
          break;
        case 'port':
          this.sshConfig.port += key.replace(/\D|\n|\t|\b/g, '');
          break;
        case 'user':
          this.sshConfig.user += key.replace(/\n|\t|\b/g, '');
          break;
        case 'pass':
          this.sshConfig.pass += key.replace(/\n|\t|\b/g, '');
          break;
        case 'terminal':
          this.handleTerminalInput(key);
          break;
      }
    },
    handleTerminalInput(key) {
      if (!this.isConnected) return;
      if (key === '\b') {
        this.currentCommand = this.currentCommand.slice(0, -1);
        const lastLineIdx = this.terminalLines.length - 1;
        this.terminalLines[lastLineIdx] = "[root@localhost docs]# " + this.currentCommand;
      } else if (key === '\n') {
        this.execCmd();
      } else {
        this.currentCommand += key;
        const lastLineIdx = this.terminalLines.length - 1;
        this.terminalLines[lastLineIdx] = "[root@localhost docs]# " + this.currentCommand;
      }
      this.scrollToBottom();
    },
    async connectSSH() {
      if (!this.sshConfig.host) {
        $falcon.toast("主机不能为空");
        return;
      }
      try {
        const res = await $falcon.ssh_connect({
          host: this.sshConfig.host,
          port: parseInt(this.sshConfig.port),
          user: this.sshConfig.user,
          pass: this.sshConfig.pass
        });
        if (res.code === 0) {
          this.isConnected = true;
          this.sshConnId = res.connId;
          this.terminalLines = this.terminalLines.slice(0, -1).concat("[root@localhost docs]# ");
          this.currentCommand = "";
          this.startStream();
          $falcon.toast("SSH连接成功（支持vim/passwd）");
          this.scrollToBottom();
        } else {
          $falcon.toast(`连接失败：${res.msg}`);
        }
      } catch (err) {
        $falcon.toast(`连接异常：${err.message}`);
      }
    },
    async disconnectSSH() {
      if (!this.isConnected) return;
      try {
        await $falcon.ssh_disconnect(this.sshConnId);
        this.isConnected = false;
        this.sshConnId = "";
        this.addTerminalLine("已断开SSH连接");
        this.stopStream();
      } catch (err) {
        $falcon.toast(`断开失败：${err.message}`);
      }
    },
    startStream() {
      // 启动流式传输（模拟实时读取）
      this.streamTimer = setInterval(async () => {
        if (!this.isConnected) return;
        try {
          const res = await $falcon.ssh_stream_read({ connId: this.sshConnId });
          if (res.code === 0 && res.data) {
            this.addTerminalLine(res.data);
          }
        } catch (err) {
          this.addTerminalLine(`流读取错误：${err.message}`);
          this.stopStream();
        }
      }, 50);
    },
    stopStream() {
      if (this.streamTimer) {
        clearInterval(this.streamTimer);
        this.streamTimer = null;
      }
    },
    execCmd() {
      if (!this.isConnected || !this.currentCommand.trim()) return;
      this.addTerminalLine(`[root@localhost docs]# ${this.currentCommand}`);
      $falcon.ssh_stream_write({
        connId: this.sshConnId,
        data: this.currentCommand + "\n"
      });
      this.currentCommand = "";
      this.terminalLines.push("[root@localhost docs]# ");
      this.scrollToBottom();
    },
    execQuickCmd(cmd) {
      if (!this.isConnected) return;
      this.addTerminalLine(`[root@localhost docs]# ${cmd}`);
      $falcon.ssh_stream_write({
        connId: this.sshConnId,
        data: cmd + "\n"
      });
      this.terminalLines.push("[root@localhost docs]# ");
      this.scrollToBottom();
    },
    addTerminalLine(line) {
      this.terminalLines.push(line);
      if (this.terminalLines.length > this.maxTerminalLines) {
        this.terminalLines.shift();
      }
      this.scrollToBottom();
    },
    scrollToBottom() {
      this.$nextTick(() => {
        const terminal = this.$refs.terminal;
        if (terminal.scrollTo) {
          terminal.scrollTo({ top: terminal.scrollHeight });
        }
      });
    }
  }
};
</script>
<style lang="less" scoped>
@import "../../styles/base.less";
</style>
