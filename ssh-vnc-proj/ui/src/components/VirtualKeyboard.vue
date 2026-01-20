<template>
  <div class="keyboard-box">
    <div class="flex-row justify-center">
      <div class="key-normal" @click="pressKey('q')">q</div>
      <div class="key-normal" @click="pressKey('w')">w</div>
      <div class="key-normal" @click="pressKey('e')">e</div>
      <div class="key-normal" @click="pressKey('r')">r</div>
      <div class="key-normal" @click="pressKey('t')">t</div>
      <div class="key-normal" @click="pressKey('y')">y</div>
      <div class="key-normal" @click="pressKey('u')">u</div>
      <div class="key-normal" @click="pressKey('i')">i</div>
      <div class="key-normal" @click="pressKey('o')">o</div>
      <div class="key-normal" @click="pressKey('p')">p</div>
    </div>
    <div class="flex-row justify-center">
      <div class="key-normal" @click="pressKey('a')">a</div>
      <div class="key-normal" @click="pressKey('s')">s</div>
      <div class="key-normal" @click="pressKey('d')">d</div>
      <div class="key-normal" @click="pressKey('f')">f</div>
      <div class="key-normal" @click="pressKey('g')">g</div>
      <div class="key-normal" @click="pressKey('h')">h</div>
      <div class="key-normal" @click="pressKey('j')">j</div>
      <div class="key-normal" @click="pressKey('k')">k</div>
      <div class="key-normal" @click="pressKey('l')">l</div>
    </div>
    <div class="flex-row justify-center">
      <div class="key-normal" @click="pressKey('z')">z</div>
      <div class="key-normal" @click="pressKey('x')">x</div>
      <div class="key-normal" @click="pressKey('c')">c</div>
      <div class="key-normal" @click="pressKey('v')">v</div>
      <div class="key-normal" @click="pressKey('b')">b</div>
      <div class="key-normal" @click="pressKey('n')">n</div>
      <div class="key-normal" @click="pressKey('m')">m</div>
      <div class="key-wide" @click="pressKey(' ')">空格</div>
    </div>
    <div class="flex-row justify-center">
      <div class="key-func" @click="pressKey('esc')">ESC</div>
      <div class="key-func" @click="pressKey('backspace')">删除</div>
      <div class="key-func" @click="pressKey('enter')">回车</div>
      <div class="key-func" @click="toggleKeyboard">收起</div>
    </div>
  </div>
</template>

<script>
export default {
  name: 'VirtualKeyboard',
  data() {
    return {
      show: true
    }
  },
  methods: {
    pressKey(key) {
      this.$emit('key-press', key)
    },
    toggleKeyboard() {
      this.show = !this.show
      this.$emit('toggle', this.show)
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

.justify-center {
  justify-content: center;
}
</style>
