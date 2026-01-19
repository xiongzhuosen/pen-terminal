<template>
  <div class="file-list">
    <div class="file-item" v-for="(file, idx) in fileList" :key="idx" @click="selectFile(file.path)">
      <text>{{file.name}}</text>
      <text class="file-size">{{file.size}} B</text>
    </div>
  </div>
</template>

<script>
export default {
  name: "FileList",
  props: {
    fileList: {
      type: Array,
      default: () => []
    }
  },
  methods: {
    selectFile(path) {
      this.$emit("fileSelect", path);
    }
  }
};
</script>

<style lang="less" scoped>
@import "base.less";

.file-list {
  width: 100%;
  height: 100%;
  overflow-y: auto;
  .file-item {
    display: flex;
    justify-content: space-between;
    padding: 10px;
    border-bottom: @border-width-default @border-style-default @border-color-default;
    &:active {
      background: @bg-color-default;
    }
    .file-size {
      color: #999;
      font-size: 14px;
    }
  }
}
</style>
