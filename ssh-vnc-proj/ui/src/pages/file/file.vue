<template>
  <div class="page-root">
    <FileManager 
      :initPath="currentPath" 
      :sshConfig="sshConfig"
      @open-file="openFile"
      @file-opt="handleFileOpt"
      @go-to-ssh="goToSsh"
      @show-info="showInfo"
    />
    <div class="viewer-modal" v-if="showViewer">
      <div class="viewer-header">
        <text class="viewer-title">{{ currentFile.name }}</text>
        <text class="close-btn" @click="closeViewer">关闭</text>
      </div>
      <scroller class="scroller-box">
        <text v-if="viewType === 'text'" v-text="fileContent"></text>
        <text v-else-if="viewType === 'hex'" v-text="hexContent"></text>
        <div v-else-if="viewType === 'md'" v-html="mdContent"></div>
      </scroller>
      <input v-if="viewType === 'text'" v-model="fileContent" class="input-box" style="height: 100px;" />
      <text class="save-btn" v-if="viewType === 'text'" @click="saveFile">保存</text>
    </div>
  </div>
</template>

<script>
import FileManager from '../../components/FileManager.vue'
import * as marked from 'marked'

export default {
  name: 'FilePage',
  components: { FileManager },
  data() {
    return {
      currentPath: '/',
      sshConfig: $falcon.storage.get('sshConfig') || {},
      showViewer: false,
      currentFile: null,
      fileContent: '',
      hexContent: '',
      mdContent: '',
      viewType: ''
    }
  },
  methods: {
    openFile(file) {
      this.currentFile = file
      this.showViewer = true
      const ext = file.name.split('.').pop() || ''
      this.viewType = ext === 'md' ? 'md' : ext === 'hex' ? 'hex' : 'text'
      const content = $api.file_read(file.path)
      this.fileContent = content
      if (this.viewType === 'hex') {
        this.hexContent = content.split('').map(c => c.charCodeAt(0).toString(16).padStart(2, '0')).join(' ')
      } else if (this.viewType === 'md') {
        this.mdContent = marked.parse(content)
      }
    },
    closeViewer() {
      this.showViewer = false
      this.currentFile = null
    },
    saveFile() {
      $api.file_write(this.currentFile.path, this.fileContent)
      this.closeViewer()
      $falcon.modal.toast('保存成功')
    },
    handleFileOpt(opt) {
      if (opt.type === 'chmod') {
        $falcon.modal.prompt('输入权限（如777）', (val) => {
          $api.file_chmod(opt.path, val)
          $falcon.modal.toast('操作成功')
        })
      } else if (opt.type === 'chown') {
        $falcon.modal.prompt('输入用户:组（如root:root）', (val) => {
          $api.file_chown(opt.path, val)
          $falcon.modal.toast('操作成功')
        })
      }
    },
    goToSsh(config) {
      $falcon.navTo('ssh', config)
    },
    showInfo(info) {
      $falcon.modal.alert(info)
    }
  }
}
</script>

<style lang="less" scoped>
@import "../../styles/base.less";
</style>
