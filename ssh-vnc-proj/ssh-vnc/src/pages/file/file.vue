<template>
  <div class="wrapper">
    <input v-model="filePath" placeholder="输入文件路径 e.g. /root/test.md" class="ipt-path" />
    <text class="btn success" @click="viewFile">查看文件</text>
    <div class="file-content" v-if="content">{{content}}</div>
  </div>
</template>

<script>
const sshVncLib = window.require('./libs/libssh-vnc-full.so')

export default {
  name: 'file',
  data() {
    return {
      filePath: '/root/test.md',
      content: ''
    }
  },
  methods: {
    onShow() {
      console.log(`文件管理页面参数: ${JSON.stringify(this.$page.options)}`);
    },
    viewFile() {
      if(!this.filePath) return this.content = '❌ 文件路径不能为空'
      const result = new Array(8192).fill(0)
      const res = sshVncLib.file_view(this.filePath, result, 8192)
      this.content = res === 0 ? result.join('') : `❌ 读取失败，错误码: ${res}`
    }
  }
}
</script>

<style lang="less" scoped>
@import "base.less";

.ipt-path {
  width: 100%;
  padding: 8px;
  margin-bottom: 10px;
  #border();
  outline: none;
}

.file-content {
  width: 100%;
  height: calc(100% - 80px);
  margin-top: 10px;
  padding: 10px;
  #border();
  overflow-y: auto;
  white-space: pre-wrap;
}
</style>
