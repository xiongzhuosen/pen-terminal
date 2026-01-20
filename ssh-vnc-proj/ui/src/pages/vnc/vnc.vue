<template>
  <div class="page-root">
    <div class="wrapper">
      <!-- 连接配置 -->
      <div class="conn-config">
        <input class="input" v-model="conn.ip" placeholder="服务器IP" />
        <input class="input" v-model="conn.port" placeholder="端口(默认5900)" />
        <input class="input" v-model="conn.pass" placeholder="密码" type="password" />
        <text class="btn-success" @click="connectVNC">{{connected?'断开':'连接'}}</text>
      </div>
      <!-- VNC画布 -->
      <VncCanvas :connected="connected" ref="canvas" @scaleChange="handleScale" @mouseEvent="handleMouseEvent" />
      <!-- 内置键盘 -->
      <VirtualKeyboard @keyPress="handleKeyPress" />
    </div>
  </div>
</template>
<script>
import VncCanvas from '../../components/VncCanvas.vue';
import VirtualKeyboard from '../../components/VirtualKeyboard.vue';
const so = window.require('./libs/libssh-vnc-full.so');
export default {
  name: "vnc",
  components: { VncCanvas, VirtualKeyboard },
  data() { return { connected: false, conn: { ip: "192.168.1.100", port: "5900", pass: "" } }; },
  methods: {
    onShow() { console.log("VNC页面显示"); },
    // 连接VNC
    connectVNC() {
      if (this.connected) { so.vnc_disconnect(); this.connected = false; return; }
      const res = so.vnc_connect(this.conn.ip, this.conn.port, this.conn.pass);
      if (res === 0) {
        this.connected = true;
        this.startFrameStream();
      } else alert(`VNC连接失败: ${res}`);
    },
    // 启动帧流监听
    startFrameStream() {
      this.$page.setInterval(() => {
        const buf = new Array(4096).fill(0);
        const len = so.vnc_read_frame(buf, 4096);
        if (len > 0) this.$refs.canvas.updateFrame(buf.join(''));
      }, 30);
    },
    // 处理缩放
    handleScale(scale) { so.vnc_set_scale(scale); },
    // 处理键鼠事件
    handleMouseEvent(evt) { so.vnc_send_input(JSON.stringify(evt)); },
    // 处理键盘输入
    handleKeyPress(key) { so.vnc_send_key(key); }
  }
};
</script>
<style lang="less" scoped>
@import "../../styles/base.less";
.conn-config { display: flex; flex-direction: column; gap: @padding-sm; margin-bottom: @padding-md; }
</style>
