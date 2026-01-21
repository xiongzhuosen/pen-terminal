<template>
  <view class="wrapper">
    <view class="card">
      <text class="modal-title" style="font-size: 16px; text-align: center; margin-bottom: 16px;">SSH/VNC 全功能工具</text>
      <view class="btn" @click="goSSH" @touchstart="onTouch('ssh')" @touchend="onTouchEnd">
        <text>SSH 终端</text>
      </view>
      <view class="btn" style="margin-top: 8px;" @click="goVNC" @touchstart="onTouch('vnc')" @touchend="onTouchEnd">
        <text>VNC 远程</text>
      </view>
      <view class="btn" style="margin-top: 8px;" @click="goFile" @touchstart="onTouch('file')" @touchend="onTouchEnd">
        <text>文件管理</text>
      </view>
      <view class="btn" style="margin-top: 8px;" @click="goAbout" @touchstart="onTouch('about')" @touchend="onTouchEnd">
        <text>关于</text>
      </view>
    </view>
  </view>
</template>

<script>
export default {
  name: "IndexPage",
  data() {
    return {
      activeBtn: ""
    };
  },
  methods: {
    goSSH() { $falcon.navigateTo({ url: "/pages/ssh/ssh.vue" }); },
    goVNC() { $falcon.navigateTo({ url: "/pages/vnc/vnc.vue" }); },
    goFile() { $falcon.navigateTo({ url: "/pages/file/file.vue" }); },
    goAbout() { $falcon.navigateTo({ url: "/pages/about/about.vue" }); },
    onTouch(btn) { this.activeBtn = btn; },
    onTouchEnd() { this.activeBtn = ""; }
  }
};
</script>

<style lang="less" scoped>
@import "../../styles/base.less";
</style>
