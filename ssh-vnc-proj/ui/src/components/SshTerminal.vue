<template>
  <div class="terminal-box">
    <scroller class="scroller-box" ref="scroller">
      <div class="terminal-text" v-text="content"></div>
    </scroller>
    <div class="flex-row align-center padding-xs">
      <text class="terminal-prefix" v-text="prefix"></text>
      <input 
        class="terminal-input" 
        v-model="input" 
        @confirm="sendCmd" 
        @input="handleInput"
        placeholder="输入命令..."
      />
    </div>
  </div>
</template>

<script>
export default {
  name: 'SshTerminal',
  props: {
    prefix: { type: String, default: 'root@device:~$ ' }
  },
  data() {
    return {
      content: '=== SSH 终端 ===\n支持 vim/passwd 交互式命令\n',
      input: ''
    }
  },
  methods: {
    sendCmd() {
      const cmd = this.input.trim()
      if (!cmd) return
      this.content += `${this.prefix}${cmd}\n`
      this.$emit('send-cmd', cmd)
      this.input = ''
      this.scrollToBottom()
    },
    handleInput(val) {
      this.input = val
      this.$emit('input-change', val)
    },
    appendOutput(output) {
      this.content += output
      this.scrollToBottom()
    },
    scrollToBottom() {
      this.$page.setTimeout(() => {
        if (this.$refs.scroller) {
          this.$refs.scroller.scrollTop = this.$refs.scroller.scrollHeight
        }
      }, 50)
    },
    inputChar(char) {
      if (char === 'backspace') {
        this.input = this.input.slice(0, -1)
      } else if (char === 'enter') {
        this.sendCmd()
      } else if (char === 'esc') {
        this.$emit('send-esc')
      } else {
        this.input += char
      }
      this.handleInput(this.input)
    }
  }
}
</script>

<style lang="less" scoped>
@import "../styles/base.less";

.flex-row {
  display: flex;
  flex-direction: row;
}

.align-center {
  align-items: center;
}

.padding-xs {
  padding: @spacing-xs;
}
</style>
