<template>
  <div class="terminal">
    <div class="terminal-log" ref="log">{{log}}</div>
    <div class="terminal-input">
      <span>> </span>
      <text class="input-text">{{input}}</text>
    </div>
  </div>
</template>
<script>
export default {
  name: "Terminal",
  props: { initLog: { type: String, default: "欢迎使用流式终端\n" } },
  data() { return { log: "", input: "" } },
  mounted() { this.log = this.initLog; },
  methods: {
    // 接收键盘输入
    handleKey(key) {
      if (key === "backspace") { this.input = this.input.slice(0, -1); return; }
      if (key === "enter") { this.emitInput(); return; }
      this.input += key;
    },
    // 发送输入到父组件
    emitInput() {
      if (!this.input.trim()) return;
      this.log += `> ${this.input}\n`;
      this.$emit("cmdInput", this.input);
      this.input = "";
      this.scrollToBottom();
    },
    // 追加输出
    appendOutput(output) {
      this.log += output + "\n";
      this.scrollToBottom();
    },
    // 滚动到底部
    scrollToBottom() { this.$refs.log.scrollTop = this.$refs.log.scrollHeight; }
  }
};
</script>
<style lang="less" scoped>
@import "../styles/base.less";
.terminal {
  width: @screen-w - 16px;
  height: 200px;
  padding: @padding-sm;
  overflow-y: auto;
}
.terminal-log { min-height: 170px; }
.terminal-input { display: flex; align-items: center; }
.input-text { flex: 1; height: @input-height; line-height: @input-height; }
</style>
