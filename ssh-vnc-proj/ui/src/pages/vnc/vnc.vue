<template>
  <view class="wrapper">
    <!-- VNC 连接配置 -->
    <view class="card">
      <input class="input" v-model="vncHost" placeholder="VNC 主机（如：192.168.1.1）" />
      <input class="input" v-model="vncPort" placeholder="VNC 端口（默认：5900）" />
      <input class="input" v-model="vncPass" placeholder="VNC 密码" type="password" />
      <view style="flex-direction: row; justify-content: space-between;">
        <view class="btn" style="flex: 1; margin-right: 4px;" @click="connectVNC" :class="{ 'btn-active': isConnecting }">
          <text>连接 VNC</text>
        </view>
        <view class="btn" style="flex: 1;" @click="disconnectVNC" :class="{ 'btn-active': isDisconnecting }">
          <text>断开 VNC</text>
        </view>
      </view>
      <view style="flex-direction: row; justify-content: space-between; margin-top: 8px;">
        <view class="btn" style="flex: 1; margin-right: 4px;" @click="setScale(1.0)">
          <text>1.0x</text>
        </view>
        <view class="btn" style="flex: 1; margin-right: 4px;" @click="setScale(1.5)">
          <text>1.5x</text>
        </view>
        <view class="btn" style="flex: 1;" @click="setScale(2.0)">
          <text>2.0x</text>
        </view>
      </view>
    </view>

    <!-- VNC 画布（触摸鼠标·媲美 RVNC） -->
    <view class="vnc-canvas" @touchstart="onTouchStart" @touchmove="onTouchMove" @touchend="onTouchEnd">
      <image class="vnc-frame" :src="vncFrame" />
      <view class="scale-tip" v-if="isConnected">
        <text>缩放：{{ currentScale }}x</text>
      </view>
    </view>

    <!-- 内置键盘 -->
    <VirtualKeyboard @send-key="sendKey" />
  </view>
</template>

<script>
import VirtualKeyboard from "../../components/VirtualKeyboard.vue";

export default {
  name: "VNCPage",
  components: { VirtualKeyboard },
  data() {
    return {
      vncHost: "",
      vncPort: "5900",
      vncPass: "",
      isConnected: false,
      isConnecting: false,
      isDisconnecting: false,
      vncFrame: "",
      currentScale: 1.0,
      frameTimer: null,
      touchX: 0,
      touchY: 0
    };
  },
  onUnload() {
    if (this.frameTimer) this.clearInterval(this.frameTimer);
    this.disconnectVNC();
  },
  methods: {
    // 连接 VNC
    async connectVNC() {
      if (!this.vncHost || !this.vncPass) {
        $falcon.toast.show("主机/密码不能为空");
        return;
      }
      this.isConnecting = true;
      const ret = await $api.vnc_connect(this.vncHost, this.vncPort, this.vncPass);
      this.isConnecting = false;
      if (ret === 0) {
        this.isConnected = true;
        this.startFrameRead();
        $falcon.toast.show("VNC 连接成功");
      } else {
        $falcon.toast.show(`VNC 连接失败：错误码 ${ret}`);
      }
    },
    // 断开 VNC
    async disconnectVNC() {
      if (!this.isConnected) return;
      this.isDisconnecting = true;
      const ret = await $api.vnc_disconnect();
      this.isDisconnecting = false;
      if (ret === 0) {
        this.isConnected = false;
        this.vncFrame = "";
        if (this.frameTimer) {
          this.clearInterval(this.frameTimer);
          this.frameTimer = null;
        }
        $falcon.toast.show("VNC 已断开");
      }
    },
    // 设置缩放
    async setScale(scale) {
      if (!this.isConnected) return;
      this.currentScale = scale;
      await $api.vnc_set_scale(scale);
      $falcon.toast.show(`缩放设置为 ${scale}x`);
    },
    // 帧数据读取（实时渲染）
    startFrameRead() {
      if (this.frameTimer) return;
      this.frameTimer = this.setInterval(async () => {
        const frame = await $api.vnc_read_frame();
        if (frame) this.vncFrame = frame; // base64 图片
      }, 50);
    },
    // 触摸鼠标（down/move/up·媲美 RVNC）
    onTouchStart(e) {
      if (!this.isConnected) return;
      this.touchX = e.touches[0].x;
      this.touchY = e.touches[0].y;
      $api.vnc_send_mouse({ type: "down", x: this.touchX, y: this.touchY });
    },
    onTouchMove(e) {
      if (!this.isConnected) return;
      const x = e.touches[0].x;
      const y = e.touches[0].y;
      $api.vnc_send_mouse({ type: "move", x, y });
    },
    onTouchEnd() {
      if (!this.isConnected) return;
      $api.vnc_send_mouse({ type: "up", x: this.touchX, y: this.touchY });
    },
    // 发送键盘按键
    async sendKey(key) {
      if (!this.isConnected) return;
      await $api.vnc_send_key(key);
    }
  }
};
</script>

<style lang="less" scoped>
@import "../../styles/base.less";
</style>
