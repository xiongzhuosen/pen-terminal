<template>
  <div class="page-root">
    <div class="page-header">
      <text class="header-title">SSH 终端</text>
      <text class="header-btn" @click="showConfig">配置</text>
    </div>
    <QuickCmd @exec-cmd="execQuickCmd" />
    <SshTerminal ref="terminal" @send-cmd="sendCmd" @send-esc="sendEsc" />
    <VirtualKeyboard @key-press="handleKey" @toggle="showKeyboard = $event" />
  </div>
</template>

<script>
import SshTerminal from '../../components/SshTerminal.vue'
import QuickCmd from '../../components/QuickCmd.vue'
import VirtualKeyboard from '../../components/VirtualKeyboard.vue'

export default {
  name: 'SshPage',
  components: { SshTerminal, QuickCmd, VirtualKeyboard },
  data() {
    return {
      showKeyboard: true,
      sshConfig: {
        ip: '',
        port: '22',
        user: 'root',
        pass: '',
        keyPath: '',
        useKey: false
      },
      connected: false
    }
  },
  onShow() {
    const config = $falcon.storage.get('sshConfig')
    if (config) this.sshConfig = config
  },
  methods: {
    showConfig() {
      $falcon.modal.show({
        title: 'SSH 配置',
        content: `
          <input class="input-box" v-model="sshConfig.ip" placeholder="IP" />
          <input class="input-box" v-model="sshConfig.port" placeholder="端口" />
          <input class="input-box" v-model="sshConfig.user" placeholder="用户" />
          <input class="input-box" v-model="sshConfig.pass" placeholder="密码" type="password" />
          <input class="input-box" v-model="sshConfig.keyPath" placeholder="密钥路径" />
          <checkbox v-model="sshConfig.useKey">使用密钥</checkbox>
        `,
        onConfirm: () => {
          $falcon.storage.set('sshConfig', this.sshConfig)
          this.connectSsh()
        }
      })
    },
    connectSsh() {
      let code = 0
      if (this.sshConfig.useKey) {
        code = $api.ssh_connect_key(this.sshConfig.ip, this.sshConfig.port, this.sshConfig.user, this.sshConfig.keyPath)
      } else {
        code = $api.ssh_connect(this.sshConfig.ip, this.sshConfig.port, this.sshConfig.user, this.sshConfig.pass)
      }
      this.connected = code === 0
      this.$refs.terminal.appendOutput(code === 0 ? '连接成功\n' : '连接失败\n')
    },
    sendCmd(cmd) {
      if (!this.connected) return
      $api.ssh_exec(cmd)
      this.$page.setInterval(() => {
        const output = $api.ssh_read()
        if (output) this.$refs.terminal.appendOutput(output)
      }, 50)
    },
    execQuickCmd(cmd) {
      this.sendCmd(cmd)
    },
    sendEsc() {
      $api.ssh_send_esc()
    },
    handleKey(key) {
      this.$refs.terminal.inputChar(key)
    }
  }
}
</script>

<style lang="less" scoped>
@import "../../styles/base.less";
</style>
