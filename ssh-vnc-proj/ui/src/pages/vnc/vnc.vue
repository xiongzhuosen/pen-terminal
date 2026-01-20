<template>
  <div class="page-root">
    <div class="page-header">
      <text class="header-title">VNC 远程</text>
      <text class="header-btn" @click="showConfig">配置</text>
    </div>
    <VncCanvas 
      :frameData="frameData" 
      @mouse-event="sendMouseEvent" 
      @scale-change="handleScale"
    />
    <VirtualKeyboard @key-press="sendKey" @toggle="showKeyboard = $event" />
  </div>
</template>

<script>
import VncCanvas from '../../components/VncCanvas.vue'
import VirtualKeyboard from '../../components/VirtualKeyboard.vue'

export default {
  name: 'VncPage',
  components: { VncCanvas, VirtualKeyboard },
  data() {
    return {
      showKeyboard: true,
      vncConfig: { ip: '', port: '5900', pass: '' },
      frameData: '',
      connected: false,
      frameTimer: null
    }
  },
  onShow() {
    const config = $falcon.storage.get('vncConfig')
    if (config) this.vncConfig = config
  },
  methods: {
    showConfig() {
      $falcon.modal.show({
        title: 'VNC 配置',
        content: `
          <input class="input-box" v-model="vncConfig.ip" placeholder="IP" />
          <input class="input-box" v-model="vncConfig.port" placeholder="端口" />
          <input class="input-box" v-model="vncConfig.pass" placeholder="密码" type="password" />
        `,
        onConfirm: () => {
          $falcon.storage.set('vncConfig', this.vncConfig)
          this.connectVnc()
        }
      })
    },
    connectVnc() {
      const code = $api.vnc_connect(this.vncConfig.ip, this.vncConfig.port, this.vncConfig.pass)
      this.connected = code === 0
      if (this.connected) {
        this.frameTimer = this.$page.setInterval(() => {
          this.frameData = $api.vnc_read_frame()
        }, 30)
      }
    },
    sendMouseEvent(evt) {
      if (!this.connected) return
      $api.vnc_send_mouse(JSON.stringify(evt))
    },
    sendKey(key) {
      if (!this.connected) return
      $api.vnc_send_key(key)
    },
    handleScale(scale) {
      $api.vnc_set_scale(scale)
    }
  },
  onUnload() {
    if (this.frameTimer) this.$page.clearInterval(this.frameTimer)
    $api.vnc_disconnect()
  }
}
</script>

<style lang="less" scoped>
@import "../../styles/base.less";
</style>
