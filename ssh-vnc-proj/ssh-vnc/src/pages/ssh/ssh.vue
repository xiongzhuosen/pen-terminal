<template>
  <div class="wrapper">
    <div class="config-bar">
      <input v-model="sshIp" placeholder="SSH服务器IP" class="ipt-ip" />
      <text class="btn success" @click="connectSSH">{{connected?'已连接':'连接SSH'}}</text>
    </div>
    <Terminal ref="terminal" @sendCmd="sendSSHCommand" class="terminal" />
  </div>
</template>

<script>
import Terminal from '@/components/Terminal.vue'
// 对接后端库
const sshVncLib = window.require('./libs/libssh-vnc-full.so')

export default {
  name: 'ssh',
  components: { Terminal },
  data() {
    return {
      sshIp: '192.168.1.100',
      connected: false
    }
  },
  methods: {
    onShow() {
      console.log(`SSH页面参数: ${JSON.stringify(this.$page.options)}`);
    },
    connectSSH() {
      const res = sshVncLib.ssh_init(this.sshIp)
      if(res === 0) {
        this.connected = true
        this.$refs.terminal.showMsg('✅ SSH连接成功')
      } else {
        this.$refs.terminal.showMsg(`❌ 连接失败，错误码: ${res}`)
      }
    },
    sendSSHCommand(cmd) {
      if(!this.connected) return this.$refs.terminal.showMsg('❌ 请先连接SSH')
      const result = new Array(4096).fill(0)
      sshVncLib.ssh_send_cmd(cmd, result, 4096)
      this.$refs.terminal.appendOutput(result.join(''))
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

.terminal {
  height: calc(100% - 50px);
}
</style>
