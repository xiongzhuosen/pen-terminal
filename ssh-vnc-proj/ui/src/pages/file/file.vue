<template>
  <div class="page-root">
    <div class="wrapper">
      <!-- 路径栏 -->
      <div class="path-bar">
        <input class="input" v-model="curPath" @keyup.enter="changePath" />
        <text class="btn-primary" @click="changePath">跳转</text>
      </div>
      <!-- 文件列表 -->
      <div class="file-list">
        <div class="file-item" v-for="(f, idx) in fileList" :key="idx" @click="selectFile(f)">
          <text>{{f.name}}</text>
          <text class="text-sm">{{f.size}} B</text>
        </div>
      </div>
      <!-- 操作按钮 -->
      <div class="file-ops">
        <text class="btn-primary" @click="viewFile">查看</text>
        <text class="btn-warning" @click="editFile">编辑</text>
        <text class="btn-danger" @click="chmodFile">修改权限</text>
        <text class="btn-danger" @click="chownFile">修改属主</text>
        <text class="btn-warning" @click="lsattrFile">查看属性</text>
      </div>
      <!-- 文件查看器 -->
      <FileViewer v-if="showViewer" :filePath="selectedFile" />
    </div>
  </div>
</template>
<script>
import FileViewer from '../../components/FileViewer.vue';
const so = window.require('./libs/libssh-vnc-full.so');
export default {
  name: "file",
  components: { FileViewer },
  data() { return { curPath: "/", fileList: [], selectedFile: "", showViewer: false }; },
  mounted() { this.loadFileList(); },
  methods: {
    onShow() { console.log("文件管理页面显示"); this.loadFileList(); },
    // 加载文件列表
    loadFileList() {
      const buf = new Array(8192).fill(0);
      // 关联SSH连接：优先so调用，失败则用SSH
      const res = so.file_list(this.curPath, buf, 8192);
      if (res !== 0) { so.file_list_via_ssh(this.curPath, buf, 8192); }
      this.fileList = JSON.parse(buf.join(''));
    },
    // 切换路径
    changePath() { this.loadFileList(); },
    // 选择文件
    selectFile(f) { this.selectedFile = `${this.curPath}/${f.name}`; },
    // 查看文件
    viewFile() { if (this.selectedFile) this.showViewer = true; },
    // 编辑文件
    editFile() {
      if (!this.selectedFile) return;
      const content = prompt("输入文件内容");
      if (content) so.file_write(this.selectedFile, content);
    },
    // 修改权限
    chmodFile() {
      if (!this.selectedFile) return;
      const mode = prompt("输入权限(如755)");
      if (mode) so.file_chmod(this.selectedFile, mode);
    },
    // 修改属主
    chownFile() {
      if (!this.selectedFile) return;
      const user = prompt("输入属主(如root:root)");
      if (user) so.file_chown(this.selectedFile, user);
    },
    // 查看属性
    lsattrFile() {
      if (!this.selectedFile) return;
      const buf = new Array(1024).fill(0);
      so.file_lsattr(this.selectedFile, buf, 1024);
      alert(buf.join(''));
    }
  }
};
</script>
<style lang="less" scoped>
@import "../../styles/base.less";
.path-bar { display: flex; gap: @padding-sm; margin-bottom: @padding-md; }
.file-list { width: 100%; height: 200px; border: 1px solid @color-border; overflow-y: auto; margin-bottom: @padding-md; }
.file-item { display: flex; justify-content: space-between; padding: @padding-sm; border-bottom: 1px solid @color-border; }
.file-ops { display: flex; flex-wrap: wrap; gap: @padding-sm; margin-bottom: @padding-md; }
</style>
