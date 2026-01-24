<template>
  <view class="wrapper">
    <view class="tab-bar">
      <view class="tab-btn tab-btn-active" @click="switchPage('index')">首页</view>
      <view class="tab-btn" @click="switchPage('ssh')">SSH终端</view>
      <view class="tab-btn" @click="switchPage('vnc')">VNC远程</view>
      <view class="tab-btn" @click="switchPage('file')">文件管理</view>
      <view class="tab-btn" @click="switchPage('about')">关于</view>
    </view>
    <view class="page-container" v-if="activePage === 'index'">
      <view class="card" style="flex: 1; justify-content: center; align-items: center;">
        <h2 style="color: @color-text; font-size: @font-lg; margin-bottom: 24px;">SSH/VNC 全功能工具</h2>
        <div style="display: flex; gap: 16px; width: 70%;">
          <button class="btn-primary" @click="switchPage('ssh')">SSH 终端</button>
          <button class="btn-warning" @click="switchPage('vnc')">VNC 远程</button>
          <button class="btn-success" @click="switchPage('file')">文件管理</button>
          <button class="btn" @click="switchPage('about')">关于</button>
        </div>
      </view>
    </view>
  </view>
</template>
<script>
export default {
  name: "index",
  data() {
    return {
      activePage: "index"
    };
  },
  methods: {
    switchPage(page) {
      this.activePage = page;
      $falcon.navTo(page, { from: "index" });
    }
  }
};
</script>
<style lang="less" scoped>
@import "../../styles/base.less";
</style>
