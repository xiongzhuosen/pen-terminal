<template>
  <div class="file-manager-box">
    <div class="flex-row justify-between align-center padding-sm">
      <text class="file-name">{{ currentPath }}</text>
      <div class="flex-row">
        <text class="input-box" style="width: auto;" @click="goUp">上一级</text>
        <text class="input-box" style="width: auto;" @click="refresh">刷新</text>
      </div>
    </div>
    <scroller class="scroller-box">
      <div 
        class="file-item" 
        v-for="(file, index) in fileList" 
        :key="index" 
        @click="openFile(file)"
        @longpress="showFileOpt(file)"
      >
        <text class="file-name">{{ file.name }}</text>
        <text class="file-info">{{ file.size }} | {{ file.mode }}</text>
      </div>
    </scroller>
    <div class="file-opt-bar">
      <text class="opt-btn" @click="showChmod">chmod</text>
      <text class="opt-btn" @click="showChown">chown</text>
      <text class="opt-btn" @click="showLsattr">lsattr</text>
      <text class="opt-btn" @click="goToSsh">快捷SSH</text>
    </div>
  </div>
</template>

<script>
export default {
  name: 'FileManager',
  props: {
    initPath: { type: String, default: '/' },
    sshConfig: { type: Object, default: () => ({}) }
  },
  data() {
    return {
      currentPath: this.initPath,
      fileList: [],
      selectedFile: null
    }
  },
  mounted() {
    this.getFileList()
  },
  methods: {
    getFileList() {
      const fileJson = $api.file_list(this.currentPath)
      this.fileList = JSON.parse(fileJson || '[]')
    },
    goUp() {
      if (this.currentPath === '/') return
      this.currentPath = this.currentPath.substring(0, this.currentPath.lastIndexOf('/')) || '/'
      this.getFileList()
    },
    refresh() {
      this.getFileList()
    },
    openFile(file) {
      if (file.type === 'dir') {
        this.currentPath = file.path
        this.getFileList()
      } else {
        this.$emit('open-file', file)
      }
    },
    showFileOpt(file) {
      this.selectedFile = file
      this.$emit('file-opt', { type: 'menu', file: this.selectedFile })
    },
    showChmod() {
      this.$emit('file-opt', { type: 'chmod', path: this.currentPath })
    },
    showChown() {
      this.$emit('file-opt', { type: 'chown', path: this.currentPath })
    },
    showLsattr() {
      const attr = $api.file_lsattr(this.currentPath)
      this.$emit('show-info', attr)
    },
    goToSsh() {
      this.$emit('go-to-ssh', this.sshConfig)
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

.justify-between {
  justify-content: space-between;
}

.align-center {
  align-items: center;
}

.padding-sm {
  padding: @spacing-sm;
}
</style>
