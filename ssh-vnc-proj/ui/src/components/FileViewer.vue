<template>
  <div class="file-viewer">
    <div class="tab-bar">
      <text class="tab" @click="switchTab('text')" :class="{active: tab === 'text'}">文本</text>
      <text class="tab" @click="switchTab('hex')" :class="{active: tab === 'hex'}">16进制</text>
      <text class="tab" @click="switchTab('md')" :class="{active: tab === 'md'}">MD渲染</text>
    </div>
    <div class="content">
      <div v-if="tab === 'text'" class="text-content">{{textContent}}</div>
      <div v-if="tab === 'hex'" class="hex-content">{{hexContent}}</div>
      <div v-if="tab === 'md'" class="md-content" v-html="mdContent"></div>
    </div>
  </div>
</template>
<script>
// 引入md4c-html渲染
const mdRender = window.require('./libs/libssh-vnc-full.so').md_render;
export default {
  name: "FileViewer",
  props: { filePath: { type: String, required: true } },
  data() { return { tab: "text", textContent: "", hexContent: "", mdContent: "" } },
  mounted() { this.loadFile(); },
  methods: {
    switchTab(tab) { this.tab = tab; },
    // 加载文件内容
    loadFile() {
      const so = window.require('./libs/libssh-vnc-full.so');
      // 文本内容
      const textBuf = new Array(8192).fill(0);
      so.file_read_text(this.filePath, textBuf, 8192);
      this.textContent = textBuf.join('');
      // 16进制内容
      const hexBuf = new Array(8192).fill(0);
      so.file_read_hex(this.filePath, hexBuf, 8192);
      this.hexContent = hexBuf.join('');
      // MD渲染
      const mdBuf = new Array(8192).fill(0);
      so.file_render_md(this.filePath, mdBuf, 8192);
      this.mdContent = mdBuf.join('');
    }
  }
};
</script>
<style lang="less" scoped>
@import "../styles/base.less";
.file-viewer { width: @screen-w - 16px; height: 300px; border: 1px solid @color-border; border-radius: @border-radius; }
.tab-bar { display: flex; border-bottom: 1px solid @color-border; }
.tab { flex: 1; height: 30px; line-height: 30px; text-align: center; font-size: @font-size-md; }
.active { color: @color-primary; border-bottom: 2px solid @color-primary; }
.content { width: 100%; height: 270px; padding: @padding-sm; overflow-y: auto; font-size: @font-size-sm; }
</style>
