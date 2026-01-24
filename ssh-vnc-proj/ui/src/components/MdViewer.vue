<template>
  <view class="md-viewer">
    <text
      v-for="(line, idx) in mdLines"
      :key="idx"
      :class="getLineClass(line)"
      style="white-space: pre-wrap; word-wrap: break-word; width: 100%;"
    >
      {{ renderLine(line) }}
    </text>
  </view>
</template>
<script>
export default {
  name: "MdViewer",
  props: {
    mdContent: { type: String, default: "" }
  },
  computed: {
    mdLines() {
      return this.mdContent.split("\n").filter(line => line.trim() !== "");
    }
  },
  methods: {
    getLineClass(line) {
      if (line.startsWith("# ")) return "md-h1";
      if (line.startsWith("## ")) return "md-h2";
      if (line.startsWith("### ")) return "md-h3";
      if (line.startsWith("- ")) return "md-list";
      if (line.startsWith("```")) return "md-code";
      if (line.startsWith("![")) return "md-img";
      if (line.startsWith("[")) return "md-link";
      return "md-text";
    },
    renderLine(line) {
      return line
        .replace(/^# /, "")
        .replace(/^## /, "")
        .replace(/^### /, "")
        .replace(/^- /, "• ")
        .replace(/```/g, "")
        .replace(/!\[(.*?)\]\(.*?\)/g, "🖼️ $1")
        .replace(/\[(.*?)\]\(.*?\)/g, "🔗 $1");
    }
  }
};
</script>
<style lang="less" scoped>
@import "../styles/base.less";
</style>
