<template>
  <div class="wrapper">
    <text class="greeting">SSH-VNC 远程管理</text>
    <div class="btn-group">
      <text class="btn primary" @click="jumpTo('ssh')">SSH 终端</text>
      <text class="btn primary" @click="jumpTo('vnc')">VNC 投屏</text>
      <text class="btn primary" @click="jumpTo('file')">文件管理</text>
      <text class="btn primary" @click="jumpTo('setting')">系统设置</text>
    </div>
  </div>
</template>

<script>
export default {
  name: 'index',
  methods: {
    onShow() {
      console.log("首页已显示");
    },
    jumpTo(pageName) {
      $falcon.navTo(pageName, { from: 'index' });
    }
  }
}
</script>

<style lang="less" scoped>
@import "base.less";

.greeting {
  text-align: center;
  margin: 20px 0;
  font-size: 50px;
  color: @text-color-success;
}

.btn-group {
  width: 90%;
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: 15px;
  margin-top: 20px;
}
</style>
