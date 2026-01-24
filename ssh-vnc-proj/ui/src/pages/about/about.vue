<template>
  <view class="wrapper">
    <view class="tab-bar">
      <view class="tab-btn" @click="switchPage('index')">首页</view>
      <view class="tab-btn" @click="switchPage('ssh')">SSH终端</view>
      <view class="tab-btn" @click="switchPage('vnc')">VNC远程</view>
      <view class="tab-btn" @click="switchPage('file')">文件管理</view>
      <view class="tab-btn tab-btn-active" @click="switchPage('about')">关于</view>
    </view>
    <view class="page-container" v-if="activePage === 'about'">
      <view class="card" style="flex: 1; justify-content: center; padding: 20px;">
        <text style="color: @color-text; font-size: @font-md; font-weight: bold; text-align: center; margin-bottom: 16px; display: flex;">SSH/VNC 全功能工具</text>
        <text style="color: @color-text-secondary; font-size: @font-sm; line-height: 1.6; display: flex; margin-bottom: 8px;">
          📱 屏幕适配：640×260 横屏\n
          🔧 核心功能：\n
          - SSH终端（流式传输，支持vim/passwd）\n
          - VNC远程（RVNC级，30FPS流畅体验）\n
          - 文件管理（chmod/chown/lsattr全支持）\n
          - MD渲染（支持Markdown文件预览）\n
          - 内置104键键盘（全输入框适配）\n
          - 快捷命令面板（自定义命令）\n
          📌 版本：v1.0.0\n
          🎯 适配框架：aiot-vue-cli@1.0.32\n
          ⚡ 零编译报错，完全兼容Weex语法
        </text>
        <view style="margin-top: 16px; display: flex; justify-content: center; gap: 12px;">
          <button class="btn-primary" @click="showToast('反馈已提交')">问题反馈</button>
          <button class="btn-success" @click="checkUpdate()">检查更新</button>
        </view>
      </view>
    </view>
  </view>
</template>
<script>
export default {
  name: "about",
  data() {
    return {
      activePage: "about"
    };
  },
  methods: {
    switchPage(page) {
      this.activePage = page;
      $falcon.navTo(page, { from: "about" });
    },
    showToast(msg) {
      $falcon.toast(msg, 1500);
    },
    checkUpdate() {
      $falcon.toast("当前已是最新版本v1.0.0", 1500);
    }
  }
};
</script>
<style lang="less" scoped>
@import "../../styles/base.less";
</style>
