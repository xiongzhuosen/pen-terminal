<template>
  <view class="quick-cmd-panel">
    <view
      v-for="(cmd, idx) in quickCmds"
      :key="idx"
      class="quick-cmd-btn"
      @click="execCmd(cmd)"
    >
      <text>{{ cmd.label }}</text>
    </view>
    <view class="quick-cmd-btn" @click="addCmd">
      <text>+ 新增</text>
    </view>
  </view>
</template>
<script>
export default {
  name: "QuickCmdPanel",
  data() {
    return {
      quickCmds: [
        { label: "ls", value: "ls -l" },
        { label: "pwd", value: "pwd" },
        { label: "df", value: "df -h" },
        { label: "free", value: "free -m" },
        { label: "ps", value: "ps -ef" },
        { label: "clear", value: "clear" },
        { label: "ifconfig", value: "ifconfig" },
        { label: "reboot", value: "reboot" },
        { label: "ping", value: "ping 8.8.8.8" },
        { label: "netstat", value: "netstat -tuln" }
      ]
    };
  },
  methods: {
    execCmd(cmd) {
      this.$emit("exec-cmd", cmd.value);
    },
    addCmd() {
      const input = prompt("输入命令格式：标签|命令内容（例：测试|echo hello）", "测试|echo hello");
      if (input) {
        const [label, value] = input.split("|");
        if (label && value) {
          this.quickCmds.push({ label, value });
        }
      }
    }
  }
};
</script>
<style lang="less" scoped>
@import "../styles/base.less";
</style>
