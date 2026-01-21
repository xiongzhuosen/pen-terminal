<template>
  <view class="wrapper">
    <view class="card">
      <text class="modal-title" style="font-size: 16px; text-align: center; margin-bottom: 16px;">关于 SSH/VNC 全功能工具</text>
      <view class="viewer-content" style="height: 500px;">
        <text style="font-size: 12px; line-height: 1.5; color: #e0e0e0;">
          版本：v1.0.0（零报错·Weex 兼容版）\n
          架构：ARM C++ SO 库 + Vue Weex 前端\n
          核心功能：\n
          ✅ SSH：密码/密钥双连接、流式仿真终端（支持 vim/passwd）、自定义快捷命令（方块点击）\n
          ✅ VNC：媲美 RVNC Viewer、触摸鼠标控制、多档位缩放、实时帧渲染\n
          ✅ 文件管理：chmod 赋权、chown 改属主、lsattr 查属性、删除/重命名、多格式查看\n
          ✅ 文件查看：文本/HEX 二进制/MD 格式渲染（支持 Markdown 语法）\n
          ✅ 内置键盘：全页面复用，支持 ESC/方向键/CTRL+C/TAB/ENTER 等特殊键\n
          ✅ 主题：260×640 小屏精准适配，黑色质感 UI\n
          技术栈：\n
          前端：Vue 2.x + Less + Weex + aiot-cli\n
          后端：C++ + libssh2 + libvncserver + md4c + jsoncpp\n
          存储：本地快捷命令持久化（$falcon.storage）\n
          注意事项：\n
          1. SSH 流式终端需后端 $api.ssh_read_stream 支持实时数据推送\n
          2. VNC 连接需后端 $api.vnc_read_frame 提供 base64 帧数据\n
          3. 文件操作需后端 $api.file_* 接口支持（chmod/chown/lsattr/读写）\n
          4. MD 渲染需后端 $api.file_render_md 返回渲染后内容\n
          5. 内置键盘仅支持英文输入，特殊键映射已适配终端/VNC 交互\n
          © 2025 全功能 SSH/VNC/文件管理工具 · 260×640 黑主题版
        </text>
      </view>
      <view class="btn" style="margin-top: 16px;" @click="goBack">
        <text>返回首页</text>
      </view>
    </view>
  </view>
</template>

<script>
export default {
  name: "AboutPage",
  methods: {
    goBack() {
      $falcon.navigateBack();
    }
  }
};
</script>

<style lang="less" scoped>
@import "../../styles/base.less";
</style>
