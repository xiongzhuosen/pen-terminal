<template>
  <view class="quick-cmd">
    <!-- 快捷命令方块 -->
    <view 
      v-for="(cmd, idx) in cmdList" 
      :key="idx" 
      class="cmd-btn"
      @touchstart="onCmdTouch(idx)"
      @touchend="onCmdTouchEnd"
      @click="execQuickCmd(cmd.content)"
    >
      <text>{{ cmd.name }}</text>
    </view>
    <!-- 新增命令按钮 -->
    <view 
      class="add-btn"
      @touchstart="onAddTouch"
      @touchend="onAddTouchEnd"
      @click="showAddModal"
    >
      <text>+新增</text>
    </view>

    <!-- 新增命令模态框 -->
    <view class="modal" v-if="showAddModal">
      <view class="modal-header">
        <text class="modal-title">新增快捷命令</text>
        <text class="modal-close" @click="hideAddModal">×</text>
      </view>
      <view class="modal-content">
        <input class="input" v-model="newCmd.name" placeholder="命令名称（如：查看IP）" />
        <input class="input" v-model="newCmd.content" placeholder="命令内容（如：ifconfig）" />
        <view class="modal-footer">
          <view class="btn" @click="addCmd">
            <text>保存</text>
          </view>
        </view>
      </view>
    </view>
  </view>
</template>

<script>
export default {
  name: "QuickCmdPanel",
  data() {
    return {
      cmdList: [],
      showAddModal: false,
      newCmd: { name: "", content: "" },
      activeCmdIdx: -1,
      activeAdd: false
    };
  },
  onLoad() {
    this.loadCmdList();
  },
  methods: {
    // 加载本地存储的快捷命令（默认8条，可自定义）
    loadCmdList() {
      const defaultCmds = [
        { name: "查看IP", content: "ifconfig" },
        { name: "磁盘占用", content: "df -h" },
        { name: "内存占用", content: "free -m" },
        { name: "赋权777", content: "chmod 777" },
        { name: "进程查看", content: "ps aux" },
        { name: "系统信息", content: "uname -a" },
        { name: "日志查看", content: "tail -n 20 /var/log/syslog" },
        { name: "清屏", content: "clear" }
      ];
      const local = $falcon.storage.get("quickCmdList");
      this.cmdList = local ? JSON.parse(local) : defaultCmds;
    },
    // 执行快捷命令（触发父组件）
    execQuickCmd(cmd) {
      this.$emit("exec-quick-cmd", cmd);
    },
    // 新增命令相关
    showAddModal() {
      this.showAddModal = true;
      this.newCmd = { name: "", content: "" };
    },
    hideAddModal() {
      this.showAddModal = false;
    },
    addCmd() {
      if (!this.newCmd.name || !this.newCmd.content) {
        $falcon.toast.show("名称和命令不能为空");
        return;
      }
      this.cmdList.push({ ...this.newCmd });
      $falcon.storage.set("quickCmdList", JSON.stringify(this.cmdList));
      this.hideAddModal();
      $falcon.toast.show("新增成功");
    },
    // 触摸模拟:active
    onCmdTouch(idx) {
      this.activeCmdIdx = idx;
    },
    onCmdTouchEnd() {
      this.activeCmdIdx = -1;
    },
    onAddTouch() {
      this.activeAdd = true;
    },
    onAddTouchEnd() {
      this.activeAdd = false;
    }
  }
};
</script>

<style lang="less" scoped>
@import "../styles/base.less";
</style>
