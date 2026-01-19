<template>
  <div class="terminal">
    <div class="terminal-log" ref="log">{{terminalLog}}</div>
    <div class="terminal-input">
      <span>> </span>
      <input v-model="cmd" @keyup.enter="sendCmdHandle" placeholder="输入命令回车执行"/>
    </div>
  </div>
</template>

<script>
export default {
  name: "Terminal",
  data() {
    return {
      terminalLog: "欢迎使用SSH终端\n",
      cmd: ""
    };
  },
  methods: {
    showMsg(msg) {
      this.terminalLog += `\n${msg}\n`;
      this.scrollToBottom();
    },
    appendOutput(out) {
      this.terminalLog += out + "\n";
      this.scrollToBottom();
    },
    sendCmdHandle() {
      if(!this.cmd.trim()) return;
      this.terminalLog += `> ${this.cmd}\n`;
      this.$emit("sendCmd", this.cmd.trim());
      this.cmd = "";
      this.scrollToBottom();
    },
    scrollToBottom() {
      this.$refs.log.scrollTop = this.$refs.log.scrollHeight;
    }
  }
};
</script>

<style lang="less" scoped>
@import "base.less";

.terminal-log {
  height: calc(100% - 30px);
  overflow-y: auto;
  white-space: pre-wrap;
}

.terminal-input {
  height: 30px;
  display: flex;
  align-items: center;
  input {
    flex: 1;
    background: transparent;
    border: none;
    color: @text-color-terminal;
    outline: none;
    font-size: 14px;
  }
}
</style>
