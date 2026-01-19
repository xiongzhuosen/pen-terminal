<template>
  <div class="vnc-canvas">
    <canvas ref="canvas" width="1024" height="768"></canvas>
    <div class="status" v-if="!connected">未连接VNC服务器</div>
  </div>
</template>

<script>
export default {
  name: "VncCanvas",
  props: {
    connected: {
      type: Boolean,
      default: false
    }
  },
  mounted() {
    this.initCanvas();
  },
  methods: {
    initCanvas() {
      const ctx = this.$refs.canvas.getContext('2d');
      ctx.fillStyle = @bg-color-default;
      ctx.fillRect(0, 0, 1024, 768);
    },
    updateCanvas(frameData) {
      if(!this.connected) return;
      const ctx = this.$refs.canvas.getContext('2d');
      const img = new Image();
      img.onload = () => ctx.drawImage(img, 0, 0);
      img.src = frameData;
    }
  }
};
</script>

<style lang="less" scoped>
@import "base.less";

.vnc-canvas {
  width: 100%;
  height: 100%;
  text-align: center;
  canvas {
    width: 100%;
    height: 100%;
    #border();
  }
  .status {
    margin-top: 20px;
    color: @text-color-default;
  }
}
</style>
