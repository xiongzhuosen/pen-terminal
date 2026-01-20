<template>
  <div class="page-root">
    <div class="wrapper">
      <text class="text-lg">SSH-VNC 管理工具</text>
      <text class="text-md">版本: 2.0.0</text>
      <text class="text-md">适配屏幕: 260×640</text>
      <text class="text-md">功能支持:</text>
      <text class="text-sm">1. SSH多连接/密钥登录/流式终端</text>
      <text class="text-sm">2. VNC键鼠/缩放/对标RealVNC</text>
      <text class="text-sm">3. 文件管理(lsattr/chmod/chown)</text>
      <text class="text-sm">4. 文本/16进制/MD多格式查看</text>
      <text class="text-sm">5. 内置虚拟键盘</text>
    </div>
  </div>
</template>
<script>
export default { name: "about", methods: { onShow() { console.log("关于页面显示"); } } };
</script>
<style lang="less" scoped>
@import "../../styles/base.less";
.wrapper { justify-content: center; align-items: center; text-align: center; gap: @padding-md; }
</style>
