<template>
  <div class="vnc-canvas" @longpress="zoom" @touchstart="mouseDown" @touchend="mouseUp" @touchmove="mouseMove">
    <canvas ref="canvas" width="260" height="400"></canvas>
    <div class="status" v-if="!connected">未连接VNC</div>
  </div>
</template>
<script>
export default {
  name: "VncCanvas",
  props: { connected: { type: Boolean, default: false } },
  data() { return { scale: 1.0, lastX: 0, lastY: 0 } },
  mounted() { this.initCanvas(); },
  methods: {
    initCanvas() {
      const ctx = this.$refs.canvas.getContext('2d');
      ctx.fillStyle = '#f5f5f5';
      ctx.fillRect(0,0,260,400);
    },
    // 长按缩放
    zoom() { this.scale = this.scale === 1.0 ? 1.5 : 1.0; this.$emit("scaleChange", this.scale); },
    // 鼠标按下
    mouseDown(e) {
      this.lastX = e.touches[0].clientX;
      this.lastY = e.touches[0].clientY;
      this.$emit("mouseEvent", { type: "down", x: this.lastX, y: this.lastY });
    },
    // 鼠标移动
    mouseMove(e) {
      const x = e.touches[0].clientX;
      const y = e.touches[0].clientY;
      this.$emit("mouseEvent", { type: "move", x, y, dx: x - this.lastX, dy: y - this.lastY });
      this.lastX = x; this.lastY = y;
    },
    // 鼠标抬起
    mouseUp() { this.$emit("mouseEvent", { type: "up" }); },
    // 更新画布帧
    updateFrame(frameData) {
      if(!this.connected) return;
      const ctx = this.$refs.canvas.getContext('2d');
      const img = new Image();
      img.onload = () => {
        ctx.clearRect(0,0,260,400);
        ctx.drawImage(img,0,0,260*this.scale,400*this.scale);
      };
      img.src = frameData;
    }
  }
};
</script>
<style lang="less" scoped>
@import "../styles/base.less";
.vnc-canvas { width: @screen-w - 16px; height: 400px; text-align: center; }
canvas { width: 100%; height: 100%; border: 1px solid @color-border; }
.status { margin-top: 20px; font-size: @font-size-md; }
</style>
