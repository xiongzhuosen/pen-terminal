<template>
  <div class="wrapper">
    <div class="config-bar">
      <input v-model="vncIp" placeholder="VNC服务器IP" class="ipt-ip" />
      <text class="btn success" @click="connectVNC">{{connected?'已连接':'连接VNC'}}</text>
    </div>
    <VncCanvas :connected="connected" ref="vncCanvas" style="height: calc(100% - 50px);" />
  </div>
</template>

<script>
import VncCanvas from '@/components/VncCanvas.vue'
const sshVncLib = window.require('./libs/libssh-vnc-full.so')

export default {
  name: 'vnc',
  components: { VncCanvas },
  data() {
    return {
      vncIp: '192.168.1.100',
      connected: false,
      renderTimer: null
    }
  },
  methods: {
    onShow() {
      console.log(`VNC页面参数: ${JSON.stringify(this.$page.options)}`);
    },
    connectVNC() {
      const res = sshVncLib.vnc_init(this.vncIp)
      if(res === 0) {
        this.connected = true
        this.startRender()
      } else {
        alert(`VNC连接失败: ${res}`)
      }
    },
    startRender() {
      this.renderTimer = this.$page.setInterval(() => {
        const frame = sshVncLib.vnc_get_frame()
        this.$refs.vncCanvas.updateCanvas(frame)
      }, 30);
    },
    onUnload() {
      this.$page.clearInterval(this.renderTimer);
    }
  }
}
</script>

<style lang="less" scoped>
@import "base.less";

.config-bar {
  width: 100%;
  display: flex;
  gap: 10px;
  margin-bottom: 10px;
  .ipt-ip {
    flex: 1;
    padding: 8px;
    #border();
    outline: none;
  }
}
</style>
