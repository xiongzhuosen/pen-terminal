<template>
  <div 
    class="vnc-box" 
    @longpress="toggleScale" 
    @touchstart="handleMouseDown" 
    @touchmove="handleMouseMove" 
    @touchend="handleMouseUp"
  >
    <div class="vnc-wrap" :style="{ transform: `scale(${scale})`, transformOrigin: '0 0' }">
      <image class="vnc-img" :src="frameData" mode="widthFix" />
    </div>
  </div>
</template>

<script>
export default {
  name: 'VncCanvas',
  props: {
    frameData: { type: String, default: '' },
    initScale: { type: Number, default: 1.0 }
  },
  data() {
    return {
      scale: this.initScale,
      scaleList: [1.0, 1.2, 1.5, 2.0],
      scaleIndex: 0,
      lastX: 0,
      lastY: 0
    }
  },
  methods: {
    toggleScale() {
      this.scaleIndex = (this.scaleIndex + 1) % this.scaleList.length
      this.scale = this.scaleList[this.scaleIndex]
      this.$emit('scale-change', this.scale)
    },
    handleMouseDown(e) {
      this.lastX = e.touches[0].clientX / this.scale
      this.lastY = e.touches[0].clientY / this.scale
      this.$emit('mouse-event', { type: 'down', x: this.lastX, y: this.lastY })
    },
    handleMouseMove(e) {
      const x = e.touches[0].clientX / this.scale
      const y = e.touches[0].clientY / this.scale
      this.$emit('mouse-event', { type: 'move', x, y })
      this.lastX = x
      this.lastY = y
    },
    handleMouseUp() {
      this.$emit('mouse-event', { type: 'up', x: this.lastX, y: this.lastY })
    }
  }
}
</script>

<style lang="less" scoped>
@import "../styles/base.less";
</style>
