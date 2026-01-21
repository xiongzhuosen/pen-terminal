<template>
  <view class="wrapper">
    <!-- SSH 连接配置 -->
    <view class="card">
      <input class="input" v-model="sshHost" placeholder="SSH 主机（如：192.168.1.1）" />
      <input class="input" v-model="sshPort" placeholder="SSH 端口（默认：22）" />
      <input class="input" v-model="sshUser" placeholder="SSH 用户名（如：root）" />
      <input class="input" v-model="sshPass" placeholder="SSH 密码" type="password" />
      <input class="input" v-model="sshKeyPath" placeholder="密钥路径（可选）" />
      <view style="flex-direction: row; justify-content: space-between;">
        <view class="btn" style="flex: 1; margin-right: 4px;" @click="connectSSH" :class="{ 'btn-active': isConnecting }">
          <text>密码连接</text>
        </view>
        <view class="btn" style="flex: 1;" @click="connectSSHKey" :class="{ 'btn-active': isConnecting }">
          <text>密钥连接</text>
        </view>
      </view>
      <view class="btn" style="margin-top: 8px;" @click="disconnectSSH" :class="{ 'btn-active': isDisconnecting }">
        <text>断开连接</text>
      </view>
    </view>

    <!-- 快捷命令面板 -->
    <QuickCmdPanel @exec-quick-cmd="execQuickCmd" />

    <!-- 流式终端（仿真·支持 vim/passwd） -->
    <view class="terminal" ref="terminal">
      <text>{{ terminalContent }}</text>
    </view>

    <!-- 命令输入 -->
    <view class="card" style="flex-direction: row; align-items: center;">
      <input class="input" style="flex: 1; margin-bottom: 0; margin-right: 4px;" v-model="cmdInput" placeholder="输入命令" @confirm="execCmd" />
      <view class="btn" @click="execCmd">
        <text>执行</text>
      </view>
    </view>

    <!-- 内置键盘 -->
    <VirtualKeyboard @send-key="sendKey" />
  </view>
</template>

<script>
import QuickCmdPanel from "../../components/QuickCmdPanel.vue";
import VirtualKeyboard from "../../components/VirtualKeyboard.vue";

export default {
  name: "SSHPage",
  components: { QuickCmdPanel, VirtualKeyboard },
  data() {
    return {
      sshHost: "",
      sshPort: "22",
      sshUser: "",
      sshPass: "",
      sshKeyPath: "",
      isConnected: false,
      isConnecting: false,
      isDisconnecting: false,
      terminalContent: "=== SSH 仿真终端 ===\n连接后支持 vim/passwd 等交互式命令\n",
      cmdInput: "",
      streamTimer: null
    };
  },
  onUnload() {
    // 清理定时器+断开连接
    if (this.streamTimer) this.clearInterval(this.streamTimer);
    this.disconnectSSH();
  },
  methods: {
    // 密码连接
    async connectSSH() {
      if (!this.sshHost || !this.sshUser || !this.sshPass) {
        $falcon.toast.show("主机/用户名/密码不能为空");
        return;
      }
      this.isConnecting = true;
      const ret = await $api.ssh_connect(this.sshHost, this.sshPort, this.sshUser, this.sshPass);
      this.isConnecting = false;
      if (ret === 0) {
        this.isConnected = true;
        this.terminalContent += `\n✅ 连接成功：${this.sshHost}:${this.sshPort}\n`;
        this.startStreamRead();
        $falcon.toast.show("SSH 连接成功");
      } else {
        this.terminalContent += `\n❌ 连接失败：错误码 ${ret}\n`;
        $falcon.toast.show("SSH 连接失败");
      }
    },
    // 密钥连接
    async connectSSHKey() {
      if (!this.sshHost || !this.sshUser || !this.sshKeyPath) {
        $falcon.toast.show("主机/用户名/密钥路径不能为空");
        return;
      }
      this.isConnecting = true;
      const ret = await $api.ssh_connect_key(this.sshHost, this.sshPort, this.sshUser, this.sshKeyPath);
      this.isConnecting = false;
      if (ret === 0) {
        this.isConnected = true;
        this.terminalContent += `\n✅ 密钥连接成功：${this.sshHost}:${this.sshPort}\n`;
        this.startStreamRead();
        $falcon.toast.show("SSH 密钥连接成功");
      } else {
        this.terminalContent += `\n❌ 密钥连接失败：错误码 ${ret}\n`;
        $falcon.toast.show("SSH 密钥连接失败");
      }
    },
    // 断开连接
    async disconnectSSH() {
      if (!this.isConnected) return;
      this.isDisconnecting = true;
      const ret = await $api.ssh_disconnect();
      this.isDisconnecting = false;
      if (ret === 0) {
        this.isConnected = false;
        this.terminalContent += `\n🔌 已断开连接\n`;
        if (this.streamTimer) {
          this.clearInterval(this.streamTimer);
          this.streamTimer = null;
        }
        $falcon.toast.show("SSH 已断开");
      }
    },
    // 执行命令
    async execCmd() {
      if (!this.isConnected || !this.cmdInput) return;
      this.terminalContent += `\n$ ${this.cmdInput}\n`;
      await $api.ssh_exec(this.cmdInput);
      this.cmdInput = "";
    },
    // 快捷命令执行
    async execQuickCmd(cmd) {
      if (!this.isConnected) return;
      this.terminalContent += `\n$ ${cmd}\n`;
      await $api.ssh_exec(cmd);
    },
    // 发送键盘按键（支持 vim/passwd 交互）
    async sendKey(key) {
      if (!this.isConnected) return;
      await $api.ssh_send_key(key);
    },
    // 流式读取终端数据（仿真核心）
    startStreamRead() {
      if (this.streamTimer) return;
      this.streamTimer = this.setInterval(async () => {
        const data = await $api.ssh_read_stream();
        if (data) {
          this.terminalContent += data;
          // 滚动到底部（Weex 用 scrollTo，替代 overflow:scroll）
          this.$refs.terminal.scrollTo({ y: 99999 });
        }
      }, 100);
    }
  }
};
</script>

<style lang="less" scoped>
@import "../../styles/base.less";
</style>
